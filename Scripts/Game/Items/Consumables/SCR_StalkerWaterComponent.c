class SCR_StalkerWaterComponentClass : SCR_GammaConsumableBaseComponentClass
{
}

class SCR_StalkerWaterComponent : SCR_GammaConsumableBaseComponent
{
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_fHungerRestore = 0;
		m_fThirstRestore = 40;  // High hydration
		m_fSleepRestore = 5;    // Minor stamina/sleep boost
		m_fRadiationCleanse = 0;
	}
}
