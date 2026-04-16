enum ESquadCombatRole
{
	UNASSIGNED,
	BASE_OF_FIRE, // Suppresses the enemy
	MANEUVER      // Flanks the enemy
};

class SCR_AdvancedCombatAIComponentClass : ScriptComponentClass
{
}

class SCR_AdvancedCombatAIComponent : ScriptComponent
{
	[Attribute("100", desc: "Maximum Psychological Suppression limit. At max, AI cowers and cannot return fire.")]
	protected float m_fMaxSuppression;

	[Attribute("15", desc: "How much Suppression a single near-miss bullet adds")]
	protected float m_fSuppressionPerBullet;

	[Attribute("5", desc: "How much Suppression decays naturally per second")]
	protected float m_fSuppressionDecayRate;
	
	[Attribute("1.2", desc: "OODA Reaction Delay. Base time in seconds before AI processes a threat and returns fire.")]
	protected float m_fBaseReactionTime;

	[Attribute("{E5B990DB0AF3D288}Prefabs/AI/Waypoints/AIWaypoint_Defend.et", UIWidgets.ResourceNamePicker, desc: "Waypoint for Base of Fire / Suppressive Cover")]
	protected ResourceName m_sDefendWaypoint;

	[Attribute("{A80A64757C8AECB8}Prefabs/AI/Waypoints/AIWaypoint_Move.et", UIWidgets.ResourceNamePicker, desc: "Waypoint for Flanking/Maneuvering")]
	protected ResourceName m_sMoveWaypoint;

	protected SCR_AIGroup m_Group;
	protected float m_fCurrentSuppression = 0;
	protected ESquadCombatRole m_CurrentRole = ESquadCombatRole.UNASSIGNED;
	
	// OODA Loop tracking
	protected IEntity m_KnownThreat = null;
	protected float m_fTimeThreatFirstSpotted = 0;
	protected bool m_bHasProcessedOODA = false;
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Group = SCR_AIGroup.Cast(owner);
		
		if (!m_Group) return; // Waits for SetGroup if attached to spawner

		StartTactics();
	}
	
	void SetGroup(SCR_AIGroup group)
	{
		m_Group = group;
		StartTactics();
	}
	
	protected void StartTactics()
	{
		// Tactical evaluation tick logic (3 times a second during combat)
		GetGame().GetCallqueue().CallLater(TacticalThink, 333, true);
		
		// Decay suppression slowly
		GetGame().GetCallqueue().CallLater(DecaySuppression, 1000, true);
	}

	// Simulated Hook: Should be called by Arma's projectile system when a bullet flies near this entity
	void OnBulletSnap(vector trajectory)
	{
		m_fCurrentSuppression += m_fSuppressionPerBullet;
		if (m_fCurrentSuppression > m_fMaxSuppression)
			m_fCurrentSuppression = m_fMaxSuppression;
	}

	// Hook: To be called when the vanilla AI perception system spots an enemy
	void OnThreatSpotted(IEntity threat)
	{
		if (m_KnownThreat != threat)
		{
			m_KnownThreat = threat;
			m_fTimeThreatFirstSpotted = GetGame().GetWorld().GetWorldTime() / 1000.0; // Current time in seconds
			m_bHasProcessedOODA = false;
			
			Print("Advanced AI: Threat Spotted. Beginning Observe -> Orient -> Decide OODA Loop buffer.");
		}
	}

	protected void DecaySuppression()
	{
		if (m_fCurrentSuppression > 0)
		{
			m_fCurrentSuppression -= m_fSuppressionDecayRate;
			if (m_fCurrentSuppression < 0) m_fCurrentSuppression = 0;
		}
	}

	protected void TacticalThink()
	{
		if (!m_Group || !m_KnownThreat) return;

		// 1. Evaluate Psychological Suppression State
		if (m_fCurrentSuppression >= m_fMaxSuppression * 0.8)
		{
			// Heavily Suppressed: The AI's brain locks up prioritizing survival over offense.
			// Native Enfusion implementation would lower stance to PRONE and severly penalize aim error.
			Print("Advanced AI: " + m_Group.ToString() + " is SUPPRESSED! Cowering in cover.");
			return; // Skip combat tactics, they are pinned!
		}

		// 2. OODA Loop Processing Delay
		if (!m_bHasProcessedOODA)
		{
			float currentTime = GetGame().GetWorld().GetWorldTime() / 1000.0;
			float elapsedTime = currentTime - m_fTimeThreatFirstSpotted;
			
			// If we are under fire ourselves, OODA delay INCREASES (Stress tunnel vision)
			float effectiveReactionDelay = m_fBaseReactionTime + (m_fCurrentSuppression / 50.0);
			
			if (elapsedTime >= effectiveReactionDelay)
			{
				m_bHasProcessedOODA = true;
				Print("Advanced AI: OODA Loop Complete. ACT phase begun! Returning fire.");
				EvaluateSquadDynamics();
			}
			return;
		}

		// (The AI natively handles pulling the trigger via vanilla Arma scripts from this point)
	}

	protected void EvaluateSquadDynamics()
	{
		// Asymmetric Bounding Overwatch: Assigning Roles mathematically
		// If we do not have a role, we dynamically pick one based on our distance/state
		
		if (m_CurrentRole != ESquadCombatRole.UNASSIGNED) return;
		
		// For simplicity in this script layer, we randomly roll roles, 
		// but typically we'd elect the closest unit as Maneuver and the farthest in heavy cover as Base of Fire.
		if (Math.RandomFloat01() > 0.5)
		{
			m_CurrentRole = ESquadCombatRole.BASE_OF_FIRE;
			Print("Advanced AI: Role Assigned -> BASE OF FIRE. Providing suppressive cover.");
			
			// Inject Defend Waypoint
			AssignWaypointNatively(m_sDefendWaypoint, m_Group.GetOrigin());
		}
		else
		{
			m_CurrentRole = ESquadCombatRole.MANEUVER;
			
			// Calculate Flanking Vector
			vector targetPos = m_KnownThreat.GetOrigin();
			vector myPos = m_Group.GetOrigin();
			
			// Vector Math: Get direction to target, rotate 90 degrees around Y axis (Up) to find the flank
			vector dir = vector.Direction(myPos, targetPos);
			dir.Normalize();
			
			vector flankDir;
			flankDir[0] = -dir[2];
			flankDir[1] = 0;
			flankDir[2] = dir[0];
			
			// Push the flank point 30 meters out laterally, and slightly forward
			vector desiredFlank = myPos + (flankDir * 30.0) + (dir * 10.0);
			
			Print("Advanced AI: Role Assigned -> MANEUVER. Pathing to algorithmic side-flank vector.");
			AssignWaypointNatively(m_sMoveWaypoint, desiredFlank);
		}
	}
	
	protected void AssignWaypointNatively(ResourceName wpObj, vector location)
	{
		array<AIWaypoint> wps = new array<AIWaypoint>();
		m_Group.GetWaypoints(wps);
		foreach (AIWaypoint oldWp : wps)
		{
			m_Group.RemoveWaypoint(oldWp);
		}
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = location;

		IEntity wpEnt = GetGame().SpawnEntityPrefab(Resource.Load(wpObj), GetGame().GetWorld(), params);
		AIWaypoint wp = AIWaypoint.Cast(wpEnt);

		if (wp)
		{
			wp.SetCompletionRadius(10.0);
			m_Group.AddWaypoint(wp);
		}
	}
}
