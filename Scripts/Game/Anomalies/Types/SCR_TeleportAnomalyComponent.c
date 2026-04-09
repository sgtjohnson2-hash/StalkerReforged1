class SCR_TeleportAnomalyComponentClass : SCR_AnomalyComponentClass
{
}

class SCR_TeleportAnomalyComponent : SCR_AnomalyComponent
{
	override protected void OnEntityDamaged(IEntity ent)
	{
		// Teleports don't just damage, they move the entity
		vector newPos;
		float randomX = Math.RandomFloat(-50, 50);
		float randomZ = Math.RandomFloat(-50, 50);
		
		vector origin = ent.GetOrigin();
		newPos = "0 0 0";
		newPos[0] = origin[0] + randomX;
		newPos[1] = origin[1] + 10; // Drop from sky
		newPos[2] = origin[2] + randomZ;
		
		ent.SetOrigin(newPos);
		
		Print("Teleport anomaly triggered, teleported entity: " + ent.ToString());
	}
}
