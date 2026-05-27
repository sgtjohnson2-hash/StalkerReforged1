class SCR_ScavengeAction : ScriptedUserAction
{
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		Print("Client: Player began scavenging world object...");
		
		// Trigger server-authoritative scavenging
		SCR_ScavengeObjectComponent scavComponent = SCR_ScavengeObjectComponent.Cast(pOwnerEntity.FindComponent(SCR_ScavengeObjectComponent));
		if (scavComponent)
		{
			scavComponent.ExecuteScavengeServer(pUserEntity);
		}
	}

	override bool CanBePerformedScript(IEntity user)
	{
		// Action is always visible, but the server handles cooldown notifications
		return true;
	}

	override bool GetActionNameScript(out string outName)
	{
		outName = "Search / Salvage Scrap";
		return true;
	}

	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}
}

class SCR_ScavengeObjectComponentClass : ScriptComponentClass
{
}

class SCR_ScavengeObjectComponent : ScriptComponent
{
	[Attribute("60", desc: "Cooldown in seconds before this object can be scavenged again")]
	protected float m_fScavengeCooldown;

	[Attribute("50", desc: "Percentage chance (0-100) of successfully finding items")]
	protected int m_iSuccessChance;

	[Attribute("Prefabs/Items/SCR_ScrapParts.et", UIWidgets.ResourceNamePicker, desc: "Item to award on success", ext: "et")]
	protected ResourceName m_sScrapPrefab;

	protected bool m_bIsOnCooldown = false;
	protected float m_fCooldownTimeRemaining = 0.0;

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (Replication.IsServer())
		{
			GetGame().GetCallqueue().CallLater(ProcessCooldownTick, 1000, true);
		}
	}

	// Server-authoritative scavenging action
	void ExecuteScavengeServer(IEntity player)
	{
		if (!Replication.IsServer()) return;

		if (m_bIsOnCooldown)
		{
			Print("Server RP: Scavenge failed: Object is on cooldown.");
			SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
			if (network)
			{
				network.SendNetworkMessage("This scrap heap has already been scavenged. Try again in " + Math.Floor(m_fCooldownTimeRemaining) + " seconds.", "System");
			}
			return;
		}

		// Trigger RNG roll for salvage success
		int roll = Math.RandomIntInclusive(0, 100);
		if (roll <= m_iSuccessChance)
		{
			SCR_InventoryStorageManagerComponent invManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
			if (invManager)
			{
				bool added = invManager.TrySpawnAndAddItem(m_sScrapPrefab);
				if (added)
				{
					Print("Server RP: Successfully salvaged scrap metal into player inventory.");
					SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
					if (network)
					{
						network.SendNetworkMessage("Salvaged scrap parts from the wreckage.", "Inventory");
					}
				}
				else
				{
					Print("Server RP: Salvage failed: Player inventory is full!");
					return;
				}
			}
		}
		else
		{
			Print("Server RP: Salvaged the pile but found nothing useful.");
			SCR_PDANetworkManager network = SCR_PDANetworkManager.GetInstance();
			if (network)
			{
				network.SendNetworkMessage("Searched the wreckage but found nothing useful.", "System");
			}
		}

		// Activate server-side cooldown
		m_bIsOnCooldown = true;
		m_fCooldownTimeRemaining = m_fScavengeCooldown;
	}

	protected void ProcessCooldownTick()
	{
		if (!Replication.IsServer() || !m_bIsOnCooldown) return;

		if (m_fCooldownTimeRemaining > 0)
		{
			m_fCooldownTimeRemaining -= 1.0;
		}
		else
		{
			m_bIsOnCooldown = false;
			m_fCooldownTimeRemaining = 0.0;
			Print("Server RP: Scavenge object cooldown expired. Ready for salvage.");
		}
	}
}
