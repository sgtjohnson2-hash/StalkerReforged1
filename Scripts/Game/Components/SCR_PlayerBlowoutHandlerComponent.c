class SCR_PlayerBlowoutHandlerComponentClass : ScriptComponentClass
{
}

class SCR_PlayerBlowoutHandlerComponent : ScriptComponent
{
	protected static SCR_PlayerBlowoutHandlerComponent s_Instance;

	protected IEntity m_Owner;
	protected bool m_bInPeak;

	static SCR_PlayerBlowoutHandlerComponent GetInstance()
	{
		return s_Instance;
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_Owner = owner;
		
		// Only register local player
		if (!GetGame().GetPlayerManager().GetPlayerControlledEntity(GetGame().GetPlayerController().GetPlayerId()) == owner) return;
		s_Instance = this;
	}

	void OnWarningPhase()
	{
		Print("Client: Warning Phase. Sirens playing.");
		// PlaySound("blowout_siren");
	}

	void OnPeakPhase()
	{
		Print("Client: Peak Phase. Screen shaking, applying damage if outside.");
		m_bInPeak = true;
		
		// Start checking for roof/indoor status every 2 seconds
		GetGame().GetCallqueue().CallLater(CheckDamage, 2000, true);
	}

	void OnEndPhase()
	{
		Print("Client: End Phase. Restoring.");
		m_bInPeak = false;
		GetGame().GetCallqueue().Remove(CheckDamage);
	}

	protected void CheckDamage()
	{
		if (!m_bInPeak || !m_Owner) return;

		// Raycast upwards to see if player is under a roof
		vector mat[4];
		m_Owner.GetTransform(mat);
		
		vector startPoint = mat[3] + "0 1.5 0"; // Head bone
		vector endPoint = startPoint + "0 20 0"; // Cast 20m up

		BaseWorld world = m_Owner.GetWorld();
		bool hit = world.TracePosition(startPoint, endPoint, null, null);

		if (!hit)
		{
			// No roof above player -> Player is outside, apply massive damage
			DamageManagerComponent damageManager = DamageManagerComponent.Cast(m_Owner.FindComponent(DamageManagerComponent));
			if (damageManager)
			{
				damageManager.SetHealthScaled(0); // Instakill
				Print("Client: Player died to blowout exposure!");
			}
		}
	}
}
