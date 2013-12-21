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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "song.h"
#include "recycle.h"
#include "interp.h"

extern char *target_name;
extern int eq_positions[24];

/* Contains the tattoo spells/functions */

/* PALISON */
void tattoo_tick_palison(CHAR_DATA* ch, OBJ_DATA* tattoo){

  CHAR_DATA* vch = ch->fighting;
  /* All we do here is cast sear at 11/12/13 hours if we are fighting. */

  if (vch == NULL || mud_data.time_info.hour < 11 || mud_data.time_info.hour > 13)
    return;

  if ( !IS_OUTSIDE(ch) || IS_SET(ch->in_room->room_flags, ROOM_DARK))
    return;

  act("$p flares brightly as sun's rays fall upon its surface!", ch, tattoo, vch, TO_ALL);
  spell_sear(skill_lookup("sear"), ch->level, ch, vch, TARGET_CHAR);
}

/* Virigoth */
CHAR_DATA* undead_check_guard(CHAR_DATA* ch, CHAR_DATA* mob)
{
  const int MAX_CHS = 12;

  CHAR_DATA* och;
  CHAR_DATA* och_next;
  AFFECT_DATA af;

  CHAR_DATA* chs[MAX_CHS];

  int tot_chs = 0;
  
/* Put all avaliable saving chars in an array */
  for ( och = ch->in_room->people; och != NULL; och = och_next ){
    och_next = och->next_in_room;
	
    if (!IS_NPC(och) || !IS_UNDEAD(och)
	|| !can_see(och, ch)
	|| och->level > ch->level
	|| och->fighting == ch
	|| ch->fighting == och
	|| och == mob
	|| (och->master != NULL && och->master != ch)
	)
      continue;

    chs[tot_chs++] = och;
  }//end for

  /* now we pick one character out of all possible choices */
  if (tot_chs == 0)
    return NULL;
  /* we return a random pick */  
  och =  chs[number_range(0, UMAX(0, tot_chs - 1))];
  //set temporary charm flag to prevent from fleeing.
  af.type = gsn_charm_person;
  af.level = ch->level;
  af.duration = 1 + number_fuzzy(1);
  af.where = TO_AFFECTS;
  af.bitvector = AFF_CHARM;
  af.location = APPLY_NONE;
  af.modifier = 0;
  if (!IS_AFFECTED(och, AFF_CHARM))
    affect_to_char(och, &af);
  return och;
}

/* Tatto spell for Virigoth tattoo, not usuable by normal means */
void spell_darklife( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  //This is basicly a shadowstalk spell except it lasts only a single combat round.
  CHAR_DATA* victim = ch->fighting;
  CHAR_DATA *shadow;
  OBJ_DATA *wield;
  int i;
  char buf[MSL];
  char *name;
  
  if (victim == NULL)
    {
      send_to_char("You must be in combat.\n\r", ch);
      return;
    }
  if (ch->fighting != victim)
    {
      send_to_char("You must be in combat.\n\r", ch);
      return;
    }

  if (ch->class == class_lookup("vampire")
      || ch->class == class_lookup("necromancer")){
    spell_weaken( gsn_weaken, level, ch, ch->fighting, target );
    return;
  }

  if (room_has_sun(ch->in_room))
    {
      send_to_char("The light banishes your shadow as it takes life of its own.\n\r", ch);
      return;
    }

  send_to_char("Your shadow seems to take on a life of its own!\n\r", ch); 
  act("$n's shadow takes on a life of its own!",ch,NULL,NULL,TO_ROOM);

  shadow = create_mobile( get_mob_index(MOB_VNUM_SHADOW) );
  for (i=0;i < MAX_STATS; i++)
    shadow->perm_stat[i] = ch->perm_stat[i];
  
  shadow->max_hit = ch->max_hit;
  shadow->hit = shadow->max_hit;
  shadow->max_mana = ch->max_mana;
  shadow->mana = shadow->max_mana;
  shadow->level = ch->level - 2;
  for (i=0; i < 3; i++)
    shadow->armor[i] = ch->armor[i];
  shadow->armor[3] =ch->armor[3];
  shadow->sex = ch->sex;  
  shadow->gold = 0;
    
//Now we copy over little things like weapon stats.
  shadow->damroll = ch->damroll;
  shadow->hitroll = ch->hitroll;
    
  if ( (wield = get_eq_char(ch,WEAR_WIELD)) != NULL)
    {
      shadow->damage[DICE_NUMBER] = wield->value[1];
      shadow->damage[DICE_TYPE] = wield->value[2];
      shadow->dam_type = wield->value[3];
    }
 

  name = IS_NPC(ch) ? ch->short_descr : ch->name;
  sprintf( buf, shadow->short_descr, name );
  free_string( shadow->short_descr );
  shadow->short_descr = str_dup( buf );
  
  sprintf( buf, shadow->long_descr, name );
  free_string( shadow->long_descr );
  shadow->long_descr = str_dup( buf );
  
  sprintf( buf, shadow->description, name );
  free_string( shadow->description );
  shadow->description = str_dup( buf );
  
  char_to_room(shadow,ch->in_room);

  //set char flags so mobs dont flee from it.
  SET_BIT(shadow->affected_by, AFF_CHARM);
  
  //stripsome usless flags that come with the shadowstalk mob
  affect_strip ( shadow, gsn_invis			);
  affect_strip ( shadow, gsn_mass_invis			);
  REMOVE_BIT   ( shadow->affected_by, AFF_INVISIBLE	);
  
  affect_strip( shadow, gsn_hide );
  REMOVE_BIT   ( shadow->affected_by, AFF_HIDE		);
  
  //we make the shadow attack and disappear.
  //add a leader/charm here to give exp on kills
  shadow->leader = ch;
  SET_BIT(shadow->affected_by, AFF_CHARM);
  multi_hit(shadow, victim, shadow->dam_type + TYPE_HIT);
  extract_char(shadow, TRUE);
}//end spell.


/* Over lay to beckon social */
void do_beckon(CHAR_DATA *ch, char *argument){

    char arg[MIL];
    CHAR_DATA *victim;
    bool fFol = TRUE;


    /* Now check if the target should follow */
    one_argument(argument,arg);

    if (ch->tattoo != deity_lookup("cycle"))
      fFol = FALSE;

    if (arg[0] == '\0')
      fFol = FALSE;


    if ((victim = get_char_room(ch, NULL, arg)) == NULL){
      fFol = FALSE;
      check_social(ch, "beckon", argument);
      return;
    }

    if (victim->fighting != NULL
	|| !IS_NPC(victim)
	|| !IS_UNDEAD(victim)
	|| IS_SET(victim->act, ACT_TOO_BIG)
	|| victim->level > 4 * ch->level / 5)
      fFol = FALSE;


    if (!fFol)
      {
	check_social(ch, "beckon", argument);
	return;
      }

    if ( (IS_EVIL(ch) && IS_GOOD(victim))
	 || (IS_EVIL(victim) && IS_GOOD(ch)) ){
      send_to_char("You have a feeling that your align forbids them from following you.\n\r", ch);
      return;
    }

    //message
    act("You beckon for $N to follow.", ch, NULL, victim, TO_CHAR);
    act("$n beckons $N to follow...hmmm what's going on?", ch, NULL, victim, TO_NOTVICT);
    act("$n beckons for you to follow.", ch, NULL, victim, TO_VICT);

    //make the character a follower
    if (ch->pet)
      {
	if (ch->pet->in_room == victim->in_room){
	  if (ch->pet == victim)
	    act("$n points at $mself obviously confused.", victim, NULL, NULL, TO_ROOM);
	  else
	    act("$n glances meaningfully at $N and turns away.", victim, NULL, ch->pet, TO_ROOM);
	}
	else
	  act("$n mumbles something about $N then turns away.", victim, NULL, ch->pet, TO_ROOM);
	return;
      }
    else if (!IS_NPC(ch) && ch->pcdata->familiar)
      {
	if (ch->pcdata->familiar->in_room == victim->in_room){
	  if (ch->pcdata->familiar == victim)
	    act("$n points at $mself obviously confused.", victim, NULL, NULL, TO_ROOM);
	  else
	    act("$n glances meaningfully at $N and turns away.", victim, NULL, ch->pcdata->familiar, TO_ROOM);
	}
	else
	  act("$n mumbles something about $N then turns away.", victim, NULL, ch->pcdata->familiar, TO_ROOM);
	return;
      }
    else if ( victim->master){
      if (victim->master->in_room == victim->in_room)
	act("$n glances meaningfully at $N and turns away.", victim, NULL, victim->master, TO_ROOM);
      else
	act("$n mumbles something about $N then turns away.", victim, NULL, victim->master, TO_ROOM);
      return;
    }


    act("$N glances at $n's tattoo and obediently follows $m.", ch, NULL, victim, TO_ROOM);
    act("$N glances at your tattoo and $S eyes widen in fear.", ch, NULL, victim, TO_CHAR);

    SET_BIT(victim->affected_by, AFF_CHARM);
    victim->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
    SET_BIT(victim->act, ACT_PET);

    REMOVE_BIT(victim->act, ACT_AGGRESSIVE);
    victim->hunting = NULL;

    add_follower(victim,ch);
    victim->leader = ch;
    ch->pet = victim;

}//end beckon

/* summons a demon for 1 tick */
void spell_demon_summon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *demon;
    AFFECT_DATA af;

    int i;

    if (is_affected(ch,sn))
	return;

    if (get_control(ch, MOB_VNUM_DEMON) > 0)
	return;

    act("A demon comes to your aid!",ch,NULL,NULL,TO_CHAR);
    act("A demon comes to $n's aid!",ch,NULL,NULL,TO_ROOM);

    demon = create_mobile( get_mob_index(MOB_VNUM_DEMON) );
    for (i=0;i < MAX_STATS; i++)
	demon->perm_stat[i] = ch->perm_stat[i];
    demon->max_hit = IS_NPC(ch)? URANGE(ch->max_hit,1 * ch->max_hit,30000) : URANGE(ch->pcdata->perm_hit,1*ch->pcdata->perm_hit,30000);
    demon->hit = demon->max_hit;
    demon->max_mana = IS_NPC(ch)? ch->max_mana : ch->pcdata->perm_mana;
    demon->mana = demon->max_mana;
    demon->level = ch->level / 2;
    for (i=0; i < 3; i++)
	demon->armor[i] = interpolate(demon->level,100,-100);
    demon->armor[3] = interpolate(demon->level,100,0);
    demon->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
    demon->summoner = ch;
    char_to_room(demon,ch->in_room);

    af.type               = gsn_timer;
    af.level              = level; 
    af.duration           = 0;
    af.bitvector          = 0;
    af.modifier           = 0;
    af.location           = APPLY_NONE;
    affect_to_char(demon, &af);  
    SET_BIT(demon->affected_by, AFF_CHARM);
    add_follower(demon,ch);
    demon->leader = ch;
}

/* if in forest mountain field, vegetaion lags the person for 2 ticks. */
void spell_verdigar_tat( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;

  int sector = ch->in_room->sector_type;
  /* Viri: Give same funcionatliy to all calsses
     with added bonus of pet -hide for rangers */

  if (ch->class == class_lookup("ranger"))
    return;

  if (sector != SECT_FOREST && sector != SECT_FIELD 
      && sector != SECT_HILLS && sector != SECT_MOUNTAIN){
    act("Few tiny roots sprout from the soil then wither on the hard ground.\n\r", ch, NULL, NULL, TO_ALL);
    return;
  }

  act("Roots and plants lash at you binding you to the ground!", victim, NULL, NULL, TO_CHAR);
  act("Roots and plants lash at $n binding $m to the ground!", victim, NULL, NULL, TO_ROOM);

  if (IS_AFFECTED(victim, AFF_FLYING) && !is_affected(victim, gsn_thrash)){
    AFFECT_DATA af;
    af.where	= TO_AFFECTS;
    af.type   = gsn_thrash;
    af.level  = ch->level;
    af.duration = 0;
    af.modifier = 0;
    af.location = 0;
    af.bitvector = 0;
    affect_to_char(victim,&af);
    act("You can't seem to get back into the air",victim,NULL,NULL,TO_CHAR);
    act("$n can't seem to get back into the air",victim,NULL,NULL,TO_ROOM);

    WAIT_STATE2(victim, skill_table[sn].beats);
  }
}

/* Eirik tattoo spell, makes person always visible to the one who marked them */
void spell_mark_prey( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af, *paf;

    if (is_affected(victim, gsn_mark_prey))
      return;
    if ( saves_spell( level, victim,DAM_OTHER,skill_table[sn].spell_type) ){
      return;
    }
    af.where     = TO_AFFECTS;
    af.type      = gsn_mark_prey;
    af.level	 = level;
    af.duration  = number_range(4, 6);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    paf = affect_to_char( victim, &af );
    string_to_affect(paf, ch->name);
    act_new( "You sense you have been marked somehow.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n has been marked by $G's power.", victim, NULL, ch, TO_ROOM );
}

/* Tattoo spell for Sirantnot usable otherwise */
void spell_swords_to_plows( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  /* 
     spell takes a difference between the caster's and victims rares
     if the difference is positive it gets a chance based on quadratic
     increase on the difference, and if chance roll paces a single rare
     on the victim gets turned into a relativly worthless item.

     Description of the worthless item is  based on item type.
  */
  const int max_rare = 25;

  CHAR_DATA* victim = (CHAR_DATA*) vo;
  OBJ_DATA* obj;
  OBJ_DATA* rares[max_rare];

  int chance = 0;
  int ch_rare = 0;
  int ch_worn = 0;
  int vch_rare = 0;
  int i = 0;



  if (victim == NULL || IS_NPC(victim)
      || IS_IMMORTAL(victim))
    return;

  /* check if the ch is not near naked to prevent abuse */
  for (i = 0; i < sizeof(eq_positions)/sizeof(eq_positions[0]); i++){
    if ( ( obj = get_eq_char( ch, eq_positions[i] ) ) != NULL )
      ch_worn ++;
  }
  if (ch_worn < (2 * (sizeof(eq_positions)/sizeof(eq_positions[0])) / 3)
      && !IS_IMMORTAL(ch))
    return;

  /* count rares on ch */
  for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ){
    if (IS_LIMITED(obj) && !IS_SET(obj->wear_flags, ITEM_HAS_OWNER))
      ch_rare ++;
  }
  /* count rares on victim and prep them for selection */
  for ( obj = victim->carrying; obj != NULL; obj = obj->next_content ){
    if (IS_LIMITED(obj) && !IS_SET(obj->wear_flags, ITEM_HAS_OWNER)){
      rares[++vch_rare] = obj;
    }
  }
  /* get the chance */
  chance = (vch_rare - ch_rare) * (vch_rare - ch_rare) / 2;
  chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);

  if (number_percent() > chance)
    return;

  /* get a random rare to convert */
  if (vch_rare)
    obj = rares[number_range(1, vch_rare)];
  else
    return;

  act("Before your eyes $p shimmers and changes.", victim, obj, NULL, TO_ALL);

  /* now we start converting it */
  {
    AFFECT_DATA* paf;

    obj->enchanted = TRUE;
    for (paf = obj->affected; paf != NULL; paf = paf->next)
      free_affect(paf);
    /* get rid of rare etc. */
    REMOVE_BIT(obj->wear_flags, ITEM_RARE);
    REMOVE_BIT(obj->wear_flags, ITEM_UNIQUE);
    /* extras*/
    obj->extra_flags = 0;

    /* reset the values */
    if (obj->item_type != ITEM_SCROLL
	&& obj->item_type != ITEM_STAFF
	&& obj->item_type != ITEM_POTION){
      obj->value[1] = 5;
      obj->value[2] = 5;
    } 
    /* set name short and long desc. */
    {
      char* name = item_name(obj->item_type);
      char shield[] = "shield";
      char buf[MIL];
      
      if (obj->item_type  == ITEM_WEAPON)
	name = weapon_name(obj->value[0]);
      else if (obj->item_type  == ITEM_ARMOR){
	if (CAN_WEAR(obj, ITEM_WEAR_SHIELD))
	  name = shield;
      }

      /* name */
      sprintf(buf, "copper %s", name);
      free_string(obj->name);
      obj->name = str_dup(buf);
      
      /* short */
      sprintf(buf, "a copper %s", name);
      free_string(obj->short_descr);
      obj->short_descr = str_dup(buf);
      
      /* long */
      sprintf(buf, "A copper %s is here", name);
      free_string(obj->description);
      obj->description = str_dup(buf);
      
      /* material */
      sprintf(buf, "copper");
      free_string(obj->material);
      obj->material = str_dup(buf);
    }
  }
}






