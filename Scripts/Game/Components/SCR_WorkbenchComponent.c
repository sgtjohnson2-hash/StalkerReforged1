class SCR_OpenWorkbenchAction : ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		// Look for the repair system on the player or global game mode to open UI
		Print("Client: Player interacted with Workbench.");
		
		// Typically we would open an Enfusion UI Layout here
		// GetGame().GetMenuManager().OpenMenu(MenuID.WORKBENCH_UI);
		
		// For our script architecture, we'll notify the UI scripting component natively
		SCR_WorkbenchUI uiSys = SCR_WorkbenchUI.Cast(pUserEntity.FindComponent(SCR_WorkbenchUI));
		if (uiSys)
		{
			uiSys.ToggleWorkbenchMenu();
		}
		else
		{
			Print("Client Error: Player missing SCR_WorkbenchUI component to open menu!");
		}
	}

	override bool CanBePerformedScript(IEntity user)
	{
		return true;
	}

	override bool GetActionNameScript(out string outName)
	{
		outName = "Use Workbench (Crafting/Repair)";
		return true;
	}

	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
}

class SCR_WorkbenchComponentClass : ScriptComponentClass
{
}

class SCR_WorkbenchComponent : ScriptComponent
{
	// Helper component attached to physical vice/tables to give context
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		Print("Server: Workbench initialized at " + owner.GetOrigin());
	}
}
