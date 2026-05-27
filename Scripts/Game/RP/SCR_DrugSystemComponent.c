class SCR_DrugSystemComponentClass : ScriptComponentClass
{
}

class SCR_DrugSystemComponent : ScriptComponent
{
	[Attribute("100", desc: "Maximum toxicity threshold before overdose occurs")]
	protected float m_fMaxToxicity;

	[Attribute("2", desc: "Toxicity decay rate per second")]
	protected float m_fToxicityDecayRate;

	[RplProp()]
	protected float m_fCurrentToxicity = 0;

	// Active combat buffs tracking on the server
	protected bool m_bStaminaBuffActive = false;
	protected bool m_bHealBuffActive = false;
	protected float m_fBuffDurationRemaining = 0;

	protected IEntity m_Owner;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Owner = owner;

		// Process drug ticks and buff decays exclusively on server
		if (Replication.IsServer())
		{
			GetGame().GetCallqueue().CallLater(ProcessDrugTick, 1000, true);
		}
	}

	// Server-authoritative drug consumption callback
	void InjectDrug(float toxicityGain, float buffDuration, string buffType)
	{
		if (!Replication.IsServer()) return;

		m_fCurrentToxicity += toxicityGain;
		m_fBuffDurationRemaining = buffDuration;

		if (buffType == "Stamina")
		{
			m_bStaminaBuffActive = true;
			Print("Server: Player drug buff active -> STAMINA BOOST");
		}
		else if (buffType == "Healing")
		{
			m_bHealBuffActive = true;
			Print("Server: Player drug buff active -> HP REGENERATION");
		}

		Replication.BumpMe();

		// Check for lethal overdose immediately
		if (m_fCurrentToxicity >= m_fMaxToxicity)
		{
			TriggerOverdose();
		}
	}

	protected void ProcessDrugTick()
	{
		if (!m_Owner || !Replication.IsServer()) return;

		// 1. Decay Toxicity naturally
		if (m_fCurrentToxicity > 0)
		{
			m_fCurrentToxicity -= m_fToxicityDecayRate;
			if (m_fCurrentToxicity < 0) m_fCurrentToxicity = 0;
			Replication.BumpMe();
		}

		// 2. Process active combat buffs
		if (m_fBuffDurationRemaining > 0)
		{
			m_fBuffDurationRemaining -= 1.0;

			SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_Owner.FindComponent(SCR_CharacterDamageManagerComponent));
			if (damageManager && damageManager.GetHealth() > 0)
			{
				// Healing buff: Restores 3 HP per second
				if (m_bHealBuffActive)
				{
					HitZone defaultHZ = damageManager.GetDefaultHitZone();
					if (defaultHZ)
					{
						defaultHZ.Heal(3.0);
						Print("Server RP: Regenerating character health via drug active buff.");
					}
				}
			}

			if (m_fBuffDurationRemaining <= 0)
			{
				m_bStaminaBuffActive = false;
				m_bHealBuffActive = false;
				Print("Server RP: Active drug combat buffs expired.");
			}
		}

		// 3. Apply continuous toxicity damage if overdosed
		if (m_fCurrentToxicity >= m_fMaxToxicity * 0.8)
		{
			SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_Owner.FindComponent(SCR_CharacterDamageManagerComponent));
			if (damageManager && damageManager.GetHealth() > 0)
			{
				HitZone defaultHZ = damageManager.GetDefaultHitZone();
				if (defaultHZ)
				{
					// Toxicity poison deals 5 damage per tick
					defaultHZ.Damage(5.0);
					Print("Server RP: Character taking severe toxicity poison damage!");
				}
			}
		}
	}

	protected void TriggerOverdose()
	{
		Print("Server RP: PLAYER TOXICITY OVERDOSE! Instakill triggered.");
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_Owner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager)
		{
			HitZone defaultHZ = damageManager.GetDefaultHitZone();
			if (defaultHZ)
			{
				defaultHZ.Damage(100.0); // Dealt lethal damage
			}
		}
	}
	
	float GetToxicity() { return m_fCurrentToxicity; }
	bool IsStaminaBuffActive() { return m_bStaminaBuffActive; }
	bool IsHealBuffActive() { return m_bHealBuffActive; }
}
