class SCR_StalkerSausageComponentClass : SCR_GammaConsumableBaseComponentClass
{
}

class SCR_StalkerSausageComponent : SCR_GammaConsumableBaseComponent
{
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_fHungerRestore = 15;  // Low nutrition
		m_fThirstRestore = -20; // Exceptionally salty, heavy dehydration
		m_fSleepRestore = 0;
		m_fRadiationCleanse = 0;
	}
}
