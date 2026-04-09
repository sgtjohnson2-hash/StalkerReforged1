class SCR_GammaConsumableBaseComponentClass : ScriptComponentClass
{
}

class SCR_GammaConsumableBaseComponent : ScriptComponent
{
	[Attribute("0", desc: "Amount of Hunger to restore")]
	protected float m_fHungerRestore;
	
	[Attribute("0", desc: "Amount of Thirst to restore (negative values dehydrate)")]
	protected float m_fThirstRestore;
	
	[Attribute("0", desc: "Amount of Sleep to restore (negative values cause exhaustion)")]
	protected float m_fSleepRestore;
	
	[Attribute("0", desc: "Amount of Radiation (mSv) to remove")]
	protected float m_fRadiationCleanse;

	// This is typically called by a UI action (like "Consume")
	void OnConsume(IEntity consumer)
	{
		// 1. Hook into Survival Manager
		SCR_SurvivalManagerComponent survivalMgr = SCR_SurvivalManagerComponent.Cast(consumer.FindComponent(SCR_SurvivalManagerComponent));
		if (survivalMgr)
		{
			if (m_fHungerRestore != 0) survivalMgr.ConsumeFood(m_fHungerRestore);
			if (m_fThirstRestore != 0) survivalMgr.ConsumeDrink(m_fThirstRestore);
			if (m_fSleepRestore != 0) survivalMgr.Rest(m_fSleepRestore);
		}
		else
		{
			Print("Warning: Consumable used on entity without a SurvivalManagerComponent.");
		}

		// 2. Hook into Radiation Manager if this item cleanses radiation
		if (m_fRadiationCleanse > 0)
		{
			SCR_RadiationSicknessComponent radMgr = SCR_RadiationSicknessComponent.Cast(consumer.FindComponent(SCR_RadiationSicknessComponent));
			if (radMgr)
			{
				radMgr.RemoveRadiation(m_fRadiationCleanse);
				Print("Client: Cleansed " + m_fRadiationCleanse + " mSv of radiation.");
			}
		}

		// Physical item gets destroyed inside inventory network after using
		// e.g. GetGame().GetInventoryManager().DeleteEntity(GetOwner());
		Print("Client: Item " + GetOwner().GetPrefabData().GetPrefabName() + " consumed.");
	}
}
