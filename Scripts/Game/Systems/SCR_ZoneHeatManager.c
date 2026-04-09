class SCR_ZoneHeatManagerClass : GenericEntityClass
{
}

class SCR_ZoneHeatManager : GenericEntity
{
	protected static SCR_ZoneHeatManager s_Instance;

	[Attribute("100", desc: "Heat needed to trigger a Soviet QRF")]
	protected float m_fMaxHeat;
	
	[Attribute("15", desc: "Heat added per kill")]
	protected float m_fHeatPerKill;
	
	[Attribute("5", desc: "Heat decay per minute")]
	protected float m_fHeatDecayRate;
	
	[Attribute("4700 0 11000", desc: "Coordinates of the Soviet Airbase to track localized assaults")]
	protected vector m_vAirbaseLocation;
	
	[Attribute("20", desc: "Number of bodies dropped at the Airbase before it is considered mathematically LOST")]
	protected float m_fMaxAirbaseCasualties;
	
	protected float m_fCurrentAirbaseCasualties = 0;
	protected bool m_bAirbaseLockdown = false; // Is mass response currently running
	
	protected float m_fCurrentHeat = 0;
	protected vector m_vLastHotzone = "0 0 0";
	protected bool m_bQRFActive = false;

	static SCR_ZoneHeatManager GetInstance()
	{
		return s_Instance;
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		s_Instance = this;
		
		GetGame().GetCallqueue().CallLater(DecayHeat, 60000, true); // Decay heat every 60 seconds
	}

	void RegisterKill(vector killLocation)
	{
		// Is this kill physically happening at the Airbase? (Within a 400m radius)
		float distToAirbase = vector.Distance(killLocation, m_vAirbaseLocation);
		
		if (distToAirbase < 400)
		{
			m_fCurrentAirbaseCasualties++;
			Print("Server: Airbase casualty reported. Total Base Casualties: " + m_fCurrentAirbaseCasualties);
			
			SCR_MilitaryQRFManager qrfMgr = SCR_MilitaryQRFManager.GetInstance();
			
			// If we just pushed over the capture threshold, lock the base down globally!
			if (m_fCurrentAirbaseCasualties >= m_fMaxAirbaseCasualties)
			{
				if (qrfMgr) qrfMgr.SetAirbaseLost(true);
				
				SCR_PDA_UI ui = SCR_PDA_UI.GetInstance();
				if (ui) ui.ReceiveMessage("General Command", "THE AIRFIELD HAS FALLEN. ALL FORCES ROUTE FROM MAINLAND.");
			}
			// If the base isn't lost, but is currently under attack (and not already defending)
			else if (!m_bAirbaseLockdown)
			{
				m_bAirbaseLockdown = true;
				if (qrfMgr) qrfMgr.LaunchMassResponse(m_vAirbaseLocation);
				
				// Reset the scramble lockdown after 10 minutes so they can send another wave if still under attack
				GetGame().GetCallqueue().CallLater(ResetAirbaseLockdown, 600000, false);
			}
		}
		
		// Continue applying standard global zone heat regardless
		if (m_bQRFActive) return;

		m_fCurrentHeat += m_fHeatPerKill;
		m_vLastHotzone = killLocation;

		if (m_fCurrentHeat >= m_fMaxHeat)
		{
			TriggerQRF();
		}
	}

	protected void DecayHeat()
	{
		if (m_bQRFActive) return; // Don't decay if currently engaged
		
		m_fCurrentHeat -= m_fHeatDecayRate;
		if (m_fCurrentHeat < 0) m_fCurrentHeat = 0;
	}

	protected void TriggerQRF()
	{
		m_bQRFActive = true;
		
		Print("Server: ZONE HEAT CRITICAL! Dispatching Military QRF to: " + m_vLastHotzone.ToString());
		
		// Ping PDA Network
		SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
		if (network)
		{
			// Cheat access to SendNetworkMessage equivalent using UI directly for the broadcast
			SCR_PDA_UI ui = SCR_PDA_UI.GetInstance();
			if (ui) ui.ReceiveMessage("Military COMMs Intercept", "Rotors inbound to sector. Lethal force authorized.");
		}

		SCR_MilitaryQRFManager qrfMgr = SCR_MilitaryQRFManager.GetInstance();
		if (qrfMgr)
		{
			qrfMgr.DispatchHelicopter(m_vLastHotzone);
		}
		
		// Reset heat after a delay (e.g. 10 minutes)
		GetGame().GetCallqueue().CallLater(ResetQRF, 600000, false);
	}

	protected void ResetQRF()
	{
		m_bQRFActive = false;
		m_fCurrentHeat = 0;
		Print("Server: QRF Cooldown completed. Heat reset.");
	}
	
	protected void ResetAirbaseLockdown()
	{
		m_bAirbaseLockdown = false;
		Print("Server: Airbase Scramble cooldown completed. Ready for next defensive wave.");
	}
}
