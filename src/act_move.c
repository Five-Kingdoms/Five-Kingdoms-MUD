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
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "jail.h"
#include "cabal.h"
#include "autoquest.h"

char *	const	dir_name	[]		=
{ "north", "east", "south", "west", "up", "down" };

char *	const	shrt_dir_name	[]		=
{ "N", "E", "S", "W", "U", "D" };

char *	const	dir_name2	[]		=
{ "north of you", "east of you", "south of you", "west of you", "above you", "below you" };

const	sh_int	rev_dir		[]		=
{ DIR_SOUTH, DIR_WEST, DIR_NORTH, DIR_EAST, DIR_DOWN, DIR_UP };

const	sh_int	movement_loss	[MAX_SECT]	=
//INSIDE CITY FIELD FORST HILLS MOUNT SWIM NOSWIM SWAMP AIR DESERT LAVA SNOW
{   1,    2,   3,    4,    4,    7,    5,    4,    10,   1,   8,    8,   6 };

extern void dreamprobe_cast( CHAR_DATA* victim, char* msg );

/*room_spec_update*/
/*linked only in handler.c*/
/*returns true if flag was cleared, false otherwise*/
bool room_spec_update(ROOM_INDEX_DATA* in_room)
{
  CHAR_DATA* vch;

  //This function checks the contents of the room
  //in order to decide if the room should be stripped
  //of the spec_flag

  //Eventualy the rooms will have effects attached to themselfs.
  //for now we check each spell individualy.

if (!IS_SET(in_room->room_flags2, ROOM_SPEC_UPDATE))
  return TRUE;

//Begin running through each effect, testing if they cause the flag to stay.
//if test for flag stay is true we return saving more checks.

  /////////FIRESTORM///////////
 for  (vch = in_room->people; vch != NULL; vch = vch->next_in_room) {
  if (is_affected(vch, gen_firestorm) || is_affected (vch, gen_anger))
    return FALSE;
 }

//We strip the effect.
 REMOVE_BIT(in_room->room_flags2, ROOM_SPEC_UPDATE);
 return TRUE;
}//end room_spec_update

int MOVE_CHECK( CHAR_DATA* ch, ROOM_INDEX_DATA** to_room ){
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;
  int result = 0;
  int fSuccess = TRUE;

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL && paf->type == paf_next->type) {
      continue;
    }

    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(vch, NULL, *to_room, paf->type, NULL, 
				      NULL, TRUE, EFF_UPDATE_MOVE)) != TRUE) {
	if (result == -1) {
	  return -1;
	}
	else {
	  fSuccess = result;
	}
      }
    }
  }
  return fSuccess;
}

//------PART 2----------/
void do_out(CHAR_DATA *ch, int door)
{
    CHAR_DATA *ich;
    for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
    {

	if (ich == ch)
	    continue;
	if (IS_IMMORTAL(ich) && ich->level >= ch->level && IS_SET(ich->act,PLR_HOLYLIGHT))
	  act_new( "$n leaves $t.", ch, dir_name[door], ich, TO_VICT, POS_RESTING + 100 );
	else if (ich->race == grn_feral && IS_AWAKE(ich) && !IS_IMMORTAL(ch)
		 && (IS_AFFECTED(ch, AFF_SNEAK) || ch->class == gcn_ranger)){
	  act_new( "Someone leaves $t.", ch, dir_name[door], ich, TO_VICT, POS_RESTING);
	}
	else if (!IS_IMMORTAL(ch)
		 && is_affected(ich, gsn_sense_motion) 
		 && get_skill(ich,gsn_sense_motion) 
		 > number_percent() && monk_good(ch, WEAR_HEAD) )
	  act_new( "Someone leaves the room.", ch, NULL, ich, TO_VICT, POS_RESTING + 100 );
    }
}

void do_in(CHAR_DATA *ch)
{
    CHAR_DATA *ich;
    for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
    {
	if (ich == ch)
	    continue;
	if (IS_IMMORTAL(ich) && ich->level >= ch->level && IS_SET(ich->act,PLR_HOLYLIGHT))
	    act_new( "$n enters the room.", ch, NULL, ich, TO_VICT, POS_RESTING + 100 );
	else if (ich->race == grn_feral && IS_AWAKE(ich) && !IS_IMMORTAL(ch)
		 && (IS_AFFECTED(ch, AFF_SNEAK) || ch->class == gcn_ranger)){
	  send_to_char( "Someone enters the room.\n\r", ich);
	}
	else if (!IS_IMMORTAL(ch)
		 && is_affected(ich, gsn_sense_motion) && get_skill(ich,gsn_sense_motion) > number_percent() && monk_good(ch, WEAR_HEAD) )
	    act_new( "Someone enters the room.", ch, NULL, ich, TO_VICT, POS_RESTING + 100 );
    }
}

bool find_kick_exit(CHAR_DATA *ch, CHAR_DATA *victim)
{
    ROOM_INDEX_DATA *in_room, *to_room;
    EXIT_DATA *pexit;
    bool found = FALSE;
    int i;
    if (ch->in_room == NULL || victim->in_room == NULL)
	return FALSE;
    in_room = ch->in_room;
    to_room = ch->in_room;
    for (i = 0; i < 6; i++)
    {
	if ( ( pexit = in_room->exit[i] ) == NULL || ( to_room = pexit->to_room ) == NULL || !can_see_room(ch,pexit->to_room))
	    continue;
	if (to_room->area != in_room->area)
	{
	    found = TRUE;
	    break;
	}
    }
    if (found)
    {
	do_say(ch,"Ghosts and newbies are not allowed in here!");
	act("$N throws you out of the room!",victim,NULL,ch,TO_CHAR);
	act("$N throws $n out of the room!",victim,NULL,ch,TO_NOTVICT);
	act("You throw $N out of the room!",ch,NULL,victim,TO_CHAR);
	char_from_room(victim);
	char_to_room(victim, to_room);
	act_new( "$n is thrown in.", victim, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	do_look(victim,"auto");
    }
    return found;
}

//////////////////
//vamp_can_sneak//
/////////////////
//Cheks if vampire can sneak in the given room.
//returns true if can.

bool vamp_can_sneak(CHAR_DATA* ch, ROOM_INDEX_DATA* in_room)
{
//IF not vamp, or in mist form or bat form, cannot sneak 
  if (ch->class != class_lookup("vampire"))
    return FALSE;
  if ( (is_affected(ch, gsn_bat_form)) || (is_affected(ch, gsn_mist_form)) )
    return FALSE;

//No we know he is a vamp, and maybe in wolf form
//If is in wolf-form and  is in forest, hills, or 
//mountain then vamp can sneak.

  if ( (is_affected(ch,gsn_wolf_form))
       && (in_room->sector_type == SECT_FOREST 
	   || in_room->sector_type == SECT_HILLS 
	   || in_room->sector_type == SECT_SNOW
	   || in_room->sector_type == SECT_MOUNTAIN 
	   || in_room->sector_type == SECT_FIELD
	   || in_room->sector_type == SECT_DESERT
	   || in_room->sector_type == SECT_SWAMP))
    return TRUE;

//IF not in wolf from, or not in forest like terrain we check for city sneak.
//We already  know he is vamp, not in any of forms so must be human.
//al is left is to check for terrain.

  else if ( (in_room->sector_type == SECT_CITY || in_room->sector_type == SECT_INSIDE) && (!is_affected(ch, gsn_wolf_form)) )
  return  TRUE;

//if non of the above returned true by now, then he cannot sneak.
  return FALSE;
}

/* checks if disguise allowed someone to sneak past the mob */
bool disguise_check( CHAR_DATA* ch, int iClass ){
  AFFECT_DATA* paf;
  if (ch == NULL || iClass < 0 )
    return FALSE;
  else if ( (paf = affect_find(ch->affected, gen_guise)) != NULL
       && iClass == paf->modifier){
    send_to_char("You attempt to sneak past in your disguise.\n\r", ch);
    /* check if disguised thief suceds walkign past */
    if (number_percent() > UMAX(50, get_skill(ch, gsn_sneak) - 5)){
      send_to_char("You got caught!\n\r", ch);
      check_improve(ch, gsn_sneak, FALSE, 35);
      affect_strip(ch, gen_guise);
      return FALSE;
    }
    else{
      check_improve(ch, gsn_sneak, TRUE, 5);
      return TRUE;
    }
  }
  return FALSE;
}
	

void move_char( CHAR_DATA *ch, int door, bool fForce )
{
  AFFECT_DATA *paf, *pRipple = NULL;
  CHAR_DATA *fch, *fch_next, *wch;
  ROOM_INDEX_DATA *in_room, *to_room;
  EXIT_DATA *pexit;
  char buf[MSL];
  bool found = FALSE, condor = FALSE, fEnfeeble = FALSE;
  int i, first = door, second = door, chance = number_percent();
  int cskill = get_skill(ch,gsn_quiet_movement);
  int climb = 0;
  int lag = 1;
  if ( door < 0 || door > 5 )
    {
      bug( "Do_move: bad door %d.", door );
      return;
    }
  if (cskill > 1)
    {
      cskill += 2*(get_curr_stat(ch,STAT_LUCK) - 16);
      if (IS_WEREBEAST(ch->race))
	cskill += 10;
    }
  if(IS_AFFECTED2(ch,AFF_SEVERED))
    {
        send_to_char("But you haven't got any legs!\n\r",ch);
        return;
    }
  //fForce causes us to ignore misdirection
  if (IS_AFFECTED2(ch,AFF_MISDIRECTION) && !fForce)
    {
      for ( paf = ch->affected; paf != NULL; paf = paf->next )
        {
	  if (paf->type == gsn_misdirection && paf->location == APPLY_DEX)
	    first = paf->level;
	  if (paf->type == gsn_misdirection && paf->location == APPLY_INT)
	    second = paf->level;
	}
      if (door == first)
	door = second;
      else if (door == second)
	door = first;
    }
  else if (is_song_affected(ch,gsn_disorient))
    {
      for ( paf = ch->affected2; paf != NULL; paf = paf->next )
        {
	  if (paf->type == gsn_disorient && paf->location == APPLY_DEX)
	    first = paf->level;
	  if (paf->type == gsn_disorient && paf->location == APPLY_INT)
	    second = paf->level;
	}
      if (door == first)
	door = second;
      else if (door == second)
	door = first;
    }
/* catch death shroud move */
  i =0;
  if ( (paf = affect_find(ch->affected, gen_d_shroud)) != NULL){
    run_effect_update(ch, paf, 0, paf->type, &i , NULL, TRUE, 
		      EFF_UPDATE_PRECOMMAND);
  }
  
  in_room = ch->in_room;
  if ( ( pexit = in_room->exit[door] ) == NULL || ( to_room = pexit->to_room ) == NULL || !can_see_room(ch,pexit->to_room))
    {
      send_to_char( "Alas, you cannot go that way.\n\r", ch );
      return;
    }
  if (IS_SET(pexit->exit_info, EX_CLOSED) 
      && ((!IS_AFFECTED(ch, AFF_PASS_DOOR) && !is_ghost(ch,600)) || IS_SET(pexit->exit_info,EX_NOPASS))
      && !IS_TRUSTED(ch,ANGEL))
    {
      act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
      return;
    }
  if ( IS_AFFECTED(ch, AFF_CHARM) 
       && ch->master != NULL 
       && in_room == ch->master->in_room 
       && !is_affected(ch,gsn_linked_mind) && !fForce)
    {
      send_to_char( "What?  And leave your beloved master?\n\r", ch );
      return;
    }
  if ( room_is_private( to_room ) )
    {
      send_to_char( "That room is private right now.\n\r", ch );
      return;
    }
  if ( !have_access(ch,to_room))
    {
      send_to_char( "Don't think so.\n\r",ch);
      return;
    }
  if ( IS_SET(to_room->room_flags,ROOM_NO_GHOST) && !IS_IMMORTAL(ch) && is_ghost(ch,600))
    {
      send_to_char("Ghosts are not allowed in there!\n\r",ch);
      return;
    }

  /* CABAL GATE */
  if (ch->incog_level == 0
      && ch->invis_level == 0
      && ch->in_room->pCabal 
      && ch->in_room->pCabal->fGate
      && ch->in_room->pCabal->anchor 
      && ch->in_room == ch->in_room->pCabal->anchor
      && to_room->pCabal){
    if (IS_NPC(ch) && IS_SET(ch->act, ACT_RAIDER)){
      act("$n scales the walls over the gate.", ch, NULL, NULL, TO_ROOM);
    }
    else{
      send_to_char( ch->in_room->pCabal->pIndexData->gate_msg, ch );
      send_to_char("\n\r", ch);
      return;
    }
  }

/* traps */
  if (trip_traps(ch, pexit->traps)){
    //fForce causes us to walk through traps while hitting them
    if (!fForce)
      return;
    //need a pair of special checks since the trap above might not block rest
    //of move after death
    if (ch == NULL || ch->in_room == NULL)
      return;
    else if (is_ghost(ch, 1))
      return;
  }

/* move update */
  if ( MOVE_CHECK(ch, &to_room) == -1)
    bug("Error at: MOVE_CHECK returned failure.\n\r", 0);  

  /* movement restrictions */
  /* Exit Triggers */
  if ( !IS_NPC(ch) && ch->in_room
       && (p_exit_trigger( ch, door, PRG_MPROG )
	   ||  p_exit_trigger( ch, door, PRG_OPROG )
	   ||  p_exit_trigger( ch, door, PRG_RPROG )) )
    return;
  if (ch == NULL || ch->in_room == NULL)
    return;
  /* JAILS (EVERYONE) (Only when moving from non-cell to cell or cell-noncell*/
  if ( (IS_SET(to_room->room_flags2, ROOM_JAILCELL)
	&& !IS_SET(in_room->room_flags2, ROOM_JAILCELL)
	&& jail_check(to_room))
       || (!IS_SET(to_room->room_flags2, ROOM_JAILCELL)
	   && jail_check(in_room)
	   && IS_SET(in_room->room_flags2, ROOM_JAILCELL)) ){
    
    /* stops only Non-Justices, and any wanteds*/
    if (!IS_NPC(ch) && ch->pCabal && IS_CABAL(ch->pCabal, CABAL_JUSTICE)
	&& !IS_IMMORTAL(ch)
	&& !IS_SET(ch->act, PLR_WANTED)){
      AFFECT_DATA* paf;
      /* check for ABUSE_CHECK (on entrance to the cell only)*/
      if ( IS_SET(to_room->room_flags2, ROOM_JAILCELL)
	   && (paf = affect_find(ch->affected, gsn_judge)) != NULL
	   && paf->modifier > ABUSE_CHECK
	   && !IS_IMMORTAL(ch)){
	act("A magical barrier crackles as it stops $n.", ch, NULL, NULL, TO_ROOM);
	act("You've spent too much time in the cells.  Wait a while.", ch, NULL, NULL, TO_CHAR);
	return;
      }
      act("A magical barrier parts about $n.", ch, NULL, NULL, TO_ROOM);
      act("A magical barrier parts about you.", ch, NULL, NULL, TO_CHAR);
    }
    else if (!IS_IMMORTAL(ch)){
      act("A magical barrier crackles as it stops $n.", ch, NULL, NULL, TO_ROOM);
      act("A magical barrier crackles as it stops you.", ch, NULL, NULL, TO_CHAR);
      return;
    }
  }
  
  /* guilds and cabals (PC ONLY)*/
  if ( !IS_NPC(ch)){
      int iClass, iGuild, iJail, move;
      CHAR_DATA *mob;
      bool fbloody = FALSE, fguild = FALSE;
      
      if (!IS_NPC(ch) && !IS_IMMORTAL(ch)){
	int class = ch->class;
	int dkClass = class_lookup("dark-knight");
	bool fJustice = (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_JUSTICE) && IS_AREA(ch->in_room->area, AREA_LAWFUL));
	if (IS_SET(to_room->room_flags2, ROOM_GUILD_ENTRANCE)){
	  for ( iClass = 0; class_table[iClass].name != NULL && iClass < MAX_CLASS; iClass++ ){
	    for ( iGuild = 0; iGuild < MAX_GUILD; iGuild ++) {
	      if ( to_room->vnum == class_table[iClass].guild[iGuild]){
		fbloody = TRUE;
		if (is_exiled(ch, ch->in_room->area->vnum))
		  continue;
		if (disguise_check( ch, iClass )){
		  fguild = TRUE;
		}
		else if ((iClass == class 
			  || (IS_MASQUE(ch) && iClass == dkClass)
			  || fJustice)
			 && (!IS_SET(ch->act,PLR_OUTCAST) || fJustice))
		  fguild = TRUE;
	      }
	    }
	  }
	}//END guilds
	/* check for jail */
	for ( iJail = 0; jail_table[iJail].name != NULL && iJail < MAX_HOMETOWN; iJail++ ){
	  if (!IS_IMMORTAL(ch))
	    break;
	  if (to_room->vnum == jail_table[iJail].entrance){
	    if (ch->pCabal == NULL || !IS_CABAL(ch->pCabal, CABAL_JUSTICE)
		|| IS_WANTED(ch))
	      fguild = FALSE;
	    else
	      fguild = TRUE;
	    fbloody = TRUE;
	    break;
	  }
	}//END jails
	if (ch->class == gcn_adventurer)
	  fguild = TRUE;
	if (fbloody && !fguild){
	  for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)
	    if (IS_NPC(mob) && IS_SET(mob->off_flags,GUILD_GUARD))
	      {
		mprog_guild_trigger(mob,ch);
		return;
	      }
	}
	else if (fbloody && is_fight_delay(ch,90)){
	  for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)
	    if (IS_NPC(mob) && IS_SET(mob->off_flags,GUILD_GUARD))
	      {
		mprog_bloody_trigger(mob,ch);
		return;
	      }
	}
	else if (fbloody && ch->pCabal && IS_CABAL(ch->pCabal, CABAL_JUSTICE)){
	  for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)
	    if (IS_NPC(mob) && IS_SET(mob->off_flags,GUILD_GUARD))
	      {
		mprog_justice_trigger(mob,ch);
		break;
	      }
	}
      }
      
      if (!IS_NPC(ch))
	climb =  get_skill(ch,gsn_climb);
      else
	climb = 0;

      if ( in_room->sector_type == SECT_AIR || to_room->sector_type == SECT_AIR )
	{
	  if ( !(IS_AFFECTED(ch, AFF_FLYING) && !is_affected(ch,gsn_thrash)) && !IS_IMMORTAL(ch) && !is_ghost(ch,600)){
	    if ( climb > 0)
	      {
		send_to_char("You attempt to climb in that direction.\n\r", ch);
		if (number_percent() > 2 * climb / 3)
		  {
		    send_to_char("You failed to find a path.\n\r", ch);
		    WAIT_STATE2(ch,skill_table[gsn_climb].beats);
		    check_improve(ch, gsn_climb, FALSE, 1);
		    return;
		  }
		if (ch->mana < 4){
		  send_to_char("You lack the concentration required.\n\r", ch);
		  return;
		}
		WAIT_STATE2(ch,skill_table[gsn_climb].beats);
		ch->mana -= 4;
		check_improve(ch, gsn_climb, TRUE, 1);		
	      }
	    else if (is_affected(ch, gsn_sretrib)){
	      send_to_char("You attempt to climb in that direction.\n\r", ch);
	      if (ch->mana < 1){
		send_to_char("You lack the concentration required.\n\r", ch);
		return;
	      }
	      ch->mana -= 1;
	      }
	      else if (is_song_affected(ch,gsn_condor))
		{
		  condor = TRUE;
		  song_strip(ch,gsn_condor);
		  sendf(ch,"A large condor swoops in and carries you %s.\n\r",dir_name[door]);
		}
	      else
		{
		  send_to_char( "You can't fly.\n\r", ch );
		  return;
		}
	    }
	}
      if (( in_room->sector_type == SECT_WATER_NOSWIM || to_room->sector_type == SECT_WATER_NOSWIM )
	  && !(IS_AFFECTED(ch,AFF_FLYING) && !is_affected(ch,gsn_thrash)) && !is_ghost(ch,600)
	  && !IS_SET(ch->form, FORM_AMPHIBIAN) && !IS_SET(ch->form, FORM_FISH))
	{
	  OBJ_DATA *obj;
	  bool found;
	  found = FALSE;
	  if (is_song_affected(ch,gsn_condor))
	    {
	      found = TRUE;
	      condor = TRUE;
	      song_strip(ch,gsn_condor);
	      sendf(ch,"A large condor swoops in and carries you %s.\n\r",dir_name[door]);
	    }
	  else if (IS_IMMORTAL(ch))
	    found = TRUE;
	  else if (is_affected(ch, gsn_sretrib)){
	    send_to_char("You attempt to swim in that direction.\n\r", ch);
	    if (ch->mana < 1){
	      send_to_char("You lack the concentration required.\n\r", ch);
	      return;
	    }
	    ch->mana -= 1;
	  }
	  else
	    {
	      for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
		{
		  if ( obj->item_type == ITEM_BOAT ) 
		    {
		      found = TRUE;
		      break;
		    }
		}
	    }
	  if ( !found && ch->race != race_lookup("storm"))
	    {
	      send_to_char( "You need a boat to go there.\n\r", ch );
	      return;
	    }
	}
      if ( is_affected(ch, gsn_cognizance) || is_affected(ch, gsn_sretrib))
    	{
	  for ( i = 0; i <= 5; i++ )
	    if (to_room->exit[i] != NULL)
	      found = TRUE;
	  if ( !found )
	    {
	      send_to_char("You sense danger in that direction.\n\r",ch);
	      return;
	    }
        }
      if (is_affected(ch,gen_ensnare) && in_room->area != to_room->area)
	{
	  send_to_char("As you approach a host of guards come into view.  To proceed would be certain death!\n\r",ch);
	  return;
	}
      else if (is_affected(ch,gsn_tarot) && tarot_find(ch->affected) == 6 && in_room->area != to_room->area)
	{
	  send_to_char("You have been restricted to this area by your fate!\n\r",ch);
	  return;
	}
      
      move = (movement_loss[UMIN(MAX_SECT-1, in_room->sector_type)] + movement_loss[UMIN(MAX_SECT-1, to_room->sector_type)])/2;
      
      /* check for forest walk */
      if (ch->class == gcn_ranger && to_room->sector_type == SECT_FOREST){
	if ( number_percent() < get_skill(ch, gsn_forest_walk)){
	  check_improve(ch, gsn_forest_walk, TRUE, 1);
	  move = move / 2;
	}
      }

      /* if they can climb, max cost of 4, and 3 if it was 4 */
      if (move >  3 && number_percent() < climb){
	if (move == 4) 
	  move = 3;
	else
	  move = 4;
	check_improve(ch, gsn_climb, TRUE, 1);
      }
      if (is_affected(ch, gsn_sretrib)){
	if (move > 2) 
	  move = 2;
      }

      if (is_affected(ch, gsn_caltraps)){
	fEnfeeble = TRUE;
	move = 2 * move;
      }
      else if (is_affected(ch, gsn_enfeeblement )){
	fEnfeeble = TRUE;
	move *= 2;
      }
      if (IS_AFFECTED(ch, AFF_PLAGUE))
	fEnfeeble = TRUE;

      if (IS_AFFECTED(ch,AFF_SLOW))
	move *= 2;
      if (is_affected(ch, gsn_sneak))
	move *= 2;

/* fly/haste */
      if (IS_AFFECTED(ch,AFF_HASTE))
	move /= 2;
      else if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_FLYING) 
	       && !is_affected(ch,gsn_thrash) ){
	move /= 2;
      }
      /* if they ghost max move cost of 3 */
      if (move > 3 && is_ghost(ch, 600))
	move = 3;

      /* CABAL */   
      if (move > 2 && ch->in_room && ch->in_room->area->pCabal){
	/* warmaster lowers movement cost by 1 */
	if (!str_cmp(ch->in_room->area->pCabal->name, "Warmaster") 
	    && !class_table[ch->class].fMana 
	    && !class_table[ch->class].sMana)
	  move -= 1;
      }

      if (!IS_NPC(ch) && !IS_AFFECTED(ch,AFF_FLYING) 
	  && ch->pcdata->pStallion != NULL){
	if ( IS_SET(ch->pcdata->pStallion->affected_by, AFF_FLYING)
	     || (ch->pet && ch->pet->pIndexData == ch->pcdata->pStallion
		 && IS_AFFECTED(ch->pet, AFF_FLYING))
	     ){
	  move = UMAX(1, move / 2);
	}
	else
	  move = UMAX(movement_loss[SECT_INSIDE], move - 2);
      }
      if (!fEnfeeble){
	if ( move > 2 && is_affected(ch,gen_crusade))
	  move = 2;
	if ( move > 1 && is_affected(ch,gsn_heroism) )
	  move = 1;
	else if ( move > 1 && is_affected(ch,gen_avenger) )
	  move = 1;
      }
      if (condor || IS_NPC(ch))
	move = 1;

      //INSTEAD OF MOVE LAG, MIST OFRM TAKES MORE MOVS
      if (is_affected(ch,gsn_mist_form))
	move *= 2;

      //MOVEMEN LAG (WORST TO LEAST LAG PENALTY):	   
      if (IS_AFFECTED2(ch, AFF_HOLD)){
	send_to_char("You can barely move.\n\r",ch);
	lag *=6;
	move *=3;
      }
      else if (is_affected(ch, gsn_ghoul_touch)){
	send_to_char("You feel sluggish.\n\r",ch);
	lag *=3;
	move *=3;
      }
      else if (is_affected(ch, gsn_paralyze)){
	lag *=3;
	move *=3;
      }
      else if (is_affected(ch, gsn_webbing)){
	lag *= 2;
	move *= 2;
      }

      if ( ch->move < move && !IS_IMMORTAL(ch)){
	send_to_char( "You are too exhausted.\n\r", ch );
	return;
      }
      else if (!IS_IMMORTAL(ch) ){
	ch->move -= UMAX(1, move);
      }
      WAIT_STATE( ch, lag );
      
      //ADDITIONAL MOVEMENT LAGS DUE TO EFFECTS 
      if (is_affected(ch,gsn_trap_silvanus))
        {
	  send_to_char("Your trap falls apart!\n\r",ch);
	  WAIT_STATE(ch,24);
	  affect_strip(ch,gsn_trap_silvanus);
        } 
      if (is_affected(ch,gsn_deadfall))
        {
          send_to_char("Your deadfall loses its potency.\n\r",ch);
          WAIT_STATE(ch,24);
          affect_strip(ch,gsn_deadfall);
        }
      if (is_affected(ch,gsn_kinetics))
	{
	  move *= 2;
	  ch->mana += URANGE(0, move/3, ch->max_mana - ch->mana);
	}
  }
  if (!IS_NPC(ch) && is_affected(ch,gsn_pixie_dust) && number_percent() < 50)
    {
      OBJ_DATA *obj, *gobj;
      char obuf[MSL];
      send_to_char("You shed off some pixie dust.\n\r",ch);
      act("A bit of pixie dust sheds off $n.",ch,NULL,NULL,TO_ROOM);
      obj = create_object( get_obj_index( OBJ_VNUM_PIXIE_DUST ), ch->level);
      free_string(obj->description);
      sprintf(obuf,"A trail of pixie dust leading %s.", dir_name[door]);
      obj->description = str_dup( obuf );
      obj->timer = 2;
      for ( gobj = ch->in_room->contents; gobj != NULL; gobj = gobj->next_content )
        {
	  if (gobj->pIndexData->vnum == OBJ_VNUM_PIXIE_DUST)
	    {
	      obj_from_room( gobj );
	      extract_obj(gobj);
	    }
	}
      obj_to_room(obj, ch->in_room);
    }

  if ( IS_AFFECTED(ch, AFF_HIDE) 
       && to_room->sector_type != SECT_CITY
       && to_room->sector_type != SECT_INSIDE 
       && to_room->sector_type != SECT_FIELD)
    {
      affect_strip( ch, gsn_hide );
      REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
      act("You step out of the shadows.",ch,NULL,NULL,TO_CHAR);
      act("$n steps out of the shadows.",ch,NULL,NULL,TO_ROOM);
    }
  if ( IS_AFFECTED2(ch, AFF_CAMOUFLAGE) 
       && to_room->sector_type != SECT_FOREST 
       && to_room->sector_type != SECT_HILLS 
       && to_room->sector_type != SECT_SNOW 
       && to_room->sector_type != SECT_FIELD
       && to_room->sector_type != SECT_MOUNTAIN)
    {
      affect_strip( ch, gsn_camouflage );
      REMOVE_BIT   ( ch->affected2_by, AFF_CAMOUFLAGE		);
      act("You step out from your cover.",ch,NULL,NULL,TO_CHAR);
      act("$n steps out from $s cover.",ch,NULL,NULL,TO_ROOM);
    }
/* stallions do not make for sneaking */
  if ( !IS_NPC(ch) && ch->pcdata->pStallion != NULL && ch->invis_level <= LEVEL_HERO){
    bool fFly = FALSE;
    if (IS_AFFECTED(ch, AFF_HIDE)){
      affect_strip( ch, gsn_hide );
      REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
      act("You step out of the shadows.",ch,NULL,NULL,TO_CHAR);
      act("$n steps out of the shadows.",ch,NULL,NULL,TO_ROOM);
    }
    if (IS_AFFECTED2(ch,AFF_CAMOUFLAGE) ){
      if (chance > cskill ){
	affect_strip(ch,gsn_camouflage);
	REMOVE_BIT (ch->affected2_by, AFF_CAMOUFLAGE);
	act("You step out from your cover.",ch,NULL,NULL,TO_CHAR);
	act("$n steps out from $s cover.",ch,NULL,NULL,TO_ROOM);
	if (get_skill(ch,gsn_quiet_movement) != 0)
	  check_improve(ch,gsn_quiet_movement,FALSE,0);
      }
      else if (get_skill(ch,gsn_quiet_movement) > 1)
	check_improve(ch,gsn_quiet_movement,TRUE,0);
    }
    /* check if the stallion flies in */
    if (IS_SET(ch->pcdata->pStallion->affected_by, AFF_FLYING)
	|| (ch->pet && ch->pet->pIndexData == ch->pcdata->pStallion && IS_AFFECTED(ch->pet, AFF_FLYING)) )
      fFly = TRUE;
    act_new( "$n $t $T.", ch, fFly ? "flies" : "rides", dir_name[door], TO_ROOM, POS_RESTING + 100 );
  }
  else if (!(!IS_NPC(ch) && IS_SET(ch->act2,PLR_NOWHO)) && ch->invis_level <= LEVEL_HERO)
    {
      if (!IS_AFFECTED(ch, AFF_SNEAK) 
	   && IS_AFFECTED(ch, AFF_HIDE)
	  && !sneak_fade_check(ch))
    	{
	  affect_strip( ch, gsn_hide );
	  REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
	  act("You step out of the shadows.",ch,NULL,NULL,TO_CHAR);
	  act("$n steps out of the shadows.",ch,NULL,NULL,TO_ROOM);
    	}
      if (IS_AFFECTED2(ch,AFF_CAMOUFLAGE) )
	{
	  if (cskill < 2 || (chance > cskill && sneak_fade_check(ch)) )
	    {
	      affect_strip(ch,gsn_camouflage);
	      REMOVE_BIT (ch->affected2_by, AFF_CAMOUFLAGE);
	      act("You step out from your cover.",ch,NULL,NULL,TO_CHAR);
	      act("$n steps out from $s cover.",ch,NULL,NULL,TO_ROOM);
	      if (get_skill(ch,gsn_quiet_movement) != 0)
		check_improve(ch,gsn_quiet_movement,FALSE,0);
	    }
	  else if (get_skill(ch,gsn_quiet_movement) != 0)
	    check_improve(ch,gsn_quiet_movement,TRUE,0);
	}
    
      
      //This bit here checks for atuo sneak for rangers.
      if ( (to_room->sector_type != SECT_FOREST 
	    && to_room->sector_type != SECT_HILLS 
	    && to_room->sector_type != SECT_SNOW
	    && to_room->sector_type != SECT_MOUNTAIN ) 
	   || (chance > cskill 
	       && !is_affected(ch,gsn_wolf_form)) 
	   || IS_AFFECTED(ch,AFF_FAERIE_FIRE) 
	   || IS_AFFECTED(ch,AFF_FAERIE_FOG) )
	{
	  //If character is not a ranger sneking, we are going to test for sneaking vamp 
	  if(!IS_AFFECTED(ch, AFF_SNEAK) 
	     && !vamp_can_sneak(ch, to_room) 
	     && !sneak_fade_check(ch))
	    {
	      //if the vamp cant sneak in here, then we show him walking out.
	      if (condor)
		act_new( "$n is carried $T by a large condor.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_VAPOR)
		act_new( "$n floats $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_SOLID)
		act_new( "$n flows $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else if (IS_AFFECTED(ch, AFF_FLYING) && !is_affected(ch,gsn_thrash))
		act_new( "$n flies $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else if (is_affected(ch,gsn_mist_form))
		act_new( "$n floats $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else if ( in_room->sector_type == SECT_WATER_SWIM )
		act_new( "$n swims $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else if ( in_room->sector_type == SECT_WATER_NOSWIM )
		act_new( "$n floats $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else if (ch->max_hit != 0 && ch->hit*100/ch->max_hit <= 5)
		act_new( "$n crawls $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else if ( in_room->sector_type == SECT_MOUNTAIN || in_room->sector_type == SECT_AIR )
		act_new( "$n climbs $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else if (ch->max_hit != 0 && ch->hit*100/ch->max_hit <= 20)
	        act_new( "$n limps $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else if (is_affected(ch,gsn_caltraps))
		act_new( "$n limps $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else if (is_affected(ch,gsn_entangle))
		act_new( "$n trails vines $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	      else
		act_new( "$n walks $T.", ch, NULL, dir_name[door], TO_ROOM, POS_RESTING + 100 );
	    }//END IF(!vamp_can_sneak
	}//END IF( sector_>..(the ranger check)
      //do_out simply sendsout message to those that can sense sneak
      do_out(ch, door);
    }//END if NOT AFFECTED BY SNEAK
  else
    //do_out simply sendsout message to those that can sense sneak 
    do_out(ch, door);


  //change the actual position of the character
  char_from_room( ch );
/* check if any updates killed the char etc. */
  if (ch == NULL || ch->extracted)
    return;
/* ripple effect */
  if ( !IS_NPC(ch) 
       && (pRipple = affect_find(ch->affected, gsn_ripple)) != NULL
       && number_percent() < 50){
    to_room = in_room;
    --pRipple->modifier;
  }

  char_to_room( ch, to_room );
/* strip ripple if run down */
  if (pRipple && pRipple->modifier < 0){
    act(skill_table[gsn_ripple].msg_off, ch, NULL, NULL, TO_CHAR);
    act(skill_table[gsn_ripple].msg_off2, ch, NULL, NULL, TO_ROOM);
    affect_strip(ch, gsn_ripple);
  }
  
/* check if any updates killed the char etc. */
  if (ch->extracted || ch->in_room == NULL)
    return;
  if ( !IS_NPC(ch) && ch->pcdata->pStallion != NULL && ch->invis_level <= LEVEL_HERO){
    bool fFly =  FALSE;
    if (IS_SET(ch->pcdata->pStallion->affected_by, AFF_FLYING)
	|| (ch->pet && ch->pet->pIndexData == ch->pcdata->pStallion && IS_AFFECTED(ch->pet, AFF_FLYING)) )
      fFly = TRUE;
    act_new( "$n $t in.", ch, fFly ? "flies" : "rides", NULL, TO_ROOM, POS_RESTING + 100 );
  }
  else
    { 
      if ( !IS_AFFECTED(ch, AFF_SNEAK) 
	   && !(!IS_NPC(ch) 
		&& IS_SET(ch->act2,PLR_NOWHO)) 
	   && ch->invis_level <= LEVEL_HERO 
	   && ((to_room->sector_type != SECT_FOREST 
		&& to_room->sector_type != SECT_HILLS 
		&& to_room->sector_type != SECT_SNOW 
		&& to_room->sector_type != SECT_MOUNTAIN )  
	       || chance > cskill 
	       || (!IS_SET(race_table[ch->race].aff,AFF_SNEAK)
		   && (IS_AFFECTED(ch,AFF_FAERIE_FIRE) 
		       || IS_AFFECTED(ch,AFF_FAERIE_FOG)) ) ))
	{
	  //Since he is not ranger, or cannot sneak in forest, we check for vampire sneak
	  if(!vamp_can_sneak(ch, to_room) && !sneak_fade_check(ch))
	    {
	      if (condor)
		act_new( "$n is carried in by a large condor.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_VAPOR)
		act_new( "$n floats in.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_SOLID)
		act_new( "$n flows in.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else if (IS_AFFECTED(ch, AFF_FLYING) && !is_affected(ch,gsn_thrash))
		act_new( "$n flies in.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else if (is_affected(ch,gsn_mist_form))
		act_new( "$n floats in.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else if ( to_room->sector_type == SECT_WATER_SWIM )
		act_new( "$n swims in.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else if ( to_room->sector_type == SECT_WATER_NOSWIM )
		act_new( "$n floats in.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else if (ch->max_hit != 0 && ch->hit*100/ch->max_hit <= 5)
		act_new( "$n crawls in.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else if ( to_room->sector_type == SECT_MOUNTAIN || to_room->sector_type == SECT_AIR )
		act_new( "$n climbs in.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else if (ch->max_hit != 0 && ch->hit*100/ch->max_hit <= 20)
		act_new( "$n limps in.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else if (is_affected(ch,gsn_caltraps))
		act_new( "$n limps in.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else if (is_affected(ch,gsn_entangle))
		act_new( "$n limps in, trailing vines.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	      else
		act_new( "$n walks in.", ch, NULL, NULL, TO_ROOM, POS_RESTING + 100 );
	    }//END If(!vamp_Can_sneak
	  do_in(ch);
	}//END Ranger check
      else
	//do_in makes sure characrters who can, see sneakers.
	do_in(ch);
    }
  /* check for anger after entering */
  if (is_affected(ch, gen_anger)){
    run_effect_update(ch, NULL, to_room, gen_anger, NULL, NULL, TRUE, EFF_UPDATE_INIT);
    if (ch == NULL || ch->in_room == NULL)
      return;
  }
  do_look( ch, "auto" );

  /* tracks */
  if (!IS_NPC(ch) 
      && to_room->sector_type != SECT_WATER_SWIM
      && to_room->sector_type != SECT_WATER_NOSWIM
      && to_room->sector_type != SECT_AIR){
    to_room->tracks[anatomy_lookup(pc_race_table[ch->race].anatomy)] = mud_data.current_time;
  }

  if (condor)
    act("A large condor soars away.",ch,NULL,NULL,TO_ALL);
  if (in_room == to_room)
    return;
  if (!IS_NPC(ch) && vamp_check(ch) && !is_ghost(ch,600)
      && (ch->tattoo != deity_lookup("cycle") || mud_data.time_info.hour == 12))
    {
      send_to_char("You skin burns from exposure to the sunlight.\n\r",ch);
      ch->move -= ch->level/10;
      if (!is_affected(ch,gsn_drained))
	{
	  AFFECT_DATA af;
	  send_to_char("You feel weak...\n\r",ch);
	  af.where            = TO_AFFECTS;
	  af.type             = gsn_drained;
	  af.level            = ch->level;
	  af.duration         = -1;
	  af.location         = 0;
	  af.modifier         = 0;
	  af.bitvector        = 0;
	  affect_to_char(ch,&af);   
	}
      damage(ch,ch,ch->level/10,1052,DAM_LIGHT,TRUE);
    }
  for ( fch = in_room->people; fch != NULL; fch = fch_next )
    {
      fch_next = fch->next_in_room;
      if (fch->in_room == to_room)
	continue;
      if ( fch->master == ch && IS_AFFECTED(fch,AFF_CHARM) && fch->position < POS_STANDING)
	do_stand(fch,"");
      if (IS_SET(fch->special,SPECIAL_SENTINEL) || IS_SET(fch->special,SPECIAL_COWARD))
	continue;
      if (is_affected(fch,gsn_linked_mind))
	continue;
      if ( fch->master == ch 
	   && fch != ch 
	   && fch->position == POS_STANDING 
	   && can_see_room(fch,to_room))
	{
	  sendf(fch, "You follow %s.\n\r", PERS(ch,fch));
	  move_char( fch, door, TRUE );
	}
      else if ( is_song_affected(fch,gsn_lust) && !IS_AFFECTED(fch,AFF_CHARM)
		&& ((fch->sex == 1 && ch->sex == 2) || (fch->sex == 1 && ch->sex == 2))
		&& number_percent() < 10 && can_see_room(fch,to_room))
	{
	  if (fch->position < POS_STANDING)
	    do_stand(fch,"");
	  sendf(fch, "You scurry after %s lustfully.\n\r", PERS(ch,fch));
	  move_char( fch, door, TRUE );
	  act("$N stares at you with a lustful grin.",ch,NULL,fch,TO_CHAR);
	  act("You stare at $n lustfully.",ch,NULL,fch,TO_VICT);
	  act("$N stares at $n with a lustful grin.",ch,NULL,fch,TO_NOTVICT);
	}
    }
  
  mprog_entry_trigger( ch );
  if (ch == NULL || ch->in_room == NULL || ch->in_room != to_room )
    return;
  mprog_greet_trigger( ch );
  if (ch == NULL || ch->in_room == NULL || ch->in_room != to_room )
    return;

  if ( (!IS_NPC( ch ) || ch->trust == 6969) && ch->in_room){
    p_greet_trigger( ch, PRG_MPROG );
    if (ch == NULL || ch->extracted || ch->in_room == NULL || ch->in_room != to_room )
      return;
    p_greet_trigger( ch, PRG_OPROG );
    if (ch == NULL || ch->extracted || ch->in_room == NULL || ch->in_room != to_room )
      return;
    p_greet_trigger( ch, PRG_RPROG );
    if (ch == NULL || ch->extracted || ch->in_room == NULL || ch->in_room != to_room )
      return;
  }
  if (ch == NULL || ch->extracted || ch->in_room == NULL || ch->in_room != to_room )
    return;

  if ( IS_NPC( ch ) && ch->in_room && HAS_TRIGGER_MOB( ch, TRIG_ENTRY )){
    p_percent_trigger(ch, NULL, NULL, ch, NULL, NULL, TRIG_ENTRY);
  }
  if (ch == NULL || ch->in_room == NULL || ch->in_room != to_room )
    return;

  /* quest check */
  QuestRoomCheck( ch, ch->in_room );

  /* CABAL GUARD YELLS */
  if (!IS_IMMORTAL(ch) && !IS_NPC(ch) 
      && ch->in_room->pCabal 
      && ch->in_room->pCabal->anchor
      && ch->in_room->pCabal->anchor == ch->in_room
      && ch->in_room->pCabal->guard
      && IS_CABAL(ch->in_room->pCabal, CABAL_GUARDMOB)){
    for ( fch = ch->in_room->people; fch != NULL; fch = fch_next ){
      fch_next = fch->next_in_room;
      if (IS_NPC(fch) && IS_AWAKE(fch) 
	  && !IS_AFFECTED(fch,AFF_CHARM) 
	  && !IS_AFFECTED2(fch,AFF_HOLD)
	  && IS_SET(fch->off_flags,CABAL_GUARD) 
	  && fch->pCabal
	  && fch->pCabal->guard
	  && fch->pCabal->guard == fch->pIndexData
	  && ch->pCabal
	  && !is_captured_by(ch->pCabal, fch->pCabal)	  
	  && is_friendly( fch->pCabal, ch->pCabal ) == CABAL_ENEMY){
	sprintf(buf, "%s: Alert! %s%s%s`$%s sighted at our gates!``",
		PERS2( fch), 
		PERS(ch,fch),
		ch->pCabal ? " of [" : "",
		ch->pCabal ? ch->pCabal->who_name : "",
		ch->pCabal ? "]" : "");

	cabal_echo( fch->pCabal, buf );
	multi_hit( fch, ch, TYPE_UNDEFINED );
	break;
      }
    }
  }
  if (!IS_NPC(ch)){
    for (wch = to_room->people;wch != NULL;wch = wch->next_in_room){
      CHAR_DATA *victim;
      victim     = ch;
      if (ch == wch)
	continue;
      if ( !IS_NPC(wch) 
	   && wch->fighting == NULL 
	   && IS_AWAKE(wch) 
	   && !IS_AFFECTED(wch,AFF_CALM)
	   && victim->position > POS_SITTING
	   && !is_area_safe_quiet(wch,victim) 
	   && is_affected(wch,gsn_deadfall) )
	
	{
	  AFFECT_DATA naf;
	  if (number_percent() < get_skill(wch,gsn_deadfall)){
	    if (!IS_AFFECTED(victim,AFF_FLYING) 
		|| is_affected(victim,gsn_thrash)
		|| number_percent() < 20){
	      sprintf(buf,"Help! I'm caught in %s's deadfall!",PERS(wch,victim));
	      j_yell(victim,buf);          
	      act("You `1BRUTALIZE`` $N in your deadfall!",wch,NULL,victim,TO_CHAR);
	      act("$n `1BRUTALIZES`` $N in $S deadfall!",wch,NULL,victim,TO_NOTVICT);
	      act("You are `1BRUTALIZED`` by $n's deadfall!",wch,NULL,victim,TO_VICT);
	      if (!IS_AFFECTED(victim,AFF_FLYING) 
		  || is_affected(victim,gsn_thrash)
		  ){
		act("You can't move an inch!",wch,NULL,victim,TO_VICT);
		damage(ch,ch,40,gsn_deadfall,DAM_BASH,TRUE);
		victim->position = POS_SLEEPING;
		naf.where     = TO_AFFECTS;
		naf.type      = gsn_deadfall;
		naf.level     = wch->level;
		naf.duration  = number_range(0, 3);
		naf.location  = APPLY_NONE;
		naf.modifier  = 0;
		naf.bitvector = AFF_SLEEP;
		affect_to_char( victim, &naf );
		if ( IS_AWAKE(victim) )
		  {
		    send_to_char( "You are unconscious!\n\r", victim );
		  }
	      }
	      else{
		act("You manage to avoid $N's deadfall but it stuns you.", victim, NULL, wch, TO_CHAR);
		act("$n avoids your deadfall but it stuns $m.", victim, NULL, wch, TO_ROOM);
		victim->position = POS_RESTING;
		set_delay(wch,victim);
		WAIT_STATE2(victim,24);
	      }
	    }
	    else
	      {
		act("Your deadfall releases!",wch,NULL,NULL,TO_CHAR);
		act("$n's deadfall releases!",wch,NULL,NULL,TO_ROOM);
		WAIT_STATE(wch,24);
	      }
	  }
	}
      
      if ( !IS_NPC(wch) && is_affected(wch,gsn_trap_silvanus) && wch->fighting == NULL && IS_AWAKE(wch) &&!IS_AFFECTED(wch,AFF_CALM)
	   && !is_area_safe_quiet(wch,victim) && !(IS_AFFECTED(victim,AFF_FLYING) && !IS_AFFECTED(victim,gsn_thrash))
	   && victim->position > POS_SITTING)
	{
	  AFFECT_DATA naf;
	  if (number_percent() < get_skill(wch,gsn_trap_silvanus))
	    {
	      sprintf(buf,"Help! I've fallen into %s's trap!",PERS(wch,victim));
	      j_yell(victim,buf);
	      if (number_percent() < get_skill(victim, gsn_balance)/2 && is_empowered_quiet(victim,1) && victim->mana > 0)
		{
		  act("$N steps on your trap but $E keeps $S balance!",wch,NULL,victim,TO_CHAR);
		  act("$N steps on $n's trap but keeps $S balance!",wch,NULL,victim,TO_NOTVICT);
		  act("$n ensnares you in $s trap but you manage to stay balanced!",wch,NULL,victim,TO_VICT);
		  check_improve(victim,gsn_balance,TRUE,5);
		  victim->mana -= URANGE(0,25,victim->mana);
		}        
	      else
		{
		  act("Your trap sends $N to the ground!",wch,NULL,victim,TO_CHAR);
		  act("$n's trap sends $N to the ground!",wch,NULL,victim,TO_NOTVICT);
		  act("$n's trap sends you to the ground!",wch,NULL,victim,TO_VICT);
		  victim->position = POS_RESTING;
		  set_delay(wch,victim);
		  WAIT_STATE2(victim,24);
		}
	    }
	  else
	    {
	      act("Your trap comes apart in your face!",wch,NULL,NULL,TO_CHAR);
	      act("$n's trap comes apart in $s face!",wch,NULL,NULL,TO_ROOM);
	      wch->position = POS_RESTING;
	      WAIT_STATE(wch,24);
	    }               
	  affect_strip(wch,gsn_trap_silvanus);
	  naf.where            = TO_AFFECTS;
	  naf.type             = gsn_wire_delay;
	  naf.level            = wch->level;
	  naf.duration         = 15 - (wch->level/10);
	  naf.location         = 0;
	  naf.modifier         = 0;
	  naf.bitvector        = 0;
	  affect_to_char(wch,&naf);
	  break;
	} 
    }
  }
  if (ch->in_room && IS_SET(ch->in_room->room_flags2, ROOM_PSI_FEAR))
    effect_field_of_fear(ch);

/* after all that, assuming we are alive we check if we should draw weapons */
  if (IS_GAME(ch, GAME_ASHEATH) 
      && IS_SET(in_room->area->area_flags, AREA_LAWFUL) 
      && !IS_SET(to_room->area->area_flags, AREA_LAWFUL)){
    do_combatdraw(ch, get_eq_char(ch, WEAR_WIELD), get_eq_char(ch, WEAR_SECONDARY));
  }

/* now check for opposite, sheathing */
  else if (IS_GAME(ch, GAME_ASHEATH) 
	   && IS_SET(to_room->area->area_flags, AREA_LAWFUL) 
	   && !IS_SET(in_room->area->area_flags, AREA_LAWFUL)
	   && get_eq_char(ch, WEAR_SHEATH_L) == NULL
	   && get_eq_char(ch, WEAR_SHEATH_R) == NULL){
    do_sheath(ch, "");
  }
}

void do_north( CHAR_DATA *ch, char *argument )
{
move_char( ch, DIR_NORTH, FALSE );
}

void do_east( CHAR_DATA *ch, char *argument )
{
  move_char( ch, DIR_EAST, FALSE );
}

void do_south( CHAR_DATA *ch, char *argument )
{
  move_char( ch, DIR_SOUTH, FALSE );
}

void do_west( CHAR_DATA *ch, char *argument )
{
  move_char( ch, DIR_WEST, FALSE );
}

void do_up( CHAR_DATA *ch, char *argument )
{
  move_char( ch, DIR_UP, FALSE );
}

void do_down( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_DOWN, FALSE );
}

int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int door;
    if      ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else
    {
	for ( door = 0; door <= 5; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] ) != NULL && IS_SET(pexit->exit_info, EX_ISDOOR)
            && pexit->keyword != NULL && is_name( arg, pexit->keyword ) )
		return door;
	}
	sendf(ch, "I see no %s here.\n\r", arg);
	return -1;
    }
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	sendf(ch, "I see no door %s here.\n\r", arg);
	return -1;
    }
    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
	sendf(ch, "I see no door %s here.\n\r", arg);
	return -1;
    }
    return door;
}


int find_door_quiet( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int door;
    if      ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else
    {
	for ( door = 0; door <= 5; door++ )
	{
	    if ( ( pexit = ch->in_room->exit[door] ) != NULL && IS_SET(pexit->exit_info, EX_ISDOOR)
            && pexit->keyword != NULL && is_name( arg, pexit->keyword ) )
		return door;
	}
	return -1;
    }
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
	return -1;
    }
    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
	return -1;
    }
    return door;
}

void do_open( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    OBJ_DATA *obj;
    int door;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Open what?\n\r", ch );
	return;
    }
    if ( ( obj = get_obj_here( ch, NULL,  arg ) ) != NULL 
	 && ( door = find_door_quiet( ch, arg ) ) < 0 )
    {
	if ( obj->item_type != ITEM_CONTAINER)
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }
	REMOVE_BIT(obj->value[1], CONT_CLOSED);
	act("You open $p.",ch,obj,NULL,TO_CHAR);
        act("$n opens $p.", ch, obj, NULL, TO_ROOM );
	/* TRAP check */
	trip_traps(ch, obj->traps);
	return;
    }
    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit = ch->in_room->exit[door], *pexit_rev;
	CHAR_DATA *ich = NULL;
	bool ecto = FALSE;
	if (ch->class == class_lookup("psionicist")){
	  for (ich = ch->in_room->people; ich; ich = ich->next_in_room){
	    if (IS_NPC(ich) 
		&& ich->pIndexData->vnum == MOB_VNUM_VAPOR 
		&& ich->leader == ch){
	      ecto = TRUE;
	      break;
	    }
	  }
	}
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already open.\n\r",      ch ); return; }
	if (  IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's locked.\n\r",            ch ); return; }
	REMOVE_BIT(pexit->exit_info, EX_CLOSED);
	sprintf(buf,"$n opens the $d %s.",dir_name2[door]);
	WAIT_STATE(ch,6);
	if (ecto)
	    act( buf, ich, NULL, pexit->keyword, TO_ROOM );
	else
	{
	    act( buf, ch, NULL, pexit->keyword, TO_ROOM );
	    sprintf(buf,"You open the $d %s.",dir_name2[door]);
	    act( buf, ch, NULL, pexit->keyword, TO_CHAR );
	}
	if ( ( to_room   = pexit->to_room ) != NULL  && ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
        && pexit_rev->to_room == ch->in_room )
	{

	    CHAR_DATA *rch;
	    REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	    sprintf(buf,"The $d %s opens.",dir_name2[rev_dir[door]]);
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( buf, rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
	/* TRAP check */
	trip_traps(ch, pexit->traps);
    }
}

void do_close( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    OBJ_DATA *obj;
    int door;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Close what?\n\r", ch );
	return;
    }
    if ( ( obj = get_obj_here( ch, NULL,  arg ) ) != NULL 
	 && ( door = find_door_quiet( ch, arg ) ) < 0 )
    {
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
	    { send_to_char( "You can't do that.\n\r",      ch ); return; }
	SET_BIT(obj->value[1], CONT_CLOSED);
	act("You close $p.",ch,obj,NULL,TO_CHAR);
        act("$n closes $p.", ch, obj, NULL, TO_ROOM );

	/* TRAP check */
	trip_traps(ch, obj->traps);
	return;
    }
    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit = ch->in_room->exit[door], *pexit_rev;
        CHAR_DATA *ich = NULL;
        bool ecto = FALSE;
        if (ch->class == class_lookup("psionicist"))
            for (ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room)
                if (IS_NPC(ich) && ich->pIndexData->vnum == MOB_VNUM_VAPOR && ich->leader == ch)
                {
                    ecto = TRUE;
                    break;
                }
	if ( IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's already closed.\n\r",    ch ); return; }
	else if (IS_SET(pexit->exit_info, EX_NOCLOSE)){
	  send_to_char("It seems stuck!\n\r", ch);
	  return;
	}
	SET_BIT(pexit->exit_info, EX_CLOSED);
	sprintf(buf,"$n closes the $d %s.",dir_name2[door]);
	WAIT_STATE(ch,12);
        if (ecto)
            act( buf, ich, NULL, pexit->keyword, TO_ROOM );
	else
	{
	    act( buf, ch, NULL, pexit->keyword, TO_ROOM );
	    sprintf(buf,"You close the $d %s.",dir_name2[door]);
	    act( buf, ch, NULL, pexit->keyword, TO_CHAR );
	}
	if ( ( to_room = pexit->to_room ) != NULL && ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
	&& pexit_rev->to_room == ch->in_room )
	{
	    CHAR_DATA *rch;
	    SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	    sprintf(buf,"The $d %s closes.",dir_name2[rev_dir[door]]);
	    for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
		act( buf, rch, NULL, pexit_rev->keyword, TO_CHAR );
	}
	/* TRAP check */
	trip_traps(ch, pexit->traps);
    }
}

OBJ_DATA* has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	if ( obj->pIndexData->vnum == key )
	    return obj;
    return NULL;
}

void do_lock( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj, *key;
    int door;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Lock what?\n\r", ch );
	return;
    }
    if ( ( obj = get_obj_here( ch, NULL,  arg ) ) != NULL 
	 && ( door = find_door_quiet( ch, arg ) ) < 0 )
    {
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( ( key = has_key( ch, obj->value[2] )) == NULL )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }
	SET_BIT(obj->value[1], CONT_LOCKED);
	act("You lock $p.",ch,obj,NULL,TO_CHAR);
        act("$n locks $p.", ch, obj, NULL, TO_ROOM );
	/* TRAP check */
	if (key)
	  trip_traps(ch, key->traps);
	return;
    }
    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit = ch->in_room->exit[door], *pexit_rev;
        CHAR_DATA *ich = NULL;
        bool ecto = FALSE;
        if (ch->class == class_lookup("psionicist"))
            for (ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room)
                if (IS_NPC(ich) && ich->pIndexData->vnum == MOB_VNUM_VAPOR && ich->leader == ch)
                {
                    ecto = TRUE;
                    break;
                }
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key <= 0 || IS_SET(pexit->exit_info, EX_NOLOCK))
	  { send_to_char( "It can't be locked.\n\r",     ch ); return; }
	if ( ( key = has_key( ch, pexit->key)) == NULL && !ecto)
	  { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already locked.\n\r",    ch ); return; }
	SET_BIT(pexit->exit_info, EX_LOCKED);
        if (ecto)
            act( "$n locks the $d.", ich, NULL, pexit->keyword, TO_ROOM );
	else
	    act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "*Click*\n\r", ch );
	/* TRAP check */
	if (key)
	  trip_traps(ch, key->traps);
	if ( ( to_room = pexit->to_room ) != NULL && ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
        && pexit_rev->to_room == ch->in_room )
	    SET_BIT( pexit_rev->exit_info, EX_LOCKED );
    }
}

void do_unlock( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj, *key;
    int door;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Unlock what?\n\r", ch );
	return;
    }
    if ( ( obj = get_obj_here( ch, NULL,  arg ) ) != NULL 
	 && ( door = find_door_quiet( ch, arg ) ) < 0 )
    {
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( ( key = has_key( ch, obj->value[2] )) == NULL )
	    { send_to_char( "You lack the key.\n\r",       ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	REMOVE_BIT(obj->value[1], CONT_LOCKED);
	act("You unlock $p.",ch,obj,NULL,TO_CHAR);
        act("$n unlocks $p.", ch, obj, NULL, TO_ROOM );
	/* TRAP check */
	if (key)
	  trip_traps(ch, key->traps);
	return;
    }
    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit = ch->in_room->exit[door], *pexit_rev;
        CHAR_DATA *ich = NULL;
        bool ecto = FALSE;
        if (ch->class == class_lookup("psionicist"))
            for (ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room)
                if (IS_NPC(ich) && ich->pIndexData->vnum == MOB_VNUM_VAPOR && ich->leader == ch)
                {
                    ecto = TRUE;
                    break;
                }
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key <= 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( ( key = has_key( ch, pexit->key)) == NULL){
	  if (!ecto 
	      || IS_SET(pexit->exit_info, EX_PICKPROOF)){
	    send_to_char( "You lack the key.\n\r",       ch ); return; }
	}
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
        if (ecto)
            act( "$n unlocks the $d.", ich, NULL, pexit->keyword, TO_ROOM );
	else
	    act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	send_to_char( "*Click*\n\r", ch );
	/* TRAP check */
	if (key)
	  trip_traps(ch, key->traps);
	if ( ( to_room   = pexit->to_room ) != NULL && ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
        && pexit_rev->to_room == ch->in_room )
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
    }
}

void do_pick( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *gch;
    OBJ_DATA *obj;
    int door,chance = 0;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
	{
            sendf(ch, "%s is standing too close to the lock.\n\r", PERS(gch,ch));
	    return;
	}
    WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );
    chance = get_skill(ch,gsn_pick_lock);
    if (chance > 0)
	chance += 2*(get_curr_stat(ch,STAT_LUCK) - 16) ;
    if ( ( obj = get_obj_here( ch, NULL,  arg ) ) != NULL 
	 && ( door = find_door_quiet( ch, arg ) ) < 0 )
    {
	if ( obj->item_type != ITEM_CONTAINER )
	    { send_to_char( "That's not a container.\n\r", ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_CLOSED) )
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( obj->value[2] < 0 )
	    { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
	if ( !IS_SET(obj->value[1], CONT_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
	    { send_to_char( "You failed.\n\r",             ch ); return; }
    	if ( !IS_NPC(ch) && chance < number_percent())
    	{
	    send_to_char( "You failed.\n\r", ch);
	    check_improve(ch,gsn_pick_lock,FALSE,2);
	    return;
    	}
	REMOVE_BIT(obj->value[1], CONT_LOCKED);
        act("You pick the lock on $p.",ch,obj,NULL,TO_CHAR);
        act("$n picks the lock on $p.",ch,obj,NULL,TO_ROOM);
	check_improve(ch,gsn_pick_lock,TRUE,2);
	return;
    }
    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit = ch->in_room->exit[door], *pexit_rev;
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL(ch))
	    { send_to_char( "It's not closed.\n\r",        ch ); return; }
	if ( pexit->key < 0 && !IS_IMMORTAL(ch))
	    { send_to_char( "It can't be picked.\n\r",     ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL(ch))
	    { send_to_char( "You failed.\n\r",             ch ); return; }
    	if ( !IS_SET(pexit->exit_info, EX_EASY) )
    	    chance = 3*chance/2;
    	if ( !IS_SET(pexit->exit_info, EX_HARD) )
    	    chance = 2*chance/3;
    	if ( !IS_SET(pexit->exit_info, EX_INFURIATING) )
    	    chance /= 2;    
    	if ( !IS_NPC(ch) && chance < number_percent())
    	{
	    send_to_char( "You failed.\n\r", ch);
	    check_improve(ch,gsn_pick_lock,FALSE,2);
	    return;
    	}
	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "*Click*\n\r", ch );
	act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
	check_improve(ch,gsn_pick_lock,TRUE,2);
	if ( ( to_room = pexit->to_room ) != NULL && ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
        && pexit_rev->to_room == ch->in_room )
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
    }
}

void do_stand( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    OBJ_DATA *obj = NULL;
    char buf[MIL];

    /* check for acting on objects */
    if (argument[0] != '\0'){
      if (!str_cmp("on", argument))
	argument = one_argument(argument, buf);
      if ((obj = get_obj_list( ch, argument, ch->in_room->contents )) == NULL){
	send_to_char("You do not see such object here.\n\r", ch);
	return;
      }
      sprintf(buf," on %s", obj->short_descr);
      if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )
	p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );
      if ( obj && HAS_TRIGGER_ROOM( ch->in_room, TRIG_SIT ) )
	p_percent_trigger( NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_SIT );
    }
    else
      sprintf(buf, " up");

    if (IS_AFFECTED2(ch, AFF_CATALEPSY))
    {
        send_to_char( "You're in a state of self-induced catalepsy.  You can't do that!\n\r", ch );
        send_to_char( "Type revive to revive yourself if you want.\n\r", ch );
        return;
    } 
    switch ( ch->position )
    {
    case POS_MEDITATE:
        act_new( "You stop meditating and stand$t.", ch, buf, NULL, TO_CHAR, POS_DEAD );
	if (!IS_AFFECTED(ch, AFF_HIDE))
	  act( "$n stops meditating and stands$t.", ch, buf, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	dreamprobe_cast(ch, NULL );
	break;
    case POS_SLEEPING:
	if ( IS_AFFECTED(ch, AFF_SLEEP))
        {
            send_to_char( "You can't wake up!\n\r", ch );
            return;
        }
	if ( is_affected(ch, gsn_camp))
	{
	    send_to_char( "You feel drained from deprivation of sleep.\n\r", ch);
	    if (is_affected(ch, gsn_drained))
    	        affect_strip(ch,gsn_drained);
	    af.where		= TO_AFFECTS;
	    af.type             = gsn_drained;
            af.level            = ch->level;
            af.duration         = 10;
            af.location         = APPLY_NONE;
            af.modifier         = 0;
            af.bitvector        = 0;
            affect_to_char(ch,&af);	    
	    affect_strip(ch,gsn_camp);
	}
        act_new( "You wake and stand$t.", ch, buf, NULL, TO_CHAR, POS_DEAD );
	if (!IS_AFFECTED(ch, AFF_HIDE))
	  act( "$n wakes and stands$t.", ch, buf, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	dreamprobe_cast(ch, NULL );
	break;
    case POS_RESTING: case POS_SITTING:
        act_new( "You stand$t.", ch, buf, NULL, TO_CHAR, POS_DEAD );
	if (!IS_AFFECTED(ch, AFF_HIDE))
	  act( "$n stands$t.", ch, buf, NULL, TO_ROOM );
	ch->position = POS_STANDING;
	break;
    case POS_STANDING:
	send_to_char( "You are already standing.\n\r", ch );
	break;
    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }
}

void do_rest( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    OBJ_DATA* obj = NULL;
    char buf[MIL];

    /* check for acting on objects */
    if (argument[0] != '\0'){
      if (!str_cmp("on", argument))
	argument = one_argument(argument, buf);
      if ((obj = get_obj_list( ch, argument, ch->in_room->contents )) == NULL){
	send_to_char("You do not see such object here.\n\r", ch);
	return;
      }
      sprintf(buf," on %s", obj->short_descr);
      if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )
	p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );
      if ( obj && HAS_TRIGGER_ROOM( ch->in_room, TRIG_SIT ) )
	p_percent_trigger( NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_SIT );
    }
    else
      buf[0] = '\0';

    if (IS_AFFECTED2(ch, AFF_CATALEPSY))
    {
        send_to_char( "You're already resting...as a corpse!\n\r", ch );
        send_to_char( "Type revive to revive yourself if you want.\n\r", ch );
        return;
    } 
    else if (is_affected(ch,gen_suffocate))
    {
	send_to_char("You cannot rest while suffocating!\n\r",ch);
	return;
    }

    check_dispel(-99, ch, gsn_stance );

    switch ( ch->position )
    {
    case POS_MEDITATE:
        act_new( "You stop meditating and rest$t.", ch, buf, NULL, TO_CHAR, POS_DEAD );
        act( "$n stops meditating and rests$t.", ch, buf, NULL, TO_ROOM );
	ch->position = POS_RESTING;
	dreamprobe_cast(ch, NULL );
	break;
    case POS_SLEEPING:
	if (IS_AFFECTED(ch,AFF_SLEEP))
	{
	    send_to_char("You can't wake up!\n\r",ch);
	    return;
	}
	if ( is_affected(ch, gsn_camp))
        {
            send_to_char( "You feel drained from deprivation of sleep.\n\r", ch);
            if (is_affected(ch, gsn_drained))    
                affect_strip(ch,gsn_drained);
            af.type             = gsn_drained;
            af.level            = ch->level;
            af.duration         = 10;
            af.location         = APPLY_NONE;
            af.modifier         = 0;
            af.bitvector        = 0;
            affect_to_char(ch,&af);
            affect_strip(ch,gsn_camp);
        }

	act_new("You wake up and start resting$t.", ch, buf, NULL, TO_CHAR, POS_DEAD);
        act("$n wakes up and starts resting$t.",ch,buf,NULL,TO_ROOM);
	ch->position = POS_RESTING;
	dreamprobe_cast(ch, NULL );
	break;
    case POS_RESTING:
	send_to_char( "You are already resting.\n\r", ch );
        break;
    case POS_STANDING:
        act_new( "You rest$t.", ch, buf, NULL, TO_CHAR, POS_DEAD );
        act( "$n sits down and rests$t.", ch, buf, NULL, TO_ROOM );
	ch->position = POS_RESTING;
	break;
    case POS_SITTING:
        act_new( "You rest$t.", ch, buf, NULL, TO_CHAR, POS_DEAD );
        act("$n rests$t.",ch, buf,NULL,TO_ROOM);
	ch->position = POS_RESTING;
	break;
    }
 
}

void do_sit (CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    OBJ_DATA *obj = NULL;
    char buf[MIL];

    /* check for acting on objects */
    if (argument[0] != '\0'){
      if (!str_cmp("on", argument))
	argument = one_argument(argument, buf);
      if ((obj = get_obj_list( ch, argument, ch->in_room->contents )) == NULL){
	send_to_char("You do not see such object here.\n\r", ch);
	return;
      }
      sprintf(buf," on %s", obj->short_descr);
      if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )
	p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );
      if ( obj && HAS_TRIGGER_ROOM( ch->in_room, TRIG_SIT ) )
	p_percent_trigger( NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_SIT );
    }
    else
      buf[0] = '\0';

    if (IS_AFFECTED2(ch, AFF_CATALEPSY))
    {
        send_to_char( "You're in a state of self-induced catalepsy.  You can't do that!\n\r", ch );
        send_to_char( "Type revive to revive yourself if you want.\n\r", ch );
        return;
    } 
    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL)
    {
      sendf( ch, "You can't get into a comfortable position on the back of %s.\n\r",ch->pcdata->pStallion->short_descr);
      return;
    }

    check_dispel(-99, ch, gsn_stance );

    switch (ch->position)
    {
    case POS_MEDITATE:
        act_new( "You stop meditating and relax$t.", ch, buf, NULL, TO_CHAR, POS_DEAD );
        act( "$n stops meditating and relaxes$t.", ch, buf, NULL, TO_ROOM );
	ch->position = POS_SITTING;
	dreamprobe_cast(ch, NULL );
	break;
    case POS_SLEEPING:
        if (IS_AFFECTED(ch,AFF_SLEEP))
        {
            send_to_char("You can't wake up!\n\r",ch);
            return;
        }
	if ( is_affected(ch, gsn_camp))
        {
            send_to_char( "You feel drained from deprivation of sleep.\n\r", ch);
            if (is_affected(ch, gsn_drained))    
                affect_strip(ch,gsn_drained);
            af.type             = gsn_drained;
            af.level            = ch->level;
            af.duration         = 10;
            af.location         = APPLY_NONE;
            af.modifier         = 0;
            af.bitvector        = 0;
            affect_to_char(ch,&af);
            affect_strip(ch,gsn_camp);
        }
        act_new( "You wake and sit up$t.", ch, buf, NULL, TO_CHAR, POS_DEAD );
        act( "$n wakes and sits up$t.", ch, buf, NULL, TO_ROOM );
        ch->position = POS_SITTING;
	dreamprobe_cast(ch, NULL );
        break;
    case POS_RESTING:
        act_new( "You stop resting and sit$t.", ch, buf, NULL, TO_CHAR, POS_DEAD );
        act( "$n wakes and sits up$t.", ch, buf, NULL, TO_ROOM );
        ch->position = POS_SITTING;
        break;
    case POS_SITTING:
        send_to_char("You are already sitting down.\n\r",ch);
        break;
    case POS_STANDING:
        act_new("You sit down$t.",ch, buf,NULL,TO_CHAR, POS_DEAD);
        act("$n sits down$t.",ch, buf,NULL,TO_ROOM);
        ch->position = POS_SITTING;
        break;
    }
}

void do_meditate( CHAR_DATA *ch, char *argument )
{
    if (IS_AFFECTED2(ch, AFF_CATALEPSY))
    {
        send_to_char( "You're in a state of self-induced catalepsy.  You can't do that!\n\r", ch );
        send_to_char( "Type revive to revive yourself if you want.\n\r", ch );
        return;
    } 
/*    if (ch->class != class_lookup("monk"))
    {
	send_to_char("You assume a very comfortable position and begin to meditate.\n", ch);
	act("$n rests and begins to meditate... $e chants softly to $mself.", ch, NULL, NULL, TO_ROOM);
	return;
    }
*/
    /* reset memorization */
    if (!IS_NPC(ch)){
      //reset memticks
      reset_memticks( ch->pcdata );    
    }

    check_dispel(-99, ch, gsn_stance );

    switch ( ch->position )
    {
    case POS_MEDITATE:
	send_to_char( "You already are meditating.\n\r", ch );
	break;
    case POS_SLEEPING:
	send_to_char( "You have to wake up first.\n\r", ch );
	break;
    case POS_RESTING:
        act_new( "You sit up and begin to meditate.", ch, NULL, NULL, TO_CHAR, POS_DEAD );
	if (!IS_AFFECTED(ch, AFF_HIDE))
	  act( "$n sits up and begins to meditate.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_MEDITATE;
	dreamprobe_cast(ch, NULL );
	break;
    case POS_SITTING:
        act_new( "You begin to meditate.", ch, NULL, NULL, TO_CHAR, POS_DEAD );
	if (!IS_AFFECTED(ch, AFF_HIDE))
	  act( "$n begins to meditate.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_MEDITATE;
	dreamprobe_cast(ch, NULL);
	break;
    case POS_STANDING: 
        act_new( "You sit down and begin to meditate.", ch, NULL, NULL, TO_CHAR, POS_DEAD );
	if (!IS_AFFECTED(ch, AFF_HIDE))
	  act( "$n sits down and begins to meditate.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_MEDITATE;
	dreamprobe_cast(ch, NULL );
	break;
    case POS_FIGHTING:
	send_to_char( "You must settle down first!\n\r", ch );
	break;
    }
}

void do_sleep( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA* obj = NULL;
    char buf[MIL];
    bool force = FALSE;

    if (IS_AFFECTED(ch,AFF_SLEEP))
	force = TRUE;	

    /* check for acting on objects */
    if (!force && argument[0] != '\0'){
      if (!str_cmp("on", argument))
	argument = one_argument(argument, buf);
      if ((obj = get_obj_list( ch, argument, ch->in_room->contents )) == NULL){
	send_to_char("You do not see such object here.\n\r", ch);
	return;
      }
      sprintf(buf," on %s", obj->short_descr);
      if ( obj && HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )
	p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );
      if ( obj && HAS_TRIGGER_ROOM( ch->in_room, TRIG_SIT ) )
	p_percent_trigger( NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_SIT );
    }
    else
      buf[0] = '\0';

    if (IS_AFFECTED2(ch, AFF_CATALEPSY))
    {
        send_to_char( "You're as close to sleeping as you can get!\n\r", ch );
        send_to_char( "Type revive to revive yourself if you want.\n\r", ch );
        return;
    } 
    if (is_affected(ch,gsn_insomnia) && !force)
    {
	send_to_char("You cannot manage to sleep due to the insomnia.\n\r",ch);
	return;
    }
    else if (is_affected(ch,gen_suffocate) && !force)
    {
	send_to_char("You cannot sleep while suffocating!\n\r",ch);
	return;
    }
    if (!force && ch->class == class_lookup("vampire") 
	&& (mud_data.time_info.hour < 6 || mud_data.time_info.hour > 18))
    {
        send_to_char("Vampires do not sleep at night!\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_empower) && !force)
    {
	send_to_char("You can't concentrate enough while sleeping.\n\r",ch);
	return;
    }
    check_dispel(-99, ch, gsn_stance );

    if (IS_AFFECTED2(ch, AFF_SHADOWFORM))
    {
	affect_strip ( ch, gsn_shadowform			);
	REMOVE_BIT	 ( ch->affected2_by, AFF_SHADOWFORM	);
	act_new("Your body regains its substance, and you materialize into existence.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n's body regains its substance, and $e materializes into existence.",ch,NULL,NULL,TO_ROOM);
	WAIT_STATE( ch, 24);
    }
    if (IS_AFFECTED2(ch, AFF_TREEFORM))
    {
        affect_strip ( ch, gsn_treeform                       );
        REMOVE_BIT       ( ch->affected2_by, AFF_TREEFORM     );
        act_new("Your body returns to its normal form.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
        act("$n's body returns to its normal form.",ch,NULL,NULL,TO_ROOM);
        WAIT_STATE( ch, 24);
    }
    switch ( ch->position )
    {
    case POS_MEDITATE:
        act_new( "You stop meditating and go to sleep$t.", ch, buf, NULL, TO_CHAR, POS_DEAD );
        act( "$n stops meditating and goes to sleep.$t", ch, buf, NULL, TO_ROOM );
	ch->position = POS_SLEEPING;
	break;
    case POS_SLEEPING:
	send_to_char( "You are already sleeping.\n\r", ch );
	break;
    case POS_RESTING:
    case POS_SITTING:
    case POS_STANDING: 
        act_new( "You go to sleep$t.", ch, buf, NULL, TO_CHAR, POS_DEAD );
        act( "$n goes to sleep$t.", ch, buf, NULL, TO_ROOM );
        ch->position = POS_SLEEPING;
	dreamprobe_cast(ch, NULL );
	break;
    case POS_FIGHTING:
	send_to_char( "You are already fighting!\n\r", ch );
	break;
    }
}

void do_wake( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        do_stand( ch, argument );
        return;
    }
    if ( !IS_AWAKE(ch) )
    {
        send_to_char( "You are asleep yourself!\n\r",ch );
        return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r",ch );
        return;
    }
    if ( IS_AWAKE(victim) )
    {
        sendf(ch, "%s is already awake.\n\r", PERS(victim,ch));
        return;
    }
    if ( IS_AFFECTED(victim, AFF_SLEEP))
    {
        sendf(ch, "You can't wake %s!\n\r", PERS(victim,ch));
        return;
    }
    sendf(victim, "%s wakes you.\n\r", PERS(ch,victim));
    do_stand(victim,"");
}

void do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int chance = get_skill(ch, gsn_sneak);

    if (chance < 2){
      send_to_char("Huh?\n\r", ch);
      return;
    }
    else if (is_affected(ch, gsn_sneak)){
      send_to_char("You trample around loudly again.\n\r",ch);
      affect_strip( ch, gsn_sneak );
      return;
    }
    else if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL){
      sendf( ch, "You can't sneak on the back of %s.\n\r",ch->pcdata->pStallion->short_descr);
      return;
    }
    else if ( IS_AFFECTED(ch, AFF_FAERIE_FOG) 
	      || IS_AFFECTED(ch, AFF_FAERIE_FIRE)){
      send_to_char( "You can't sneak while glowing.\n\r", ch);
      return;
    }

    send_to_char( "You attempt to move silently.\n\r", ch );
    WAIT_STATE(ch, 2);
    affect_strip( ch, gsn_sneak );

    if (IS_AFFECTED(ch,AFF_SNEAK))
      return;
    chance = UMAX(1, chance + (get_curr_stat(ch,STAT_LUCK) - 12) / 2);

    if ( number_percent() < chance){
      check_improve(ch,gsn_sneak,TRUE,1);
      af.where     = TO_AFFECTS;
      af.type      = gsn_sneak;
      af.level     = ch->level; 
      af.duration  = ch->level;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = AFF_SNEAK;
      affect_to_char( ch, &af );
    }
    else
      check_improve(ch,gsn_sneak,FALSE,1);
}
    
void do_hide( CHAR_DATA *ch, char *argument ){

  int skill = get_skill(ch, gsn_hide);
  int roll = number_percent();

  if ( IS_AFFECTED(ch, AFF_HIDE) ){
    REMOVE_BIT(ch->affected_by, AFF_HIDE);
    act("You step out of the shadows.",ch,NULL,NULL,TO_CHAR);
    act("$n steps out of the shadows.",ch,NULL,NULL,TO_ROOM);
    return;
  }
  if (skill < 2){
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL)
    {
      sendf( ch, "You can't hide on the back of %s.\n\r",ch->pcdata->pStallion->short_descr);
      return;
    }
  if ( IS_AFFECTED(ch, AFF_FAERIE_FOG) || IS_AFFECTED(ch, AFF_FAERIE_FIRE))
    {
      send_to_char( "You can't hide while glowing.\n\r", ch);
      return;
    }
  if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
      send_to_char( "You can't hide while charmed.\n\r",ch);
      return;
    }
  if (IS_SET(ch->in_room->room_flags2, ROOM_JAILCELL)){
    send_to_char("There's nothing around to hide in.\n\r",ch);
    return;
  }
  if (ch->in_room->sector_type != SECT_INSIDE 
      && ch->in_room->sector_type != SECT_CITY 
      && ch->in_room->sector_type != SECT_FIELD)
    {
      if (ch->in_room->sector_type == SECT_AIR 
	  || ch->in_room->sector_type == SECT_DESERT)
	{
	  send_to_char("There's nothing around to hide in.\n\r",ch);
	  return;
	}
      else
	{
	  send_to_char("You're not comfortable enough in this territory to know where to hide.\n\r",ch);
	  return;
	}
    }
  send_to_char( "You attempt to hide.\n\r", ch );
  WAIT_STATE(ch,skill_table[gsn_hide].beats);


  if (skill <= 100)
    roll -= (get_curr_stat(ch,STAT_LUCK) - 14) / 2;

  if ( roll < skill ){
    SET_BIT(ch->affected_by, AFF_HIDE);
    check_improve(ch,gsn_hide,TRUE,1);
  }
  else
    check_improve(ch,gsn_hide,FALSE,1);
}

void do_camouflage( CHAR_DATA *ch, char *argument )
{
  int skill = IS_NPC(ch) ? 999 : get_skill(ch,gsn_camouflage);
  int chance = number_percent();

  const int gsn_concealment = skill_lookup("concealment");
  int conceal = get_skill(ch, gsn_concealment);
  bool fAll = FALSE;

  if ( IS_AFFECTED2(ch, AFF_CAMOUFLAGE) ){
    REMOVE_BIT(ch->affected2_by, AFF_CAMOUFLAGE);
    act("You step out from your cover.",ch,NULL,NULL,TO_CHAR);
    act("$n steps out from $s cover.",ch,NULL,NULL,TO_ROOM);
    return;
  }

  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL){
    sendf( ch, "You can't camouflage properly while on %s.\n\r",ch->pcdata->pStallion->short_descr);
    return;
  }
  if ( IS_AFFECTED(ch, AFF_FAERIE_FOG) || IS_AFFECTED(ch, AFF_FAERIE_FIRE)){
    send_to_char( "You can't camouflage yourself while glowing.\n\r", ch);
    return;
  }
  if (!IS_NPC(ch)
      && ch->in_room->sector_type != SECT_FOREST 
      && ch->in_room->sector_type != SECT_HILLS
      && ch->in_room->sector_type != SECT_SNOW
      && ch->in_room->sector_type != SECT_MOUNTAIN 
      && ch->in_room->sector_type != SECT_FIELD){
    if (ch->in_room->sector_type == SECT_AIR || ch->in_room->sector_type == SECT_DESERT){
      send_to_char("There's nothing around to camouflage yourself with.\n\r",ch);
      return;
    }
    else{
      send_to_char("You don't see any suitable cover to camouflage.\n\r",ch);
      return;
    }
  }

  if (IS_WEREBEAST(ch->race))
    chance -= 10;
  if (skill <= 100){
    chance -= (get_curr_stat(ch,STAT_LUCK) - 14) / 2;
  }

  send_to_char( "You attempt to camouflage yourself.\n\r", ch );
  WAIT_STATE(ch,skill_table[gsn_camouflage].beats);
  if ( chance < skill){
    SET_BIT(ch->affected2_by, AFF_CAMOUFLAGE);
    check_improve(ch,gsn_camouflage,TRUE,0);
  }
  else
    check_improve(ch,gsn_camouflage,FALSE,0);

  if (number_percent() < conceal){
    check_improve(ch, gsn_concealment, TRUE, 1);
    fAll = TRUE;
  }
  else
    check_improve(ch, gsn_concealment, FALSE, 1);

  /* Verdigar Tat camoes pets */
  if ((ch->tattoo && ch->tattoo == deity_lookup("nature")) || fAll){
    CHAR_DATA* vch;

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room){
      if (IS_NPC(vch) && IS_AFFECTED(vch, AFF_CHARM) 
	  && vch->master && vch->master == ch)
	SET_BIT(vch->affected2_by, AFF_CAMOUFLAGE);
    }
  }
}

void do_visible( CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA* paf;

  /* photon wall delay */
  if ( (paf = affect_find(ch->affected, gen_photon_wall)) != NULL){
    paf->modifier = 3;
  }
  if (IS_AFFECTED(ch, AFF_INVISIBLE))
    {
      affect_strip ( ch, gsn_invis			);
      affect_strip ( ch, gsn_mass_invis		);
      REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE	);
      act("You fade into existence.",ch,NULL,NULL,TO_CHAR);
      act("$n fades into existence.",ch,NULL,NULL,TO_ROOM);
    }
  if (IS_AFFECTED2(ch, AFF_TREEFORM))
    {
	affect_strip ( ch, gsn_treeform );
        REMOVE_BIT   ( ch->affected2_by, AFF_TREEFORM );
	act("You emerge from your treeformed state.",ch,NULL,NULL,TO_CHAR);
	act("$n emerges from $s treeformed state.",ch,NULL,NULL,TO_ROOM);
	WAIT_STATE( ch, 24);
    }
    if (IS_AFFECTED2(ch, AFF_CAMOUFLAGE))
    {
	affect_strip( ch, gsn_camouflage );
	REMOVE_BIT   ( ch->affected2_by, AFF_CAMOUFLAGE		);
	act("You step out from your cover.",ch,NULL,NULL,TO_CHAR);
	act("$n steps out from $s cover.",ch,NULL,NULL,TO_ROOM);
    }
    if (IS_AFFECTED(ch, AFF_HIDE))
    {
	affect_strip( ch, gsn_hide );
	REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
	act("You step out of the shadows.",ch,NULL,NULL,TO_CHAR);
	act("$n steps out of the shadows.",ch,NULL,NULL,TO_ROOM);
    }
    if (IS_AFFECTED(ch, AFF_SNEAK) 
	&& ch->race != race_lookup("faerie")
	&& ch->race != race_lookup("elf")
	&& ch->race != race_lookup("drow"))
    {
	affect_strip ( ch, gsn_sneak			);
	REMOVE_BIT   ( ch->affected_by, AFF_SNEAK		);
	send_to_char("You trample around loudly again.\n\r",ch);
    }
    if (IS_AFFECTED2(ch, AFF_SHADOWFORM))
    {
	affect_strip ( ch, gsn_shadowform			);
	REMOVE_BIT	 ( ch->affected2_by, AFF_SHADOWFORM	);
	act("Your body regains its substance, and you materialize into existence.",ch,NULL,NULL,TO_CHAR);
	act("$n's body regains its substance, and $e materializes into existence.",ch,NULL,NULL,TO_ROOM);
	WAIT_STATE( ch, 24);
    }
    if (is_affected(ch, gen_d_shroud))
      affect_strip(ch, gen_d_shroud);
    
}

void do_revert( CHAR_DATA *ch, char *argument )
{
    if (is_affected(ch,gsn_bat_form) || is_affected(ch,gsn_wolf_form) || is_affected(ch,gsn_mist_form))
    {
	affect_strip(ch,gsn_bat_form);
	affect_strip(ch,gsn_wolf_form);
	affect_strip(ch,gsn_mist_form);
	free_string( ch->short_descr );
        ch->short_descr = str_empty;
        ch->dam_type = 17;
	send_to_char("You revert back to your natural form.\n\r",ch);
	act("$n reverts back to $s natural form.",ch,NULL,NULL,TO_ROOM);
	WAIT_STATE( ch, 12);
	return;
    }
    else if (is_affected(ch,gsn_doppelganger)){
      affect_strip( ch, gsn_doppelganger );
      act("You revert to your original form.",ch,NULL,NULL,TO_CHAR);
      act("$n reverts to $s original form.",ch,NULL,NULL,TO_ROOM);
      WAIT_STATE( ch, 12);
      return;
    }
    else if (is_affected(ch, gsn_weretiger) || is_affected(ch, gsn_werewolf) 
	     || is_affected(ch, gsn_werebear) || is_affected(ch, gsn_werefalcon)){
      affect_strip(ch,gsn_weretiger);
      affect_strip(ch,gsn_werewolf);
      affect_strip(ch,gsn_werebear);
      affect_strip(ch,gsn_werefalcon);
      affect_strip(ch,gsn_werepower);

      free_string( ch->short_descr );
      ch->short_descr = str_empty;
      ch->dam_type = 17;

      send_to_char("You revert back to your natural form.\n\r",ch);
      act("$n reverts back to $s natural form.",ch,NULL,NULL,TO_ROOM);
      WAIT_STATE( ch, 12);
      return;
    }
    send_to_char("You are already in your native form.\n\r",ch);
    return;
}

void do_recall( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;
    if (IS_NPC(ch))
    {
	send_to_char("Only players can recall.\n\r",ch);
	return;
    }
    act( "$n tries to recall to $s hometown.", ch, 0, 0, TO_ROOM );
    location = get_room_index(get_temple(ch));
    if (location == NULL)
    {
	send_to_char( "You are completely lost.\n\r", ch );
	return;
    }
    if ( ch->in_room == location )
	return;
    if (ch->level > 10 && !IS_IMMORTAL(ch))
    {
        send_to_char("Only level 10 and under get free recalls.\n\r", ch);
        return;
    }
    if (is_fight_delay(ch,60))
    {
	send_to_char("You are too pumped to pray.\n\r",ch);
	return;
    }
    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT) || IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
	 || IS_AFFECTED(ch, AFF_CURSE) || IS_SET(ch->act, PLR_DOOF) || is_affected(ch, gsn_damnation) )
      
      {
	send_to_char( "The Gods have forsaken you.\n\r", ch );
	return;
      }
    else if (is_affected(ch,gen_ensnare)){
      send_to_char("You sense a powerful magical field preventing your departure.\n\r", ch);
      return;
    }
    if ( ( victim = ch->fighting ) != NULL )
    {
	int lose, skill = get_skill(ch,gsn_recall);
	if ( number_percent() < 80 * skill / 100 )
	{
	    check_improve(ch,gsn_recall,FALSE,0);
	    WAIT_STATE( ch, 4 );
	    sendf( ch, "You failed!.\n\r");
	    return;
	}
	lose = (ch->desc != NULL) ? 25 : 50;
	gain_exp( ch, 0 - lose );
	check_improve(ch,gsn_recall,TRUE,4);
	sendf( ch, "You recall from combat!  You lose %d exps.\n\r", lose );
        if ( IS_NPC(ch->fighting) && !IS_SET(ch->fighting->off_flags,CABAL_GUARD))
        {
            if (!IS_NPC(ch))
                ch->fighting->hunting = ch;   
            else if (ch->master != NULL)
                ch->fighting->hunting = ch->master;
            else
                ch->fighting->hunting = ch;
            ch->fighting->hunttime = 0;
        }
	stop_fighting( ch, TRUE );
    }
    ch->move /= 2;
    act( "$n disappears.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_look( ch, "auto" );
    send_to_char("\n\rIMPORTANT: Only level 10 and under get free recalls.\n\r", ch);
    if (ch->pet != NULL)
    {
	char_from_room( ch->pet );
	char_to_room ( ch->pet, location );
    }
}

void do_train( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    CHAR_DATA *mob;
    sh_int stat = - 1;
    char *pOutput = NULL;

    if ( IS_NPC(ch) )
	return;
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( ch->class == class_lookup("monk") && IS_NPC(mob) && IS_SET(mob->act, ACT_IS_HEALER))
            if (mob->pIndexData->vnum == get_temple(ch) || mob->pIndexData->vnum == (get_temple(ch) + 1))
		break;
	if ( IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN) )
	    break;
    }
    if ( mob == NULL )
    {
	send_to_char( "You can't do that here.\n\r", ch );
	return;
    }
    if (is_exiled(ch, mob->in_room->area->vnum )){
      char buf[MIL];
      sprintf( buf, "No one in %s will deal with you %s!", mob->in_room->area->name, PERS2(ch));
      do_say( mob, buf );
      return;
    }
    if ( argument[0] == '\0' )
    {
	sendf( ch, "You have %d training sessions.\n\r", ch->train );
	argument = "foo";
    }
    if (ch->train < 1)
    {
	send_to_char("You have no training sessions left.\n\r",ch);
	return;
    }
    if ( !str_cmp( argument, "str" ) )
    {	stat        = STAT_STR; 	pOutput     = "strength";    }
    else if ( !str_cmp( argument, "int" ) )
    {	stat	    = STAT_INT; 	pOutput     = "intelligence";    }
    else if ( !str_cmp( argument, "wis" ) )
    {	stat	    = STAT_WIS; 	pOutput     = "wisdom";    }
    else if ( !str_cmp( argument, "dex" ) )
    {	stat  	    = STAT_DEX; 	pOutput     = "dexterity";    }
    else if ( !str_cmp( argument, "con" ) )
    {	stat	    = STAT_CON; 	pOutput     = "constitution";    }
    else if ( !str_cmp(argument, "hp" ) );
    else if ( !str_cmp(argument, "mana" ) );
    else if ( !str_cmp(argument, "move" ) );
    else
    {
	strcpy( buf, "You can train:" );
	if ( ch->perm_stat[STAT_STR] < get_max_train(ch,STAT_STR))     strcat( buf, " str" );
	if ( ch->perm_stat[STAT_INT] < get_max_train(ch,STAT_INT))     strcat( buf, " int" );
	if ( ch->perm_stat[STAT_WIS] < get_max_train(ch,STAT_WIS))     strcat( buf, " wis" );
	if ( ch->perm_stat[STAT_DEX] < get_max_train(ch,STAT_DEX))     strcat( buf, " dex" );
	if ( ch->perm_stat[STAT_CON] < get_max_train(ch,STAT_CON))     strcat( buf, " con" );
        strcat( buf, " hp mana move");
	if ( buf[strlen(buf)-1] != ':' )
	{
	    strcat( buf, ".\n\r" );
	    send_to_char( buf, ch );
	}
	else
            send_to_char( "You have nothing left to train", ch);
	return;
    }
    if (!str_cmp("hp",argument)){
      if (ch->class == gcn_adventurer){
	send_to_char("Adventurers cannot train health.\n\r", ch);
	return;
      }      
      --ch->train;
      ch->pcdata->perm_hit += 10;
      ch->max_hit += 10;
      ch->hit +=10;
      act( "Your durability increases!",ch,NULL,NULL,TO_CHAR);
      act( "$n's durability increases!",ch,NULL,NULL,TO_ROOM);
      return;
    }
    if (!str_cmp("mana",argument)){
      if (ch->class == gcn_adventurer){
	send_to_char("Adventurers cannot train their minds.\n\r", ch);
	return;
      }      
      --ch->train;
      ch->pcdata->perm_mana += 20;
      ch->max_mana += 20;
      ch->mana += 20;
      act( "Your power increases!",ch,NULL,NULL,TO_CHAR);
      act( "$n's power increases!",ch,NULL,NULL,TO_ROOM);
      return;
    }
    if (!str_cmp("move",argument)){
      if (ch->class == gcn_adventurer){
	send_to_char("Adventurers cannot train their stamina.\n\r", ch);
	return;
      }      
      --ch->train;
      ch->pcdata->perm_move += 30;
      ch->max_move += 30;
      ch->move += 30;
      act( "Your endurance increases!",ch,NULL,NULL,TO_CHAR);
      act( "$n's endurance increases!",ch,NULL,NULL,TO_ROOM);
      return;
    }
    if ( ch->perm_stat[stat]  >= get_max_train(ch,stat) )
    {
      act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
      return;
    }
    --ch->train;
    ch->perm_stat[stat]++;
    act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
    act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
}

void scan_room (CHAR_DATA *ch, ROOM_INDEX_DATA *room, int distance, int dir)
{
    CHAR_DATA *victim = room->people;
    OBJ_DATA* obj;
    int number_found = 0;
    bool fLight = FALSE;

    if (IS_SET(room->room_flags,ROOM_NO_SCAN))
      return;
/* add light if required */
    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL 
	 && obj->item_type == ITEM_LIGHT
	 && obj->value[2] != 0){
      fLight = TRUE;
      room->light++;
    }
    
    while (victim != NULL){
      if ((can_see(ch,victim) 
	   || (IS_AFFECTED2(ch,AFF_SENSE_EVIL) 
	       && IS_EVIL(victim)
	       && !is_affected(victim, gen_d_shroud)
	       && (victim->invis_level < 1 || victim->incog_level < 1))) && !IS_IMMORTAL(victim))
        {
	  if (number_found == 0)
	    sendf (ch, "*** Range %d (%s) ***\n\r", distance, dir_name[dir]);
	  number_found++;
	  show_char_to_char_0(victim,ch);
        }
      victim = victim->next_in_room;
    }
    if (fLight)
      room->light --;
}

void do_scan (CHAR_DATA *ch, char *argument)
{
    EXIT_DATA * pexit;
    ROOM_INDEX_DATA * room;
    extern char * const dir_name[];
    char arg[MIL];
    int dir, distance;
    if (IS_AFFECTED2(ch,AFF_TERRAIN))
    {
      if (is_affected(ch, gsn_forest_mist)){
	if (ch->in_room->sector_type != SECT_AIR 
	    && ch->in_room->sector_type != SECT_DESERT 
	    && ch->in_room->sector_type != SECT_CITY
	    && ch->in_room->sector_type != SECT_INSIDE){
	  send_to_char("A thick mist prevents any observation.\n\r",ch);
	  return;
	}
      }
      else
	send_to_char("Thick walls of darkness stop your sight short.\n\r",ch);
      return;
    }
    if (IS_SET(ch->in_room->room_flags,ROOM_NO_SCAN))
    {
	send_to_char("A blanket of darkness prevents you from scanning out of this room.\n\r",ch);
	return;
    }

    argument = one_argument(argument, arg);
    if      (!str_cmp(arg, "n") || !str_cmp(arg, "north")) dir = 0;
    else if (!str_cmp(arg, "e") || !str_cmp(arg, "east"))  dir = 1;
    else if (!str_cmp(arg, "s") || !str_cmp(arg, "south")) dir = 2;
    else if (!str_cmp(arg, "w") || !str_cmp(arg, "west"))  dir = 3;
    else if (!str_cmp(arg, "u") || !str_cmp(arg, "up" ))   dir = 4;
    else if (!str_cmp(arg, "d") || !str_cmp(arg, "down"))  dir = 5;
    else if (!str_cmp(arg, "a") || !str_cmp(arg, "all"))   dir = 99;
    else if (IS_NULLSTR(arg)				)  dir = 99;
    else
    {
	send_to_char("Which way do you wish to scan?\n\r",ch);
	return;
    }
    if (dir == 99)
    {
	int allscan;
	int range = (ch->level + 10) / 10;

	if (IS_PERK(ch, PERK_EAGLEEYE))
	  range += 2;

    	act("You scan all around.", ch, NULL,NULL, TO_CHAR);
    	act("$n scans all around.", ch, NULL,NULL, TO_ROOM);
	for (allscan = 0; allscan <= 5; allscan++)
	{
    	    room = ch->in_room;
    	    act("You scan $T.", ch, NULL, dir_name[allscan], TO_CHAR);
	    for (distance = 1 ; distance <= range; distance++)
	    {
	        pexit = room->exit[allscan];
	        if ((pexit == NULL) || (pexit->to_room == NULL) || (IS_SET(pexit->exit_info, EX_CLOSED)))
		    break;
		if (IS_SET(pexit->to_room->room_flags,ROOM_NO_SCAN))
		{
		    send_to_char("You vision cannot penetrate the blanket of darkness surrounding this room.\n\r",ch);
		    send_to_char("*** Too Dark ***\n\r",ch);
	            break;
		}
	        scan_room(ch,pexit->to_room,distance,allscan);
        	room = pexit->to_room;
	    }
	}
    }
    else
    {	
      int range = (ch->level + 10) / 10;
      if (IS_PERK(ch, PERK_EAGLEEYE))
	range += 2;

    	room = ch->in_room;
    	act("You scan $T.", ch, NULL, dir_name[dir], TO_CHAR);
    	act("$n scans $T.", ch, NULL, dir_name[dir], TO_ROOM);
    for (distance = 1 ; distance <= range; distance++)
    {
        pexit = room->exit[dir];
        if ((pexit == NULL) || (pexit->to_room == NULL) || (IS_SET(pexit->exit_info, EX_CLOSED)))
	    break;
	if (IS_SET(pexit->to_room->room_flags,ROOM_NO_SCAN))
	{
	    send_to_char("You vision cannot penetrate the blanket of darkness surrounding this room.\n\r",ch);
	    send_to_char("*** Too Dark ***\n\r",ch);
            break;
	}
        scan_room(ch,pexit->to_room,distance,dir);
        room = pexit->to_room;
    }
    }
}

void do_scout (CHAR_DATA *ch, char *argument)
{
    EXIT_DATA * pexit;
    ROOM_INDEX_DATA * room;
    extern char * const dir_name[];
    char arg[MIL];
    int dir, distance, chance, extra = 0;
    if ( (chance = get_skill(ch,gsn_scout)) == 0)
    {
	send_to_char("You don't know how to scout.\n\r",ch);
	return;
    }
    if (IS_AFFECTED2(ch,AFF_TERRAIN))
    {
        send_to_char("Thick walls of darkness stop your sight short.\n\r",ch);
        return;
    }
    if (IS_SET(ch->in_room->room_flags,ROOM_NO_SCAN))
    {
	send_to_char("A blanket of darkness prevents you from scanning out of this room.\n\r",ch);
	return;
    }
    if (ch->in_room->vnum == 70)
    {
	send_to_char("You see millions of stars.\n\r",ch);
	return;
    }
    argument = one_argument(argument, arg);
    if      (!str_cmp(arg, "n") || !str_cmp(arg, "north")) dir = 0;
    else if (!str_cmp(arg, "e") || !str_cmp(arg, "east"))  dir = 1;
    else if (!str_cmp(arg, "s") || !str_cmp(arg, "south")) dir = 2;
    else if (!str_cmp(arg, "w") || !str_cmp(arg, "west"))  dir = 3;
    else if (!str_cmp(arg, "u") || !str_cmp(arg, "up" ))   dir = 4;
    else if (!str_cmp(arg, "d") || !str_cmp(arg, "down"))  dir = 5;
    else
    {
	send_to_char("Which direction do you wish to scout?\n\r",ch);
	return;
    }
    act("You attempt to scout $T.", ch, NULL, dir_name[dir], TO_CHAR);
    act("$n attempts to scout $T.", ch, NULL, dir_name[dir], TO_ROOM);
    WAIT_STATE( ch, skill_table[gsn_scout].beats );
    chance += 2*(get_curr_stat(ch,STAT_LUCK) - 16);
    if (IS_WEREBEAST(ch->race))
	chance += 10;
    if ( number_percent() < chance)
    {

	if (IS_PERK(ch, PERK_EAGLEEYE))
	  extra = number_range(3,5);	 
	else
	  extra = number_range(1,5);
   	check_improve(ch,gsn_scout,TRUE,0);
    }
    else
    {
	send_to_char("You squint your eyes but can't manage to see any further.\n\r",ch);
	check_improve(ch,gsn_scout,FALSE,0);
    }
    room = ch->in_room;
    for (distance = 1 ; distance <= ((ch->level + 10) /10)+extra; distance++)
    {
        pexit = room->exit[dir];
        if ((pexit == NULL) || (pexit->to_room == NULL) || (IS_SET(pexit->exit_info, EX_CLOSED)))
            break;
	if (IS_SET(pexit->to_room->room_flags,ROOM_NO_SCAN))
	{
	    send_to_char("You vision cannot penetrate the blanket of darkness surrounding this room.\n\r",ch);
	    send_to_char("*** Too Dark ***\n\r",ch);
            break;
	}
        scan_room(ch,pexit->to_room,distance,dir);
        room = pexit->to_room;
    }
}

void do_kinetics( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *tattoo = NULL;
    AFFECT_DATA af;
    if (IS_NPC(ch))
	return;
    if (is_affected(ch,gsn_kinetics))
    {
	affect_strip(ch,gsn_kinetics);
	send_to_char("Kinetic transfer OFF.\n\r",ch);
	return;
    }
    tattoo= get_eq_char(ch, WEAR_TATTOO);
    if (tattoo == NULL || tattoo->pIndexData->vnum != 93)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = gsn_kinetics;
    af.level     = ch->level;
    af.duration  = -1;   
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    send_to_char("Kinetic transfer ON.\n\r",ch);
}


//03-25 14:00 Viri: Added 2 if statements in move_char(..) for city sneak
//in vamp class.

//03-25 12:00 Viri: Created bool vamp_can_sneak(CHAR_DATA*, ROOM_INDEX_DATA*)
//the function tests if vampire can sneak in th room 
//Tests for autosneak in both wolf and human form.


//03-25 12:00 Viri: Implemented vamp_can_sneak in move_char(..)
//04-18 22:00 Viri: Added LEAVE_CHECK and ENTER_CHECK
