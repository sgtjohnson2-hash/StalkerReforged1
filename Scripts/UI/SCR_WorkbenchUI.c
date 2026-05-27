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
		Print("Client UI: WORKBENCH MENU OPENED. Awaiting player repair commands.");
	}

	protected void CloseMenu()
	{
		m_bIsOpen = false;
		Print("Client UI: WORKBENCH MENU CLOSED.");
	}

	// Triggered by UI button click on the client -> dispatches server RPC
	void OnRepairButtonPressed(IEntity targetWeapon, bool useAdvancedKit)
	{
		if (Replication.IsClient())
		{
			Rpc(RpcServer_RepairWeapon, targetWeapon, useAdvancedKit);
		}
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcServer_RepairWeapon(IEntity targetWeapon, bool useAdvancedKit)
	{
		PlayerController pc = PlayerController.Cast(GetOwner());
		if (!pc) return;
		
		IEntity character = pc.GetControlledEntity();
		if (!character) return;
		
		SCR_RepairSystemComponent repairSystem = SCR_RepairSystemComponent.Cast(character.FindComponent(SCR_RepairSystemComponent));
		if (repairSystem)
		{
			repairSystem.TryRepairWeapon(character, targetWeapon, useAdvancedKit);
		}
		else
		{
			Print("Server RP Error: No Repair System found on player character to execute repair.");
		}
	}
}

#endif
