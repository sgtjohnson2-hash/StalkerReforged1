#ifndef WORKBENCH_UI_H
#define WORKBENCH_UI_H

class SCR_WorkbenchUI : ScriptComponent
{
	protected bool m_bIsOpen;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_bIsOpen = false;
	}

	void ToggleWorkbenchMenu()
	{
		if (m_bIsOpen)
		{
			CloseMenu();
		}
		else
		{
			OpenMenu();
		}
	}

	protected void OpenMenu()
	{
		m_bIsOpen = true;
		
		// In Enfusion, this opens an actual .layout file
		// GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.WORKBENCH_GUI);
		
		Print("Client UI: WORKBENCH MENU OPENED. Awaiting player repair commands.");
	}

	protected void CloseMenu()
	{
		m_bIsOpen = false;
		
		// GetGame().GetMenuManager().CloseMenuByPreset(ChimeraMenuPreset.WORKBENCH_GUI);
		Print("Client UI: WORKBENCH MENU CLOSED.");
	}

	// This function would be hooked to a UI button click event inside the .layout
	void OnRepairButtonPressed(IEntity targetWeapon, bool useAdvancedKit)
	{
		SCR_RepairSystemComponent repairSystem = SCR_RepairSystemComponent.Cast(GetOwner().FindComponent(SCR_RepairSystemComponent));
		if (repairSystem)
		{
			repairSystem.TryRepairWeapon(GetOwner(), targetWeapon, useAdvancedKit);
		}
		else
		{
			Print("Client UI Error: No Repair System found on player bridging the UI.");
		}
	}
}

#endif
