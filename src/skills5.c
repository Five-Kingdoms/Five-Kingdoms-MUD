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
#include <stdlib.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "interp.h"
#include "cabal.h"

extern  const   struct  flag_type       fletcher_type[];
extern int rounds_per_projectile( int projectile );

/* creates launcher's ammo for unlimited weapons */
void  check_unlimited_ammo( CHAR_DATA*  ch, OBJ_DATA* launcher ){
  OBJ_DATA* ammo;
  OBJ_INDEX_DATA* index;

  if (!IS_WEAPON_STAT(launcher, RANGED_UNLIMITED))
    return;
  //has to have custom ammo
  else if (launcher->value[3] < 1)
    return;
  else if ( (index = get_obj_index( launcher->value[3])) == NULL)
    return;

  //remove any unappropriate ammo
  if ( (ammo = get_eq_char(ch, WEAR_QUIVER)) != NULL){
    if (ammo->vnum != launcher->value[3] && !IS_SET(ammo->extra_flags, ITEM_NOREMOVE)){
      unequip_char( ch, ammo );
      ammo = create_object( index, ch->level );
      equip_char(ch, ammo, WEAR_QUIVER );
    }
    else if (ammo->vnum == launcher->value[3])
      ammo->condition = 999;
    else
      return;
  }
  else{
    ammo = create_object( index, ch->level );
    obj_to_char( ammo, ch );
    equip_char(ch, ammo, WEAR_QUIVER );
    ammo->condition = 999;
  }
}

/* returns the damage done by the given weapon and projectile when shot
   reps times.
*/
int get_proj_damage( CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* launcher, OBJ_DATA* ammo, int reps, int *hits, int *h_roll, int skill, bool fArchery){
  AFFECT_DATA* paf;
//data
  const int level = ch->level;

  int thac0 = 0;
  int ac = 0;
  int dam = 0;
  int dam_type = DAM_ENERGY;
  int dt = attack_lookup("slice") + TYPE_HIT;

  int hroll = 0, droll = 0;
  const int hit_mod = 5;	//multipliers for the hit/dam found on arrows
  const int dam_mod = 1;

  int hit = 0;
  int diceroll = 0;

  int i = 0;


  /* safety */
  if (launcher == NULL || ammo == NULL){
    bug("get_proj_damage: NULL launcher or ammo.", 0 );
    return 0;
  }

  /* now get HIT/DAMROLL from projectile */
  paf = ammo->pIndexData->affected;

  /* sum up hitroll on container */
  for (; paf != NULL; paf = paf->next){
    if (paf->location == APPLY_HITROLL)
      hroll += paf->modifier;
    else if (paf->location == APPLY_DAMROLL)
      droll += paf->modifier;
  }

  /* multiply hitroll/damroll by the multipliers */
  hroll *= hit_mod;
  droll *= dam_mod;

/* return the hitroll to be used outside */
  hroll += GET_HITROLL(ch);
  *h_roll = hroll;

/* get damage type etc. */
  dt = ammo->value[3] + TYPE_HIT;
  dam_type = attack_table[ammo->value[3]].damage;

  //get thaco and AC
  thac0 = get_THAC0(ch, victim, NULL, dt, FALSE, IS_NPC(ch), 0, TRUE,
		    gsn_fired, level) / 2;
  ac = get_AC(ch, victim, dt, dam_type, TRUE);

  if (ac < 0 && IS_WEAPON_STAT(launcher, RANGED_ARMORPIERCE))
    ac = 0;

/* debug
  sendf(ch, "hroll: %d, droll: %d thac0: %d, adj. thaco0: %d ac: %d, thac0-ac: %d\n\r",
	hroll, d_roll, thac0, thac0 - hroll, ac, thac0-ac-hroll);
*/

  /* effect thaco0 by hroll */
  thac0 -= hroll;

  /* loop that repeats REPS times */
  for (i = 0; i < reps; i ++){
    int chance = 0;

    //We roll for miss
    while ( ( diceroll = number_bits( 5 ) ) >= 20 );
    if ( diceroll == 0 || ( diceroll != 19 && diceroll < thac0 - ac ) )
      continue;

    /* check other factors */
    chance = 4 * skill / 5;
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += (ch->level - victim->level) * 2;
    chance += (get_curr_stat(ch,STAT_LUCK)-get_curr_stat(victim,STAT_LUCK))* 2;

    chance -= get_skill(victim, gsn_dodge) / 10;
    if (get_eq_char(victim, WEAR_SHIELD) != NULL)
      chance -= get_skill(victim, gsn_shield_block) / 10;

    //accuracy of launcher bonus/penalty
    chance = chance * launcher->value[1] / 100;

    //archery bonus
    if (fArchery && hroll > 0)
      chance += hroll / 3;

    chance = URANGE(5, chance, 95);
/* DEBUG
    sendf(ch, "chance: %d\n\r", chance);
*/
    if (number_percent() > chance)
      continue;
    dam += URANGE(1, dice(ammo->value[1], ammo->value[2]), 75) + droll;
    hit ++;
  }

  /* pass back amount of hits */
  *hits = hit;
  /* return damage done */
  return dam;
}




void proj_spec_damage(CHAR_DATA* ch, CHAR_DATA* victim, int level, int dam, int spec, char* msg){
  AFFECT_DATA* paf, af;

  if (ch == NULL || ch->in_room == NULL
      || victim == NULL || victim->in_room == NULL)
    return;

/* EXPLODE (AREA DAMAGE) */
  if ( IS_SET(spec, PROJ_SPEC_EXPLODE)){
    CHAR_DATA* vch, *vch_next = NULL;
    char buf[MIL];
    int dam_spec = dam;
    int dt = attack_lookup("blast");
    int dam_type = attack_table[dt].damage;

    /* Useless conditional */
    if( vch_next != NULL )
        vch_next = NULL;

    dt += TYPE_NOBLOCKHIT;
    for(vch = victim->in_room->people; vch; vch = vch->next){
      vch_next = vch->next_in_room;
      if (vch == victim || vch == ch || !is_same_group(vch, victim))
	continue;
      if (is_area_safe(ch, vch))
	continue;
      if ( ch->fighting != vch && vch->fighting != ch){
	a_yell(ch,vch);
	sprintf(buf, "Help! %s just shot at me!",PERS(ch,vch));
	j_yell(vch,buf);
      }
      virtual_damage(ch, vch, NULL, dam_spec, dt, dam_type, 0, level, TRUE, TRUE, gsn_fired);
    }
    if (ch->fighting == NULL)
      return;
  }
/* FLAMES */
  if ( IS_SET(spec, PROJ_SPEC_FLAMING)){
    int dam_spec = number_range(1, level / 3 + 5);
    int dt = attack_lookup("flame");
    int dam_type = attack_table[dt].damage;
    dt += TYPE_NOBLOCKHIT;

    act("$n's flesh is seared by $t's flames.",victim, msg, NULL,TO_ROOM);
    act("Your flesh is seared with $t's flames.",victim, msg, NULL,TO_CHAR);

    fire_effect(victim, level, dam, TARGET_CHAR);
    if (ch->fighting == NULL)
      return;
    virtual_damage(ch, victim, NULL, dam_spec, dt, dam_type, 0, level, TRUE, TRUE, gsn_fired);
    if (ch->fighting == NULL)
      return;
  }

/* BARBED (DEADLY) */
  if (IS_SET(spec, PROJ_SPEC_BARBED) && !IS_UNDEAD(victim)){
    int dt = attack_lookup("brarrow");
    int dam_type = attack_table[dt].damage;
    dt += TYPE_NOBLOCKHIT;

    act("$t embeds itself into $n's flesh!",victim, msg,NULL,TO_ROOM);
    act("$t embeds itself into your flesh!",victim, msg,NULL,TO_CHAR);

    if (!saves_spell(3 * level / 4 + dam / 15, victim, dam_type, SPELL_AFFLICTIVE)){
      if ( (paf = affect_find(victim->affected, gen_bleed)) == NULL){
	af.type = gen_bleed;
	af.level = ch->level;
	af.duration = number_range(2, 3);
	af.where = TO_NONE;
	af.bitvector = 0;
	af.location = APPLY_NONE;
	/* modifier controls duration of damage (10/tick)*/
	af.modifier = number_range(8, 15);
	paf = affect_to_char(victim, &af);
	string_to_affect(paf, ch->name);
      }
      else{
	/* modifier controls duration of damage (10/tick)*/
	paf->modifier = number_range(10, 25);
	string_to_affect(paf, ch->name);
      }
    }
  }
/* SHARP */
  if ( IS_SET(spec, PROJ_SPEC_SHARP)){
    int dam_spec = number_range(1, level / 3 + 5);
    int dt = attack_lookup("pain");
    int dam_type = attack_table[dt].damage;
    dt += TYPE_NOBLOCKHIT;

    act("$t digs into $n's flesh.",victim, msg,NULL,TO_ROOM);
    act("$t digs into your flesh.",victim, msg,NULL,TO_CHAR);

    virtual_damage(ch, victim, NULL, dam_spec, dt, dam_type, 0, level, TRUE, TRUE, gsn_fired);
    if (ch->fighting == NULL)
      return;
  }
/* BLUNT */
  if ( IS_SET(spec, PROJ_SPEC_BLUNT)){

    wound_effect( ch, victim, level, 0);
    if (ch->fighting == NULL)
      return;
  }
  /* FROST */
  if ( IS_SET(spec, PROJ_SPEC_FROST)){
    int dam_spec = number_range(1, level / 3 + 5);
    int dt = attack_lookup("frarrow");
    int dam_type = attack_table[dt].damage;
    dt += TYPE_NOBLOCKHIT;

    cold_effect(victim, level,dam,TARGET_CHAR);
    if (ch->fighting == NULL)
      return;
    virtual_damage(ch, victim, NULL, dam_spec, dt, dam_type, 0, level, TRUE, TRUE, gsn_fired);
    if (ch->fighting == NULL)
      return;
  }
  /* SHOCKING */
  if ( IS_SET(spec, PROJ_SPEC_SHOCK)){
    int dam_spec = number_range(1, level / 3 + 5);
    int dt = attack_lookup("sharrow");
    int dam_type = attack_table[dt].damage;
    dt += TYPE_NOBLOCKHIT;

    shock_effect(victim, level, dam,TARGET_CHAR);
    if (ch->fighting == NULL)
      return;
    virtual_damage(ch, victim, NULL, dam_spec, dt, dam_type, 0, level, TRUE, TRUE, gsn_fired);
    if (ch->fighting == NULL)
      return;
  }
  /* PARALYZE */
  if ( IS_SET(spec, PROJ_SPEC_PARALYZE)){
    paralyze_effect(victim, level, dam, TARGET_CHAR);
  }
  /* SLEEP */
  if ( IS_SET(spec, PROJ_SPEC_SLEEP)){
    if (!saves_spell(UMIN(ch->level, level), victim, DAM_MENTAL,
		     skill_table[gsn_sleep].spell_type)
	&& !IS_AFFECTED(victim,AFF_SLEEP)
	&& !IS_UNDEAD(victim)){
      act_new("You feel the sleep serum course through your veins.",ch, msg,victim,TO_VICT, POS_DEAD);
      act("The sleep serum on $t causes $n to fall alseep.",victim, msg,NULL,TO_ROOM);
      af.type      = gsn_sleep;
      af.level     = level;
      af.duration  = number_range(0, 2);
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.where     = TO_AFFECTS;
      af.bitvector = AFF_SLEEP;
      paf = affect_to_char( victim, &af );
      stop_fighting( victim, TRUE );
      victim->position = POS_SLEEPING;
    }
  }
/* POISON */
  if ( IS_SET(spec, PROJ_SPEC_POISON)){
    if (!saves_spell(level, victim, DAM_POISON, skill_table[gsn_poison].spell_type) && !IS_AFFECTED(victim,AFF_POISON)){
      act_new("You feel poison coursing through your veins.",ch, msg,victim,TO_VICT,POS_DEAD);
      act("$n is poisoned by the venom on $t.",victim, msg,NULL,TO_ROOM);
      af.where     = TO_AFFECTS;
      af.type      = gsn_poison;
      af.level     = level * 3/4;
      af.duration  = level / 10;
      af.location  = APPLY_STR;
      af.modifier  = -1;
      af.bitvector = AFF_POISON;
      paf = affect_to_char( victim, &af );
      if (!IS_NPC(ch)){
	string_to_affect(paf, ch->name);
	/* Set bit preventing damage till first tick. */
	SET_BIT(paf->bitvector, AFF_FLAG);
      }
    }
  }
/* DISEASE */
  if ( IS_SET(spec, PROJ_SPEC_PLAGUE)){
    if (!saves_spell(level, victim, DAM_DISEASE, skill_table[gsn_plague].spell_type)
	&& !IS_AFFECTED(victim,AFF_PLAGUE)){
      af.where     = TO_AFFECTS;
      af.type 	 = gsn_plague;
      af.level	 = 3 * level / 4;
      af.duration  = level / 5;
      af.location  = APPLY_STR;
      af.modifier  = -(level/10);
      af.bitvector = AFF_PLAGUE;
      affect_to_char(victim,&af);

      /* now damage counter */
      af.where     = TO_NONE;
      af.type 	 = gsn_plague;
      af.level	 = level;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = 0;
      paf = affect_to_char(victim,&af);
      if (!IS_NPC(ch))
	string_to_affect(paf, ch->name);

      act_new("You scream in agony as plague sores erupt from your skin.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
      act("$n screams in agony as plague sores erupt from $s skin.",victim,NULL,NULL,TO_ROOM);
    }
  }
/* VORPAL */
  if ( IS_SET(spec, PROJ_SPEC_VORPAL)){
    acid_effect(victim,level,dam,TARGET_CHAR);
    if (ch->fighting == NULL)
      return;
  }
}

/* main shoot function, takes care of firing a projectile at a victim */
bool shoot( CHAR_DATA *ch, CHAR_DATA* victim, int skill, bool fArchery, bool fQuiet ){
  CHAR_DATA* dam_ch = ch, *dam_victim = victim;
  char buf[MSL];
  char short_descr[MSL];
  char ammo_name[MIL];
  char* msg;

  OBJ_DATA *ammo, *launcher;

  int spec = 0; //flag of ammunition for special attacks
  int hit = 0;	//projectiles that hit
  int fire = 0; //howmany times we hit
  int i, h_roll = 0;

  int dt = attack_lookup("pierce");
  int dam_type = DAM_PIERCE;

  int dam = 0;
  int level = 0;

  bool fRapid = FALSE;

  /* Useless conditional */
  if( fRapid != FALSE )
    fRapid = FALSE;

  /* weapon check */
  if ((launcher = get_eq_char( ch, WEAR_RANGED )) == NULL
      || launcher->item_type != ITEM_RANGED ){
    if (!fQuiet)
      send_to_char("You don't have any ranged weapons equipped.\n\r", ch);
    return FALSE;
  }

  /* unlimited weapons make their own ammo */
  check_unlimited_ammo( ch, launcher );

  /* ammo check */
  if ( (ammo = get_eq_char( ch, WEAR_QUIVER )) == NULL){
    if (!fQuiet)
      send_to_char("You don't have any ammunition for your ranged weapon.\n\r", ch);
    return FALSE;
  }
  else if (launcher->value[3] > 0 && launcher->value[3] != ammo->vnum){
    if (!fQuiet)
      send_to_char("You don't have the proper ammunition for your ranged weapon.\n\r", ch);
    return FALSE;
  }
  else if (launcher->value[3] == 0 && !IS_PROJ_TYPE(launcher, ammo->value[0])){
    if (!fQuiet)
      send_to_char("You don't have the proper ammunition for your ranged weapon.\n\r", ch);
    return FALSE;
  }
  else if (ammo->condition < 1){
    if (!fQuiet)
      send_to_char("You're out of ammo.\n\r", ch);
    obj_from_char( ammo );
    extract_obj( ammo );
    return FALSE;
  }

  /* skill penalty for ammo or launcher level above yours */
  if (ammo->level > ch->level || (launcher->level > ch->level && ch->level < 50))
    skill /= 2;
  else if (number_percent() < get_skill(ch, gsn_marksman)){
    check_improve(ch, gsn_marksman, TRUE, 1);
    skill = 13 * skill / 10;
  }

  sprintf( ammo_name, "%s", ammo->name);
  level = ammo->level;
  sprintf(short_descr, "%s", ammo->short_descr);

  /* remeber spec flag */
  spec = ammo->value[4];
  /* remember damage specifications */
  dt = ammo->value[3] + TYPE_NOBLOCKHIT;
  dam_type = attack_table[ammo->value[3]].damage;

  /* force character to reload */
  if (IS_WEAPON_STAT(launcher, RANGED_SLOW))
    ch->reload = 3;
  else
    ch->reload = 2;

  if (is_affected(ch, gsn_rapid_fire) && number_percent() < get_skill(ch, gsn_rapid_fire)){
    check_improve(ch, gsn_rapid_fire, TRUE, 1);

    fRapid = TRUE;
    ch->reload -= 1;
    skill = 3 * skill / 4;
  }

  /* get the times to fire while removing ammo */
  fire = 0;
  for (i = 0; i < launcher->value[2]; i++){
    /* skill check */
    if (number_percent() < skill){
      fire++;
    }
    if (--ammo->condition < 1){
      break;
    }
  }

  /* yell */
  if (!IS_NPC(ch)){
    a_yell(ch,victim);
    if (ch->fighting != victim && victim->fighting != ch){
      char buf[MIL];
      sprintf(buf, "Help! I've just been shot at by %s!",PERS(ch,victim));
      j_yell(victim,buf);
    }
  }

  //racial enemy
  if (!IS_NPC(ch) && ch->pcdata->enemy_race && victim->race == ch->pcdata->enemy_race){
    h_roll += ch->level / 5;
  }


  /* firing messages and dam calcs, returns damage */
  if ( (dam = get_proj_damage(ch, victim, launcher, ammo, fire, &hit, &h_roll, skill, fArchery )) > 0){
    dam = skill * dam / 100;
    dam += GET_DAMROLL(ch);
    //racial enemy
    if (!IS_NPC(ch) && ch->pcdata->enemy_race && victim->race == ch->pcdata->enemy_race){
      dam += ch->level / 5;
    }
  }
  if (is_affected(victim, gsn_missile_shield)){
    dam = 0;
  }
  dam = UMAX(0, dam );

  if (hit > 9)
    msg = "a volley of";
  else if (hit > 7)
    msg = "a hail of";
  else if (hit > 5)
    msg = "a barrage of";
  else if (hit > 3)
    msg = "several";
  else if (hit > 2)
    msg = "a few";
  else if (hit > 1)
    msg = "a couple";
  else if (hit > 0)
    msg = "one";
  else
    msg = "no";

  sprintf(buf, "You fire from $p and hit $N with %s shot%s.", msg,
	  hit == 1 ? "" : "s");
  act(buf, ch, launcher, victim, TO_CHAR);

  /* Disabled to reduce combat spam
  sprintf(buf, "$n fires from $p and hits $N with %s shot%s.", msg,
	  hit == 1 ? "" : "s");
  act(buf, ch, launcher, victim, TO_NOTVICT);
  sprintf(buf, "$n fires from $p and hits you with %s shot%s.", msg,
	  hit == 1 ? "" : "s");
  if (ch->in_room == victim->in_room)
    act(buf, ch, launcher, victim, TO_VICT);
  */
  /* debug
  sendf(ch, "shots taken: %d, shots hit: %d, damage: %d\n\r",fire, hit, dam);
  */

  if (victim->class == gcn_blademaster && is_affected(victim, gsn_iron_curtain)
      && get_eq_char(victim, WEAR_SECONDARY) != NULL){
    act("$n deflects the projectile!", victim, NULL, NULL, TO_ROOM);
    send_to_char("You deflect the projectile!\n\r", victim);
    dam_victim = ch;
    dam_ch = victim;
    dam = 2 * dam / 3;
  }


  if (victim->in_room){
    ROOM_INDEX_DATA* old_room = ch->in_room;
    ch_from_room( ch );
    ch_to_room( ch, victim->in_room );

    /* shoot progs */
    if (dam > 0 && !is_safe_quiet(dam_ch, dam_victim) && dam_victim && dam_victim->in_room && HAS_TRIGGER_OBJ( ammo, TRIG_SHOOT) ){
      p_percent_trigger( NULL, ammo, NULL, dam_victim, NULL, dam_ch, TRIG_SHOOT );
    }
    if (dam > 0 && !is_safe_quiet(dam_ch, dam_victim) && dam_victim && dam_victim->in_room &&  HAS_TRIGGER_OBJ( launcher, TRIG_SHOOT) ){
      p_percent_trigger( NULL, launcher, NULL, dam_victim, NULL, ch, TRIG_SHOOT );
    }
    /* special effects here */
    if (dam > 0 && !is_safe_quiet(dam_ch, dam_victim) && dam_victim && dam_victim->in_room && number_percent() < (hit * ammo->level / UMAX(1, i)) ){
      bool fSleep = IS_AFFECTED(dam_victim, AFF_SLEEP) > 0;
      proj_spec_damage(dam_ch, dam_victim, level, dam, spec, short_descr);
      if (IS_AFFECTED(dam_victim, AFF_SLEEP) && !fSleep)
	dam_victim = NULL;
    }

    /* actual damage */
    if (dam_victim && !is_safe_quiet(dam_ch, dam_victim)  && dam_victim->in_room){
      /* regular damage here */
      //launcher does damage
      virtual_damage(dam_ch, dam_victim, launcher, dam, dt, dam_type, h_roll, ch->level, TRUE, TRUE, 0);

      /* disabled: 'projectile' does damage
	 virtual_damage(ch, victim, NULL, dam, dt, dam_type, h_roll, ch->level, TRUE, TRUE, gsn_fired);
      */
    }

    ch_from_room( ch );
    ch_to_room( ch, old_room );
  }

  /* extract on out of ammo */
  if (ammo->condition < 1){
    act("You've run out ammunition for $p.", ch, launcher, NULL, TO_CHAR );
    obj_from_char( ammo );
    extract_obj( ammo );
  }
  /* unlimited weapons always run out of ammo (silently)*/
  if (IS_WEAPON_STAT(launcher, RANGED_UNLIMITED)){
    obj_from_char( ammo );
    extract_obj( ammo );
  }

  //skill improvement
  if (dam < 1){
    check_improve(ch, gsn_fired, FALSE, 1);
    check_improve(ch, gsn_archery, FALSE, 1);
  }
  else{
    check_improve(ch, gsn_fired, TRUE, 1);
    check_improve(ch, gsn_archery, TRUE, 1);
  }
  return TRUE;
}
//recursivly looks for a victim in rooms that are in straight line of travel from this one
CHAR_DATA* get_shoot_target( CHAR_DATA* ch, ROOM_INDEX_DATA* room, char* name, int max_range, int* range, int force_door, bool fDoors ){
  CHAR_DATA* vch;
  ROOM_INDEX_DATA* old_room;

  int door, max_door;

  if (ch == NULL || room == NULL || IS_NULLSTR( name ))
    return NULL;

  /* we cheat here to make sure we can select the target properly and
     put the character in the room before checking for target.
     This avoids problems with dark rooms, dopplegangers and other
     effects.
  */
  old_room = ch->in_room;

  if (force_door < 0 || *range > 0){
    //low level to room function, only moves ch to room
    ch_from_room( ch );
    ch_to_room( ch, room );

    vch = get_char_room( ch, NULL, name);

    ch_from_room( ch );
    ch_to_room( ch, old_room );

    //easy case
    if (vch)
      return vch;
  }

  //recurse
  if (-- max_range < 0)
    return NULL;
  else
    (*range)++;

  if (force_door >= 0){
    door = force_door;
    max_door = door + 1;
  }
  else{
    door = 0;
    max_door = MAX_DOOR;
  }

  SET_BIT(room->room_flags, ROOM_MARK );

  for (; door < max_door; door++){
    EXIT_DATA* pExit = room->exit[door];

    if (pExit == NULL || pExit->to_room == NULL)
      continue;
    else if (IS_SET(pExit->to_room->room_flags, ROOM_MARK))
      continue;
    else if (IS_SET(pExit->to_room->room_flags2, ROOM_GUILD_ENTRANCE))
      continue;
    else if (IS_SET(pExit->to_room->room_flags2, ROOM_JAILCELL))
      continue;
    else if (IS_SET(pExit->exit_info, EX_CLOSED) && fDoors)
      continue;

    vch = get_shoot_target(ch, pExit->to_room, name, max_range, range, door, fDoors);

    if (vch != NULL){
      REMOVE_BIT(room->room_flags, ROOM_MARK );
      return vch;
    }
  }
  REMOVE_BIT(room->room_flags, ROOM_MARK );
  return NULL;
}


/* shoot <victim>
   shoot <dir> <victim>
*/

void do_shoot( CHAR_DATA* ch, char* argument ){
  CHAR_DATA* victim;
  OBJ_DATA* launcher, *ammo;

  char dir[MIL];

  int lag = skill_table[gsn_fired].beats;
  const int cost = skill_table[gsn_fired].min_mana;

  int skill = get_skill(ch,gsn_fired);
  int archery = get_skill(ch, gsn_archery );
  int marksman = get_skill(ch, gsn_marksman );

  int max_range =  URANGE(1, (archery - 70) / 5 + UMAX(0, (archery - 100)), 10);
  int range = 0, door = -1;

  bool fPassDoor = FALSE;

  if ( (launcher = get_eq_char( ch, WEAR_RANGED)) == NULL){
    send_to_char("You don't have a ranged weapon equipped.\n\r", ch);
    return;
  }

  /* unlimited weapons make their own ammo */
  check_unlimited_ammo( ch, launcher );

  if ( (ammo = get_eq_char( ch, WEAR_QUIVER)) == NULL){
    send_to_char("You don't have any ammunition in our quiver.\n\r", ch);
    return;
  }

  if (ch->fighting){
    if (ch->reload > 0){
      send_to_char("You're still reloading!\n\r", ch);
      return;
    }
    else
      victim = ch->fighting;
  }
  else{
    if (IS_NULLSTR(argument)){
      send_to_char("Shoot whom?\n\r", ch);
      return;
    }

    if (IS_WEAPON_STAT( launcher, RANGED_PASSDOOR) || IS_PROJ_SPEC( ammo, PROJ_SPEC_PASSDOOR))
      fPassDoor = TRUE;

    /* check if they specified direction */
    one_argument( argument, dir );
    if ( (door = dir_lookup( dir )) >= 0){
      argument =  one_argument( argument, dir );
    }

    if ( (victim = get_shoot_target( ch, ch->in_room, argument, max_range, &range, door, fPassDoor)) == NULL){
      sendf( ch, "They are not in your line of sight, or range (%d rooms).\n\r", max_range);
      return;
    }
  }

  if (is_safe(ch, victim))
    return;
  else if (ch->mana < cost){
    send_to_char("You cannot concentrate enough to aim.\n\r",ch);
    return;
  }
  else
    ch->mana -= cost;

  if (IS_WEAPON_STAT( launcher, RANGED_FAST))
    lag /= 2;
  if (is_affected(ch, gsn_rapid_fire))
    lag /= 2;

  lag = UMAX(3, lag );
  WAIT_STATE2( ch, lag );

  if (number_percent() < marksman){
    check_improve(ch, gsn_marksman, TRUE, 1);
  }
  else{
    check_improve(ch, gsn_marksman, FALSE, 5);
    //lower skill based on range
    if (IS_WEAPON_STAT( launcher, RANGED_LONGRANGE))
      skill -= range * 5;
    else
      skill -= range * 10;
  }

  /* abuse safeties */
  if (ch->fighting != victim && IS_NPC(victim) && victim->hit < (6 * victim->max_hit / 10)){
    act("$N seems to expect your attack and easly avoids it.", ch, NULL, victim, TO_CHAR);
    act("$n seems to expect the attack and easly avoids it.", victim, NULL, victim, TO_ROOM);
    skill = -100;
  }

  if (!shoot(ch, victim, skill, archery > 1, FALSE ))
    return;


  /* set hunting state */
  if (!IS_SET(victim->act2, ACT_LAWFUL)
      && !IS_SET(victim->act, ACT_RAIDER)
      && !IS_SET(victim->off_flags,CABAL_GUARD)){
    if (!IS_NPC(ch))
      victim->hunting = ch;
    else if (ch->master != NULL)
      victim->hunting = ch->master;
    else
      victim->hunting = ch;
    ch->hunttime = 0;
  }

  /* shoot back possibly */
  if (victim && victim->in_room && victim->in_room && victim->fighting == NULL){
    skill = get_skill( victim, gsn_fired );
    archery = get_skill(victim, gsn_archery) > 1 ;

    if ( (launcher = get_eq_char( victim, WEAR_RANGED)) != NULL
	 && IS_WEAPON_STAT( launcher, RANGED_FAST)
	 && get_eq_char( victim, WEAR_QUIVER)
	 && can_see( victim, ch )){

      //No marksman since this is a fast shot back
      //low skill based on range
      if (IS_WEAPON_STAT( launcher, RANGED_LONGRANGE))
	skill -= range * 5;
      else
	skill -= range * 10;

      shoot( victim, ch, skill, archery > 1, TRUE );
    }
  }
}


/* auto fires ranged weapons in combat  if able */
void fire_ranged_weapons( CHAR_DATA* ch, CHAR_DATA* victim ){
  int skill = get_skill(ch,gsn_fired);
  int archery = get_skill(ch, gsn_archery );

  if (ch == NULL || victim == NULL || ch->fighting != victim || IS_GAME(ch, GAME_NOAUTOFIRE))
    return;
  else if (--ch->reload > 0){
    return;
  }

  /* check if we can see the victim */
  if (!can_see(ch, victim)){
    if (number_percent() < archery){
      check_improve( ch, gsn_archery, TRUE, 1);
      skill /= 1.1;
    }
    else{
      skill /= 2;
    }
  }
  shoot( ch, victim, skill, archery > 1, TRUE );
}

//--------------====VOODOO DOLL============-----------//
/* This is the function that actuly casts the spell at victim */
/* By Viri */
bool voodoo_spell_place(CHAR_DATA* ch, CHAR_DATA* victim, int sn)
{
  /*
this is fairly simple except for the duration check.
we first check if the spell exists beofre casting,
then check again after casting.

If the spell was placed by voodoo then we find it and cut the duration.
otherwise we leave it alone.
  */

  AFFECT_DATA* paf;

//const
  const int svs_mod = 40; //svs mod used when voodo is placed

//data
 int level = spell_lvl_mod(ch, sn);

//bool
bool fBefore = FALSE;
bool fAfter = FALSE;


//first we check for spell before.
  fBefore = is_affected(victim, sn);

//cast the spell
  /* special case for deteriorate since it works better with higher saves */
  if (sn == gsn_deteriorate){
    (*skill_table[sn].spell_fun) ( sn, level - svs_mod / 3, ch, victim, TARGET_CHAR);
  }
  else{
    /* reduced saves before spell */
    victim->savingspell += svs_mod;
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim, TARGET_CHAR);
    /* restore saves*/
    victim->savingspell -= svs_mod;
  }
  //check again for the spell.
  if (is_affected(victim, sn))
    fAfter = TRUE;

  if (!fBefore && fAfter)
    for (paf = victim->affected; paf != NULL; paf = paf->next)
      if (paf->type == sn)
	paf->duration /= 2;


  return fAfter;
}


/* Function returns the voodoo doll or null if not found */
/* By Viri */
OBJ_DATA* has_voodoo_doll(CHAR_DATA* ch, char* target)
{
  //all we do is search for a doll with the targets name on paf.
OBJ_DATA* obj;
AFFECT_DATA* paf;

//start running through items
 for (obj = ch->carrying; obj != NULL; obj = obj->next_content){
   if ( (paf = affect_find(obj->affected, gsn_voodoo_spell)) == NULL
	|| !can_see_obj( ch, obj))
     continue;
   if (paf->has_string && paf->string
       && !strcasecmp(target, paf->string))
     return obj;
 }
return NULL;
}//end has_voodoo_doll


/* voodoo_spell */
/* By Viri */
void voodoo_spell(CHAR_DATA* ch, char* target, int sn)
{
  /*
     This function does all the prespell processing
     then calls voodoo_spell_place
  */

  CHAR_DATA* victim;
  OBJ_DATA* doll;
  AFFECT_DATA af;

//const
  const int mana_mod = 3;

  const int base_skill = 60;
  const int skill_med = 70;
  const int skill_mod = 3;

  const int luck_med = 12;
  const int luck_mod = 5;

  const int spell_lag = 6 + number_range(12, 18);

  const int shatter_lvl = 15;


//data
  int spell_cost = _mana_cost(ch, sn) * mana_mod;
  int skill = get_skill(ch, gsn_voodoo_spell);

  int chance = 0;
  int success = 0;

//check for readiness
  if (is_affected(ch, gsn_voodoo_spell))
    {
      send_to_char("You are not yet ready for another attempt.\n\r", ch);
      return;
    }
  //check for target
  if ( (victim = get_char_world(ch, target)) == NULL)
    {
      send_to_char("Bad juju like that needs a victim to be around.\n\r",ch);
      return;
    }
  //check for mobiness
  if (IS_NPC(victim))
    {
      send_to_char("Don't you be wasting evil like that on a mere mob.\n\r", ch);
      return;
    }
  //check for PK
  if (is_safe_quiet(ch, victim))
    {
      act("$G protects $N from your malice.", ch, NULL, victim, TO_CHAR);
      return;
    }

//check for target's doll
  if ( (doll = has_voodoo_doll(ch, target)) == NULL)
    {
      OBJ_DATA* obj;
      AFFECT_DATA* paf;
      int count = 0;
      act("You don't have $N's doll handy to perform such bad voodoo.", ch, NULL, victim, TO_CHAR);
      send_to_char("You possess following dolls:\n\r", ch);
      for (obj = ch->carrying; obj != NULL; obj = obj->next){
	if ( (paf = affect_find(obj->affected, gsn_voodoo_spell)) != NULL){
	  if (paf->has_string || paf->string)
	    sendf(ch, "%d. %-5s\n\r", ++count, paf->string);
	}
      }
      return;
    }

//first we check for mana.
  if (ch->mana < spell_cost)
    {
      sendf(ch, "You need a minimum of %d mana to place that voodoo.\n\r", spell_cost);
      return;
    }

//we have target and mana. now we check success.
  ch->mana -= spell_cost;
  WAIT_STATE(ch, skill_table[gsn_voodoo_spell].beats);

//calc. chance
  chance = base_skill;
  chance += (skill - skill_med) * skill_mod;
  chance += (get_curr_stat(ch, STAT_LUCK) - luck_med) * luck_mod;

//setup the common effects.
  af.type = gsn_voodoo_spell;
  af.level = ch->level;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;

//messages
  act("You aim carefull and stab a needle into $N's doll.", ch, NULL, victim, TO_CHAR);
  act("$n concentrates momentarly and stabs a needle into $p.", ch, doll, NULL, TO_ROOM);

//decide for success.
  if ( (success  = number_percent()) < chance)
    {
      CHAR_DATA* buf_ch;
      CHAR_DATA* buf_vch;

      buf_vch = victim->fighting;
      buf_ch = ch->fighting;

    //display more messages.
      send_to_char("You feel as if something very bad was heading your way.\n\r", victim);

      if (voodoo_spell_place(ch, victim, sn)){
	/* check if we used up its "uses" stored in cost */
	if (--doll->cost < 1){
	  send_to_char("You feel the doll twitch under your fingers and come apart!\n\r",ch);
	  doll->timer = 1;
	}
	else
	  send_to_char("You feel the doll twitch under your fingers!\n\r",ch);
      }
      else
	{
	  if (sn != gsn_harm)
	    send_to_char("Nothing seems to happen.\n\r", ch);
	  else
	    send_to_char("The doll bleeds under your fingers!\n\r", ch);
	}
      if (skill < 85 && number_percent() < 20)
	check_improve(ch, gsn_voodoo_spell, TRUE, -99);
      else
	check_improve(ch, gsn_voodoo_spell, TRUE, 3);
//we make sure to clean up the act after attack.
      if (buf_vch != NULL)
	set_fighting(victim, buf_vch);
      else
	stop_fighting(victim, TRUE);

      if (buf_ch != NULL)
	set_fighting(ch, buf_ch);
      else
	stop_fighting(ch, TRUE);

      af.duration = spell_lag;
      affect_to_char(ch, &af);
      return;
    }
//failure lag.
  af.duration = spell_lag/4;
  affect_to_char(ch, &af);

  act("You fail to send the bad voodoo at $N.", ch, NULL, victim, TO_CHAR);

//check if we break the doll.
  if (success < shatter_lvl)
    {
      act("The evil magic backfires and $p burns up!", ch, doll, NULL, TO_CHAR);
      extract_obj(doll);
    }

  if (skill < 85 && number_percent() < 20)
    check_improve(ch, gsn_voodoo_spell, FALSE, -99);
  else
    check_improve(ch, gsn_voodoo_spell, FALSE, 3);

  return;
}//end voodoo spell.


OBJ_DATA* voodoo_create_doll(CHAR_DATA* ch, CHAR_DATA* victim)
{
  OBJ_DATA* doll;
  AFFECT_DATA af;
  AFFECT_DATA* paf;
  char buf[MIL];

  /* this acutly makes the doll and return pointer. */

  doll = create_object( get_obj_index( OBJ_VNUM_VOODOO_DOLL ), 0);

  //set the name
  free_string(doll->name);
  sprintf(buf, "voodoo  doll %s", victim->name);
  doll->name = str_dup(buf);

  //set short desc.
  free_string(doll->short_descr);
  sprintf(buf, "a minature replica of %s", victim->name);
  doll->short_descr = str_dup(buf);

  //affix the effect to store the name.
  af.type = gsn_voodoo_spell;
  af.level = ch->level;
  af.duration = -1;

  af.where = TO_NONE;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  paf = affect_to_obj(doll, &af);
  string_to_affect(paf, victim->name);
  return doll;
}//end create_doll




/* voodoo_Create */
/* by Viri */
bool voodoo_create(CHAR_DATA* ch, char* name)
{
  /* this is the function run to actuly create the doll */
  OBJ_DATA* doll;
  CHAR_DATA* victim;
  AFFECT_DATA af;

  char buf[MIL];
//data
  int skill = get_skill(ch, gsn_voodoo_spell);
  int chance = 0;

//const
  const int base_skill = 50;
  const int skill_med = 75;
  const int skill_mod  = 2;

  const int lag_dur = 50 + number_range(0, 50);

  if (name[0] == '\0')
    {
      send_to_char("voodoo create\n\rsyntax:  voodoo create <player name>\n\r", ch);
      return FALSE;
    }
  //get the victim
  if ( (victim = get_char_world(ch, name)) == NULL)
    {
      send_to_char("Target not found.\n\r", ch);
      return FALSE;
    }
  //check if target is player.
  if (IS_NPC(victim))
    {
      send_to_char("Why waste such bad juju on a mere mobile?.\n\r", ch);
      return FALSE;
    }
  //now we create it.
  if (ch->mana < skill_table[gsn_voodoo_spell].min_mana)
    {
      send_to_char("You dont have enough mana!\n\r", ch);
      return FALSE;
    }

  if (is_affected(ch, gen_voodoo_doll))
    {
      send_to_char("You are not yet ready for another attempt at doll making.\n\r", ch);
      return FALSE;
    }

  act("You begin to fashion a doll of $N.", ch, NULL, victim, TO_CHAR);
  act("Muttering curses and mumbling unholy words, $n begins to fashion a small wooden doll.", ch, NULL, victim, TO_ROOM);

  //calc the skill.success
  chance = base_skill;
  chance += (skill - skill_med) * skill_mod;

  WAIT_STATE(ch, skill_table[gsn_voodoo_spell].beats);
  if (number_percent() > chance)
    {
      ch->mana -= skill_table[gsn_voodoo_spell].min_mana / 2;
      send_to_char("The doll falls apart in your hands.\n\r", ch);
      act("The doll falls apart in $n's hands.", ch, NULL, NULL, TO_ROOM);
      af.type = gen_voodoo_doll;
      af.level = ch->level;
      af.duration = lag_dur / 4;
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier = 0;
      affect_to_char(ch, &af);

      check_improve(ch, gsn_voodoo_spell, FALSE, -99);
      return FALSE;

    }
  //create the dool.
  doll = voodoo_create_doll(ch, victim);
  obj_to_char(doll, ch);

  act("You create a perfect replica of $N!", ch, NULL, victim, TO_CHAR);
  act("$n creates a cute little doll.", ch, NULL, victim, TO_ROOM);
  check_improve(ch, gsn_voodoo_spell, TRUE, -99);

  //log tyhe creation.
  sprintf(buf,"%s has created a voodoo doll of %s.\r", ch->name, victim->name);
  log_event(ch, buf);

  af.type = gen_voodoo_doll;
  af.level = ch->level;
  af.duration = lag_dur;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

  return TRUE;
}


/* do_voodoo */
void do_voodoo( CHAR_DATA *ch, char *argument) {
/*
This isthe shaman voodoo doll skill. There are two main functions.
- Create  (voodoo_create)
- Place   (voodoo_place)

- Create takes the name of the target
- Create takes the spell and the name of target.
- vaiable spells are:

= weaken
= curse
= plauge
= poison
= hex
= blind
= harm
= insomnia
= enfeeblement

- spells are cast at +5 level bonus
- durations are halfed if they were not present befire thespell.
*/

  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];

//bools
  bool fCreate = TRUE;

//data
  int sn = 0;

//get the arguments as usual.
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  strcpy(arg3, argument);

  //usual checks.
  if (get_skill(ch, gsn_voodoo_spell) < 1) {
    check_social(ch, "voodoo", arg1);
    return;
  }

  if (arg1[0] == '\0') {
    send_to_char("voodoo\n\rsyntax:  voodoo [create/place]\n\r"\
		 "  -create <target>\n\r  -place <target> <spell>\n\r", ch);
    return;
  }

  if (str_prefix("create", arg1)) {
    if (!str_prefix("place", arg1)) {
      fCreate = FALSE;
    }
    else {
      do_voodoo(ch, "");
      return;
    }
  }

  if (fCreate) {
      /* CREATE */
    voodoo_create(ch, arg2);
    return;
  }//END CREATE

  /* PLACE */
  //check the target and spell.
  if (arg2[0] == '\0' || arg3[0] == '\0' ) {
    send_to_char("voodoo place\n\rsyntax:  voodoo place <player name> <spell>\n\r", ch);
    return;
  }

  //check for type of spell.
  sn = find_spell(ch, arg3);

  if ( (sn == -1 )
       || (sn != gsn_weaken
	   && sn != gsn_curse
	   && sn != gsn_plague
	   && sn != gsn_poison
	   && sn != gsn_hex
	   && sn != gsn_blindness
	   && sn != gsn_harm
	   && sn != gsn_insomnia
	   && sn != gsn_enfeeblement
	   && sn != gsn_deteriorate)) {
    send_to_char("Voodoo doll may only cause following maladies:\n\r"\
		 "weaken  curse   plague    poison  hex\n\r"\
		 "harm   insomnia blindness enfeeblement\n\r"\
		 "deteriorate.\n\r", ch);
    return;
  }

  //now everything is ok.
  voodoo_spell(ch, arg2, sn);
}

/* BATTLE MAGE */
/* field dressing */
void do_fdress( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MIL];
    sh_int chance;
    AFFECT_DATA af;

    const int min_level = 80;

    one_argument (argument,arg);
    if ((chance = get_skill(ch,gsn_field_dress)) < 1)
    {
	send_to_char("You have not been trained to apply field dressing.\n\r",ch);
	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("You can't do that while fighting.\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_field_dress))
    {
	send_to_char("You are not ready for another attempt.\n\r",ch);
	return;
    }
    victim = get_char_room(ch, NULL, arg);
    if (arg[0] == '\0')
	victim = ch;
    if (victim != ch)
    {
	send_to_char("You can only apply the dressing to your wounds.\n\r",ch);
	return;
    }
    if (ch->mana < 100)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_BLIND))
    {
	send_to_char("You can't even see your wounds!.\n\r",ch);
	return;
    }


    if ( ch->hit * 100  / ch->max_hit > min_level)
      {
	send_to_char("On mere cuts and bruises?\n\r", ch);
	return;
      }

    WAIT_STATE2(ch,skill_table[gsn_field_dress].beats);
    af.type		= gsn_field_dress;
    af.level		= ch->level;
    af.duration		= number_fuzzy(8);
    af.location		= APPLY_DEX;
    af.modifier		= -2;
    af.bitvector	= 0;
    affect_to_char(ch,&af);
    if (number_percent( ) > chance)
    {
	ch->mana -= 50;
	af.location	= APPLY_STR;
	af.modifier	= -2;
	affect_to_char(ch,&af);
	af.location	= APPLY_HIT;
	af.modifier	= -100 + ch->level;
	affect_to_char(ch,&af);
	act("You make a mistake and only make it worse.",ch,NULL,NULL,TO_CHAR);
	act("$n begins to dress his wounds but doesn't look any better.",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_field_dress,FALSE,1);
    }
    else
    {
	ch->mana -= 100;
	ch->hit = UMIN(ch->max_hit, ch->hit + number_range(ch->level, ch->level *5));
	update_pos(ch);
	act("You feel your wounds heal rapidly.", ch, NULL,NULL,TO_CHAR);
	act("$n's wounds quickly heal as he applies a field dressing.",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_field_dress,TRUE,1);
    }
}

/* lizard chameleon skill */
void do_chameleon( CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA af;
  int chance = get_skill(ch,gsn_chameleon);

  if (ch->race != race_lookup("slith"))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
  if (chance < 1)
    {
      send_to_char("You stand motionless for a second and feel slily.\n\r", ch);
      return;
    }

  if (is_affected(ch, gsn_chameleon))
    {
      send_to_char("You are not ready for another attempt.\n\r", ch);
      return;
    }
  if (ch->mana < skill_table[gsn_chameleon].min_mana)
    {
      send_to_char("You can't seem to concenrate enough.\n\r", ch);
      return;
    }
  else
    ch->mana -= skill_table[gsn_chameleon].min_mana;

  if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL)
    {
      send_to_char("You can't blend in while mounted!\n\r",ch);
      return;
    }
  if ( IS_AFFECTED(ch, AFF_FAERIE_FOG) || IS_AFFECTED(ch, AFF_FAERIE_FIRE))
    {
      send_to_char( "You can't chameleon while glowing.\n\r", ch);
      return;
    }

  if (chance > 0)
    chance += 2 * (get_curr_stat(ch,STAT_LUCK) - 12);

  send_to_char( "You attempt to blend in with your surroundings.\n\r", ch );
  WAIT_STATE(ch,skill_table[gsn_chameleon].beats);

  if ( IS_AFFECTED2(ch, AFF_CAMOUFLAGE) )
    REMOVE_BIT(ch->affected2_by, AFF_CAMOUFLAGE);

  if ( chance > number_percent())
    {
      af.type = gsn_camouflage;
      af.level = ch->level;
      af.duration = 6;

      af.where = TO_AFFECTS2;
      af.bitvector = AFF_CAMOUFLAGE;
      af.location = APPLY_NONE;
      af.modifier = 0;
      affect_to_char(ch, &af);
      check_improve(ch,gsn_chameleon,TRUE,1);
    }
  else{
    send_to_char("You failed.\n\r", ch);
    check_improve(ch,gsn_chameleon,FALSE,1);
    return;
  }
  //add wait time
  af.type = gsn_chameleon;
  af.level = ch->level;
  af.duration = 12;

  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;

  affect_to_char(ch, &af);
}


/* demon corruption ability */
void do_corrupt( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;

    int skill = get_skill(ch, gsn_corrupt);
    int save_mod = (skill - 75)/3;
    int cost = _mana_cost(ch, gsn_corrupt);

    const int res_cost = 10;

    one_argument(argument,arg);

    if (skill < 1)
    {
	send_to_char("Maybe try defecate instead?.\n\r",ch);
	return;
    }
    else if (is_affected(ch, gsn_planar_seal)){
      send_to_char("The planar seal prevents you from drawing on your powers.\n\r", ch);
      return;
    }
    if (arg[0] == '\0')
    {
	victim = ch->fighting;
	if (victim == NULL)
	{
	    send_to_char("But you aren't in combat!\n\r",ch);
	    return;
	}
    }
    else if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if ( victim == ch )
    {
	send_to_char("You are bad enough as it is.\n\r", ch);
	return;
    }

    if (is_safe(ch,victim))
	return;

    if (!IS_NPC(ch) && ch->mana < cost)
      {
	send_to_char("You lack the power to control chaos.\n\r", ch);
	return;
      }
    else if (!IS_NPC(ch))
      ch->mana -= cost;

    WAIT_STATE2(ch,skill_table[gsn_corrupt].beats);
    a_yell(ch,victim);

    //predict interdict
    if (predictCheck(ch, victim, "corrupt", skill_table[gsn_corrupt].name))
      return;

    if (!saves_spell( ch->level + save_mod , victim, DAM_ENERGY,skill_table[gsn_corrupt].spell_type)
	&& get_avatar_level(victim) < 1)
      {
	AFFECT_DATA af;
	if (ch->fighting != victim && victim->fighting != ch)
	  {
	    sprintf(buf, "Help! %s's taint is upon me!",PERS(ch,victim));
	    j_yell(victim,buf);
	  }
	act("You taint $N with pure chaos!", ch, NULL, victim,TO_CHAR);
	act("Your insides twist as you feel $n's tain upon you.",ch,NULL,victim,TO_VICT);
	damage(ch,victim, UMAX(1, save_mod * 10), gsn_corrupt, DAM_INTERNAL,TRUE);
	check_improve(ch,gsn_corrupt,TRUE,1);


	if (is_affected(victim, gsn_corrupt))
	  {
	    act("$E's has already been corrupted with chaos.",ch,NULL,victim,TO_CHAR);
	  }
	else{
	  af.type 	= gsn_corrupt;
	  af.level 	= ch->level;
	  af.duration	= skill < 85 ? 0 : 1;

	  af.where	= TO_AFFECTS;
	  af.bitvector = 0;

	  af.location	= APPLY_MANA;
	  af.modifier = -20;
	  affect_to_char(victim,&af);
	}
      }//END SUCCESS
    else
      {
        if (ch->fighting != victim && victim->fighting != ch)
	  {
            sprintf(buf, "Your efforts to taint me are uselss %s!",PERS(ch,victim));
            j_yell(victim,buf);
	  }
	if (get_avatar_level(victim) >= 2){
	  act("$n seems unaffected by your attempts.", ch, NULL, victim, TO_CHAR);
	  act("$g's power protects you against $N's corruption.", victim, NULL, ch, TO_CHAR);
	  divfavor_gain(victim,  -res_cost);
	}
	damage(ch,victim, UMAX(1, save_mod * 2), gsn_corrupt, DAM_INTERNAL,TRUE);
	check_improve(ch,gsn_corrupt,FALSE,1);
      }//EDN FAIL
}




/* Written by: Virigoth */
/* Comments:  Allows an avatar to cure most maledictions from himself based on divine favor*/
void avatar_purify(CHAR_DATA* ch, int level){
  const int chance  = URANGE(5, level + ch->pcdata->divfavor / 200, 85);

  /* Viri: No cure blind for now, would be too good ;)
  if (is_affected(ch, gsn_blindness) && number_percent() < chance)
    {
        affect_strip(ch, gsn_blindness);
	send_to_char("Your can see again.\n\r",victim);
    }
  */
  act("$g's spirit saturates your flesh restoring it anew.", ch, NULL, NULL, TO_CHAR);
  act("A bright aura of $g's power surrounds $n briefly.", ch, NULL, NULL, TO_ROOM);

  if (is_affected(ch, gsn_death_grasp) && number_percent() < chance)
    {
      affect_strip(ch, gsn_death_grasp);
      send_to_char("You regain some of your vitality.\n\r",ch);
    }
  if (is_affected(ch, gsn_poison) && number_percent() < chance)
    {
      affect_strip(ch, gsn_poison);
      send_to_char("You feel less sick.\n\r",ch);
    }
  if (is_affected(ch, gsn_plague) && number_percent() < chance)
    {
      affect_strip(ch, gsn_plague);
      send_to_char("Your sores vanish.\n\r",ch);
    }
  if (is_affected(ch, skill_lookup("dysentery")) && number_percent() < chance)
    {
      affect_strip(ch, skill_lookup("dysentery"));
      send_to_char("Your bowel movements returns to normal.\n\r",ch);
    }
  if (is_affected(ch, gsn_enfeeblement) && number_percent() < chance)
    {
      affect_strip(ch, gsn_enfeeblement);
      send_to_char("Your sores vanish.\n\r",ch);
    }
  if (is_affected(ch, gsn_embrace_poison)){
    act("The bite wound sizzles, smokes and then quickly heals and disappears.", ch, NULL, ch, TO_ROOM);
    affect_strip(ch, gsn_embrace_poison);
  }
  else
    spell_cure_serious( skill_lookup("cure serious"),  ch->level, ch, ch, TARGET_CHAR );
}


void do_entangle(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    int chance;
    if (IS_NPC(ch) || (chance = get_skill(ch,gsn_entangle)) < 1)
    {
        send_to_char("Huh?\n\r",ch);
        return;
    }
    if (ch->fighting == NULL)
    {
        send_to_char("You must be in combat.\n\r",ch);
        return;
    }
    victim = ch->fighting;
    act("Thorn briars rise from the ground and entangle the feet of $N!",ch,NULL,victim,TO_CHAR);
    act("Thorn briars rise from the ground and tie up your feet!",ch,NULL,victim,TO_VICT);
    WAIT_STATE2(ch,skill_table[gsn_entangle].beats);
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += ch->level - victim->level;
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    if(!IS_NPC(ch) && number_percent( ) >= chance)
    {
        damage(ch,victim,0,gsn_entangle,DAM_PIERCE,TRUE);
        check_improve(ch,gsn_entangle,FALSE,1);
        return;
    }
    if (!is_affected(victim,gsn_entangle))
    {
        AFFECT_DATA af;
        act("$N struggles to free $mself from the vines!",ch,NULL,victim,TO_CHAR);
        act("You struggle to free yourself from the vines!",ch,NULL,victim,TO_VICT);

        af.type         = gsn_entangle;
        af.level        = ch->level;
        af.duration     = ch->level /10;
        af.location     = APPLY_HITROLL;
        af.modifier     = -5;
        af.bitvector    = 0;
        affect_to_char(victim,&af);
        af.location     = APPLY_DAMROLL;
        af.modifier     = -5;
        affect_to_char(victim,&af);
        af.location     = APPLY_DEX;
        af.modifier     = -5;
        affect_to_char(victim,&af);
        check_improve(ch,gsn_entangle,TRUE,1);
    }
    damage(ch,victim,2 * ch->level /3,gsn_entangle,DAM_PIERCE,TRUE);
}

void do_druid_staff( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *staff;
    AFFECT_DATA *paf;
    int chance;
    if ((chance = get_skill(ch, gsn_druid_staff)) < 1)
    {
        send_to_char("You don't know how to make a druid staff.\n\r",ch);
        return;
    }
    if (ch->mana < 80)
    {
        send_to_char("You don't have enough energy to make a staff.\n\r",ch);
        return;
    }
    WAIT_STATE2(ch,skill_table[gsn_druid_staff].beats);
    if ( number_percent( ) < chance)
    {
        check_improve(ch,gsn_druid_staff,TRUE,1);
        ch->mana -= 80;
        staff = create_object( get_obj_index( OBJ_VNUM_DRUID_STAFF ), 0);
        staff->value[1] = number_range(ch->level -6, ch->level)/3;
        staff->value[2] = 2;
        staff->level = ch->level;
        staff->timer = 24;
        if (IS_EVIL(ch))
        {
            SET_BIT(staff->extra_flags,ITEM_ANTI_GOOD);
            SET_BIT(staff->extra_flags,ITEM_ANTI_NEUTRAL);
        }
        else if (IS_GOOD(ch))
        {
            SET_BIT(staff->extra_flags,ITEM_ANTI_EVIL);
            SET_BIT(staff->extra_flags,ITEM_ANTI_NEUTRAL);
        }
        else
        {
            SET_BIT(staff->extra_flags,ITEM_ANTI_GOOD);
            SET_BIT(staff->extra_flags,ITEM_ANTI_EVIL);
        }
        paf = new_affect();
        paf->type       = gsn_druid_staff;
        paf->level      = ch->level;
        paf->duration   = -1;
        paf->location   = APPLY_HIT;
        paf->modifier   = ch->level/2;
        paf->bitvector  = 0;
        paf->next       = staff->affected;
        staff->affected   = paf;
        paf = new_affect();
        paf->type       = gsn_druid_staff;
        paf->level      = ch->level;
        paf->duration   = -1;
        paf->location   = APPLY_MANA;
        paf->modifier   = ch->level/2;
        paf->bitvector  = 0;
        paf->next       = staff->affected;
        staff->affected   = paf;
        obj_to_char(staff, ch);
        act( "You whittle some wood and create $p.", ch, staff, NULL, TO_CHAR );
        act( "$n whittles a long oak branch and creates $p.", ch, staff, NULL, TO_ROOM );
    }
    else
    {
        ch->mana -= 40;
        check_improve(ch,gsn_druid_staff,FALSE,2);
        send_to_char("You failed to make a druid staff.\n\r",ch);
    }
}

void do_trap_silvanus(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int chance;
    if ( (chance = get_skill(ch,gsn_trap_silvanus)) < 1)
    {
        send_to_char("You don't know how to set a trap.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_trap_silvanus))
    {
        send_to_char("You already have a trap set up.\n\r",ch);
        return;
    }
    if (is_affected(ch,gsn_wire_delay))
    {
        send_to_char("You are not ready to set another trap.\n\r",ch);
        return;
    }
    if (ch->mana < 50)
    {
        send_to_char("You don't have enough energy to set up a trap.\n\r",ch);
        return;
    }
    ch-> mana -= 50;
    WAIT_STATE2(ch,skill_table[gsn_trap_silvanus].beats );
    if (chance < number_percent())
    {
        send_to_char("You failed to set up a trap.\n\r",ch);
        check_improve(ch,gsn_trap_silvanus,FALSE,1);
        return;
    }
    act("You set up a Trap of Silvanus and wait for your victim.",ch,NULL,NULL,TO_CHAR);
    af.where    = TO_AFFECTS;
    af.type             = gsn_trap_silvanus;
    af.level            = ch->level;
    af.duration         = ch->level/5;
    af.location         = 0;
    af.modifier         = 0;
    af.bitvector        = 0;
    affect_to_char(ch,&af);
    check_improve(ch,gsn_trap_silvanus,TRUE,1);
}


/* Written by: Virigoth */
/* Comments:  Allows an avatar to call on his deities powers */
void do_call( CHAR_DATA *ch, char *argument ){

  //const
  const int level = get_avatar_level(ch);
  const int cost = 75;
  const int sight_cost = 500;
  const int temple_cost = 30;
  const int purify_cost = 60;
  const int empower_cost = 100;

  //bool
  bool fFound = FALSE;

  //data
  int cmd;

  char arg[MIL];
  if (!IS_AVATAR(ch) || is_affected(ch, gsn_planar_seal)){
    act("You call onto $g but the heavens ring silent.", ch, NULL, NULL, TO_CHAR);
    return;
  }

  /* check for the command */
  one_argument(argument,arg);
  if (arg[0] == '\0'){
    act("Call $g\n\r  syntax:  call <power>", ch, NULL, NULL, TO_CHAR);
    send_to_char("Following powers may be utilised by you:  ", ch);
    for(cmd = 0; cmd < MAX_AVATAR_CMD; cmd ++){
      if (avatar_info[level].cmd[cmd] == NULL)
	break;
      else{
	fFound = TRUE;
	sendf(ch, "%s,  ", avatar_info[level].cmd[cmd]);
      }
    }
    if (fFound)
      send_to_char("\n\r", ch);
    else
      send_to_char("None\n\r", ch);
    act(get_avatar_desc(ch), ch, NULL, NULL, TO_CHAR);
    return;
  }

  /* begin executing the different powers */
  if (ch->mana < cost){
    send_to_char("You don't have the power.\n\r", ch);
    return;
  }


  /* quick check for faith */
  if ( (!IS_NPC(ch) && (IS_SET(ch->act,PLR_DOOF)
			    || IS_SET(ch->act,PLR_OUTCAST)))
       || is_affected(ch,gsn_damnation)
       || (!IS_IMMORTAL(ch) && IS_SET(ch->in_room->room_flags2,ROOM_NO_MAGIC)) ){
    send_to_char("The heavens ring silent.\n\r", ch);
    return;
  }


  /* find wich command is being called */
  fFound = FALSE;
  for(cmd = 0; cmd < MAX_AVATAR_CMD; cmd ++){
    if (avatar_info[level].cmd[cmd] == NULL)
      break;
    if (!str_prefix(arg, avatar_info[level].cmd[cmd])){
      fFound = TRUE;
      break;
    }
  }//END FOR

  if (!fFound){
    send_to_char("No such power.\n\r", ch);
    return;
  }


  /* select wich commands */
  switch (cmd){
  case 0:
    /* cant do it too soon after combat */
    if (is_fight_delay(ch, 20)){
      act("With the air of violence still about you, the call onto $g fails.", ch, NULL, NULL, TO_CHAR);
      return;
    }
    if (ch->pcdata->divfavor < sight_cost){
      send_to_char("The heavens ring silent.\n\r", ch);
      return;
    }
    if (IS_AFFECTED(ch, AFF_DETECT_INVIS)){
      send_to_char("You can already see invisbile.\n\r", ch);
      return;
    }
    act("Your eyes fill with searing light as $g grants you its favor.", ch, NULL, NULL, TO_CHAR);
    act("$n's eyes fill with searing light as $g grants $m its favor.", ch, NULL, NULL, TO_ROOM);
    {
      AFFECT_DATA af;
      af.level = 100;
      af.duration = 480;
      af.type = skill_lookup("detect invis");
      af.where = TO_AFFECTS;
      af.bitvector = AFF_DETECT_INVIS;
      af.location = APPLY_HITROLL;
      af.modifier = 3;
      affect_to_char(ch, &af);
    }
    divfavor_gain(ch, -sight_cost);
    break;
  case 1:
    /* cant do it too soon after combat */
    if (is_fight_delay(ch, 20)){
      act("With the air of violence still about you, the call onto $g fails.", ch, NULL, NULL, TO_CHAR);
      return;
    }
    if (ch->pcdata->divfavor < temple_cost){
      send_to_char("The heavens ring silent.\n\r", ch);
      return;
    }
    act("The space blurs around you as $g grants you his favor.", ch, NULL, NULL, TO_CHAR);
    spell_word_of_recall( skill_lookup("word of recall"), ch->level, ch, ch, TARGET_CHAR);
    divfavor_gain(ch, -temple_cost);
    break;
  case 2:
    if (ch->pcdata->divfavor < purify_cost){
      send_to_char("The heavens ring silent.\n\r", ch);
      return;
    }
    avatar_purify(ch, level);
    divfavor_gain(ch, -purify_cost);
    break;
  case 3:
    if (ch->pcdata->divfavor < empower_cost){
      send_to_char("The heavens ring silent.\n\r", ch);
      return;
    }
    act("Air begins to crackle around you as $g's divine wrath fills your body.", ch, NULL, NULL, TO_CHAR);
    act("Air crackles with energy about you as $g's righteous power empowers $n.", ch, NULL, NULL, TO_ROOM);
    spell_haste( skill_lookup("haste"), 2 * ch->level / 3, ch, ch, TARGET_CHAR);
    divfavor_gain(ch, -empower_cost);
    break;
  default:send_to_char("No such power.\n\r", ch);
    return;
  }
}

void do_sequencer(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    AFFECT_DATA* paf;

    char arg1[MIL];

    const int sn = skill_lookup("sequencer");
    const int chance = get_skill(ch,sn);
    const int lag = skill_table[sn].beats;
    const int level = ch->level;

    const int max_spell = level / 10;	//max number of spells sequenced
    const int max_level = 40;	//max level of spells added.

    int spells = 0;		//number of spells sequenced
    int cost = 0;

    if (chance < 1)
    {
        send_to_char("You don't know how to create a sequencer.\n\r",ch);
        return;
    }

    argument = one_argument(argument, arg1);

    /* show syntax */
    if (arg1[0] == '\0'){
      int i = 0;
      sendf(ch, "sequencer <use/add> [spell name]:\n\r"\
	    "You may store up to %d defensive spells of level %d and less.\n\r\n\r",
	    max_spell, max_level);
      /* Show stored spells */
      for (paf = ch->affected; paf; paf = paf->next){
	if (paf->type == sn){
	  i++;
	  sendf(ch, "%d.  %s   %s", i, skill_table[paf->modifier].name,
		i % 3 ? "": "\n\r");
	}
      }
      return;
    }

    if (number_percent() > chance){
      send_to_char("You failed.\n\r", ch);
      return;
    }
    WAIT_STATE2(ch, lag);

    /* ADDING A SPELL */
    if (!str_prefix(arg1, "add")){
      int add_sn = skill_lookup(argument);

      if (argument[0] == '\0'){
	send_to_char("Add what spell?\n\r", ch);
	return;
      }

      /* check if spell exists and ch can cast it. */
      /* NO GEN's hence the > MAX_SKILL */
      if (add_sn < 1 || get_skill(ch, add_sn) < 1
	  || add_sn > MAX_SKILL){
	send_to_char("You are not capable of casting such spell.\n\r", ch);
	return;
      }
      /* check level */
      if (sklevel(ch, add_sn) > max_level){
	sendf(ch, "Spells must be equal or lower then %dth level.\n\r", max_level);
	return;
      }

      /* Check if defensive */
      if (skill_table[add_sn].target != TAR_CHAR_DEFENSIVE
	  && skill_table[add_sn].target != TAR_CHAR_SELF){
	send_to_char("The spell must be defensive in nature.\n\r", ch);
	return;
      }
      /* Now we add them up */
      /* Count how many exists */
      for (paf = ch->affected; paf; paf = paf->next){
	if (paf->type == sn){
	  /* check if duplicate */
	  if (paf->modifier == add_sn){
	    send_to_char("Duplicate charges are not allowed.\n\r", ch);
	    return;
	  }
	  else
	    spells++;
	}
      }

      if (spells >= max_spell){
	send_to_char("The sequencer holds maximum number of spells.\n\r", ch);
	return;
      }
      /* charge for use */
      cost =  UMAX(skill_table[sn].min_mana, 3 * _mana_cost(ch, add_sn));
      if (ch->mana < cost){
	sendf(ch, "You need at least %d mana.\n\r", cost);
	return;
      }
      else
	ch->mana -= cost;

      /* All OK now we add it on, sn of spell to be cast stored in modifier */
      af.type = sn;
      af.level = level;
      af.duration = -1;

      af.where = TO_NONE;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier = add_sn;
      affect_to_char(ch, &af);
      sendf(ch, "You store %s in your sequencer.\n\r", skill_table[add_sn].name);
      act("$n seems to concentrate deeply as runes flare about $m.", ch, NULL, NULL, TO_ROOM);
      return;
    }

    /* USE */
    else if (!str_prefix(arg1, "use")){
      /* Run through all affects, if a sequencer cast the function */
      for (paf = ch->affected; paf; paf = paf->next){
	int add_sn = paf->modifier;
	if (paf->type == sn
	    && add_sn && skill_table[add_sn].spell_fun != spell_null){
	  (*skill_table[add_sn].spell_fun) ( add_sn, paf->level, ch, ch,TARGET_CHAR);
	  check_improve(ch,add_sn,TRUE,1);
	}
      }
      check_improve(ch,sn,TRUE,1);
      /* strip all spells */
      affect_strip(ch, sn);
    }
    else
      do_sequencer(ch, "");
}







void do_fasting( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    if (IS_NPC(ch))
	return;
    if (is_affected(ch,gsn_fasting))
    {
	send_to_char("You are already fasting.\n\r",ch);
	return;
    }
    if (number_percent() > get_skill(ch,gsn_fasting))
    {
	send_to_char("You stomach grumbles but you don't feel any different.\n\r",ch);
	check_improve(ch,gsn_fasting,FALSE,1);
	return;
    }
    WAIT_STATE(ch,skill_table[gsn_fasting].beats);
    send_to_char("You start fasting.\n\r",ch);
    send_to_char("You are resistant to hunger and thirst.\n\r",ch);
    af.where     = TO_AFFECTS;
    af.type      = gsn_fasting;
    af.level     = ch->level;
    af.duration  = ch->level/5;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char(ch,&af);
    check_improve(ch,gsn_fasting,TRUE,1);
}

void do_brew(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj, *potion = NULL;
    char arg[MIL], buf[MSL], type[MSL], color[MIL];
    int spell = 0;
    int level = ch->level;
    int skill = get_skill(ch, gsn_brew);
    const int dur = 140;
    one_argument( argument, arg );

    if (skill < 1){
      send_to_char("Brew? Whats that?", ch);
      return;
    }
    if (is_affected(ch, gsn_brew))
    {
        send_to_char("But you just brewed something!\n\r", ch);
        return;
    }
    if (ch->mana < 25)
    {
        send_to_char("You don't have the patience to brew anything right now.\n\r", ch);
        return;
    }
    if ( arg[0] == '\0' )
    {
        send_to_char( "Brew what?\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "You do not have that on you.\n\r", ch );
        return;
     }
    if (obj->item_type != ITEM_HERB && obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL)
    {
        send_to_char("Only herbs, pills and food items can be transformed into a potion.\n\r",ch);
        return;
    }
    if (obj->wear_loc != -1)
    {
        send_to_char("The item must be carried to be brewed.\n\r",ch);
        return;
    }
    if (number_percent() > skill){
      send_to_char("You fail.\n\r", ch);
      check_improve(ch,gsn_brew,FALSE,1);
      return;
    }
    ch->mana-=25;
    if ( (potion = create_object( get_obj_index(OBJ_VNUM_POTION), ch->level) ) == NULL){
      bug("do_brew: Could not create potion skeleton.", 0);
      return;
    }
    if (number_percent() < 25)
    {
        act("You try to brew $p, but end up overcooking it.", ch, potion, NULL, TO_CHAR);
        act("$n tries to brew $p, but ends up overcooking the recipe.", ch, potion, NULL, TO_ROOM);
        extract_obj(obj);
	check_improve(ch,gsn_brew,FALSE,1);
        return;
    }

    buf[0]='\0';
    if (obj->item_type == ITEM_HERB)
        sprintf(type, "large");
    else if (obj->item_type == ITEM_FOOD)
      sprintf(type, "bubbly");
    else
      sprintf(type, "small");

    if (obj->item_type == ITEM_HERB)
      {
	spell = obj->value[3];
	sprintf(color, "%s", skill_table[spell].name);
	level = obj->level;
      }
    else if (obj->item_type == ITEM_FOOD)
      {
        switch(number_bits(3))
	  {
	  case 0: spell = skill_lookup("cure blindness");  level = 25; sprintf(color, "red");
	  break;
        case 1: spell = skill_lookup("haste");          level = 10; sprintf(color, "green");
	  break;
        case 2: spell = skill_lookup("frenzy");         level = 35; sprintf(color, "crimson");
	  break;
        case 3: spell = skill_lookup("sanctuary");		level = 35; sprintf(color, "white");
	  break;
        case 4: spell = skill_lookup("fireball");      level = 5; sprintf(color, "crimson");
	  break;
        case 5: spell = skill_lookup("invis");          level = 40; sprintf(color, "clear");
	  break;
        case 6: spell = skill_lookup("cure light");     level = 25; sprintf(color, "yellow");
	  break;
        case 7: spell = skill_lookup("cure serious");   level = 25; sprintf(color, "brown");
	  break;
        }
    }
    else
    {
      if (number_percent() < 68){
	spell = skill_lookup("detect magic");
	sprintf(color, "clear");
      }
      else if (number_percent() < 34){
            spell = skill_lookup("detect invis");
	    sprintf(color, "white");
      }
      else{
            spell = skill_lookup("pass door");
	    sprintf(color, "misty");
      }
    }
    /* describe from type and color */
    free_string( potion->name );
    sprintf(buf, "%s %s %s", type, color, "potion");
    potion->name = str_dup( buf );

    free_string( potion->short_descr );
    if (obj->item_type == ITEM_HERB)
      sprintf(buf, "a %s potion of %s",type, color);
    else
      sprintf(buf, "a %s %s potion", type, color);
    potion->short_descr = str_dup( buf );

    free_string( potion->description );
    if (obj->item_type == ITEM_HERB)
      sprintf(buf, "A %s potion of %s is here.", type, color);
    else
      sprintf(buf, "A %s %s potion is here.", type, color);
    potion->description = str_dup( buf );

    /* set spell */

    potion->value[0] = level;
    potion->value[1] = spell;
    potion->class = ch->class;
    extract_obj(obj);
    act("You brew $p from a large kettle!", ch, potion, NULL, TO_CHAR);
    act("$n brews $p from a large kettle!", ch, potion, NULL, TO_ROOM);
    obj_to_char(potion, ch);
    potion->timer = dur;
    check_improve(ch,gsn_brew,TRUE,1);
}


/* undead drain ability */
void do_lifedrain( CHAR_DATA *ch, char *argument )
{
  char arg[MIL], buf[MSL];
  CHAR_DATA *victim;
  AFFECT_DATA* paf;

  int skill = get_skill(ch, gsn_lifedrain);
  int cost = _mana_cost(ch, gsn_lifedrain);
  const int mod = (skill - 75) / 5;
  const int res_cost = 10;
  int dam = number_range(10, 25);

  one_argument(argument,arg);

  if (skill < 1)
    {
      send_to_char("Huh?\n\r",ch);
      return;
    }

  if (arg[0] == '\0')
    {
      victim = ch->fighting;
      if (victim == NULL)
	{
	  send_to_char("But you aren't in combat!\n\r",ch);
	  return;
	}
    }
  else if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
      send_to_char("They aren't here.\n\r",ch);
      return;
    }

  if ( victim == ch )
    {
      send_to_char("You are bad enough as it is.\n\r", ch);
      return;
    }

  if (!IS_NPC(ch) && ch->mana < cost)
    {
      send_to_char("You lack the power to drain vitality.\n\r", ch);
      return;
    }
  else if (!IS_NPC(ch))
    ch->mana -= cost;
  WAIT_STATE2(ch,skill_table[gsn_lifedrain].beats);

  if (is_safe(ch,victim))
    return;

  a_yell(ch,victim);

  //predict interdict
  if (predictCheck(ch, victim, "lifedrain", skill_table[gsn_lifedrain].name))
    return;

  if ( (paf = affect_find(victim->affected, gsn_lifedrain)) != NULL
       && paf->modifier <= -50)
    {
      act("You cannot drain any more of $N's vitality.",ch,NULL,victim,TO_CHAR);
      return;
    }

  if (get_avatar_level(victim) < 1 && !is_affected(victim, gsn_ptrium)){
    const int exp_dr = number_range(8,16);
    AFFECT_DATA af;
    if (ch->fighting != victim && victim->fighting != ch)
      {
	sprintf(buf, "Help! %s touched me!",PERS(ch,victim));
	j_yell(victim,buf);
      }
    act("You grope at $N to drain $S vitality!", ch, NULL, victim,TO_CHAR);
    act("$n gropes at $N to drain $S vitality!", ch, NULL, victim,TO_NOTVICT);
    act("$n gropes at you to drain your vitality!", ch, NULL, victim,TO_VICT);

    if (number_percent() > skill){
      damage(ch,victim, 0, gsn_lifedrain, DAM_INTERNAL,TRUE);
      check_improve(ch,gsn_lifedrain,FALSE,1);
      return;
    }
/* HIT */
    if (IS_UNDEAD(victim)
	|| check_immune(victim, DAM_DISEASE, TRUE) <= IS_RESISTANT){
      act("$n seems to be unaffected.", victim, NULL, victim, TO_ROOM);
      act("Luckily you are immune to that.", ch, NULL, victim, TO_VICT);
      set_fighting( ch, victim );
      if (!victim->fighting)
	set_fighting( victim, ch );
      return;
    }
    damage(ch,victim, dam, gsn_lifedrain, DAM_INTERNAL,TRUE);

    /* undead feed here */
    if (!IS_NPC(ch)
	&& ch->pcdata->condition[COND_HUNGER] < 40
      && ch->race == race_lookup("undead")){
      int condition = ch->pcdata->condition[COND_HUNGER];
      gain_condition( ch, COND_HUNGER, (15 * victim->level / ch->level));
      gain_condition( ch, COND_THIRST, (15 * victim->level / ch->level));
      if ( condition <= 0 && ch->pcdata->condition[COND_HUNGER] > 0 )
	send_to_char( "You are no longer hungry.\n\r", ch );
      else if ( ch->pcdata->condition[COND_HUNGER] > 40 )
	send_to_char( "You are full.\n\r", ch );
    }
    /* 35 and up we life drain */
    if (ch->level > 35){
      ch->hit = UMIN(ch->max_hit, ch->hit + dam );
      if (victim->mana - dam > 0 && !IS_NPC(victim)){
	ch->mana = UMIN(ch->max_mana, ch->mana + dam );
      }
      victim->mana = UMAX(0, victim->mana - dam );
      update_pos( ch );
    }

/* check for paralyze 45 and up */
    if (ch->level >= 45
	&& !saves_spell(ch->level + mod, victim, DAM_OTHER,
			skill_table[gsn_lifedrain].spell_type)
	&& !is_affected(victim, gsn_paralyze)
	&& !is_affected(victim, gsn_ghoul_touch)){
      AFFECT_DATA af;
      act("$n has been paralyzed!", victim, NULL, NULL, TO_ROOM);
      act("You have been paralyzed!", victim, NULL, NULL, TO_CHAR);

      af.type      = gsn_paralyze;
      af.level     = ch->level;
      af.duration  = 1;
      af.where     = TO_AFFECTS;
      af.bitvector = 0;
      af.location  = APPLY_NONE;
      af.modifier  = 5;
      affect_to_char( victim, &af );
    }
    sendf(victim, "You lose %d experience!\n\r", exp_dr);
    gain_exp(victim, -exp_dr);
    sendf(ch, "You gain %d experience!\n\r", exp_dr);
    gain_exp(ch, exp_dr);
    check_improve(ch,gsn_lifedrain,TRUE,1);

    if (!paf){
      af.type 	= gsn_lifedrain;
      af.level 	= ch->level;
      af.duration = 5;

      af.where	= TO_AFFECTS;
      af.bitvector = 0;
      af.location	= APPLY_HIT_GAIN;
      af.modifier = -5;
      affect_to_char(victim,&af);
      af.location	= APPLY_MANA_GAIN;
      af.modifier = -5;
      affect_to_char(victim,&af);
      af.location	= APPLY_MOVE_GAIN;
      af.modifier = -5;
      affect_to_char(victim,&af);
    }
/* regen penalty 40 and up */
    else if (ch->level >= 40){
      for (paf = victim->affected; paf; paf = paf->next){
	if (paf->type == gsn_lifedrain){
	  paf->modifier -= 5;
	  paf->duration +=2;
	}
      }
    }
  }//END SUCCESS
  else
    {
      if (ch->fighting != victim && victim->fighting != ch)
	{
	  sprintf(buf, "Your foul touch is useless %s!",PERS(ch,victim));
	  j_yell(victim,buf);
	}
      if (get_avatar_level(victim) >= 1){
	act("$n seems unaffected by your attempts.", ch, NULL, victim, TO_CHAR);
	act("$g's power protects you against $N's touch.", victim, NULL, ch, TO_CHAR);
	divfavor_gain(victim,  -res_cost);
      }
      else{
	act("Some power protects $n against your attempts!", ch, NULL, victim, TO_CHAR);
	act("Your divine strength protects you against $N's touch.", victim, NULL, ch, TO_CHAR);
      }
      damage(ch,victim, 1, gsn_lifedrain, DAM_INTERNAL,TRUE);
      check_improve(ch,gsn_lifedrain,FALSE,1);
    }//EDN FAIL
}


/* CRUSADER SKILLS */

/* Shoulder Rush */
/* Advanced Kick replacement with extra effects */
void do_shoulder_smash( CHAR_DATA *ch, char *argument ){
    CHAR_DATA *victim;
    OBJ_DATA* obj = NULL;
    const int sn = skill_lookup("shoulder smash");
    int chance,dam;
    const int cost = skill_table[sn].min_mana;

    bool fSpiked = FALSE;

    if ( (chance = get_skill(ch, sn)) < 1){
      send_to_char("Huh?\n\r", ch);
      return;
    }
    if (IS_NULLSTR(argument) && !ch->fighting){
      send_to_char("Shoulder smash who?\n\r", ch);
      return;
    }

    /* get target */
    if (IS_NULLSTR(argument) && ch->fighting)
      victim = ch->fighting;
    else if ( ( victim = get_char_room(ch, NULL, argument )) == NULL ){
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }

    /* Safeties */
    if ( victim == ch )
    {
      send_to_char("Self mutilation is not your style.\n\r",ch);
      return;
    }

    if (is_safe(ch,victim))
	return;

    if (!IS_NPC(ch) && ch->mana < cost){
      send_to_char("You lack the strength.\n\r", ch);
      return;
    }
    else ch->mana -= cost;

    /* chance */
    chance += 2 * (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX));
    chance += (ch->level - victim->level);
    chance += affect_by_size(ch,victim);
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
/* bonus damage for spiked shoulder/shield/armor */

    if ( !fSpiked && ((obj = get_eq_char( ch, WEAR_ARMS )) != NULL)){
      if (is_name("spiked", obj->name) || is_name("barbed", obj->name))
	fSpiked = TRUE;
    }
    if ( !fSpiked && ((obj = get_eq_char( ch, WEAR_SHIELD )) != NULL) ){
      if (is_name("spiked", obj->name) || is_name("barbed", obj->name))
	fSpiked = TRUE;
    }
    if ( !fSpiked && ((obj = get_eq_char( ch, WEAR_BODY )) != NULL) ){
      if (is_name("spiked", obj->name) || is_name("barbed", obj->name))
	fSpiked = TRUE;
    }

    dam = number_range( ch->level / 2, 3 * ch->level / 2);

    if (fSpiked && obj){
      sendf(ch, "You make excellent use of %s and draw blood.\n\r",
	    obj->short_descr);
      dam = URANGE(50, 3 * dam / 2, 120);
    }
    WAIT_STATE2( ch, skill_table[sn].beats );

    a_yell(ch, victim );
    //predict interdict
    if (predictCheck(ch, victim, "shoulder", skill_table[sn].name))
      return;

    if ( number_percent( ) < chance ){
      AFFECT_DATA af;
      OBJ_DATA* obj;

      af.type = sn;
      af.level = ch->level;
      af.duration = 0;
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;

      act("$n smashes into you with $s armored shoulder!",
	  ch, NULL, victim, TO_VICT);
      act("$n smashes into $N with $s armored shoulder!",
	  ch, NULL, victim, TO_NOTVICT);
      act("You smash into $N with your armored shoulder!",
	  ch, NULL, victim, TO_CHAR);

      switch (number_range(0, 10)){
      case 0:
	obj = get_eq_char( victim, WEAR_WIELD );
	act("Your sword arm is crushed!",
	    ch, NULL, victim, TO_VICT);
	act("You crush $N's sword arm!",
	    ch, NULL, victim, TO_CHAR);
	af.where = TO_SKILL;
	af.location = (obj == NULL || obj->value[0] == WEAPON_EXOTIC ?
		       gsn_hand_to_hand :
		       *weapon_table[weapon_lookup(weapon_name(obj->value[0]))].gsn);
	af.modifier = -5;
	break;

      case 1:
	act("Your wrist is crushed!",
	    ch, NULL, victim, TO_VICT);
	act("You crush $N's wrist!",
	    ch, NULL, victim, TO_CHAR);
	af.where = TO_SKILL;
	af.location = gsn_parry;
	af.modifier = -5;
	break;
      case 2:
	act("Your chest almost caves in!",
	    ch, NULL, victim, TO_VICT);
	act("You knock the wind out of $M!",
	    ch, NULL, victim, TO_CHAR);
	af.where = TO_SKILL;
	af.location = gsn_second_attack;
	af.modifier = -15;
	break;
      case 3:
	if ( ( obj = get_eq_char( victim, WEAR_SHIELD ) ) != NULL ){
	  act("$n slams against your shield",
	      ch, NULL, victim, TO_VICT);
	  act("You slam against $N's shield",
	    ch, NULL, victim, TO_CHAR);
	  shield_disarm(ch, victim);
	  break;
	}
      case 4:
	if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) != NULL){
	  act("$n slams against your weapon",
	      ch, NULL, victim, TO_VICT);
	  act("You slam against $N's weapon.",
	    ch, NULL, victim, TO_CHAR);
	  disarm(ch, victim);
	  break;
	}
      default:
	break;
      }
      /* apply if somethign chagned */
      if (af.location != APPLY_NONE)
	affect_join(victim, &af);
      check_improve(ch, sn, TRUE, 1);
    }
/* miss */
    else{
      dam = 0;
      check_improve(ch, sn, TRUE, 2);
    }
    if (ch->fighting != victim && victim->fighting != ch){
      char buf[MIL];
      sprintf(buf, "Help! %s just smashed into me!",PERS(ch,victim));
      j_yell(victim,buf);
    }
    damage(ch, victim, dam, sn, DAM_BASH, TRUE);
}


void armored_rush(CHAR_DATA* ch, CHAR_DATA*  victim){
  char buf[MSL];
  OBJ_DATA* obj = get_eq_char(ch, WEAR_WIELD);
  int dam_type = obj ? attack_table[obj->value[3]].damage : DAM_BASH;
  int dt = obj ? TYPE_NOBLOCKHIT + obj->value[3] : gsn_armored_rush;
  int chance = 4 * get_skill(ch, gsn_armored_rush) /5;
  int dam = number_range(10 + ch->level/2, ch->size *  ch->level / 2);
  int vic_lag  = PULSE_VIOLENCE;
  int ch_lag = skill_table[gsn_armored_rush].beats;
  int temp = 0;

//bools
  bool fSuccess = FALSE;
  bool fYell = FALSE;

//check if target should yell.
  fYell = (ch->fighting != victim && victim->fighting != ch);
//sound attack to justice or imm
  a_yell(ch,victim);

  chance += get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_DEX);
  chance += GET_AC2(victim,AC_BASH) /25;
  chance -= get_skill(victim,gsn_dodge)/20;
  chance += (ch->level - victim->level);
  chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));

/* check for crusade */
  if ( (temp = check_crusade(ch, victim)) != CRUSADE_NONE){
    /* if attacker is on crusade against victim */
    if (temp == CRUSADE_MATCH)
      chance += 5;
    /* if attacker is on crusade but not agaisnt victim */
    else if (temp == CRUSADE_NOTMATCH)
      chance -= 5;
  }
  else if ( (temp = check_avenger(ch, victim)) != CRUSADE_NONE){
    if (temp == CRUSADE_MATCH)
      chance += 10;
    else if (temp == CRUSADE_NOTMATCH)
      chance -= 10;
  }

  if ( ch->race == race_lookup("avian")
       || victim->race == race_lookup("faerie") )
    chance -=5;
  if ( victim->class == class_lookup("thief") )
    chance -= 5;
  if (is_affected(victim,gsn_horse))
    chance -= 5;
  chance += affect_by_size(ch,victim);

  if (number_percent() < chance){
    vic_lag  = URANGE(1, number_range(1, ch->size - victim->size + 3), 3)
      * PULSE_VIOLENCE;
    fSuccess = TRUE;
  }
  else
    dam = 0;

//MANTIS CHECK
  if (is_affected(victim,gsn_mantis) && fSuccess){
    act("You grab hold of $n, and throw $m to the ground!",ch,NULL,victim,TO_VICT);
    act("$N grabs hold of you, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
    act("$N grabs hold of $n, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE2(ch, URANGE(1, number_range(1, victim->size), 3) * PULSE_VIOLENCE);
    damage(victim,ch,dam,gsn_mantis,DAM_BASH,TRUE);
    affect_strip(victim,gsn_mantis);
    return;
  }//END MANTIS

  //predict interdict
  if (predictCheck(ch, victim, "rush", skill_table[gsn_armored_rush].name))
    return;

//check for lag protect
  if (!do_lagprot(ch, victim, chance, &dam, &vic_lag, gsn_armored_rush, fSuccess, fYell)){
    if (fYell){
      sprintf(buf, "Help! %s just rushed into me!",PERS(ch,victim));
      j_yell(victim,buf);
    }
    if (fSuccess){
      act("With a mighty blow, $n sends you to the ground!",
	  ch,NULL,victim,TO_VICT);
      act("You slam your weapon into $N and send $M flying!",
	  ch,NULL,victim,TO_CHAR);
      act("With a mighty blow, $n sends $N to the ground.",
	  ch,NULL,victim,TO_NOTVICT);
    }
    else {
      act("You miscalculate your blow and fall off balance!",
	  ch,NULL,victim,TO_CHAR);
      act("$n miscalculates $s blow and falls off balance.",
	  ch,NULL,victim,TO_NOTVICT);
      act("You evade $n's blow, causing $m to fall off balance.",
	  ch,NULL,victim,TO_VICT);
    }
  }//END IF NO PROTECTION

  if (!fSuccess || dam == 0 || vic_lag == 0){
    check_improve(ch, gsn_armored_rush, FALSE, 5);
    ch_lag = 3 * ch_lag / 2;
  }
  else
    check_improve(ch, gsn_armored_rush, TRUE, 1);

  WAIT_STATE2(ch, ch_lag);
  if (vic_lag != 0 && dam != 0)
    WAIT_STATE2(victim, vic_lag);
  damage(ch,victim,dam, dt, dam_type, TRUE);
}


/* armored rush */
/* Advanced bash like skill that has a chance to lag whole group */
void do_armored_rush( CHAR_DATA *ch, char *argument ){

/* skill requires that the character be not grouped with
   non charmed characters, and that he is using 2h weapon

   The skill always lags the target, and has a chance to act onto
   other members of the target's group.
*/

  char arg[MIL];
  CHAR_DATA *victim, *vch, *vch_next;
  int rush_chance = 80; //starting chance to hit someone after the
                       //original victim, gets cut in half after each next hit
  one_argument(argument,arg);

  if (get_skill(ch, gsn_armored_rush)  < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_NULLSTR(arg)){
    victim = ch->fighting;
    if (victim == NULL){
      send_to_char("But you aren't fighting anyone!\n\r",ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
      send_to_char("They aren't here.\n\r",ch);
      return;
    }

  if ( victim == ch ){
    send_to_char("Self mutilation is not your style.\n\r",ch);
    return;
  }

  if (is_safe(ch,victim))
    return;

/* check for begin grouped */
  if ( (vch = get_group_room(ch, TRUE)) != NULL){
    act("$N gets in your way!", ch, NULL, vch, TO_CHAR);
    return;
  }

/* check for use of 2h weapon */
  if (has_twohanded(ch) == NULL){
    send_to_char("You must hold your weapon with both hands.\n\r", ch);
    return;
  }

/* all seems go, we now bash the targets in question */
  if (get_group_room(victim, TRUE)){
    act("$s weapon raised high, $n rushes into $N's group.",
	ch, NULL, victim, TO_NOTVICT);
    act("$s weapon raised high, $n rushes into your group.",
	ch, NULL, victim, TO_VICT);
    act("Your weapon raised high, you rush into $N's group.",
	ch, NULL, victim, TO_CHAR);
  }
  else{
    act("$s weapon raised high, $n rushes at $N.",
	ch, NULL, victim, TO_NOTVICT);
    act("$s weapon raised high, $n rushes at you.",
	ch, NULL, victim, TO_VICT);
    act("Your weapon raised high, you rush $N.",
	ch, NULL, victim, TO_CHAR);
  }


  vch = victim->in_room->people;
/* start cycling through the victims */
  for (; vch; vch = vch_next){
    vch_next = vch->next_in_room;
/* start rejecting targets */
    if (vch == victim
	|| !is_same_group(vch, victim)
	|| is_safe_quiet(vch, ch)
	|| !can_see(ch, vch)
	|| number_percent() > rush_chance)
      continue;

/* found target */
    rush_chance /= 2;
    armored_rush(ch, vch);
  }
  /* we always try to hit the victim last in case he dies*/
  armored_rush(ch, victim);
  set_fighting(ch, victim);
}


/* batter */
/* Advanced disarm like skill. Most of the real work is done by gen */
void do_batter( CHAR_DATA *ch, char *argument ){

/* skill check */
    if (get_skill(ch, gsn_batter) < 1){
      send_to_char( "Huh?\n\r", ch );
      return;
    }

/* check for turn off */
    if (is_affected(ch, gen_batter)){
      act("You will no longer batter in combat.", ch, NULL, NULL, TO_CHAR);
      act("$n seems to switch $s weapon's position slightly.", ch, NULL, NULL, TO_ROOM);
      affect_strip(ch, gen_batter);
      return;
    }

/* need 2h weapon */
    if (has_twohanded(ch) == NULL){
      send_to_char("You must hold your weapon with both hands.\n\r", ch);
      return;
    }

    WAIT_STATE2(ch, skill_table[gsn_batter].beats);

/* check for already battering. */
    if (is_affected(ch, gen_batter)){
      send_to_char("You are already trying to shake things up!\n\r", ch);
    }
    else{
/* all we do is attach the gen to ch, rest is all done with in the gen */
      AFFECT_DATA af;
      af.type = gen_batter;
      af.level = ch->level;
      af.duration = 6;
      af.where = TO_NONE;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier = 0;
      affect_to_char(ch, &af);
      act("You will now batter in combat.", ch, NULL, NULL, TO_CHAR);
      act("$n seems to switch $s weapon's position slightly.", ch, NULL, NULL, TO_ROOM);
      return;
    }
}


/* Impale, a strange cross over of push and charge */
/* impale <dir> <victim>			   */
void do_impale( CHAR_DATA *ch, char *argument ){
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim, *vch;
    OBJ_DATA *obj;

    bool fAdv = get_skill(ch, gsn_adv_handling) > 1;
    int chance = 0;
    int roll = 0;
    int door = -1;
    int dam  = number_range(3 * ch->level / 2, 3 * ch->level)  / (fAdv ?  1 :  2);
    int dam_type = DAM_BASH;
    int sn = skill_lookup("impale");

    const int cost = skill_table[sn].min_mana;



    if ((chance = get_skill(ch, sn)) < 2){
      send_to_char("Huh?\n\r",ch);
      return;
    }
    if (IS_NULLSTR(argument)){
      if (fAdv)
        send_to_char("Impale in which direction and who?\n\r",ch);
      else
        send_to_char("Impale who?\n\r",ch);
      return;
    }
    if (fAdv){
      argument = one_argument( argument, arg );
      if (IS_NULLSTR(argument)){
        send_to_char("Impale in which direction and who?\n\r",ch);
	return;
      }

//check for valid direction.
      if      ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
      else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
      else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
      else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
      else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
      else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
      else{
	send_to_char("That's not a valid direction.\n\r", ch);
	door = -1;
      }
    }
    else door = -1;

    if (ch->fighting != NULL)
    {
	send_to_char("Not while you're fighting!\n\r",ch);
	return;
    }
    else if ((victim = get_char_room(ch, NULL, argument)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }
    if ( victim == ch ){
      send_to_char("Your try to impale yourself but the weapon is just too long.\n\r", ch);
      return;
    }
    if ( is_safe( ch, victim ) )
        return;
/* need 2h weapon */
    if ( (obj = has_twohanded(ch)) == NULL){
      send_to_char("You must hold your weapon with both hands.\n\r", ch);
      return;
    }
    if (obj->value[0] != WEAPON_SWORD
	&& obj->value[0] !=  WEAPON_SPEAR
	&& obj->value[0] != WEAPON_POLEARM
	&& get_skill(ch, gsn_con_damage) < 2) {
      send_to_char( "You need a sword, spear or polearm to impale.\n\r", ch );
      return;
    }

/* cannot do it slowed */
    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE)){
      send_to_char("You can't seem to get up enough speed.\n\r",ch);
      return;
    }

    /* check for begin grouped */
    if ( ( vch = get_group_room(ch, fAdv)) != NULL){
      act("$N gets in your way!", ch, NULL, vch, TO_CHAR);
      return;
    }
    if (ch->mana < cost){
      send_to_char("You can't seem to concentrate enough to pull it off.\n\r", ch);
      return;
    }
    else
      ch->mana -= cost;
/* chance calc */
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
      chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
      chance -= 10;
    chance += (ch->level - victim->level);
    chance += affect_by_size(ch,victim);
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);

/* lag */
    WAIT_STATE2( ch, skill_table[sn].beats );

    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "impale", skill_table[sn].name))
      return;

    if (is_affected(victim, sn))
      chance = 0;
    else if (is_affected(victim, gen_watchtower)){
      sendf( ch, "%s is shielded within the Watchtower.\n\r", PERS(victim, ch ));
      chance = 0;
    }
    else{
      AFFECT_DATA af;
      af.type = sn;
      af.level = ch->level;
      af.duration = number_range(0, 2);
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier = 0;
      affect_to_char(victim, &af);
    }
/* get dam type for damages */
    dam_type = obj ? attack_table[obj->value[3]].damage : DAM_BASH;

/* attack */
    roll = number_percent( );
    dam += chance - roll;

    if ( roll < chance || !IS_AWAKE(victim)){
      sprintf(buf, "Help! %s just ran me through!",PERS(ch,victim));
      j_yell(victim,buf);
      check_improve(ch, sn, TRUE, 1);
    }
    else{
        sprintf(buf, "Help! %s tried to impale me!",PERS(ch,victim));
        j_yell(victim,buf);
	dam = 0;
	check_improve(ch, sn, FALSE, 5);
    }
    if (dam  > 0){
      EXIT_DATA *pExit;
      ROOM_INDEX_DATA* from_room = ch->in_room;
      WAIT_STATE2( victim, skill_table[sn].beats );

      /* check for begin grouped */
      if ( (vch = get_group_room(ch, TRUE)) != NULL){
	act("$N gets in your way!", ch, NULL, vch, TO_CHAR);
	return;
      }

      /* now we carry the victim out of the room */
      if (door == -1)
	door = random_door(ch, ch->in_room);
      pExit = ch->in_room->exit[door];
      /* at this point the door is either random or selected */
      if (door < 0 || door >= MAX_DOOR
	  || vch != NULL
	  || IS_NPC(victim)
	  || pExit == NULL
	  || pExit->to_room == NULL){
	act("You impale $N on your weapon but fail to leave quickly enough.",
	    ch, NULL, victim, TO_CHAR);
	act("You cry in agony as $n runs you through with $s weapon!",
	    ch, NULL, victim, TO_VICT);
	act("$n impales $N on $s weapon!",
	    ch, NULL, victim, TO_NOTVICT);
	damage(ch, victim, dam, sn + TYPE_NOBLOCKHIT, dam_type, TRUE);
	return;
      }
/* we have a valid door, make the move */
      act("You impale $N on your weapon and follow through with your charge!",
	  ch, NULL, victim, TO_CHAR);
      act("You cry in agony as $n runs you through with $s weapon!",
	    ch, NULL, victim, TO_VICT);
      act("$n impales $N on $s weapon trailing $S guts behind!",
	  ch, NULL, victim, TO_NOTVICT);
      move_char(ch, door, TRUE);
      if (from_room != ch->in_room){
	char_from_room(victim);
	char_to_room(victim, ch->in_room);
      }
    }
    damage(ch, victim, dam, sn + TYPE_NOBLOCKHIT, dam_type, TRUE);
}

/* MORE CRUSADER SKILLS */
void do_destroy_undead( CHAR_DATA *ch, char *argument ){
  CHAR_DATA *vch, *vch_next;
  AFFECT_DATA af;
  int diff = 0;

/* skill stats */
  const int sn = skill_lookup("destroy undead");
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;
  const int level = ch->level;
  int chance = get_skill(ch, sn);

  if (chance < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (!IS_GOOD(ch))
    {
      send_to_char("Only those of utmost holy faith can turn undead.\n\r",ch);
      return;
    }
  if (ch->mana < cost || is_affected(ch, sn)){
    send_to_char("You lack the strength to perform the ritual.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;
  WAIT_STATE2(ch, lag);

  if (number_percent() > chance){
    act("You brandish the holy symbol but fumble and drop the reagents!", ch, NULL, NULL, TO_CHAR);
    act("$n brandishes $s holy symbol but fumbles and drops the reagents!", ch, NULL, NULL, TO_ROOM);
    check_improve( ch, sn, FALSE, 5);
    return;
  }
  else{
    act("$n brandishes $s holy symbol and spills forth strange reagents!",ch,NULL,NULL,TO_ROOM);
    act("You brandish your holy symbol and throw potent powders into the air!",ch,NULL,NULL,TO_CHAR);
    /* skill over 75 becomes the bonus */
    chance = UMAX(0, chance - 75);
    check_improve( ch, sn, TRUE, 1);
  }
  for (vch = ch->in_room->people; vch != NULL; vch = vch_next){
    vch_next = vch->next_in_room;
    if (vch != ch && !is_area_safe(ch,vch) && IS_SET(vch->act,ACT_UNDEAD) ){
      diff = vch->level - level;
      if (diff > 25)
	damage(ch,vch,0,sn,DAM_HOLY,TRUE);
      else if (!IS_NPC(vch)){
	int dam_pc = dice(level + chance, 4);
	if (is_affected(vch,sn)){
	  act("$n seems to resist the effect!", vch, NULL, NULL, TO_ROOM);
	  act("You resist the effect!", vch, NULL, NULL, TO_CHAR);
	  continue;
	}
	m_yell(ch,vch,FALSE);
	if (!saves_spell(level + 2,vch,DAM_HOLY,skill_table[sn].spell_type) )
	  damage(ch,vch,dam_pc,sn,DAM_HOLY,TRUE);
	else
	  damage(ch,vch, dam_pc / 2,sn,DAM_HOLY,TRUE);
	af.where     = TO_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  = 2;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = 0;
	affect_to_char( vch, &af );
      }
      else if (diff > 9){
	if (!saves_spell(level,vch,DAM_HOLY,skill_table[sn].spell_type) )
	  damage(ch,vch,dice(level + (chance / 2),14)/diff,sn,DAM_HOLY,TRUE);
	else
	  damage(ch,vch,10,sn,DAM_HOLY,TRUE);
      }
      else if (diff >= -9){
	if (!saves_spell(level,vch,DAM_HOLY,skill_table[sn].spell_type) )
	  damage(ch,vch,dice(2 * (level + chance),7),sn,DAM_HOLY,TRUE);
	  else
	    damage(ch,vch,dice(2 * (level + chance),7)/2,sn,DAM_HOLY,TRUE);
      }
	else if (diff < -9){
	  if (!saves_spell(level,vch,DAM_HOLY,skill_table[sn].spell_type) && IS_NPC(vch)){
	    act("$N disintegrates into a pile of smoldering ashes!",ch,NULL,vch,TO_ALL);
	    act( "$n is DEAD!!", vch, NULL, NULL, TO_ROOM );
	    act_new( "You have been KILLED!!", vch, NULL, NULL, TO_CHAR, POS_DEAD );
	    raw_kill(ch, vch);
	    do_autos(ch);
	  }
	  else
	    damage(ch,vch,dice(2 * (level + chance / 2), 5),sn,DAM_HOLY,TRUE);
	}
      }
  }
/* set waitstate on user */
  af.type = sn;
  af.level = level;
  af.duration = 6;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.modifier = 0;
  af.location = APPLY_NONE;
  affect_to_char(ch, &af);
}

/* starts the whole crusade process */
void do_crusade( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af;
  CABAL_DATA* temp_cab = NULL;

  char arg[MIL];

  int choice = 0;
  int temp = 0;
  int type = CRUSADE_RACE;
  int cost = skill_table[gsn_crusade].min_mana;
  int chance = 0;

  const int dur = 60;

  bool fOut = FALSE;

  argument = one_argument(argument, arg);

/* check for syntax */
  if (IS_NULLSTR(arg)){
    send_to_char("You may Crusade against a particular class, race, or "\
		 "cabal.\n\rUse: crusade <name> to announce your intent.\n\r",
		 ch);
    return;
  }

/* check for misc things */
  if ( (chance = get_skill(ch,gsn_crusade)) < 1){
    send_to_char( "Huh?\n\r", ch );
    return;
  }
  if (IS_IMMORTAL(ch) && get_trust(ch) < IMPLEMENTOR){
    send_to_char("Not by imms.\n\r", ch);
    return;
  }
  if (is_affected(ch, gen_crusade)){
    send_to_char("You are already on a crusade.\n\r",ch);
    return;
  }

  if (ch->mana < cost){
    send_to_char("You should rest first before this mighty task.\n\r",ch);
    return;
  }
  ch->mana -= cost;
  WAIT_STATE2(ch,skill_table[gsn_crusade].beats);

  if (IS_AFFECTED(ch,AFF_CALM)){
    send_to_char("You can't get worked up in your calm state.\n\r",ch);
    return;
  }

  if (number_percent() > chance){
    send_to_char("You failed to start on your crusade.\n\r",ch);
    check_improve(ch,gsn_crusade,FALSE, 0);
    return;
  }
  else
    check_improve(ch,gsn_crusade,TRUE, 0);




/* start checking the choice in order of:
- avatar
- race
- class
- cabal
*/

  if (!str_cmp(arg, "avatar"))
    choice = -1;
  else{
    if ( (temp = race_lookup(arg)) == 0)
      choice = -2;
    else
      choice = temp;

    if (choice == -2){
      type = CRUSADE_CLASS;
      /* class_lookup returns -1 on not found */
      if ( (temp = class_lookup(arg)) == -1)
	choice = -2;
      else
	choice = temp;
    }

    if (choice == -2){
      type = CRUSADE_CABAL;
      if ( (temp_cab = get_cabal(arg)) == NULL)
	choice = -2;
      else
	choice = get_parent(temp_cab)->vnum;
    }
  }
  if (choice == -2 ){
    send_to_char("This does not appear to be a race, class or a cabal.\n\r", ch);
    return;
  }

/* DEBUG
  sendf(ch, "you have chosen: %s\n\r",
	type == CRUSADE_CLASS ? class_table[choice].name:
	type == CRUSADE_CABAL ? temp_cab->name:
	type == CRUSADE_RACE ? (choice == -1 ? "avatar" : race_table[choice].name) : "unknown");
*/

  /* now we attach the gsn */
  af.type = gen_crusade;
  af.level = type;
  af.duration = dur;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = choice;

  if (!IS_NPC(ch)){
    if (type == CRUSADE_CLASS
	&& (choice == ch->class
	    || choice == class_lookup("healer")
	    || choice == class_lookup("paladin")) )
      fOut = TRUE;
    else if (type == CRUSADE_RACE
	     && (choice == -1 || pc_race_table[choice].align == ALIGN_GOOD)
	     )
      fOut = TRUE;
  }

  if (fOut && str_cmp(argument, "confirm")){
    send_to_char("Are you sure you wish to do this?\n\r"\
		 "Use \"crusade <name> confirm\" to confirm your choice.\n\r", ch);
    return;
  }
  else if (fOut && !str_cmp(argument, "confirm")){
    send_to_char("How dare you even contemplate such heresy!\n\r", ch);
    if (!IS_SET(ch->act, PLR_OUTCAST)){
      char buf[MIL];
      SET_BIT(ch->act, PLR_OUTCAST);
      send_to_char( "You have been branded as an outcast!\n\r", ch );
      sprintf(buf, "$N has been outcasted for crusading against his own.");
      wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE, LEVEL_HERO);
      log_event(ch, buf);
    }
  }
/* rest is done in gen init function */
  affect_to_char(ch, &af);
}


/* starts the whole avenger process */
/* can only be used on a corpse with avenger affect */
void do_avenger( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af, *paf;
  OBJ_DATA* obj;
  CHAR_DATA* vch;

  char arg[MIL];
  char buf[MIL];

  int sn = skill_lookup("avenger");
  int cost = skill_table[sn].min_mana;
  int chance = 0;

  const int dur = 120;

  argument = one_argument(argument, arg);

/* check for skill things */
  if ( (chance = get_skill(ch, sn)) < 1){
    send_to_char( "Huh?\n\r", ch );
    return;
  }
/* check for syntax */
  if (IS_NULLSTR(arg)){
    send_to_char("You may avenge a death based on remains.\n\r"\
		 "Use \"avenge <corpse>\"\n\r", ch);
    return;
  }
/* look for corpse */
  if ( (obj = get_obj_list( ch, arg, ch->in_room->contents ) ) == NULL){
    send_to_char( "You can't find it.\n\r", ch );
    return;
  }
/* make sure its pc's */
  if (obj->item_type != ITEM_CORPSE_PC){
    send_to_char("You may only avenge deaths of players.\n\r",ch);
    return;
  }
/* make sure we have an avenger effect */
  if ( (paf = affect_find(obj->affected, sn)) == NULL
       || !paf->has_string){
    send_to_char("Not on this corpse\n\r.", ch);
    bug("do_avenger: PC corpse with no avenger effect!", obj->pIndexData->vnum);
    return;
  }

  if (is_affected(ch, gen_avenger)){
    send_to_char("You are already on a quest of holy revenge.\n\r",ch);
    return;
  }

  if (ch->mana < cost){
    send_to_char("You should rest first before this mighty task.\n\r",ch);
    return;
  }
  ch->mana -= cost;
  WAIT_STATE2(ch,skill_table[sn].beats);

  if (IS_AFFECTED(ch,AFF_CALM)){
    send_to_char("You can't get worked up in your calm state.\n\r",ch);
    return;
  }

  if (number_percent() > chance){
    send_to_char("You failed to start on your quest of divine retribution.\n\r",ch);
    check_improve(ch, sn, FALSE, 5);
    return;
  }
  else
    check_improve(ch, sn, TRUE, 0);

/* try to find the original culprit
   since npcs count too, this will have to be done the hard way
  for npcs  (modifier == 0 for pc)
*/
  if (paf->modifier){
    AFFECT_DATA* baf;
    bool fFound = FALSE;
/* in case of npcs we can match which particular npc killed
   the person by the id stored in paf->modifier on corpse and npc
*/
    for ( vch = char_list; vch != NULL ; vch = vch->next ){
      if ( vch->in_room == NULL )
	continue;
      if (!is_name(paf->string, vch->name))
	continue;
      /* now we check if this mob killed this corpse */
      for (baf = vch->affected; baf != NULL; baf = baf->next){
	if (baf->type == sn
	    && baf->modifier == paf->modifier)
	  fFound = TRUE;
      }
      if (fFound)
	break;
    }
  }
  else
    vch = get_char(paf->string);
/* now we check the victim */
  if (vch == NULL){
    send_to_char("The vile murder seems to have disappeared from the lands!\n\r", ch);
    return;
  }
  if (vch == ch){
    send_to_char("Now that would not make sense would it?\n\r", ch);
    return;
  }
  if (ch->id == obj->owner){
    send_to_char("You cannot avenge yourself!\n\r", ch);
    return;
  }
  if (!IS_NPC(vch) && !is_pk_abs(ch, vch)){
    act("$N is protected from your rightous hand by power of $G.",
	ch, NULL, vch, TO_CHAR);
    return;
  }
/* set not quit on person for some 24 ticks */
  af.type = gsn_noquit;
  af.level = ch->level;
  af.duration = 24;
  af.where = TO_NONE;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(vch, &af);

  /* now we attach the gsn */
  af.type = gen_avenger;
  af.level = ch->level;
  af.duration = dur;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = paf->modifier;
  paf = affect_to_char(ch, &af);
  string_to_affect(paf, vch->name);

/* since the string takes effect after gen init, we do the yell here */
  sprintf(buf,"By the Divine Right of %s's power, %s will pay for this deed!",
	  IS_NPC(ch) ? "The One God" : ch->pcdata->deity, paf->string);
  do_yell(ch, buf);
}

/* permament version of windmill, on/off toggle */
void do_windmill( CHAR_DATA *ch, char *argument ){
/* skill requires that the character be not grouped with
   non charmed characters, and that he is using 2h weapon

   The skill causes the user to swing till turned off
*/
  AFFECT_DATA af;
  OBJ_DATA* obj;
  CHAR_DATA* vch;
  int skill = get_skill(ch, gsn_windmill);
  const int sn = gsn_windmill;
  const int cost = skill_table[gsn_windmill].min_mana;
  const int lag = skill_table[gsn_windmill].beats;

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (is_affected(ch, gsn_windmill) && IS_NULLSTR(argument)){
    send_to_char("You let your body relax and hold your weapon in more conventional way.\n\r", ch);
    act("$n seems to relax and holds $s weapon in more conventional way.",
	ch, NULL, NULL, TO_ROOM);
    WAIT_STATE2(ch, 3 * lag / 2);
    affect_strip(ch, gsn_windmill);
    return;

  }
/* check for begin grouped */
  if ( (vch = get_group_room(ch, FALSE)) != NULL){
    act("$N gets in your way!", ch, NULL, vch, TO_CHAR);
    return;
  }

/* check for use of 2h weapon */
  if ( (obj = has_twohanded(ch)) == NULL){
    send_to_char("You must hold your weapon with both hands.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE)){
    send_to_char("You can't seem to get up enough speed.\n\r",ch);
    return;
  }
  if (ch->mana < cost){
    send_to_char("You can't seem concentrate enough.\n\r",ch);
    return;
  }
  else
    ch->mana -= cost;
  WAIT_STATE2(ch, lag);

  if (ch->fighting == NULL && !IS_NULLSTR(argument)){
    /* check for righteousness */
    if (is_affected(ch, gsn_righteous)){
      CHAR_DATA* vch, *vch_next;
      char buf[MIL];
      act("The air singing under $p you swing about you madly!",
	  ch, obj, NULL, TO_CHAR);
      act("The air singing under $p $n swings about $mself madly!",
	  ch, obj, NULL, TO_ROOM);
      for (vch = ch->in_room->people; vch != NULL; vch = vch_next){
	vch_next = vch->next_in_room;
	if ( vch != ch && !is_area_safe(ch, vch)){
	  if (ch->fighting != vch && vch->fighting != ch){
	    a_yell(ch,vch);
	    sprintf(buf, "Help! %s just swung right into me!",PERS(ch,vch));
	    j_yell(vch,buf);

	  }
	  damage(ch, vch, UMIN(ch->level, ch->level*2), obj->value[3] + TYPE_HIT, attack_table[obj->value[3]].damage, TRUE);
	}
      }
      return;
    }
  }
  if (IS_AFFECTED2(ch, AFF_RAGE) || is_affected(ch, gsn_windmill)){
    send_to_char("You are already fighting with reckless abandonment.\n\r",ch);
    return;
  }

/* all seems go, we set the effect, duration is 6
unlike windmill, here we set modifier to < 0 and it will not
get checked.
*/
  if (is_affected(ch, sn)){
    send_to_char("You are already prepared to cut down anything in near proximity!\n\r", ch);
    return;
  }
  if (ch->fighting)
    skill += 30;
  if (number_percent() > 4 * skill / 5){
    send_to_char("You fail to enrage yourself sufficiently.\n\r", ch);
    check_improve(ch, sn, FALSE, 0);
    return;
  }
  check_improve(ch, sn, TRUE, 1);

  act("Battlelust floods your mind as you prepare to windmill $p.",
      ch, obj, NULL, TO_CHAR);
  act("Bloodlust fills $n's eyes as $e switches grip on $p.",
      ch, obj, NULL, TO_ROOM);

  affect_strip(ch, gsn_windmill);
  af.type = gsn_windmill;
  af.duration = 6;
  af.level = ch->level;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_DAMROLL;
  af.modifier = number_fuzzy(ch->level / 10);
  affect_to_char(ch, &af);
}



void do_high( CHAR_DATA *ch, char *argument ){

  if (get_skill(ch, gsn_2h_tactics) < 1 && get_skill(ch, gsn_battlestance) < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (get_skill(ch, gsn_battlestance) > 1 && !monk_good(ch, WEAR_LEGS)){
    send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
    return;
  }
  if (IS_NPC(ch)){
    send_to_char("PC only\n\r", ch);
    return;
  }
  if (ch->pcdata->wep_pos == WEPPOS_HIGH){
    send_to_char("Your weapon is already at high level.\n\r", ch);
    return;
  }
  else
    WAIT_STATE2(ch, skill_table[gsn_2h_tactics].beats);
  send_to_char("You raise your weapon high.\n\r", ch);
  act("$n raises $s weapon to a high position.", ch, NULL, NULL, TO_ROOM);
  ch->pcdata->wep_pos = WEPPOS_HIGH;
}

void do_low( CHAR_DATA *ch, char *argument ){

  if (get_skill(ch, gsn_2h_tactics) < 1 && get_skill(ch, gsn_battlestance) < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (IS_NPC(ch)){
    send_to_char("PC only\n\r", ch);
    return;
  }
  else if (get_skill(ch, gsn_battlestance) > 1 && !monk_good(ch, WEAR_LEGS)){
    send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
    return;
  }
  if (ch->pcdata->wep_pos == WEPPOS_LOW){
    send_to_char("Your weapon is already at low level.\n\r", ch);
    return;
  }
  else
    WAIT_STATE2(ch, skill_table[gsn_2h_tactics].beats);
  send_to_char("You lower your weapon to a defensive position.\n\r", ch);
  act("$n lowers $s weapon to a defensive position.", ch, NULL, NULL, TO_ROOM);
  ch->pcdata->wep_pos = WEPPOS_LOW;
}

void do_normal( CHAR_DATA *ch, char *argument ){

  if (IS_NPC(ch)){
    send_to_char("PC only\n\r", ch);
    return;
  }
  if (ch->pcdata->wep_pos == WEPPOS_NORMAL){
    send_to_char("Your weapon is already at normal level.\n\r", ch);
    return;
  }
  else
    WAIT_STATE2(ch, skill_table[gsn_2h_tactics].beats);
  if (ch->pcdata->wep_pos == WEPPOS_LOW){
    send_to_char("You raise your weapon to normal level.\n\r", ch);
    act("$n raises $s weapon to normal level.", ch, NULL, NULL, TO_ROOM);
  }
  else {
    send_to_char("You lower your weapon to normal level.\n\r", ch);
    act("$n lowers $s weapon to normal level.", ch, NULL, NULL, TO_ROOM);
  }

  ch->pcdata->wep_pos = WEPPOS_NORMAL;
}

/* trip with damage */
void do_cutter( CHAR_DATA *ch, char *argument ){
    OBJ_DATA* obj = get_eq_char(ch, WEAR_WIELD);
    CHAR_DATA *victim, *vch;

    char buf[MSL];

    int chance = 0, dam = number_range(ch->level, 3 * ch->level / 2);
    const int dam_type = obj ? attack_table[obj->value[3]].damage : DAM_BASH;
    int attack_sn = obj ? obj->value[3] : attack_lookup("slash");
    int gsn_cutter = skill_lookup("cutter");

/* check for skill */
    if ( (chance = get_skill(ch, gsn_cusinart)) < 1){
      if ( (chance = get_skill(ch,gsn_cutter)) < 1){
	send_to_char("Huh?\n\r",ch);
	return;
      }
    }
/* get target */
    if (IS_NULLSTR(argument)){
      victim = ch->fighting;
      if (victim == NULL){
	send_to_char("But you aren't fighting anyone!\n\r",ch);
	return;
      }
    }
    else if ((victim = get_char_room(ch, NULL, argument)) == NULL){
      send_to_char("They aren't here.\n\r",ch);
      return;
    }
/* group check */
    if ( (vch = get_group_room(ch, TRUE)) != NULL){
      act("$N gets in your way!", ch, NULL, vch, TO_CHAR);
      return;
    }
/* check for 2h */
    if (has_twohanded(ch)  == NULL){
      send_to_char("You must hold your weapon with both hands.\n\r", ch);
      return;
    }

/* fly immunity */
    if (IS_AFFECTED(victim,AFF_FLYING)
	&& !is_affected(victim,gsn_thrash)){
      act("You can't reach $S feet!",ch,NULL,victim,TO_CHAR);
      return;
    }
    if ( victim == ch ){
      send_to_char("Ouch!\n\r", ch);
      return;
    }

/* pk check */
    if (is_safe(ch,victim))
      return;
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim ){
      sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
      return;
    }
/* lag */
    WAIT_STATE2(ch ,skill_table[gsn_cutter].beats);

/* chance (same as trip) */
    chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX))*3;
    chance += (ch->level - victim->level);
    chance -= affect_by_size(ch,victim);
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    if (IS_AFFECTED2(victim,AFF_BLUR))
      chance /= 2;

/* yell out */
    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "cutter", skill_table[gsn_cutter].name))
      return;

    if (number_percent() < chance){
      if (ch->fighting != victim && victim->fighting != ch){
	sprintf(buf, "Help! %s just cleaved my legs!",PERS(ch,victim));
	j_yell(victim,buf);
      }

      act("You swing at $N's legs and send $m to the ground.", ch, NULL, victim, TO_CHAR);
      act("$n swings at $N's legs and send $m to the ground.", ch, NULL, victim, TO_NOTVICT);
      act("$n swings at your legs and send you to the ground.", ch, NULL, victim, TO_VICT);
      WAIT_STATE2(victim, UMIN(2,number_range(victim->size,1)) * PULSE_VIOLENCE);
      damage(ch, victim, dam, attack_sn + TYPE_NOBLOCKHIT, dam_type, TRUE);
      victim->position = POS_RESTING;
      check_improve(ch,gsn_cutter,TRUE,1);
      check_improve(ch,gsn_cusinart, TRUE,1);
    }
    else{
      if (ch->fighting != victim && victim->fighting != ch){
	sprintf(buf, "Help! %s just tried to cleave my legs apart!",
		PERS(ch,victim));
	j_yell(victim,buf);
      }
      damage(ch, victim, 0, attack_sn + TYPE_NOBLOCKHIT, dam_type, TRUE);
      check_improve(ch,gsn_cutter,FALSE,1);
      check_improve(ch,gsn_cusinart,FALSE,1);
    }
}


/*----===== MODIFY =====-----*/
/* counts how many stones a character has, returns number */
int get_max_stone(OBJ_DATA* list){
  OBJ_DATA* obj;
  int tot = 0;
/* ez */
  if (list == NULL)
    return tot;
  for ( obj = list; obj != NULL; obj = obj->next_content ){
    if (obj->pIndexData->vnum == OBJ_VNUM_STARSTONE)
      tot++;
    if (obj->contains)
      tot += get_max_stone(obj->contains);
  }
  return tot;
}
/* returns pointer to the first stone found */
OBJ_DATA* get_stone( OBJ_DATA* list ){
  OBJ_DATA*  obj, *vobj;

  if (list == NULL)
    return NULL;
  for ( obj = list; obj != NULL; obj = obj->next_content ){
    if (obj->pIndexData->vnum == OBJ_VNUM_STARSTONE)
      return obj;
    if (obj->contains && (vobj = get_stone(obj->contains)) != NULL)
      return vobj;
  }
  return NULL;
}
/* find position in amodify table, or 0 for not found */
int amodify_lookup(const char* name){
  int i;
  for (i = 0; amodify_table[i].name != NULL; i++){
    if (LOWER(name[0]) == LOWER(amodify_table[i].name[0])
	&& !str_prefix(name, amodify_table[i].name))
      return i;
  }
  return 0;
}

int amodify_lookup_id(int ident ){
  int i;
  for (i = 0; amodify_table[i].name != NULL; i++){
    if (amodify_table[i].ident == ident )
      return i;
  }
  return 0;
}
/* shows avaliable choices */
void amodify_show(CHAR_DATA* ch, OBJ_DATA* obj, int level){
  AFFECT_DATA* paf;
  const int max = amodify_table[0].level;
  int skip[max];
  char text[max][MIL];
  int last_text = 0;
  int i = 0;
  int bits = 0;

  if ( (paf = affect_find(obj->affected, gsn_basic_armor)) != NULL)
    bits = paf->bitvector;

/* reset the skip table */
  for (i = 0; i < max; i++)
    skip[i] = FALSE;
  /* compose the listing */
  for (i = 0; amodify_table[i].name != NULL; i++){
    int j = 0;
    char* mark;
    if (skip[i] || amodify_table[i].level > level)
      continue;
    skip[i] = TRUE;
    /* we have an entry to show */
    /* create the V or X */
    if (obj->pIndexData->vnum == OBJ_VNUM_JUGGERNAUT)
      mark = "`1X``";
    else if( IS_SET(bits, amodify_table[i].bit))
      mark = "`8V``";
    else
      mark = "`#V``";

    sprintf(text[last_text], "%2d. %-5s %s",
	    last_text + 1,
	    mark,
	    amodify_table[i].name);
    /* check for other bits of same type */
    for (j = i; j < max; j ++){
      char buf[MIL];
      if (skip[j] || amodify_table[j].level > level
	  || amodify_table[i].bit != amodify_table[j].bit)
	continue;
      skip[j] = TRUE;
      sprintf(buf, "%-3s %s", " or", amodify_table[j].name);
      /* attach this choice */
      strcat(text[last_text], buf);
    }
    strcat(text[last_text++], "\n\r");
  }
  if (last_text == 0){
    send_to_char("You have no possible modifications to make.\n\r", ch);
    return;
  }
  else
    send_to_char("You may make following modifications:\n\r", ch);
/* now show the table */
  for (i = 0; i < last_text; i ++)
    send_to_char(text[i], ch);
}

/* checks character inventory for appropriate prerequisites */
bool amodify_req( CHAR_DATA* ch, OBJ_DATA* obj, OBJ_DATA* part, int choice){
  int stones = 0;

/* JUGGERNAUT CHECK */
  if (obj->pIndexData->vnum == OBJ_VNUM_JUGGERNAUT){
    send_to_char("You cannot improve on perfection.\n\r", ch);
    return FALSE;
  }
/* location */
  if (!CAN_WEAR(obj, ITEM_WEAR_HEAD)
      && !CAN_WEAR(obj, ITEM_WEAR_LEGS)
      && !CAN_WEAR(obj, ITEM_WEAR_FEET)
      && !CAN_WEAR(obj, ITEM_WEAR_ARMS)
      && !CAN_WEAR(obj, ITEM_WEAR_HANDS)
      && !CAN_WEAR(obj, ITEM_WEAR_BODY)
      && !CAN_WEAR(obj, ITEM_WEAR_FACE)){
    send_to_char("You may only modify face, torso, arms, hands, legs and feet armor.\n\r", ch);
    return FALSE;
  }
/* start running through the inventory checking things */
  stones = get_max_stone(ch->carrying);

  if (stones < amodify_table[choice].stones){
    sendf(ch, "You need %d starstone%s.\n\r",
	  amodify_table[choice].stones,
	  amodify_table[choice].stones != 1 ? "s" : "");
    return FALSE;
  }
  /* match parts (must be of object level or greater )*/
  if (part == NULL)
    return TRUE;
/* level and conversion item */
  if (amodify_table[choice].level > 1 && amodify_table[choice].ident != 5
      && obj->pIndexData->vnum != OBJ_VNUM_ARMORCRAFT){
    send_to_char("This modification requires the object to be prepared with \"conversion\"\n\r", ch);
    return FALSE;
  }
/* fSame */
  if (amodify_table[choice].fSame){
    AFFECT_DATA* paf;
    int vnum = obj->pIndexData->vnum;
    if (obj->pIndexData->vnum == OBJ_VNUM_ARMORCRAFT){
      if (obj->vnum != obj->pIndexData->vnum)
	vnum = obj->vnum;
      else if ( (paf = affect_find(obj->affected, gsn_basic_armor)) == NULL
		|| paf->level == 0){
	bug("amodify_req: object was ARMORCRAFT without vnum data", vnum);
	return FALSE;
      }
      else
	vnum = paf->level;
    }
    if (vnum != part->pIndexData->vnum){
      send_to_char("This modification requries the part to be the same as the object.\n\r", ch);
      return FALSE;
    }
  }
/* fRare */
  if (amodify_table[choice].fRare && !IS_LIMITED(part)){
    if (amodify_table[choice].fSame)
      send_to_char("This requires the part and the original to be of rare or unique quality.\n\r", ch);
    else
      send_to_char("This requires the part to be of rare or unique quality.\n\r", ch);
    return FALSE;
  }
/* same location */
  if ( (CAN_WEAR(obj, ITEM_WEAR_HEAD) && !CAN_WEAR(part, ITEM_WEAR_HEAD))
       || (CAN_WEAR(obj, ITEM_WEAR_FACE) && !CAN_WEAR(part, ITEM_WEAR_FACE))
       || (CAN_WEAR(obj, ITEM_WEAR_BODY) && !CAN_WEAR(part, ITEM_WEAR_BODY))
       || (CAN_WEAR(obj, ITEM_WEAR_ARMS) && !CAN_WEAR(part, ITEM_WEAR_ARMS))
       || (CAN_WEAR(obj, ITEM_WEAR_HANDS) && !CAN_WEAR(part, ITEM_WEAR_HANDS))
       || (CAN_WEAR(obj, ITEM_WEAR_LEGS) && !CAN_WEAR(part, ITEM_WEAR_LEGS))
       || (CAN_WEAR(obj, ITEM_WEAR_FEET) && !CAN_WEAR(part, ITEM_WEAR_FEET))
       ){
    send_to_char("The part must be same type of armor as the object.\n\r", ch);
    return FALSE;
  }
  if (part->wear_loc != WEAR_NONE){
    send_to_char("The part cannot be worn.\n\r", ch);
    return FALSE;
  }
  if (part->level < (3 * obj->level / 4) && !amodify_table[choice].fSame){
    send_to_char("The part must be at least three quarts of the rank of the object.\n\r", ch);
    return FALSE;
  }
  return TRUE;
}

/* ARMOR MODIFY interpret function */
void do_amodify( CHAR_DATA *ch, OBJ_DATA* obj, char *argument ){
  AFFECT_DATA af, *paf, *mark;
  OBJ_DATA* part = NULL;

  int level = 0;
  int choice = 0;
  int stones = 0;

  char type[MIL];

/* find the choice, first try whole argument, then first part */
  if ( (choice = amodify_lookup(argument)) == 0){
    argument = one_argument(argument, type);
    if ( (choice = amodify_lookup(type)) == 0){
      sendf(ch, "That modification does not seem to exists.\n\r"\
	    "Use \"modify <obj>\" for list of modifications.\n\r");
      return;
    }
  }
  else
    argument = one_argument(argument, type);

/* get level of mods allowed for this char */
  if (get_skill(ch, skill_lookup("history of armaments")) > 0)
    level = 1;
  if (get_skill(ch, gsn_armor_enhance) > 0)
    level = 2;
  if (get_skill(ch, gsn_armorcraft) > 0)
    level = 3;

/* check for part */
  if (amodify_table[choice].fPart){
    if (IS_NULLSTR(argument)){
      send_to_char("This modification requires a part.\n\r", ch);
      return;
    }
    else if ( (part = get_obj_carry(ch, argument, ch)) == NULL){
      sendf(ch, "You don't seem to have a part %s in your inventory.\n\r",
	    argument);
      return;
    }
  }
  if ( (mark = affect_find(obj->affected, gsn_basic_armor)) != NULL
       && IS_SET(amodify_table[choice].bit, mark->bitvector)){
    sendf(ch, "Your previous modifications prevent application of %s.\n\r",
	  amodify_table[choice].name);
    return;
  }
  if (!amodify_req(ch, obj, part, choice))
    return;

  /* use up the part if any */
  if (part && part == obj){
    send_to_char("You cannot use the objects itself as a part!\n\r", ch);
    return;
  }
  if ( part ){
    act("You take $p apart into its basic parts.",
	ch, part, NULL, TO_CHAR);
    act("$n takes $p apart into its basic parts.",
	ch, part, NULL, TO_ROOM);
    obj_from_char( part );
    extract_obj( part );
  }
  /* use up the stones */
  for (stones = 0; stones < amodify_table[choice].stones; stones ++){
    if ( (part = get_stone(ch->carrying)) == NULL){
      bug("amodify: get_max_stone and get_stone did not match.", stones);
      send_to_char("Error counting stones, contanct an Implementor!\n\r", ch);
    }
    if (part->in_obj)
      obj_from_obj( part );
    else if (part->carried_by)
      obj_from_char ( part );
    else
      bug("amodify: extract starstone was not done from object or character", 0 );
      extract_obj ( part );
  }
  if (stones)
    sendf(ch, "You use up %d starstone%s in preparation.\n\r", stones,
	  stones == 1 ? "" : "s");

  affect_strip(ch, gen_acraft);
  /* setup the gen which does all the real work */
  af.type = gen_acraft;
  af.level = level;
  af.duration = amodify_table[choice].dur;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = amodify_table[choice].ident;
  paf = affect_to_char(ch, &af);
  string_to_affect(paf, obj->name);
/* setup an identifier for this item to be checked later */
  af.type = gsn_identify;
  af.level = 60;
  af.duration = 30;
  af.where = TO_NONE;
  af.bitvector = 0;
  af.location = 0;
  af.modifier = number_range(1, 10000);
  affect_strip_obj(obj, gsn_identify);
  affect_to_obj(obj, &af);
  affect_to_char(ch, &af);
}


/* WEAPON MODIFY STUFF */
/* find position in wmodify table, or 0 for not found */
int wmodify_lookup(const char* name){
  int i;
  for (i = 0; wmodify_table[i].name != NULL; i++){
    if (LOWER(name[0]) == LOWER(wmodify_table[i].name[0])
	&& !str_prefix(name, wmodify_table[i].name))
      return i;
  }
  return 0;
}

/* find position in wmodify table, or 0 for not found */
int wmodify_lookup_id(int ident ){
  int i;
  for (i = 0; wmodify_table[i].name != NULL; i++){
    if (wmodify_table[i].ident == ident )
      return i;
  }
  return 0;
}

/* shows avaliable choices based on object and level */
void wmodify_show(CHAR_DATA* ch, OBJ_DATA* obj, int level){
  AFFECT_DATA* paf;
  const int max = wmodify_table[0].level;
  int skip[max];
  char text[max][MIL];
  int last_text = 0;
  int i = 0;
  int hwep = 0, bits = 0;

/* get the object info */
  if ( (paf = affect_find(obj->affected, gen_hwep)) != NULL)
    hwep = paf->level;
  if ( (paf = affect_find(obj->affected, gsn_arms_main)) != NULL)
    bits = paf->bitvector;

/* reset the skip table */
  for (i = 0; i < max; i++)
    skip[i] = FALSE;

  /* compose the listing */
  for (i = 0; wmodify_table[i].name != NULL; i++){
    char* mark;
    int j = 0;
    if (skip[i] || wmodify_table[i].level > level)
      continue;
    skip[i] = TRUE;
    /* we have an entry to show */

    /* create the V or X */
    if (wmodify_table[i].hlevel > hwep)
      mark = "`1X``";
    else if( IS_SET(bits, wmodify_table[i].bit))
      mark = "`8V``";
    else
      mark = "`#V``";

    sprintf(text[last_text], "%2d. %-5s %-25s",
	    last_text + 1,
	    mark,
	    wmodify_table[i].name);
    /* check for other bits of same type */
    for (j = i; j < max; j ++){
      char buf[MIL];
      if (skip[j] || wmodify_table[j].level > level
	  || wmodify_table[i].bit != wmodify_table[j].bit)
	continue;
      skip[j] = TRUE;
      sprintf(buf, "%-3s %-25s", "or", wmodify_table[j].name);
      /* attach this choice */
      strcat(text[last_text], buf);
    }
    strcat(text[last_text++], "\n\r");
  }
  if (last_text == 0){
    send_to_char("You have no possible modifications to make.\n\r", ch);
    return;
  }
  else
    send_to_char("You may make following modifications:\n\r", ch);

/* now show the table coloring as required*/
  for (i = 0; i < last_text; i ++)
    send_to_char(text[i], ch);
}

/* checks character inventory for appropriate prerequisites */
bool wmodify_req( CHAR_DATA* ch, OBJ_DATA* obj, OBJ_DATA* part, int choice){
  int stones = 0;
  AFFECT_DATA* paf;
  int hwep = 0;

/* location */
  if (!CAN_WEAR(obj, ITEM_WIELD) ){
    send_to_char("You may only modify weapons.\n\r", ch);
    return FALSE;
  }
/* start running through the inventory checking things */
  stones = get_max_stone(ch->carrying);

  if (stones < wmodify_table[choice].stones){
    sendf(ch, "You need %d starstone%s.\n\r",
	  wmodify_table[choice].stones,
	  wmodify_table[choice].stones != 1 ? "s" : "");
    return FALSE;
  }

/* level and conversion item */
  if ( (paf = affect_find(obj->affected, gen_hwep)) != NULL)
    hwep = paf->level;

  if (wmodify_table[choice].hlevel > 0
      && obj->pIndexData->vnum != OBJ_VNUM_WEAPONCRAFT){
    send_to_char("This modification requires the weapon to be consecrated.\n\r", ch);
    return FALSE;
  }
  if (wmodify_table[choice].hlevel > hwep){
    sendf(ch,"This modification requires the weapon to be of %d%s circle.\n\r",
	  wmodify_table[choice].hlevel,
	  wmodify_table[choice].hlevel == 1 ? "st." :
	  wmodify_table[choice].hlevel == 2 ? "nd." :
	  wmodify_table[choice].hlevel == 3 ? "rd." :"th.");
    return FALSE;
  }

  /* match parts (must be of object level or greater )*/
  if (part == NULL)
    return TRUE;

/* fRare */
  if (wmodify_table[choice].fRare && !IS_LIMITED(part)){
    send_to_char("This requires the part to be of rare or unique quality.\n\r", ch);
    return FALSE;
  }
/* same location */
  if ( !CAN_WEAR(obj, ITEM_WIELD)){
    send_to_char("The part must be a weapon.\n\r", ch);
    return FALSE;
  }
  if (part->wear_loc != WEAR_NONE){
    send_to_char("The part cannot be worn.\n\r", ch);
    return FALSE;
  }
  if (part->level < (3 * obj->level / 4)){
    send_to_char("The part must be at least three quarts the rank of the weapon.\n\r", ch);
    return FALSE;
  }
  return TRUE;
}

/* WEAPON MODIFY interpret function */
void do_wmodify( CHAR_DATA *ch, OBJ_DATA* obj, char *argument ){
  AFFECT_DATA af, *paf, *mark;
  OBJ_DATA* part = NULL;

  int level = 0;
  int choice = 0;
  int stones = 0;

  char type[MIL];

/* find the choice, first try whole argument, then first part */
  if ( (choice = wmodify_lookup(argument)) == 0){
    argument = one_argument(argument, type);
    if ( (choice = wmodify_lookup(type)) == 0){
      sendf(ch, "That modification does not seem to exists.\n\r"\
	    "Use \"modify <obj>\" for list of modifications.\n\r");
      return;
    }
  }
  else
    argument = one_argument(argument, type);

/* get level of mods allowed for this char */
  if (get_skill(ch, gsn_arms_main) > 0)
    level = 2;
  if (get_skill(ch, gsn_weaponcraft) > 0)
    level = 3;

/* check for part */
  if (wmodify_table[choice].fPart){
    if (IS_NULLSTR(argument)){
      send_to_char("This modification requires a part.\n\r", ch);
      return;
    }
    else if ( (part = get_obj_carry(ch, argument, ch)) == NULL){
      sendf(ch, "You don't seem to have a part %s in your inventory.\n\r",
	    argument);
      return;
    }
  }

  if ( (mark = affect_find(obj->affected, gsn_arms_main)) != NULL
       && IS_SET(wmodify_table[choice].bit, mark->bitvector)){
    sendf(ch, "Your previous modifications prevent application of %s.\n\r",
	  wmodify_table[choice].name);
    return;
  }
  if (!wmodify_req(ch, obj, part, choice))
    return;

  /* use up the part if any */
  if (part && part == obj){
    send_to_char("You cannot use the weapon itself as a part!\n\r", ch);
    return;
  }
  if ( part ){
    act("You take $p apart into its basic parts.",
	ch, part, NULL, TO_CHAR);
    act("$n takes $p apart into its basic parts.",
	ch, part, NULL, TO_ROOM);
    obj_from_char( part );
    extract_obj( part );
  }
  /* use up the stones */
  for (stones = 0; stones < wmodify_table[choice].stones; stones ++){
    if ( (part = get_stone(ch->carrying)) == NULL){
      bug("amodify: get_max_stone and get_stone did not match.", stones);
      send_to_char("Error counting stones, contanct an Implementor!\n\r", ch);
    }
    if (part->in_obj)
      obj_from_obj( part );
    else if (part->carried_by)
      obj_from_char ( part );
    else
      bug("amodify: extract starstone was not done from object or character", 0 );
      extract_obj ( part );
  }

  if (stones)
    sendf(ch, "You use up %d starstone%s in preparation.\n\r", stones,
	  stones == 1 ? "" : "s");

  affect_strip(ch, gen_wcraft);
  /* setup the gen which does all the real work */
  af.type = gen_wcraft;
  af.level = level;
  af.duration = wmodify_table[choice].dur;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = wmodify_table[choice].ident;
  paf = affect_to_char(ch, &af);
  string_to_affect(paf, obj->name);
/* setup an identifier for this item to be checked later */
  af.type = gsn_identify;
  af.level = 60;
  af.duration = 30;
  af.where = TO_NONE;
  af.bitvector = 0;
  af.location = 0;
  af.modifier = number_range(1, 10000);
  affect_strip_obj(obj, gsn_identify);
  affect_to_obj(obj, &af);
  affect_to_char(ch, &af);
}

void wmodify_help( CHAR_DATA* ch, int choice ){
  char buf[MIL];
  char* adj;

  switch (wmodify_table[choice].level){
  default:
  case 0: adj = "common"; break;
  case 1: adj = "journeyman's"; break;
  case 2: adj = "expert's"; break;
  case 3: adj = "master's"; break;
  }
  sprintf(buf, "Weapon modification '%s':\n\r",  wmodify_table[choice].name);
  send_to_char( buf, ch );
  sprintf(buf, "  %s\n\r\n\r", wmodify_table[choice].help);
  send_to_char( buf, ch );
  sprintf(buf, "Requires %s level of skill, %s parts, %d hours and ",
	  adj,
	  wmodify_table[choice].fPart ? wmodify_table[choice].fRare ?
	  "high quality" : "common" : "no",
	  wmodify_table[choice].dur);
  send_to_char( buf, ch );
  if (wmodify_table[choice].stones){
    sendf(ch, "%d starstone%s.\n\r",
	  wmodify_table[choice].stones,
	  wmodify_table[choice].stones == 1 ? "" : "s");
  }
  else
    send_to_char("no starstones.\n\r", ch);
  if (wmodify_table[choice].weight != 0){
    sprintf(buf, "You judge the process will %s the weapons's weight by %d%%.\n\r",
	    wmodify_table[choice].weight >= 0 ? "increase" : "decrease",
	    wmodify_table[choice].weight);
    send_to_char( buf, ch );
  }
  if (wmodify_table[choice].hlevel){
    sprintf(buf, "Only a Consecrated weapon of the %d%s circle can withstand the process.\n\r",
	    wmodify_table[choice].hlevel,
	    wmodify_table[choice].hlevel == 1 ? "st." :
	    wmodify_table[choice].hlevel == 2 ? "nd." :
	    wmodify_table[choice].hlevel == 3 ? "rd." :"th.");
    send_to_char( buf, ch);
  }
  send_to_char("\n\r", ch);
}

void amodify_help( CHAR_DATA* ch, int choice ){
  char buf[MIL];
  char* adj;

  switch (amodify_table[choice].level){
  default:
  case 0: adj = "common"; break;
  case 1: adj = "journeyman's"; break;
  case 2: adj = "expert's"; break;
  case 3: adj = "master's"; break;
  }
  sprintf(buf, "Armor modification '%s':\n\r",  amodify_table[choice].name);
  send_to_char( buf, ch );
  sprintf(buf, "  %s\n\r\n\r", amodify_table[choice].help);
  send_to_char( buf, ch );
  sprintf(buf, "Requires %s level of skill, %s%s parts, %d hours and ",
	  adj,
	  amodify_table[choice].fPart ? amodify_table[choice].fSame ?
	  "matching " : "" : "",
	  amodify_table[choice].fPart ? amodify_table[choice].fRare ?
	  "high quality" : "common" : "no",
	  amodify_table[choice].dur);
  send_to_char( buf, ch );
  if  (amodify_table[choice].stones){
    sendf(ch, "%d starstone%s.\n\r",
	  amodify_table[choice].stones,
	  amodify_table[choice].stones == 1 ? "" : "s");
  }
  else
    send_to_char("no starstones.\n\r", ch);
  if (amodify_table[choice].weight != 0){
    sprintf(buf, "You judge the process will %s the armor's weight by %d%%.\n\r",
	    amodify_table[choice].weight >= 0 ? "increase" : "decrease",
	    amodify_table[choice].weight);
    send_to_char( buf, ch );
  }
  send_to_char("\n\r", ch);
}

/* main modify function, directs onto small functions as needed */
/* sytanx:
   modify <obj>		:shows modifications possible for object
   modify help <name>	:shows help for particular modification
   modify <obj> <name>	:modifies an object when no part is needed
   modif <obj> <name> <part>	:modifies an object using a part
*/
void do_modify( CHAR_DATA *ch, char *argument ){
  char arg[MIL];
  OBJ_DATA* obj;

  if (ch->class != class_lookup("crusader")){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (IS_NULLSTR(argument)){
    send_to_char("What object do you wish to modify, or help on which modification?\n\r", ch);
    return;
  }
  argument = one_argument(argument, arg);

  /* HELP */
  if (!str_prefix(arg, "help")){
    int mod = wmodify_lookup(argument);
    bool fFound = FALSE;

    if (mod != 0){
      fFound = TRUE;
      wmodify_help( ch, mod );
    }
    mod = amodify_lookup(argument);
    if (mod != 0){
      fFound = TRUE;
      amodify_help( ch, mod );
    }
    if (!fFound){
      send_to_char("No help on that modification.\n\r"\
		   "Try \"modify <obj>\" for a list of possible modifications.\n\r", ch);
    }
    return;
  }
/* OBJ */
  if ( (obj = get_obj_carry(ch, arg, ch)) == NULL){
    sendf(ch, "You don't seem to be carrying %s.\n\r", arg);
    return;
  }
  if (obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON){
    send_to_char("You can only modify weapons or armors.\n\r", ch);
    return;
  }
/* SHOW LIST */
  else if (IS_NULLSTR(argument)){
    int level = 0;
    if (obj->item_type == ITEM_WEAPON){
      if (get_skill(ch, gsn_arms_main) > 0)
	level = 2;
      if (get_skill(ch, gsn_weaponcraft) > 0)
	level = 3;
      wmodify_show(ch, obj, level );
      return;
    }
    else if (obj->item_type == ITEM_ARMOR){
      if (get_skill(ch, skill_lookup("history of armaments")) > 0)
	level = 1;
      if (get_skill(ch, gsn_armor_enhance) > 0)
	level = 2;
      if (get_skill(ch, gsn_armorcraft) > 0)
	level = 3;
      amodify_show(ch, obj, level );
      return;
    }
  }
/* do the actual work */
  else{
    CHAR_DATA* vch = get_group_world(ch, TRUE);
    const int lag = skill_table[gsn_armorcraft].beats;
    /* check for being tired */
    if (ch->mana < 3 * ch->max_mana / 5
	|| ch->hit < 3 * ch->hit / 5){
      send_to_char("Your mind and body are too weak to begin.\n\r", ch);
      return;
    }
    if (ch->position < POS_RESTING){
      send_to_char("Nah... You feel too relaxed...\n\r", ch);
      return;
    }
    WAIT_STATE2(ch, lag);
    if (vch){
      act("You begin but soon thoughs of $N's safety break your concentration.", ch, NULL, vch, TO_CHAR);
      return;
    }
    if (obj->item_type == ITEM_ARMOR)
      do_amodify(ch, obj, argument);
    else if (obj->item_type == ITEM_WEAPON)
      do_wmodify(ch, obj, argument);
    else
      do_modify(ch, "");
  }
}

/* allows to choose damage type with control damage skill */
void do_damage( CHAR_DATA *ch, char *argument ){
  int attack_type = 0;
  AFFECT_DATA af, *paf;

  if (get_skill(ch, gsn_con_damage) < 1){
    do_kill(ch, argument);
    return;
  }
  if (IS_NULLSTR(argument)){
    send_to_char("Which damage type do you wish to concentrate on? (slash, blunt, pierce or normal)\n\r", ch);
    return;
  }
  paf = affect_find(ch->affected, gsn_con_damage);

/* parse for damage type */
  if (!str_prefix(argument, "slash"))
    attack_type = attack_lookup("slash");
  else if (!str_prefix(argument, "pierce"))
    attack_type = attack_lookup("pierce");
  else if (!str_prefix(argument, "blunt"))
    attack_type = attack_lookup("slap");
  else
    affect_strip(ch, gsn_con_damage);

  /* if we have chosen a damage type, we set it now */
  if (attack_type){
    sendf(ch, "You will now concentrate on causing %sing blows.\n\r", attack_table[attack_type].noun);
    if (paf)
      paf->modifier = attack_type;
    else{
      af.type = gsn_con_damage;
      af.duration = -1;
      af.level = 60;
      af.where = TO_NONE;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier = 0;
      affect_to_char(ch, &af);
    }
  }
  else
    send_to_char("You stop controlling the damage you cause.\n\r", ch);
  WAIT_STATE2(ch, skill_table[gsn_con_damage].beats);
}


/* Witch compass, allows to sense more or less where the victim is */
void do_w_compass( CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim;
  AFFECT_DATA* paf, af;

  const int dur = 12;		//duration before the effect wears off
  const int uses = 12;		//how many uses in the duration
  const int too_close = 5;	//if closer then this range, then no reading
  const int max_dist = 1024;	//max range for tracking
  const int sn = skill_lookup("witch compass");
  const int lag = skill_table[sn].beats;
  const int cost = skill_table[sn].min_mana;

  int direction = 0;
  int dist = 0;
  int skill = get_skill(ch, sn);

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (is_affected(ch, skill_lookup("calm"))){
    send_to_char("You feel far too content in your calm state.\n\r", ch);
    return;
  }
  if (ch->mana < cost){
    send_to_char("The witch compass seems drained of energy.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;
  WAIT_STATE2(ch, lag);

/* two different modes, one if paf exists one if not */
  paf = affect_find(ch->affected, sn);

/* charge up the compass */
  if (paf == NULL){
    if ( (victim = ch->fighting) == NULL){
      send_to_char("You must be in combat to draw a sample!\n\r", ch);
      return;
    }
    /* try to draw blood */
    act("You draw forth the witch compass and swing its bladed edges!",
	ch, NULL, victim, TO_CHAR);
    act("$n swings a strange clocklike device towards you!",
	ch, NULL, victim, TO_VICT);
    act("$n swings a strange clocklike device towards $N!",
	ch, NULL, victim, TO_NOTVICT);
    check_improve(ch, sn, TRUE, 1);
    if (IS_NPC(ch)){
      send_to_char("The witch compass seems unaffected.\n\r", ch);
      return;
    }
    af.type = sn;
    af.level = ch->level;
    af.duration = dur;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = uses;
    paf = affect_to_char(ch, &af);
    string_to_affect(paf, victim->name);
    damage(ch, victim, 5, sn, DAM_INTERNAL, TRUE);
    return;
  }

/* PATH exists */
  else {
/* cause of drawn blood */
/* if in combat, reset the compass */
  if ( (victim = ch ->fighting) != NULL){
    affect_strip(ch, sn);
    do_w_compass(ch, "");
    return;
  }
/* check remining uses */
  if (--paf->modifier < 1){
    send_to_char("The witch compass has run out of blood.\n\r", ch);
    affect_strip(ch, sn);
    return;
  }
  if (number_percent() > 4 * skill / 5){
    act("You set the witch compass on the ground but try as you might the wierd thing won't even budge.",
	ch, NULL, NULL, TO_CHAR);
    act("$n sets a strange clocklike device on the ground and curses when nothing happends.",
	ch, NULL, NULL, TO_ROOM);
    check_improve(ch, sn, FALSE, 5);
    return;
  }
  else{
    act("You set the witch compass on the ground and activate it.",
	ch, NULL, NULL, TO_CHAR);
    act("$n sets a strange clocklike device on the ground and tinkers with it for a moment.",
	ch, NULL, NULL, TO_ROOM);
    check_improve(ch, sn, TRUE, 1);
  }
/* we have enough uses, try to locate the victim */
  if ( IS_NULLSTR(paf->string)
       || (victim = get_char(paf->string)) == NULL
       || (!IS_UNDEAD(victim) && !IS_DEMON(victim) && !IS_AVATAR(victim)) ){
    send_to_char("The crucifix shaped needle stays perfecly still.\n\r", ch);
    return;
  }
/* check if we can get a path */
  direction = find_first_step(ch->in_room, victim->in_room, max_dist, TRUE, &dist);
  switch(direction){
  case BFS_NO_PATH:
  case BFS_ERROR:
    send_to_char("The crucifix shaped needle stays perfecly still.\n\r", ch);
    return;
  case BFS_ALREADY_THERE:
    send_to_char("The crucifix shaped needle begins to spin about madly!\n\r", ch);
    return;
  }
  if (dist <= too_close){
    send_to_char("The crucifix shaped needle begins to spin about madly!\n\r", ch);
    return;
  }
//  sendf(ch, "dist = %d\n\r", dist);
/* now we know we are not too close, and not too far, we show them the door */
  sendf( ch, "The crucifix needle spins lightly as %s's blood pools on the %s edge!.\n\r", PERS2(victim), dir_name[direction] );
  }
}


/* pommel shamsh, either misdirection or 1 tick blind */
void do_pommel_smash( CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim;
  OBJ_DATA* obj;
  AFFECT_DATA af;

  const int sn = skill_lookup("pommel smash");
  const int skill = get_skill(ch, sn);
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;

  int dam = number_range(35, 55) + UMAX(-30, (get_curr_stat(ch,STAT_STR) - 20) * 10);
  int chance = skill;

  bool fSpike = FALSE;

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if ( (victim = ch->fighting) == NULL){
    send_to_char("But you aren't in combat!\n\r",ch);
    return;
  }

  if ( (obj = has_twohanded(ch)) == NULL){
    send_to_char("You must hold your weapon with both hands.\n\r", ch);
    return;
  }

  if (ch->mana < cost){
    send_to_char("You can't gather enough strength or concentration.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;
  WAIT_STATE2(ch, lag);

/* now we are in combat with two handed weapon, check for spikes */
  if (IS_NULLSTR(argument) && (is_name("spiked", obj->name) || is_name("barbed", obj->name)))
    fSpike = TRUE;

  /* chance to hit */
  chance = chance * (get_weapon_skill_obj(ch, obj) + skill) / 200;
  chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
  chance += (ch->level - victim->level);
  chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
  chance += get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_DEX);
  chance += GET_AC2(victim,AC_BASH) /20;
  chance -= get_skill(victim,gsn_dodge)/20;
  if (victim->race == race_lookup("illithid"))
    chance -=10;
  chance = URANGE(5, chance, 95);
//  sendf(ch, "chance: %d\n\r",chance);

  if (number_percent() > chance){
    damage(ch, victim, 0, sn, DAM_BASH, TRUE);
    check_improve(ch, sn, FALSE, 5);
    return;
  }
  else
    check_improve(ch, sn, TRUE, 1);
/* use chance later for blunt effect again */
/* success, select between spiked and none */
  if (fSpike){
    if (check_immune(victim, DAM_PIERCE, TRUE) < IS_NORMAL)
      chance /= 2;
    act("You plant $p's spikes right into $N's forehead!",
	ch, obj, victim, TO_CHAR);
    act("$n plants $p's spikes right into $N's forehead!",
	ch, obj, victim, TO_NOTVICT);
    act("$n plants $p's spikes right into your forehead!",
	ch, obj, victim, TO_VICT);
    dam  = 5 * dam / 4;
    if (!IS_AFFECTED(victim, AFF_BLIND)){
      AFFECT_DATA af;
      af.type = gsn_blindness;
      af.level = ch->level;
      af.duration = 0;
      af.where = TO_AFFECTS;
      af.bitvector = AFF_BLIND;
      af.location = APPLY_HITROLL;
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	if (is_affected(victim, gsn_battletrance))
	  af.modifier      = 0;
	else
	  af.modifier      = -5;
      }
      else
	af.modifier      = -10;
      affect_to_char(victim,&af);
      af.location     = APPLY_AC;
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	if (is_affected(victim, gsn_battletrance))
	  af.modifier      = 0;
	else
	  af.modifier      = +15;
      }
      else
	af.modifier      = +25;
      affect_to_char(victim,&af);
      act("You are blinded!", victim,NULL,NULL,TO_CHAR);
      act("$n appears to be blinded.",victim,NULL,NULL,TO_ROOM);
    }
    damage(ch, victim, dam, sn, DAM_PIERCE, TRUE);
  }
/* normal blunt */
  else{
    if (check_immune(victim, DAM_BASH, TRUE) < IS_NORMAL)
      chance /= 2;
    if (IS_UNDEAD(victim) || check_immune(victim, DAM_BASH, FALSE) == IS_IMMUNE){
      act("$N seems to shake off the blow!", ch, NULL, victim, TO_NOTVICT);
      act("You easly shake off the blow.", ch, NULL, victim, TO_VICT);
      act("$N seems to not even notice the blow.", ch, NULL, victim, TO_ROOM);
    }
    else if (!is_affected(victim, sn)){
      act("$n smashes $p's pommel across your head dazing and confusing you.",
	  ch, obj, victim, TO_VICT);
      act("$n eyes take on a dazed and confuzed look.",
	  victim, NULL, NULL, TO_ROOM);
      af.where     = TO_AFFECTS2;
      af.type      = sn;
      af.level     = number_range(0,5);
      af.duration  = number_range(0, 1);
      af.location  = APPLY_DEX;
      af.modifier  = -1;
      af.bitvector = AFF_MISDIRECTION;
      affect_to_char( victim, &af );
    }
    damage(ch, victim, dam, sn, DAM_BASH, TRUE);
  }
}

/* one hit kill/damage/bleeding move similiar to cleave. */
void do_behead( CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim;
  OBJ_DATA* obj;
  char buf[MIL];

  const int sn = gsn_behead;
  const int lag = skill_table[sn].beats;
  const int cost = skill_table[sn].min_mana;

  int skill = get_skill(ch, sn);
  int chance = skill;
  int success = number_percent();

  int bleed = 40;	//if success lower then this then bleed


  if (chance < 1){
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (IS_NULLSTR(argument)){
    send_to_char("Rid who's body of their head?\n\r",ch);
    return;
  }
  if (ch->fighting != NULL){
    send_to_char("Not while you're fighting!\n\r",ch);
    return;
  }
  else if ((victim = get_char_room(ch, NULL, argument)) == NULL){
    send_to_char("They aren't here.\n\r",ch);
    return;
  }
  if ( victim == ch ){
    send_to_char("Why not just hang yourself instead?\n\r", ch);
    return;
  }
  if ( is_safe( ch, victim ) )
    return;

  if (IS_NPC(victim) && IS_SET(victim->act, ACT_TOO_BIG) ){
    sendf(ch, "%s is too alert for you to attempt such an act.\n\r", PERS(victim,ch));
    return;
  }

/* need 2h weapon */
  if ( (obj = has_twohanded(ch)) == NULL){
    send_to_char("You must hold your weapon with both hands.\n\r", ch);
    return;
  }
  if (obj->value[0] != WEAPON_SWORD
      && obj->value[0] !=  WEAPON_AXE
      && obj->value[0] != WEAPON_POLEARM
      && get_skill(ch, gsn_con_damage) < 2) {
    send_to_char( "You need a sword, axe or polearm to behead.\n\r", ch );
    return;
  }

/* cannot do it slowed */
  if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE)){
    send_to_char("You can't seem to get up enough speed.\n\r",ch);
    return;
  }
  if (ch->mana < cost){
    send_to_char("You can't concentrate enough to aim well enough.\n\r", ch);
    return;
  }
/* lag */
    WAIT_STATE2( ch, lag );

/* chance calculation */
    chance += (get_curr_stat(ch,STAT_DEX)-get_curr_stat(victim, STAT_DEX)) * 2;
    if (IS_AFFECTED(ch,AFF_HASTE))
      chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
      chance -= 10;
    chance += (ch->level - victim->level);
    chance += affect_by_size(ch,victim);
    chance += (get_curr_stat(ch,STAT_LUCK)-get_curr_stat(victim,STAT_LUCK)) *3;

    if (!IS_AWAKE(victim) ){
      chance += 10;
      success /= 1.5;
    }

/* skill bonus */
    success -= UMAX(0, (skill + get_weapon_skill_obj(ch, obj) - 200) / 6);

//now little bonuses for type of weapon.
    if (obj->value[3] == WEAPON_AXE)
      success -= 1;
/* bless bonus */
    if ( (ch->alignment > GOOD_THRESH)
	 && (IS_OBJ_STAT(obj,ITEM_BLESS)) )
      success -=1;
    else if ( (ch->alignment < EVIL_THRESH)
	      && (IS_OBJ_STAT(obj,ITEM_EVIL)) )
      success -=1;
/* low cone bonus */
    if (get_curr_stat(victim, STAT_CON) < 19)
      success -=1;

    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "behead", skill_table[sn].name))
      return;

    if (is_affected(victim, sn)){
      act("$N moves aside just in time.", ch, NULL, victim, TO_CHAR);
      chance = 0;
      success = 100;
    }
    else{
      AFFECT_DATA af;
      af.type = sn;
      af.level = ch->level;
      af.duration = 3;
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier = 0;
      affect_to_char(victim, &af);
    }


/* we can yell now since its always done out of combat */
    sprintf(buf, "Help! %s just tried to take my head!", PERS(ch,victim));
    j_yell(victim,buf);

/* normal hit with chance to bleed */
    if ( success < chance || !IS_AWAKE(victim) ){
      int dam_type = attack_table[obj->value[3]].damage;
      int dam = 3 * ch->level + 3 * UMAX(0, skill - 75);
      dam += (success < 50 ? ch->level : 0);

      /* counter */
      if (counter_check(ch, victim, get_skill(victim, gsn_counter), dam, dam_type, sn)){
	return;
      }

      /* check for bleeding */
      if (success < bleed){
	if (IS_UNDEAD(victim)){
	  act("$N shreds $n's neck yet not a single drop of blood spings forth!",
	      victim, obj, ch, TO_ROOM);
	  act("$n shreds your neck bur your flesh refuses to bleed.",
	      victim, obj, ch, TO_CHAR);
	}
	else{
	  AFFECT_DATA* paf, af;
	  act("$N shreds $n's neck spraying blood about!",
	      victim, obj, ch, TO_ROOM);
	  act("$N shreds your neck and you begin to bleed profusley.",
	      victim, obj, ch, TO_CHAR);

	  af.type = gen_bleed;
	  af.level = ch->level;
	  af.duration = number_range(3, 6);
	  af.where = TO_NONE;
	  af.bitvector = 0;
	  af.location = APPLY_NONE;
	  /* modifier controls how many individuals bleeds happen (10/tick) */
	  af.modifier = number_range(8, 32);
	  paf = affect_to_char(victim, &af);
	  string_to_affect(paf, ch->name);
	}
      }
      damage( ch, victim, dam, sn, dam_type,TRUE);
      check_improve(ch, sn, TRUE, 5);
    }
    else{
      check_improve(ch, sn, FALSE, 1);
      damage( ch, victim, 0, sn, DAM_BASH,TRUE);
    }
}

/* chance for damage ignoring blows on next round of combat */
void do_armorpierce( CHAR_DATA *ch, char *argument ){
/*
   Skill requires that the character is using 2h weapon
   gives chance for armor piercing blows for next round.
   Only one blow damages armor.
*/

  AFFECT_DATA af;
  OBJ_DATA* obj;
  CHAR_DATA* vch;

  const int cost = skill_table[gsn_apierce].min_mana;
  const int lag = skill_table[gsn_apierce].beats;



  if (get_skill(ch, gsn_apierce)  < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if ( (vch = ch->fighting) == NULL){
    send_to_char("You must be in combat.\n\r", ch);
    return;
  }

/* check for use of 2h weapon */
  if ( (obj = has_twohanded(ch)) == NULL){
    send_to_char("You must hold your weapon with both hands.\n\r", ch);
    return;
  }
  if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE)){
    send_to_char("You can't seem to get up enough speed.\n\r",ch);
    return;
  }

  if (ch->mana < cost){
    send_to_char("You can't seem to aim or strike hard enough.\n\r",ch);
    return;
  }
  WAIT_STATE2(ch, lag);

/* all seems go, we set the effect, duration is 0
which means no armor was damage yet.  Effects gets
stripped at end of combat round.
*/
  act("You being to aim powerful piercing blows at $N's armors.",
      ch, obj, vch, TO_CHAR);
  act("$n seems to be trying to land peculiarly placed blows...",
      ch, obj, NULL, TO_ROOM);

  affect_strip(ch, gen_apierce);
  af.type = gen_apierce;
  af.duration = 0;
  af.level = ch->level;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);
}

/*
allows crusader to enter the chant mode, much like request, where
in the end he casts effects of a psalm on themselfs
only argument is the psalm
*/
void do_chant( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af, *paf;
  int psalm;
  int found = 0;
  int skill = 4 * get_skill(ch, gsn_chant ) / 5;
  bool fTwo = get_skill(ch, skill_lookup("pious"));

  const int cost = skill_table[gsn_chant].min_mana;
  const int lag = skill_table[gsn_chant].beats;

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (IS_NULLSTR(argument)){
    send_to_char("Chant which psalm?\n\r", ch);
    return;
  }

  if (is_affected(ch, gsn_chant)){
    send_to_char("You feel no where near virtous enough to attempt a new chant.\n\r", ch);
    return;
  }
  if (is_affected(ch, gsn_drained)){
    send_to_char("You feel far to weak to attempt such a feat.\n\r", ch);
    return;
  }
  if ( (psalm = get_psalm(ch, argument)) < 1){
    send_to_char("You know not the words to such psalm.\n\r", ch);
    return;
  }
  /* check for any psalms on character */
  for (paf = ch->affected; paf; paf = paf->next){
    int sn = 0;
    if (paf->type == gsn_virtues)
      continue;
    /* skip is this is a psalm we already checked */
    if (found == paf->type)
      continue;
    if ( (sn = psalm_check_lookup_sn( paf->type )) == 0)
      continue;
    /* this is a psalm! check for chant master*/
    if (fTwo && psalm_table[psalm].fTwo){
      /* we look for another prayer active */
      if (found == 0 && psalm_table[sn].fTwo){
	found = paf->type;
	continue;
      }
    }
    sendf(ch, "The psalm of %s is still fresh in your mind!\n\r",
	  psalm_table[sn].name);
    return;
  }//END psalm check

  if (is_fight_delay(ch, 90)){
    send_to_char("The recent combat ruins any chance of peaceful contemplation.\n\r", ch);
    return;
  }
  /* check for being tired */
  if (ch->mana < ch->max_mana / 2 || ch->hit < ch->hit / 2 || ch->mana < cost){
    send_to_char("Your mind and body are too weak to begin.\n\r", ch);
    return;
  }
  ch->mana -=  cost;
  WAIT_STATE2(ch, lag);
  if (!psalm_table[psalm].fGroup){
    CHAR_DATA* vch = get_group_world(ch, TRUE);
    if (vch){
      act("You begin but soon thoughs of $N's safety break your concentration.", ch, NULL, vch, TO_CHAR);
      skill = 0;
    }
  }

  if (number_percent() > skill){
    send_to_char("You failed to begin the chant.\n\r", ch);
    check_improve(ch, gsn_chant, TRUE, 1);
    return;
  }

  /* ok all prereqs done */
  /* marker for chant wait */
  af.type = gsn_chant;
  af.level = ch->level;
  af.duration = fTwo ? psalm_table[psalm].wait / 4 : psalm_table[psalm].wait;
  if ((paf = affect_find(ch->affected, gsn_virtues)) != NULL){
    af.duration /= 2;
    if (--paf->modifier < 0){
      if ( paf->type > 0 && skill_table[paf->type].msg_off )
	act_new(skill_table[paf->type].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
      affect_strip(ch, paf->type);
    }
  }
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_WIS;
  af.modifier = 1;
  affect_to_char(ch, &af);

  /* start the chant */

  af.type = gen_chant;
  af.level = ch->level;
  af.duration = psalm_table[psalm].chant;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = psalm;
  affect_to_char(ch, &af);

}

/* interrupts a chant, or modification */
void do_interrupt( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA* paf;
  bool fFound = FALSE;

/* ACRAFT */
  if ( (paf = affect_find(ch->affected, gen_acraft)) != NULL){
    fFound = TRUE;
    paf->duration = 1;
    affect_strip(ch, gen_acraft);
  }
/* WCRAFT */
  if ( (paf = affect_find(ch->affected, gen_wcraft)) != NULL){
    fFound = TRUE;
    paf->duration = 1;
    affect_strip(ch, gen_wcraft);
  }

/* CHANT */
  if ( (paf = affect_find(ch->affected, gen_chant)) != NULL){
    fFound = TRUE;
    paf->duration = 1;
    affect_strip(ch, gen_chant);
  }

  if (!fFound){
    send_to_char("You are not involved in anything that can be interrupted.\n\r", ch);
    return;
  }
  if (ch->position != POS_STANDING){
    do_stand(ch, "");
  }
}

/* ranger skill to make arrows */
void do_fletchery( CHAR_DATA *ch, char *argument ){
    OBJ_DATA *obj;
    AFFECT_DATA *paf;

    char buf[MIL];
    char arg[MIL];
    char spec_name[MIL];
    const int sn = skill_lookup("fletchery");
    const int cost = skill_table[sn].min_mana;
    const int lag = skill_table[sn].beats;

    int type = PROJECTILE_ARROW;
    int skill = get_skill(ch, sn);
    int chance = 4 * skill / 5, roll = 0;
    int spec_flag = NO_FLAG;
    int attack = 2;

    argument = one_argument( argument, arg );
    if (chance < 1){
      send_to_char("You don't know how to make arrows.\n\r",ch);
      return;
    }
    if (ch->in_room->sector_type != SECT_FOREST){
      send_to_char("You must be in forest in order to gather the materials.\n\r", ch);
      return;
    }
/* type of arrow here */
    if (IS_NULLSTR(arg)
	|| (type = flag_value( projectile_type, arg )) == NO_FLAG){
      send_to_char("Create what type of projectiles?\n\r", ch);
      send_to_char("You may create:\n\r", ch);
      show_flag_cmds(ch, projectile_type);
      return;
    }
/* default name */
    sprintf( spec_name, "ranger ");
    /* special type */
    if (skill > 80){
      int pos = 0;
      if (!IS_NULLSTR(argument)){
	for (pos = 0; fletcher_type[pos].name != NULL; pos++){
	  if (skill <= 80 + (pos * 2))
	    break;
	  if (!str_prefix(argument, fletcher_type[pos].name)){
	    spec_flag = fletcher_type[pos].bit;
	    break;
	  }
	}
      }
      if (spec_flag == NO_FLAG){
	send_to_char("You may create following special types of projectiles:\n\r", ch);
	for (pos = 0; fletcher_type[pos].name != NULL; pos++){
	  if (skill <= 80 + (pos * 2))
	    break;
	  sendf(ch, "%d. %s\n\r", pos + 1, fletcher_type[pos].name);
	}
	return;
      }
      else
	sprintf(spec_name, "%s ", fletcher_type[pos].name );
    }
    if (ch->mana < cost){
      send_to_char("You don't have enough energy to make a projectile.\n\r",ch);
      return;
    }
    else
      ch->mana -= cost;
    WAIT_STATE2(ch, lag);
    if ( (roll = number_percent()) > chance){
      send_to_char("You failed to find anything to use for materials.\n\r", ch);
      check_improve(ch, sn, FALSE, 5);
      return;
    }
    else
      check_improve(ch, sn, TRUE, 1);

    obj = create_object( get_obj_index( OBJ_VNUM_RANGER_ARROW ), 0);
    if (obj == NULL){
      bug("do_fletchery: could not create initial object>", 0);
      return;
    }

/* name */
    sprintf(buf, obj->name, spec_name, arg);
    free_string(obj->name);
    obj->name = str_dup( buf );

/* short desc */
    sprintf(buf, obj->short_descr, spec_name, arg);
    free_string(obj->short_descr);
    obj->short_descr = str_dup( buf );

/* long desc */
    sprintf(buf, obj->description, spec_name, arg);
    free_string(obj->description);
    obj->description = str_dup( buf );

    /* condition (ammo count) */
    obj->condition = UMIN(100, number_range( 10 * rounds_per_projectile( type ), 100));

    /* choose attack */
    if (IS_SET(spec_flag, PROJ_SPEC_SHOCK))
      attack = attack_lookup("shbite");
    else if (IS_SET(spec_flag, PROJ_SPEC_FLAMING))
      attack = attack_lookup("flbite");
    else if (IS_SET(spec_flag, PROJ_SPEC_FROST))
      attack = attack_lookup("frbite");
    else if (IS_SET(spec_flag, PROJ_SPEC_POISON))
      attack = attack_lookup("psbite");
    else if (IS_SET(spec_flag, PROJ_SPEC_PASSDOOR))
      attack = attack_lookup("magic");
    else
      attack = attack_lookup("pierce");

    obj->value[0] = type;
    obj->value[1] = 3;
    obj->value[2] = UMAX(1, ((skill * 60 / 100)  / rounds_per_projectile( type )) / 3);
    obj->value[3] = attack;
    obj->value[4] = spec_flag == NO_FLAG ?  0 :  spec_flag;
    obj->level = ch->level;

    paf = new_affect();
    paf->type       = sn;
    paf->level      = ch->level;
    paf->duration   = -1;
    paf->location   = APPLY_HITROLL;
    paf->modifier   = UMAX(0, skill - 50) / 10;
    paf->bitvector  = 0;
    paf->next       = obj->affected;
    obj->affected   = paf;
    obj_to_ch(obj, ch);

    act("You create some $p.", ch, obj, NULL, TO_CHAR);
    act("$n creates some $p.", ch, obj, NULL, TO_ROOM);
}


/* ranger skill to make fired weapons */
void do_bowyer( CHAR_DATA *ch, char *argument ){
  OBJ_DATA *obj;
  AFFECT_DATA *paf;

  char buf[MIL];
  char arg[MIL];
  char spec_name[MIL];
  const int sn = skill_lookup("bowyer");
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;

  int type = PROJECTILE_ARROW;
  int skill = get_skill(ch, sn);
  int chance = 4 * skill / 5;
  int spec_flag = skill > 89 ? RANGED_FAST : NO_FLAG;

  argument = one_argument( argument, arg );
  if (chance < 1){
    /* fake to a social */
    check_social(ch, "bow", "");
    return;
  }
  if (ch->in_room->sector_type != SECT_FOREST){
    send_to_char("You must be in forest in order to gather the materials.\n\r", ch);
    return;
  }
/* type of arrow here */
  if (IS_NULLSTR(arg)
      || (type = flag_value( projectile_type, arg )) == NO_FLAG){
    send_to_char("Create for what type of projectiles?\n\r", ch);
    send_to_char("You may choose:\n\r", ch);
    show_flag_cmds(ch, projectile_type);
    return;
  }
  /* default name */
  sprintf( spec_name, "shooter");

  /* chose projectile specific name */
  switch ( type ){
  case PROJECTILE_ARROW:	sprintf( spec_name, "bow");		break;
  case PROJECTILE_BOLT:		sprintf( spec_name, "crossbow");	break;
  case PROJECTILE_STONE:	sprintf( spec_name, "sling");		break;
  case PROJECTILE_QUARREL:	sprintf( spec_name, "arbalest");	break;
  case PROJECTILE_DART:		sprintf( spec_name, "blowpipe");	break;
  case PROJECTILE_SPEAR:	sprintf( spec_name, "spearthrower");	break;
  case PROJECTILE_BLADE:	sprintf( spec_name, "discgun");		break;
  case PROJECTILE_BULLET:	sprintf( spec_name, "thunderstick");	break;
  default:			sprintf( spec_name, "shooter");		break;
  }


  if (ch->mana < cost){
    send_to_char("You don't have enough energy to make a weapon.\n\r",ch);
    return;
  }
  else
    ch->mana -= cost;
  WAIT_STATE2(ch, lag);

  if (number_percent() > chance){
    send_to_char("You failed to find anything to use for materials.\n\r", ch);
    check_improve(ch, sn, FALSE, 5);
      return;
  }
  else
    check_improve(ch, sn, TRUE, 1);

  obj = create_object( get_obj_index( OBJ_VNUM_RANGER_BOW ), 0);
  if (obj == NULL){
    bug("do_bowyer: could not create initial object>", 0);
    return;
  }

/* name */
  sprintf(buf, obj->name, ch->name, spec_name);
  free_string(obj->name);
  obj->name = str_dup( buf );

/* short desc */
  sprintf(buf, obj->short_descr, ch->name, spec_name);
  free_string(obj->short_descr);
  obj->short_descr = str_dup( buf );

/* long desc */
  sprintf(buf, obj->description, ch->name, spec_name);
  free_string(obj->description);
  obj->description = str_dup( buf );

  obj->value[0] = type;
  obj->value[1] = 75 + 2 * (skill - 75);
  obj->value[2] = UMIN(10, rounds_per_projectile( type ));
  obj->value[4] = spec_flag == NO_FLAG ?  0 :  spec_flag;
  obj->level = ch->level;
  obj->timer = 48;

  paf = new_affect();
  paf->type       = sn;
  paf->level      = ch->level;
  paf->duration   = -1;
  paf->location   = APPLY_HITROLL;
  paf->modifier   = URANGE(-1, (skill - 80) / 10, 2);
  paf->bitvector  = 0;
  paf->next       = obj->affected;
  obj->affected   = paf;
  paf = new_affect();
  paf->type       = sn;
  paf->level      = ch->level;
  paf->duration   = -1;
  paf->location   = APPLY_DAMROLL;
  paf->modifier   = URANGE(-1, (skill - 80) / 10, 2);
  paf->bitvector  = 0;
  paf->next       = obj->affected;
  obj->affected   = paf;
  obj_to_ch(obj, ch);

  act("You create $p.", ch, obj, NULL, TO_CHAR);
  act("$n creates $p.", ch, obj, NULL, TO_ROOM);
}

/* thief traps skill lookup function */
int thieftrap_lookup(char* name){
  int i = 0;
  for (i = 0; thief_traps[i].name != NULL; i++){
    if (LOWER(thief_traps[i].name[0]) == LOWER(name[0])
	&& !str_cmp(thief_traps[i].name, name))
      return i;
  }
  return 0;
}

/* prefix lookup instead of exact lookup */
int thieftrap_plookup(char* name){
  int i = 0;
  for (i = 0; thief_traps[i].name != NULL; i++){
    if (LOWER(thief_traps[i].name[0]) == LOWER(name[0])
	&& !str_prefix(name, thief_traps[i].name))
      return i;
  }
  return 0;
}


/* theif's trap */
void do_traps( CHAR_DATA *ch, char *argument ){
  OBJ_DATA* pObj = NULL;
  EXIT_DATA* pExit = NULL;
  TRAP_DATA* pt;
  AFFECT_DATA af, *paf;

  char trap[MIL];
  const int sn = skill_lookup("traps");
  int trap_sn = sn;

  int lag = skill_table[sn].beats;	 //varies with traps
  int cost = skill_table[sn].min_mana;		//varies with traps

  int skill = get_skill(ch, sn);
  int trap_skill = 0;

  int dir = -1;
  int vnum = 0;
  int pos = 0;

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  argument = one_argument(argument, trap);

  if (IS_IMMORTAL(ch) && mud_data.mudport != TEST_PORT){
    send_to_char("Not on this port.\n\r", ch);
    return;
  }
/* argument */
  if (IS_NULLSTR(trap)){
    send_to_char("Set what trap and on where?\n\r", ch);
    return;
  }
/* trap check */
  if ( (pos = thieftrap_plookup( trap )) <= 0 || thief_traps[pos].vnum < 1
       || get_skill(ch, skill_lookup(thief_traps[pos].skill)) < 1){
    char out[MSL];
    int i = 0;
    int found = 0;
    sprintf(out, "You may currently create following traps:\n\r");
    for (i = 0; thief_traps[i].name != NULL; i++){
      char buf[MIL];
      if (thief_traps[i].vnum &&
	  get_skill(ch, skill_lookup(thief_traps[i].skill)) > 0){
	sprintf(buf, " %-16s [%s%s%s]",
		thief_traps[i].name,
		thief_traps[i].small ? "S" : " ",
		thief_traps[i].exit ? "E" : " ",
		thief_traps[i].obj ? "O" :" ");
	strcat(out, buf);
//	if (found != 0 && (found + 1) % 4 == 0)
	strcat(out, "\n\r");
//	else
//	  strcat(out, ", ");
	found ++;
      }
    }
    strcat(out, "\n\r");
    send_to_char(out, ch);
    return;
  }
  else{
    trap_sn = skill_lookup(thief_traps[pos].skill);
    vnum = thief_traps[pos].vnum;
    cost = skill_table[trap_sn].min_mana;
    lag = skill_table[trap_sn].beats;
    trap_skill = get_skill(ch, trap_sn);
  }
  /* wait check */
  if ( (paf = affect_find(ch->affected, sn )) != NULL){
    if (paf->modifier > 0){
      send_to_char("You're not ready to set a new trap yet.\n\r", ch);
      return;
    }
    else if (vnum == paf->level){
      sendf( ch, "You do not have enough materials to create another %s\n\r", thief_traps[pos].name );
      return;
    }
  }
  /* we got the trap, now we need a target */
  if (IS_NULLSTR(argument)){
    sendf(ch, "Prepare the %s trap where? (object or exit)\n\r",
	  thief_traps[pos].name);
    return;
  }

  /* look for door first */
  if ( (dir = dir_lookup(argument)) < 0
       && (pObj = get_obj_here(ch, NULL, argument)) == NULL){
    send_to_char("There is no such path or object in the area.\n\r", ch);
    return;
  }
  /* exits */
  if (dir >= 0 && (pExit = ch->in_room->exit[dir]) == NULL){
    send_to_char("There is no path leading in that direction.\n\r", ch);
    return;
  }
  /* trap checks */
  if ( (pObj && pObj->traps) || (pExit && pExit->traps)){
    send_to_char("Its already been armed with a trap!\n\r", ch);
    return;
  }
/* a final check for peace of mind */
  if (pObj == NULL && pExit == NULL){
    send_to_char("Install the trap on what?\n\r", ch);
    return;
  }

/* type check */
  if (pObj){
    if (!thief_traps[pos].obj){
      send_to_char("This trap cannot fit onto objects.\n\r", ch);
      return;
    }
    if (CAN_WEAR(pObj, ITEM_TAKE) && !thief_traps[pos].small){
      send_to_char("This trap is too big to fit onto that object.\n\r", ch);
      return;
    }
  }
  if (pExit && !thief_traps[pos].exit){
    send_to_char("This trap cannot fit onto doors nor passages.\n\r", ch);
    return;
  }
  /* mana and skill checks */
  if (ch->mana < cost){
    send_to_char("You can't concentrate sufficiently.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2(ch, lag);

/* general skill check */
  if (number_percent() > skill){
    send_to_char("You lose concentration and fail.\n\r", ch);
    if (number_percent() < 30)
      check_improve(ch, sn, FALSE, -99);
    else
      check_improve(ch, sn, FALSE, 5);
    return;
  }
  else{
    check_improve(ch, sn, TRUE, 1);
  }

  if (number_percent() > trap_skill){
    send_to_char("You forget a small detail of the trap and it falls apart.\n\r", ch);
    check_improve(ch, trap_sn, FALSE, -99);
    return;
  }
  else{
    check_improve(ch, trap_sn, TRUE, 1);
  }

  /* now we have a target, prepare the trap */
  if ( (pt = create_trap(get_trap_index( vnum ), ch)) == NULL){
    send_to_char("There is something amiss, contant an Immortal.\n\r", ch);
    bug("do_traps: could not create a trap vnum: %d", vnum);
    return;
  }
  /* set the trap stats if any */
  pt->level = ch->level  + (skill - 75) / 5;
/* vary the level a wee bit */
  pt->level = number_range(pt->level - 2, pt->level + 2);
  pt->duration = thief_traps[pos].duration;
  pt->level = UMIN(ch->level + 5, pt->level);

/* set the wait afect */

  if (paf != NULL){
    /* first trap vnum is in level, second in modifier */
    paf->modifier = vnum;
    if (UMIN(pt->duration / 6, 6) > paf->duration)
      paf->duration = UMIN(pt->duration / 6, 6);
  }
  else{
    af.type = sn;
    af.duration = UMIN(pt->duration / 6, 6);
    af.level = vnum;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_char(ch, &af);
  }
  /* create object already attatches a trap to ch */
  /* attatch the trap to obj/exit */
  if (pObj){
    trap_to_obj( pt, pObj);
    act("With some effort you set $t onto $P and stand back.", ch, pt->name, pObj, TO_CHAR);
    if (!IS_AFFECTED(ch, AFF_SNEAK))
      act("$n hovers around $P for a while tinkering with something.", ch, pt->name, pObj, TO_ROOM);
    if (pObj->carried_by && pObj->carried_by == ch && IS_TRAP(pt, TRAP_DELAY)){
      act("You arm $t.", ch, pt->name, NULL, TO_CHAR);
      pt->armed = TRUE;
    }
  }
  else if (pExit){
    char buf[MIL];
    trap_to_exit( pt, pExit);
    sprintf(buf, "a %s leading %s",
	    IS_SET(pExit->exit_info, EX_ISDOOR) ? "door" : "path",
	    dir_name[dir]);
    act("With some effort you set $t onto $T and stand back.", ch, pt->name,
	buf, TO_CHAR);
    if (!IS_AFFECTED(ch, AFF_SNEAK))
      act("$n hovers around $T for a while tinkering with something.", ch, pt->name, buf, TO_ROOM);
  }
}

/* disarms traps */
void do_defuse( CHAR_DATA *ch, char *argument ){
  OBJ_DATA* pObj = NULL;
  TRAP_DATA* pt, *pTrap;

  const int sn = skill_lookup("defuse");
  const int lag = skill_table[sn].beats;
  const int cost = skill_table[sn].min_mana;

  int skill = get_skill(ch, sn) / 2;
  int gain = 0;
  int chance = 0;
  int dir = 0;

/* argument */
  if (IS_NULLSTR(argument)){
    send_to_char("Defuse which exit or object?\n\r", ch);
    return;
  }

  /* look for door first */
  if ( (dir = dir_lookup(argument)) < 0
       && (pObj = get_obj_here(ch, NULL, argument)) == NULL){
    send_to_char("There is no such path or object in the area.\n\r", ch);
    return;
  }
  /* exits */
  if (dir >= 0 && (ch->in_room->exit[dir]) == NULL){
    send_to_char("There is no path leading in that direction.\n\r", ch);
    return;
  }
/* cost and lag */
  if (ch->mana < cost){
    send_to_char("You can't muster up enough concentration.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;
  WAIT_STATE2(ch, lag);

/* get our target now */
  if (pObj)
    pt = pObj->traps;
  else if (dir >= 0)
    pt = ch->in_room->exit[dir]->traps;
  else{
    send_to_char("Which door or object?\n\r", ch);
    return;
  }
  /* check if there is a trap we can see */
  if ( (pTrap = can_see_trap(ch, pt)) == NULL){
    send_to_char("You look carefuly but do not spot any traps.\n\r", ch);
    trip_traps(ch, pt);
    return;
  }

  /* add level and wisdom modifiers to skill */
  skill += 2 * ch->level / 3;
  skill += (get_curr_stat(ch, STAT_WIS) -  18) * 5;
  /* add some randomness */
  skill = number_range(skill, 3 * skill / 2);
  /* luck */
  skill += (get_curr_stat(ch,STAT_LUCK) - 12) * 4;

  /* now we have a trap that we spotted */
  if (IS_TRAP(pTrap, TRAP_EASY))
    chance = pTrap->level;
  else if (IS_TRAP(pTrap, TRAP_MEDIUM))
    chance = 3 * pTrap->level / 2;
  else if (IS_TRAP(pTrap, TRAP_HARD))
    chance = pTrap->level * 2;
  else
    chance = 4 * pTrap->level / 3;

  chance = URANGE(1, chance, 100);

  /* check for the blanket flags */
  if (IS_TRAP(pTrap, TRAP_NODISARM))
    chance = 1000;
  else if (IS_TRAP(pTrap, TRAP_ALLDISARM))
    chance = 0;

  /* first check if we failed */
  if (chance > skill){
    send_to_char("You failed.\n\r", ch);
    check_improve(ch, sn, FALSE, 1);
    /* now check if we blew it. */
    if (IS_TRAP(pTrap, TRAP_EASY))
      return;
    else if (IS_TRAP(pTrap, TRAP_MEDIUM) && number_percent() > 75)
      trip_traps(ch, pt);
    else if (IS_TRAP(pTrap, TRAP_HARD))
      trip_traps(ch, pt);
    else if (number_percent() > 90)
      trip_traps(ch, pt);
    return;
  }
  /* success */
  check_improve(ch, sn, TRUE, 1);
  act("You defuse $t.", ch, pTrap->name, NULL, TO_CHAR);
  if (!IS_AFFECTED(ch, AFF_SNEAK))
    act("$n defuses $t.", ch, pTrap->name, NULL, TO_ROOM);
  if (ch->level < 50 && !is_affected(ch, sn)){
    gain = number_range(pTrap->level, pTrap->level * 2);
    if (get_skill(ch, gsn_detect_hidden) > 1)
      gain *= 3;
    if (pTrap->owner != ch){
      AFFECT_DATA af;
      sendf(ch, "You've gained %d experience!\n\r", gain);
      gain_exp(ch, gain );

      //set wait timer
      af.type = sn;
      af.level = 60;
      af.duration = number_range(2, 4);
      af.where = TO_NONE;
      af.bitvector = 0;
      af.location = 0;
      af.modifier = 0;
      affect_to_char( ch, &af );
    }
  }
  pTrap->armed = FALSE;
  /* make sure to take care of delay traps here */
  REMOVE_BIT(pTrap->flags, TRAP_DELAY);

  /* owned traps get removed */
  if (pTrap->owner){
    if (pTrap->owner != ch && IS_TRAP(pTrap, TRAP_NOTIFY))
      sendf(pTrap->owner, "You sense %s has defused %s.\n\r", PERS2(ch), pTrap->name);
    else if (ch != pTrap->owner)
      sendf(pTrap->owner, "You sense %s has been defused.\n\r", pTrap->name);
    extract_trap( pTrap );
  }
}

/* warrior selectable skills */
/* boosts hitroll in fight.c considerably, prevents secondary attacks */
void do_feign( CHAR_DATA *ch, char *argument ){
  OBJ_DATA* sec;
  AFFECT_DATA af;
  int sn = gsn_feign;
  int skill = 4 * get_skill(ch, sn) / 5;
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (ch->fighting == NULL){
    send_to_char("But you aren't fighting anyone!\n\r",ch);
    return;
  }

  /* check for both weapons */
  if (get_eq_char(ch, WEAR_WIELD) == NULL
      || (sec = get_eq_char(ch, WEAR_SECONDARY)) == NULL){
    send_to_char("You need a pair of weapons to feign successfuly.\n\r", ch);
    return;
  }

/* weapon type check */
  if (sec->value[0] != WEAPON_DAGGER
      && sec->value[0] != WEAPON_SWORD){
    send_to_char("You may only feign with a sword or dagger.\n\r", ch);
    return;
  }

  if (ch->mana < cost){
    send_to_char("You lack the concentration required.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2(ch, lag);

  if (number_percent() > skill){
    send_to_char("You underestimate your opponent and fail.\n\r", ch);
    check_improve(ch, sn, FALSE, 1);
    return;
  }
  else{
    send_to_char("You feign with your weapon and gain an opening!\n\r", ch);
    check_improve(ch, sn, TRUE, 1);
  }

  af.type = sn;
  af.level = ch->level;
  af.duration = 0;
  af.where = TO_NONE;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);
}

/* prevents one secondary attack on user and victim, prevents dual parry/sblock
on victim
*/
void do_weapon_lock( CHAR_DATA *ch, char *argument ){
  OBJ_DATA* sec;
  CHAR_DATA* victim;
  AFFECT_DATA af;
  int sn = gsn_wlock;
  int skill = 4 * get_skill(ch, sn) / 5;
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if ( (victim = ch->fighting) == NULL){
    send_to_char("But you aren't fighting anyone!\n\r",ch);
    return;
  }

  if (is_affected(ch, sn)){
    send_to_char("But your arm is already tied up!\n\r", ch);
    return;
  }
  /* check for both weapons */
  if ( ((sec = get_eq_char(ch, WEAR_WIELD)) == NULL || sec->value[0] != WEAPON_WHIP)
       && ((sec = get_eq_char(ch, WEAR_SECONDARY)) == NULL || sec->value[0] != WEAPON_WHIP)
       ){
    send_to_char("You need a whip to weapon lock with.\n\r", ch);
    return;
  }

  if (ch->mana < cost){
    send_to_char("You lack the concentration required.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  skill += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX));
  skill += (ch->level - victim->level);
  skill += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
  skill += affect_by_size(ch,victim);

  if (!can_see(ch,victim))
    skill -= 5;

  WAIT_STATE2(ch, lag);

  if (number_percent() > skill){
    act("You try to disable $N's arm, but fail.", ch, NULL, victim, TO_CHAR);
    act("$n tries to disable your arm, but fails.", ch, NULL, victim, TO_VICT);
    act("$n tries to disable $N's arm, but fails.",ch, NULL, victim, TO_NOTVICT);
    check_improve(ch, sn, FALSE, 1);
    return;
  }
  else{
    act("You diable $N's weaker arm!.", ch, NULL, victim, TO_CHAR);
    act("$n disables your weaker arm!.", ch, NULL, victim, TO_VICT);
    act("$n disables $N's weaker arm!.",ch, NULL, victim, TO_NOTVICT);
    check_improve(ch, sn, TRUE, 1);
  }

  af.type = sn;
  af.level = ch->level;
  af.duration = 0;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);
  af.location = APPLY_NONE;
  af.modifier = 2;
  affect_to_char(victim, &af);
}

/* flail smash, may cause stunning (short paralysis) */
void do_mantis_maul( CHAR_DATA *ch, char *argument ){
  OBJ_DATA* obj;
  CHAR_DATA* victim;
  char buf[MIL];

  const int sn = skill_lookup("mantis maul");
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;
  int dam = number_range(15, 25) + UMAX(-30, (get_curr_stat(ch,STAT_STR) - 20) * 10);
  int skill = 4 * get_skill(ch, sn) / 5;
  int dam_type = DAM_PIERCE;
  int imm = IS_NORMAL;


  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_NULLSTR(argument)){
    victim = ch->fighting;
    if (victim == NULL){
      send_to_char("But you aren't in combat!\n\r",ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, NULL, argument)) == NULL){
    send_to_char("They aren't here.\n\r",ch);
    return;
  }

  /* check for weapons */
  if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL
      || obj->value[0] != WEAPON_FLAIL){
    send_to_char("You require a flail in your primary hand.\n\r", ch);
    return;
  }
  else
    dam_type = attack_table[obj->value[3]].damage;

  if (ch->mana < cost){
    send_to_char("You lack the concentration required.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  if (is_safe(ch,victim))
    return;

  WAIT_STATE2(ch, lag);

  skill += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX));
  skill += (ch->level - victim->level);
  skill += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
  skill += affect_by_size(ch,victim);
  skill -= get_skill(victim,gsn_dodge)/20;

  if (has_twohanded(ch) != NULL){
    send_to_char("Using both your arms you put a hefty swing onto the flail.\n\r", ch);
    dam += number_range(10, 20);
    skill += 15;
  }

/* check for immunities */
  imm = check_immune(victim, dam_type, TRUE);
  if ( imm < IS_NORMAL){
    skill /= 2;
  }
  else if (imm > IS_NORMAL){
    skill = 3 * skill / 2;
  }
  /* yell out */
  a_yell(ch,victim);

  //predict interdict
  if (predictCheck(ch, victim, "maul", skill_table[sn].name))
    return;

  /* check for miss */
  if (number_percent() > skill){
    dam = 0;
    if (ch->fighting != victim && victim->fighting != ch){
      sprintf(buf, "Die %s you grain thrashing fool!",PERS(ch,victim));
      j_yell(victim,buf);
    }
    check_improve(ch, sn, FALSE, 2);
    damage(ch, victim, dam, sn, dam_type, TRUE);
    return;
  }
  else{
    if (ch->fighting != victim && victim->fighting != ch){
      sprintf(buf, "Help! %s just mauled my pretty face!",PERS(ch,victim));
      j_yell(victim,buf);
    }
    check_improve(ch, sn, TRUE, 1);
  }

/* check for paralyze */
  if (imm == IS_IMMUNE){
    act("$N seems to shake off the blow!", ch, NULL, victim, TO_NOTVICT);
    act("You easly shake off the blow.", ch, NULL, victim, TO_VICT);
    act("$N seems to not even notice the blow.", ch, NULL, victim, TO_ROOM);
  }
  else if (!is_affected(victim, gsn_paralyze)
	   && !is_affected(victim, gsn_ghoul_touch)){
    AFFECT_DATA af;
    act("$n has been paralyzed!", victim, NULL, NULL, TO_ROOM);
    act("You have been paralyzed!", victim, NULL, NULL, TO_CHAR);

    af.type      = gsn_paralyze;
    af.level     = ch->level;
    af.duration  = 0;
    af.where     = TO_AFFECTS;
    af.bitvector = 0;
    af.location  = APPLY_NONE;
    af.modifier  = number_range(3, 5);
    affect_to_char( victim, &af );
  }
  damage(ch, victim, dam, sn, dam_type, TRUE);
}


/* focused bash, may cause disorientation (short misdirection) */
void do_focused_bash( CHAR_DATA *ch, char *argument ){
  OBJ_DATA* obj;
  CHAR_DATA* victim;
  char buf[MIL];

  const int sn = skill_lookup("focused bash");
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;
  int dam = number_range(35, 55) + UMAX(-30, (get_curr_stat(ch,STAT_STR) - 20) * 10);
  int skill = 4 * get_skill(ch, sn) / 5;
  int dam_type = DAM_PIERCE;
  int imm = IS_NORMAL;


  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_NULLSTR(argument)){
    victim = ch->fighting;
    if (victim == NULL){
      send_to_char("But you aren't in combat!\n\r",ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, NULL, argument)) == NULL){
    send_to_char("They aren't here.\n\r",ch);
    return;
  }

  /* check for weapons */
  if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL
      || obj->value[0] != WEAPON_MACE){
    send_to_char("You require a mace in your primary hand.\n\r", ch);
    return;
  }
  else
    dam_type = attack_table[obj->value[3]].damage;

  if (ch->mana < cost){
    send_to_char("You lack the concentration required.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  if (is_safe(ch,victim))
    return;

  WAIT_STATE2(ch, lag);

  skill += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX));
  skill += (ch->level - victim->level);
  skill += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
  skill += affect_by_size(ch,victim);
  skill -= get_skill(victim,gsn_dodge)/20;

  if (has_twohanded(ch) != NULL){
    send_to_char("Using both your arms you put a hefty swing onto the mace.\n\r", ch);
    dam  += number_range(10, 20);
    skill += 10;
  }

/* check for immunities */
  imm = check_immune(victim, dam_type, TRUE);
  if ( imm < IS_NORMAL){
    skill /= 2;
  }
  else if (imm > IS_NORMAL){
    skill = 3 * skill / 2;
  }

  /* yell out */
  a_yell(ch,victim);
  //predict interdict
  if (predictCheck(ch, victim, "focusedbash", skill_table[sn].name))
    return;

  /* check for miss */
  if (number_percent() > skill){
    dam = 0;
    if (ch->fighting != victim && victim->fighting != ch){
      sprintf(buf, "Die %s you head bashing fool!",PERS(ch,victim));
      j_yell(victim,buf);
    }
    check_improve(ch, sn, FALSE, 2);
    damage(ch, victim, dam, sn, dam_type, TRUE);
    return;
  }
  else{
    if (ch->fighting != victim && victim->fighting != ch){
      sprintf(buf, "Help! %s just smashed me across my head!",PERS(ch,victim));
      j_yell(victim,buf);
    }
    check_improve(ch, sn, TRUE, 1);
  }

/* check for stun */
  if (imm == IS_IMMUNE){
    act("$N seems to shake off the blow!", ch, NULL, victim, TO_NOTVICT);
    act("You easly shake off the blow.", ch, NULL, victim, TO_VICT);
    act("$N seems to not even notice the blow.", ch, NULL, victim, TO_ROOM);
  }
  else if (!is_affected(victim, sn)){
    AFFECT_DATA af;
    act("$n smashes $p across your head dazing and confusing you.",
	ch, obj, victim, TO_VICT);
    act("$n eyes take on a dazed and confuzed look.",
	victim, NULL, NULL, TO_ROOM);
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = number_range(0,5);
    af.duration  = number_range(0, 1);
    af.location  = APPLY_DEX;
    af.modifier  = -1;
    af.bitvector = AFF_MISDIRECTION;
    affect_to_char( victim, &af );
    af.level     = number_range(0,5);
    af.location  = APPLY_INT;
    affect_to_char( victim, &af );
  }
  damage(ch, victim, dam, sn, dam_type, TRUE);
}


/* Shield bash */
/* Advanced Kick replacement with extra effects */
void do_shield_bash( CHAR_DATA *ch, char *argument ){
    CHAR_DATA *victim;
    OBJ_DATA* obj = get_eq_char(ch, WEAR_SHIELD);
    const int sn = skill_lookup("shield bash");
    int chance,dam;
    const int cost = skill_table[sn].min_mana;


    if ( (chance = get_skill(ch, sn)) < 1){
      send_to_char("Huh?\n\r", ch);
      return;
    }

    if ( (victim = ch->fighting) == NULL){
      send_to_char("But you aren't in combat!\n\r",ch);
      return;
    }

    if (obj == NULL){
      send_to_char("Don't you think you need a shield to shield bash?\n\r", ch);
      return;
    }
    if (!IS_NPC(ch) && ch->mana < cost){
      send_to_char("You lack the strength.\n\r", ch);
      return;
    }
    else ch->mana -= cost;

    /* chance */
    chance += 2 * (get_curr_stat(ch,STAT_DEX)- get_curr_stat(victim,STAT_DEX));
    chance += GET_AC2(victim,AC_BASH) /25;
    chance += (ch->level - victim->level);
    chance += affect_by_size(ch,victim);
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));

/* bonus damage for spiked shield */
    dam = number_range( ch->level / 2, 3 * ch->level / 2);
    if (is_name("spiked", obj->name) || is_name("barbed", obj->name)){
      sendf(ch, "You make excellent use of %s and draw blood.\n\r",
	    obj->short_descr);
      dam = URANGE(50, 3 * dam / 2, 120);
    }

    WAIT_STATE2( ch, skill_table[sn].beats );
    if ( number_percent( ) < chance ){
      AFFECT_DATA af;
      OBJ_DATA* obj;

      af.type = sn;
      af.level = ch->level;
      af.duration = 0;
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;

      act("$n smashes into you with $s shield!",
	  ch, NULL, victim, TO_VICT);
      act("$n smashes into $N with $s shield!",
	  ch, NULL, victim, TO_NOTVICT);
      act("You smash into $N with your shield!",
	  ch, NULL, victim, TO_CHAR);

      switch (number_range(0, 10)){
      case 0:
	obj = get_eq_char( victim, WEAR_WIELD );
	act("Your sword arm is crushed!",
	    ch, NULL, victim, TO_VICT);
	act("You crush $N's sword arm!",
	    ch, NULL, victim, TO_CHAR);
	af.where = TO_SKILL;
	af.location = (obj == NULL || obj->value[0] == WEAPON_EXOTIC ?
		       gsn_hand_to_hand :
		       *weapon_table[weapon_lookup(weapon_name(obj->value[0]))].gsn);
	af.modifier = -5;
	break;

      case 1:
	act("Your wrist is crushed!",
	    ch, NULL, victim, TO_VICT);
	act("You crush $N's wrist!",
	    ch, NULL, victim, TO_CHAR);
	af.where = TO_SKILL;
	af.location = gsn_parry;
	af.modifier = -5;
	break;
      case 2:
	act("Your chest almost caves in!",
	    ch, NULL, victim, TO_VICT);
	act("You knock the wind out of $M!",
	    ch, NULL, victim, TO_CHAR);
	af.where = TO_SKILL;
	af.location = gsn_second_attack;
	af.modifier = -15;
	break;
      case 3:
	if ( ( obj = get_eq_char( victim, WEAR_SHIELD ) ) != NULL ){
	  act("$n slams against your shield",
	      ch, NULL, victim, TO_VICT);
	  act("You slam against $N's shield",
	    ch, NULL, victim, TO_CHAR);
	  shield_disarm(ch, victim);
	  break;
	}
      case 4:
	if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) != NULL){
	  act("$n slams against your weapon",
	      ch, NULL, victim, TO_VICT);
	  act("You slam against $N's weapon.",
	    ch, NULL, victim, TO_CHAR);
	  disarm(ch, victim);
	  break;
	}
      default:
	break;
      }
      /* apply if somethign chagned */
      if (af.location != APPLY_NONE)
	affect_join(victim, &af);
      check_improve(ch, sn, TRUE, 0);
    }
/* miss */
    else{
      dam = 0;
      check_improve(ch, sn, TRUE, 0);
    }
    damage(ch, victim, dam, sn, DAM_BASH, TRUE);
}

/* skill wrapper for echo */
void do_act( CHAR_DATA *ch, char *argument ){
  int skill = get_skill(ch, skill_lookup("act"));

  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) ){
    send_to_char( "You are anti-social!\n\r", ch );
    return;
  }
  if ( IS_NULLSTR(argument)){
    send_to_char( "Local echo what?\n\r", ch );
    return;
  }
  act("$t", ch, argument, ch, TO_ALL);
}


/* feral claw attack, causes cumulative spell failure */
void do_fury( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af, *paf;
  CHAR_DATA* victim;
  char buf[MIL];

  const int sn = gsn_fury;
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;
  int dam = number_range(15, 25) + UMAX(-10, (get_curr_stat(ch,STAT_STR) - 20) * 10);
  int skill = 4 * get_skill(ch, sn) / 5;
  int dam_type = DAM_SLASH;
  int imm = IS_NORMAL;


  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_NULLSTR(argument)){
    victim = ch->fighting;
    if (victim == NULL){
      send_to_char("But you aren't in combat!\n\r",ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, NULL, argument)) == NULL){
    send_to_char("They aren't here.\n\r",ch);
    return;
  }

  if (is_safe(ch,victim))
    return;

  if (ch->mana < cost){
    send_to_char("You lack the concentration required.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2(ch, lag);

  skill += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX));
  skill += (ch->level - victim->level);
  skill += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
  skill += affect_by_size(ch,victim);
  skill -= get_skill(victim,gsn_dodge)/20;

/* check for immunities */
  imm = check_immune(victim, dam_type, TRUE);
  if ( imm < IS_NORMAL){
    skill /= 2;
  }
  else if (imm > IS_NORMAL){
    skill = 3 * skill / 2;
  }
  /* yell out */
  a_yell(ch,victim);

  //predict interdict
  if (predictCheck(ch, victim, "fury", skill_table[gsn_fury].name))
    return;

  /* check for miss */
  if (number_percent() > skill){
    dam = 0;
    if (ch->fighting != victim && victim->fighting != ch){
      sprintf(buf, "Die %s you beastly fool!",PERS(ch,victim));
      j_yell(victim,buf);
    }
    check_improve(ch, sn, FALSE, 2);
    damage(ch, victim, dam, sn, dam_type, TRUE);
    return;
  }
  else{
    if (ch->fighting != victim && victim->fighting != ch){
      sprintf(buf, "Help! %s just tore apart my face!",PERS(ch,victim));
      j_yell(victim,buf);
    }
    check_improve(ch, sn, TRUE, 1);
  }

/* check for paralyze */
  if (imm == IS_IMMUNE){
    act("$N seems to shake off the blows!", ch, NULL, victim, TO_NOTVICT);
    act("You easly shake off the blows.", ch, NULL, victim, TO_VICT);
    act("$N seems to not even notice the blows.", ch, NULL, victim, TO_ROOM);
  }
  //disrupt spell casting
  else{
    if ( (paf = affect_find(victim->affected, gsn_mind_disrupt)) == NULL){
      af.type		= gsn_mind_disrupt;
      af.level		= ch->level;
      af.duration	= 1;
      af.where		= TO_AFFECTS;
      af.bitvector	= 0;
      af.location	= APPLY_NONE;
      af.modifier	= -50;
      affect_join( victim, &af );
      act("You disrupt $N's ability to concentrate.", ch, NULL, victim, TO_CHAR);
      act("$n disrupts your ability to concentrate.", ch, NULL, victim, TO_VICT);
    }
    else{
      af.type		= gsn_mind_disrupt;
      af.level		= ch->level;
      af.duration	= paf->duration > 4 ? 0 : 1;
      af.where		= TO_AFFECTS;
      af.bitvector	= 0;
      af.location	= APPLY_NONE;
      af.modifier	= paf->modifier > -60 ? -10 : 0;
      if (af.duration > 0 || af.modifier < 0){
	affect_join( victim, &af);
	if (af.modifier < 0){
	  act("You disrupt $N's ability to concentrate.", ch, NULL, victim, TO_CHAR);
	  act("$n disrupts your ability to concentrate.", ch, NULL, victim, TO_VICT);
	}
      }
      else
	send_to_char("Your fury has no further effect.\n\r", ch);
    }
  }
  damage(ch, victim, dam, sn, dam_type, TRUE);
}

/* boulder throw, might cause misdirection */
void do_boulder_throw( CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim;
  char buf[MIL];
  char* string;

  const int sn = skill_lookup("boulder throw");
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;
  int dam = number_range(35, 55) + UMAX(-30, (get_curr_stat(ch,STAT_STR) - 20) * 10);
  int skill = get_skill(ch, sn);
  int dam_type = DAM_BASH;
  int imm = IS_NORMAL;


  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (IS_NULLSTR(argument)){
    victim = ch->fighting;
    if (victim == NULL){
      send_to_char("But you aren't in combat!\n\r",ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, NULL, argument)) == NULL){
    send_to_char("They aren't here.\n\r",ch);
    return;
  }

  /* get the string for sector */
  switch( ch->in_room->sector_type ){
  default:		string = NULL;				break;
  case SECT_FIELD:	string = "a large stone";		break;
  case SECT_FOREST:	string = "a fallen trunk";		break;
  case SECT_HILLS:	string = "a boulder";			break;
  case SECT_MOUNTAIN:	string = "a large boulder";		break;
  case SECT_LAVA:	string = "a chunk of ice";		break;
  case SECT_SNOW:	string = "a chunk of basalt";		break;
  }

  if (is_safe(ch,victim))
    return;

  if (IS_NULLSTR( string )){
    send_to_char("There isn't anything you can throw here.\n\r", ch);
    return;
  }

  if (ch->mana < cost){
    send_to_char("You lack the concentration required.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  act("You rip $t from the ground and throw it at $N.", ch, string, victim, TO_CHAR);
  act("$n rips $t from the ground and throws it at $N.", ch, string, victim, TO_NOTVICT);
  act("$n rips $t from the ground and throws it at you.", ch, string, victim, TO_VICT);
  make_item_char(ch, OBJ_VNUM_BOULDER_HOLE, 12);
  WAIT_STATE2(ch, lag);

  skill += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX));
  skill += (ch->level - victim->level);
  skill += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
  skill += affect_by_size(ch,victim);
  skill += GET_AC2(victim,AC_BASH) /25;
  skill -= get_skill(victim,gsn_dodge)/20;
  skill -= get_skill(victim,gsn_shield_block)/20;

/* check for immunities */
  imm = check_immune(victim, dam_type, TRUE);
  if ( imm < IS_NORMAL){
    skill /= 2;
  }
  else if (imm > IS_NORMAL){
    skill = 3 * skill / 2;
  }

  /* yell out */
  a_yell(ch,victim);

  //predict interdict
  if (predictCheck(ch, victim, "boulder", skill_table[sn].name))
    return;

  /* check for miss */
  if (number_percent() > skill){
    dam = 0;
    if (ch->fighting != victim && victim->fighting != ch){
      sprintf(buf, "Die %s you boulder throwing fool!",PERS(ch,victim));
      j_yell(victim,buf);
    }
    check_improve(ch, sn, FALSE, 2);
    damage(ch, victim, dam, sn, dam_type, TRUE);
    return;
  }
  else{
    if (ch->fighting != victim && victim->fighting != ch){
      sprintf(buf, "Help! %s just threw %s at my head!",PERS(ch,victim), string);
      j_yell(victim,buf);
    }
    check_improve(ch, sn, TRUE, 1);
  }

/* check for stun */
  if (imm == IS_IMMUNE){
    act("$N seems to shake off the impact!", ch, NULL, victim, TO_NOTVICT);
    act("You easly shake off the impact.", ch, NULL, victim, TO_VICT);
    act("$N seems to not even notice the impact.", ch, NULL, victim, TO_ROOM);
  }
  else if (!is_affected(victim, sn)){
    AFFECT_DATA af;
    act("The boulder's impact dazes and confuses you.",	ch, NULL, victim, TO_VICT);
    act("$n eyes take on a dazed and confuzed look.",	NULL, NULL, NULL, TO_ROOM);
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = number_range(0,5);
    af.duration  = number_range(0, 1);
    af.location  = APPLY_DEX;
    af.modifier  = -1;
    af.bitvector = AFF_MISDIRECTION;
    affect_to_char( victim, &af );
    af.level     = number_range(0,5);
    af.location  = APPLY_INT;
    affect_to_char( victim, &af );
  }
  damage(ch, victim, dam, sn, dam_type, TRUE);
}


/* blademaster skills */
void do_onslaught( CHAR_DATA* ch, char* argument ){
  CHAR_DATA* victim;
  AFFECT_DATA* paf, af;
  OBJ_DATA* wield;

  int sn = gsn_onslaught;
  int cost = skill_table[sn].min_mana;
  int lag = skill_table[sn].beats;

  int skill = get_skill(ch, sn);
  int dam = 0;
  int level = 0;
  int dam_type = DAM_PIERCE;

  bool fHaste = (IS_AFFECTED(ch, AFF_HASTE) && !IS_AFFECTED(ch, AFF_SLOW));
  bool fSlow = (!IS_AFFECTED(ch, AFF_HASTE) && IS_AFFECTED(ch, AFF_SLOW));

  //check for blademaster bladestorm haste
  if (fHaste && ch->class == gcn_blademaster && is_affected(ch, gsn_bladestorm) && get_eq_char( ch, WEAR_SECONDARY) == NULL)
    fHaste = FALSE;

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if ( (victim = ch->fighting) == NULL){
    send_to_char("But you aren't in combat!\n\r",ch);
    return;
  }
  else if ( (wield = getBmWep( ch, TRUE)) == NULL){
    send_to_char("You need a weapon first.\n\r", ch);
    return;
  }
  else if (!monk_good(ch, WEAR_BODY)){
    send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
    return;
  }
  else if (wield->value[0] != WEAPON_AXE){
    if (ch->mana < cost){
      send_to_char("You lack the energy.\n\r", ch);
      return;
    }
    else
      ch->mana -=  cost;
  }

  //adjust lag
  if (fHaste)
    lag = 2 * lag / 3;
  else if (fSlow)
    lag = 3 * lag / 2;

  //get damtype
  dam_type = attack_table[wield->value[3]].damage;

  //check what onslaught level we are on
  if ( (paf = affect_find(ch->affected, sn)) != NULL)
    level = paf->level;

  //strip onslaught from the character
  affect_strip(ch, sn );

  //setup the new af to be placed when a skill hits
  af.type	= sn;
  af.level	= level + 1;
  af.duration	= 2; 	//combat rounds before expiration
  af.where	= TO_NONE;
  af.bitvector  = 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;

  switch( level ){
    //TRHUST
  default:
  case 0:
    if (number_percent() < skill){
      dam = ch->level;
      check_improve( ch, sn, TRUE, 4);
      WAIT_STATE2( ch, lag );
      if (get_skill(ch, skill_lookup("chop")) > 1)
	affect_to_char(ch, &af );
    }
    else{
      WAIT_STATE2( ch, 2 * lag );
      check_improve( ch, sn, FALSE, 8);
    }
    damage( ch, victim, dam, sn, dam_type, TRUE);
    break;
    //CHOP
  case 1:
    sn = skill_lookup("chop");
    skill = get_skill(ch, sn);
    if (number_percent() < skill){
      dam = number_range(ch->level, 3 * ch->level / 2);
      check_improve( ch, sn, TRUE, 4);
      WAIT_STATE2( ch, lag );
      if (get_skill(ch, skill_lookup("bladerush")) > 1)
	affect_to_char(ch, &af );
    }
    else{
      WAIT_STATE2( ch, 2 * lag );
      check_improve( ch, sn, FALSE, 8);
    }
    damage( ch, victim, dam, sn, dam_type, TRUE);
    break;
    //BLADERUSH
  case 2:
    sn = skill_lookup("bladerush");
    skill = get_skill(ch, sn);
   if (number_percent() < skill){
     dam = number_range(ch->level, 2 * ch->level);
     check_improve( ch, sn, TRUE, 4);
     WAIT_STATE2( ch, lag );

     //try for disarm
     if (number_percent() < skill && get_eq_char(victim, WEAR_WIELD) != NULL){
       if (is_affected(ch, gsn_ironarm)){
	 WAIT_STATE2(victim, PULSE_VIOLENCE + 1);
	 damage(ch, victim, UMIN(victim->hit, number_range(3, 10)), gsn_ironarm, DAM_INTERNAL, TRUE);
       }
       disarm(ch, victim );
     }
     if (get_skill(ch, skill_lookup("backpin")) > 1)
       affect_to_char(ch, &af );
   }
   else{
     WAIT_STATE2( ch, 2 * lag );
     check_improve( ch, sn, FALSE, 8);
   }
   damage( ch, victim, dam, sn, dam_type, TRUE);
   break;
   //BACKPIN
  case 3:
    sn = skill_lookup("backpin");
    skill = get_skill(ch, sn);

    //backpin miss
   if (number_percent() > skill){
     WAIT_STATE2( ch, 2 * lag );
     check_improve( ch, sn, FALSE, 8);
     damage( ch, victim, dam, sn, DAM_PIERCE, TRUE);
     break;
   }
   else{
     int snPin = skill_lookup("pinwheel");
     bool fPinHit = number_percent() < (2 * (get_skill(ch, snPin) - 50));

     //backpin hit
     dam = number_range(3 * ch->level / 2, 2 * ch->level);
     check_improve( ch, sn, TRUE, 4);
     WAIT_STATE2( ch, 2 * lag );

     if (!fPinHit){
       act("$n drives $s weapon into your back!", ch, NULL, victim, TO_VICT);
       act("$n drives $s weapon into $N's back!", ch, NULL, victim, TO_NOTVICT);
       act("You drive your weapon into $N's back!", ch, NULL, victim, TO_CHAR);
     }
     else{
       act("$n drives $s weapon into your back and twists it viciously!", ch, NULL, victim, TO_VICT);
       act("$n drives $s weapon into $N's back and twists it viciously!", ch, NULL, victim, TO_NOTVICT);
       act("You drive your weapon into $N's back and twist it viciously!", ch, NULL, victim, TO_CHAR);
     }

     //backpin hit
     damage( ch, victim, dam, sn, dam_type, TRUE);
     if (ch->fighting == NULL)
       break;
     //PINWHEEL
     if (fPinHit){
       dam = number_range(3 * ch->level / 2, 2 * ch->level);
       check_improve( ch, snPin, TRUE, 4);

       if (!IS_UNDEAD(victim) && !is_affected(victim, gen_bleed)){
	 /* we hit their throat, attach the gen and do the damage */
	 af.type = gen_bleed;
	 af.level = ch->level;
	 af.duration = number_range(1, 2);
	 af.where = TO_NONE;
	 af.bitvector = 0;
	 af.location = APPLY_NONE;
	 /* modifier controls how many individuals bleeds happen (8 /tick) */
	 af.modifier = af.duration * 8;
	 paf = affect_to_char(victim, &af);
	 string_to_affect(paf, ch->name);
       }
       damage( ch, victim, dam, snPin, DAM_INTERNAL, TRUE);
     }
     else{
       check_improve( ch, snPin, FALSE, 8);
     }
   }
   break;
  }
}


//syntax: critical <area> [victim]
void do_critical( CHAR_DATA* ch, char* argument ){
  CHAR_DATA* victim;
  OBJ_DATA* wield;
  AFFECT_DATA af;

  char arg1[MIL], buf[MIL];

  const int sn = skill_lookup("critical strike");
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;

  int anat = 0;
  int skill = get_skill(ch, sn);
  int type = 0;
  int dam = number_range(ch->level / 2, 2 * ch->level);
  int dam_type = DAM_PIERCE;

  bool fYell = FALSE;
  bool fSuccess = FALSE;

  if (IS_NPC(ch) || skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument)){
    send_to_char("Area		  Anatomy\n\r"\
		 "------------------------\n\r", ch);
    for (type = 0; critical_table[type].name; type++){
      sendf(ch, "%-15.15s   %d\n\r", critical_table[type].name, critical_table[type].min_anat);
    }
    send_to_char("Strike which particular area?\n\r",ch);
    return;
  }
  else{
    bool fFound = TRUE;
    //get the area of body
    argument = one_argument( argument, arg1 );

    for (type = 0; critical_table[type].name; type++){
      if (!str_prefix(arg1, critical_table[type].name)){
	fFound = TRUE;
	break;
      }
    }
    if (!fFound){
      send_to_char("No such anatomical area.\n\r", ch);
      return;
    }
  }

  if ( (wield = getBmWep( ch, TRUE)) == NULL){
    send_to_char("You need a weapon first.\n\r", ch);
    return;
  }
  else if (wield->value[0] == WEAPON_AXE){
    send_to_char("This weapon is too clumsy to perform a critical strike.\n\r",ch);
    return;
  }
  else if (!monk_good(ch, WEAR_ARMS)){
    send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
    return;
  }
  //get victim
  if (IS_NULLSTR(argument)){
    if ( (victim = ch->fighting) == NULL){
      send_to_char("But you aren't in combat!\n\r",ch);
      return;
    }
  }
  else{
    if ((victim = get_char_room(ch, NULL, argument)) == NULL){
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
  }

  //get anatomy of victim
  if (race_table[victim->race].pc_race)
    anat = anatomy_lookup(pc_race_table[victim->race].anatomy);
  else
    anat = 0;
  anat = ch->pcdata->anatomy[anat];

  //check minimum skill
  if (anat < critical_table[type].min_anat){
    send_to_char("You don't know enough about this race to aim that spot.\n\r", ch);
    type = 0;
  }

  //we have victim, and type of hit.  Trye to execute the critical strike.

  if (is_safe(ch, victim))
    return;
  else if (ch->mana < cost){
    send_to_char("You lack the energy to execute the strike.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  //get damtype
  dam_type = attack_table[wield->value[3]].damage;
  WAIT_STATE2(ch, lag );

  skill = 4 * skill / 5;
  skill += get_curr_stat(ch, STAT_DEX) - get_curr_stat(victim,STAT_DEX);
  skill += GET_AC2(victim,AC_BASH) /25;
  skill -= get_skill(victim,gsn_dodge)/20;
  skill += (ch->level - victim->level);
  skill += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
  if (is_affected(victim,gsn_horse))
    skill -= 5;
  if (!IS_NPC(victim) && victim->pcdata->pStallion && number_percent() < get_skill(victim, gsn_mounted)){
    skill -= 12;
    check_improve(victim, gsn_mounted, TRUE, 0);
  }

  //check if target should yell.
  fYell = (ch->fighting != victim && victim->fighting != ch);

  //sound attack to justice /imm
  a_yell(ch,victim);

  //predict interdict
  if (predictCheck(ch, victim, "critical", skill_table[sn].name))
    return;

  /* DEBUG
  sendf(ch, "CHANCE: %d\n\r", skill);
  */
  //miss
  if (number_percent() > skill){
    check_improve(ch, sn, FALSE, 2);

    /* increase waitstate if this was a lagging strike */
    if (!str_cmp(critical_table[type].name, "diaphragm") || !str_cmp(critical_table[type].name, "solar plexus"))
      WAIT_STATE2(ch, 3 * lag / 2);

    fSuccess = FALSE;
    dam = 0;
  }
  else{
    check_improve(ch, sn, TRUE, 1);
    fSuccess = TRUE;
  }

  if (fYell){
    sprintf(buf, "Help! %s is trying to cut me apart!", PERS(ch, victim));
    j_yell(victim,buf);
  }

//MANTIS CHECK
  if (is_affected(victim,gsn_mantis) && fSuccess){
    act("You use $n's momentum against him, and throw $m to the ground!",ch,NULL,victim,TO_VICT);
    act("$N reverses your attack, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
    act("$N reversed $n's attack, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE2(ch, URANGE(1, number_range(1, victim->size), 3) * PULSE_VIOLENCE);
    damage(victim,ch,dam,gsn_mantis,DAM_BASH,TRUE);
    affect_strip(victim,gsn_mantis);
    return;
  }

  if (fSuccess){
    af.type	= sn;
    af.duration	= number_fuzzy(1);
    af.level	= ch->level;
    af.where	= TO_AFFECTS;
    af.bitvector  = 0;
    af.location	= APPLY_NONE;
    af.modifier	= 0;

    act("You wound $N's $t.", ch, critical_table[type].name, victim, TO_CHAR);
    act("$n wounds your $t.", ch, critical_table[type].name, victim, TO_VICT);
    act("$n wounds $N's $t.", ch, critical_table[type].name, victim, TO_NOTVICT);

    //hit
    switch( type ){
      //CHEST
    default:
    case 0:
    dam = 3 * dam / 2;
    break;

    //STOMACH
    case 1:
      victim->move = UMAX(0, ch->move - 2 * dam / 3);
      break;

      //LEGS
    case 2:
      if (!is_affected(victim, sn)){
	af.location = APPLY_DEX;
	af.modifier = -ch->level / 10;
	affect_to_char(victim, &af);
      }
      break;

      //HEAD
    case 3:
      victim->mana = UMAX(0, ch->mana - 2 * dam / 3);
      break;

      //WRISTS
    case 4:
      if (!is_affected(victim, gsn_weaken)){
	af.type		= gsn_weaken;
	af.location	= APPLY_STR;
	af.modifier	= -ch->level / 10;
	affect_to_char(victim, &af);
	act_new( "You feel your strength slip away.", victim,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n looks tired and weak.",victim,NULL,NULL,TO_ROOM);
      }
      break;

      //FEET
    case 5:
      if (!is_affected(victim, gsn_caltraps)){
	af.type		= gsn_caltraps;
	af.duration	= number_fuzzy(ch->level / 9);
	af.location	= APPLY_HITROLL;
	af.modifier	= -ch->level / 10;
	affect_to_char(victim, &af);
	act("$N starts limping.",ch,NULL,victim,TO_CHAR);
	act("You start to limp.",ch,NULL,victim,TO_VICT);
      }
      break;

      //SPLEEN
    case 6:
      if (!is_affected(victim, gsn_drained)){
	af.type		= gsn_drained;
	af.duration	= number_range(4, 6);
	affect_to_char(victim, &af);
      }
      break;

      //TEMPLE
    case 7:
      if (!is_affected(victim, gsn_tele_lock)){
	af.type		= gsn_tele_lock;
	af.location	= APPLY_NONE;
	af.modifier	= 0;
	affect_to_char(victim, &af);
	act_new( "You are now locked down",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
	act("$N is now locked down.",ch,NULL,victim,TO_CHAR);
      }
      break;

      //DIAPHRAGM (medium lag)
    case 8:
      if (get_lagprot(victim) != LAG_SHIELD_NONE){
	dam = 0;
      }
      else{
	if (!is_affected(ch, gsn_deathweaver)){
	  act("You stun $N with the blow.", ch, NULL, victim, TO_CHAR);
	  act("$n stuns you with the blow.", ch, NULL, victim, TO_VICT);
	  act("$n stuns $N with the blow.", ch, NULL, victim, TO_NOTVICT);
	  WAIT_STATE2(victim, number_range(1, 3) * PULSE_VIOLENCE);
	}
      }
      break;

      //EYES
    case 9:
      if (!IS_AFFECTED(victim, AFF_BLIND)){
	/* blind target */
	af.type		= gsn_blindness;
	af.duration	= number_range(0, 1);
	af.bitvector	= AFF_BLIND;
	af.location	= APPLY_HITROLL;
	if (!IS_NPC(victim)
	    && number_percent() < get_skill(victim,gsn_blind_fighting)){
	  if (is_affected(victim, gsn_battletrance))
	    af.modifier      = 0;
	  else
	    af.modifier      = -3;
	}
	else
	  af.modifier      = -6;
      affect_to_char(victim,&af);
      af.location     = APPLY_AC;
      if (!IS_NPC(victim)
	  && number_percent() < get_skill(victim,gsn_blind_fighting)){
	if (is_affected(victim, gsn_battletrance))
	  af.modifier      = 0;
	else
	  af.modifier      = +5;
      }
      else
	af.modifier      = +15;
      affect_to_char(victim,&af);
      act_new( "You are blinded!", victim,NULL,NULL,TO_CHAR,POS_DEAD );
      act("$n appears to be blinded.",victim,NULL,NULL,TO_ROOM);
      }
      break;

      //HEART
    case 10:
      if (!is_affected(victim, gsn_spellbane)){
	af.type = gsn_spellbane;
	af.duration += ch->level / 8;
	af.location = APPLY_SPELL_LEVEL;
	af.modifier = -ch->level / 2;
	affect_to_char(victim, &af);
      }

      //LUNGS
    case 11:
      if ( !is_affected(victim, gen_move_dam)){
	AFFECT_DATA af, *paf;

	af.type = gen_move_dam;
	af.duration = number_range(1, 2);
	af.where = TO_AFFECTS;
	af.bitvector = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	paf = affect_to_char(victim, &af);
	if (!IS_NPC(ch)){
	  string_to_affect(paf, ch->name);
	}
      }
      break;

      //Kidneys
    case 12:
      if (!is_affected(victim, gsn_poison)){
	AFFECT_DATA* paf;
	af.type		= gsn_poison;
	af.duration	+= 6;
	af.location	= APPLY_STR;
	af.modifier	= -2;
	af.bitvector	= AFF_POISON;
	paf = affect_to_char( victim, &af );
	if (!IS_NPC(ch))
	  string_to_affect(paf, ch->name);
	SET_BIT(paf->bitvector, AFF_FLAG);
	act_new( "You feel very sick.", victim,NULL,NULL,TO_CHAR,POS_DEAD );
	act("$n looks very ill.",victim,NULL,NULL,TO_ROOM);
      }
      break;

      //Solar plexus (heavy lag)
    case 13:
      if (get_lagprot(victim) != LAG_SHIELD_NONE){
	dam = 0;
      }
      else{
	if (!is_affected(ch, gsn_deathweaver)){
	  act("You stun $N with the blow.", ch, NULL, victim, TO_CHAR);
	  act("$n stuns you with the blow.", ch, NULL, victim, TO_VICT);
	  act("$n stuns $N with the blow.", ch, NULL, victim, TO_NOTVICT);
	  WAIT_STATE2(victim, number_range(2, 4) * PULSE_VIOLENCE);
	}
      }
      break;

      //Aorta
    case 14:
      if (!is_affected(victim, gen_bleed)){
	AFFECT_DATA af, *paf;

	act("A spray of blood shoots forth from the wound!", victim, NULL, NULL, TO_ROOM);
	act("A spray of blood shoots forth from the wound!", victim, NULL, NULL, TO_CHAR);

	af.type = gen_bleed;
	af.level = ch->level;
	af.duration = number_range(3, 6);
	af.where = TO_NONE;
	af.bitvector = 0;
	af.location = APPLY_NONE;
	/* modifier controls duration of damage (10/tick)*/
	af.modifier = af.duration * 8;
	paf = affect_to_char(victim, &af);
	string_to_affect(paf, ch->name);
      }
      break;

      //Intestine
    case 15:
      if (!is_affected(victim, gsn_plague)){
	AFFECT_DATA* paf;
	const int dur = number_range(3, 6);

	af.where     = TO_AFFECTS;
	af.type 	 = gsn_plague;
	af.level	 = ch->level;
	af.duration  = dur;
	af.location  = APPLY_STR;
	af.modifier  = -1;
	af.bitvector = AFF_PLAGUE;
	affect_to_char(victim,&af);

	/* now damage counter */
	af.where     = TO_NONE;
	af.type 	 = gsn_plague;
	af.level	 = ch->level;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = 0;
	paf = affect_to_char(victim,&af);
	if (!IS_NPC(ch))
	  string_to_affect(paf, ch->name);

	act_new("You scream in agony as plague sores erupt from your skin.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n screams in agony as plague sores erupt from $s skin.",victim,NULL,NULL,TO_ROOM);
      }
      break;

      //Brain
    case 16:
      if (!is_affected(victim, gsn_paralyze)){
	AFFECT_DATA af;
	act("$n has been paralyzed!", victim, NULL, NULL, TO_ROOM);
	act("You have been paralyzed!", victim, NULL, NULL, TO_CHAR);

	af.type      = gsn_paralyze;
	af.level     = ch->level;
	af.duration  = 1;
	af.where     = TO_AFFECTS;
	af.bitvector = 0;
	af.location  = APPLY_NONE;
	af.modifier  = 3;
	affect_to_char( victim, &af );
      }
      break;
    }
  }
  damage(ch, victim, dam, sn, dam_type, TRUE );
}


//functions to set preferred battle position
void do_left( CHAR_DATA* ch, char* argument ){
  if (IS_NPC(ch)){
    send_to_char("Mobs cannot choose battle positions.\n\r", ch);
    return;
  }
  else if (get_skill(ch, gsn_footwork) < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (ch->pcdata->bat_pos == BATPOS_LEFT){
    send_to_char("You're already attacking with left hand.\n\r", ch);
    return;
  }
  else if (!monk_good(ch, WEAR_FEET)){
    send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
    return;
  }
  WAIT_STATE2(ch, PULSE_VIOLENCE );
  ch->pcdata->bat_pos = BATPOS_LEFT;
  send_to_char("You will now attack with your left hand.\n\r", ch);
  act("$n switches $s stance to attack with the left hand.", ch, NULL, NULL, TO_ROOM);
  return;
}

void do_right( CHAR_DATA* ch, char* argument ){
  if (IS_NPC(ch)){
    send_to_char("Mobs cannot choose battle positions.\n\r", ch);
    return;
  }
  else if (get_skill(ch, gsn_footwork) < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (ch->pcdata->bat_pos == BATPOS_RIGHT){
    send_to_char("You're already attacking with right hand.\n\r", ch);
    return;
  }
  else if (!monk_good(ch, WEAR_FEET)){
    send_to_char("Your heavy equipment hinders your movement.\n\r",ch);
    return;
  }
  WAIT_STATE2(ch, PULSE_VIOLENCE );
  ch->pcdata->bat_pos = BATPOS_RIGHT;
  send_to_char("You will now attack with your right hand.\n\r", ch);
  act("$n switches $s stance to attack with the right hand.", ch, NULL, NULL, TO_ROOM);
  return;
}


void do_middle( CHAR_DATA* ch, char* argument ){
  if (IS_NPC(ch)){
    send_to_char("Mobs cannot choose battle positions.\n\r", ch);
    return;
  }
  else if (ch->pcdata->bat_pos == BATPOS_MIDDLE){
    send_to_char("You're already attacking with both hands.\n\r", ch);
    return;
  }
  WAIT_STATE2(ch, PULSE_VIOLENCE );
  ch->pcdata->bat_pos = BATPOS_MIDDLE;
  send_to_char("You will now attack with both hands.\n\r", ch);
  act("$n switches $s stance to attack with both hands.", ch, NULL, NULL, TO_ROOM);
  return;
}

//allows the user to choose which skill they are going to predict
void do_predict( CHAR_DATA* ch, char* argument ){
  AFFECT_DATA af, *paf;
  char buf[MIL];

  const int sn = gsn_predict;
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;

  int choice;
  bool fFound = FALSE;

  if (get_skill(ch, sn) < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (is_affected(ch, sn)){
    send_to_char("You're still concentrating on watching for your last prediction!\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_spellkiller)){
    send_to_char("You cannot predict attacks and spells at the same time.\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument)){
    send_to_char("Predict what command?\n\r", ch);
    return;
  }

  //look for valid command
  for (choice = 0; !IS_NULLSTR(predict_table[choice]); choice++){
    if (!str_prefix(argument, predict_table[choice])){
      fFound = TRUE;
      break;
    }
  }
  if (!fFound){
    send_to_char("You cannot predict that.\n\r", ch);
    return;
  }

  if (ch->mana < cost){
    send_to_char("You can't concentrate enough.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2(ch, lag );
  sprintf(buf, "%s", predict_table[choice]);
  af.type	= sn;
  af.level	= ch->level;
  af.duration	= 12;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  paf = affect_to_char(ch, &af);
  string_to_affect( paf, buf);

  sendf(ch, "You begin to look for any signs of %s.\n\r", paf->string);
  act("$n begins to analyze your every move.", ch, NULL, NULL, TO_ROOM);
}

//checks for blademaster predict (ch is user of skill, victim is user of predict)
bool predictCheck( CHAR_DATA* ch, CHAR_DATA* victim, char* command, char* skill){
  AFFECT_DATA* paf, af;
  OBJ_DATA* obj;
  char buf[MIL];
  int ini_dam, dam;
  int sn = gsn_predict;

  if (ch == NULL || victim == NULL || IS_NULLSTR(command) || IS_NULLSTR(skill))
    return FALSE;
  else if (IS_IMMORTAL(ch))
    return FALSE;
  //anti abuse for combat opening
  else if (ch->fighting == NULL && is_affected(ch, gsn_counter))
    return FALSE;

  ini_dam = dam = dice(6, 6) + number_range(victim->level, 2 * victim->level);

  //remove predict from user of the skill
  check_dispel(-99, ch, gsn_predict );

  if (IS_AFFECTED(victim, AFF_SLOW) && !IS_AFFECTED(victim, AFF_HASTE))
    return FALSE;
  else if ( (paf = affect_find(victim->affected, gsn_predict)) == NULL)
    return FALSE;
  else if (IS_NULLSTR(paf->string) || str_cmp(paf->string, command))
    return FALSE;
  else if (!monk_good(victim, WEAR_HEAD))
    return FALSE;
  else if (number_percent() > get_skill(victim, gsn_predict)){
    check_improve(victim, gsn_predict, FALSE, 10);
    return FALSE;
  }
  else if (get_eq_char(victim, WEAR_WIELD) == NULL)
    return FALSE;
  else if (!can_see(victim, ch) && !is_affected(victim, gsn_battlefocus)){
    return FALSE;
  }
  //must have 2 weapons
  else if (get_eq_char(victim, WEAR_SECONDARY) == NULL)
    return FALSE;
  //get obj for first hit
  else if ( (obj = get_eq_char(victim, WEAR_WIELD)) == NULL)
    return FALSE;
  else if (victim->position < POS_FIGHTING && victim->position > POS_STUNNED){
    const int gsn_vigil = skill_lookup("vigil");
    //blademaster vigil
    if (!IS_AFFECTED(victim, AFF_SLEEP) && number_percent() < get_skill(victim, gsn_vigil)){
      send_to_char("You sense an impeding attack!\n\r", victim);
      act("$n springs up suddenly $s weapons flashing in a deadly arc!", victim, NULL, NULL, TO_ROOM);
      check_improve(victim, gsn_vigil, TRUE, 30);
      sn = gsn_vigil;
      dam = 3 * dam / 2;
      do_wake(victim, "");
    }
    else
      return FALSE;
  }

  if (victim->fighting == NULL || victim->fighting != ch){
    sprintf(buf, "You're a fool if you think you can surprise me, %s!", PERS2(ch));
    j_yell(victim, buf );
  }

  //lag user at least 2 rounds
  WAIT_STATE(ch, 2 * PULSE_VIOLENCE);

  //set the anti-abuse affect
  af.type	= gsn_counter;
  af.level	= ch->level;
  af.duration	= 0;
  af.where	= TO_NONE;
  af.bitvector	= 0;
  af.modifier	= 0;
  af.location	= APPLY_NONE;
  affect_to_char(ch, &af);

  act("$n steps quickly out of your way ready for the $t!", victim, skill, ch, TO_VICT);
  act("You step quickly out of $N's way ready for the $t!", victim, skill, ch, TO_CHAR);
  act("$n steps quickly out of $N's way ready for the $t!", victim, skill, ch, TO_NOTVICT);

  //hit the use of the skill
  damage(victim, ch, dam, sn, attack_table[obj->value[3]].damage, TRUE);

  check_improve(victim, gsn_predict, TRUE, 5);

  if (ch->fighting != victim)
    return TRUE;

  //second counter
  if ( (obj = get_eq_char(victim, WEAR_SECONDARY)) != NULL){
    int gsn_twin_counter = skill_lookup("twin counter");
    if (number_percent() > get_skill(victim, gsn_twin_counter)){
      check_improve(victim,gsn_twin_counter,FALSE,10);
      return TRUE;
    }
    else if (monk_good(victim, WEAR_HANDS)){
      check_improve(victim,gsn_twin_counter,TRUE,10);
      damage(victim, ch, 4 * ini_dam / 5, gsn_twin_counter, attack_table[obj->value[3]].damage, TRUE);
    }
  }
  return TRUE;
}

//allows the user to choose which spell they are going to kill when its cast by person attacking them
void do_spellkiller( CHAR_DATA* ch, char* argument ){
  AFFECT_DATA af;

  const int sn = gsn_spellkiller;
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;

  int choice;

  if (get_skill(ch, sn) < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (is_affected(ch, sn)){
    send_to_char("You're still concentrating on killing the last chosen spell!\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_predict)){
    send_to_char("You cannot predict attacks and spells at the same time.\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument)){
    send_to_char("Try to kill which spell?\n\r", ch);
    return;
  }
  if ( (choice = skill_lookup(argument)) < 1
       || skill_table[choice].spell_fun == spell_null
       || IS_GEN(choice)
       ){
    send_to_char("No such spell.\n\r", ch);
    return;
  }

  if (ch->mana < cost){
    send_to_char("You can't concentrate enough.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2(ch, lag );

  af.type	= sn;
  af.level	= ch->level;
  af.duration	= 12;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= choice;
  affect_to_char(ch, &af);

  sendf(ch, "You begin to look for any signs of %s.\n\r", skill_table[choice].name);
  act("$n begins to analyze your every move.", ch, NULL, NULL, TO_ROOM);
}


//checks for blademaster spellkiller (ch is user of skill, victim is user of spellkiller)
bool spellkillerCheck( CHAR_DATA* ch, CHAR_DATA* victim, int spell ){
  AFFECT_DATA* paf;
  OBJ_DATA* obj;
  int dam;
  int sn = gsn_spellkiller;

  if (ch == NULL || victim == NULL || spell < 1)
    return FALSE;
  else if (IS_IMMORTAL(ch))
    return FALSE;
  //classes that cannot be spellkilled because they commune etc.
  else if (ch->class == gcn_cleric
	   || ch->class == gcn_shaman
	   || ch->class == gcn_healer
	   || ch->class == gcn_paladin
	   || ch->class == gcn_druid
	   || ch->class == gcn_psi)
    return FALSE;
  else if (IS_AFFECTED(victim, AFF_SLOW) && !IS_AFFECTED(victim, AFF_HASTE))
    return FALSE;
  //must be fighting the person casting
  else if (victim->fighting != ch)
    return FALSE;
  if ( (paf = affect_find(victim->affected, sn)) == NULL)
    return FALSE;
  else if (paf->modifier < 1)
    return FALSE;
  else if (paf->modifier != spell)
    return FALSE;
  else if (!can_see(victim, ch) && !is_affected(victim, gsn_battlefocus)){
    return FALSE;
  }
  else if (!monk_good(victim, WEAR_ABOUT))
    return FALSE;
  else if (number_percent() > get_skill(victim, sn)){
    check_improve(victim, sn, FALSE, 1);
    return FALSE;
  }
  //must have 2h weapon
  else if ( (obj = has_twohanded(victim)) == NULL)
    return FALSE;

  dam = dice(6, 6) + skill_table[spell].min_mana;

  act("You strike at the spell as it takes shape!", victim, skill_table[paf->modifier].name, ch, TO_CHAR);
  act("$n strikes at the spell as it takes shape!", victim, skill_table[paf->modifier].name, ch, TO_ROOM);

  //hit the user of the spell
  damage(victim, ch, dam, sn, attack_table[obj->value[3]].damage, TRUE);
  check_improve(victim, sn, TRUE, 1);

  return TRUE;
}
