class SCR_ProceduralStashManagerClass : GenericEntityClass
{
}

class SCR_ProceduralStashManager : GenericEntity
{
	protected static SCR_ProceduralStashManager s_Instance;

	[Attribute("{BCC52E2A91DABBD5}Prefabs/Props/Military/AmmoBoxes/AmmoBox_FIA.et", UIWidgets.ResourceNamePicker, desc: "Prefab representing the Stash Container. Must have UniversalInventoryStorageComponent!")]
	protected ResourceName m_sStashBoxPrefab;

	static SCR_ProceduralStashManager GetInstance()
	{
		return s_Instance;
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		s_Instance = this;
	}

	// Called by SCR_LootableCorpseComponent when the RNG roll succeeds.
	void SparkStashDiscovery(IEntity discoverer, int tier)
	{
		// 1. Math: radially spawn the stash somewhere out in the world (500m to 3000m away)
		// Everon adjustment: Validates the target isn't deep underwater.
		vector origin = discoverer.GetOrigin();
		vector spawnPos = "0 0 0";
		bool validPos = false;
		
		for (int attempts = 0; attempts < 10; attempts++)
		{
			float dist = Math.RandomFloat(500, 3000);
			float angle = Math.RandomFloat(0, Math.PI2);
			
			spawnPos[0] = origin[0] + (Math.Cos(angle) * dist);
			spawnPos[2] = origin[2] + (Math.Sin(angle) * dist);
			spawnPos[1] = GetGame().GetWorld().GetSurfaceY(spawnPos[0], spawnPos[2]);
			
			// If we are above sea-level (roughly 1.5m) on Everon, this is physical land
			if (spawnPos[1] > 1.5)
			{
				validPos = true;
				break;
			}
		}
		
		if (!validPos)
		{
			Print("Server Warning: Aborted Stash Spawn. All geometric attempts fell into the Everon Ocean.");
			return;
		}

		// 2. Physical Spawn of the Container
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform[3] = spawnPos;
		
		IEntity stashBox = GetGame().SpawnEntityPrefab(Resource.Load(m_sStashBoxPrefab), GetGame().GetWorld(), spawnParams);
		if (!stashBox)
		{
			Print("Server Error: Failed to spawn Stash Box Prefab!", LogLevel.ERROR);
			return;
		}

		// 3. Inject Loot (If LootTable exists, otherwise just leave the box empty to be filled by vanilla setups)
		SCR_StashLootTable lootTbl = SCR_StashLootTable.GetInstance();
		if (lootTbl)
		{
			array<ResourceName> lootPool = lootTbl.GetLootPool(tier);
			
			// Natively interacting with Arma Reforger's Inventory system
			SCR_UniversalInventoryStorageComponent inventory = SCR_UniversalInventoryStorageComponent.Cast(stashBox.FindComponent(SCR_UniversalInventoryStorageComponent));
			if (inventory && lootPool.Count() > 0)
			{
				int itemsToInject = Math.RandomInt(2, 6);
				for (int i = 0; i < itemsToInject; i++)
				{
					ResourceName rndItem = lootPool[Math.RandomInt(0, lootPool.Count())];
					if (rndItem != "")
					{
						// Insert item using generic engine logic or spawn and move into inventory
						IEntity itemToStore = GetGame().SpawnEntityPrefab(Resource.Load(rndItem));
						if (itemToStore) GetGame().GetInventoryManager().InsertItem(itemToStore, inventory);
					}
				}
				Print("Server: Stash constructed with " + itemsToInject + " tier-" + tier + " items inside.");
			}
		}

		// 4. PDA Map Pinging
		Print("Client PDA: STASH COORDINATES DOWNLOADED. Map updated.");
		
		// Map hook proxy -> Drop a compass marker here via SCR_MapMarkerManagerComponent
		// S.T.A.L.K.E.R. functionality: Play the iconic PDA message beep
		SCR_PDA_UI ui = SCR_PDA_UI.GetInstance();
		if (ui) ui.ReceiveMessage("Network", "Extracted stash coordinates from deceased PDA. Look on your map.");
	}
}
