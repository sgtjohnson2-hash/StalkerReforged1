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
		// Spawn particle emitters, sounds here based on derived anomaly type
	}

	override void EOnFrame(IEntity owner, float timeSlice)
	{
		// Poll for damage in an radius, or use a trigger component if available.
		// For simplicity, a sphere query can be done:
		GetGame().GetWorld().QueryEntitiesBySphere(owner.GetOrigin(), m_fTriggerRadius, QueryDamageEntities, null, EQueryEntitiesFlags.ALL);
	}

	bool QueryDamageEntities(IEntity ent)
	{
		if (!ent) return true;
		
		// Apply damage logic to the entity if it is a character or vehicle
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(ent.FindComponent(DamageManagerComponent));
		if (damageManager)
		{
			// Implement custom event or damage. We pass dummy parameters for the PoC.
			damageManager.SetHealthScaled(damageManager.GetHealthScaled() - (m_fDamagePerTick * 0.01)); // Simple arbitrary damage simulation
			OnEntityDamaged(ent);
		}

		return true;
	}

	protected void OnEntityDamaged(IEntity ent)
	{
		// Derived classes override this to apply visual PFX, play shock sounds, cast fire etc.
	}
}
