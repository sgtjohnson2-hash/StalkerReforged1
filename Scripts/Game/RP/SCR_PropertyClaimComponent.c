class SCR_PropertyClaimComponentClass : ScriptComponentClass
{
}

class SCR_PropertyClaimComponent : ScriptComponent
{
	[Attribute("", desc: "Unique ID of the claimed structure or area")]
	protected string m_sPropertyID;

	[Attribute("500", desc: "Daily renting cost in RU")]
	protected int m_iRentCostRU;

	[RplProp()]
	protected string m_sOwnerPlayerUID = ""; // Empty means unclaimed/for sale
	
	[RplProp()]
	protected bool m_bIsDoorLocked = false;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
	}

	// Server-authoritative property claim purchase
	bool PurchaseProperty(IEntity player)
	{
		if (!Replication.IsServer()) return false;

		if (m_sOwnerPlayerUID != "")
		{
			Print("Server RP: Property purchase failed: already owned by " + m_sOwnerPlayerUID);
			return false;
		}

		// Query player's PDA Task Manager to check and subtract currency
		PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player));
		if (!pc) return false;

		SCR_PDATaskManagerComponent pdaMgr = SCR_PDATaskManagerComponent.Cast(pc.FindComponent(SCR_PDATaskManagerComponent));
		if (pdaMgr && pdaMgr.GetPlayerRuBalance() >= m_iRentCostRU)
		{
			pdaMgr.AddPlayerRuBalance(-m_iRentCostRU); // Deduct first day rent
			
			// Extract unique player backend GUID
			m_sOwnerPlayerUID = GetGame().GetPlayerManager().GetPlayerName(pc.GetPlayerId());
			m_bIsDoorLocked = true;
			Replication.BumpMe();

			Print("Server RP: Property successfully rented to: " + m_sOwnerPlayerUID);
			
			SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
			if (network)
			{
				network.SendNetworkMessage("Safehouse contract approved. Door locks are keyed to your PDA signature.", "Real Estate Agent");
			}
			return true;
		}
		
		Print("Server RP: Purchase failed: Insufficient RU balance!");
		return false;
	}

	// Toggles door lock state if character is the owner or carries the physical keycard
	bool ToggleDoorLock(IEntity player)
	{
		if (!Replication.IsServer()) return false;

		PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player));
		if (!pc) return false;

		string playerName = GetGame().GetPlayerManager().GetPlayerName(pc.GetPlayerId());

		// Verify ownership: either player matches m_sOwnerPlayerUID or they carry the matching keycard
		bool accessGranted = (playerName == m_sOwnerPlayerUID);

		if (!accessGranted)
		{
			// Loop through inventory to search for a keycard item containing the property ID
			SCR_InventoryStorageManagerComponent invManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
			if (invManager)
			{
				array<IEntity> items = new array<IEntity>();
				invManager.GetItems(items);
				foreach (IEntity item : items)
				{
					if (item && item.GetPrefabData().GetPrefabName().Contains("Keycard_" + m_sPropertyID))
					{
						accessGranted = true;
						break;
					}
				}
			}
		}

		if (accessGranted)
		{
			m_bIsDoorLocked = !m_bIsDoorLocked;
			Replication.BumpMe();
			Print("Server RP: Door lock toggled. Locked: " + m_bIsDoorLocked);
			
			SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
			if (network)
			{
				string stateText = "UNLOCKED";
				if (m_bIsDoorLocked) stateText = "LOCKED";
				network.SendNetworkMessage("Safehouse door state updated: " + stateText, "Security Interface");
			}
			return true;
		}

		Print("Server RP: Access Denied! Player does not own this property or hold a valid keycard.");
		return false;
	}

	bool IsLocked() { return m_bIsDoorLocked; }
	string GetOwner() { return m_sOwnerPlayerUID; }
	int GetRentCost() { return m_iRentCostRU; }
}
