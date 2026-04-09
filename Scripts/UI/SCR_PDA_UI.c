#ifndef PDA_UI_H
#define PDA_UI_H

class SCR_PDA_UI : ScriptComponent
{
	protected static SCR_PDA_UI s_Instance;
	
	protected bool m_bIsOpen;

	static SCR_PDA_UI GetInstance()
	{
		return s_Instance;
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_bIsOpen = false;
		
		// Only attach to local player context if running on MP
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
		// Pseudo Enfusion call: GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.STALKER_PDA);
		Print("Client UI: PDA OPENED.");
		
		RefreshTaskList();
	}

	protected void ClosePDA()
	{
		m_bIsOpen = false;
		// GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.STALKER_PDA);
		Print("Client UI: PDA CLOSED.");
	}

	// Receives messages from the Network Manager
	void ReceiveMessage(string sender, string message)
	{
		// Native GUI widget population code goes here:
		// TextWidget textComp = TextWidget.Cast(m_wRoot.FindAnyWidget("MessageText"));
		// textComp.SetText(sender + ": " + message);
		
		// Audio cue for new message
		// PlaySound("pda_notification");
		
		Print("PDA UI Event -> [NEW MESSAGE] From: " + sender + " | Body: " + message);
	}

	void RefreshTaskList()
	{
		if (!m_bIsOpen) return;
		
		SCR_PDATaskManagerComponent taskMgr = SCR_PDATaskManagerComponent.Cast(GetOwner().FindComponent(SCR_PDATaskManagerComponent));
		if (!taskMgr) return;
		
		array<ref SCR_StalkerTask> activeTasks = taskMgr.GetActiveTasks();
		Print("PDA UI Event -> Refreshing Task Display. Total Tasks: " + activeTasks.Count());
		
		foreach (SCR_StalkerTask task : activeTasks)
		{
			string status = "[ IN PROGRESS ]";
			if (task.m_bIsCompleted) status = "[ COMPLETED ]";
			
			Print("  Task: " + task.m_sTaskName + " " + status + " - Reward: " + task.m_iRewardRU + " RU");
		}
	}
}

#endif
