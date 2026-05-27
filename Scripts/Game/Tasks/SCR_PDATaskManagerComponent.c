enum EStalkerTaskType
{
	ASSASSINATION,
	FETCH,
	FACTION_ASSAULT
}

// Inherit from Reforger's base task so it integrates natively
class SCR_StalkerBaseTask : SCR_BaseTask
{
	[Attribute("1000", desc: "Reward in RU upon completion")]
	protected int m_iRewardRU;
	
	[Attribute("0", desc: "Task type mapping")]
	protected EStalkerTaskType m_eTaskType;
	
	protected IEntity m_TargetEntity;

	int GetRewardRU()
	{
		return m_iRewardRU;
	}
	
	EStalkerTaskType GetStalkerTaskType()
	{
		return m_eTaskType;
	}
	
	void SetTargetEntity(IEntity target)
	{
		m_TargetEntity = target;
	}

	// This method hooks into vanilla task completion logic
	override void Finish(bool showMsg = true)
	{
		super.Finish(showMsg);
		
		// Runs on the server to award currency
		if (Replication.IsServer())
		{
			// Find the assignee or owning player controller
			IEntity assignee = GetAssignee();
			if (!assignee)
			{
				// Fallback to local controlled player if assignee is not set
				array<int> players = new array<int>();
				GetGame().GetPlayerManager().GetPlayers(players);
				if (players.Count() > 0)
				{
					PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(players[0]);
					if (pc) assignee = pc.GetControlledEntity();
				}
			}
			
			if (assignee)
			{
				PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(assignee));
				if (pc)
				{
					SCR_PDATaskManagerComponent mgr = SCR_PDATaskManagerComponent.Cast(pc.FindComponent(SCR_PDATaskManagerComponent));
					if (mgr)
					{
						mgr.AddPlayerRuBalance(m_iRewardRU);
					}
				}
			}
		}
	}
}

class SCR_PDATaskManagerComponentClass : ScriptComponentClass
{
}

class SCR_PDATaskManagerComponent : ScriptComponent
{
	[RplProp()]
	protected int m_iPlayerRU = 0;

	// Fetches the dynamic instance specific to the local player controller ( multiplayer safe )
	static SCR_PDATaskManagerComponent GetInstance()
	{
		PlayerController pc = GetGame().GetPlayerController();
		if (!pc) return null;
		return SCR_PDATaskManagerComponent.Cast(pc.FindComponent(SCR_PDATaskManagerComponent));
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		// If running on a client controller, request a dynamic mission after load
		if (Replication.IsClient())
		{
			GetGame().GetCallqueue().CallLater(RequestDynamicMission, 5000, false);
		}
	}

	// Client calls this to request a dynamic mission, which triggers a server RPC
	void RequestDynamicMission()
	{
		if (Replication.IsClient())
		{
			Rpc(RpcServer_RequestDynamicMission);
		}
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcServer_RequestDynamicMission()
	{
		SCR_GammaMissionManager missionMgr = SCR_GammaMissionManager.GetInstance();
		if (!missionMgr) 
		{
			Print("Server Error: No Gamma Mission Manager found in world! Cannot spawn tasks.");
			return;
		}

		PlayerController pc = PlayerController.Cast(GetOwner());
		if (!pc) return;

		IEntity controlledChar = pc.GetControlledEntity();
		if (!controlledChar) return;

		vector pOrigin = controlledChar.GetOrigin();
		
		// Generate map target on the server
		missionMgr.CreateDynamicMission(pOrigin, EStalkerTaskType.FACTION_ASSAULT);
		
		Print("Server: Dynamic Mission generated for player " + pc.GetPlayerId());
	}
	
	void AddPlayerRuBalance(int amount)
	{
		if (!Replication.IsServer()) return;
		
		m_iPlayerRU += amount;
		Replication.BumpMe();
		Print("Server: Balance Updated for player. Total RU: " + m_iPlayerRU);
	}
	
	int GetPlayerRuBalance()
	{
		return m_iPlayerRU;
	}
}
