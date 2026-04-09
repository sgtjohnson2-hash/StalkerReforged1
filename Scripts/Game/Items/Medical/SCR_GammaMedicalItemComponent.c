class SCR_GammaMedicalItemComponentClass : SCR_ConsumableItemComponentClass
{
}

class SCR_GammaMedicalItemComponent : SCR_ConsumableItemComponent
{
	[Attribute("10", desc: "Amount of health to restore over time")]
	protected float m_fHealAmount;

	[Attribute("5", desc: "Duration in seconds over which healing applies")]
	protected float m_fHealDuration;
	
	[Attribute("0", desc: "Amount of radiation to remove")]
	protected float m_fRadRemoval;

	protected IEntity m_OwnerCharacter;
	protected float m_fTimer;
	protected bool m_bIsHealing;
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_bIsHealing = false;
	}

	// Called when the player consumes/uses the item
	override void OnUse(IEntity user)
	{
		super.OnUse(user);
		m_OwnerCharacter = user;
		
		if (!m_bIsHealing)
		{
			Print("Used Medical Item: " + GetOwner().GetName() + ". Starting Heal Over Time...");
			StartHealOverTime();
		}
	}
	
	void StartHealOverTime()
	{
		m_bIsHealing = true;
		m_fTimer = 0;
		// Register a periodic update to apply healing over time
		GetGame().GetCallqueue().CallLater(ApplyHealTick, 1000, true);
	}
	
	void ApplyHealTick()
	{
		if (!m_OwnerCharacter || m_fTimer >= m_fHealDuration)
		{
			GetGame().GetCallqueue().Remove(ApplyHealTick);
			m_bIsHealing = false;
			Print("Finished Medical Item Heal Over Time.");
			return;
		}

		float tickHeal = m_fHealAmount / m_fHealDuration;
		
		// Assuming standard damage manager component is attached to the character
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_OwnerCharacter.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager)
		{
			// Heal the character (negative damage or specific Heal method)
			damageManager.AddHealth(tickHeal);
			Print("Healed " + tickHeal + " HP.");
			
			// Optional logic for removing radiation
			if (m_fRadRemoval > 0)
			{
				SCR_RadiationSicknessComponent radComponent = SCR_RadiationSicknessComponent.Cast(m_OwnerCharacter.FindComponent(SCR_RadiationSicknessComponent));
				if (radComponent)
				{
					float tickRad = m_fRadRemoval / m_fHealDuration;
					radComponent.ReduceRadiation(tickRad);
				}
			}
		}

		m_fTimer += 1.0;
	}
}
