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
		
		if (m_Group && Replication.IsServer())
		{
			StartTactics();
		}
	}
	
	void SetGroup(SCR_AIGroup group)
	{
		m_Group = group;
		if (Replication.IsServer())
		{
			StartTactics();
		}
	}
	
	protected void StartTactics()
	{
		if (!Replication.IsServer()) return;

		// Tactical evaluation tick logic (3 times a second during combat)
		GetGame().GetCallqueue().CallLater(TacticalThink, 333, true);
		
		// Active Threat scanning loop on the server every 2 seconds
		GetGame().GetCallqueue().CallLater(ScanThreats, 2000, true);
		
		// Decay suppression slowly
		GetGame().GetCallqueue().CallLater(DecaySuppression, 1000, true);
	}

	void OnBulletSnap(vector trajectory)
	{
		if (!Replication.IsServer()) return;

		m_fCurrentSuppression += m_fSuppressionPerBullet;
		if (m_fCurrentSuppression > m_fMaxSuppression)
			m_fCurrentSuppression = m_fMaxSuppression;
	}

	void OnThreatSpotted(IEntity threat)
	{
		if (!Replication.IsServer()) return;

		if (m_KnownThreat != threat)
		{
			m_KnownThreat = threat;
			m_fTimeThreatFirstSpotted = GetGame().GetWorld().GetWorldTime() / 1000.0;
			m_bHasProcessedOODA = false;
			
			Print("Server Advanced AI: Threat Spotted. OODA Observe -> Orient sequence initialized.");
		}
	}

	protected void DecaySuppression()
	{
		if (!Replication.IsServer()) return;

		if (m_fCurrentSuppression > 0)
		{
			m_fCurrentSuppression -= m_fSuppressionDecayRate;
			if (m_fCurrentSuppression < 0) m_fCurrentSuppression = 0;
		}
	}

	protected void ScanThreats()
	{
		if (!m_Group || !Replication.IsServer()) return;
		
		// Query nearby radius to find active hostile entities
		vector center = m_Group.GetOrigin();
		GetGame().GetWorld().QueryEntitiesBySphere(center, 150.0, CheckEnemyEntity, null, EQueryEntitiesFlags.ALL);
	}

	protected bool CheckEnemyEntity(IEntity ent)
	{
		if (!ent || ent == m_Group) return true;
		
		// If it's a character, evaluate faction hostilities
		FactionAffiliationComponent factionAff = FactionAffiliationComponent.Cast(ent.FindComponent(FactionAffiliationComponent));
		if (factionAff)
		{
			Faction enemyFaction = factionAff.GetAffiliatedFaction();
			Faction groupFaction = m_Group.GetFaction();
			
			if (enemyFaction && groupFaction && enemyFaction.IsEnemy(groupFaction))
			{
				// Trigger the threat spotting OODA logic
				OnThreatSpotted(ent);
				return false; // Threat spotted, terminate search
			}
		}
		return true;
	}

	protected void TacticalThink()
	{
		if (!m_Group || !m_KnownThreat || !Replication.IsServer()) return;

		// 1. Evaluate Psychological Suppression State
		if (m_fCurrentSuppression >= m_fMaxSuppression * 0.8)
		{
			Print("Server Advanced AI: Group is heavily suppressed, freezing tactical maneuvres.");
			return; 
		}

		// 2. OODA Loop Processing Delay
		if (!m_bHasProcessedOODA)
		{
			float currentTime = GetGame().GetWorld().GetWorldTime() / 1000.0;
			float elapsedTime = currentTime - m_fTimeThreatFirstSpotted;
			
			float effectiveReactionDelay = m_fBaseReactionTime + (m_fCurrentSuppression / 50.0);
			
			if (elapsedTime >= effectiveReactionDelay)
			{
				m_bHasProcessedOODA = true;
				Print("Server Advanced AI: OODA Loop Complete. Returning fire.");
				EvaluateSquadDynamics();
			}
			return;
		}
	}

	protected void EvaluateSquadDynamics()
	{
		if (m_CurrentRole != ESquadCombatRole.UNASSIGNED) return;
		
		if (Math.RandomFloat01() > 0.5)
		{
			m_CurrentRole = ESquadCombatRole.BASE_OF_FIRE;
			Print("Server Advanced AI: Role Assigned -> BASE OF FIRE. Providing cover.");
			AssignWaypointNatively(m_sDefendWaypoint, m_Group.GetOrigin());
		}
		else
		{
			m_CurrentRole = ESquadCombatRole.MANEUVER;
			
			vector targetPos = m_KnownThreat.GetOrigin();
			vector myPos = m_Group.GetOrigin();
			
			vector dir = vector.Direction(myPos, targetPos);
			dir.Normalize();
			
			vector flankDir;
			flankDir[0] = -dir[2];
			flankDir[1] = 0;
			flankDir[2] = dir[0];
			
			vector desiredFlank = myPos + (flankDir * 30.0) + (dir * 10.0);
			
			Print("Server Advanced AI: Role Assigned -> MANEUVER. Moving to flanking vector.");
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
