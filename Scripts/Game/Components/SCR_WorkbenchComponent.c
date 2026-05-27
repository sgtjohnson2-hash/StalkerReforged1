class SCR_OpenWorkbenchAction : ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		Print("Client: Player interacted with Workbench.");
		
		// Retrieve PlayerController associated with the interacting character
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(playerId);
		if (pc)
		{
			SCR_WorkbenchUI uiSys = SCR_WorkbenchUI.Cast(pc.FindComponent(SCR_WorkbenchUI));
			if (uiSys)
			{
				uiSys.ToggleWorkbenchMenu();
			}
			else
			{
				Print("Client Error: PlayerController missing SCR_WorkbenchUI component to open menu!");
			}
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
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		Print("Server: Workbench initialized at " + owner.GetOrigin());
	}
}
