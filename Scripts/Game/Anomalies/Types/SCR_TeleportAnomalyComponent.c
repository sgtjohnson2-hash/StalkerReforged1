class SCR_TeleportAnomalyComponentClass : SCR_AnomalyComponentClass
{
}

class SCR_TeleportAnomalyComponent : SCR_AnomalyComponent
{
	override protected void OnEntityDamaged(IEntity ent)
	{
		// Teleportation must be handled exclusively on server authority
		if (!Replication.IsServer()) return;

		vector newPos;
		float randomX = Math.RandomFloat(-50, 50);
		float randomZ = Math.RandomFloat(-50, 50);
		
		vector origin = ent.GetOrigin();
		newPos = "0 0 0";
		newPos[0] = origin[0] + randomX;
		newPos[2] = origin[2] + randomZ;
		newPos[1] = ent.GetWorld().GetSurfaceY(newPos[0], newPos[2]) + 10.0; // Teleport and drop 10m high

		Physics phys = ent.GetPhysics();
		if (phys)
		{
			// Disabling physics momentarily prevents coordinate lock glitches and rubberbanding
			phys.SetActive(false);
			ent.SetOrigin(newPos);
			phys.SetActive(true);
			
			// Apply a smooth downward velocity to drop the player character
			vector fallVel = "0 -2 0";
			phys.SetVelocity(fallVel);
		}
		else
		{
			ent.SetOrigin(newPos);
		}
		
		Print("Server: Teleport anomaly triggered. Character translated to " + newPos.ToString());
	}
}
