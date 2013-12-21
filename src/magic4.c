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
#include "recycle.h"
#include "interp.h"

extern char *target_name;

void spell_isolate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
/* 
The spell when saves are failed does one of the following:
- If cast on leader ONE of the followers is nuked.
- If cast on follower he is unlinked from leader.
- Save is vs Mental, if NPC it is based on master/leader if any.
*/

  CHAR_DATA* victim = (CHAR_DATA*) vo;
  CHAR_DATA* leader = NULL;
  CHAR_DATA* follower = victim;

//Select the leader.
  if (victim->leader != NULL)
    leader = victim->leader;
//if no leader, check for master (person being followed)
  if (leader == NULL && victim->master != NULL)
    leader = victim->master;
  
//if there is no leader at this point we check if someone is following victim
  if (leader == NULL)
    {
      CHAR_DATA* vch;
      bool fFound = FALSE;
      for (vch = victim->in_room->people; vch != NULL; vch = vch->next_in_room)
	{
	  if (IS_IMMORTAL(victim))
	    continue;
	  if (victim == vch->leader)
	      fFound = TRUE;
	  else
	    if (victim == vch->master)
	      fFound = TRUE;

	  if (fFound)
	    {
	      leader = victim;
	      follower = vch;
	      break;
	    }
	}//end follower search.
      //check if not found.
      if (leader == NULL)
	{
	  act("There is no point, $N is already utterly alone.", ch, NULL, victim, TO_CHAR);
	  return;
	}
    }//end if no leader.

  //  sendf(ch, "master: %s victim: %s\n\r", leader->name, victim->name);
  //we try to stop target from following leader.
    if (IS_NPC(follower))
      { 
	if (check_immune(follower, DAM_CHARM, TRUE) == IS_IMMUNE){
	  act("$N ignores your attempt.", ch, NULL, follower, TO_CHAR);
	  return;
	}
	if(!saves_spell(level, leader, DAM_CHARM, sn))
	  {
	    act("$N loses all interest in you and returns from where $E came.", leader, NULL, follower, TO_CHAR);
	    act("$N loses all interest in $n and returns from where $E came.",  leader, NULL, follower, TO_ROOM);
	    stop_follower(follower);
	    extract_char(follower, TRUE);
	    set_fighting(ch, leader);
	    return;
	  }
      }
    else
      {
	if (!saves_spell(level, follower, DAM_CHARM, sn))
	  {
	    act("You suddenly lose all interest in following anyone.", follower, NULL, NULL, TO_CHAR);
	    act("$N suddenly wonders off from the group.",  victim, NULL, follower, TO_ROOM);
	    stop_follower(follower);
	    return;
	  }
      }
    act("$N resists your attempt.", ch, NULL, follower, TO_CHAR);

    return;
}//end isolate.


void spell_minister( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  /* The good cleric spell that causes victims mana to be sapped each turn. */
  CHAR_DATA*  victim = (CHAR_DATA* ) vo;
  AFFECT_DATA af;
  AFFECT_DATA* paf;

if (is_affected(victim, gen_seremon))
    {
      act("You have already gifted $N with your teachings.", ch, NULL, victim, TO_CHAR);
      return;
    }

  if (!saves_spell(level, victim, DAM_MENTAL, sn))
    {
      act("You place your teachings in $N's mind.", ch, NULL, victim, TO_CHAR);
      af.type = gen_seremon;
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.level = ch->level;
      af.modifier = af.level;//how much mana to lose on first tick (-5 there after)
      af.duration = number_fuzzy(af.level / 8);
      paf = affect_to_char(victim, &af);

      string_to_affect(paf, PERS2( ch ));
    }
  else
    act("You failed in your righteous misson.", ch, NULL, victim, TO_CHAR);
}


void spell_bless_arms( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  /* another run of the mill spell, basicly adds teh bless/evil flag on the item. */
  OBJ_DATA* obj = (OBJ_DATA*) vo;
  AFFECT_DATA* paf;
//bool
  bool fIsGood = TRUE;
  bool fFound = FALSE;

  //const
  const int magic_mod = -10;
  const int glow_mod = -20;
  const int hum_mod = -30;

  //data
  int chance = get_skill(ch, sn);
  int g_type = 0;

  //EZ
  if (obj->item_type != ITEM_WEAPON 
      && !(obj->item_type == ITEM_ARMOR && CAN_WEAR(obj, ITEM_WEAR_SHIELD))
      )
    {
      act("$g will only place his blessing onto weapons and shields.", ch, NULL, NULL, TO_CHAR);
      return;
    }

  if (is_affected_obj(obj, sn)){
      send_to_char("This item has already been blessed. Your prayers ring silent.\n\r", ch);
      return;
    }

  if (is_affected(ch, skill_lookup("calm")))
    {
      send_to_char("You are in far to peacefull mood to think of violence.", ch);
      return;
    }

  if (IS_EVIL(ch))
    fIsGood = FALSE;

//get the chance.
  if (IS_OBJ_STAT(obj,ITEM_MAGIC))
    chance += magic_mod;
    if (IS_OBJ_STAT(obj,ITEM_GLOW))
    chance += glow_mod;
  if (IS_OBJ_STAT(obj,ITEM_HUM))
    chance += hum_mod;
  act("You begin the prayer and call onto your faith in $g.", ch, obj, NULL, TO_CHAR);
  //now we enchant.
  if (number_percent() < chance)
    {
      AFFECT_DATA af;
      if (fIsGood)
	act("A holy white aura envelops $p.", ch, obj, NULL, TO_ALL);
      else
	act("A dark ominous aura envelops $p.", ch, obj, NULL, TO_ALL);
      SET_BIT(obj->extra_flags, (fIsGood? ITEM_BLESS : ITEM_EVIL));
      //special deity based effects now:
      if (IS_NPC(ch))
	return;
      af.type = sn;
      af.where = TO_NONE;
      af.bitvector = 0;
      af.level = ch->level;
      af.duration = -1;
      af.location = APPLY_NONE;

//Select the god by number.
      g_type = deity_table[IS_NPC(ch) ? 0 : ch->pcdata->way].path;
      
/* LIFE */
      if (g_type == PATH_LIFE)
	{
	  af.location = APPLY_HIT;
	  af.modifier = 5;
	}
      /* UNDEAD */
      else if (g_type == PATH_DEATH)
	{
	  if (obj->item_type == ITEM_ARMOR)
	    {
	      af.location = APPLY_SAVING_MENTAL;
	      af.modifier = -3;
	    }
	  else
	    {
	      af.location = APPLY_MANA;
	      af.modifier = -5;
	      obj->value[3] = attack_lookup("drain");
	    }
	}
      /* KNOWLEDGE */
      else if (g_type == PATH_KNOW)
	{
	  af.location = APPLY_SAVING_SPELL;
	  af.modifier = -1;
	}
      /* CHANCE */
      else if (g_type == PATH_CHANCE)
	{
	  af.location = APPLY_LUCK;
	  af.modifier = 1;
	}
      else{
	af.location = APPLY_AC;
	af.modifier = -5;
      }
      if (af.location == APPLY_NONE)
	return;
      //we do a whole enchant thing.
      if (!obj->enchanted)
	{
	  AFFECT_DATA *af_new;
	  obj->enchanted = TRUE;
	  for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next) 
	    {
	      af_new = new_affect();
	      af_new->next = obj->affected;
	      obj->affected = af_new;
	      af_new->where= paf->where;
	      af_new->type = UMAX(0,paf->type);
	      af_new->level= paf->level;
	      af_new->duration= paf->duration;
	      af_new->location= paf->location;
	      af_new->modifier= paf->modifier;
	      af_new->bitvector= paf->bitvector;
	      af_new->has_string = paf->has_string;
	      af_new->string = paf->string;
	    }
	}
      //now we check if we should amplify effects or just add them.
      for(paf = obj->affected; paf != NULL; paf = paf->next)
	{
	  if (paf->location == af.location)
	    {
	      fFound = TRUE;
	      paf->modifier += af.modifier;
	      break;
	    }
	}
      if (!fFound)
	affect_to_obj(obj, &af);
    }//end enchant/bless
  else
    act("Nothing seems to happen.", ch, NULL, NULL, TO_ALL);
}

void spell_holy_hands( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  /* This is the initilizaer for the gen_holy_hands effect. */

  //the only decision we make here is setting modifier to the god.
  AFFECT_DATA af;
  int g_type = 0;

  if (get_eq_char(ch, WEAR_WIELD) != NULL)
    {
      send_to_char("You may not hold a weapon while your hands are empowered.\n\r", ch);
      return;
    }

  if (is_affected(ch, gen_holy_hands))
    {
      act("$g's power already courses through your hands.", ch, NULL, NULL, TO_CHAR);
	return;
    }

  if (is_affected(ch, skill_lookup("calm")))
    {
      send_to_char("You feel far to peacefull.", ch);
      return;
    }

  g_type = deity_table[IS_NPC(ch) ? 0 : ch->pcdata->way].path;

  af.type = gen_holy_hands;
  af.bitvector = 0;
  af.where = TO_AFFECTS;
  af.location = APPLY_NONE;
  af.modifier = g_type;
  af.duration = number_fuzzy(level / 5);
  af.level = level;
  affect_to_char(ch, &af);
}


bool eff_turn_undead(int level, CHAR_DATA *vch)
{
  /* This is the effect for new turn undead. */
  CHAR_DATA* save_ch = vch;

  //npcs's save on teh leaders.
  if (vch->leader != NULL && IS_NPC(vch))
    save_ch = vch->leader;


  if (saves_spell(level + 5, save_ch, DAM_MENTAL,skill_table[gsn_turn_undead].spell_type)
      || is_affected(vch, gsn_turn_undead)
      )
    return FALSE;
  
  //anti abuse check for terror.
  if (IS_NPC(vch) 
      && (IS_SET(vch->act, ACT_TOO_BIG) 
	   || IS_SET(vch->act, ACT_PRACTICE)
	   || IS_SET(vch->act, ACT_TRAIN)
	   || !IS_SET(vch->act, ACT_UNDEAD)
	   || IS_SET(vch->act, ACT_NOPURGE)
	   || IS_SET(vch->act, ACT_IS_HEALER)
	   || IS_SET(vch->act, ACT_GAIN)
	   )
      )
    return FALSE;
  else
    return TRUE;
}//end effect.





void spell_turn_undead( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *vch, *vch_next;
    AFFECT_DATA af;
    int i = 0;

    act("$n brandishes $s holy symbol and utters words summoning $g's divine power!",ch,NULL,NULL,TO_ROOM);
    act("You brandish your holy symbol and utter words summoning $g's divine power!",ch,NULL,NULL,TO_CHAR);

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
      {
	vch_next = vch->next_in_room;
        if (vch != ch && !is_area_safe(ch,vch) && IS_SET(vch->act,ACT_UNDEAD) )
        {
	  m_yell(ch,vch,FALSE);
	  if (!eff_turn_undead(level, vch))
	    {
	      act("$N resists $g's power!.", ch, NULL, vch, TO_CHAR);
	      act("You shrug off $n's feeble attempts.", ch, NULL, vch, TO_VICT);
	      damage(ch, vch, level, sn, DAM_HOLY,TRUE);
	    }
	  else
	    {
	      int dam =  number_range(level, 3 * level);
	      if (IS_NPC(vch))
		dam *= 2;
	      damage(ch, vch, dam, sn, DAM_HOLY,TRUE);
	      act("$n's eyes widen in fear of $G's power.", vch, NULL, ch, TO_ROOM);
	      send_to_char("You panic and try to flee!\n\r", vch);
	
	      af.type = sn;
	      af.duration = number_fuzzy(2);
	      af.level  = level;

	      af.where = TO_AFFECTS;
	      af.bitvector = 0;

	      af.location = APPLY_HITROLL;
	      af.modifier = -level/5;
	      affect_to_char(vch, &af);

	      af.location = APPLY_DAMROLL;
	      af.modifier = -level/5;
	      affect_to_char(vch, &af);

	      af.location = APPLY_SAVING_SPELL;
	      af.modifier = level/5;
	      affect_to_char(vch, &af);
	      vch->fighting = ch;
	      for (i = 0; i < 3; i++)
		{
		  if (vch->in_room != ch->in_room)
		    break;
		  else
		    do_flee(vch, "");
		}
	    }//end if turned
	}//end if area save.
      }//end for
}//end turn undead


//DEITY EFFECTS FOR DIVINE INTERVENSION
/* FAITH (remove harmful spells) */
void divine_faith(CHAR_DATA* ch, int level, int sn)
{
  AFFECT_DATA* paf;
  //called only if god answers :)

  act("The soft peacefull aura of $g envelops you as your body and soul is restored.", ch, NULL, NULL, TO_CHAR);
  act("A soft white light envelops $n as $g manifests his love for $m.\n\r You could almost hear ring of bells and angels singing.",
      ch, NULL, NULL, TO_ROOM);
  broadcast(ch, "A sense of serenity and calm fills you momentraly.");
  ch->hit += 4 * level;

  //start healing.
  for (paf = ch->affected; paf != NULL; paf = paf->next){
    if (IS_GNBIT(paf->type, GN_HAR))
      paf->duration = 0;
  }
  affect_strip(ch, gsn_embrace_poison);
  affect_strip(ch, gen_unlife);
}


/* COMBAT, (enlarge and haste) */
void divine_combat(CHAR_DATA* ch, int level, int sn)
{
  AFFECT_DATA af;

  act("$g's strength and power suffocates your body as it grows in size.", ch, NULL, NULL, TO_CHAR);
  act("Thunder rumbles in the distance even as $n seems to grow in size and power.", ch, NULL, NULL, TO_ROOM);
  broadcast(ch, "A rumbling thunder rolls in the distance.");

  af.type = sn;
  af.duration = number_fuzzy(12);
  af.level = level;

  af.where = TO_AFFECTS;
  af.bitvector = AFF_HASTE;
  af.location = APPLY_DAMROLL;
  af.modifier = 3 * af.duration / 4;
  affect_to_char(ch, &af);

  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_HITROLL;
  affect_to_char(ch, &af);
}

/* KNOWLDGE-NATURE/MYSTIC */
void divine_knowledge(CHAR_DATA* ch, int level, int sn)
{
  AFFECT_DATA* paf;
  //called only if god answers :)
  act("The air around you hums and crackles with potent energies as giant vortex of magic surrounds you briefly.", ch, NULL, NULL, TO_CHAR);
  act("Crackling with titanic energies a shimmering vortex of magic surrounds $n then dissapates quickly.", ch, NULL, NULL, TO_ROOM);
  broadcast(ch, "Your skin tingles as potent arcane forces are released near by.");
  ch->mana += 12 * level;

//extend spell duration of any beneficial spell
  for (paf = ch->affected; paf != NULL; paf = paf->next)
    {
      if (!IS_GNBIT(paf->type, GN_EXTEND))
	continue;
      else if (paf->duration > 0 && paf->duration < 120)
	paf->duration = UMIN( paf->duration + 23, paf->duration * 2);
    }
}

/* LIFE (powerful turn undead) */
void divine_life(CHAR_DATA* ch, int level, int sn)
{
  CHAR_DATA* vch, *vch_next;
  int diff = 0;
  int dam = 0;

  act("The air around you sizzles as a solid beam of white light errupts at your feet and engulfs you within.", ch, NULL, NULL, TO_CHAR);
  act("Flooding the room with terrible radiance, a solid column of white light engulfs $n!", ch, NULL, NULL, TO_ROOM);
  broadcast(ch, "The area brightens up momentraly from an unkown source.");
  for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
      bool fAuto = FALSE;
      vch_next = vch->next_in_room;
      if (!is_area_safe(ch,vch) && IS_UNDEAD(vch) && ch != vch)
        {
	  if (vch != ch)
	    m_yell(ch,vch,FALSE);
	  diff = vch->level - level;
	  //different effects based on level diff.
	  if (diff > 25)
	    dam = 1;
	  else if (diff > 9)
	    {
	      if (!saves_spell(level,vch,DAM_HOLY,skill_table[sn].spell_type) )
		dam = dice (level, 12) / diff;
	      else
		dam = 1;
	    }
	  else if (IS_NPC(ch) && (vch->pIndexData->vnum == MOB_VNUM_SHADOW
				  || vch->pIndexData->vnum == MOB_VNUM_NIGHTWALKER))
	    fAuto = TRUE;
	  else if (diff >= -4)
	    {
	      if (!saves_spell(level,vch,DAM_HOLY,skill_table[sn].spell_type) )
		dam = dice (2 * level, 5);
	      else
		dam = dice (level, 4);
	    }
	  else if (diff < -4 && IS_NPC(vch))
	    {
	      if ((!saves_spell(level,vch,DAM_HOLY,skill_table[sn].spell_type)
		  || diff <= -5)
		  && !(IS_SET(vch->act, ACT_TOO_BIG) 
		       || IS_SET(vch->act, ACT_PRACTICE)
		       || IS_SET(vch->act, ACT_TRAIN)
		       || IS_SET(vch->act, ACT_NOPURGE)
		       || IS_SET(vch->act, ACT_IS_HEALER)
		       || IS_SET(vch->act, ACT_GAIN)
		       )
		  )
		fAuto = TRUE;
	      else
		dam = dice (2 * level, 6);
	    }
	  else
	    dam = dice (level, 12);
	  if (fAuto)
	    {
	      act("$g's power envelops $N and $E disintegrates into smoldering ashes!",ch,NULL,vch,TO_ALL);
	      act( "$n is DEAD!!", vch, NULL, NULL, TO_ROOM );
	      act_new( "You have been KILLED!!", vch, NULL, NULL, TO_CHAR, POS_DEAD );
	      raw_kill(ch, vch);
	      do_autos(ch);
	    }
	  else{
	    /* make sure this is not akiller vs pc's */
	    if (!IS_NPC(vch) && dam > 450)
	      dam = 450;
	    damage(ch, vch, dam, sn, DAM_HOLY, TRUE);
	  }
	}//end if UNDEAD
    }//END FOR
}//END HELAMAN


/* CHANCE-GREED (gain gold, cps, and blackjack threats in the room) */
void divine_greed( CHAR_DATA* ch,int level, int sn ){
  AFFECT_DATA af;
  CHAR_DATA* vch;
  int gain = 0;
  bool fFound = FALSE;

  /* gain 3-8 points per PC in the room up to 30 points. */
  for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
    if (IS_NPC(vch))
      continue;
    gain += number_range(3, 8);
    if (is_area_safe_quiet(ch, vch) || ch == vch)
      continue;
    /* slit their purses */
    af.where	= TO_NONE;
    af.type   = gen_gold_loss;
    af.level  = ch->level;
    af.duration = 6;
    af.modifier = 0;
    af.location = 0;
    af.bitvector = 0;
    affect_to_char(vch,&af);
  }
  act("An giggle is heard in the room as something moves in the shadows.", ch, NULL, NULL, TO_ALL);

  if (gain){
    CP_GAIN(ch, gain, TRUE );
    ch->in_bank += gain * 500;
    sendf(ch, "`8A Bank squire hands you a stub and gallops away.``\n\r Deposit: %d by: [%s].  Current total: %d\n\r", gain * 500, "Someone", ch->in_bank);
  }
  if (fFound)
    send_to_char("You notice a few purses around you develop sizable cuts.\n\r", ch);
}

/* CHANCE-GUILE (shadowform) */
void divine_guile( CHAR_DATA* ch,int level, int sn ){
  spell_shadowform( gsn_shadowform, level, ch, ch, 0);
}

/* CHANCE-DISCORD (fire shield/res magic) */
void divine_discord(CHAR_DATA* ch, int level, int sn)
{
  AFFECT_DATA af;

  act("You scream in agony as black flames errupt around your flesh.", ch, NULL, NULL, TO_CHAR);
  act("$n screams in pain as black flames errupt from $s flesh.", ch, NULL, NULL, TO_ROOM);
  broadcast(ch, "Your vision blurs and shifts for a moment, then all is back to normal.");

  affect_strip(ch, sn );
  af.type = sn;
  af.duration = number_fuzzy(12);
  af.level = level;

  af.where = TO_RESIST;
  af.bitvector = RES_MAGIC;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);
  af.where = TO_AFFECTS2;
  af.bitvector = AFF_FIRE_SHIELD;
  affect_to_char(ch, &af);
  send_to_char("You are now resistant to magic.\n\r", ch);

  check_dispel(-99, ch, skill_lookup("fire shield"));
  check_dispel(-99, ch, gsn_steel_wall);
}



/* DEATH */
void divine_death(CHAR_DATA* ch, int level, int sn)
{
  CHAR_DATA* vch, *vch_next;
  OBJ_DATA* obj;
  CHAR_DATA* victim;

  bool fFail = FALSE;
  bool fFound = FALSE;

  act("The ground around you cracks and erupts with a black cloud of sinister mist!", ch, NULL, NULL, TO_ALL);
  broadcast(ch, "The slight breeze carries in stench of death and decay.");
  
  for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
      vch_next = vch->next_in_room;
      if (vch != ch && !is_safe_quiet(ch, vch)){
	m_yell(ch, vch, TRUE);
	spell_plague(gsn_plague, level, ch, vch, TARGET_CHAR);
	damage(ch, vch, level, gsn_plague, DAM_DISEASE, TRUE);
      }
    }
  if ( (obj = get_obj_list(ch, "corpse", ch->in_room->contents)) == NULL)
    {
      act("The vile mist slowly disappears leaving behind a carpet of disgusting maggots.", ch, NULL, NULL, TO_ALL);
      return;
    }
  if (get_summoned(ch, MOB_VNUM_ZOMBIE) >= 1 && !IS_IMMORTAL(ch))
    fFail = TRUE;
  if ((obj->item_type != ITEM_CORPSE_PC && obj->item_type != ITEM_CORPSE_NPC)
      || (obj->owner == 0 || obj->pIndexData->vnum == OBJ_VNUM_FAKE_CORPSE))
    fFail = TRUE;

  if (obj->item_type == ITEM_CORPSE_PC)
    {
      if( (obj->level < 15 || obj->level > ch->level + 10)  && !IS_IMMORTAL(ch))
	{
	  act("The vile mist slowly disappears leaving behind a carpet of disgusting maggots.", ch, NULL, NULL, TO_ALL);
	  return;
	}
      else
	for (victim = player_list; victim != NULL; victim = victim->next_player)
	  if (victim->id == obj->owner)
	    {
	      fFound = TRUE;
	      break;
	    }
      if (!fFound)
	{
	  act("The vile mist slowly disappears leaving behind a carpet of disgusting maggots.", ch, NULL, NULL, TO_ALL);
	  return;
	}
    }//end if PC
  else if (obj->item_type == ITEM_CORPSE_NPC && obj->owner == 3)
    fFail = TRUE;
  else if (!fFail)
    victim = create_mobile( get_mob_index( obj->owner ) );
    fFail = TRUE;
  if (fFail)
    {
      act("The vile mist slowly disappears leaving behind a carpet of disgusting maggots.", ch, NULL, NULL, TO_ALL);
      return;
    }
  
  //now we can make the zombie.
  act("The jet black cloud swirls around $p only to disappear suddenly as if blown away.", ch, obj, NULL, TO_ALL);
  vch = create_zombie(ch, victim, obj);
  act("$N comes back from the dead to do $n's bidding.",ch, NULL, victim, TO_ROOM);
  act("$N comes back from the dead to do your bidding.",ch, NULL, victim, TO_CHAR);
  if (IS_NPC(victim))
    extract_char(victim,TRUE);
  obj_from_room ( obj );
  extract_obj ( obj );

}

void spell_divine_int( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  //this is the divine intervension spell. preatty humdrum except different effects for each diety.
  //cast at self only.
  int g_type = 0;
  int dur = 0;
  char* way_str;

  g_type = deity_table[IS_NPC(ch) ? 0 : ch->pcdata->way].path;
  way_str = deity_table[IS_NPC(ch) ? 0 : ch->pcdata->way].way;

  //messages.
  act("You call onto $g to help you in your cause.", ch, NULL, NULL, TO_CHAR);
  if (!is_affected(ch, sn))
    {
      AFFECT_DATA af;
      CHAR_DATA* vch;
      for ( vch = char_list; vch != NULL; vch = vch->next )
	{
	  if ( vch->in_room == NULL )
	    continue;
	  if ( vch->in_room->area == ch->in_room->area )
	    act("The ground trembles and shivers as powers of $t sweeps through the area.", vch, 
		way_str, ch, TO_CHAR);
	}
      switch (g_type)
	{
	default:
	case PATH_FAITH:	
	  /* ONE GOD/FAITH */
	  divine_faith(ch, level, sn);
	  dur = number_range(24, 36);break;
	case PATH_KNOW: 
	  /* KNOWLEDGE */
	  if (str_cmp(way_str, "Combat")){
	    divine_knowledge(ch, level, sn);
	    dur = number_range(24, 36);break;
	  }
	  else{
	    /* COMBAT */
	    divine_combat(ch, level, skill_lookup("haste"));
	    dur = number_range(12, 24);break;
	  }
	case PATH_LIFE: 
	  /* LIFE */
	  divine_life(ch, level, sn);
	  dur = number_range(4, 8);break;
	case PATH_CHANCE: 
	  /* GREED */
	  if (!str_cmp(way_str, "Greed")){
	    divine_greed(ch, level, sn);
	    dur = number_range(8, 16);break;
	  }
	  /* GUILE */
	  else if (!str_cmp(way_str, "Guile")){
	    divine_guile(ch, level, sn);
	    dur = number_range(8, 16);break;
	  }
	  /* DISCORD */
	  else if (!str_cmp(way_str, "Discord")){
	    divine_discord(ch, level, skill_lookup("demonfire"));
	    dur = number_range(12, 24);break;
	  }
	case PATH_DEATH: 
	  /* DEATH */
	  divine_death(ch, level, sn);
	  dur = number_range(36, 72);break;
	}
      af.type = sn;
      af.level = level;
      af.duration = dur;
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier = 0;
      affect_to_char(ch, &af);
    }
  else
    send_to_char("The heavens are silent.\n\r", ch);
}




/* BATTLE MAGE STUFF */


void spell_forcefield( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    
    /* 
       this is BGM version of protective shield, acts as protective sheld AND 
       limits the amount of blunt damage that acn be done BEFORE sanc etc.
    */

//const
    const int min_ceil = 60;//lowest lvl of bash dam  shielding
    const int base_ceil = 260;//base shielding
    const int lvl_mod = 11;//base_ceil - (lvl / lvl_mod) * lvl_mult = ceiling
    const int lvl_mult = 50;


    if ( is_affected( ch, sn ) )
    {
        send_to_char("You are already shielded.\n\r",ch);
	return;
    }

    if ( is_affected( ch, gsn_mana_lock))
    {
	send_to_char("Your manalock seems to interfere.\n\r",ch);
	return;
    }

    if ( is_affected( ch, gen_ref_shield))
    {
	send_to_char("Your reflective shield seems to interfere.\n\r",ch);
	return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level/5);
    af.location  = APPLY_NONE;
    af.modifier  = UMAX(min_ceil,base_ceil - ((level / lvl_mod) * lvl_mult));
    af.bitvector = 0;
    affect_to_char( victim, &af );

    act( "You weave air and energy together into a thick, flexible forcefield.", ch, NULL, NULL, TO_CHAR );
    act( "$n is surrounded by a flexible forcefield.", ch, NULL, NULL, TO_ROOM );

}

void spell_terra_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    AFFECT_DATA af;

    int sector = ch->in_room->sector_type;
    int skill = get_skill(ch, sn);

    if (is_affected(ch, gen_terra_shield)){
      send_to_char("You already hold the ground to your bidding.\n\r", ch);
      return;
    }
//check for sector.
    if (sector == SECT_AIR || sector == SECT_WATER_SWIM
	|| sector == SECT_WATER_NOSWIM)
      {
	send_to_char("There is no ground here for you to animate.\n\r", ch);
	return;
      }

    af.type = gen_terra_shield;
    af.level = level;
    af.duration = UMAX(0, skill - 75)/5 + level / 6;
    af.duration += (level * skill / 100) > 35? 5 : 0;
    
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    
    affect_to_char(ch, &af);
}

void spell_rust( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  AFFECT_DATA af;
  AFFECT_DATA* paf, *paf_next;
  CHAR_DATA* victim = (CHAR_DATA*) vo;


//data
  int change = 0;
  int skill = get_skill(ch, sn);
  int armor = 0;
  int hit = 0;
  int dam = 0;
  int dur = (skill > 85? 1 : 0);

  const int max_dur = 5 + level / 10;
  bool is_new = FALSE;



//now we calculate the reduction.
  change = level / 5 + (level > 35? 1: 0) 
    + (level > 40 ? 2 : 0) 
    + (level > 45 ? 2 : 0) 
    + (level > 49 ? 3 : 0);
  if (saves_spell(level + UMAX(0, (skill - 75) / 8 ), victim, DAM_DROWNING, skill_table[sn].spell_type))
    {
      act("$N possesions seems to resist the spell.", ch, NULL, victim, TO_CHAR);
      act("Your items seems to resist the spell slightly.", ch, NULL, victim, TO_VICT);
      change = 2 * change / 3;
    }
  
  if ( (paf = affect_find(victim->affected, gsn_rust)) == NULL)
    is_new = TRUE;

  //we check if target is affected by rust.  
  if (is_new)
    {
      send_to_char("A layer of rust sprouts on your weapons and armor.\n\r", victim);
      act("$n's possessions redden with rust.", victim, NULL, NULL, TO_ROOM);
      dur += 2;
    }
  else if (paf->duration < max_dur)
    {
      send_to_char("The layer of rust on your items thickens.\n\r", victim);
      act("$n's weapons and armor thicken with rust.", victim, NULL, NULL, TO_ROOM);
    }
  else 
    {
      act("$N's items are as rusty as they are going to get.", ch, NULL, victim, TO_CHAR);
      return;
    }

  dam = dur;
  hit = dur;
  
  armor = change;
  
  //  sendf(ch, "change: %d hit %d dam %d armor %d\n\r", change, hit, dam, armor);     
  if (is_new)
    {
      af.type = sn;
      af.duration = dur;
      af.level = level;
      
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      if (hit > 0)
	{
	  af.duration = dur;
	  af.location = APPLY_HITROLL;
	  af.modifier = - hit;
	  affect_to_char(victim, &af);
	}
      if (dam > 0)
	{
	  af.duration = dur;
	  af.location = APPLY_DAMROLL;
	  af.modifier = - dam;
	  affect_to_char(victim, &af);
	}
      if (armor > 0)
	{
	  af.duration = dur;
	  af.location = APPLY_AC;
	  af.modifier = armor;
	  affect_to_char(victim, &af);
	}
      return;
    }
  //Else we enhance existing efffects.
  for (paf = victim->affected; paf != NULL; paf = paf_next)
    {
      AFFECT_DATA af_new;
      paf_next = paf->next;
      if (paf->type != sn)
	continue;

      af_new.type = paf->type;
      af_new.duration = paf->duration;
      af_new.level = paf->level;
      af_new.where = paf->where;
      af_new.bitvector = paf->bitvector;
      af_new.location = paf->location;
      af_new.modifier = paf->modifier;

      if (paf->location == APPLY_AC){
	if (paf->modifier < 150)
	  af_new.modifier += armor;
      }
      else if (paf->location == APPLY_HITROLL){
	if (paf->modifier > -max_dur)
	  af_new.modifier -= 2 * hit;
      }
      else if (paf->location == APPLY_DAMROLL){
	if (paf->modifier > -max_dur)
	  af_new.modifier -= 2 * dam;
      }
      if (paf->duration < max_dur){
	af_new.duration += 3;
      }
      affect_remove( victim, paf);
      affect_to_char(victim, &af_new);
    }
}



void spell_loc_grav( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim;
  OBJ_DATA* obj = NULL;
  OBJ_DATA* wield;
  OBJ_DATA* sec;
  OBJ_DATA* hold;
  OBJ_DATA* shield;

  char arg1[MIL];
  char arg2[MIL];

//data
  int chance = 15 + UMAX(-5, get_skill(ch, sn) - 80);
  int wear_loc = WEAR_NONE;

//const
  const int weight_mod = 10;//divisor
  const int level_mod = 5;
  const int str_med = 22; //median
  const int str_mod = 10;//multiplier
  const int luck_mod = 2;
//bool
  bool prim = FALSE;

//first we check for prim/sec
  target_name = one_argument(target_name, arg1);
  target_name = one_argument(target_name, arg2);

//check for args.

  if (str_prefix(arg1, "secondary") || arg1[0] == '\0')
   prim = TRUE;

 if ( (victim = ch->fighting) == NULL){
   send_to_char("With no opponent in combat range the spell fails.\n\r", ch);
   return;
 }

//now we run through items looking for something to grab.
/*
  prim:
- wield, hold.
  !prim:
- secondary, hold, shield.
*/

//we grab the pointers first for simplicity.
  wield = get_eq_char(victim, WEAR_WIELD);
  sec = get_eq_char(victim, WEAR_SECONDARY);
  hold = get_eq_char(victim, WEAR_HOLD);
  shield = get_eq_char(victim, WEAR_SHIELD);



  if (prim)
    {
      if (wield != NULL)
	{obj = wield; wear_loc = WEAR_WIELD;}
      else if (shield != NULL && hold != NULL)
	{obj = hold;wear_loc = WEAR_HOLD;}
    }
  else
    {
      if (shield != NULL)
	{obj =  shield;wear_loc = WEAR_SHIELD;}
      else if (sec != NULL)
	{obj = sec;wear_loc = WEAR_SECONDARY;}
      else if (hold != NULL)
	{obj = hold;wear_loc = WEAR_HOLD;}
    }
  
  //final check.
  if (obj == NULL)
    {
      act("$N has nothing in that hand.", ch, NULL, victim, TO_CHAR);
      return;
    }
 
//messages 
  act("You begin to focus forces of gravity on $p.", ch, obj, victim, TO_CHAR);


//now we get a chance.
  chance += obj->weight / weight_mod;
  chance += (str_med - get_curr_stat(ch, STAT_STR)) * str_mod;
  chance += (level - victim->level) * level_mod;
  chance += (get_curr_stat(ch, STAT_LUCK) - get_curr_stat(ch, STAT_LUCK)) * luck_mod;
  chance = URANGE(5, chance, 95);

  if (number_percent() < chance)
    {
      act("$p seems to take on the weight of a mountain!", ch, obj, victim, TO_VICT);
      if (wear_loc == WEAR_WIELD)
	disarm(ch, victim);
      else if (wear_loc == WEAR_SECONDARY)
	disarm_offhand(ch, victim);
      else if (wear_loc == WEAR_SHIELD)
	shield_disarm(ch, victim);
      else if (wear_loc == WEAR_HOLD)
	{
	  if (obj->item_type == ITEM_KEY){
	    send_to_char("That item is too small for you to focus on properly.\n\r", ch);
	    return;
	  }
	  if (IS_OBJ_STAT(obj, ITEM_NOREMOVE))
	    {
	      act("It wont even budge!", ch, NULL, NULL, TO_ALL);
	      return;
	    }
	  act("$p slams to the ground.", victim, obj, victim, TO_ROOM);
	  act("Counter to your efforts $p slams to the ground.", victim, obj, NULL, TO_CHAR);
	  obj_from_char(obj);
	  if (IS_OBJ_STAT(obj, ITEM_NODROP))
	    obj_to_char(obj, victim);
	  else
	    obj_to_room(obj, ch->in_room);
	}//END HOLD
      return;
    }//END SUCCESS
  act("You miscalculate the focus.", ch, NULL, victim, TO_CHAR);
  act("$p seems a bit heavier momentarly.", ch, obj, victim, TO_VICT);
}

void spell_air_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    AFFECT_DATA af;

    int skill = get_skill(ch, sn);
    if (is_affected(ch, sn))
      {
	send_to_char("You are already protected by an air shield.\n\r", ch);
	return;
      }

    send_to_char("You compress the air around you into a thick protective layer.\n\r", ch);
    act("$n's image turns cloudy as air thickens around him.", ch, NULL, NULL, TO_ROOM);

    af.type = sn;
    af.level = level;
    af.duration = UMAX(0, skill - 75) + level / 5;

    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_AC;
    af.modifier = -level;

    affect_to_char(ch, &af);
}


void spell_blades( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    AFFECT_DATA af;

    if (is_affected(ch, gen_blades))
      {
	send_to_char("Given the angular velocity of the blades, the safe limit is 2 blades at a time.\n\r", ch);
	return;
      }
    af.type = gen_blades;
    af.level = level;
    af.duration = level/5 + (level >= 40? 1 : 0)
			      + (level >= 45? 1 : 0) + (level >= 48? 3 : 0);


    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_char(ch, &af);

}


void spell_sharpmetal( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
//We damage the victim, then search for more targets to hit.
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  CHAR_DATA* vch, *vch_next;

static const sh_int dam_each[] =
{0,
 10, 10, 10, 10, 10,    10, 20, 20, 20, 20,
 20, 20, 20, 20, 20,    20, 20, 20, 20, 20,
 30, 40, 45, 46, 47,    50, 52, 55, 57, 60,
 65, 70, 70, 75, 80,    80, 83, 85, 88, 90,
 91, 92, 93, 94, 95,   105,110,115,120,125,
};
 int dam_type = number_range(1, 3); //(PIERCE, SLASH, BASH)

 int dam_lvl = 0;
 int dam = 0;
 int up = 0;//upkeep

 dam_lvl = UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
 dam_lvl	= UMAX(0, dam_lvl);
 dam = number_range(2 * dam_each[dam_lvl] / 3, dam_each[dam_lvl]);


 act("You create a mass of metal shards and sends them hurling!", ch, NULL, victim, TO_CHAR);
 act("$n creates a mass of metal shards and sends them hurling!", ch, NULL, victim, TO_ROOM);
 act("$n directs the stream of deadly metal in your direction!", ch, NULL, victim, TO_VICT);

//no we start damaging.
 if (saves_spell(level, victim, dam_type, skill_table[sn].spell_type))
   dam = 4 * dam / 5;

 damage(ch, victim, dam, sn, dam_type, TRUE);

//now we hit the rest that are in combat.
 for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
     vch_next = vch->next_in_room;

//dont hit the original twice
     if (vch == victim)
       continue;

//only if target is fighting us.
     if (vch->fighting == NULL)
       continue;

//dont hit area safes.
     if (is_area_safe_quiet(ch, vch))
       continue;

//only if target is in combat with us or our mates.
     if (vch->fighting != ch 
	 && !is_same_group(ch, vch->fighting) 
	 )
       continue;

     if (!IS_NPC(ch) && ch->mana < (up = _mana_cost(ch, sn)) )
       {
	 send_to_char("You lack the power to sustain the spray.\n\r", ch);
	 break;
       }
     else if (!IS_NPC(ch))
       ch->mana -= up;

     act("$n directs the stream of deadly metal in your direction!", ch, NULL, victim, TO_VICT);

     dam = number_range(2 * dam_each[dam_lvl] / 3, dam_each[dam_lvl]);
     dam_type = number_range(1, 3); //(PIERCE, SLASH, BASH)
     if (saves_spell(level, vch, dam_type, skill_table[sn].spell_type))
       dam = 4 * dam / 5;
     damage(ch, vch, dam, sn, dam_type, TRUE);
   }//end for
}//END SPELL



void spell_plumbum( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  //    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    int skill = get_skill(ch, sn);

    if (is_affected(ch, gen_plumbum))
      {
	send_to_char("You are already intensifying the gravity in the area.\n\r", ch);
	return;
      }
    af.type = gen_plumbum;
    af.level = level;
    af.duration = UMAX(0, skill - 75)/5 + level / 8;
    af.duration += (level * skill / 100) > 35? 5 : 0;
    
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    
    affect_to_char(ch, &af);


}


void spell_sear( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA* victim = (CHAR_DATA*) vo;

static const sh_int dam_each[] =
{0,
 10, 10, 10, 10, 10,    20, 20, 20, 20, 20,
 30, 30, 30, 30, 30,    31, 32, 33, 34, 35,
 40, 42, 44, 46, 48,    50, 52, 54, 56, 58,
 60, 61, 62, 63, 64,    65, 66, 67, 68, 69,
 70, 71, 72, 73, 74,    75, 85, 95, 110,120,
};

 int time = mud_data.time_info.hour;
 int dam_lvl = 0;
 int dam = 0;
 bool fAdv = get_skill( ch, sn ) > 100;

 dam_lvl = UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
 dam_lvl	= UMAX(0, dam_lvl);
 dam = number_range(dam_each[dam_lvl],  2 * dam_each[dam_lvl]);

 if ( !IS_OUTSIDE(ch) )
   {
     if (fAdv)
       send_to_char("Your mind throbbing with effort, you try to reach the distant sun rays.\n\r", ch );
     else{
       send_to_char("The rays of the sun do not reach here.\n\r", ch);
       return;
     }
   }
 if (time < 7 || time > 18)
   {
     send_to_char("Without the sun, you lack a power source.\n\r", ch);
     return;
   }
 else if (time > 10 && time < 14)
   {
     send_to_char("With the sun near its zenith, you release a deadly beam of light.\n\r", ch);
     act("With sun near its zenith, $n releases a deadly beam of light.", ch, NULL, NULL, TO_ROOM);
     dam = 3 * dam / 2;
     level = level + 3;
   }
 else
   {
     send_to_char("You focus the sunlight into a searing beam.\n\r", ch);
     act("$n focuses the sun's rays into a searing beam.", ch, NULL, NULL, TO_CHAR);
   }
 
 if (saves_spell(level, victim, DAM_LIGHT, skill_table[sn].spell_type))
   dam /= 2;

 spell_blindness(gsn_blindness, level, ch, victim, TARGET_CHAR);
 damage(ch, victim, dam, sn, DAM_LIGHT, TRUE);
}


void spell_dan_blade( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  OBJ_DATA* obj = (OBJ_DATA*) vo;
    AFFECT_DATA af;
    AFFECT_DATA* paf;

    char buf[MIL];
    /* This is the dancing blades spell.
- The gen_dan_balde is set onto the character to run the effects
- teh gsn_Dan_blade is set ontot hte object as a link between the char. and obj.

- modifier stores the common key, to give singularity.
- Only weapons are valid.
- Spears, Maces, Flails and whips are not valid targets.
    */
    
//data
    int key = number_range(-32768, 32768);
    int dur = 0;

    strcpy(buf, ch->name);
    if (is_affected(ch, gen_dan_blade))
      {
	send_to_char("You may only control one blade at a time.\n\r", ch);
	return;
      }
//Carried?
    if (obj->wear_loc != -1)
      {
	send_to_char("The item must be carried to be enchanted.\n\r",ch);
	return;
      }
    
    //check for weapons.
    if (obj->item_type != ITEM_WEAPON)
      {
	send_to_char("That item is not a weapon.\n\r",ch);
	return;
      }
    //Weapon type
    if (obj->value[0] == WEAPON_MACE
	|| obj->value[0] == WEAPON_FLAIL
	|| obj->value[0] == WEAPON_SPEAR
	|| obj->value[0] == WEAPON_WHIP
	)
      {
	send_to_char("You may only place this spell on bladed weapons.\n\r", ch);
	return;
      }
    
    //Misc factors
    if (affect_find(obj->affected, gen_malform))
      {
	act("$p soaks the spell right up.", ch, obj, NULL, TO_CHAR);
	return;
      }
    
    if (is_affected_obj(obj, sn))
      {
	act("$p is already your servant.", ch, obj, NULL, TO_CHAR);
	return;
      }
    
    //all seems ok.
    dur = number_fuzzy(8) + (level > 49? 4 : 0);
    
    //we place the affect on object first.
    af.type = sn;
    af.level = number_fuzzy(level);
    af.duration = dur;
    
    af.where = TO_NONE;
    af.bitvector = 1;//VERY IMPORTANT, WEAPON IS VISIBLE ONLY IF bitvector != 0;
    
    af.location = APPLY_NONE;
    af.modifier = key;//VERY IMPORTANT, the key is the link between gen and gsn.
    if ( (paf = affect_to_obj(obj, &af)) != NULL)
      string_to_affect(paf, ch->name);

    
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.type = gen_dan_blade;
    if ( (paf = affect_to_char(ch, &af)) != NULL)
      string_to_affect(paf, obj->short_descr);

    
}


void spell_manalock( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    AFFECT_DATA af;

    if (is_affected(ch, gsn_forcefield) )
      {
	send_to_char("Your forcefield seems to interfere.\n\r", ch);
	return;
      }

    if (is_affected(ch, gen_ref_shield))
      {
	send_to_char("Your reflective shield seems to interfere.\n\r", ch);
	return;
      }

    if (is_affected(ch, gsn_mana_lock))
      {
	send_to_char("Your enchantements are already locked.\n\r", ch);
	return;
      }

    send_to_char("You place a protective ward on your enchantements.\n\r", ch);
    act("The air around $n humms and crackles briefly.", ch, NULL, NULL, TO_ROOM);
    af.type = sn;
    af.duration = number_fuzzy(24);
    af.level = level;
    
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    
    af.location = APPLY_SAVING_SPELL;
    af.modifier = -level/10;
    affect_to_char(ch, &af);
}


void spell_ref_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
// humdrum spell gen does all the work.
    AFFECT_DATA af, *paf;

    if (is_affected(ch, gsn_forcefield) )
      {
	send_to_char("Your forcefield seems to interfere.\n\r", ch);
	return;
      }

    if (is_affected(ch, gsn_mana_lock))
      {
	send_to_char("Your manalock seems to interfere.\n\r", ch);
	return;
      }

    if ( (paf = affect_find( ch->affected, gen_ref_shield)) != NULL){
      if (get_skill(ch, sn ) < 86){
	send_to_char("You are already surrouned by a reflective field.\n\r", ch);
	return;
      }
      else if (paf->modifier < 75){
	send_to_char("You boost the charge on your reflective shield.\n\r", ch );
	act("The air around $n sizzles with energy.", ch, NULL, NULL, TO_ROOM);
	paf->modifier += 10;
      }
      else{
	send_to_char("The charge on your reflective shield is too high to handle safely.\n\r",ch );
      }
      return;
    }
    
    af.type = gen_ref_shield;
    af.duration = number_fuzzy(2 + level / 5);
    af.level = level;

    af.where = TO_AFFECTS;
    af.bitvector = 0;

    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_char(ch, &af);
}





/* shows the global meteor swarm messages */
void m_swarm_msg(CHAR_DATA* ch, bool d_head)
{
  CHAR_DATA* vch;
  AFFECT_DATA* paf;

  //room messge for d_head
  if (d_head)
    {
      act("$n has summoned Death's Head, may your god have mercy on your soul.", ch, NULL, NULL, TO_ROOM);
      act("You have summoned Death's Head, may $g have mercy on your soul.", ch, NULL, NULL, TO_CHAR);
    }

  for ( vch = char_list; vch != NULL; vch = vch->next )
    {
      if ( vch->in_room == NULL )
	continue;
  
      //deaths head message.      
      if (vch->in_room != ch->in_room && d_head)
	send_to_char("On the horizon you see an ominous red streak descend towards the ground."\
		     "\n\rMoments later the ground trembles with astonishing force.\n\r", vch); 
      
      if ( (paf = affect_find(vch->affected, gsn_burrow)) != NULL)
	{
	  if (paf->modifier == ch->in_room->vnum
	      && !is_safe_quiet(ch, vch))
	    {
	      send_to_char("The ground begins to heave and collapse around you!\n\r", vch);
	      do_unburrow(vch, "");
	    }
	}

      if ( vch->in_room->area == ch->in_room->area 
	   && ch->in_room != vch->in_room
	   && !d_head)
	  send_to_char( "The ground shudders under heavy impacts.\n\r", vch );
    }//end if same area.

}
    


void spell_meteor_swarm( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  /* 
spell runs throuh all valid targets giving them a cell number
for mteors brouth down, each meteor hits a cell, 
we run through all ch's and if the cell matches the metoer we damage.
  */

  const int max_cell = 4;
  const int max_tar = 12;

  int targets = 0;

  CHAR_DATA* vch;
  
  struct _target_ch
  {
    CHAR_DATA* ch;
    sh_int cell;
  }cells[max_tar];


  bool d_head = ( (mud_data.time_info.month == mud_data.time_info.day 
		 && mud_data.time_info.hour%12 == 0)? TRUE : FALSE);
  bool ffield = FALSE;
  bool fAdv = get_skill( ch, sn ) > 100;

static const sh_int dam_each[] =
{0,
  1,  2,  3,  4,  5,    10, 11, 12, 13, 14,//2 rock
 20, 21, 22, 23, 25,    30, 31, 32, 33, 35,//3 rock
 35, 40, 40, 45, 45,    50, 55, 55, 60, 60,//4 rocks
 62, 64, 68, 72, 75,    76, 77, 78, 80, 85,//6 rocks
 90, 92, 96,100,110,   115,120,130,140,150,//8 rocks
};


 int dam = 0; 
 int dam_lvl = 0;
 int i = 0;
 int rocks = 1;
 int rocks_max = 2 + (level > 15 ? 1: 0) 
   + (level > 30 ? 1: 0) 
   + (level > 40 ? 2: 0) 
   + (level > 45 ? 2: 0)
   + (level > 48? number_range(0, (get_curr_stat(ch, STAT_LUCK) - 8) / 2): 0);



 int rocks_min = UMAX(1, rocks_max / 2);
 int dam_mult = 100;
 
 dam_lvl = UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
 dam_lvl = UMAX(0, dam_lvl);
 dam = d_head && level > 45? 600 + number_range(0, 200) : number_range(dam_each[dam_lvl] / 2, dam_each[dam_lvl]);


 if (!IS_OUTSIDE(ch))
   {
     if (fAdv)
       act("Sucking all wind into it, a dark pulsating rift opens overhead!\n\r", ch, NULL, NULL, TO_ALL);
     else{
       send_to_char("Without clear path to the heavens the spell fails.\n\r", ch);
       return;
     }
   }

//we get the number of rocks we are bringing down.
 if (!d_head)
   rocks = number_range(rocks_min, rocks_max);
 //DEBUG
 // sendf(ch, "min: %d, max:%d, rocks:%d", rocks_min, rocks_max, rocks);
 if (ch->mana > 20 && is_affected(ch, gsn_forcefield))
   {
     send_to_char("You pump energy into your forcefield for your protection.\n\r", ch);
     act("The forcfield around $n hums and crackles.", ch, NULL, NULL, TO_ROOM);
     ch->mana -= 20;
     ffield = TRUE;
   }

 //show messages.
 m_swarm_msg(ch, d_head);


 //zero the array just in case.
 for (i = 0; i < max_tar; i ++)
   {cells[i].ch = NULL;cells[i].cell = 0;}

 //now we select targets 
 for (vch = ch->in_room->people;vch != NULL; vch = vch->next_in_room)
   {
     if (targets >= max_tar )
       break;
     
     
     if (ch != vch && is_safe_quiet(ch, vch))
       continue;
     
     if (is_same_group(ch, vch) && ffield && !d_head)
       continue;
     
     cells[targets].ch = vch;
     cells[targets].cell = number_range(1, max_cell);

     targets++;
   }//END target selection

 //Death head attack.
 if (d_head)
   {
     for(i = 0; i <= targets; i++)
       {
	 vch = cells[i].ch;
	 if (vch != NULL)
	   damage(ch, vch, dam, sn, DAM_INTERNAL, TRUE);
       }
     return;
   }
 
 //show damage messgae.
 switch (number_range(0, 3))
   {
   case 0:
   case 1:
     act("Amall skyrocks shower the area.", ch, NULL, ch, TO_ALL);
     dam_mult = 80;
     break;
   case 2:
     dam_mult = 100;
     act("The sky darkens as flaming boulders begin to impact in the area.", ch, NULL, ch, TO_ALL);
     break;
   case 3:
     dam_mult = 130;
     act("You vainly look for cover as small astroids level the area.", ch, NULL, ch, TO_ALL);
     break;
   }//END SWITCH

 //start hitting people now.
 for (i = 0; i < rocks; i++)
   {
     int j = 0;
     //each meteor gets a cell number
     int cell_hit = number_range(1, max_cell);
       
       //we now run through all cahracters checking if the character is in the cell hit.
     for (j = 0; j < max_tar; j++)
       {
	 //check if the cell is hit.
	 if (cells[j].cell != cell_hit)
	   continue;
	 if ( (vch = cells[j].ch) == NULL)
	   continue;
	 //hit the char.
	 
	 if ( vch->in_room != ch->in_room
	      || (is_area_safe_quiet(ch, vch) && vch != ch)
	      || is_ghost(vch, 600)
	      )
	   continue;
	 //Hit now.

	 if (saves_spell(level, vch, DAM_BASH, skill_table[sn].spell_type))
	   dam /= 2;

	 dam = dam_mult * dam / 100;
	 damage(ch, vch, dam, sn, DAM_BASH, TRUE);
	 dam =  number_range(dam_each[dam_lvl] / 2, dam_each[dam_lvl]);
       }//end cell check
   }//end rocks
}//end spell




void spell_comprehend_lan( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;
  if (is_affected(victim, sn)){
    send_to_char("The enchantement is already in effect.\n\r", ch);
    return;
  }

  af.type = sn;
  af.level = level;
  af.duration = 2* level;

  af.location = APPLY_NONE;
  af.modifier = 0;
  af.where = TO_AFFECTS;
  af.bitvector = 0;

  send_to_char("You begin to understand all manner of speech.\n\r", victim);
  if (ch != victim)
    act("$N begins to understand all manner of speech.\n\r", ch, NULL, victim, TO_CHAR);
  affect_to_char(victim, &af);
}


/* Item spell to purge embrace poison */
void spell_purge_unlife( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = ch;
  
  if ( !is_affected( victim, gsn_embrace_poison ) ){
    act("You aren't infected with vampire's bite.",victim,NULL,victim,TO_CHAR);
    return;
  }

  if (check_dispel(level - 3, victim,gsn_embrace_poison))
    act("The bite wound sizzles, smokes and then quickly heals and disappears.", victim, NULL, victim, TO_ALL);
  
  else{
    act("The bite wound smokes momentarly then explodes in a fountain of puss.", ch, NULL, victim, TO_ROOM);
    damage(victim, victim, UMAX(1, level / 2), gsn_embrace_poison, DAM_POISON, TRUE);
  }
  WAIT_STATE2(ch, 4 * PULSE_VIOLENCE);
}



void spell_sacred_runes( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (victim != ch){
      act("You may only submit your own flesh to $g.", ch, NULL, NULL, TO_CHAR);
      return;
    }

    if (victim->class == class_lookup("psionicist"))
    {
        send_to_char("Strangly nothing happens...\n\r", ch);
	return;
    }

    if (is_affected(ch, gsn_sacred_runes)){
      send_to_char("You are already protected by the sacred runes.\n\r", ch);
      return;
    }

    if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
    {
      send_to_char("The sanctuary seems to negate the effect.\n\r", ch);
      return;
    }

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = UMAX(1, level - 8);
    af.duration  = number_fuzzy( level / 10);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    damage(ch, ch, number_range(level / 2, level), gsn_sacred_runes, DAM_INTERNAL, TRUE);
    act("Bloody runes of $g's power begin to cover your flesh.", ch, NULL, ch, TO_CHAR);
    act("Bloody runes of $g's power begin to cover $n's flesh.", ch, NULL, ch, TO_ROOM);

}

void spell_visitation( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;
  AFFECT_DATA* paf;

  if ( is_affected( victim, gen_visit ) )
    {
      act("$N's soul is already marked.",ch,NULL,victim,TO_CHAR);
      return;
    }
  if (saves_spell(level,victim,DAM_NEGATIVE,skill_table[sn].spell_type))
    {
      act_new("You feel uneasy but it passes.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
      if (ch != victim)
	act("$N seems to be unaffected.",ch,NULL,victim,TO_CHAR);
      return;
    }
    af.where     = TO_AFFECTS;
    af.type 	 = gen_visit;
    af.level	 = level;
    af.duration = number_range(2, 5); 
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    paf = affect_to_char(victim,&af);
    string_to_affect(paf, ch->name);
    act("You mark $N's soul for your demonic servants.", ch, NULL, victim, TO_CHAR);
}

// Endorphin rush, ripped down cure serious, but physical (for warmasters) 
// Coded by: Athaekeetha

void spell_endorphin_rush( int sn, int level, CHAR_DATA *ch, 
			   void *vo,int target ) {
  
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int heal = dice(3, 4);

  act_new("You feel a rush of endorphins, numbing your pain.", ch,NULL,
	  victim,TO_CHARVICT,POS_DEAD );
  if ( ch != victim ) {
    act("You cause $N's endorphins to surge.", ch,NULL,victim,TO_CHAR );
  }

  victim->hit = UMIN( victim->hit + (heal/2), victim->max_hit );
  victim->mana = UMIN( victim->mana + (heal/2), victim->max_mana );
  update_pos( victim );
}

//Lifeforce spell for Druids. 12-02-00 Shadow

void spell_lifeforce( int sn, int level, CHAR_DATA *ch, void *vo,int
target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (((IS_GOOD(ch) && IS_EVIL(victim)) || (IS_EVIL(ch) && IS_GOOD(victim))) && !IS_IMMORTAL(ch))
    {
        sendf(ch, "It would go against your beliefs to aid %s.\n\r", PERS(victim,ch));
        return;
    }
    if (victim->class == class_lookup("psionicist"))
    {
        send_to_char("Strangely nothing happens...\n\r", ch);
        return;
    }
    if (ch->in_room->sector_type != SECT_FOREST)
    {
        send_to_char("A lifeforce can only be instilled within a forest!\n\r",ch);
        return;
    }           
    if (is_affected(ch, gsn_sacred_runes)){
      send_to_char("The vile runes seem to negate the effect.\n\r", ch);
      return;
    }
    if ( IS_AFFECTED(victim, AFF_SANCTUARY) )
    {
        if (victim == ch)
            act("Not with sanctuary.",ch,NULL,victim,TO_CHAR);
        else
            act("$N is already protected by a lifeforce.",ch,NULL,victim,TO_CHAR);
        return;
    }
    if (is_affected(victim, sn))
    {
        if (victim == ch)
            act("You are already harnessing the lifeforce.",ch,NULL,victim,TO_CHAR);
        else
            act("$N is already harnessing the lifeforce.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level / 6);
    af.location  = APPLY_NONE; 
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act_new( "You are surrounded by a green aura.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n is surrounded by a green aura.", victim, NULL, NULL, TO_ROOM );
}

void spell_thorn_shield(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (is_affected(victim,sn))
    {
        send_to_char("You are already protected by a shield of thorns.\n\r",ch);
        return;
    }
    if ( is_affected(victim,skill_lookup("protective shield")))
    {
        send_to_char("Not with protective shield.\n\r",ch);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy( level/6);
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "You are protected by a shield of thorns.\n\r",ch);
    act( "$n's skin becomes covered in thorns.", ch, NULL, NULL, TO_ROOM);
}
 
//=====Circle of Protection=====//
//12-02-00 -Shadow
void spell_circle_of_protection( int sn, int level, CHAR_DATA *ch, void *vo,int target){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  char arg[MIL];
  if (ch->in_room->sector_type != SECT_FOREST)
  {
      send_to_char("Circles of protection must be formed in a forest.\n\r",ch);
      return;
  }
  if (is_affected(ch, sn))
  {
      send_to_char("You are already protected.\n\r", ch);
      return;
  }
  target_name = one_argument(target_name, arg);

  if (arg[0] == '\0'){
    send_to_char("Circle of Protection  :\n\r"\
                 "Syntax: cast 'circle of protection' <red/blue/green/white/black>\n\r", ch);
    return;
  }

  if (victim != NULL) {
    send_to_char ("Circles of protection can only be created around you.\n\r", ch);
    return;
  }
  affect_strip(ch, gsn_circle_of_protection);
  af.type = sn;
  af.level = level;
  af.duration = 24;
  af.where = TO_AFFECTS;
  af.modifier = 0;
  af.location = APPLY_NONE;


  /* set type of prtection */
  if (!str_prefix(arg, "red"))
  {
    af.where = TO_AFFECTS2;
    af.bitvector = AFF_FIRE_SHIELD;
    affect_to_char( ch, &af );
    act_new( "You surround yourself in a red circle of protection.",ch,NULL,ch,TO_CHARVICT,POS_DEAD );
    act("$N surrounds $mself in a red circle of protection.",ch,NULL,ch,TO_ROOM);
    return;
  }
  if (!str_prefix(arg, "blue"))
  {
    af.where = TO_AFFECTS2;
    af.bitvector = AFF_ICE_SHIELD;  
    affect_to_char( ch, &af );
    act_new( "You surround yourself in a blue circle of protection.",ch,NULL,ch,TO_CHARVICT,POS_DEAD );
    act("$N surrounds $mself in a blue circle of protection.",ch,NULL,ch,TO_ROOM);
    return;
  }
  if (!str_prefix(arg, "green"))
  {
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    act_new( "You surround yourself in a green circle of protection.",ch,NULL,ch,TO_CHARVICT,POS_DEAD );
    act("$N surrounds $mself in a green circle of protection.",ch,NULL,ch,TO_ROOM);
    return;
  }
  if (!str_prefix(arg, "white"))
  {
    af.where = TO_AFFECTS;
    af.bitvector = AFF_PROTECT_GOOD;
    affect_to_char( ch, &af );
    act_new( "You surround yourself in a white circle of protection.",ch,NULL,ch,TO_CHARVICT,POS_DEAD );
    act("$N surrounds $mself in a white circle of protection.",ch,NULL,ch,TO_ROOM);
        return;
  }
  else
  {
    af.where = TO_AFFECTS;
    af.bitvector = AFF_PROTECT_EVIL;
    affect_to_char( ch, &af );
    act_new( "You surround yourself in a black circle of protection.",ch,NULL,ch,TO_CHARVICT,POS_DEAD );
    act("$N surrounds $mself in a black circle of protection.",ch,NULL,ch,TO_ROOM);
        return;
  }
}                                                

void spell_forest_mist( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    
    if (ch->in_room->sector_type == SECT_AIR 
	|| ch->in_room->sector_type == SECT_DESERT 
	|| ch->in_room->sector_type == SECT_CITY
	|| ch->in_room->sector_type == SECT_INSIDE){
      send_to_char("You cannot form enough mist in city, indoors, deserts or air.\n\r",ch);
      return;
    }
    if ( is_affected( victim, sn ) )
    {
        if (victim == ch)
            act("You can see nothing through the thick mist!",ch,NULL,victim,TO_CHAR);
        else
            act("$N is already shrouded in the forest mists!", ch,NULL,victim,TO_CHAR);
        return;
    }
    if ( saves_spell( level, victim, DAM_MALED,skill_table[sn].spell_type))
    {
        sendf(ch, "You failed to call forth the forest mists around %s.\n\r", PERS(victim,ch));
        return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = (level / 12) + 1;
    af.location  = 0;
    af.modifier  = 0;
    /* There isas slight trick here to speed up the checks in act_move and such */
    /* Instead of whole new bit flag, the code first checks for terrain, then if present */
    /* checks if terrain due to forest mist */
    af.bitvector = AFF_TERRAIN;
    affect_to_char( victim, &af );

    act("Heavy forest mists rise and form all around $n.", victim, NULL, NULL, TO_ROOM);
    act("A heavy forest mist rises, obscuring any vision.",ch, NULL, victim, TO_VICT);
}

void spell_restoration( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    bool found = FALSE;
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    if (IS_EVIL(victim))
    {
        sendf(ch,"The Earth Mother does not like %s.\n\r",PERS(victim,ch));
        return;
    }
    if (ch->in_room->sector_type != SECT_FOREST)
    {
        send_to_char("Restorative healing can only occur in a forest.\n\r",ch);
        return;
    }
    if (is_affected(ch, sn)){
      send_to_char("You are not ready yet to perform a restoration.\n\r", ch);
      return;
    }
    if (victim != ch)
    {
        act("$n touches you with a green aura, instilling earth energy into you.",ch,NULL,victim,TO_VICT);
        act("You touch $N with a green aura, restoring $S health.",ch,NULL,victim,TO_CHAR);
    }
    if (check_dispel(-99,victim,gsn_poison))
        found = TRUE;
    if (check_dispel(-99,victim,gsn_plague))
        found = TRUE;
    if (check_dispel(-99,victim,skill_lookup("weaken")))
        found = TRUE;
    if (found || victim->hit < victim->max_hit){
      AFFECT_DATA af;

      act("$n is restored to health.",victim,NULL,NULL,TO_ROOM);
      send_to_char("A warm green aura surrounds your body, then fades.\n\r",victim);
      
      if (!IS_NPC(ch) && !IS_NPC(victim) && ch->class == class_lookup("druid") && ch != victim)
	gain_exp(ch,number_range(1,ch->level/8));
      victim->hit = UMIN( victim->hit + 100, victim->max_hit );
      update_pos( victim );

      af.type = sn;
      af.duration = 24;
      af.level = level;
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.modifier = 0;
      af.location = APPLY_NONE;
      affect_to_char( ch, &af);
    }
    else
    {
      act("$N does not seem to be affected by the green aura.",ch,NULL,victim,TO_CHAR);
      send_to_char("You don't seem to feel any different.\n\r",victim);
    }
}

void spell_treeform( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You are already a tree.\n\r",ch);
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
    af.duration  = level / 6;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_TREEFORM;
    affect_to_char( victim, &af );

    af.where	 = TO_AFFECTS2;
    af.bitvector = AFF_BARRIER;

    af.modifier  = -20 * (level / 12);
    af.location  = APPLY_AC;
    affect_to_char(victim,&af);

    act( "Your body twists and forms into the shape of a tree.", ch, NULL, NULL, TO_CHAR);
    act( "A large oak tree forms, shuddering and twisting.", ch, NULL, NULL, TO_ROOM);
}

void spell_druid_gate( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  ROOM_INDEX_DATA *pRoomIndex;
  int e_r1[] = {ROOM_NO_TELEPORTIN, ROOM_NO_GATEIN};
  int i_se[] = {SECT_FOREST};
  int area_pool = 10;

  if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL){
    sendf(ch, "You cannot gate while on %s!\n\r",ch->pcdata->pStallion->short_descr);
    return;
  }
  if ( ch->in_room == NULL
       || ch->fighting
       || IS_SET(ch->in_room->room_flags, ROOM_NO_GATEOUT)
       || IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORTOUT)
       || IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)
       || ( ch != ch && IS_SET(ch->imm_flags,IMM_SUMMON))
       || is_affected(ch, gsn_tele_lock)
       || ( !IS_NPC(ch) && ch->fighting != NULL ))
    {
      send_to_char("You failed to invoke a druid gate.\n\r",ch);
      return;
    }
  else if (is_affected(ch,gen_ensnare)){
    send_to_char("You sense a powerful magical field preventing your departure.\n\r", ch);
    return;
  }
  pRoomIndex =  get_rand_room(0,0,		//area range (0 to ignore)
			      0,0,		//room ramge (0 to ignore)
			      NULL,0,		//areas to choose from
			      NULL,0,		//areas to exclude
			      i_se,1,		//sectors required
			      NULL,0,		//sectors to exlude
			      NULL,0,		//room1 flags required
			      e_r1,2,		//room1 flags to exclude
			      NULL,0,		//room2 flags required
			      NULL,0,		//room2 flags to exclude
			      area_pool,	//number of seed areas
			      TRUE, 		//exit required?
			      FALSE,		//Safe?
			      ch);		//Character for room checks
  
  if (!pRoomIndex
      || IS_SET(pRoomIndex->area->area_flags, AREA_NOMORTAL) ){
    send_to_char("You failed to invoke a druid gate.\n\r", ch);
    return;
  }
  send_to_char("You invoke a druid gate, and vanish!\n\r",ch);
  act( "$n invokes a druid gate, and vanishes!", ch, NULL, NULL, TO_ROOM );
  char_from_room( ch );
  char_to_room( ch, pRoomIndex );
  act( "$n arrives from a druid gate.", ch, NULL, NULL, TO_ROOM );
  do_look( ch, "auto" );
}




void spell_mirror_image( int sn, int level, CHAR_DATA *ch, void *vo, int target ) {

  CHAR_DATA *victim = (CHAR_DATA*) vo;
  CHAR_DATA* gch;
  AFFECT_DATA af;
  int  tot_mi = 0;
  char long_buf[MIL], short_buf[MIL];

  if (IS_NPC(victim)){
    send_to_char("Why would you do that?.\n\r",ch);
    return;
  }
  if (is_affected(ch, sn) || is_affected(victim,gsn_doppelganger)){
    send_to_char("You do not have enough power to create the mirror.\n\r",ch);
    return;
  }
  
/* count current mirrors */
  for (gch = char_list; gch != NULL; gch = gch->next){
    if (IS_NPC(gch) && is_affected(gch,gsn_mirror_image)) {
      tot_mi++;
    }
  }
/* check for max */
  if (tot_mi >= level/10){
    send_to_char("You cannot have any more mirrors.\n\r",ch);
  }
  
  sprintf(long_buf, "A mirror image stands here.\n\r");
  sprintf(short_buf, "a mirror image");
  
/* create the image */
  gch = create_mobile( get_mob_index(MOB_VNUM_DUMMY) );

/* copy strings */
  free_string(gch->name);
  free_string(gch->short_descr);
  free_string(gch->long_descr);
  gch->name = str_dup(victim->name);
  gch->short_descr = str_dup(short_buf);
  gch->long_descr = str_dup(long_buf);
/* set proper sex */
  gch->sex = victim->sex;
/* move to room */
  char_to_room(gch,victim->in_room);
/* set stats */
  gch->max_hit = gch->hit = 1;
  gch->level = 1;
  gch->master = victim;

  if (ch == victim){
    act("A mirror image of yourself appears beside you!",ch,NULL,NULL,TO_CHAR);
    act("A mirror image of $n appears beside $M!",ch,NULL,victim,TO_ROOM);
  }
  else{
    act("A mirror of $N appears beside $M!",ch,NULL,victim,TO_CHAR);
    act("A mirror of $N appears beside $M!",ch,NULL,victim,TO_NOTVICT);
    act("A mirror image of yourself appears beside you!",ch,NULL,victim,TO_VICT);
  }

/* set timer on the image and the user */
  af.type = gsn_mirror_image;
  af.level     = level;
  af.duration = -1;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  affect_to_char(gch,&af);
/* set wait time on user */
  af.type = sn;
  af.duration = 5;
  affect_to_char(ch, &af);

/* set timer so mirror expires */
  af.type = gsn_timer;
  af.duration = 24;
  affect_to_char(gch,&af);
}    

/* Songbird (group haste)
   Written By: Ath
*/

void spell_songbird( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    AFFECT_DATA af;
    CHAR_DATA *gch;

/* send the song to the caster */
    act("A songbird sings '`6A graceful sight not seen by all,``'",ch,NULL,NULL,TO_CHAR); 
    act("A songbird sings '`6Who's feathers shine and liven us all,``'",ch,NULL,NULL,TO_CHAR);
    act("A songbird sings '`6With great swoops of tremendous haste,``'",ch,NULL,NULL,TO_CHAR);
    act("A songbird sings '`6Grant us your blessing with grace.``'",ch,NULL,NULL,TO_CHAR);

/* send the song to everyone else in the room */
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
      act("A songbird sings '`6A graceful sight not seen by all,``'",ch,NULL,gch,TO_VICT); 
      act("A songbird sings '`6Who's feathers shine and liven us all,``'",ch,NULL,gch,TO_VICT);
      act("A songbird sings '`6With great swoops of tremendous haste,``'",ch,NULL,gch,TO_VICT);
      act("A songbird sings '`6Grant us your blessing with grace.``'",ch,NULL,gch,TO_VICT);
    }

/* loop over everyone else in the room */
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {

/* if the person is not in casters group, or is already hasted, ignore them */
      if (!is_same_group (gch, ch) || IS_AFFECTED (gch, AFF_HASTE)) {
	continue;
      }

/* if the person is not of caster's align, and caster is not imm, ignore them */
      if (((IS_GOOD(ch) && IS_EVIL(gch)) || (IS_EVIL(ch) && IS_GOOD(gch))) && !IS_IMMORTAL(ch)) {
	continue;
      }
      
/* if the person is FAST ignore them */
      if (IS_SET(gch->off_flags,OFF_FAST)) {
	continue;
      }

/* otherwise give them the (slightly weakened) haste affect */
      af.where     = TO_AFFECTS;
      af.type      = skill_lookup ("haste");
      af.level     = level;
      if (gch == ch)
        af.duration  = level/4;
      else
        af.duration  = level/6;
      af.location  = APPLY_DEX;
      af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
      af.bitvector = AFF_HASTE;
      affect_to_char( gch, &af );
      act_new( "You feel yourself moving more quickly.", gch,NULL,NULL,TO_CHAR,POS_DEAD);
    }
}

   

/* Written by: Virigoth							*
 * Returns: void							*
 * Used: magic.c, magic4.c						*
 * Comment: A dispell magic like spell, except on failed save prevents  *
 * any defensive spells (TAR_SELF, TAR_CHAR_DEFENSIVE) from casting.	*/

void spell_reveal_hidden( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  /* basicly this is an inverse "where" list showing only those that are 
     not seen by character (camo/hidden/invis only)
  */
  CHAR_DATA* victim;
  bool found = FALSE;
  AFFECT_DATA af;

  if (is_affected(ch, sn)){
    send_to_char("You fail sensing its still too early for new attempt.\n\r", ch);
    return;
  }
  send_to_char("You scan your immediate area for anything unseen.\n\r", ch);

  for ( victim = player_list; victim != NULL; victim = victim->next_player )
    if ( (!IS_NPC(victim) 
	  || is_affected(victim,gsn_doppelganger)) 
	 && !IS_IMMORTAL(victim) 
	 && victim->in_room != NULL
	 && !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
	 && !room_is_private(victim->in_room)
	 && victim->in_room->area == ch->in_room->area
	 && (!IS_AFFECTED2(victim,AFF_SHADOWFORM) 
	     || IS_IMMORTAL(ch))
	 && !IS_AFFECTED2(victim,AFF_TREEFORM)
	 && !is_affected(victim,gsn_bat_form) 
	 && !is_affected(victim,gsn_coffin) 
	 && !is_affected(victim,gsn_entomb)
	 && !IS_AFFECTED2(victim,AFF_CATALEPSY)
	 && !can_see( ch, victim )  
	 && (IS_AFFECTED(victim, AFF_HIDE) || IS_AFFECTED(victim, AFF_INVISIBLE)
	     || IS_AFFECTED2(victim, AFF_CAMOUFLAGE)) )
      {
	found = TRUE;
	if (victim->invis_level < 1 || victim->incog_level < 1)
	  sendf( ch, "%s%-28s %s\n\r", is_pk(ch,victim) ? "<`1PK``> " : "     ", PERS2(victim), victim->in_room->name);
      }
  if ( !found )
    send_to_char( "But find nothing..\n\r", ch );
  if (IS_IMMORTAL(ch))
    return;
  af.type = sn;
  af.duration = number_fuzzy(3);
  af.level = level;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);
}


/* Written by: Virigoth							*
 * Returns: void							*
 * Used: magic4.c							*
 * Comment: Sets the resurrection gsn onto the owner of the corpse, the *
 * target char still needs to "accept" the resurection.			*/

void spell_resurrection( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    AFFECT_DATA af, *paf;
    CHAR_DATA  *victim;

    if (obj->item_type != ITEM_CORPSE_PC && obj->item_type != ITEM_CORPSE_NPC)
    {
        send_to_char("That's not a corpse.\n\r",ch);
	return;
    }
    if (obj->item_type != ITEM_CORPSE_PC){
      send_to_char("That corpse has no trace of a soul in it.\n\r", ch);
      return;
    }
    if (obj->owner == 0 || obj->pIndexData->vnum == OBJ_VNUM_FAKE_CORPSE){
      send_to_char("That corpse has no trace of a soul in it.\n\r", ch);
      return;
    }
    if (obj->condition < 2){
      send_to_char("The remains are far too damaged to be resurrected.\n\r",ch);
      return;
    }
    /* check for paf from death */
    if ( (paf = affect_find(obj->affected, gsn_resurrection)) == NULL
	 || !paf->has_string){
      send_to_char("You are too late, for the soul has departed already.\n\r", ch);
      return;
    }
    /* grab the owner of the corpse */
    if ( (victim = get_char(paf->string)) == NULL){
      sendf(ch, "%s's soul has departed the lands already.\n\r", paf->string);
      return;
    }
    if ( (IS_GOOD(victim) && IS_EVIL(ch))
	|| (IS_GOOD(ch) && IS_EVIL(victim)) ){
      act("$N is too different from you.\n\r", ch, NULL, victim, TO_CHAR);
      return;
    }
    if (ch == victim){
      act("Not even $g's power can allow you to resurrect yourself.", ch, NULL, NULL, TO_CHAR);
      return;
    }
    /* check for ghost */
    if (!is_ghost(victim, 600)){
      act("$N is no longer a ghost and not subject to resurrection.", ch, NULL, victim, TO_CHAR);
      act("You feel a gentle tug on your soul from $N's resurrection.", victim, NULL,ch, TO_CHAR);
      return;
    }

    /* check if not already resurrecting himself */
    if (is_affected(victim, sn)){
      send_to_char("That person has already been given the choice of new life.\n\r", ch);
      return;
    }
    /* messages */
    act("You call onto $g to grant $N new life.", ch, NULL, victim, TO_CHAR);
    act("$n calls onto $g to grant $N a new life.", ch, NULL, victim, TO_ROOM);

    /* there is a cost */
    af.type = gsn_drained;
    af.duration = 6;
    af.level = level;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_STR;
    af.modifier = -3;
    affect_join(ch, &af);
    if (number_percent() > 2 * get_skill(ch, sn) / 3){
      act("$n's prayers seem to fail.", ch, NULL, NULL, TO_ROOM);
      act("Your prayers fail to gain you attention.", ch, NULL, NULL, TO_CHAR);
      return;
    }
    /* all seems ok, set an affect to let the player accept */
    af.type = sn;
    af.duration = 6;
    af.level = level;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    paf = affect_to_char(victim, &af);
    string_to_affect(paf, ch->name);
    act("$n has called onto $g  to grant you a chance for a new life.\n\r"\
	"Use \"accept\" to accept the resurrection.", ch, NULL, victim, TO_VICT);
}

void resurrection_accept(CHAR_DATA* ch){
  CHAR_DATA* och;

  OBJ_DATA* corpse = NULL;
  OBJ_DATA* obj, *next_obj;
  AFFECT_DATA* paf = NULL, af, *taf;
  char buf[MIL];
  int exp;

  /* check if affected by resurrection */
  if ( (paf = affect_find(ch->affected, gsn_resurrection)) == NULL){
    send_to_char("You have not been given a chance to be brought back.\n\r", ch);
    return;
  }
  if (paf->has_string)
    och = get_char(paf->string);
  else
    och = NULL;

  if (!is_ghost(ch, 600)){
    send_to_char("You are no longer a ghost\n\r", ch);
    affect_strip(ch, gsn_resurrection);
    return;
  }
  /* locate the corpse */
  for ( obj = object_list; obj != NULL; obj = next_obj ){
    next_obj = obj->next;
    if (obj->item_type != ITEM_CORPSE_PC)
      continue;
    if (!obj->in_room)
      continue;
    if ( (taf = affect_find(obj->affected, gsn_resurrection)) == NULL
	 || !taf->has_string)
      continue;
    if (str_cmp(taf->string, ch->name))
      continue;

    //get the first corpse, preferably at caster's feet
    if (corpse == NULL && (och == NULL || obj->in_room == och->in_room)){
      corpse = obj;
      paf = taf;
    }
    else{
      affect_strip_obj( obj, gsn_resurrection );
    }
  }
  if (!corpse || !paf){
    send_to_char("Your remains are no longer avaliable for resurrection.\n\r", ch);
    affect_strip(ch, gsn_resurrection);
    return;
  }
  /* move char to corpse */
  if (IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)){
    send_to_char("The nature of your immediate area prevents the your resurrection.\n\r", ch);
    return;
  }
  /* move objects over */
  for (obj = corpse->contains; obj != NULL; obj = next_obj){
    next_obj = obj->next_content;
    obj_from_obj(obj);
    obj_to_ch(obj, ch);
  }
  sprintf(buf, "Accepted resurrection into room %d.", corpse->in_room->vnum);
  log_event(ch, buf);

  /* grant the death back only if exp was lost (NPC KILL) */
  if (paf->modifier > 0)
    ch->pcdata->dall = UMAX(0, ch->pcdata->dall - 1);

  /* check con loss */
  if (paf->level
      && ch->perm_stat[STAT_CON] < get_max_train(ch, STAT_CON)){
    send_to_char("You feel healthier.\n\r", ch);
    ch->perm_stat[STAT_CON]++;
  }

  /* give back exp */
  exp = UMIN(paf->modifier * paf->duration / paf->bitvector,
	     paf->modifier);
  gain_exp( ch, exp );

  /* move the char */
  char_from_room(ch);
  char_to_room(ch, corpse->in_room);

  /* Few messages now */
  act("Suddenly color and life returns to $p and $e arises from death!", ch, corpse, NULL, TO_ROOM);
  act("Your vision blurs and you feel yourself being pulled rapidly.\n\r"\
      "With great effort your body heaves as your heart begins to beat once again!\n\r", ch, NULL, NULL, TO_CHAR);

  /* heal char. */
  ch->hit = UMAX(ch->hit, 3 * ch->max_hit / 4);
  ch->move = UMAX(ch->move, 3 * ch->max_move / 4);

  /* set calm */
  af.where = TO_AFFECTS;
  af.type = skill_lookup("calm");
  af.level = ch->level;
  af.duration = 24;
  af.location = APPLY_HITROLL;
  af.modifier = -10;
  af.bitvector = AFF_CALM;
  if (!is_affected(ch, af.type)){
    affect_to_char(ch,&af);
    af.location = APPLY_DAMROLL;
    affect_to_char(ch,&af);
  }

  /* set ghost to run out  */
  ch->pcdata->ghost = (time_t)(mud_data.current_time - 520);
  send_to_char("You begin to quickly lose your ghost form.\n\r", ch);

  /* destroy the corpse */
  extract_obj(corpse);
  affect_strip(ch, gsn_resurrection);
  do_look(ch, "auto");

/* we reward original caster stored in the paf */
  if (och == NULL)
    return;
  else{
    AFFECT_DATA af, *paf;
    int gain = number_range(1000, 5000);

    act("You feel $g's blessing fill your soul as $N is resurrected!",
	och, NULL, ch, TO_CHAR);

    if (!is_affected(och, gsn_timer)){
      AFFECT_DATA af;

      af.type = gsn_timer;
      af.level = 60;
      af.duration = 240;
      af.where = TO_NONE;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier = 0;
/* Viri: Hopefully we will not need this but for now its in */
      affect_to_char(och, &af);

      if (och->level < 50){
	sendf(och, "You gain %d experience!\n\r", gain);
	gain_exp(och, gain);
      }
      gain = number_range(20, 75);
      CP_GAIN(och, gain, TRUE);
    }

    if ( (paf = affect_find(och->affected, gsn_mystic_healing)) != NULL
	 && paf->modifier >= 50)
      return;
    if (!paf){
      af.type = gsn_mystic_healing;
      af.level = 50;
      af.duration = -1;
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_MOVE_GAIN;
      af.modifier = 10;
      affect_to_char(och, &af);
      af.location = APPLY_HIT_GAIN;
      affect_to_char(och, &af);
      af.location = APPLY_MANA_GAIN;
      affect_to_char(och, &af);
    }
    else{
      for (paf = och->affected; paf; paf = paf->next){
	if (paf->type == gsn_mystic_healing){
	  paf->modifier += 10;
	}
      }
    }
  }
}

/* accepts various things like resurrection */
void do_accept(CHAR_DATA *ch, char *argument){

  if (is_affected(ch, gsn_resurrection)){
    if (ch->pCabal && IS_CABAL(ch->pCabal, CABAL_NOMAGIC)){
      send_to_char("You are not allowed to use magic!\n\r", ch);
      return;
    }
    resurrection_accept(ch);
    return;
  }
}

// life-insurance: For healer mobs, secores all items on death
// Coded by: Athaekeetha
// 
// This is just a stub function that is never called.

void spell_life_insurance( int sn, int level, CHAR_DATA *ch, 
			   void *vo,int target ) {
}

/* Written by: Virigoth							*
 * Returns: void							*
 * Used: magic4.c							*
 * Comment: A spell that creates a virtual attacked based on casters mana.
 */

void spell_light_sword( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
//data
  int thac0 = 0;
  int ac = 0;
  int dam = 0;
  int dam_type = DAM_ENERGY;
  int dt = attack_lookup("slice") + TYPE_HIT;

  int d1 = 0;
  int d2 = 0;

  int h_roll = 0 ;
  int d_roll = 0 ;
  int diceroll = 0; 
  if (ch->mana < 300){
    act("The light sword sputters and goes out.", ch, NULL, NULL, TO_CHAR);
    act("The light sword sputters and goes out.", ch, NULL, victim, TO_VICT);
    return;
  }
  // the hit/dam 
  h_roll = URANGE(0, (ch->mana - 300) / 40, 25);
  d_roll = URANGE(0, (ch->mana - 300) / 20, 50);
  d1 = URANGE(0, (ch->mana - 200) / 100, 8);
  d2 = URANGE(0, (ch->mana - 200) / 100, 7);

//add dice roll for damage
  dam = d_roll + dice(d1, d2);
  dam = UMIN(125, dam);

  //get thaco and AC
  thac0 =  get_THAC0(ch, victim, NULL, dt, FALSE, IS_NPC(ch), ACT_WARRIOR, TRUE,
		     gsn_ancient_lore, level);
  ac = get_AC(ch, victim, dt, dam_type, TRUE);

  //We roll for miss
  while ( ( diceroll = number_bits( 5 ) ) >= 20 );
  if ( diceroll == 0 || ( diceroll != 19 && diceroll < thac0 - ac ) )
    dam = 0;

  //and we make an attack.
  //  sendf(ch, "d_roll: %d, dam: %d, h_roll: %d\n\r", d_roll, dam, h_roll);
  if (ch->fighting != NULL 
      && victim->hit > 0 
      && victim->position > POS_STUNNED 
      && victim->in_room == ch->in_room
      && !is_safe_quiet(ch, victim))
    virtual_damage(ch, victim, NULL, dam, dt, dam_type, h_roll, level, TRUE, TRUE, gsn_ancient_lore);
  check_improve(ch, gsn_ancient_lore, TRUE, 1);
}

/* used in spell_psi_blast to influence the chance to do some type of damage */
void spell_psi_control( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  int i = 0;
  bool fFound = FALSE;
  //  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;
  static char * const type  [] = { "water",  "cold",  "fire", "shock", "light", "energy" };

  if (is_affected(ch, sn)){
    send_to_char("You are already exerting control  over your constructs.\n\r", ch);
    return;
  }

  if (IS_NULLSTR(target_name)){

    send_to_char("syntax: psionic control <element>\n\r"\
		 "Following are valid choices for element.\n\r", ch);
    for (i = 0; i < 6; i ++){
      sendf(ch, "%s\n\r", type[i]);
    }
    return;
  }
  /* check for valid choice */
  for (i = 0; i < 6; i ++){
    if (!str_prefix(target_name, type[i])){
      fFound = TRUE;
      break;
    }
  }
  if (!fFound){
    send_to_char("syntax: psionic control <element>\n\r"\
		 "Following are valid choices for element.\n\r", ch);
    for (i = 0; i < 6; i ++){
      sendf(ch, "%s\n\r", type[i]);
    }
    return;
  }
  /* the way this works is we store the position of attack type 
     that we selected by taking the value of i and adding attack positon
     of "hydrokinesis" 
  */
  sendf(ch, "Your psionic blast will now tend towards element of %s.\n\r",
	type[i]);
  i += attack_lookup("hydrokinesis");
  af.type = sn;
  af.level = level;
  af.duration = 12;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = i;
  affect_to_char(ch, &af);
}

void spell_unholy_gift( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;
  /* This is a spell that used unlife effect for two purposes
     1) Tatto ability for tattoo of namless to give a person ability to 
     appear as undead (No vuln to holy, 12h duration)
     2) Elder Vamp ability (Vuln to holy, 480 duration)
  */
  if (IS_UNDEAD(victim) || is_affected(victim, gen_unlife))
    return;

  af.where = TO_RESIST;
  af.type = gen_unlife;
  af.level= ch->level;
  af.duration =  ch == victim ? 12 : 240;
  af.location = APPLY_NONE;
  /* Normal processing in gen but no sleep*/
  af.modifier = ch == victim ? 1 : 0;
  af.bitvector = RES_COLD | RES_POISON | RES_DISEASE;
  affect_to_char( victim, &af );
  if (victim != ch){
    af.where = TO_VULN;
    /* ignored in gen */
    af.modifier = -1;
    af.bitvector = VULN_HOLY;
    affect_to_char( victim, &af );
  }
}

void spell_drug_use( int sn, int level, CHAR_DATA *ch, void *vo,int target){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af, *paf;
  int chance, dur = 720;

  if ( (paf = affect_find(ch->affected, gen_addiction)) != NULL
       && paf->modifier != 0){
    send_to_char("Your withdrawal pains subside.\n\r",victim);
    paf->modifier = 0;
    return;
  }
  chance = 80 + (get_curr_stat(ch, STAT_CON) - 15);

  if (number_percent() < chance || paf != NULL)
    return;

  act_new("You have become addicted to drugs!",ch,NULL,victim,TO_CHARVICT, POS_DEAD );
  act("$n begins to shudder and shake as $e is overcome with addiction.",victim,NULL,NULL,TO_ROOM );
  send_to_char("You begin to shudder and shake from your addiction!\n\r",victim);

  af.where     = TO_AFFECTS;
  af.type      = gen_addiction;
  af.level     = level;
  af.duration  = dur;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = -1;
  affect_to_char(ch, &af);   
}  

void spell_death_cube( int sn, int level, CHAR_DATA *ch, void *vo,int target){
  OBJ_DATA* cube = (OBJ_DATA*) vo;
  OBJ_DATA* obj, *obj_next;


/* all we do here is check for multiple death cubes */
  for (obj = ch->carrying; obj != NULL; obj = obj_next){
    obj_next = obj->next_content;

    if (obj != cube && obj->pIndexData->vnum == cube->pIndexData->vnum){
      act("$p crumbles to dust.", ch, obj, NULL, TO_ALL);
      extract_obj(obj);
    }
  }

/* check if cube has 5 cost */
  if (cube->cost < 5)
    return;

/* send a note about it */
  {
    BUFFER *buffer;
    NOTE_DATA *note;

    char buf[MIL];
    char arg[MIL];

    char* sender = "`!HAL 1000``";
    char* namelist, *strtime;
    
    buffer = new_buf();
    sprintf(buf,"`!Following generated at %s.``\n\r",((char *) ctime( &mud_data.current_time )));
    add_buf(buffer,buf);
    sprintf(buf, "%s's %s has achived charge of %d, and has been extracted.",
	    ch->name, cube->short_descr, cube->cost);
    add_buf(buffer, buf);
    namelist = str_dup("immortal");
    for (;;)
      {
	namelist = one_argument(namelist,arg);
	if (arg[0] == '\0')
	  break;
	note = new_note();
	note->prefix = 0;
	note->next = NULL;
	note->type = NOTE_NOTE;
	free_string(note->sender);
	note->sender = str_dup( sender );
	free_string(note->subject);
	note->subject = str_dup("RP Quest Update (Soul Cube)");
	free_string(note->text);
	note->text = str_dup(buf_string(buffer));
	strtime                         = ctime( &mud_data.current_time );
	strtime[strlen(strtime)-1]      = '\0';
	note->date                 = str_dup( strtime );
	note->date_stamp           = mud_data.current_time;
	free_string(note->to_list);
	note->to_list = str_dup(arg);
	append_note(note);    
      }
    free_buf(buffer);
  }
  act("$p rises into the air and with a flash of dark light.. Disappears!.", ch, cube, NULL, TO_ALL);
  extract_obj(cube);
}

void spell_doomsday(int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
  if (strcmp(ch->name, "Crypticant"))
    {
      act("`!You attempt to harness the dark forces around the Doomsday Cube, but to no avail!``", ch, NULL, NULL, TO_CHAR); 
      act("`!$n attempts to harness the dark forces around the Doomsday Cube, but fails!``", ch, NULL, NULL, TO_ROOM);
      vo = ch;
    }

  if (vo != NULL)
    {
      act("`8The Doomsday Cube drains another soul!", ch , NULL, vo, TO_CHAR);
      act("`8You feel your soul pulled from your body...``", ch , NULL, vo, TO_VICT);
      act("`8$N's body collapses in the throes of death, as $s soul is drawn into the cube!\n\r``", ch, NULL, vo, TO_NOTVICT); 
      act("`8The world blackens as your mortal body dies...``", ch, NULL, vo, TO_VICT);
      raw_kill(ch, vo);
    }
}       

void spell_fired_projectile( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    CHAR_DATA *victim;
    int dam;
    int weight = get_obj_weight(obj);

    if ((victim = ch->fighting) == NULL)
    {
        send_to_char("You are not in combat.\n\r", ch);  
        return;
    }

    if (!CAN_WEAR(obj, ITEM_TAKE)
        || CAN_WEAR(obj, ITEM_NO_SAC)
        || obj->item_type == ITEM_CABAL
        || is_affected_obj(obj, gsn_gravitate)
        || weight > 1000

        )

    {
        send_to_char("You cannot lift that object.\n\r", ch);
        return;
    }

    dam = dice(level / 5, 8);
    dam += UMIN(weight, 500) / 3;
    if ( saves_spell( level, victim,DAM_PIERCE,skill_table[sn].spell_type) )
      dam /= 2;
    act( "You fire $p straight at $N!", ch, obj, victim, TO_CHAR);
    act( "$n fires a $p straight at you!", ch, obj, victim, TO_VICT );
    act( "$n fires a $p straight at $N!", ch, obj, victim, TO_NOTVICT );
    damage( ch, victim, dam, sn, DAM_PIERCE ,TRUE);
    /* move the object from where it is to the ground */
    if (!ch->in_room)
      return;
    if (obj->carried_by)
      obj_from_char(obj);
    else if (obj->in_obj)
      obj_from_obj(obj);
    else if (obj->in_room)
      obj_from_room(obj); 
    obj_to_ch(obj, victim);
} 

/* PSALMS */
void spell_righteous( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return ;
  af.type	= sn;
  af.level	= level;
  af.duration   = 96;
  af.where      = TO_AFFECTS;
  af.bitvector	= 0;
  af.modifier	= number_fuzzy(level / 7);
  af.location	= APPLY_HITROLL;
  affect_to_char(ch,&af);
  af.location	= APPLY_DAMROLL;
  affect_to_char(ch,&af);
}

void spell_dvoid( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  if (is_affected(ch, gen_dvoid))
    return;
  af.type	= gen_dvoid;
  af.level	= level;
  af.duration   = 120;
  af.where      = TO_AFFECTS;
  af.bitvector	= 0;
  af.modifier	= -2;
  af.location	= APPLY_SAVING_SPELL;
  affect_to_char(ch,&af);
}

void spell_insight( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  if (is_affected(ch, gen_dvoid))
    return;
  af.type	= sn;
  af.level	= level;
  af.duration   = 300;
  af.where      = TO_AFFECTS;
  af.bitvector	= AFF_DETECT_INVIS | AFF_DETECT_EVIL | AFF_DETECT_GOOD;
  af.modifier	= 0;
  af.location	= APPLY_NONE;
  affect_to_char(ch,&af);
}

void spell_dwrath( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA *obj = get_eq_char(ch, WEAR_WIELD);
  AFFECT_DATA af;
  if (obj == NULL)
    return;
  if (is_affected_obj(obj, sn))
    return;
  af.type	= sn;
  af.level	= level;
  af.duration   = 24;
  af.where      = TO_WEAPON;
  af.bitvector	= 0;
  af.modifier	= attack_lookup("divine");
  af.location	= APPLY_O_DTYPE;
  affect_to_obj(obj, &af);
  af.type	= sn;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

}

void spell_sbane( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA *obj = get_eq_char(ch, WEAR_WIELD);
  AFFECT_DATA af;
  if (obj == NULL)
    return;
  if (is_affected_obj(obj, sn))
    return;
  af.type	= sn;
  af.level	= level;
  af.duration   = 24;
  af.where      = TO_WEAPON;
  af.bitvector	= 0;
  af.modifier	= attack_lookup("rays");
  af.location	= APPLY_O_DTYPE;
  affect_to_obj(obj, &af);
  af.type	= sn;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

}


void spell_pwater( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA *obj = get_eq_char(ch, WEAR_WIELD);
  AFFECT_DATA af;
  if (obj == NULL)
    return;
  if (is_affected_obj(obj, sn))
    return;
  af.type	= sn;
  af.level	= level;
  af.duration   = 24;
  af.where      = TO_WEAPON;
  af.bitvector	= 0;
  af.modifier	= attack_lookup("spray");
  af.location	= APPLY_O_DTYPE;
  affect_to_obj(obj, &af);
  af.type	= sn;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

}


void spell_icefire( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA *obj = get_eq_char(ch, WEAR_WIELD);
  AFFECT_DATA af;
  if (obj == NULL)
    return;
  if (is_affected_obj(obj, sn))
    return;
  af.type	= sn;
  af.level	= level;
  af.duration   = 24;
  af.where      = TO_WEAPON;
  af.bitvector	= 0;
  af.modifier	= attack_lookup("icefire");
  af.location	= APPLY_O_DTYPE;
  affect_to_obj(obj, &af);
  af.type	= sn;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

}

void spell_smight( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA *obj = get_eq_char(ch, WEAR_WIELD);
  AFFECT_DATA af;
  if (obj == NULL)
    return;
  if (is_affected_obj(obj, sn))
    return;
  af.type	= sn;
  af.level	= level;
  af.duration   = 24;
  af.where      = TO_WEAPON;
  af.bitvector	= WEAPON_FLAMING;
  af.modifier	= attack_lookup("flame");
  af.location	= APPLY_O_DTYPE;
  affect_to_obj(obj, &af);
  af.type	= sn;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

}

void spell_dmight( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA *obj = get_eq_char(ch, WEAR_WIELD);
  AFFECT_DATA af;
  if (obj == NULL)
    return;
  if (is_affected_obj(obj, sn))
    return;
  af.type	= sn;
  af.level	= level;
  af.duration   = 24;
  af.where      = TO_WEAPON;
  af.bitvector	= WEAPON_PARALYZE;
  af.modifier	= attack_lookup("shbite");
  af.location	= APPLY_O_DTYPE;
  affect_to_obj(obj, &af);
  af.type	= sn;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

}

void spell_vitality( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return;

  af.type = sn;
  af.level = level;
  af.duration = 300;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_HIT;
  af.modifier = level;
  affect_to_char(ch, &af);
  af.location = APPLY_HIT_GAIN;
  af.modifier = 30;
  affect_to_char(ch, &af);
}

void spell_sretrib( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  if (is_affected(ch, sn))
    return;
  af.type	= sn;
  af.level	= level;
  af.duration   = 48;
  af.where      = TO_AFFECTS;
  af.bitvector	= 0;
  af.modifier	= 30;
  af.location	= APPLY_MOVE_GAIN;
  affect_to_char(ch,&af);
}

void spell_virtues( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return;

  af.type = sn;
  af.level = level;
  af.duration = -1;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 1;
  affect_to_char(ch, &af);

}

void spell_preserve( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return;

  af.type = sn;
  af.level = level;
  af.duration = 46;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

}

void spell_fpart( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return;

  af.type = sn;
  af.level = level;
  af.duration = 48;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

}

void spell_vredem( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return;

  af.type = sn;
  af.level = level;
  af.duration = 240;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

}


void spell_ogtrium( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return;

  af.type = sn;
  af.level = level;
  af.duration = 240;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

}

void spell_strium( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return;

  af.type = sn;
  af.level = level;
  af.duration = 240;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);
  af.type = gen_strium;
  af.where = TO_NONE;
  affect_to_char(ch, &af);

}

void spell_ptrium( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return;

  af.type = sn;
  af.level = level;
  af.duration = 240;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);
  send_to_char("A feeling of divine purity fills your flesh.\n\r", ch);

}

void spell_uorder( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  bool fMsg = FALSE;
  int gsn_behead = skill_lookup("behead");
  if (is_affected(ch, sn))
    return;
  send_to_char("A high official of the Order of Crusades walks up to you.\n\r", ch);
  send_to_char("The official says '`#Know that as long as you remain true to our Holy Mission``\n\r", ch);
  send_to_char("The official says '`#the Order will overlook the rare adornments  you place upon yourself.``\n\r", ch);
  af.type = sn;
  af.level = level;
  af.duration = -1;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);

/* check for skill boost */
  if (get_skill(ch, gsn_crusade) < 100){
    int sn = gsn_crusade;
    SKILL_DATA* nsk;
    if ( (nsk = nskill_find(ch->pcdata->newskills,sn)) != NULL){
      nsk->learned = 100;
    }
    else
      ch->pcdata->learned[sn] = 100;
    fMsg = TRUE;
  }
  if (get_skill(ch, gsn_avenger) < 100){
    int sn = gsn_avenger;
    SKILL_DATA* nsk;
    if ( (nsk = nskill_find(ch->pcdata->newskills,sn)) != NULL){
      nsk->learned = 100;
    }
    else
      ch->pcdata->learned[sn] = 100;
    fMsg = TRUE;
  }
  if (get_skill(ch, gsn_behead) < 100){
    int sn = gsn_behead;
    SKILL_DATA* nsk;
    if ( (nsk = nskill_find(ch->pcdata->newskills,sn)) != NULL){
      nsk->learned = 100;
      fMsg = TRUE;
    }
  }
  if (fMsg)
    send_to_char("Your soul soars and you are enlighted in the ways of the Order of Crusades!", ch);
}

void spell_istrength( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return;

  af.type = sn;
  af.level = level;
  af.duration = 240;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_STR;
  af.modifier = 25;
  affect_to_char(ch, &af);

}

void spell_dheal( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return;
  if (!ch->fighting){
    send_to_char("You feel your body quickly regenerate itsself.\n\r", ch);
    ch->hit = ch->max_hit;
    update_pos(ch);
  }
  else
    send_to_char("Your concentration is disturbed.\n\r", ch);
  
  af.type = sn;
  af.level = level;
  af.duration = 4;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_HIT_GAIN;
  af.modifier = 200;
  affect_to_char(ch, &af);
}

void spell_purity( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af, *paf;
  
  if ( (paf = affect_find(ch->affected, gen_ward)) != NULL
       && paf->has_string
       && !IS_NULLSTR(paf->string)
       && !str_cmp(paf->string, skill_table[gsn_poison].name))
    return;
  af.type	= gen_ward;
  af.level	= level;
  af.duration   = 96;
  af.where      = TO_NONE;
  af.bitvector	= 0;
  af.modifier	= 0;
  af.location	= APPLY_NONE;
  paf = affect_to_char(ch,&af);
  string_to_affect(paf, skill_table[gsn_poison].name);
  af.type = sn;
  af.where = TO_AFFECTS;
  paf = affect_to_char(ch,&af);
}


void spell_const( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af, *paf;
  
  if ( (paf = affect_find(ch->affected, gen_ward)) != NULL
       && paf->has_string
       && !IS_NULLSTR(paf->string)
       && !str_cmp(paf->string, skill_table[gsn_plague].name))
    return;
  af.type	= gen_ward;
  af.level	= level;
  af.duration   = 96;
  af.where      = TO_NONE;
  af.bitvector	= 0;
  af.modifier	= 0;
  af.location	= APPLY_NONE;
  paf = affect_to_char(ch,&af);
  string_to_affect(paf, skill_table[gsn_plague].name);
  af.type = sn;
  af.where = TO_AFFECTS;
  paf = affect_to_char(ch,&af);
}

void spell_epal( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af, *paf;
  
  if ( (paf = affect_find(ch->affected, gen_ward)) != NULL
       && paf->has_string
       && !IS_NULLSTR(paf->string)
       && !str_cmp(paf->string, skill_table[gsn_blindness].name))
    return;
  af.type	= gen_ward;
  af.level	= level;
  af.duration   = 96;
  af.where      = TO_NONE;
  af.bitvector	= 0;
  af.modifier	= 0;
  af.location	= APPLY_NONE;
  paf = affect_to_char(ch,&af);
  string_to_affect(paf, skill_table[gsn_blindness].name);
  af.type = sn;
  af.where = TO_AFFECTS;
  paf = affect_to_char(ch,&af);
}

void spell_baptize( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  int i = 0;

  if (is_affected(ch, sn))
    return;
  send_to_char("As you complete the psalm your memories begin to grow cloudy.\n\r", ch);
  act("$n's eyes cloud over and $e gets a dreamy look on $s face.", ch, NULL, NULL, TO_ROOM);
  send_to_char("You've lost knowledge of all your Psalms!\n\r", ch);
  if (IS_NPC(ch))
    return;
  
  /* lower max psalm by one */
  if (get_skill(ch, skill_lookup("psalm master")) < 1)
    ch->pcdata->psalm_pen += 1;
  /* wipe all the psalms clean */
  for (i = 0; i < MAX_PSALM; i ++){
    ch->pcdata->psalms[i] = 0;
  }
  af.type = sn;
  af.level = level;
  af.duration = 10;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_INT;
  af.modifier = -10;
  affect_to_char(ch, &af);
}

void spell_icalm( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn))
    return;
  send_to_char("As you complete the psalm a wave of holiness washes over you.\n\r", ch);
  act("$n's face beams with inner light for a moment then fades.", ch, NULL, NULL, TO_ROOM);
  if (IS_NPC(ch))
    return;
  
/* raise holy weapon by 2 */
  hwep_gain(ch, ch, 0 );
  af.type = sn;
  af.level = level;
  af.duration = 10;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier =  0;
  affect_to_char(ch, &af);
}

void spell_webcaster( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;

  if ( saves_spell( level, victim, DAM_OTHER, skill_table[sn].spell_type) ){
    act("A sticky web falls on $n but $e breaks free.", victim, NULL, NULL, TO_ROOM);
    act("A sticky web falls on you but you manage to break free.", victim, NULL, NULL, TO_CHAR);
    WAIT_STATE2(victim, skill_table[sn].beats / 2);
    return;
  }
  act("A sticky web falls on $n enveloping $m tightly.", victim, NULL, NULL, TO_ROOM);
  act("A sticky web falls on you enveloping you tightly.", victim, NULL, NULL, TO_CHAR);
  if (!IS_NPC(victim)
      && victim->pcdata->pStallion ){
    do_dismount(victim, "");
  }
  WAIT_STATE2(victim, skill_table[sn].beats );
  if (is_affected(victim, gsn_thrash))
    return;
  af.type	= gsn_thrash;
  af.level	= ch->level;
  af.duration	= 1;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_DEX;
  af.modifier	= -2;
  affect_to_char(victim,&af);
  if (IS_AFFECTED(victim, AFF_FLYING)){
    act("You can't seem to get back into the air",victim,NULL,NULL,TO_CHAR);
    act("$n can't seem to get back into the air",victim,NULL,NULL,TO_ROOM);
  }
}

void spell_bind_spirit(int sn,int level,CHAR_DATA *ch, void *vo,int target){

  AFFECT_DATA af;
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  CHAR_DATA* victim = NULL;
  bool found = FALSE;
    
  if ( is_affected( ch, gen_spirit ) ){
    
    send_to_char("You've already bound a spirit to yourself.\n\r", ch);
    return;
  }
  if (obj->item_type != ITEM_CORPSE_PC && obj->item_type != ITEM_CORPSE_NPC){
    send_to_char("That's not a corpse.\n\r",ch);
    return;
  }
  if (obj->owner == 0 || obj->pIndexData->vnum == OBJ_VNUM_FAKE_CORPSE
      || is_affected_obj(obj, sn )){
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

  if (victim == NULL){
    send_to_char("The spirits have already departed that corpse.\n\r", ch );
    return;
  }
  else{
    af.type      = gen_spirit;
    af.level     = victim->level + 10;
    af.duration  = 1 + number_fuzzy(ch->level / 5);

    af.where     = TO_AFFECTS;
    af.bitvector = 0;

    af.location  = APPLY_HIT_GAIN;
    af.modifier  = -25;
    affect_to_char( ch, &af );

    af.location  = APPLY_MANA_GAIN;
    af.modifier  = -25;

    affect_to_char( ch, &af );

    act ("You bind the spirit of $N to your will!", ch, NULL, victim, TO_CHAR);
    act ("$n binds the spirit of $N to his will!", ch, NULL, victim, TO_ROOM);

    if (IS_NPC(victim))
      extract_char(victim,TRUE);
    
    af.duration = -1;
    af.location = 0;
    af.modifier = 0;
    af.type	= sn;
    affect_to_obj( obj, &af );
  }
}

void spell_stallion( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{

    AFFECT_DATA af;
    CHAR_DATA *mob;

    if ( is_affected( ch, gsn_mounted ) ){
      send_to_char("You already have a mount.\n\r",ch);
      return;
    }
    if (IS_SET(ch->in_room->room_flags,ROOM_INDOORS))
    {
      send_to_char("You cannot call onto the Goliath indoors!\n\r",ch);
      return;
    }
    mob = create_mobile( get_mob_index(MOB_VNUM_STALLION) );

    act("The ground shaking under its hoofs $N comes to your side!", ch,NULL,mob, TO_CHAR);
    act("The ground shaking under its hoofs, a giant warhorse comes to $n's side!", ch,NULL, mob, TO_ROOM);

    af.type      = gsn_mounted;
    af.level	 = level;
    af.duration  = 24;
    af.where	 = TO_AFFECTS;
    af.bitvector = 0;
    af.location  = APPLY_NONE;
    af.modifier  = mob->pIndexData->vnum;
    affect_to_char( ch, &af );
}


bool defuse_traps(CHAR_DATA* ch, TRAP_DATA* ptrap){
  bool fFound = FALSE;

  //run through traps and disable them.
  for (;ptrap; ptrap = ptrap->next_trap){
    act("You defuse $t.", ch, ptrap->name, NULL, TO_CHAR);
    act("$n defuses $t.", ch, ptrap->name, NULL, TO_ROOM);
    ptrap->armed = FALSE;
    fFound = TRUE;

    /* make sure to take care of delay traps here */
    REMOVE_BIT(ptrap->flags, TRAP_DELAY);
    
    /* owned traps get removed */
    if (ptrap->owner){
      if (ptrap->owner != ch && IS_TRAP(ptrap, TRAP_NOTIFY))    
	sendf(ptrap->owner, "You sense %s has defused %s.\n\r", PERS2(ch), ptrap->name);
      else if (ch != ptrap->owner)
	sendf(ptrap->owner, "You sense %s has been defused.\n\r", ptrap->name);
      extract_trap( ptrap );
    }
  }//end for traps
  return fFound;
}
void spell_defusion( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA* obj;
  TRAP_DATA* ptrap;
  EXIT_DATA* pexit;
  int door = 0;
  bool fFound = FALSE;

  //check room traps first
  for (door = 0; door < MAX_DOOR; door++){
    if ( (pexit = ch->in_room->exit[door]) == NULL)
      continue;
    else if ( (ptrap = pexit->traps) == NULL)
      continue;
    else
      fFound |= defuse_traps( ch, ptrap );
  }//end for room exists

  for (obj = ch->in_room->contents; obj; obj = obj->next_content){
    if ( (ptrap = obj->traps) == NULL)
      continue;
    else
      fFound |= defuse_traps( ch, ptrap);
  }

  if (!fFound){
    send_to_char("There aren't any traps here.\n\r", ch);
  }
}


void spell_steel_wall( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, gsn_mimic)){
      send_to_char("You must first get rid of the reflective shield protecting you.\n\r", ch);
      return;
  }
  if (is_affected(ch,sn)){
    send_to_char("You are already protected by the wall of steel.\n\r", ch);
    return;
  }	
  if (IS_AFFECTED2(ch, AFF_FIRE_SHIELD)){
    send_to_char("The magical flames around you seem to interfere!\n\r", ch);
    return;
  }
  af.type   = sn;
  af.level  = level;
  af.duration = number_fuzzy(level/8);
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_AC;
  af.modifier = -1 * (level  + level / number_fuzzy( 3 ));

  affect_to_char(ch,&af);
  send_to_char("You summon  mass of steel shards to whirl around you.\n\r",ch);
  act("A mass of whirling steel shards forms a deadly wall around $n.", ch, NULL, NULL, TO_ROOM);
}


/* BLADEMASTER SPELLS */
void spell_deathweaver( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn)){
    send_to_char("You're already in this stance.\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_deathweaver)
	   || is_affected(ch, gsn_doomsinger)
	   || is_affected(ch, gsn_shadowdancer)
	   || is_affected(ch, gsn_puppet_master)
	   || is_affected(ch, gsn_bladestorm)
	   || is_affected(ch, gsn_ironarm)
	   || is_affected(ch, gsn_iron_curtain)){
    send_to_char("You may enter only a single stance at a time.\n\r", ch);
    return;
  }
  send_to_char("You clear your mind and focus on the secrets of Deathweaver.\n\r", ch);
  act("$n meditates briefly and looks far more stable and sure of $s footing.", ch, NULL, NULL, TO_ROOM);

  af.type	= sn;
  af.duration	= 12;
  af.level	= level;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( ch, &af);
}

void spell_bladestorm( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn)){
    send_to_char("You're already in this stance.\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_deathweaver)
	   || is_affected(ch, gsn_doomsinger)
	   || is_affected(ch, gsn_shadowdancer)
	   || is_affected(ch, gsn_puppet_master)
	   || is_affected(ch, gsn_bladestorm)
	   || is_affected(ch, gsn_ironarm)
	   || is_affected(ch, gsn_iron_curtain)){
    send_to_char("You may enter only a single stance at a time.\n\r", ch);
    return;
  }
  else if ( get_eq_char(ch, WEAR_SECONDARY) == NULL){
    send_to_char("You must use two weapons with this stance.\n\r", ch);
    return;
  }
  send_to_char("You clear your mind and focus on the secrets of Bladestorm.\n\r", ch);
  act("$n meditates briefly and begins to move much faster.", ch, NULL, NULL, TO_ROOM);

  af.type	= sn;
  af.duration	= 12;
  af.level	= level;

  af.where	= TO_AFFECTS;
  af.bitvector	= AFF_HASTE;
  af.location	= APPLY_AC;
  af.modifier	= 25 + number_fuzzy(level);
  affect_to_char( ch, &af);
}

void spell_ironarm( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn)){
    send_to_char("You're already in this stance.\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_deathweaver)
	   || is_affected(ch, gsn_doomsinger)
	   || is_affected(ch, gsn_shadowdancer)
	   || is_affected(ch, gsn_puppet_master)
	   || is_affected(ch, gsn_bladestorm)
	   || is_affected(ch, gsn_ironarm)
	   || is_affected(ch, gsn_iron_curtain)){
    send_to_char("You may enter only a single stance at a time.\n\r", ch);
    return;
  }
  else if ( get_eq_char(ch, WEAR_SECONDARY) == NULL){
    send_to_char("You must use two weapons with this stance.\n\r", ch);
    return;
  }

  send_to_char("You clear your mind and focus on the secrets of Ironarm.\n\r", ch);
  act("$n meditates briefly then steps into a fighting stance.", ch, NULL, NULL, TO_ROOM);

  af.type	= sn;
  af.duration	= 12;
  af.level	= level;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_DAMROLL;
  af.modifier	= number_fuzzy(level / 10);
  affect_to_char( ch, &af);

}

void spell_iron_curtain( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn)){
    send_to_char("You're already in this stance.\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_deathweaver)
	   || is_affected(ch, gsn_doomsinger)
	   || is_affected(ch, gsn_shadowdancer)
	   || is_affected(ch, gsn_puppet_master)
	   || is_affected(ch, gsn_bladestorm)
	   || is_affected(ch, gsn_ironarm)
	   || is_affected(ch, gsn_iron_curtain)){
    send_to_char("You may enter only a single stance at a time.\n\r", ch);
    return;
  }
  else if ( get_eq_char(ch, WEAR_SECONDARY) == NULL){
    send_to_char("You must use two weapons with this stance.\n\r", ch);
    return;
  }
  send_to_char("You clear your mind and focus on the secrets of Kyousanken.\n\r", ch);
  act("$n meditates briefly then moves $s weapons to cover $mself.", ch, NULL, NULL, TO_ROOM);

  af.type	= sn;
  af.duration	= 12;
  af.level	= level;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_HIT;
  af.modifier	= level;
  affect_to_char( ch, &af);
}
void spell_doomsinger( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn)){
    send_to_char("You're already in this stance.\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_deathweaver)
	   || is_affected(ch, gsn_doomsinger)
	   || is_affected(ch, gsn_shadowdancer)
	   || is_affected(ch, gsn_puppet_master)
	   || is_affected(ch, gsn_bladestorm)
	   || is_affected(ch, gsn_ironarm)
	   || is_affected(ch, gsn_iron_curtain)){
    send_to_char("You may enter only a single stance at a time.\n\r", ch);
    return;
  }
  else if ( has_twohanded(ch) == NULL){
    send_to_char("You must use a two handed weapon with this stance.\n\r", ch);
    return;
  }

  send_to_char("You clear your mind and focus on the secrets of Doomsinger.\n\r", ch);
  act("$n meditates briefly then swings $s weapon which emits a strange tone.", ch, NULL, NULL, TO_ROOM);

  af.type	= sn;
  af.duration	= 12;
  af.level	= level;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_HITROLL;
  af.modifier	= number_fuzzy(level / 10);
  affect_to_char( ch, &af);
}
void spell_shadowdancer( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn)){
    send_to_char("You're already in this stance.\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_deathweaver)
	   || is_affected(ch, gsn_doomsinger)
	   || is_affected(ch, gsn_shadowdancer)
	   || is_affected(ch, gsn_puppet_master)
	   || is_affected(ch, gsn_bladestorm)
	   || is_affected(ch, gsn_ironarm)
	   || is_affected(ch, gsn_iron_curtain)){
    send_to_char("You may enter only a single stance at a time.\n\r", ch);
    return;
  }
  else if ( has_twohanded(ch) == NULL){
    send_to_char("You must use a two handed weapon with this stance.\n\r", ch);
    return;
  }

  send_to_char("You clear your mind and focus on the secrets of Shadowdancer.\n\r", ch);
  act("$n meditates briefly and begins to move in a strange alien way.", ch, NULL, NULL, TO_ROOM);

  af.type	= sn;
  af.duration	= 12;
  af.level	= level;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_AC;
  af.modifier	= -level;
  affect_to_char( ch, &af);
}

void spell_puppet_master( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  OBJ_DATA* obj;
  int wep;

  if (is_affected(ch, sn)){
    send_to_char("You're already in this stance.\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_deathweaver)
	   || is_affected(ch, gsn_doomsinger)
	   || is_affected(ch, gsn_shadowdancer)
	   || is_affected(ch, gsn_puppet_master)
	   || is_affected(ch, gsn_bladestorm)
	   || is_affected(ch, gsn_ironarm)
	   || is_affected(ch, gsn_iron_curtain)){
    send_to_char("You may enter only a single stance at a time.\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(target_name)){
    send_to_char("Imitate which weapon type?\n\r", ch);
    return;
  }
  else if ( (wep = weapon_lookup( target_name)) < 0){
    send_to_char("No such weapon type.\n\r", ch);
    return;
  }
  else if ( (obj = has_twohanded(ch)) == NULL){
    send_to_char("You must use a two handed weapon with this stance.\n\r", ch);
    return;
  }
  send_to_char("You clear your mind and focus on the secrets of Kairishi.\n\r", ch);
  act("$n meditates briefly then changes $s hold on $s weapon.", ch, NULL, NULL, TO_ROOM);

  af.type	= sn;
  af.duration	= 12;
  af.level	= level;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= wep;
  affect_to_obj( obj, &af);

  af.location	= APPLY_HITROLL;
  af.modifier	= number_fuzzy(level / 7);
  affect_to_char( ch, &af);

}

//blademaster blood vow for their weapon
void spell_blood_vow( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA* obj = (OBJ_DATA*) vo;
  AFFECT_DATA af;

  const int dur = 120;

  if (obj->item_type != ITEM_WEAPON){
    send_to_char("You can only swear a blood vow onto a weapon.\n\r", ch);
    return;
  }
  else if (obj->wear_loc != -1){
    send_to_char("The weapon must be in your inventory.\n\r",ch);
    return;
  }
  else if (is_affected(ch, sn) && !is_affected_obj(obj, sn)){
    send_to_char("You're not yet ready to swear a new blood vow.\n\r", ch);
    return;
  }

  //strip the bloodvow from the weapon
  affect_strip_obj(obj, sn);
  //strip it from us
  affect_strip(ch, sn);

  //object affects
  af.type	= sn;
  af.level	= level;
  af.duration	= dur;

  //bless-evil and hitroll
  af.where	= IS_GOOD(ch) || IS_EVIL(ch) ? TO_OBJECT : TO_WEAPON;
  af.bitvector	= IS_GOOD(ch) ? ITEM_BLESS : IS_EVIL(ch) ? ITEM_EVIL : WEAPON_SHARP;
  af.location	= APPLY_HITROLL;
  af.modifier	= level / 5;
  affect_to_obj(obj, &af);

  //nodrop and hp
  af.where	= TO_OBJECT;
  af.bitvector	= ITEM_NODROP;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_obj(obj, &af);

  //mana and sharp
  af.where	= TO_OBJECT;
  af.bitvector	= ITEM_NOUNCURSE;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_obj(obj, &af);

  //take off the mana/hp from the character
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char(ch, &af);

  act("You slice your palms on $p and swear the blood vow.", ch, obj, NULL, TO_CHAR);
  act("$n slices $s palms on $p and swears the blood vow.", ch, obj, NULL, TO_ROOM);
  damage(ch, ch, number_range(1, 3), sn, DAM_INTERNAL, TRUE );
}

//warcry like spell
void spell_battlesphere( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  
  if (is_affected(ch, sn)){
    send_to_char("You've already focus your mind on your battle sphere.\n\r",ch);
    return;
  }
  if (IS_AFFECTED(ch,AFF_CALM)){
    send_to_char("You can't get worked up in your calm state.\n\r",ch);
    return;
  }

  act("You empty your mind and focus on your sphere of battle.",ch,NULL,NULL,TO_CHAR);
  act("$n closes $s eyes and meditates briefly.", ch, NULL, NULL, TO_ROOM);
    
  af.where		= TO_AFFECTS;
  af.type		= sn;
  af.level		= level;
  af.duration		= level;
  af.modifier		= UMAX(1, ch->level / 8);;
  af.location		= APPLY_HITROLL;
  af.bitvector		= 0;
  affect_to_char(ch,&af);
  af.modifier	= 0 - (ch->level /8);
  af.location	= APPLY_SAVING_SPELL;
  affect_to_char(ch,&af);
}


//allows counters etc. while blinded
void spell_battlefocus( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  
  if (is_affected(ch, sn)){
    send_to_char("You've already focused on battle.\n\r",ch);
    return;
  }
  else if (!is_affected(ch, gsn_battlesphere)){
    send_to_char("You must first enter the battlesphere.\n\r", ch);
    return;
  }
  if (IS_AFFECTED(ch,AFF_CALM)){
    send_to_char("You can't get worked up in your calm state.\n\r",ch);
    return;
  }

  act("You empty your mind and focus only on battle and possible attacks.",ch,NULL,NULL,TO_CHAR);
  act("$n closes $s eyes and meditates briefly.", ch, NULL, NULL, TO_ROOM);
    
  af.type		= sn;
  af.level		= level;
  af.duration		= level / 2;
  af.where		= TO_AFFECTS;
  af.bitvector		= 0;
  af.modifier		= -ch->level;
  af.location		= APPLY_AC;

  affect_to_char(ch,&af);
}


//negates blind penalties
void spell_battletrance( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  
  if (is_affected(ch, sn)){
    send_to_char("You've already entered the battletrance.\n\r",ch);
    return;
  }
  else if (!is_affected(ch, gsn_battlefocus)){
    send_to_char("You must first enter the battlefocus.\n\r", ch);
    return;
  }
  if (IS_AFFECTED(ch,AFF_CALM)){
    send_to_char("You can't get worked up in your calm state.\n\r",ch);
    return;
  }

  act("You empty your mind and enter a deep trance.",ch,NULL,NULL,TO_CHAR);
  act("$n closes $s eyes and seems to enter a deep trance.", ch, NULL, NULL, TO_ROOM);
    
  af.type		= sn;
  af.level		= level;
  af.duration		= level / 3;
  af.where		= TO_AFFECTS;
  af.bitvector		= 0;
  af.location		= APPLY_NONE;
  af.modifier		= 0;
  affect_to_char(ch,&af);
}


void spell_soul_tap( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA* soul;
  AFFECT_DATA af;

  const int dur = 72;

  if (is_affected(ch, sn)){
    send_to_char("You're still tapping the energies in the last soul!\n\r", ch);
    return;
  }
  else if ( (soul = get_eq_char(ch, WEAR_SHROUD)) == NULL){
    send_to_char("You must first wear a soul.\n\r", ch);
    return;
  }
  
  act("A dark vortex of negative energy consumes $p!", ch, soul, NULL, TO_ALL);
  
  af.type	= sn;
  af.level	= level;
  af.duration	= number_range(dur, 3 * dur / 2);
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_MALED_LEVEL;
  af.modifier	= 1;

  affect_to_char( ch, &af);
  soul->timer = 1;
}
  
