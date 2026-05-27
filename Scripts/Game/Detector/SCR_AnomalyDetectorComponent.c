class SCR_AnomalyDetectorComponentClass : ScriptComponentClass
{
}

class SCR_AnomalyDetectorComponent : ScriptComponent
{
	[Attribute("100", desc: "Maximum detection range")]
	protected float m_fMaxDetectionRange;

	protected IEntity m_Owner;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Owner = owner;
		
		// Scan loop only runs on client
		if (Replication.IsClient())
		{
			GetGame().GetCallqueue().CallLater(ScanAnomalies, 1000, true);
		}
	}

	protected void ScanAnomalies()
	{
		if (!Replication.IsClient()) return;
		
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc) return;
		
		IEntity localPlayer = pc.GetControlledEntity();
		if (!localPlayer) return;
		
		// Ensure this detector is carried/held by the local player character to prevent ambient ghost alerts
		if (m_Owner.GetRootParent() != localPlayer) return;

		float closestDistance = m_fMaxDetectionRange;
		vector playerPos = localPlayer.GetOrigin();
		
		// Query physical world sphere for nearby anomalies
		GetGame().GetWorld().QueryEntitiesBySphere(playerPos, m_fMaxDetectionRange, QueryAnomalies, null, EQueryEntitiesFlags.ALL);
	}

	bool QueryAnomalies(IEntity ent)
	{
		SCR_AnomalyComponent anomaly = SCR_AnomalyComponent.Cast(ent.FindComponent(SCR_AnomalyComponent));
		if (anomaly)
		{
			float dist = vector.Distance(ent.GetOrigin(), m_Owner.GetOrigin());
			Print("Client UI Detector: Anomaly detected at distance: " + dist);
			// Play local beep sound based on distance here
		}
		return true;
	}
}
