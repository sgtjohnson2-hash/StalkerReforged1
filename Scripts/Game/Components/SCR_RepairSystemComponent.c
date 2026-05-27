class SCR_RepairSystemComponentClass : ScriptComponentClass
{
}

class SCR_RepairSystemComponent : ScriptComponent
{
	[Attribute("10", desc: "Base repair amount given by common scrap parts")]
	protected float m_fCommonScrapValue;

	[Attribute("25", desc: "Base repair amount given by weapon kits")]
	protected float m_fWeaponKitValue;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
	}

	// Server-authoritative inventory checks and weapon repair execution
	bool TryRepairWeapon(IEntity player, IEntity weapon, bool useAdvancedKit)
	{
		if (!Replication.IsServer()) return false;

		// 1. Check if the weapon has our durability component
		SCR_WeaponDurabilityComponent durComp = SCR_WeaponDurabilityComponent.Cast(weapon.FindComponent(SCR_WeaponDurabilityComponent));
		if (!durComp)
		{
			Print("Server: Repair Failed: Weapon does not have a Durability Component.");
			return false;
		}

		if (durComp.GetDurability() >= 100)
		{
			Print("Server: Repair Failed: Weapon is already in perfect condition.");
			return false;
		}

		// 2. Query Reforger's real inventory manager component
		SCR_InventoryStorageManagerComponent invManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!invManager)
		{
			Print("Server: Repair Failed: No Inventory Manager found on player.");
			return false;
		}

		string itemNeeded = "Scrap";
		if (useAdvancedKit) itemNeeded = "WeaponKit";

		// Find and consume the matching repair item inside the inventory
		array<IEntity> items = new array<IEntity>();
		invManager.GetItems(items);
		
		IEntity itemToConsume = null;
		foreach (IEntity item : items)
		{
			if (item && item.GetPrefabData().GetPrefabName().Contains(itemNeeded))
			{
				itemToConsume = item;
				break;
			}
		}

		if (!itemToConsume)
		{
			Print("Server: Repair Failed: Missing required item: " + itemNeeded);
			return false;
		}

		// 3. Consume the item and apply repair
		float assignedRepairValue = m_fCommonScrapValue;
		if (useAdvancedKit) assignedRepairValue = m_fWeaponKitValue;

		// Server deletes the consumed material natively
		invManager.TryDeleteItem(itemToConsume);
		
		durComp.RepairWeapon(assignedRepairValue);
		Print("Server: Repair Success: Applied " + assignedRepairValue + " durability to weapon.");
		
		return true;
	}
}
