class SCR_RadiationSicknessComponentClass : ScriptComponentClass
{
}

class SCR_RadiationSicknessComponent : ScriptComponent
{
	[Attribute("0.1", desc: "How much health to drain per 1.0 of radiation per second")]
	protected float m_fRadiationDamageMultiplier;

	[RplProp()]
	protected float m_fAccumulatedRadiation;
	
	protected IEntity m_Owner;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Owner = owner;
		m_fAccumulatedRadiation = 0.0;
		
		// Run radiation damage ticks exclusively on the server (authority)
		if (Replication.IsServer())
		{
			GetGame().GetCallqueue().CallLater(ProcessRadiationTick, 1000, true);
		}
	}

	void AddRadiation(float amount)
	{
		if (!Replication.IsServer()) return;
		
		m_fAccumulatedRadiation += amount;
		Replication.BumpMe();
		Print("Server: Character absorbed " + amount + " mSv of radiation. Total: " + m_fAccumulatedRadiation);
	}

	void ReduceRadiation(float amount)
	{
		if (!Replication.IsServer()) return;
		
		m_fAccumulatedRadiation -= amount;
		if (m_fAccumulatedRadiation < 0) 
			m_fAccumulatedRadiation = 0;
			
		Replication.BumpMe();
		Print("Server: Character removed " + amount + " mSv of radiation. Total: " + m_fAccumulatedRadiation);
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

		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_Owner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager && damageManager.GetHealth() > 0)
		{
			HitZone defaultHZ = damageManager.GetDefaultHitZone();
			if (defaultHZ)
			{
				defaultHZ.Damage(damageTick);
				Print("Server: Character took " + damageTick + " radiation damage. Health remaining: " + defaultHZ.GetHealth());
			}
		}
	}
}
