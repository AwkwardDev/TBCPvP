 /*
  * Copyright (C) 2010-2012 Project SkyFire <http://www.projectskyfire.org/>
  * Copyright (C) 2010-2012 Oregon <http://www.oregoncore.com/>
  * Copyright (C) 2006-2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
  * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
  *
  * This program is free software; you can redistribute it and/or modify it
  * under the terms of the GNU General Public License as published by the
  * Free Software Foundation; either version 2 of the License, or (at your
  * option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but WITHOUT
  * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
  * more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program. If not, see <http://www.gnu.org/licenses/>.
  */

/* ScriptData
SDName: Terokkar_Forest
SD%Complete: 80
SDComment: Quest support: 9889, 10009, 10873, 10896, 11096, 10052, 10051, 10898, 10446/10447, 10852, 10887, 10922, 11085, Skettis->Ogri'la Flight
SDCategory: Terokkar Forest
EndScriptData */

/* ContentData
mob_unkor_the_ruthless
mob_infested_root_walker
mob_rotting_forest_rager
mob_netherweb_victim
npc_floon
npc_isla_starmane
npc_skyguard_prisoner
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*######
## mob_unkor_the_ruthless
######*/

enum eUnkor
{
    SAY_SUBMIT                  = -1000194,

    FACTION_HOSTILE             = 45,
    FACTION_FRIENDLY            = 35,
    QUEST_DONTKILLTHEFATONE     = 9889,

    SPELL_PULVERIZE             = 2676
    //SPELL_QUID9889            = 32174
};

struct mob_unkor_the_ruthlessAI : public ScriptedAI
{
    mob_unkor_the_ruthlessAI(Creature* c) : ScriptedAI(c) {}

    bool CanDoQuest;
    uint32 UnkorUnfriendly_Timer;
    uint32 Pulverize_Timer;

    void Reset()
    {
        CanDoQuest = false;
        UnkorUnfriendly_Timer = 0;
        Pulverize_Timer = 3000;
        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->setFaction(FACTION_HOSTILE);
    }

    void EnterCombat(Unit * /*who*/) {}

    void DoNice()
    {
        DoScriptText(SAY_SUBMIT, me);
        me->setFaction(FACTION_FRIENDLY);
        me->SetStandState(UNIT_STAND_STATE_SIT);
        me->RemoveAllAuras();
        me->DeleteThreatList();
        me->CombatStop();
        UnkorUnfriendly_Timer = 60000;
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (done_by->GetTypeId() == TYPEID_PLAYER)
            if ((me->GetHealth()-damage)*100 / me->GetMaxHealth() < 30)
        {
            if (Group* pGroup = CAST_PLR(done_by)->GetGroup())
            {
                for (GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                {
                    Player *pGroupie = itr->getSource();
                    if (pGroupie &&
                        pGroupie->GetQuestStatus(QUEST_DONTKILLTHEFATONE) == QUEST_STATUS_INCOMPLETE &&
                        pGroupie->GetReqKillOrCastCurrentCount(QUEST_DONTKILLTHEFATONE, 18260) == 10)
                    {
                        pGroupie->AreaExploredOrEventHappens(QUEST_DONTKILLTHEFATONE);
                        if (!CanDoQuest)
                            CanDoQuest = true;
                    }
                }
            } else
            if (CAST_PLR(done_by)->GetQuestStatus(QUEST_DONTKILLTHEFATONE) == QUEST_STATUS_INCOMPLETE &&
                CAST_PLR(done_by)->GetReqKillOrCastCurrentCount(QUEST_DONTKILLTHEFATONE, 18260) == 10)
            {
                CAST_PLR(done_by)->AreaExploredOrEventHappens(QUEST_DONTKILLTHEFATONE);
                CanDoQuest = true;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (CanDoQuest)
        {
            if (!UnkorUnfriendly_Timer)
            {
                //DoCast(me, SPELL_QUID9889);        //not using spell for now
                DoNice();
            }
            else
            {
                if (UnkorUnfriendly_Timer <= diff)
                {
                    EnterEvadeMode();
                    return;
                } else UnkorUnfriendly_Timer -= diff;
            }
        }

        if (!UpdateVictim())
            return;

        if (Pulverize_Timer <= diff)
        {
            DoCast(me, SPELL_PULVERIZE);
            Pulverize_Timer = 9000;
        } else Pulverize_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_unkor_the_ruthless(Creature* creature)
{
    return new mob_unkor_the_ruthlessAI (creature);
}

/*######
## mob_infested_root_walker
######*/

struct mob_infested_root_walkerAI : public ScriptedAI
{
    mob_infested_root_walkerAI(Creature *c) : ScriptedAI(c) {}

    void Reset() { }
    void EnterCombat(Unit *who) { }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (done_by && done_by->GetTypeId() == TYPEID_PLAYER)
            if (me->GetHealth() <= damage)
                if (rand()%100 < 75)
                    //Summon Wood Mites
                    me->CastSpell(me, 39130, true);
    }
};
CreatureAI* GetAI_mob_infested_root_walker(Creature* creature)
{
    return new mob_infested_root_walkerAI (creature);
}

/*######
## mob_rotting_forest_rager
######*/

struct mob_rotting_forest_ragerAI : public ScriptedAI
{
    mob_rotting_forest_ragerAI(Creature *c) : ScriptedAI(c) {}

    void Reset() { }
    void EnterCombat(Unit *who) { }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (done_by->GetTypeId() == TYPEID_PLAYER)
            if (me->GetHealth() <= damage)
                if (rand()%100 < 75)
                    //Summon Lots of Wood Mights
                    me->CastSpell(me, 39134, true);
    }
};
CreatureAI* GetAI_mob_rotting_forest_rager(Creature* creature)
{
    return new mob_rotting_forest_ragerAI (creature);
}

/*######
## mob_netherweb_victim
######*/

#define QUEST_TARGET        22459
//#define SPELL_FREE_WEBBED   38950

const uint32 netherwebVictims[6] =
{
    18470, 16805, 21242, 18452, 22482, 21285
};
struct mob_netherweb_victimAI : public ScriptedAI
{
    mob_netherweb_victimAI(Creature *c) : ScriptedAI(c) {}

    void Reset() { }
    void EnterCombat(Unit *who) { }
    void MoveInLineOfSight(Unit *who) { }

    void JustDied(Unit* Killer)
    {
        if (Killer->GetTypeId() == TYPEID_PLAYER)
        {
            if (CAST_PLR(Killer)->GetQuestStatus(10873) == QUEST_STATUS_INCOMPLETE)
            {
                if (rand()%100 < 25)
                {
                    DoSpawnCreature(QUEST_TARGET, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    CAST_PLR(Killer)->KilledMonsterCredit(QUEST_TARGET, me->GetGUID());
                } else
                DoSpawnCreature(netherwebVictims[rand()%6],0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);

                if (rand()%100 < 75)
                    DoSpawnCreature(netherwebVictims[rand()%6],0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                DoSpawnCreature(netherwebVictims[rand()%6],0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            }
        }
    }
};
CreatureAI* GetAI_mob_netherweb_victim(Creature* creature)
{
    return new mob_netherweb_victimAI (creature);
}

/*######
## npc_floon
######*/

#define GOSSIP_FLOON1           "You owe Sim'salabim money. Hand them over or die!"
#define GOSSIP_FLOON2           "Hand over the money or die...again!"

enum eFloon
{
    SAY_FLOON_ATTACK        = -1000195,

    FACTION_HOSTILE_FL      = 1738,
    FACTION_FRIENDLY_FL     = 35,

    SPELL_SILENCE           = 6726,
    SPELL_FROSTBOLT         = 9672,
    SPELL_FROST_NOVA        = 11831
};

struct npc_floonAI : public ScriptedAI
{
    npc_floonAI(Creature* c) : ScriptedAI(c) {}

    uint32 Silence_Timer;
    uint32 Frostbolt_Timer;
    uint32 FrostNova_Timer;

    void Reset()
    {
        Silence_Timer = 2000;
        Frostbolt_Timer = 4000;
        FrostNova_Timer = 9000;
        me->setFaction(FACTION_FRIENDLY_FL);
    }

    void EnterCombat(Unit *who) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Silence_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_SILENCE);
            Silence_Timer = 30000;
        } else Silence_Timer -= diff;

        if (FrostNova_Timer <= diff)
        {
            DoCast(me, SPELL_FROST_NOVA);
            FrostNova_Timer = 20000;
        } else FrostNova_Timer -= diff;

        if (Frostbolt_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_FROSTBOLT);
            Frostbolt_Timer = 5000;
        } else Frostbolt_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_floon(Creature* creature)
{
    return new npc_floonAI (creature);
}

bool GossipHello_npc_floon(Player* player, Creature* creature)
{
    if (player->GetQuestStatus(10009) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(1, GOSSIP_FLOON1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(9442, creature->GetGUID());
    return true;
}

bool GossipSelect_npc_floon(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF)
    {
        player->ADD_GOSSIP_ITEM(1, GOSSIP_FLOON2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->SEND_GOSSIP_MENU(9443, creature->GetGUID());
    }
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        player->CLOSE_GOSSIP_MENU();
        creature->setFaction(FACTION_HOSTILE_FL);
        DoScriptText(SAY_FLOON_ATTACK, creature, player);
        ((npc_floonAI*)creature->AI())->AttackStart(player);
    }
    return true;
}

/*######
## npc_isla_starmane
######*/

enum eIsla
{
    SAY_PROGRESS_1  = -1000353,
    SAY_PROGRESS_2  = -1000354,
    SAY_PROGRESS_3  = -1000355,
    SAY_PROGRESS_4  = -1000356,

    QUEST_EFTW_H    = 10052,
    QUEST_EFTW_A    = 10051,
    GO_CAGE         = 182794,
    SPELL_CAT       = 32447
};

struct npc_isla_starmaneAI : public npc_escortAI
{
    npc_isla_starmaneAI(Creature* c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* player = GetPlayerForEscort();

        if (!player)
            return;

        switch (i)
        {
        case 0:
            {
            GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 10);
            if (Cage)
                Cage->SetGoState(GO_STATE_ACTIVE);
            }
            break;
        case 2: DoScriptText(SAY_PROGRESS_1, me, player); break;
        case 5: DoScriptText(SAY_PROGRESS_2, me, player); break;
        case 6: DoScriptText(SAY_PROGRESS_3, me, player); break;
        case 29:DoScriptText(SAY_PROGRESS_4, me, player);
            if (player)
            {
                if (player->GetTeam() == ALLIANCE)
                    player->GroupEventHappens(QUEST_EFTW_A, me);
                else if (player->GetTeam() == HORDE)
                    player->GroupEventHappens(QUEST_EFTW_H, me);
            }
            me->SetInFront(player); break;
        case 30: me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE); break;
        case 31: DoCast(me, SPELL_CAT);
            me->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE); break;
        }
    }

    void Reset()
    {
        me->RestoreFaction();
    }

    void JustDied(Unit* killer)
    {
        if (Player* player = GetPlayerForEscort())
        {
            if (player->GetTeam() == ALLIANCE)
                player->FailQuest(QUEST_EFTW_A);
            else if (player->GetTeam() == HORDE)
                player->FailQuest(QUEST_EFTW_H);
        }
    }
};

bool QuestAccept_npc_isla_starmane(Player* player, Creature* creature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_EFTW_H || quest->GetQuestId() == QUEST_EFTW_A)
    {
        CAST_AI(npc_escortAI, (creature->AI()))->Start(true, false, player->GetGUID());
        creature->setFaction(113);
    }
    return true;
}

CreatureAI* GetAI_npc_isla_starmaneAI(Creature* creature)
{
    return  new npc_isla_starmaneAI(creature);
}

/*######
## go_skull_pile
######*/
#define GOSSIP_S_DARKSCREECHER_AKKARAI   "<Call forth Darkscreecher Akkarai>"
#define GOSSIP_S_KARROG                  "<Call forth Karrog>"
#define GOSSIP_S_GEZZARAK_THE_HUNTRESS   "<Call forth Gezzarak the Huntress>"
#define GOSSIP_S_VAKKIZ_THE_WINDRAGER    "<Call forth Vakkiz the Windrager>"

bool GossipHello_go_skull_pile(Player* player, GameObject* _GO)
{
    if ((player->GetQuestStatus(11885) == QUEST_STATUS_INCOMPLETE) || player->GetQuestRewardStatus(11885))
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_DARKSCREECHER_AKKARAI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_KARROG, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_GEZZARAK_THE_HUNTRESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_VAKKIZ_THE_WINDRAGER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    }

    player->SEND_GOSSIP_MENU(10888, _GO->GetGUID());
    return true;
}

void SendActionMenu_go_skull_pile(Player* player, GameObject* _GO, uint32 action)
{
    // Despawn the GameObject
    if (player->HasItemCount(32620, 10))
    {
        _GO->SetGoState(GO_STATE_ACTIVE);
        _GO->SetRespawnTime(600);
        player->CLOSE_GOSSIP_MENU();
    }

    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
              player->CastSpell(player, 40642, false);
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
              player->CastSpell(player, 40640, false);
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
              player->CastSpell(player, 40632, false);
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
              player->CastSpell(player, 40644, false);
            break;
    }
}

bool GossipSelect_go_skull_pile(Player* player, GameObject* _GO, uint32 sender, uint32 action)
{
    switch (sender)
    {
        case GOSSIP_SENDER_MAIN:    SendActionMenu_go_skull_pile(player, _GO, action); break;
    }
    return true;
}

/*######
## npc_skywing
######*/

enum
{
    QUEST_SKYWING         = 10898,

    NPC_LUANGA_IMPRISONER = 18533
};

struct npc_skywingAI : public npc_escortAI
{
    npc_skywingAI(Creature *c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* player = GetPlayerForEscort();
        if (!player)
            return;

        switch (i)
        {
        case 7:
            me->SummonCreature(NPC_LUANGA_IMPRISONER, -3399.274658, 4055.948975, 18.603474, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            break;
        case 8:
            player->AreaExploredOrEventHappens(10898);
            break;
        }
    }

    void EnterCombat(Unit* /*who*/) {}

    void Reset() {}

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(me);
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_npc_skywingAI(Creature* creature)
{
    return new npc_skywingAI(creature);
}

bool QuestAccept_npc_skywing(Player* player, Creature* creature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_SKYWING)
    {
        if (npc_skywingAI* pEscortAI = CAST_AI(npc_skywingAI, creature->AI()))
            pEscortAI->Start(false, true, player->GetGUID(), quest);
    }

    return true;
}

/*######
## npc_slim
######*/

enum eSlim
{
    FACTION_CONSORTIUM  = 933
};

bool GossipHello_npc_slim(Player* player, Creature* creature)
{
    if (creature->isVendor() && player->GetReputationRank(FACTION_CONSORTIUM) >= REP_FRIENDLY)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        player->SEND_GOSSIP_MENU(9896, creature->GetGUID());
    }
    else
        player->SEND_GOSSIP_MENU(9895, creature->GetGUID());

    return true;
}

bool GossipSelect_npc_slim(Player* player, Creature* creature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        player->SEND_VENDORLIST(creature->GetGUID());

    return true;
}

/*########
####npc_akuno
#####*/

enum
{
    SAY_AKU_START           = -1000477,
    SAY_AKU_AMBUSH_A        = -1000478,
    SAY_AKU_AMBUSH_B        = -1000479,
    SAY_AKU_AMBUSH_B_REPLY  = -1000480,
    SAY_AKU_COMPLETE        = -1000481,

    SPELL_CHAIN_LIGHTNING   = 39945,

    QUEST_ESCAPING_TOMB     = 10887,
    NPC_CABAL_SKIRMISHER    = 21661
};

static float m_afAmbushB1[]= {-2895.525879f, 5336.431641f, -11.800f};
static float m_afAmbushB2[]= {-2890.604980f, 5331.938965f, -11.282f};

struct npc_akunoAI : public npc_escortAI
{
    npc_akunoAI(Creature* creature) : npc_escortAI(creature) { Reset(); }

    uint32 m_uiChainLightningTimer;

    void Reset()
    {
        m_uiChainLightningTimer = 1000;
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
            case 5:
                DoScriptText(SAY_AKU_AMBUSH_A, me);
               me->SummonCreature(NPC_CABAL_SKIRMISHER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                break;
            case 14:
                DoScriptText(SAY_AKU_AMBUSH_B, me);

                if (Creature* pTemp =me->SummonCreature(NPC_CABAL_SKIRMISHER, m_afAmbushB1[0], m_afAmbushB1[1], m_afAmbushB1[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000))
                    DoScriptText(SAY_AKU_AMBUSH_B_REPLY, pTemp);

               me->SummonCreature(NPC_CABAL_SKIRMISHER, m_afAmbushB2[0], m_afAmbushB2[1], m_afAmbushB2[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                break;
            case 15:
                SetRun();
                break;
            case 18:
                DoScriptText(SAY_AKU_COMPLETE, me);

                if (Player* player = GetPlayerForEscort())
                    player->GroupEventHappens(QUEST_ESCAPING_TOMB, me);

                break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(me);
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

        if (m_uiChainLightningTimer < uiDiff)
        {
            DoCast(me->getVictim(), SPELL_CHAIN_LIGHTNING);
            m_uiChainLightningTimer = urand(7000, 14000);
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_akuno(Player* player, Creature* creature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPING_TOMB)
    {
        if (npc_akunoAI* pEscortAI = CAST_AI(npc_akunoAI, creature->AI()))
        {
            creature->SetStandState(UNIT_STAND_STATE_STAND);
            creature->setFaction(FACTION_ESCORT_N_NEUTRAL_ACTIVE);

            DoScriptText(SAY_AKU_START, creature);
            pEscortAI->Start(false, false, player->GetGUID(), pQuest);
        }
    }
    return true;
}

CreatureAI* GetAI_npc_akuno(Creature* creature)
{
    return new npc_akunoAI(creature);
}

/*######
## npc_skyguard_handler_deesak
######*/

#define GOSSIP_DEESAK "Absolutely! Send me to the Skyguard Outpost."

bool GossipHello_npc_skyguard_handler_deesak(Player* player, Creature* creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetGUID());

    uint32 textEntry = 0;
    if (player->GetReputationRank(1031) >= REP_EXALTED) // Sha'tari Skyguard
    {
        textEntry = 10979;
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DEESAK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }
    else
        textEntry = 10980;

    player->SEND_GOSSIP_MENU(textEntry, creature->GetGUID());
    return true;
}

bool GossipSelect_npc_skyguard_handler_deesak(Player* player, Creature* creature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
        player->CastSpell(player, 41279, true);

    return true;
}

/*######
## npc_letoll
######*/

enum
{
    SAY_LE_START                    = -1000511,
    SAY_LE_KEEP_SAFE                = -1000512,
    SAY_LE_NORTH                    = -1000513,
    SAY_LE_ARRIVE                   = -1000514,
    SAY_LE_BURIED                   = -1000515,
    SAY_LE_ALMOST                   = -1000516,
    SAY_LE_DRUM                     = -1000517,
    SAY_LE_DRUM_REPLY               = -1000518,
    SAY_LE_DISCOVERY                = -1000519,
    SAY_LE_DISCOVERY_REPLY          = -1000520,
    SAY_LE_NO_LEAVE                 = -1000521,
    SAY_LE_NO_LEAVE_REPLY1          = -1000522,
    SAY_LE_NO_LEAVE_REPLY2          = -1000523,
    SAY_LE_NO_LEAVE_REPLY3          = -1000524,
    SAY_LE_NO_LEAVE_REPLY4          = -1000525,
    SAY_LE_SHUT                     = -1000526,
    SAY_LE_REPLY_HEAR               = -1000527,
    SAY_LE_IN_YOUR_FACE             = -1000528,
    SAY_LE_HELP_HIM                 = -1000529,
    EMOTE_LE_PICK_UP                = -1000530,
    SAY_LE_THANKS                   = -1000531,

    QUEST_DIGGING_BONES             = 10922,

    NPC_RESEARCHER                  = 22464,
    NPC_BONE_SIFTER                 = 22466,

    MAX_RESEARCHER                  = 4
};

//Some details still missing from here, and will also have issues if followers evade for any reason.
struct npc_letollAI : public npc_escortAI
{
    npc_letollAI(Creature* creature) : npc_escortAI(creature)
    {
        m_uiEventTimer = 5000;
        m_uiEventCount = 0;
        Reset();
    }

    std::list<Creature*> m_lResearchersList;

    uint32 m_uiEventTimer;
    uint32 m_uiEventCount;

    void Reset() {}

    //will make them follow, but will only work until they enter combat with any unit
    void SetFormation()
    {
        uint32 uiCount = 0;

        for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
        {
            float fAngle = uiCount < MAX_RESEARCHER ? M_PI/MAX_RESEARCHER - (uiCount*2*M_PI/MAX_RESEARCHER) : 0.0f;

            if ((*itr)->isAlive() && !(*itr)->isInCombat())
                (*itr)->GetMotionMaster()->MoveFollow(me, 2.5f, fAngle);

            ++uiCount;
        }
    }

    Creature* GetAvailableResearcher(uint8 uiListNum)
    {
        if (!m_lResearchersList.empty())
        {
            uint8 uiNum = 1;

            for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
            {
                if (uiListNum && uiListNum != uiNum)
                {
                    ++uiNum;
                    continue;
                }

                if ((*itr)->isAlive() && (*itr)->IsWithinDistInMap(me, 20.0f))
                    return (*itr);
            }
        }

        return NULL;
    }

    void JustStartedEscort()
    {
        m_uiEventTimer = 5000;
        m_uiEventCount = 0;

        m_lResearchersList.clear();

        me->GetCreatureListWithEntryInGrid(m_lResearchersList, NPC_RESEARCHER, 25.0f);

        if (!m_lResearchersList.empty())
            SetFormation();
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
            case 0:
                JustStartedEscort();
                for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
                {
                    (*itr)->SetUnitMovementFlags(MOVEFLAG_WALK_MODE);
                }
                if (Player* player = GetPlayerForEscort())
                    DoScriptText(SAY_LE_KEEP_SAFE, me, player);
                break;
            case 1:
                DoScriptText(SAY_LE_NORTH, me);
                break;
            case 10:
                DoScriptText(SAY_LE_ARRIVE, me);
                break;
            case 12:
                DoScriptText(SAY_LE_BURIED, me);
                SetEscortPaused(true);
                break;
            case 13:
                SetRun();
                for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
                {
                    (*itr)->SetUnitMovementFlags(MOVEFLAG_NONE);
                }
                break;
        }
    }

    void Aggro(Unit* pWho)
    {
        if (pWho->isInCombat() && pWho->GetTypeId() == TYPEID_UNIT && pWho->GetEntry() == NPC_BONE_SIFTER)
            DoScriptText(SAY_LE_HELP_HIM, me);
    }

    void JustSummoned(Creature* pSummoned)
    {
        Player* player = GetPlayerForEscort();

        if (player && player->isAlive())
            pSummoned->AI()->AttackStart(player);
        else
            pSummoned->AI()->AttackStart(me);
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
        {
            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (m_uiEventTimer < uiDiff)
                {
                    m_uiEventTimer = 7000;

                    switch (m_uiEventCount)
                    {
                        case 0:
                            DoScriptText(SAY_LE_ALMOST, me);
                            break;
                        case 1:
                            DoScriptText(SAY_LE_DRUM, me);
                            break;
                        case 2:
                            if (Creature* pResearcher = GetAvailableResearcher(0))
                                DoScriptText(SAY_LE_DRUM_REPLY, pResearcher);
                            break;
                        case 3:
                            DoScriptText(SAY_LE_DISCOVERY, me);
                            break;
                        case 4:
                            if (Creature* pResearcher = GetAvailableResearcher(0))
                                DoScriptText(SAY_LE_DISCOVERY_REPLY, pResearcher);
                            break;
                        case 5:
                            DoScriptText(SAY_LE_NO_LEAVE, me);
                            break;
                        case 6:
                            if (Creature* pResearcher = GetAvailableResearcher(1))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY1, pResearcher);
                            break;
                        case 7:
                            if (Creature* pResearcher = GetAvailableResearcher(2))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY2, pResearcher);
                            break;
                        case 8:
                            if (Creature* pResearcher = GetAvailableResearcher(3))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY3, pResearcher);
                            break;
                        case 9:
                            if (Creature* pResearcher = GetAvailableResearcher(4))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY4, pResearcher);
                            break;
                        case 10:
                            DoScriptText(SAY_LE_SHUT, me);
                            break;
                        case 11:
                            if (Creature* pResearcher = GetAvailableResearcher(0))
                                DoScriptText(SAY_LE_REPLY_HEAR, pResearcher);
                            break;
                        case 12:
                            DoScriptText(SAY_LE_IN_YOUR_FACE, me);
                            me->SummonCreature(NPC_BONE_SIFTER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                            break;
                        case 13:
                            DoScriptText(EMOTE_LE_PICK_UP, me);

                            if (Player* player = GetPlayerForEscort())
                            {
                                DoScriptText(SAY_LE_THANKS, me, player);
                                player->GroupEventHappens(QUEST_DIGGING_BONES, me);
                            }

                            SetEscortPaused(false);
                            break;
                    }

                    ++m_uiEventCount;
                }
                else
                    m_uiEventTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_letoll(Creature* creature)
{
    return new npc_letollAI(creature);
}

bool QuestAccept_npc_letoll(Player* player, Creature* creature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_DIGGING_BONES)
    {
        if (npc_letollAI* pEscortAI = dynamic_cast<npc_letollAI*>(creature->AI()))
        {
            DoScriptText(SAY_LE_START, creature);
            creature->setFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);

            pEscortAI->Start(false, false, player->GetGUID(), pQuest, true);
        }
    }

    return true;
}

/*######
## npc_mana_bomb_exp_trigger
######*/

enum
{
    SAY_COUNT_1                 = -1000472,
    SAY_COUNT_2                 = -1000473,
    SAY_COUNT_3                 = -1000474,
    SAY_COUNT_4                 = -1000475,
    SAY_COUNT_5                 = -1000476,

    SPELL_MANA_BOMB_LIGHTNING   = 37843,
    SPELL_MANA_BOMB_EXPL        = 35513,

    NPC_MANA_BOMB_EXPL_TRIGGER  = 20767,
    NPC_MANA_BOMB_KILL_TRIGGER  = 21039
};

struct npc_mana_bomb_exp_triggerAI : public ScriptedAI
{
    npc_mana_bomb_exp_triggerAI(Creature* creature) : ScriptedAI(creature) { Reset(); }

    GameObject* pManaBomb;

    bool m_bIsActivated;
    uint32 m_uiEventTimer;
    uint32 m_uiEventCounter;

    void Reset()
    {
        pManaBomb = NULL;
        m_bIsActivated = false;
        m_uiEventTimer = 1000;
        m_uiEventCounter = 0;
    }

    void DoTrigger(Player* player, GameObject* pGo)
    {
        if (m_bIsActivated)
            return;

        m_bIsActivated = true;

        player->KilledMonsterCredit(NPC_MANA_BOMB_KILL_TRIGGER, me->GetGUID());

        pManaBomb = pGo;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bIsActivated)
            return;

        if (m_uiEventTimer < uiDiff)
        {
            m_uiEventTimer = 1000;

            if (m_uiEventCounter < 10)
                me->CastSpell(me, SPELL_MANA_BOMB_LIGHTNING, false);

            switch (m_uiEventCounter)
            {
                case 5:
                    if (pManaBomb)
                        pManaBomb->SetGoState(GO_STATE_ACTIVE);

                    DoScriptText(SAY_COUNT_1, me);
                    break;
                case 6:
                    DoScriptText(SAY_COUNT_2, me);
                    break;
                case 7:
                    DoScriptText(SAY_COUNT_3, me);
                    break;
                case 8:
                    DoScriptText(SAY_COUNT_4, me);
                    break;
                case 9:
                    DoScriptText(SAY_COUNT_5, me);
                    break;
                case 10:
                    me->CastSpell(me, SPELL_MANA_BOMB_EXPL, false);
                    break;
                case 30:
                    if (pManaBomb)
                        pManaBomb->SetGoState(GO_STATE_READY);

                    Reset();
                    break;
            }

            ++m_uiEventCounter;
        }
        else
            m_uiEventTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_mana_bomb_exp_trigger(Creature* creature)
{
    return new npc_mana_bomb_exp_triggerAI(creature);
}

/*######
## go_mana_bomb
######*/

bool GOHello_go_mana_bomb(Player* player, GameObject* pGo)
{
    if (Creature* creature = GetClosestCreatureWithEntry(pGo, NPC_MANA_BOMB_EXPL_TRIGGER, INTERACTION_DISTANCE))
    {
        if (npc_mana_bomb_exp_triggerAI* pBombAI = dynamic_cast<npc_mana_bomb_exp_triggerAI*>(creature->AI()))
            pBombAI->DoTrigger(player, pGo);
    }

    return true;
}

/*#####
## go_veil_skith_cage & npc_captive_child
#####*/

enum
{
    QUEST_MISSING_FRIENDS     = 10852,
    NPC_CAPTIVE_CHILD         = 22314,
    SAY_THANKS_1              = -1000590,
    SAY_THANKS_2              = -1000591,
    SAY_THANKS_3              = -1000592,
    SAY_THANKS_4              = -1000593
};

bool GOHello_veil_skith_cage(Player* player, GameObject* pGo)
{
    if (player->GetQuestStatus(QUEST_MISSING_FRIENDS) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* pChild = pGo->FindNearestCreature( NPC_CAPTIVE_CHILD, 5, true))
        {
            player->KilledMonsterCredit(NPC_CAPTIVE_CHILD, pChild->GetGUID());

            switch (urand(0, 3))
            {
                case 0: DoScriptText(SAY_THANKS_1, pChild); break;
                case 1: DoScriptText(SAY_THANKS_2, pChild); break;
                case 2: DoScriptText(SAY_THANKS_3, pChild); break;
                case 3: DoScriptText(SAY_THANKS_4, pChild); break;
            }

            pChild->GetMotionMaster()->Clear();
            pChild->GetMotionMaster()->MovePoint(0, -2648.049f, 5274.573f, 1.691529f);
        }
    }
    return false;
};

struct npc_captive_child : public ScriptedAI
{
    npc_captive_child(Creature* creature) : ScriptedAI(creature) { Reset(); }

    void Reset() {}

    void WaypointReached(uint32 uiPointId)
    {
        // we only have one waypoint
        me->ForcedDespawn();
    }
};

CreatureAI* GetAI_npc_captive_child(Creature* creature)
{
    return new npc_captive_child(creature);
}

/*######
## npc_skyguard_prisoner
######*/

enum SPrisoner
{
    SAY_START                     = -1000716,
    SAY_DONT_LIKE                 = -1000717,
    SAY_COMPLETE                  = -1000718,

    GO_PRISONER_CAGE              = 185952,

    QUEST_ESCAPE_FROM_SKETTIS     = 11085,
};

struct npc_skyguard_prisonerAI : public npc_escortAI
{
    npc_skyguard_prisonerAI(Creature* creature) : npc_escortAI(creature) {}

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    uint32 CalculateWaypointID()
    {
        //TODO: we have 2 ways to calculate wich waypoint we need to use

        /*if (abs(me->GetPositionX() + 4108.25) < abs(me->GetPositionX() + 3718.81) &&
            abs(me->GetPositionX() + 4108.25) < abs(me->GetPositionX() + 3671.51))
            return 1;

        if (abs(me->GetPositionX() + 3718.81) < abs(me->GetPositionX() + 4108.25) &&
            abs(me->GetPositionX() + 3718.81) < abs(me->GetPositionX() + 3671.51))
            return 2;

        return 3;*/

        switch (me->GetGUIDLow())
        {
        case 1189307:
            return 1;
            break;
        case 1189309:
            return 2;
            break;
        default:
            return 3;
            break;
        }
    }

    void StartEvent(Player* player, const Quest* pQuest)
    {
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        switch (CalculateWaypointID())
        {
        case 1:
            AddWaypoint(0, -4108.25, 3032.18, 344.799, 3000);
            AddWaypoint(1, -4114.41, 3036.73, 344.039);
            AddWaypoint(2, -4126.41, 3026.07, 344.156);
            AddWaypoint(3, -4145.17, 3029.69, 337.423);
            AddWaypoint(4, -4173.69, 3035.72, 343.346);
            AddWaypoint(5, -4173.70, 3047.37, 343.888);
            AddWaypoint(6, -4183.47, 3060.62, 344.157, 3000);
            AddWaypoint(7, -4179.13, 3090.20, 323.971, 30000);
            Start(false, false, player->GetGUID(), pQuest);
            break;
        case 2:
            AddWaypoint(0, -3718.81, 3787.24, 302.890, 3000);
            AddWaypoint(1, -3714.44, 3780.35, 302.075);
            AddWaypoint(2, -3698.33, 3788.04, 302.171);
            AddWaypoint(3, -3679.36, 3780.25, 295.077);
            AddWaypoint(4, -3654.82, 3770.43, 301.291);
            AddWaypoint(5, -3656.07, 3757.31, 301.985);
            AddWaypoint(6, -3648.83, 3743.07, 302.173, 3000);
            AddWaypoint(7, -3659.16, 3714.94, 281.576, 30000);
            Start(false, false, player->GetGUID(), pQuest);
            break;
        case 3:
            AddWaypoint(0, -3671.51, 3385.36, 312.956, 3000);
            AddWaypoint(1, -3677.74, 3379.05, 312.136);
            AddWaypoint(2, -3667.52, 3366.45, 312.233);
            AddWaypoint(3, -3672.87, 3343.52, 304.994);
            AddWaypoint(4, -3679.35, 3319.01, 311.419);
            AddWaypoint(5, -3692.93, 3318.69, 312.081);
            AddWaypoint(6, -3704.08, 3309.56, 312.233, 3000);
            AddWaypoint(7, -3733.99, 3315.77, 292.093, 30000);
            Start(false, false, player->GetGUID(), pQuest);
            break;
        }
        return;
    }

    void WaypointReached(uint32 uiPointId)
    {
        if (Player* player = GetPlayerForEscort())
        {
            switch (uiPointId)
            {
            case 0:
                DoScriptText(SAY_START, me);
                break;
            case 6:
                DoScriptText(SAY_DONT_LIKE, me);
                break;
            case 7:
                DoScriptText(SAY_COMPLETE, me);
                if (Player* player = GetPlayerForEscort())
                    player->GroupEventHappens(QUEST_ESCAPE_FROM_SKETTIS, me);
                break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_escortAI::UpdateAI(uiDiff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_skyguard_prisoner(Player* player, Creature* creature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_FROM_SKETTIS)
    {
        if (GameObject* pGo = creature->FindNearestGameObject(GO_PRISONER_CAGE, 10.0f))
            pGo->UseDoorOrButton();

        if (npc_skyguard_prisonerAI* pEscortAI = CAST_AI(npc_skyguard_prisonerAI, creature->AI()))
            pEscortAI->StartEvent(player, pQuest);
    }
    return true;
}

CreatureAI* GetAI_npc_skyguard_prisoner(Creature* creature)
{
    return new npc_skyguard_prisonerAI(creature);
}

/*######
## npc_severin
######*/

#define ADARIS_SAY1 "The arakkoa are hidden... everywhere!"
#define ADARIS_SAY2 "The elixir... give it to me, Severin!"
#define ADARIS_SAY3 "I see them now... assassins from Skettis!"
#define ADARIS_SAY4 "The arakkoa can send all the assassins they have... they will meet the same fate as these!"
#define ADARIS_SAY5 "Is this... blood in my lungs? If I don't die to an assassin's blade, it'll be my old injuries that get me."
#define SEVERIN_SAY1 "Rest now, Adaris. You need to recover your strength."
#define SEVERIN_SAY2 "Adaris!"
#define SEVERIN_SAY3 "Rest now, friend. You need your strength."

enum eSeverin
{
    QUEST_WORLD_OF_SHADOWS = 11004,
    NPC_SKY_COMMANDER_ADARIS =  23038,
    NPC_SKETTIS_ASSASIN = 23207,

    EVENT_ACCEPT_QUEST = 1,
    EVENT_COMPLETE_QUEST = 2,

    SPELL_CLEAVE = 36664
};

struct npc_severinAI : public ScriptedAI
{
    npc_severinAI(Creature* creature) : ScriptedAI(creature) { }

    bool eventRunning;
    uint32 waitTimer;
    uint8 currentPhase;
    uint8 eventEntry;
    Creature* Adaris;
    Creature* assassinOne;
    Creature* assassinTwo;

    void Reset()
    {
        eventRunning = false;
        waitTimer = 0;
        currentPhase = 0;
        eventEntry = 0;
        Adaris = 0;
        assassinOne = 0;
        assassinTwo = 0;

        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void StartEvent(Player* pInvoker, uint32 eventId)
    {
        if (eventRunning)
            return;

        eventRunning = true;
        currentPhase = 1;
        eventEntry = eventId;

        me->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE);
        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (Adaris = me->FindNearestCreature(NPC_SKY_COMMANDER_ADARIS, 5.0f))
        {
            Adaris->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            Adaris->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!eventRunning)
            return;

        if (waitTimer)
        {
            if (waitTimer <= diff)
            {
                currentPhase++;
                waitTimer = 0;
            }
            else
            {
                waitTimer -= diff;
                return;
            }
        }

        if (currentPhase && eventEntry)
        {
            switch (eventEntry)
            {
                case EVENT_ACCEPT_QUEST:
                {
                    switch (currentPhase)
                    {
                        case 1:
                            waitTimer = 1000;
                            break;
                        case 2:
                            Adaris->MonsterSay(ADARIS_SAY1, LANG_UNIVERSAL, 0);
                            waitTimer = 5000;
                            break;
                        case 3:
                            me->MonsterSay(SEVERIN_SAY1, LANG_UNIVERSAL, 0);
                            me->SetStandState(UNIT_STAND_STATE_KNEEL);
                            waitTimer = 10000;
                            break;
                        case 4:
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            Reset();
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case EVENT_COMPLETE_QUEST:
                {
                    switch (currentPhase)
                    {
                        case 1:
                            waitTimer = 2000;
                            break;
                        case 2:
                            Adaris->MonsterSay(ADARIS_SAY2, LANG_UNIVERSAL, 0);
                            Adaris->SetStandState(UNIT_STAND_STATE_KNEEL);
                            waitTimer = 4000;
                            break;
                        case 3:
                            Adaris->SetStandState(UNIT_STAND_STATE_STAND);
                            waitTimer = 1000;
                            break;
                        case 4:
                            Adaris->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                            Adaris->MonsterSay(ADARIS_SAY3, LANG_UNIVERSAL, 0);
                            waitTimer = 7000;
                            break;
                        case 5:
                            Adaris->GetMotionMaster()->MovePoint(0, -3402.4777f, 3616.7644f, 277.0920f);
                            waitTimer = 2000;
                            break;
                        case 6:
                            me->GetMotionMaster()->MovePoint(0, -3401.7917f, 3619.8828f, 278.0560f);
                            me->MonsterYell(SEVERIN_SAY2, LANG_UNIVERSAL, 0);
                            assassinOne = Adaris->SummonCreature(NPC_SKETTIS_ASSASIN, -3401.7045f, 3614.4214f, 276.6273f, 2.3267f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000);
                            Adaris->CastSpell(Adaris, SPELL_CLEAVE, true);
                            Adaris->DealDamage(assassinOne, assassinOne->GetMaxHealth());
                            waitTimer = 4000;
                            break;
                        case 7:
                            assassinTwo = Adaris->SummonCreature(NPC_SKETTIS_ASSASIN, -3399.7294f, 3616.4116f, 276.4681f, 2.4131f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000);
                            Adaris->CastSpell(Adaris, SPELL_CLEAVE, true);
                            Adaris->DealDamage(assassinTwo, assassinTwo->GetMaxHealth());
                            waitTimer = 3000;
                            break;
                        case 8:
                            me->SetUnitMovementFlags(MOVEFLAG_WALK_MODE);
                            me->GetMotionMaster()->MovePoint(1, -3407.5900f, 3624.5700f, 278.0750f);
                            waitTimer = 4000;
                            break;
                        case 9:
                            me->SetFacingToOrientation(4.5553f);
                            Adaris->MonsterSay(ADARIS_SAY4, LANG_UNIVERSAL, 0);
                            waitTimer = 4000;
                            break;
                        case 10:
                            Adaris->SetUnitMovementFlags(MOVEFLAG_WALK_MODE);
                            Adaris->GetMotionMaster()->MovePoint(1, -3408.5500f, 3622.6000f, 278.0719f);
                            waitTimer = 4000;
                            break;
                        case 11:
                            Adaris->SetFacingToOrientation(5.9429f);
                            Adaris->SetStandState(UNIT_STAND_STATE_KNEEL);
                            waitTimer = 4000;
                            break;
                        case 12:
                            Adaris->MonsterSay(ADARIS_SAY5, LANG_UNIVERSAL, 0);
                            waitTimer = 6000;
                            break;
                        case 13:
                            Adaris->SetStandState(UNIT_STAND_STATE_SIT);
                            waitTimer = 500;
                            break;
                        case 14:
                            me->SetStandState(UNIT_STAND_STATE_KNEEL);
                            me->MonsterSay(SEVERIN_SAY3, LANG_UNIVERSAL, 0);
                            waitTimer = 6000;
                            break;
                        case 15:
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            waitTimer = 4000;
                            break;
                        case 16:
                            Adaris->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE);
                            Adaris->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                            Adaris->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            Reset();
                            break;
                        default:
                            break;
                    }
                    break;
                }
                break;
                default:
                    break;
            }
        }
    }
};

CreatureAI* GetAI_npc_severin(Creature* creature)
{
    return new npc_severinAI(creature);
}

bool QuestAccept_npc_severin(Player* player, Creature* creature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_WORLD_OF_SHADOWS)
        CAST_AI(npc_severinAI, creature->AI())->StartEvent(player, EVENT_ACCEPT_QUEST);

    return true;
}

bool QuestComplete_npc_severin(Player* player, Creature* creature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_WORLD_OF_SHADOWS)
        CAST_AI(npc_severinAI, creature->AI())->StartEvent(player, EVENT_COMPLETE_QUEST);

    return true;
}

/*######
## npc_sahaak
######*/

enum eSahaak
{
    AURA_SHABBY_ARAKKOA_DISGUISE = 41181
};

bool GossipHello_npc_sahaak(Player* player, Creature* creature)
{
    if (player->HasAura(AURA_SHABBY_ARAKKOA_DISGUISE, 0))
        player->SEND_VENDORLIST(creature->GetGUID());
    else
        player->SEND_GOSSIP_MENU(10930, creature->GetGUID());
    
    return true;
}

/*######
## mob_terokk
######*/

enum eTerokkMob
{
    SPELL_SHADOW_BOLT_VOLLEY = 40721,
    SPELL_CLEAVE_TEROKK = 15284,
    SPELL_DIVINE_SHIELD = 40733,
    SPELL_ENRAGE = 28747,
    SPELL_WILL_OF_ARRAKOA_GOD = 40722,
    SPELL_CHOSEN_ONE = 40726,
    SPELL_ANCIENT_FLAMES = 40657,
    SPELL_SKYGUARD_FLARE_TARGET = 40656,
    SPELL_SKYGUARD_FLARE = 40655,

    SKYGUARD_WP_CIRCLE_MAX = 6,
    SKYGUARD_WP_MIDDLE = 7,
    SKYGUARD_WP_DESPAWN = 8,
    SKYGUARD_WP_AFTER_SPAWN = 9,
    SKYGUARD_WP_MIDDLE_MAX = 4,

    SAY_SUMMON = -1580401
};

float skyguardAltitude = 324;
float groundAltitiude = 286;

float skyguardSpawn[3][2] = {
    { -3637, 3572 },
    { -3645, 3574 },
    { -3641, 3564 }
};

float skyguardWPStart[3][2] = {
    { -3803, 3504 },
    { -3811, 3506 },
    { -3807, 3496 }
};

float skyguardWPMiddle[4][3] = {
    { -3785, 3507, 315 },
    { -3798, 3500, 316 },
    { -3798, 3515, 308 },
    { -3786, 3515, 314 }
};

float skyguardWPs[6][2] = {
    { -3830, 3513 },
    { -3795, 3549 },
    { -3761, 3538 },
    { -3759, 3498 },
    { -3783, 3467 },
    { -3811, 3494 }
};

struct mob_terokkAI : public ScriptedAI
{
    mob_terokkAI(Creature* c) : ScriptedAI(c) {}

    uint32 ShadowBoltVolley_Timer;
    uint32 Cleave_Timer;
    uint32 ChosenOne_Timer;
    uint32 ChosenOneActive_Timer;
    uint32 SkyguardFlare_Timer;
    uint64 ChosenOneTarget;
    uint64 SkyguardGUIDs[3];
    uint32 CheckTimer;
    uint8 phase;            // 0: 100% to 70% hp, 1: under 30% hp and shield up, 2: under 30% hp and shield down
    uint8 skyguardTurn;

    std::list<Creature*> skyguardTargets;

    void Reset()
    {
        ResetSkyguards();
        ShadowBoltVolley_Timer = 5000;
        Cleave_Timer = 7000;
        ChosenOne_Timer = 30000;
        ChosenOneActive_Timer = 0;
        ChosenOneTarget = 0;
        phase = 0;
        skyguardTurn = 0;
        for (int i = 0; i < 3; i++)
            SkyguardGUIDs[i] = 0;
    }

    void EnterCombat(Unit* who)
    {
        me->MonsterSay(SAY_SUMMON, LANG_UNIVERSAL, 0);
    }

    void Despawn(Unit* unit)
    {
        unit->CombatStop();
        unit->AddObjectToRemoveList();
    }

    void ResetSkyguards()
    {
        for (int i = 0; i < 3; i++)
        {
            if (SkyguardGUIDs[i])
                if (Creature* skyguard = Creature::GetCreature(*me, SkyguardGUIDs[i]))
                    skyguard->GetMotionMaster()->MovePoint(SKYGUARD_WP_DESPAWN, skyguardSpawn[i][0], skyguardSpawn[i][1], skyguardAltitude);
            SkyguardGUIDs[i] = 0;
        }
    }

    void EnterEvadeMode()
    {
        ResetSkyguards();
        ScriptedAI::EnterEvadeMode();
    }

    void JustDied(Unit *)
    {
        ResetSkyguards();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (ShadowBoltVolley_Timer < diff)
        {
            DoCast(me, SPELL_SHADOW_BOLT_VOLLEY);
            ShadowBoltVolley_Timer = 10000 + rand() % 5000;
        }
        else
            ShadowBoltVolley_Timer -= diff;

        if (Cleave_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_CLEAVE_TEROKK);
            Cleave_Timer = 7000 + rand() % 2000;
        }
        else
            Cleave_Timer -= diff;

        if (ChosenOneTarget)
        {
            if (ChosenOneActive_Timer < diff)
            {
                if (me->getVictim()->GetGUID() == ChosenOneTarget)
                    me->AddThreat(me->getVictim(), -500000.0f);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                ChosenOneActive_Timer = 0;
                ChosenOneTarget = 0;
            }
            else
                ChosenOneActive_Timer -= diff;
        }

        if (phase == 0)
        {
            if (ChosenOne_Timer < diff)
            {
                if (Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                {
                    DoCast(target, SPELL_CHOSEN_ONE);
                    me->AddThreat(target, 500000.0f);
                    me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    ChosenOneTarget = target->GetGUID();
                    ChosenOneActive_Timer = 12000;
                    ChosenOne_Timer = 30000 + rand() % 20000;
                }
            }
            else
                ChosenOne_Timer -= diff;
        }

        if (phase == 0 && me->GetHealth() / (float)me->GetMaxHealth() < 0.30f)
        {
            phase = 1;
            DoCast(me, SPELL_DIVINE_SHIELD, true);
            for (int i = 0; i < 3; i++)
                if (Creature *skyguard = me->SummonCreature(23377, skyguardSpawn[i][0], skyguardSpawn[i][1], skyguardAltitude, 3.30f, TEMPSUMMON_MANUAL_DESPAWN, 0))
                {
                    SkyguardGUIDs[i] = skyguard->GetGUID();
                    skyguard->setActive(true);
                    skyguard->GetMotionMaster()->MovePoint(SKYGUARD_WP_AFTER_SPAWN, skyguardWPStart[i][0], skyguardWPStart[i][1], skyguardAltitude);
                    skyguard->Mount(21158);
                }

            CheckTimer = 2000;
            SkyguardFlare_Timer = 15000;
        }

        if (phase > 0)
        {
            if (CheckTimer < diff)
            {
                me->GetCreatureListWithEntryInGrid(skyguardTargets, 23277, 5.0f);
                if (phase == 1)
                {
                    if (!skyguardTargets.empty() && !(*skyguardTargets.begin())->HasAura(SPELL_SKYGUARD_FLARE, 0))
                    {
                        me->RemoveAurasDueToSpell(SPELL_DIVINE_SHIELD);
                        DoCast(me, SPELL_ENRAGE, true);
                        phase = 2;
                    }
                }
                else    // phase == 2
                {
                    if (skyguardTargets.empty())
                    {
                        me->RemoveAurasDueToSpell(SPELL_ENRAGE);
                        DoCast(me, SPELL_DIVINE_SHIELD, true);
                        phase = 1;
                    }
                }
                CheckTimer = 2000;
            }
            else
                CheckTimer -= diff;

            if (SkyguardFlare_Timer < diff)
            {
                if (Creature *skyguard = Creature::GetCreature(*me, SkyguardGUIDs[skyguardTurn++])){
                    uint32 i = rand() % SKYGUARD_WP_MIDDLE_MAX;
                    skyguard->GetMotionMaster()->MovePoint(SKYGUARD_WP_MIDDLE, skyguardWPMiddle[i][0], skyguardWPMiddle[i][1], skyguardWPMiddle[i][2]);
                }
                skyguardTurn %= 3;
                SkyguardFlare_Timer = 20000;
            }
            else
                SkyguardFlare_Timer -= diff;

        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_terokk(Creature *creature)
{
    return new mob_terokkAI(creature);
}

/*######
## npc_skyguard_ace
######*/

struct npc_skyguard_aceAI : public ScriptedAI
{

    npc_skyguard_aceAI(Creature* creature) : ScriptedAI(creature) {}

    uint64 TargetGUID;
    uint32 TargetLifetime;
    int32 AncientFlame_Timer;
    int32 Move_Timer;
    int NextWP;

    void Reset()
    {
        TargetGUID = 0;
        TargetLifetime = 0;
        Move_Timer = -1;
        AncientFlame_Timer = -1;
    }

    void EnterCombat(Unit *who) {}

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (id < SKYGUARD_WP_CIRCLE_MAX)
        {
            NextWP = (id + 1) % SKYGUARD_WP_CIRCLE_MAX;
            Move_Timer = 1;
        }
        else if (id == SKYGUARD_WP_AFTER_SPAWN)
        {
            NextWP = rand() % SKYGUARD_WP_CIRCLE_MAX;
            Move_Timer = 1;
        }
        else if (id == SKYGUARD_WP_DESPAWN)
        {
            me->CombatStop();
            me->AddObjectToRemoveList();
        }
        else if (id == SKYGUARD_WP_MIDDLE)
        {
            DoCast(me, SPELL_SKYGUARD_FLARE, false);
            NextWP = rand() % SKYGUARD_WP_CIRCLE_MAX;
            Move_Timer = 9000;
        }

    }

    void JustSummoned(Creature *creature)
    {
        if (creature->GetEntry() == 23277)
        {
            float x, y, z;
            creature->GetPosition(x, y, z);
            z = groundAltitiude;
            creature->Relocate(x, y, z);
            creature->CastSpell(creature, SPELL_SKYGUARD_FLARE_TARGET, false);
            TargetGUID = creature->GetGUID();
            TargetLifetime = 20500;
            AncientFlame_Timer = 5500;
            DoCast(creature, SPELL_ANCIENT_FLAMES, false);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (TargetGUID)
        {
            if (TargetLifetime < diff)
            {
                if (Unit* unit = me->GetMap()->GetCreature(TargetGUID))
                {
                    unit->CombatStop();
                    unit->AddObjectToRemoveList();
                }
                TargetGUID = 0;
            }
            else
                TargetLifetime -= diff;
        }

        if (Move_Timer >= 0)
        {
            if (Move_Timer < diff)
            {
                if (me->GetMotionMaster()->empty())
                    me->GetMotionMaster()->MovePoint(NextWP, skyguardWPs[NextWP][0], skyguardWPs[NextWP][1], skyguardAltitude);
                Move_Timer = -1;
            }
            else{
                Move_Timer -= diff;
                Move_Timer = Move_Timer < 0 ? 0 : Move_Timer;
            }
        }

        if (AncientFlame_Timer >= 0)
        {
            if (AncientFlame_Timer <= diff)
            {
                if (Unit* target = me->GetMap()->GetCreature(TargetGUID))
                {
                    target->CastStop();
                    target->RemoveAurasDueToSpell(SPELL_SKYGUARD_FLARE_TARGET);
                    // HACK: cast ancient flames so players can be damaged by it, we keep cast ancient flames by skyguard ace only for animation
                    target->CastSpell(target, SPELL_ANCIENT_FLAMES, true);
                }
                AncientFlame_Timer = -1;
            }
            else
                AncientFlame_Timer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_skyguard_ace(Creature *creature)
{
    return new npc_skyguard_aceAI(creature);
}

void AddSC_terokkar_forest()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mob_unkor_the_ruthless";
    newscript->GetAI = &GetAI_mob_unkor_the_ruthless;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_infested_root_walker";
    newscript->GetAI = &GetAI_mob_infested_root_walker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_rotting_forest_rager";
    newscript->GetAI = &GetAI_mob_rotting_forest_rager;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_netherweb_victim";
    newscript->GetAI = &GetAI_mob_netherweb_victim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_floon";
    newscript->pGossipHello =  &GossipHello_npc_floon;
    newscript->pGossipSelect = &GossipSelect_npc_floon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_isla_starmane";
    newscript->GetAI = &GetAI_npc_isla_starmaneAI;
    newscript->pQuestAccept = &QuestAccept_npc_isla_starmane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_skull_pile";
    newscript->pGOHello  = &GossipHello_go_skull_pile;
    newscript->pGOSelect = &GossipSelect_go_skull_pile;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skywing";
    newscript->GetAI = &GetAI_npc_skywingAI;
    newscript->pQuestAccept = &QuestAccept_npc_skywing;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_slim";
    newscript->pGossipHello =  &GossipHello_npc_slim;
    newscript->pGossipSelect = &GossipSelect_npc_slim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_akuno";
    newscript->GetAI = &GetAI_npc_akuno;
    newscript->pQuestAccept = &QuestAccept_npc_akuno;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_letoll";
    newscript->GetAI = &GetAI_npc_letoll;
    newscript->pQuestAccept = &QuestAccept_npc_letoll;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mana_bomb_exp_trigger";
    newscript->GetAI = &GetAI_npc_mana_bomb_exp_trigger;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_mana_bomb";
    newscript->pGOHello = &GOHello_go_mana_bomb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skyguard_handler_deesak";
    newscript->pGossipHello =  &GossipHello_npc_skyguard_handler_deesak;
    newscript->pGossipSelect = &GossipSelect_npc_skyguard_handler_deesak;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_veil_skith_cage";
    newscript->pGOHello =  &GOHello_veil_skith_cage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_captive_child";
    newscript->GetAI = &GetAI_npc_captive_child;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skyguard_prisoner";
    newscript->GetAI = &GetAI_npc_skyguard_prisoner;
    newscript->pQuestAccept = &QuestAccept_npc_skyguard_prisoner;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_severin";
    newscript->GetAI = &GetAI_npc_severin;
    newscript->pQuestAccept = &QuestAccept_npc_severin;
    newscript->pQuestComplete = &QuestComplete_npc_severin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_sahaak";
    newscript->pGossipHello = &GossipHello_npc_sahaak;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_terokk";
    newscript->GetAI = &GetAI_mob_terokk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skyguard_ace";
    newscript->GetAI = &GetAI_npc_skyguard_ace;
    newscript->RegisterSelf();
}

