class SCR_BurnerAnomalyComponentClass : SCR_AnomalyComponentClass
{
}

class SCR_BurnerAnomalyComponent : SCR_AnomalyComponent
{
	[Attribute("{7C07386762313629}Particles/Fire/Burner.ptc", desc: "Burner particle effect to play on activation")]
	protected ResourceName m_sBurnerParticle;

	override protected void OnEntityDamaged(IEntity ent)
	{
		super.OnEntityDamaged(ent);
		
		// Logic to trigger fire particle and sound since someone got damaged
		// For Example: PlaySound("burner_burst");
		Print("Burner anomaly burst triggered on entity: " + ent.ToString());
		
		// In a real Enfusion setup, we'd spawn the particle emitter here via SCR_ParticleEmitter
	}
}
