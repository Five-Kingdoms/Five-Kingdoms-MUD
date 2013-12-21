/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  * 
 *                                                                         *
 *      ROM 2.4 is copyright 1993-1998 Russ Taylor                         *
 *      ROM has been brought to you by the ROM consortium                  *
 *          Russ Taylor (rtaylor@hypercube.org)                            *
 *          Gabrielle Taylor (gtaylor@hypercube.org)                       *
 *          Brian Moore (zump@rom.org)                                     *
 *      By using this code, you have agreed to follow the terms of the     *
 *      ROM license, in the file Rom24/doc/rom.license                     *
 *                                                                         *
 * Code Adapted and Improved by Abandoned Realms Mud                       *
 * and Aabahran: The Forsaken Lands Mud by Virigoth                        *
 *                                                                         *
 * Continued Production of this code is available at www.flcodebase.com    *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "merc.h"
#include "autoquest.h"
#include "interp.h"
#include "clan.h"

/***************************************************************/
/*Following are army orientanted routines for Forsaken Lands  */
/*mud created by Virigoth circa Apr 2004.  Copyrighted for Forsaken*/
/*Lands mud Apr 04, 2004.  Do not use or copy without explicit */
/*permission of author. (Voytek Plawny aka Virigoth)	       */
/***************************************************************/

const char* quest_table[QUEST_MAX] = {
  "none",
  "slay",
  "get",
  "take",
  "find",
};

const char* arg_table[QUEST_ARG_MAX] = {
  "none",
  "package",
  "scroll",
  "jewel",
  "priceless diamond",
  "magical trinket",
  "magical artifact",
  "reagent",
  "potion",
  "note",
};
  
/* GLOBALS */

/* PROTOTYPES */

/* PRIVATE */
/* sets a quest on a given character*/
void SetQuest( CHAR_DATA* victim, AQUEST_DATA* pQuest){
  if (victim == NULL || IS_NPC(victim))
    return;

  memcpy(&victim->pcdata->aquest, pQuest, sizeof( *pQuest));
}

/* clears a quest on a character */
void ClearQuest( CHAR_DATA* ch ){
  if (IS_NPC(ch))
    return;
  else
    memset(&ch->pcdata->aquest, 0, sizeof( AQUEST_DATA ));
}

/* automaticly fails everyone who has the clan quest */
void ClearClanQuest( int clan_handle ){
  CHAR_DATA* ch;

  for (ch = player_list; ch; ch = ch->next_player){
    if (!HAS_CLAN(ch) || ch->pCabal != NULL || GET_CLAN(ch) != clan_handle || !HAS_AQUEST(ch) || ch->pcdata->aquest.fClan == FALSE)
      continue;
    else
      ch->pcdata->aquest.life = 0;
    QuestTimeCheck( ch );
  }
}

/* prepares a quest's target to be a quest destination */
void PrepareQuestTarget( int vnum, int life ){
  CHAR_DATA *wch;
  AFFECT_DATA af;

  af.type = gsn_tele_lock;
  af.level = 60;
  af.duration = life == 0 ? 30 : (life - mud_data.current_time) / 30;

  af.where	= TO_IMMUNE;
  af.bitvector	= IMM_SUMMON;
  af.location	= APPLY_NONE;
  af.modifier	= 0;

  for ( wch = char_list; wch != NULL ; wch = wch->next ){
    if (!IS_NPC(wch) || wch->pIndexData->vnum != vnum)
      continue;
    affect_to_char( wch, &af);
    af.bitvector	= IMM_CHARM;
    affect_to_char( wch, &af);
  }
}

/* support function to pick a quest target by type 
   returns vnum of the target mob
*/
int GetQuestTarget( CHAR_DATA* ch, int type ){
  /* Basic rules:
     - Mob must have count of 1 in the world
     - Must have its level within lvl_min, lvl_max range
     - Cannot be aggressive unless this is a slay quest
     - Must have humanoid form unless its a slay quest
     - Cannot be NON_COMBAT if quest is slay
     - Cannot be guild master or part of any cabal
     - Cannot be a pet
     - cannot be in an area that is:
       AREA_MUDSCHOOL,
       AREA_NOMORTAL, (nogate)
       AREA_SYSTEM,
       AREA_CABAL

  */
     
  /* beacuse all the rules can be evaluated by mob index rather then mob
     we simply make a list of viable mobs and pick from that
  */

  const int tot_mobs	= top_mob_index / 10;
  MOB_INDEX_DATA* mobs [tot_mobs];
  MOB_INDEX_DATA *pMobIndex;
  int max_mob = 0;
  
  int min_lvl = 0, max_lvl = 0;
  int i;
  
  /* set min and max levels */
  switch( type ){
  case QUEST_FIND:
    min_lvl = 1;
    max_lvl = ch->level;
    break;
  case QUEST_GIVE:
    min_lvl = 15;
    max_lvl = ch->level + 5;
    break;
  case QUEST_GET:
    min_lvl = 15;
    max_lvl = ch->level + 10;
    break;
  case QUEST_SLAY:
  default:
    min_lvl = ch->level - 5;
    max_lvl = ch->level + 5;
    break;
  }

  max_lvl = UMAX(15, max_lvl);

  //we run through the mob hash table testing and adding mobs
  for (i = 0; i < MAX_KEY_HASH; i++){
    for ( pMobIndex  = mob_index_hash[i]; pMobIndex != NULL; pMobIndex  = pMobIndex->next ) {
      if (pMobIndex->count != 1)
	continue;
      else if (pMobIndex->level < min_lvl || pMobIndex->level > max_lvl)
	continue;
      else if (IS_SET(pMobIndex->act, ACT_PRACTICE) || pMobIndex->pCabal)
	continue;
      else if (IS_SET(pMobIndex->act, ACT_PET))
	continue;
      else if (IS_SET(pMobIndex->act, ACT_AGGRESSIVE) && type != QUEST_SLAY)
	continue;
      else if (!IS_SET(race_table[pMobIndex->race].form, FORM_HUMANOID) && type != QUEST_SLAY)
	continue;
      else if (IS_SET(pMobIndex->act, ACT_NONCOMBAT) && type == QUEST_SLAY)
	continue;

      else if (IS_AREA(pMobIndex->area, AREA_MUDSCHOOL)
	       || IS_AREA(pMobIndex->area, AREA_NOMORTAL)
	       || IS_AREA(pMobIndex->area, AREA_RESTRICTED)
	       || IS_AREA(pMobIndex->area, AREA_CABAL))
	continue;
      else
	mobs[max_mob++] = pMobIndex;

      if (max_mob >= tot_mobs)
	max_mob = tot_mobs - 1;
    }
  }

  //if we did not find anything return error
  if (max_mob < 1)
    return 0;
  else
    return (mobs[number_range(0, max_mob - 1)]->vnum);
}

/* sets an argument for a quest */
int GetQuestArg( int type ){
  if (type != QUEST_GET && type != QUEST_GIVE)
    return 0;
  else
    return (number_range(1, QUEST_ARG_MAX - 1 ));
}

/* the main function which picks and generates the quest */
/* 1) Pick the type of quest based on quest level
   2) Pick the lifetime based on quest level
   3) Pick the target based on character level
   4) Set the argument as necessary
*/
AQUEST_DATA* GenerateQuest( CHAR_DATA* ch, int level ){
  static AQUEST_DATA quest;

  //reset new quest
  memset(&quest, 0, sizeof(quest));

  //pick type of the quest and lifetime
  switch( level ){
  case LEVEL_EASY:
    if (number_percent() < 50){
      quest.quest = QUEST_FIND;
      quest.life  = mud_data.current_time + number_range(1800, 3600);
    }
    else{
      quest.quest = QUEST_GIVE;
      quest.life  = mud_data.current_time + number_range(1800, 2700);
    }
    break;
  case LEVEL_MEDIUM:
    if (number_percent() < 50){
      quest.quest = QUEST_GIVE;
      quest.life  = mud_data.current_time + number_range(1800, 2700);
    }
    else{
      quest.quest = QUEST_GET;
      quest.life  = mud_data.current_time + number_range(900, 1800);      
    }
    break;
  case LEVEL_HARD:
  default:
    if (number_percent() < 30){
      quest.quest = QUEST_GET;
      quest.life  = mud_data.current_time + number_range(720, 1080);      
    }else{
      quest.quest = QUEST_SLAY;
      if (number_percent() < 50)
	quest.life  = mud_data.current_time + number_range(1800, 2700);      
      else
	quest.life  = mud_data.current_time + number_range(900, 1800);      
    }
    break;
  }
  
  /* pick the target */
  if ( (quest.target = GetQuestTarget( ch, quest.quest)) < 1)
    return NULL;
  /* set the anti-abuse quest stuff on the mob */
  PrepareQuestTarget( quest.target, quest.life );

  /* pick delivery target for get quest */
  if (quest.quest == QUEST_GET){
    if ( (quest.origin = GetQuestTarget( ch, quest.quest)) < 1)
      return NULL;
    /* set the anti-abuse quest stuff on the mob */
    PrepareQuestTarget( quest.origin, quest.life );
  }    

  /* set the argument */
  quest.arg = GetQuestArg( quest.quest);

  /* we have everything */
  return  (&quest);
}

/* executes the actions/rewards of a quest */
void ExecuteQuest( CHAR_DATA* ch, CHAR_DATA* mob, AQUEST_DATA* pQ ){
  char buf[MIL];
  int gold = 0;
  int pts = 0;
  int vnum = mob->pIndexData->vnum;

  /* two different cases here 
     1) mob = origin, and target is 0 = quest complete
     2) mob = target, complete the quest based on quest type, set target = 0 
  */
  if (vnum != pQ->target && vnum != pQ->origin)
    return;
  /* QUEST COMPLETION */
  else if (vnum == pQ->origin && pQ->target == 0){
    sendf( ch, "Congratulations! You have completed your quest!\n\r");
    act("$N tells you 'Well done $n! Consider this to be your reward.", ch, NULL, mob, TO_CHAR);

    /* reward */
    switch( pQ->quest ){
    case QUEST_FIND:		gold = ch->level * 75;	pts = 1;	break;
    case QUEST_GIVE:		gold = ch->level * 100;	pts = 1;	break;
    case QUEST_GET:		gold = ch->level * 150;	pts = 2;	break;
    case QUEST_SLAY:		gold = ch->level * 250;	pts = 3;	break;
    default:		gold = ch->level * 50;	pts = 1;	break;
    }

    /* CABAL member */
    if (ch->pCabal){
      pts = number_range(pts, 2 * pts);
      sendf( ch, "Your bank account has recieved %d gold and %d %ss!\n\r", gold, pts, ch->pCabal->currency);
      ch->in_bank += gold;
      CP_GAIN(ch, pts, FALSE );
    }
    else if (HAS_CLAN(ch) && pQ->fClan){
      sendf( ch, "Your bank account has recieved %d gold!\n\r", gold);
      ch->in_bank += gold;
      ch->pcdata->clan_points += pts;

      sprintf( buf, "%s is the first to complete the clan quest!", PERS2(ch));
      ClanEcho( GET_CLAN(ch), buf );

      //we need to clear the quest before clearing clan ques
      ClearQuest( ch );
      /* fail every one else */
      ClearClanQuest( GET_CLAN(ch));

      /* save the char, and then save the clan */
      save_char_obj( ch );
      ClanSave(GET_CLAN(ch));
    }
    else{
      sendf( ch, "Your bank account has recieved %d gold!\n\r", gold);
      ch->in_bank += gold;
    }
    ClearQuest( ch );
  }
  /* TARGET COMPLETION */
  else if (vnum == pQ->target){
    MOB_INDEX_DATA* pOri = get_mob_index( pQ->origin );
    char buf[MIL];

    //reset target
    pQ->target = 0;

    switch( pQ->quest ){
    case QUEST_FIND:  
      act("You have found $N!\n\rYou quickly map the location to show to $t.", ch, pOri->short_descr, mob, TO_CHAR);
      break;
    case QUEST_GIVE:
      sprintf( buf,"Ah there you are %s.  I have been waiting for you!", PERS2(ch) );
      do_say(mob, buf );
      act("You hand a $t to $N.", ch, arg_table[pQ->arg], mob, TO_CHAR);
      act("$n hands a $t to $N.", ch, arg_table[pQ->arg], mob, TO_ROOM);
      act("$N tells you 'You should head back to $t to get your reward.'", ch, pOri->short_descr, mob, TO_CHAR);
      break;
    case QUEST_GET:
      sprintf( buf,"Ah there you are %s.  You sure took your time!", PERS2(ch) );
      do_say(mob, buf );
      act("$N hands you a $t.", ch, arg_table[pQ->arg], mob, TO_CHAR);
      act("$N hands $n a $t.", ch, arg_table[pQ->arg], mob, TO_ROOM);
      sprintf( buf, "%s in %s", pOri->short_descr, pOri->area->name );
      act("$N tells you 'Take this to $t and waste not an hour!'", ch, buf, mob, TO_CHAR );
      break;
    case QUEST_SLAY:
      act("You slice $N's head off to present to $t as proof of your victory.", ch, pOri->short_descr, mob, TO_CHAR );
      break;
    }
  }
}
  
bool GiveClanQuest( CHAR_DATA* mob, int clan_handle ){
  CHAR_DATA* ch, *victim = NULL;
  AQUEST_DATA* pQuest;
  int count = 0;
  int level;

  /* 1) Search through all present players and check for 2 or more non-cabaled members of clan
     2) Generate quest based on highest leveled member
     3) Give the quest to all non cabaled clan members
  */
  for (ch = player_list; ch; ch = ch->next_player){
    if (!HAS_CLAN(ch) || ch->pCabal != NULL || GET_CLAN(ch) != clan_handle)
      continue;
    else if (IS_IMMORTAL(ch) || IS_MASTER(ch))
      continue;
    else if (HAS_AQUEST(ch) && ch->pcdata->aquest.fClan)
      continue;

    count++;

    if (victim == NULL)
      victim = ch;
    else if (ch->level > victim->level)
      victim = ch;
  }

  /* check for at least two clannies */
  if (count < 2)
    return FALSE;

  //pick the difficulty
  if (number_percent() < 50)
    level = LEVEL_EASY;
  else
    level = LEVEL_MEDIUM;

  /* make the quest */
  if ( (pQuest = GenerateQuest( victim, level )) == NULL){
    send_to_char("Error generating quest.\n\r", ch);
    return FALSE;
  }
  /* set the clan flag */
  pQuest->fClan = TRUE;

  //Generate quest might set origin
  if (pQuest->origin == 0){
    pQuest->origin = mob->pIndexData->vnum;
  }

  //echo on clanchat
  ClanEcho( GET_CLAN(victim), "A Clan Quest has been handed out.  The first to complete it shall have the reward." );

  /* set the quest on all clannies wthout cabal present */
  for (ch = player_list; ch; ch = ch->next_player){
    if (!HAS_CLAN(ch) || ch->pCabal != NULL || GET_CLAN(ch) != clan_handle)
      continue;
    SetQuest(ch, pQuest );

    /* echo the quest to the character */
    sendf( ch, "%s has given you a quest!\n\r", PERS2(mob));
    sendf( ch, "You have to %s.\n\r", GetQuestString( pQuest ));
  }
  return TRUE;
}


/* INTERFACE */
bool QuestRoomCheck( CHAR_DATA* ch, ROOM_INDEX_DATA* room ){
  CHAR_DATA* vch;

  if (IS_NPC(ch) || room == NULL || !HAS_AQUEST(ch))
    return FALSE;
  
  for (vch = room->people; vch; vch = vch->next_in_room){
    if (!IS_NPC(vch))
      continue;
    else if (ch->pcdata->aquest.quest != QUEST_SLAY && vch->pIndexData->vnum == ch->pcdata->aquest.target)
      break;
    else if (vch->pIndexData->vnum == ch->pcdata->aquest.origin)
      break;
  }

  if (vch != NULL){
    ExecuteQuest( ch, vch, &ch->pcdata->aquest );
    return TRUE;
  }
  else
    return FALSE;
}

bool QuestKillCheck(CHAR_DATA* och, CHAR_DATA* victim){
  CHAR_DATA* ch = och;

  if (IS_NPC(och) && IS_AFFECTED(och, AFF_CHARM) && och->master)
    ch = och->master;
  else
    ch = och;

  if (!HAS_AQUEST(ch) || !IS_NPC(victim))
    return FALSE;
  else if (victim->pIndexData->vnum == ch->pcdata->aquest.target){
    ExecuteQuest( ch, victim, &ch->pcdata->aquest);
    return TRUE;
  }

  return FALSE;
}

bool QuestTimeCheck(CHAR_DATA* ch ){
  if (!HAS_AQUEST(ch))
    return FALSE;
  else if (mud_data.current_time < ch->pcdata->aquest.life)
    return FALSE;

  send_to_char("You have run out of time and failed your quest!\n\r", ch);
  ClearQuest( ch );

  return TRUE;
}

/* returns a string with a quest */
char* GetQuestString( AQUEST_DATA* pQ ){
  static char out[MIL];
  char tim[MIL];
  MOB_INDEX_DATA* pMob = NULL;
  MOB_INDEX_DATA* pOrg;

  int time_diff;

  out[0] = 0;

  if (pQ == NULL)
    return out;
  else if ( pQ->target && (pMob = get_mob_index(pQ->target)) == NULL)
    return out;
  else if ( (pOrg = get_mob_index(pQ->origin)) == NULL)
    return out;
  
  if ( (time_diff = pQ->life - mud_data.current_time) > 720)
    sprintf( tim, "%2.1f days", (float) (time_diff / 720));
  else
    sprintf( tim, "%d hour%s", time_diff / 30, time_diff / 30 == 1 ? "" : "s" );
    
  /* special quest for target being 0 (half completed quest) */
  if (pQ->target == 0 || pMob == NULL){
    sprintf( out, "return to %s within %s for your reward",
	     pOrg->short_descr,
	     tim );    
    return out;
  }

  switch( pQ->quest ){
  case QUEST_FIND:
    sprintf( out, "%s %s in %s and return to %s within %s",
	     quest_table[pQ->quest], 
	     pMob->short_descr,
	     pMob->area->name,
	     pOrg->short_descr,
	     tim );
    break;
  case QUEST_GIVE:
    sprintf( out, "%s a %s to %s in %s and return to %s within %s",
	     quest_table[pQ->quest], 
	     arg_table[pQ->arg],
	     pMob->short_descr,
	     pMob->area->name,
	     pOrg->short_descr,
	     tim );
    break;
  case QUEST_GET:
    sprintf( out, "%s a %s from %s in %s and deliver it to %s within %s",
	     quest_table[pQ->quest], 
	     arg_table[pQ->arg],
	     pMob->short_descr,
	     pMob->area->name,
	     pOrg->short_descr,
	     tim );
    break;
  case QUEST_SLAY:
    sprintf( out, "%s %s in %s and return to %s within %s",
	     quest_table[pQ->quest], 
	     pMob->short_descr,
	     pMob->area->name,
	     pOrg->short_descr,
	     tim );
    break;
  }

  return out;
}


/* sets a quest on a victim
   aquest <easy/medium/hard> <victim>
*/
void aquest( CHAR_DATA* ch, char* argument){
  AQUEST_DATA* pQuest;
  CHAR_DATA* victim;
  char diff[MIL];
  int level;

  if (!IS_NPC(ch)){
    send_to_char("NPC only.\n\r", ch);
    return;
  }

  argument = one_argument( argument, diff);

  if (IS_NULLSTR(argument)){
    send_to_char("aquest <easy/medium/hard> <victim>\n\r", ch);
    return;
  }
  else if ( (victim = get_char_room( NULL, ch->in_room, argument)) == NULL){
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  else if (HAS_AQUEST( victim)){
    send_to_char("You are already on a quest!\n\r", victim);
    send_to_char("They are already on a quest!\n\r", ch);
    return;
  }

  if (!str_prefix(diff, "Easy")){	level = LEVEL_EASY; }
  else if (!str_prefix(diff, "Medium")){level = LEVEL_MEDIUM; }
  else					level = LEVEL_HARD;

  if ( (pQuest = GenerateQuest( victim, level )) == NULL){
    send_to_char("Error generating quest.\n\r", ch);
    return;
  }

  //Generate quest might set origin
  if (pQuest->origin == 0){
    pQuest->origin = ch->pIndexData->vnum;
  }

  SetQuest(victim, pQuest );

  /* echo the quest to the character */
  sendf( victim, "%s has given you a quest!\n\r", PERS2(ch));
  sendf( victim, "You have to %s.\n\r", GetQuestString( pQuest ));
}


/* sets a quest on a whole clan (needs at least 2 non cabaled members)
   cquest <clan>
*/
void cquest( CHAR_DATA* ch, char* argument){
  int clan_handle;

  if (!IS_NPC(ch)){
    send_to_char("NPC only.\n\r", ch);
    return;
  }

  if (IS_NULLSTR(argument)){
    send_to_char("cquest <clan>\n\r", ch);
    return;
  }
  else if ( (clan_handle = GetClanHandle( argument )) < 1){
    send_to_char("No such clan.\n\r", ch);
    return;
  }
  if (GiveClanQuest( ch, clan_handle ))
   ch->mprog_delay = 180;

}


