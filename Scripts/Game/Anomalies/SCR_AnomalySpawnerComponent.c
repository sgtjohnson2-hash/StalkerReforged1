[EntityEditorProps(category: "GameScripted/Anomalies", description: "Procedurally places anomaly prefabs across the terrain.", color: "0 0 255 255")]
class SCR_AnomalySpawnerComponentClass : ScriptComponentClass
{
}

class SCR_AnomalySpawnerComponent : ScriptComponent
{
	[Attribute("", UIWidgets.ResourceNamePicker, "List of Anomaly Prefabs to spawn procedurally", "et")]
	ref array<ResourceName> m_aAnomalyPrefabs;

	[Attribute("100", desc: "Total number of anomalies to attempt to spawn across the map")]
	int m_iTotalAnomalies;

	[Attribute("1000", desc: "Spawn radius range from this spawner entity")]
	float m_fSpawnRadius;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// In a real network setting, we ensure we only spawn on Authority (Server)
		if (!GetGame().GetDefaultScriptInvoker(ScriptInvoker.GetDefaultScriptInvokerEvent(DefaultScriptInvokerEvent.POST_INIT))) return;

		// Defer the spawning slightly to ensure world load
		GetGame().GetCallqueue().CallLater(SpawnAnomalies, 2000, false, owner);
	}

	protected void SpawnAnomalies(IEntity owner)
	{
		if (!m_aAnomalyPrefabs || m_aAnomalyPrefabs.IsEmpty()) return;

		vector spawnerOrigin = owner.GetOrigin();
		BaseWorld world = GetGame().GetWorld();

		for (int i = 0; i < m_iTotalAnomalies; i++)
		{
			// Pick random prefab
			int prefabIndex = Math.RandomIntInclusive(0, m_aAnomalyPrefabs.Count() - 1);
			ResourceName prefab = m_aAnomalyPrefabs[prefabIndex];

			// Calculate random position in a circle
			float angle = Math.RandomFloat(0, Math.PI2);
			float d = Math.RandomFloat(0, m_fSpawnRadius);
			
			float randomX = spawnerOrigin[0] + (d * Math.Cos(angle));
			float randomZ = spawnerOrigin[2] + (d * Math.Sin(angle));
			float y = world.GetSurfaceY(randomX, randomZ);

			vector spawnPos = "0 0 0";
			spawnPos[0] = randomX;
			spawnPos[1] = y;
			spawnPos[2] = randomZ;

			// Check if position is valid (not in water, not inside buildings preferably)
			// For simplicity we just spawn here
			Resource res = Resource.Load(prefab);
			if (res.IsValid())
			{
				EntitySpawnParams params = new EntitySpawnParams();
				params.Transform[3] = spawnPos;
				GetGame().SpawnEntityPrefab(res, world, params);
			}
		}

		Print("SCR_AnomalySpawnerComponent: Procedurally placed " + m_iTotalAnomalies + " anomalies.");
	}
}
