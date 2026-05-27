class SCR_LootableCorpseComponentClass : ScriptComponentClass
{
}

class SCR_LootableCorpseComponent : ScriptComponent
{
	[Attribute("20", desc: "Percentage chance (0-100) that this AI's death drops a stash coordinate")]
	protected int m_iStashDropChance;

	[Attribute("1", desc: "Tier of the loot box spawned if this AI drops coordinates (1=Adv, 2=Rare, 0=Basic)")]
	protected int m_iAssociatedTier;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// Hook into the native damage manager to listen for death
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageMgr)
		{
			damageMgr.GetOnDestroyed().Insert(OnCorpseStashRoll);
		}
	}

	protected void OnCorpseStashRoll(IEntity entity)
	{
		// Stash discovery and zone heat tracking must occur exclusively on the server (authority)
		if (!Replication.IsServer()) return;

		// Notify the Military Zone Heat system of a death in the quadrant
		SCR_ZoneHeatManager heatMgr = SCR_ZoneHeatManager.GetInstance();
		if (heatMgr)
		{
			heatMgr.RegisterKill(entity.GetOrigin());
		}

		// Mathematical probability evaluation for stashes
		int roll = Math.RandomIntInclusive(0, 100);
		if (roll <= m_iStashDropChance)
		{
			SCR_ProceduralStashManager stashMgr = SCR_ProceduralStashManager.GetInstance();
			if (stashMgr)
			{
				stashMgr.SparkStashDiscovery(entity, m_iAssociatedTier);
			}
		}
	}

	void ~SCR_LootableCorpseComponent()
	{
		IEntity owner = GetOwner();
		if (!owner) return;
		
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageMgr)
		{
			damageMgr.GetOnDestroyed().Remove(OnCorpseStashRoll);
		}
	}
}
