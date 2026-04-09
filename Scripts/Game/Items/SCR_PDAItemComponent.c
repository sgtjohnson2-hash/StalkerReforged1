class SCR_PDAItemComponentClass : ScriptComponentClass
{
}

class SCR_PDAItemComponent : ScriptComponent
{
	[Attribute("100", desc: "Starting battery life of the PDA")]
	protected float m_fBatteryLife;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		Print("Server: PDA Item registered.");
	}

	bool HasPower()
	{
		return m_fBatteryLife > 0;
	}

	void DrainPower(float amount)
	{
		m_fBatteryLife -= amount;
		if (m_fBatteryLife < 0) m_fBatteryLife = 0;
	}
}
