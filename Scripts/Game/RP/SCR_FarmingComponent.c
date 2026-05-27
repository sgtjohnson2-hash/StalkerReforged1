enum EFarmingCropState
{
	EMPTY,
	PLANTED,
	SPROUT,
	MATURE,
	HARVEST_READY
}

class SCR_FarmingComponentClass : ScriptComponentClass
{
}

class SCR_FarmingComponent : ScriptComponent
{
	[Attribute("10.0", desc: "Growth time per stage in seconds")]
	protected float m_fGrowthStageInterval;

	[Attribute("20.0", desc: "Maximum water capacity of the soil")]
	protected float m_fMaxWater;

	[RplProp()]
	protected EFarmingCropState m_eCropState = EFarmingCropState.EMPTY;

	[RplProp()]
	protected float m_fWaterLevel = 0.0;

	protected string m_sPlantedCropType = "";
	protected float m_fGrowthProgress = 0.0;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// Run growth tick simulation exclusively on server
		if (Replication.IsServer())
		{
			GetGame().GetCallqueue().CallLater(ProcessGrowthTick, 1000, true);
		}
	}

	// Server-authoritative seed planting
	bool PlantSeed(IEntity player, string cropType)
	{
		if (!Replication.IsServer()) return false;
		if (m_eCropState != EFarmingCropState.EMPTY) return false;

		// Consume the seed item from player inventory
		SCR_InventoryStorageManagerComponent invManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!invManager) return false;

		array<IEntity> items = new array<IEntity>();
		invManager.GetItems(items);
		IEntity seedItem = null;
		foreach (IEntity item : items)
		{
			if (item && item.GetPrefabData().GetPrefabName().Contains(cropType + "Seed"))
			{
				seedItem = item;
				break;
			}
		}

		if (!seedItem)
		{
			Print("Server RP: Farming Failed: Missing seed item in inventory.");
			return false;
		}

		invManager.TryDeleteItem(seedItem);

		m_eCropState = EFarmingCropState.PLANTED;
		m_sPlantedCropType = cropType;
		m_fGrowthProgress = 0.0;
		Replication.BumpMe();

		Print("Server RP: Crop seed planted: " + cropType);
		return true;
	}

	// Adds water to the soil to nourish the crop
	void WaterCrop(float amount)
	{
		if (!Replication.IsServer()) return;

		m_fWaterLevel += amount;
		if (m_fWaterLevel > m_fMaxWater) m_fWaterLevel = m_fMaxWater;
		Replication.BumpMe();

		Print("Server RP: Crop watered. Level: " + m_fWaterLevel);
	}

	// Ticks growth on the server if water is available
	protected void ProcessGrowthTick()
	{
		if (!Replication.IsServer() || m_eCropState == EFarmingCropState.EMPTY || m_eCropState == EFarmingCropState.HARVEST_READY) return;

		// Soil needs water to grow crops
		if (m_fWaterLevel > 0)
		{
			m_fWaterLevel -= 0.1; // Consume water slowly
			if (m_fWaterLevel < 0) m_fWaterLevel = 0.0;

			m_fGrowthProgress += 1.0;
			Replication.BumpMe();

			if (m_fGrowthProgress >= m_fGrowthStageInterval)
			{
				m_fGrowthProgress = 0.0;
				AdvanceStage();
			}
		}
	}

	protected void AdvanceStage()
	{
		if (m_eCropState == EFarmingCropState.PLANTED)
		{
			m_eCropState = EFarmingCropState.SPROUT;
			Print("Server RP: Crop advanced to SPROUT.");
		}
		else if (m_eCropState == EFarmingCropState.SPROUT)
		{
			m_eCropState = EFarmingCropState.MATURE;
			Print("Server RP: Crop advanced to MATURE.");
		}
		else if (m_eCropState == EFarmingCropState.MATURE)
		{
			m_eCropState = EFarmingCropState.HARVEST_READY;
			Print("Server RP: Crop ready for HARVEST!");
			
			SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
			if (network)
			{
				network.SendNetworkMessage("Your planted " + m_sPlantedCropType + " crop is ripe and ready to harvest.", "Farming Alert");
			}
		}
		Replication.BumpMe();
	}

	// Harvests the crop, spawning and injecting the mature items into the player's inventory
	bool Harvest(IEntity player)
	{
		if (!Replication.IsServer()) return false;
		if (m_eCropState != EFarmingCropState.HARVEST_READY) return false;

		SCR_InventoryStorageManagerComponent invManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!invManager) return false;

		ResourceName harvestedPrefab = "Prefabs/Items/" + m_sPlantedCropType + ".et";
		
		// Attempt to insert harvested crop directly into player's inventory
		bool added = invManager.TrySpawnAndAddItem(harvestedPrefab);
		
		if (added)
		{
			Print("Server RP: Successfully harvested " + m_sPlantedCropType + " into inventory.");
			m_eCropState = EFarmingCropState.EMPTY;
			m_sPlantedCropType = "";
			m_fGrowthProgress = 0.0;
			Replication.BumpMe();
			return true;
		}

		Print("Server RP: Harvesting failed: Inventory is full!");
		return false;
	}

	EFarmingCropState GetCropState() { return m_eCropState; }
	float GetWaterLevel() { return m_fWaterLevel; }
}
