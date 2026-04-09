class SCR_TaskTargetComponentClass : ScriptComponentClass
{
}

class SCR_TaskTargetComponent : ScriptComponent
{
	protected SCR_StalkerBaseTask m_AssignedTask;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// Hook into the native Reforger damage manager to listen for death
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageMgr)
		{
			damageMgr.GetOnDestroyed().Insert(OnTargetDestroyed);
		}
	}

	void AssignTask(SCR_StalkerBaseTask task)
	{
		m_AssignedTask = task;
		Print("Server: Entity Target Assigned to Task: " + task.GetTitle());
	}

	protected void OnTargetDestroyed()
	{
		if (m_AssignedTask && m_AssignedTask.GetTaskState() == SCR_TaskState.ASSIGNED)
		{
			// Target eliminated! Finish the mission natively.
			m_AssignedTask.Finish(true);
			Print("Server: Target Eliminatated. Resolving Stalker Task!");
		}
	}

	void ~SCR_TaskTargetComponent()
	{
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(GetOwner().FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageMgr)
		{
			damageMgr.GetOnDestroyed().Remove(OnTargetDestroyed);
		}
	}
}
