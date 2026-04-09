class SCR_StashLootTableClass : GenericEntityClass
{
}

class SCR_StashLootTable : GenericEntity
{
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Basic Stash Loot (e.g., Bandages, Junk, 9x18mm Ammo)")]
	ref array<ResourceName> m_aBasicLoot;
	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Advanced Stash Loot (e.g., Medkits, 5.45x39mm Ammo, Weapon Parts)")]
	ref array<ResourceName> m_aAdvancedLoot;
	
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Rare Stash Loot (e.g., Exoskeleton Parts, Artifacts, Tier 3 AP Ammo)")]
	ref array<ResourceName> m_aRareLoot;

	static SCR_StashLootTable s_Instance;

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		s_Instance = this;
	}

	static SCR_StashLootTable GetInstance()
	{
		return s_Instance;
	}

	// Helper function for the Manager to query items by tier
	array<ResourceName> GetLootPool(int tier)
	{
		switch (tier)
		{
			case 1: return m_aAdvancedLoot;
			case 2: return m_aRareLoot;
			default: return m_aBasicLoot;
		}
		return m_aBasicLoot;
	}
}
