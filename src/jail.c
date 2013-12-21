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
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "jail.h"
#include "bounty.h"
#include "interp.h"
#include "recycle.h"
#include "cabal.h"
#include "olc.h"




/* Written by: Virigoth
 *  returns: number of rares transfered
 *  comments: takes a sorted group (highest to lowest level) of rares and			*
 *  transfers them to ch.  Number of rare items taken is "number", ignores owner only items	*
 * if fRandom then level is ignored and random rares are transfered				*/
int get_random_rares( CHAR_DATA* ch, CHAR_DATA* victim, int number, bool fRandom){
  const int MAX_OBJ = 32;
  OBJ_DATA* bag;
  OBJ_DATA* rares[MAX_OBJ];
  int max_rares = 0;
  int trans = 0;

  OBJ_DATA* obj, *obj_next;
  int i = 0;

/* first cycle through the victim's items, and put limited pointers into "rares" */
  for ( obj = victim->carrying; obj != NULL; obj = obj_next ){
    obj_next = obj->next_content;

    if (obj->wear_loc == WEAR_TATTOO
	|| !IS_LIMITED(obj)
	|| IS_SET(obj->wear_flags, ITEM_HAS_OWNER)
	|| is_affected_obj(obj, gen_malform)
	|| is_affected_obj(obj, gen_hwep)
	|| !can_take( ch, obj ) )
      continue;

    rares[max_rares++] = obj;
  }

  if (max_rares < 1)
    return trans;

/* sort the rares from highest level to lowest */
  for (i = 1; i < max_rares; i++){
    int j = 0;
    for (j = 0; j  < max_rares - i; j ++ ){
      obj = rares[j];
      obj_next = rares[j + 1];

      if (obj_next->level > obj->level){
	rares[j + 1] = obj;
	rares[j] = obj_next;
      }
    }
  }
  /* create a bag */
  if ( (bag = create_object(get_obj_index(OBJ_VNUM_JAILBAG), 0)) == NULL){
    bug("get_random_rares: Could not create a jailbag", 0);
    return 0;
  }
  else{
    char buf[MIL];
    /* set the desc. and details */
    sprintf( buf, bag->name, victim->name );
    free_string( bag->name );
    bag->name = str_dup( buf );
    sprintf( buf, bag->short_descr, victim->name );
    free_string( bag->short_descr );
    bag->short_descr = str_dup( buf );
    sprintf( buf, bag->description, victim->name );
    free_string( bag->description );
    bag->description = str_dup( buf );
  }

/* run through number of rares and either pull from top or select randomly */
  if (fRandom){
    for (i =0; i < number; i++){
      int index = number_range(0, max_rares - 1);
      obj = rares[index];

      REMOVE_BIT(obj->extra_flags,ITEM_INVENTORY);
      obj_from_char( obj );
      if (ch == victim)
	extract_obj( obj );
      else
	obj_to_obj( obj, bag );
      trans ++;
      rares[index] = rares[max_rares--];
      if (max_rares < 1)
	return trans;
    }
  }
  else{
    for (i =0; i < number && i < max_rares; i++){
      obj = rares[i];

      REMOVE_BIT(obj->extra_flags,ITEM_INVENTORY);
      obj_from_char( obj );
      if (ch == victim)
	extract_obj( obj );
      else
	obj_to_obj( obj, bag );
      trans++;
    }
  }
  if (trans > 0)
    obj_to_char( bag, ch );
  else
    extract_obj( bag );
  return trans;
}


/* Written by: Virigoth
 *   returns: pointer to the bag
 *   comments: Strips victim of all objects and deposits into OBJ_VNUM_JAILBAG
 *   which is placed on ch. (fLimOnly = TRUE causes only limited items to be bagged) *
 *  fAll causes all items on character to be put in the bag (minus tattoo) */
OBJ_DATA* create_jailbag(CHAR_DATA* ch, CHAR_DATA* victim, bool fLimOnly, bool fAll){
  OBJ_DATA* bag = NULL;
  OBJ_DATA* obj, *obj_next;
  char buf[MIL];

  int dur = 60;

  /* we run through all belongings and move to the bag.  The bag should be made in OLC
     in such way as to prevent any other object frombeing put in there. */

  if (ch->in_room == NULL)
    return NULL;
  /* PC only */
  if (IS_NPC(victim))
    return NULL;
  if (IS_IMMORTAL(victim))
    return NULL;
  /* hard core cirminals get everything stripped */
  if (victim->pcdata->flagged >= 5)
    fLimOnly = FALSE;

  /*Viri:  new jail bag each death, was causing too much headache with incorrectly linked objects
    for ( obj = victim->carrying; obj != NULL; obj = obj_next ){
    obj_next = obj->next_content;
    if (obj->pIndexData->vnum == OBJ_VNUM_JAILBAG
    && is_name(victim->name, obj->name))
    bag = obj;
    }

    if (!bag){
      for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
	obj_next = obj->next_content;
	  if (obj->pIndexData->vnum == OBJ_VNUM_JAILBAG
	      && is_name(victim->name, obj->name))
	    bag = obj;
	}
    }
    */

  if (!bag) {
    /* if no existin bag with victims name on victim or ch we make new one */
    if ( (bag = create_object(get_obj_index(OBJ_VNUM_JAILBAG), 0)) == NULL){
      bug("create_jailbag: Could not create a jailbag", 0);
      return NULL;
    }
  }

  /* set the desc. and details */
  sprintf( buf, bag->name, victim->name );
  free_string( bag->name );
  bag->name = str_dup( buf );
  sprintf( buf, bag->short_descr, victim->name );
  free_string( bag->short_descr );
  bag->short_descr = str_dup( buf );
  sprintf( buf, bag->description, victim->name );
  free_string( bag->description );
  bag->description = str_dup( buf );


  /* start transfer */
  if (!fLimOnly && victim->gold > 0 ){
    obj_to_obj(create_money(victim->gold), bag);
    victim->gold = 0;
  }

  if (!IS_IMMORTAL(victim)){
    for ( obj = victim->carrying; obj != NULL; obj = obj_next ){
      obj_next = obj->next_content;

      if (obj->wear_loc == WEAR_TATTOO)
	continue;

      if (!fAll){
	if (fLimOnly && !IS_LIMITED(obj))
	  continue;

	if (IS_SET(obj->wear_flags, ITEM_HAS_OWNER))
	  continue;
	if( is_affected_obj(obj, gen_malform)
	    || is_affected_obj(obj, gen_hwep))
	  continue;
      }
      REMOVE_BIT(obj->extra_flags,ITEM_INVENTORY);
      obj_from_char( obj );
      obj_to_obj( obj, bag );
    }
  }
 /* dont create a bag if empty or ch == victim */
  if (!bag->contains || ch == victim){
    extract_obj(bag);
    return NULL;
  }

    /* move the bag to ch automaticly */
    obj_to_char(bag, ch);

    /* set the timer to prevent abuse */
    bag->timer = dur;
    return bag;
}

/* finds a random justice and and jails the victim using that justice */
CHAR_DATA* surrender( CHAR_DATA* victim ){
  const int  max_justice = 16;
  CHAR_DATA* justice[max_justice];
  int last_justice = 0;

  CHAR_DATA* vch;

  for (vch = player_list; vch; vch = vch->next_player ){
    if (!IS_IMMORTAL(vch) && vch->pCabal && IS_CABAL(vch->pCabal, CABAL_JUSTICE))
      justice[last_justice++] = vch;
  }

  if (last_justice < 1)
    return victim;
  else
    vch = justice[number_range(0, last_justice - 1)];

  act("A guard drags $N towards you handing $M to you.", vch, NULL, victim, TO_CHAR );
  act("A guard drags you towards $n handing you to $m.", vch, NULL, victim, TO_VICT );

  return vch;
}

/* returns position in jail_table, based on current room vnum  */
int jail_cell_lookup(int vnum){
  int jail = 0;

  for (jail = 0; jail < MAX_HOMETOWN; jail ++){
    int cell = 0;
    /* search through each hometown for matching vnum of room */
    if (!jail_table[jail].name)
      break;
    /* check each cell */
    for (cell = 0; cell < MAX_CELL; cell++){
      if (!jail_table[jail].cell[cell])
	continue;
      if (jail_table[jail].cell[cell] == vnum){
	return jail;
      }
    }//END cell match
  }//END jail search
  return 0;
}

/* returns spot in execution_Table */
int jexecution_lookup(const char *name)
{
   int e;
   for ( e = 0; execution_table[e].name != NULL; e++){
     if (!execution_table[e].name[0])
       continue;
     if (LOWER(name[0]) == LOWER(execution_table[e].name[0])
	 && !str_prefix( name, execution_table[e].name))
       return e;
   }
   return 0;
}
/* Written by: Virigoth							*
 * Returns: pointer to paf with gen_jail so name of justice can be set.	*
 * Used: jail.c, effect.c						*
 * Comment: Sets a current sentance based on "type" and "level".  Type  *
 * is JAIL_XXX from jail.h, level is severity which affects things based*
 * on type. Ch is the person to have gen_jail set/changed		*/

AFFECT_DATA* set_sentence(CHAR_DATA* ch, int type, int level){
  AFFECT_DATA* paf, af;
  CHAR_DATA* wch;

  char buf[MIL];
  char buf2[MIL];
  int cur_dur = 0;

  /* poor man flag for change */
  buf[0] = '\0';

  /* set common things */
  af.type = gen_jail;
  af.where = TO_AFFECTS;
  af.bitvector  = 0;
  af.location = APPLY_NONE;

  /* check if there is existing duration/type */
  if ( (paf = affect_find(ch->affected, gen_jail)) != NULL)
    cur_dur = paf->duration;


  switch(type){
  default:
    bug("set_sentence: Unkown type.", 0);
    return NULL;
    /* NORMAL */
  case JAIL_NORMAL:
    af.modifier = JAIL_NORMAL;
    af.duration = DUR_NORMAL;
    af.level = 0;
    sprintf(buf, "Normal for %d hours", af.duration);
    break;
    /* BAIL */
  case JAIL_BAIL:
    af.modifier = JAIL_BAIL;
    af.duration = UMAX(DUR_BAIL, cur_dur);
    af.level = level;
    sprintf(buf, "Bail of %d gold for %d hours (Use \"bail\" to pay)",
	    SEV_BAIL * level, af.duration);
    break;
    /* EXTENDED */
  case JAIL_EXTENDED:
    af.modifier = JAIL_EXTENDED;
    af.duration = UMAX(5, DUR_EXTENDED + (SEV_EXTENDED * level) - (DUR_NORMAL - cur_dur));
    af.level = level;
    sprintf(buf, "Extended for %d hours.", af.duration);
    break;
    /* EXECUTION */
  case JAIL_EXECUTE:
    af.modifier = JAIL_EXECUTE;
    af.duration = DUR_EXECUTE;
    af.level = level;
    sprintf(buf, "Execution in %d hours.", af.duration);
    break;
  case JAIL_RELEASE:
    af.modifier = JAIL_RELEASE;
    af.duration = DUR_RELEASE;
    af.level = level;
    sprintf(buf, "Release in %d hours.", af.duration);
    break;
  }
  act_new("Your sentence has been set to $t.", ch, buf, NULL, TO_CHAR, POS_DEAD);
  sprintf(buf2, "%s's sentence has been set to %s.", PERS2(ch), buf);

  /* broadcast on justice channel */
  for ( wch = char_list; wch != NULL; wch = wch->next ){
    if ( IS_NPC(wch) && IS_SET(wch->off_flags,CABAL_GUARD)
	 && wch->pIndexData->group == VNUM_JUSTICE ){
      REMOVE_BIT(wch->comm,COMM_NOCHANNELS);
      REMOVE_BIT(wch->comm,COMM_NOCABAL);
      do_jcabaltalk(wch, buf2);
      SET_BIT(wch->comm,COMM_NOCABAL);
      SET_BIT(wch->comm,COMM_NOCHANNELS);
      break;
    }
  }//END CABAL CHANNEL TALK

    /* conclave gets reported when someone gets thrown injail */
  if (type == JAIL_NORMAL){
    sprintf(buf2, "Contacts report of %s's arrival in %s.", PERS2(ch), ch->in_room->area->name);
    for ( wch = char_list; wch != NULL; wch = wch->next ){
      if ( IS_NPC(wch) && IS_SET(wch->off_flags,CABAL_GUARD)
	   && wch->pIndexData->group == VNUM_LEGION ){
	REMOVE_BIT(wch->comm,COMM_NOCHANNELS);
	REMOVE_BIT(wch->comm,COMM_NOCABAL);
	do_cabaltalk(wch, buf2);
	SET_BIT(wch->comm,COMM_NOCABAL);
	SET_BIT(wch->comm,COMM_NOCHANNELS);
	break;
      }
    }
  }
  /* decide between new paf, or current */
  if (paf){
    paf->modifier = af.modifier;
    paf->duration = af.duration;
    paf->level = level;
    return paf;
  }
  return affect_to_char(ch, &af);
}

/* Written by: Virigoth							*
 * Returns: pointer of cell to put char. in, requires first cell in 	*
 * jail_table to contain cells and data to be used in case of no match  *
 * found								*
 * Used: jail.c,							*
 * Comment: Jailcells stored in jail_table, cell is based on justice    *
 * hometown, if no jail cells found there, it will search for first jail*
 * possible								*/

ROOM_INDEX_DATA* get_jailcell(int hometown){
  int jail = jail_lookup(hometown_table[hometown].name);
  int small = 9999;
  int cell = 0;
  ROOM_INDEX_DATA* ret_cell = NULL;
  AREA_DATA* pArea;

  // first check if the hometown is lawful
  if ( (pArea = get_area_data_str(jail_table[jail].name)) != NULL
       && !IS_AREA(pArea, AREA_LAWFUL)){
    /* check all jails pick first lawful city with jail */
    int jaill;
    for ( jaill = 0; jail_table[jaill].name != NULL && jaill < MAX_HOMETOWN; jaill++){
      if ( (pArea = get_area_data_str(jail_table[jaill].name)) != NULL
	   && IS_AREA(pArea, AREA_LAWFUL))
	jail = jaill;
    }
  }
  /* we need to traverse through the cells in the given city
     and select the one with lowest amount of characters */
  for (cell = 0; cell < MAX_CELL; cell++){
    ROOM_INDEX_DATA* room;
    CHAR_DATA* vch;
    int count = 0;

    /* skip 0 cells */
    if (!jail_table[jail].cell[cell])
      continue;
    /* get the room, check if exists */
    if ( (room = get_room_index(jail_table[jail].cell[cell])) == NULL){
      char buf[MIL];
      sprintf(buf, "get_jailcell: cell %d does not exist.",
	      jail_table[jail].cell[cell]);
      bug(buf, 0);
      return NULL;
    }

    /* count PC's in the room */
    for (vch = room->people; vch; vch = vch->next_in_room){
      if (!IS_NPC(vch) && (vch->pCabal == NULL || !IS_CABAL(vch->pCabal, CABAL_JUSTICE)))
	count++;
    }

    /* decide if this room is the one with least people */
    if (count < small){
      small = count;
      ret_cell = room;
    }
    /* continue the search */
  }
  /* search over, make sure there is a room at all */
  if (!ret_cell){
    char buf[MIL];
    sprintf(buf, "get_jailcell: No valid cell found for hometown %d.",
	    hometown);
    bug(buf, 0);
  }
  return ret_cell;

}

/* Written by: Virigoth							*
 * Returns: Void							*
 * Used: jail.c,							*
 * Comment: Causes the exe_mob as per jail_table to yell the string	*
 * the mob is created at the "guardhouse" room, then destroyed		*/
void jail_announce(int jail, char* string){
  CHAR_DATA* mob;
  ROOM_INDEX_DATA* room;

  /* safety */
  if (!string)
    return;
  if (jail > MAX_HOMETOWN){
    bug("jail_announce: jail was greater then MAX_HOMETOWN.", 0);
    return;
  }
  /* Grab the room to be yelled in*/
  if ( (room = get_room_index(jail_table[jail].exe_room))  == NULL){
    char buf[MIL];
    sprintf(buf, "jail_announce: room %d does not exist.",
	    jail_table[jail].exe_room);
    bug(buf, 0);
    return;
  }
  /* grab the mob to do the yelling */
  if ( (mob = create_mobile( get_mob_index(jail_table[jail].exe_mob)))  == NULL){
    char buf[MIL];
    sprintf(buf, "jail_announce: mob %d does not exist.",
	    jail_table[jail].exe_mob);
    bug(buf, 0);
    return;
  }

    /* move mob to the right room, yell, and cleanup */
    char_to_room(mob, room);
    REMOVE_BIT(mob->comm,COMM_NOYELL);
    REMOVE_BIT(mob->comm,COMM_NOCHANNELS);
    do_yell(mob, string);
    SET_BIT(mob->comm,COMM_NOYELL);
    SET_BIT(mob->comm,COMM_NOCHANNELS);
    /* just in case something did hit him */
    stop_fighting(mob, FALSE);
    char_from_room(mob);
    extract_char(mob, TRUE);
}

/* Allows a criminal to surrender */
void do_surrender( CHAR_DATA* ch, char* argument ){

  if (IS_OUTLAW( ch )){
    send_to_char("Go fall on a sword instead.\n\r", ch);
    return;
  }
  else if (!IS_WANTED( ch )){
    send_to_char("You have not been marked a criminal.\n\r", ch);
    return;
  }
  else if (is_affected(ch, gen_jail) || IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL)){
    send_to_char("You're already in jail.\n\r", ch);
    return;
  }
  else if (is_fight_delay( ch, 180 )){
    send_to_char("You cannot surrender after recent combat.\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument)){
    send_to_char("Are you sure you wish to surrender to forces of Law?\n\rUse \"surrender confirm\" to confirm.\n\r", ch );
    return;
  }
  else{
    char buf[MIL];
    sprintf( buf, "%s has chosen to surrender to our judgement.", PERS2( ch ));
    cabal_echo_flag( CABAL_JUSTICE, buf );
    jail_char( ch, ch );
  }
}

/* Written by: Virigoth							*
 * Returns: Void							*
 * Used: fight.c,							*
 * Comment: Checkes for wanted status, and transports victim to correct jail*
 * if needed. Sets gen_jail if needed.	jail selected based on ch home  */

void jail_char(CHAR_DATA* cha, CHAR_DATA* victim){
  CHAR_DATA* ch  = cha;
  ROOM_INDEX_DATA* cell;
  AFFECT_DATA* paf;
  AREA_DATA* pArea;
  char buf[MIL];
  int max_pen = 0, crime = 0;

  /* check for problems */
  if (!ch || !victim){
    bug("jail_char: NULL ch or victim passed.", 0);
    return;
  }

  if (IS_NPC(victim)){
    bug("jail_char: victim NPC passed.", 0);
    return;
  }
  /* check for wanted status */
  if (!IS_SET(victim->act,PLR_WANTED))
    return;
  /* check for charmies */
  if (IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM)){
    if (ch->leader)
      ch = ch->leader;
    else if (ch->master)
      ch = ch->master;
    else{
      bug("jail_char: NPC without master passed.", 0);
      return;
    }
  }
  /* check for mob apprehension */
  else if (IS_NPC(ch) || ch == victim)
    ch = surrender( victim );

  /* grab the jail cell to transport too */
  if ( (cell = get_jailcell(ch->hometown)) == NULL){
    char buf[MIL];
    sprintf(buf, "jail_char: get_jailcell returned NULL for town of %s.",
	    hometown_table[ch->hometown].name);
    bug(buf, 0);
    return;
  }

  /* stop the fight, make things all safe so victim doesnt die in cell */
  stop_fighting( victim, TRUE );
  if (IS_AFFECTED2(victim,AFF_RAGE))
    {
      REMOVE_BIT(victim->affected2_by,AFF_RAGE);
      affect_strip(victim,gsn_rage);
    }
  affect_strip(victim,gsn_poison);
  affect_strip(victim,gsn_plague);
  affect_strip(victim,gen_bleed);
  affect_strip(victim,gsn_insect_swarm);
  affect_strip(victim, skill_lookup("visitation"));
  affect_strip(victim,skill_lookup("dysentery"));

  victim->pcdata->condition[COND_HUNGER]=45;
  victim->pcdata->condition[COND_THIRST]=45;
  victim->hit = UMAX(1, 25 * victim->max_hit / 100);
  victim->move = 25 * victim->max_move / 100;
  victim->mana = 0 * victim->max_mana / 100;
  victim->position = POS_RESTING;
  die_follower(victim, FALSE);

  /* get the belongings for die hard criminals, if ch and victim are same (surrender) items are poofed */
  if (victim->pcdata->flagged > 11 ){		/* 10+ */
    create_jailbag(ch, victim, TRUE, FALSE);
  }
  else if (victim->pcdata->flagged > 6 ){		/* 6 - 10 */
    int num = get_random_rares( ch, victim, 6, FALSE );
    sendf( ch, "You recive %d item%s from %s.\n\r", num, num == 1 ? "" : "s", PERS2( victim ));
    sendf( victim, "You lost %d item%s.\n\r", num, num == 1 ? "" : "s");
  }
  else if (victim->pcdata->flagged > 3 ){		/* 3 - 5 */
    int num = get_random_rares( ch, victim, 3, FALSE );
    sendf( ch, "You recive %d item%s from %s.\n\r", num, num == 1 ? "" : "s", PERS2( victim ));
    sendf( victim, "You lost %d item%s.\n\r", num, num == 1 ? "" : "s");
  }
  else if (victim->pcdata->flagged > 1 ){		/* 1 - 2 */
    int num = get_random_rares( ch, victim, 1, TRUE );
    sendf( ch, "You recive %d item%s from %s.\n\r", num, num == 1 ? "" : "s", PERS2( victim ));
    sendf( victim, "You lost %d item%s.\n\r", num, num == 1 ? "" : "s");
  }


  /* Print some text and move the victim */
  if (ch != victim ){
    act("As you lose your consciousness you see a swarm of guards surround you quickly.", ch, NULL, victim, TO_VICT);
    act("As $N loses $S consciousness you call onto guards to capture $M.", ch, NULL, victim, TO_CHAR);
    act("As $N loses $S consciousness $n calls onto guards to capture $M.", ch, NULL, victim, TO_NOTVICT);
  }
  else{
    act("The guards lead you away.\n\r", ch, NULL, victim, TO_CHAR );
  }

  /* give credit */
  if (ch != victim
      && IS_CABAL(ch->pCabal, CABAL_JUSTICE)
      && IS_SET(victim->act,PLR_WANTED)
      && !IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL)
      && !is_affected(ch, gen_jail)){
    cp_event(ch, victim, NULL, CP_EVENT_CAPTURE);
  }

  /* boost the relations between the justice and royals of areas that this person commited crimes in */
  for (paf = victim->affected; paf; paf = paf->next ){
    if (paf->type == gsn_wanted && paf->bitvector){
      /* crime found/wanted we increase the relations but need area first */
      if ( (pArea = get_area_data( paf->location )) == NULL){
	bug("jail_char: could not get area for vnum %d.", paf->location);
	continue;
      }
      else if (pArea->pCabal)
	affect_justice_relations(pArea->pCabal, 50);
    }
  }
  /* move the victim */
  act("The guards quickly carry $N off to $S jailcell.", ch, NULL, victim, TO_NOTVICT);
  char_from_room(victim);
  char_to_room(victim, cell);
  act("A small squad of justice guards throws $n into the cell.", victim, NULL, NULL, TO_ROOM);
  do_look(victim, "auto");


  /* get the maximum penalty based on "wanted" effect */
  crime = show_crimes( victim, victim, TRUE, TRUE );
  switch ( crime ){
  case CRIME_LIGHT:
    max_pen = JAIL_BAIL;		break;
  case CRIME_NORMAL:
    max_pen = JAIL_EXTENDED;		break;
  case CRIME_HEAVY:
    max_pen = JAIL_EXECUTE;		break;
  case CRIME_DEATH:
    max_pen = JAIL_EXECUTE;		break;
  default:
    max_pen = JAIL_NORMAL;		break;
  }

  /* outlaws always get the cut */
  if (IS_OUTLAW(victim)){
    crime = CRIME_DEATH;
    max_pen = JAIL_EXECUTE;
  }

  if ( crime == CRIME_DEATH || ch == victim){
    if( (paf = set_sentence(victim, max_pen, number_range(1, 5))) == NULL)
      send_to_char("`!An error occured, contact an immortal.``\n\r", ch);
    else
      string_to_affect(paf, ch == victim ? "surrendered" : ch->name);
  }
  else if( (paf = set_sentence(victim, JAIL_NORMAL, 1)) == NULL)
    send_to_char("`!An error occured, contact an immortal.``\n\r", ch);
  else
    string_to_affect(paf, ch == victim ? "surrendered" : ch->name);


  /* Yell out the announcement */
  if (ch == victim)
    sprintf(buf, "Justice be praised, %s has surrendered to Justice!",
	    PERS2(victim));
  else
    sprintf(buf, "Justice be praised, %s has been captured by %s!",
	    PERS2(victim), PERS2(ch));
  jail_announce(jail_cell_lookup(victim->in_room->vnum), buf);

  /* increase capture count if ch != victim */
  if (ch != victim
      && ch->pCabal
      && IS_CABAL(ch->pCabal, CABAL_JUSTICE)){
    ch->pcdata->member->kills++;
  }
}



/* Written by: Virigoth							*
 * Returns: Void							*
 * Used: effect.c,							*
 * Comment: Transports the character out of the jail cell into the room *
 * dictated by "guardhouse" in jail_table					*/

void unjail_char(CHAR_DATA* ch){

  ROOM_INDEX_DATA* room;
  CHAR_DATA* wch;
  AFFECT_DATA* paf;

  char buf[MIL];
  int jail = 0;
  int exit = 0;

  /* check for problems */
  if (!ch){
    bug("unjail_char: NULL ch passed.", 0);
    return;
  }

  if (IS_NPC(ch)){
    bug("unjail_char: ch NPC passed.", 0);
    return;
  }

  /* check that char. is in a cell */
  if (!IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL))
    bug("unjail_char: ch not in room marked ROOM_JAILCELL", 0);

  /* check for exit out of the jail */
  jail = jail_cell_lookup(ch->in_room->vnum);
  exit = jail_table[jail].guardhouse;

  /* if exit is 0 we send to home temple */
  if ( (room = get_room_index(exit))  == NULL){
    char buf[MIL];
    sprintf(buf, "unjail_char: room %d does not exist as an exit.",
	    exit);
    bug(buf, 0);
  }
  if (!room)
    room = get_room_index(get_temple(ch));
  if (room == NULL){
    send_to_char("You are completely lost.\n\r",ch);
    return;
  }
  /* Yell out the announcement */
  sprintf(buf, "The sentence served, %s is now free and innocent!", PERS2(ch));
  jail_announce(jail, buf);

  /* move the char out of jail*/
  char_from_room(ch);
  char_to_room(ch, room);
  act("The guards escort $n into the area and release $m.", ch, NULL, NULL, TO_ROOM);
  do_look(ch, "auto");
  /* get rid of flag */
  if (!IS_OUTLAW( ch )){
    REMOVE_BIT(ch->act, PLR_WANTED);
  }

  /* broadcast on justice channel */
  for ( wch = char_list; wch != NULL; wch = wch->next ){
    if ( IS_NPC(wch) && IS_SET(wch->off_flags,CABAL_GUARD)
	 && wch->pIndexData->group == VNUM_JUSTICE ){
      REMOVE_BIT(wch->comm,COMM_NOCHANNELS);
      REMOVE_BIT(wch->comm,COMM_NOCABAL);
      do_jcabaltalk(wch, buf);
      SET_BIT(wch->comm,COMM_NOCABAL);
      SET_BIT(wch->comm,COMM_NOCHANNELS);
      break;
    }
  }//END CABAL CHANNEL TALK
  /* small check */
  if ( (paf = affect_find(ch->affected, gen_jail)) != NULL){
    paf->modifier = JAIL_NONE;
    affect_strip(ch, gen_jail);
  }
  //set wanted to expire
  for (paf = ch->affected; paf; paf = paf->next){
    if (paf->type == gsn_wanted)
      paf->duration = 0;
  }
  //  affect_strip( ch, gsn_wanted );
}





/* Written by: Virigoth							*
 * Returns: Void							*
 * Used: effect.c,							*
 * Comment: Command which lets the justice dictate fate of prisoner.    *
 * only the justice matching the string passed in gen_jail can affect   *
 * the prisoner, and only if prisoner is stillin a ROOM_JAILCELL room.  */

void do_jail( CHAR_DATA *ch, char *argument ){
  /*
    jail <criminal> <sentence> <severity/type of execution>


SENTENCE FLOW:  (NORMAL SET ON CAPTURE OR DEATH FOR ATTORICTY CRIMES)

	     /-> JAIL_TRANSFER
	    /--> JAIL_BAIL -> JAIL_EXTEND-> JAIL_RELEASE
JAIL_NORMAL ---> JAIL_EXTEND -> JAIL_RELEASE
	    \--> JAIL_EXECUTE
	     \-> JAIL_RELEASE
  */

  CHAR_DATA* victim;	//criminal
  CHAR_DATA* justice;	//Owner of criminal
  CHAR_DATA* vch;

  AFFECT_DATA* paf;	//Info from gen_jail (if not found, abort)

  BUFFER *buffer;	//For display of criminals

  char arg1[MIL];	//Target criminal
  char arg2[MIL];	//Sentence
  char buf[MIL];

  int count = 0;

  int cur_sen = JAIL_NORMAL;
  int new_sen = JAIL_NORMAL;
  int new_sev = 1;
  int max_pen = JAIL_BAIL;

  bool fAllow = TRUE;	//used tocheck if sentence flow is allowed

  if (!ch->pCabal || !IS_CABAL(ch->pCabal, CABAL_JUSTICE))
    return;

  /* first check for null to show syntax  mates */
  if (argument[0] == '\0'){
    sendf(ch, "Syntax: jail <criminal> <sentence> <severity (1-10)>\n\r"\
	  "for <sentence> of bail, amount is %d x severity gold with limit of %dh\n\r"\
	  "for <sentence> of extended, length is %d + (%d x severity) hours\n\r\n\r"\
	  "Following are all criminals with their jurisdictions:\n\r",
	  SEV_BAIL, DUR_BAIL, DUR_EXTENDED, SEV_EXTENDED);
    /* run through players showing info */
    buffer=new_buf();
    for (vch = player_list; vch; vch = vch->next){
      /* count only those in cells and still wanted*/
      if (!IS_SET(vch->act, PLR_WANTED)|| !vch->in_room
	  || !IS_SET(vch->in_room->room_flags2, ROOM_JAILCELL))
      continue;
      /* check for gen_jail with justice name */
      if ( (paf = affect_find(vch->affected, gen_jail)) == NULL
	   || !paf->has_string)
      continue;
      /* add info to buffer */
      sprintf(buf, "%d. %-5s %-15s in %-10s assigned to %-15s [%-2s (%dh)]\n\r",++count, PERS2(vch),
	      vch->in_room->area->name, vch->in_room->name, paf->string,
	      paf->modifier == JAIL_NONE ? "None" :
	      paf->modifier == JAIL_NORMAL ? "Normal" :
	      paf->modifier == JAIL_EXTENDED ? "Extended" :
	      paf->modifier == JAIL_BAIL ? "Bail" :
	      paf->modifier == JAIL_EXECUTE ? "Execution" :
	      paf->modifier == JAIL_RELEASE ? "Release" : "Error",
	      paf->duration);


      add_buf( buffer, buf );
    }
    /* check if anything found */
    if (!count)
      send_to_char("The jails are empty.\n\r", ch);
    else
      page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
    return;
  }



  /* start testing the actual arguments */
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if ( (victim = get_char(arg1)) == NULL || IS_NPC(victim)){
    send_to_char("No such player found.\n\r", ch);
    return;
  }

  /* check they are criminal and justice have jurisdiction */
  if ( (!IS_SET(victim->act, PLR_WANTED)|| !victim->in_room
	|| !IS_SET(victim->in_room->room_flags2, ROOM_JAILCELL))
       && !IS_IMMORTAL(ch)){
    send_to_char("The target player must be [`1WANTED``] and be in a jailcell.\n\r", ch);
    return;
  }

  if ( (paf = affect_find(victim->affected, gen_jail)) == NULL){
    send_to_char("That player is not affected by any type of incarceration.\n\r", ch);
    return;
  }
  else if (!paf->has_string || !is_name(paf->string, ch->name)){
    if (!IS_IMMORTAL(ch)){
      send_to_char("You do not have jurisdiction over that criminal.\n\r", ch);
      return;
    }
  }

  if (arg2[0] == '\0'
      || (str_cmp("bail", arg2)
      && str_cmp("extend", arg2)
      && str_cmp("execute", arg2)
      && str_cmp("release", arg2)
      && str_cmp("transfer", arg2))){
    send_to_char("The sentence must be one of following:\n\r"\
		 "bail, extend, execute, release, transfer\n\r", ch);
    return;
  }

  /* get the maximum penalty based on "wanted" effect */
  switch (show_crimes( ch, victim, FALSE, TRUE ) ){
  case CRIME_LIGHT:
    max_pen = JAIL_BAIL;		break;
  case CRIME_NORMAL:
    max_pen = JAIL_EXTENDED;		break;
  case CRIME_HEAVY:
    max_pen = JAIL_EXECUTE;		break;
  case CRIME_DEATH:
    max_pen = JAIL_EXECUTE;		break;
  }

  /* select action and check over severities*/
  if (!str_cmp("bail", arg2)){
    new_sen = JAIL_BAIL;
    if (!(new_sev = atoi(argument))
	|| new_sev > 10){
      send_to_char("Severity must be on scale 1-10", ch);
      return;
    }
  }
  else if (!str_cmp("extend", arg2)){
    new_sen = JAIL_EXTENDED;
    if (!(new_sev = atoi(argument))){
      send_to_char("Severity must be on scale 1-10", ch);
      return;
    }
  }
  else if (!str_cmp("execute", arg2)){
    new_sen = JAIL_EXECUTE;
    if ( (new_sev = jexecution_lookup(argument)) == 0){
      int i = 0;
      send_to_char("Following are valid executions:\n\r", ch);
      for (i = 0; execution_table[i].name != NULL; i++){
	if (!execution_table[i].name[0])
	  continue;
	sendf(ch, "%s\n\r", execution_table[i].name);
      }
      return;
    }
  }
  else if (!str_cmp("release", arg2))
    new_sen = JAIL_RELEASE;
  else if (!str_cmp("transfer", arg2))
    new_sen = JAIL_TRANSFER;
  else{
    sprintf(buf, "do_jail: Sentence %s not found or not valid.", arg2);
    bug(buf, 0);
    send_to_char(buf, ch);
    return;
  }

  /* check if the sentence selected is not over max */
  if (new_sen > max_pen ){
    act("That sentence is too severe based on the crimes $N has commited.", ch, NULL, victim, TO_CHAR);
    return;
  }
  /* All checks are clear, we now make sure that the SENTENCE flow is satisfied */
  cur_sen = paf->modifier;

  switch (cur_sen){
  default:
  case JAIL_RELEASE:
  case JAIL_TRANSFER:
  case JAIL_EXECUTE:
    fAllow = FALSE;
    break;
  case JAIL_EXTENDED:
    if (new_sen != JAIL_RELEASE
	&& new_sen != JAIL_TRANSFER)
      fAllow = FALSE;
    break;
  case JAIL_BAIL:
    if (new_sen != JAIL_TRANSFER)
      fAllow = FALSE;
    break;
  case JAIL_NORMAL:
    break;
  }


  if (!fAllow){
    send_to_char("This new verdict is not allowed due to criminals current sentence.\n\r", ch);
    return;
  }

  /* change verdict */
  if (new_sen == JAIL_TRANSFER){
    if ( (justice = get_char(argument)) == NULL){
      send_to_char("No such player found.\n\r", ch);
      return;
    }

    if (justice == ch){
      send_to_char("You already have jurisdiction over that criminal.\n\r", ch);
      return;
    }
    if (!justice->desc || ch->in_room->vnum == ROOM_VNUM_LIMBO){
      send_to_char("Their soul seemed to vacate the body for now.\n\r", ch);
      return;
    }

    if (IS_NPC(justice) || justice->pCabal == NULL
	|| !IS_CABAL(justice->pCabal, CABAL_JUSTICE)){
      send_to_char("That person is not a Justice member.\n\r", ch);
      return;
    }
    /* change the name of Justice in charge */
    string_to_affect(paf, justice->name);
    act("You have given $N jurisdiction over $t.", ch, PERS2(victim), justice, TO_CHAR);
    act("$n has given you jurisdiction over $t.", ch, PERS2(victim), justice, TO_VICT);
    act("Jurisdiction over your sentance has just been handed to $N.", victim, NULL, justice, TO_CHAR);
    return;
  }
  /* change sentance */
  set_sentence(victim, new_sen, new_sev);
  act_new("$N's new sentence has been set.", ch, NULL, victim, TO_CHAR, POS_DEAD);
}



/* Written by: Virigoth							*
 * Returns: Void							*
 * Used: effect.c,							*
 * Comment: Performs the actual execution and creates an object EXECUTION*
 * as a marker, which decays after "decay" ticks. The periodic yells are*
 * done by gen. Execution performed based on current Jailcell location	*/
void jail_execute(CHAR_DATA* ch, CHAR_DATA* mob, int exe_type, char* name){

  ROOM_INDEX_DATA* room;
  AFFECT_DATA* crime;
  OBJ_DATA* obj, *bag, *tattoo;
  CHAR_DATA *vch, *vch_next;
  char crime_str[MIL], strsave[MIL];
  int crimes = 0;
  char buf[MIL];
  int jail = 0;
  int exit = 0;

  const int dur = 480;

  /* check for problems */
  if (!ch){
    bug("jail_execute: NULL ch passed.", 0);
    return;
  }
  if (IS_NPC(ch)){
    bug("jail_execute: NPC ch passed.", 0);
    return;
  }
  /* check that char. is in a cell */
  if (!IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL))
    bug("jail_execute: ch not in room marked ROOM_JAILCELL", 0);

  jail = jail_cell_lookup(ch->in_room->vnum);
  exit = jail_table[jail].exe_room;

  /* if exit is 0 we send to home temple */
  if ( (room = get_room_index(exit))  == NULL){
    char buf[MIL];
    sprintf(buf, "jail_execute: room %d does not exist as an exe_room.",
	    exit);
    bug(buf, 0);
  }
  if (!room)
    room = get_room_index(get_temple(ch));
  if (room == NULL){
    send_to_char("You are completely lost.\n\r",ch);
    return;
  }
  /* move the char out of jail, announce */
  char_from_room(ch);
  char_to_room(ch, room);
  act("The guards escort $n into the area.", ch, NULL, NULL, TO_ROOM);
  do_look(ch, "auto");
  /* Announce one last time */
/* we make a bitvector of all the cimes the person has commited */
  for (crime = ch->affected; crime != NULL; crime = crime->next){
    if (crime->type == gsn_wanted && crime->bitvector != 0){
      crimes |= crime->bitvector;
    }
  }
  sprintf( crime_str, "For the crimes of " );
  if (IS_SET(crimes, CRIME_FLAG_S)){
    strcat( crime_str, "Not sheathing" );
    strcat( crime_str, ", ");
  }
  if (IS_SET(crimes, CRIME_FLAG_L)){
    strcat( crime_str, crime_table[CRIME_LOOT].name );
    strcat( crime_str, ", ");
  }
  if (IS_SET(crimes, CRIME_FLAG_T)){
    strcat( crime_str, crime_table[CRIME_THEFT].name );
    strcat( crime_str, ", ");
  }
  if (IS_SET(crimes, CRIME_FLAG_A)){
    strcat( crime_str, crime_table[CRIME_ASSAULT].name );
    strcat( crime_str, ", ");
  }
  if (IS_SET(crimes, CRIME_FLAG_M)){
    strcat( crime_str, crime_table[CRIME_MURDER].name );
    strcat( crime_str, ", ");
  }
  if (IS_SET(crimes, CRIME_FLAG_O)){
    strcat( crime_str, crime_table[CRIME_OBSTRUCT].name );
    strcat( crime_str, ", ");
  }

  sprintf(buf, "%sby the Law of Aabahran,\n\r"\
	  "%s has been sentenced by %s %s\n\r"\
	  "May Virigoth have mercy on your soul %s.",
	  crime_str,
	  PERS2(ch), IS_NULLSTR(name) ? "Justice" : name,
	  execution_table[exe_type].announce,
	  PERS2(ch));
  jail_announce(jail, buf);

  /* show the TOVICT messages from the table*/
  act(execution_table[exe_type].pre_act, mob, NULL, ch, TO_CHAR);
  act(execution_table[exe_type].on_act_r, mob, NULL, ch, TO_NOTVICT);
  act(execution_table[exe_type].on_act_c, mob, NULL, ch, TO_VICT);

  /* create bag with stuff (NULL if no bag, or no stuff to bag)*/
  bag = create_jailbag(mob, ch, FALSE, TRUE);

  /* get rid of flag */
  if (!IS_OUTLAW( ch ))
    REMOVE_BIT(ch->act, PLR_WANTED);
  affect_strip( ch, gsn_wanted );

  /* give penalty */
  if (!IS_OUTLAW( ch ))
    ch->exp -= UMIN(6000,UMIN(exp_per_level(ch,ch->level),ch->exp/10));

  /* create the object */
  obj = create_object( get_obj_index( OBJ_VNUM_EXECUTION ), 0);
  free_string(obj->description);
  sprintf(buf, execution_table[exe_type].room_desc, PERS2(ch));
  obj->description = str_dup( buf );
  obj->timer = dur;
  obj_to_room(obj, room);

  /* broadcast on justice channel */
  sprintf(buf, "%s has been executed by %s as per %s's orders.  The Justice prevails!", PERS2(ch), PERS2(mob), IS_NULLSTR(name) ? "Justice" : name);
  cabal_echo_flag( CABAL_JUSTICE, buf );


  /* slay him */
  sprintf( log_buf, "`&%s executed at %s [room %d]``",
	   ch->name,
	   ch->in_room->name,
	   ch->in_room->vnum);

  nlogf( "%s", log_buf );
  wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);
  ch->pcdata->dall++;
  ch->pcdata->dpc++;
  mud_data.deaths++;

  stop_fighting( ch, TRUE );
  if (!IS_NPC(ch) && IS_SET(ch->act, PLR_DOOF))
    REMOVE_BIT(ch->act, PLR_DOOF);
  death_cry( ch, mob );

  for ( vch = char_list; vch != NULL; vch = vch_next ){
    vch_next = vch->next;
    if (IS_NPC(vch) && vch->hunting != NULL && vch->hunting == ch ){
      vch->hunting = NULL;
      if (IS_SET(vch->act,ACT_AGGRESSIVE))
	{
	  act( "$n slowly disappears.",vch,NULL,NULL,TO_ROOM );
	  extract_char( vch, TRUE );
	}
    }
  }
  sendf(ch, "\n\rAs the cold chill of death descends upon you %s grants you yet another chance.\n\r"\
	"You have been granted a temporary sanctuary as an invincible ghost.\n\rAs long as you don't attack"\
	" anything.\n\r", (ch->pcdata->deity[0] == '\0' ?  "The Creator" : ch->pcdata->deity));

  tattoo = get_eq_char(ch, WEAR_TATTOO);
  if (tattoo != NULL)
    obj_from_char(tattoo);

  extract_char( ch, FALSE );

  if (tattoo != NULL)
    {
      obj_to_char(tattoo, ch);
      equip_char(ch, tattoo, WEAR_TATTOO);
    }

  /* move the bag to ex-criminal */
  if (bag){
    obj_from_char(bag);
    obj_to_char(bag, ch);
  }

  /* Destory the mob */
  char_from_room(mob);
  extract_char(mob, TRUE);


  ch->position	= POS_STANDING;
  ch->hit		= 1;
  ch->mana	= 1;
  ch->move	= UMAX( 100, ch->move );

  ch->pcdata->ghost = time(NULL);
  ch->pcdata->fight_delay = time(NULL);
  ch->pcdata->pk_delay = time(NULL);
  ch->pcdata->condition[COND_HUNGER]=48;
  ch->pcdata->condition[COND_THIRST]=48;
  if (!IS_NPC(ch) && ch->pcdata->familiar != NULL){
    extract_char( ch->pcdata->familiar, TRUE );
    ch->pcdata->familiar=NULL;
  }
  if (ch->pcdata->dall > 60 && get_trust(ch) < MASTER){
    send_to_char("You have died due to failing health.\n\r",ch);
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
    wiznet("$N dies due to failing health.",ch,NULL,0,0,0);
    ch->pcdata->dall = 0;
    if (ch->level > 20)
      deny_record(ch);
    if (ch->pCabal){
      CABAL_DATA* pCab = ch->pCabal;
      update_cabal_skills(ch, ch->pCabal, TRUE, TRUE);
      char_from_cabal( ch );
      CHANGE_CABAL(pCab);
      save_cabals( TRUE, NULL );
    }
    if (ch->pcdata->pbounty){
      rem_bounty( ch->pcdata->pbounty );
      free_bounty( ch->pcdata->pbounty );
    }
    extract_char( ch, TRUE );
    if ( ch->desc != NULL )
      close_socket( ch->desc );
    unlink(strsave);
    return;
  }
  if (ch->perm_stat[STAT_CON] > 3){
    if (ch->pcdata->dall%5 == 0)
      {
	ch->perm_stat[STAT_CON]--;
	send_to_char("You feel less healthy.\n\r",ch);
      }
  }
  /* Cabal Requip */
  save_char_obj( ch );
}


/* Written by: Virigoth							*
 * Returns: Void							*
 * Used: jail.c,interp.c						*
 * Comment: Allows to pay the bribe to current justice overseeing char.
 * if justice is not present, gold is deducted but not deposited anywhere */
void do_bail( CHAR_DATA *cha, char *argument ){
  /* bail <target> <gold/cp>, if <target> is "gold" or "cp" is ignored */

  AFFECT_DATA* paf;
  CHAR_DATA* justice;
  CHAR_DATA* ch = cha;

  char arg1[MIL];

  int bail = 0;
  int assets = 0;

  bool fGold = FALSE;

  argument = one_argument(argument, arg1);
  /* select between paying for self, or other */
  if (arg1[0] != '\0' && str_cmp("gold", arg1)
      && str_cmp("cp", arg1)){
    if ( (ch = get_char_world(cha, arg1)) == NULL){
      send_to_char("You cannot locate them.\n\r", cha);
      return;
    }
    if (IS_NPC(ch)){
      send_to_char("Pay bail for a mob?  What a grand idea!\n\r", cha);
      return;
    }
  }

  /* now "ch" is proper target */
  /* check if affected by bail */

  if ( (paf = affect_find(ch->affected, gen_jail)) == NULL
       || paf->modifier != JAIL_BAIL){
    send_to_char("There is no need or no possibility of paying bail.\n\r", cha);
    return;
  }
  /* calculate bail */
  bail = paf->level * SEV_BAIL;

  /* check for latter arguments (no gold/cp argument)*/
  if (ch == cha){
    if (arg1[0] == '\0'){
      /* Cabaled char's get to pay with cp too */
      if (!ch->pCabal)
	sendf(cha, "Your bail has been set at %d, use \"bail gold\" to pay.\n\r", bail);
      else
	sendf(cha, "Your bail has been set at %d Gp or %d Cp, use \"bail <gold/cp>\" to pay.\n\r", bail, bail / 100);
    return;
    }
    else if (!str_cmp("gold", arg1)){
      assets = ch->in_bank + ch->gold;
      fGold = TRUE;
    }
    else if (!str_cmp("cp", arg1)){
      assets = GET_CP(ch);
      bail /= 100;
    }
    else{
      send_to_char("Bail <criminal> <gold/cp>\n\rBail <gold/cp>\n\r", cha);
      return;
    }
  }
  else{
    if (argument[0] == '\0'){
      /* Cabaled char's get to pay with cp too */
      if (!ch->pCabal)
	sendf(cha, "Their bail has been set at %d, use \"bail gold\" to pay.\n\r", bail);
      else
	sendf(cha, "Their bail has been set at %d Gp or %d Cp, use \"bail <gold/cp>\" to pay.\n\r", bail, bail / 100);
      return;
    }
    else if (!str_cmp("gold", argument)){
      assets = ch->in_bank + ch->gold;
      fGold = TRUE;
    }
    else if (!str_cmp("cp", argument)){
      assets = GET_CP(ch);
      bail /= 100;
    }
    else{
      send_to_char("Bail <criminal> <gold/cp>\n\rBail <gold/cp>\n\r", cha);
      return;
    }
  }

/* check our account */
if (assets < bail){
    sendf(cha ,"Your total assets are %d (%s) which is not enough to pay the bail of %d (%s).\n\r",
	  assets, fGold ? "gold" : "cp" , bail, fGold ? "gold" : "cp");
    return;
  }
  if (fGold){
    /* start withdrawing */
    cha->in_bank -= bail;
    if (cha->in_bank < 0){
      cha->gold += cha->in_bank;
      cha->in_bank= 0;
    }
    sendf(cha, "Your bank balance is %d, you have %ld gold left on you.\n\r", cha->in_bank, cha->gold);
  }
  else
    CP_GAIN(cha, -bail, TRUE);

  if ( (justice = get_char(paf->string)) == NULL)
    return;

  unjail_char(ch);

  /* give justice gold */
  if (fGold){
    justice->in_bank += bail;
    sendf(justice, "%s has paid the bail. Your bank balance is: %d.\n\r", PERS2(ch), justice->in_bank);
  }else
    CP_GAIN(justice, bail, TRUE);
}


/* Written by: Virigoth							*
 * Returns: TRUE if cells have power					*
 * Used: jail.c, act_move.c						*
 * Comment:Compares the vnum of room passed to jail_table for match of  *
 * cell. If match is made, checks [].guardhouse for any mob marked	*
 * "guild_guard", if found returns TRUE, if not, FALSE			*/
bool jail_check(ROOM_INDEX_DATA* room){
  CHAR_DATA* vch;
  ROOM_INDEX_DATA* enter;
  int jail = jail_cell_lookup(room->vnum);

  /* try to get index to guardhouse */
  if ( (enter = get_room_index(jail_table[jail].guardhouse)) == NULL){
    char buf[MIL];
    sprintf(buf ,"jail_check: Jail guardhouse %d could not be found.",
	    jail_table[jail].guardhouse);
    bug(buf, 0);
    return FALSE;
  }

  /* all we do is check the guardhouse for cabal_guard */
  for (vch = enter->people; vch; vch = vch->next_in_room){
    if (IS_NPC(vch) && IS_SET(vch->off_flags, GUILD_GUARD))
      return TRUE;
  }
  return FALSE;
}


/* Written by: Virigoth							*
 * Returns: void							*
 * Used: jail.c, act_move.c						*
 * Comment: If ch is in powered jailcell and has no gen_jail, gsn_judge *
 * is applied with increasing modifier.  Past certain point (max_mod)	*
 * ch is teleported out of jail, and will not be able to pass jail fields*
 * if fGain == TRUE we only check gain the the affect (vtick) FALSE we	*
 * check lowering of the affect (tick)
 * This is run once per 3 sec (10 x Tick).				*/
void jail_abuse_check(CHAR_DATA* ch, bool fGain){
  AFFECT_DATA* paf, af;

  int mod = 0;

  const int ratio = ABUSE_RATIO;	//ratio of time in CELL : OUTSIDE
  const int max_mod = ABUSE_KICK; //How many iterations before being kicked
				  //if in a cell. (10 per tick)

  /* check if this is gain and we are in a cell */
  if (fGain && !IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL))
    return;
  /* check if this is an inmate */
  if (IS_IMMORTAL(ch) || IS_NPC(ch) || IS_WANTED(ch) || is_affected(ch, gen_jail))
    return;

  /* check if affect exists */
  if ( (paf = affect_find(ch->affected, gsn_judge)) != NULL)
    mod = paf->modifier;

  /* check if in jailcell if its powered we add */
  if (IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL)
      && jail_check(ch->in_room))
    mod += ratio;
  else if (!fGain)
    mod -= PULSE_TICK / PULSE_VIOLENCE;
  else
    mod --;

  /* safety */
  mod = URANGE(-1, mod, 3000);

  /* add on or apply the mod */
  if (!paf){
    af.type = gsn_judge;
    af.level = 1;
    af.duration = -1;
    af.where = TO_NONE;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = mod;
    if (mod > 0)
      affect_to_char(ch, &af);
    return;
  }
  else
    paf->modifier = mod;

  /* check if we should kick them out */
  if (mod > max_mod){
    AFFECT_DATA* paf;
    ROOM_INDEX_DATA* room;
    int jail = jail_cell_lookup(ch->in_room->vnum);
    int exit = jail_table[jail].guardhouse;

    /* if exit is 0 we send to home temple */
    if ( (room = get_room_index(exit))  == NULL){
      char buf[MIL];
      sprintf(buf, "jail_abuse_check: room %d does not exist as an exit.",
	      exit);
      bug(buf, 0);
    }
    if (!room)
      room = get_room_index(get_temple(ch));
    if (room == NULL){
      send_to_char("You are completely lost.\n\r",ch);
      return;
    }
    /* we have the room, so we move them */
    act("A squad of guards surrounds the cell and quicly escort $n out.", ch, NULL, NULL, TO_ROOM);
    act("A squad of guards surrounds the cell and quicly escort you out.", ch, NULL, NULL, TO_CHAR);
    char_from_room(ch);
    char_to_room(ch, room);
    act("A squad of jail guards escort $n in and leave.", ch, NULL, NULL, TO_ROOM);
    do_look(ch, "auto");
    /* decreae the modifier byone ratio at least so we dont get double transport */
    if ( (paf = affect_find(ch->affected, gsn_judge)) != NULL)
      paf->modifier = UMAX(0, paf->modifier - ABUSE_RATIO);
  }//END kick out
  else if (mod < 1)
    affect_strip(ch, gsn_judge);
}

/* checks if a given crime in given room is a crime */
bool is_crime( ROOM_INDEX_DATA* room, int crime, CHAR_DATA* victim){
  if (room == NULL || crime >= MAX_CRIME)
    return FALSE;
  else if (!IS_SET(room->area->area_flags, AREA_LAWFUL))
    return FALSE;
  else if (victim && IS_NPC(victim) && IS_SET(victim->act, ACT_RAIDER))
    return FALSE;
  else if (victim && victim->pCabal && IS_CABAL(victim->pCabal, CABAL_JUSTICE))
    return TRUE;
  else if (room->area->crimes[crime] != 0)
    return TRUE;
  else
    return FALSE;
}

/* marks a given character for a given crime using the "Wanted" skill */
/* information is stored in a seperate wanted effect for each city
   crimes commited	: bitvector
   city			: location
   times flagged	: modifier (if its none zero then the crime has not been flagged for yet)

*/
AFFECT_DATA* set_crime( CHAR_DATA* ch, CHAR_DATA* victim, AREA_DATA* area, int crime){
  int dur = 1440;
  AFFECT_DATA* paf;
  CHAR_DATA* vch;
  int city = 0;
  bool fJustice = FALSE;

  if (ch == NULL || IS_NPC(ch) || area == NULL || crime >= MAX_CRIME)
    return NULL;
  else{
    city = area->vnum;
    dur = URANGE(1, ch->pcdata->flagged, 10) * dur / 10;
  }

  if (area->pCabal != NULL && IS_AREA(area, AREA_CABAL) && IS_CABAL(area->pCabal, CABAL_JUSTICE))
    fJustice = TRUE;

  /* don't bother if victim is wanted and attack is justice */
  if (!fJustice && ch->pCabal && IS_CABAL(ch->pCabal, CABAL_JUSTICE) && victim && IS_WANTED(victim))
    return NULL;

  //if in justice cabal we do not need to check
  /* run through the area checking if there are justice about */
  for (vch = player_list; vch; vch = vch->next_player){
    if (!IS_NPC(vch) && vch->desc && vch->in_room->area == area
	&& vch->pCabal
	&& (IS_CABAL(vch->pCabal, CABAL_JUSTICE) || IS_HIGHBORN(ch))
	&& !IS_AFFECTED(vch, AFF_INVISIBLE)
	&& !IS_AFFECTED(vch, AFF_HIDE)
	&& !IS_AFFECTED2(vch, AFF_CAMOUFLAGE))
      break;
  }
  if (!fJustice && vch == NULL){
    /* run through cabals check if justice parent cabal has any players in it */
    CABAL_DATA* pCab;
    for (pCab = cabal_list; pCab; pCab = pCab->next){
      if (!IS_CABAL(pCab, CABAL_JUSTICE) || pCab->parent)
	continue;
      if (pCab->present < 1)
	continue;
      else
	break;
    }
    /* the crime was not spotted, we affect the relation ships adversly */
    if (area->pCabal && pCab)
      affect_justice_relations( area->pCabal, -5 );
    return NULL;
  }
  /* we run through all the pafs on the character looking wanted
     with given city
  */

  for (paf = ch->affected; paf; paf = paf->next){
    if (paf->type == gsn_wanted && paf->location == city)
      break;
  }

  /* now we either have a paf or not */
  if (paf == NULL){
    AFFECT_DATA af;

    af.type	=	gsn_wanted;
    af.duration	=	dur;
    af.level	=	60;

    af.where	=	TO_NONE;
    af.bitvector=	0;
    af.location	=	city;
    //new crime, will be put into bitvector when wanted.
    if (victim && victim->pCabal && IS_CABAL(victim->pCabal, CABAL_JUSTICE))
      af.modifier =	crime_table[CRIME_OBSTRUCT].bit;
    else
      af.modifier =	crime_table[crime].bit;

    paf = affect_to_char( ch, &af);
  }
  else{
/* we update an existing crime info for a city */
    paf->modifier	|= crime_table[crime].bit; //add this crime to the ones waiting to be put in bitvector when wanted
    paf->duration	= dur;
  }
  affect_strip(ch, gsn_timer );
  return paf;
}

/* shows the crimes victim has been flagged for, and returns the maximum penalty */
int show_crimes( CHAR_DATA* ch, CHAR_DATA* victim, bool fPunish, bool fPenOnly ){
  char buf[MSL];
  char buf2[MIL];
  AFFECT_DATA* paf;
  AREA_DATA* pArea;

  int max_pen = CRIME_ALLOW;

  buf[0] = '\0';
  for (paf = victim->affected; paf; paf = paf->next){
    if ( paf->type == gsn_wanted && paf->bitvector != 0
	 && (pArea = get_area_data( paf->location )) != NULL){
      bool fFirst = TRUE;
      int i = 0;
      /* we run through all crimes making a list */
      for (i = 0; i < MAX_CRIME; i++){
	if (!IS_SET(paf->bitvector, crime_table[i].bit))
	  continue;
	if (fFirst){
	  sprintf(buf2, "%-15s (%2dd): %-10s %s%s%s\n\r",
		  pArea->name,
		  paf->duration / 24,
		  crime_table[i].name,
		  fPunish ? "(" : "",
		  fPunish ? punish_table[pArea->crimes[i]].name : "",
		  fPunish ? ")" : "");
	  fFirst = FALSE;
	}
	else{
	  sprintf(buf2, "%-20s : %-10s %s%s%s\n\r",
		  "",
		  crime_table[i].name,
		  fPunish ? "(" : "",
		  fPunish ? punish_table[pArea->crimes[i]].name : "",
		  fPunish ? ")" : "");
	}
	strcat( buf, buf2 );
	/* select the highest penalty */
	if (punish_table[pArea->crimes[i]].bit > max_pen)
	  max_pen = punish_table[pArea->crimes[i]].bit;
      }
    }
  }

  if (IS_NULLSTR(buf) || fPenOnly)
    return max_pen;
  else if ( ch == victim )
    send_to_char("You are wanted for following crimes:\n\r", ch);
  else
    sendf( ch, "%s is wanted for following crimes:\n\r", PERS2( victim ));
  send_to_char( buf, ch );
  return max_pen;
}

/* returns an integer representing a crime or -1 */
int crime_lookup (const char *name ){
    int crime;
    for (crime = 0; crime_table[crime].name != NULL; crime++){
      if (LOWER(name[0]) == LOWER(crime_table[crime].name[0]) && !str_prefix(name, crime_table[crime].name))
	return crime;
    }
    return -1;
}










