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

	[Attribute("4700 200 11000", desc: "Everon Airbase coordinate (Z value is +200m for safe aerial spawn)")]
	protected vector m_vAirbaseSpawnPos;
	
	protected bool m_bAirbaseLost = false;

	static SCR_MilitaryQRFManager GetInstance()
	{
		return s_Instance;
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		s_Instance = this;
	}

	void SetAirbaseLost(bool state)
	{
		if (!Replication.IsServer()) return;
		m_bAirbaseLost = state;
		if (state) Print("Server: Airbase has been officially designated as LOST. All future QRFs will route from off-map sectors.");
	}

	void DispatchHelicopter(vector targetZone)
	{
		if (!Replication.IsServer()) return;
		
		vector spawnPos = m_vAirbaseSpawnPos;

		// If the airbase is lost, use the fallback edge-of-map radial math
		if (m_bAirbaseLost)
		{
			vector origin = targetZone;
			float dist = 4000; // Edge of map distance
			float angle = Math.RandomFloat(0, Math.PI2);
			
			spawnPos[0] = origin[0] + (Math.Cos(angle) * dist);
			spawnPos[2] = origin[2] + (Math.Sin(angle) * dist);
			spawnPos[1] = GetGame().GetWorld().GetSurfaceY(spawnPos[0], spawnPos[2]) + 200; 
			Print("Server: Airbase offline. QRF routing from ocean boundaries.");
		}

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

		// 3. Spawn the AI Crew and board them into the helicopter
		IEntity groupEntity = GetGame().SpawnEntityPrefab(Resource.Load(m_sCrewGroupPrefab), GetGame().GetWorld(), params);
		SCR_AIGroup aiGroup = SCR_AIGroup.Cast(groupEntity);
		if (!aiGroup) return;

		BoardGroupIntoVehicle(aiGroup, heli);
		Print("Server: Mi-8 QRF Spawned! AI Crew boarded dynamically.");
		
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

	// Used when the Airbase is directly assaulted
	void LaunchMassResponse(vector hotzone)
	{
		if (!Replication.IsServer()) return;
		Print("Server: MASS RESPONSE SCRAMBLED. Multiple bogeys incoming.");
		
		// Spawn 2 Gunships!
		DispatchHelicopter(hotzone);
		
		// Small offset so the second chopper doesn't collide
		vector offsetZone = hotzone;
		offsetZone[0] = offsetZone[0] + 50; 
		DispatchHelicopter(offsetZone);
	}
	
	// Helper to board group members into compartment slots natively
	protected void BoardGroupIntoVehicle(SCR_AIGroup aiGroup, IEntity vehicle)
	{
		if (!aiGroup || !vehicle) return;
		
		array<IEntity> members = new array<IEntity>();
		aiGroup.GetMembers(members);
		
		foreach (IEntity member : members)
		{
			CompartmentAccessComponent compartmentAccess = CompartmentAccessComponent.Cast(member.FindComponent(CompartmentAccessComponent));
			if (compartmentAccess)
			{
				compartmentAccess.MoveInVehicle(vehicle);
			}
		}
	}
}
