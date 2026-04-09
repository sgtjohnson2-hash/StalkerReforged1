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
		
		// Hook into the native Reforger damage manager to listen for death
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(owner.FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageMgr)
		{
			damageMgr.GetOnDestroyed().Insert(OnCorpseStashRoll);
		}
	}

	protected void OnCorpseStashRoll(IEntity entity)
	{
		// Notify the Military Zone Heat system of a death in the quadrant
		SCR_ZoneHeatManager heatMgr = SCR_ZoneHeatManager.GetInstance();
		if (heatMgr)
		{
			heatMgr.RegisterKill(entity.GetOrigin());
		}

		// S.T.A.L.K.E.R. functionality: Evaluate mathematical probability
		int roll = Math.RandomIntInclusive(0, 100);
		
		if (roll <= m_iStashDropChance)
		{
			// The dead stalker had a PDA hook mapping to a stash! Find the manager and trigger it.
			SCR_ProceduralStashManager stashMgr = SCR_ProceduralStashManager.GetInstance();
			if (stashMgr)
			{
				// In a full MP setting, this should ideally target the killer, but we will broadcast via the origin entity.
				stashMgr.SparkStashDiscovery(entity, m_iAssociatedTier);
			}
		}
	}

	void ~SCR_LootableCorpseComponent()
	{
		SCR_CharacterDamageManagerComponent damageMgr = SCR_CharacterDamageManagerComponent.Cast(GetOwner().FindComponent(SCR_CharacterDamageManagerComponent));
		if (damageMgr)
		{
			damageMgr.GetOnDestroyed().Remove(OnCorpseStashRoll);
		}
	}
}
