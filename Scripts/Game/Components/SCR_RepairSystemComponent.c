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

	// Pseudo logic for checking inventory and consuming items to repair a target weapon
	bool TryRepairWeapon(IEntity player, IEntity weapon, bool useAdvancedKit)
	{
		// 1. Check if the weapon has our durability component
		SCR_WeaponDurabilityComponent durComp = SCR_WeaponDurabilityComponent.Cast(weapon.FindComponent(SCR_WeaponDurabilityComponent));
		if (!durComp)
		{
			Print("Repair Failed: Weapon does not have a Durability Component.");
			return false;
		}

		if (durComp.GetDurability() >= 100)
		{
			Print("Repair Failed: Weapon is already in perfect condition.");
			return false;
		}

		// 2. Mock Inventory checks (A real implementation loops through standard Reforger SCR_InventoryStorageManagerComponent)
		bool hasScrap = true; // Temporary mock
		bool hasKit = true; // Temporary mock

		if (!hasScrap)
		{
			Print("Repair Failed: Missing Scrap material!");
			return false;
		}

		// 3. Consume items and apply repair
		float assignedRepairValue = m_fCommonScrapValue;

		if (useAdvancedKit)
		{
			if (!hasKit)
			{
				Print("Repair Failed: Missing Weapon Kit!");
				return false;
			}
			assignedRepairValue = m_fWeaponKitValue;
		}

		durComp.RepairWeapon(assignedRepairValue);
		Print("Repair Success: Applied " + assignedRepairValue + " durability to weapon.");
		
		return true;
	}
}
