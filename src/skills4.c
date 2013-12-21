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
 * and -Mirlan-: The Age of Mirlan Mud by Virigoth                        *
 *                                                                         *
 * Continued Production of this code is available at www.flcodebase.com    *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "interp.h"

extern void cast_new( CHAR_DATA *ch, char *argument, bool iscommune, bool fIgnorePos );
void do_tune( CHAR_DATA *ch, char *argument)
{
    char arg[MIL], arg2[MIL];
    OBJ_DATA *obj;
    AFFECT_DATA *paf;
    int chance, loc_var, result, fail = 25, bonus = 0, stat_chg, added, level = ch->level, sn = skill_lookup("tune");
    bool stop = FALSE, old_found = FALSE;
    argument = one_argument( argument, arg);
    argument = one_argument( argument, arg2);
    if (ch->fighting != NULL)
    {
        send_to_char("Shouldn't you be concentrating on the battle instead?\n\r",ch);
        return;
    }
    if (arg[0] == '\0' || (chance = get_skill(ch,sn)) == 0)
    {
        send_to_char("You put your lips together and whistle a tune.\n\r",ch);
        return;
    }
    if (ch->mana < 100)
    {
        send_to_char("You feel too weak to forge right now.\n\r", ch);
        return;
    }
    if ((obj = get_obj_list(ch,arg,ch->carrying)) == NULL || CAN_WEAR(obj,ITEM_WEAR_TATTOO))
    {
        send_to_char("You don't have that instrument.\n\r",ch);
        return;
    }
    if (obj->wear_loc != -1)
    {
        send_to_char("You must carry the instrument you wish to tune.\n\r",ch);
        return;
    }
    if (obj->item_type != ITEM_INSTRUMENT)
    {
        send_to_char("How do you expect to tune that?\n\r",ch);
	  return;
    }
    WAIT_STATE(ch, skill_table[skill_lookup("tune")].beats);
    if (!IS_IMMORTAL(ch))
    	ch->mana -= 100;
    if (IS_IMMORTAL(ch) && arg2[0] != '\0')
    {
	  if (!is_number(arg2))
	  {
	      send_to_char("Must be numeric.\n\r",ch);
	      return;
	  }
        stat_chg = atoi(arg2);
        if( (stat_chg <= 0) || (stat_chg > 5) )
        {
            send_to_char("Must be numeric 1 to 5.\n\r",ch);
            return;
        }
    }
    else     //checks what stat it'll change
        stat_chg = number_range(1, 5);
    if (!obj->enchanted)
    {
        for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
        {
            if ( paf->location == APPLY_HIT || paf->location == APPLY_MANA )
            {
                bonus = paf->modifier;
		if (bonus > 24)
		    stop = TRUE;
                fail += 2 * (bonus * bonus)/25;
            }
            else if (paf->location == APPLY_DAMROLL || paf->location == APPLY_HITROLL)
            {
                bonus = paf->modifier;
		if (bonus > 4)
		    stop = TRUE;
                fail += 3 * (bonus * bonus);
            }
            else if (paf->location > APPLY_STR && paf->location < APPLY_CON )
            {
                bonus = paf->modifier;
		if (bonus > 4)
		    stop = TRUE;
                fail += 2 * (bonus * bonus);
            }
            else
                fail += 25;
        }
    }
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
        if ( paf->location == APPLY_HIT || paf->location == APPLY_MANA )
        {
            bonus = paf->modifier;
	    if (bonus > 24)
		stop = TRUE;
            fail += 2 * (bonus * bonus)/25;
        }
        else if (paf->location == APPLY_DAMROLL || paf->location == APPLY_HITROLL)
        {
            bonus = paf->modifier;
	    if (bonus > 4)
		stop = TRUE;
            fail += 3 * (bonus * bonus);
        }
        else if (paf->location > APPLY_STR && paf->location < APPLY_CON )
        {
            bonus = paf->modifier;
	    if (bonus > 4)
	    	stop = TRUE;
            fail += 2 * (bonus * bonus);
        }
        else
            fail += 25;
    }
    fail -= level;
    if (obj->level > level)
	fail += obj->level - level;
    if (IS_OBJ_STAT(obj,ITEM_BLESS))
        fail -= 15;
    if (IS_OBJ_STAT(obj,ITEM_GLOW))
        fail += 5;
    fail = URANGE(5,fail,95);
    result = number_percent();
    if (result < (fail / 5) || chance < fail/4)
    {
        act("$p is ruined from excessive tuning!",ch,obj,NULL,TO_ALL);
        extract_obj(obj);
        return;
    }
    if (result < (fail / 2) || chance < fail/2 || stop)
    {
        AFFECT_DATA *paf_next;
        act("$p goes completely out of tune.",ch,obj,NULL,TO_ALL);
        obj->enchanted = TRUE;
        for (paf = obj->affected; paf != NULL; paf = paf_next)
        {
            paf_next = paf->next;
            free_affect(paf);
        }
        obj->affected = NULL;
        REMOVE_BIT(obj->extra_flags,ITEM_GLOW);
        REMOVE_BIT(obj->extra_flags,ITEM_HUM);
        REMOVE_BIT(obj->extra_flags,ITEM_MAGIC);
        return;
    }
    if ( result <= fail || result > chance)
    {
        send_to_char("Nothing seemed to happen.\n\r",ch);
        return;
    }
    act("$p is more finely tuned.",ch,obj,NULL,TO_ALL);
    SET_BIT(obj->extra_flags, ITEM_MAGIC);
    if (obj->level < LEVEL_HERO - 1)
        obj->level = UMIN(LEVEL_HERO - 1,obj->level + 1);
    if (!obj->enchanted)
    {
	AFFECT_DATA *af_new;
	obj->enchanted = TRUE;
	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
	{
	    af_new = new_affect();
	    af_new->next = obj->affected;
	    obj->affected = af_new;
	    af_new->where	= paf->where;
	    af_new->type 	= UMAX(0,paf->type);
	    af_new->level	= paf->level;
	    af_new->duration	= paf->duration;
	    af_new->location	= paf->location;
	    af_new->modifier	= paf->modifier;
	    af_new->bitvector	= paf->bitvector;
	}
    }
//*************Blah blah blah
    switch (stat_chg)
    {
            // 1 = int, 2 = wis, 3 = dex, 4 = hp, 5 = mana
            case (1):
 	        loc_var		= APPLY_INT;
    		added 		= 1;
               	break;
            case (2):
	        loc_var		= APPLY_WIS;
    		added 		= 1;
                break;
            case (3):
	        loc_var		= APPLY_DEX;
    		added 		= 1;
                break;
            case (4):
	        loc_var		= APPLY_HIT;
    		added 		= 5;
                break;
            case (5):
	        loc_var		= APPLY_MANA;
    		added 		= 5;
                break;
            default:
	       send_to_char("Something is admist, notify an IMP!\n\r",ch);
	       return;
    }
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if (paf->location == loc_var)
	{
	    paf->modifier += added;
	    old_found = TRUE;
	}
    }
//adding stats in
    if (!old_found)
    {
	paf = new_affect();
	paf->where	= TO_OBJECT;
	paf->type	= sn;
	paf->level	= level;
	paf->duration	= -1;
	paf->location	= loc_var;
	paf->modifier	= added;
	paf->bitvector  = 0;
    	paf->next	= obj->affected;
    	obj->affected	= paf;
    }
    check_improve(ch,sn,TRUE,1);
}

void do_ventriloquate( CHAR_DATA *ch, char *argument)
{
    char buf1[MSL], buf2[MSL], speaker[MIL];
    CHAR_DATA *vch;
    int chance, sn = skill_lookup("ventriloquate");
    argument = one_argument( argument, speaker );
    if ( (chance = get_skill(ch,sn)) == 0)
    {
        send_to_char("You don't know how to throw your voice.\n\r",ch);
        return;
    }
    if (ch->mana < 5)
    {
        send_to_char("Rest up first.\n\r", ch);
        return;
    }
    if (speaker[0] == '\0' || argument[0] == '\0')
    {
        send_to_char("Syntax: ventriloquate <speaker> <message>\n\r",ch);
        return;
    }
    ch->mana -= 5;
    WAIT_STATE(ch, skill_table[sn].beats);
    if (chance < number_percent())
    {
	send_to_char("You failed.\n\r",ch);
	return;
    }
    sprintf( buf1, "%s says '`#%s``'\n\r", speaker, argument );
    sprintf( buf2, "Someone makes %s say '`#%s``'\n\r", speaker, argument );
    buf1[0] = UPPER(buf1[0]);
    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
        if (!is_exact_name( speaker, vch->name) && IS_AWAKE(vch))
            send_to_char( saves_spell(ch->level,vch,DAM_OTHER,skill_table[sn].spell_type) ? buf2 : buf1, vch );
}

void do_feed( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MIL], arg2[MIL];
    OBJ_DATA *obj;
    argument = one_argument( argument, arg);
    argument = one_argument( argument, arg2);
    if (arg[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char("Feed who what?\n\r",ch);
        return;
    }
    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
    if ((obj = get_obj_list(ch,arg2,ch->carrying)) == NULL )
    {
        send_to_char("You don't have that item.\n\r",ch);
        return;
    }
    if (obj->wear_loc != -1)
    {
        send_to_char("You must carry the item you wish to feed.\n\r",ch);
        return;
    }
    if (obj->item_type != ITEM_FOOD
	&& obj->item_type != ITEM_DRINK_CON)
    {
        send_to_char("You can't feed someone with that.\n\r",ch);
	  return;
    }
    if (victim->master != ch && !is_same_group(ch,victim))
    {
	act("$N	isn't following you.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (!IS_AWAKE(victim))
    {
	act("$N is sleeping at the moment.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (!can_see_obj( victim, obj ) )
    {
        sendf(ch, "%s can't see it.\n\r", PERS(victim,ch));
        return;
    }
    if ( obj->value[3] != 0 ){
      send_to_char("You cannot feed someone with dangerous goods.\n\r", ch);
      return;
    }
/*    if (!IS_NPC(victim))
    {
	act("$N can do that $Mself.",ch,NULL,victim,TO_CHAR);
	return;
    } */
    if (IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_PHOENIX && victim->practice > 20)
    {
	act("$N isn't hungry right now.",ch,NULL,victim,TO_CHAR);
	return;
    }
    WAIT_STATE2(ch, PULSE_VIOLENCE / 2);
    act("$n feeds $N with $p.",ch,obj,victim,TO_NOTVICT);
    act("$n feeds you with $p.",ch,obj,victim,TO_VICT);
    act("You feed $N with $p.",ch,obj,victim,TO_CHAR);
    /* obj_to_ch unlike obj_to_char omits can_Carry checks so the item will always get moved */
    obj_from_char(obj);
    obj_to_ch(obj,victim);

    if (obj->item_type == ITEM_DRINK_CON){
      do_drink(victim, obj->name);

      /* obj_to_ch omits can_Carry checks so the item will always get moved */
      obj_from_char(obj);
      obj_to_ch(obj,ch);
    }
    else if (obj->item_type == ITEM_FOOD)
      do_eat(victim,obj->name);
}

void do_ignite( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    int value;
    if (!IS_NPC(ch))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (ch->pIndexData->vnum != MOB_VNUM_PHOENIX)
	return;
    if ((victim = ch->master) == NULL)
	return;
    if (victim->level < ch->level/2)
    {
	act("$n tries to ignite but is too inexperienced.",ch,NULL,NULL,TO_ROOM);
	return;
    }
    if (ch->hit < ch->max_hit/2)
    {
	act("$n tries to ignite but looks too hurt.",ch,NULL,NULL,TO_ROOM);
	return;
    }
    act("$n ignites into a ball of energy, and combines with $N!",ch,NULL,victim,TO_NOTVICT);
    act("$n ignites into a ball of energy, and combines with you!",ch,NULL,victim,TO_VICT);
    value = ((ch->level*ch->hit)/4)/victim->level;
    victim->hit = UMIN(victim->max_hit, victim->hit + value);
    update_pos(victim);
    char_from_room(ch);
    extract_char(ch,TRUE);
}

/* checks for dreamprobe link between the two chars */
bool dprobe_check(CHAR_DATA* ch, CHAR_DATA* victim){
  AFFECT_DATA* dprobe;

  if (ch == NULL || victim == NULL)
    return FALSE;
  else if (ch->in_room == victim->in_room)
    return FALSE;
  else if ( (dprobe = affect_find(ch->affected, gsn_dreamprobe)) != NULL
	    && dprobe->has_string
	    && !IS_NULLSTR(dprobe->string)
	    && !str_cmp(dprobe->string, victim->name))
    return TRUE;
  else
    return FALSE;
}

/* psi dreamwalk ability */
void do_dreamwalk( CHAR_DATA *ch, char *argument){
  CHAR_DATA* victim;
  AFFECT_DATA* paf;
  char buf[MIL];
  const int cost = 15;
  int sn = 0;

  const int gsn_omen		= skill_lookup("omen");
  const int gsn_deathmare	= skill_lookup("deathmare");
  const int gsn_dreamprobe	= skill_lookup("dreamprobe");
  const int gsn_mindmelt	= skill_lookup("mindmelt");

  if (get_skill(ch, gsn_telepathy) < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument)){
    send_to_char("Dreamwalk in who's dream?\n\r", ch);
    return;
  }

/* VICTIM CHOICE */
  if ( (victim = get_char_world(ch, argument)) != NULL
       && (ch->in_room == victim->in_room || dprobe_check(ch, victim))
       ){
    AFFECT_DATA af;
    if (is_safe(ch, victim)){
      return;
    }
    else if (IS_AWAKE(victim)){
      send_to_char("They must be sleeping first.\n\r", ch);
      return;
    }
    else if (IS_NPC(victim) || IS_UNDEAD(victim) || IS_DEMON(victim)){
      send_to_char("Try as you might you do not sense any dreams.\n\r", ch);
      return;
    }
    else if (ch->in_room != victim->in_room
	     && victim
	     && !dprobe_check(ch, victim)){
      send_to_char("They aren't here.\n\r", ch );
      return;
    }
    affect_strip(ch, gen_dreamwalk );
    act("You tune your senses into $N's dream.", ch, NULL, victim, TO_CHAR);
    act("$n touches $N's head and chants softly.", ch, NULL, victim, TO_ROOM);

    af.type	= gen_dreamwalk;
    af.level	= 0; //used to keep track of the messages
    af.duration	= 4;
    af.where	= TO_AFFECTS;
    af.bitvector = 0;
    af.location	= APPLY_NONE;
    af.modifier = 0;//sn of the spell to cast
    paf = affect_to_char(ch, &af);
    string_to_affect(paf, victim->name );
    return;
  }
  /* SPELL CHOICE */
  sn = find_spell(ch,argument);
  if (sn != gsn_omen
      && sn != gsn_deathmare
      && sn != gsn_mindmelt
      && sn != gsn_dreamprobe){
    send_to_char("You can only dreamwalk omen, deathmare, dreamprobe and mind melt.\n\r", ch);
    return;
  }
  else if ( (paf = affect_find(ch->affected, gen_dreamwalk)) == NULL
	    || !paf->has_string
	    || IS_NULLSTR(paf->string)){
    send_to_char("You must enter someone's dream first.\n\r", ch);
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You cannot concentrate sufficiently.\n\r", ch);
    return;
  }
  WAIT_STATE(ch, 2 * PULSE_VIOLENCE );
  if (number_percent() < get_skill(ch, gsn_telepathy)){
    ch->mana -= cost;
    check_improve(ch, gsn_telepathy, TRUE, 1);
  }
  else{
    ch->mana -= cost / 2;
    check_improve(ch, gsn_telepathy, FALSE, 1);
    send_to_char("You failed to enter their dreams.\n\r", ch );
    return;
  }

  /* compose the spell target */
  sprintf(buf, "\'%s\' %s", skill_table[sn].name, paf->string);

  /* cast the spell */
  a_yell( ch, victim );
  cast_new( ch, buf, FALSE, TRUE );
}

/* wrap around telepathic tell */
void do_telepathy( CHAR_DATA* ch, char* argument ){
  bool fTele = IS_TELEPATH( ch );

  if (get_skill( ch, gsn_telepathy) < 2){
    send_to_char("Huh?\n\r", ch );
    return;
  }

  if (!fTele)
    SET_BIT(ch->comm, COMM_TELEPATH);
  do_tell( ch, argument );
  if (!fTele)
    REMOVE_BIT(ch->comm, COMM_TELEPATH);
}


//shows tracks in the room
void show_tracks( CHAR_DATA* ch, ROOM_INDEX_DATA* in_room ){
  AFFECT_DATA* paf;
  char* age_str;
  long age;

  if (ch->class != gcn_ranger)
    return;
  else if ( (paf = affect_find(ch->affected, gsn_tracker)) == NULL)
    return;
  else if (paf->modifier < 1 || paf->modifier >= MAX_ANATOMY)
    return;

  age = (mud_data.current_time - in_room->tracks[paf->modifier]) / 30;

  switch( age ){
  case -1:
  case 0:	age_str = "new";	break;
  case 1:	age_str = "fresh";	break;
  case 2:	age_str = "recent";	break;
  case 3:
  case 4:	age_str = "old";	break;
  default:
    return;
  }
  sendf(ch, "You spot some %s spur.\n\r", age_str );
}

/* new ranger skills */
void do_track( CHAR_DATA* ch, char* argument ){
  AFFECT_DATA af, *paf;

  const int lag = skill_table[gsn_tracker].beats;
  const int cost = skill_table[gsn_tracker].min_mana;

  int skill = get_skill(ch, gsn_tracker );
  int anat;

  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument)){
    send_to_char("Track footprints of which type?\n\r", ch);
    return;
  }
  else if ( (anat = anatomy_lookup( argument )) < 1){
    int i;
    send_to_char("You may track:\n\r", ch );

    for (i = 1; i < MAX_ANATOMY && !IS_NULLSTR(anatomy_table[i].name); i++){
      sendf(ch, "%s\n\r", anatomy_table[i].name );
    }
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You cannot remember the spur.\n\r", ch);
    return;
  }

  ch->mana -= cost;
  WAIT_STATE(ch, lag );

  if (number_percent() > skill){
    send_to_char("You failed to recall the shape of the footprint.\n\r", ch);
    check_improve(ch, gsn_tracker, FALSE, 5);
    return;
  }
  check_improve(ch, gsn_tracker, TRUE, 1);

  if ( (paf = affect_find( ch->affected, gsn_tracker )) != NULL){
    paf->modifier = anat;
    paf->duration = number_fuzzy( ch->level / 6 );
  }
  else{
    af.type	= gsn_tracker;
    af.level	= ch->level;
    af.duration = number_fuzzy( ch->level / 6 );
    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location = APPLY_NONE;
    af.modifier = anat;
    affect_to_char(ch, &af);
  }
  sendf( ch, "You begin to look for %s spur.\n\r", anatomy_table[anat].name );
}


/* causes any ranger pets to return to him */
void do_pack_call( CHAR_DATA* ch, char* argument ){
  CHAR_DATA* pet;
  AFFECT_DATA af;
  const int gsn_pack_call = skill_lookup("pack call");
  int skill = get_skill(ch, gsn_pack_call );

  const int lag = skill_table[gsn_pack_call].beats;
  const int cost = skill_table[gsn_pack_call].min_mana;
  bool fSet = FALSE;

  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_pack_call)){
    send_to_char("You're not ready to call on your pack yet.\n\r", ch);
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You can't manage to concentrate.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;
  WAIT_STATE2(ch, lag );

  if (number_percent() < 4 * skill / 5){
    send_to_char("You failed to call on your pack.\n\r", ch);
    check_improve(ch, gsn_pack_call, FALSE, 5);
    return;
  }
  else
    check_improve(ch, gsn_pack_call, TRUE, 5);
  send_to_char("You call on your pack.\n\r", ch);
  act("$n calls on $s pack.", ch, NULL, NULL, TO_ROOM);

  //we generate paths for each creature found.
  for (pet = char_list; pet; pet = pet->next){
    if (!IS_NPC(pet) || !IS_AFFECTED(pet, AFF_CHARM) || pet->master == NULL)
      continue;
    else if (pet->pIndexData->vnum != MOB_VNUM_RAVEN
	     && pet->pIndexData->vnum != MOB_VNUM_LEOPARD
	     && pet->pIndexData->vnum != MOB_VNUM_WYVERN
	     && pet->pIndexData->vnum != MOB_VNUM_DISPLACER
	     && pet->pIndexData->vnum != MOB_VNUM_MAMMOTH)
      continue;
    else if (pet->in_room == ch->in_room)
      continue;
    if (set_path( pet, pet->in_room, ch->in_room, 256, NULL) < 1){
      sendf(ch, "%s could not find its way here.\n\r", PERS2(pet));
    }
    else
      fSet = TRUE;
  }
  if (fSet){
    af.type = gsn_pack_call;
    af.level = ch->level;
    af.duration = 12;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_char(ch, &af);
  }
}

/* faster reload in combat at 3/4 damage */
void do_rapid_fire( CHAR_DATA* ch, char* argument ){
  AFFECT_DATA af;
  int skill = get_skill(ch, gsn_rapid_fire );

  const int lag = skill_table[gsn_rapid_fire].beats;
  const int cost = skill_table[gsn_rapid_fire].min_mana;


  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_rapid_fire)){
    send_to_char("You're already prepared to quicky fire in combat.\n\r", ch);
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You can't manage to concentrate.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;
  WAIT_STATE2(ch, lag );

  af.type	= gsn_rapid_fire;
  af.duration	= number_fuzzy(ch->level / 10 );
  af.level	= ch->level;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.modifier	= -1;
  af.location	= APPLY_DEX;
  affect_to_char( ch, &af);
  send_to_char("You will now forsake aim for speed in ranged combat.\n\r", ch);
}
