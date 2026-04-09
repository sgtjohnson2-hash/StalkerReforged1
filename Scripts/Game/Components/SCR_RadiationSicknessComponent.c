class SCR_RadiationSicknessComponentClass : ScriptComponentClass
{
}

class SCR_RadiationSicknessComponent : ScriptComponent
{
	[Attribute("0.1", desc: "How much health to drain per 1.0 of radiation per second")]
	protected float m_fRadiationDamageMultiplier;

	protected float m_fAccumulatedRadiation;
	protected IEntity m_Owner;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Owner = owner;
		m_fAccumulatedRadiation = 0.0;
		
		// Start checking radiation effects every second
		GetGame().GetCallqueue().CallLater(ProcessRadiationTick, 1000, true);
	}

	void AddRadiation(float amount)
	{
		m_fAccumulatedRadiation += amount;
		Print("Client: Absorbed " + amount + " mSv of radiation. Total: " + m_fAccumulatedRadiation);
	}

	void ReduceRadiation(float amount)
	{
		m_fAccumulatedRadiation -= amount;
		if (m_fAccumulatedRadiation < 0) 
			m_fAccumulatedRadiation = 0;
			
		Print("Client: Removed " + amount + " mSv of radiation. Total: " + m_fAccumulatedRadiation);
	}

	float GetRadiationTotal()
	{
		return m_fAccumulatedRadiation;
	}

	protected void ProcessRadiationTick()
	{
		if (m_fAccumulatedRadiation <= 0 || !m_Owner)
			return;

		// Calculate damage tick: linear relation to current radiation
		float damageTick = m_fAccumulatedRadiation * m_fRadiationDamageMultiplier;

		DamageManagerComponent damageManager = DamageManagerComponent.Cast(m_Owner.FindComponent(DamageManagerComponent));
		if (damageManager && damageManager.GetHealth() > 0)
		{
			// Reforger uses arbitrary hitzones, but we'll deal raw health script damage.
			// Since Enfusion doesn't universally use `SetHealth`, we simulate health drain:
			float currentHealth = damageManager.GetHealth();
			float newHealth = currentHealth - damageTick;
			
			if (newHealth < 0) newHealth = 0;
			
			// This is pseudo-code for dealing arbitrary damage if setting health directly isn't exposed perfectly
			// damageManager.SetHealth(newHealth);
			Print("Client: Took " + damageTick + " radiation damage. Health: " + newHealth);
			
			// Play dosimeter tick sound if radiation is high
			// PlaySound("dosimeter_tick_high");
		}
	}
}
