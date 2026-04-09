class SCR_StalkerVodkaComponentClass : SCR_GammaConsumableBaseComponentClass
{
}

class SCR_StalkerVodkaComponent : SCR_GammaConsumableBaseComponent
{
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_fHungerRestore = 0;
		m_fThirstRestore = 5;       // Mild fluid intake
		m_fSleepRestore = -30;      // Intoxication causes massive sleep drain
		m_fRadiationCleanse = 50;   // The true purpose of Vodka in the Zone
	}
}
