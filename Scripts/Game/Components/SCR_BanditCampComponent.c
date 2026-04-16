class SCR_BanditCampComponentClass : ScriptComponentClass
{
}

class SCR_BanditCampComponent : ScriptComponent
{
	[Attribute("25", desc: "Radius in meters to scatter the Bandit thugs")]
	protected float m_fCampRadius;

	[Attribute("{7A9EE19AB67B298B}Prefabs/Groups/INDEP/Group_FIA_Squad.et", UIWidgets.ResourceNamePicker, desc: "AIGroup representing the Bandit Gang protecting the camp")]
	protected ResourceName m_sBanditGroupPrefab;
	
	[Attribute("{E5B990DB0AF3D288}Prefabs/AI/Waypoints/AIWaypoint_Defend.et", UIWidgets.ResourceNamePicker, desc: "Native waypoint establishing patrol/defend vectors around the cache")]
	protected ResourceName m_sDefendWaypoint;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// Register this camp to the global pool for QRF spawning
		SCR_BanditQRFManager qrfMgr = SCR_BanditQRFManager.GetInstance();
		if (qrfMgr) qrfMgr.RegisterCamp(this);
		
		// Spawn the Bandit garrison troops a few seconds after server init to ensure mapping is complete
		GetGame().GetCallqueue().CallLater(EstablishBanditCamp, 4000, false);
	}

	protected void EstablishBanditCamp()
	{
		vector origin = GetOwner().GetOrigin();
		
		// 1. Spawn the Group Prefab exactly at the Component's coordinate
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = origin;
		
		IEntity groupEntity = GetGame().SpawnEntityPrefab(Resource.Load(m_sBanditGroupPrefab), GetGame().GetWorld(), params);
		SCR_AIGroup aiGroup = SCR_AIGroup.Cast(groupEntity);
		
		if (!aiGroup)
		{
			Print("Server Error: Bandit Camp failed to establish AI Group prefab.");
			return;
		}
		
		SCR_StalkerAIBrainComponent brain = SCR_StalkerAIBrainComponent.Cast(GetOwner().FindComponent(SCR_StalkerAIBrainComponent));
		if (brain)
		{
			// Connect the Utility AI to the newly spawned group
			brain.SetGroup(aiGroup);
			Print("Server: Bandit Camp Hooked natively with UTILITY BRAIN at " + origin.ToString() + ". Autonomous lifecycle begun.");
		}
		else
		{
			// 2. Fallback: Generate Vanilla Defend Waypoint
			IEntity waypointEnt = GetGame().SpawnEntityPrefab(Resource.Load(m_sDefendWaypoint), GetGame().GetWorld(), params);
			AIWaypoint defendWP = AIWaypoint.Cast(waypointEnt);
			
			if (defendWP)
			{
				// Give them a patrol radius
				defendWP.SetCompletionRadius(m_fCampRadius);
				aiGroup.AddWaypoint(defendWP);
			}
			Print("Server: Bandit Camp Hooked securely natively at " + origin.ToString() + ". Thugs on static patrol.");
		}
		
		// 3. Optional: Connect Advanced Combat AI
		SCR_AdvancedCombatAIComponent combatAI = SCR_AdvancedCombatAIComponent.Cast(GetOwner().FindComponent(SCR_AdvancedCombatAIComponent));
		if (combatAI)
		{
			combatAI.SetGroup(aiGroup);
			Print("Server: Advanced Combat AI Tactical Layer Attached.");
		}
	}
}
