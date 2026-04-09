class SCR_StrongholdComponentClass : ScriptComponentClass
{
}

class SCR_StrongholdComponent : ScriptComponent
{
	[Attribute("10", desc: "Radius in meters to scatter the garrison troops")]
	protected float m_fGarrisonRadius;

	[Attribute("{E552DABF3636C2AD}Prefabs/Groups/OPFOR/Group_USSR_RifleSquad.et", UIWidgets.ResourceNamePicker, desc: "AIGroup representing the baseline defenders")]
	protected ResourceName m_sGarrisonGroupPrefab;
	
	[Attribute("{E5B990DB0AF3D288}Prefabs/AI/Waypoints/AIWaypoint_Defend.et", UIWidgets.ResourceNamePicker, desc: "Native waypoint establishing patrol/defend vectors")]
	protected ResourceName m_sDefendWaypoint;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// Spawn the garrison troops a few seconds after server init to ensure mapping is complete
		GetGame().GetCallqueue().CallLater(EstablishGarrison, 5000, false);
	}

	protected void EstablishGarrison()
	{
		vector origin = GetOwner().GetOrigin();
		
		// 1. Spawn the Group Prefab exactly at the Component's coordinate
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = origin;
		
		IEntity groupEntity = GetGame().SpawnEntityPrefab(Resource.Load(m_sGarrisonGroupPrefab), GetGame().GetWorld(), params);
		SCR_AIGroup aiGroup = SCR_AIGroup.Cast(groupEntity);
		
		if (!aiGroup)
		{
			Print("Server Error: Static Stronghold failed to establish AI Group prefab.");
			return;
		}
		
		// 2. Generate Defend Waypoint
		IEntity waypointEnt = GetGame().SpawnEntityPrefab(Resource.Load(m_sDefendWaypoint), GetGame().GetWorld(), params);
		AIWaypoint defendWP = AIWaypoint.Cast(waypointEnt);
		
		if (defendWP)
		{
			// Give them a patrol radius
			defendWP.SetCompletionRadius(m_fGarrisonRadius);
			aiGroup.AddWaypoint(defendWP);
		}
		
		Print("Server: Soviet Stronghold Established natively at " + origin.ToString() + ". Garrison patrol active.");
	}
}
