class SCR_AnomalyComponentClass : ScriptComponentClass
{
}

class SCR_AnomalyComponent : ScriptComponent
{
	[Attribute("10", desc: "Damage per tick inside the anomaly")]
	protected float m_fDamagePerTick;

	[Attribute("1", desc: "Radius of the anomaly trigger area")]
	protected float m_fTriggerRadius;

	protected IEntity m_Owner;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		m_Owner = owner;
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
	}

	override void EOnFrame(IEntity owner, float timeSlice)
	{
		// Spatial queries and tick damage execution must run exclusively on the server (authority)
		if (!Replication.IsServer()) return;
		
		GetGame().GetWorld().QueryEntitiesBySphere(owner.GetOrigin(), m_fTriggerRadius, QueryDamageEntities, null, EQueryEntitiesFlags.ALL);
	}

	bool QueryDamageEntities(IEntity ent)
	{
		if (!ent) return true;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(ent.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageManager && damageManager.GetHealth() > 0)
		{
			HitZone defaultHZ = damageManager.GetDefaultHitZone();
			if (defaultHZ)
			{
				// Apply tick-rate scaled damage to character default hitzone
				defaultHZ.Damage(m_fDamagePerTick * 0.1);
				OnEntityDamaged(ent);
			}
		}

		return true;
	}

	protected void OnEntityDamaged(IEntity ent)
	{
		// Derived classes override this to apply visual PFX, play shock sounds, cast fire etc.
	}
}
