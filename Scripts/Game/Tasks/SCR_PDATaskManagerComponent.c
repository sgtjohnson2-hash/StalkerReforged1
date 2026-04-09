enum EStalkerTaskType
{
	ASSASSINATION,
	FETCH,
	MUTANT_HUNT
}

// Inherit from Arma Reforger's vanilla SCR_BaseTask so it appears natively on the HUD/Map
class SCR_StalkerBaseTask : SCR_BaseTask
{
	[Attribute("1000", desc: "Reward in RU upon completion")]
	protected int m_iRewardRU;
	
	[Attribute("0", desc: "Task type mapping")]
	protected EStalkerTaskType m_eTaskType;

	int GetRewardRU()
	{
		return m_iRewardRU;
	}
	
	EStalkerTaskType GetStalkerTaskType()
	{
		return m_eTaskType;
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
		
		// Generates a mock task via the vanilla task manager on init
		GetGame().GetCallqueue().CallLater(GenerateProceduralTask, 2000, false);
	}

	void GenerateProceduralTask()
	{
		// Native Reforger API requires interacting with SCR_BaseTaskManager
		SCR_BaseTaskManager taskManager = GetTaskManager();
		if (!taskManager) 
		{
			Print("Client PDA Error: Vanilla SCR_BaseTaskManager not found in world!");
			return;
		}

		// In actual Enfusion, we would spawn the Prefab representing the SCR_StalkerBaseTask.
		// For script representation, we print finding the task manager successfully.
		Print("Client PDA: Integrated with Vanilla SCR_BaseTaskManager to generate [Hunt Flesh] task.");
		
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
