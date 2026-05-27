#ifndef STALKER_PDA_MENU_H
#define STALKER_PDA_MENU_H

class SCR_StalkerPDAMenuClass : ChimeraMenuBaseClass
{
}

class SCR_StalkerPDAMenu : ChimeraMenuBase
{
	protected Widget m_wRoot;
	protected TextWidget m_wTasksText;
	protected TextWidget m_wChatLogsText;
	protected TextWidget m_wFactionLeaderboardText;
	
	protected SCR_PDATaskManagerComponent m_TaskManager;

	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		m_wRoot = GetRootWidget();

		if (m_wRoot)
		{
			// Map layout widgets inside the .layout file
			m_wTasksText = TextWidget.Cast(m_wRoot.FindAnyWidget("TasksLogText"));
			m_wChatLogsText = TextWidget.Cast(m_wRoot.FindAnyWidget("ChatNetworkLogText"));
			m_wFactionLeaderboardText = TextWidget.Cast(m_wRoot.FindAnyWidget("FactionWarText"));
		}

		// Retrieve local PlayerController's task manager
		PlayerController pc = GetGame().GetPlayerController();
		if (pc)
		{
			m_TaskManager = SCR_PDATaskManagerComponent.Cast(pc.FindComponent(SCR_PDATaskManagerComponent));
		}

		// Default open tab updates
		RefreshTasksTab();
		RefreshChatTab();
		RefreshFactionsTab();
	}

	// Refreshes the active tasks list widget with server-replicated task data
	void RefreshTasksTab()
	{
		if (!m_wTasksText) return;

		SCR_BaseTaskManager taskManager = SCR_BaseTaskManager.Cast(GetGame().GetTaskManager());
		if (!taskManager)
		{
			m_wTasksText.SetText("Task Manager Offline.");
			return;
		}

		array<SCR_BaseTask> tasks = new array<SCR_BaseTask>();
		taskManager.GetTasks(tasks);

		string tasksDisplay = "=== ACTIVE ASSIGNMENTS ===\n\n";
		int count = 0;

		foreach (SCR_BaseTask task : tasks)
		{
			SCR_StalkerBaseTask stalkerTask = SCR_StalkerBaseTask.Cast(task);
			if (stalkerTask)
			{
				string status = "[ IN PROGRESS ]";
				if (stalkerTask.GetTaskState() == SCR_TaskState.FINISHED)
				{
					status = "[ COMPLETED ]";
				}

				tasksDisplay += "- " + stalkerTask.GetTitle() + "\n";
				tasksDisplay += "  Status: " + status + "\n";
				tasksDisplay += "  Reward: " + stalkerTask.GetRewardRU() + " RU\n\n";
				count++;
			}
		}

		if (count == 0)
		{
			tasksDisplay += "No active tasks in your PDA database. Visit a faction base to request contracts.";
		}

		m_wTasksText.SetText(tasksDisplay);
	}

	// Refreshes the local PDA logs tab
	void RefreshChatTab()
	{
		if (!m_wChatLogsText) return;

		// PDA local logs can display network notifications
		m_wChatLogsText.SetText("=== PDA NETWORK CHAT ===\n\n[System]: Listening to ambient frequency...\n");
	}

	// Refreshes the territory counts from the Faction War Manager
	void RefreshFactionsTab()
	{
		if (!m_wFactionLeaderboardText) return;

		SCR_FactionWarManager warMgr = SCR_FactionWarManager.GetInstance();
		if (!warMgr)
		{
			m_wFactionLeaderboardText.SetText("Faction intelligence offline.");
			return;
		}

		int ussrScore, usScore, fiaScore;
		warMgr.GetFactionControlStats(ussrScore, usScore, fiaScore);

		string statsDisplay = "=== TERRITORY CONTROL BOARD ===\n\n";
		statsDisplay += "  MILITARY SPETSNAZ (USSR) : " + ussrScore + " Sectors Secured\n";
		statsDisplay += "  NATO MERCENARIES (US)    : " + usScore + " Sectors Secured\n";
		statsDisplay += "  RENEGADE BANDITS (FIA)   : " + fiaScore + " Sectors Secured\n\n";
		statsDisplay += "===============================\n";
		statsDisplay += "Identification Tags: Wiped and encrypted.";

		m_wFactionLeaderboardText.SetText(statsDisplay);
	}

	// UI Action: Switch to Tasks tab
	void UI_OnTasksTabClick()
	{
		RefreshTasksTab();
		if (m_wTasksText) m_wTasksText.SetVisible(true);
		if (m_wChatLogsText) m_wChatLogsText.SetVisible(false);
		if (m_wFactionLeaderboardText) m_wFactionLeaderboardText.SetVisible(false);
	}

	// UI Action: Switch to Chat/Messages tab
	void UI_OnChatTabClick()
	{
		RefreshChatTab();
		if (m_wTasksText) m_wTasksText.SetVisible(false);
		if (m_wChatLogsText) m_wChatLogsText.SetVisible(true);
		if (m_wFactionLeaderboardText) m_wFactionLeaderboardText.SetVisible(false);
	}

	// UI Action: Switch to Factions tab
	void UI_OnFactionsTabClick()
	{
		RefreshFactionsTab();
		if (m_wTasksText) m_wTasksText.SetVisible(false);
		if (m_wChatLogsText) m_wChatLogsText.SetVisible(false);
		if (m_wFactionLeaderboardText) m_wFactionLeaderboardText.SetVisible(true);
	}

	// Button hook: Click to join USSR faction
	void UI_OnJoinUSSRClick()
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (pc)
		{
			IEntity player = pc.GetControlledEntity();
			if (player)
			{
				SCR_PlayerFactionManagerComponent factionMgr = SCR_PlayerFactionManagerComponent.Cast(player.FindComponent(SCR_PlayerFactionManagerComponent));
				if (factionMgr)
				{
					factionMgr.JoinFaction("USSR");
					GetGame().GetCallqueue().CallLater(RefreshFactionsTab, 200, false);
				}
			}
		}
	}

	// Button hook: Click to join US faction
	void UI_OnJoinUSClick()
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (pc)
		{
			IEntity player = pc.GetControlledEntity();
			if (player)
			{
				SCR_PlayerFactionManagerComponent factionMgr = SCR_PlayerFactionManagerComponent.Cast(player.FindComponent(SCR_PlayerFactionManagerComponent));
				if (factionMgr)
				{
					factionMgr.JoinFaction("US");
					GetGame().GetCallqueue().CallLater(RefreshFactionsTab, 200, false);
				}
			}
		}
	}

	// Button hook: Click to join FIA faction
	void UI_OnJoinFIAClick()
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (pc)
		{
			IEntity player = pc.GetControlledEntity();
			if (player)
			{
				SCR_PlayerFactionManagerComponent factionMgr = SCR_PlayerFactionManagerComponent.Cast(player.FindComponent(SCR_PlayerFactionManagerComponent));
				if (factionMgr)
				{
					factionMgr.JoinFaction("FIA");
					GetGame().GetCallqueue().CallLater(RefreshFactionsTab, 200, false);
				}
			}
		}
	}
}

#endif
