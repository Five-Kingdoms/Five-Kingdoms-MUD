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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"
#include "recycle.h"
#include "jail.h"
#include "cabal.h"
#include "bounty.h"
#include "armies.h"
#include "clan.h"
#include "autoquest.h"

extern void dreamprobe_cast( CHAR_DATA* victim, char* msg );
CHAR_DATA* masochism_effect( CHAR_DATA* ch, int dam, int dt );
extern bool forms(CHAR_DATA *ch, char *argument, AFFECT_DATA* paf);
extern void fulfill_bounty( CHAR_DATA *ch, CHAR_DATA *victim, char* name);
extern CHAR_DATA* undead_check_guard args( (CHAR_DATA* ch, CHAR_DATA* mob) );
extern CHAR_DATA* knight_rescue args( (CHAR_DATA* ch, CHAR_DATA* victim, int dam, int dt) );
extern void throw(CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj, int throw, bool fThief, bool fAutoMiss);
extern bool check_melee_actions( CHAR_DATA* ch);
extern bool check_secart_actions( CHAR_DATA* ch);
extern void deathblow( CHAR_DATA* ch, CHAR_DATA* victim, int type, bool fMiss );
extern void fire_ranged_weapons( CHAR_DATA* ch, CHAR_DATA* victim );

//checks if doomsinger works
bool doomsingerCheck( CHAR_DATA* ch ){
  if (IS_NPC(ch))
    return FALSE;
  else if (ch->class != gcn_blademaster)
    return FALSE;
  else if (!is_affected(ch, gsn_doomsinger))
    return FALSE;
  else if (!has_twohanded(ch))
    return FALSE;

  return TRUE;
}
//checks for possibility of deathstrike when blademaster dies to pc
bool deathstrikeCheck( CHAR_DATA* ch, CHAR_DATA* victim){
  const int sn = skill_lookup("deathstrike");
  int skill, anat, dur, dam;

  if (ch == NULL || victim == NULL)
    return FALSE;
  else if (ch == victim)
    return FALSE;
  else if (ch->in_room == NULL || victim->in_room == NULL || ch->in_room != victim->in_room)
    return FALSE;
  else if (IS_IMMORTAL(ch))
    return FALSE;
  else if (IS_NPC(ch))
    return FALSE;
  else if (IS_NPC(victim))
    return FALSE;
  else if (getBmWep(victim, TRUE) == NULL)
    return FALSE;

  //get the skill/chance
  skill = get_skill(victim, sn );

  if (skill < 2)
    return FALSE;

  skill = (skill - 50) * 2;
  skill += (get_curr_stat(victim, STAT_LUCK) - get_curr_stat(ch, STAT_LUCK)) * 5;

  if (number_percent() > skill){
    check_improve(victim, sn, FALSE, -99);
    dam = 0;
  }
  else{
    check_improve(victim, sn, TRUE, -99);
    dam = ch->max_hit / 3;
  }

  //get the duration based on anatomy
  anat = anatomy_lookup(race_table[ch->race].name);
  anat = victim->pcdata->anatomy[anat];

  if (anat < 101)
    dur = 0;
  else if (anat < 111)
    dur = 1;
  else
    dur = 2;

  act("$n suddenly spins about, $s back against your chest!", victim, NULL, ch, TO_VICT );
  act("$n suddenly spins about, $s back against $N's chest!", victim, NULL, ch, TO_NOTVICT );
  act_new("You spin about placing your back against $N's chest!", victim, NULL, ch, TO_CHAR, POS_DEAD );


  //if they can survive the hit, just damage, otherwise, mortal wound
  if (ch->hit < dam){
    int temp_hp = ch->hit;
    int temp_mana = ch->mana;
    int temp_pos = victim->position;

    victim->position = POS_STANDING;
    //fake messages
    dam_message( victim, victim, NULL, dam, sn, FALSE, FALSE, sn, TRUE );
    dam_message( victim, ch, NULL, dam, sn, FALSE, FALSE, sn, TRUE );
    mercy_effect(victim, ch, dur );

    victim->position = temp_pos;

    ch->hit = temp_hp;
    ch->mana = temp_mana;
    return TRUE;
  }
  else{
    int temp_pos = victim->position;

    victim->position = POS_STANDING;

    dam /= 2;
    dam_message( victim, victim, NULL, dam, sn, FALSE, FALSE, sn, TRUE );
    damage(victim, ch, dam, sn, DAM_INTERNAL, TRUE );

    victim->position = temp_pos;
    return FALSE;
  }
}

//blademaster deathweaver defense check
CHAR_DATA* bmDeathweaverCheck( CHAR_DATA* att, CHAR_DATA* weaver){
  const int MAX_CHARS = 6;
  CHAR_DATA* vch;
  CHAR_DATA* chars[MAX_CHARS];

  int max = 0;

  if (IS_NPC(weaver))
    return NULL;
  else if (weaver->class != gcn_blademaster)
    return NULL;
  else if (weaver->mana < 2)
    return NULL;
  else if (!is_affected(weaver, gsn_deathweaver))
    return NULL;
  else if (number_percent() < (get_skill(weaver, gsn_deathweaver) / 2 + (get_curr_stat(weaver, STAT_LUCK) - 14) * 2) )
    return NULL;

  check_improve(weaver, gsn_deathweaver, TRUE, 1);

  /* find possible redirection targets */
  for (vch = att->in_room->people; vch; vch = vch->next_in_room){
    if (vch->fighting == NULL || vch->fighting != weaver || vch == att)
      continue;
    else
      chars[max++] = vch;
  }

  if (max < 1)
    return NULL;
  else
    return (chars[number_range(0, max - 1)]);
}

/* checks if person has sheathed weapons, and no primaries */
bool is_sheathed(CHAR_DATA* ch){
  if (get_eq_char(ch, WEAR_SHEATH_L) == NULL)
    return FALSE;
  if (get_eq_char(ch, WEAR_SHEATH_R) == NULL)
    return FALSE;
  if (get_eq_char(ch, WEAR_WIELD) != NULL)
    return FALSE;
  if (get_eq_char(ch, WEAR_SECONDARY) != NULL)
    return FALSE;
  return TRUE;
}

/* ninja shadow arts */
bool shadow_strike(CHAR_DATA* ch, CHAR_DATA* victim){
  const int skill = get_skill(ch, gsn_shadow_strike);
  const int damage_ch = (skill / 2)
    + 2 * (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK))
    + (skill > 80? 3 : 0) + (skill > 90? 6 : 0) + (skill > 99? 9 : 0);
  int chance = 20;

  chance += UMAX(0, (skill - 75) * 2);
  if (IS_NPC(ch))
    return FALSE;
  if (skill < 2 || number_percent() > chance)
    return FALSE;
  if (ch->fighting != victim)
    return FALSE;
  act("You feign and strike at $N's nerve point.", ch, NULL, victim, TO_CHAR);
  act("$n feigns and reaches out towards $N.",ch, NULL, victim, TO_NOTVICT);
  act("$n feigns and reaches out towards you.",ch, NULL, victim, TO_VICT);

  if (number_percent() < damage_ch){
    AFFECT_DATA af, *paf;
    int high = -99;
    int high_loc = 0;
    bool fMax = FALSE;

    check_improve(ch, gsn_shadow_strike, TRUE, 1);

    af.location = number_range(1, 5); //(str to con)
    //check for maximum -3 penalty
    for (paf = victim->affected; paf; paf = paf->next){
      if (paf->type != gsn_shadow_strike)
	continue;
      if (paf->location == af.location && paf->modifier <= -3)
	fMax = TRUE;
      if (paf->modifier > high){
	high = paf->modifier;
	high_loc = paf->location;
      }
    }

    //if highest value is lower then -3 we do nothing
    if (high != -99 && high < -3)
      return FALSE;
    //switch location if required
    if (fMax){
      if (high > -3 && high_loc)
	af.location = high_loc;
      else{
	damage(ch, victim, number_range(1 + ch->level / 2, 1 + ch->level), gsn_shadow_strike, DAM_PIERCE, TRUE);
	return FALSE;
      }
    }
    af.type = gsn_shadow_strike;
    af.level = ch->level;
    af.duration = number_range(1, 3);
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.modifier = -1;
    affect_join(victim, &af);

    damage(ch, victim, number_range(1 + ch->level / 2, 1 + ch->level), gsn_shadow_strike, DAM_PIERCE, TRUE);
    return TRUE;
  }
  else{
    damage(ch, victim, 0, gsn_shadow_strike, DAM_BASH, TRUE);
    check_improve(ch, gsn_shadow_strike, FALSE, 2);
    return FALSE;
  }
}

/* this kicks in when a person fleels giving skilled opponent chance
for free hit to the back
*/

bool throw_effect(CHAR_DATA* ch, CHAR_DATA* victim, bool fAutoMiss){
/* all we do here is check for ninja/thief and their types of
   ammo, then try to throw */
  bool fThief = ch->class == class_lookup("thief");
  bool fNinja = ch->class == class_lookup("ninja");

  OBJ_DATA* obj;

  int throws = (get_skill(ch, gsn_throw) - 60) / 8;
  int i = 1;

  //need double sheeth and empty sheaths
  if (get_skill(ch, gsn_sheath) < 2
      || (get_eq_char(ch, WEAR_SHEATH_L) != NULL
	  && get_eq_char(ch, WEAR_SHEATH_R) != NULL)
      ){
    return FALSE;
  }
  else if ((!fThief && !fNinja) || !can_see(ch, victim))
    return FALSE;
  else if (number_percent() > get_skill(ch, gsn_throw)){
    check_improve(ch, gsn_throw, FALSE, 1);
    return FALSE;
  }

/* get ammo */
  for (obj = ch->carrying; obj; obj = obj->next_content){
    if (obj->item_type != ITEM_THROW)
      continue;
    if (fNinja && obj->cost != 0)
      continue;
    if (fThief && obj->cost != 1)
      continue;
    break;
  }
  if (obj == NULL || obj->value[0] < 1)
    return FALSE;

  if (fNinja)
    i = number_range(1, throws);
  check_improve(ch, gsn_throw, TRUE, 1);
  throw(ch, victim, obj, i, fThief, fAutoMiss);
  return TRUE;
}

bool backcutter_effect(CHAR_DATA* ch, CHAR_DATA* victim, bool fAutoMiss){
  OBJ_DATA* obj;
  int chance = 0;
  int sn = skill_lookup("backcutter");
  int dam_type = DAM_PIERCE;
  int dam = number_range(ch->level, 2 * ch->level);
  const int cost = skill_table[sn].min_mana;

/* safety */
  if (ch == NULL)
    return FALSE;

  if (IS_NPC(ch) || is_safe_quiet(ch, victim))
    return FALSE;

  if ( (chance = get_skill(ch, gsn_orealts_2nd))  < 1){
    if ( (chance = get_skill(ch, sn)) < 1)
      return FALSE;
  }

/* final parting effect */
  if (is_affected(ch, gsn_fpart) || is_affected(victim, gsn_fpart)){
    /* check if fleeing person gets hit */
    if (victim->hit < (25 * victim->max_hit / 100)){
      int dam = number_range(2 * victim->level, 3 * victim->level);
      if (is_affected(ch, gsn_fpart)){
	affect_strip(ch, gsn_fpart);
	act_new(skill_table[gsn_fpart].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
      }
      if (is_affected(victim, gsn_fpart)){
	affect_strip(victim, gsn_fpart);
	act_new(skill_table[gsn_fpart].msg_off,victim,NULL,NULL,TO_CHAR,POS_DEAD);
      }
      damage(ch, victim, dam, gsn_fpart, DAM_INTERNAL, TRUE);
      return TRUE;
    }//sucessful final parting
  }//final part check

/* check for mana */
  if (ch->mana < cost)
    return FALSE;

  /* get weapon to be used if any */
  obj = get_eq_char( ch, WEAR_WIELD );
  /* warriors can only do this using spears and staffs */
  if ((!obj || (obj->value[0] != WEAPON_SPEAR
		&& obj->value[0] != WEAPON_STAFF))
      && ch->class == class_lookup("warrior")){
    return FALSE;
  }

  if (fAutoMiss || number_percent() > chance){
    check_improve(ch, gsn_orealts_2nd, FALSE, 1);
    check_improve(ch, sn, FALSE, 1);
    dam = 0;
  }

  /* draw mana only on success */
  ch->mana -= cost;
  /* hit them */

  dam_type = obj ? attack_table[obj->value[3]].damage : DAM_BASH;
  damage(ch, victim, dam, sn, dam_type, TRUE);

  check_improve(ch, gsn_orealts_2nd, FALSE, 1);
  check_improve(ch, sn, FALSE, 1);
  return TRUE;
}


/* checks and validates if ch is on crusade against victim */
/* Returns CRUSADE_NONE, CRUSADE_MATCH, CRUSADE_NOTMATCH */
/* used in defense checks */
int check_crusade(CHAR_DATA* ch, CHAR_DATA* victim){
  AFFECT_DATA* paf;

  if (IS_NPC(ch) || IS_NPC(victim))
    return CRUSADE_NONE;

  if ( (paf = affect_find(ch->affected, gen_crusade)) == NULL)
    return CRUSADE_NONE;

/* start checking */
  switch(paf->level){
  case CRUSADE_RACE:
    if (paf->modifier == -1
	&& IS_AVATAR(victim))
      return CRUSADE_MATCH;
    else if (paf->modifier == race_lookup("undead") && IS_UNDEAD(victim))
      return CRUSADE_MATCH;
    else if (paf->modifier == victim->race)
      return CRUSADE_MATCH;
    break;
  case CRUSADE_CLASS:
    if (paf->modifier == victim->class)
      return CRUSADE_MATCH;
    break;
  case CRUSADE_CABAL:
    if (victim->pCabal && paf->modifier == get_parent(victim->pCabal)->vnum)
      return CRUSADE_MATCH;
    break;
  default:
    return CRUSADE_NONE;
  }
  return CRUSADE_NOTMATCH;
}

/* checks for avenger match, uses same codes as check_crusade */
int check_avenger(CHAR_DATA* ch, CHAR_DATA* victim){
  AFFECT_DATA* paf, *baf;

  if (IS_NPC(ch))
    return CRUSADE_NONE;

  /* check for the paf */
  if ( (paf = affect_find(ch->affected, gen_avenger)) == NULL)
    return CRUSADE_NONE;

  /* we know the char is avenger, check victim now */
  if (!is_name(paf->string, victim->name))
    return CRUSADE_NOTMATCH;

  /* now if npc we check if this is the victim we are looking for */
  if (IS_NPC(victim)){
    for (baf = victim->affected; baf != NULL; baf = baf->next){
      if (baf->type == gsn_avenger
	  && baf->modifier == paf->modifier)
	return CRUSADE_MATCH;
    }
    return CRUSADE_NOTMATCH;
  }
  else
    return CRUSADE_MATCH;
}

/* handle all combinations of defenses */
bool check_defenses( CHAR_DATA *ch,
		CHAR_DATA *victim,
		OBJ_DATA* obj,
		int dt,
		int h_roll,
		int lvl,
		bool fVirt,
		int sn){

  const int MAX_DEF = 9;	//Current maximum number of defenses
  int seed[MAX_DEF];
  int order[MAX_DEF];
  int i;
  int cur_max = MAX_DEF;
  int cur_order = 0;

  /* absolute fails */
  if (victim->position < POS_FIGHTING
      || is_affected(victim, gsn_mortally_wounded)
      || is_affected(ch, gsn_feign))
    return FALSE;

  /* check for mirage */
  if (ch->class == gcn_psi
      && is_affected(ch, gsn_mirage)
      && !saves_spell(ch->level, victim, DAM_MENTAL, skill_table[gsn_mirage].spell_type)){
    return FALSE;
  }
/* we first fill the seed with ordered defense numbers */
  for (i = 0; i  < MAX_DEF; i++){
    seed[i] = i;
    order[i] = -1;
  }

/* begin to randomly put seed in order */
  for (i = 0; i  < MAX_DEF; i++){
    int rand = number_range(0, cur_max - 1);
    int j = 0;
    /* place a random seed into a spot in order */
    order[cur_order++] = seed[rand];
    /* set that seed spot to -1 indicating empty */
    seed[rand] = -1;

    /* now we need to compact the seed array */
    for (j = 0; j < cur_max; j++){
      /* we check if the current sport is empty */
      if (seed[j] == -1 ){
	/* we compact by bubbling the empty spot up */
	seed[j] = seed[j + 1];
	seed[j + 1] = -1;
      }
    }
    /* lower current max by one, since the empty spot is now at top */
    cur_max--;
  }

/* DEBUG
  sendf(ch, "Defense Order [");
  for (i = 0; i < MAX_DEF; i++)
    sendf(ch, "%d ", order[i]);
  sendf(ch, "]\n\r");
*/
/* now we apply the defenses */

/* PARRY		0
 * DUAL PARRY		1
 * MONK FIRST PARRY	2
 * MONK SECOND PARRY	3
 * DODGE		4
 * SHIELD BLOCK		5
 * WEAPON BLOCK		6
 * RIPOSTE		7
 * BLINK		8
 */

  for (i = 0; i < MAX_DEF; i++){
    switch (order[i]){
/* PARRY */
    case 0:
      if (check_parry( ch, victim, obj, dt, h_roll, lvl, fVirt, sn))
	return TRUE;
      break;
/* DUAL PARRY */
    case 1:
      if (check_dual_parry( ch, victim, obj, dt, h_roll, lvl, fVirt, sn))
	return TRUE;
      break;
/* MONK FIRST PARRY */
    case 2:
      if ( check_parry_monk( ch, victim, obj, dt, h_roll, lvl,  FALSE, fVirt, sn))
	return TRUE;
      break;
/* MONK SECOND PARRY */
    case 3:
      if ( check_parry_monk( ch, victim, obj, dt, h_roll, lvl,  TRUE, fVirt, sn))
	return TRUE;
      break;
/* DODGE */
    case 4:
      if ( check_dodge( ch, victim, obj, dt, h_roll, lvl, fVirt, sn) )
	return TRUE;
      break;
/* SHIELD BLOCK */
    case 5:
      if ( check_shield_block( ch, victim, obj, dt, h_roll, lvl, fVirt, sn) )
	return TRUE;
      break;
/* WEAPON BLOCK */
    case 6:
      if ( check_block( ch, victim, obj, dt, h_roll, lvl, fVirt, sn) )
	return TRUE;
      break;
/* RIPOSTE */
    case 7:
      if ( check_riposte( ch, victim, obj, dt, h_roll, lvl, fVirt, sn) )
	return TRUE;
      break;
/* BLINK */
    case 8:
      if ( check_blink( ch, victim, obj, dt, h_roll, lvl, fVirt, sn) )
	return TRUE;
      break;
    }
/* END DEFENSES */
  }
  return FALSE;
}

/* Used to morph a mob to current victims level and power */
/* Assumes that the original stats for the mob were set for */
/* an opponent of level 50				   */
void morph_mob(CHAR_DATA* mob, CHAR_DATA* victim){
  MOB_INDEX_DATA* pMobIndex;

  int mod = 0;
  int hp_level = 0;
  int level = mob->level;
  int i = 0;
  int hitroll = 0;
  int damroll = 0;

  bool fWarrior = FALSE;
  bool fHalfWar = FALSE;
  bool fRouge = FALSE;
  bool fMage = FALSE;

/* decide what type of character this is */
  if (IS_NPC(victim))
    fWarrior = TRUE;
  else if (victim->class == gcn_blademaster)
    fWarrior = TRUE;
  else if (victim->class == class_lookup("monk"))
    fHalfWar = TRUE;
  else if (victim->class == class_lookup("vampire"))
    fWarrior = TRUE;
  else{
    int attack = 1;
    int defense = 0;

/* Offense */
    if (victim->pcdata->learned[gsn_second_attack])
      attack++;
    if (victim->pcdata->learned[gsn_third_attack])
      attack++;
    if (victim->pcdata->learned[gsn_fourth_attack])
      attack++;
    if (victim->pcdata->learned[gsn_blades])
      attack += 2;
    if (victim->pcdata->learned[gsn_dan_blade])
      attack++;

/* defense */
    if (victim->pcdata->learned[gsn_parry])
      defense++;
    if (victim->pcdata->learned[gsn_dual_parry])
      defense++;
    if (victim->pcdata->learned[gsn_dodge])
      defense++;
    if (victim->pcdata->learned[gsn_shield_block]
	|| victim->pcdata->learned[gsn_2hands])
      defense++;
    if (victim->pcdata->learned[gsn_terra_shield])
      defense++;
    if (victim->pcdata->learned[gsn_blades])
      defense++;
    if (victim->pcdata->learned[skill_lookup("blur")])
      defense++;

/* select which type this is */
    if (attack > 3 || defense > 3)
      fWarrior = TRUE;
    else if (attack > 2 || defense > 1){
      if (victim->pcdata->learned[gsn_hide]
	  || victim->pcdata->learned[gsn_camouflage])
	fRouge = TRUE;
      else
	fHalfWar = TRUE;
    }
    else
      fMage = TRUE;
  }
  act("$n seems to change in size and shape to match yours.",
      mob, NULL, victim, TO_VICT);
  act("$n seems to change in size and shape to match $N's.",
      mob, NULL, victim, TO_NOTVICT);

/* adjust things base on general archtype */
/* most of this is based on special guards plus few modifications */
  if (!IS_NPC(mob)){
    bug("morph_mob: PC passed as mob.", 0);
    return;
  }
  else{
    if ( ( pMobIndex = get_mob_index( mob->pIndexData->vnum)) == NULL){
      bug("morph_mob: mob index not found.", mob->pIndexData->vnum);
      return;
    }
  }
  if (fWarrior){
    level += 2;
    hitroll += level/5;
    damroll += level/6;
  }
  else if (fHalfWar){
    level += 1;
    hitroll += level/6;
    damroll += level/6;
  }
  else if (fRouge){
    hitroll -= level/6;
    damroll -= level/8;
  }
  else if (fMage){
    level --;
    hitroll -= level/8;
    damroll -= level/10;
  }

  mob->size = victim->size;
  level = UMAX(5, (victim->level + (victim->level/15) ));
  mod = 100 * victim->level / pMobIndex->level;
  mob->level = level;

  for (i=0;i < MAX_STATS; i++)
    mob->perm_stat[i] = get_max_train(victim, i);
  for (i=0; i < 4; i++)
    mob->armor[i] = pMobIndex->ac[i] * mod / 100;
  hp_level = 100 * mob->hit / mob->max_hit;
  mob->max_hit = UMAX(1, mod * mob->max_hit / 100);
  mob->hit = UMAX(1, hp_level * mob->max_hit / 100);

  damroll = pMobIndex->damage[DICE_BONUS] * mod / 100;
  hitroll = pMobIndex->hitroll * mod / 100;

  mob->hitroll    = hitroll;
  mob->damroll    = damroll;
  mob->damage[DICE_NUMBER]  = UMAX(1, mod * pMobIndex->damage[DICE_NUMBER] / 100);

}

/* PREVOILENCE_CHECK */
/* Called in fight.c only */
/* same args as set_fighting in which it will be called. */
int PREVIOLENCE_CHECK( CHAR_DATA *ch, CHAR_DATA *victim)
{
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int fSuccess = TRUE;
  int result = 0;

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL)
      if (paf->type == paf_next->type)
	continue;

    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(vch, paf, victim, paf->type, NULL, NULL,
				      TRUE, EFF_UPDATE_PREVIOLENCE)) != TRUE) {
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
}//end PREVIOLENCE_CHECK

/* POSTVOILENCE_CHECK */
/* Called in fight.c only */
/* same args as stop_fighting in which it will be called. */
int POSTVIOLENCE_CHECK( CHAR_DATA *ch)
{
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int fSuccess = TRUE;
  int result = 0;

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }

    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(vch, paf, NULL, paf->type,
				      NULL, NULL, TRUE, EFF_UPDATE_POSTVIOLENCE)) != TRUE) {
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
}//end POSTVIOLENCE_CHECK

/* DODGE_CHECK */
/* Called in fight.c only */
inline int DODGE_CHECK( CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
  int fSuccess  = TRUE;
  CHAR_DATA* vch = ch;

  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int result = 0;

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
      paf_next = paf->next;
      //we only run the update once per effect.
      if (paf_next != NULL) {
	if (paf->type == paf_next->type) {
	  continue;
	}
      }

      //GENS THAT ARE HANDLED SEPERATLY


      if (IS_GEN(paf->type)) {
	if ((result = run_effect_update(vch, victim, paf, paf->type,
					&dt, NULL, TRUE, EFF_UPDATE_DODGE)) != TRUE) {
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
}// end DODGE_CHECK

/* PARRY_CHECK */
/* Called in fight.c only */
inline int PARRY_CHECK( CHAR_DATA *ch, OBJ_DATA* obj, OBJ_DATA* vobj, int dt)
{

  int fSuccess  = TRUE;
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int result = 0;


//OBJECT ORIENTATED EFFECS>
  if (obj == NULL || vobj == NULL) {
    return fSuccess;
  }

  for (paf = obj->affected; paf != NULL; paf = paf_next) {
      paf_next = paf->next;
      //we only run the update once per effect.
      if (paf_next != NULL) {
	if (paf->type == paf_next->type) {
	  continue;
	}
      }

      //GENS THAT ARE HANDLED SEPERATLY


      if (IS_GEN(paf->type)) {
	if ((result = run_effect_update(vch, obj, vobj, paf->type,
					&dt, NULL, TRUE, EFF_UPDATE_PARRY)) != TRUE) {
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
}// end PARRY_CHECK


/* SHBLOCK_CHECK */
/* Called in fight.c only */
inline int SHBLOCK_CHECK( CHAR_DATA *ch, OBJ_DATA* obj, OBJ_DATA* vobj, int dt)
{
  int fSuccess  = TRUE;
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int result = 0;


//OBJECT ORIENTATED EFFECS>
  if (obj == NULL || vobj == NULL) {
    return fSuccess;
  }

  for (paf = obj->affected; paf != NULL; paf = paf_next) {
      paf_next = paf->next;
      //we only run the update once per effect.
      if (paf_next != NULL) {
	if (paf->type == paf_next->type) {
	  continue;
	}
      }

      //GENS THAT ARE HANDLED SEPERATLY


      if (IS_GEN(paf->type)) {
	if ((result = run_effect_update(vch, obj, vobj, paf->type, &dt,
					NULL, TRUE, EFF_UPDATE_SHBLOCK)) != TRUE) {
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

}// end SHBLOCK_CHECK



/* PREKILL_CHECK */
/* Called in fight.c only */
/* same args as raw_kill in which it will be called. */
int PREKILL_CHECK( CHAR_DATA *ch, CHAR_DATA *victim)
{
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int fSuccess = TRUE;
  int result = 0;

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }

    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(vch, victim, paf, paf->type,
				      NULL, NULL, TRUE, EFF_UPDATE_PREKILL)) != TRUE) {
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

}//end PREKILL_CHECK

/* PREDEATH_CHECK */
/* Called in fight.c only */
/* same args as raw_kill in which it will be called. */
int PREDEATH_CHECK( CHAR_DATA *ch, CHAR_DATA *victim)
{
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int fSuccess = TRUE;
  int result = 0;

  //MALFORM WEAPON
  run_effect_update(victim, vch, NULL, gen_malform, NULL, NULL, TRUE, EFF_UPDATE_PREDEATH);

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }

    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(vch, victim, paf, paf->type,
				      NULL, NULL, TRUE, EFF_UPDATE_PREDEATH)) != TRUE) {
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

}//end PREDEAH_CHECK


/* POSTKILL_CHECK */
/* Called in fight.c only */
/* same args as raw_kill in which it will be called. */
int POSTKILL_CHECK( CHAR_DATA *ch, CHAR_DATA *victim)
{
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int fSuccess = TRUE;
  int result = 0;

  //MALFORM WEAPON//
  run_effect_update(vch, victim, NULL, gen_malform, NULL, NULL, TRUE, EFF_UPDATE_POSTKILL);

  //COUP
  run_effect_update(vch, victim, NULL, gen_coup, NULL, NULL, TRUE, EFF_UPDATE_POSTKILL);

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }

//SPECIAL GENS HERE
    if (paf->type == gen_malform
	|| paf->type == gen_coup) {
      continue;
    }
    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(vch, victim, paf, paf->type,
				      NULL, NULL, TRUE, EFF_UPDATE_POSTKILL)) != TRUE) {
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
}//end POSTKILL_CHECK



/* POSTDEATH_CHECK */
/* Called in fight.c only */
/* same args as raw_kill in which it will be called. */
/* FIRST argument is the attacker, SECOND the person that died */
int POSTDEATH_CHECK( CHAR_DATA *ch, CHAR_DATA *victim)
{
  CHAR_DATA* vch = victim;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int fSuccess = TRUE;
  int result = 0;

  //COUP
  run_effect_update(victim, ch, NULL, gen_coup, NULL, NULL, TRUE, EFF_UPDATE_POSTDEATH);


  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }
//SPECIAL GENS HERE
    if (paf->type == gen_coup) {
      continue;
    }

    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(victim, vch, paf, paf->type,
				      NULL, NULL, TRUE, EFF_UPDATE_POSTDEATH)) != TRUE) {
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
}//end POSTDEATH_CHECK




/* PRECOMBAT_CHECK */
/* Called in fight.c only */
/* same args as multi_hit in which it will be called. */
int PRECOMBAT_CHECK( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
//This is the function that perfomrs checking for any
//PRE_COMBAT effects.
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int fSuccess = TRUE;
  int result = 0;

/* victim check
  for (paf = victim->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }
    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(vch, paf, victim, paf->type,
				      &dt, NULL, TRUE, EFF_UPDATE_PRECOMBAT)) != TRUE) {
	if (result == -1) {
	  return -1;
	}
	else {
	  fSuccess = result;
	}
      }
    }
  }
*/

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }
//SPECIAL GENS HERE
    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(vch, paf, victim, paf->type,
				      &dt, NULL, TRUE, EFF_UPDATE_PRECOMBAT)) != TRUE) {
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

}//end PRECOMBAT CHECK



/* POSTCOMBAT_CHECK */
/* Called in fight.c only */
/* same args as multi_hit in which it will be called. */

int POSTCOMBAT_CHECK( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int result = 0;
  int fSuccess = TRUE;

//SPECIAL GENS
  //-----PANIC---//
  if (vch->class == class_lookup("berserker")) {
    fSuccess =  run_effect_update(vch, NULL, victim, gen_panic, NULL,
				  NULL, TRUE, EFF_UPDATE_POSTCOMBAT);
  }

  //-----PLUMBUM---//
  if ( (paf = affect_find(ch->affected, gsn_plumbum)) != NULL) {
    run_effect_update(ch, victim, paf, gen_plumbum, NULL, NULL, dt, EFF_UPDATE_POSTCOMBAT);
  }

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
      paf_next = paf->next;
      //we only run the update once per effect.
      if (paf_next != NULL) {
	if (paf->type == paf_next->type) {
	  continue;
	}
      }

      //GENS THAT ARE HANDLED SEPERATLY
      if (paf->type == gen_panic
	  || paf->type == gen_plumbum) {
	continue;
      }


      if (IS_GEN(paf->type)) {
	if ((result = run_effect_update(vch, victim, paf, paf->type,
					NULL, NULL, dt, EFF_UPDATE_POSTCOMBAT)) != TRUE) {
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

}//end POSTCOMBAT CHECK

/* PREDAMAGE_CHECK */
/* Called in fight.c only */
/* same args as damage in which it will be called. */

int PREDAMAGE_CHECK(CHAR_DATA *ch,CHAR_DATA *victim,int* dam,int* dt,int* dam_type)
{
//This is the function that perfomrs checking for any
//PRE_DAMAGE effects.

  CHAR_DATA* vch = ch;

  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int result = 0;
  int fSuccess = TRUE;

//SPECIAL GENS
//----POSITION----//
    if (victim->position == POS_MEDITATE) {
	act_new("You stop meditating and stand up.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n stops meditating and stands up.",victim,NULL,NULL,TO_ROOM);
//	victim->position = POS_STANDING;
    }
    for (paf = victim->affected; paf; paf = paf_next ){
      paf_next = paf->next;
//-----BLADES----- ON VICTIM (DEFLECT)
      if ( paf->type == gen_blades){
	run_effect_update(ch, victim, paf, paf->type, dam, dam_type, *dt, EFF_UPDATE_PREDAMAGE);
      }
//-----TERRASHIELD----- ON VICTIM (DEFLECT)
      else if ( paf->type == gen_terra_shield){
	run_effect_update(ch, victim, paf, paf->type, dam, dam_type, *dt, EFF_UPDATE_PREDAMAGE);
      }
//-----REFLECTIVE SHIELD----- ON VICTIM (REFLECT)
      else if ( paf->type == gen_ref_shield) {
	run_effect_update(ch, victim, paf, paf->type, dam, dam_type,*dt, EFF_UPDATE_PREDAMAGE);
      }
//-----CHRONO SHIELD----- ON VICTIM (ABSORB)
      else if ( paf->type == gen_chron) {
	run_effect_update(ch, victim, paf, paf->type, dam, dam_type,*dt, EFF_UPDATE_PREDAMAGE);
      }
//-----EFUZAN'S WARD----- ON VICTIM (REDUCTION)
      else if ( paf->type == gen_sav_ward) {
	run_effect_update(ch, victim, paf, paf->type, dam, dam_type, *dt, EFF_UPDATE_PREDAMAGE);
      }
//------PC CHARM ON VICTIM (WAKE UP)
      else if (paf->type == gen_pc_charm) {
	run_effect_update(victim, victim, paf, paf->type, dam, dt, TRUE, EFF_UPDATE_PREDAMAGE);
      }
//------REQUEST ON VICTIM (SHATTER)
      else if (paf->type == gen_request) {
	run_effect_update(victim, victim, paf, paf->type, dam,  dt, TRUE, EFF_UPDATE_PREDAMAGE);
      }
//------CONJURE ON VICTIM (SHATTER)
      else if ( paf->type == gen_conjure) {
	run_effect_update(victim, victim, paf, paf->type, dam,  dt, TRUE, EFF_UPDATE_PREDAMAGE);
      }
//------CHANT ON VICTIM (DRAIN)
      else if (paf->type == gen_chant) {
	run_effect_update(victim, victim, paf, paf->type, dam,  dt, TRUE, EFF_UPDATE_PREDAMAGE);
      }
    }
/* NORMAL PAFS */

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
      paf_next = paf->next;
      //we only run the update once per effect.
      if (paf_next != NULL) {
	if (paf->type == paf_next->type) {
	  continue;
	}
      }

      //GENS THAT ARE HANDLED IN ABOVE
      if (paf->type == gen_blades
	  || paf->type == gen_terra_shield
	  || paf->type == gen_sav_ward
	  || paf->type == gen_ref_shield
	  || paf->type == gen_chron) {
	continue;
      }
      /* special check for apierce */
      if (paf->type == gen_apierce){
	  if ((result = run_effect_update(vch, victim, paf, paf->type, dam,
					 dam_type, *dt, EFF_UPDATE_PREDAMAGE)) == TRUE){
	    *dt = gen_apierce;
	  }
      }
//------BATTER( NEEDS A DAMAGE CHANGE IF CON DAMAGE ACTIVE)
      else if ( paf->type == gen_batter){
	if (number_percent() < (get_skill(ch, gsn_con_damage) - 60) * 2){
	  *dam_type = DAM_BASH;
	  if (*dt >= TYPE_NOBLOCKHIT)
	    *dt = attack_lookup("slap") + TYPE_NOBLOCKHIT;
	  else if (*dt >= TYPE_HIT)
	    *dt = attack_lookup("slap") + TYPE_HIT;
	  check_improve(vch, gsn_con_damage, TRUE, 1);
	}
	run_effect_update(vch, victim, paf, paf->type, dam, dam_type, *dt, EFF_UPDATE_PREDAMAGE);
      }
      else if (IS_GEN(paf->type)) {
	if ((result = run_effect_update(vch, victim, paf, paf->type, dam,
					dam_type, *dt, EFF_UPDATE_PREDAMAGE)) != TRUE) {
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
}//end PREDAMAGE CHECK


/* POSTDAMAGE_CHECK */
/* Called in fight.c only */
/* same args as damage in which it will be called. */

int POSTDAMAGE_CHECK(CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* obj, int dam,int dt,int dam_type)
{
  //THIS IS SPLIT INTO TW PARTS:
  //OBJECT AND CHARATER

  CHAR_DATA* vch = ch;

  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int result = 0;
  int fSuccess = TRUE;


//-----VENGEGEANCE----- ON VICTIM (REFLECT)
  if ( (paf = affect_find(victim->affected, gen_vengeance)) != NULL) {
    run_effect_update(vch, victim, paf, paf->type, &dam, &dam_type, dt, EFF_UPDATE_POSTDAMAGE);
  }
//-----TEMPORAL AVENGER----- ON VICTIM (COLLECT)
  if ( (paf = affect_find(victim->affected, gen_temp_avenger)) != NULL) {
    run_effect_update(vch, victim, paf, paf->type, &dam, &dam_type, dt, EFF_UPDATE_POSTDAMAGE);
  }



  //REST OF EFFECTCS
  for (paf = vch->affected; paf != NULL; paf = paf_next) {
      paf_next = paf->next;
      //we only run the update once per effect.
      if (paf_next != NULL) {
	if (paf->type == paf_next->type) {
	  continue;
	}
      }

      //GENS THAT ARE HANDLED SEPERATLY
      if (paf->type == gen_vengeance)
	continue;

      if (IS_GEN(paf->type)) {
	if ((result = run_effect_update(vch, victim, paf, paf->type, &dam,
					&dam_type, dt, EFF_UPDATE_POSTDAMAGE)) != TRUE) {
	  if (result == -1) {
	    return -1;
	  }
	  else {
	    fSuccess = result;
	  }
	}
      }
    }


//OBJECT ORIENTATED EFFECS>
  if (obj == NULL || victim == NULL)
    return fSuccess;

  for (paf = obj->affected; paf != NULL; paf = paf_next)
    {
      paf_next = paf->next;
      //we only run the update once per effect.
      if (paf_next != NULL) {
	if (paf->type == paf_next->type) {
	  continue;
	}
      }

      //GENS THAT ARE HANDLED SEPERATLY


      if (IS_GEN(paf->type)) {
	if ((result = run_effect_update(vch, victim, obj, paf->type, &dam,
					&dam_type, dt, EFF_UPDATE_POSTDAMAGE)) != TRUE) {
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



/*
  //-----DARK METAMORPHOSIS---//
  if ( (paf = affect_find(obj->affected, gen_dark_meta)) != NULL)
    fSuccess = run_effect_update(ch, victim, obj, gen_dark_meta, &dam,
    &dam_type, TRUE, EFF_UPDATE_POSTDAMAGE);


  //-----Soul Reaver---//
  if ( (paf = affect_find(obj->affected, gen_soul_reaver)) != NULL)
    fSuccess = run_effect_update(ch, victim, obj, gen_soul_reaver, &dam, &dam_type, TRUE, EFF_UPDATE_POSTDAMAGE);

  //-----Hand of Chaos---//
  if ( (paf = affect_find(obj->affected, gen_handof_chaos)) != NULL)
    fSuccess = run_effect_update(ch, victim, obj, gen_handof_chaos, &dam, &dam_type, TRUE, EFF_UPDATE_POSTDAMAGE);
*/

  return fSuccess;
}//end POSTDAMAGE CHECK

//-----------------------------------END CHECKS----------------------------//



/* ogre damage ignore */
bool ogre_ignore(CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj, int dam, int dt, bool fVirt)
{
  int chance = 5 + (dam > 50? 5 : 0)
    + (dam > 120 ? 80 : 0)
    + (dam > 350 ?  45 : 0);
  bool fOgre = victim->race == grn_ogre;
  bool fBlood = get_skill(victim, gsn_bloodhaze) > 1;

  //safety
  if (obj == NULL )
    fVirt = FALSE;

  if (ch == NULL || victim == NULL || ch == victim)
    return FALSE;

  if (!fOgre && !fBlood){
    return FALSE;
  }

  /* bonus from bloodhaze for ogres */
  if (fOgre && fBlood){
    chance += 15;
  }

  if (dam < 1 ) {
    return FALSE;
  }
  //roll for ignore
  if (number_range(1, 1000) < chance) {
      char buf[MIL];
      sprintf(buf, "You roar in pain and ignore %s %s.",
	      fVirt? "$p's" : "$N's",
	      get_vir_attack(dt));
      act(buf, victim, obj, ch, TO_CHAR);
      sprintf(buf, "$n roars in pain and ignores %s %s.",
	      fVirt? "$p's" : "$N's",
	      get_vir_attack(dt));
      act(buf, victim, obj, ch, TO_NOTVICT);
      sprintf(buf, "$n roars in pain and ignores %s %s.",
	      fVirt? "$p's" : "your",
	      get_vir_attack(dt));
      act(buf, victim, obj, ch, TO_VICT);
      return TRUE;
    }
  return FALSE;
}

/* juggernaut plate damage ignore */
bool jugger_ignore(CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj, int dam, int dt, bool fVirt)
{
  int chance = 40 + (dam < 41 ? (41 - dam) * 4 : 0);

  //safety
  if (obj == NULL)
    fVirt = FALSE;
  if (ch == victim)
    return FALSE;
  if (is_armor_enhanced(victim) < ENHANCE_JUGGER){
    return FALSE;
  }
  if (dam < 1) {
    return FALSE;
  }
  //roll for ignore
  if (number_range(1, 1000) < chance) {
      char buf[MIL];
      sprintf(buf, "%s %s glances off of the Juggernaut Plate.",
	      fVirt? "$p's" : "$N's",
	      get_vir_attack(dt));
      act(buf, victim, obj, ch, TO_ALL);
      return TRUE;
  }
  return FALSE;
}

/* check_anatomy */
/* returns TRUE if learned, -1 if mob level reached. */
inline int check_anatomy_improve(CHAR_DATA* ch, int anatomy, bool fMOB, int iMobLvl)
{
  /*
     runs a check for improvement of monk anatomies.
     Anatomies are learned from mobs bit slower then pc's but the
     maximum level is npc_max of the mob level.
  */

  const int npc_max = 200;//(lvl 50 mob can teach up to 100 anatom.)
  const int npc_pen = -10;//penatly to learnm from npc
  const int npc_extra = 4000;
  const int pc_extra = 1000;

  int chance = 50 + (anatomy? 0: npc_pen); //base level
  int max = get_max_anat(ch, anatomy);

  //Check for max anatomy
  if (ch->pcdata->anatomy[anatomy] >= max)
    {
      ch->pcdata->anatomy[anatomy] = max;
      return FALSE;
    }


//INT
  chance += int_app[get_curr_stat(ch,STAT_INT)].learn / 5;
//LEVEL
  chance += ch->level / 4;
//WIS
  chance += get_curr_stat(ch,STAT_WIS) / 2;
//CURRENT VALUE
  chance -= ch->pcdata->anatomy[anatomy] / 2;

//Check to learn
  if (number_range(1,(fMOB? npc_extra: pc_extra)) < chance)
    {
      //MOB LEARN CHECK
      if (fMOB && (ch->pcdata->anatomy[anatomy] >= (npc_max * iMobLvl / 100 )))
	return -1;
      ch->pcdata->anatomy[anatomy]++;
      return TRUE;
    }
  return FALSE;
}

/* check_anatomy */
/* returns damage increase or 0 in percent */
inline int check_anatomy(CHAR_DATA* ch, CHAR_DATA* victim)
{
  int diceroll = 0;
  int result = 0;
  int anatomy = 0;
  bool fMOB = IS_NPC(victim);

  if ( IS_NPC(ch) || get_skill(ch,gsn_anatomy) < 1)
    return FALSE;

  diceroll = number_percent();
  diceroll -= 2*(get_curr_stat(ch,STAT_LUCK) - 16);

  if (diceroll <= get_skill(ch,gsn_anatomy))
    {
      if (race_table[victim->race].pc_race)
	anatomy = anatomy_lookup(pc_race_table[victim->race].anatomy);

      //check for improve of anatomies.
      check_improve(ch,gsn_anatomy,TRUE,1);

      result = check_anatomy_improve(ch, anatomy, fMOB, victim->level);

      if (result == -1)
	sendf(ch, "You sense there is little more you can learn about %s here.\n\r", anatomy_table[anatomy].name);
      else if (result)
	sendf(ch,"You have gained knowledge about %s!\n\r", anatomy_table[anatomy].name);


      result = number_range(ch->pcdata->anatomy[anatomy] / 3, get_skill(ch,gsn_anatomy) / 5 + ch->pcdata->anatomy[anatomy]);
      return (UMIN(result, 110));

    }//end success check
  return 0;
}




/*Unholy Gift*/
/*exists only in fight.c*/
bool unholy_gift(CHAR_DATA* ch, CHAR_DATA* victim)
{
  //This is the ability only given to  a single head vamp.
  //It allows for conversion of race to _undead_ when killed.
  int chance_mod = 30;
  int skill;

//EZ
  if ( (ch==NULL) || (victim == NULL) )
    return FALSE;


//This only works if the character is not undead. and is an PC
  if (IS_NPC(victim) || IS_UNDEAD(victim))
    return FALSE;
//Also only works if the charcter is not mob and vampire.
  if (IS_NPC(ch) || ch->class != class_lookup("vampire"))
    return FALSE;

//The attacker must have the skill
  if( (skill = get_skill(ch, gsn_unholy_gift)) == 0)
    return FALSE;


//If al those are ok, then we make a roll.
  if (number_percent() > chance_mod)
    return FALSE;

//Else we succeeded.  So we begin spitting out messages.
  act_new("The last thing you feel is $n's fangs in your neck.", ch, NULL, victim, TO_VICT, POS_DEAD);
  act("Aura of evil fills the room as $n buries $s fangs in $N's neck.", ch, NULL, victim, TO_NOTVICT);
  act("You bless $N with your dark gift and bury your fangs in $S throat.", ch, NULL, victim, TO_CHAR);


//Now we simply slay him.
  raw_kill(ch, victim);
  act_new( "You have been `1EMBRACED!!``", victim,NULL,NULL,TO_CHAR,POS_DEAD);
  /* cast unlife on him */
  spell_unholy_gift(gsn_unholy_gift, ch->level, ch, victim, 0);
  return TRUE;
}//end unholygift.


/*MULT_HIT_WOLF*/
/*exists only in fight.c*/
void multi_hit_wolf(CHAR_DATA* ch, CHAR_DATA* victim)
{
//Modifiers:
  const int haste_mod = 25;
  const int slow_mod = 20;
  const int poison_mod = 20;
  const int bite_mod = 30;

//variables.
  int i =0;
  bool haste = FALSE;
  bool slow = FALSE;

//holders for attack skills.
  int skill_2 = 0;
  int skill_3 = 0;
  int skill_4 = 0;

//Poison Afffect.
  AFFECT_DATA af;

//EZ cases.
  if ((ch == NULL) || (victim == NULL) )
    return;
  if(!is_affected(ch, gsn_wolf_form))
    return;


//First few checks for haste/slow:
  if (IS_AFFECTED(ch, AFF_HASTE))
    haste = TRUE;
//day is equivalent to being slow.
  if (IS_AFFECTED(ch, AFF_SLOW)
      || vamp_day_check(ch) )
    slow = TRUE;
  if(haste && slow)
    haste = slow = FALSE;

//and skills.
  skill_2 = (get_skill(ch, gsn_second_attack)*3)/4;
  skill_4 = (get_skill(ch, gsn_third_attack)*3)/4;
  skill_3 = (skill_2 + skill_4)/2;


//now we slap on modifiers
  if (haste){skill_2 = 100;skill_3+=haste_mod;skill_4+=haste_mod;}
  if (slow){skill_2-=slow_mod;skill_3-=slow_mod;skill_4=0;}

//Executes the whole round of attaks for wolf_form.
//chance of attacks is based on 1/2/3 attack skill.
//the whole thing is run twice.
  for(i = 0; i < 2; i++)
    {
//First attack always succeeds.
      one_hit(ch, victim, attack_lookup("claw") + TYPE_HIT, FALSE);
      if ( ch->fighting != victim )
	return;
//second attack
      if (number_percent() < skill_2){
	one_hit( ch, victim, attack_lookup("charge") + TYPE_HIT, FALSE );
	if ( ch->fighting != victim )
	  return;
      }

//third
      if (number_percent() < skill_3){
//we do a roll here for poison bite
	if (number_percent() < bite_mod)
	  {
//we roll if we actuly bite him.
	    if( (number_percent() < poison_mod) && (!is_affected(victim, gsn_poison)) )
	      {
		AFFECT_DATA* baf;
//we apply poison
		act("$n's jaws dig deep into your flesh, the bite setting your blood afire with pain.",ch,NULL,victim,TO_VICT);
		act("$n's jaws dig deep into $N's flesh and $E screams in pain.",ch,NULL,victim,TO_NOTVICT);
		act("You tear a big chunk of $N's flesh out with your slavering teeth.",ch,NULL,victim,TO_CHAR);

		//we cast a little spell.
		af.where     = TO_AFFECTS;
		af.type      = gsn_poison;
		af.level     = ch->level;
		af.duration  = ch->level/5;
		af.location  = APPLY_STR;
		af.modifier  = -2;
		af.bitvector = AFF_POISON;
		baf = affect_to_char( victim, &af );
		if (!IS_NPC(ch))
		  string_to_affect(baf, ch->name);
		act_new( "You feel very sick.", victim,NULL,NULL,TO_CHAR,POS_DEAD );
		act("$n looks very ill.",victim,NULL,NULL,TO_ROOM);
	      }//end if poison
	    else
	      {
//we didnt poison him.
		//we didnt poison him.
		act("$n's jaws dig deep into your flesh, the bite flashing with seering pain.",ch,NULL,victim,TO_VICT);
		act("$n's jaws dig deep into $N's flesh and $E winces in pain.",ch,NULL,victim,TO_NOTVICT);
		act("You tear a big chunk of $N's flesh out with your slavering teeth.",ch,NULL,victim,TO_CHAR);
	      }//end else
	  }//end poiso roll
//end we make the attack.
	one_hit(ch, victim, attack_lookup("bite")+ TYPE_HIT, FALSE);
	if ( ch->fighting != victim )
	  return;
      }//end if third.

//fourth (only if not slow ie: not slow, in home terrain and at night.)
      if (number_percent() < skill_4 && !slow){
	one_hit(ch, victim, attack_lookup("slash")  + TYPE_HIT, FALSE);
	if ( ch->fighting != victim )
	  return;
      }

//end of run we do it again.
    }//end for
}//end multi_hit_wolf;

/*exists only in fight.c*/
void multi_hit_werebeast(CHAR_DATA* ch, CHAR_DATA* victim){
  AFFECT_DATA* paf;

//Modifiers:
  const int haste_mod = 25;
  const int slow_mod = 25;

//variables.
  int i = 0, max = 2;
  bool haste = FALSE;
  bool slow = FALSE;

//holders for attack skills.
  int skill_2 = 0;
  int skill_3 = 0;

//EZ cases.
  if ((ch == NULL) || (victim == NULL) )
    return;

//First few checks for haste/slow:
  if (IS_AFFECTED(ch, AFF_HASTE))
    haste = TRUE;
  if (IS_AFFECTED(ch, AFF_SLOW) )
    slow = TRUE;
  if(haste && slow)
    haste = slow = FALSE;

//and skills.
  skill_2 = 2 * get_skill(ch, gsn_second_attack) / 3;
  skill_3 = get_skill(ch, gsn_third_attack) / 2;



//now we slap on modifiers
  if (haste){skill_2 = 100; skill_3 += haste_mod;}
  if (slow){skill_2 -= slow_mod; skill_3 -= 0;}

//Executes the whole round of attaks for wereform
//chance of attacks is based on 1/2/3 attack skill.
//the whole thing is run twice.
  if ( (paf = affect_find(ch->affected, gsn_wlock)) != NULL && paf->modifier > 0)
    max = 1;
  else
    max = 2;

  for(i = 0; i < max; i++){
//First attack always succeeds.
    one_hit(ch, victim, ch->dam_type + TYPE_HIT, FALSE);
    if ( ch->fighting != victim )
      return;
//mob spec attacks
    if (IS_NPC(ch)){
      CHAR_DATA* vch, *vch_next;
      int number;
      if (IS_SET(ch->off_flags,OFF_AREA_ATTACK)){
	for (vch = ch->in_room->people; vch != NULL; vch = vch_next){
	  vch_next = vch->next_in_room;
	  if ((vch != victim && vch->fighting == ch))
	    one_hit(ch, vch, ch->dam_type + TYPE_HIT,FALSE);
	}
      }
      if (ch->wait > 0)
	return;
      number = number_range(0,8);
      switch(number) {
      case (0) :
        if (IS_SET(ch->off_flags,OFF_BASH))
	  do_bash(ch,""); break;
      case (1) :
        if (IS_SET(ch->off_flags,OFF_BERSERK) && !IS_AFFECTED(ch,AFF_BERSERK))
	  do_berserk(ch,""); break;
      case (2) :
        if (IS_SET(ch->off_flags,OFF_DISARM) || (get_weapon_sn(ch,FALSE) != gsn_hand_to_hand && IS_SET(ch->act,ACT_WARRIOR)))
	  do_disarm(ch,""); break;
      case (3) :
        if (IS_SET(ch->off_flags,OFF_KICK))
	  do_kick(ch,""); break;
      case (4) :
        if (IS_SET(ch->off_flags,OFF_KICK_DIRT))
	  do_dirt(ch,""); break;
      case (5) :
	break;
      case (6) :
	if (IS_SET(ch->off_flags,OFF_TRIP))
	  do_trip(ch,""); break;
      case (7) :
	if (IS_SET(ch->act,ACT_WARRIOR))
	  do_shield_disarm(ch,""); break;
      case (8) :
	break;
      }
    }
//second attack
    if (number_percent() < skill_2){
      one_hit( ch, victim, ch->dam_type + TYPE_HIT, FALSE );
    }
    if ( ch->fighting != victim )
      return;
//third
    if (number_percent() < skill_3){
      one_hit(ch, victim, ch->dam_type + TYPE_HIT, FALSE);
    }//end if third.
    if ( ch->fighting != victim )
      return;
  }
}


/* holy hands evade */
bool holy_hands_evade(CHAR_DATA* ch, CHAR_DATA* victim, int dt)
{
  //ability of the cleric to sptop a blow with his holy hands.
  int base_chance = 30 + (number_percent())/10;
  int lvl_mod = 1;

  base_chance+= (victim->level - ch->level) *lvl_mod;

//now we make a quick roll
     if (number_percent() < base_chance)
       {
  //we evaded!
	 act("$n stretches his hands, palms out and the blow stops short.", victim, NULL, NULL, TO_ROOM);
	 act("Your faith holding fast, you stop the blow with $g's power.", victim, NULL, NULL, TO_CHAR);
	 victim->mana = UMAX(0, victim->mana - 1);
	 return TRUE;
       }
     else
       return FALSE;
}

/*wolf evade*/
/*used only in fight.c*/
bool wolf_evade(CHAR_DATA* ch, CHAR_DATA* victim, int dt)
{
//This is much like spell_blur except more mundane and works
//in places that vamp wolf can sneak.
     int base_chance = 15 + (number_percent())/5;
     int haste_mod = 15;
     int dex_mod = 2;
     int lvl_mod = 1;

     bool haste = FALSE;
     bool slow = FALSE;

//EZ cases
     if( (ch == NULL || victim == NULL))
       return FALSE;
     if (ch == victim)
       return FALSE;
     if (!is_affected(victim, gsn_wolf_form))
       return FALSE;
     if (!vamp_can_sneak(victim, victim->in_room))
       return FALSE;

//now we add things up.

//First few checks for haste/slow:
     if (IS_AFFECTED(ch, AFF_HASTE))
       haste = TRUE;
//day is equivalent to being slow.
     if (IS_AFFECTED(ch, AFF_SLOW) || vamp_day_check(ch))
       slow = TRUE;
     if(haste && slow)
       haste = slow = FALSE;

     if (haste) base_chance+=haste_mod;
     if (slow)base_chance-=haste_mod;

     base_chance+= (get_curr_stat(victim, STAT_DEX) - get_curr_stat(ch, STAT_DEX)) * dex_mod;
     base_chance+= (victim->level - ch->level) *lvl_mod;

//now we make a quick roll
     if (number_percent() < base_chance)
       {
  //we evaded!
	 act("Blurring with speed, $N disappears in undergrowth and evades the blow.", ch, NULL, victim, TO_NOTVICT);
	 act("$N disappears in undergrowth momentarly and your blow lands short.", ch, NULL, victim, TO_CHAR);
	 act("Confident in your enviroment, you easly evade the blow.", ch, NULL, victim, TO_VICT);
	 return TRUE;
       }
 //we didn't.
     return FALSE;
}//end wolf_evade


/*check_dodge_wolf*/
/*linked only to fight.c*/
bool check_dodge_wolf( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn)
{
//this is the dodge for vamp wolves.
//they get huge bonus to dodge
//in order to boost the defenses to match that of a human form or more.

//EZ
  if (ch == NULL || victim == NULL)
    return FALSE;
  if (!is_affected(victim, gsn_wolf_form))
      return FALSE;

 //and this little eazy check.

  if (!check_dodge( ch, victim, vir_obj, dt, h_roll, level, fVirt, sn))
    {
      if (!check_dodge( ch, victim, vir_obj, dt, h_roll, level, fVirt, sn))
	return FALSE;
    }

    return TRUE;
}//end check_dodge_wolf


void seasons_dam(CHAR_DATA *ch, int season)
{
    CHAR_DATA *vch, *vch_next;
    int dam = ch->level + 25 + dice(5,9);
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;
        if ( !is_area_safe(ch,vch))
        {
            s_yell(ch,vch,FALSE);
            if (saves_spell(ch->level,vch,DAM_OTHER,song_table[gsn_seasons].spell_type))
		dam /= 2;
	    switch (season)
	    {
		case (0):
                    damage( ch,vch,dam,skill_lookup("deluge water"), DAM_DROWNING,TRUE);
		    break;
		case (1):
                    damage( ch,vch,dam,skill_lookup("fireball"), DAM_FIRE,TRUE);
		    break;
		case (2):
                    damage( ch,vch,dam,skill_lookup("call lightning"), DAM_LIGHTNING,TRUE);
		    break;
		case (3):
                    damage( ch,vch,dam,skill_lookup("iceball"), DAM_COLD,TRUE);
		    break;
	    }
        }
    }
}

/* Control the fights going on.           *
 * Called periodically by update_handler. */
void violence_update( void )
{
    CHAR_DATA *ch, *ch_next, *victim;
    AFFECT_DATA af, *paf;
    OBJ_DATA *wield, *second;
    char buf[MSL];
    OBJ_DATA *obj, *obj_next;
    bool room_trig = FALSE;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next	= ch->next;

	if (IS_STONED(ch))
	  continue;

	if ( !IS_NPC(ch) && ch->position > POS_SLEEPING )
	  {
	    int wie_p, wie_s;
            wield = get_eq_char(ch,WEAR_WIELD);
            second = get_eq_char(ch,WEAR_SECONDARY);
	    wie_p = get_obj_weight(wield);
	    wie_s = get_obj_weight(second);

	    if (ch->fighting && do_combatdraw(ch, wield, second)){
	      wield = get_eq_char(ch,WEAR_WIELD);
	      second = get_eq_char(ch,WEAR_SECONDARY);
	    }
	    if (second != NULL && (get_eq_char(ch,WEAR_WIELD) == NULL))
            {
                unequip_char(ch, second);
                equip_char( ch, second, WEAR_WIELD);
                act( "$n switches $p to their good hand.",ch,second,NULL,TO_ROOM);
                act( "You switch your weapon to your good hand.",ch,second,NULL,TO_CHAR);
	    }
	    if (wield != NULL
		&& second != NULL
		&& (ch->race != grn_minotaur || wield->value[0] != WEAPON_AXE || second->value[0] != WEAPON_AXE)
		&& !IS_AFFECTED2(ch,AFF_RAGE)
		&& wie_s > 3 * wie_p / 4
		&& wie_s > 100
		&& get_skill(ch, gsn_bladework) < 2)
	    {
		unequip_char(ch,second);
		act("Your secondary weapon becomes too heavy for you to wield.",ch,second,NULL,TO_CHAR);
		act("$p becomes too heavy for $n to wield.",ch,second,NULL,TO_ROOM);
	    }
        }
	if (!IS_NPC(ch) && IS_AFFECTED2(ch,AFF_SHADOWFORM) && get_charmed_by(ch))
	{
	    send_to_char("Your followers have given you away.\n\r", ch);
            affect_strip ( ch, gsn_shadowform                       );
            REMOVE_BIT       ( ch->affected2_by, AFF_SHADOWFORM     );
            act_new("Your body regains its substance, and you materialize into existence.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
            act("$n's body regains its substance, and $e materializes into existence.",ch,NULL,NULL,TO_ROOM);
	}
        if (!IS_NPC(ch) && IS_AFFECTED(ch,AFF_TREEFORM) && get_charmed_by(ch))
        {
            send_to_char("Your followers have given you away.\n\r", ch);
            affect_strip ( ch, gsn_treeform                       );
            REMOVE_BIT       ( ch->affected_by, AFF_TREEFORM     );
            act_new("Your body returns to its normal form.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
            act("$n's body returns to its normal form.",ch,NULL,NULL,TO_ROOM);
	}
	if (IS_AFFECTED2(ch, AFF_CATALEPSY) && ch->position != POS_SLEEPING)
	    do_revive(ch, "");
        if (IS_AFFECTED2(ch, AFF_RAGE) && ch->fighting == NULL && IS_AWAKE(ch) && !IS_AFFECTED(ch,AFF_CALM))
        {
	    CHAR_DATA *vch, *vch_next;
            bool found = FALSE;
            bool fight = FALSE;
            int count  = 0;
            victim     = NULL;
            for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
            {
                vch_next = vch->next_in_room;
                if ( is_area_safe_quiet(ch,vch) || !can_see( ch, vch ) || ch == vch)
		    continue;
                if (!IS_NPC(vch))
                {
                    found = TRUE;
                    if ( number_range( 0, count ) == 0 )
                    {
                        victim = vch;
                        fight = TRUE;
                    }
                    count++;
                }
            }
            if (found && !fight)
            {
                REMOVE_BIT(ch->affected2_by, AFF_RAGE);
                affect_strip(ch,gsn_rage);
                act("After finding no one to fight, you snap out of your madness.",ch,NULL,NULL,TO_CHAR);
                act("$n gasps for air as $e snaps out of $s insanity.",ch,NULL,NULL,TO_ROOM);
                ch->hit -= (ch->level * ch->perm_stat[STAT_CON]) /3;
                if (IS_IMMORTAL(ch))
                    ch->hit = UMAX(1,ch->hit);
                if (ch->hit < -2)
                {
                    int shock_mod = (25 * (0 - ch->hit)) / ch->max_hit;
                    if (number_percent( ) < con_app[UMAX(1,ch->perm_stat[STAT_CON] - shock_mod)].shock)
                    {
                        act_new("Your body suddenly awakens to the wounds you've sustained and you lose consciousness.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
                        act("$n suddenly clutches $s wounds and slumps to the ground, unconscious.",ch,NULL,NULL,TO_ROOM);
                        ch->hit = -2;
                        ch->position = POS_STUNNED;
                        act("$n is stunned, but will probably recover.",ch,NULL,NULL,TO_ROOM);
                        act_new("You are stunned, but will probably recover.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
                    }
                    else
                    {
                        act_new("Your body could not sustain the injuries you've suffered.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
                        act("$n suddenly clutches $s wounds and slumps to the ground.",ch,NULL,NULL,TO_ROOM);
                        act( "$n is DEAD!!", ch, NULL, NULL, TO_ROOM );
                        act_new( "You have been KILLED!!", ch,NULL,NULL,TO_CHAR,POS_DEAD);
			raw_kill( ch, ch );
                    }
                }
                continue;
            }
            if ( victim != NULL )
                do_murder( ch, victim->name );
        }
	if ( ch == NULL || ch->in_room == NULL )
	  continue;
	update_violence(ch);
	if ( ch == NULL || ch->in_room == NULL )
	  continue;

	if ( is_song_affected(ch,gsn_dance_song))
	{
       	    if (!IS_AWAKE(ch) && !IS_AFFECTED(ch,AFF_SLEEP))
            	do_wake(ch,"");
    	    do_visible(ch,NULL);
	    send_to_char("You can't stop dancing!\n\r",ch);
	    act("$n dances wildly in front of you!",ch,NULL,NULL,TO_ROOM);
	    ch->move--;
	}
	if ( is_song_affected(ch,gsn_seasons))
	{
	    AFFECT_DATA *paf = affect_find(ch->affected2,gsn_seasons);
	    if (paf != NULL)
	    {
		switch (paf->modifier)
		{
		    case (0):
			sprintf(buf,"Rains of spring in flooding tides.");
			song_to_room(ch,buf,FALSE,FALSE,FALSE);
			paf->modifier++;
			seasons_dam(ch,0);
			break;
		    case (1):
			sprintf(buf,"Summer's heat is scorching high.");
			song_to_room(ch,buf,FALSE,FALSE,FALSE);
			paf->modifier++;
			seasons_dam(ch,1);
			break;
		    case (2):
			sprintf(buf,"Autumn storms lights up the skies.");
			song_to_room(ch,buf,FALSE,FALSE,FALSE);
			paf->modifier++;
			seasons_dam(ch,2);
			break;
		    case (3):
			sprintf(buf,"Glaciers in winter made of ice.");
			song_to_room(ch,buf,FALSE,FALSE,FALSE);
			paf->modifier++;
			seasons_dam(ch,3);
			break;
		    default:
			song_affect_remove(ch,paf);
			break;
		}
	    }
	}
	victim = NULL;
	if ( ( victim = ch->fighting ) != NULL ){
	  if ( IS_AWAKE(ch)
	       && ch->in_room == victim->in_room
	       && !is_affected(ch,gsn_ecstacy))
	    {
	      multi_hit( ch, victim, TYPE_UNDEFINED );
// POSTCOMBAT CHECK //
	      if (  POSTCOMBAT_CHECK(ch, victim, TYPE_UNDEFINED ) == -1)
		bug("Error at: PostcombatCheck, returned fail\n\r", 0);
	    }
	  else
	    stop_fighting( ch, FALSE );
	  if (ch == NULL)
	    continue;
	  if ( ( victim = ch->fighting ) == NULL ){
	    continue;
	  }
	  /* OLD MPROG */
	  if (IS_NPC(ch) && ch->pIndexData->progtypes & FIGHT_PROG)
	    mprog_fight_trigger( ch, victim );
	  if (ch == NULL || !ch->fighting)
	    continue;
	  if (IS_NPC(ch) && ch->pIndexData->progtypes & HITPRCNT_PROG)
	    mprog_hitprcnt_trigger( ch, victim );
	  if (ch == NULL || !ch->fighting)
	    continue;

	  /* NEW PROGS */
	  if (IS_NPC(ch)){
	    if (HAS_TRIGGER_MOB(ch, TRIG_FIGHT))
	      p_percent_trigger( ch, NULL, NULL, victim, NULL, NULL, TRIG_FIGHT);
	    if (ch == NULL || !ch->fighting)
	      continue;
	    if ( HAS_TRIGGER_MOB(ch, TRIG_HPCNT) )
	      p_hprct_trigger( ch, victim );
	  }
	  if (ch == NULL || !ch->fighting)
	    continue;
	  for ( obj = ch->carrying; obj; obj = obj_next )
	    {
	      obj_next = obj->next_content;

	      if ( obj->wear_loc != WEAR_NONE && HAS_TRIGGER_OBJ( obj, TRIG_FIGHT ) )
		p_percent_trigger( NULL, obj, NULL, victim, NULL, ch, TRIG_FIGHT );
	      if (ch == NULL || !ch->fighting)
		continue;
	    }
	  if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_FIGHT ) && room_trig == FALSE )
	    {
	      room_trig = TRUE;
	      p_percent_trigger( NULL, NULL, ch->in_room, victim, NULL, NULL, TRIG_FIGHT );
	    }
/* amke sure we are fighting after trigger */
	  if (!ch->fighting)
	    continue;

	  if ( is_affected(ch,gsn_refrain) && number_percent() > 50)
	    {
	      AFFECT_DATA *saf;
	      saf = affect_find(ch->affected,gsn_refrain);
	      if (refrain_sing(ch,saf->modifier))
		affect_strip(ch,gsn_refrain);
	    }
	  check_assist(ch,victim);
	  if ( IS_AFFECTED(ch,AFF_SLEEP) && ch->position > POS_SLEEPING )
            {
	      REMOVE_BIT(ch->affected_by, AFF_SLEEP);
	      affect_strip(ch,gsn_sleep);
	      affect_strip(ch,gsn_hypnosis);
	      affect_strip(ch,gsn_coffin);
	      if ( is_affected(ch, gsn_camp)){
		send_to_char( "You feel drained from deprivation of sleep.\n\r", ch);
		if (!is_affected(ch, gsn_drained)){
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
	      }
	      affect_strip(ch,gsn_entomb);
	      song_strip(ch,gsn_lullaby);
	    }
	  if ( IS_AFFECTED(victim,AFF_SLEEP) && victim->position > POS_SLEEPING )
            {
	      REMOVE_BIT(victim->affected_by, AFF_SLEEP);
	      affect_strip(victim,gsn_sleep);
	      affect_strip(victim,gsn_hypnosis);
	      affect_strip(victim,gsn_coffin);
	      affect_strip(victim,gsn_entomb);
	      song_strip(victim,gsn_lullaby);
	      if ( is_affected(victim, gsn_camp)){
		send_to_char( "You feel drained from deprivation of sleep.\n\r", ch);
		if (!is_affected(victim, gsn_drained)){
		  af.where		= TO_AFFECTS;
		  af.type             = gsn_drained;
		  af.level            = victim->level;
		  af.duration         = 10;
		  af.location         = APPLY_NONE;
		  af.modifier         = 0;
		  af.bitvector        = 0;
		  affect_to_char(victim,&af);
		  affect_strip(victim,gsn_camp);
		}
	      }
            }
	}
	if (ch == NULL)
	  continue;
/* after violence cleanup */
	if ( is_affected(ch,gsn_rake) )
	  {
	    send_to_char("You gain control of your weapon.\n\r",ch);
	    affect_strip(ch,gsn_rake);
	  }
	if (!IS_NPC(ch)){
	  affect_strip(ch,gsn_feign);
	}
	if (!IS_NPC(ch)
	    && (paf = affect_find(ch->affected,gsn_cyclone)) != NULL
	    && --paf->duration < 1){
	  affect_strip(ch, gsn_cyclone);
	}
	if (!IS_NPC(ch)
	    && (paf = affect_find(ch->affected,gsn_maelstrom)) != NULL
	    && --paf->duration < 1){
	  affect_strip(ch, gsn_maelstrom);
	}
	if (!IS_NPC(ch)
	    && (paf = affect_find(ch->affected,gsn_onslaught)) != NULL
	    && --paf->duration < 1){
	  affect_strip(ch, gsn_onslaught);
	}
    }
}

void check_assist(CHAR_DATA *ch,CHAR_DATA *victim)
{
    CHAR_DATA *rch, *rch_next;
    char buf[MSL];
    if (ch->in_room == NULL)
	return;
    for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
    {
	rch_next = rch->next_in_room;
	if (IS_AWAKE(rch) && rch->fighting == NULL && !is_affected(rch,gsn_ecstacy)){
	  /* quick check for ASSIST_PLAYER */
	  if (!IS_NPC(ch) && IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_PLAYERS)
	      && rch->level + 6 > victim->level && !IS_AFFECTED2(ch, AFF_HOLD) && can_see(rch,victim)){
	    act("$n screams and attacks!",rch,NULL,victim,TO_ALL);
	    multi_hit(rch,victim,TYPE_UNDEFINED);
	    continue;
	  }
	  /* PCs next */
	  else if (!IS_NPC(ch)
	      || IS_AFFECTED(ch,AFF_CHARM)
	      || IS_AFFECTED(rch,AFF_CHARM)){
	    if ( ((!IS_NPC(rch) && IS_SET(rch->act,PLR_AUTOASSIST))
		  || IS_AFFECTED(rch,AFF_CHARM))
		 && is_same_group(ch,rch)
		 && !is_safe(rch, victim) )
	      {
		if ( IS_NPC(rch)
		     && !IS_NPC(victim)
		     && rch->master != NULL
		     && rch->master != victim
		     && !IS_NPC(rch->master)
		     && !is_pk(rch->master, victim) )
		  continue;
		/* JUSTICE GUARD CAPTAIN/HOUND ASSIST CHECK  */
		if (IS_NPC(rch)
		    && rch->in_room
		    && (rch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD
			|| rch->pIndexData->vnum == MOB_VNUM_HOUND)
		    && !IS_LEADER( ch )
		    && !IS_AREA(rch->in_room->area, AREA_LAWFUL)
		    && !IS_WANTED(victim))
		  continue;

		if (IS_NPC(rch) && IS_SET(rch->act, ACT_NONCOMBAT))
		  continue;

		/*Viri: sheath check for ps's didn't want this to be run
		  as often as the if's above
		*/
		if (!IS_NPC(rch) && is_sheathed(rch))
		  continue;
		a_yell(rch,victim);
		if (!IS_NPC(rch))
		  {
		    sprintf(buf, "Help! I am being attacked by %s!",PERS(rch,victim));
		    j_yell(victim, buf );
		  }
		if (IS_NPC(rch)
		    && (rch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD
			|| rch->pIndexData->vnum == MOB_VNUM_HOUND)){
		  rch->fighting = victim;
		  spec_special_guard(rch);
		}
		else
		  multi_hit (rch,victim,TYPE_UNDEFINED);
	      }
	    continue;
	  }
	  /* now check the NPC cases */
	  if (IS_NPC(ch) && !IS_AFFECTED(ch,AFF_CHARM))
	    if ( ((IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALL))
		  || (IS_NPC(rch) && rch->group && rch->group == ch->group)
		  || (IS_NPC(rch) && rch->race == ch->race
		      && IS_SET(rch->off_flags,ASSIST_RACE))
		  || (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALIGN)
		      && ((IS_GOOD(rch) && IS_GOOD(ch))
			  || (IS_EVIL(rch) && IS_EVIL(ch)) || (IS_NEUTRAL(rch) && IS_NEUTRAL(ch))))
		  || (is_same_group(ch,rch) && !is_safe(rch, victim) )
		  || (rch->pIndexData == ch->pIndexData && IS_SET(rch->off_flags,ASSIST_VNUM)))
		 && !(IS_AFFECTED(rch,AFF_CHARM) && rch->master != ch)
		 && !IS_SET(rch->act, ACT_IS_HEALER) )
	      {
		CHAR_DATA *vch, *target = NULL;
		int number = 0;
		if (number_bits(1) == 0)
		  continue;
		for (vch = ch->in_room->people; vch; vch = vch->next_in_room)
		  if (can_see(rch,vch) && is_same_group(vch,victim) && number_range(0,number) == 0)
		    {
		      target = vch;
		      number++;
		    }
		if (target != NULL && !IS_AFFECTED2(rch, AFF_HOLD) && can_see(rch,target))
		  {
		    act("$n screams and attacks!",rch,NULL,target,TO_ALL);
		    multi_hit(rch,target,TYPE_UNDEFINED);
		  }
	      }
	}
    }
}

void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int chance, chance1 = 0, change = victim->hit;
    AFFECT_DATA *paf, *paf_next, *form;
    CHAR_DATA *vch, *vch_next;
    OBJ_DATA *hold, *obj;

    int melee_loc = number_range(1, 3);	//after which attack we do meleeactions
    int secart_loc = number_range(1, 3);//after which attack we do sec-arts
    int shadow_loc = number_range(1, 3);//after which attack we do sec-arts
//bool
    bool fHaste = (IS_AFFECTED(ch, AFF_HASTE) && !IS_AFFECTED(ch, AFF_SLOW));
    bool fSlow = (!IS_AFFECTED(ch, AFF_HASTE) && IS_AFFECTED(ch, AFF_SLOW));
    bool fPugil = FALSE;
    bool fFeign = !IS_NPC(ch) && is_affected(ch, gsn_feign);
    bool fWlock = !IS_NPC(ch) && ((paf = affect_find(ch->affected, gsn_wlock)) != NULL && paf->modifier > 0);
    bool fWerebeastMultiHit = FALSE;

    //check for blademaster bladestorm haste
    if (fHaste && ch->class == gcn_blademaster && is_affected(ch, gsn_bladestorm) && get_eq_char( ch, WEAR_SECONDARY) == NULL)
      fHaste = FALSE;

    if ( victim != ch && victim->in_room == ch->in_room)
      {
	if ( ch->position > POS_STUNNED )
	  {
	    if ( ch->fighting == NULL )
	      set_fighting( ch, victim);
	    if (ch->timer <= 4)
	      ch->position = POS_FIGHTING;
	  }
/*
	if ( victim->position > POS_STUNNED )
	  {
	    if ( victim->fighting == NULL )
	      set_fighting( victim, ch);
	    if (victim->timer <= 4)
	      victim->position = POS_FIGHTING;
	  }
*/
      }
    if (ch->desc == NULL)
	ch->wait = UMAX(0,ch->wait - PULSE_VIOLENCE);

// PRECOMBAT CHECK //
    if (  PRECOMBAT_CHECK(ch, victim, dt) == -1)
      bug("Error at: PrecombatCheck, returned fail\n\r", 0);


    if (ch->class == class_lookup("monk"))
        for ( paf = ch->affected; paf != NULL; paf = paf_next )
	{
	    paf_next = paf->next;
            if ( paf->type == gsn_throw )
		if (paf->duration-- == 0)
		{
		    act_new(skill_table[gsn_throw].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
		    affect_remove( ch, paf);
		}
	}
    if ( is_affected(ch,gsn_mortally_wounded) )
	return;
    if ( is_affected(ch,gsn_ecstacy) )
	return;
    if (ch->position < POS_RESTING)
	return;
    /* Make sure they are fighting at this point */
    if (!ch->fighting)
      return;
    else if (ch->fighting != victim)
      return;
    if ( dt == gsn_backstab || dt == gsn_cleave
	 || dt == gsn_assassinate || dt == gsn_charge || dt == gsn_ambush)
      return;
    if (!IS_NPC(victim))
    	victim = check_guard(victim,ch);

    /* ranged weapons fire */
    fire_ranged_weapons( ch, victim );
    if (ch->fighting != victim )
      return;

    /* check for future use of pugil here */
    if ( (obj = get_eq_char(ch, WEAR_WIELD)) != NULL
	 && obj->item_type == ITEM_WEAPON
	 && obj->value[0] == WEAPON_STAFF
	 && get_eq_char(ch, WEAR_SECONDARY) == NULL
	 && get_eq_char(ch, WEAR_SHIELD) == NULL
	 && get_eq_char(ch, WEAR_HOLD) == NULL
	 && number_percent() < get_skill(ch, gsn_pugil)){
      check_improve(ch, gsn_pugil, TRUE, 1);
//      send_to_char("PUGIL\n\r", ch);
      fPugil = TRUE;
    }
    /* auto-deathblow check */
    if (is_affected(ch, gsn_cyclone) && is_affected(ch, gsn_maelstrom)){
      deathblow( ch, victim, DB_FULL, FALSE );
      affect_strip(ch, gsn_cyclone );
      affect_strip(ch, gsn_maelstrom );
    }
    if (ch->fighting == NULL)
      return;
    /* form execution */
    if ( (form = affect_find(ch->affected, gsn_forms)) != NULL
	 && form->modifier++ % 2 != 0
	 && form->has_string){
      forms(ch, form->string, form);
      if (ch->fighting != victim)
	return;
    }

    /* transorm check */
    if (ch != victim && ch->race == grn_werebeast && ch->level >= 15 && ch->hit < ch->max_hit / 2){
      int transchance = 20 - (20 * ch->hit / ch->max_hit);
      if (number_percent() < transchance){
	if (!is_affected(ch,gsn_transform))
	  beast_transform(ch);
      }
    }
    /* check if we are going to run werebeast multihit */
    for (paf = ch->affected; paf; paf = paf->next){
      if (paf->type == gsn_weretiger
	  || paf->type == gsn_werewolf
	  || paf->type == gsn_werebear
	  || paf->type == gsn_werefalcon){
	fWerebeastMultiHit = TRUE;
	break;
      }
    }

    if (ch->race == grn_werebeast && fWerebeastMultiHit){
      multi_hit_werebeast(ch, victim);
      multi_hit_extra( ch, victim, dt, change );
    }
    else if (ch->race == grn_vampire && is_affected(ch, gsn_wolf_form))
      multi_hit_wolf(ch, victim);
    else if (IS_NPC(ch)){
      mob_hit(ch,victim,dt);
      multi_hit_extra( ch, victim, dt, change );
      return;
    }
//otherwise we proced with all this junk that will need to be cleaned up.
    else
      {
	if (ch->fighting != NULL && ch->fighting->fighting != NULL && ch->fighting->fighting == ch
	    && ch->class == class_lookup("monk") && ch->hit > 0 && ch->hit < ch->max_hit / 10
	    && (hold = get_eq_char(ch, WEAR_HOLD)) != NULL && hold->pIndexData->vnum == OBJ_VNUM_PRAYER_BEADS)
	  {
	    int anatomy = 0;
	    if (!IS_NPC(victim))
	      anatomy = anatomy_lookup(pc_race_table[victim->race].anatomy);
	    chance = 25 + ch->pcdata->anatomy[anatomy] / 10;
	    chance += get_skill(ch,gsn_anatomy) / 20;
	    chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX))/2;
	    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK);
	    if (IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG) )
	      chance = 0;
	    if ( number_percent( ) < chance && !IS_IMMORTAL(victim) && !IS_SET(victim->act, ACT_UNDEAD))
	      {
		AFFECT_DATA af;
		af.type             = gsn_strangle;
		af.level            = ch->level;
		af.duration         = number_range(1, 2);
		af.location         = APPLY_NONE;
		af.where            = TO_AFFECTS;
		af.modifier         = 0;
		af.bitvector        = AFF_SLEEP;
		affect_to_char(victim,&af);
		act("$n places $s prayer beads around your neck and tightens.",ch,NULL,victim,TO_VICT);
		act("You place your prayer beads around $N's neck and tighten.",ch,NULL,victim,TO_CHAR);
		act("$n places $s prayer beads around $N's neck and tightens.",ch,NULL,victim,TO_NOTVICT);
		stop_fighting( ch, FALSE );
		stop_fighting( victim, FALSE );
		do_sleep(victim,"");
		if ( number_percent( ) > chance )
		  {
		    act("The string breaks and the prayer beads fall apart.",ch,NULL,NULL,TO_ALL);
		    obj_from_char( hold );
		    extract_obj( hold );
		  }
		return;
	      }
	  }

	if (ch->class == class_lookup("monk") && get_eq_char( ch, WEAR_WIELD ) == NULL )
	  {
/* monk first attack */
	    if (!fFeign)
	      one_hit( ch, victim, number_range(1053,1055), FALSE );
	    if ( ch->fighting != victim )
	      return;
	    if (melee_loc == 1 && check_melee_actions(ch))
	      return;
	    if (secart_loc == 1 && check_secart_actions(ch))
	      return;
	    if (shadow_loc == 1 && shadow_strike(ch, victim)){
	      if (ch->fighting != victim)
		return;
	    }

	    if (is_affected(ch,gsn_monkey) && monk_good(ch, WEAR_ABOUT) && monk_good(ch, WEAR_WAIST) )
	      {
		chance = 100;
		if (!IS_AFFECTED(ch,AFF_HASTE) && IS_AFFECTED(ch,AFF_SLOW))
		  chance = 50;
		if (number_percent( ) < chance){
		  one_hit( ch, victim, number_range(1053,1055), FALSE );
		  check_improve(ch, gsn_monkey, TRUE, 1);
		  if ( ch->fighting != victim )
		    return;
		}
	      }
	  }
	else
	  {
	    /* first attack */
	    if (ch->class == gcn_blademaster && getBmWearLoc(ch, TRUE) == WEAR_SECONDARY)
	      one_hit( ch, victim, dt, TRUE );
	    else
	      one_hit( ch, victim, dt, FALSE );

	    if ( ch->fighting != victim )
	      return;
	    if (melee_loc == 1 && check_melee_actions(ch))
	      return;
	    if (secart_loc == 1 && check_secart_actions(ch))
	      return;
	    if (shadow_loc == 1 && shadow_strike(ch, victim)){
	      if (ch->fighting != victim)
		return;
	    }
	    //Additional attack due to haste
	    if (fHaste && ch->fighting){
	      if (ch->class == gcn_blademaster && getBmWearLoc(ch, TRUE) == WEAR_SECONDARY)
		one_hit( ch, victim, dt, TRUE );
	      else
		one_hit( ch, victim, dt, FALSE );
	      if ( ch->fighting != victim )
		return;
	    }
	    /* first dual attack */
	    chance = get_skill(ch, gsn_dual_wield);
	    if (!fFeign
		&& !fWlock
		&& (fPugil || get_eq_char(ch,WEAR_SECONDARY))
		&& number_percent( ) < chance){
	      one_hit( ch,victim,dt,TRUE);
	      check_improve(ch,gsn_dual_wield,TRUE,1);
	      if (ch->fighting != victim)
		return;
	    }
	  }
	if (IS_AFFECTED2(ch, AFF_RAGE)){
	  for (vch = ch->in_room->people; vch != NULL; vch = vch_next){
	    vch_next = vch->next_in_room;
	    if ((vch != victim && vch->fighting == ch)){
	      if (IS_NPC(ch))
		one_hit(ch,vch,dt,FALSE);
	      if (!IS_NPC(ch) && number_percent( ) < 2 * get_skill(ch, gsn_swing) /3){
		one_hit(ch,vch,dt,FALSE);
		check_improve(ch,gsn_swing, TRUE, 1);
		if (!IS_NPC(ch) && number_percent( ) < get_skill(ch, gsn_swing)/2
		    && (fPugil || get_eq_char(ch, WEAR_SECONDARY)) && number_percent( ) < get_skill(ch, gsn_dual_wield)){
		  check_improve(ch, gsn_swing, TRUE, 1);
		  one_hit(ch,vch,dt,TRUE);
		}
	      }
	    }
	  }
	}
	else if ( ((chance = get_skill(ch, gsn_cusinart))
		   || is_affected(ch, gsn_windmill))
		  && has_twohanded(ch) != NULL){
	  /* you hit a mate if they are near by */
	  if ( chance < 2 && (vch = get_group_room(ch, FALSE)) != NULL ){
	    if (ch->fighting == NULL ||  ch->fighting != vch
		|| vch->fighting == NULL ||  vch->fighting != ch ){
	      char buf[MIL];
	      sprintf(buf, "Die %s you inconsiderate fool!", PERS2(ch));
	      a_yell(ch, vch);
	      j_yell(vch, buf);
	    }
	    one_hit(ch,vch,dt,FALSE);
	  }
	  for (vch = ch->in_room->people; vch != NULL; vch = vch_next){
	    vch_next = vch->next_in_room;
	    if ((vch == victim || vch->fighting != ch))
	      continue;
	    if (IS_NPC(ch))
	      one_hit(ch,vch,dt,FALSE);
	    if (!IS_NPC(ch) && number_percent( ) < 2 * get_skill(ch, gsn_swing) /3){
	      one_hit(ch,vch,dt,FALSE);
	      check_improve(ch,gsn_windmill, TRUE, 1);
	      check_improve(ch,gsn_swing, TRUE, 1);
	    }
	  }
	  if (chance > 1){
	    for (vch = ch->in_room->people; vch != NULL; vch = vch_next){
	      vch_next = vch->next_in_room;
	      if ((vch == victim || vch->fighting != ch))
		continue;
	      if (IS_NPC(ch))
		one_hit(ch,vch,dt,FALSE);
	      if (!IS_NPC(ch) && number_percent( ) < 2 * chance / 3){
		one_hit(ch,vch,dt,FALSE);
		check_improve(ch,gsn_swing, TRUE, 1);
		check_improve(ch,gsn_cusinart, TRUE, 1);
	      }
	    }
	  }
	}
//BEGIN MONK MULTI HIT
	if (ch->class == class_lookup("monk"))
	  {
	    //check for items wornd
	    int items = 0;
	    OBJ_DATA *hold;
	    if (get_eq_char( ch, WEAR_WIELD ) != NULL )
	      items = 3;
	    if (get_eq_char( ch, WEAR_SHIELD ) != NULL )
	      items++;
	    hold = get_eq_char( ch, WEAR_HOLD);
	    if ( hold != NULL && hold->pIndexData->vnum != OBJ_VNUM_PRAYER_BEADS)
	      items++;

	    //Second attack
	    chance = get_skill(ch, gsn_martial_arts_b);
	    if (!fWlock && items < 3 && number_percent( ) < chance && monk_good(ch, WEAR_ARMS))
	      {
		one_hit( ch, victim, number_range(1053,1055), FALSE );
		check_improve(ch,gsn_martial_arts_b,TRUE,1);
		if ( ch->fighting != victim )
		  return;
	      }
	    if (melee_loc == 2 && check_melee_actions(ch))
	      return;
	    if (secart_loc == 2 && check_secart_actions(ch))
	      return;
	    if (shadow_loc == 2 && shadow_strike(ch, victim)){
	      if (ch->fighting != victim)
		return;
	    }
	    //third attack
	    chance = get_skill(ch, gsn_kickboxing_b);
	    if (items < 2 && number_percent( ) < chance && monk_good(ch, WEAR_LEGS))
	      {
		one_hit( ch, victim, number_range(1059,1061), FALSE );
		check_improve(ch,gsn_kickboxing_b,TRUE,1);
		if ( ch->fighting != victim )
		  return;

		//zaf 11-14-99 (extra atack hasted.)
		if (IS_AFFECTED(ch,AFF_HASTE) && !IS_AFFECTED(ch,AFF_SLOW))
		  {
		    one_hit( ch, victim, number_range(1059,1061), FALSE );
		    check_improve(ch,gsn_kickboxing_b,TRUE,0);
		    if ( ch->fighting != victim )
		      return;
		  }

	      }
	    if (melee_loc == 3 && check_melee_actions(ch))
	      return;
	    if (secart_loc == 3 && check_secart_actions(ch))
	      return;
	    if (shadow_loc == 3 && shadow_strike(ch, victim)){
	      if (ch->fighting != victim)
		return;
	    }
//ADVANCED ATTACKS
	    //FIRST
	    chance = get_skill(ch, gsn_martial_arts_a)/2;
	    if (!fWlock && items < 2 && is_empowered_quiet(ch,3) && number_percent( ) < chance && monk_good(ch, WEAR_HANDS))
	      {
		one_hit( ch, victim, number_range(1056,1058), FALSE );
		check_improve(ch,gsn_martial_arts_a,TRUE,0);
		if ( ch->fighting != victim )
		  return;

		//zaf 11-14-99 (extra attack)
		if (IS_AFFECTED(ch,AFF_HASTE) && !IS_AFFECTED(ch,AFF_SLOW))
		  {
		    one_hit( ch, victim, number_range(1056,1058), FALSE );
		    check_improve(ch,gsn_martial_arts_a,TRUE,0);
		    if ( ch->fighting != victim )
		      return;
		  }
	      }
	    //SECOND
	    chance = get_skill(ch, gsn_kickboxing_a)/2;
	    if (items < 1 && is_empowered_quiet(ch,0) && number_percent( ) < chance && monk_good(ch, WEAR_FEET))
	      {
		one_hit( ch, victim, number_range(1062,1064), FALSE );
		check_improve(ch,gsn_kickboxing_a,TRUE,0);
		if ( ch->fighting != victim )
		  return;
	      }

	    multi_hit_extra( ch, victim, dt, change );
	    if ( ch->fighting != victim )
	      return;
//MONKEY STANCE ATTACKS
	    if (!is_affected(ch,gsn_monkey) || !monk_good(ch, WEAR_ABOUT) || !monk_good(ch, WEAR_WAIST))
	      return;
	    check_improve(ch, gsn_monkey, TRUE, 1);
	    //FIRST
	    chance = get_skill(ch, gsn_martial_arts_b);
	    if (!IS_AFFECTED(ch,AFF_HASTE) && IS_AFFECTED(ch,AFF_SLOW))
	      chance /= 2;
	    if (items < 3 && number_percent( ) < chance && monk_good(ch, WEAR_ARMS))
	      {
		one_hit( ch, victim, number_range(1053,1055), FALSE );
		check_improve(ch,gsn_martial_arts_b,TRUE,0);
		if ( ch->fighting != victim )
		  return;
	      }

	    //SECOND
	    chance = get_skill(ch, gsn_kickboxing_b);
	    if (!IS_AFFECTED(ch,AFF_HASTE) && IS_AFFECTED(ch,AFF_SLOW))
	      chance /= 2;
	    if (items < 3 && number_percent( ) < chance && monk_good(ch, WEAR_LEGS))
	      {
		one_hit( ch, victim, number_range(1059,1061), FALSE );
		check_improve(ch,gsn_kickboxing_b,TRUE,0);
		if ( ch->fighting != victim )
		  return;
	      }

	    //THIRD
	    chance = get_skill(ch, gsn_martial_arts_a)/2;
	    if (!IS_AFFECTED(ch,AFF_HASTE) && IS_AFFECTED(ch,AFF_SLOW))
	      chance /= 2;
	    if (items < 1 && number_percent( ) < chance && monk_good(ch, WEAR_HANDS))
	      {
		one_hit( ch, victim, number_range(1056,1058), FALSE );
		check_improve(ch,gsn_martial_arts_a,TRUE,0);
		if ( ch->fighting != victim )
		  return;
	      }

	    //FOURTH
	    chance = get_skill(ch, gsn_kickboxing_a)/2;
	    if (!IS_AFFECTED(ch,AFF_HASTE) && IS_AFFECTED(ch,AFF_SLOW))
	      chance /= 2;
	    if (items < 2 && number_percent( ) < chance && monk_good(ch, WEAR_FEET))
	      {
		one_hit( ch, victim, number_range(1062,1064), FALSE );
		check_improve(ch,gsn_kickboxing_a,TRUE,0);
		if ( ch->fighting != victim )
		  return;
	      }
    ////////////////
    //RETURNS HERE//
    ////////////////
	    return;
	  }//END MONK ATTACKS
//NORMAL ATTACKS
/* Second Attack */
	if (fSlow)
	  chance = 0;
	//this stance requires 2 weapons, but the attack is lost even without 2 weapons
	else if (ch->class == gcn_blademaster && is_affected(ch, gsn_iron_curtain))
	  chance = 0;
	else if (fHaste){
	  if (ch->class == gcn_blademaster && monk_good(ch, WEAR_FEET))
	    chance = 100;
	  else
	    chance = get_skill(ch, gsn_second_attack);
	}
	else{
	  if (ch->class == gcn_blademaster && monk_good(ch, WEAR_FEET))
	    chance = get_skill(ch, gsn_footwork);
	  else
	    chance = get_skill(ch, gsn_second_attack)/2;
	}
	//fury for ferals
	if (ch->race == grn_feral)
	  chance += ch->level;
	if ( has_twohanded(ch) != NULL
	     && number_percent() < get_skill( ch, gsn_adv_handling) ){
	  check_improve(ch, gsn_adv_handling, TRUE, 0);
	  chance += 15;
	}

	/* Second Primary Attack */
	if (number_percent( ) < chance )
	  {
	    if (ch->class == gcn_blademaster && getBmWearLoc(ch, FALSE) == WEAR_SECONDARY)
	      one_hit( ch, victim, dt, TRUE );
	    else
	      one_hit( ch, victim, dt, FALSE );
	    check_improve(ch, gsn_footwork, TRUE, 0);
	    check_improve(ch,gsn_second_attack,TRUE,0);
	    if ( ch->fighting != victim )
	      return;
	  }

	if (melee_loc == 2 && check_melee_actions(ch))
	  return;
	if (secart_loc == 2 && check_secart_actions(ch))
	  return;
	if (shadow_loc == 2 && shadow_strike(ch, victim)){
	  if (ch->fighting != victim)
	    return;
	}
	/* Secondary Dual Attack */
	if (fSlow )
	  chance = 0;
	else if (fHaste)
	  chance = 4 * get_skill(ch, gsn_second_attack)/ 5;
	else
	  chance = get_skill(ch, gsn_second_attack)/2;

	//fury for ferals
	if (chance && ch->race == grn_feral)
	  chance += ch->level;

	if ( !fWlock
	     && (fPugil || get_eq_char(ch,WEAR_SECONDARY))
	     && number_percent( ) < chance
	     && ((!IS_NPC(ch)
		  && get_skill(ch,gsn_dual_wield) > number_range( 0, 100 ))
		 || IS_NPC(ch)) )
	  {
	    one_hit( ch,victim,dt,TRUE);
	    check_improve(ch,gsn_dual_wield,TRUE,1);
	    if (ch->fighting != victim)
	      return;
	  }


/* Third Attack */
	/* Third attack Primary */
	if (fSlow)
	  chance = 0;
	else if (fHaste){
	  if (ch->class == gcn_blademaster)
	    chance  = 8 * get_skill(ch, gsn_bladework) / 10;
	  else
	    chance  = get_skill(ch, gsn_third_attack) / 2;
	}
	else{
	  if (ch->class == gcn_blademaster)
	    chance  = 7 * get_skill(ch, gsn_bladework) / 10;
	  else
	    chance = get_skill(ch, gsn_third_attack) / 4;
	}

	//Psi Time Compression
	if (chance == 0 && !fSlow
	    && ch->class == gcn_psi
	    && is_affected(ch, gsn_time_comp))
	  chance = fHaste ? 50  : 25;
	//fury for ferals
	if (ch->race == grn_feral)
	  chance += ch->level;
	if ( has_twohanded(ch) != NULL
	     && number_percent() < get_skill( ch, gsn_adv_handling) ){
	  check_improve(ch, gsn_adv_handling, TRUE, 1);
	  chance += 10;
	}

	if (melee_loc == 3 && check_melee_actions(ch))
	  return;
	if (secart_loc == 3 && check_secart_actions(ch))
	  return;
	if (shadow_loc == 3 && shadow_strike(ch, victim)){
	  if (ch->fighting != victim)
	    return;
	}
	if ( number_percent( ) < chance )
	  {
	    if (ch->class == gcn_blademaster && getBmWearLoc(ch, TRUE) == WEAR_SECONDARY)
	      one_hit( ch, victim, dt, TRUE );
	    else
	      one_hit( ch, victim, dt, FALSE );
	    check_improve(ch, gsn_bladework, TRUE, 0);
	    check_improve(ch,gsn_third_attack,TRUE,1);
	    if ( ch->fighting != victim )
	      return;
	  }
	/* Third attack Secondary */
	if (fSlow || fWlock)
	  chance = 0;
	else if (fHaste)
	  chance = 3 * get_skill(ch, gsn_third_attack) /5;
	else
	  chance = get_skill(ch, gsn_third_attack) / 5;

	if ((fPugil
	     || get_eq_char(ch,WEAR_SECONDARY)) && number_percent( ) < chance
	    && ( ( !IS_NPC(ch) && get_skill(ch,gsn_dual_wield) > number_range( 0, 100 ))
		 || IS_NPC(ch)) )
	  {
	    one_hit( ch,victim,dt,TRUE);
	    check_improve(ch,gsn_dual_wield,TRUE, 1);
	    if (ch->fighting != victim)
	      return;
	  }

/* Fourth Attack */
	/* Fourth attack Primary */
	if (fSlow || fPugil)
	  chance = 0;
	else if (fHaste){
	  if (ch->class == gcn_blademaster && monk_good(ch, WEAR_LEGS))//same chance as not hasted
	    chance  = 5 * get_skill(ch, gsn_battlestance) / 10;
	  else
	    chance = get_skill(ch, gsn_fourth_attack) / 4;
	}
	else{
	  if (ch->class == gcn_blademaster && monk_good(ch, WEAR_LEGS))//same chance as hsted
	    chance  = 5 * get_skill(ch, gsn_battlestance) / 10;
	  else
	    chance = get_skill(ch, gsn_fourth_attack) / 6;
	}

	//Psi Time Compression
	if (chance == 0 && !fSlow
	    && ch->class == gcn_psi
	    && is_affected(ch, gsn_time_comp))
	  chance = fHaste ? 30  : 15;

	if ( has_twohanded(ch) != NULL
	     && number_percent() < get_skill( ch, gsn_adv_handling) ){
	  check_improve(ch, gsn_adv_handling, TRUE, 1);
	  chance += 10;
	}
	if ( number_percent( ) < chance
	     || (ch->class == gcn_vampire
		 && !vamp_day_check(ch)))
	  {
	    if (ch->class == gcn_blademaster && getBmWearLoc(ch, FALSE) == WEAR_SECONDARY)
	      one_hit( ch, victim, dt, TRUE );
	    else
	      one_hit( ch, victim, dt, FALSE );
	    check_improve(ch, gsn_battlestance, TRUE, 0);
	    check_improve(ch,gsn_fourth_attack,TRUE,0);
	    if ( ch->fighting != victim )
	      return;
	  }
	/* Fourth Attack Secondary */
	if (fSlow || fPugil || fWlock)
	  chance = 0;
	else if (fHaste)
	  chance = get_skill(ch, gsn_fourth_attack) / 6;
	else
	  chance = get_skill(ch, gsn_fourth_attack) / 8;

	if ( (get_eq_char(ch,WEAR_SECONDARY)) && number_percent( ) < chance
	     && ( ( !IS_NPC(ch) && get_skill(ch,gsn_dual_wield) > number_range( 0, 100 ))
		  || IS_NPC(ch))
	     )
	  {
	    one_hit( ch,victim,dt,TRUE);
	    check_improve(ch,gsn_dual_wield,TRUE,1);
	    if (ch->fighting != victim)
	      return;
	  }

	/* fifth attack for blademasters */
	if (fSlow || fPugil)
	  chance = 0;
	else if (fHaste){
	  chance  = 3 * get_skill(ch, gsn_avatar_of_steel) / 10;
	}
	else{
	  chance  = 3 * get_skill(ch, gsn_avatar_of_steel) / 10;
	}
	if ( has_twohanded(ch) != NULL
	     && number_percent() < get_skill( ch, gsn_adv_handling) ){
	  check_improve(ch, gsn_adv_handling, TRUE, 1);
	  chance += 10;
	}
	if ( number_percent( ) < chance  ){
	  bool fPrim = number_percent() < 50;
	  if (ch->class == gcn_blademaster && getBmWearLoc(ch, fPrim) == WEAR_SECONDARY)
	    one_hit( ch, victim, dt, TRUE );
	  else
	    one_hit( ch, victim, dt, FALSE );
	  check_improve(ch, gsn_avatar_of_steel, TRUE, 0);
	  if ( ch->fighting != victim )
	    return;
	}

/* END OF REGUARL 5 ATTCKS */
	if (ch->class == gcn_bard && IS_AREA(ch->in_room->area, AREA_CITY)){
	  if ( (chance = get_skill(ch,gsn_brawl)) > 1 ){
	    chance /= 2;
	    if ( number_percent( ) < chance ){
	      one_hit( ch, victim, dt, FALSE);
	      check_improve(ch,gsn_brawl,TRUE,1);
	    }
	    if ( ch->fighting != victim )
	      return;
	  }
	}

	multi_hit_extra( ch, victim, dt, change );
	if ( ch->fighting != victim )
	  return;
	if (ch->class == class_lookup("berserker") && !IS_AFFECTED2(ch,AFF_RAGE) && !IS_NPC(ch) && ch->level >= 6)
	  {
	    chance1 = 4;
	    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
	      chance1 = 0;
	    if (IS_AFFECTED(ch, AFF_CALM))
	      chance1 = 0;
	    if (100 * ch->hit / ch->max_hit > 75 - ch->level)
	      chance1 = 0;
	    if (chance1 != 0)
	      {
		if (IS_AFFECTED(ch, AFF_BERSERK))
		  chance1 += 1;
		if (ch->race == race_lookup("dwarf"))
		  chance1 += 1;
		if (IS_AFFECTED(ch,AFF_HASTE))
		  chance1 += 1;
		chance1 += ((75 - ch->level) - (100 * ch->hit / ch->max_hit))/5;
		chance1 -= (get_skill(ch,gsn_rage)/25);
		chance1 = UMAX(1, chance1);
	      }
	    if (number_percent( ) < chance1)
	      {
		AFFECT_DATA af;
		act("You feel your veins course with power as the madness engulfs you!",ch,NULL,NULL,TO_CHAR);
		act("$n's eyes suddenly burn like hot coals as $e is gripped by insanity!",ch,NULL,NULL,TO_ROOM);
		af.where            = TO_AFFECTS2;
		af.type             = gsn_rage;
		af.level            = ch->level;
		af.duration         = UMAX(0, 4 - number_fuzzy(ch->level / 11));
		af.location         = APPLY_STR;
		af.modifier         = ch->size * 2;
		af.bitvector        = AFF_RAGE;
		affect_to_char(ch,&af);
		af.location         = APPLY_HITROLL;
		af.modifier         = str_app[get_curr_stat(ch,STAT_STR)].tohit + ch->level/4;
		affect_to_char(ch,&af);
		af.location         = APPLY_DAMROLL;
		af.modifier         = str_app[get_curr_stat(ch,STAT_STR)].todam + ch->level/4;
		affect_to_char(ch,&af);
		af.location         = APPLY_HIT;
		af.modifier         = (ch->level * ch->perm_stat[STAT_CON])/3;
	    affect_to_char(ch,&af);
            ch->hit += (ch->level * ch->perm_stat[STAT_CON]) / 3;
            check_improve(ch,gsn_rage,TRUE,1);
	      }
	  }
      }//end if WOLF

}

void multi_hit_extra( CHAR_DATA *ch, CHAR_DATA *victim, int dt, int change )
{
  int chance = ch->level, iWear, cycle;
  OBJ_DATA *wield, *claw, *obj;
  wield = get_eq_char(ch,WEAR_WIELD);
  claw = get_eq_char(ch, WEAR_HANDS);

  if ( ch->fighting != victim )
    return;
  if (is_affected(victim, gsn_mortally_wounded))
    return;

  if (claw && claw->pIndexData->vnum == OBJ_VNUM_CLAWS){
    damage(ch,victim,number_range(0, (claw->level + ch->level)/4),1005,DAM_SLASH,TRUE);
    if (ch->fighting != victim)
      return;
  }
  for ( iWear = 0; iWear < MAX_WEAR; iWear++ ){
    obj = get_eq_char( ch, iWear );
    //cast combat spells.
    if( obj != NULL
	&& (obj->eldritched || obj->pIndexData->spell != NULL)
	&& ch->fighting != NULL)
      do_obj_spell( obj, ch, victim );
    if ( ch->fighting != victim )
      return;
  }
/* check prim/sec */
  for (cycle = 0; cycle < 2; cycle++){
    if (cycle == 0)
      wield = getBmWep(ch, TRUE);
    else
      wield = getBmWep(ch, FALSE);

    if (wield == NULL)
      continue;
    else
      chance = wield->level;

    if (number_percent( ) < chance ){
      int dam;
      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_POISON)){
	int level;
	AFFECT_DATA *poison, af, *paf;
	if ((poison = affect_find(wield->affected,gsn_poison)) == NULL)
	  level = UMAX(wield->level,1);
	else
	  level = poison->level;
	if (!saves_spell(level / 2,victim,DAM_POISON,skill_table[gsn_poison].spell_type)) {
	  bool fNew = is_affected(victim, gsn_poison);
	  act("You feel poison coursing through your veins.", victim,wield,NULL,TO_CHAR);
	  act("$n is poisoned by the venom on $p.", victim,wield,NULL,TO_ROOM);
	  af.where     = TO_AFFECTS;
	  af.type      = gsn_poison;
	  af.level     = level * 3/4;
	  af.duration  = level / 10;
	  af.location  = APPLY_STR;
	  af.modifier  = fNew ? -1 : 0;
	  af.bitvector = AFF_POISON;
	  affect_join( victim, &af );
	  paf = affect_find( victim->affected, gsn_poison );
	  if (!IS_NPC(ch))
	    string_to_affect(paf, ch->name);
	  /* Set bit preventing damage till first tick. */
	  if (ch->class != class_lookup("shaman") && fNew)
	    SET_BIT(paf->bitvector, AFF_FLAG);
	}
	if (ch->fighting == NULL)
	  return;
      }
      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC)){
	dam = number_range(1, wield->level / 3 + 1);
	act("$p draws life from $n.",victim,wield,NULL,TO_ROOM);
	act("You feel $p drawing your life away.", victim,wield,NULL,TO_CHAR);
	damage(ch,victim,dam,33 + TYPE_NOBLOCKHIT,DAM_NEGATIVE,TRUE);
	ch->hit += dam/2;
	if (ch->fighting == NULL)
	  return;
      }
      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FLAMING)){
	dam = number_range(1,wield->level / 3 + 5);
	act("$n's flesh is seared by the flaming $p.",victim,wield,NULL,TO_ROOM);
	act("$p sears your flesh with flames.",victim,wield,NULL,TO_CHAR);
	fire_effect(victim,wield->level,dam,TARGET_CHAR);
	if (ch->fighting == NULL)
	  return;
	damage(ch,victim,dam,29 + TYPE_NOBLOCKHIT,DAM_FIRE,TRUE);
	if (ch->fighting == NULL)
	  return;
      }
      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FROST)){
	dam = number_range(5,wield->level / 3 + 15);
	act("The cold touch of $p surrounds $n with ice.",victim,wield,NULL,TO_ROOM);
	act("The cold touch of $p surrounds you with ice.",victim,wield,NULL,TO_CHAR);
	cold_effect(victim,wield->level,dam,TARGET_CHAR);
	if (ch->fighting == NULL)
	  return;
	damage(ch,victim,dam,30 + TYPE_NOBLOCKHIT,DAM_COLD,TRUE);
	if (ch->fighting == NULL)
	  return;
      }
      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHOCKING)){
	dam = number_range(10,wield->level/3 + 10);
	act("$n is shocked by the lightning from $p.",victim,wield,NULL,TO_ROOM);
	act("You are shocked by the lightning from $p.",victim,wield,NULL,TO_CHAR);
	shock_effect(victim,wield->level/2,dam,TARGET_CHAR);
	if (ch->fighting == NULL)
	  return;
	damage(ch,victim,dam,28 + TYPE_NOBLOCKHIT,DAM_LIGHTNING,TRUE);
      }
      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_PARALYZE)){
	dam = number_range(10,wield->level/3 + 10);
	act("$p tears into your nerves.",victim,wield,NULL,TO_CHAR);
	act("$p tears into $n's nerves.",victim,wield,NULL,TO_ROOM);
	paralyze_effect(victim,wield->level,dam,TARGET_CHAR);
	if (ch->fighting == NULL)
	  return;
      }
      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VORPAL)){
	dam = number_range(1,wield->level/5 + 1);
	act("$p burns $n with corrosive acid.",victim,wield,NULL,TO_ROOM);
	act("You are burned by the corrosive acid on $p.",victim,wield,NULL,TO_CHAR);
	acid_effect(victim,wield->level,dam,TARGET_CHAR);
	if (ch->fighting == NULL)
	  return;
	damage(ch,victim,dam,31 + TYPE_NOBLOCKHIT,DAM_POISON,TRUE);
	if (ch->fighting == NULL)
	  return;
      }
      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHARP)){
	dam = number_range(15,wield->level/2 + ch->level / 2);
	act("$p digs into $n's body.",victim,wield,NULL,TO_ROOM);
	act("You can feel $p dig into your body.",victim,wield,NULL,TO_CHAR);
	damage(ch,victim,dam,18 + TYPE_NOBLOCKHIT,DAM_SLASH,TRUE);
	if (ch->fighting == NULL)
	  return;
      }
      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_WOUNDS)){
	wound_effect( ch, victim, ch->level, 0);
	if (ch->fighting == NULL)
	  return;
      }
      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_DEADLY)
	  && !IS_UNDEAD(ch->fighting)){
	dam = number_range(15,wield->level/2 + 25);
	if (!saves_spell(3 * wield->level / 4 + dam / 15, victim,
			 DAM_SLASH, SPELL_AFFLICTIVE)){
	  AFFECT_DATA af, *paf;
	  act("A spray of blood shoots forth from $n's wound!", victim, NULL, NULL, TO_ROOM);
	  act("A spray of blood shoots forth from your wound!", victim, NULL, NULL, TO_CHAR);
	  if ( (paf = affect_find(victim->affected, gen_bleed)) == NULL){
	    af.type = gen_bleed;
	    af.level = ch->level;
	    af.duration = number_range(3, 6);
	    af.where = TO_NONE;
	    af.bitvector = 0;
	    af.location = APPLY_NONE;
	    /* modifier controls duration of damage (10/tick)*/
	    af.modifier = number_range(5, 15);
	    paf = affect_to_char(victim, &af);
	    string_to_affect(paf, ch->name);
	  }
	  else{
	    /* modifier controls duration of damage (10/tick)*/
	    paf->modifier = number_range(5, 15);
	    string_to_affect(paf, ch->name);
	  }
	}
      }
    }
    if ( ch->fighting == NULL || ch->fighting != victim )
      return;
    if (wield && !str_cmp(wield->material,"practice") && number_percent( ) < 10){
      if (ch->max_hit != 0 && ch->hit*100/ch->max_hit <= 20)
	act( "$p whispers 'Run away!'", ch, wield, NULL, TO_CHAR);
      else if (ch->max_hit != 0 && ch->hit*100/ch->max_hit <= 60)
	act( "$p whispers 'You can do it!'", ch, wield, NULL, TO_CHAR);
      else
	act( "$p whispers 'You're doing great!'", ch, wield, NULL, TO_CHAR);
    }
  }//END PRIM/SEC LOOP
  if ( change != victim->hit && IS_AFFECTED2(victim, AFF_ICE_SHIELD) ){
    damage(victim,ch,number_range(victim->level/2,3*victim->level/2),skill_lookup("ice shield"),DAM_COLD,TRUE);
    if ( ch->fighting == NULL || ch->fighting != victim )
      return;
  }
  if ( change != victim->hit && IS_AFFECTED2(victim, AFF_FIRE_SHIELD) ){
    damage(victim,ch,number_range(victim->level/2,3*victim->level/2),skill_lookup("fire shield"),DAM_FIRE,TRUE);
    if ( ch->fighting == NULL || ch->fighting != victim )
      return;
  }
  if ( is_affected(victim, skill_lookup("divine retribution")) && !IS_GOOD(ch)){
    int dam2;
    dam2 = number_range(victim->level, 3*victim->level/2);
    if (IS_NEUTRAL(ch))
      dam2 = 2*dam2/3;
    damage(victim,ch,dam2,skill_lookup("divine retribution"),DAM_HOLY,TRUE);
    if ( ch->fighting == NULL || ch->fighting != victim )
      return;
  }
  if ( change != victim->hit && is_affected(victim, skill_lookup("shield of thorns"))){
    int dam2;
    dam2 = number_range(2*victim->level/3,3*victim->level/2);
    damage(victim,ch,dam2,skill_lookup("shield of thorns"),DAM_PIERCE,TRUE);
    if ( ch->fighting == NULL || ch->fighting != victim )
      return;
  }
  if ( change != victim->hit && is_affected(victim, gsn_steel_wall )){
    int dam2;
    dam2 = number_range( victim->level / 2, 3 * victim->level / 2);
    damage(victim, ch, dam2, gsn_steel_wall ,DAM_SLASH, TRUE);
    if ( ch->fighting == NULL || ch->fighting != victim )
      return;
  }

  if (ch->race == race_lookup("slith") && number_percent() < 15 + ch->level
      && get_curr_stat(ch, STAT_STR) > 12){
    if (number_percent() < 75)
      damage(ch,victim,number_range(ch->level, 3 * ch->level / 2 - (30 - ch->perm_stat[STAT_STR])),1071,DAM_PIERCE,TRUE);
    else
      {
	act("You strike with your tail at $N's legs.", ch, NULL, victim, TO_CHAR);
	act("$n strikes with $s tail at $N's legs.", ch, NULL, victim, TO_NOTVICT);
	act("$n strikes with $s tail at your legs.", ch, NULL, victim, TO_VICT);
	trip(ch, victim, 50);
      }
    if ( ch->fighting == NULL || ch->fighting != victim )
      return;
  }
  if ( ch->fighting != victim )
    return;
}

void mob_hit (CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
    int chance, number, change = victim->hit;
    CHAR_DATA *vch, *vch_next;
    char buf[MSL];
    if (IS_SET(ch->act,ACT_THIEF) && ch->fighting == NULL)
    {
	buf[0]='\0';
	strcat(buf,str_dup(victim->name));
	do_backstab(ch,buf);
    }
    one_hit(ch,victim,dt,FALSE);
    if (ch->fighting != victim)
	return;
    if (ch->fighting != victim || dt == gsn_backstab || dt == gsn_cleave
    || dt == gsn_assassinate || dt == gsn_charge || dt == gsn_ambush)
	return;

    if (IS_SET(ch->off_flags,OFF_AREA_ATTACK)){
	for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
	{
	    vch_next = vch->next_in_room;
	    if ((vch != victim && vch->fighting == ch))
		one_hit(ch,vch,dt,FALSE);
	}
    }
    if (ch->fighting != victim)
	return;

    if ( (IS_AFFECTED(ch,AFF_HASTE) || IS_SET(ch->off_flags,OFF_FAST)) && !IS_AFFECTED(ch,AFF_SLOW))
      one_hit(ch,victim,dt,FALSE);
    if (ch->fighting != victim)
      return;

    if (IS_SET(ch->off_flags,OFF_EXTRA_ATTACK))
      one_hit(ch,victim,dt,FALSE);
    if (ch->fighting != victim)
      return;

    chance = get_skill(ch,gsn_second_attack)/2;
    if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST) && !IS_AFFECTED(ch,AFF_HASTE))
      chance /= 2;
    if (number_percent() < chance)
      {
        one_hit(ch,victim,dt,FALSE);
	if (ch->fighting != victim)
	  return;
      }
    chance = get_skill(ch,gsn_third_attack)/4;
    if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST) && !IS_AFFECTED(ch,AFF_HASTE))
      chance = 0;
    if (number_percent() < chance)
      {
        one_hit(ch,victim,dt,FALSE);
	if (ch->fighting != victim)
	  return;
      }
    if ( change != victim->hit && IS_AFFECTED2(victim, AFF_ICE_SHIELD) )
      damage(victim,ch,number_range(victim->level/2,3*victim->level/2),skill_lookup("ice shield"),DAM_COLD,TRUE);
    if (ch->fighting != victim)
      return;
    if ( chance != victim->hit && IS_AFFECTED2(victim, AFF_FIRE_SHIELD) )
      damage(victim,ch,number_range(victim->level/2,3*victim->level/2),skill_lookup("fire shield"),DAM_FIRE,TRUE);
    if (ch->fighting != victim)
      return;
    if ( is_affected(victim, skill_lookup("divine retribution")) && !IS_GOOD(ch))
      {
	int dam2;
	dam2 = number_range(victim->level,3*victim->level/2);
	if (IS_NEUTRAL(ch))
	  dam2 = 2*dam2/3;
        damage(victim,ch,dam2,skill_lookup("divine retribution"),DAM_HOLY,TRUE);
      }
    if (ch->fighting != victim)
      return;
    if ( is_affected(victim, skill_lookup("shield of thorns")))
      {
        int dam2;
        dam2 = number_range(2*victim->level/3,3*victim->level/2);
        damage(victim,ch,dam2,skill_lookup("shield of thorns"),DAM_PIERCE,TRUE);
      }
    if (ch->fighting != victim)
      return;

    if (ch->wait > 0)
      return;
    number = number_range(0,8);
    switch(number)
    {
    case (0) :
        if (IS_SET(ch->off_flags,OFF_BASH))
            do_bash(ch,""); break;
    case (1) :
        if (IS_SET(ch->off_flags,OFF_BERSERK) && !IS_AFFECTED(ch,AFF_BERSERK))
            do_berserk(ch,""); break;
    case (2) :
        if (IS_SET(ch->off_flags,OFF_DISARM) || (get_weapon_sn(ch,FALSE) != gsn_hand_to_hand && IS_SET(ch->act,ACT_WARRIOR)))
            do_disarm(ch,""); break;
    case (3) :
        if (IS_SET(ch->off_flags,OFF_KICK))
            do_kick(ch,""); break;
    case (4) :
        if (IS_SET(ch->off_flags,OFF_KICK_DIRT))
            do_dirt(ch,""); break;
    case (5) :
	break;
    case (6) :
	if (IS_SET(ch->off_flags,OFF_TRIP))
            do_trip(ch,""); break;
    case (7) :
	if (IS_SET(ch->act,ACT_WARRIOR))
            do_shield_disarm(ch,""); break;
    case (8) :
	break;
    }
}

int get_AC(CHAR_DATA *ch, CHAR_DATA *victim, int dt, int dam_type, bool fVirt)
{
/* LOWER ac results in more MISSES */

//data
  int victim_ac;

//AC PENALTY
  switch(dam_type)
    {
    case(DAM_PIERCE):	victim_ac = GET_AC(victim,AC_PIERCE)/10;	break;
    case(DAM_BASH):	victim_ac = GET_AC(victim,AC_BASH)/10;		break;
    case(DAM_SLASH):	victim_ac = GET_AC(victim,AC_SLASH)/10;	break;
    default:		victim_ac = GET_AC(victim,AC_EXOTIC)/10;	break;
    };

//smaller bonus for large ac.

  if (!IS_NPC(victim) && victim_ac < -25)
    victim_ac = (victim_ac + 25) / 2 - 25;

  if (get_skill(ch, gsn_apierce) > 1)
    victim_ac /= 2;

//BLINDNESS
  if ( !can_see( ch, victim ) && !fVirt)
    {
      if (!IS_NPC(ch)
	  && UMAX(1,number_percent() - (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK))) < get_skill(ch,gsn_blind_fighting))
	{
	  check_improve(ch,gsn_blind_fighting, TRUE, 1);
	  if (!is_affected(ch, gsn_battletrance))
	    victim_ac -= 2;
	}
      else
	{
	  check_improve(ch,gsn_blind_fighting, FALSE, 1);
	  victim_ac -= 5;
	}
    }

if ( !can_see( victim, ch ) )
  {
    if (!IS_NPC(victim) && UMAX(1,number_percent() - (get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK))) < get_skill(victim,gsn_blind_fighting))
      {
	check_improve(victim,gsn_blind_fighting, TRUE, 1);
	if (!is_affected(victim, gsn_battletrance))
	  victim_ac += 3;
      }
    else
      {
	check_improve(victim,gsn_blind_fighting, FALSE, 1);
	victim_ac += 10;
      }
  }

//POSITONS
  if ( victim->position < POS_FIGHTING)
    victim_ac += 4;
  if (victim->position < POS_RESTING)
    victim_ac += 6;

  return victim_ac;
}


/* RETURNS THAC0 of CH against Victim.  use fNpc to switch between PC/NPC Thac0 */
int get_THAC0(CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* obj, int dt, bool secondary, bool fNpc, long act, bool fVirt, int gsn, int level)
{
/* HIGHER thac0 results in more MISSES */

//DATA
    int thac0;
    int thac0_00;
    int thac0_32;
    int sn = -1;
    int skill = 0;

    /* Useless conditional */
    if( sn != -1 )
        sn = -1;

//Get damage text (split into weapon/spell/skill)
//for thac0 calc.

//NPC THAC0
    if (fNpc )
    {
	thac0_00 = 20;
	if (IS_SET(act, ACT_WARRIOR))
	    thac0_32 = -10;
	else if (IS_SET(act, ACT_THIEF))
	    thac0_32 = -4;
	else if (IS_SET(act, ACT_CLERIC))
	    thac0_32 = 2;
	else if (IS_SET(act, ACT_MAGE))
	    thac0_32 = 6;
	else
	  thac0_32 = -4;
    }
    else
//PC THAC0
    {
	thac0_00 = class_table[ch->class].thac0_00;
	thac0_32 = class_table[ch->class].thac0_32;
    }

    sn = (fVirt? gsn : get_weapon_sn(ch,secondary));
    skill = 20 + (fVirt? get_skill(ch, gsn) : get_weapon_skill_obj(ch,obj));

//COMMON THAC0
    thac0  = interpolate( level, thac0_00, thac0_32 );
    thac0 -= (GET_HITROLL(ch) / 2) * UMIN(100, skill) /100;

//Penalty for skills.
    if (dt == gsn_backstab
	|| dt == gsn_dual_backstab
	|| dt == gsn_cleave
	|| dt == gsn_assassinate
	|| dt == gsn_charge
	|| dt == gsn_ambush
	|| dt == gsn_circle
	|| dt == gsn_riposte
	)


//penalty for weapon skill
    thac0 += 5 * (100 - skill) / 100;

    //Penalty for secondary weapon
    if (get_eq_char(ch,WEAR_SECONDARY) != NULL && !fVirt
	&& ( ch->class != class_lookup("warrior")
	     && ch->class != class_lookup("ranger")) )
        thac0 += 10;
   return thac0;
}

void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary)
{
    OBJ_DATA *wield;
    AFFECT_DATA *paf;
    int apierce = 0;
//BOOL
    bool fReverse = FALSE;
    bool fPugil = FALSE;
    int victim_ac, thac0, dam, diceroll, sn = -1, skill, dam_type, success, dam_limit = 30000;
    int result = 0;
    int temp = 0;
    int ac_red = 1600;
    int hroll, droll;

    /* Useless conditional */
    if( result != 0 )
        result = 0;

    if ( is_affected(ch,gsn_mortally_wounded) )
	return;
    if ( is_affected(ch,gsn_ecstacy) )
	return;
    if (victim == ch || ch == NULL || victim == NULL)
	return;
    if ( ch->in_room != victim->in_room )
	return;
    if ( ( paf = affect_find(ch->affected,gsn_rake)) != NULL )
      {
	if (!secondary && paf->modifier == 1)
	{
	    act("Your primary weapon has been pushed aside!",ch,NULL,NULL,TO_CHAR);
	    act("$n's primary weapon has been pushed aside!",ch,NULL,victim,TO_VICT);
	    return;
	}
	if (secondary && paf->modifier == 0)
	{
	    act("Your secondary weapon has been raked aside!",ch,NULL,NULL,TO_CHAR);
	    act("$n's secondary weapon has been raked aside!",ch,NULL,victim,TO_VICT);
	    return;
	}
    }
    else{
      //DRAW WEAPON
      OBJ_DATA* wield, *second;
      wield = get_eq_char(ch,WEAR_WIELD);
      second = get_eq_char(ch,WEAR_SECONDARY);
      do_combatdraw(ch, wield, second);
    }

    if (!secondary)
        wield = get_eq_char( ch, WEAR_WIELD );
     /* check for use of pugil here */
    else if ( (wield = get_eq_char(ch, WEAR_WIELD)) != NULL
	      && wield->item_type == ITEM_WEAPON
	      && wield->value[0] == WEAPON_STAFF
	      && get_eq_char(ch, WEAR_SECONDARY) == NULL
	      && get_eq_char(ch, WEAR_SHIELD) == NULL
	      && get_eq_char(ch, WEAR_HOLD) == NULL){
      check_improve(ch, gsn_pugil, TRUE, 1);
      fPugil = TRUE;
    }
    else
      wield = get_eq_char( ch, WEAR_SECONDARY );


    if (wield != NULL){
      AFFECT_DATA* paf;
      if ( (paf = affect_find(ch->affected, gsn_con_damage)) != NULL
	   && number_percent() < ((get_skill(ch, gsn_con_damage) - 50) * 2)){
	dam_type = attack_table[paf->modifier].damage;
	dt = paf->modifier + TYPE_HIT;
	check_improve(ch, gsn_con_damage, TRUE, 1);
      }
    }
    if ( dt == TYPE_UNDEFINED )
    {
	dt = TYPE_HIT;
	if ( wield != NULL && wield->item_type == ITEM_WEAPON )
	    dt += wield->value[3];
	else
    	    dt += ch->dam_type;
    }

    if (dt < TYPE_HIT)
    {
/* check for control damage here, stored in paf->modifier */
      if (wield != NULL){
	dam_type = attack_table[wield->value[3]].damage;
      }
      else
	dam_type = attack_table[ch->dam_type].damage;
    }
    else{
      /* check for monk magical chii level 2 */
      if ( (paf = affect_find(ch->affected, gsn_chii_bolt)) != NULL
	   && paf->modifier == 2)
	dam_type = attack_table[attack_lookup("magic")].damage;
      else
	dam_type = attack_table[dt - TYPE_HIT].damage;
    }
    if (dam_type == -1)
      dam_type = DAM_BASH;

    if (!secondary || fPugil)
      sn = get_weapon_sn(ch,FALSE);
    else
      sn = get_weapon_sn(ch,TRUE);

    skill = 20 + get_weapon_skill_obj(ch, wield);

//GET thac0 and AC
    thac0 = get_THAC0(ch, victim, wield, dt, secondary, IS_NPC(ch), ch->act,
		      FALSE, dt, ch->level);
    victim_ac = get_AC(ch, victim, dt, dam_type, FALSE);
    /* debug */
/*    sendf(ch, "your thaco: %d, their ac %d", thac0, victim_ac); */

    //sendf(ch, "sn: %d\n\r", sn);
    // PRACTICE BONUS
    if (sn != -1)
      {
	if (wield && !str_cmp(wield->material,"practice"))
	  check_improve(ch,sn,TRUE,3);
	else
	  check_improve(ch,sn,TRUE,1);
	if (wield != NULL &&
	    (IS_WEAPON_STAT(wield, WEAPON_TWO_HANDS) || is_affected(ch, gsn_double_grip)))
	  check_improve(ch,gsn_2hands,TRUE,1);
	if (wield && is_affected(ch, gsn_double_grip))
	  check_improve(ch,gsn_double_grip,TRUE,1);
      }


//ROLL FOR HIT
    while ( ( diceroll = number_bits( 5 ) ) >= 20 );
    if ((diceroll == 0 || ( diceroll != 19 && diceroll < thac0 - victim_ac )) )
      {
	//MISS
	damage( ch, victim, 0, dt, dam_type, TRUE );
	tail_chain( );
	return;
      }

    if ( IS_NPC(ch) && (!ch->pIndexData->new_format || wield == NULL))
      {
	if (!ch->pIndexData->new_format)
	  {
	    dam = number_range( ch->level / 2, ch->level * 3 / 2 );
	    if ( wield != NULL )
	      dam += dam / 2;
	  }
	else
	  dam = dice(ch->damage[DICE_NUMBER],ch->damage[DICE_TYPE]);
      }
    else
      //SUCCESSFULL HIT
      {
	//DAMAGE/DDAMTYPE CALC.
	if ( wield != NULL )
	  {
	    //New Format
	    //PC (skill = weaponskill + 20)
	    if (wield->pIndexData->new_format)
	      {
		//powerstrike
		if (number_percent() < get_skill(ch, gsn_powerstrike) / 2
		    && get_eq_char(ch, WEAR_WIELD) != NULL
		    && get_eq_char(ch, WEAR_SECONDARY) != NULL
		    && monk_good(ch, WEAR_WAIST) ){
		  int chance =  (get_curr_stat(ch, STAT_LUCK) - 14) * 3;
		  check_improve(ch, gsn_powerstrike, TRUE, 0);
		  if (number_percent() < chance)
		    dam = wield->value[1] * wield->value[2] * skill / 120;
		  else
		    dam = dice(wield->value[1],wield->value[2]) * skill/120;
		}
		else
		  dam = dice(wield->value[1],wield->value[2]) * skill/120;

		if (wield->level > ch->level && wield->level > 10)
		  dam = dam * URANGE(15, ch->level, 50) / UMIN(50, 1 + wield->level);
		dam_limit = 3 * wield->value[1] * (wield->value[2] + 1);
	      }
	    else
	      {
		//OLD FORMAT
                dam = number_range( wield->value[1] * skill/120, wield->value[2] * skill/120);
		if (wield->level > (5 * ch->level/4) && wield->level > 20)
		  {
		    //DAMAGE PENALTY FOR LVL
		    if (ch->level < 15)
		      dam = dam * URANGE(15,ch->level,45) / UMIN(50, 5 * wield->level/4);
		    else
		      dam = dam * URANGE(15,5 * ch->level/4,45) / UMIN(45, wield->level);
		  }
		dam_limit = 3 * (wield->value[1] + wield->value[2]);
	      }
	  }
//HAND TO HAND ENHANCERS
	else if (is_affected(ch, gsn_body_weaponry))
	  dam = dice(ch->level / 4, 7);
	else if ( is_affected(ch, gen_holy_hands))
	  dam = dice(ch->level, 3) / 5;
	/* monk h2h */
	else if (get_skill(ch, gsn_martial_arts_b) > 1)
	  dam = dice(1 + (ch->level / 6), 6) * skill / 120;
	else if (ch->race == grn_werebeast)
	  dam = dice(1 + (ch->level / 7), 6);
	else
	  dam = dice(ch->level, 3) * skill / 750;
      }
//AVENGER and other crusader benefits
    if ( (temp = check_avenger(ch, victim)) != CRUSADE_NONE){
      if (temp == CRUSADE_MATCH)
	dam += 15 * dam / 100;
      else if (temp == CRUSADE_NOTMATCH)
	dam -= 10 * dam / 100;
    }
    else if (dam_type == DAM_BASH && number_percent() < get_skill(ch, gsn_dam_masteryB)){
      dam += number_range(5, 20) * dam / 100;
      check_improve(ch, gsn_dam_masteryB, TRUE, 1);
    }
    else if (dam_type == DAM_SLASH && number_percent() < get_skill(ch, gsn_dam_masteryS)){
      dam += number_range(5, 20) * dam / 100;
      check_improve(ch, gsn_dam_masteryS, TRUE, 1);
    }
    else if (dam_type == DAM_PIERCE && number_percent() < get_skill(ch, gsn_dam_masteryP)){
      dam += number_range(5, 20) * dam / 100;
      check_improve(ch, gsn_dam_masteryP, TRUE, 1);
    }


//ENHANCED DAMAGE
    if ( get_skill(ch,gsn_enhanced_damage) > 0 && !vamp_check(ch))
      {
	diceroll = number_percent();
	diceroll -= 2*(get_curr_stat(ch,STAT_LUCK) - 16);
	if (diceroll <= get_skill(ch,gsn_enhanced_damage))
	  {
	    check_improve(ch,gsn_enhanced_damage,TRUE,1);
	    dam += (dam * URANGE(20,diceroll,60)) /100;

	    /* Extra race bonus */
	    if (ch->race == race_lookup("ogre") )
	      dam += (dam * URANGE(0,diceroll,20)) /100;
	    else if ( ch->race == race_lookup("fire")
		      && wield
		      && (dam_type == DAM_FIRE || !str_cmp("fire", wield->material)) )
	      dam += (dam * URANGE(0,diceroll,20)) /100;
	  }
      }
//DOUBLE GRIP /2handed (not staffs )
    if  (is_affected(ch,gsn_double_grip)
	 && get_skill(ch,gsn_double_grip) > 0)
      {
	diceroll = number_percent();
	diceroll -= (get_curr_stat(ch,STAT_LUCK) - 16);
	if (diceroll <= get_skill(ch,gsn_double_grip)){
	  dam += (dam * number_range(5, 15)) /100;
	  check_improve(ch, gsn_double_grip, TRUE, 1);
	}
      }
    else if ( wield
	      && (IS_WEAPON_STAT(wield, WEAPON_TWO_HANDS) || is_affected(ch, gsn_double_grip))
	      && wield->value[0] != WEAPON_STAFF)
      dam += (dam * 15) / 100;


    //ANATOMY
    dam += dam * check_anatomy(ch, victim) / 200;

    //CEILING
      if (dam > dam_limit)
	dam = dam_limit;

      //HORSE
      if (!IS_NPC(victim) && victim->pcdata->pStallion != NULL && number_percent() < get_skill(victim, gsn_mounted)){
	dam -= dam/6;
	check_improve(victim, gsn_mounted, TRUE, 1);
      }

      //BAKCSTABS
      if ( dt == gsn_backstab )
	{
	  dam = UMIN(ch->level/12, 4) * dam + UMIN(50,GET_DAMROLL(ch))*2;
	  if (dam > 400)
	    dam = 400;
	}
      if ( dt == gsn_dual_backstab )
	{
	  dam = UMIN(ch->level/12, 3) * dam + UMIN(50,GET_DAMROLL(ch));
	  if (dam > 300)
	    dam = 300;
	}

      //POSITONS
      if ( !IS_AWAKE(victim))
	{
	  if (dt == gsn_backstab || dt == gsn_dual_backstab || dt == gsn_cleave || dt == gsn_assassinate || dt == gsn_ambush)
	    dam *= 1.5;
	  else
	    dam *= 2;
    }
    else if (victim->position < POS_FIGHTING)
      {
        if (dt == gsn_backstab || dt == gsn_dual_backstab || dt == gsn_cleave || dt == gsn_assassinate || dt == gsn_ambush)
	  dam *= 1.2;
	else
	    dam *= 1.5;
    }

    //MORE ATTACK DAMGE CHANGES
      if (dt == gsn_cleave)
	dam = (dam * 2) + ch->level;
      if (dt == gsn_assassinate)
	dam *= 2;
      if (dt == gsn_charge)
	dam = 3 * dam / 2;
      if (dt == gsn_ambush)
    {
	if (!can_see(victim,ch) && IS_AFFECTED2(ch,AFF_CAMOUFLAGE))
	    dam = (3 * dam) + ch->level;
	else
	    dam = (2 * dam) + ch->level;
	if (victim->fighting != NULL)
	    dam *= 2/3;
    }

    if (dt == gsn_circle || dt == gsn_sheath)
	dam += UMIN(dam, (2 * dam * UMIN(ch->level,50)) / 80) + ch->level;

    if (dt == gsn_riposte )
	dam /= 4;

/* damage increase due to rage */
    if (IS_AFFECTED2(ch, AFF_RAGE)){
      int chance = 2 * get_skill(ch, gsn_rage) / 3 + ch->level / 3;
      chance += 2 * (get_curr_stat(ch,STAT_LUCK) - 16);
      chance = UMAX(1, chance);

      /* check if we do rage */
      if (number_percent() < chance){
	int str_bonus = get_curr_stat(ch, STAT_STR);
	int lvl_bonus = ch->level / 2;

	/* path of devastation	= str_bonus + level bonus
	   path of fury		= str_bonus;
	   path of anger	= str_bonus / 4;
	   otherwise		= str_bonus / 2
	*/

	if (get_skill(ch, gsn_path_dev))
	  dam += dam * (str_bonus + lvl_bonus) / 100;
	else if (get_skill(ch, gsn_path_fury))
	  dam += dam * str_bonus / 100;
	else if (get_skill(ch, gsn_path_anger))
	  dam += dam * str_bonus / 400;
	else
	  dam += dam * str_bonus / 200;
	check_improve(ch,gsn_rage,TRUE,1);
      }
    }

    //HROLL and DROLL
    hroll = GET_HITROLL(ch);
    droll = GET_DAMROLL(ch);

    /* racial enemy */
    if (!IS_NPC(ch) && ch->pcdata->enemy_race && victim->race == ch->pcdata->enemy_race){
      hroll += ch->level / 5;
      droll += ch->level / 5;
    }

    //CAPS
    if (hroll > 25)
      hroll = (hroll - 25) / 2 + 25;
    else if (hroll > 50)
      hroll = (hroll - 50) / 3 + (25 / 2) + 25;

    if (droll > (ch->level * 2) && droll > 20){
      int tdam, ddam, i;
      tdam = droll - (ch->level * 2);
      ddam = 0;
      for(i = 3;tdam > 10;i++){
	tdam -= 10;
	ddam += UMAX(0,7 - i);
      }
      ddam += (tdam / (i + 1)) + (ch->level * 2);
      dam += ddam * UMIN(100,skill) /100;
    }
    else if (droll <= (ch->level * 2) || droll <= 20)
      dam += droll * UMIN(80, skill) / 100;

    if (dam > 750)
      dam = 750;

//DAMAGE REDUCTION DUE TO AC
    if (is_armor_enhanced(ch) > ENHANCE_NONE)
      ac_red = 1200;
    else
      ac_red = 1600;
    switch(dam_type)
    {
	case(DAM_PIERCE):temp = GET_AC(victim,AC_PIERCE) * dam / ac_red;break;
	case(DAM_BASH):	 temp = GET_AC(victim,AC_BASH)   * dam / ac_red;break;
	case(DAM_SLASH): temp = GET_AC(victim,AC_SLASH)  * dam / ac_red;break;
	default:	 temp = GET_AC(victim,AC_EXOTIC) * dam / ac_red;break;
    };
    /* armor piercing */
    /* better chance of ignore blow when using skill and has chance for
       damaging blow */

    apierce = get_skill(ch, gsn_apierce);
    if ( temp < 0 && (paf = affect_find(ch->affected, gen_apierce)) != NULL
	 && paf->modifier >= 0 //modifer set to -1 after success in gen
	 && number_percent() < apierce){
      /* gen will attempt armor destroying blow */
      paf->modifier = 1;
      check_improve(ch, gsn_apierce, TRUE, 1);
    }
    else if (number_percent() < apierce / 2){
      check_improve(ch, gsn_apierce, TRUE, 1);
    }
    else
      dam =  UMAX(1, dam + temp);

//MORE DAMAGE CAPPING
    if (ch->level < 15)
      dam = UMIN(dam, (25 + (ch->level * 2)));
    dam = UMAX(0,dam);

//COUNTER
    if (victim->class == gcn_monk
	&& victim->fighting
	&& is_affected(victim, gsn_horse)
	&& is_empowered_quiet(victim,1)){
      success = get_skill(victim, gsn_horse);
      success = ((success > 99?10:(success > 95?5:0)) + (success - 75)/5) + 10;
      check_improve(victim, gsn_horse, TRUE, 1);
      fReverse = TRUE;
    }
    else
      success = get_skill(victim, gsn_counter);

    //skill modifiers
    if (success > 0){
      success += victim->level - ch->level;
      success += get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR);
      success += (get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX)) *2;
      success += (get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK));
      success = UMIN(90, success);
    }
    if (fReverse
	&& victim->fighting
	&& number_percent() < success
	&& (victim->class == gcn_monk
	    && is_affected(victim, gsn_horse)
	    && is_empowered_quiet(victim,1)
	    && victim->mana >= 2
	    && victim->fighting == ch
	    )
	){
      victim->mana -= 2;
      act("You reverse $n's attack against $m.",ch,NULL,victim,TO_VICT);
      act("$N moves suddenly and reverses your attack against you.",ch,NULL,victim,TO_CHAR);
      act("$N reverses $n's attack against $m.",ch,NULL,victim,TO_NOTVICT);

      result = virtual_damage( victim, ch, wield, dam, dt, dam_type, hroll, ch->level, TRUE, FALSE, 0);
    }
    else if (victim->fighting == NULL && counter_check(ch, victim, success, dam, dam_type, gsn_counter)){
      set_fighting( ch, victim );
    }
    else{
      //Avenger boosts hitroll
      if ( (temp = check_avenger(ch, victim)) != CRUSADE_NONE){
	if (temp == CRUSADE_MATCH)
	  hroll += 20 * hroll / 100;
	else if (temp == CRUSADE_NOTMATCH)
	  hroll -= 15 * hroll / 100;
      }

//////////////////
//DO DAMAGE HERE//
//////////////////
      result = virtual_damage( ch, victim, wield, dam, dt, dam_type,
			       hroll, ch->level, TRUE, FALSE, 0);
    }
    tail_chain( );

}

int dam_spell(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dam_type)
{
    AFFECT_DATA *paf;
    //Damage Non is not affected by any of this.
    if (dam < 1)
      return 0;

    if (dam_type == DAM_INTERNAL)
      return dam;

    if (dam_type != DAM_NONE)
      {
	/* area ownwers */
	if (ch->in_room && ch->in_room->area->pCabal && IS_EVIL(ch) && IS_CABAL(ch->in_room->area->pCabal, CABAL_SWARM) && !is_same_cabal(ch->in_room->area->pCabal, victim->pCabal))
	  dam = 11 * dam / 10;
	/* force field */
	if ((dam > 1 && dam_type == DAM_BASH) && ((paf = affect_find(victim->affected, gsn_forcefield)) != NULL))
	  dam = UMIN(dam, paf->modifier);
	if (dam > 1 && ((paf = affect_find(victim->affected, gen_discord)) != NULL))
	  dam -= (paf->modifier? dam / 4 : 0);
	if (dam > 1 && ((paf = affect_find(ch->affected, gen_discord)) != NULL))
	  dam += (paf->modifier? 0 : 15 * dam / 100);
	if ( dam > 1 && (dam_type <= DAM_SLASH) && is_affected(victim, gsn_air_shield))
	    dam = 2 * dam / 3;
	if ( dam > 1 && !IS_NPC(victim) && victim->pcdata->condition[COND_DRUNK]  > 10 )	dam = dam * 8 / 10;
	if ( dam > 1 && is_affected(victim, gsn_mental_barrier) && dam_type == DAM_MENTAL)	dam -= dam / 3;
	if ( dam > 1 && is_affected(victim, gsn_psychic_purge)
	     && dam_type == DAM_MENTAL
	     && check_immune(victim, DAM_MENTAL, FALSE) < IS_VULNERABLE)
	  dam += dam / 4;
	if ( dam > 1 && is_affected(victim, gen_watchtower) )			 dam = 3 * dam / 4;
	/* EXCLUSIVE SANC LIKE EFFECTS */
	if ( dam > 1 && IS_AFFECTED(victim, AFF_SANCTUARY) )			 dam /= 2;
	else if ( dam > 1 && is_affected(victim, gsn_lifeforce) )		 dam /= 2;
	else if ( dam > 1 && is_affected(victim, gsn_pyramid_of_force) )	 dam = 5 * dam / 10;
	if ( dam > 1 && !IS_NPC(ch) && IS_SET(ch->act2,PLR_GIMP) )			        dam /= 2;
	if ( dam > 1 && IS_AFFECTED2(victim, AFF_ICE_SHIELD) && dam_type == DAM_FIRE)       dam /= 2;
	if ( dam > 1 && IS_AFFECTED2(victim, AFF_FIRE_SHIELD) && dam_type == DAM_COLD)      dam /= 2;
	if ( dam > 1 && IS_AFFECTED2(victim, AFF_ICE_SHIELD) && dam_type == DAM_COLD)	dam = dam * 3 / 2;
	if ( dam > 1 && IS_AFFECTED2(victim, AFF_FIRE_SHIELD) && dam_type == DAM_FIRE)	dam = dam * 3 / 2;
	if ( dam > 1 && is_affected(victim, gsn_wolf_form) && dam_type > 3) dam = 7 * dam / 10;
	if (dam > 1 && dam_type == DAM_MENTAL && IS_SET(victim->act, ACT_UNDEAD)) dam = dam / 2;
	if ( dam > 1 && (paf = affect_find(victim->affected, gen_dvoid)) != NULL
		   && paf->modifier  < -6)
	  dam -= dam / 4;
	else if ( dam > 1 && ((IS_AFFECTED(victim, AFF_PROTECT_EVIL) && IS_EVIL(ch))
	  || (IS_AFFECTED(victim, AFF_PROTECT_GOOD) && IS_GOOD(ch) )))
	  dam -= dam / 4;
	else if ( dam > 1 && (IS_GOOD(ch) || IS_EVIL(ch))
		  && is_affected(victim, gsn_greenheart)){
	  dam -= dam / 4;
	}
	if ( dam > 1 && (IS_GOOD(victim) || IS_EVIL(victim))
	     && is_affected(ch, gsn_greenheart)){
	  dam = 11 * dam / 10;
	}
	if ( dam > 1 && IS_AFFECTED(victim, AFF_WARD_LIGHT) ){
	  dam -= dam / 5;
	}
	if ( is_affected(victim,gsn_horse) && monk_good(ch, WEAR_HEAD))
	  dam -= dam / 5;
	if ( is_affected(victim,gsn_aura) )
	  dam -= dam / 6;
	if ( victim->class == gcn_blademaster && is_affected(victim,gsn_iron_curtain) && get_eq_char(victim, WEAR_SECONDARY) != NULL)
	  dam -= dam / 4;
	if ( is_affected(victim,gsn_stance) )	dam = dam / 2;
	else if ( is_affected(victim,gsn_sacred_runes) )
	  dam = dam / 2;
      }

    if ( dam > 1 && IS_AFFECTED2(victim, AFF_MANA_SHIELD) && dam_type > 3)
      {
	if (dam / 5 <= victim->mana)
	  victim->mana -= dam / 5;
	else
	  {
	    dam -= victim->mana * (victim->level/5);
	    victim->mana = 0;
	    REMOVE_BIT(victim->affected2_by,AFF_MANA_SHIELD);
	    if (is_affected(ch, skill_lookup("mana shield"))){
	      act("Your mana shield flares brightly then dissipates.",victim,NULL,NULL,TO_CHAR);
	      act("$n's mana shield flares brightly then dissipates.",victim,NULL,NULL,TO_ROOM);
	    }
	    else{
	      act("Your magical protection dissipates.",victim,NULL,NULL,TO_CHAR);
	      act("$n's magical protection dissipates.",victim,NULL,NULL,TO_ROOM);
	    }

	}
    }
    if ( dam > 1 && IS_AFFECTED2(victim, AFF_BARRIER) && (dam_type <= 3 && dam_type))
    {
	if (dam / 3 <= victim->mana)
	    victim->mana -= dam / 3;
	else
	{
	    dam -= victim->mana * (victim->level/3);
	    victim->mana = 0;
	    REMOVE_BIT(victim->affected2_by,AFF_BARRIER);
	    act("Your physical barrier flares brightly then dissipates.",victim,NULL,NULL,TO_CHAR);
	    act("$n's physical barrier flares brightly then dissipates.",victim,NULL,NULL,TO_ROOM);
	}
    }
    return dam;
}

int dam_monk(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt)
{
  int chance = 0, anatomy = 0, shield = 0;
  bool fIgnore = FALSE;
  bool fCrane = FALSE;
  bool fTiger = FALSE;
  bool fDragon = FALSE;
  bool fMonkey = FALSE;
  int  Chak = 0;	//duration to shave off of a prot. shield if present.

  if (race_table[victim->race].pc_race)
    anatomy = anatomy_lookup(pc_race_table[victim->race].anatomy);

  if (is_empowered_quiet(ch, 0)){
    chance = ch->pcdata->anatomy[anatomy] / 2;
    chance += get_skill(ch,gsn_anatomy) / 10;
    chance += GET_HITROLL(ch) / 2;
    chance += URANGE(-5, (ch->level - victim->level), 5);
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    chance = URANGE(0, chance, 70);
  }

  //get which stance we are in.
  if (is_affected(ch, gsn_crane) && is_empowered_quiet(ch, 0))
    fCrane = TRUE;
  else if (is_affected(ch, gsn_tiger) && is_empowered_quiet(ch, 0))
    fTiger = TRUE;
  else if (is_affected(ch, gsn_dragon) && is_empowered_quiet(ch, 0))
    fDragon = TRUE;
  else if (is_affected(ch, gsn_monkey) && is_empowered_quiet(ch, 0))
    fMonkey = TRUE;


  if (fMonkey)
    dam = 6 * dam / 10;

  if (fCrane && number_percent() < get_skill(ch, gsn_crane))
    {
      chance += 5 + UMAX(0, get_skill(ch,gsn_crane) - 80);
      check_improve(ch, gsn_crane, TRUE, 1);
    }

  //IMMUNES
  shield = get_lagprot(victim);
  if (IS_SET(victim->act, ACT_UNDEAD)
      || (victim->race == race_lookup("ogre")
	  && number_percent() < victim->level
	  )
      || shield == LAG_SHIELD_PROT
      || shield == LAG_SHIELD_FFIELD
      || shield == LAG_SHIELD_ARMOR
      || shield == LAG_SHIELD_NONCORP
      || shield == LAG_SHIELD_IWILL
      )
    fIgnore = TRUE;
  if (fIgnore && is_affected(ch, gsn_invigorate))
    fIgnore = FALSE;

  switch (dt)
    {
    case (1053): dam = 5 * dam / 8;	break;
    case (1054): dam = 7 * dam / 8;	break;
    case (1055): dam = dam;
      if ( fCrane && number_percent() < chance)
	{
	  act("$n's elbow brutaly connects with your temple.", ch, NULL, victim, TO_VICT);
	  act("$n's elbow brutaly connects with $N's temple.", ch, NULL, victim, TO_NOTVICT);
	  act("Your elbow brutaly connects with $N's temple.", ch, NULL, victim, TO_CHAR);
	  check_improve(ch, gsn_crane, TRUE, 1);
	  if (fIgnore)
	    {
	    send_to_char("You easly shrug off the feeble blow.\n\r", victim);
	    act("$n seems to not even notice!", victim, NULL, victim, TO_ROOM);
	    }
	  else
	    {
	      victim->mana -= victim->max_mana / UMAX(1,(70 - chance / 2));
	      victim->mana = UMAX(0, victim->mana);
	      WAIT_STATE2(victim, 6);
	    }
	}
      break;

    case (1056): dam = dam;
      if ( fCrane && number_percent() < chance)
	{
	  act("$n drives his palm into your torso with crushing strength.", ch, NULL, victim, TO_VICT);
	  act("$n drives his palm into $N's torso with crushing strength", ch, NULL, victim, TO_NOTVICT);
	  act("You drive your palm into a sensetive spot on $N's torso.", ch, NULL, victim, TO_CHAR);
	  check_improve(ch, gsn_crane, TRUE, 1);

	  if (fIgnore)
	    {
	    send_to_char("You easly shrug off the feeble blow.\n\r", victim);
	    act("$n seems to not even notice!", victim, NULL, victim, TO_ROOM);
	    }
	  else
	    {
	      WAIT_STATE2(victim, 6);
	      dam = 6 * dam / 5;
	    }
	}
      break;
    case (1057): dam = 8 * dam / 7;
      if (number_percent() < chance )
	{
	  if (fTiger && number_percent() < 2 * (get_skill(ch, gsn_tiger) - 50) )
	    {
	      act("$n executes \"twin tiger claw\", stunning you.",ch,NULL,victim,TO_VICT);
	      act("You execute \"twin tiger claw\", stunning $N.",ch,NULL,victim,TO_CHAR);
	      act("$n executes \"twin tiger claw\", stunning $N.",ch,NULL,victim,TO_NOTVICT);
	      check_improve(ch, gsn_tiger, TRUE, -99);
	      WAIT_STATE2(victim, 34);
	      Chak += 4;
	      damage(ch, victim, UMIN(3 * dam / 2, victim->hit - 1), gsn_tiger, DAM_PIERCE, TRUE);
	      if (victim->position <= POS_STUNNED)
		dam  = 0;
	      else
		dam += victim->hit/10;
	    }
	  else
	    {
	      act("$n strikes a chakera point, stunning you.",ch,NULL,victim,TO_VICT);
	      act("You strike a chakera point, stunning $N.",ch,NULL,victim,TO_CHAR);
	      act("$n strikes a chakera point stunning $N.",ch,NULL,victim,TO_NOTVICT);

	      /* get the right stance improv */
	      if (fCrane)
		check_improve(ch, gsn_crane, TRUE, 0);
	      else if (fTiger)
		check_improve(ch, gsn_tiger, TRUE, 0);
	      else if (fDragon)
		check_improve(ch, gsn_dragon, TRUE, 0);
	      else if (fMonkey)
		check_improve(ch, gsn_dragon, TRUE, 0);


	      if (fIgnore)
		{
		  send_to_char("You easly shrug off the feeble blow.\n\r", victim);
		  act("$N seems to not even notice!", victim, NULL, victim, TO_ROOM);
		  Chak += 2;
		}
	      else
		{
		  dam += victim->hit/10;
		  WAIT_STATE2(victim, 24);
		}//end not igonre
	    }
	}
      break;
    case (1058): dam = 8 * dam / 7;
      if ( number_percent() < (fDragon? 10 + chance/3 : chance / 3) && !fCrane)
	{
	  if (fDragon && number_percent() < 2 * (get_skill(ch, gsn_dragon) - 50))
	    {
	      act("$n executes \"strike of the phantom dragon\".",ch,NULL,victim,TO_VICT);
	      act("You execute \"strike of the phantom dragon\".",ch,NULL,victim,TO_CHAR);
	      act("$n executes \"strike of the phantom dragon\"",ch,NULL,victim,TO_NOTVICT);
	      WAIT_STATE2(victim, number_range(3, 4) * 12);
	      Chak += 10;
	      dam = dam * ch->level / 15;
	      act("A phantasmal dragon rears its head and breaths.",ch,NULL,victim,TO_ALL);
	      damage(ch, victim, 3 * UMIN(dam, victim->hit - 1) / 2, gsn_phantom_dragon, DAM_ENERGY, TRUE);
	      check_improve(ch, gsn_dragon, TRUE, -99);
	      if (victim->position <= POS_STUNNED
		  || !victim->fighting )
		dam  = 0;
	      else
		{
		  act("Blood spraying about, $n buries $s fingers in $N's torso!", ch, NULL, victim, TO_NOTVICT);
		  act("The power of Dragon in your veins you bury your fingers in $N's torso!", ch, NULL,victim,  TO_CHAR);
		  act("The world dims with pain as $n buries his fingers in your torso.", ch, NULL, victim, TO_VICT);
		}
	    }
	  else
	    {
	      act("$n strikes a chakera point, forcefully sending you collapsing to the ground.",ch,NULL,victim,TO_VICT);
	      act("You struck a chakera point, forcefully sending $N collapsing to the ground.",ch,NULL,victim,TO_CHAR);
	      act("$n strikes a chakera point, forcefully sending $N collapsing to the ground.",ch,NULL,victim,TO_NOTVICT);
	      /* get the right stance improv */
	      if (fCrane)
		check_improve(ch, gsn_crane, TRUE, 0);
	      else if (fTiger)
		check_improve(ch, gsn_tiger, TRUE, 0);
	      else if (fDragon)
		check_improve(ch, gsn_dragon, TRUE, 0);
	      else if (fMonkey)
		check_improve(ch, gsn_dragon, TRUE, 0);
	      if (fIgnore)
		{
		  send_to_char("You easly shrug off the feeble blow.\n\r", victim);
		  act("$N seems to not even notice!", victim, NULL, victim, TO_ROOM);
		  Chak += 2;
		}
	      else
		{
		  WAIT_STATE2(victim, 24);
		  dam = dam * number_range(45, ch->pcdata->anatomy[anatomy]) / 30;
		  dam += victim->hit / 15;
		}
	    }//END REGULAR DRAGON
	}//END STRIKE OF DRAGON
      break;
    case (1059): dam = 5 * dam / 8;	break;
    case (1060): dam = 7 * dam / 8;	break;
    case (1061): dam = dam;	        break;
    case (1062): dam = dam;		break;
    case (1063): dam = 8 * dam / 7;     break;
    case (1064): dam = 8 * dam / 7;     break;
    }
  //do the effects for the kicks.
  if ( fCrane && number_percent() < chance
       && dt <= 1064 && dt >= 1062)

    {
      act("$n's kick brutaly connects with your leg.", ch, NULL, victim, TO_VICT);
      act("$n's kick brutaly connects with $N's leg.", ch, NULL, victim, TO_NOTVICT);
      act("Your kick brutaly connects with $N's leg.", ch, NULL, victim, TO_CHAR);
      check_improve(ch, gsn_crane, TRUE, 1);
      dam = 6 * dam / 5;
      if (fIgnore)
	{
	  send_to_char("You easly shrug off the feeble blow.\n\r", victim);
	  act("$n seems to not even notice!", victim, NULL, victim, TO_ROOM);
	}
      else
	{
	  victim->move -= victim->max_move * 2 / UMAX(1,(70 - chance / 2));
	  victim->move = UMAX(0, victim->move);
	  WAIT_STATE2(victim, 6);
	}
    }
  /* check for weakening of prot. shield */
  if (fIgnore && Chak && !IS_NPC(victim)){
    AFFECT_DATA* paf;
    if ( (paf = affect_find(victim->affected, gsn_protective_shield)) != NULL){
      paf->duration -= Chak;
      if (paf->duration < 0){
	affect_strip(victim, gsn_protective_shield);
	if ( skill_table[paf->type].msg_off )
	  act_new(skill_table[paf->type].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
	if ( skill_table[paf->type].msg_off2 )
	  act(skill_table[paf->type].msg_off2,ch,NULL,NULL,TO_ROOM);
      }
    }
    else if ( (paf = affect_find(victim->affected, gsn_forcefield)) != NULL){
      paf->duration -= Chak;
      if (paf->duration < 0){
	affect_strip(victim, gsn_forcefield);
	if ( skill_table[paf->type].msg_off )
	  act_new(skill_table[paf->type].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
	if ( skill_table[paf->type].msg_off2 )
	  act(skill_table[paf->type].msg_off2,ch,NULL,NULL,TO_ROOM);
      }
    }
    else if ( (paf = affect_find(victim->affected, gsn_iron_will)) != NULL){
      paf->duration -= Chak;
      if (paf->duration < 0){
	affect_strip(victim, gsn_iron_will);
	if ( skill_table[paf->type].msg_off )
	  act_new(skill_table[paf->type].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
	if ( skill_table[paf->type].msg_off2 )
	  act(skill_table[paf->type].msg_off2,ch,NULL,NULL,TO_ROOM);
      }
    }
  }
  return dam;
}

void dam_update(CHAR_DATA *ch, CHAR_DATA *victim, int dam)
{
  update_pos( victim );
  switch( victim->position )
    {
    case POS_MORTAL:
        act("$n is mortally wounded, and will die soon, if not aided.", victim, NULL, NULL, TO_ROOM );
        act_new("You are mortally wounded, and will die soon, if not aided.",victim, NULL, NULL, TO_CHAR, POS_DEAD );
	break;
    case POS_INCAP:
	act("$n is incapacitated and will slowly die, if not aided.",victim, NULL, NULL, TO_ROOM );
        act_new("You are incapacitated and will slowly die, if not aided.",victim, NULL, NULL, TO_CHAR, POS_DEAD );
	break;
    case POS_STUNNED:
        act("$n is stunned, but will probably recover.",victim, NULL, NULL, TO_ROOM );
        act_new("You are stunned, but will probably recover.",victim, NULL, NULL, TO_CHAR, POS_DEAD );
	break;
    case POS_DEAD:
      if (!IS_NPC(victim)
	  && ( (!IS_NPC(ch) && is_challenger(ch,victim))
	       || (IS_NPC(ch)
		   && ch->master
		   && is_challenger(ch->master,victim))
	       )
	  )
	{
	    act( "$n is mortally wounded!!", victim, NULL, NULL, TO_ROOM);
	    act_new( "You have been mortally wounded!!", victim, NULL, NULL, TO_CHAR, POS_DEAD );
	}
      else if ( !IS_NPC(victim)
		&& victim->pcdata->pbounty
		&& can_collect(victim->pcdata->pbounty, ch )){
	if (!is_affected(victim, gsn_mortally_wounded)){
	  act( "$n is mortally wounded!!", victim, NULL, NULL, TO_ROOM);
	  act_new( "You have been mortally wounded!!", victim, NULL, NULL, TO_CHAR, POS_DEAD );
	}
      }
      else if (!IS_NPC(victim) && IS_SET(victim->act, PLR_WANTED)
	       && ch != victim
	       && ( (!IS_NPC(ch) && ch->pCabal && IS_CABAL(ch->pCabal, CABAL_JUSTICE))
		    || (IS_NPC(ch) && ch->leader
			&& ch->leader->pCabal && IS_CABAL(ch->leader->pCabal, CABAL_JUSTICE))
		    || (IS_NPC(ch) && IS_SET(ch->act2, ACT_LAWFUL))
		    )
	       ){
	act("$n stops short of a killing blow.", ch, NULL, victim, TO_ROOM);
	act_new("$n stops short of a killing blow.", ch, NULL, victim, TO_VICT, POS_DEAD);
	act("You stop short of a killing blow.", ch, NULL, victim, TO_CHAR);
      }
      else if (ch != victim
	       && !IS_NPC(ch)
	       && !IS_NPC(victim)
	       && (IS_GAME(ch, GAME_MERCY) || (ch->in_room && IS_ROOM2(ch->in_room, ROOM_MERCY)))
	       && !is_affected(victim, gsn_mercy_wait)
	       ){
	act("At the last moment you halt your deathblow.", ch, NULL, NULL, TO_CHAR);
	act("At the last moment $n halts $s deathblow.", ch, NULL, NULL, TO_ROOM);
      }
      /* BLADEMASTER DEATHSTRIKE */
      else if (deathstrikeCheck(ch, victim)){
	  act( "$n is DEAD!!", victim, NULL, NULL, TO_ROOM );
	  act_new( "You have been `1KILLED``!!\n\r", victim, NULL, NULL, TO_CHAR, POS_DEAD );
      }
      else if (IS_SET(victim->act,ACT_UNDEAD))
	{
	  act( "$n is SLAIN!!", victim, NULL, NULL, TO_ROOM );
	  act_new( "You have been `1SLAYED``!!\n\r", victim, NULL, NULL, TO_CHAR, POS_DEAD );
	}
      else
	{
	  act( "$n is DEAD!!", victim, NULL, NULL, TO_ROOM );
	  act_new( "You have been `1KILLED``!!\n\r", victim, NULL, NULL, TO_CHAR, POS_DEAD );

	}
      break;
    default:
      if ( dam > victim->max_hit / 4 )
	{

	  //Continuum message.
	  continuum(victim, CONTINUUM_HURT);

	  send_to_char( "That really did HURT!\n\r", victim );
	  if (number_percent( ) > (victim->level * 2) && !IS_AFFECTED(victim,AFF_CALM) && victim->level >= 6
	      && !IS_AFFECTED2(victim,AFF_RAGE) && !IS_NPC(victim) && victim->class == class_lookup("berserker"))
	    {
	      AFFECT_DATA af;
	      act("You feel your veins course with power as the madness engulfs you!",victim,NULL,NULL,TO_CHAR);
	      act("$n's eyes suddenly burn like hot coals as $e is gripped by insanity!",victim,NULL,NULL,TO_ROOM);
	      af.where            = TO_AFFECTS2;
	      af.type             = gsn_rage;
	      af.level            = victim->level;
	      af.duration         = UMAX(0, 4 - number_fuzzy(victim->level / 11));
	      af.location         = APPLY_STR;
	      af.modifier         = victim->size *2;
	      af.bitvector        = AFF_RAGE;
	      affect_to_char(victim,&af);
	      af.location         = APPLY_HITROLL;
	      af.modifier         = str_app[get_curr_stat(victim,STAT_STR)].tohit + victim->level/4;
	      affect_to_char(victim,&af);
	      af.location         = APPLY_DAMROLL;
	      af.modifier         = str_app[get_curr_stat(victim,STAT_STR)].todam + victim->level/4;
	      affect_to_char(victim,&af);
	      af.location         = APPLY_HIT;
	      af.modifier         = (victim->level * victim->perm_stat[STAT_CON]) / 3;
	      affect_to_char(victim,&af);
	      victim->hit += (victim->level * victim->perm_stat[STAT_CON]) / 3;
	      check_improve(victim,gsn_rage,TRUE,1);
	    }
	}
      if ( victim->hit < victim->max_hit / 4 )
	send_to_char( "You sure are BLEEDING!\n\r", victim );
      break;
    }
}

void dam_dead(CHAR_DATA *ch, CHAR_DATA *victim)
{
  bool fBountyCheck = TRUE;
  if (!IS_NPC(victim)
      && ( (!IS_NPC(ch)
	    && is_challenger(ch,victim))
	   || (IS_NPC(ch)
	       && ch->master
	       && is_challenger(ch->master,victim)) ))
    {
      mortal_wound(ch,victim);
      return;
    }
  else if ( !IS_NPC(victim)
	    && victim->pcdata->pbounty
	    && can_collect(victim->pcdata->pbounty, ch )){
    if (!is_affected(victim, gsn_mortally_wounded)){
      mercy_effect(ch, victim, 4);
      return;
    }
    else{
      send_to_char( "You have been `1KILLED`` by a Bounty Hunter!\n\r\n\r",victim );
      fulfill_bounty( ch, victim, "1" );
      fBountyCheck = FALSE;
      affect_strip(ch, skill_lookup("analyze"));
    }
  }
/* ACT_NODEATH mobs */
  else if (IS_NPC(ch)
	   && !IS_NPC(victim)
	   && IS_SET(ch->act2, ACT_NODEATH)){
    int pos = POS_STANDING;
    OBJ_DATA* obj = NULL;
    mortal_wound(ch,victim);
    pos = ch->position;
    /* triggers */
    /* we save position so we can allow mobs to do things on death. */
    ch->position = POS_STANDING;
    if (HAS_TRIGGER_MOB(ch, TRIG_KILL))
      p_percent_trigger(ch, NULL, NULL, victim, NULL, NULL, TRIG_KILL);

/* Objects */
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ){
      if (HAS_TRIGGER_OBJ(obj, TRIG_KILL))
	p_percent_trigger(NULL, obj, NULL, victim, NULL, NULL, TRIG_KILL);
    }
    /* Room Triggers */
    if (HAS_TRIGGER_ROOM(ch->in_room, TRIG_KILL))
      p_percent_trigger(NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_KILL);
    ch->position = pos;
    return;
  }
  /* jail captuer for justice */
  else if (!IS_NPC(victim)
	   && IS_SET(victim->act, PLR_WANTED)
	   && ch != victim
	   && ( (!IS_NPC(ch) && ch->pCabal && IS_CABAL(ch->pCabal, CABAL_JUSTICE))
		|| (IS_NPC(ch) && ch->leader
		    && ch->leader->pCabal && IS_CABAL(ch->leader->pCabal, CABAL_JUSTICE))
		|| (IS_NPC(ch) && IS_SET(ch->act2, ACT_LAWFUL))
		)
	   ){
    jail_char(ch, victim);
    return;
  }
/* MERCY KILL FOR WARRIORS */
  else if (ch != victim
	   && !IS_NPC(ch)
	   && !IS_NPC(victim)
	   && (IS_GAME(ch, GAME_MERCY)
	       || (ch->in_room && IS_ROOM2(ch->in_room, ROOM_MERCY)))
	   && !is_affected(victim, gsn_mercy_wait)
	   ){
    mercy_effect(ch, victim, 2);
    return;
  }
  /* CABAL ALTAR DEATH */
  else if (ch != victim
	   && IS_NPC(victim)
	   && (victim->pIndexData->vnum == MOB_VNUM_ALTAR || IS_SET(victim->off_flags, CABAL_GUARD))){
    cabal_altar_death( ch, victim);
    return;
  }
  if (unholy_gift(ch, victim))
    return;
  group_gain( ch, victim );
  if ((victim == ch && victim->class != class_lookup("vampire"))
      || (IS_NPC(ch) && !IS_AFFECTED(ch,AFF_CHARM)) ){
    if (IS_NPC(ch) && IS_SET(ch->act2, ACT_NOLOSS))
      send_to_char("Somehow you are spared the loss of your experience!\n\r", victim);
    else
      victim->exp -= UMIN(15000,UMIN(exp_per_level(victim,victim->level),
				     victim->exp/10));
  }
  _raw_kill( ch, victim, fBountyCheck );

  if (ch != victim)
    do_autos(ch);
  if (IS_AFFECTED2(ch,AFF_RAGE))
    {
      REMOVE_BIT(ch->affected2_by,AFF_RAGE);
      affect_strip(ch,gsn_rage);
      act("You snap out of your madness at the death of your opponent.",ch,NULL,NULL,TO_CHAR);
      act("$n gasps for air as $e snaps out of $s insanity.",ch,NULL,NULL,TO_ROOM);
      ch->hit -= (ch->level * ch->perm_stat[STAT_CON]) /3;
      if (IS_IMMORTAL(ch))
	ch->hit = UMAX(1,ch->hit);
      if (ch->hit < -2)
	{
	  int shock_mod = (25 * (0 - ch->hit)) / ch->max_hit;
	  if (number_percent( ) < con_app[UMAX(1,ch->perm_stat[STAT_CON] - shock_mod)].shock + 2*(get_curr_stat(ch,STAT_LUCK) - 16))
	    {
	      act_new("Your body suddenly awakens to the wounds you've sustained and you lose consciousness.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
	      act("$n suddenly clutches $s wounds and slumps to the ground, unconscious.",ch,NULL,NULL,TO_ROOM);
	      ch->hit = -2;
	      ch->position = POS_STUNNED;
	      act( "$n is stunned, but will probably recover.",ch, NULL, NULL, TO_ROOM );
	      act_new("You are stunned, but will probably recover.",ch, NULL, NULL, TO_CHAR, POS_DEAD );
	    }
	  else
	    {
	      act_new("Your body could not sustain the injuries you've suffered.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
	      act("$n suddenly clutches $s wounds and slumps to the ground.",ch,NULL,NULL,TO_ROOM);
	      act( "$n is DEAD!!",ch, 0, 0, TO_ROOM );
	      act_new( "You have been `1KILLED``!!", ch,NULL,NULL,TO_CHAR,POS_DEAD);
	      raw_kill( ch, ch );

	    }
	}
    }
}

void dam_after(CHAR_DATA *ch, CHAR_DATA *victim, int dam)
{
    if ( !IS_NPC(victim) && victim->desc == NULL && victim->level < 11)
	if ( number_range( 0, victim->wait ) == 0 )
	{
	    do_recall( victim, ch->name );
	    return;
	}
    if ( IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 4)
      if ( ( IS_SET(victim->act, ACT_WIMPY)
	     && number_bits( 2 ) == 0 && victim->hit < victim->max_hit / 5)
	   || ( IS_AFFECTED(victim, AFF_CHARM)
		&& victim->master != NULL
		&& victim->master->in_room != victim->in_room
		&& !is_affected(victim,gsn_linked_mind))
	   || ( IS_NPC(victim) && IS_SET(victim->special,SPECIAL_COWARD))
	   || ( IS_NPC(ch) && !IS_AFFECTED(ch,AFF_CHARM)
		&& !IS_AFFECTED(victim,AFF_CHARM)
		&& !IS_SET(victim->act, ACT_SENTINEL)
		&& !IS_SET(victim->act, ACT_RAIDER )
		&& !IS_SET(ch->act, ACT_RAIDER )
		&& ch->level > victim->level
		&& !IS_SET(victim->off_flags,CABAL_GUARD)
		&& !IS_SET(victim->off_flags,GUILD_GUARD)
		&& !is_affected(victim,gsn_timer)
		&& !is_affected(ch,gsn_timer) ))
	do_flee( victim, ch->name );
    if ( !IS_NPC(victim)
	 && victim->hit > 0
	 && victim->hit <= victim->wimpy
	 && victim->wait < PULSE_VIOLENCE / 4)
      do_flee( victim, ch->name );
}



/* Virtual damage function */
/*(X = Virtual only)
ch = character doing the damage
vic = character receving damage
obj = obj doing the damage (X)
dam = damage to be done + race vuln.
dam_type = type of damage: slash, fire, ice etc.
h_roll = hitroll to be used for parry/block
lvl = level of the attacker / obj/spell
show = damage shown?
fVirt = Virtual attack (obj/spell/skill that can be blocked etc. like reg. attack.) (X)
sn = skill/spell doing the virtual attack (X)

*/

int virtual_damage(CHAR_DATA *ch,CHAR_DATA *victim, OBJ_DATA* obj, int dam, int dt, int dam_type, int h_roll, int lvl,  bool set_combat, bool fVirt, int sn)
{
    bool immune;
    AFFECT_DATA* paf;


    if (ch->in_room == NULL || ch->in_room->vnum == ROOM_VNUM_LIMBO )
	/* Viri ||  ch->in_room->vnum == ROOM_VNUM_BURROW) */
	return FALSE;

    if (IS_NPC(victim) && is_affected(victim,gsn_mirror_image))
    {
	extract_char(victim,TRUE);
	return FALSE;
    }

    if (IS_NPC(victim) && is_affected(victim,gsn_decoy))
    {
      CHAR_DATA* vch = victim->summoner ? victim->summoner : victim;
	send_to_char("It's a trap!\n\r",ch);
	act("$n explodes, a powdery dust spills from it.",victim,NULL,NULL,TO_ALL);
	spell_poison(gsn_poison,victim->level + 3,vch,(void *) ch,TARGET_CHAR);
	spell_blindness(gsn_blindness,victim->level + 3,vch,(void *) ch,TARGET_CHAR);
	WAIT_STATE2(victim, 2 * PULSE_VIOLENCE);
	paralyze_effect(victim, victim->level, number_range(victim->level, 2 * victim->level), TARGET_CHAR);
	extract_char(victim,TRUE);
	return FALSE;
    }
//DRAW WEAPONS
    else if (ch != victim && ch->in_room == victim->in_room && set_combat){
      OBJ_DATA* wield, *second;

      wield = get_eq_char(ch,WEAR_WIELD);
      second = get_eq_char(ch,WEAR_SECONDARY);
      do_combatdraw(ch, wield, second);

      wield = get_eq_char(victim,WEAR_WIELD);
      second = get_eq_char(victim,WEAR_SECONDARY);
      do_combatdraw(victim, wield, second);

      /* set the right dam. message for attacker */

    }
    if ( victim != ch && ch->in_room == victim->in_room)
      {
	if ( ch->position > POS_STUNNED && set_combat)
	  {
	    if ( ch->fighting == NULL )
	      set_fighting( ch, victim);
	    if (ch->timer <= 4)
	      ch->position = POS_FIGHTING;
	  }

	if ( victim->position > POS_STUNNED && set_combat)
	{
	  if ( victim->fighting == NULL )
	    set_fighting( victim, ch );
	  if (victim->timer <= 4)
	    victim->position = POS_FIGHTING;
	}

//CHARMIE DEFENDER CHECK
	if ( ch->position > POS_STUNNED && set_combat)
        {
	    if ( ch->fighting == NULL )
		set_fighting( ch, victim );

	    if ( IS_NPC(ch)
		 && IS_NPC(victim)
		 && !IS_AFFECTED(ch,AFF_CHARM)
		 && IS_AFFECTED(victim, AFF_CHARM)
		 && ch->in_room == victim->in_room
		 && victim->master != NULL
		 && victim->master->in_room == ch->in_room
		 && victim->master != ch->fighting
		 && number_bits( 3 ) == 0 )
	    {
	      stop_fighting( ch, FALSE );
	      multi_hit( ch, victim->master, TYPE_UNDEFINED );
	      return FALSE;
	    }
	}
	if ( victim->master == ch && set_combat)
	  stop_follower( victim );
      }
//VISIBLE THE CH
    if (set_combat || ch->fighting == victim)
      do_visible(ch,NULL);
    do_visible(victim, "");

    /* adjust damage if object passed */
    if (obj){
      dam = UMAX(0, get_curr_cond( obj ) * dam / 100);
    }

    /* masochism check */
    if (ch != victim && dt >= TYPE_HIT && dam > 0 && set_combat){
      CHAR_DATA* och = masochism_effect(ch, dam, dt);
      if (och){
	victim = ch;
	ch = och;
	dt = gsn_masochism;
	dam /= 2;
	if (number_percent() < 10)
	  act("A bit of drool drips from $n's lips.", victim, NULL, victim, TO_ROOM);
      }
    }//End masochism check

//PREDAMAGE CHECK
    if (PREDAMAGE_CHECK(ch, victim, &dam, &dt, &dam_type) == -1)
      bug("Error at: PREDAMAGE_CHECK\n\r", 0);

    /* IMPORTANT, MISS can still be blocked, but damage marked as < 0 returned from (terra shield, etc..) should not*/
    if (dam < 0)
      return 0;

//Do Damage reduction
    dam = dam_spell(ch,victim,dam,dam_type);
    immune = FALSE;
/* we cheat here abit to allow normal weapon dmage text to be unblockable */
/* if we add TYPE_NOBLOCK onto the "dt" it will not be blocked, and will
   be subracted later
*/
    if ((dt >= TYPE_HIT
	 && dt < TYPE_NOBLOCKHIT
	 && dt < 5000
	 && (dt < 1065 || dt > 1070)
	 && ch != victim)
	|| (fVirt && dt >= TYPE_HIT && dt < TYPE_NOBLOCKHIT)){
      //Psionist body weaponry.
      if (is_affected(ch, gsn_body_weaponry)){
	dt = number_range(1028, 1030);
	dam_type = attack_table[dt - 1000].damage;
      }
//VAMPIRE WOLF.
      if (is_affected(victim, gsn_wolf_form)){
	if (wolf_evade(ch, victim, dt))
	  dam = 0;
	else if (check_dodge_wolf( ch, victim, obj, dt, h_roll, lvl, fVirt, sn))
	  return FALSE;
      }
//END WOLF DEFENSE.
      else{
	if (check_defenses(ch, victim, obj, dt, h_roll, lvl, fVirt, sn)){
	  CHAR_DATA* tmp = NULL;
	  if (ch == NULL || ch->fighting == NULL)
	    return FALSE;
	  if (dam > 0 && victim->class == gcn_blademaster)
	    tmp = bmDeathweaverCheck(ch, victim);
	  else if (dam > 0 && doomsingerCheck( ch )){
	    tmp = victim;
	    dam /= 8;
	  }
	  if (tmp)
	    victim = tmp;
	  else
	    return FALSE;
	}
	if (ch == NULL || ch->fighting == NULL)
	  return FALSE;
      }//END ELSE FROM WOLF.

      if (IS_AFFECTED2(victim,AFF_BLUR)
	  && ch != victim
	  && !(dam_type == DAM_NONE || dam_type == DAM_INTERNAL)
	  && number_percent() < 15 + (get_curr_stat(victim,STAT_LUCK) - 14)){
	if (is_affected(victim, gsn_forestmeld)){
	  if (victim->in_room->sector_type == SECT_FOREST){
	    act( "$n swings wildly and misses you by a mile.", ch, NULL, victim, TO_VICT    );
	    act( "You swing right through $N's blurred image.", ch, NULL, victim, TO_CHAR    );
	    return FALSE;
	  }
	}
	else{
	  act( "$n swings wildly and misses you by a mile.", ch, NULL, victim, TO_VICT    );
	  act( "You swing right through $N's blurred image.", ch, NULL, victim, TO_CHAR    );
	  return FALSE;
	}
      }
//Holy Hands
      if (is_affected(victim, gen_holy_hands) && get_eq_char(victim, WEAR_WIELD) == NULL)
	if (holy_hands_evade(ch, victim, dt))
	  return FALSE;

    }//END DEFESE CHECKS

/* check for TYPE_NOBLOCKHIT */
    if (dt >= TYPE_NOBLOCKHIT)
      dt -= (TYPE_NOBLOCKHIT- TYPE_HIT);


    //MONK DAMAGE
    if (dam > 0 && !IS_NPC(ch) && dt >= 1053 && dt <= 1064)
      dam = dam_monk(ch,victim,dam,dt);

    /* check for torment bind Moloch (location = AFFL_LEVEL) */
    if ( (paf = affect_find(ch->affected, gsn_torment_bind)) == NULL
	 || paf->location != APPLY_AFFL_LEVEL){
      switch(check_immune(victim,dam_type, FALSE))
	{
	case(IS_IMMUNE):   immune = TRUE; dam = 0;      break;
	case(IS_RESISTANT):
	  /* high sight/gladiator hth form */
	  if (dt > TYPE_HIT
	      && (number_percent() < get_skill(ch, gsn_high_sight)
		  || is_affected(ch, gsn_enhanced_damage))
	      )
	    check_improve(ch, gsn_high_sight, TRUE, 1);
	  else
	    dam -= dam/3;
	  break;
	case(IS_NORMAL):
	  /* high sight */
	  if (dt > TYPE_HIT && number_percent() < get_skill(ch, gsn_high_sight)
	      && victim->vuln_flags != 0){
	    check_improve(ch, gsn_high_sight, TRUE, 1);
	    dam = 12 * dam / 10;
	  }
	  break;
	case(IS_VULNERABLE): 	        dam += dam/2; break;
	}
    }
    if (IS_STONED(victim)){
      dam = 0;
      immune = TRUE;
    }
    if ( IS_AFFECTED(victim, AFF_SLEEP) )
      {
     	REMOVE_BIT(ch->affected_by,AFF_SLEEP);
	affect_strip( ch,gsn_sleep );
	affect_strip( ch,gsn_hypnosis );
	affect_strip(ch,gsn_coffin);
	affect_strip(ch,gsn_entomb);
	song_strip(ch,gsn_lullaby);
      }
    if ( is_affected(victim, gsn_camp)){
      send_to_char( "You feel drained from deprivation of sleep.\n\r", victim);
      if (!is_affected(victim, gsn_drained)){
	AFFECT_DATA af;
	af.where		= TO_AFFECTS;
	af.type             = gsn_drained;
	af.level            = victim->level;
	af.duration         = 10;
	af.location         = APPLY_NONE;
	af.modifier         = 0;
	af.bitvector        = 0;
	affect_to_char(victim ,&af);
	affect_strip(victim, gsn_camp);
      }
    }
    if (is_affected(victim, gsn_coffin))
      affect_strip(victim, gsn_coffin);
    if (is_affected(victim, gsn_burrow))
      do_unburrow(victim, "");
    if (victim->position >= POS_MEDITATE
	&& victim->position <= POS_SITTING){
      send_to_char("The pain gets you to your feet.\n\r", victim);
      if (!IS_AWAKE(ch)){
	victim->position = POS_STANDING;
	dreamprobe_cast(ch, NULL );
      }
      else
	victim->position = POS_STANDING;
    }

// Ogre ignore check
    if (dam_type != DAM_INTERNAL && ogre_ignore(ch,victim, obj, dam,  dt, fVirt))
      dam = 5;
    if (dam_type != DAM_INTERNAL && jugger_ignore(ch,victim, obj, dam,  dt, fVirt))
      dam = -1;

    /* Dont show damage if done to self and sn = -1 */
    if (sn != -1 || dam < 0){
      /* we show a fake splitting if this is dblow and it might kill */
      if (dt == gsn_forms && victim->hit + 20 < dam){
	act("$n cleaves $N `1IN HALF``!!", ch, NULL, victim, TO_NOTVICT);
	act("You cleave $N `1IN HALF``!!", ch, NULL, victim, TO_CHAR);
	act_new( "$n cleaves you `1IN HALF``!!", ch,NULL,victim,TO_VICT,POS_DEAD);
      }
      else if (dt == gsn_behead && victim->hit + 20 < dam){
	act("$n's head flies off as $N cleaves $S neck in twain.",
	    victim, NULL, ch, TO_ROOM);
	act("The last thing you feel is $n's blade bite into your neck.",
	    ch, NULL, victim, TO_VICT);
	act("You have been `1BEHEADED``!!.", ch, NULL, victim, TO_VICT);
	act("$N has been `1BEHEADED``!!.", victim, NULL, victim, TO_ROOM);
      }
      else{
	int dam_show = dam;
	if (!IS_NPC(victim)){
	  if (is_affected(victim, gsn_numbness))
	    dam_show = 2 * dam / 3;
	  if (is_affected(victim, gsn_nerve_amp))
	    dam_show = 3 * dam / 2;
	}
	dam_message( ch, victim, obj, dam_show, dt, immune, fVirt, sn, set_combat );
      }
    }

//Mist form revert. done here as we want mist to revert after the whole round.
    if (is_affected(victim, gsn_mist_form))
      do_revert(victim, NULL);

//Return if immune and such.
    if (dam <= 0)
	return FALSE;

//Loose hp
    victim->hit -= dam;

    if (ch == NULL || victim == NULL)
	return FALSE;

    if (tarot_find(victim->affected) == 12 && tarot_find(ch->affected) != 12 && ch != victim && number_percent() > 50)
	damage(ch,ch,dam,dt,dam_type, set_combat);


//Check for ecstacy
    if (is_affected(victim,gsn_ecstacy))
    {
	send_to_char("You snap back into reality.\n\r",victim);
	affect_strip(victim,gsn_ecstacy);
    }


//Immortal HP check (MASTERS CAN DIE TO MOBS AND MASTERS)
    if ( victim->hit < 1
	 && !IS_NPC(victim)
	 && victim->level >= LEVEL_IMMORTAL){
      if ((!IS_NPC(ch) && ch != victim && ch->level < MASTER) || victim->level > MASTER)
	victim->hit = 1;
    }
//Illithid HP drain save.
    else if (victim->hit < 1 && !IS_NPC(victim) && victim->race == race_lookup("illithid") && !is_affected(victim,gsn_drained))
    {
	AFFECT_DATA iaf;
	iaf.where	 = TO_AFFECTS;
        iaf.type         = gsn_drained;
        iaf.level        = victim->level;
        iaf.duration     = 12;
        iaf.modifier     = 0;
        iaf.location     = 0;
        iaf.bitvector    = 0;
        affect_to_char(victim,&iaf);
	act("You enter a state of shock, and you redirect your energy to sustain your body.",victim,NULL,NULL,TO_CHAR);
	act("$n enters a state of shock, and redirects $s energy to sustain $s body.",victim,NULL,NULL,TO_ROOM);
	victim->hit = victim->mana/3;
	victim->mana = 0;
    }
//Tarto save
    else if (victim->hit < 1 && !IS_NPC(victim) && tarot_find(victim->affected) == 21)
    {
	affect_strip(victim,gsn_tarot);
	victim->hit = 1;
	send_to_char("You were meant to die but somehow you cheat death!\n\r",victim);
	act("$n was supposed to die but somehow cheats death!",victim,NULL,NULL,TO_ROOM);
	stop_fighting( victim, TRUE );
	return dam;
    }
    else if (IS_NPC(victim) && victim->hit < 1 && victim->pIndexData->vnum == MOB_VNUM_SERPENT_U3 && victim->level >= 50)
    {
	victim->hit = victim->max_hit / 2;
	send_to_char("You were meant to die but somehow you cheat death!\n\r",victim);
	act("$n was supposed to die but somehow cheats death!",victim,NULL,NULL,TO_ROOM);
	stop_fighting( victim, TRUE );
	victim->level --;
	return dam;
    }
//Vicarious redemption
    else if (victim->hit < 2 && !IS_NPC(victim) && is_affected(victim,gsn_vredem)){
      ROOM_INDEX_DATA *location;
      location = get_room_index(get_temple(victim));
      if (location != NULL  && !is_affected(victim,gsn_damnation)
	  && !IS_SET(victim->act,PLR_OUTCAST)){
	act("A nearby peasant says '`#Quick! Devout father $n is in trouble!``'.",
	    victim, NULL,NULL,TO_ALL);
	act("As your vision fades you feel yourself being dragged away.",
	    victim,NULL,NULL,TO_CHAR);
	act("A swarm of The One God's followers snatches $n's body and carries it away.",
	    victim,NULL,victim,TO_ROOM);
	victim->hit = victim->mana/3;
	victim->mana = 0;
	if (victim->fighting != NULL){
	  if ( IS_NPC(victim->fighting)
	       && !IS_SET(victim->fighting->act2, ACT_LAWFUL)
	       && !IS_SET(victim->fighting->act, ACT_RAIDER)
	       && !IS_SET(victim->fighting->off_flags,CABAL_GUARD)){
            if (!IS_NPC(victim))
	      victim->fighting->hunting = victim;
            else if (victim->master != NULL)
	      victim->fighting->hunting = victim->master;
            else
	      victim->fighting->hunting = victim;
            victim->fighting->hunttime = 0;
	  }
	  stop_fighting(victim,TRUE);
	}//end if fighting
	victim->move /= 2;
	if (!IS_NPC(victim) && victim->pcdata->pStallion != NULL ){
	  victim->pcdata->pStallion = NULL;
	}
	char_from_room(victim);
	char_to_room(victim,location);
	mercy_effect(ch, victim, 1);
	act("A swarm of The One God's followers drags $n's body into the room.", victim,NULL,victim,TO_ROOM);
	act("A nearby peasant says '`#We did what we could kids, lets go.``'.",	    victim, NULL,NULL,TO_ALL);
	affect_strip(victim, gsn_vredem);
	act_new(skill_table[gsn_vredem].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
      }
    }
    else if (victim->hit < 2
	     && !IS_NPC(victim)
	     && number_percent() < get_skill(victim, gsn_adrenaline_rush) / 3
	     && !is_affected(victim, gsn_drained)){
      AFFECT_DATA iaf;

      send_to_char("A potent rush of adrenaline floods your veins!\n\r", ch);
      act("$n's eyes bulge out in their sockets as a rush of adrenaline revives $m!", victim, NULL, NULL, TO_ROOM);

      iaf.where	 = TO_AFFECTS;
      iaf.type         = gsn_drained;
      iaf.level        = victim->level;
      iaf.duration     = 12;
      iaf.modifier     = 0;
      iaf.location     = 0;
      iaf.bitvector    = 0;
      affect_to_char(victim,&iaf);

      victim->hit = victim->max_hit / 4;
      victim->mana = 0;
    }
    else
      victim = knight_rescue(ch, victim, dam, dt);
//Update state
    dam_update(ch,victim, dam);

    /* Viri: dam_dead should do this
//Stop combat if knocked out etc.
    if ( !IS_AWAKE(victim))
	stop_fighting( victim, FALSE );
    */

//Check if we kill him.
    if ( victim->position == POS_DEAD )
    {
      dam_dead(ch,victim);
      return dam;
    }

//POSTEDAMAGE CHECK
    if (POSTDAMAGE_CHECK(ch, victim, obj, dam, dt, dam_type) == -1)
      bug("Error at: POSTDAMAGE_CHECK\n\r", 0);

    if ( victim == ch )
	return dam;

//Wimpy etc.
    dam_after(ch,victim,dam);
    tail_chain( );


    return dam;
}//END VIRTUAL DAMAGE


int damage(CHAR_DATA *ch,CHAR_DATA *victim,int dam,int dt,int dam_type, bool show)
{
  /* we do a bit of screwing around here, since the "show" flag enedup being used elsewhere,
so, we use last argument (sn) when the int damage show is false, and victim = null, we make ch = vitim
and the sn = -1 which skips all damage messages. (used in mobprogs) All this mess needs to eb fixed
when we rewrite fight.c */
  if (!victim && !show)
    return virtual_damage(ch, ch, NULL, dam, dt, dam_type, GET_HITROLL(ch), ch->level, show, FALSE, -1) ;
  else
    return virtual_damage(ch, victim, NULL, dam, dt, dam_type, GET_HITROLL(ch), ch->level, show, FALSE, 0) ;
}

/* used in is_safe checks */
inline bool check_mercy_safe( CHAR_DATA* ch, CHAR_DATA* victim, bool fSilent){
  if (ch->in_room
      && IS_ROOM2(ch->in_room, ROOM_MERCY)
      && is_affected(victim, gsn_mortally_wounded)){
    if (!fSilent)
      send_to_char("They are already mortally wounded!\n\r", ch);
    return TRUE;
  }
  else if (IS_GAME(ch, GAME_MERCY)
	   && is_affected(victim, gsn_mortally_wounded)){
    if (!fSilent)
      send_to_char("They are already mortally wounded!\n\r", ch);
    return TRUE;
  }
  else
    return FALSE;
}

bool is_area_safe_quiet(CHAR_DATA *ch, CHAR_DATA *victim)
{
  if (victim->in_room == NULL || ch->in_room == NULL)
    return TRUE;
  else if (victim == ch )
    return TRUE;
  else if (victim->fighting == ch)
    return FALSE;
  else if (IS_NPC(ch) && IS_GOOD(ch) && IS_GOOD(victim))
    return TRUE;
  else if (IS_NPC(victim) && IS_SET(victim->act, ACT_NONCOMBAT)){
    return TRUE;
  }
/* Area attacks from mobs only affect those whom the mob is fighting */
  else if (IS_NPC(ch) && ch->fighting && !is_same_group(ch->fighting, victim))
    return TRUE;
  else if (IS_IMMORTAL(victim) && victim->invis_level > 2)
    return TRUE;
  else if (!IS_NPC(ch) && !IS_NPC(victim) && !IS_IMMORTAL(ch) && IS_IMMORTAL(victim))
    return TRUE;
  else if ( !IS_NPC(victim) && !is_fight_delay(victim,120) && victim->desc == NULL && !is_affected(ch,gsn_linked_mind))
    return TRUE;
  else if (is_ghost(victim,600) && !IS_SET(ch->off_flags, GUILD_GUARD) && !IS_SET(ch->off_flags, CABAL_GUARD))
    return TRUE;
  else if ( !IS_NPC(ch) && !IS_NPC(victim) && is_challenger(ch, victim) && challenge_safe(ch, victim))
    return TRUE;

  if ( IS_AFFECTED2(ch, AFF_HOLD) )
    return TRUE;

   /* killing mobiles */
    if (IS_NPC(victim))
    {
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	    return TRUE;
        if ( IS_SET(victim->act,ACT_PET) && victim->master == ch)
            return TRUE;
    	if (victim->pIndexData->vnum == MOB_VNUM_SHADOW && ch != victim->hunting)
	    return TRUE;
    	if (victim->pIndexData->vnum == MOB_VNUM_NEMESIS && ch != victim->hunting)
	    return TRUE;
        if ( victim->summoner == ch)
            return TRUE;
	if ( victim->master != NULL && !is_pk(ch,victim->master) )
            return TRUE;
	if (is_same_group(ch,victim))
            return TRUE;
    }
    /* killing players */
    else
    {
      AFFECT_DATA* paf;
      if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	return TRUE;
      if (check_mercy_safe(ch, victim, TRUE))
	return TRUE;
      if (IS_NPC(ch) && ch->master != NULL && !IS_NPC(ch->master) && !is_pk(ch->master,victim))
	return TRUE;
      if (IS_NPC(ch))
	return FALSE;
      if (is_same_group(ch,victim))
	return TRUE;
      if (!is_pk(ch,victim))
	return TRUE;
      else if ( (paf = affect_find(victim->affected, gen_diplomacy)) != NULL && paf->modifier == TRUE)
	return TRUE;
    }
    if (!IS_NPC(ch))
        ch->pcdata->ghost = (time_t)NULL;
    return FALSE;
}


bool is_safe_quiet_all(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
	return TRUE;
    else if (victim == ch )
	return TRUE;
    else if (victim->fighting == ch)
	return FALSE;
    else if (IS_NPC(ch) && IS_GOOD(ch) && IS_GOOD(victim))
	return TRUE;
    else if (IS_NPC(victim) && IS_SET(victim->act, ACT_NONCOMBAT)){
      return TRUE;
    }
    else if (IS_IMMORTAL(victim) && victim->invis_level > 2)
	return TRUE;
    else if (IS_IMMORTAL(ch) && IS_IMMORTAL(victim) && victim->level > LEVEL_IMMORTAL && !is_same_group(ch,victim))
      return FALSE;
    else if ( !IS_NPC(victim) && !is_fight_delay(victim,120) && victim->desc == NULL && !is_affected(ch,gsn_linked_mind))
      return TRUE;
    else if (is_ghost(victim,600) && !IS_SET(ch->off_flags, GUILD_GUARD) && !IS_SET(ch->off_flags, CABAL_GUARD))
      return TRUE;
    else  if ( !IS_NPC(ch) && !IS_NPC(victim) && is_challenger(ch, victim)
	       && challenge_safe(ch, victim))
      return TRUE;
    else if ( IS_AFFECTED2(ch, AFF_HOLD) )
      return TRUE;

    /* killing mobiles */
    if (IS_NPC(victim))
    {
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	    return TRUE;
        if ( IS_SET(victim->act,ACT_PET) && victim->master == ch)
            return TRUE;
    	if (victim->pIndexData->vnum == MOB_VNUM_SHADOW && ch != victim->hunting)
	    return TRUE;
    	if (victim->pIndexData->vnum == MOB_VNUM_NEMESIS && ch != victim->hunting)
	    return TRUE;
        if ( victim->summoner == ch)
            return TRUE;
	if ( victim->master != NULL && !is_pk(ch,victim->master) )
            return TRUE;
	if (is_same_group(ch,victim))
            return TRUE;
    }
    /* killing players */
    else
    {
      AFFECT_DATA* paf;
        if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
            return TRUE;
	if (check_mercy_safe(ch, victim, TRUE))
	  return TRUE;
        if (IS_NPC(ch) && ch->master != NULL && !IS_NPC(ch->master) && !is_pk(ch->master,victim))
            return TRUE;
        if (IS_NPC(ch))
            return FALSE;
        if (!is_pk(ch,victim))
            return TRUE;
	else if ( (paf = affect_find(victim->affected, gen_diplomacy)) != NULL && paf->modifier == TRUE)
	  return TRUE;
    }
    if (!IS_NPC(ch))
        ch->pcdata->ghost = (time_t)NULL;
    return FALSE;
}


bool is_area_safe(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
      return TRUE;
    else if (victim == ch )
      return TRUE;
    else if (victim->fighting == ch)
      return FALSE;
    else if (IS_NPC(ch) && IS_GOOD(ch) && IS_GOOD(victim))
      return TRUE;
    else if (IS_NPC(victim) && IS_SET(victim->act, ACT_NONCOMBAT)){
      sendf(ch, "%s cannot be attacked this way.\n\r", PERS(victim, ch ));
      return TRUE;
    }
/* Area attacks from mobs only affect those whom the mob is fighting */
    else if (IS_NPC(ch) && ch->fighting && !is_same_group(ch->fighting, victim))
      return TRUE;
    else if (IS_IMMORTAL(victim) && victim->invis_level > 2)
      return TRUE;
    else if (IS_IMMORTAL(ch) && IS_IMMORTAL(victim) && victim->level > LEVEL_IMMORTAL && !is_same_group(ch,victim))
	return FALSE;
    else if (IS_IMMORTAL(ch) && victim->level < ch->level && !is_same_group(ch,victim))
	return FALSE;
    else if (!IS_NPC(ch) && !IS_NPC(victim) && !IS_IMMORTAL(ch) && IS_IMMORTAL(victim))
    {
	send_to_char("Do you honestly think you can kill a god?\n\r",ch);
	return TRUE;
    }
    else if ( !IS_NPC(victim) && !is_fight_delay(victim,120) && victim->desc == NULL && !is_affected(ch,gsn_linked_mind))
    {
	sendf(ch, "%s is lost.\n\r", PERS(victim,ch));
	return TRUE;
    }
    else if (is_ghost(victim,600) && !IS_SET(ch->off_flags, GUILD_GUARD) && !IS_SET(ch->off_flags, CABAL_GUARD))
    {
        send_to_char("You can't kill a ghost!\n\r",ch);
        return TRUE;
    }
    else if ( !IS_NPC(ch) && !IS_NPC(victim) && is_challenger(ch, victim)
	 && challenge_safe(ch, victim)){
      sendf(ch, "You can't attack %s yet.\n\r",PERS(victim,ch));
      return TRUE;
    }
    else if ( IS_AFFECTED2(ch, AFF_HOLD) )
    {
        send_to_char("You are barred from your agressive nature.\n\r",ch);
        return TRUE;
    }
   /* killing mobiles */
    else if (IS_NPC(victim))
    {
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
        {
	    send_to_char("Not in this room.\n\r",ch);
	    return TRUE;
	}
        if ( IS_SET(victim->act,ACT_PET) && victim->master == ch)
        {
            sendf(ch, "But %s looks so cute and cuddly...\n\r",PERS(victim,ch));
            return TRUE;
        }
    	if (victim->pIndexData->vnum == MOB_VNUM_SHADOW && ch != victim->hunting)
    	{
	    send_to_char("You can't attack that, it's just a shadow.\n\r",ch);
	    return TRUE;
    	}
    	if (victim->pIndexData->vnum == MOB_VNUM_NEMESIS && ch != victim->hunting)
    	{
	    send_to_char("You can't seem to harm it.\n\r",ch);
	    return TRUE;
    	}
        if ( victim->summoner == ch)
        {
            send_to_char("Why would you turn on your own creation?\n\r",ch);
            return TRUE;
        }
	if ( victim->master != NULL && !is_pk(ch,victim->master) )
        {
            sendf(ch,"The gods protect %s.\n\r",PERS(victim,ch));
            return TRUE;
        }
	if (is_same_group(ch,victim))
            return TRUE;
    }
    /* killing players */
    else
    {
      AFFECT_DATA* paf;
        if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
        {
            send_to_char("Not in this room.\n\r",ch);
            return TRUE;
        }
	if (check_mercy_safe(ch, victim, FALSE))
	  return TRUE;
        if (IS_NPC(ch) && ch->master != NULL && !IS_NPC(ch->master) && !is_pk(ch->master,victim))
            return TRUE;
        if (IS_NPC(ch))
            return FALSE;
	if (is_same_group(ch,victim))
            return TRUE;
        if (!is_pk(ch,victim))
        {
	    act("The gods protect $N from $n.",ch,NULL,victim, TO_ALL);
            return TRUE;
        }
	else if ( (paf = affect_find(victim->affected, gen_diplomacy)) != NULL && paf->modifier == TRUE){
	  act("$n's royal entourage protects $m from $N.", victim, NULL, ch, TO_NOTVICT);
	  act("$n's royal entourage protects you from $N.", victim, NULL, ch, TO_CHAR);
	  act("$n's royal entourage protects $m from you.", victim, NULL, ch, TO_VICT);
	  return TRUE;
	}
    }
    if (!IS_NPC(ch))
        ch->pcdata->ghost = (time_t)NULL;
    return FALSE;
}

bool is_safe(CHAR_DATA *ch, CHAR_DATA *victim)
{
  if (victim->in_room == NULL || ch->in_room == NULL)
    return TRUE;
  else if (victim == ch )
    return TRUE;
  else if (victim->fighting == ch)
    return FALSE;
  else if (IS_NPC(victim) && IS_SET(victim->act, ACT_NONCOMBAT)){
    sendf(ch, "%s cannot be attacked this way.\n\r", PERS(victim, ch ));
    return TRUE;
  }
  else if (!IS_NPC(ch) && IS_IMMORTAL(ch) && victim->level < ch->level )
    return FALSE;
  else if (IS_NPC(ch) && IS_GOOD(ch) && IS_GOOD(victim))
    return TRUE;
  else if (!IS_NPC(ch) && !IS_NPC(victim) && !IS_IMMORTAL(ch) && IS_IMMORTAL(victim))
    {
      if (can_see(ch,victim))
	send_to_char("Do you honestly think you can kill a god?\n\r",ch);
      return TRUE;
    }
  if ( !IS_NPC(victim)
       && !is_fight_delay(victim,80)
       && victim->desc == NULL
       && !is_affected(victim,gsn_linked_mind)
       && !(!IS_NPC(victim)
	    && (IS_SET(victim->act2,PLR_NOQUIT)
		|| is_affected(victim,gsn_noquit)
		|| is_affected(victim,gen_ensnare))))
    {
      sendf(ch,"%s is lost.\n\r",PERS(victim,ch));
      return TRUE;
    }
  if (is_ghost(victim,600) && !IS_SET(ch->off_flags, GUILD_GUARD) && !IS_SET(ch->off_flags, CABAL_GUARD))
    {
        send_to_char("You can't kill a ghost!\n\r",ch);
        return TRUE;
    }
    if ( !IS_NPC(ch) && !IS_NPC(victim) && challenge_safe(ch,victim) && is_challenger(ch,victim))
    {
	sendf(ch,"You can't attack %s yet.\n\r",PERS(victim,ch));
	return TRUE;
    }
    if ( IS_AFFECTED2(victim,AFF_SHADOWFORM) )
    {
        sendf(ch,"Your attacks seem to pass right through %s.\n\r",PERS(victim,ch));
        return TRUE;
    }
    if ( IS_AFFECTED2(victim,AFF_TREEFORM) )
    {
        sendf(ch,"You cannot seem to move to attack %s.\n\r",PERS(victim,ch));
        return TRUE;
    }
    if ( IS_AFFECTED2(ch, AFF_HOLD) )
    {
        send_to_char("You are barred from your agressive nature.\n\r",ch);
        return TRUE;
    }
    /* killing mobiles */
    if (IS_NPC(victim))
    {
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
        {
	    send_to_char("Not in this room.\n\r",ch);
	    return TRUE;
	}
    	if (victim->pIndexData->vnum == MOB_VNUM_SHADOW &&  ch != victim->hunting)
    	{
	    send_to_char("You can't attack that, it's just a shadow.\n\r",ch);
	    return TRUE;
    	}
    	if (victim->pIndexData->vnum == MOB_VNUM_NEMESIS && ch != victim->hunting)
    	{
	    send_to_char("You can't seem to harm it.\n\r",ch);
	    return TRUE;
    	}
        if ( victim->summoner == ch)
        {
            send_to_char("Why would you turn on your own creation?\n\r",ch);
            return TRUE;
        }
/* wanted people can attack special guards */
	if ( victim->master != NULL )
	{
	  if ( (victim->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD || victim->pIndexData->vnum == MOB_VNUM_HOUND)
		&& ((!IS_NPC(ch) && IS_SET(ch->act,PLR_WANTED))
		    || (IS_NPC(ch) && ch->master != NULL && IS_SET(ch->master->act,PLR_WANTED))))
		return FALSE;
	    if ((!IS_NPC(ch) && !is_pk(ch,victim->master)) || (IS_NPC(ch) && ch->master != NULL && !is_pk(ch->master,victim->master)))
	    {
	    	sendf(ch,"The gods protect %s.\n\r",PERS(victim,ch));
	    	return TRUE;
	    }
	}
    }
    /* killing players */
    else
    {
      AFFECT_DATA* paf;
        if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	  {
            send_to_char("Not in this room.\n\r",ch);
            return TRUE;
	  }
	if (check_mercy_safe(ch, victim, FALSE))
	  return TRUE;
        if (IS_NPC(ch) && ch->master != NULL && !IS_NPC(ch->master) && !is_pk(ch->master,victim))
            return TRUE;
        if (IS_NPC(ch))
	  return FALSE;
        if (!is_pk(ch,victim))
	  {
	    act("The gods protect $N from $n.",ch,NULL,victim, TO_ALL);
            return TRUE;
        }
	else if ( (paf = affect_find(victim->affected, gen_diplomacy)) != NULL && paf->modifier == TRUE){
	  act("$n's royal entourage protects $m from $N.", victim, NULL, ch, TO_NOTVICT);
	  act("$n's royal entourage protects you from $N.", victim, NULL, ch, TO_CHAR);
	  act("$n's royal entourage protects $m from you.", victim, NULL, ch, TO_VICT);
	  return TRUE;
	}
    }
    if (!IS_NPC(ch))
        ch->pcdata->ghost = (time_t)NULL;
    return FALSE;
}

bool is_safe_quiet(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
	return TRUE;
    else if (victim == ch )
	return TRUE;
    else if (victim->fighting == ch)
	return FALSE;
    else if (IS_NPC(victim) && IS_SET(victim->act, ACT_NONCOMBAT)){
      return TRUE;
    }
    else if (!IS_NPC(ch) && IS_IMMORTAL(ch) && victim->level < ch->level )
      return FALSE;
    else if (IS_NPC(ch) && IS_GOOD(ch) && IS_GOOD(victim))
      return TRUE;
    if ( !IS_NPC(victim) && !is_fight_delay(victim,80) && victim->desc == NULL && !is_affected(victim,gsn_linked_mind) && !(!IS_NPC(victim) && (IS_SET(victim->act2,PLR_NOQUIT)
	|| is_affected(victim,gsn_noquit) || is_affected(victim,gen_ensnare))))
      return TRUE;
    if (is_ghost(victim,600) && !IS_SET(ch->off_flags, GUILD_GUARD) && !IS_SET(ch->off_flags, CABAL_GUARD))
      return TRUE;
    if ( !IS_NPC(ch) && !IS_NPC(victim) && challenge_safe(ch,victim) && is_challenger(ch,victim))
      return TRUE;
    if ( IS_AFFECTED2(victim,AFF_SHADOWFORM) )
        return TRUE;
    if ( IS_AFFECTED(victim,AFF_TREEFORM) )
      return TRUE;
    if ( IS_AFFECTED2(ch, AFF_HOLD) )
      return TRUE;

    /* killing mobiles */
    if (IS_NPC(victim))
    {
	if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	    return TRUE;
	if (IS_NPC(victim) && IS_SET(victim->act, ACT_NONCOMBAT)){
	  return TRUE;
	}
    	if (victim->pIndexData->vnum == MOB_VNUM_SHADOW  && ch != victim->hunting)
	    return TRUE;
    	if (victim->pIndexData->vnum == MOB_VNUM_NEMESIS  && ch != victim->hunting)
	    return TRUE;
        if ( victim->summoner == ch)
            return TRUE;
	if ( victim->master != NULL && victim->in_room == victim->master->in_room)
	{
/* wanted people can attack the guard */
	    if ( (victim->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD || victim->pIndexData->vnum == MOB_VNUM_HOUND)
		&& ((!IS_NPC(ch) && IS_SET(ch->act,PLR_WANTED))
		    || (IS_NPC(ch) && ch->master != NULL && IS_SET(ch->master->act,PLR_WANTED))))
		return FALSE;
	    if ((!IS_NPC(ch) && !is_pk(ch,victim->master)) || (IS_NPC(ch) && ch->master != NULL && !is_pk(ch->master,victim->master)))
	    	return TRUE;
	}
    }
    /* killing players */
    else
    {
      AFFECT_DATA* paf;
      if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
	return TRUE;
      if (check_mercy_safe(ch, victim, FALSE))
	return TRUE;
        if (IS_NPC(ch) && ch->master != NULL && !IS_NPC(ch->master) && !is_pk(ch->master,victim))
            return TRUE;
        if (IS_NPC(ch))
            return FALSE;
        if (!is_pk(ch,victim))
            return TRUE;
	else if ( (paf = affect_find(victim->affected, gen_diplomacy)) != NULL && paf->modifier == TRUE){
	  return TRUE;
	}
    }
    if (!IS_NPC(ch))
        ch->pcdata->ghost = (time_t)NULL;
    return FALSE;
}

int get_dchance( int ini_skill, int cur_skill, int hitroll ){
  if (cur_skill <= ini_skill / 2)
    return cur_skill;
  else
    return ( UMAX(ini_skill / 2, cur_skill - hitroll));
}

bool check_parry_monk( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool Second, bool fVirt, int sn)
{
  AFFECT_DATA* paf;
  OBJ_DATA *obj;
  OBJ_DATA* obj2 = (vir_obj != NULL? vir_obj : NULL);
  int items = 0, chance, ch_weapon, vict_weapon, ch_vict_weapon, v_wtype, c_wtype = 0;
  int temp, ini_chance;
  char buf1[MSL], buf2[MSL], buf3[MSL], buf4[MSL];


//easy hits.
    if ( !IS_AWAKE(victim) || is_affected(victim,gsn_ecstacy))
	return FALSE;

    if (victim->class != class_lookup("monk"))
      return FALSE;

//get which parry is possible.
    if (get_eq_char( victim, WEAR_WIELD ) != NULL )
	return FALSE;
    if (get_eq_char( victim, WEAR_SHIELD ) != NULL )
	items++;

    obj = get_eq_char( victim, WEAR_HOLD);

    if ( obj != NULL && obj->pIndexData->vnum != OBJ_VNUM_PRAYER_BEADS)
	items++;

    if (Second)
    {
      if (items > 0
	  || !is_empowered_quiet(victim,1)
	  || ((paf = affect_find(victim->affected, gsn_wlock)) && paf->modifier != 0)
	  || (chance = get_skill(victim,gsn_second_parry) / 3) == 0)
	return FALSE;
    }
    else
    {
      if (items > 1 || (chance = get_skill(victim,gsn_first_parry) / 2) == 0)
	return FALSE;
    }
    ini_chance = chance;

//bonus/penalty for blind.
  //we only check if character can see if this is not virtual attack.
    if (!can_see(ch,victim) && !fVirt)
    {
      if (!IS_NPC(ch) && number_percent() < get_skill(ch,gsn_blind_fighting)){
	if (!is_affected(ch, gsn_battletrance))
	  chance *= 1.1;
      }
      else
	chance *= 1.3;
    }

    if (!can_see(victim,ch))
    {
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	if (!is_affected(victim, gsn_battletrance))
	  chance /= 1.1;
      }
      else
	chance /= 1.3;
    }

/* crusade effects */
    if ( (temp = check_crusade(ch, victim)) != CRUSADE_NONE){
      /* if attacker is on crusade against victim */
      if (temp == CRUSADE_MATCH)
	chance /= 1.3;
      /* if attacker is on crusade but not agaisnt victim */
      else if (temp == CRUSADE_NOTMATCH)
	chance *= 1.1;
    }
    if ( (temp = check_crusade(victim, ch)) != CRUSADE_NONE){
      /* if victim is on crusade against attacker */
      if (temp == CRUSADE_MATCH)
	chance *= 1.3;
      /* if victim is on crusade but not against attacker */
      else if (temp == CRUSADE_NOTMATCH)
	chance /= 1.1;
    }
/* end crusade bonuses */

/* check weapon position */
    if (!IS_NPC(ch)){
      /* harder to block for victim if attacker is high */
      if (ch->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 10;
      /* easier to block for victim if attacker is low */
      else if (ch->pcdata->wep_pos == WEPPOS_LOW)
	chance += 5;
    }
    if (!IS_NPC(victim)){
      /* harder to block for victim if victim is high */
      if (victim->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 5;
      /* easier to block for victim if victim is low */
      else if (victim->pcdata->wep_pos == WEPPOS_LOW)
	chance += 10;
    }


    //we use the held item if this is not virtual attack.
    if (!fVirt)
      obj2 = get_eq_char(ch,WEAR_WIELD);

//weapons skills.
    if (!IS_NPC(ch))
    {
    	ch_weapon = (fVirt? get_skill(ch, sn) : get_weapon_skill_obj(ch,obj2));
	ch_vict_weapon = (fVirt? 2 * victim->level : get_weapon_skill_obj(victim, obj2));
    }
    else
    {
        ch_weapon = 100;
        ch_vict_weapon = (fVirt? 2 * victim->level : get_weapon_skill_obj(victim,obj2));
    }
    vict_weapon = get_weapon_skill_obj(victim, obj);


    //DEFENDER'S WEAPON
    v_wtype = get_weapon_obj_rating( NULL, WEAPON_DEF );

    //ATTACKER'S WEAPON
    if (fVirt){
      if (dt == gen_blades || dt == gen_dan_blade)
	c_wtype = URANGE(-2, (ch_weapon - 80) / 5, 5);
      else
	c_wtype = 0;
    }
    else if ( ch->class == gcn_psi
	      && (paf = affect_find( ch->affected, gsn_false_weapon)) != NULL)
      c_wtype = get_weapon_rating( paf->modifier, WEAPON_ATT );
    else
      c_wtype = get_weapon_obj_rating( obj2, WEAPON_ATT );


//Calcualte chance
    chance = chance * vict_weapon / 100;
    if (Second)
        chance += (v_wtype - c_wtype) * 4;
    else
	chance += (v_wtype - c_wtype) * 3;

    chance += (ch_vict_weapon - ch_weapon) / 5;
    chance += get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX);
    chance += get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR);
    chance += get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK);
    if (!IS_NPC(ch)){
      chance = get_dchance(ini_chance, chance, (h_roll - victim->level/5)/3);
    }
    else
      chance -= h_roll/4;
    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL && number_percent() < get_skill(ch, gsn_mounted)){
      chance -= 3;
      check_improve(ch, gsn_mounted, TRUE, 1);
    }
    if (IS_NPC(ch) && IS_SET(ch->off_flags, CABAL_GUARD))
	chance -= 10;
    else
        chance += victim->level - level;
    if (!IS_NPC(victim) && IS_SET(victim->act2,PLR_GIMP))
	chance /= 2;

//Check for success
    if ( number_percent( ) >= chance)
	return FALSE;

//show text
    if (Second)
	sprintf(buf3, "second");
    else
	sprintf(buf3, "first");

    if (fVirt)
    {
	/* We select two types of origin of the virual attack.
	   1) obj
	   2) spell
	*/

        sprintf(buf1, "You parry a %s's %s with your %s hand.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt), buf3);
        sprintf(buf2, "$N parries %s's %s with $S %s hand.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt), buf3);
        sprintf(buf4, "$N parries a %s's %s with $S %s hand.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt), buf3);
    }
    else
    {
        sprintf(buf1, "You parry $n's attack with your %s hand.", buf3);
        sprintf(buf2, "$N parries your attack with $S %s hand.", buf3);
        sprintf(buf4, "$N parries $n's attack with $S %s hand.", buf3);
    }
    if (!doomsingerCheck(ch)){
      act( buf1, ch, vir_obj, victim, TO_VICT );
      act( buf2, ch, vir_obj, victim, TO_CHAR );
      act( buf4, ch, vir_obj, victim, TO_WATCHROOM);
    }

    if (Second){
      check_improve(victim,gsn_second_parry,TRUE, 0);
      if (ch && victim && ch->fighting == victim
	  && check_ccombat_actions( victim, ch, CCOMBAT_DPARRY))
	return TRUE;
    }
    else
        check_improve(victim,gsn_first_parry,TRUE, 0);
    return TRUE;
}


bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn)
{
    OBJ_DATA *obj = NULL;
    OBJ_DATA* obj2 = (vir_obj!= NULL? vir_obj : NULL);
    int chance, ini_chance, ch_weapon, vict_weapon, ch_vict_weapon, c_wtype = 0, v_wtype = 0;
    int temp = 0;
    char buf1[MSL], buf2[MSL], buf3[MSL];
    AFFECT_DATA* paf;
    bool fIll = FALSE;

//easy hits.
    if ( !IS_AWAKE(victim) || is_affected(victim,gsn_ecstacy))
	return FALSE;

//skill check
    if ((chance = get_skill(victim,gsn_parry) / 2) == 0)
    	return FALSE;
    if ( (obj = getBmWep( victim, TRUE ) ) == NULL ){
      fIll = victim->race == grn_illithid;

      if (IS_NPC(victim))
	chance /= 2;
      else if ( !fIll
		&& !is_affected(victim, gsn_body_weaponry) )
	return FALSE;
    }
    ini_chance = chance;

    /* weapon raked away */
    if ( ( paf = affect_find(ch->affected,gsn_rake)) != NULL
	 && paf->modifier == 1)
      return FALSE;

//bonus/penalty for blind.
  //we only check if character can see if this is not virtual attack.
    if (!can_see(ch,victim) && !fVirt)
    {
      if (!IS_NPC(ch) && number_percent() < get_skill(ch,gsn_blind_fighting)){
	if (!is_affected(ch, gsn_battletrance))
            chance *= 1.1;
      }
        else
            chance *= 1.3;
    }

    if (!can_see(victim,ch))
    {
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	if (!is_affected(victim, gsn_battletrance))
	  chance /= 1.1;
      }
      else
	chance /= 1.3;
    }

/* crusade effects */
    if ( (temp = check_crusade(ch, victim)) != CRUSADE_NONE){
      /* if attacker is on crusade against victim */
      if (temp == CRUSADE_MATCH)
	chance /= 1.3;
      /* if attacker is on crusade but not agaisnt victim */
      else if (temp == CRUSADE_NOTMATCH)
	chance *= 1.1;
    }
    if ( (temp = check_crusade(victim, ch)) != CRUSADE_NONE){
      /* if victim is on crusade against attacker */
      if (temp == CRUSADE_MATCH)
	chance *= 1.3;
      /* if victim is on crusade but not against attacker */
      else if (temp == CRUSADE_NOTMATCH)
	chance /= 1.1;
    }
/* end crusade bonuses */

/* check weapon position */
    if (!IS_NPC(ch)){
      /* harder to block for victim if attacker is high */
      if (ch->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 10;
      /* easier to block for victim if attacker is low */
      else if (ch->pcdata->wep_pos == WEPPOS_LOW)
	chance += 5;
    }
    if (!IS_NPC(victim)){
      /* harder to block for victim if victim is high */
      if (victim->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 5;
      /* easier to block for victim if victim is low */
      else if (victim->pcdata->wep_pos == WEPPOS_LOW)
	chance += 10;
    }

//we use the held item if this is not virtual attack.
    if (!fVirt)
      obj2 = getBmWep(ch,TRUE);

//weapons skills.
  if (!IS_NPC(ch))
    {
      ch_weapon = (fVirt? get_skill(ch, sn) : get_weapon_skill_obj(ch,obj2));
      ch_vict_weapon = (fVirt? 2 * victim->level : get_weapon_skill_obj(victim,obj2));
    }
  else
    {
      ch_weapon = 100;
      ch_vict_weapon = (fVirt? 2 * victim->level : get_weapon_skill_obj(victim,obj2));
    }
    vict_weapon = get_weapon_skill_obj(victim, obj);


    //DEFENDER'S WEAPON MODIFIER
    v_wtype = get_weapon_obj_rating( obj, WEAPON_DEF );

    //ATTACKER'S WEAPON MODIFIER
    if (fVirt){
      if (dt == gen_blades || dt == gen_dan_blade)
	c_wtype = URANGE(-2, (ch_weapon - 80) / 5, 5);
      else
	c_wtype = 0;
    }
    else if ( ch->class == gcn_psi
	      && (paf = affect_find( ch->affected, gsn_false_weapon)) != NULL)
      c_wtype = get_weapon_rating( paf->modifier, WEAPON_ATT );
    else
      c_wtype = get_weapon_obj_rating( obj2, WEAPON_ATT );

    chance = chance * vict_weapon / 100;
    chance += (v_wtype - c_wtype) * 3;
    chance += (ch_vict_weapon - ch_weapon) / 5;
    chance += get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX);
    chance += get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR);
    chance += get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK);

    /* DEBUG
    sendf(ch, "`#Char Weapon skill: `!%d``\n\r"\
	  "`#Vic Weapon skill: `!%d``\n\r"\
	  "`#Vict with char Weapon skill: `!%d``\n\r"\
	  "`#Chance: `!%d``\n\r"\
	  ,ch_weapon, vict_weapon, ch_vict_weapon, chance);
    */
    if (!IS_NPC(ch)){
      chance = get_dchance(ini_chance, chance, (h_roll - victim->level/5)/3);
    }
    else
    	chance -= h_roll/4;
    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL && number_percent() < get_skill(ch, gsn_mounted)){
      chance -= 3;
      check_improve(ch, gsn_mounted, TRUE, 1);
    }
    if (IS_NPC(ch) && IS_SET(ch->off_flags, CABAL_GUARD))
	chance -= 15;
    else
        chance += victim->level - level;

    if (!IS_NPC(victim) && IS_SET(victim->act2,PLR_GIMP))
	chance /= 2;

//Check for success
    if ( number_percent( ) >= chance)
	return FALSE;

    if (fVirt)
    {
	/* We select two types of origin of the virual attack.
	   1) obj
	   2) spell
	*/

        sprintf(buf1, "You parry a %s's %s.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));

        sprintf(buf2, "$N parries a %s's %s.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));

        sprintf(buf3, "$N parries a %s's %s.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));
    }
    else
    {
        sprintf(buf1, "You parry $n's attack.");
        sprintf(buf2, "$N parries your attack.");
        sprintf(buf3, "$N parries $n's attack.");
    }
    if (!doomsingerCheck(ch)){
      act( buf1, ch, vir_obj, victim, TO_VICT );
      act( buf2, ch, vir_obj, victim, TO_CHAR );
      act( buf3, ch, vir_obj, victim, TO_WATCHROOM);
    }
    check_improve(victim,gsn_parry,TRUE, 0);

//PARRY CHECK
      if (PARRY_CHECK(ch, obj, obj2, dt) == -1)
	bug("Error at: PARRY_CHECK\n\r", 0);

    return TRUE;
}

bool check_dual_parry( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn)
{
    OBJ_DATA *obj = NULL;
    OBJ_DATA* obj2 = (vir_obj != NULL? vir_obj : NULL);
    int chance, ini_chance, ch_weapon, vict_weapon, ch_vict_weapon, c_wtype = 0, v_wtype = 0;
    int temp = 0;
    char buf1[MSL], buf2[MSL], buf3[MSL];
    AFFECT_DATA* paf;

//easy hits.
    if ( !IS_AWAKE(victim)
	 || is_affected(victim,gsn_ecstacy)
	 || ((paf = affect_find(victim->affected, gsn_wlock)) && paf->modifier != 0)
	 )
      return FALSE;

//skill
    chance = get_skill(victim,gsn_dual_parry) / 3;
    ini_chance = chance;
    /* Useless conditional */
    if( ini_chance != chance )
        ini_chance = chance;

    //Obj check
    if ( get_eq_char(victim, WEAR_SECONDARY) == NULL || ( obj = getBmWep( victim, FALSE ) ) == NULL){
      if (IS_NPC(victim) && get_eq_char( victim, WEAR_SHIELD ) == NULL)
	chance /= 2;
      else
	return FALSE;
    }

//Item check
    if (obj && CAN_WEAR(obj, ITEM_PARRY)){
      if (chance == 0)
	chance = 15;
      else
	chance += 5;
    }
    if (chance < 1)
      return FALSE;
    if (obj && obj->value[0] == WEAPON_DAGGER)
      chance += 5;

/* weapon raked away */
    if ( ( paf = affect_find(ch->affected,gsn_rake)) != NULL
	 && paf->modifier == 0)
      return FALSE;

//bonus/penalty for blind.
  //we only check if character can see if this is not virtual attack.
    if (!can_see(ch,victim) && !fVirt)
    {
      if (!IS_NPC(ch) && number_percent() < get_skill(ch,gsn_blind_fighting)){
	if (!is_affected(ch, gsn_battletrance))
	  chance *= 1.1;
      }
      else
	chance *= 1.3;
    }

    if (!can_see(victim,ch))
    {
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	if (!is_affected(victim, gsn_battletrance))
	  chance /= 1.1;
      }
      else
	chance /= 1.3;
    }

/* crusade effects */
    if ( (temp = check_crusade(ch, victim)) != CRUSADE_NONE){
      /* if attacker is on crusade against victim */
      if (temp == CRUSADE_MATCH)
	chance /= 1.3;
      /* if attacker is on crusade but not agaisnt victim */
      else if (temp == CRUSADE_NOTMATCH)
	chance *= 1.1;
    }
    if ( (temp = check_crusade(victim, ch)) != CRUSADE_NONE){
      /* if victim is on crusade against attacker */
      if (temp == CRUSADE_MATCH)
	chance *= 1.3;
      /* if victim is on crusade but not against attacker */
      else if (temp == CRUSADE_NOTMATCH)
	chance /= 1.1;
    }
/* end crusade bonuses */

/* check weapon position */
    if (!IS_NPC(ch)){
      /* harder to block for victim if attacker is high */
      if (ch->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 10;
      /* easier to block for victim if attacker is low */
      else if (ch->pcdata->wep_pos == WEPPOS_LOW)
	chance += 5;
    }
    if (!IS_NPC(victim)){
      /* harder to block for victim if victim is high */
      if (victim->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 5;
      /* easier to block for victim if victim is low */
      else if (victim->pcdata->wep_pos == WEPPOS_LOW)
	chance += 10;
    }


    //we use the held item if this is not virtual attack.
    if (!fVirt)
      obj2 = getBmWep(ch, TRUE);

//weapons skills.
    if (!IS_NPC(ch)){
      ch_weapon = (fVirt? get_skill(ch, sn) : get_weapon_skill_obj(ch, obj2));
      ch_vict_weapon = (fVirt? 2 * victim->level : get_weapon_skill_obj(victim, obj2));
    }
    else{
      ch_weapon = 100;
      ch_vict_weapon = (fVirt? 2 * victim->level : get_weapon_skill_obj(victim, obj2));
    }
    vict_weapon = get_weapon_skill_obj(victim, obj);

    //DEFENDER'S WEAPON MODIFIER (penalty to defense due to dual parry)
    v_wtype = UMAX(-5, get_weapon_obj_rating( obj, WEAPON_DEF ) - 2);

    //ATTACKER'S WEAPON MODIFIER
    if (fVirt){
      if (dt == gen_blades || dt == gen_dan_blade)
	c_wtype = URANGE(-2, (ch_weapon - 80) / 5, 5);
      else
	c_wtype = 0;
    }
    else if ( ch->class == gcn_psi
	      && (paf = affect_find( ch->affected, gsn_false_weapon)) != NULL)
      c_wtype = get_weapon_rating( paf->modifier, WEAPON_ATT );
    else
      c_wtype = get_weapon_obj_rating( obj2, WEAPON_ATT );

    chance = chance * vict_weapon/100;
    chance += (v_wtype - c_wtype) * 4;
    chance += (ch_vict_weapon - ch_weapon) / 5;
    chance += get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX);
    chance += get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR);
    chance += get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK);

    /* Viri: make dual parry very affected by hitroll and blind */
    if (!IS_NPC(ch)){
      chance -= (h_roll - victim->level/5) /  2;
    }
    else
    	chance -= h_roll/4;
    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL && number_percent() < get_skill(ch, gsn_mounted)){
      chance -= 3;
      check_improve(ch, gsn_mounted, TRUE, 1);
    }
    if (IS_NPC(ch) && IS_SET(ch->off_flags, CABAL_GUARD))
	chance -= 10;
    else
        chance += victim->level - level;
    if (!IS_NPC(victim) && IS_SET(victim->act2,PLR_GIMP))
	chance /= 2;

//Check for success
    if ( number_percent( ) >= chance)
	return FALSE;

    if (fVirt)
    {
	/* We select two types of origin of the virual attack.
	   1) obj
	   2) spell
	*/

        sprintf(buf1, "You dual parry a %s's %s.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));

        sprintf(buf2, "$N dual parries a %s's %s.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));

        sprintf(buf3, "$N dual parries a %s's %s.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));
    }
    else
    {
        sprintf(buf1, "You dual parry $n's attack.");
        sprintf(buf2, "$N dual parries your attack.");
        sprintf(buf3, "$N dual parries $n's attack.");
    }
    if (!doomsingerCheck(ch)){
      act( buf1, ch, vir_obj, victim, TO_VICT );
      act( buf2, ch, vir_obj, victim, TO_CHAR );
      act( buf3, ch, vir_obj, victim, TO_WATCHROOM );
    }
    check_improve(victim,gsn_dual_parry,TRUE, 0);

    if (ch && victim && ch->fighting == victim
	&& check_ccombat_actions( victim, ch, CCOMBAT_DPARRY))
      return TRUE;

//PARRY CHECK
      if (obj && obj2 && PARRY_CHECK(ch, obj, obj2, dt) == -1)
	bug("Error at: PARRY_CHECK\n\r", 0);

    return TRUE;
}


bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn)
{
    OBJ_DATA *obj = NULL;
    OBJ_DATA* obj2 = (vir_obj != NULL? vir_obj : get_eq_char(ch,WEAR_WIELD));

    int chance, ini_chance, temp = 0;
    char buf1[MSL], buf2[MSL], buf3[MSL];
    AFFECT_DATA* paf;

//Sure hit
    if ( !IS_AWAKE(victim)
	 || ((paf = affect_find(victim->affected, gsn_wlock)) && paf->modifier != 0)
	 || is_affected(victim,gsn_ecstacy))
      return FALSE;
//Skill check
    if ( (chance = get_skill(victim,gsn_shield_block) / 2) == 0)
	return FALSE;
//Item check
    if ( !is_affected(victim, skill_lookup("spirit shield")) && (obj = get_eq_char( victim, WEAR_SHIELD )) == NULL )
        return FALSE;

    ini_chance = chance;


//bonus/penalty for blind.
  //we only check if character can see if this is not virtual attack.
    if (!can_see(ch,victim) && !fVirt)
    {
      if (!IS_NPC(ch) && number_percent() < get_skill(ch,gsn_blind_fighting)){
	if (!is_affected(ch, gsn_battletrance))
	  chance *= 1.1;
      }
      else
	chance *= 1.3;
   }

    if (!can_see(victim,ch))
    {
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	if (!is_affected(victim, gsn_battletrance))
	  chance /= 1.1;
      }
      else
	chance /= 1.3;
    }

/* crusade effects */
    if ( (temp = check_crusade(ch, victim)) != CRUSADE_NONE){
      /* if attacker is on crusade against victim */
      if (temp == CRUSADE_MATCH)
	chance /= 1.3;
      /* if attacker is on crusade but not agaisnt victim */
      else if (temp == CRUSADE_NOTMATCH)
	chance *= 1.1;
    }
    if ( (temp = check_crusade(victim, ch)) != CRUSADE_NONE){
      /* if victim is on crusade against attacker */
      if (temp == CRUSADE_MATCH)
	chance *= 1.3;
      /* if victim is on crusade but not against attacker */
      else if (temp == CRUSADE_NOTMATCH)
	chance /= 1.1;
    }
/* end crusade bonuses */

/* check weapon position */
    if (!IS_NPC(ch)){
      /* harder to block for victim if attacker is high */
      if (ch->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 10;
      /* easier to block for victim if attacker is low */
      else if (ch->pcdata->wep_pos == WEPPOS_LOW)
	chance += 5;
    }
    if (!IS_NPC(victim)){
      /* harder to block for victim if victim is high */
      if (victim->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 5;
      /* easier to block for victim if victim is low */
      else if (victim->pcdata->wep_pos == WEPPOS_LOW)
	chance += 10;
    }

    //bonus for bless
    if (obj && IS_GOOD(victim) && IS_OBJ_STAT(obj,ITEM_BLESS))
      chance += 5;
    else if (obj && IS_EVIL(victim) && IS_OBJ_STAT(obj,ITEM_DARK))
      chance += 5;

    chance += get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR);
    chance += get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX);
    chance += get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK);

    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL && number_percent() < get_skill(ch, gsn_mounted)){
      chance -= 3;
      check_improve(ch, gsn_mounted, TRUE, 1);
    }
    if (IS_NPC(ch) && IS_SET(ch->off_flags, CABAL_GUARD))
	chance -= 10;
    else
        chance += victim->level - level;

    if (!IS_NPC(ch)){
      chance = get_dchance(ini_chance, chance, (h_roll - victim->level/5)/4);
    }
    else
    	chance -= h_roll/4;

    if (!IS_NPC(victim) && IS_SET(victim->act2,PLR_GIMP) )
	chance /= 2;

    if ( number_percent( ) >= chance )
        return FALSE;

    if (fVirt)
    {
	/* We select two types of origin of the virual attack.
	   1) obj
	   2) spell
	*/

        sprintf(buf1, "You block a %s's %s with your shield.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));

        sprintf(buf2, "$N blocks a %s's %s with $S shield.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));

        sprintf(buf3, "$N blocks a %s's %s with $S shield.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));
    }
    else
    {
        sprintf(buf1, "You block $n's attack with your shield.");
        sprintf(buf2, "$N blocks your attack with $S shield.");
        sprintf(buf3, "$N blocks $n's attack with $S shield.");
    }

    if (!doomsingerCheck(ch)){
      act( buf1, ch, vir_obj, victim, TO_VICT );
      act( buf2, ch, vir_obj, victim, TO_CHAR );
      act( buf3, ch, vir_obj, victim, TO_WATCHROOM );
    }
    check_improve(victim,gsn_shield_block,TRUE, 0);

//SHBLOCK CHECK
      if (SHBLOCK_CHECK(ch, obj2, obj, dt) == -1)
	bug("Error at: BLOCK_CHECK\n\r", 0);

    return TRUE;
}

bool check_blink( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn)
{
    int chance, ini_chance, temp = 0;
    char buf1[MSL], buf2[MSL], buf3[MSL];

//Sure hit
    if ( !IS_AWAKE(victim) || victim->class != gcn_psi
	 || !is_affected(victim, gsn_blink)
	 || is_affected(victim,gsn_ecstacy))
      return FALSE;
//Skill check
    if ( (chance = get_skill(victim,gsn_blink) / 2) == 0)
      return FALSE;

    ini_chance = chance;

//bonus/penalty for blind.
  //we only check if character can see if this is not virtual attack.
    if (!can_see(ch,victim) && !fVirt){
      if (!IS_NPC(ch) && number_percent() < get_skill(ch,gsn_blind_fighting)){
	if (!is_affected(ch, gsn_battletrance))
	  chance *= 1.1;
      }
      else
	chance *= 1.3;
    }

    if (!can_see(victim,ch)){
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	if (!is_affected(victim, gsn_battletrance))
	  chance /= 1.1;
      }
      else
	chance /= 1.3;
    }

/* crusade effects */
    if ( (temp = check_crusade(ch, victim)) != CRUSADE_NONE){
      /* if attacker is on crusade against victim */
      if (temp == CRUSADE_MATCH)
	chance /= 1.3;
      /* if attacker is on crusade but not agaisnt victim */
      else if (temp == CRUSADE_NOTMATCH)
	chance *= 1.1;
    }
    if ( (temp = check_crusade(victim, ch)) != CRUSADE_NONE){
      /* if victim is on crusade against attacker */
      if (temp == CRUSADE_MATCH)
	chance *= 1.3;
      /* if victim is on crusade but not against attacker */
      else if (temp == CRUSADE_NOTMATCH)
	chance /= 1.1;
    }
/* end crusade bonuses */

/* check weapon position */
    if (!IS_NPC(ch)){
      /* harder to block for victim if attacker is high */
      if (ch->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 10;
      /* easier to block for victim if attacker is low */
      else if (ch->pcdata->wep_pos == WEPPOS_LOW)
	chance += 5;
    }
    if (!IS_NPC(victim)){
      /* harder to block for victim if victim is high */
      if (victim->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 5;
      /* easier to block for victim if victim is low */
      else if (victim->pcdata->wep_pos == WEPPOS_LOW)
	chance += 10;
    }

    chance += 3 * (get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK));

    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL && number_percent() < get_skill(ch, gsn_mounted)){
      chance -= 3;
      check_improve(ch, gsn_mounted, TRUE, 1);
    }
    if (IS_NPC(ch) && IS_SET(ch->off_flags, CABAL_GUARD))
      chance -= 10;
    else
      chance += victim->level - level;

    if (!IS_NPC(ch)){
      chance = get_dchance(ini_chance, chance, (h_roll - victim->level/5)/4);
    }
    else
      chance -= h_roll/4;

    if (!IS_NPC(victim) && IS_SET(victim->act2,PLR_GIMP) )
      chance /= 2;

    if ( number_percent( ) >= chance )
      return FALSE;

    if (fVirt){
      /* We select two types of origin of the virual attack.
	 1) obj
	 2) spell
      */

      sprintf(buf1, "You blink out before a %s's %s.",
	      (vir_obj == NULL? get_vir_attack(sn) : "$p"),
	      get_vir_attack(dt));

      sprintf(buf2, "$N blinks out before a %s's %s.",
	      (vir_obj == NULL? get_vir_attack(sn) : "$p"),
	      get_vir_attack(dt));

      sprintf(buf3, "$N blocks a %s's %s with $S shield.",
	      (vir_obj == NULL? get_vir_attack(sn) : "$p"),
	      get_vir_attack(dt));
    }
    else{
      sprintf(buf1, "You blink out before $n's attack.");
      sprintf(buf2, "$N blinks out before your attack.");
      sprintf(buf3, "$N blinks out before $n's attack.");
    }

    if (!doomsingerCheck(ch)){
      act( buf1, ch, vir_obj, victim, TO_VICT );
      act( buf2, ch, vir_obj, victim, TO_CHAR );
      act( buf3, ch, vir_obj, victim, TO_WATCHROOM );
    }
    check_improve(victim, gsn_blink, TRUE, 0);
    return TRUE;
}

bool check_block( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn)
{
  AFFECT_DATA* paf;
  OBJ_DATA *obj = get_eq_char(victim, WEAR_WIELD);
  OBJ_DATA* obj2 = (vir_obj != NULL? vir_obj : get_eq_char(ch,WEAR_WIELD));

  int chance, ini_chance, temp = 0, ch_weapon, vict_weapon, ch_vict_weapon, c_wtype = 0, v_wtype = 0;
  char buf1[MSL], buf2[MSL], buf3[MSL];

//Sure hit
  if ( !IS_AWAKE(victim)
       || ((paf = affect_find(victim->affected, gsn_wlock)) && paf->modifier != 0)
       || is_affected(victim,gsn_ecstacy))
    return FALSE;

  if (obj == NULL)
    return FALSE;

//Skill check
  if ( (chance =  get_skill(victim,gsn_2hands) / 2) == 0)
    return FALSE;

//Item check (must be 2 h weapon)
  if (!IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
      && !is_affected(victim, gsn_double_grip))
    return FALSE;

//heck ifheld in 2 hands (second hand empty.)
  if (get_eq_char(victim,WEAR_SHIELD) || get_eq_char(victim,WEAR_SECONDARY)
      || get_eq_char(victim,WEAR_HOLD))
    return FALSE;

  ini_chance = chance;

//bonus/penalty for blind.
  //we only check if character can see if this is not virtual attack.
    if (!can_see(ch,victim) && !fVirt)
    {
      if (!IS_NPC(ch) && number_percent() < get_skill(ch,gsn_blind_fighting)){
	if (!is_affected(ch, gsn_battletrance))
	  chance *= 1.1;
      }
      else
	chance *= 1.3;
    }

    if (!can_see(victim,ch))
    {
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	if (!is_affected(victim, gsn_battletrance))
	  chance /= 1.1;
      }
      else
	chance /= 1.3;
    }

/* crusade effects */
    if ( (temp = check_crusade(ch, victim)) != CRUSADE_NONE){
      /* if attacker is on crusade against victim */
      if (temp == CRUSADE_MATCH)
	chance /= 1.3;
      /* if attacker is on crusade but not agaisnt victim */
      else if (temp == CRUSADE_NOTMATCH)
	chance *= 1.1;
    }
    if ( (temp = check_crusade(victim, ch)) != CRUSADE_NONE){
      /* if victim is on crusade against attacker */
      if (temp == CRUSADE_MATCH)
	chance *= 1.3;
      /* if victim is on crusade but not against attacker */
      else if (temp == CRUSADE_NOTMATCH)
	chance /= 1.1;
    }
/* end crusade bonuses */

/* check weapon position */
    if (!IS_NPC(ch)){
      /* harder to block for victim if attacker is high */
      if (ch->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 10;
      /* easier to block for victim if attacker is low */
      else if (ch->pcdata->wep_pos == WEPPOS_LOW)
	chance += 5;
    }
    if (!IS_NPC(victim)){
      /* harder to block for victim if victim is high */
      if (victim->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 5;
      /* easier to block for victim if victim is low */
      else if (victim->pcdata->wep_pos == WEPPOS_LOW)
	chance += 10;
    }

//weapons skills.
    if (!IS_NPC(ch)){
      ch_weapon = (fVirt? get_skill(ch, sn) : get_weapon_skill_obj(ch, obj2));
      ch_vict_weapon = (fVirt? 2 * victim->level : get_weapon_skill_obj(victim, obj2));
    }
    else{
      ch_weapon = 100;
      ch_vict_weapon = (fVirt? 2 * victim->level : get_weapon_skill_obj(victim, obj2));
    }
    vict_weapon = get_weapon_skill_obj(victim, obj);

    //DEFENDER'S WEAPON MODIFIER
    v_wtype = get_weapon_obj_rating( obj, WEAPON_DEF );

    //ATTACKER'S WEAPON MODIFIER
    if (fVirt){
      if (dt == gen_blades || dt == gen_dan_blade)
	c_wtype = URANGE(-2, (ch_weapon - 80) / 5, 5);
      else
	c_wtype = 0;
    }
    else if ( ch->class == gcn_psi
	      && (paf = affect_find( ch->affected, gsn_false_weapon)) != NULL)
      c_wtype = get_weapon_rating( paf->modifier, WEAPON_ATT );
    else
      c_wtype = get_weapon_obj_rating( obj2, WEAPON_ATT );

    chance = chance * vict_weapon/100;
    chance += (v_wtype - c_wtype) * 3;
    chance += (ch_vict_weapon - ch_weapon) / 5;

    chance += (get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR)) * 2;
    chance += get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX);
    chance += get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK);

    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL && number_percent() < get_skill(ch, gsn_mounted)){
      chance -= 3;
      check_improve(ch, gsn_mounted, TRUE, 1);
    }
    if (!IS_NPC(victim) && IS_AFFECTED2(victim,AFF_RAGE)
	&& get_skill(victim, gsn_path_fury) > 1){
      chance += 5;
    }

    if (IS_NPC(ch) && IS_SET(ch->off_flags, CABAL_GUARD))
	chance -= 10;
    else
        chance += victim->level - level;

    if (!IS_NPC(ch)){
      chance = get_dchance(ini_chance, chance, (h_roll - victim->level/5)/4);
    }
    else
    	chance -= h_roll/4;

    if (!IS_NPC(victim) && IS_SET(victim->act2,PLR_GIMP) )
	chance /= 2;

    if ( number_percent( ) >= chance )
        return FALSE;

    if (fVirt)
    {
	/* We select two types of origin of the virual attack.
	   1) obj
	   2) spell
	*/

        sprintf(buf1, "You block a %s's %s with your weapon.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));

        sprintf(buf2, "$N blocks a %s's %s with $S weapon.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));

        sprintf(buf3, "$N blocks a %s's %s with $S weapon.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));
    }
    else
    {
        sprintf(buf1, "You block $n's attack with your weapon.");
        sprintf(buf2, "$N blocks your attack with $S weapon.");
        sprintf(buf3, "$N blocks $n's attack with $S weapon.");
    }

    if (!doomsingerCheck(ch)){
      act( buf1, ch, vir_obj, victim, TO_VICT );
      act( buf2, ch, vir_obj, victim, TO_CHAR );
      act( buf3, ch, vir_obj, victim, TO_WATCHROOM );
    }
    check_improve(victim,gsn_2hands,TRUE, 0);

//SHBLOCK CHECK
      if (SHBLOCK_CHECK(ch, obj2, obj, dt) == -1)
	bug("Error at: BLOCK_CHECK\n\r", 0);

    return TRUE;
}

bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn)
{
    int chance, ini_chance, temp = 0;
    char buf1[MSL], buf2[MSL], buf3[MSL];
    bool fDrunk = FALSE;
    bool fWereBeast = FALSE;

//Sure hit
    if ( !IS_AWAKE(victim) || is_affected(victim,gsn_ecstacy))
      return FALSE;
//Skill check
    if ( ( chance = get_skill(victim,gsn_dodge) / 2) == 0)
	return FALSE;

    /* try for sidestep */
    if (ch && victim && ch->fighting == victim
	&& check_ccombat_actions( victim, ch, CCOMBAT_EVADE))
      return TRUE;
    ini_chance = chance;
//bonus/penalty for blind.
  //we only check if character can see if this is not virtual attack.
    if (!can_see(ch,victim) && !fVirt)
    {
      if (!IS_NPC(ch) && number_percent() < get_skill(ch,gsn_blind_fighting)){
	if (!is_affected(ch, gsn_battletrance))
	  chance *= 1.1;
      }
      else
	chance *= 1.3;
   }

    if (!can_see(victim,ch))
    {
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	if (!is_affected(victim, gsn_battletrance))
	  chance /= 1.1;
      }
      else
	chance /= 1.3;
    }

/* crusade effects */
    if ( (temp = check_crusade(ch, victim)) != CRUSADE_NONE){
      /* if attacker is on crusade against victim */
      if (temp == CRUSADE_MATCH)
	chance /= 1.3;
      /* if attacker is on crusade but not agaisnt victim */
      else if (temp == CRUSADE_NOTMATCH)
	chance *= 1.1;
    }
    if ( (temp = check_crusade(victim, ch)) != CRUSADE_NONE){
      /* if victim is on crusade against attacker */
      if (temp == CRUSADE_MATCH)
	chance *= 1.3;
      /* if victim is on crusade but not against attacker */
      else if (temp == CRUSADE_NOTMATCH)
	chance /= 1.1;
    }
/* end crusade bonuses */

/* check weapon position */
    if (!IS_NPC(ch)){
      /* harder to block for victim if attacker is high */
      if (ch->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 10;
      /* easier to block for victim if attacker is low */
      else if (ch->pcdata->wep_pos == WEPPOS_LOW)
	chance += 5;
    }
    if (!IS_NPC(victim)){
      /* harder to block for victim if victim is high */
      if (victim->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 5;
      /* easier to block for victim if victim is low */
      else if (victim->pcdata->wep_pos == WEPPOS_LOW)
	chance += 10;
    }

    chance += get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK);

    if (!IS_NPC(ch)){
      chance = get_dchance(ini_chance, chance, (h_roll - victim->level/5)/4);
    }
    else
      chance -= h_roll/4;

//WOLF CHECK
    if (is_affected(victim, gsn_wolf_form) && vamp_can_sneak(victim, victim->in_room))
      chance *= 1.2;
    else if (victim->race == grn_werebeast){
      AFFECT_DATA* paf = victim->affected;
      for (;paf; paf = paf->next){
	if (paf->type == gsn_weretiger
	    || paf->type == gsn_werewolf
	    || paf->type == gsn_werebear
	    || paf->type == gsn_werefalcon)
	  break;
      }
      if (paf != NULL){
	fWereBeast = TRUE;
	chance = 10 + 12 * chance / 10;
      }
    }
    if (!fWereBeast && IS_AFFECTED2(victim,AFF_RAGE))
      return FALSE;

    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL && number_percent() < get_skill(ch, gsn_mounted)){
      chance -= 3;
      check_improve(ch, gsn_mounted, TRUE, 1);
    }
    if (IS_NPC(ch) && IS_SET(ch->off_flags, CABAL_GUARD))
	chance -= 10;
    else
        chance += victim->level - level;

    if (!IS_NPC(victim) && is_affected(victim,gsn_drunken)
	&& is_empowered_quiet(victim,1)){

      if (number_percent() < get_skill(victim, gsn_drunken)){
	chance += 5 + URANGE(10,victim->pcdata->condition[COND_DRUNK],35);
	check_improve(victim, gsn_drunken, TRUE, 0);
	fDrunk = TRUE;
      }
    }
    if (!IS_NPC(victim) && IS_SET(victim->act2,PLR_GIMP) )
	chance /= 2;

    if ( number_percent( ) >= chance )
        return FALSE;

    if (fVirt)
    {
	/* We select two types of origin of the virual attack.
	   1) obj
	   2) spell
	*/
        sprintf(buf1, "You dodge a %s's %s.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));

        sprintf(buf2, "$N dodges %s's %s.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));

        sprintf(buf3, "$N dodges %s's %s.",
		(vir_obj == NULL? get_vir_attack(sn) : "$p"),
		get_vir_attack(dt));
    }
    else if (fDrunk){
      sprintf(buf1, "You stagger wildly out of $n's attack.");
      sprintf(buf2, "$N staggers wildly out of your attack.");
      sprintf(buf3, "$N staggers wildly out of $n's attack.");
    }
    else if (victim->class == gcn_blademaster && is_affected(victim, gsn_deathweaver)){
      sprintf(buf1, "You weave out of $n's attack.");
      sprintf(buf2, "$N weaves out of your attack.");
      sprintf(buf3, "$N weaves out of $n's attack.");
    }
    else
      {
        sprintf(buf1, "You dodge $n's attack.");
        sprintf(buf2, "$N dodges your attack.");
        sprintf(buf3, "$N dodges $n's attack.");
      }
    if (!doomsingerCheck(ch)){
      act( buf1, ch, vir_obj, victim, TO_VICT );
      act( buf2, ch, vir_obj, victim, TO_CHAR );
      act( buf3, ch, vir_obj, victim, TO_WATCHROOM );
    }
    check_improve(victim,gsn_dodge,TRUE, 0);
    return TRUE;
}

bool check_riposte( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* vir_obj, int dt, int h_roll, int level, bool fVirt, int sn)
{
  AFFECT_DATA* paf;
  OBJ_DATA *vic_obj = NULL;
  OBJ_DATA *ch_obj = (fVirt? vir_obj : get_eq_char( ch, WEAR_WIELD));
  int ch_weapon, vict_weapon, ch_vict_weapon, c_wtype = 0, v_wtype = 0;
  int chance, ini_chance, temp = 0;
  char buf1[MSL], buf2[MSL], buf3[MSL];

//Sure hit
  if ( !IS_AWAKE(victim) || is_affected(victim,gsn_ecstacy))
    return FALSE;
//Skill
  if ( (chance = get_skill(victim,gsn_riposte) / 3) == 0)
    return FALSE;
//virtual attacks are hard to riposte
  if (fVirt)
    chance /= 3;
//item check
  if ( (vic_obj = get_eq_char( victim, WEAR_WIELD )) == NULL)
    return FALSE;
  ini_chance = chance;

//bonus/penalty for blind.
  //we only check if character can see if this is not virtual attack.
  if (!can_see(ch,victim) && !fVirt){
    if (!IS_NPC(ch) && number_percent() < get_skill(ch,gsn_blind_fighting)){
      if (!is_affected(ch, gsn_battletrance))
	chance *= 1.1;
    }
    else
      chance *= 1.3;
  }

  if (!can_see(victim,ch))
    {
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
	if (!is_affected(victim, gsn_battletrance))
	  chance /= 1.2;
      }
      else
	chance /= 1.4;
    }

/* crusade effects */
  if ( (temp = check_crusade(ch, victim)) != CRUSADE_NONE){
    /* if attacker is on crusade against victim */
    if (temp == CRUSADE_MATCH)
      chance /= 1.3;
    /* if attacker is on crusade but not agaisnt victim */
    else if (temp == CRUSADE_NOTMATCH)
      chance *= 1.1;
  }
  if ( (temp = check_crusade(victim, ch)) != CRUSADE_NONE){
    /* if victim is on crusade against attacker */
    if (temp == CRUSADE_MATCH)
      chance *= 1.3;
    /* if victim is on crusade but not against attacker */
    else if (temp == CRUSADE_NOTMATCH)
      chance /= 1.1;
  }
/* end crusade bonuses */

/* check weapon position */
  if (!IS_NPC(ch)){
    /* harder to block for victim if attacker is high */
    if (ch->pcdata->wep_pos == WEPPOS_HIGH)
      chance -= 10;
    /* easier to block for victim if attacker is low */
    else if (ch->pcdata->wep_pos == WEPPOS_LOW)
      chance += 5;
  }
  if (!IS_NPC(victim)){
    /* harder to block for victim if victim is high */
      if (victim->pcdata->wep_pos == WEPPOS_HIGH)
	chance -= 5;
      /* easier to block for victim if victim is low */
      else if (victim->pcdata->wep_pos == WEPPOS_LOW)
	chance += 10;
  }

//weapons skills.
  if (!IS_NPC(ch)){
    ch_weapon = (fVirt? get_skill(ch, sn) : get_weapon_skill_obj(ch, ch_obj));
    ch_vict_weapon = (fVirt? 2 * victim->level : get_weapon_skill_obj(victim, ch_obj));
  }
  else{
    ch_weapon = 100;
    ch_vict_weapon = (fVirt? 2 * victim->level : get_weapon_skill_obj(victim, ch_obj));
  }
  vict_weapon = get_weapon_skill_obj(victim, vic_obj);

  //DEFENDER'S WEAPON MODIFIER
  v_wtype = get_weapon_obj_rating( vic_obj, WEAPON_DEF );

  //ATTACKER'S WEAPON MODIFIER
  if (fVirt){
    if (dt == gen_blades || dt == gen_dan_blade)
      c_wtype = URANGE(-2, (ch_weapon - 80) / 5, 5);
    else
      c_wtype = 0;
  }
  else if ( ch->class == gcn_psi
	    && (paf = affect_find( ch->affected, gsn_false_weapon)) != NULL)
    c_wtype = get_weapon_rating( paf->modifier, WEAPON_ATT );
  else
    c_wtype = get_weapon_obj_rating( ch_obj, WEAPON_ATT );

  chance = chance * vict_weapon/100;
  chance += (v_wtype - c_wtype) * 3;
  chance += (ch_vict_weapon - ch_weapon) / 5;

  chance += get_curr_stat(victim,STAT_DEX) - get_curr_stat(ch,STAT_DEX);
  chance += get_curr_stat(victim,STAT_STR) - get_curr_stat(ch,STAT_STR);
  chance += get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK);

  if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL && number_percent() < get_skill(ch, gsn_mounted)){
    chance -= 3;
    check_improve(ch, gsn_mounted, TRUE, 1);
  }

  if (IS_NPC(ch) && IS_SET(ch->off_flags, CABAL_GUARD))
    chance -= 10;
  else
    chance += victim->level - level;

  if (!IS_NPC(ch)){
    chance = get_dchance(ini_chance, chance, (h_roll - victim->level/5)/2);
  }
  else
    chance -= h_roll/4;

  if (!IS_NPC(victim) && IS_SET(victim->act2,PLR_GIMP) )
    chance /= 2;

  if (IS_AFFECTED2(victim,AFF_RAGE))
    return FALSE;

  if ( number_percent( ) >= chance )
    return FALSE;

  if (fVirt)
    {
      /* We select two types of origin of the virual attack.
	 1) obj
	 2) spell
      */

      sprintf(buf1, "You blocka  %s's %s and attempts to strike at $n through the brief opening",
	      (vir_obj == NULL? get_vir_attack(sn) : "$p"),
	      get_vir_attack(dt));

      sprintf(buf2, "$N blocks your %s's %s and attempts to strike at you through the brief opening",
	      (vir_obj == NULL? get_vir_attack(sn) : "$p"),
	      get_vir_attack(dt));

      sprintf(buf3, "$N blocks a %s's %s and attempts to strike at $n through the brief opening",
	      (vir_obj == NULL? get_vir_attack(sn) : "$p"),
	      get_vir_attack(dt));
    }
    else
      {
        sprintf(buf1, "You block $n's attack and attempt to strike at the brief opening.");
        sprintf(buf2, "$N blocks your attack and attempts to strike at the brief opening.");
        sprintf(buf3, "$N blocks $n's attack and attempts to strike at the brief opening.");
      }
  if (!doomsingerCheck(ch)){
    act( buf1, ch, vir_obj, victim, TO_VICT );
    act( buf2, ch, vir_obj, victim, TO_CHAR );
    act( buf3, ch, vir_obj, victim, TO_WATCHROOM );
    /* hit back */
    one_hit(victim,ch,gsn_riposte,FALSE);
  }
  check_improve(victim,gsn_riposte,TRUE, 0);
  return TRUE;
}

void update_pos( CHAR_DATA *victim )
{
    if ( victim->hit > 0 )
    {
    	if ( victim->position <= POS_STUNNED )
	    victim->position = POS_STANDING;
	return;
    }
    if ( IS_NPC(victim) && victim->hit < 1 )
    {
	victim->position = POS_DEAD;
	return;
    }
    if ( victim->hit <= -11)
    {
	victim->position = POS_DEAD;
	return;
    }
    if ( victim->hit <= -6)
      victim->position = POS_MORTAL;
    else if ( victim->hit <= -3)
      victim->position = POS_INCAP;
    else
      victim->position = POS_STUNNED;
}

void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
  if (ch == NULL || victim == NULL)
    return;

  if ( IS_AFFECTED(ch, AFF_SLEEP) )
    {
      REMOVE_BIT(ch->affected_by,AFF_SLEEP);
      affect_strip( ch,gsn_sleep );
      affect_strip( ch,gsn_hypnosis );
      affect_strip(ch,gsn_coffin);
      affect_strip(ch,gsn_entomb);
      song_strip(ch,gsn_lullaby);
    }
  if ( IS_AFFECTED2(ch, AFF_CATALEPSY) )
    do_revive(ch, "");
  if (is_affected(victim,gsn_prayer))
    {
      send_to_char("Your prayer has been disrupted.\n\r",victim);
      affect_strip(victim,gsn_prayer);
    }
  if (is_affected(ch,gsn_prayer))
    {
      send_to_char("Your prayer has been disrupted.\n\r",ch);
      affect_strip(ch,gsn_prayer);
    }
  if (is_affected(victim,gsn_trap_silvanus))
    {
      send_to_char("You lost your trap in the frey.\n\r",victim);
      affect_strip(victim,gsn_trap_silvanus);
    }
  if (is_affected(ch,gsn_trap_silvanus))
    {
      send_to_char("You lost your trap in the frey.\n\r",ch);
      affect_strip(ch,gsn_trap_silvanus);
    }
  if (is_affected(victim,gsn_empower))
    {
      send_to_char("You cannot concentrate on empowering while fighting!\n\r",victim);
      affect_strip(victim,gsn_empower);
      affect_strip(victim,gsn_offensive);
      affect_strip(victim,gsn_defensive);
    }
  if (is_affected(ch,gsn_empower))
    {
      send_to_char("You cannot concentrate on empowering while fighting!\n\r",ch);
      affect_strip(ch,gsn_empower);
      affect_strip(ch,gsn_offensive);
      affect_strip(ch,gsn_defensive);
    }

  if (IS_NPC(ch) && is_affected(ch,gsn_mirror_image))
    {
      extract_char(ch,TRUE);
      return;
    }

  if (IS_NPC(ch) && is_affected(ch,gsn_decoy))
    {
      send_to_char("It's a trap!\n\r",victim);
      act("$n explodes, a powdery dust spills from it.",ch,NULL,NULL,TO_ALL);
      spell_poison(gsn_poison,victim->level,ch,(void *) victim,TARGET_CHAR);
      spell_blindness(gsn_blindness,victim->level,ch,(void *) victim,TARGET_CHAR);
      extract_char(ch,TRUE);
      return;
    }

// PREVIOLENCE CHECK //
    if (  PREVIOLENCE_CHECK(ch, victim) == -1)
      bug("Error at: PreViolenceCheck, returned fail\n\r", 0);

/* morph check */
    if ( IS_NPC(ch) && !IS_NPC(victim) && IS_SET(ch->act2, ACT_MORPH)
	 && ch->fighting != victim)
      morph_mob(ch, victim);

    ch->fighting = victim;
    ch->position = POS_FIGHTING;
    set_delay(ch, victim);
}

void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;
    AFFECT_DATA* paf;

    for ( fch = char_list; fch != NULL; fch = fch->next ){
      if ( fch == ch || ( fBoth && fch->fighting == ch ) ){
// POSTVIOLENCE CHECK //
	if (  POSTVIOLENCE_CHECK(fch) == -1)
	  bug("Error at: PreViolenceCheck, returned fail\n\r", 0);

	fch->fighting	= NULL;
	fch->position	= IS_NPC(fch) ? fch->default_pos : POS_STANDING;
	update_pos( fch );
	//clear ranged weapon reload status
	fch->reload = 0;

	affect_strip( fch, gsn_phantasmal_griffon );
	affect_strip( fch, gsn_illusionary_spectre );
	affect_strip( fch, gsn_phantom_dragon );
	affect_strip( fch, gsn_throw );
	affect_strip( fch, gsn_forms );
	affect_strip( fch, gsn_onslaught );
	if ( (paf = affect_find(fch->affected, gsn_wlock)) != NULL){
	  if (paf->modifier != 0)
	    act(skill_table[gsn_wlock].msg_off, fch, NULL, NULL, TO_CHAR);
	  affect_strip(fch, gsn_wlock);
	}
	if (fch == ch || fch->in_room != ch->in_room)
	  continue;
	set_delay(ch, fch);
      }
    }
}

OBJ_DATA* make_corpse( CHAR_DATA *ch )
{
    char buf[MSL];
    OBJ_DATA *corpse, *obj, *obj_next;
    char *name;
    if (ch->in_room == NULL)
	return NULL;
    //not on mirrors
    if (IS_NPC(ch))      if (ch->pIndexData->vnum == MOB_VNUM_DECOY || ch->pIndexData->vnum == MOB_VNUM_DUMMY)
	return NULL;

    if ( IS_NPC(ch) )
    {
	name		= str_dup(ch->short_descr);
        corpse          = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
	corpse->timer	= number_range( 3, 6 );
        corpse->owner   = ch->pIndexData->vnum;
    }
    else
    {
	name		= str_dup(ch->name);
        corpse          = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
	corpse->timer	= number_range( 25, 40 );
	corpse->owner	= ch->id;
	corpse->pCabal	= get_parent(ch->pCabal);
    }
    if (ch->gold > 0 )
    {
        obj_to_obj(create_money(ch->gold), corpse);
        ch->gold = 0;
    }
    corpse->cost = 0;
    corpse->recall  = get_temple_pitroom(ch);
    corpse->level = ch->level;
    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );
    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    /* transfer eq */
    if (IS_IMMORTAL(ch) || is_affected(ch, gsn_life_insurance)){
      AFFECT_DATA* paf = affect_find(ch->affected, gsn_life_insurance);
      if (paf)
	CP_GAIN(ch, -paf->modifier, TRUE);
      obj_to_room( corpse, ch->in_room );
      return corpse;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj_next ){
      obj_next = obj->next_content;

      affect_strip_obj( obj, gsn_bloodvow);
      REMOVE_BIT(obj->extra_flags,ITEM_INVENTORY);

      obj_from_char( obj );
      if (obj->item_type == ITEM_POTION)
	obj->timer = number_range(500,1000);
      if (obj->item_type == ITEM_HERB)
	obj->timer = number_range(500,1000);
      if (obj->item_type == ITEM_SCROLL)
	obj->timer = number_range(1000,2500);
      if (IS_SET(obj->extra_flags,ITEM_ROT_DEATH) && !IS_NPC(ch)){
	/*  Changed to instant poof for non-keys: Viri */
	if (obj->item_type == ITEM_KEY){
	  obj->timer = number_range(5,10);
	  REMOVE_BIT(obj->extra_flags,ITEM_ROT_DEATH);
	}
	else{
	  act("$p crumbles to dust.", ch, obj, NULL, TO_CHAR);
	  extract_obj(obj);
	  continue;
	}
      }
      REMOVE_BIT(obj->extra_flags,ITEM_VIS_DEATH);
      if (CAN_WEAR(obj, ITEM_UNIQUE) && obj->pIndexData->count > obj->pIndexData->cost){
	act_new("$p crumbles to dust.", ch, obj, NULL, TO_CHAR, POS_DEAD);
	if (obj->item_type == ITEM_SOCKET && obj->in_obj
	    && IS_OBJ_STAT(obj->in_obj, ITEM_SOCKETABLE)){
	  unadorn_obj(obj, obj->in_obj, ch);
	}
	extract_obj( obj );
      }
      else if (!can_exist( obj->pIndexData )){
	act_new("$p crumbles to dust.", ch, obj, NULL, TO_CHAR, POS_DEAD);
	if (obj->item_type == ITEM_SOCKET && obj->in_obj
	    && IS_OBJ_STAT(obj->in_obj, ITEM_SOCKETABLE)){
	  unadorn_obj(obj, obj->in_obj, ch);
	}
	extract_obj( obj );
      }
      else
	obj_to_obj( obj, corpse );
    }
    obj_to_room( corpse, ch->in_room );
    return corpse;
}

void dump_corpse( CHAR_DATA *ch )
{
    OBJ_DATA *obj, *obj_next;
    if (ch->gold > 0 )
    {
        obj_to_room(create_money(ch->gold), ch->in_room);
        ch->gold = 0;
    }
    if (IS_IMMORTAL(ch) || is_affected(ch, gsn_life_insurance)){
      AFFECT_DATA* paf = affect_find(ch->affected, gsn_life_insurance);
      if (paf)
	CP_GAIN(ch, -paf->modifier, TRUE);
      act("$n's body disintegrates into dust.",ch,NULL,NULL,TO_ROOM);
      return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj_next ){
      obj_next = obj->next_content;

      affect_strip_obj( obj, gsn_bloodvow);
      REMOVE_BIT(obj->extra_flags,ITEM_INVENTORY);

      obj_from_char( obj );
      if (obj->item_type == ITEM_POTION)
	obj->timer = number_range(500,1000);
      if (obj->item_type == ITEM_HERB)
	obj->timer = number_range(500,1000);
      if (obj->item_type == ITEM_SCROLL)
	obj->timer = number_range(1000,2500);
      if (IS_SET(obj->extra_flags,ITEM_ROT_DEATH) && !IS_NPC(ch))
	{
	  /*  Changed to instant poof for non-keys: Viri */
	  if (obj->item_type == ITEM_KEY){
	    obj->timer = number_range(5,10);
	    REMOVE_BIT(obj->extra_flags,ITEM_ROT_DEATH);
	  }
	  else{
	    act("$p crumbles to dust.", ch, obj, NULL, TO_CHAR);
	    extract_obj(obj);
	    continue;
	  }
	}
      REMOVE_BIT(obj->extra_flags,ITEM_VIS_DEATH);
      if (!can_exist( obj->pIndexData ))
	extract_obj( obj );
      else{
	obj_to_room( obj, ch->in_room );
	if (is_affected_obj(obj, gsn_levitate))
	  act( "$p slowly rises and stays suspended in mid-air.", ch, obj, NULL, TO_ALL );
      }
    }
    act("$n's body disintegrates into dust.",ch,NULL,NULL,TO_ROOM);
}

void death_cry( CHAR_DATA *ch, CHAR_DATA* killer )
{
    ROOM_INDEX_DATA *was_in_room;
    AFFECT_DATA *paf;
    CHAR_DATA* vch;
    char *msg = "You hear $n's death cry.";
    bool fFound = FALSE;
    int door, vnum = 0;
    if (ch->in_room == NULL)
	return;
    switch ( number_range(0,19))
    {
    case  0: msg  = "$n hits the ground ... DEAD.";			break;
    case  1: msg = "$n splatters blood on your armor."; 		break;
    case  2: if (IS_SET(ch->parts,PART_GUTS))
        { msg = "$n spills $s guts all over the floor."; vnum = OBJ_VNUM_GUTS; } break;
    case  3: if (IS_SET(ch->parts,PART_HEAD))
        { msg = "$n's severed head plops on the ground."; vnum = OBJ_VNUM_SEVERED_HEAD; } break;
    case  4: if (IS_SET(ch->parts,PART_HEART))
        { msg = "$n's heart is torn from $s chest."; vnum = OBJ_VNUM_TORN_HEART; } break;
    case  5: if (IS_SET(ch->parts,PART_ARMS))
        { msg = "$n's arm is dismembered from $s dead body."; vnum = OBJ_VNUM_SLICED_ARM; } break;
    case  6: if (IS_SET(ch->parts,PART_LEGS))
        { msg = "$n's leg is dismembered from $s dead body."; vnum = OBJ_VNUM_SLICED_LEG; } break;
    case  7: if (IS_SET(ch->parts,PART_BRAINS))
        { msg = "$n's head is shattered, and $s brains splash all over you."; vnum = OBJ_VNUM_BRAINS; } break;
    case  8: if (IS_SET(ch->parts,PART_HANDS))
        { msg = "$n's hand is ripped from $s arm."; vnum = OBJ_VNUM_HAND; } break;
    case  9: if (IS_SET(ch->parts,PART_FEET))
        { msg = "$n's foot is ripped from $s leg."; vnum = OBJ_VNUM_FOOT; } break;
    case  10: if (IS_SET(ch->parts,PART_FINGERS))
        { msg = "$n's fingers are severed from $s hand."; vnum = OBJ_VNUM_FINGERS; } break;
    case  11: if (IS_SET(ch->parts,PART_EAR))
        { msg = "$n's ear is ripped off of $s head."; vnum = OBJ_VNUM_EAR; } break;
    case  12: if (IS_SET(ch->parts,PART_EYE))
        { msg = "$n's eyes are pulled out of $s head."; vnum = OBJ_VNUM_EYES; } break;
    case  13: if (IS_SET(ch->parts,PART_LONG_TONGUE))
        { msg = "$n's tongue is ripped out of $s head."; vnum = OBJ_VNUM_TONGUE; } break;
    case  14: if (IS_SET(ch->parts,PART_WINGS))
        { msg = "$n's wings slowly float to the ground."; vnum = OBJ_VNUM_WINGS; } break;
    case  15: if (IS_SET(ch->parts,PART_TAIL))
        { msg = "$n's tail is severed from $s dead body."; vnum = OBJ_VNUM_TAIL; } break;
    case  16: if (IS_SET(ch->parts,PART_CLAWS))
        { msg = "$n's claws fall to the ground."; vnum = OBJ_VNUM_CLAWS; } break;
    case  17: if (IS_SET(ch->parts,PART_FANGS))
        { msg = "$n's fangs are extracted from $s head."; vnum = OBJ_VNUM_FANGS; } break;
    case  18: if (IS_SET(ch->parts,PART_HORNS))
        { msg = "$n's horn is extracted from $s head."; vnum = OBJ_VNUM_HORN; } break;
    case  19: if (IS_SET(ch->parts,PART_TUSKS))
        { msg = "$n's tusk is extracted from $s head."; vnum = OBJ_VNUM_TUSK; } break;
    }

    for (vch = killer->in_room->people; vch; vch = vch->next_in_room){
      if (is_same_group(vch, killer) && vch->race == race_lookup("illithid")){
	fFound = TRUE;
	break;
      }
    }

    if (fFound && vnum){
      act("You expertly extract a still warm brain from $n's corpse.", ch, NULL, vch, TO_VICT);
      vnum = OBJ_VNUM_BRAINS;
      act( "$N expertly extracts a still warm brain from $n's corpse.", ch, NULL, vch, TO_NOTVICT);
    }
    else
      act( msg, ch, NULL, killer, TO_ROOM );

    if ( vnum != 0 )
    {
	char buf[MSL];
	char *name 	= IS_NPC(ch) ? ch->short_descr : ch->name;
	OBJ_DATA *obj   = create_object( get_obj_index( vnum ), 0);
	obj->timer	= number_range( 4, 7 );
	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );
	if (obj->item_type == ITEM_FOOD)
	{
	    if (IS_SET(ch->form,FORM_POISON))
	      obj->value[3] = 1;
	    else if (!IS_SET(ch->form,FORM_EDIBLE))
		obj->item_type = ITEM_TRASH;
	}
	if (vnum == OBJ_VNUM_SEVERED_HEAD)
	{
            paf = new_affect();
            paf->type       = 0;
            paf->level      = ch->level;
            paf->duration   = -1;
            paf->location   = APPLY_DAMROLL;
            paf->modifier   = ch->level/10;
            paf->bitvector  = 0;
            paf->next       = obj->affected;
            obj->affected   = paf;
	}
	if (vnum == OBJ_VNUM_EYES)
	{
            paf = new_affect();
            paf->type       = 0;
            paf->level      = ch->level;
            paf->duration   = -1;
            paf->location   = APPLY_HITROLL;
            paf->modifier   = ch->level/10;
            paf->bitvector  = 0;
            paf->next       = obj->affected;
            obj->affected   = paf;
	}
	if (vnum == OBJ_VNUM_TAIL || vnum == OBJ_VNUM_FANGS || vnum == OBJ_VNUM_HORN || vnum == OBJ_VNUM_TUSK)
	{
            obj->value[1] = number_range(ch->level-3, ch->level+3)/3 + 2;
            obj->value[2] = 2;
	}
	if (vnum == OBJ_VNUM_CLAWS)
	    obj->level = ch->level;
	obj_to_room( obj, ch->in_room );
    }
    if ( IS_NPC(ch) )
	msg = "You hear something's death cry.";
    else
	msg = "You hear someone's death cry.";
    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;
	if ( was_in_room != NULL && ( pexit = was_in_room->exit[door] ) != NULL
        && pexit->to_room != NULL && pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;
}

/* run to check if a crusader wtih psalm of one god is about in group */
bool onegod_resurrect(CHAR_DATA* victim, ROOM_INDEX_DATA* room){
  CHAR_DATA* vch;
  if (IS_NPC(victim))
    return FALSE;
  for (vch = player_list; vch; vch = vch->next){
    if (!IS_NPC(vch)
	&& vch->hit < vch->max_hit / 3
	&& vch != victim
	&& is_same_group(vch, victim)
	&& is_affected(vch, gsn_ogtrium))
      return TRUE;
  }
  return FALSE;
}


void _raw_kill( CHAR_DATA *ch, CHAR_DATA *victim, bool fBountyCheck)
{
    OBJ_DATA *tattoo = NULL, *crown = NULL, *locker = NULL;
    OBJ_DATA* corpse = NULL;
    OBJ_DATA* obj;
    CHAR_DATA *vch, *vch_next;
    AFFECT_DATA af, *paf, *paf_next;
    bool is_lag = FALSE;
    bool fRes = FALSE;
    bool fNoDeathCount = FALSE;
    char strsave[MIL];


//PREKILLCHECK
    if (PREKILL_CHECK(ch, victim) == -1)
      bug("Error at: PREKILL_CHECK\n\r", 0);
//PREDEATH
    if (PREDEATH_CHECK(ch, victim) == -1)
      bug("Error at: PREDEATH_CHECK\n\r", 0);

    /* check for one god resurrection */
    if (victim->in_room)
      fRes = onegod_resurrect(victim, victim->in_room);

    //Continuum message.
    continuum(victim, CONTINUUM_DEATH);

    if (IS_NPC(ch)){
      int pos = ch->position;
      /* we save position so we can allow mobs to do things on death. */
      ch->position = POS_STANDING;
      if (HAS_TRIGGER_MOB(ch, TRIG_KILL)){
	p_percent_trigger(ch, NULL, NULL, victim, NULL, NULL, TRIG_KILL);
	if (ch == NULL || victim == NULL || ch->in_room == NULL || victim->in_room == NULL)
	  return;
      }
      ch->position = pos;
    }
/* Objects */
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content ){
      if (HAS_TRIGGER_OBJ(obj, TRIG_KILL)){
	p_percent_trigger(NULL, obj, NULL, victim, NULL, NULL, TRIG_KILL);
	if (ch == NULL || victim == NULL || ch->in_room == NULL || victim->in_room == NULL)
	  return;
      }
    }
    if (IS_NPC(victim)){
      int pos = victim->position;
      /* we save position so we can allow mobs to do things on death. */
      victim->position = POS_STANDING;
      if (HAS_TRIGGER_MOB(victim, TRIG_DEATH)){
	p_percent_trigger(victim, NULL, NULL, ch, NULL, NULL, TRIG_DEATH);
	if (ch == NULL || victim == NULL || ch->in_room == NULL || victim->in_room == NULL)
	  return;
      }
      victim->position = pos;
    }
    /* Objects */
    for ( obj = victim->carrying; obj != NULL; obj = obj->next_content ){
      if (HAS_TRIGGER_OBJ(obj, TRIG_DEATH)){
	p_percent_trigger(NULL, obj, NULL, victim, NULL, NULL, TRIG_DEATH);
	if (ch == NULL || victim == NULL || ch->in_room == NULL || victim->in_room == NULL)
	  return;
      }
    }
    /* Room Triggers */
    if (ch->in_room){
      if (HAS_TRIGGER_ROOM(ch->in_room, TRIG_KILL)){
	p_percent_trigger(NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_KILL);
	if (ch == NULL || victim == NULL || ch->in_room == NULL || victim->in_room == NULL)
	  return;
      }

      sprintf( log_buf, "`&%s got toasted by %s at %s [room %d]``",
	       (IS_NPC(victim) ? victim->short_descr : victim->name),
	       (IS_NPC(ch) ? ch->short_descr : ch->name),
	       ch->in_room->name,
	       ch->in_room->vnum);

      if (!IS_NPC(ch) && IS_IMMORTAL(ch) && victim->level >= 50)
	nlogf( "%s imm killed by %s at %d", victim->name, (IS_NPC(ch) ? ch->short_descr : ch->name), ch->in_room->vnum );
      if (!IS_NPC(victim)){
	nlogf( "%s killed by %s at %d", victim->name, (IS_NPC(ch) ? ch->short_descr : ch->name), ch->in_room->vnum );
	wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);
      }

    }//end if in room

    /* race kills increase */
    if (!IS_NPC(ch) && victim->race < MAX_PC_RACE && victim->race > 0 && ch->pcdata->race_kills[victim->race] < 255){
      int aos_level = ch->pcdata->race_kills[victim->race] / AOS_KILLS_PER_LEVEL;

      ch->pcdata->race_kills[victim->race] += IS_NPC(victim) ? 1 : 5;

      if (ch->pcdata->race_kills[victim->race] / AOS_KILLS_PER_LEVEL != aos_level){
	sendf(ch, "Your %s reputation has decreased. (see \"help reputation\")\n\r", pc_race_table[victim->race].name);
      }
    }

    /* crown, lockers and tattoo */
    if (!IS_NPC(victim)){
      if ( (tattoo = get_eq_char(victim, WEAR_TATTOO)) != NULL)
	obj_from_char(tattoo);
      if ( (crown = get_eq_char(victim, WEAR_HEAD)) != NULL){
	if(crown->vnum == OBJ_VNUM_CROWN)
	  obj_from_char(crown);
	else
	crown = NULL;
      }
      for (locker = victim->carrying; locker; locker = locker->next_content){
	if (locker->vnum == OBJ_VNUM_LOCKER){
	  obj_from_char( locker );
	  break;
	}
      }

    }
    if ( !IS_NPC(victim) ){
      /* CTF item transfer, checks all cases where victim was a PC */
      handle_ctf_kill( ch, victim );

      /*
	 warmaster main cabal members (do not have subcabal) do not get deaths
	 counted
      */
      if (victim->pCabal && victim->pCabal->parent == NULL
	  && IS_CABAL(victim->pCabal, CABAL_NOMAGIC)){
	fNoDeathCount = TRUE;
      }
      else if (crown == NULL)
	victim->pcdata->dall++;
      else
	victim->pcdata->dall += 5;

      if (!IS_NPC(ch)){
	bool fRougeB = FALSE;
	victim->pcdata->dpc++;
	mud_data.deaths++;
	if (ch != victim)
	  ch->pcdata->kpc++;

	//penalty warning for goods killing goods
	if (ch != victim && ch->level > 15
	    && IS_GOOD(ch) && IS_GOOD(victim)
	    && !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim)){
	  char buf[MIL];
	  sprintf(buf, warn_table[WARN_GOODPK].subj, ch->name, victim->name);
	  do_hal("immortal", buf, warn_table[WARN_GOODPK].text, NOTE_PENALTY);
	}

	/* CTF item transfer */
	handle_ctf_kill( ch, victim );
	/* penatly for killing pcs if a bounty is present */
	if (fBountyCheck)
	  fRougeB = check_rouge_bounty( ch, -50 );

	/* cabal update to points */
	if (!fRougeB
	    && fBountyCheck
	    && ch->pCabal
	    && victim->pCabal )
	  cp_event(ch, victim, NULL, CP_EVENT_PC);
	/* avatar */
	divfavor_update(ch, victim);
	/* raid mob */
	raidmob_check( ch, victim );
	/* stalker vs unnatural reward */
	stalker_reward( ch, victim );

	/* MURDER CRIME CHECK */
	if (ch->in_room && is_crime(ch->in_room, CRIME_MURDER, victim)){
	  set_crime(ch, victim, ch->in_room->area, CRIME_MURDER);
	}
      }
      else
	{
	  /* Give credit to master of charmies if they kill */
	  if (IS_AFFECTED(ch,AFF_CHARM)){
	    if (ch->master != NULL && !IS_NPC(ch->master)){
	      bool fRougeB = FALSE;
	      ch->master->pcdata->kpc++;
	      victim->pcdata->dpc++;

	      /* cabal update to points */
	      /* penatly for killing pcs if a bounty is present */
	      if (fBountyCheck)
		fRougeB = check_rouge_bounty( ch->master, -50 );
	      /* cabal update to points */
	      if (!fRougeB
		  && fBountyCheck
		  && ch->master->pCabal
		  && victim->pCabal)
		cp_event(ch->master, victim, NULL, CP_EVENT_PC);
	      /* avatar */
	      divfavor_update(ch, victim);
	      /* raid mob */
	      raidmob_check( ch, victim );
	      /* stalker vs unnatural reward */
	      stalker_reward( ch, victim );
	    }
	    else
	      victim->pcdata->dnpc++;
	  }
	  else
	    victim->pcdata->dnpc++;
	  mud_data.deaths++;
	}
      /* check for PK warning violation */
      if (!IS_NPC(ch) && ch != victim && IS_GAME(ch, GAME_PKWARN)){
	char buf[MIL];
	sprintf(buf, warn_table[WARN_PK_VIO].subj, ch->name);
	do_hal("immortal", buf, warn_table[WARN_PK_VIO].text, NOTE_PENALTY);
      }
    }
    else
      {
	wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0,0);
	if (!IS_NPC(ch)){
	  ch->pcdata->knpc++;

	  /* ASSAULT CRIME CHECK */
	  if (ch->in_room && is_crime(ch->in_room, CRIME_ASSAULT, victim)){
	    set_crime(ch, victim, ch->in_room->area, CRIME_ASSAULT);
	  }
	}
	/* raid mob */
	raidmob_check( ch, victim );

	/* quest check */
	QuestKillCheck(ch, victim );

      }//END NPC DEATH
    stop_fighting( victim, TRUE );
    if (!IS_NPC(victim) && IS_SET(victim->act, PLR_DOOF))
      REMOVE_BIT(victim->act, PLR_DOOF);

    if (IS_NPC(victim)){
      if (victim->pIndexData->progtypes & DEATH_PROG)
        mprog_death_trigger( ch, victim );
    }

    if (!IS_NPC(victim))
      {
	if (!is_affected(victim,gsn_mortally_wounded))
            death_cry( victim, ch );
	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;
	    if (IS_NPC(vch) && vch->hunting != NULL && vch->hunting == victim )
	    {
		vch->hunting = NULL;
		if (IS_SET(vch->act,ACT_AGGRESSIVE))
            	{
            	    act( "$n slowly disappears.",vch,NULL,NULL,TO_ROOM );
            	    extract_char( vch, TRUE );
            	}
	    }
        }
      }
    else
      death_cry( victim, ch );

    /* Cheat check */
    if ( (IS_IMMORTAL(ch)
	  || (ch->leader && IS_IMMORTAL(ch->leader))
	  || (ch->master && IS_IMMORTAL(ch->master))
	  || (IS_NPC(ch) && ch->desc && !is_affected(ch, gsn_linked_mind)))
	 && IS_NPC(victim) ){
      OBJ_DATA* obj;
      char buf[MIL];
      for ( obj = victim->carrying; obj != NULL; obj = obj->next_content ){
	if (IS_LIMITED(obj)
	    && !IS_SET(obj->wear_flags, ITEM_HAS_OWNER)){
	  SET_BIT(obj->extra_flags, ITEM_STAIN);
	  sprintf(buf, "%s stained by %s.", obj->short_descr, PERS2(ch));
	  send_to_char(buf, ch);
	  log_string(buf);
	}
      }
    }

    if (!IS_IMMORTAL(victim)
	&& ((IS_NPC(victim)
	     && IS_SET(victim->form,FORM_INSTANT_DECAY))
	    || IS_SET(victim->act,ACT_UNDEAD)))
      dump_corpse( victim );
    else
      corpse = make_corpse( victim );

    /* For future resurrection */
    if (corpse && !IS_NPC(victim)){
      af.type = gsn_resurrection;
      af.level = victim->pcdata->dall % 5 == 0 ? TRUE : FALSE;
      af.duration = 24;
      af.where = TO_NONE;
    /* bit vector has original duration */
      af.bitvector = af.duration;
      af.location = APPLY_NONE;
      /* exp is refunded based on duration remaning when accepted */
      /* set to 0 if PC kill */
      af.modifier = ( (IS_NPC(ch) && !IS_SET(ch->act2, ACT_NOLOSS)) || ch == victim) ? UMIN(15000,UMIN(exp_per_level(victim,victim->level),victim->exp/10)) : 0;
      if ( (paf = affect_to_obj(corpse, &af)) != NULL)
	string_to_affect(paf, victim->name);

      /* for future avengers */
      af.type = skill_lookup("avenger");
      af.level = 60;
      af.duration = -1;
      af.where = TO_NONE;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      /* modifier stores if it was an npc that killed the person */
      /* if it was npc then the npc gets an avenger too with matching. */
      /* id in modifier */
      if (IS_NPC(ch)){
	af.modifier = ch->id;
	if ( (paf = affect_to_obj(corpse, &af)) != NULL)
	  string_to_affect(paf, ch->name);
	affect_to_char(ch, &af);
      }
      else{
	af.modifier = 0;
      if ( (paf = affect_to_obj(corpse, &af)) != NULL)
	string_to_affect(paf, ch->name);
      }
    }

    if ( IS_NPC(victim) )
    {
      kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
      if (victim->summoner
	  && !IS_NPC(victim->summoner)
	  && victim->summoner->pcdata->pStallion
	  && victim->pIndexData == victim->summoner->pcdata->pStallion
	  && victim->summoner->in_room
	  && victim->in_room
	  && victim->in_room == victim->summoner->in_room){

	victim->summoner->pcdata->pStallion = NULL;
	act("You fall flat on your back!",victim->summoner,NULL,NULL,TO_CHAR);
	act("$n falls flat on $s back!",victim->summoner,NULL,NULL,TO_ROOM);
	victim->summoner->position = POS_RESTING;
      }
      if (victim->pIndexData->vnum >= MOB_VNUM_SERVANT_FIRE
	  && victim->pIndexData->vnum <= MOB_VNUM_SERVANT_EARTH
	  && victim->summoner != NULL
	  && !IS_NPC(victim->summoner))
 	{
	  if (is_affected(victim->summoner,gsn_familiar_link))
	    {
	      AFFECT_DATA faf;
	      send_to_char("Your energy drains out as your familiar dies.\n\r",victim->summoner);
	      affect_strip(victim->summoner,gsn_familiar_link);
	      faf.where	 = TO_AFFECTS;
	      faf.type         = gsn_drained;
	      faf.level        = victim->level;
	      faf.duration     = 12;
	      faf.modifier     = 0;
	      faf.location     = 0;
	      faf.bitvector    = 0;
	      affect_to_char(victim->summoner,&faf);
	    }
	  victim->summoner->pcdata->familiar=NULL;
	}
      //POSTKILLCHECK
      if (POSTKILL_CHECK(ch, victim) == -1)
	bug("Error at: POSTKILL_CHECK\n\r", 0);
      if (POSTDEATH_CHECK(ch, victim) == -1)
	bug("Error at: POSTDEATH_CHECK\n\r", 0);

      extract_char( victim, TRUE );
      return;
    }
    //A check just in case.
    if (IS_NPC(victim))
      {
        send_to_char("You turn into an invincible ghost for a few minutes.\n\r", victim);
        send_to_char("As long as you don't attack anything.\n\r", victim);
      }
    else
      {
	sendf(victim, "As the cold chill of death descends upon you %s grants you yet another chance.\n\r"\
	      "You have been granted a temporary sanctuary as an invincible ghost.\n\rAs long as you don't attack"\
	      " anything.\n\r", (victim->pcdata->deity[0] == '\0' ?  "The Creator" : victim->pcdata->deity));
      }

    //POSTKILLCHECK
    if (POSTKILL_CHECK(ch, victim) == -1)
      bug("Error at: POSTKILL_CHECK\n\r", 0);
    if (POSTDEATH_CHECK(ch, victim) == -1)
      bug("Error at: POSTDEATH_CHECK\n\r", 0);

    if ((victim->alignment > 0 && victim->in_room == get_room_index( hometown_table[victim->hometown].g_vnum ) )
	|| (victim->alignment == 0 && victim->in_room == get_room_index( hometown_table[victim->hometown].n_vnum ) )
	|| (victim->alignment < 0 && victim->in_room == get_room_index( hometown_table[victim->hometown].e_vnum ) ))
      is_lag = TRUE;


    extract_char( victim, FALSE );
    victim->summoner=NULL;

//strip effects.

    for (paf = victim->affected; paf; paf = paf_next ){
      paf_next = paf->next;
      if (paf->type == gsn_wanted
	  || paf->type == gsn_exile
	  || paf->type == gsn_mark_fear
	  || paf->type == gen_unlife
	  || (IS_GEN(paf->type) && IS_SET(effect_table[GN_POS(paf->type)].flags, EFF_F1_NODEATH))
	  || IS_GNBIT(paf->type, GN_NODEATH))

	continue;
      else
	affect_remove( victim, paf );
    }

    while (victim->affected2)
      song_affect_remove( victim, victim->affected2);
    clear_nskill(victim);
    if (victim->race == grn_vampire){
        free_string( victim->short_descr );
        victim->short_descr = str_dup ( "" );
        victim->dam_type = 17;
    }
    if (victim->race == grn_werebeast){
      victim->dam_type = 17;
    }
    victim->position	= POS_STANDING;
    victim->hit		= 1;
    victim->mana	= 1;
    victim->move	= UMAX( 100, victim->move );
    if (is_lag)
	WAIT_STATE2(victim,24);
    if (!IS_NPC(victim))
    {
        victim->pcdata->ghost = time(NULL);
	//unbloody the victim
	victim->pcdata->fight_delay = mud_data.current_time - 600;
	victim->pcdata->pk_delay = mud_data.current_time - 600;
	victim->pcdata->condition[COND_HUNGER]=48;
	victim->pcdata->condition[COND_THIRST]=48;
        if (is_affected(victim,gsn_familiar_link))
	{
	    send_to_char("Your energy drains out as you lose link with your familiar.\n\r",victim);
	    affect_strip(victim,gsn_familiar_link);
	}
	if (!IS_NPC(victim) && victim->pcdata->familiar != NULL)
	{
	    extract_char( victim->pcdata->familiar, TRUE );
	    victim->pcdata->familiar=NULL;
	}
        if (victim->pcdata->dall > 60 && get_trust(victim) < MASTER)
        {
	     send_to_char("You have died due to failing health.\n\r",victim);
	     sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( victim->name ) );
	     wiznet("$N dies due to failing health.",victim,NULL,0,0,0);
	     victim->pcdata->dall = 0;
             if (victim->level > 20)
	       deny_record(victim);
	     if (victim->pCabal){
	       CABAL_DATA* pCab = victim->pCabal;
	       update_cabal_skills(victim, victim->pCabal, TRUE, TRUE);
	       char_from_cabal( victim );
	       CHANGE_CABAL(pCab);
	       save_cabals( TRUE, NULL );
	     }
	     if (HAS_CLAN(victim)){
	       CharFromClan( victim );
	     }
	     if (victim->pcdata->pbounty){
	       rem_bounty( victim->pcdata->pbounty );
	       free_bounty( victim->pcdata->pbounty );
	     }
             extract_char( victim, TRUE );
             if ( victim->desc != NULL )
	       close_socket( victim->desc );
	     unlink(strsave);
	     return;
	}
        if ( !IS_MASTER(victim)
	     && victim->perm_stat[STAT_CON] > 3
	     && !fNoDeathCount){
	  if (victim->pcdata->dall%5 == 0)
            {
	      victim->perm_stat[STAT_CON]--;
	      send_to_char("You feel less healthy.\n\r",victim);
            }
	}
        if (tattoo != NULL) {
            obj_to_char(tattoo, victim);
            equip_char(victim, tattoo, WEAR_TATTOO);
        }
        if (crown != NULL) {
            obj_to_char(crown, victim);
            equip_char(victim, crown, WEAR_HEAD);
        }
	if (locker != NULL){
	  obj_to_char( locker, victim  );
	}
	/* Cabal Requip */
	cabal_requip(victim);
        save_char_obj( victim );
	if (fRes){
	  AFFECT_DATA* paf, af;
	  /* all seems ok, set an affect to let the player accept */
	  af.type = gsn_resurrection;
	  af.duration = 6;
	  af.level = victim->level;
	  af.where = TO_AFFECTS;
	  af.bitvector = 0;
	  af.location = APPLY_NONE;
	  af.modifier = 0;
	  paf = affect_to_char(victim, &af);
	  string_to_affect(paf, ch->name);
	  act("$n has called onto $g  to grant you a chance for a new life.\n\r"\
	      "Use \"accept\" to accept the resurrection.", ch, NULL, victim, TO_VICT);
	}
    }//END !IS_NPC(vic)
}

void raw_kill( CHAR_DATA *ch, CHAR_DATA *victim ){
  _raw_kill( ch, victim, TRUE );
}

void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *gch, *gch_next_in_room;
    int xp, members = 0, group_levels = 0;
    //    char buf[MAX_STRING_LENGTH];

    if ( victim == ch )
	return;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
      if ( is_same_group( gch, ch ) )
        {
	  if (IS_NPC(gch))
	    continue;
	  members++;
	  group_levels += gch->level;
	}

    if ( members == 0 && !IS_NPC(ch) )
    {
      bug( "Group_gain: members.", members );
      members = 1;
      group_levels = ch->level ;
    }

    for ( gch = ch->in_room->people; gch != NULL; gch = gch_next_in_room )
    {
	gch_next_in_room = gch->next_in_room;
	if ( !is_same_group( gch, ch ) || IS_NPC(gch))
	  continue;
        xp = xp_compute( gch, victim, group_levels, members );

	if (IS_PERK(gch, PERK_ADVENTURER))
	  xp = 115 * xp / 100;

	//DIETY CHECK
	if (!IS_NPC(gch) && !IS_IMMORTAL(gch) && gch->level >= 10 && gch->pcdata->way == 0) {
	  send_to_char("`@\n\rYou may not proceed past 10th rank until "\
		       "you choose your religion.\n\rUse <religion> for more "\
		       "info.``\n\r\n\r", gch);
	}
	else if (!IS_NPC(gch) && !IS_IMMORTAL(gch) && gch->level >= 15
		 && IS_SET(gch->act2, PLR_DESC)) {
	  send_to_char("`@\n\rYou may not proceed past 15th rank until "\
		       "your description has been approved."\
		       "\n\rUse <help desc> for more info.``\n\r\n\r", gch);

	}
	else if (!IS_GAME(gch, GAME_NOEXP))
	  {
	    sendf( gch, "You receive %d experience points.\n\r", xp );
	    gain_exp( gch, xp );
	  }

	if (IS_AFFECTED2(gch,AFF_RAGE))
	{
	    REMOVE_BIT(gch->affected2_by,AFF_RAGE);
	    affect_strip(gch,gsn_rage);
	    act("You snap out of your madness at the death of your opponent.",gch,NULL,NULL,TO_CHAR);
	    act("$n gasps for air as $e snaps out of $s insanity.",gch,NULL,NULL,TO_ROOM);
	    gch->hit -= (gch->level * gch->perm_stat[STAT_CON]) /3;
            if (IS_IMMORTAL(gch))
                gch->hit = UMAX(1,gch->hit);
	    if (gch->hit < -2)
            {
                int shock_mod = (25 * (0 - gch->hit)) / gch->max_hit;
		if (number_percent( ) < con_app[UMAX(1,gch->perm_stat[STAT_CON] - shock_mod)].shock)
                {
 		    act_new("Your body suddenly awakens to the wounds you've sustained and you lose consciousness.",gch,NULL,NULL,TO_CHAR,POS_DEAD);
		    act("$n suddenly clutches $s wounds and slumps to the ground, unconscious.",gch,NULL,NULL,TO_ROOM);
                    gch->hit = -2;
                    gch->position = POS_STUNNED;
		    act( "$n is stunned, but will probably recover.",gch, NULL, NULL, TO_ROOM );
        	    act_new("You are stunned, but will probably recover.",gch, NULL, NULL, TO_CHAR, POS_DEAD );
                }
                else
                {
		    act_new("Your body could not sustain the injuries you've suffered.",gch,NULL,NULL,TO_CHAR,POS_DEAD);
		    act("$n suddenly clutches $s wounds and slumps to the ground.",gch,NULL,NULL,TO_ROOM);
		    act( "$n is DEAD!!",gch, 0, 0, TO_ROOM );
		    act_new( "You have been `1KILLED``!!", gch,NULL,NULL,TO_CHAR,POS_DEAD);
                    raw_kill( gch, gch );
		}
	    }
	}

    }
}

int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim, int total_levels, int members )
{
    int xp, base_exp, level_range = victim->level - gch->level;
    if (IS_NPC(victim) && IS_SET(victim->act2,ACT_NOEXP))
	return 0;
    else if (IS_NPC(victim) && IS_SET(victim->act, ACT_RAIDER)){
      level_range = 9;
      members = UMAX(members, 3);
      total_levels = UMAX(total_levels, 3 * gch->level);
    }
    switch (level_range)
    {
 	default : 	base_exp =   0;		break;
	case -9 :	base_exp =   1;		break;
	case -8 :	base_exp =   2;		break;
	case -7 :	base_exp =   5;		break;
	case -6 : 	base_exp =   9;		break;
	case -5 :	base_exp =  11;		break;
	case -4 :	base_exp =  22;		break;
	case -3 :	base_exp =  33;		break;
	case -2 :	base_exp =  50;		break;
	case -1 :	base_exp =  66;		break;
	case  0 :	base_exp =  83;		break;
	case  1 :	base_exp = 105;		break;
	case  2 :	base_exp = 130;		break;
	case  3 :	base_exp = 165;		break;
	case  4 :	base_exp = 200;		break;
	case  5 :	base_exp = 240;		break;
	case  6 :	base_exp = 260;		break;
	case  7 :	base_exp = 265;		break;
	case  8 :	base_exp = 270;		break;
	case  9 :	base_exp = 270;		break;
    }
    if (level_range > 9)
      base_exp = 270;

    if (gch->in_room && IS_AREA(gch->in_room->area, AREA_MUDSCHOOL)){
      base_exp = 3 * base_exp;
    }

/* align based computations */
    if (IS_NPC(victim) && IS_SET(victim->act,ACT_NOALIGN))
      xp = base_exp;
    else if ((IS_EVIL(gch) && IS_GOOD(victim))
	     || (IS_EVIL(victim) && IS_GOOD(gch)))
      xp = base_exp * 8/5;
    else if ( IS_GOOD(gch)
	      && IS_GOOD(victim) && !IS_AFFECTED(victim, AFF_CHARM)
	      && !is_affected(gch, gsn_damnation)) {
      xp = -5000;
      if (!IS_NPC(victim) && IS_SET(victim->act,PLR_WANTED))
	xp = 0;
      else{
	sendf( gch, "The heavens frown upon your actions.\n\r");
	spell_damnation(gsn_damnation, gch->level, gch, gch,TARGET_CHAR);
      }
      return xp;
    }
    else if ( !IS_NEUTRAL(gch) && IS_NEUTRAL(victim) )
      xp = base_exp * 1.1;
    else if ( IS_NEUTRAL(gch) && !IS_NEUTRAL(victim) )
      xp = base_exp * 1.3;
    else xp = base_exp;


    /* randomize the rewards */
    xp = number_range (xp * 3/4, xp * 5/4);

    /* the xp is distributed based on level, high level characters in a group get bigger share */
    xp = xp * gch->level/total_levels;

    /* group bonuses */
    if (members == 2)
      xp = 2 * xp;
    else if (members > 2 && members < 5 )
      xp = 4 * xp;
    return xp;
}

inline char* parse_dam_message(CHAR_DATA* ch, CHAR_DATA* victim, CHAR_DATA* to,
			       OBJ_DATA* obj, const char* string, bool fMiss){
  static char * const him_her [] = { "it",  "him", "her" };
  static char * const his_her [] = { "its",  "his", "her" };
  static char dest[256];

/* color prefixes */
  static char* nor = "`1";	//normal red
  static char* com = "`3";	//common damage
  static char* oth = "`8";	//other damage
  static char* mis = "`6";	//miss

  const char* sptr = string;
  char* dptr = dest;
  const char* i;

  /* Init */
  dest[0] = '\0';

/* safety */
  if (!ch || !victim || !to)
    return dest;

/* begin the format */
  while (*sptr != '\0'){
    if (*sptr != '$'){
      *dptr++ = *sptr++;
      continue;
    }

    /* hit a format character */
    switch ( *++sptr ){
    default:	i = "< @@@ >";				break;
    case  'n':	i = PERS(ch, to);			break;
    case  'N':	i = PERS(victim, to);			break;
    case  'p':	i = obj ? (can_see_obj(to, obj) ? obj->short_descr :
      "something") : "< @@@ >";				break;
    case  'c':
      if (fMiss)
	i = mis;			/* MISS */
      else if (!IS_COLOR(to, COLOR_DAMAGE))
	i = nor;			/* NON COLORED DAMAGE */
      else if (victim == to)
	i  = nor;			/*  SHOWN TO VICTIM */
      else if (ch == to)
	i = com;			/*  SHOWN TO ATTACKER */
      else if (to->fighting && ch->fighting && to->fighting == ch->fighting
	       && is_same_group(ch, to))
	i = com;			/* SHOWN TO GROUP IF COMMON TARGET */
      else
	i = oth;			/* DAMAGE SHOWN TO EVERYONE ELSE */
      break;
    case  'm':  i = (ch->doppel != NULL && !IS_IMMORTAL(to)
		     && is_affected(ch, gsn_doppelganger) ) ?
		  him_her[URANGE(0,ch->doppel->sex,2)] : him_her[URANGE(0,ch->sex,2)];
    break;
    case 's':  i = (ch->doppel != NULL && !IS_IMMORTAL(to)
		    && is_affected(ch,gsn_doppelganger)) ?
		 his_her[URANGE(0,ch->doppel->sex,2)] :
		   his_her[URANGE(0,ch->sex,2)];
    break;
    }
    ++sptr;
    while ( (*dptr = *i) != '\0' ){
      ++dptr, ++i;
    }
  }
/* formatting done */
  *dptr++ = '\n';
  *dptr++ = '\r';
  *dptr++ = '\0';
  dest[0]   = UPPER(dest[0]);
  return dest;
}

void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA* obj, int dam, int dt, bool immune, bool fVirt, int sn, bool fShow ){
  char buf1[256], buf2[256], buf3[256];
  char ori_buf[MIL];
  int len;

  static ROOM_INDEX_DATA* room;
  static CHAR_DATA* vch, *vch_next;
  static Double_List * tmp_list;

  const char *vs, *vp, *attack;
  char punct;

/* origin strings (origin pointer poinst to these or ori_buf) */
  static char * ori_obj = "$p";
  static char * ori_ch  = "$n";
  char * origin = ori_ch;

  if (ch == NULL || victim == NULL)
    return;

  //DAMAGE VERB SELECTION
  if      ( dam ==   0 ) { vs = "miss``";       vp = "misses``";}
  else if ( dam <=   4 ) { vs = "scratch``";    vp = "scratches``";   }
  else if ( dam <=   8 ) { vs = "graze``";      vp = "grazes``";      }
  else if ( dam <=  12 ) { vs = "hit``";        vp = "hits``";        }
  else if ( dam <=  16 ) { vs = "injure``";     vp = "injures``";     }
  else if ( dam <=  20 ) { vs = "wound``";      vp = "wounds``";      }
  else if ( dam <=  24 ) { vs = "maul``";       vp = "mauls``";       }
  else if ( dam <=  28 ) { vs = "decimate``";   vp = "decimates``";   }
  else if ( dam <=  32 ) { vs = "devastate``";  vp = "devastates``";  }
  else if ( dam <=  36 ) { vs = "maim``";       vp = "maims``";       }
  else if ( dam <=  42 ) { vs = "MUTILATE``";   vp = "MUTILATES``";   }
  else if ( dam <=  52 ) { vs = "LACERATE``";   vp = "LACERATES``"; }
  else if ( dam <=  65 ) { vs = "DISMEMBER``";  vp = "DISMEMBERS``";  }
  else if ( dam <=  80 ) { vs = "MASSACRE``";   vp = "MASSACRES``";   }
  else if ( dam <=  100 ) { vs = "MANGLE``";    vp = "MANGLES``";     }
  else if ( dam <=  130 ) { vs = "*** DEMOLISH ***``";	vp = "*** DEMOLISHES ***``";         }
  else if ( dam <= 175)  { vs = "*** OBLITERATE ***``";       vp = "*** OBLITERATES ***``";        }
  else if ( dam <= 250)  { vs = "=== DISINTEGRATE ===``";    vp = "=== DISINTEGRATES ===``";		}
  else if ( dam <= 325)  {vs = ">>> ANNIHILATE <<<``";       vp = ">>> ANNIHILATES <<<``";}
  else if ( dam <= 400)  {vs = "<<< ERADICATE >>>``";        vp = "<<< ERADICATES >>>``";}
  else{vs = "do UNSPEAKABLE things to``"; vp = "does UNSPEAKABLE things to``";}


  //PUNCTUATION
  punct   = (dam <= 24) ? '.' : '!';
  //ORIGIN
  /* NOTICE: (viri)
     The origin of damage for non-obj virtual attacks is
     taken form the noun_damage of the sn.
     As both dt and sn are under your control this gives
     you quite a bit of control with picking origin (sn)
     damage text (dt)
  */
/* select origin based on fvirtual objects status */
  if (fVirt){
    if (obj != NULL)
      origin = ori_obj;
    else{
      strcpy(ori_buf, get_vir_attack(sn));
      origin = ori_buf;
    }
  }

/* no damage noun case */
  if ( dt == TYPE_HIT ){
    if (ch  == victim){
      if (fVirt){
	sprintf( buf1, "$n's %s $c%s $m%c",origin, vp, punct);
	sprintf( buf2, "Your %s $c%s you%c",origin,vp,punct);
      }
      else{
	sprintf( buf1, "$n $c%s $mself%c",vp, punct);
	sprintf( buf2, "You $c%s yourself%c",vs,punct);
      }
    }
    else{
      if (fVirt){
	sprintf( buf1, "$n's %s $c%s $N%c",origin,  vp, punct);
	sprintf( buf2, "Your %s $c%s $N%c",origin,  vs, punct);
	sprintf( buf3, "$n's %s $c%s you%c",origin, vp, punct);
      }
      else{
	sprintf( buf1, "$n $c%s $N%c",  vp, punct);
	sprintf( buf2, "You $c%s $N%c", vs, punct);
	sprintf( buf3, "$n $c%s you%c", vp, punct);
      }
    }
  }
  else{
    /* normal damage text case */
    if ( (attack = get_vir_attack(dt)) == NULL){
      bug( "Dam_message: bad dt %d.", dt );
      dt  = TYPE_HIT;
      attack  = attack_table[0].name;
    }

  //check if the damage noun is a plural, then we use singular verbs for both
  //ex: HELLSTREAM LACERATES but HELLSTREAMS LACERTATE
    len = strlen( attack );
    if (len > 0 && attack[ len - 1] == 's' && (len < 2 || attack[len - 2] != 's'))
      vp = vs;

/* case of immunity */
    if (immune){
      if (ch == victim){
	if (fVirt){
	  sprintf(buf1,"$n is unaffected by a %s's %s.",origin, attack);
	}
	else{
	  sprintf(buf1,"$n is unaffected by $s own %s.",attack);
	}
	sprintf(buf2,"Luckily, you are immune to that.");
      } //END IMMUNE SELF
      else{
	if (fVirt){
	  sprintf(buf1,"$N is unaffected by a %s's %s!",origin, attack);
	  sprintf(buf2,"$N is unaffected by your %s's %s!",origin, attack);
	  sprintf(buf3,"a %s's %s is powerless against you.",origin, attack);
	}
	else{
	  sprintf(buf1,"$N is unaffected by $n's %s!",attack);
	  sprintf(buf2,"$N is unaffected by your %s!",attack);
	  sprintf(buf3,"$n's %s is powerless against you.",attack);
	}
      }//END IUMMUNE TARGET
    }//END IMMUNE
    else{
/* normal damage */
      if (ch == victim){
	if (dt == 1052){
	  sprintf( buf1, "The sun's %s $c%s $n%c", attack, vp, punct);
	  sprintf( buf2, "The sun's %s $c%s you%c", attack, vp, punct);
	}
	else{
	  if (fVirt){
	    sprintf( buf1, "%s%s's %s $c%s $m%c",
		     obj == NULL ? "a ":"", origin,attack,vp, punct);
	    sprintf( buf2, "%s%s's %s $c%s you%c",
		     obj == NULL ? "a ":"", origin,attack,vp, punct);
	  }
	  else{
	    sprintf( buf1, "$n's %s $c%s $m%c",attack, vp, punct);
	    sprintf( buf2, "Your %s $c%s you%c",attack ,vp, punct);
	  }
	}//END HIT SELF
      }
      else{
	if (fVirt){
	  sprintf( buf1, "%s%s's %s $c%s $N%c",
		   obj == NULL ? "a ":"", origin,attack, vp, punct);
	  sprintf( buf2, "%s%s's %s $c%s $N%c",
		   obj == NULL ? "Your ":"", origin, attack, vp, punct);
	  sprintf( buf3, "%s%s's %s $c%s you%c",
		   obj == NULL ? "a ":"", origin, attack, vp, punct);
	}
	else{
	  sprintf( buf1, "$n's %s $c%s $N%c",  attack, vp, punct);
	  sprintf( buf2, "Your %s $c%s $N%c",  attack, vp, punct);
	  sprintf( buf3, "$n's %s $c%s you%c", attack, vp, punct);
	}
      }//END HIT TARGET
    }//END NON-IMMUNE
  }

/* SHOW THE MESSAGES HERE */
/*
   This is a simple version of ACT_NEW code, which only parses buf1-3
   for six things:
   $n = character
   $N = victim
   $m = character him_/her,
   $c = color chance to damage if needed.
*/

/* first send the messages to character and victim */
  if (IS_AWAKE(ch) && ch->desc && (ch->in_room == victim->in_room || fShow))
    send_to_char(parse_dam_message(ch, victim, ch, obj, buf2, dam == 0), ch);
  if (IS_AWAKE(victim) && victim->desc && ch != victim)
    send_to_char(parse_dam_message(ch, victim, victim, obj, buf3, dam == 0), victim);
/* conditions when room message is not shown */
  if (victim->in_room == NULL
/* Viri: Damage done about victim now,should not need this
      || (ch->in_room != victim->in_room && !fShow)	//remote damage etc.
*/
      || (dt == 1052 && dam < 40) )			//sun hitting vamps
    return;
  else
    room = victim->in_room;

/* print the messages to people in the room now */
  for (vch = room->people; vch != NULL; vch = vch_next){
    vch_next = vch->next_in_room;
    if (vch != ch && vch != victim && IS_AWAKE(vch) && vch->desc){
      send_to_char(parse_dam_message(ch, victim, vch, obj, buf1, dam == 0), vch);
    }
  }

/* watchrooms */
  if ( (tmp_list = room->watch_vnums) == NULL)
    return;
  while (tmp_list != NULL) {
    if ( (room = get_room_index ((long long)tmp_list->cur_entry)) != NULL){
      for (vch = room->people; vch != NULL; vch = vch_next){
	vch_next = vch->next_in_room;
	if (vch != victim && IS_AWAKE(vch) && vch->desc){
	  sendf (vch, "[%s] %s", ch->in_room->name,
		 parse_dam_message(ch, victim, vch, obj, buf1, dam == 0));
	}
      }//end for
    }//end get room
    tmp_list = tmp_list->next_node;
  }//end while

/* Viri: LEFT HERE FOR REFENCE
  if (ch == victim){
  if( dt != 1052 || (dt == 1052 && dam > 40))
  act(buf1,ch,NULL, NULL,TO_DAMCOLOR);
  act(buf2,ch,NULL,NULL,TO_CHAR);
  }
  else{
  if (ch->in_room == victim->in_room || fShow){
  act( buf1, ch, obj, victim, TO_DAMCOLOR );
  act( buf2, ch, obj, victim, TO_CHAR );
  }
  act( buf3, ch, obj, victim, TO_VICT );
  }
*/
}

void do_kill( CHAR_DATA *ch, char *argument )
{
    char buf[MSL], arg[MIL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Kill whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (ch == victim){
      send_to_char("Ouch!\n\r", ch);
      return;
    }
    if ( is_safe( ch, victim ) )
	return;
    /* JUSTICE GUARD CAPTAIN CHECK */
    if (IS_NPC(ch)
	&& (ch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD
	    || ch->pIndexData->vnum == MOB_VNUM_HOUND)
	&& ch->master && !IS_LEADER(ch->master)
	&& !IS_AREA(ch->in_room->area, AREA_LAWFUL)){
      if (ch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD)
	do_say(ch, "Do your own dirty work.  Sir." );
      else if (ch->pIndexData->vnum == MOB_VNUM_HOUND)
	check_social(ch, "whine", "");
      return;
    }
    if (IS_NPC(ch) && IS_SET(ch->act, ACT_NONCOMBAT)){
      return;
    }
    if (ch->position == POS_FIGHTING)
    {
	if (victim == ch->fighting)
	    send_to_char( "You do the best you can!\n\r",ch);
        else if ( !is_same_group(victim->fighting,  ch) )
            send_to_char( "One battle at a time.\n\r", ch );
	else
        {
	    sendf(ch,"You start aiming at %s.\n\r",PERS(victim,ch));
            ch->fighting = victim;
	}
	return;
    }
    if (is_sheathed(ch)){
      send_to_char("Your weapons are sheathed!\n\r", ch);
      return;
    }
    if ( !IS_NPC(victim) )
    {
        send_to_char( "You must MURDER a player.\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
	send_to_char( "You hit yourself.  Ouch!\n\r", ch );
	return;
    }
    if ( is_safe( ch, victim ) )
	return;
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch));
	return;
    }
    WAIT_STATE2( ch, 1 * PULSE_VIOLENCE );
    sprintf(buf, "Help! I am being attacked by %s!",PERS(ch,victim));
    j_yell(victim, buf );
    a_yell(ch,victim);
    multi_hit( ch, victim, TYPE_UNDEFINED );
}

void do_murde( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to MURDER, spell it out.\n\r", ch );
}

void do_murder( CHAR_DATA *ch, char *argument )
{
    char buf[MSL], arg[MIL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Murder whom?\n\r", ch );
	return;
    }
    if (IS_AFFECTED(ch,AFF_CHARM) || (IS_NPC(ch) && IS_SET(ch->act,ACT_PET)))
	return;
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "Suicide is a mortal sin.\n\r", ch );
	return;
    }
    if ( is_safe( ch, victim ) )
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
	sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch));
	return;
    }
    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "You do the best you can!\n\r", ch );
	return;
    }
    if (is_sheathed(ch)){
      send_to_char("Your weapons are sheathed!\n\r", ch);
      return;
    }
    WAIT_STATE2( ch, 1 * PULSE_VIOLENCE );
    sprintf(buf, "Help! I am being attacked by %s!",PERS(ch,victim));
    j_yell(victim, buf);
    a_yell(ch,victim);
    multi_hit( ch, victim, TYPE_UNDEFINED );
    if (ch->fighting == victim && !is_ghost(victim, 600) && get_skill(ch, gsn_throw) > 84){
      throw_effect(ch, victim, FALSE);
    }

}

//generates a random list of doors to check in room
//does not check for anything other then existance of an exit
int* gen_door_order( ROOM_INDEX_DATA* room, int* max_door ){
  static int doors[MAX_DOOR];
  int temp, i, j, max_doors = 0;

  memset( doors, 0, sizeof( int ) * MAX_DOOR );
  *max_door = 0;

  /*1) Fill doors with exits in the room */
  for (i = 0; i < MAX_DOOR; i++){
    if (room->exit[i] != NULL)
      doors[max_doors++] = i;
  }
  if (max_doors < 1)
    return NULL;

  /* randomly swap each spot for another spot */
  for (i = 0; i < max_doors; i++){
    j = rand() % max_doors;
    temp = doors[i];
    doors[i] = doors[j];
    doors[j] = temp;
  }
  *max_door = max_doors;
  return doors;
}

//tries to flee a character from his room into given direction
bool flee_char( CHAR_DATA*  ch, CHAR_DATA* victim, int door, bool fRetreat, bool fShadow ){
  EXIT_DATA* pexit = ch->in_room->exit[door];
  char* str_flee = "flee";
  char* str_fled = "fled";

  if (pexit == NULL || pexit->to_room == NULL)
    return FALSE;
  else if (!fShadow
	   && IS_SET(pexit->exit_info, EX_CLOSED)
	   && !IS_AFFECTED(ch, AFF_PASS_DOOR))
    return FALSE;
  else if ( IS_NPC(ch) && IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB))
    return FALSE;

  /* CAN FLEE */
  stop_fighting( ch, TRUE );

  if (fRetreat){
    str_flee = "retreat";
    str_fled = "retreated";
  }
  act( "$n has $t!", ch, str_fled, NULL, TO_ROOM );

  if ( !IS_NPC(ch) ){
    if (ch->level > 34 && ch->class == class_lookup("warrior"))
      sendf(ch, "You %s %s from combat!\n\r", str_flee, dir_name[door]);
    else
      sendf(ch,  "You %s from combat!\n\r", str_flee );
    send_to_char( "You lost 10 exp.\n\r", ch);
    gain_exp( ch, -10 );
  }

  move_char( ch, door, TRUE );
  if (ch->in_room == NULL)
    return TRUE;

  if ( IS_NPC(victim)
       && !IS_SET(victim->act2, ACT_LAWFUL)
       && !IS_SET(victim->off_flags,CABAL_GUARD)
       && !IS_SET(victim->act, ACT_RAIDER)
       && !IS_AFFECTED(ch, AFF_CHARM)
       && victim->pIndexData->vnum != MOB_VNUM_ALTAR
       && !IS_AFFECTED(victim, AFF_CHARM)){
    if (!IS_NPC(ch))
      victim->hunting = ch;
    else if (ch->master != NULL)
      victim->hunting = ch->master;
    else if (ch->leader != NULL)
      victim->hunting = ch->leader;
    else
      victim->hunting = ch;
    victim->hunttime = 0;
  }
  return TRUE;
}

void do_flee( CHAR_DATA *ch, char *argument ){
  ROOM_INDEX_DATA *was_in;
  CHAR_DATA *victim, *och;
  AFFECT_DATA* paf;
  int retreat = get_skill(ch, skill_lookup("retreat"));
  int i, max_doors = 0, *doors, door = -1, flee_door;
  int chance;
  bool fRetreat = FALSE;
  bool fShadow = FALSE;

  if ( ( victim = ch->fighting ) == NULL ){
    if ( ch->position == POS_FIGHTING )
      ch->position = POS_STANDING;
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }
  else if (IS_AFFECTED2(ch, AFF_RAGE)){
    send_to_char("You can't flee, you're fighting to the DEATH!\n\r",ch);
    return;
  }

  if (is_affected(ch, gsn_shadow_door))
    fShadow = TRUE;

  /* flee failures */
  chance = 35 + 3 * (16 - get_curr_stat(ch,STAT_LUCK));

  if (IS_SET(ch->act, PLR_DOOF) && number_percent() < chance){
    send_to_char("You attempt to flee, but the gods only laugh at your misery.\n\r",ch);
    WAIT_STATE2(ch, PULSE_VIOLENCE);
    return;
  }
  else if (!fShadow
	   && (paf = affect_find(ch->affected, gsn_wlock)) != NULL){
    act("You've been weapon locked!", ch, NULL, victim, TO_CHAR);
    act("$n tries to flee but $s weapon is all tangled up!", ch, NULL, NULL, TO_ROOM);
    WAIT_STATE2(ch, PULSE_VIOLENCE );
    if (--paf->modifier < 1)
      check_dispel(-99, ch, gsn_wlock);
    return;
  }
  else if (!fShadow
	   && is_affected(ch,gsn_hysteria)
	   && number_percent() < chance){
    send_to_char("You panic and couldn't escape!\n\r",ch);
    WAIT_STATE2(ch, PULSE_VIOLENCE );
    return;
  }
  else if (!fShadow && (paf = affect_find(ch->affected, gsn_leash) ) != NULL
	   && paf->has_string
	   && !IS_NULLSTR(paf->string)
	   && (och = get_char( paf->string)) != NULL){
    if (--paf->modifier < 1){
      check_dispel( -99, ch, gsn_leash );
    }
    WAIT_STATE2(ch, PULSE_VIOLENCE / 3 );
    damage(och, ch, 3, gsn_leash, DAM_INTERNAL, FALSE );
    return;
  }
  else
    WAIT_STATE2(ch, PULSE_VIOLENCE / 3);

  /* RETREAT */
  /* check if we are fleeing or retreating */
  if (number_percent() < 4 * retreat / 5){
    fRetreat = TRUE;
    check_improve(ch, skill_lookup("retreat"), TRUE, 1);
  }
  else
    check_improve(ch, skill_lookup("retreat"), FALSE, 1);

  if (retreat > 1 && !IS_NULLSTR(argument)){
    if      ( !str_cmp( argument, "n" )
	      || !str_cmp( argument, "north" ) ) door = DIR_NORTH;
    else if ( !str_cmp( argument, "s" )
	      || !str_cmp( argument, "south" ) ) door = DIR_SOUTH;
    else if ( !str_cmp( argument, "e" )
	      || !str_cmp( argument, "east" ) ) door = DIR_EAST;
    else if ( !str_cmp( argument, "w" )
	      || !str_cmp( argument, "west" ) ) door = DIR_WEST;
    else if ( !str_cmp( argument, "u" )
	      || !str_cmp( argument, "up" ) ) door = DIR_UP;
    else if ( !str_cmp( argument, "d" )
	      || !str_cmp( argument, "down" ) ) door = DIR_DOWN;
    else
      door = -1;
    }
  //remember the room we are in
  was_in = ch->in_room;

  doors = gen_door_order( was_in, &max_doors );

  for ( i = 0; i < max_doors; i++ ){
    /* if we tried to retreat in direction try that once first */
    if (door > -1){
      flee_door = door;
      door = i = -1;
    }
    else
      flee_door = doors[i];

    /* flat out chance of failing */
    if (!fShadow && !fRetreat && number_percent() < (100 / (1 + max_doors)))
      continue;

//DEBUG    sendf(ch, "Trying %s.\n\r", dir_name[flee_door]);

    if (!flee_char( ch, victim, flee_door, fRetreat, fShadow ) || ch->in_room == was_in)
      continue;
    //check if we are alive still

    /* SUCESS we escaped! */
    //get shadow door to invis us immediatly
    if (fShadow)
      spell_shadow_door( gsn_shadow_door, ch->level, ch, ch, -1 );
    else
      WAIT_STATE2(ch, PULSE_VIOLENCE / 3);

    if ( IS_NPC(ch) && IS_SET(ch->special,SPECIAL_COWARD)
	 && ch->summoner != NULL){
      REMOVE_BIT(ch->special,SPECIAL_COWARD);
      SET_BIT(ch->special,SPECIAL_RETURN);
    }
    /* remove stand ground */
    affect_strip(ch, gsn_stand_ground );

    if (!is_safe_quiet(ch, victim)){
      /* See the character tries to flee, we attempt a backcutter */
      if (backcutter_effect(victim, ch, fRetreat || fShadow))
	return;
      if (get_skill(victim, gsn_throw) > 90
	  && throw_effect(victim, ch, fRetreat || fShadow))
	return;
    }
    return;
  }
  send_to_char( "You couldn't escape!\n\r", ch );
}

void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
}

void do_sever( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
       send_to_char("Who are you going to rip in half?\n\r",ch);
       return;
    }

    if ( ( victim = get_char_room( ch, NULL,arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if(victim == ch)
    {
	send_to_char("You reach your neck fine, but can't get your ankles.\n\r",ch);
	return;
    }

    if(IS_AFFECTED2(victim, AFF_SEVERED))
    {
	send_to_char("They have no legs to rip off.\n\r",ch);
	return;
    }
    if ( !IS_NPC(victim) && victim->level >= get_trust(ch) && ch != victim)
    {
        sendf(ch, "You failed to slay %s.\n\r", PERS2(victim) );
        return;
    }
    SET_BIT(victim->affected2_by,AFF_SEVERED);
    set_delay(victim, ch);

    act( "You rip off $N's arms and legs in total fury!",  ch, NULL, victim, TO_CHAR);
    act_new( "`!$n rips off both of your arms and legs!``", ch, NULL, victim, TO_VICT, POS_DEAD );
    act( "`!$n tears off $N's arms and legs in a fit of rage!``",  ch, NULL, victim, TO_NOTVICT );
    sprintf(buf,"GAAAAAAAAAAHHH!!!!");
    do_yell(victim, buf);
    obj = create_object( get_obj_index(OBJ_VNUM_LEGS ), 0 );
    obj->timer = 5;

if(IS_NPC(victim))
    sprintf(buf,"A pair of %s's arms are here, twitching.",victim->short_descr);
else
    sprintf(buf,"A pair of %s's arms are here, twitching.",victim->name);
    free_string(obj->description);
    obj->description = str_dup( buf );

if(IS_NPC(victim))

    sprintf(buf,"A pair of %s's arms",victim->short_descr);
else
    sprintf(buf,"A pair of %s's arms",victim->name);
    free_string(obj->short_descr);
    obj->short_descr = str_dup( buf );
    obj_to_char(obj,ch);
}

void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MIL];
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, NULL,arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim == ch && ch->desc == NULL )
	return;
    if ( !IS_NPC(victim) && victim->level >= get_trust(ch) && ch != victim )
    {
	sendf(ch, "You failed to slay %s.\n\r", PERS2(victim) );
	return;
    }

    act( "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR    );
    act_new( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT, POS_DEAD    );
    act( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
    raw_kill( ch, victim );
}

void do_qslay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MIL];
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Slay whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, NULL,arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim == ch && ch->desc == NULL )
	return;
    if ( !IS_NPC(victim) && victim->level >= get_trust(ch) && ch != victim )
    {
	sendf(ch, "You failed to slay %s.\n\r", PERS2(victim) );
	return;
    }
    act( "You slay $M quietly.",  ch, NULL, victim, TO_CHAR    );
    raw_kill( ch, victim );
}

void do_smite( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MIL];
    char buf1[MIL];
    char buf2[MIL];
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Smite whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( !IS_NPC(victim) && victim->level > get_trust(ch) && ch != victim )
    {
	sendf(ch, "You failed to smite %s.\n\r", PERS2(victim) );
	return;
    }
    act( "You send a thunderbolt to smite $N!",  ch, NULL, victim, TO_CHAR    );
    if (argument[0] != '\0')
    {
	sendf( victim, "%s\n\r", argument);
	sprintf(buf1, "%s", victim->name);
	sprintf(buf2, "%s's", victim->name);
	str_replace( argument, "Your", buf2);
	str_replace( argument, "You", buf1);
	act( argument, ch, NULL, victim, TO_NOTVICT);
    }
    else
    {
    	send_to_char( "A bolt from the heavens smites you!\n\r",victim);
    	act( "A bolt from the heavens smites $N!",  ch, NULL, victim, TO_NOTVICT );
    }
    victim->hit/=2;
}

/* oengod psalm */
CHAR_DATA* onegod_check_guard(CHAR_DATA* ch, CHAR_DATA* mob)
{
  const int MAX_CHS = 12;

  CHAR_DATA* och;
  CHAR_DATA* och_next;

  CHAR_DATA* chs[MAX_CHS];

  int tot_chs = 0;

/* Put all avaliable saving chars in an array */
  for ( och = ch->in_room->people; och != NULL; och = och_next ){
    och_next = och->next_in_room;

    if (IS_NPC(och)
	|| och->hit < och->max_hit / 3
	|| och->fighting == NULL
	|| ch->fighting == NULL
	|| och->fighting != ch->fighting
	|| och->fighting == ch
	|| ch->fighting == och
	|| och == mob
	|| och == ch
	|| IS_AFFECTED(och, AFF_CHARM)
	|| !is_same_group(ch, och)
	|| !can_see(och, ch)
	|| !is_affected(och, gsn_ogtrium)
	)
      continue;
    chs[tot_chs++] = och;
  }//end for

  /* now we pick one character out of all possible choices */
  if (tot_chs == 0)
    return NULL;
  /* we return a random pick */
  och =  chs[number_range(0, UMAX(0, tot_chs - 1))];
  return och;
}

CHAR_DATA *check_guard(CHAR_DATA *ch, CHAR_DATA *mob)
{
    int chance;
    CHAR_DATA *och, *och_next, *sub = NULL;


/* Ranger MindLink */
    if (!IS_NPC(ch))
      chance = (get_skill(ch,gsn_mind_link) / 3 + (1.5 * (ch->level - mob->level)));
    else
      chance = 0;

    if (number_percent() < UMIN(95, chance)){
      /* Find any beasts */
      for ( och = ch->in_room->people; och != NULL; och = och_next ){
	och_next = och->next_in_room;
	if ( IS_AFFECTED(och, AFF_CHARM) && IS_NPC(och)
	     && och->master == ch
	     && can_see(och, ch)
	     && och->fighting != ch
	     && och->pIndexData->vnum == MOB_VNUM_MAMMOTH)
	  {sub = och;break;}
      }//End If is guarding

      /* Check if any found if not continue to next check*/
      if (sub != NULL)
	{
	  act("$n jumps in front of $N!",sub,NULL,ch,TO_NOTVICT);
	  act("$n jumps in front of you!",sub,NULL,ch,TO_VICT);
	  act("You jump in front of $N!",sub,NULL,ch,TO_CHAR);
	  check_improve(ch,gsn_mind_link,TRUE,1);
	  return sub;
	}
    }
/* GUARD CAPTAIN */
    if (!IS_NPC(ch) && ch->pet && ch->pet->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD
	&& ch->pet->fighting && number_percent() < 25){
      sub = ch->pet;
      act("$n jumps in front of $N!",sub,NULL,ch,TO_NOTVICT);
      act("$n jumps in front of you!",sub,NULL,ch,TO_VICT);
      act("You jump in front of $N!",sub,NULL,ch,TO_CHAR);
      return sub;
    }

/* Virigoth Tattoo */
    if (ch->tattoo == deity_lookup("cycle"))
      chance = ch->level + 3 * (ch->level - mob->level);
    else
      chance = 0;
    if (number_percent() < UMIN(95, chance)){
      sub = undead_check_guard(ch, mob);
      /* Check if any found if not continue to next check*/
      if (sub != NULL)
	{
	  act("$n jumps in front of $N!",sub,NULL,ch,TO_NOTVICT);
	  act("$n jumps in front of you!",sub,NULL,ch,TO_VICT);
	  act("You jump in front of $N!",sub,NULL,ch,TO_CHAR);
	  return sub;
	}
    }

/* One God Psalm */
    if (!IS_NPC(ch) && number_percent() < 20){
      sub = onegod_check_guard(ch, mob);
      /* Check if any found if not continue to next check*/
      if (sub != NULL)
	{
	  act("$n steps in front of $N!",sub,NULL,ch,TO_NOTVICT);
	  act("$n steps in front of you!",sub,NULL,ch,TO_VICT);
	  act("You step in front of $N!",sub,NULL,ch,TO_CHAR);
	  return sub;
	}
    }
/* Guard Skill */
    if (!IS_NPC(ch) && ch->pcdata->guarded_by != NULL
	&& ch->in_room == ch->pcdata->guarded_by->in_room
	&& can_see(ch->pcdata->guarded_by, ch))
      chance = get_skill(ch->pcdata->guarded_by, gsn_guard);
    else
      chance = 0;
    if (number_percent() < UMIN(100,chance))
      {
	if (number_percent() < ch->level  + 1.5 * (ch->level - mob->level))
	  {
	    act("$n jumps in front of $N!",ch->pcdata->guarded_by,NULL,ch,TO_NOTVICT);
	    act("$n jumps in front of you!",ch->pcdata->guarded_by,NULL,ch,TO_VICT);
	    act("You jump in front of $N!",ch->pcdata->guarded_by,NULL,ch,TO_CHAR);
	    check_improve(ch->pcdata->guarded_by,gsn_guard,TRUE,1);
	    return ch->pcdata->guarded_by;
	  }
	else
	  {
	    check_improve(ch->pcdata->guarded_by, gsn_guard, FALSE, 1);
	    return ch;
	  }
      }//end guard success
    return ch;
}///end check guard


void mortal_wound( CHAR_DATA *och, CHAR_DATA *victim )
{
    CHAR_DATA* ch = och;
    AFFECT_DATA af;
    char buf[MSL];

    /* check for npc/master */
    if (IS_NPC(och)){
      if (och->master)
	ch = och->master;
      else{
	ch = och;
      }
    }
    if (IS_NPC(victim))
      return;


    stop_fighting( victim, TRUE );
    stop_fighting( och, TRUE);

    if (IS_AFFECTED2(victim,AFF_RAGE))
    {
    	REMOVE_BIT(victim->affected2_by,AFF_RAGE);
        affect_strip(victim,gsn_rage);
    }
    affect_strip(victim,gen_firestorm);
    affect_strip(victim,gen_anger);
    affect_strip(victim,gsn_poison);
    affect_strip(victim,gen_bleed);
    affect_strip(victim,gsn_plague);
    affect_strip(victim,gsn_insect_swarm);
    affect_strip(victim,skill_lookup("visitation"));
    affect_strip(victim,skill_lookup("dysentery"));

    victim->pcdata->condition[COND_HUNGER]=15;
    victim->pcdata->condition[COND_THIRST]=15;
    victim->hit = 1;
    victim->mana = 1;
    victim->position = POS_RESTING;
    die_follower(victim, FALSE);

    if (!IS_NPC(ch)){
      if (is_challenger(ch,victim))
	{
	  if (ch->pCabal && IS_SET(ch->pCabal->progress, PROGRESS_CHALL)){
	    update_challenge( ch, ch->name, victim->name, 1, 0, 0, 0 );
	    sendf(ch, "You've gained some worth in eyes of [%s]!\n\r",
		  ch->pCabal->who_name);
	    CP_GAIN( ch, number_range(15, 50), TRUE );
	  }
	  if (victim->pCabal && IS_SET(victim->pCabal->progress, PROGRESS_CHALL)){
	    update_challenge( victim, victim->name, ch->name, 0, 1, 0, 0 );
	    sendf(victim, "You've gained some worth in eyes of [%s]!\n\r",
		  victim->pCabal->who_name);
	    CP_GAIN( victim, number_range(15, 50), TRUE );
	  }
	  save_challenges();
	  remove_challenge(ch,victim);
	  send_to_char("You have won your challenge.\n\r",ch);
	  send_to_char("You have lost the challenge.\n\r",victim);
	  send_to_char("`!FINISH THEM!``\n\r",ch);

	}
      if (ch->pCabal){
	sprintf(buf,"%s has claimed victory over %s.", PERS2(ch), PERS2(victim));
	cabal_echo(ch->pCabal, buf);
      }
      if (victim->pCabal){
	sprintf(buf,"%s has been defeated by %s.",PERS2(victim),PERS2(ch));
	cabal_echo(victim->pCabal, buf);
      }
    }
    if (IS_AFFECTED2(ch,AFF_RAGE))
      {
	REMOVE_BIT(ch->affected2_by,AFF_RAGE);
        affect_strip(ch,gsn_rage);
	act("You snap out of your madness at the death of your opponent.",ch,NULL,NULL,TO_CHAR);
	act("$n gasps for air as $e snaps out of $s insanity.",ch,NULL,NULL,TO_ROOM);
	ch->hit -= (ch->level * ch->perm_stat[STAT_CON]) /3;
        if (IS_IMMORTAL(ch))
            ch->hit = UMAX(1,ch->hit);
	if (ch->hit < -2)
        {
            int shock_mod = (25 * (0 - ch->hit)) / ch->max_hit;
	    if (number_percent( ) < con_app[UMAX(1,ch->perm_stat[STAT_CON] - shock_mod)].shock)
            {
 		act_new("Your body suddenly awakens to the wounds you've sustained and you lose consciousness.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
		act("$n suddenly clutches $s wounds and slumps to the ground, unconscious.",ch,NULL,NULL,TO_ROOM);
                ch->hit = -2;
                ch->position = POS_STUNNED;
		act( "$n is stunned, but will probably recover.",ch, NULL, NULL, TO_ROOM );
        	act_new("You are stunned, but will probably recover.",ch,NULL,NULL,TO_CHAR,POS_DEAD );
            }
            else
            {
		act_new("Your body could not sustain the injuries you've suffered.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
		act("$n suddenly clutches $s wounds and slumps to the ground.",ch,NULL,NULL,TO_ROOM);
		act( "$n is DEAD!!",ch, 0, 0, TO_ROOM );
		act_new( "You have been `1KILLED``!!", ch,NULL,NULL,TO_CHAR,POS_DEAD);
		raw_kill( ch, ch );
	    }
	}
      }
    af.type             = gsn_mortally_wounded;
    af.level            = victim->level;
    af.duration         = IS_NPC(ch) ? 4 : 2;
    af.location         = APPLY_NONE;
    af.modifier         = 0;
    af.bitvector        = 0;
    affect_to_char(victim,&af);
    send_to_char("You are too hurt to do anything for a while.\n\r",victim);
}

void mercy_effect( CHAR_DATA *och, CHAR_DATA *victim, int dur )
{
    CHAR_DATA* ch = och;
    AFFECT_DATA af;

    /* check for npc/master */
    if (IS_NPC(och)){
      if (och->master)
	ch = och->master;
      else{
	ch = och;
      }
    }


    stop_fighting( victim, TRUE );

    if (IS_AFFECTED2(victim,AFF_RAGE))
    {
    	REMOVE_BIT(victim->affected2_by,AFF_RAGE);
        affect_strip(victim,gsn_rage);
    }
    affect_strip(victim,gsn_poison);
    affect_strip(victim,gen_bleed);
    affect_strip(victim,gsn_plague);
    affect_strip(victim,gsn_insect_swarm);
    affect_strip(victim,skill_lookup("visitation"));
    affect_strip(victim,skill_lookup("dysentery"));

    victim->pcdata->condition[COND_HUNGER]=15;
    victim->pcdata->condition[COND_THIRST]=15;
    victim->hit = 1;
    victim->mana = 1;
    victim->position = POS_RESTING;
    die_follower(victim, FALSE);


    if (IS_AFFECTED2(ch,AFF_RAGE))
      {
	REMOVE_BIT(ch->affected2_by,AFF_RAGE);
        affect_strip(ch,gsn_rage);
	act("You snap out of your madness at the death of your opponent.",ch,NULL,NULL,TO_CHAR);
	act("$n gasps for air as $e snaps out of $s insanity.",ch,NULL,NULL,TO_ROOM);
	ch->hit -= (ch->level * ch->perm_stat[STAT_CON]) /3;
        if (IS_IMMORTAL(ch))
            ch->hit = UMAX(1,ch->hit);
	if (ch->hit < -2)
        {
            int shock_mod = (25 * (0 - ch->hit)) / ch->max_hit;
	    if (number_percent( ) < con_app[UMAX(1,ch->perm_stat[STAT_CON] - shock_mod)].shock)
            {
 		act_new("Your body suddenly awakens to the wounds you've sustained and you lose consciousness.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
		act("$n suddenly clutches $s wounds and slumps to the ground, unconscious.",ch,NULL,NULL,TO_ROOM);
                ch->hit = -2;
                ch->position = POS_STUNNED;
		act( "$n is stunned, but will probably recover.",ch, NULL, NULL, TO_ROOM );
        	act_new("You are stunned, but will probably recover.",ch,NULL,NULL,TO_CHAR,POS_DEAD );
            }
            else
            {
		act_new("Your body could not sustain the injuries you've suffered.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
		act("$n suddenly clutches $s wounds and slumps to the ground.",ch,NULL,NULL,TO_ROOM);
		act( "$n is DEAD!!",ch, 0, 0, TO_ROOM );
		act_new( "You have been `1KILLED``!!", ch,NULL,NULL,TO_CHAR,POS_DEAD);
		raw_kill( ch, ch );
	    }
	}
      }
    af.type             = gsn_mortally_wounded;
    af.level            = victim->level;
    af.duration         = dur;
    af.location         = APPLY_NONE;
    af.modifier         = 0;
    af.bitvector        = 0;

    if (!is_affected(victim, af.type))
      affect_to_char(victim,&af);

    //anti abuse check for people mercing someone over and over again
    af.type = gsn_mercy_wait;
    af.duration = dur + 1;
    affect_strip(victim, af.type );
    affect_to_char(victim, &af);

    send_to_char("You are too hurt to do anything for a while.\n\r",victim);
}


/* returns if challenger can fight yet */
bool challenge_safe(CHAR_DATA* ch, CHAR_DATA* victim){

  if ( !IS_NPC(ch) && !IS_NPC(victim) && is_challenger(ch, victim)){
    AFFECT_DATA* paf;
    for (paf = ch->affected; paf; paf = paf->next){
      if (paf->type != gen_challenge || paf->modifier < 1)
	continue;
      if (str_cmp(victim->name, paf->string))
	continue;
      return TRUE;
    }
  }
  return FALSE;
}

/* returns if character is in challange with victim */
bool is_challenger( CHAR_DATA *och, CHAR_DATA *victim )
{
  CHAR_DATA* ch = och;
  AFFECT_DATA* paf;
  bool fCh = FALSE;	//check if victim name found on character
  bool fVch = FALSE;	//check if character name found on victim

    if (IS_NPC(victim))
      return FALSE;
    if (IS_NPC(och)){
      if (och->master && !IS_NPC(och->master))
	ch = och;
      else
	return FALSE;
    }
    /* check if ch has challenged victim */
    for (paf = ch->affected; paf; paf = paf->next){
      if (paf->type != gen_challenge
	  || !paf->has_string)
	continue;
      if (str_cmp(victim->name, paf->string))
	continue;
      fCh = TRUE;
      break;
    }
    if (!fCh)
      return FALSE;

    /* check if victim has challenged ch */
    for (paf = victim->affected; paf; paf = paf->next){
      if (paf->type != gen_challenge
	  || !paf->has_string)
	continue;
      if (str_cmp(ch->name, paf->string))
	continue;
      fVch = TRUE;
      break;
    }
    if (!fVch)
      return FALSE;

    return TRUE;
}
void remove_challenge( CHAR_DATA *ch, CHAR_DATA *victim){
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  /* we scan through effects knocking out the paf */
  /* the gen will attempt to knock the other path automaticly */
  for (paf = ch->affected; paf; paf = paf_next){
    paf_next = paf->next;
    if (paf->type != gen_challenge
	|| !paf->has_string)
      continue;
    if (str_cmp(victim->name, paf->string))
      continue;
    affect_remove(ch, paf);
  }
}

/////////////////////
//get_spell_damage //
////////////////////
//Calculates the damage that will be done to target
//returns FALSE if immune or not affected by that particular dam_type
//Returns -1 if damge is not allowed (ie: safe rooms etc.)
/*
!!!NOTE!!!

This function does not duplicate damage done by phantasmal stuff
as the damage was calculated INSIDE the damage function, not by
a spell or skill function outside.
That will have to be changed later.
*/


int get_spell_damage(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dam_type)
{
  //Vars needed for the below
  AFFECT_DATA *paf;//effects

//THINGS THAT ARE NOT IMMUNE BUT DO NOT TAKE DAMAGE

  //DEAD
    if ( victim->position == POS_DEAD)
	return FALSE;

  //LIMBO
    if (ch->in_room == NULL || ch->in_room->vnum == ROOM_VNUM_LIMBO)
	return -1;

  //MIRROR IMAGE
    if (IS_NPC(victim) && is_affected(victim,gsn_mirror_image))
	return -1;

  //DECOY
    if (IS_NPC(victim) && is_affected(victim,gsn_decoy))
	return -1;



//THINGS THAT CAUSE IMMUNITY


  //MANA SHIELD
    if (IS_AFFECTED2(victim, AFF_MANA_SHIELD) && dam_type > 3)
    {
	if (dam / 5 <= victim->mana)
	  dam = FALSE;
	else
	    UMIN(0, dam -= victim->mana * (victim->level/5));
    }

  //BARRIER
    if (dam > 1 && IS_AFFECTED2(victim, AFF_BARRIER) && dam_type <= 3)
    {
	if (dam / 2 <= victim->mana)
	  dam = FALSE;
	else
	    UMIN(0, dam -= victim->mana * (victim->level/2));
    }


//---------END EASY CASES    CALCULATE DAMAGE    -----------//
//These are the standard modifiers to damage.
    if ( dam > 1 && !IS_NPC(victim) && victim->pcdata->condition[COND_DRUNK]  > 10 )	dam = dam * 9 / 10;
    if ( dam > 1 && is_affected(victim, gsn_pyramid_of_force) )			        dam = 3 * dam / 5;
    if ( dam > 1 && is_affected(victim, gsn_mental_barrier) && dam_type == DAM_MENTAL)	dam -= dam / 4;
    if ( dam > 1 && IS_AFFECTED(victim, AFF_SANCTUARY) )	dam /= 2;
    if ( dam > 1 && !IS_NPC(ch) && IS_SET(ch->act2,PLR_GIMP) )			        dam /= 2;
    if ( dam > 1 && IS_AFFECTED2(victim, AFF_ICE_SHIELD) && dam_type == DAM_FIRE)       dam /= 2;
    if ( dam > 1 && IS_AFFECTED2(victim, AFF_FIRE_SHIELD) && dam_type == DAM_COLD)      dam /= 2;
    if ( dam > 1 && IS_AFFECTED2(victim, AFF_ICE_SHIELD) && dam_type == DAM_COLD)	dam = dam * 3 / 2;
    if ( dam > 1 && IS_AFFECTED2(victim, AFF_FIRE_SHIELD) && dam_type == DAM_FIRE)	dam = dam * 3 / 2;
    if ( dam > 1 && is_affected(victim, gsn_wolf_form) && dam_type > 3) dam = 7 * dam / 10;
    if (dam > 1 && dam_type == DAM_MENTAL && IS_SET(victim->act, ACT_UNDEAD)) dam = dam / 2;
    if ( dam > 1 && ((IS_AFFECTED(victim, AFF_PROTECT_EVIL) && IS_EVIL(ch) )
    || (IS_AFFECTED(victim, AFF_PROTECT_GOOD) && IS_GOOD(ch) )))			dam -= dam / 4;
    if ( dam > 1 && IS_AFFECTED(victim, AFF_WARD_LIGHT) )
    {
		if (IS_AFFECTED(victim, AFF_PROTECT_GOOD) && IS_GOOD(ch) )		        dam -= dam / 20;
		else			dam -= dam / 6;
    }
    if ( is_affected(victim,gsn_horse) && monk_good(ch, WEAR_HEAD) )			dam -= dam / 5;
    if ( is_affected(victim,gsn_aura) )							dam -= dam / 6;
    if ( is_affected(victim,gsn_stance) )					        dam = dam * 3 / 5;
    if ( is_affected(victim,gsn_chii_bolt) && (paf = affect_find(victim->affected,gsn_chii_bolt)) != NULL && paf->modifier == 2)
        dam = dam * 3 / 5;

//WE DO FINAL CHECK ON DAMAGE:

  //RACE/CLASS CHECK:
    switch(check_immune(victim,dam_type, FALSE))
    {
    case(IS_IMMUNE):                    return  0;    break;
    case(IS_RESISTANT): 	        dam -= dam/3; break;
    case(IS_VULNERABLE): 	        dam += dam/2; break;
    }

//RETURN THE FINAL DAMAGE
    return dam;
}//END check_damage(..

/*continuum ability of vamps*/
/*exists only in fight.c*/

inline void set_continuum(CHAR_DATA* ch)
{
  //we add the continuum affect if not already there.
  AFFECT_DATA con;

  if (!is_affected(ch, gsn_continuum))
    {
      con.type = gsn_continuum;
      con.level = ch->level;
      con.duration = 0;

      con.where = TO_NONE;
      con.location = APPLY_NONE;
      con.modifier = 0;
      con.bitvector = 0;
      affect_to_char( ch, &con );

    }
}//end set_continuum


void continuum(CHAR_DATA* ch, int type)
{
  //single buf for message.
  char buf[MIL];
  DESCRIPTOR_DATA *d;
  bool send = FALSE;

  //We send out messages to all the vamps.
  //most messages add the continuum effect, so they will not be reapeated
  //and spammed to all the vamps.

//make a vamp sheck first of all
  if (ch->class != class_lookup("vampire"))
    return;

  //now we begin to check for different type of messages.
  switch (type)
    {
    case CONTINUUM_NONE:
      break;
    case CONTINUUM_DEATH:
      sprintf(buf,"A cold emptiness invades your mind as you sense %s's passing.", ch->name);
      send = TRUE;
      break;
    case CONTINUUM_ATTACK:
      if(!is_affected(ch, gsn_continuum))
	{
	  sprintf(buf,"You have a strong hunch that %s has just been attacked.", ch->name);
	  set_continuum(ch);
	  send = TRUE;
	}//end if not continuum
      else
	return;
      break;
    case CONTINUUM_HURT:
      sprintf(buf,"You feel a sharp pain, and you become certain that %s has just been seriously hurt.", ch->name);
      send = TRUE;
      break;
    }//end switch (type)

  //now we send out the message.
  if( (!IS_IMMORTAL(ch)) && send)
    {
      for ( d = descriptor_list; d; d = d->next )
	{
	  if ( ( d->connected == CON_PLAYING ) && (IS_UNDEAD(d->character) && d->character != ch) )
	    sendf( d->character, "`7[`8Continuum`7]`8\t%s\n\r``", buf );
	}//end for
    }//end if not immortal. (we dont broadcast immortal stuff.)
}//end continuum.



// 03-25 ~12:00 Viri: created the bool affect_dark_metamophosis(..), it does
//the whole dark met. thing, as well as returns true for success.

//03-28 13:00 Viri: created the get_spell_damage(..) so
//we can properly calculate damage done by spells at least.

//04-05 23:00 Viri: Addec continuum for vamps.
//01-10-01	Viri: added checks for ch->in_romm == victim->in_room in multi_hit/virt_Damage
// for setting fight status on victim/ch and autoredirct on charmies.
//05-21-01  Ath: added messages fro NONVICT for dodge, parry, shield_block, etc..
