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

	[RplProp()]
	protected float m_fCurrentDurability;
	
	protected IEntity m_OwnerWeapon;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_OwnerWeapon = owner;
		m_fCurrentDurability = m_fMaxDurability;
	}

	// Called on the server when the weapon is fired
	void OnWeaponFired()
	{
		if (!Replication.IsServer()) return;

		m_fCurrentDurability -= m_fDegradationPerShot;
		if (m_fCurrentDurability < 0) m_fCurrentDurability = 0;
		
		Replication.BumpMe();
		Print("Server: Weapon fired. Durability now: " + m_fCurrentDurability);

		if (m_fCurrentDurability <= m_fJamThreshold)
		{
			float jamChance = (m_fJamThreshold - m_fCurrentDurability) / m_fJamThreshold; 
			if (Math.RandomFloat01() < jamChance)
			{
				TriggerJam();
			}
		}
	}

	protected void TriggerJam()
	{
		Print("Server: WEAPON JAMMED!");
		
		// Replicated warning notification to the client
		SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
		if (network)
		{
			network.SendNetworkMessage("Your weapon has jammed! Clear the chamber immediately!", "Weapon Warning");
		}
	}
	
	float GetDurability()
	{
		return m_fCurrentDurability;
	}
	
	void RepairWeapon(float amount)
	{
		if (!Replication.IsServer()) return;
		
		m_fCurrentDurability += amount;
		if (m_fCurrentDurability > m_fMaxDurability) m_fCurrentDurability = m_fMaxDurability;
		
		Replication.BumpMe();
		Print("Server: Weapon repaired. Durability now: " + m_fCurrentDurability);
	}
}
