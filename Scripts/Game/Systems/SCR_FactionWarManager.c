class SCR_FactionWarManagerClass : GenericEntityClass
{
}

class SCR_FactionWarManager : GenericEntity
{
	protected static SCR_FactionWarManager s_Instance;

	protected ref array<SCR_FactionBaseComponent> m_aActiveBases = new array<SCR_FactionBaseComponent>();
	
	[Attribute("{22A875E30470BD4F}Prefabs/AI/Waypoints/AIWaypoint_SearchAndDestroy.et", UIWidgets.ResourceNamePicker, desc: "Waypoint archetype for Assault Squads")]
	protected ResourceName m_sAssaultWaypoint;
	
	[Attribute("{7A9EE19AB67B298B}Prefabs/Groups/INDEP/Group_FIA_Squad.et", UIWidgets.ResourceNamePicker)]
	protected ResourceName m_sBanditAssaultGroup;
	
	[Attribute("{84E5BBAB25EA23E5}Prefabs/Groups/BLUFOR/Group_US_RifleSquad.et", UIWidgets.ResourceNamePicker)]
	protected ResourceName m_sMercAssaultGroup;
	
	[Attribute("{13A84988F0CAEE44}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed.et", UIWidgets.ResourceNamePicker, desc: "UH-1H Huey for Mercenary Aerial Insertion (Gunship)")]
	protected ResourceName m_sMercHelicopterPrefab;
	
	[Attribute("{B76CC790A2EACA3F}Prefabs/Vehicles/Helicopters/UH1H/UH1H_unarmed.et", UIWidgets.ResourceNamePicker, desc: "UH-1H Huey for Mercenary Troop Transport")]
	protected ResourceName m_sMercTransportPrefab;
	
	[Attribute("{201F87042C06BD16}Prefabs/AI/Waypoints/AIWaypoint_GetOut.et", UIWidgets.ResourceNamePicker, desc: "Waypoint archetype for dismounting transport")]
	protected ResourceName m_sGetOutWaypoint;

	
	[Attribute("{E552DABF3636C2AD}Prefabs/Groups/OPFOR/Group_USSR_RifleSquad.et", UIWidgets.ResourceNamePicker)]
	protected ResourceName m_sMilitaryAssaultGroup;

	static SCR_FactionWarManager GetInstance()
	{
		return s_Instance;
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		s_Instance = this;
		
		// Run evaluation every 10 minutes (600000ms)
		GetGame().GetCallqueue().CallLater(EvaluateGeopolitics, 600000, true);
	}

	void RegisterBase(SCR_FactionBaseComponent base)
	{
		if (!m_aActiveBases.Contains(base))
		{
			m_aActiveBases.Insert(base);
			Print("FW: Registered active base to global matrix.");
		}
	}

	void GetFactionControlStats(out int ussrScore, out int usScore, out int fiaScore)
	{
		ussrScore = 0;
		usScore = 0;
		fiaScore = 0;
		
		foreach (SCR_FactionBaseComponent base : m_aActiveBases)
		{
			string faction = base.GetControllingFaction();
			if (faction == "USSR") ussrScore++;
			else if (faction == "US") usScore++;
			else if (faction == "FIA") fiaScore++;
		}
	}

	protected void EvaluateGeopolitics()
	{
		Print("FW: Evaluating global faction matrix...");
		
		if (m_aActiveBases.Count() < 2) return;
		
		// 1. Pick a random base to LAUNCH the attack
		int r = Math.RandomInt(0, m_aActiveBases.Count());
		SCR_FactionBaseComponent attackerBase = m_aActiveBases[r];
		string attackerIdentity = attackerBase.GetControllingFaction();
		
		// 2. Pick a valid ENEMY base to attack
		SCR_FactionBaseComponent targetBase;
		foreach (SCR_FactionBaseComponent base : m_aActiveBases)
		{
			if (base.GetControllingFaction() != attackerIdentity)
			{
				targetBase = base;
				break;
			}
		}
		
		if (!targetBase) return; // World is completely conquered by one faction!
		
		// 3. Initiate Faction Assault
		LaunchAssault(attackerBase, targetBase);
	}
	
	protected void LaunchAssault(SCR_FactionBaseComponent originBase, SCR_FactionBaseComponent targetBase)
	{
		string attackingFaction = originBase.GetControllingFaction();
		ResourceName assaultPrefab = "";
		
		if (attackingFaction == "USSR") assaultPrefab = m_sMilitaryAssaultGroup;
		else if (attackingFaction == "US") assaultPrefab = m_sMercAssaultGroup;
		else if (attackingFaction == "FIA") assaultPrefab = m_sBanditAssaultGroup;
		
		if (assaultPrefab == "") return;

		// -------------------------------------------------------------
		// MERCENARY AERIAL DEPLOYMENT LOGIC (US Faction)
		// -------------------------------------------------------------
		if (attackingFaction == "US")
		{
			// Math: Spawn helicopter 3500m away, 200m in the air
			vector origin = targetBase.GetOwner().GetOrigin();
			float dist = 3500;
			float angle = Math.RandomFloat(0, Math.PI2);
			
			vector spawnPos = "0 0 0";
			spawnPos[0] = origin[0] + (Math.Cos(angle) * dist);
			spawnPos[2] = origin[2] + (Math.Sin(angle) * dist);
			spawnPos[1] = GetGame().GetWorld().GetSurfaceY(spawnPos[0], spawnPos[2]) + 200; 

			// 1. SPAWN GUNSHIP (Loitering Cover)
			EntitySpawnParams heliParams = new EntitySpawnParams();
			heliParams.TransformMode = ETransformMode.WORLD;
			heliParams.Transform[3] = spawnPos;
			
			IEntity gunship = GetGame().SpawnEntityPrefab(Resource.Load(m_sMercHelicopterPrefab), GetGame().GetWorld(), heliParams);
			IEntity gunshipCrew = GetGame().SpawnEntityPrefab(Resource.Load(m_sMercAssaultGroup), GetGame().GetWorld(), heliParams);
			SCR_AIGroup aiGunshipCrew = SCR_AIGroup.Cast(gunshipCrew);
			
			if (aiGunshipCrew)
			{
				EntitySpawnParams wpParamsGunship = new EntitySpawnParams();
				wpParamsGunship.TransformMode = ETransformMode.WORLD;
				wpParamsGunship.Transform[3] = targetBase.GetOwner().GetOrigin();
				
				IEntity wpt = GetGame().SpawnEntityPrefab(Resource.Load(m_sAssaultWaypoint), GetGame().GetWorld(), wpParamsGunship);
				AIWaypoint wpSearchHeli = AIWaypoint.Cast(wpt);
				if (wpSearchHeli)
				{
					wpSearchHeli.SetCompletionRadius(250); // Loiter radius
					aiGunshipCrew.AddWaypoint(wpSearchHeli);
				}
			}
			
			// 2. SPAWN TRANSPORT (Dismount Assault)
			vector transportPos = spawnPos;
			transportPos[0] = transportPos[0] + 50; // Fly 50m to the right in formation
			
			EntitySpawnParams transportParams = new EntitySpawnParams();
			transportParams.TransformMode = ETransformMode.WORLD;
			transportParams.Transform[3] = transportPos;
			
			IEntity transportHeli = GetGame().SpawnEntityPrefab(Resource.Load(m_sMercTransportPrefab), GetGame().GetWorld(), transportParams);
			IEntity transportSquad = GetGame().SpawnEntityPrefab(Resource.Load(m_sMercAssaultGroup), GetGame().GetWorld(), transportParams);
			SCR_AIGroup aiTransportSquad = SCR_AIGroup.Cast(transportSquad);
			
			if (aiTransportSquad)
			{
				EntitySpawnParams wpParamsTransport = new EntitySpawnParams();
				wpParamsTransport.TransformMode = ETransformMode.WORLD;
				wpParamsTransport.Transform[3] = targetBase.GetOwner().GetOrigin();
				
				IEntity wptOut = GetGame().SpawnEntityPrefab(Resource.Load(m_sGetOutWaypoint), GetGame().GetWorld(), wpParamsTransport);
				AIWaypoint wpGetOut = AIWaypoint.Cast(wptOut);
				if (wpGetOut)
				{
					wpGetOut.SetCompletionRadius(20); // Get extremely close to the ground/base to deploy troops
					aiTransportSquad.AddWaypoint(wpGetOut);
				}
			}
			
			SCR_PDA_UI ui = SCR_PDA_UI.GetInstance();
			if (ui) ui.ReceiveMessage("Network", "Radar spike! A Mercenary 2-Ship formation (Gunship + Transport) is vectoring onto the " + targetBase.GetControllingFaction() + " sector!");
			
			Print("FW: Mercenary Aerial 2-Ship Assault launched from boundary targeting " + targetBase.GetControllingFaction());
			return; // Abort standard ground spawning
		}

		// -------------------------------------------------------------
		// STANDARD GROUND DEPLOYMENT LOGIC (USSR & FIA)
		// -------------------------------------------------------------
		
		// Spawn the Attackers at their home base
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = originBase.GetOwner().GetOrigin();
		
		IEntity groupEntity = GetGame().SpawnEntityPrefab(Resource.Load(assaultPrefab), GetGame().GetWorld(), params);
		SCR_AIGroup aiGroup = SCR_AIGroup.Cast(groupEntity);
		
		if (!aiGroup) return;

		// Assign them a Search & Destroy directly on top of the enemy base coordinates
		EntitySpawnParams wpParams = new EntitySpawnParams();
		wpParams.TransformMode = ETransformMode.WORLD;
		wpParams.Transform[3] = targetBase.GetOwner().GetOrigin();
		
		IEntity waypointEnt = GetGame().SpawnEntityPrefab(Resource.Load(m_sAssaultWaypoint), GetGame().GetWorld(), wpParams);
		AIWaypoint wp = AIWaypoint.Cast(waypointEnt);
		
		if (wp)
		{
			wp.SetCompletionRadius(40);
			aiGroup.AddWaypoint(wp);
		}
		
		// Broadcast to players via PDA Simulation
		SCR_PDA_UI ui = SCR_PDA_UI.GetInstance();
		if (ui) ui.ReceiveMessage("Network", "We just saw a " + attackingFaction + " hit squad leaving camp heading towards the " + targetBase.GetControllingFaction() + " sector!");
		
		Print("FW: Assault Squad launched from " + attackingFaction + " targeting " + targetBase.GetControllingFaction());
	}
}
