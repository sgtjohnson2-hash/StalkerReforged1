class SCR_StalkerBeansComponentClass : SCR_GammaConsumableBaseComponentClass
{
}

class SCR_StalkerBeansComponent : SCR_GammaConsumableBaseComponent
{
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_fHungerRestore = 50;  // High nutrition
		m_fThirstRestore = -5;  // Slightly salty, dehydrates a bit
		m_fSleepRestore = 0;
		m_fRadiationCleanse = 0;
	}
}
