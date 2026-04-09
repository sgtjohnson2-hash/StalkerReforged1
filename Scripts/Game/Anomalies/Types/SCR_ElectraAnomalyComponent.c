class SCR_ElectraAnomalyComponentClass : SCR_AnomalyComponentClass
{
}

class SCR_ElectraAnomalyComponent : SCR_AnomalyComponent
{
	[Attribute("{1234567890ABCDEF}Particles/Electric/Electra.ptc", desc: "Electra particle effect to play on activation")]
	protected ResourceName m_sElectraParticle;

	override protected void OnEntityDamaged(IEntity ent)
	{
		super.OnEntityDamaged(ent);
		
		Print("Electra anomaly shocked entity: " + ent.ToString());
	}
}
