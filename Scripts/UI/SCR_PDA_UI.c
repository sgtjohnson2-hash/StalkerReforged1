#ifndef PDA_UI_H
#define PDA_UI_H

class SCR_PDA_UI : ScriptComponent
{
	protected static SCR_PDA_UI s_Instance;
	
	protected bool m_bIsOpen;
	
	// UI Widget references that would be mapped in the .layout file
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

	static SCR_PDA_UI GetInstance()
	{
		return s_Instance;
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_bIsOpen = false;
		s_Instance = this;
	}

	void TogglePDA()
	{
		if (m_bIsOpen) ClosePDA();
		else OpenPDA();
	}

	protected void OpenPDA()
	{
		m_bIsOpen = true;
		
		// In a real Enfusion UI, we'd initialize the layout here and assign m_wRoot
		// m_wRoot = GetGame().GetWorkspace().CreateWidgets("{PATH_TO_PDA_LAYOUT.layout}");
		// m_wMessageLog = TextWidget.Cast(m_wRoot.FindAnyWidget("NetworkLogText"));
		
		// S.T.A.L.K.E.R. functionality: Play the iconic PDA draw sound
		// PlaySound(m_sSoundOpen);
		Print("Client UI: [PDA DRAWN] Played sound 'pda_draw.ogg'");
		
		// Default to Tasks tab on open like GAMMA
		SwitchToTab("Tasks");
	}

	protected void ClosePDA()
	{
		m_bIsOpen = false;
		
		// PlaySound("Sound_PDA_Close");
		Print("Client UI: [PDA STOWED]");
		
		// if (m_wRoot) m_wRoot.RemoveFromHierarchy();
	}

	// Handles switching between Map, Tasks, and Chat Network exactly like S.T.A.L.K.E.R.
	void SwitchToTab(string tabName)
	{
		Print("Client UI: PDA Switched to Tab -> " + tabName);
		
		// Pseudo-logic to hide other tabs and show active one
		/*
		m_wMapTab.SetVisible(tabName == "Map");
		m_wTasksTab.SetVisible(tabName == "Tasks");
		m_wLogsTab.SetVisible(tabName == "Chat");
		m_wFactionsTab.SetVisible(tabName == "Factions");
		*/
		
		if (tabName == "Tasks") RefreshTaskList();
		if (tabName == "Factions") RefreshFactionsBoard();
	}

	// This function simulates the clicking of an HTML-style widget button on the Factions Tab
	void UI_SelectFaction(string factionKey)
	{
		Print("Client UI: User clicked 'Join " + factionKey + "' on their PDA.");
		
		// In Reforger, UI components can fetch the controlled player entity:
		// IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		// For our structural script, we pseudo-reference the owner.
		IEntity player = GetOwner(); 
		
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

	// Receives messages from the Network Manager (The "Chat" Tab)
	void ReceiveMessage(string sender, string message)
	{
		// S.T.A.L.K.E.R. functionality: Play the iconic PDA message beep
		// PlaySound(m_sSoundMessage);
		
		string formattedLog = "[" + sender + "]: " + message + "\n";
		
		// Append to the visual chat widget log
		// if (m_wMessageLog) m_wMessageLog.SetText(m_wMessageLog.GetText() + formattedLog);
		
		Print("PDA UI Widget Updated -> " + formattedLog);
	}

	void RefreshTaskList()
	{
		if (!m_bIsOpen) return;
		
		// Integrate with vanilla Reforger Task Manager
		SCR_BaseTaskManager taskManager = GetTaskManager();
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
		
		// Print the live pseudo-UI block
		Print("=================================");
		Print("    ZONE TERRITORY LEADERBOARD   ");
		Print("=================================");
		Print("  MILITARY (Spetsnaz): " + ussrScore + " Bases Control");
		Print("  MERCENARIES (NATO) : " + usScore + " Bases Control");
		Print("  RENEGADES (Bandits): " + fiaScore + " Bases Control");
		Print("---------------------------------");
		Print(" RECRUITMENT TERMINALS AVAILABLE ");
		Print(" > [JOIN USSR] ");
		Print(" > [JOIN US] ");
		Print(" > [JOIN FIA] ");
		Print(" > [SEVER CONTRACT (Loner)] ");
		Print("=================================");
	}
}

#endif
