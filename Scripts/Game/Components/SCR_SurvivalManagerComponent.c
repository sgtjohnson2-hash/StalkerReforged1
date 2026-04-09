class SCR_SurvivalManagerComponentClass : ScriptComponentClass
{
}

class SCR_SurvivalManagerComponent : ScriptComponent
{
	[Attribute("100", desc: "Maximum Hunger")]
	protected float m_fMaxHunger;

	[Attribute("100", desc: "Maximum Thirst")]
	protected float m_fMaxThirst;
	
	[Attribute("100", desc: "Maximum Sleep")]
	protected float m_fMaxSleep;

	[Attribute("0.05", desc: "Hunger drain per second")]
	protected float m_fHungerDrainRate;

	[Attribute("0.1", desc: "Thirst drain per second")]
	protected float m_fThirstDrainRate;
	
	[Attribute("0.02", desc: "Sleep drain per second")]
	protected float m_fSleepDrainRate;

	protected float m_fCurrentHunger;
	protected float m_fCurrentThirst;
	protected float m_fCurrentSleep;

	protected IEntity m_Owner;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Owner = owner;
		
		m_fCurrentHunger = m_fMaxHunger;
		m_fCurrentThirst = m_fMaxThirst;
		m_fCurrentSleep = m_fMaxSleep;
		
		// Run survival tick every second
		GetGame().GetCallqueue().CallLater(ProcessSurvivalTick, 1000, true);
	}

	void ConsumeFood(float amount)
	{
		m_fCurrentHunger += amount;
		if (m_fCurrentHunger > m_fMaxHunger) m_fCurrentHunger = m_fMaxHunger;
		Print("Client: Consumed Food. Hunger: " + m_fCurrentHunger);
	}

	void ConsumeDrink(float amount)
	{
		m_fCurrentThirst += amount;
		if (m_fCurrentThirst > m_fMaxThirst) m_fCurrentThirst = m_fMaxThirst;
		Print("Client: Consumed Drink. Thirst: " + m_fCurrentThirst);
	}
	
	void Rest(float amount)
	{
		m_fCurrentSleep += amount;
		if (m_fCurrentSleep > m_fMaxSleep) m_fCurrentSleep = m_fMaxSleep;
		Print("Client: Rested. Sleep: " + m_fCurrentSleep);
	}

	protected void ProcessSurvivalTick()
	{
		if (!m_Owner) return;

		m_fCurrentHunger -= m_fHungerDrainRate;
		m_fCurrentThirst -= m_fThirstDrainRate;
		m_fCurrentSleep -= m_fSleepDrainRate;

		// Make sure they don't drop below 0
		if (m_fCurrentHunger < 0) m_fCurrentHunger = 0;
		if (m_fCurrentThirst < 0) m_fCurrentThirst = 0;
		if (m_fCurrentSleep < 0) m_fCurrentSleep = 0;

		// Calculate survival penalties
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(m_Owner.FindComponent(DamageManagerComponent));
		if (damageManager && damageManager.GetHealth() > 0)
		{
			float damageTick = 0;
			
			// Depleted needs deal small damage ticks (like GAMMA starvation/dehydration)
			if (m_fCurrentHunger <= 0) damageTick += 0.5;
			if (m_fCurrentThirst <= 0) damageTick += 1.0;
			
			// Sleep deprivation might cause stamina drain or screen blur, but minimal damage
			if (m_fCurrentSleep <= 0) damageTick += 0.1;

			if (damageTick > 0)
			{
				float newHealth = damageManager.GetHealth() - damageTick;
				if (newHealth < 0) newHealth = 0;
				Print("Client: Starving/Dehydrated. Took " + damageTick + " damage.");
			}
		}
	}
}
