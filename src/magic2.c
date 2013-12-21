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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "cabal.h"
#include "recycle.h"
#include "interp.h"

char *target_name;

/* manalock effect for dispel/cancellation */
/* TRUE on removal of mana lock */
bool effect_manalock(int sn, int level, CHAR_DATA* ch, CHAR_DATA* victim)
{

  int upkeep = UMAX(5, (get_skill(victim, gsn_dispel_magic) - get_skill(ch, sn)) * 2);
  int chance = 50 +  (get_skill(ch, gsn_mana_lock) - 60) * 2;

  /*
    ch is the character attempting to dispel
    victim is holder of manalock
  */

//roll if we block iwth mana lock.
  if (number_percent() < chance)
    {
      //check upkeep.
      if (victim->mana < upkeep)
	{
	  send_to_char("Unable to sustain your manalock you lose control.\n\r", victim);
	  affect_strip(victim, gsn_mana_lock);//VERY important to strip it first :)
	  spell_dispel_magic(sn, victim->level + 8, victim, victim, TARGET_CHAR);
	  act("The area around $n shimmers and blurs.", victim, NULL, NULL, TO_ROOM);
	  return TRUE;
	}
      else
	victim->mana -= upkeep;

   //do messages
      if (ch == victim)
	act("Your manalock seems to interfere.", ch, NULL, victim, TO_CHAR);
      else
	{
	  act("You feed energy to your manalock and negate $n's spell.", ch, NULL, victim, TO_VICT);
	  act("$N's manalock seems to negate your spell.", ch, NULL, victim, TO_CHAR);
	}
      check_improve(ch, gsn_mana_lock, TRUE, 1);
      return FALSE;
    }

  check_improve(ch, gsn_mana_lock, FALSE, 1);  
  if (check_dispel(level,victim,gsn_mana_lock)){
      return FALSE;
    }
  return FALSE;
}

void spell_cancellation( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA *victim;
  AFFECT_DATA* paf, *paf_next;
  char arg1[MIL];
  int csn = 0; //0 means cancell all
  bool found = FALSE;

  target_name = one_argument( target_name, arg1);

/* get the victim first */
  /* CASE: null spell arguments */
  if (IS_NULLSTR(arg1))
    victim = ch;
  /* CASE: one argument (must select if it is a person or a spell)*/
  else if (IS_NULLSTR(target_name)){
    /* decide between spell or victim target */
    if ( (victim = get_char_room( ch, NULL, arg1)) == NULL){
      victim = ch;
      /* check for spell */
      if ( (csn = skill_lookup(arg1)) < 1){
	send_to_char("No such spell or skill.\n\r",ch);
	return;
      }
    }
    else
      csn = 0; //cancel all spells
  }
  /* CASE: first and second argument */
  else{
    if ( (victim = get_char_room( ch, NULL, arg1)) == NULL){
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    if ( (csn = skill_lookup(target_name)) < 1){
      send_to_char("No such spell or skill.\n\r",ch);
      return;
    }
  }

  /* victim check */
  if (ch != victim && (victim->master == NULL || victim->master != ch)){
    send_to_char("They are not following you.\n\r", ch);
    return;
  }

  /* csn checks */
  if (csn){
    char *spell_name = IS_GEN(csn) ? effect_table[GN_POS(csn)].name : skill_table[csn].name;
    if (!IS_GNBIT(csn, GN_CANCEL)){
      sendf(ch, "%s cannot be cancelled.\n\r", spell_name);
	    
      return;
    }
    if (!is_affected(victim, csn)){
      if (victim == ch){
	sendf(ch, "You are not affected by %s.\n\r", spell_name);
	return;
      }
      else{
	sendf(ch, "They are not affected by %s.\n\r", spell_name);
	return;
      }
    }
  }//end csn checks

  /* now we cancel the spell/spells */
  /* check if there is anything to dispel */
  if (csn < 1){
    for (paf = victim->affected; paf; paf = paf_next){
      paf_next = paf->next;

      /* check if we can dispel */
      if (IS_GNBIT(paf->type, GN_CANCEL))
	break;
    }
    if (paf == NULL){
      send_to_char("There are no more spells to cancel.\n\r", ch);
      return;
    }
  }

  /* look for mana lock */
  if (is_affected(victim, gsn_mana_lock)){
    if (effect_manalock(sn, level, ch, victim))
      found = TRUE;
    else
      return;
  }
  
  /* begin checking spells */
  for (paf = victim->affected; paf; paf = paf_next){
    paf_next = paf->next;
    /* always skip to the last duplicate affect */
    if (paf_next && paf_next->type == paf->type)
      continue;
    if (csn && paf->type != csn)
      continue;

    /* check if we can dispel */
    if (!IS_GNBIT(paf->type, GN_CANCEL))
      continue;

    /* can dispel, check */
    if (check_dispel(level, victim, paf->type)){
      found = TRUE;
    }
  }
  if (found){
    send_to_char("Ok.\n\r",ch);
    /* CONTINENCY */
    check_contingency(victim, NULL, CONT_DISPEL);
  }
  else
    send_to_char("Spell failed.\n\r",ch);
}

void spell_dispel_magic( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{

  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA* paf, *paf_next;
  bool found = FALSE;

  /* quick check to make sure there is anything left to dispel */
  /* begin checking spells */
  for (paf = victim->affected; paf; paf = paf_next){
    paf_next = paf->next;
    if (IS_GNBIT(paf->type, GN_DISPEL))
      break;
  }
  if (paf == NULL){
    send_to_char("There is nothing left to dispel.\n\r", ch);
    return;
  }
  /* check saves */
  if (ch == victim 
      || (psiamp_failed(ch, victim, level, DAM_MENTAL, skill_table[sn].spell_type, skill_table[sn].flags)
	  && saves_spell(level, victim,DAM_MENTAL, skill_table[sn].spell_type))
      )
    {
      send_to_char( "You feel a brief tingling sensation.\n\r",victim);
      send_to_char( "You failed.\n\r", ch);
      return;
    }
  /* look for mana lock */
  if (is_affected(victim, gsn_mana_lock)){
    if (effect_manalock(sn, level, ch, victim))
      found = TRUE;
    else
      return;
  }
  
  /* begin checking spells */
  for (paf = victim->affected; paf; paf = paf_next){
    paf_next = paf->next;
    /* always skip to the last duplicate affect */
    if (paf_next && paf_next->type == paf->type)
      continue;
    /* check if we can dispel */
    if (!IS_GNBIT(paf->type, GN_DISPEL))
      continue;

    /* can dispel, check */
    if (check_dispel(level, victim, paf->type)){
      found = TRUE;
    }
  }

  if (found){
    send_to_char("Ok.\n\r",ch);
    /* CONTINENCY */
    check_contingency(victim, NULL, CONT_DISPEL);
  }
  else
    send_to_char("Spell failed.\n\r",ch);
}

/*effect_freeze_weapon*/
/*only in magic2.c*/
bool effect_freeze_weapon(CHAR_DATA* ch, CHAR_DATA* victim, int gsn)
{
//The effect causes a weapon of an opponent to crumble due to 
//intense temp. change.

  //We will need these for item.s
  OBJ_DATA* wield;
  OBJ_DATA* dual;

  //And an int for chance.
  int chance = 45;
  const int luck_mod = 2;

//Few saftey checks.
  if (ch == NULL || victim == NULL)
    return FALSE;

//Currently this only works for icestorm.
  if (gsn != skill_lookup("ice storm"))
    return FALSE;

  //Ad finaly we go on.
  //first run the check if we succeed.
  chance += (16 - get_curr_stat(victim, STAT_LUCK))*luck_mod;

  //and now we roll.
  if (number_percent() < chance)
    {
      //Ok now do the evil work.
      if ( (wield = get_eq_char(victim,WEAR_WIELD)) != NULL)
	{
	  //we check for ice as weapon material 
	  //(everything else is destroyed, as even the magic fueling 
	  //magical weapons is extinguished inthe terrible cold :)
	  if ( (wield->item_type == ITEM_WEAPON 
		|| wield->item_type == ITEM_WAND
		|| wield->item_type == ITEM_STAFF
		|| wield->item_type == ITEM_POTION
		|| wield->item_type == ITEM_THROW
		|| wield->item_type == ITEM_ARMOR) 
	       && strcmp(wield->material, "ice"))
	    {
	      if (!IS_OBJ_STAT(wield,ITEM_BURN_PROOF)
		  && !IS_OBJ_STAT(wield,ITEM_HAS_OWNER)){
		act("$p shatters into a hundred icy shards in $n's hands.", victim, wield, NULL, TO_ROOM);
		act("Due to intense temperature stress $p shatters in your hands.", ch, wield, victim, TO_VICT);
		//we destroy it!
		extract_obj(wield);
	      }
	    }//END IF PROPER ITEM
	}//END IF wield!=null.
    }//end if <chance

  //and now we roll AGAIn for the second item.
  if (number_percent() > chance)
    return FALSE;
  //Ok now do more of the evil work.

  if (( dual = get_eq_char(victim,WEAR_SHIELD)) == NULL) 
    {
      if  (( dual = get_eq_char(victim,WEAR_SECONDARY)) == NULL)
	dual = get_eq_char(victim,WEAR_HOLD);
    }
    if (dual != NULL)
      {
//we check for ice as weapon material 
//(everything else is destroyed, as even the magic fueling 
//magical weapons is extinguished inthe terrible cold :)
	if ( (dual->item_type == ITEM_WEAPON 
	  || dual->item_type == ITEM_WAND
	  || dual->item_type == ITEM_STAFF
	  || dual->item_type == ITEM_POTION
	  || dual->item_type == ITEM_THROW
          || dual->item_type == ITEM_ARMOR) 
          && strcmp(dual->material, "ice"))
	{
	  if (!IS_OBJ_STAT(dual,ITEM_BURN_PROOF)
	      && !IS_OBJ_STAT(dual,ITEM_HAS_OWNER)){
	    act("$p shatters into a hundred icy shards in $N's hands.", victim, dual, victim, TO_ROOM);
	    act("Due to intense temperature stress $p shatters in your hands.", ch,dual, victim, TO_VICT);
	    //we destroy it!
	    extract_obj(dual);
	  }
	}//END IF PROPER ITEM
    }//END IF wield!=null.
  return TRUE;
}//END EFFECT_FREEZE_WQEAPON

void spell_ice_storm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *vch, *vch_next;
    static const sh_int dam_each[] = 
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,  30,	 35,  40,  45,  50,  55,
	 60,  65,  70,  75,  80,	 82,  84,  86,  88,  90,
	 92,  94,  96,  98, 100,	102, 104, 106, 108, 110,
	115, 120, 125, 130, 135,	137, 139, 142, 145, 150
    };
    int dam;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( 3 * dam_each[level] / 2, 5 * dam_each[level] / 2 );
    act("You summon the elemental force of ice to your aid.", ch,NULL,NULL,TO_CHAR );
    act("$n calls upon the forces of nature and creates a vortex of subzero temperature.",ch,NULL,NULL,TO_ROOM);
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;
	if ( vch != ch && !is_area_safe(ch,vch))
	{
	  m_yell(ch,vch,FALSE);
	  if ( saves_spell( level, vch, DAM_COLD,skill_table[sn].spell_type) )
	    dam = 4 * dam / 5;
	  else
	    effect_freeze_weapon(ch, vch, sn);
	  damage( ch, vch, dam, sn, DAM_COLD ,TRUE);    
	}
    }
}


/*FireStorm*/
/*linked only to magic2.c*/
void spell_firestorm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  //The caster creates a standing storm of fire in the room.
  //Intial damage is done as per table.
  //update of firestorm(combat and regular) done by effect_firestorm_update(..)
  //blinding effect done by effect_firebreath
  //in fight.c, and act_move.c

  CHAR_DATA *vch, *vch_next;
  AFFECT_DATA af;

  static const sh_int dam_each[] = 
  {
    0,
    0,   0,   0,   0,   0,  0,   0,   0,   0,   0,
    0,   0,   0,   0,  40, 45,  50,  55,  60,  65,
    70,  75,  80,  85,  90, 92,  94,  96,  98, 100,
    102, 104, 106, 108, 110,112, 114, 116, 118, 120,
    122, 124, 126, 128, 130,135, 140, 145, 150, 150
  };
  int dam;
  level= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
  level= UMAX(0, level);
  dam= number_range( 2 * dam_each[level]/3,  dam_each[level] * 2 );
  
  if(is_affected(ch, gen_firestorm))
    {
      act("You cause the firestorm to violently expand.", ch,NULL,NULL,TO_CHAR );
      act("The firestorm explodes in hellish fury!",ch,NULL,NULL,TO_ROOM);
//we cut the damage down.
      dam = 3 * dam / 5;
    }
  else
    {
      act("You create a vortex of fire, sulfur, and brimstone overhead.", ch,NULL,NULL,TO_CHAR );
      act("$n calls upon the forces of Hell and raises a Firestorm!",ch,NULL,NULL,TO_ROOM);
    }
//We attach the firestorm effect to the caster.  Now effect_update_firestorm will check this
  //when appropriate and do damage/clear it etc.
  //The modifier is the room vnum, TO_NONE prevent this from being shown on casters aff.
if (!is_affected(ch, gen_firestorm))
  {
    af.where        = TO_AFFECTS;
    af.type         = gen_firestorm;
    af.level        = level;
    af.duration     = number_range(0, 1);
    af.location     = APPLY_NONE;
    af.modifier     = ch->in_room->vnum;
    af.bitvector    = 0;
    affect_to_char(ch,&af);
  }
  
//We run through all the people in the room.
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
      {
        vch_next = vch->next_in_room;
        if ( saves_spell( level, vch, DAM_FIRE,skill_table[sn].spell_type) )
	  dam /= 2;
	
//And we do damage unless safe.
	  if ( vch != ch && !is_area_safe(ch,vch))
	    {
	      m_yell(ch,vch,FALSE);
//Now we do the chance to blind effect.
//the effect blinds unless victim svs at (lvl/4 +dam/15) lvl.
	      fire_effect(vch, ch->level, 2 * dam, TARGET_CHAR);
	      damage( ch, vch, dam, sn, DAM_FIRE ,TRUE);    
	    }//end if safe
      }//end for vch
}//end spell_firestorm


/*New druid spell. Testing. Don't touch. -Shadow */

void spell_arms_of_gaia( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_EVIL(ch) &&
    IS_GOOD(victim))) && !IS_IMMORTAL(ch))
    {
        sendf(ch, "It would go against your beliefs to aid %s.\n\r",
        PERS(victim,ch));
	return;
    }
    if (ch->in_room->sector_type != SECT_FOREST)
    {
        send_to_char("You cannot seem to surround yourself outside of a forest.\n\r",ch);
        return;
    }

    if (is_affected(victim, sn))
    {
      send_to_char("You are already within Gaia's arms.\n\r", ch);
      return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 5 + level;
    af.location  = APPLY_AC;
    af.modifier  = -50;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You surround yourself in the Arms of Gaia.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n bows $s head and proffers a blessing to the Earth Mother.", victim, NULL, NULL, TO_ROOM );
}

void spell_protective_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You are already shielded.\n\r",ch);
	return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level/6);
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_PROTECT_SHIELD;
    affect_to_char( victim, &af );
    act( "You surround yourself in a protective shell of magic.", ch, NULL, NULL, TO_CHAR );
    act( "$n is surrounded by a protective shell of magic.", ch, NULL, NULL, TO_ROOM );
}

void spell_missile_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target){
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) ){
      send_to_char("You are already shielded.\n\r",ch);
      return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act( "You are now protected from ranged weapons.", ch, NULL, NULL, TO_CHAR );
    act( "$n is surrounded by a missile shield.", ch, NULL, NULL, TO_ROOM );
}

void spell_jet_steam( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    OBJ_DATA *fountain;
    bool found = FALSE;

    //test for weather
    if (ch->in_room->sector_type == SECT_WATER_SWIM 
	||  ch->in_room->sector_type == SECT_WATER_NOSWIM
	|| ( mud_data.weather_info.sky >= SKY_RAINING &&  temp_adjust( ch->in_room->temp ) > 35))
      found = TRUE;
    //if still nto found, test for fountain.
    if (!found)
      for ( fountain = ch->in_room->contents; fountain != NULL; fountain = fountain->next_content )
	if ( fountain->item_type == ITEM_FOUNTAIN )
	  {
	    found = TRUE;   
	    break;
	  }
    //now if we havent found anythign by this time then we fail.
    if ( !found )
      {
        send_to_char("You need a source of water to cast that spell.\n\r",ch);
        return;
      }

    dam = 50 + dice( 6, 3 ) * level/10;
    if ( saves_spell( level, victim, DAM_DROWNING,skill_table[sn].spell_type ) )
	dam /= 2;
    act("You make the water sizzle with heat.", ch,NULL,NULL,TO_CHAR );
    act("$n channels energy into the water.",ch,NULL,NULL,TO_ROOM);
    damage( ch, victim, dam, sn,DAM_DROWNING,TRUE);
}

void spell_mystic_tendrils( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    int m_gain = number_range(level/2, level) + int_app[get_curr_stat(ch,STAT_INT)].practice * 2,
    chance = number_percent(), dam = number_range(level/2, 2 * level / 3);
    chance += (dam - level) /10;
    chance += 2*(get_curr_stat(ch,STAT_LUCK) - 16);
    if ( ch->hit < dam)
    {
    	send_to_char( "You tear into your physical body.\n\r", ch );
	damage(ch,ch,dam,sn, DAM_NONE,TRUE);
    }
    else
    {
    	send_to_char( "You tear into your physical body.\n\r", ch );
	damage( ch, ch, dam, sn,DAM_NONE,TRUE);
	if (chance < 20)
	{
	    send_to_char("Something disturbs you and you lose the spell.\n\r",ch);
	    m_gain = 0;
	}
	else if (chance < 60)
	{
	    send_to_char("You regain some mental strength.\n\r",ch);
	    m_gain /= 2;
	}
	else if (chance < 90)
	{
	    send_to_char("You regain more mental strength.\n\r",ch);
	}
	else
	{
	    send_to_char("Yes! You feel a sudden surge of mental strength.\n\r",ch);
	    m_gain *= 2;
	}
    	ch->mana = UMIN(ch->max_mana, ch->mana + m_gain);
    }
}

void spell_ice_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED2(victim, AFF_FIRE_SHIELD) || IS_AFFECTED2(victim, AFF_ICE_SHIELD) || IS_AFFECTED2(victim,AFF_MANA_SHIELD))
    {
        send_to_char("You are already surrounded by a shield.\n\r",ch);
        return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level/7);
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_ICE_SHIELD;
    affect_to_char( victim, &af );
    act( "An icy blue shell of cold revolves around you.", ch, NULL, NULL, TO_CHAR );
    act( "$n is surrounded by an icy blue shell.", ch, NULL, NULL, TO_ROOM );
}

void spell_fire_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED2(victim, AFF_FIRE_SHIELD) || IS_AFFECTED2(victim, AFF_ICE_SHIELD) || IS_AFFECTED2(victim,AFF_MANA_SHIELD))
    {
        send_to_char("You are already surrounded by a shield.\n\r",ch);
        return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level/7);
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_FIRE_SHIELD;
    affect_to_char( victim, &af );
    act( "A flaming sphere revolves around you.", ch, NULL, NULL, TO_CHAR );
    act( "$n is surrounded by a sphere of flames.", ch, NULL, NULL, TO_ROOM );
}

void spell_mana_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  if ( IS_AFFECTED2(victim, AFF_FIRE_SHIELD) || IS_AFFECTED2(victim, AFF_ICE_SHIELD) || IS_AFFECTED2(victim,AFF_MANA_SHIELD))
    {
      send_to_char("You are already surrounded by a shield.\n\r",ch);
      return;
    }
  af.where     = TO_AFFECTS2;
  af.type      = sn;
  af.level     = level;
  af.duration  = number_fuzzy( level/7);
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_MANA_SHIELD;
  affect_to_char( victim, &af );
  act( "A shimmering sphere of magic encases you.", ch, NULL, NULL, TO_CHAR );
  act( "$n is encased by a shimmering sphere of magic.", ch, NULL, NULL, TO_ROOM );
}

void spell_hellstream( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA* paf;
  OBJ_DATA *obj_melt;

  
  bool issave = FALSE;
  const int base_chance = 3;
  const int glow_mod = 4;
  const int hum_mod = 4;
  const int enchant_mod = 18;
  const int svs_mod = 12;
    
  const int melt_check = 40;  //chance to start melting when svs fail.
  
  int dam = (7 * level / 2) + (level / 5) * (dice( 3, 6 ) + 2);

  if ( saves_spell(level + 2, victim, DAM_ENERGY,skill_table[sn].spell_type ) )
    {
      issave = TRUE;
      dam /= 2;
    }
  act("You bow your head as you call forth the full power of the arcane.", victim,NULL,ch,TO_VICT );
  act("The ground erupts as you are caught in a searing beam of arcane light.",victim,NULL,ch,TO_CHAR);
  act("The ground erupts as $n is caught in a searing beam of arcane light.",victim,NULL,NULL,TO_ROOM);

  
  //Check if we should start meltint
  if (number_percent() < melt_check && !issave
      && check_immune(victim, DAM_ENERGY, FALSE) != IS_IMMUNE)
    {
      int chance = 0;
      //Begin cycling through all items and melting.
      for ( obj_melt = victim->carrying; obj_melt != NULL; obj_melt = obj_melt->next_content)
	{
	  bool has_saves = FALSE;
	  chance = base_chance;
	  //we add up modifiers
	  if (IS_OBJ_STAT(obj_melt, ITEM_GLOW))
	    chance += glow_mod;
	  if (IS_OBJ_STAT(obj_melt, ITEM_HUM))
	    chance += hum_mod;
	  //Now we check for saves vs spell or aff.
	  if (obj_melt->enchanted)
	    chance+= enchant_mod;
	  else //Item is NOT enchanted 
	    {
	      //Begin searching throgh paf's for svs
	      for ( paf = obj_melt->pIndexData->affected; paf != NULL; paf = paf->next )
		if( (paf->location == APPLY_SAVING_AFFL) ||  (paf->location == APPLY_SAVING_SPELL) )
		  has_saves = TRUE;
	    }//end non-enchant check.
	  //check regular affects unless we already found one
	  if (!has_saves)
	    for ( paf = obj_melt->affected; paf != NULL; paf = paf->next )
	      if( (paf->location == APPLY_SAVING_AFFL) ||  (paf->location == APPLY_SAVING_SPELL) )
		has_saves = TRUE;
	  
	  //Finaly now if there are saves we apply the mod.
	  if (has_saves)
	    chance+= svs_mod;
	  
	  //We check if we blow it.
	  if ( ( number_percent() < chance) 
	       && !IS_OBJ_STAT(obj_melt,ITEM_BURN_PROOF)
	       && !IS_OBJ_STAT(obj_melt,ITEM_HAS_OWNER))
	    {
	      if (has_saves)
		act("$p shudders under the stress and explodes.",ch,obj_melt,NULL, TO_ALL);
	      else
		{
		  act("$p crumbles in $n's hellstream.",ch,obj_melt,NULL,TO_ROOM);
		  act("$p crumbles in your hellstream.",ch,obj_melt,NULL,TO_CHAR);
		}//end else (has saves)
	      
	      //check if we spill items.	    
	      if (obj_melt->item_type == ITEM_CONTAINER)
		{
		  OBJ_DATA *t_obj, *n_obj;
		  act("The items in $p scatter to the ground.",victim,obj_melt,NULL,TO_CHAR);            
		  for (t_obj = obj_melt->contains; t_obj != NULL; t_obj = n_obj)
		    {
		      n_obj = t_obj->next_content;
		      obj_from_obj(t_obj);            
		      if ((number_bits(2) == 0 || victim->in_room == NULL) 
			  && !IS_OBJ_STAT(t_obj,ITEM_BURN_PROOF))
			extract_obj(t_obj);
		      else
			obj_to_room(t_obj,victim->in_room);
		    }//end for t_obj
		}//end if CONTAINER
	      extract_obj(obj_melt);
	    }//end if blow
	}//end for obj_melt
    }//end if notsave
  damage( ch, victim, dam, sn,DAM_ENERGY,TRUE);
}//edn spell_hellstream

void spell_flesh_golem( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *mob;
    int dur = (is_affected(ch, gsn_soul_tap) ? 6 : 12);

/* cap to ignore spell level boost */
    level = UMIN(ch->level, level);
    
    if (get_summoned(ch, MOB_VNUM_FLESH_GOLEM) > 0)
    {
	send_to_char("Your flesh golem is already in existence.\n\r",ch);
	return;
    }
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You do not feel up to creating another flesh golem.\n\r",ch);
	return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = dur;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    act ("You create a flesh golem!", ch, NULL, NULL, TO_CHAR);
    act ("$n creates a flesh golem!", ch, NULL, NULL, TO_ROOM);
    mob = create_mobile( get_mob_index( MOB_VNUM_FLESH_GOLEM ) );
    char_to_room(mob,ch->in_room);
    mob->level      = level;
    mob->max_hit    = ch->level*17;
    mob->hit        = mob->max_hit;
    mob->alignment = ch->alignment;
    SET_BIT(mob->affected_by, AFF_CHARM);
    mob->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
    mob->summoner = ch;
    mob->hitroll        = ch->level/5;
    mob->damroll        = ch->level/5 + 5;
    mob->damage[DICE_NUMBER]    = URANGE(2,(ch->level/10),4);
    mob->damage[DICE_TYPE]      = (ch->level/10) +4;
    add_follower(mob,ch);
    mob->leader=ch;
}

void spell_stone_golem( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *mob;
    int dur = (is_affected(ch, gsn_soul_tap) ? 6 : 12);

/* cap to ignore spell level boost */
    level = UMIN(ch->level, level);

    if (!can_follow(ch, ch))
      return;

    if (get_summoned(ch, MOB_VNUM_STONE_GOLEM) > 0)
    {
	send_to_char("Your stone golem is already in existence.\n\r",ch);
	return;
    }
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You do not feel up to creating another stone golem.\n\r",ch);
	return;
    }
    level = UMIN(ch->level, level);
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = dur;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    act ("You create a stone golem!", ch, NULL, NULL, TO_CHAR);
    act ("$n creates a stone golem!", ch, NULL, NULL, TO_ROOM);
    mob = create_mobile( get_mob_index( MOB_VNUM_STONE_GOLEM ) );
    char_to_room(mob,ch->in_room);
    mob->level      = level;
    mob->max_hit    = level*17;
    mob->hit        = mob->max_hit;
    mob->alignment = ch->alignment;
    SET_BIT(mob->affected_by, AFF_CHARM);
    mob->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
    mob->summoner = ch;
    mob->hitroll        = level/5 + 5;
    mob->damroll        = (level/5)*2 + 7;
    mob->damage[DICE_NUMBER]    = URANGE(3,(level/10),5);
    mob->damage[DICE_TYPE]      = (level/10)*2 + 1;
    add_follower(mob,ch);
    mob->leader=ch;
}

void spell_soul_capture(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo, *newobj;
    CHAR_DATA *victim;
    char buf[MSL];
    bool found = FALSE;
    if (obj->item_type == ITEM_CORPSE_NPC 
	|| obj->owner == 0 
	|| obj->condition < 2
	|| obj->pIndexData->vnum == OBJ_VNUM_FAKE_CORPSE)
    {
        send_to_char("You cannot seem to find a soul.\n\r",ch);
	return;
    }
    if (obj->item_type != ITEM_CORPSE_PC)
    {
        send_to_char("There's no soul in that!\n\r",ch);
	return;
    }
    if (obj->level < 15 && !IS_IMMORTAL(ch))
    {
	send_to_char("Not on that corpse.\n\r",ch);
	return;
    }
    if (!can_loot(ch, obj)){
      act("$g's power shields $p from your greedy fingers.", ch, obj, NULL, TO_CHAR);
      return;
    }
    for (victim = player_list; victim != NULL; victim = victim->next_player)
        if (victim->id == obj->owner)
        {
	    found = TRUE;
	    break;
	}
    if (!found)
    {
        send_to_char("The soul has faded away.\n\r",ch);
        return;
    }
    newobj = create_object(get_obj_index(OBJ_VNUM_SOUL),0);
    free_string( newobj->short_descr );
    sprintf( buf, "the soul of %s", victim->name);
    newobj->short_descr = str_dup( buf );
    free_string( newobj->description );
    sprintf( buf, "The soul of %s has been captured.", victim->name);
    newobj->description = str_dup( buf );
    obj_to_char( newobj, ch );
    sprintf( buf, "You capture the soul of %s.", victim->name);
    act (buf, ch, NULL, NULL, TO_CHAR);
    sprintf( buf, "$n captures the soul of %s.", victim->name);
    act (buf, ch, NULL, NULL, TO_ROOM);

    free_string(obj->description);
    obj->description = str_dup("A desiccated corpse lies here.");
    obj->condition = 1;
}

void spell_vampiric_touch( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = dice( 20, 2 );
    int gain = 0;

    
    if ( saves_spell( level, victim, DAM_ENERGY,skill_table[sn].spell_type ) )
	dam /= 2;
    if (ch != victim){
      gain = 20 + dice ( 7, 5 );
      /* scale gain by level */
      gain = UMAX(1, victim->level) * gain / ch->level;
    }
    else
      gain = 0;

    if ( damage( ch, victim, dam, sn,DAM_ENERGY,TRUE)){
      ch->hit = UMIN( ch->max_hit, ch->hit + gain);
    }
}

void spell_hold_undead( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (ch == victim)
    {
        send_to_char( "You can't hold yourself.\n\r",ch);
        return;
    }
    if (!IS_UNDEAD(victim))
    {
        sendf( ch, "%s is unaffected by your spell.\n\r", PERS(victim,ch) );
        multi_hit( victim, ch, TYPE_UNDEFINED );
	return;
    }
    if ( IS_AFFECTED2(victim, AFF_HOLD) )
    {
        sendf(ch, "%s is already as calm as can be.\n\r", PERS(victim,ch));
        return;
    }
    if ( saves_spell( level, victim, DAM_CHARM,skill_table[sn].spell_type ) )
    {
        sendf(ch, "You failed to hold %s.\n\r", PERS(victim,ch));
        multi_hit( victim, ch, TYPE_UNDEFINED );
        return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level	 = level;
    af.duration  = number_fuzzy( 1 );
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_HOLD;
    affect_to_char( victim, &af );
    af.where     = TO_AFFECTS;
    af.bitvector = AFF_SLOW;
    affect_to_char( victim, &af );
    act_new( "Your body stops responding as $n bars you from your agressive nature.", ch,NULL,victim,TO_VICT,POS_DEAD );
    act("$n exerts $s presence over $N.",ch,NULL,victim,TO_NOTVICT);
    act("$N freezes and calms down as you exert your presence.",ch,NULL,victim,TO_CHAR);
}
CHAR_DATA* create_zombie(CHAR_DATA* ch, CHAR_DATA* victim, OBJ_DATA* obj)
{
  CHAR_DATA* mob;
  int i;
  char buf[MSL];

  AFFECT_DATA af;

  mob = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE ) );
  mob->level      = UMIN(52, victim->level);
  mob->alignment  = ch->alignment;
  mob->sex        = victim->sex;
  mob->class      = victim->class;
  mob->size       = victim->size;
  
  //check for origins npc/pc
  if (obj->item_type == ITEM_CORPSE_NPC)
    {
      if (victim->max_hit != 0)
	mob->max_hit = UMIN(2000,victim->max_hit /3);
      else
	mob->max_hit = 1;
      mob->hit        = mob->max_hit;
      if (victim->hitroll != 0)
	mob->hitroll = UMIN(40, victim->hitroll / 2);
      else
	mob->hitroll = 0;
      if (victim->damroll != 0)
	mob->damroll = URANGE(1, victim->damroll / 2, 45);
      else
	mob->damroll = 0;


      for (i = 0; i < 4; i++)
	{
	  if (victim->armor[i] != 0)
	    mob->armor[i] = victim->armor[i];
	  else
	    mob->armor[i] = 0;
	}
      
      //set stats
      for (i = 0; i < MAX_STATS; i++)
	{
	  mob->perm_stat[i]     = victim->perm_stat[i]/2;
	  mob->mod_stat[i]      = victim->mod_stat[i]/2;
	}
      
      //set dam and acts.
      mob->dam_type   = victim->dam_type;
      mob->act        = victim->act;
      mob->off_flags  = victim->off_flags;
      
      //remove unneccessary flags
      REMOVE_BIT(mob->off_flags,ASSIST_ALL);
      REMOVE_BIT(mob->off_flags,ASSIST_ALIGN);
      REMOVE_BIT(mob->off_flags,ASSIST_RACE);
      REMOVE_BIT(mob->off_flags,ASSIST_PLAYERS);
      REMOVE_BIT(mob->off_flags,ASSIST_GUARD);
      REMOVE_BIT(mob->off_flags,ASSIST_VNUM);
      REMOVE_BIT(mob->off_flags,CABAL_GUARD);
      REMOVE_BIT(mob->off_flags,GUILD_GUARD);
      REMOVE_BIT(mob->off_flags,OFF_AREA_ATTACK);
      REMOVE_BIT(mob->act,ACT_SCAVENGER);
      REMOVE_BIT(mob->act,ACT_IS_HEALER);
      REMOVE_BIT(mob->act,ACT_TRAIN);
      REMOVE_BIT(mob->act,ACT_PRACTICE);
      REMOVE_BIT(mob->act,ACT_TOO_BIG);
      SET_BIT(mob->act,ACT_UNDEAD);
      
      //setup combat stats
      for (i = 0; i < 3; i++)
	{
	  if (victim->damage[i] != 0)
	    mob->damage[i] = UMAX(1, victim->damage[i]/2);
	  else
	    mob->damage[i] = 0;
	}
    }//END IF NPC
  else
    {
      mob->max_hit = 20 * victim->level;
      mob->hit        = mob->max_hit;
      
      //DAM/HIT
      mob->hitroll = victim->level/10;
      mob->damroll = 2*victim->level/5;
      
      //ARMOR
      for (i = 0; i < 3; i++)
	mob->armor[i] = interpolate(victim->level,100,-100);;
      mob->armor[3] = interpolate(victim->level,100,0);;
      
      //STATS      
      for (i = 0; i < MAX_STATS; i++)
	{
	  mob->perm_stat[i]     = victim->perm_stat[i];
	  mob->mod_stat[i]      = victim->mod_stat[i];
	}
      if (obj->level != 0)
	mob->damage[0] = obj->level/7;
      else
	mob->damage[0] = 0;
      mob->damage[1] = 5;
      mob->damage[2] = 1;
    }//END PC 
  
  
  //SETUP COMON THINGS
  obj->short_descr[0] = LOWER(obj->short_descr[0]);
  sprintf( buf, "A zombie of %s", obj->short_descr);
  str_replace( buf, "the corpse of ", "" );
  free_string( mob->short_descr );
  mob->short_descr = str_dup( buf );
  sprintf( buf, "A zombie of %s stands here serving %s master.\n\r", obj->short_descr, (ch->sex == 2) ? "her" : "his");
  str_replace( buf, "the corpse of ", "" );
  free_string( mob->long_descr );
  mob->long_descr = str_dup( buf );
  
//Load Zombie into room
  char_to_room(mob,ch->in_room);
  SET_BIT(mob->affected_by, AFF_CHARM);
  mob->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
  
//Setup the stats.
  mob->summoner = ch;
  add_follower(mob,ch);
  mob->leader=ch;

  //check inv.
  empty_obj(obj);
  //add an effect to store original mobs vnum.
  //bitvector stores the offense flags seton the zombie, acts' are done manualy when loading
  af.type = skill_lookup("animate corpse");
  af.level = 60;
  af.duration = -1;

  /* store current offense flags in bitvector */
  af.where = TO_NONE;
  af.bitvector = mob->off_flags;
  /* store original vnum in modifier (if PC corpse, then just zombie vnum) */
  af.location = APPLY_NONE;
  af.modifier = IS_NPC(victim) ? victim->pIndexData->vnum : mob->pIndexData->vnum;
  affect_to_char(mob, &af);
  return mob;
}//end Create zombie


void spell_animate_corpse(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA af;
    CHAR_DATA  *victim;
    bool found = FALSE;
    bool fAdv = is_affected(ch, gsn_soul_tap);
    int dur = 12;
    int chance = 0;

    if (get_summoned(ch, MOB_VNUM_ZOMBIE) >= UMAX(1,(ch->level/10)-2) && !IS_IMMORTAL(ch))
    {
	send_to_char("You can't control another zombie.\n\r",ch);
	return;
    }
    if (!can_follow(ch, ch))
      return;
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You are too drained to animate anything.\n\r",ch);
	return;
    }
    if (obj->item_type != ITEM_CORPSE_PC && obj->item_type != ITEM_CORPSE_NPC)
    {
        send_to_char("That's not a corpse.\n\r",ch);
	return;
    }
    if (obj->owner == 0 || obj->pIndexData->vnum == OBJ_VNUM_FAKE_CORPSE)
    {
	send_to_char("There is not enough power left in that corpse.\n\r",ch);
	return;
    }
    if (obj->item_type == ITEM_CORPSE_PC)
    {
    	if (obj->level < 15 && !IS_IMMORTAL(ch))
    	{
	    send_to_char("Not on that corpse.\n\r",ch);
	    return;
    	}
	if (!can_loot(ch, obj)){
	  act("$g's power shields $p from your greedy fingers.", ch, obj, NULL, TO_CHAR);
	  return;
	}
	for (victim = player_list; victim != NULL; victim = victim->next_player)
	    if (victim->id == obj->owner)
	    {
		found = TRUE;
		break;
	    }
	if (!found)
	{
	    send_to_char("There is not enough power left in that corpse.\n\r",ch);
	    return;
	}
    }
    else if (obj->item_type == ITEM_CORPSE_NPC && obj->owner == 3)
    {
	send_to_char("There is not enough power left in that corpse.\n\r",ch);
	return;
    }
    else
	victim = create_mobile( get_mob_index( obj->owner ) );

    
    if (fAdv)
      chance = 125;
    else
      chance = get_skill(ch,sn);

    if (chance < 1)
      chance = 75;

    if (obj->level - ch->level > 10 
	|| obj->level > 52
	|| number_percent() > chance + 5*(ch->level - obj->level) - 25 + (get_curr_stat(ch,STAT_LUCK)-16))
    {
    	af.where     = TO_AFFECTS2;
    	af.type      = sn;
    	af.level     = level;
    	af.duration  = dur/2;
    	af.location  = 0;
    	af.modifier  = 0;
    	af.bitvector = 0;
    	affect_to_char( ch, &af );
        act("You destroyed the corpse.",ch,NULL,NULL,TO_CHAR);
        act("$n destroyed the corpse.",ch,NULL,NULL,TO_ROOM);

	//empty obj
	empty_obj(obj);
	obj_from_room ( obj );
	extract_obj ( obj );
    }
    else
    {
    	af.where     = TO_AFFECTS2;
    	af.type      = sn;
    	af.level     = level;
    	af.duration  = dur;
    	af.location  = 0;
    	af.modifier  = 0;
    	af.bitvector = 0;
    	affect_to_char( ch, &af );
        act ("You raise the dead into the form of a zombie!", ch, NULL, NULL, TO_CHAR);
        act ("$n raises the dead!", ch, NULL, NULL, TO_ROOM);

	create_zombie(ch, victim, obj);
	if (IS_NPC(victim))
	  extract_char(victim,TRUE);
	obj_from_room ( obj );
	extract_obj ( obj );
    }
}

void spell_shatter( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  //This is the new shatter spell.

  /*
EFFECTS:
0 = Arms (-lvl/7 str, -lvl/5 hit/dam )
1 = Legs (-lvl/7 dex, enfeeble)
2 = Pelvis(-lvl/2% Mvs, +lvl/3% Damage, lvl%/2 Dysentry) 
3 = Skull ( - (lvl to 3 *lvl/2)% mana, + lvl/3% damage, lvl/2% misdirection)
4 = Spleen (Drained, -lvl%2Mv, +1/8 Hp Dam) 
  */

    static const sh_int dam_each[] = 
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	 15,   15,   15,   15,  15,	 18,  20,  21,  22,  23,
	 24,  25,  26,  27,  28,	 29,  30,  40,  50,  60,
	 62,  64,  66,  68,  70,  	 72,  74,  76,  78,  70,
	 81,  82,  83,  84,  85,	 95, 105, 115, 130, 145
    };

  //POINTERS
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
  //BUFFERS
    char part[MSL], buf[MSL];

  //DATA
    int dam;
    int number = number_range(0,4);
  //CONST
    //arm
    const int arm_str = number_fuzzy(8);
    const int arm_hit = number_fuzzy(8);
    const int arm_dam = number_fuzzy(8);
    const int arm_dur = number_fuzzy(8);
    //legs
    const int leg_dex = number_fuzzy(8);
    const int leg_mov = 1;
    const int leg_enf = 2;
    const int leg_dur = number_fuzzy(8);

    //pelvis
    const int pel_mov = 2;
    const int pel_dam = 3;
    const int pel_dys = 2;
    const int pel_dur = 2;

    //skull
    const int sku_dam = 3;
    const int sku_mis = 2;
    const int sku_dur = 5;

    //spleen
    const int spl_mov = 2;
    const int spl_dam = 8;
    const int spl_dur = 5;

  //FLAGS
    bool fAutoFail = FALSE;

  //Get current damage
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range(3 * dam_each[level] / 4, dam_each[level]);

    //check if we fail.
    if (is_affected( victim, sn ))
      fAutoFail = TRUE;
    else if ( saves_spell( level, victim, DAM_NEGATIVE,skill_table[sn].spell_type ) )
      fAutoFail = TRUE;

    //Display teh fail message.
    if (fAutoFail)
	send_to_char("You failed to shatter their bones.\n\r", ch);
    else
      {
	switch(number) 
	  {
/* ARMS */
	  case (0) :
	    sprintf(part, "arms");
	    
	    af.type      = sn;
	    af.level     = level;
	    af.duration  = level/arm_dur;
	    af.bitvector = 0;

	    
	    af.where     = TO_AFFECTS2;
	    af.location  = APPLY_HITROLL;
	    af.modifier  = -level/arm_hit;
	    affect_to_char( victim, &af );

	    af.where = TO_AFFECTS2;
	    af.location  = APPLY_DAMROLL; 
	    af.modifier  = -level/arm_dam;      
	    affect_to_char( victim, &af );

	    af.where = TO_AFFECTS2;
	    af.location  = APPLY_STR; 
	    af.modifier  = -level/arm_str;      
	    affect_to_char( victim, &af );

	    break;

/* LEGS */
	  case (1) :
	    sprintf(part, "legs");
	    victim->move -= (level / leg_mov) * victim->move / 100;
	    
	    af.type      = sn;
	    af.level     = level;
	    af.duration  = level/leg_dur;
	    af.bitvector = 0;

	    af.where = TO_AFFECTS2;
	    af.location  = APPLY_DEX; 
	    af.modifier  = -level/leg_dex;      
	    affect_to_char( victim, &af );
	    
	    //check for chance to enfeeble.
	    if (number_percent() < level / leg_enf)
	      spell_enfeeblement(skill_lookup("enfeeblement"), level, ch, victim, TARGET_CHAR);
	    break;
	    
/* PELVIS */
	  case (2) :
	    sprintf(part, "pelvis");
	    dam += (level / pel_dam) * dam / 100;
	    victim->move -=  (level / pel_mov) * victim->move / 100;
	    
	    af.type      = sn;
	    af.level     = level;
	    af.duration  = level/pel_dur;
	    af.bitvector = 0;
	    af.where = TO_AFFECTS;
	    af.location = APPLY_NONE;
	    affect_to_char( victim, &af );
	    
	    //chance for dysentry
	    if (number_percent() < level / pel_dys)
	      spell_dysentery(skill_lookup("dysentery"), level, ch, victim, TARGET_CHAR);
	    break;
	    
/* SKULL */
	  case (3) :
	    
	    sprintf(part, "skull");
	    dam += (level / sku_dam) * dam / 100;
	    victim->mana -= (number_range(level, 3 * level / 2)) * victim->mana / 100;

	    af.type      = sn;
	    af.level     = level;
	    af.duration  = level/sku_dur;
	    af.bitvector = 0;
	    af.where     = TO_AFFECTS2;
	    af.location  = APPLY_NONE;
	    af.modifier  = level;
	    affect_to_char( victim, &af );
//check for misdiretion
	    if (number_percent() < level / sku_mis)
	      {
	    af.type      = gsn_misdirection;
	    af.level     = level;
	    af.duration  = level/sku_dur;
	    af.bitvector = 0;
	    af.where     = TO_NONE;
	    af.location  = APPLY_NONE;
	    af.modifier  = level;
	    affect_to_char( victim, &af );
	  }

	    break;

/* SPLEEN */
	  case (4) :
	    sprintf(part, "spleen");        
	    
	    dam += victim->hit / spl_dam;
	    victim->move -= UMAX(0, (level / spl_mov) * victim->move / 100);

	    af.type      = sn;
	    af.level     = level;
	    af.duration  = level/spl_dur;
	    af.location = APPLY_NONE;
	    af.where = TO_AFFECTS;
	    af.modifier = 0;
	    af.bitvector = 0;
	    affect_to_char( victim, &af );

	    if (!is_affected(victim, gsn_drained))
	      {
		af.type = gsn_drained;
		af.where     = TO_NONE;
		af.location = APPLY_NONE;
		af.modifier = 0;
		affect_to_char( victim, &af );
	      }
	    break;
	  }

//floor the mvoe and mana properly.
	if (victim->mana < 0)victim->mana = 0;
	if (victim->move < 0)victim->move = 0;

	sprintf(buf, "You shatter $N's %s!", part);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf, "You feel a sharp pain in your %s!", part);
	act(buf,ch,NULL,victim,TO_VICT);
	sprintf(buf, "$n shatters $N's %s!", part);
	act(buf,ch,NULL,victim,TO_NOTVICT);

	damage( ch, victim, number_fuzzy(dam), sn,DAM_NEGATIVE,TRUE);
      }//end if 
}//end do shatter.

void spell_chorus_of_anguish( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *vch, *vch_next;  
    int dam;
    act("The cries of the damned fill the air.",ch,NULL,NULL,TO_ALL);
    dam = level * 1.5 + dice(3,15);
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;
        if (saves_spell(level,vch,DAM_NEGATIVE,skill_table[sn].spell_type))
	    dam /= 2;
        if (vch != ch && !is_area_safe(ch,vch))
        {
            send_to_char("You can't stand the cries of the damned.\n\r",vch);
	    m_yell(ch,vch,FALSE);
            damage(ch,vch,dam,sn,DAM_NEGATIVE,TRUE);
        }
    }
}

void spell_strength_damned( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dur = (is_affected(ch, gsn_soul_tap) ? 30 : 60);

    if (!IS_NPC(victim))
    {
        send_to_char("Not on players.\n\r",ch);
        return;
    }
    if (!IS_AFFECTED(victim,AFF_CHARM) || ch != victim->master || victim->pIndexData->vnum != MOB_VNUM_ZOMBIE)
    {
        sendf(ch, "You don't have control over %s.\n\r", PERS(victim,ch));
        return;
    }
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You can't seem to find a source of unholy strength.\n\r",ch);
        return;
    }
    if ( is_affected( victim, sn ) )
    {
        sendf(ch, "You can't strengthen %s any further.\n\r", PERS(victim,ch));
        return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = dur + 2 * (50 - level);
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    af.duration  = -1;
    affect_to_char( victim, &af );
    act("$N's wounds close as $E grows larger with unholy strength.",ch,NULL,victim,TO_ALL);
    victim->max_hit  = 3 * victim->max_hit / 2;
    victim->hit      =  victim->max_hit;
    victim->damage[0] = UMAX(1, 3*victim->damage[0]/2);
    victim->damage[1] = UMAX(1, 3*victim->damage[1]/2);
    victim->perm_stat[STAT_STR] = UMIN(25, 5 * victim->perm_stat[STAT_STR]/2);
    victim->mod_stat[STAT_STR] = UMIN(25, 3 * victim->mod_stat[STAT_STR]/2);
    victim->perm_stat[STAT_DEX] = UMIN(25, 3 * victim->perm_stat[STAT_DEX]/2);
    victim->mod_stat[STAT_DEX] = UMIN(25, 3 * victim->mod_stat[STAT_DEX]/2);
}

void spell_death_grasp( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af, *paf;
    int dam = number_fuzzy(level);
    for ( paf = victim->affected; paf != NULL; paf = paf->next )
        if (paf->type == gsn_death_grasp)
            if (paf->modifier >= 5)
            {
                send_to_char("You can't drain any more vitality.\n\r",ch);
                return;
            }

    level += (ch->tattoo == deity_lookup("cycle") ? level/10 : 0);
    level += (20 - get_curr_stat(victim, STAT_CON));
/* the below is done, so each 2% of skill over 75 results in -1 save to spell*/
    level += (get_skill(ch, sn) - 75) / 5;

    if ( !saves_spell( level, victim, DAM_NEGATIVE,skill_table[sn].spell_type ) )
    {
        af.where     = TO_AFFECTS2;
        af.type      = gsn_death_grasp;
        af.level     = level;
        af.duration  = level/10;
        af.location  = 0;
        af.modifier  = 1;
        af.bitvector = 0;
        affect_join( victim, &af );

        act("Rotting hands erupt from the ground and drain your vitality.",ch,NULL,victim,TO_VICT);
        act("Rotting hands erupt from the ground and claw at $N.",victim,NULL,victim,TO_ROOM);

    }
    damage( ch, victim, dam, sn,DAM_NEGATIVE,TRUE);
}

void spell_powerword_kill( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA *paf;
//DATA
    int times = 0;
    int chance = 0;
    int skill = get_skill(ch, skill_lookup("powerword kill"));
    int dam = number_fuzzy(100) + dice(10,15);
//CONST
    const int luck_mod = 2;
    const int grasp_mod = 7;
    const int lvl_mod = 2;
    const int skill_mod = 1;
    const int skill_med = 75;
 
//begin calulating chances.


//Get the number of times target was grasped.
    for ( paf = victim->affected; paf != NULL; paf = paf->next )
      if (paf->type == gsn_death_grasp)
	times += paf->modifier;

//calculate the chance:
    chance += (get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK)) / luck_mod;
    chance += grasp_mod * times;
    chance += (UMAX(0, victim->level - ch->level)) / lvl_mod * UMAX(1, times);

    if (IS_NPC(victim))
      chance += 15;
    else
      chance += skill_mod * (skill - skill_med) / 100;

    if (chance > 60) chance = 60;

    //give chance con bonus (not affected by ceiling)
    chance = 20 * chance / get_curr_stat(victim, STAT_CON);

    if (victim->level > 50 || (!IS_NPC(victim) && times < 2))
      chance = 0;
    
    if (IS_IMMORTAL(ch))
      chance = 100;

    //special checks.
    if (IS_UNDEAD(victim))
      chance = 0;
    //    sendf(ch, "chance: %d\n\r", chance);

    if ( (number_percent() < chance 
	  && !IS_IMMORTAL(victim)) 
	 && (!IS_NPC(victim) || !IS_SET(victim->act,ACT_TOO_BIG))){
        act("You point your finger at $N and $E crumples to the ground.",ch,NULL,victim,TO_CHAR);
        act("$n points a finger at you and utters Word of Death.",ch,NULL,victim,TO_VICT);
        act("$N crumples to the ground, face frozen and contorted in fear.",ch,NULL,victim,TO_NOTVICT);
	act( "$n is DEAD!!", victim, NULL, NULL, TO_ROOM );
	act_new( "You have been KILLED!!", victim, NULL, NULL, TO_CHAR, POS_DEAD );
        raw_kill(ch, victim);
	do_autos(ch);
        victim->hit = 1;
	victim->mana = 1;
	return;
    }
    else
    {
        act("Spell failed.",ch,NULL,victim,TO_CHAR);
        act_new("Your heart stops beating for a split second.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
	if (paf){
	  paf->modifier -= 2;
	  if (paf->modifier < 1)
	    affect_strip(victim, gsn_death_grasp);
	}
    }
    damage( ch, victim, dam, sn, DAM_NEGATIVE ,TRUE);
}

void spell_knock( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    int door = (int) vo, np = number_percent();
    ROOM_INDEX_DATA *to_room;
    CHAR_DATA *gch;
    EXIT_DATA *pexit = ch->in_room->exit[door], *pexit_rev;
    if ( door < 0 )
        return;
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
	{
            act( "$N is standing too close to the lock.",ch, NULL, gch, TO_CHAR );
	    return;
	}
    if ( !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL(ch))
        { send_to_char( "It's not closed.\n\r",        ch ); return; }
    if ( pexit->key < 0 && !IS_IMMORTAL(ch))
        { send_to_char( "It can't be picked.\n\r",     ch ); return; }
    if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
        { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
    if ( IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL(ch))
        { send_to_char( "You failed to open the door.\n\r", ch ); return; }
    if ( !IS_SET(pexit->exit_info, EX_EASY) )
        np = 2*np/3;
    if ( !IS_SET(pexit->exit_info, EX_HARD) )
        np = 3*np/2;
    if ( !IS_SET(pexit->exit_info, EX_INFURIATING) )
        np *= 2;
    if ( !IS_NPC(ch) && UMAX(1,np - 2*(get_curr_stat(ch,STAT_LUCK) - 16)) > get_skill(ch,sn))
    {
        send_to_char( "You failed to open the door.\n\r", ch);
	return;
    }
    REMOVE_BIT(pexit->exit_info, EX_LOCKED);
    act( "An arcane hand raps gently upon the $d as the lock glows.", ch, NULL, pexit->keyword, TO_ALL );
    if ( ( to_room = pexit->to_room ) != NULL && ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
    && pexit_rev->to_room == ch->in_room )
        REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
}

void spell_blur( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You are already blurred.\n\r",ch);
	return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level / 2);
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_BLUR;
    affect_to_char( victim, &af );
    act( "Your physical contours become distorted.", ch, NULL, NULL, TO_CHAR );
    act( "$n's contours become blurred.", ch, NULL, NULL, TO_ROOM );
}

void spell_phantasmal_griffon( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (victim == ch)
    {
	send_to_char("You can't fool yourself.\n\r",ch);
	return;
    }
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You can't create that illusion right now.\n\r",ch);
	return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = gsn_phantasmal_griffon;
    af.level     = 100;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    act("You call your phantasmal griffon to your aid!",ch,NULL,victim,TO_CHAR);
    act("A griffon screams from above!",ch,NULL,victim,TO_ALL);
    damage( ch, victim, 0, sn,DAM_NONE,FALSE);
}

void spell_illusionary_spectre( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (victim == ch)
    {
	send_to_char("You can't fool yourself.\n\r",ch);
	return;
    }
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You can't create that illusion right now.\n\r",ch);
	return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = gsn_illusionary_spectre;
    af.level     = 100;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    act("You send an illusionary spectre into the battle!",ch,NULL,victim,TO_CHAR);
    act("A spectre materializes from the Nexus!",ch,NULL,victim,TO_ALL);
    damage( ch, victim, 0, sn,DAM_NONE,FALSE);
}

void spell_phantom_dragon( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (victim == ch)
    {
	send_to_char("You can't fool yourself.\n\r",ch);
	return;
    }
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You can't create that illusion right now.\n\r",ch);
	return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = gsn_phantom_dragon;
    af.level     = 100;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    act("You send an esoteric phantasm into battle!",ch,NULL,victim,TO_CHAR);
    act("An esoteric phantasm takes form and assails your senses!",ch,NULL,victim,TO_ALL);
    damage( ch, victim, 0, sn,DAM_NONE,FALSE);
}

void spell_shrink( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if  (saves_spell( level, victim, DAM_MALED,skill_table[sn].spell_type ))
      {
	send_to_char("You failed.\n\r", ch);
	return;
      }

    if ( is_affected( victim, sn ) || victim->size <= SIZE_TINY)
    {
	if (victim == ch)
            act("You are already as small as possible.",ch,NULL,victim,TO_CHAR);
 	else
            act("$N can't be shrunken any further.", ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 6;
    af.location  = APPLY_SIZE;
    af.modifier  = -1;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    af.location  = APPLY_CON;
    af.modifier  = -2;
    affect_to_char( victim, &af );
    af.location  = APPLY_STR;
    af.modifier  = -2;
    affect_to_char( victim, &af );
    af.location  = APPLY_DAMROLL;
    af.modifier  = -3;
    affect_to_char( victim, &af );
    act_new( "The world and everything around you gets bigger.", victim,NULL,NULL,TO_CHAR,POS_DEAD );
    act("$n shrinks in size.",victim,NULL,NULL,TO_ROOM);
}

void spell_enlarge( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ) || victim->size >= SIZE_HUGE)
    {
	if (victim == ch)
            act("You are already enlarged.",ch,NULL,victim,TO_CHAR);
 	else
            act("You can't make $N any larger.", ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 6;
    af.location  = APPLY_SIZE;
    af.modifier  = 1;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    af.location  = APPLY_CON;
    af.modifier  = 2;
    affect_to_char( victim, &af );
    af.location  = APPLY_STR;
    af.modifier  = 2;
    affect_to_char( victim, &af );
    af.location  = APPLY_DAMROLL;
    af.modifier  = 3;
    affect_to_char( victim, &af );
    act_new( "The world and everything around you seem to shrink.", victim,NULL,NULL,TO_CHAR,POS_DEAD );
    act("$n dwarfs everything around $m as $e grows.",victim,NULL,NULL,TO_ROOM);
}

void spell_veil( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
            act("You are as veiled as it gets.",ch,NULL,victim,TO_CHAR);
 	else
            act("$N is already surrounded by the dark walls.", ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( saves_spell( level, victim, DAM_MALED,skill_table[sn].spell_type ))
    {
	sendf(ch, "You failed to veil %s in darkness.\n\r", PERS(victim,ch));
	return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = (level / 6) + 1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_TERRAIN;
    affect_to_char( victim, &af );

    act("Thick shadows veil $n from outside world.", victim, NULL, NULL, TO_ROOM);
    act("Thick walls of darkness veil you from the outside world.",ch, NULL, victim, TO_VICT);
}

void spell_prismatic_spray( int sn, int level, CHAR_DATA *ch, void *vo, int target ) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] = 
    {
	 0,
	 0,  0,  0,  0,  0,	 0,  0,  0, 30, 41,
	42, 43, 44, 44, 45,	45, 46, 46, 47, 48,
	49, 50, 51, 51, 52,	52, 53, 53, 54, 55,
	56, 57, 58, 58, 59,	59, 60, 60, 61, 62,
	63, 64, 65, 65, 66,	66, 67, 67, 68, 69 
    };
    int dam;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level], dam_each[level] * 2 );
    act("A prism of colors shoot out from your hands.", ch,NULL,NULL,TO_CHAR );
    act("A prism of colors shoot out from $n's hands.",ch,NULL,NULL,TO_ROOM);
    switch(number_range(0,9))
    {
	case 0 :
	act_new("You are struck by a beam of violet light.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n is struck by a beam of violet light.",victim,NULL,NULL,TO_ROOM);
    	if ( saves_spell( level, victim, DAM_LIGHT,SPELL_AFFLICTIVE ) )
	    dam /= 2;
	break;
	case 1 :
	act_new("You are struck by a beam of orange light.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n is struck by a beam of orange light.",victim,NULL,NULL,TO_ROOM);
    	if ( saves_spell( level, victim, DAM_MALED,SPELL_MALEDICTIVE ) )
	    dam /= 2;
	else
            spell_slow(skill_lookup("slow"), level, ch, (void *) victim,TARGET_CHAR);
	break;
	case 2 :
	act_new("You are struck by a beam of yellow light.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n is struck by a beam of yellow light.",victim,NULL,NULL,TO_ROOM);
    	if ( saves_spell( level, victim, DAM_MALED,SPELL_MALEDICTIVE ) )
	    dam /= 2;
        else
	    spell_blindness(gsn_blindness, level, ch, (void *) victim,TARGET_CHAR);
	break;
	case 3 :
	act_new("You are struck by a beam of green light.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n is struck by a beam of green light.",victim,NULL,NULL,TO_ROOM);
    	if ( saves_spell( level, victim, DAM_POISON,SPELL_MALEDICTIVE ) )
	    dam /= 2;
        else
	    spell_poison(gsn_poison, level, ch, (void *) victim,TARGET_CHAR);
	break;
	case 4 :
	act_new("You are struck by a beam of blue light.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n is struck by a beam of blue light.",victim,NULL,NULL,TO_ROOM);
    	if ( saves_spell( level, victim, DAM_MALED,SPELL_MALEDICTIVE ) )
	    dam /= 2;
        else
	    spell_weaken(skill_lookup("weaken"), level, ch, (void *) victim,TARGET_CHAR);
	break;
	case 5 :
	act_new("You are struck by a beam of violet light.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n is struck by a beam of violet light.",victim,NULL,NULL,TO_ROOM);
    	if ( saves_spell( level, victim, DAM_LIGHT,SPELL_AFFLICTIVE ) )
	    dam /= 2;
	break;
	case 6 :
	act_new("You are struck by a beam of white light.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n is struck by a beam of white light.",victim,NULL,NULL,TO_ROOM);
	dam = 3 * dam /2;
    	if ( saves_spell( level, victim, DAM_LIGHT,SPELL_AFFLICTIVE ) )
	    dam /= 2;
	break;
	default :
	act_new("You are struck by a beam of red light.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
	act("$n is struck by a beam of red light.",victim,NULL,NULL,TO_ROOM);
	dam /= 2;
    	if ( saves_spell( level, victim, DAM_LIGHT,SPELL_AFFLICTIVE ) )
	    dam /= 2;
	break;
    }
    damage( ch, victim, dam, sn, DAM_LIGHT, TRUE);
}

void spell_misdirection( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    act("With a flash of arcane energy a mighty thunderclap explodes beside $N.", ch, NULL, victim, TO_NOTVICT);
    act("With a flash of arcane energy a mighty thunderclap explodes beside you.", ch, NULL, victim, TO_VICT);

    damage(ch, victim, level, gsn_misdirection, DAM_SOUND, TRUE);

//deafen if vuln to sound. (damage or spell)
    if (check_immune(victim,DAM_SOUND, TRUE) == IS_VULNERABLE
	|| check_immune(victim,DAM_SOUND,FALSE) == IS_VULNERABLE)
      spell_silence(gsn_silence, level + 5, ch, victim, TARGET_CHAR );

    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
            act("You are already dazed.",ch,NULL,victim,TO_CHAR);
 	else
            act("$N already is dazed and confused.", ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( saves_spell( level, victim, DAM_SOUND,skill_table[sn].spell_type ))
    {
      act("$n covers $s ears just in time!", victim, NULL, NULL, TO_ROOM);
      act("You cover your ears just in time!", victim, NULL, NULL, TO_CHAR);
      return;
    }

      act("$s ears trickling with blood, $n looks around dazed and confused.", victim, NULL, NULL, TO_ROOM);
      act("The mighty force of explosion dazes your senses and confuses your mind.", victim, NULL, NULL, TO_CHAR);

    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = number_range(0,5);
    af.duration  = level / 10 - 1;
    af.location  = APPLY_DEX;
    af.modifier  = -1;
    af.bitvector = AFF_MISDIRECTION;
    affect_to_char( victim, &af );
    af.level     = number_range(0,5);
    af.location  = APPLY_INT;
    af.modifier  = -1;
    af.bitvector = AFF_MISDIRECTION;
    affect_to_char( victim, &af );

}

void spell_deluge_water( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = level + dice( 10, 5 );
    if ( saves_spell( level, victim, DAM_DROWNING,skill_table[sn].spell_type ) )
	dam /= 2;
    act("You create a cube of water.", ch,NULL,NULL,TO_CHAR );
    act("$n creates a cube of water.",ch,NULL,NULL,TO_ROOM);
    damage( ch, victim, dam, sn,DAM_DROWNING,TRUE);
}

void spell_shadowform( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You are already in shadowform.\n\r",ch);
	return;
    }
    if (get_charmed_by(ch))
    {
        send_to_char( "You can't hide yourself when you have followers.\n\r", ch);
        return;
    }
    if ( IS_AFFECTED(ch, AFF_FAERIE_FOG) || IS_AFFECTED(ch, AFF_FAERIE_FIRE))
    {
        send_to_char( "You can't change your form while glowing.\n\r", ch);
        return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = get_skill(ch, sn) < 1 ? number_fuzzy(7) : 12;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_SHADOWFORM;
    affect_to_char( victim, &af );
    af.bitvector = AFF_PASS_DOOR;
    act( "Your body loses its substance as you take on an ethereal form.", ch, NULL, NULL, TO_CHAR );
    act( "$n's body loses its substance, and $e dematerializes.", ch, NULL, NULL, TO_ROOM ); 
}

void spell_feeblemind( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
            act("You are already feebleminded.",ch,NULL,victim,TO_CHAR);
 	else
            act("$N already under the effects of feeblemind.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( saves_spell( level, victim, DAM_MENTAL,skill_table[sn].spell_type ) )
    {
        sendf(ch, "You failed to feeblemind %s.\n\r", PERS(victim,ch));
        return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/5;
    af.location  = APPLY_INT;
    af.modifier  = -(level-5)/3;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    af.location  = APPLY_WIS;
    af.modifier  = -level/5;
    affect_to_char( victim, &af );
    act_new("You feel your mind slipping away.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n gets a blank look on $s face.",victim,NULL,NULL,TO_ROOM);
}

void spell_flame_arrow(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] = 
    {
	 0,
	 0,  0,  0,  0,  0,	 0,  0,  0, 40, 41,
	42, 43, 44, 45, 46,	47, 48, 49, 50, 51,
	52, 53, 54 ,55, 56,	60, 63, 69, 70, 71,
	72, 74, 76, 78, 80,	81, 82, 83, 84, 85,
	85, 85, 85, 85, 85,	90, 92, 94, 98, 100 
    };
    int dam;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level], 3 * dam_each[level] / 2 );
    act("You create a flaming arrow and send it streaking towards $N.",ch,NULL,victim,TO_CHAR);
    act("$n creates a flaming arrow and points it towards you.",ch,NULL,victim,TO_VICT);
    act("$n creates a flaming arrow.",ch,NULL,victim,TO_NOTVICT);
    if (IS_AFFECTED2(ch, AFF_FIRE_SHIELD) 
	&& check_immune(victim, DAM_FIRE, TRUE) != IS_VULNERABLE)
      dam += (10 + 2 * dam / 10);
    if ( saves_spell( level, victim,DAM_FIRE,skill_table[sn].spell_type) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_FIRE ,TRUE);
}

void spell_iceball( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *vch, *vch_next;
    static const sh_int dam_each[] = 
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,  30,	 35,  40,  45,  50,  55,
	 60,  65,  70,  75,  80,	 82,  84,  86,  88,  90,
	 90,  90,  90,  90,  90,  	 92,  94,  96,  98, 100,
	101, 102, 103, 104, 105,	106, 107, 108, 109, 110
    };
    int dam;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range(3 * dam_each[level] / 4, 2 * dam_each[level]);
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;
        if ( saves_spell( level, vch, DAM_COLD,skill_table[sn].spell_type) )
	    dam = 80 * dam / 100;
	if ( vch != ch && !is_area_safe(ch,vch))
	{
	    m_yell(ch,vch,FALSE);
            damage( ch, vch, dam, sn, DAM_COLD ,TRUE);
	}
    }
}

void spell_icicle(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    static const sh_int dam_each[] = 
    {
	 0,
	 0,  0,  0,  0,  0,	 0,  0,  0, 30, 31,
	32, 33, 34, 35, 36,	37, 38, 39, 40, 41,
	42, 43, 44 ,45, 46,	50, 52, 53, 54, 55,
	66, 67, 68, 68, 69,	70, 72, 74, 78, 80,
	85, 86, 87, 88, 89,	90, 92, 94, 98, 100 
    };
    int dam;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( dam_each[level], 3 * dam_each[level] / 2 );
    act("You conjure a magical bolt of ice and launch it at $N.",ch,NULL,victim,TO_CHAR);
    act("$n conjures a magical bolt of ice and launches it at you.",ch,NULL,victim,TO_VICT);
    act("$n conjures a magical bolt.",ch,NULL,victim,TO_NOTVICT);
    if (IS_AFFECTED2(ch, AFF_ICE_SHIELD) 
	&& check_immune(victim, DAM_COLD, TRUE) != IS_VULNERABLE)
      dam += (10 + 2 * dam / 10);
    if ( saves_spell( level, victim,DAM_COLD,skill_table[sn].spell_type) )
	dam /= 2;
    damage( ch, victim, dam, sn, DAM_COLD ,TRUE);
}

/* void spell_ranger_staff( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *staff = create_object( get_obj_index( OBJ_VNUM_RANGER_STAFF ), 0);
    AFFECT_DATA *paf;
    staff->value[1] = number_range(level -6, level)/3 + 2;
    staff->value[2] = 2;
    staff->level = level;
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
    paf->type       = sn;
    paf->level      = level;
    paf->duration   = -1;
    paf->location   = APPLY_HITROLL;
    paf->modifier   = level/5;
    paf->bitvector  = 0;
    paf->next       = staff->affected;
    staff->affected   = paf;
    paf = new_affect();
    paf->type       = sn;
    paf->level      = level;
    paf->duration   = -1;
    paf->location   = APPLY_DAMROLL;
    paf->modifier   = level/5;
    paf->bitvector  = 0;
    paf->next       = staff->affected;
    staff->affected   = paf;
    obj_to_char(staff, ch);
    act( "You create $p.", ch, staff, NULL, TO_CHAR );
    act( "$n creates $p.", ch, staff, NULL, TO_ROOM );
}*/

void spell_wrath(int sn, int level, CHAR_DATA *ch, void *vo,int target){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  const int holy_w = skill_lookup("holy word");
  int dam = 0;
  bool fHoly = is_affected(ch, holy_w);
  int chance = fHoly ? 66 : 33;

  if (victim->hit > ch->level * 8)
    dam = number_range(14 * level / 5, 28 * level / 5);
  else{
    /* holy word allows for single powerful hit */
    if (fHoly){
      act_new(skill_table[holy_w].msg_off,ch,NULL,NULL,TO_CHAR,POS_DEAD);
      affect_strip(ch, holy_w);
      dam = number_range(14 * level / 5, 28 * level / 5);
    }
    else{
      dam = UMIN(victim->hit, dice (ch->level, 5));
    }
  }
  if (IS_GOOD(victim) ){
    act("$N follows the path of goodness and is not affected by your wrath.\n\r", ch, NULL, victim, TO_CHAR);
    act("$n attempts to wrath you, but you are unaffected.",ch,NULL,victim,TO_VICT);
    act("$N is unaffected by $n's wrath.",ch,NULL,victim,TO_NOTVICT);
    return;
  }
  if (IS_NEUTRAL(victim) )
    dam /= 3;
  if ( saves_spell( level, victim,DAM_HOLY,skill_table[sn].spell_type) ){
    dam /= 2;
  }
  if ( is_affected(ch,gsn_heroism) )
    dam = 4 * dam / 3;

  if (IS_EVIL(victim) 
      && !is_affected(victim,gsn_curse) 
      && number_percent() < chance){
    spell_curse(gsn_curse, level, ch, (void *) victim,TARGET_CHAR);
  }
  damage( ch, victim, dam, sn, DAM_HOLY ,TRUE);
}

void spell_ikuzachi( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam = level + dice(11,10);
    if ( saves_spell( level, victim, DAM_ENERGY,skill_table[sn].spell_type ) )
        dam /= 2;
    act("You focus your mind to invoke the ancient arts.", ch,NULL,victim,TO_CHAR);
    act("$n closes $s eyes for a moment then glares in your direction.",ch,NULL,victim,TO_VICT);
    act("$n closes $s eyes for a moment in deep concentration.",ch,NULL,victim,TO_NOTVICT);
    damage( ch, victim, dam, sn,DAM_ENERGY,TRUE);
}

void spell_silence( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( victim, sn ) )
    {
	if (victim == ch)
            act("Your world is already dead silent.",ch,NULL,victim,TO_CHAR);
 	else
            act("$N has already been silenced.", ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( saves_spell( level, victim, IS_NPC(victim) ? DAM_MENTAL : DAM_CHARM,skill_table[sn].spell_type ))
    {
	sendf(ch, "You failed to silence %s.\n\r", PERS(victim,ch));
	return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 8;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SILENCE;
    affect_to_char( victim, &af );
    if (victim == ch)
        send_to_char("Your world suddenly becomes dead silent.\n\r",ch);
    else
    {
        act("$N has been silenced.",ch,NULL,victim,TO_CHAR);
        act_new("Your world suddenly becomes dead silent.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    }
}


void spell_damnation( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (is_affected(victim,sn))
    {
        act("$N is already damned.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 3*level;
    af.location  = APPLY_HITROLL;
    af.modifier  = -1 * (level / 8);
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "The Gods have forsaken you.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
    if ( ch != victim )  
        act("$N looks very uncomfortable.",ch,NULL,victim,TO_CHAR);
}

void spell_flashfire( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA* vch;
    CHAR_DATA* vch_next;

    AFFECT_DATA af;
    act("You create a bright flash of light.",ch,NULL,NULL,TO_CHAR);
    act("$n sends a brilliant sphere of light spiraling down!",ch,NULL,NULL,TO_ROOM);


    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
      {
	vch_next = vch->next_in_room;
	if ( vch != ch && !is_area_safe(ch,vch) )

	  {
	    /* skip mirror image too */
	    if (IS_NPC(vch) && vch->master && vch->master == ch)
	      continue;
	    m_yell(ch,vch,FALSE);
	    if (!vch->fighting)
	      multi_hit( vch, ch, TYPE_UNDEFINED);
	    
	  }
	else
	  continue;

	if ( IS_AFFECTED(vch, AFF_BLIND))
	  continue;
	
	if ( saves_spell(level, vch, DAM_OTHER,skill_table[sn].spell_type) )
	  {	
	    act_new("You squint your eyes, unaffected by the glare.",ch,NULL,vch,TO_CHARVICT,POS_DEAD);
	    act("$N avoids the glare.",ch,NULL,vch,TO_CHAR);
	    continue;
	  }
	else 
	  {
	    act_new("You have been blinded by the flash of light!",ch,NULL,vch,TO_CHARVICT,POS_DEAD);
	    act("$n is blinded by the flashfire!",vch,NULL,vch,TO_ROOM);
	  }
	
	af.where     = TO_AFFECTS; 
	af.type      = gsn_blindness;
	af.level     = level;
	af.location  = APPLY_HITROLL;
	af.duration  = level / 8 + UMAX(0, (get_skill(ch, sn) - 75) / 5);
	af.bitvector = AFF_BLIND;
	if (!IS_NPC(vch) && number_percent() < get_skill(vch,gsn_blind_fighting)){
	  if (is_affected(vch, gsn_battletrance))
	    af.modifier      = 0;
	  else
	    af.modifier      = -3;
	}
	else
	  af.modifier      = -6;
	affect_to_char(vch,&af);
	af.location     = APPLY_AC;
	if (!IS_NPC(vch) && number_percent() < get_skill(vch,gsn_blind_fighting)){
	  if (is_affected(vch, gsn_battletrance))
	    af.modifier      = 0;
	  else
	    af.modifier      = 5;   
	}
	else
	  af.modifier      = 15;
	affect_to_char(vch,&af);
	
      }
}

void spell_thunder_storm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *vch, *vch_next;
    static const sh_int dam_each[] = 
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,  30,	 35,  40,  45,  50,  55,
	 60,  65,  70,  75,  80,	 82,  84,  86,  88,  90,
	 92,  94,  96,  98, 100,	105, 110, 115, 120, 125,
	127, 129, 131, 133, 135,	140, 145, 150, 150, 150
    };
    int dam;
    int armor = 0;
    level	= UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level	= UMAX(0, level);
    dam		= number_range( 2 * dam_each[level]/3, dam_each[level] * 2 );
        if ( !IS_OUTSIDE(ch) )
    {
        send_to_char( "Inside? Are you joking?!.\n\r", ch );
        return;
    }
    if ( ( mud_data.weather_info.sky < SKY_RAINING
           || temp_adjust( ch->in_room->temp ) <= 35)
         && !IS_IMMORTAL(ch))
    {
        send_to_char( "The sky seems a bit too clear.\n\r", ch );
        return;
    }
    act("You summon the force of thunder storm to your aid.",ch,NULL,NULL,TO_CHAR );
    act("$n calls upon the forces of nature and calls forth a thunder storm.",ch,NULL,NULL,TO_ROOM);
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;
        if ( saves_spell( level, vch, DAM_LIGHTNING,skill_table[sn].spell_type) )
	    dam /= 2;
//We give bonus to the spell for the ac person has. (ac is n've so we subtr.)
//bonus is 1/2 value over 150, and regular over 200.
	armor = -(vch->armor[0] + vch->armor[1] +vch->armor[2])/3;
	if (armor > 200)
	  act("Sparks arc around $n's armor!", vch, NULL, NULL, TO_ROOM);
	dam += UMAX(0 ,(armor-150)/2);
	dam += UMAX(0, (armor - 200));
	dam = UMIN(300, dam);
//And we apply damage.
	if ( vch != ch && !is_area_safe(ch,vch))
	{
	    m_yell(ch,vch,FALSE);
            damage( ch, vch, dam, sn, DAM_LIGHTNING,TRUE);    
	}
    }
}

void spell_esuna( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    bool found = FALSE;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if (IS_EVIL(victim))
    {
	sendf(ch,"Your god does not like %s.\n\r",PERS(victim,ch));
	return;
    }
    if (victim != ch)
    {
    	act("$n soothes you with a glow of white light.",ch,NULL,victim,TO_VICT);
    	act("You cure $N of $S ailments.",ch,NULL,victim,TO_CHAR);
    }
    if (check_dispel(level+2,victim,gsn_poison))
	found = TRUE;
    if (check_dispel(level+2,victim,gsn_death_grasp))
	found = TRUE;
    if (check_dispel(level+2,victim,gsn_plague))
	found = TRUE;
    if (!is_song_affected(victim,gsn_unforgiven) && check_dispel(level+2,victim,gsn_curse))
	found = TRUE;
    if (check_dispel(level+2,victim,gsn_blindness))
	found = TRUE;
    if (check_dispel(level+2,victim,gsn_sleep))
	found = TRUE;
    if (check_dispel(level+2,victim,gsn_silence))
	found = TRUE;
    if (check_dispel(level+2,victim,gsn_blasphemy))
	found = TRUE;
    if (check_dispel(level+2,victim,gsn_hysteria))
	found = TRUE;
    if (check_dispel(level+2,victim,skill_lookup("weaken")))
	found = TRUE;
    if (check_dispel(level+2,victim,gsn_insomnia))
	found = TRUE;
    if (check_dispel(level+2,victim,skill_lookup("dysentery")))
	found = TRUE;
    if (check_dispel(level,victim,gsn_ecstacy))
	found = TRUE;
    if (check_dispel(level,victim,gsn_drug_use))
	found = TRUE;
    if (found)
    {
	act("$n looks more relieved.",victim,NULL,NULL,TO_ROOM);
	send_to_char("A warm glow runs through your body.\n\r",victim);
        if (!IS_NPC(ch) && !IS_NPC(victim) && ch->class == class_lookup("healer") &&  ch != victim)
            gain_exp(ch,number_range(1,ch->level/8));
    }
    else
    {
	act("$N does not seem to be affected.",ch,NULL,victim,TO_CHAR);
	send_to_char("You don't feel any different.\n\r",victim);
    }
}

void spell_holy_water(int sn,int level,CHAR_DATA *ch, void *vo,int target)
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA af;
    int liquid = obj->value[2];
    if( is_affected(ch,sn) )
    {
	send_to_char("You are not ready to bless more water yet.\n\r",ch);
	return;
    }
    if (obj->item_type != ITEM_DRINK_CON)
    {
        send_to_char("That isn't a drink container.\n\r",ch);
        return;
    }
    if (obj->wear_loc != -1)
    {
        send_to_char("The item must be carried to be blessed.\n\r",ch);
        return;
    }
    if ( obj->value[1] <= 0 )
    {
        send_to_char( "It is already empty.\n\r", ch );
        return;
    }
    if ( obj->value[1] > 250 )
    {
        send_to_char( "There is too much water for you to bless.\n\r", ch );
        return;
    }
    if ( liquid != 0 )
    {
	send_to_char("You can only bless pure water.\n\r",ch);
	return;
    }
    act("The water in $p glows as you bless it with your faith.",ch,obj,NULL,TO_CHAR);
    act("The water in $p glows as $n prays with $s hand over it.",ch,obj,NULL,TO_ROOM);
    obj->value[2] = 1;
    obj->value[3] = 0;
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 48;
    af.modifier  = 0;  
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
}

void spell_barrier( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected(victim, sn))
    {
        send_to_char("You are already surrounded by a shield.\n\r",ch);
        return;
    }
    if (is_affected(victim, skill_lookup("divine retribution")))
    {
	send_to_char("You must wait for your divine retribution to wear off.\n\r",ch);
	return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level/9);
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_BARRIER;
    affect_to_char( victim, &af );
    act( "You are enclosed in a physical barrier.", ch, NULL, NULL, TO_CHAR );
    act( "$n is encased by a transparent sphere.", ch, NULL, NULL, TO_ROOM );
}

void spell_vitalize( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (is_affected(victim,sn))
    {
	if (ch != victim)
	    send_to_char("Their metabolism has already been increased.\n\r",ch);
	else
	    send_to_char("Your metabolism is at its peak!\n\r",ch);
	return;
    }
    if (IS_GOOD(ch) && IS_EVIL(victim))
    {
	sendf(ch,"Your god does not like %s.\n\r",PERS(victim,ch));
	return;
    }
    if (IS_GOOD(victim) && IS_EVIL(ch))
    {
	sendf(ch,"Your god does not like %s.\n\r",PERS(victim,ch));
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level/5);
    af.location  = APPLY_CON;
    af.modifier  = 2;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    if (ch != victim)
	act("You vitalize $N's metabolism.",ch,NULL,victim,TO_CHAR);
    send_to_char("You are filled with energy as your metabolism increases!\n\r",victim);
    act("$n seems to be more energetic.",victim,NULL,NULL,TO_ROOM);
}

void spell_sacrifice( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;   
    if (is_affected(ch,sn))
    {
        send_to_char("Your body has not recovered from the last sacrifice!\n\r",ch);
        return;
    }
    if (ch == victim)
    {
        send_to_char("This spell must be casted on someone else.\n\r",ch); 
	return;
    }
    if (ch->hit < ch->max_hit/3 || ch->mana < ch->max_mana/3 || ch->move < ch->max_move/3)
    {
	send_to_char("You body is in no condition to perform a sacrifice.\n\r",ch);
	return;
    }
    if (IS_EVIL(victim))
    {
	sendf(ch,"Your god does not like %s.\n\r",PERS(victim,ch));
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    af.type	 = gsn_drained;
    af.duration  = 5;
    affect_to_char( ch, &af );
    ch->hit = UMAX(1,ch->max_hit * .05);
    ch->mana = UMAX(1,ch->max_mana * .05);
    ch->move = UMAX(1,ch->max_move * .05);
    ch->position = POS_RESTING;
    affect_strip(victim,gsn_plague);
    affect_strip(victim,gsn_poison);
    affect_strip(victim,gsn_blindness);
    victim->hit = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    update_pos(victim);
    act("You sacrifice your own health to restore $N!",ch,NULL,victim,TO_CHAR);
    act("$n passes on $s life energy into your body!",ch,NULL,victim,TO_VICT);
    act("$n slumps to the ground as $e passes $s life onto $N.",ch,NULL,victim,TO_NOTVICT);
}

void spell_heavens_gate( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    char arg[MIL];
    bool gate_pet;
    ROOM_INDEX_DATA *room;
    if (IS_NPC(ch))
	return;
    one_argument(target_name, arg);
    if (arg[0] == '\0')
    {
    	if (ch->pcdata->beacon == 0 || (room = get_room_index(ch->pcdata->beacon)) == NULL)
    	{
	    send_to_char("You do not have a marker set.\n\r",ch);
	    return;
    	}

	if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL)
	  {
	    send_to_char("Your mount refuses to enter the gate.\n\r",ch);
	    return;
	  }
    	if ( IS_SET(room->room_flags, ROOM_SAFE)
    	|| IS_SET(room->room_flags, ROOM_PRIVATE)
    	|| IS_SET(room->room_flags, ROOM_SOLITARY)
    	|| IS_SET(room->room_flags, ROOM_NO_GATEIN)
    	|| IS_SET(ch->in_room->room_flags, ROOM_NO_GATEOUT)
    	|| IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)
        || is_affected(ch, gsn_tele_lock)
    	|| IS_SET(room->room_flags, ROOM_NO_INOUT))
    	{
    	    send_to_char("You failed to create a portal.\n\r",ch);
    	    return;
    	}
	else if (is_affected(ch,gen_ensnare)){
	  send_to_char("You sense a powerful magical field preventing your departure.\n\r", ch);
	  return;
	}
    	if (ch->pet != NULL && ch->in_room == ch->pet->in_room)
    	    gate_pet = TRUE;
    	else
    	    gate_pet = FALSE;
    	act("$n steps through a dark portal and vanishes.",ch,NULL,NULL,TO_ROOM);
    	act("You step through a dark portal and vanish.",ch,NULL,NULL,TO_CHAR);
    	char_from_room(ch);
    	char_to_room(ch,room);
    	act("$n has arrived through a gate.",ch,NULL,NULL,TO_ROOM);
    	do_look(ch,"auto");
    	if (gate_pet)
    	{
    	    act("$n steps through a dark portal and vanishes.",ch->pet,NULL,NULL,TO_ROOM);
    	    act("You step through a dark portal and vanish.",ch->pet,NULL,NULL,TO_CHAR);
    	    char_from_room(ch->pet);
    	    char_to_room(ch->pet,room);
    	    act("$n has arrived through a gate.",ch->pet,NULL,NULL,TO_ROOM);
    	    do_look(ch->pet,"auto");
    	}
    }
    else if (!str_cmp(arg, "marker"))
    {
    	if (ch->in_room == NULL)
	    return;
    	if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE)
    	|| IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
    	|| IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
    	|| IS_SET(ch->in_room->room_flags, ROOM_NO_GATEIN)
    	|| IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT))
    	{
    	    send_to_char("You failed to set a marker.\n\r",ch);
    	    return;
    	}
    	send_to_char("You mark the area for future use.\n\r",ch);
    	ch->pcdata->beacon = ch->in_room->vnum;
    }
    else
	send_to_char("To place a marker, <syntax: ca 'portal' marker>.\n\r",ch);
}

void spell_rejuvenate( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal = dice(3, 20) + level + 30;
    victim->hit = UMIN( victim->hit + heal, victim->max_hit ); 
    update_pos( victim );
    act_new("A jolt of energy runs through you!", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
    if ( ch != victim )
        act("You rejuvenate $N's health.", ch,NULL,victim,TO_CHAR );
}

void spell_prayer( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;  
    if (IS_NPC(victim))
	return;
    if (IS_AFFECTED(victim,AFF_RAGE) || IS_AFFECTED(victim,AFF_BERSERK) || is_affected(victim,skill_lookup("frenzy")))
    {
        if (victim == ch)
            send_to_char("Your prayers cannot reach your gods in your state of mind.",ch); 
        else
	    send_to_char("Your prayer will not reach one in an unstable state of mind.\n\r",ch);
        return;
    }
    if (is_affected(victim,sn))
    {
	if (victim == ch)
	    send_to_char("You are already blessed with a prayer.\n\r",ch);
	else
	    act("$N is already blessed with a prayer.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (!IS_GOOD(victim))
    {
        act("Your god doesn't seem to like $N",ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( is_affected( victim, gsn_blasphemy ) )
    {   
        sendf(ch, "%s's faith have been lost.\n\r", PERS(victim,ch));
        return;
    }
    if (is_fight_delay(victim,90))
    {
	if (ch == victim)
	    send_to_char("You are too worked up to start a prayer.\n\r",ch);
	else
	    act("$N is too worked up to listen to your prayer.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 2;
    af.modifier  = level / 2;
    af.bitvector = 0;
    af.location  = APPLY_MANA;
    affect_to_char(victim,&af);
    af.modifier  = -level / 10;
    af.location  = APPLY_DAMROLL;
    affect_to_char(victim,&af);
    af.modifier  = level / 12;
    af.location  = APPLY_LUCK;
    affect_to_char(victim,&af);
    af.modifier  = -10 * (level / 12);
    af.location  = APPLY_AC;
    affect_to_char(victim,&af);
    act_new("You are filled with serenity by the prayer.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n's eyes are filled with serenity.",victim,NULL,NULL,TO_ROOM);
}

void spell_spirit_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;  
    if (is_affected(victim,sn))
    {
	send_to_char("You already have a spirit shield.\n\r",victim);
	return;
    }
    act_new("You create a spirit shield on your arm.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n creates a spirit shield on $s arm.",victim,NULL,NULL,TO_ROOM);
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/10;
    af.modifier  = 0;
    af.bitvector = 0;
    af.location  = 0;
    affect_to_char(victim,&af);
}

void spell_invigorate( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *vch, *vch_next;
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;
	if (vch->master == ch || is_same_group(vch, ch))
        {
	    if (IS_EVIL(vch))
	    {
		sendf(ch,"Your god does not like %s.\n\r",PERS(vch,ch));
		return;
	    }
	    vch->move = vch->max_move;
	    check_dispel(level,vch,skill_lookup("enfeeblement"));
	    act_new("Your weariness is alleviated from your feet.", ch,NULL,vch,TO_CHARVICT,POS_DEAD );
	    if ( ch != vch )
	        act("You renergize the strength of $N's feet.", ch,NULL,vch,TO_CHAR );
	}
    }
}

void spell_group_recall( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *vch, *vch_next;
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;
	if ( vch->pCabal && IS_CABAL(get_parent(vch->pCabal), CABAL_NOMAGIC))
	  continue;
	if (vch->master == ch || is_same_group(vch, ch))
	  spell_word_of_recall(skill_lookup("word of recall"),level,ch,(void *) vch,TARGET_CHAR);
    }
}

void spell_blasphemy( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected(victim, sn))
    {
        act("$N's faith has already been shaken.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( saves_spell(level, victim, DAM_OTHER,skill_table[sn].spell_type) )
    {	
	if ( ch != victim )
	    act("You fail to taint $N's faith.",ch,NULL,victim,TO_CHAR);
	send_to_char("You remain true to your faith.\n\r",victim);
	return;
    }
    act_new("Your faith shatters as you have doubts about your religion!",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
        act("$N is plagued by doubts of $s own faith!",ch,NULL,victim,TO_CHAR);
    af.where     = TO_AFFECTS; 
    af.type      = sn;
    af.level     = level;
    af.location  = 0;
    af.duration  = level/8;
    af.bitvector = 0;
    af.modifier  = 0;
    affect_to_char(victim,&af);
}

void spell_demonic_visage( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    OBJ_DATA *obj;
    AFFECT_DATA af;
    if ( ch == victim )
    {
	send_to_char("You try to scare yourself without much success.\n\r",ch);
	return;
    }
    if ( is_affected(victim, sn))
    {
        act("$N still cannot shake the images of demons from $s mind.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if (!can_see(victim,ch) || ch->position <= POS_SLEEPING)
    {
        act("$N cannot see your demonic visage.",ch,NULL,victim,TO_CHAR);
        return;
    }
    send_to_char("You peel away your face to reveal your demonic image.\n\r",ch);
    act("$n peels $s face away to reveal $s demonic image.",ch,NULL,NULL,TO_ROOM);
    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL && ( obj = get_eq_char( victim, WEAR_SECONDARY ) ) == NULL )
    {
	send_to_char("Your opponent is not using a weapon.\n\r",ch);
        return;
    }
    if ( saves_spell(level, victim, DAM_OTHER,skill_table[sn].spell_type) )
    {	
	act("$N is not affected by your demonic appearance.",ch,NULL,victim,TO_CHAR);
	act("$n's demonic appearance does not affect you.",ch,NULL,victim,TO_VICT);
	return;
    }
    send_to_char("You drop your weapon as you are taken back by the horrifying image!\n\r",victim);
    act( "$n almost drops $p.", victim, obj, NULL, TO_ROOM );
    unequip_char( ch, obj );
    af.where     = TO_AFFECTS; 
    af.type      = sn;
    af.level     = level;
    af.location  = 0;
    af.duration  = 2;
    af.bitvector = 0;
    af.modifier  = 0;
    affect_to_char(victim,&af);
}

void spell_blanket_darkness( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    ROOM_INDEX_DATA * room;
    AFFECT_DATA af;
    int np = number_percent();
    if (ch->in_room == NULL)
	return;
    room = ch->in_room;
    if (is_affected(ch,sn))
    {
	send_to_char("You are not ready to blanket another room.\n\r",ch);
	return;
    }
    if (room->sector_type == SECT_CITY || IS_SET(room->area->area_flags,AREA_CITY))
    {
	send_to_char("There are too many lights in the city.\n\r",ch);
	return;
    }
    if (room->pCabal)
    {
	send_to_char("Not in a cabal.\n\r",ch);
	return;
    }
    if (np - 2*(get_curr_stat(ch,STAT_LUCK) - 16) < 50)
    {
	send_to_char("You failed to blanket this room in darkness.\n\r",ch);
    	af.where     = TO_AFFECTS; 
    	af.type      = sn;
    	af.level     = level;
    	af.location  = 0;
    	af.duration  = 12;
    	af.bitvector = 0;
    	af.modifier  = 0;
    	affect_to_char(ch,&af);    
	return;
    }
    if (IS_SET(room->room_flags,ROOM_NO_SCAN))
    {
	send_to_char("This room is already covered in darkness.\n\r",ch);
	return;
    }
    send_to_char("You enshroud the room with a blanket of darkness.\n\r",ch);
    act("A blanket of darkness enshrouds the room.",ch,NULL,NULL,TO_ROOM);
    SET_BIT(room->room_flags,ROOM_NO_SCAN);
    SET_BIT(room->room_flags,ROOM_DARK);
    SET_BIT(room->room_flags,ROOM_NOWHERE);
    SET_BIT(room->room_flags,ROOM_INDOORS);
    SET_BIT(room->room_flags,ROOM_NO_INOUT);
    af.where     = TO_AFFECTS; 
    af.type      = sn;
    af.level     = level;
    af.location  = 0;
    af.duration  = 100;
    af.bitvector = 0;
    af.modifier  = 0;
    affect_to_char(ch,&af);    
}

void spell_enfeeblement( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dam;
    if ( ch != victim )
        act("You drain the strength from $N's legs!",ch,NULL,victim,TO_CHAR);
    send_to_char("Your legs feel sluggish as their strength are drained.\n\r",victim);
    dam = UMAX( 1 * victim->move / 10, 15);
    victim->move = UMAX(0, (victim->move - dam));
    if ( !is_affected(victim, sn) && !saves_spell(level, victim, DAM_OTHER,skill_table[sn].spell_type) )
    {
	if (ch != victim )
	   act("$N's legs are exhausted by your enfeeblement.",ch,NULL,victim,TO_CHAR);
	send_to_char("Your legs collapse from exhaustion.\n\r",victim);
    	af.where     = TO_AFFECTS; 
    	af.type      = sn;
    	af.level     = level;
    	af.location  = 0;
    	af.duration  = level / 8;
    	af.bitvector = 0;
    	af.modifier  = 0;
    	affect_to_char(victim,&af);
    }
}
void spell_hysteria( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected(victim, sn))
    {
        act("$N is already in hysteria.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( saves_spell(level - 3, victim, DAM_OTHER,skill_table[sn].spell_type) )
    {	
	if ( ch != victim )
	    act("You fail to bring $N into hysteria.",ch,NULL,victim,TO_CHAR);
	send_to_char("You momentarily filled with anxiety but manage to keep your wits together.\n\r",victim);
	return;
    }
    act_new("You start to panic as you enter a state of hysteria!",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
        act("$N starts to panic from $S hysteria!",ch,NULL,victim,TO_CHAR);
    af.where     = TO_AFFECTS; 
    af.type      = sn;
    af.level     = level;
    af.location  = 0;
    af.duration  = 5;
    af.bitvector = 0;
    af.modifier  = 0;
    affect_to_char(victim,&af);
}
/* SPELL MASS HYSTERIA
   General effects: hysteria, area.
   Coded By:        Athaekeetha
*/

void spell_mass_hysteria (int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  CHAR_DATA *vch, *vch_next;

  /* first hit the target */
  spell_hysteria( gsn_hysteria, level, ch, victim, target) ;

  /* then anyone grouped with target */
  for (vch = victim->in_room->people; vch; vch = vch_next){
    vch_next = vch->next_in_room;
    
    if (!is_same_group(vch, victim))
      continue;
    else if (vch == victim)
      continue;
    else if (is_area_safe_quiet( vch, ch))
      continue;
    else
      spell_hysteria( gsn_hysteria, level, ch, vch, target) ;
    m_yell(ch,vch,FALSE);
    if (vch->fighting == NULL)
      set_fighting(vch, ch );
  }
}

void spell_ethereal_passage( int sn, int level, CHAR_DATA *ch, void *vo,int target) {
  ROOM_INDEX_DATA* old;
  char arg[MIL];
  int door;
  bool fClear = TRUE;
  
  target_name = one_argument( target_name, arg );
  
  if ( arg[0] == '\0' ){
    send_to_char("What direction? <north/east/south/west/up/down>\n\r",ch);
    return;
  }
  if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL){
    send_to_char("Your mount refuses to enter the passage.\n\r",ch);
    return;
  }
  if (!str_cmp(arg,"north") || !str_cmp(arg,"n"))
    door = DIR_NORTH;
  else if (!str_cmp(arg,"east") || !str_cmp(arg,"e"))
    door = DIR_EAST;
  else if (!str_cmp(arg,"south") || !str_cmp(arg,"s"))
    door = DIR_SOUTH;
  else if (!str_cmp(arg,"west") || !str_cmp(arg,"w"))
    door = DIR_WEST;
  else if (!str_cmp(arg,"up") || !str_cmp(arg,"u"))
    door = DIR_UP;
  else if (!str_cmp(arg,"down") || !str_cmp(arg,"d"))
    door = DIR_DOWN;
  else
    {
      send_to_char("That direction does not exist. <north/east/south/west/up/down>\n\r",ch);
      return;
    }
  act("$n creates an ethereal passage.",ch,NULL,NULL,TO_ROOM);
  act("You create an ethereal passage.",ch,NULL,NULL,TO_CHAR);

  if (!IS_AFFECTED(ch, AFF_PASS_DOOR)){
    SET_BIT(ch->affected_by, AFF_PASS_DOOR );
    fClear = TRUE;
  }
  old = ch->in_room;
  move_char( ch, door, FALSE );

  if (fClear){
   REMOVE_BIT(ch->affected_by, AFF_PASS_DOOR );
   affect_check(ch, TO_AFFECTS, AFF_PASS_DOOR);
  }
  if (ch->in_room == old)
    act("An ethereal passage fizzles and closes.",ch,NULL,NULL,TO_ALL);
  else
    act("An ethereal passage opens and $n steps out.",ch,NULL,NULL,TO_ROOM);

}

void spell_insomnia( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected(victim, sn))
    {
        act("$N already has insomnia.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( saves_spell(level, victim, DAM_OTHER,skill_table[sn].spell_type) )
    {	
	if ( ch != victim )
	    act("You fail to deprive $N of sleep.",ch,NULL,victim,TO_CHAR);
	send_to_char("You resist the plague of insomnia.\n\r",victim);
	return;
    }
    act_new("You no longer feel the need to sleep!",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
    if ( ch != victim )
        act("You manage to infect $N with insomnia!",ch,NULL,victim,TO_CHAR);
    af.where     = TO_AFFECTS; 
    af.type      = sn;
    af.level     = level;
    af.location  = 0;
    af.duration  = 12;
    af.bitvector = 0;
    af.modifier  = 0;
    affect_to_char(victim,&af);
}

void spell_dysentery( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af, *paf;
    if ( is_affected(victim, sn))
    {
        act("$N already has dysentery.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if (IS_NPC(victim))
    {
	send_to_char("Mobs are not affected by nutrition.\n\r",ch);
	return;
    }
    if ( ch == victim || saves_spell(level, victim, DAM_DISEASE,skill_table[sn].spell_type) )
    {	
	if ( ch != victim )
	    act("You fail to disrupt $N's bowel movements.",ch,NULL,victim,TO_CHAR);
	send_to_char("Your stomach rumbles softly, but you manage to suppress the urge.\n\r",victim);
	return;
    }
    victim->position = POS_SITTING;
    update_pos(victim);
    act_new("You feel drained as your intestinal tracks goes haywire!",ch,NULL,victim,TO_CHARVICT,POS_DEAD);    
    act("$n's face changes color as $e grabs $s stomach in agonizing pain!",victim,NULL,NULL,TO_ROOM);    
    send_to_char("You quickly crouch on the ground as fecal matter explodes from your rear.\n\r",victim);
    act("$n crouches on the ground wincing painfully as fecal matter is expelled from $s rear.",victim,NULL,NULL,TO_ROOM);
    act("A strong odor fills the room.",victim,NULL,NULL,TO_ROOM);
    victim->pcdata->condition[COND_HUNGER] = 2;
    defecate(victim);
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.location  = 0;
    af.duration  = 8;
    af.bitvector = 0;
    af.modifier  = 0;
    paf = affect_to_char(victim,&af);
    if (!IS_NPC(ch))
      string_to_affect( paf, ch->name );
}

void spell_phantom_grasp( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af, *paf;
    int dam = number_range( 1, 5);
    int mod = (get_skill(ch, sn ) - 75) * 2;
    int chance = 50 + mod;

    act("You consume $N with the phantoms of the underworld.",ch,NULL,victim,TO_CHAR);
    act("You are consumed by the phantoms of the underworld.",victim,NULL,NULL,TO_CHAR);
    act("$N is consumed by the phantoms of the underworld.",ch,NULL,victim,TO_NOTVICT);

    paf = affect_find(victim->affected,sn);

    if ( saves_spell( level + (mod / 10), victim,DAM_NEGATIVE,skill_table[sn].spell_type) ){

      chance /= 2;
    }
    damage( ch, victim, dam, sn, DAM_NEGATIVE ,TRUE);
    if (number_percent() < chance ){
	af.where     = TO_AFFECTS2;
	af.type      = sn;
	af.level     = level;
	af.duration  = number_range(1, 3);
	af.location  = APPLY_SAVING_SPELL;
	af.modifier  = number_range(1, 3);;
	af.bitvector = 0;
	if (paf == NULL){
	  affect_to_char( victim, &af );	
	}
	else{
	  if (paf->duration > 24)
	    af.duration = 0;
	  if (paf->modifier > 14)
	    af.modifier = 0;
	  else{
	    act("The phantoms drains $n of $s magical resistence.",victim,NULL,NULL,TO_ROOM);
	    act("The phantoms drains you of your magical resistence.",victim,NULL,NULL,TO_CHAR);
	  }
	  affect_join( victim, &af );	
	}

    }
    else{
      send_to_char("You failed.\n\r", ch);
    } 
}

void spell_dark_shroud( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected(victim, sn))
    {
        send_to_char("You are already surrounded by a dark shroud.\n\r",victim);
        return;
    }
    send_to_char("You envelop yourself in a dark shroud.\n\r",ch);
    act("$n is enveloped in a dark shroud.",ch,NULL,NULL,TO_ROOM);
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.location  = 0;
    af.modifier  = 0;
    af.duration  = level/2;
    af.bitvector = 0;
    affect_to_char(victim,&af);
}

void spell_deteriorate( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int saves = 0;

    if ( is_affected(victim, sn))
    {
        act("$N is already deteriorated.",ch,NULL,victim,TO_CHAR);
        return;
    }

    //calc_saves reports saves chance with the base 40 included (5-95)
    saves = URANGE(0, calc_saves(level, victim, DAM_OTHER, skill_table[sn].spell_type) - 40, 55);

/* the spell level increases past 30 saves at rate of 2 level for 3 saves over 30.
   this results in final svs change of -2 per save over 30.
   At  0:  0 + 60 = 60
   At 10: 10 + 40 = 50
   At 20: 20 + 20 = 40
   At 30: 30 -  0 = 30
   At 40: 40 - 20 = 20
   At 50: 50 - 40 = 10
   At 55: 55 - 50 =  5
*/
    if ( saves_spell(level + 2 * (saves - 30) / 3, victim, DAM_OTHER, skill_table[sn].spell_type) ){
      if ( ch != victim )
	act("You fail to deteriorate $N.",ch,NULL,victim,TO_CHAR);
      act("You resist $n's deterioration.",ch,NULL,victim,TO_VICT);
      return;
    }
    if (ch != victim )
      act("You deteriorate $N's defenses.",ch,NULL,victim,TO_CHAR);
    send_to_char("You defense lowers from deterioration.\n\r",victim);
    
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.location  = APPLY_AC;
    af.modifier  = level;
    af.duration  = number_range(10, 24);
    af.bitvector = 0;
    affect_to_char(victim,&af);
    af.location  = APPLY_SAVING_SPELL;   
    af.modifier  = 4 * saves / 10;
    affect_to_char( victim, &af );
}

void spell_cure_dysentery( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if ( !is_affected( victim, skill_lookup("dysentery") ) )
    {
        if (victim == ch)
            act("You don't have dysentery.",ch,NULL,victim,TO_CHAR);
        else
            act("$N doesn't seem to have dysentery.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if (check_dispel(level - 3,victim,skill_lookup("dysentery")))
    {
        act_new("A feeling of relieve runs through your body.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
        act("$n looks relieved.",victim,NULL,NULL,TO_ROOM);   
    }
    else
        act("You failed to cure $N of dysentery.",ch,NULL,victim,TO_CHAR); 
}

void spell_cure_insomnia( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if ( !is_affected( victim, gsn_insomnia ) )
    {
        if (victim == ch)
            act("You don't have insomnia.",ch,NULL,victim,TO_CHAR);
        else
            act("$N doesn't seem to have insomnia.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if (check_dispel(level-3,victim,gsn_insomnia))
    {
        act_new("A familiar feeling of fatigue returns to your body.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
        act("$n looks more relaxed.",victim,NULL,NULL,TO_ROOM);   
    }
    else
        act("You failed to cure $N of insomnia.",ch,NULL,victim,TO_CHAR); 
}

void spell_remove_hysteria( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if ( !is_affected( victim, gsn_hysteria ) )
    {
        if (victim == ch)
            act("You are not affected by hysteria.",ch,NULL,victim,TO_CHAR);
        else
            act("$N doesn't appear to be affected by hysteria.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if (check_dispel(level,victim,gsn_hysteria))
    {
        act_new("You recollect your thoughts and your wits.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
        act("$n looks steadier.",victim,NULL,NULL,TO_ROOM);   
    }
    else
        act("You failed to remove $N of $s hysteria.",ch,NULL,victim,TO_CHAR); 
}

void spell_accession( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    AFFECT_DATA af;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dur;
    if (IS_NPC(victim))
    {
	send_to_char("Mobs cannot be affected.\n\r",ch);
	return;
    }
    if ( is_affected( victim, sn ) )
    {
        if (victim == ch)
            act("You are not ready tap into your hidden potential.",ch,NULL,victim,TO_CHAR);
        else
            act("$N is not ready to tap into $S hidden potential.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if (IS_EVIL(victim))
    {
	send_to_char("It is against your belief to help evils.\n\r",ch);
	return;
    }
    send_to_char("Your hidden potential is unleashed.\n\r",victim);
    dur = number_fuzzy(level/7);
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.location  = 0;
    af.modifier  = 0;
    af.duration  = dur+6;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    af.duration  = dur;
    if (ch == victim )
    {
    	af.location  = APPLY_DAMROLL;
    	af.modifier  = level / 15;
   	affect_to_char( victim, &af );
    	af.location  = APPLY_HITROLL;
    	af.modifier  = level / 15;
   	affect_to_char( victim, &af );
    	af.location  = APPLY_SAVING_SPELL;   
    	af.modifier  = - level / 5;
    	affect_to_char( victim, &af );
    	af.location  = APPLY_AC;
    	af.modifier  = - level;
    	affect_to_char(victim,&af);
    	af.location  = APPLY_MOVE;   
    	af.modifier  = 2*level;
    	affect_to_char( victim, &af );
    	af.location  = APPLY_MANA;   
    	af.modifier  = level;
    	affect_to_char( victim, &af );
    	af.location  = APPLY_HIT;   
    	af.modifier  = level;
	victim->hit += level;
	victim->mana += level;
	victim->move += 2*level;
    	affect_to_char( victim, &af );
    	af.location  = APPLY_CON;
    	af.modifier  = level / 25;
   	affect_to_char( victim, &af );
    	af.location  = APPLY_DEX;
    	af.modifier  = level / 15;
   	affect_to_char( victim, &af );
    	af.location  = APPLY_WIS;
    	af.modifier  = level / 8;
   	affect_to_char( victim, &af );
    	af.location  = APPLY_INT;
    	af.modifier  = level / 10;
   	affect_to_char( victim, &af );
    	af.location  = APPLY_STR;
    	af.modifier  = level / 20;
   	affect_to_char( victim, &af );
    }
    else
    {
    	act("You unleash $N's hidden potential.",ch,NULL,victim,TO_CHAR);
	level = victim->level;
    	if (victim->class == class_lookup("warrior"))
    	{
	    af.location  = APPLY_DAMROLL;
	    af.modifier  = level / 10;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HITROLL;
	    af.modifier  = level / 7;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_SAVING_SPELL;   
	    af.modifier  = - level / 10;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_AC;
	    af.modifier  = - level / 2;
	    affect_to_char(victim,&af);
	    af.location  = APPLY_MOVE;   
	    af.modifier  = level / 2;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_MANA;   
	    af.modifier  = level / 5;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HIT;   
	    af.modifier  = level/2;
	    affect_to_char( victim, &af );
	    victim->hit += level/2;
	    victim->mana += level/5;
	    victim->move += level/2;
	    af.location  = APPLY_CON;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_DEX;
	    af.modifier  = level / 18;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_WIS;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_INT;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_STR;
	    af.modifier  = level / 12;
	    affect_to_char( victim, &af );
	}
    	else if (victim->class == class_lookup("berserker"))
    	{
	    af.location  = APPLY_DAMROLL;
	    af.modifier  = level / 7;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HITROLL;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_SAVING_SPELL;   
	    af.modifier  = - level / 12;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_AC;
	    af.modifier  = - level/3;
	    affect_to_char(victim,&af);
	    af.location  = APPLY_MOVE;   
	    af.modifier  = level / 3;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_MANA;   
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HIT;   
	    af.modifier  = level;
	    affect_to_char( victim, &af );
	    victim->hit += level;
	    victim->mana += level/15;
	    victim->move += level/3;
	    af.location  = APPLY_CON;
	    af.modifier  = level / 12;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_DEX;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_WIS;
	    af.modifier  = level / 25;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_INT;
	    af.modifier  = level / 25;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_STR;
	    af.modifier  = level / 10;
	    affect_to_char( victim, &af );
	}
    	else if (victim->class == class_lookup("thief") || victim->class == class_lookup("bard"))
    	{
	    af.location  = APPLY_DAMROLL;
	    af.modifier  = level / 12;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HITROLL;
	    af.modifier  = level / 10;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_SAVING_SPELL;   
	    af.modifier  = - level / 7;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_AC;
	    af.modifier  = - level;
	    affect_to_char(victim,&af);
	    af.location  = APPLY_MOVE;   
	    af.modifier  = 3*level/2 ;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_MANA;   
	    af.modifier  = level / 3;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HIT;   
	    af.modifier  = level / 3;
	    affect_to_char( victim, &af );
	    victim->hit += level/3;
	    victim->mana += level/3;
	    victim->move += level/2;
	    af.location  = APPLY_CON;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_DEX;
	    af.modifier  = level / 10;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_WIS;
	    af.modifier  = level / 18;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_INT;
	    af.modifier  = level / 18;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_STR;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	}
    	else if (victim->class == class_lookup("ranger"))
    	{
	    af.location  = APPLY_DAMROLL;
	    af.modifier  = level / 11;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HITROLL;
	    af.modifier  = level / 11;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_SAVING_SPELL;   
	    af.modifier  = - level / 6;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_AC;
	    af.modifier  = - 2*level/3;
	    affect_to_char(victim,&af);
	    af.location  = APPLY_MOVE;   
	    af.modifier  = level;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_MANA;   
	    af.modifier  = level / 4;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HIT;   
	    af.modifier  = level / 2;
	    affect_to_char( victim, &af );
	    victim->hit += level;
	    victim->mana += level/4;
	    victim->move += level/2;
	    af.location  = APPLY_CON;
	    af.modifier  = level / 10;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_DEX;
	    af.modifier  = level / 17;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_WIS;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_INT;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_STR;
	    af.modifier  = level / 12;
	    affect_to_char( victim, &af );
	}
    	else if (victim->class == class_lookup("ninja"))
    	{
	    af.location  = APPLY_DAMROLL;
	    af.modifier  = level / 12;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HITROLL;
	    af.modifier  = level / 12;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_SAVING_SPELL;   
	    af.modifier  = - level / 5;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_AC;
	    af.modifier  = - 2*level/3;
	    affect_to_char(victim,&af);
	    af.location  = APPLY_MOVE;   
	    af.modifier  = level / 2;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_MANA;   
	    af.modifier  = level / 3;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HIT;   
	    af.modifier  = level / 3;
	    affect_to_char( victim, &af );
	    victim->hit += level/3;
	    victim->mana += level/3;
	    victim->move += level/2;
	    af.location  = APPLY_CON;
	    af.modifier  = level / 17;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_DEX;
	    af.modifier  = level / 12;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_WIS;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_INT;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_STR;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	}
    	else if (victim->class == class_lookup("paladin"))
    	{
	    af.location  = APPLY_DAMROLL;
	    af.modifier  = level / 10;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HITROLL;
	    af.modifier  = level / 10;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_SAVING_SPELL;   
	    af.modifier  = - level / 8;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_AC;
	    af.modifier  = - level/2;
	    affect_to_char(victim,&af);
	    af.location  = APPLY_MOVE;   
	    af.modifier  = level / 2;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_MANA;   
	    af.modifier  = level / 3;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HIT;   
	    af.modifier  = level / 3;
	    affect_to_char( victim, &af );
	    victim->hit += level/3;
	    victim->mana += level/3;
	    victim->move += level/2;
	    af.location  = APPLY_CON;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_DEX;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_WIS;
	    af.modifier  = level / 12;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_INT;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_STR;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	}
    	else if (victim->class == class_lookup("healer"))
    	{
	    af.location  = APPLY_DAMROLL;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HITROLL;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_SAVING_SPELL;   
	    af.modifier  = - level / 7;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_AC;
	    af.modifier  = - level/2;
	    affect_to_char(victim,&af);
	    af.location  = APPLY_MOVE;   
	    af.modifier  = level;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_MANA;   
	    af.modifier  = level/2;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HIT;   
	    af.modifier  = level/2;
	    affect_to_char( victim, &af );
	    victim->hit += level/2;
	    victim->mana += level/2;
	    victim->move += level;
	    af.location  = APPLY_CON;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_DEX;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_WIS;
	    af.modifier  = level / 12;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_INT;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_STR;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	}
    	else if (victim->class == class_lookup("battlemage") || victim->class == class_lookup("invoker"))
    	{
	    af.location  = APPLY_DAMROLL;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HITROLL;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_SAVING_SPELL;   
	    af.modifier  = - level / 4;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_AC;
	    af.modifier  = - level;
	    affect_to_char(victim,&af);
	    af.location  = APPLY_MOVE;   
	    af.modifier  = level;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_MANA;   
	    af.modifier  = level;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HIT;   
	    af.modifier  = level / 5;
	    affect_to_char( victim, &af );
	    victim->hit += level/5;
	    victim->mana += level;
	    victim->move += level;
	    af.location  = APPLY_CON;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_DEX;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_WIS;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_INT;
	    af.modifier  = level / 10;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_STR;
	    af.modifier  = level / 25;
	    affect_to_char( victim, &af );
	}
	else{
	    af.location  = APPLY_DAMROLL;
	    af.modifier  = level / 10;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HITROLL;
	    af.modifier  = level / 7;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_SAVING_SPELL;   
	    af.modifier  = - level / 10;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_AC;
	    af.modifier  = - level / 2;
	    affect_to_char(victim,&af);
	    af.location  = APPLY_MOVE;   
	    af.modifier  = level / 2;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_MANA;   
	    af.modifier  = level / 5;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_HIT;   
	    af.modifier  = level/2;
	    affect_to_char( victim, &af );
	    victim->hit += level/2;
	    victim->mana += level/5;
	    victim->move += level/2;
	    af.location  = APPLY_CON;
	    af.modifier  = level / 15;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_DEX;
	    af.modifier  = level / 18;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_WIS;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_INT;
	    af.modifier  = level / 20;
	    affect_to_char( victim, &af );
	    af.location  = APPLY_STR;
	    af.modifier  = level / 12;
	    affect_to_char( victim, &af );
	}
    }
}

void spell_morph( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    char arg[MIL];
    int chance = number_percent();
    AFFECT_DATA af;
    target_name = one_argument(target_name, arg);
    if (is_affected(ch,gsn_bat_form) || is_affected(ch,gsn_wolf_form) || is_affected(ch,gsn_mist_form))
    {
	send_to_char("You are already morphed.\n\r",ch);
	return;
    }
    if (arg[0] == '\0')
    {
        send_to_char("syntax: cast 'morph' <form>.\n\r",ch);
        return;
    }

    chance -= 2*(get_curr_stat(ch,STAT_LUCK) - 16);
    if (!str_cmp(arg, "bat") && chance < get_skill(ch,gsn_bat_form))
    {
      const int cost = skill_table[gsn_bat_form].min_mana;
      if (ch->mana < cost){
	send_to_char("You cannot concentrate sufficiently.\n\r", ch);
	return;
      }
      else
	ch->mana -= cost;

      affect_strip(ch, gen_conceal);
	send_to_char("You submit yourself to the sweet agony of transformation and take form of a bat.\n\r",ch);
	act("Shadows thicken around $n as his flesh melds itself into a bat.",ch,NULL,NULL,TO_ROOM);
    	af.where     = TO_AFFECTS;
    	af.type      = gsn_bat_form;
    	af.level     = level;
    	af.duration  = 12;
    	af.location  = APPLY_SIZE;
    	af.modifier  = -2;  
	af.bitvector = 0;
    	affect_to_char( ch, &af );

    	af.where     = TO_AFFECTS;
    	af.location  = APPLY_DAMROLL;
    	af.modifier  = -20;  
    	af.bitvector = AFF_FLYING;
    	affect_to_char( ch, &af );

	af.where = TO_VULN;
	af.bitvector = VULN_SOUND;
    	af.location  = APPLY_HITROLL;
    	af.modifier  = -15;  
    	affect_to_char( ch, &af );

    	af.location  = APPLY_CON;
    	af.modifier  = -7;  
    	affect_to_char( ch, &af );

    	af.location  = APPLY_DEX;
    	af.modifier  = 5;  
    	affect_to_char( ch, &af );

    	af.location  = APPLY_STR;
    	af.modifier  = -10;  
    	affect_to_char( ch, &af );

	ch->short_descr = str_dup("a vampire bat");
	ch->dam_type = 10;
	check_improve(ch,gsn_bat_form,TRUE,1);
	return;
    }
    else if (!str_cmp(arg, "wolf") && chance < get_skill(ch,gsn_wolf_form))
    {
      const int cost = skill_table[gsn_wolf_form].min_mana;
      if (ch->mana < cost){
	send_to_char("You cannot concentrate sufficiently.\n\r", ch);
	return;
      }
      else
	ch->mana -= cost;
      affect_strip(ch, gen_conceal);
	send_to_char("You submit yourself to the sweet agony of transformation and take form of a wolf.\n\r",ch);
	act("Shadows thicken around $n as his flesh melds itself into a giant wolf.",ch,NULL,NULL,TO_ROOM);
	check_social(ch, "howl", "");
    	af.where     = TO_AFFECTS2;
    	af.type      = gsn_wolf_form;
    	af.level     = level;
    	af.duration  = 12;
    	af.modifier  = 0;  
    	af.location  = APPLY_NONE;
    	af.bitvector = AFF_KEEN_SIGHT;
    	affect_to_char( ch, &af );
    	af.bitvector = 0;
    	af.location  = APPLY_DAMROLL;
    	af.modifier  = - ch->level / 5;
    	affect_to_char( ch, &af );
    	af.location  = APPLY_HITROLL;
    	af.modifier  = ch->level/4;  
    	affect_to_char( ch, &af );
    	af.location  = APPLY_AC;
    	af.modifier  = -ch->level*3;  
    	affect_to_char( ch, &af );
    	af.location  = APPLY_DEX;
    	af.modifier  = 5;  
    	affect_to_char( ch, &af );
    	af.location  = APPLY_STR;
    	af.modifier  = -1;  
    	affect_to_char( ch, &af );
	ch->short_descr = str_dup("a vampire wolf");
	ch->dam_type = 5;
	check_improve(ch,gsn_wolf_form,TRUE,1);
	return;
    }
    else if (!str_cmp(arg, "mist") && chance < get_skill(ch,gsn_mist_form))
    {
      const int cost = skill_table[gsn_mist_form].min_mana;
      if (ch->mana < cost){
	send_to_char("You cannot concentrate sufficiently.\n\r", ch);
	return;
      }
      else
	ch->mana -= cost;
      affect_strip(ch, gen_conceal);
	send_to_char("You submit yourself to the sweet agony of transformation and disperse into fine mist.\n\r",ch);
	act("$n evaporates into a fine cloud of mist.",ch,NULL,NULL,TO_ROOM);
    	af.where     = TO_AFFECTS;
    	af.type      = gsn_mist_form;
    	af.level     = level;
    	af.duration  = 12;
    	af.modifier  = 0;  
    	af.location  = APPLY_NONE;
    	af.bitvector = AFF_PASS_DOOR;
    	affect_to_char( ch, &af );
    	af.bitvector = 0;
    	af.location  = APPLY_DAMROLL;
    	af.modifier  = -ch->damroll;  
    	af.bitvector = AFF_FLYING;
    	affect_to_char( ch, &af );
    	af.location  = APPLY_HITROLL;
    	af.modifier  = -ch->hitroll;  
    	affect_to_char( ch, &af );
    	af.location  = APPLY_CON;
    	af.modifier  = -5;  
    	affect_to_char( ch, &af );
    	af.location  = APPLY_DEX;
    	af.modifier  = -5;  
    	affect_to_char( ch, &af );
    	af.location  = APPLY_STR;
    	af.modifier  = -10;  
    	affect_to_char( ch, &af );

	ch->short_descr = str_dup("a cloud of mist");
	check_improve(ch,gsn_mist_form,TRUE,1);
	return;
    }
    else
    {
	send_to_char("You failed to begin the Transformation.\n\r",ch);
	return;
    }
}

void spell_dark_meta( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    AFFECT_DATA af;
    AFFECT_DATA* paf;
    OBJ_DATA* obj = (OBJ_DATA*) vo;

  if (obj->item_type != ITEM_WEAPON)
    {
      send_to_char("That item is not a weapon.\n\r",ch);
      return;
    }

  if (obj->wear_loc == WEAR_NONE)
    {
      send_to_char("You must have the item equipped.\n\r", ch);
      return;
    }

    if ( (paf = affect_find(obj->affected, gen_dark_meta)) != NULL)
    {
      if (paf->modifier++ < 3)
	{
	  act("You fortify your link to $p.",ch, obj, NULL, TO_CHAR);
	  act("$p's surface ripples slightly.", ch, obj, NULL, TO_ROOM);
	  paf->duration += 2;
	}
      else
	send_to_char("You have reached the limit of your abilities.\n\r", ch);
	  return;
    }

    act("You transform $p and attune it to your hunger.",ch, obj, NULL, TO_CHAR);
    act("$p's surface ripples and it seems to meld itself into $n's hand.", ch, obj, NULL, TO_ROOM);

    af.where     = TO_NONE;
    af.type      = gen_dark_meta;
    af.level     = level;
    af.duration  = 6;
    af.location  = APPLY_NONE;
    af.modifier  = 0;  
    af.bitvector = 0;
    affect_to_obj( obj, &af );
}

void spell_soul_steal( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    if ( obj->item_type != ITEM_CORPSE_PC && obj->item_type != ITEM_CORPSE_NPC)
    {
        send_to_char( "There is no life force in that.\n\r",ch);
        return;
    }
    if ( obj->condition < 2)
      {
        send_to_char( "There is no life force in that.\n\r",ch);
        return;
      }
    if (obj->item_type == ITEM_CORPSE_PC && obj->level < 15 && !IS_IMMORTAL(ch))
      {
        send_to_char("Not that corpse.\n\r",ch);
        return;
      }
    if (obj->pIndexData->vnum == OBJ_VNUM_FAKE_CORPSE)
    {
        send_to_char("Not that corpse.\n\r",ch);
        return;
    }
    if (!can_loot(ch, obj)){
      act("$g's power shields $p from your greedy maw.", ch, obj, NULL, TO_CHAR);
      return;
    }
    act("You suck the life force from $p.",ch,obj,NULL,TO_CHAR);
    act("$n sucks the life force from $p.",ch,obj,NULL,TO_ROOM);
    
    act("You are empowered by cosuming their life force.", ch,NULL,NULL,TO_CHAR);
    ch->mana = UMIN( ch->mana + number_range(obj->level, 3 * obj->level), ch->max_mana );
    update_pos( ch );
    act("$p withers into a desiccated carcass.",ch,obj,NULL,TO_ALL);
    free_string(obj->description);
    obj->description = str_dup("A desiccated corpse lies here.");
    obj->condition = 1;
}

void spell_bloodlust( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = NULL, *vch, *vch_next;
    AFFECT_DATA af;
    bool found = FALSE;
    if (is_affected( ch, sn ) )
    {
	send_to_char("You are already in a rage of lust.\n\r",ch);
	return;
    }
    if (IS_NPC(ch))
	return;
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next_in_room;
        if ( vch != ch && !is_safe(ch,vch) && !IS_SET(vch->act,ACT_UNDEAD))
        {
	    victim = vch;
	    found = TRUE;
	    break;
	}
    }
    if (!found)
    {
	send_to_char("Nothing here interests you.\n\r",ch);
	return;
    }
    send_to_char("Your senses goes haywire as you lust for blood amplifies!\n\r",ch);
    act("$n's eyes flood crimson red.",ch,NULL,NULL,TO_ROOM);
    m_yell(ch,victim,FALSE);
    multi_hit(ch,victim,TYPE_UNDEFINED);
    ch->pcdata->condition[COND_HUNGER] = 0;
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy(level/5);
    af.location  = APPLY_NONE;
    af.modifier  = 0;  
    af.bitvector = AFF_KEEN_SIGHT;
    affect_to_char( ch, &af );
    af.where     = TO_AFFECTS;
    af.location  = APPLY_NONE;
    af.modifier  = 0;  
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( ch, &af );
    af.bitvector = AFF_DETECT_INVIS;
    af.location  = APPLY_DAMROLL;
    af.modifier  = ch->level/6;  
    af.bitvector = 0;
    affect_to_char( ch, &af );
    af.location  = APPLY_HITROLL;
    affect_to_char( ch, &af );
}

void spell_twilight_shift( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
  char arg[MIL];
  CHAR_DATA *victim = NULL, *mob;
  int chance = number_percent(), skill =get_skill(ch,skill_lookup("twilight shift")) ;
  ROOM_INDEX_DATA *pRoomIndex = NULL;
  int count = 0, number = number_argument( target_name, arg );

  if (target_name == NULL)
    {
      send_to_char("Shift to what?\n\r", ch);
      return;
    }
  if (chance > skill)
    {
      send_to_char("You fail to complete the Transformation.\n\r", ch);
      return;
    }
  if (chance < 100 - skill){
    int e_r1[] = {ROOM_NO_TELEPORTIN, ROOM_NO_GATEIN, ROOM_NO_SUMMONIN};
    int va = ch->in_room->area->vnum;
    int area_pool = 1;
    /* get the random room */
    pRoomIndex =  get_rand_room(va,va,	//area range (0 to ignore)
				0,0,		//room ramge (0 to ignore)
				NULL,0,	//areas to choose from
				NULL,0,	//areas to exclude
				NULL,0,	//sectors required
				NULL,0,	//sectors to exlude
				NULL,0,	//room1 flags required
				e_r1,2,	//room1 flags to exclude
				NULL,0,	//room2 flags required
				NULL,0,	//room2 flags to exclude
				area_pool,	//number of seed areas
				FALSE,	//exit required?
				FALSE,	//Safe?
				ch);		//Character for room checks
    
  }
  /* scan through the mobs in this area and see if we match anyone */
  for (mob = char_list; mob; mob = mob->next){
    if (mob->in_room == NULL || mob->in_room->area != ch->in_room->area)
      continue;
    else if ( !can_see( ch, mob ) )
      continue;      
    else if (!is_auto_name(arg, mob->name) )
      continue;
    
    if ( ++count == number ){
      victim = mob;
      break;
    }
  }
  
  if(victim == NULL){
    send_to_char("You fail to locate your target\n\r", ch);
    return;
  }
  else if ( (victim == ch || victim->in_room == NULL)
	    || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE) 
	    || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)   
	    || IS_SET(victim->in_room->room_flags, ROOM_NO_GATEIN)
	    || IS_SET(ch->in_room->room_flags, ROOM_NO_GATEOUT)  
	    || IS_SET(victim->in_room->room_flags, ROOM_NO_INOUT)
	    || IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)){
    send_to_char("You fail to locate a safe path to your target\n\r", ch);
    return;
  }
  else if ( (victim->level >= level + 5)
	    || (!IS_NPC(victim) && victim->level > LEVEL_HERO)
	    || (!IS_IMMORTAL(ch) && IS_IMMORTAL(victim))
	    || is_affected(victim,gsn_coffin)
	    || is_affected(victim,gsn_entomb)
	    || is_affected(victim, gsn_tele_lock)
	    || is_affected(victim, gsn_burrow)){
    send_to_char("Something interferes with your spell.\n\r",ch);
    return;
  }
  else if (pRoomIndex == NULL)
    pRoomIndex = victim->in_room;
  
  act("$n scatters into a swarm of bats and disperses into the night.",ch,NULL,NULL,TO_ROOM);
  act("Your body dissolves into a swarm of bats.",ch,NULL,NULL,TO_CHAR);
  char_from_room(ch);
  char_to_room(ch,pRoomIndex);
  do_look(ch,"auto");
  act("Your body takes shape as the bats rejoins to mold your form.",ch,NULL,NULL,TO_CHAR);
  act("The sky darkens as a swarm of bats swoop in and converges into the form of $n.",ch,NULL,NULL,TO_ROOM);
}

void spell_ecstacy( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int opp = 0;
    bool fElder = get_skill(ch, gsn_unholy_gift) > 1;

    if ( is_affected(victim, sn) )
    {
        act( "$N is already in a state of ecstacy.", ch, NULL, victim, TO_CHAR);
        return;
    }
    if ( victim == ch )
    {
        send_to_char( "You can't fool yourself!\n\r", ch );
        return;
    }
    if (is_affected(victim, gen_watchtower)){
      sendf( ch, "%s is shielded within the Watchtower.\n\r", PERS(victim, ch ));
      return;
    }
    if ( (ch->sex == 1 && victim->sex == 2) || (ch->sex == 2 && victim->sex == 1))
	opp = 3;
    else if ( (ch->sex == 1 && victim->sex == 1) || (ch->sex == 2 && victim->sex == 2))
      opp = 0;
    if (IS_AWAKE(victim) || IS_UNDEAD(victim)){
      if ( victim->fighting != NULL 
	   || (IS_NPC(victim) 
	       && IS_SET(victim->act,ACT_UNDEAD)) 
	   || (IS_NPC(victim) 
	       && IS_SET(victim->act,ACT_TOO_BIG))
	   || saves_spell( level + 2 * opp, victim, DAM_CHARM, skill_table[sn].spell_type) )
	{
	  act( "You fail to induce $N into ecstacy.", ch, NULL, victim, TO_CHAR);
	  return;
	}
      
      if (is_affected(victim, gsn_horse) && monk_good(victim, WEAR_HEAD) && !IS_NPC(victim) && !IS_IMMORTAL(ch))
	{
	  if (fElder){
	  act( "You try to reverse $n's attempt to charm you, and fail!", ch, NULL, victim, TO_VICT );
	  act("$N tries to reverse your charm, and fails!", ch, NULL, victim, TO_CHAR );
	  }
	  else{
	    act( "You reverse $n's attempt to charm you, and charm $M instead!", ch, NULL, victim, TO_VICT );
	    act("$N reverses your charm, and charms you instead!", ch, NULL, victim, TO_CHAR );
	    
	    af.where     = TO_AFFECTS;
	    af.type      = sn;
	    af.level     = level;
	    af.duration  = 0;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = 0;
	    affect_to_char( ch, &af );
	    
	    send_to_char( "Your wildest fantasy comes true right in front of your eyes!\n\r", ch );
	    act("$n's eyes wanders dreamily as a wide grin stretches across $s face.",ch,NULL,NULL,TO_ROOM);
	    stop_follower(ch);
	    return; 
	  }
	}
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 1 + number_fuzzy( level / 15);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "Your wildest fantasy comes true right in front of your eyes!\n\r", victim );
    act("$n's eyes wanders dreamily as a wide grin stretches across $s face.",victim,NULL,NULL,TO_ROOM);
    stop_follower(victim);
}

void spell_shadowstalk( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    CHAR_DATA *shadow;
    OBJ_DATA *wield;
    int i;
    char buf[MSL];
    char *name;
    bool fElder = get_skill(ch, gsn_unholy_gift) > 1;

    if ( is_affected(ch, sn) )
    {
	send_to_char("You are not ready to give life to another shadow yet.\n\r",ch);
        return; 
    }
    if ( ( victim = get_char_world( ch, target_name ) ) == NULL
    ||   victim == ch || IS_NPC(victim) || is_safe(ch,victim))
    {
        send_to_char("You failed.\n\r", ch);
        return;
    }
    send_to_char("You mark a mystical symbol on the ground, and your shadow takes form!\n\r",ch);
    act("$n marks a strange symbol on the ground, and $s shadow comes to life!",ch,NULL,NULL,TO_ROOM);
    shadow = create_mobile( get_mob_index(MOB_VNUM_SHADOW) );
    for (i=0;i < MAX_STATS; i++)
        shadow->perm_stat[i] = ch->perm_stat[i];

//We spiff up some of the stats as mobs are weaker by nature.
    shadow->max_hit = (4 * ch->max_hit) / 3;
    shadow->hit = shadow->max_hit;
    shadow->max_mana = (4 * ch->max_mana) / 3;
    shadow->mana = shadow->max_mana;
    shadow->level = fElder ? ch->level + 3: ch->level;
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
    shadow->hunting = victim;

    shadow->pCabal = ch->pCabal;
    shadow->leader = ch;

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = fElder ? 12 : 24;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    /* make sure its killed in 24 hours, if not by sun */
    af.type = gsn_timer;
    af.where     = TO_AFFECTS;
    af.level     = level;
    af.duration  = fElder ? 48 : 24;
    af.modifier  = 0;  
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( shadow, &af );



}

void spell_restore_faith( int sn, int level, CHAR_DATA *ch, void *vo,int target) 
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if ( !is_affected( victim, gsn_blasphemy ) )
    {
        if (victim == ch)
            act("You did not lose your faith.",ch,NULL,victim,TO_CHAR);
        else
            act("$N doesn't seem to have lost $s faith.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if (check_dispel(level-5,victim,gsn_blasphemy))
    {
        act_new("You once again have faith in your religion.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
        act("$n looks more determined.",victim,NULL,NULL,TO_ROOM);   
    }
    else
        act("You failed to restore $N's faith.",ch,NULL,victim,TO_CHAR); 
}

//03-25 13:00 Viri: Changed bat morp messages, will do the rest once
//I get to those forms.
//03-31-00 Added Arms_of_gaia spell for druids. -Shadow
//03-1-23:00 Viri: remmed out arms_of_gaia code so I can compile my stuff:)
//rem it back in when you debug it:)
//04-01 9:00 Ath: Added spell_mass_hysteria
//04-03 23:00 Viri: Fixed the bug in blood lust that removed detet invis.
//04-03 23:50 viri: Added Keensight to wolf_form for vamps.
//04-13 10:30 Viri: Made thnder storm do bonus damage for high AC.
//04-13 11:00 Viri: Gave ice storm chance to crumble a weapon.
//created effect_freeze_weapon(..) for that.
//04-24 Viri: modfifed iceball for steadied damage
//04-24 Viri: reworked Hellstream for different item busting effect.
