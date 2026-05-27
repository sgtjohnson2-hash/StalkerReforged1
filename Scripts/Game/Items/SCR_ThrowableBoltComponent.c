class SCR_ThrowableBoltComponentClass : ScriptComponentClass
{
}

class SCR_ThrowableBoltComponent : ScriptComponent
{
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
	}
	
	// Server-authoritative physics collision callback
	void OnCollision(IEntity owner, IEntity hitEntity, vector hitPos)
	{
		if (!Replication.IsServer()) return;
		
		Print("Server: Bolt hit surface at " + hitPos.ToString() + ", checking for anomalies...");
		
		// Perform sphere query to find and visually activate anomalies
		GetGame().GetWorld().QueryEntitiesBySphere(hitPos, 5.0, QueryForceTrigger, null, EQueryEntitiesFlags.ALL);
	}

	bool QueryForceTrigger(IEntity ent)
	{
		SCR_AnomalyComponent anomaly = SCR_AnomalyComponent.Cast(ent.FindComponent(SCR_AnomalyComponent));
		if (anomaly)
		{
			Print("Server: Bolt triggered anomaly at " + ent.GetOrigin().ToString());
			// Trigger anomaly visual burst natively on clients from the server here
		}
		return true;
	}
}
