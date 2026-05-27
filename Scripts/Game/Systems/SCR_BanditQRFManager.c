class SCR_BanditQRFManagerClass : GenericEntityClass
{
}

class SCR_BanditQRFManager : GenericEntity
{
	protected static SCR_BanditQRFManager s_Instance;

	[Attribute("{FE18CF81EA3ED6D8}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_FIA_armed.et", UIWidgets.ResourceNamePicker, desc: "Armed Bandit Vehicle Prefab")]
	protected ResourceName m_sBanditVehiclePrefab;
	
	[Attribute("{7A9EE19AB67B298B}Prefabs/Groups/INDEP/Group_FIA_Squad.et", UIWidgets.ResourceNamePicker, desc: "AIGroup to spawn containing the Bandit gang")]
	protected ResourceName m_sBanditGroupPrefab;
	
	[Attribute("{201F87042C06BD16}Prefabs/AI/Waypoints/AIWaypoint_GetOut.et", UIWidgets.ResourceNamePicker, desc: "Waypoint archetype for jumping out of trucks and engaging")]
	protected ResourceName m_sGetOutWaypointPrefab;
	
	protected ref array<SCR_BanditCampComponent> m_aActiveCamps = new array<SCR_BanditCampComponent>();

	static SCR_BanditQRFManager GetInstance()
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
	
	void RegisterCamp(SCR_BanditCampComponent camp)
	{
		if (!m_aActiveCamps.Contains(camp))
		{
			m_aActiveCamps.Insert(camp);
			Print("Server: SCR_BanditQRFManager: Registered camp to global pool.");
		}
	}

	void DispatchMotorizedBandits(vector targetZone)
	{
		if (!Replication.IsServer()) return;
		
		// Force QRF to spawn physically from an established base
		if (m_aActiveCamps.Count() == 0)
		{
			Print("Server Warning: No physical Bandit Camps established on the map. QRF cannot dispatch.");
			return;
		}
		
		// Grab the closest (or first available) camp
		SCR_BanditCampComponent dispatchCamp = m_aActiveCamps[0];
		vector spawnPos = dispatchCamp.GetOwner().GetOrigin();
		
		// Adjust UAZ spawn slightly so it doesn't crush the garrisoned guards
		spawnPos[0] = spawnPos[0] + 15;

		// 2. Physical Object Spawning
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = spawnPos;
		
		IEntity truck = GetGame().SpawnEntityPrefab(Resource.Load(m_sBanditVehiclePrefab), GetGame().GetWorld(), params);
		if (!truck)
		{
			Print("Server Error: Bandit QRF failed to deploy physical transport.");
			return;
		}

		// 3. Crew Injection & Compartment Boarding
		IEntity groupEntity = GetGame().SpawnEntityPrefab(Resource.Load(m_sBanditGroupPrefab), GetGame().GetWorld(), params);
		SCR_AIGroup aiGroup = SCR_AIGroup.Cast(groupEntity);
		if (!aiGroup) return;

		BoardGroupIntoVehicle(aiGroup, truck);
		Print("Server: Bandit Motorized Transport spawned! Bandits boarded vehicle dynamically.");
		
		// 4. Generate Get-Out Waypoint inside the Hotzone
		EntitySpawnParams wpParams = new EntitySpawnParams();
		wpParams.TransformMode = ETransformMode.WORLD;
		wpParams.Transform[3] = targetZone;
		
		IEntity waypointEnt = GetGame().SpawnEntityPrefab(Resource.Load(m_sGetOutWaypointPrefab), GetGame().GetWorld(), wpParams);
		AIWaypoint qrfWP = AIWaypoint.Cast(waypointEnt);
		
		if (qrfWP && aiGroup)
		{
			qrfWP.SetCompletionRadius(50); // Dismount target
			aiGroup.AddWaypoint(qrfWP);
		}
		
		Print("Server: Motorized Bandits navigating to Target Zone. Prepare for dismount!");
		
		// Notify PDA Network
		SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
		if (network)
		{
			network.SendNetworkMessage("We hear gunshots. Mounting up and heading over there now.", "Renegade Radio");
		}
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
