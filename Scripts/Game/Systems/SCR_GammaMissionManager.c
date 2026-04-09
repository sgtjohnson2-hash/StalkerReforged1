class SCR_GammaMissionManagerClass : GenericEntityClass
{
}

class SCR_GammaMissionManager : GenericEntity
{
	protected static SCR_GammaMissionManager s_Instance;

	// Placeholder vanilla prefabs to act as "Mutant" and "Bandit" until Stalker assets are imported
	[Attribute("{BCE5F98AC8212BD9}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Rifleman.et", UIWidgets.ResourceNamePicker, desc: "Prefab representing a Stalker Bandit")]
	protected ResourceName m_sBanditPrefab;

	[Attribute("{7A9EE19AB67B298B}Prefabs/Characters/Factions/INDEP/FIA/Character_FIA_Rifleman.et", UIWidgets.ResourceNamePicker, desc: "Prefab representing a generic Mutant (Using FIA as placeholder)")]
	protected ResourceName m_sMutantPrefab;
	
	[Attribute("100", desc: "Base reward scaling multiplier per 500m")]
	protected int m_iRewardScaleRate;

	static SCR_GammaMissionManager GetInstance()
	{
		return s_Instance;
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		s_Instance = this;
	}

	// This is called by the PDA Manager whenever a new dynamic task needs to be instantiated.
	// It calculates a random position within radius and physically spawns the prefab.
	SCR_StalkerBaseTask CreateDynamicMission(vector originPlayerPos, EStalkerTaskType type, int minRadius = 300, int maxRadius = 1500)
	{
		// 1. Math: Generate coordinates
		float dist = Math.RandomFloat(minRadius, maxRadius);
		float angle = Math.RandomFloat(0, Math.PI2);
		
		vector spawnPos = "0 0 0";
		spawnPos[0] = originPlayerPos[0] + (Math.Cos(angle) * dist);
		spawnPos[2] = originPlayerPos[2] + (Math.Sin(angle) * dist);
		
		// Attempt to snap to terrain height (Simplified, requires BaseWorld raycasting in actual Enfusion but this is the schematic)
		spawnPos[1] = GetGame().GetWorld().GetSurfaceY(spawnPos[0], spawnPos[2]);

		// 2. Math: Reward calculation based on distance
		int dynamicReward = 500 + (Math.Floor(dist / 500) * m_iRewardScaleRate);

		// 3. Logic: Spawning the target prefab
		ResourceName prefabToSpawn = "";
		string missionName = "";
		string missionDesc = "";

		switch(type)
		{
			case EStalkerTaskType.MUTANT_HUNT:
				prefabToSpawn = m_sMutantPrefab;
				missionName = "Purge Mutants";
				missionDesc = "A mutant pack is disrupting operations. Eliminate them.";
				break;
				
			case EStalkerTaskType.ASSASSINATION:
				prefabToSpawn = m_sBanditPrefab;
				missionName = "Bounty: Bandit";
				missionDesc = "Take out the hostile squatter camping the area.";
				break;
		}

		// Spawn Entity
		IEntity spawnedEntity = null;
		if (prefabToSpawn != "")
		{
			EntitySpawnParams spawnParams = new EntitySpawnParams();
			spawnParams.TransformMode = ETransformMode.WORLD;
			spawnParams.Transform[3] = spawnPos;
			
			spawnedEntity = GetGame().SpawnEntityPrefab(Resource.Load(prefabToSpawn), GetGame().GetWorld(), spawnParams);
		}

		if (!spawnedEntity)
		{
			Print("Server Error: Failed to spawn dynamic mission target!", LogLevel.ERROR);
			return null;
		}

		// 4. Logic: Linking the Vanilla Task Framework
		SCR_BaseTaskManager taskManager = GetTaskManager();
		if (!taskManager) return null;

		// Note: We would spawn the Task prefab entity natively, but we will mock the return for architecture purposes
		Print("Server: Dynamic Mission deployed at " + spawnPos.ToString());
		
		// In a real Enfusion hook, we'd do taskMgr.CreateTask(). 
		// For now we simulate attaching the TaskTargetComponent.
		SCR_TaskTargetComponent targetTracker = SCR_TaskTargetComponent.Cast(spawnedEntity.FindComponent(SCR_TaskTargetComponent));
		if (!targetTracker)
		{
			// Fallback if component wasn't attached via workbench prefab overrides
			Print("Server Warning: Target prefab is missing SCR_TaskTargetComponent. Adding dynamically is not supported natively, ensure it is on the .et file.", LogLevel.WARNING);
		}
		
		// Map hook proxy (See PDATaskManager for actual assignment)
		return null;
	}
}
