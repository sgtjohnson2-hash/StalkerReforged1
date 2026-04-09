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
		
		// Run scanning loop
		GetGame().GetCallqueue().CallLater(ScanAnomalies, 1000, true);
	}

	protected void ScanAnomalies()
	{
		// Only run for the local player who owns this detector
		if (!GetGame().GetPlayerManager().GetPlayerControlledEntity(GetGame().GetPlayerController().GetPlayerId())) return;
		
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(GetGame().GetPlayerController().GetPlayerId());
		// If detector isn't equipped/in hands, we normally wouldn't run. Let's assume it's running always for this prototype.

		float closestDistance = m_fMaxDetectionRange;
		vector playerPos = player.GetOrigin();
		
		// We could query spheres, or keep a global registry of Anomalies. 
		// For prototype, we search for SCR_AnomalyComponent nearby
		GetGame().GetWorld().QueryEntitiesBySphere(playerPos, m_fMaxDetectionRange, QueryAnomalies, null, EQueryEntitiesFlags.ALL);
		
		// We would set bip interval based on `closestDistance`.
	}

	bool QueryAnomalies(IEntity ent)
	{
		SCR_AnomalyComponent anomaly = SCR_AnomalyComponent.Cast(ent.FindComponent(SCR_AnomalyComponent));
		if (anomaly)
		{
			float dist = vector.Distance(ent.GetOrigin(), m_Owner.GetOrigin());
			// Trigger UI bip or audio bip here based on distance.
			Print("Detector: Anomaly detected at distance " + dist);
			
			// PlaySound("detector_bip");
		}
		return true;
	}
}
