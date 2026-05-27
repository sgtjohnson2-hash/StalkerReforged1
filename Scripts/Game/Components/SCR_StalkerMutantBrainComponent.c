enum EStalkerMutantType
{
	SNORK,
	BLOODSUCKER
}

class SCR_StalkerMutantBrainComponentClass : ScriptComponentClass
{
}

class SCR_StalkerMutantBrainComponent : ScriptComponent
{
	[Attribute("0", UIWidgets.ComboBox, "Mutant Type", enums: ParamEnumArray.FromEnum(EStalkerMutantType))]
	protected EStalkerMutantType m_eMutantType;

	[Attribute("15", desc: "Leap range for Snork or aggro scan range")]
	protected float m_fActionRange;

	protected IEntity m_Owner;
	protected IEntity m_Target = null;
	
	// State trackers
	protected bool m_bIsInvisible = false;
	protected bool m_bLeapCooldown = false;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Owner = owner;

		// AI target selection and behaviors run exclusively on the server (authority)
		if (Replication.IsServer())
		{
			GetGame().GetCallqueue().CallLater(ScanTargets, 1500, true);
			GetGame().GetCallqueue().CallLater(MutantBehaviorThink, 500, true);
		}
	}

	protected void ScanTargets()
	{
		if (!m_Owner || !Replication.IsServer()) return;

		m_Target = null;
		
		// Query nearby radius to find active characters
		vector center = m_Owner.GetOrigin();
		GetGame().GetWorld().QueryEntitiesBySphere(center, 50.0, CheckTargetEntity, null, EQueryEntitiesFlags.ALL);
	}

	protected bool CheckTargetEntity(IEntity ent)
	{
		if (!ent || ent == m_Owner) return true;

		// Target characters/players
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(ent.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager && damageManager.GetHealth() > 0)
		{
			// Mutants target players or opposing faction members
			m_Target = ent;
			return false; // Target found, stop query
		}
		return true;
	}

	protected void MutantBehaviorThink()
	{
		if (!m_Owner || !m_Target || !Replication.IsServer()) return;

		float dist = vector.Distance(m_Owner.GetOrigin(), m_Target.GetOrigin());

		if (m_eMutantType == EStalkerMutantType.SNORK)
		{
			ProcessSnorkBehavior(dist);
		}
		else if (m_eMutantType == EStalkerMutantType.BLOODSUCKER)
		{
			ProcessBloodsuckerBehavior(dist);
		}
	}

	// -------------------------------------------------------------
	// SNORK LEAP & CHARGE BEHAVIORS
	// -------------------------------------------------------------
	protected void ProcessSnorkBehavior(float dist)
	{
		if (dist <= m_fActionRange && !m_bLeapCooldown)
		{
			m_bLeapCooldown = true;
			
			Print("Server RP AI: SNORK charging leap velocity!");
			
			// Calculate jump direction vector
			vector dir = vector.Direction(m_Owner.GetOrigin(), m_Target.GetOrigin());
			dir.Normalize();
			
			// Add high leap arc
			vector leapVelocity;
			leapVelocity[0] = dir[0] * 12.0; // Horizontal force
			leapVelocity[1] = 6.0;           // Vertical jump force
			leapVelocity[2] = dir[2] * 12.0;

			Physics phys = m_Owner.GetPhysics();
			if (phys)
			{
				phys.SetActive(true);
				phys.SetVelocity(leapVelocity);
			}

			// Strike player mid-air after a brief offset delay
			GetGame().GetCallqueue().CallLater(SnorkMeleeStrike, 800, false);
			
			// Set 5 seconds leap cooldown
			GetGame().GetCallqueue().CallLater(ResetLeapCooldown, 5000, false);
		}
	}

	protected void SnorkMeleeStrike()
	{
		if (!m_Owner || !m_Target || !Replication.IsServer()) return;

		float dist = vector.Distance(m_Owner.GetOrigin(), m_Target.GetOrigin());
		if (dist <= 3.0) // Collided mid-air
		{
			SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
			if (damageManager && damageManager.GetHealth() > 0)
			{
				HitZone defaultHZ = damageManager.GetDefaultHitZone();
				if (defaultHZ)
				{
					defaultHZ.Damage(35.0); // Dealt heavy claw strike
					Print("Server RP AI: SNORK mid-air strike successful!");
				}
			}
		}
	}

	protected void ResetLeapCooldown()
	{
		m_bLeapCooldown = false;
	}

	// -------------------------------------------------------------
	// BLOODSUCKER INVISIBILITY & SIPHON BEHAVIORS
	// -------------------------------------------------------------
	protected void ProcessBloodsuckerBehavior(float dist)
	{
		// 1. Cloak/Uncloak dynamic state machine
		if (dist > 10.0 && !m_bIsInvisible)
		{
			m_bIsInvisible = true;
			Rpc(RpcDo_ClientSetVisibility, false);
			Print("Server RP AI: BLOODSUCKER cloaked.");
		}
		else if (dist <= 10.0 && m_bIsInvisible)
		{
			m_bIsInvisible = false;
			Rpc(RpcDo_ClientSetVisibility, true);
			Print("Server RP AI: BLOODSUCKER uncloaked!");
		}

		// 2. Vampire life siphon melee
		if (dist <= 3.0)
		{
			SCR_CharacterDamageManagerComponent targetDmg = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
			SCR_CharacterDamageManagerComponent myDmg = SCR_CharacterDamageManagerComponent.Cast(m_Owner.FindComponent(SCR_CharacterDamageManagerComponent));

			if (targetDmg && myDmg && targetDmg.GetHealth() > 0)
			{
				HitZone playerHZ = targetDmg.GetDefaultHitZone();
				HitZone myHZ = myDmg.GetDefaultHitZone();

				if (playerHZ && myHZ)
				{
					playerHZ.Damage(15.0); // Siphoned 15 HP
					myHZ.Heal(15.0);       // Siphoned to own health

					Print("Server RP AI: BLOODSUCKER executed vampire life siphon!");
				}
			}
		}
	}

	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_ClientSetVisibility(bool state)
	{
		if (!m_Owner) return;

		if (state)
		{
			// Unhide the entity mesh on clients
			m_Owner.SetFlags(EntityFlags.VISIBLE);
			m_Owner.ClearFlags(EntityFlags.NO_LINK);
		}
		else
		{
			// Hide the entity mesh on clients to simulate invisibility
			m_Owner.ClearFlags(EntityFlags.VISIBLE);
			m_Owner.SetFlags(EntityFlags.NO_LINK);
		}
	}
}
