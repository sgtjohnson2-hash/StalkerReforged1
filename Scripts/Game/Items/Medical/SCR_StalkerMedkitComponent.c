class SCR_StalkerMedkitComponentClass : SCR_GammaMedicalItemComponentClass
{
}

class SCR_StalkerMedkitComponent : SCR_GammaMedicalItemComponent
{
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// Configure Gamma specific Medkit values
		// e.g., AI-2 heals 30 HP over 6 seconds and removes some radiation
		m_fHealAmount = 30.0;
		m_fHealDuration = 6.0;
		m_fRadRemoval = 15.0; 
	}
}
