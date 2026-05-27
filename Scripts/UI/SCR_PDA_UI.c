#ifndef PDA_UI_H
#define PDA_UI_H

class SCR_PDA_UI : ScriptComponent
{
	protected bool m_bIsOpen;
	
	// UI Widget references
	protected Widget m_wRoot;
	protected TextWidget m_wMessageLog;
	protected Widget m_wMapTab;
	protected Widget m_wTasksTab;
	protected Widget m_wLogsTab;
	protected Widget m_wFactionsTab;
	
	[Attribute("Sound_PDA_Open", desc: "Sound event triggered when PDA is pulled out")]
	protected string m_sSoundOpen;
	
	[Attribute("Sound_PDA_Message", desc: "Sound event for incoming network updates")]
	protected string m_sSoundMessage;

	// Multiplayer-safe instance retrieval from local PlayerController
	static SCR_PDA_UI GetInstance()
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc) return null;
		return SCR_PDA_UI.Cast(pc.FindComponent(SCR_PDA_UI));
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_bIsOpen = false;
	}

	void TogglePDA()
	{
		if (m_bIsOpen) ClosePDA();
		else OpenPDA();
	}

	protected void OpenPDA()
	{
		m_bIsOpen = true;
		Print("Client UI: [PDA DRAWN] Played open sound.");
		SwitchToTab("Tasks");
	}

	protected void ClosePDA()
	{
		m_bIsOpen = false;
		Print("Client UI: [PDA STOWED]");
	}

	void SwitchToTab(string tabName)
	{
		Print("Client UI: PDA Switched to Tab -> " + tabName);
		
		if (tabName == "Tasks") RefreshTaskList();
		if (tabName == "Factions") RefreshFactionsBoard();
	}

	void UI_SelectFaction(string factionKey)
	{
		Print("Client UI: User clicked 'Join " + factionKey + "' on their PDA.");
		
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc) return;
		
		IEntity player = pc.GetControlledEntity();
		if (player)
		{
			SCR_PlayerFactionManagerComponent factionMgr = SCR_PlayerFactionManagerComponent.Cast(player.FindComponent(SCR_PlayerFactionManagerComponent));
			if (factionMgr)
			{
				factionMgr.JoinFaction(factionKey);
			}
			else
			{
				Print("Client Error: Player missing SCR_PlayerFactionManagerComponent.");
			}
		}
	}

	void ReceiveMessage(string sender, string message)
	{
		string formattedLog = "[" + sender + "]: " + message + "\n";
		Print("PDA UI Widget Updated -> " + formattedLog);
	}

	void RefreshTaskList()
	{
		if (!m_bIsOpen) return;
		
		SCR_BaseTaskManager taskManager = SCR_BaseTaskManager.Cast(GetGame().GetTaskManager());
		if (!taskManager) return;
		
		array<SCR_BaseTask> outTasks = new array<SCR_BaseTask>();
		taskManager.GetTasks(outTasks);
		
		Print("PDA [TASKS TAB] Active UI Refresh:");
		
		foreach (SCR_BaseTask vanillaTask : outTasks)
		{
			SCR_StalkerBaseTask stalkerTask = SCR_StalkerBaseTask.Cast(vanillaTask);
			if (stalkerTask)
			{
				string status = "[ IN PROGRESS ]";
				if (stalkerTask.GetTaskState() == SCR_TaskState.FINISHED) status = "[ COMPLETED ]";
				
				Print("  [PDA UI] " + stalkerTask.GetTitle() + " " + status + " - Reward: " + stalkerTask.GetRewardRU() + " RU");
			}
		}
	}
	
	void RefreshFactionsBoard()
	{
		Print("PDA [FACTIONS TAB] Accessing Global Leaderboard...");
		
		SCR_FactionWarManager warMgr = SCR_FactionWarManager.GetInstance();
		if (!warMgr)
		{
			Print("  [ERROR] Faction Network Offline.");
			return;
		}
		
		int ussrScore, usScore, fiaScore;
		warMgr.GetFactionControlStats(ussrScore, usScore, fiaScore);
		
		Print("=================================");
		Print("    ZONE TERRITORY LEADERBOARD   ");
		Print("=================================");
		Print("  MILITARY (Spetsnaz): " + ussrScore + " Bases Control");
		Print("  MERCENARIES (NATO) : " + usScore + " Bases Control");
		Print("  RENEGADES (Bandits): " + fiaScore + " Bases Control");
		Print("---------------------------------");
	}
}

#endif
