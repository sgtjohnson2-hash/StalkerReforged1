class SCR_StalkerBandageComponentClass : SCR_GammaMedicalItemComponentClass
{
}

class SCR_StalkerBandageComponent : SCR_GammaMedicalItemComponent
{
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// Bandages stop bleeding quickly but heal very little
		m_fHealAmount = 5.0;
		m_fHealDuration = 2.0;
		m_fRadRemoval = 0.0;
	}

	override void OnUse(IEntity user)
	{
		super.OnUse(user);
		
		Print("Applying bandage...");
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(user.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager)
		{
			// In Reforger Damage Manager, we might need to remove blood loss status.
			// Depending on the exact methods available in Reforger's damage model.
			// We can stop generic DoT or specific bleeding handlers.
			
			// Pseudo-implementation to clear bleeding:
			// damageManager.SetBloodLoss(0);
			// For now, we print we are stopping bleeding:
			Print("Bandage applied: Bleeding stopped (simulated).");
		}
	}
}
