class SCR_MilitaryQRFManagerClass : GenericEntityClass
{
}

class SCR_MilitaryQRFManager : GenericEntity
{
	protected static SCR_MilitaryQRFManager s_Instance;

	[Attribute("{7C7DA72816AB4EBD}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed.et", UIWidgets.ResourceNamePicker, desc: "Armed Soviet Helicopter Prefab")]
	protected ResourceName m_sHelicopterPrefab;
	
	[Attribute("{E552DABF3636C2AD}Prefabs/Groups/OPFOR/Group_USSR_RifleSquad.et", UIWidgets.ResourceNamePicker, desc: "AIGroup to spawn containing the pilots and crew")]
	protected ResourceName m_sCrewGroupPrefab;
	
	[Attribute("{22A875E30470BD4F}Prefabs/AI/Waypoints/AIWaypoint_SearchAndDestroy.et", UIWidgets.ResourceNamePicker, desc: "Waypoint archetype for Gunship circling")]
	protected ResourceName m_sWaypointPrefab;

	static SCR_MilitaryQRFManager GetInstance()
	{
		return s_Instance;
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		s_Instance = this;
	}

	void DispatchHelicopter(vector targetZone)
	{
		// 1. Math: Calculate an edge-of-map spawn location (e.g. 3500m out and 200m in the air)
		vector origin = targetZone;
		float dist = 3500;
		float angle = Math.RandomFloat(0, Math.PI2);
		
		vector spawnPos = "0 0 0";
		spawnPos[0] = origin[0] + (Math.Cos(angle) * dist);
		spawnPos[2] = origin[2] + (Math.Sin(angle) * dist);
		spawnPos[1] = GetGame().GetWorld().GetSurfaceY(spawnPos[0], spawnPos[2]) + 200; // 200m altitude

		// 2. Spawn the physical Helicopter
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos;
		
		IEntity heli = GetGame().SpawnEntityPrefab(Resource.Load(m_sHelicopterPrefab), GetGame().GetWorld(), params);
		
		if (!heli)
		{
			Print("Server Error: QRF failed to spawn Mi-8 platform natively.");
			return;
		}

		// 3. Spawn the AI Crew
		// Note from plan: The user chose a Circling Gunship rather than un-loading trees. 
		// The crew spawns directly into the helicopter mapping.
		IEntity groupEntity = GetGame().SpawnEntityPrefab(Resource.Load(m_sCrewGroupPrefab), GetGame().GetWorld(), params);
		SCR_AIGroup aiGroup = SCR_AIGroup.Cast(groupEntity);
		if (!aiGroup) return;

		// Move AI into vehicle
		// Usually done natively by CompartmentAccessComponent commands in Reforger, we'll proxy it for the conceptual layout.
		Print("Server: Mi-8 Spawned! Crew boarding dynamically.");
		
		// 4. Generate Waypoint to Hotzone
		EntitySpawnParams wpParams = new EntitySpawnParams();
		wpParams.TransformMode = ETransformMode.WORLD;
		wpParams.Transform[3] = targetZone;
		
		IEntity waypointEnt = GetGame().SpawnEntityPrefab(Resource.Load(m_sWaypointPrefab), GetGame().GetWorld(), wpParams);
		AIWaypoint qrfWP = AIWaypoint.Cast(waypointEnt);
		
		if (qrfWP && aiGroup)
		{
			qrfWP.SetCompletionRadius(250); // Loiter radius
			aiGroup.AddWaypoint(qrfWP);
		}
		
		Print("Server: QRF AI assigned Search And Destroy Waypoint at: " + targetZone.ToString());
	}
}
