enum ERPTraderType
{
	GENERAL,
	MILITARY,
	RENEGADE,
	SCIENTIST,
	DRUG_DEALER
}

class SCR_EconomyManagerClass : GenericEntityClass
{
}

class SCR_EconomyManager : GenericEntity
{
	protected static SCR_EconomyManager s_Instance;

	// Inherent buy/sell price multipliers for the server economy
	[Attribute("1.0", desc: "Base buy price multiplier")]
	protected float m_fBaseBuyMultiplier;

	[Attribute("0.5", desc: "Base sell price multiplier")]
	protected float m_fBaseSellMultiplier;

	static SCR_EconomyManager GetInstance()
	{
		return s_Instance;
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		s_Instance = this;
	}

	// Calculates the final price of an item factoring in player faction relations
	int CalculateTransactionPrice(IEntity player, ERPTraderType traderType, int basePrice, bool isBuying)
	{
		float factionMultiplier = 1.0;

		SCR_PlayerFactionManagerComponent factionMgr = SCR_PlayerFactionManagerComponent.Cast(player.FindComponent(SCR_PlayerFactionManagerComponent));
		if (factionMgr)
		{
			string faction = factionMgr.GetCurrentFaction();

			switch (traderType)
			{
				case ERPTraderType.MILITARY:
					if (faction == "USSR") factionMultiplier = 0.8; // Military gets 20% discount
					else if (faction == "FIA") factionMultiplier = 3.0; // Hostile markup
					break;
					
				case ERPTraderType.RENEGADE:
					if (faction == "FIA") factionMultiplier = 0.85; // Bandits discount
					else if (faction == "USSR") factionMultiplier = 4.0; // Extreme military hostile markup
					break;
					
				case ERPTraderType.SCIENTIST:
					if (faction == "CIV") factionMultiplier = 0.9; // Loners discount
					break;
			}
		}

		float transactionDirectionMultiplier = m_fBaseSellMultiplier;
		if (isBuying) transactionDirectionMultiplier = m_fBaseBuyMultiplier;

		return Math.Floor(basePrice * transactionDirectionMultiplier * factionMultiplier);
	}

	// Server-authoritative item purchase bridge
	bool ExecutePurchase(IEntity player, ERPTraderType traderType, ResourceName itemPrefab, int basePrice)
	{
		if (!Replication.IsServer()) return false;

		PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player));
		if (!pc) return false;

		SCR_PDATaskManagerComponent pdaMgr = SCR_PDATaskManagerComponent.Cast(pc.FindComponent(SCR_PDATaskManagerComponent));
		SCR_InventoryStorageManagerComponent invManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));

		if (!pdaMgr || !invManager) return false;

		int finalPrice = CalculateTransactionPrice(player, traderType, basePrice, true);

		if (pdaMgr.GetPlayerRuBalance() >= finalPrice)
		{
			// Spawn item and insert into player's inventory
			bool added = invManager.TrySpawnAndAddItem(itemPrefab);
			if (added)
			{
				pdaMgr.AddPlayerRuBalance(-finalPrice);
				Print("Server RP: Transaction successful. Purchased item: " + itemPrefab + " for " + finalPrice + " RU.");
				return true;
			}
			Print("Server RP: Purchase failed: Player inventory is full!");
			return false;
		}

		Print("Server RP: Purchase failed: Insufficient RU balance!");
		return false;
	}

	// Converts collected scrap metal parts into raw currency (RU)
	bool ConvertScrapToRu(IEntity player, int payPerScrap)
	{
		if (!Replication.IsServer()) return false;

		PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player));
		SCR_InventoryStorageManagerComponent invManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		SCR_PDATaskManagerComponent pdaMgr = SCR_PDATaskManagerComponent.Cast(pc.FindComponent(SCR_PDATaskManagerComponent));

		if (!pc || !invManager || !pdaMgr) return false;

		array<IEntity> items = new array<IEntity>();
		invManager.GetItems(items);
		
		int scrapCount = 0;
		array<IEntity> scrapToDelete = new array<IEntity>();

		foreach (IEntity item : items)
		{
			if (item && item.GetPrefabData().GetPrefabName().Contains("Scrap"))
			{
				scrapCount++;
				scrapToDelete.Insert(item);
			}
		}

		if (scrapCount == 0)
		{
			Print("Server RP: Scrap conversion failed: No scrap found in inventory.");
			return false;
		}

		// Delete all scrap items and credit the player's account
		foreach (IEntity scrap : scrapToDelete)
		{
			invManager.TryDeleteItem(scrap);
		}

		int totalPayout = scrapCount * payPerScrap;
		pdaMgr.AddPlayerRuBalance(totalPayout);

		Print("Server RP: Converted " + scrapCount + " scrap metal for " + totalPayout + " RU.");
		
		SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
		if (network)
		{
			network.SendNetworkMessage("Traded " + scrapCount + " scrap parts for " + totalPayout + " RU.", "Traders Union");
		}
		return true;
	}
}
