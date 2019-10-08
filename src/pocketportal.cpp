/* Credits: Micrah/Milestorme Module creator */

#include "Define.h"
#include "GossipDef.h"
#include "Item.h"
#include "Player.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "Configuration/Config.h"
#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "Map.h"

enum Vendors
{
    NPC_VENDOR_A    = 128,
    NPC_VENDOR_H    = 128,
};



class pocket_portal : public ItemScript
{
public:
    pocket_portal() : ItemScript("pocket_portal") { }

    bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/) override
    {
        if (!sConfigMgr->GetBoolDefault("PocketPortal.Enable", true)) 
            return false;  

        if (player->IsInCombat() || player->GetMap()->IsBattleArena() || player->InBattleground())
           return false;


        player->PlayerTalkClass->ClearMenus();

        if (sConfigMgr->GetBoolDefault("PocketPortal.Enable", true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Pocket Portal", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, item->GetGUID());

        return false; // Cast the spell on use normally
    }
void OnGossipSelect(Player* player, Item* /*item*/, uint32 /*sender*/, uint32 action) override
    {
        switch (action)
        {
             case GOSSIP_ACTION_INFO_DEF + 1: /*portal*/
            {
                uint32 vendorId = 0;
                std::string salute;
                if (player->GetTeamId() == TEAM_ALLIANCE)
                {
                    vendorId = NPC_VENDOR_A;
                    salute = "Greetings";
                } else {
                    vendorId = NPC_VENDOR_H;
                    salute = "Zug zug";
                }

                SummonTempNPC(player, vendorId, salute.c_str());
                player->CLOSE_GOSSIP_MENU();
                break;
            }
			
		}
    }
			
 
    void SummonTempNPC(Player* player, uint32 entry, const char* salute = "")
    {
        if (!player || entry == 0)
            return;

        int npcDuration = sConfigMgr->GetIntDefault("Portal.NpcDuration", 60) * IN_MILLISECONDS;
        if (npcDuration <= 0) // Safeguard
            npcDuration = 60;

        Creature* npc = player->SummonCreature(entry, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, npcDuration);
        npc->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        npc->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, player->GetFollowAngle());
        npc->setFaction(player->getFaction());

		if (salute && !(salute[0] == '\0'))
            npc->MonsterWhisper(salute, player, false);
    }
};


class pportal_world : public WorldScript
{
public:
    pportal_world() : WorldScript("pportal_world") { }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) 
        {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/pocketportal.conf";            
            
#if PLATFORM == PLATFORM_WINDOWS
            cfg_file = "pocketportal.conf";
#endif
            std::string cfg_def_file = cfg_file + ".dist";
            
            // Load .conf.dist config
            if (!sConfigMgr->LoadMore(cfg_def_file.c_str()))
            {
                sLog->outString();
                sLog->outError("Module config: Invalid or missing configuration dist file : %s", cfg_def_file.c_str());
                sLog->outError("Module config: Verify that the file exists and has \'[worldserver]' written in the top of the file!");
                sLog->outError("Module config: Use default settings!");
                sLog->outString();
            }
            
            // Load .conf config
            if (!sConfigMgr->LoadMore(cfg_file.c_str()))
            {
                sLog->outString();
                sLog->outError("Module config: Invalid or missing configuration file : %s", cfg_file.c_str());
                sLog->outError("Module config: Verify that the file exists and has \'[worldserver]' written in the top of the file!");
                sLog->outError("Module config: Use default settings!");
                sLog->outString();
            }
        }
    }
};

void AddSC_pocket_portal()
{
    new pocket_portal();
    new pportal_world();
}
