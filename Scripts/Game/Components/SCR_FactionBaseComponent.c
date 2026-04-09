class SCR_FactionBaseComponentClass : ScriptComponentClass
{
}

class SCR_FactionBaseComponent : ScriptComponent
{
	[Attribute("USSR", UIWidgets.ComboBox, "Default Starting Faction", "", ParamEnumArray.FromEnum(SCR_EBaseFaction))]
	protected string m_sControllingFaction; // "US" (Mercs), "USSR" (Military), "FIA" (Bandits)
	
	[Attribute("50", desc: "Radius defining the capture zone of the base")]
	protected float m_fCaptureRadius;
	
	[Attribute("{E552DABF3636C2AD}Prefabs/Groups/OPFOR/Group_USSR_RifleSquad.et", UIWidgets.ResourceNamePicker, "Military Defender Prefab")]
	protected ResourceName m_sMilitaryDefenders;
	
	[Attribute("{7A9EE19AB67B298B}Prefabs/Groups/INDEP/Group_FIA_Squad.et", UIWidgets.ResourceNamePicker, "Bandit Defender Prefab")]
	protected ResourceName m_sBanditDefenders;
	
	[Attribute("{84E5BBAB25EA23E5}Prefabs/Groups/BLUFOR/Group_US_RifleSquad.et", UIWidgets.ResourceNamePicker, "Mercenary Defender Prefab")]
	protected ResourceName m_sMercDefenders;
	
	[Attribute("{E5B990DB0AF3D288}Prefabs/AI/Waypoints/AIWaypoint_Defend.et", UIWidgets.ResourceNamePicker, desc: "Native waypoint establishing patrol vectors")]
	protected ResourceName m_sDefendWaypoint;

	protected IEntity m_CurrentGarrison;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// Register globally to the War Manager
		SCR_FactionWarManager warMgr = SCR_FactionWarManager.GetInstance();
		if (warMgr) warMgr.RegisterBase(this);
		
		// Establish initial defenses
		GetGame().GetCallqueue().CallLater(EstablishDefenses, 5000, false);
		
		// Run a slow, continuous mathematical physics check for capture influence
		GetGame().GetCallqueue().CallLater(EvaluateCapturePhysics, 15000, true);
	}

	string GetControllingFaction()
	{
		return m_sControllingFaction;
	}

	void EstablishDefenses()
	{
		// Wipe out any existing garrison logically if the base flipped
		if (m_CurrentGarrison)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(m_CurrentGarrison);
			m_CurrentGarrison = null;
		}

		vector origin = GetOwner().GetOrigin();
		ResourceName spawnPrefab = "";
		
		if (m_sControllingFaction == "USSR") spawnPrefab = m_sMilitaryDefenders;
		else if (m_sControllingFaction == "US") spawnPrefab = m_sMercDefenders;
		else if (m_sControllingFaction == "FIA") spawnPrefab = m_sBanditDefenders;

		if (spawnPrefab == "") return;

		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = origin;
		
		m_CurrentGarrison = GetGame().SpawnEntityPrefab(Resource.Load(spawnPrefab), GetGame().GetWorld(), params);
		SCR_AIGroup aiGroup = SCR_AIGroup.Cast(m_CurrentGarrison);
		
		if (aiGroup)
		{
			IEntity waypointEnt = GetGame().SpawnEntityPrefab(Resource.Load(m_sDefendWaypoint), GetGame().GetWorld(), params);
			AIWaypoint defendWP = AIWaypoint.Cast(waypointEnt);
			
			if (defendWP)
			{
				defendWP.SetCompletionRadius(m_fCaptureRadius);
				aiGroup.AddWaypoint(defendWP);
			}
		}
		
		Print("FW: Base established defenses for Faction " + m_sControllingFaction + " at " + origin.ToString());
	}

	protected void EvaluateCapturePhysics()
	{
		// Native query inside the Capture Radius to find any enemy FactionAffiliationComponents
		// For simplicity in the script outline, we'll pseudo-replicate the search.
		// In a live physical engine block, this uses GetGame().GetWorld().QueryEntitiesBySphere()
		
		// Placeholder for logic execution: 
		// If another faction's entity array heavily outnumbers the controlling faction's array, flip the state.
		
		/*
		int ussrCount, usCount, fiaCount = 0;
		// physics trace logic mapping counts to the base variables
		// if (usCount > 4 && ussrCount == 0) FlipBase("US");
		*/
	}
	
	void TriggerFlipBase(string overridingFaction)
	{
		if (m_sControllingFaction == overridingFaction) return;
		
		Print("FW: BASE CAPTURED! Territory has flipped from " + m_sControllingFaction + " to " + overridingFaction);
		
		// Update Network/PDA
		SCR_PDA_UI ui = SCR_PDA_UI.GetInstance();
		if (ui) ui.ReceiveMessage("General Network", "Territory shifts! The " + overridingFaction + " have seized a new sector!");

		m_sControllingFaction = overridingFaction;
		EstablishDefenses();
	}
}

enum SCR_EBaseFaction
{
	"USSR",
	"US",
	"FIA"
}
