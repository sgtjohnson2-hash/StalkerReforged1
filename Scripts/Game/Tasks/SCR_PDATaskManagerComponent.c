enum EStalkerTaskType
{
	ASSASSINATION,
	FETCH,
	FACTION_ASSAULT
}

// Inherit from Arma Reforger's vanilla SCR_BaseTask so it appears natively on the HUD/Map
class SCR_StalkerBaseTask : SCR_BaseTask
{
	[Attribute("1000", desc: "Reward in RU upon completion")]
	protected int m_iRewardRU;
	
	[Attribute("0", desc: "Task type mapping")]
	protected EStalkerTaskType m_eTaskType;
	
	// Track the specific enemy or item this task revolves around
	protected IEntity m_TargetEntity;

	int GetRewardRU()
	{
		return m_iRewardRU;
	}
	
	EStalkerTaskType GetStalkerTaskType()
	{
		return m_eTaskType;
	}
	
	void SetTargetEntity(IEntity target)
	{
		m_TargetEntity = target;
	}

	// This method hooks into vanilla task completion logic
	override void Finish(bool showMsg = true)
	{
		super.Finish(showMsg);
		
		// When the task finishes natively (e.g. via GameMaster or script), grant player RU
		Print("Client PDA: Task [" + GetTitle() + "] Completed natively through SCR_BaseTask framework!");
		
		SCR_PDATaskManagerComponent mgr = SCR_PDATaskManagerComponent.GetInstance();
		if (mgr)
		{
			mgr.AddPlayerRuBalance(m_iRewardRU);
		}
	}
}

class SCR_PDATaskManagerComponentClass : ScriptComponentClass
{
}

class SCR_PDATaskManagerComponent : ScriptComponent
{
	protected static SCR_PDATaskManagerComponent s_Instance;
	protected int m_iPlayerRU = 0;

	static SCR_PDATaskManagerComponent GetInstance()
	{
		return s_Instance;
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		s_Instance = this;
		
		// Call to request a dynamic mission generation after server spins up
		GetGame().GetCallqueue().CallLater(RequestDynamicMission, 5000, false);
	}

	// Native hook to query the Global Mission Manager and map a UI marker
	void RequestDynamicMission()
	{
		SCR_GammaMissionManager missionMgr = SCR_GammaMissionManager.GetInstance();
		if (!missionMgr) 
		{
			Print("Server Error: No Gamma Mission Manager found in world! Cannot spawn tasks.");
			return;
		}

		// Pull the player's current coordinate as the origin for the radial mission spawn
		IEntity localPlayer = GetGame().GetPlayerManager().GetPlayerControlledEntity(GetGame().GetPlayerController().GetPlayerId());
		if (!localPlayer) return;

		vector pOrigin = localPlayer.GetOrigin();
		
		// Generate the physical map target! (Replaced Mutant Hunt with Faction Assault)
		missionMgr.CreateDynamicMission(pOrigin, EStalkerTaskType.FACTION_ASSAULT);
		
		Print("Client PDA: Dynamic Mission created and target spawned coordinates synced to map!");
		
		// In a real Enfusion hook, creating the vanilla task automatically plots the HUD mapping.
		// SCR_MapMarkerManagerComponent mapManager = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		
		SCR_PDA_UI ui = SCR_PDA_UI.GetInstance();
		if (ui) ui.RefreshTaskList();
	}
	
	void AddPlayerRuBalance(int amount)
	{
		m_iPlayerRU += amount;
		Print("Client PDA: Balance Updated. Total RU: " + m_iPlayerRU);
	}
	
	int GetPlayerRuBalance()
	{
		return m_iPlayerRU;
	}
}
