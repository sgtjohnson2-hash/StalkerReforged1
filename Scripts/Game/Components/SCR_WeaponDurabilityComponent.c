class SCR_WeaponDurabilityComponentClass : ScriptComponentClass
{
}

class SCR_WeaponDurabilityComponent : ScriptComponent
{
	[Attribute("100", desc: "Maximum Weapon Durability")]
	protected float m_fMaxDurability;

	[Attribute("0.1", desc: "Durability loss per shot")]
	protected float m_fDegradationPerShot;
	
	[Attribute("30", desc: "Durability threshold below which jamming starts")]
	protected float m_fJamThreshold;

	protected float m_fCurrentDurability;
	protected IEntity m_OwnerWeapon;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_OwnerWeapon = owner;
		m_fCurrentDurability = m_fMaxDurability;
		
		// In Enfusion, we hook into the weapon component's firing event.
		// For this example, we assume there's a BaseWeaponComponent we can listen to.
		BaseWeaponComponent weaponComp = BaseWeaponComponent.Cast(owner.FindComponent(BaseWeaponComponent));
		if (weaponComp)
		{
			// Reforger specific event hooking (pseudo implementation based on common Enforce weapon APIs)
			// weaponComp.GetOnFire().Insert(OnWeaponFired);
		}
	}

	void OnWeaponFired()
	{
		m_fCurrentDurability -= m_fDegradationPerShot;
		if (m_fCurrentDurability < 0) m_fCurrentDurability = 0;
		
		Print("Weapon fired. Durability now: " + m_fCurrentDurability);

		if (m_fCurrentDurability <= m_fJamThreshold)
		{
			// Calculate jam chance based on how low it is
			float jamChance = (m_fJamThreshold - m_fCurrentDurability) / m_fJamThreshold; 
			// Example: at 0 durability, jam chance is 1.0 (100%). At 30, it is 0.0.
			
			if (Math.RandomFloat01() < jamChance)
			{
				Print("WEAPON JAMMED!");
				// In an actual Reforger implementation, we would force the weapon state to jammed
				// BaseWeaponComponent weaponComp = BaseWeaponComponent.Cast(m_OwnerWeapon.FindComponent(BaseWeaponComponent));
				// if (weaponComp) weaponComp.SetJammed(true);
			}
		}
	}
	
	float GetDurability()
	{
		return m_fCurrentDurability;
	}
	
	void RepairWeapon(float amount)
	{
		m_fCurrentDurability += amount;
		if (m_fCurrentDurability > m_fMaxDurability) m_fCurrentDurability = m_fMaxDurability;
		Print("Weapon repaired. Durability now: " + m_fCurrentDurability);
	}
}
