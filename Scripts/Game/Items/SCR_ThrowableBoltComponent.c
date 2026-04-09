class SCR_ThrowableBoltComponentClass : ScriptComponentClass
{
}

class SCR_ThrowableBoltComponent : ScriptComponent
{
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// When the bolt is thrown and hits the ground, it triggers anomalies without damaging players.
		// Reforger uses SCR_ConsumableItemComponent for grenades. 
		// Here we would hook into OnHit event to trigger an EOnFrame or similar anomaly response.
	}
	
	// A mock up of the collision callback
	void OnCollision(IEntity owner, IEntity hitEntity, vector hitPos)
	{
		Print("Bolt hit surface at " + hitPos.ToString() + ", checking for anomalies to visually trigger them...");
		
		// This uses QueryEntitiesBySphere to find an anomaly and force its PFX to show
		GetGame().GetWorld().QueryEntitiesBySphere(hitPos, 5.0, QueryForceTrigger, null, EQueryEntitiesFlags.ALL);
	}

	bool QueryForceTrigger(IEntity ent)
	{
		SCR_AnomalyComponent anomaly = SCR_AnomalyComponent.Cast(ent.FindComponent(SCR_AnomalyComponent));
		if (anomaly)
		{
			Print("Bolt triggered anomaly at " + ent.GetOrigin().ToString());
			// Force the visual trigger of the anomaly without applying damage
		}
		return true;
	}
}
