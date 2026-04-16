enum EStalkerAIAction
{
	PATROL,
	REST,
	SCAVENGE
};

class SCR_StalkerAIBrainComponentClass : ScriptComponentClass
{
}

class SCR_StalkerAIBrainComponent : ScriptComponent
{
	[Attribute("{E5B990DB0AF3D288}Prefabs/AI/Waypoints/AIWaypoint_Defend.et", UIWidgets.ResourceNamePicker, desc: "Waypoint for resting/defending")]
	protected ResourceName m_sRestWaypointPrefab;
	
	[Attribute("{93291E72AC23930F}Prefabs/AI/Waypoints/AIWaypoint_Patrol.et", UIWidgets.ResourceNamePicker, desc: "Waypoint for patrolling/wandering")]
	protected ResourceName m_sPatrolWaypointPrefab;
	
	[Attribute("{22A875E30470AD4F}Prefabs/AI/Waypoints/AIWaypoint_SearchAndDestroy.et", UIWidgets.ResourceNamePicker, desc: "Waypoint for scavenging/hunting")]
	protected ResourceName m_sScavengeWaypointPrefab;

	[Attribute("10", desc: "How fast greed/boredom builds up natively")]
	protected float m_fGreedDecayRate;

	protected SCR_AIGroup m_Group;
	
	// Needs range continuously 0 to 100. Lower means more satisfied, higher means CRITICAL NEED.
	protected float m_fNeedRest = 0;
	protected float m_fNeedGreed = 0; 
	
	protected EStalkerAIAction m_CurrentAction = EStalkerAIAction.PATROL;
	protected float FAST_LOGISTIC_LUT[101];

	[Attribute("1", UIWidgets.ComboBox, "Archetype Enum", enums: ParamEnumArray.FromEnum(EStalkerArchetype))]
	protected EStalkerArchetype m_Archetype;
	
	protected ref SCR_StalkerArchetypeConfig m_ArchetypeConfig;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Group = SCR_AIGroup.Cast(owner);
		
		// Load the Archetype Config based on assigned enum
		SCR_StalkerArchetypeLibrary lib = SCR_StalkerArchetypeLibrary.GetInstance();
		m_ArchetypeConfig = lib.GetConfig(m_Archetype);

		if (!m_Group)
		{
			// If not directly on the group, see if we are attached to a spawner that tracks it
			return; 
		}

		// Initialize Fast Logistic LUT (S-Curve) for O(1) utility calculations
		for (int i = 0; i <= 100; i++)
		{
			float x = i * 0.1;
			FAST_LOGISTIC_LUT[i] = 1.0 / (1.0 + Math.Exp(-1.0 * (x - 5.0)));
		}

		// Stagger start to avoid mass server tick spikes
		float startDelay = Math.RandomFloat(5000, 15000);
		GetGame().GetCallqueue().CallLater(Think, startDelay, true);
	}
	
	// Called by Spawners (like Bandit Camp) to directly set the controlled group if the component is on the spawner
	void SetGroup(SCR_AIGroup group)
	{
		m_Group = group;
		
		if (FAST_LOGISTIC_LUT[50] == 0) // if not init
		{
			for (int i = 0; i <= 100; i++)
			{
				float x = i * 0.1;
				FAST_LOGISTIC_LUT[i] = 1.0 / (1.0 + Math.Exp(-1.0 * (x - 5.0)));
			}
		}
		
		if (!m_ArchetypeConfig)
		{
			SCR_StalkerArchetypeLibrary lib = SCR_StalkerArchetypeLibrary.GetInstance();
			m_ArchetypeConfig = lib.GetConfig(m_Archetype);
		}

		GetGame().GetCallqueue().CallLater(Think, Math.RandomFloat(1000, 5000), true);
	}

	protected float LogisticCurveFast(float urgencyValue)
	{
		int index = Math.Floor(urgencyValue * 10);
		if (index < 0) index = 0;
		if (index > 100) index = 100;
		return FAST_LOGISTIC_LUT[index];
	}

	protected void Think()
	{
		if (!m_Group) return;

		// 1. Degrade / Accumulate needs over time factoring in Archetype Multipliers
		m_fNeedRest += (2.0 * m_ArchetypeConfig.m_fRestMultiplier);
		m_fNeedGreed += (1.5 * m_ArchetypeConfig.m_fGreedMultiplier);

		// Cap max needs
		if (m_fNeedRest > 100) m_fNeedRest = 100;
		if (m_fNeedGreed > 100) m_fNeedGreed = 100;

		// 2. Score actions (EXRP-RAI Style)
		float patrolScore = 2.0 + m_ArchetypeConfig.m_fPatrolBias; // Base baseline + bias
		float restScore = LogisticCurveFast(m_fNeedRest / 10.0) * 10.0; // Scales up aggressively if need is high
		float scavengeScore = (LogisticCurveFast(m_fNeedGreed / 10.0) * 8.0) + m_ArchetypeConfig.m_fScavengeBias;

		// Archetype/Trait Bias emulation: Add random noise so they don't all act identical
		patrolScore += Math.RandomFloat(0, 1.0);
		restScore += Math.RandomFloat(0, 1.0);
		scavengeScore += Math.RandomFloat(0, 1.0);

		// Repetition Penalty (don't keep spamming the exact same waypoint class if we can help it)
		if (m_CurrentAction == EStalkerAIAction.PATROL) patrolScore -= 1.0;
		if (m_CurrentAction == EStalkerAIAction.REST) restScore -= 1.0;
		if (m_CurrentAction == EStalkerAIAction.SCAVENGE) scavengeScore -= 1.0;

		// 3. Selection
		EStalkerAIAction bestAction = EStalkerAIAction.PATROL;
		float bestScore = patrolScore;

		if (restScore > bestScore)
		{
			bestScore = restScore;
			bestAction = EStalkerAIAction.REST;
		}
		if (scavengeScore > bestScore)
		{
			bestScore = scavengeScore;
			bestAction = EStalkerAIAction.SCAVENGE;
		}

		// 4. Execution (If we changed states)
		if (bestAction != m_CurrentAction)
		{
			ExecuteAction(bestAction);
		}
		else
		{
			// If we are currently resting, reduce the need! 
			// (If we were patrolling, need just goes up)
			if (m_CurrentAction == EStalkerAIAction.REST)
			{
				m_fNeedRest -= 20.0;
				if (m_fNeedRest < 0) m_fNeedRest = 0;
			}
			else if (m_CurrentAction == EStalkerAIAction.SCAVENGE)
			{
				m_fNeedGreed -= 15.0;
				if (m_fNeedGreed < 0) m_fNeedGreed = 0;
			}
		}
	}

	protected void ExecuteAction(EStalkerAIAction newAction)
	{
		m_CurrentAction = newAction;
		
		// Flush current native waypoints
		array<AIWaypoint> wps = new array<AIWaypoint>();
		m_Group.GetWaypoints(wps);
		foreach (AIWaypoint wp : wps)
		{
			m_Group.RemoveWaypoint(wp);
		}

		ResourceName wpPrefab = m_sPatrolWaypointPrefab;
		float radius = 50.0;

		if (newAction == EStalkerAIAction.REST)
		{
			wpPrefab = m_sRestWaypointPrefab;
			radius = 15.0; // Tight radius for resting at camp
			Print("Stalker Brain: Rest utility won. Assigning Defend WP.");
		}
		else if (newAction == EStalkerAIAction.SCAVENGE)
		{
			wpPrefab = m_sScavengeWaypointPrefab;
			radius = 150.0; // Wide radius looking for targets
			Print("Stalker Brain: Scavenge utility won. Assigning Search WP.");
		}
		else
		{
			Print("Stalker Brain: Patrol utility won. Assigning Patrol WP.");
		}

		// Physically spawn and inject the new waypoint into the Enfusion world
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = m_Group.GetOrigin();

		IEntity wpEnt = GetGame().SpawnEntityPrefab(Resource.Load(wpPrefab), GetGame().GetWorld(), params);
		AIWaypoint wp = AIWaypoint.Cast(wpEnt);

		if (wp)
		{
			wp.SetCompletionRadius(radius);
			m_Group.AddWaypoint(wp);
		}
	}
}
