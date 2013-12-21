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
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "interp.h"
#include "cabal.h"
#include "jail.h"
#include "olc.h"
#include "mob_cmds.h"


#define CD CHAR_DATA
CD *    find_keeper     args( (CHAR_DATA *ch ) );
#undef CD

extern void dam_update args ((CHAR_DATA *ch, CHAR_DATA *victim, int dam) );
extern void do_mpzecho	( CHAR_DATA *ch, char *argument );
extern bool check_social( CHAR_DATA *ch, char *command, char *argument) ;
extern void cast_new( CHAR_DATA *ch, char *argument, bool iscommune, bool fIgnorePos );
extern inline int check_anatomy(CHAR_DATA* ch, CHAR_DATA* victim);
char *target_name;

/* CABAL SPELLS SKILLS ETC */

void throatcut(CHAR_DATA* ch, CHAR_DATA* victim, bool fMantis){
  AFFECT_DATA af, *paf;
  const int sn = skill_lookup("throat cut");
  int  chance = 4 * get_skill(ch, sn) / 5;

  /* chance */
  chance += 2 * (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX));
  chance += (ch->level - victim->level);
  chance += affect_by_size(ch,victim);
  chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));

  /* yells */
  a_yell(ch, victim);
  if (victim->fighting != ch && victim != ch){
    char arg1[MIL];
    sprintf(arg1, "Die %s you bloody fool!",PERS(ch,victim));
    j_yell(victim,arg1);
  }

  /* check for success */
  if (is_affected(victim, gen_bleed)){
    send_to_char("Their throat has already been cut open.\n\r", ch);
    chance = 0;
  }

  if (number_percent() > chance){
    damage(ch, victim, 0, sn, DAM_SLASH, TRUE );
    check_improve(ch, sn, FALSE, 2 );
    return;
  }
  else
    check_improve(ch, sn, TRUE, 0 );

//MANTIS CHECK
  if (fMantis && is_affected(victim,gsn_mantis) && IS_AWAKE(victim)){
    act("You sense $n's movement and throw $m to the ground!",ch,NULL,victim,TO_VICT);
    act("$N senses your attack, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
    act("$N senses $n's attack, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE2(ch, URANGE(2, 1 + number_range(1, victim->size), 4) * PULSE_VIOLENCE);
    do_visible(ch, "");

    affect_strip(victim,gsn_mantis);
    damage(victim,ch,10,gsn_mantis,DAM_BASH,TRUE);
    return;
  }//END MATIS

  damage(ch, victim, 5, sn, DAM_SLASH, TRUE );
  act("With a deft slice you gore $N's throat!", ch, NULL, victim, TO_CHAR );
  act("With a deft slice $n gores $N's throat!", ch, NULL, victim, TO_NOTVICT );
  act("With a deft slice $n gores your throat!", ch, NULL, victim, TO_VICT );
  if (IS_UNDEAD(victim))
    return;
  /* we hit their throat, attach the gen and do the damage */
  af.type = gen_bleed;
  af.level = ch->level;
  af.duration = number_range(2, 4);
  af.where = TO_NONE;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  /* modifier controls how many individuals bleeds happen (8 /tick) */
  af.modifier = af.duration * 8;
  paf = affect_to_char(victim, &af);
  string_to_affect(paf, ch->name);
}

/* ***** KNIGHT POWERS ***** */

void spell_stand_ground( int sn, int level, CHAR_DATA *ch, void *vo,int target){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;
  int skill = 0;

  if (is_affected(victim, sn)){
    if (ch == victim)
      send_to_char("You've already enhanced your defenses!\n\r", ch );
    else
      act("$N has already enhanced $S defenses!", ch, NULL, victim, TO_CHAR );
    return;
  }

  if (ch == victim)
    act("You enhance your ability to stand ground in combat.", victim, NULL, NULL, TO_CHAR);
  else
    act("You enhance $N's ability to stand ground in combat.", ch, NULL, victim, TO_CHAR);

  if (get_skill(victim, gsn_shield_block) > 1)
    skill = gsn_shield_block;
  else if (get_skill(victim, gsn_2hands) > 1)
    skill = gsn_2hands;
  else if (get_skill(victim, gsn_dodge) > 1)
    skill = gsn_dodge;
  else
    skill = gsn_parry;

  af.type	= sn;
  af.level	= level;
  af.duration	= -1;
  af.where	= TO_SKILL;
  af.bitvector	= 0;
  af.location	= skill;
  af.modifier	= 10;
  affect_to_char( victim, &af );

  af.where	= TO_AFFECTS;
  af.location	= APPLY_AC;
  af.modifier	= -level;
  affect_to_char( victim, &af );

  af.where	= TO_AFFECTS;
  af.location	= APPLY_DAMROLL;
  af.modifier	= number_range(4, 6);
  affect_to_char( victim, &af );
}



void spell_holy_weapon( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    char buf[MSL], buf2[MSL], arg[MIL], arg2[MIL];
    OBJ_DATA *obj;
    AFFECT_DATA *paf;
    AFFECT_DATA af;
    int temp = 0;
    target_name = one_argument(target_name, arg);
    target_name = one_argument(target_name, arg2);



/* Checks to see if the caster is a mob */
    if (IS_NPC(ch))
        return;
    buf[0]='\0';
    buf2[0]='\0';
    if (arg[0] == '\0')
    {
	send_to_char("syntax: cast 'chaosbane' <weapon type or \"beads\">.\n\r",ch);
	return;
    }
    if (is_affected(ch, sn))
      {
	send_to_char("You are not ready to create a new weapon.\n\r", ch);
	return;
      }

    obj = create_object( get_obj_index( OBJ_VNUM_HOLY_WEAPON ), level);
    if (!str_cmp(arg, "dagger"))
    {
	free_string( obj->name );
	strcat(buf, "a gleaming steel dagger");
	obj->name = str_dup( buf );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	strcat(buf, " is here.");
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->value[0] = WEAPON_DAGGER;
	obj->value[3] = attack_lookup("pierce");
	obj->weight = 40;
    }
    else if (!str_cmp(arg, "mace"))
    {
	free_string( obj->name );
	strcat(buf, "a gleaming steel mace");
	obj->name = str_dup( buf );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	strcat(buf, " is here.");
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->value[0] = WEAPON_MACE;
	obj->value[3] = attack_lookup("crush");
	obj->weight = 250;
    }
    else if (!str_cmp(arg, "flail"))
    {
	free_string( obj->name );
	strcat(buf, "a gleaming steel flail");
	obj->name = str_dup( buf );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	strcat(buf, " is here.");
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->value[0] = WEAPON_FLAIL;
	obj->value[3] = attack_lookup("pound");
	obj->weight = 120;
    }
    else if (!str_cmp(arg, "sword"))
    {
	free_string( obj->name );
	strcat(buf, "a gleaming steel sword");
	obj->name = str_dup( buf );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	strcat(buf, " is here.");
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->value[0] = WEAPON_SWORD;
	obj->value[3] = attack_lookup("slash");
	obj->weight = 180;
    }
    else if (!str_cmp(arg, "axe"))
    {
	free_string( obj->name );
	strcat(buf, "a gleaming two-handed axe");
	obj->name = str_dup( buf );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	strcat(buf, " is here.");
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->value[0] = WEAPON_AXE;
	obj->value[3] = attack_lookup("cleave");
	obj->value[4] |= (flag_value( weapon_type2, "twohands" ) != NO_FLAG
              ? flag_value( weapon_type2, "twohands" ) : 0 );
	obj->weight = 350;
    }
    else if (!str_cmp(arg, "staff"))
    {
	free_string( obj->name );
	strcat(buf, "a gleaming steel staff");
	obj->name = str_dup( buf );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	strcat(buf, " is here.");
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->value[0] = WEAPON_STAFF;
	obj->value[3] = attack_lookup("smash");
	obj->value[4] |= (flag_value( weapon_type2, "twohands" ) != NO_FLAG
              ? flag_value( weapon_type2, "twohands" ) : 0 );
	obj->weight = 50;
    }
    else if (!str_cmp(arg, "polearm") || !str_cmp(arg, "glaive"))
    {
	free_string( obj->name );
	strcat(buf, "a gleaming steel glaive");
	obj->name = str_dup( buf );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	strcat(buf, " is here.");
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->value[0] = WEAPON_POLEARM;
	obj->value[3] = attack_lookup("cleave");
	obj->value[4] |= (flag_value( weapon_type2, "twohands" ) != NO_FLAG
			  ? flag_value( weapon_type2, "twohands" ) : 0 );
	obj->weight = 280;
    }
    else if (!str_cmp(arg, "spear"))
    {
	free_string( obj->name );
	strcat(buf, "a gleaming steel spear");
	obj->name = str_dup( buf );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	strcat(buf, " is here.");
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->value[0] = WEAPON_SPEAR;
	obj->value[3] = attack_lookup("pierce");
	obj->weight = 150;
    }
    else if (!str_cmp(arg, "exotic"))
    {
	free_string( obj->name );
	strcat(buf, "a gleaming steel katar");
	obj->name = str_dup( buf );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );
	strcat(buf, " is here.");
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->value[0] = WEAPON_EXOTIC;
	obj->value[3] = attack_lookup("pierce");
	obj->weight = 50;
    }
    else if (!str_cmp(arg, "beads")){
      int level = ch->level;
      OBJ_DATA* beads;
      OBJ_SPELL_DATA sp, *spb;

      beads = create_object( get_obj_index( OBJ_VNUM_PRAYER_BEADS ), 0);

      beads->level = level;
      beads->timer = 24;

      af.type       = gsn_beads;
      af.level      = ch->level;
      af.duration   = -1;
      af.location   = APPLY_HITROLL;
      af.modifier   = 10;
      af.bitvector  = 0;
      affect_to_obj( beads, &af );

      af.location   = APPLY_AC;
      af.modifier   = -50;
      affect_to_obj( beads, &af );

      af.location   = APPLY_SAVING_SPELL;
      af.modifier   = -5;
      affect_to_obj( beads, &af );

      af.location   = APPLY_LUCK;
      af.modifier   = 5;
      affect_to_obj( beads, &af );

      /* change the short desc */
      free_string( beads->short_descr );
      beads->short_descr = str_dup("gleaming steel beads");

      free_string( beads->description );
      beads->description = str_dup("You see gleaming steel beads here.");

      /* add the spell */
      sp.spell	= skill_lookup("cure serious");
      sp.target = SPELL_TAR_SELF;
      sp.percent= 30;

      spb = spell_to_obj(beads, &sp, FALSE);

      spb->message = str_dup("$p shine with inner light.");
      spb->message2= str_dup("$p shine with inner light.");

      obj_to_char(beads, ch);
      act( "You create $p.", ch, beads, NULL, TO_CHAR );
      act( "$n creates $p.", ch, beads, NULL, TO_ROOM );

      af.type = sn;
      af.level = level;
      af.duration = 24;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      affect_to_char(ch, &af);

      if (ch->pcdata->rank >= 2)
	SET_BIT(obj->extra_flags, ITEM_NODROP);
      if (ch->pcdata->rank >= 3)
	SET_BIT(obj->extra_flags, ITEM_NOREMOVE);
      return;
    }
    else
    {
      send_to_char("You can't make a Chaosbane like that!\n\r", ch);
      return;
    }

    temp = UMAX(3, (get_skill(ch, sn) - 50) / 7);
    obj->value[1] = number_range(temp, 7);
    obj->value[2] = number_range(3, level / 10) + 3;
    obj->level = level;
    obj->timer = 24;
    paf = new_affect();
    paf->type       = sn;
    paf->level      = level;
    paf->duration   = -1;
    paf->location   = APPLY_HITROLL;
    paf->modifier   = level/10 + 1;
    paf->bitvector  = 0;
    paf->next       = obj->affected;
    obj->affected   = paf;
    paf = new_affect();
    paf->type       = sn;
    paf->level      = level;
    paf->duration   = -1;
    paf->location   = APPLY_DAMROLL;
    paf->modifier   = level/10 + (IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS) ? 3 : 1);
    paf->bitvector  = 0;
    paf->next       = obj->affected;
    obj->affected   = paf;

    if (ch->pcdata->rank >= 2)
      SET_BIT(obj->extra_flags, ITEM_NODROP);
    if (ch->pcdata->rank >= 3)
      SET_BIT(obj->extra_flags, ITEM_NOREMOVE);

    if (IS_ELDER(ch))
    {
	free_string( obj->name );
	strcat(buf2, "'Chaosbane' the Bringer of Order");
	obj->name = str_dup( buf2 );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf2 );
	strcat(buf2, " is here.");
	free_string( obj->description );
	obj->description = str_dup( buf2 );
    	obj->timer = 48;
    	obj->value[1] = 7;
    	obj->value[2] = 8;
	obj->value[3] = attack_lookup("divine");
    }
    obj->value[4] |= flag_value( weapon_type2, "sharp" );
/* tack on "spiked" */
    sprintf(buf, "%s spiked", obj->name );
    free_string(obj->name);
    obj->name = str_dup( buf );

    obj_to_char(obj, ch);
    act( "You create $p.", ch, obj, NULL, TO_CHAR );
    act( "$n creates $p.", ch, obj, NULL, TO_ROOM );

    af.type = sn;
    af.level = level;
    af.duration = 24;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    affect_to_char(ch, &af);
}

void do_guard(CHAR_DATA *ch, char *argument)
{
    char arg[MIL];
    CHAR_DATA *victim;
    const int cost = skill_table[gsn_guard].min_mana;
    const int lag = skill_table[gsn_guard].beats;


    one_argument(argument, arg);

    if (IS_NPC(ch))
	return;
    if ( get_skill(ch,gsn_guard) < 2){
      send_to_char( "Huh?\n\r", ch );
      return;
    }
    if ( arg[0] == '\0' )
      {
        send_to_char("Guard whom?\n\r", ch);
        return;
      }
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
      {
        send_to_char( "They aren't here.\n\r", ch );
        return;
      }
    if (IS_NPC(victim))
      {
        sendf(ch, "%s doesn't need any of your help!\n\r", PERS2(victim));
        return;
      }
    if (!str_cmp(arg, "none") || !str_cmp(arg, "self") || victim == ch)
      {
	if (ch->pcdata->guarding == NULL)
	{
	    send_to_char("You can't guard yourself!\n\r", ch);
	    return;
	}
	else
        {
	    act("You stop guarding $N.", ch, NULL, ch->pcdata->guarding, TO_CHAR);
	    act("$n stops guarding you.", ch, NULL, ch->pcdata->guarding, TO_VICT);
	    act("$n stops guarding $N.", ch, NULL, ch->pcdata->guarding, TO_NOTVICT);
	    ch->pcdata->guarding->pcdata->guarded_by = NULL;
	    ch->pcdata->guarding = NULL;
	    return;
        }
    }
    if (ch->pcdata->guarding == victim)
    {
        sendf(ch, "You're already guarding %s!\n\r", PERS2(victim));
        return;
    }
    if (ch->pcdata->guarding != NULL)
    {
        send_to_char("But you're already guarding someone else!\n\r", ch);
        return;
    }
    if (victim->pcdata->guarded_by != NULL)
    {
        sendf(ch, "%s is already being guarded by someone.\n\r", PERS2(victim));
        return;
    }
    if (victim->pcdata->guarding == ch)
    {
        sendf(ch, "But %s is guarding you!\n\r", PERS2(victim));
        return;
    }
    if (!is_same_group(victim, ch))
    {
        sendf(ch, "But you aren't in the same group as %s.\n\r", PERS2(victim));
        return;
    }
    if (IS_AFFECTED(ch,AFF_CHARM))
    {
        sendf(ch, "You like your master too much to bother guarding %s!\n\r", PERS2(victim));
        return;
    }
    if (victim->fighting != NULL)
    {
        sendf(ch, "Why don't you let %s stop fighting first?\n\r", PERS2(victim));
        return;
    }
    if (ch->fighting != NULL)
    {
        send_to_char("Better finish your own battle before you worry about guarding someone else.\n\r",ch);
        return;
    }
    if (ch->mana < cost){
      send_to_char("You lack the concentration.\n\r", ch );
      return;
    }
    else
      ch->mana -= cost;
    WAIT_STATE( ch, lag );

    /* subtract cp's */
    handle_skill_cost( ch, ch->pCabal, gsn_guard );

    act("You are now guarding $N.", ch, NULL, victim, TO_CHAR);
    act_new("You are being guarded by $n.", ch, NULL, victim, TO_VICT, POS_DEAD);
    act("$n is now guarding $N.", ch, NULL, victim, TO_NOTVICT);
    ch->pcdata->guarding = victim;
    victim->pcdata->guarded_by = ch;
}

void spell_vengeance( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;

  if (is_affected(ch, gen_vengeance)){
    send_to_char("You've already shielded yourself with Vengeance of Light.\n\r", ch );
    return;
  }
  act("An angry white aura surrounds $n.", ch, NULL, NULL, TO_ROOM );
  act("An angry white aura surrounds you.", ch, NULL, NULL, TO_CHAR );

  af.type	= gen_vengeance;
  af.level	= level;
  af.duration	= number_fuzzy( 8 );

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char(ch, &af );
}


void spell_sense_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

/* Checks to see if the caster is a mob */
    if (IS_NPC(ch))
        return;
    if ( is_affected( ch, sn ) )
    {
	send_to_char("You are already sensing evil presences.\n\r",ch);
	return;
    }
    af.where	 = TO_AFFECTS2;
    af.type      = sn;
    af.level	 = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_SENSE_EVIL;
    affect_to_char( ch, &af );
    af.where	 = TO_AFFECTS;
    af.bitvector = AFF_DETECT_EVIL;
    affect_to_char( ch, &af );
    send_to_char( "You feel more aware of the evil surrounding you.\n\r", ch );
}

void spell_ward_of_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

/* Checks to see if the caster is a mob */
    if (IS_NPC(ch))
        return;
    if ( is_affected( ch, sn ) )
      {
	send_to_char("You are already divinely blessed.\n\r",ch);
	return;
    }
    af.where	 = TO_AFFECTS;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 24;
    af.modifier  = 15;
    af.location  = APPLY_MANA_GAIN;
    af.bitvector = AFF_WARD_LIGHT;
    affect_to_char( ch, &af );
    af.location  = APPLY_DAMROLL;
    af.modifier  = level / 15;
    affect_to_char( ch, &af );
    send_to_char( "You feel divinely blessed.\n\r", ch );
}


/* Minor Banishment for Knights(linked in const.c & magic.h) */
void spell_mbanish( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dam;

    //const
    const int dur = number_range(0, 1);

/* Checks to see if the victim is a demon */
    if (victim->race != race_lookup("demon"))
    {
	send_to_char("The spell seems to fail.\n\r",ch);
	return;
    }
/* Checks to see if the victim is casting on themselves */
    if (victim == ch)
    {
    	send_to_char("Banish yourself?  What a novel idea.\n\r",ch);
	return;
    }

    dam = number_range(level / 2, level);
    if (ch->race == race_lookup("demon") && !IS_IMMORTAL(ch)){
      send_to_char("Something goes horribly wrong.\n\r", ch);
      damage(ch,victim,3 * dam,sn,DAM_INTERNAL, TRUE);
      return;
    }


//we do small amount of damage and set hysteria and drained on failed save.

    if ( saves_spell( level, victim,DAM_OTHER,skill_table[sn].spell_type))
    {
      act("You momentarly weaken $N's link to the Nexus.", ch, NULL, victim, TO_CHAR);
      act("You feel your link to Chaos falter.", ch, NULL, victim, TO_VICT);
      damage(ch,victim,dam,sn,DAM_INTERNAL, TRUE);
      return;
    }
    dam *= 2;
    act("You completely severe $N's link to the Nexus!", ch, NULL, victim, TO_CHAR);
    act("You panic as your link to Chaos is severed!", ch, NULL, victim, TO_VICT);
    act("$N begins to panic.", victim, NULL, victim, TO_ROOM);
    damage(ch,victim,dam,sn,DAM_INTERNAL, TRUE);

    //setup hysteria and drain for one tick.
    af.where     = TO_AFFECTS;
    af.type      = gsn_hysteria;
    af.level     = level;
    af.location  = APPLY_HITROLL;
    af.duration  = dur;
    af.bitvector = 0;
    af.modifier  = -2;
    if (!is_affected(victim, gsn_hysteria))
      affect_to_char(victim,&af);
    af.where     = TO_AFFECTS;
    af.type      = gsn_drained;
    af.level     = level;
    af.location  = APPLY_DAMROLL;
    af.duration  = dur;
    af.bitvector = 0;
    af.modifier  = -2;
    if (!is_affected(victim, gsn_drained))
      affect_to_char(victim,&af);

}

/* ***** LEGION POWERS ***** */


/* void spell_nightmare(int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED2(victim, AFF_NIGHTMARE) )
    {
        if (victim == ch)
            act("You are already under the effects of nightmare.",ch,NULL,victim,TO_CHAR);
        else
            act("$N has already been terrorized by nightmares.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if ( saves_spell( level, victim,DAM_NEGATIVE,skill_table[sn].spell_type) )
    {
	act("You failed to invade $N's dreams.",ch,NULL,victim,TO_CHAR);
	return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 10;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_NIGHTMARE;
    affect_to_char( victim, &af );
    affect_strip ( victim, gsn_invis			);
    affect_strip ( victim, gsn_mass_invis		);
    if ( IS_NPC(victim) || !IS_SET(race_table[victim->race].aff,AFF_SNEAK) )
    {
    	affect_strip ( victim, gsn_sneak			);
	REMOVE_BIT   ( victim->affected_by, AFF_SNEAK	);
    }
    REMOVE_BIT   ( victim->affected_by, AFF_HIDE	);
    REMOVE_BIT   ( victim->affected_by, AFF_INVISIBLE	);
    affect_strip(victim,gsn_camouflage);
    REMOVE_BIT   ( victim->affected2_by, AFF_CAMOUFLAGE	);
    act_new( "You are surrounded by a pink outline.", victim, NULL, NULL, TO_CHAR, POS_DEAD );
    act( "$n is surrounded by a pink outline.", victim, NULL, NULL, TO_ROOM );
} */


/* remove shadowdemon */
void spell_shadowdemon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *mob;
    int i;
    if (get_summoned(ch, MOB_VNUM_SHADOWDEMON) > 0)
    {
        send_to_char("Your shadowdemon is already in existence.\n\r",ch);
        return;
    }
    if ( is_affected( ch, sn ) )
    {
        send_to_char("You are too weak to summon a shadowdemon.\n\r",ch);
        return;
    }
    af.where     = TO_AFFECTS2;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    act ("You summon a shadowdemon from the depths of darkness!", ch, NULL, NULL, TO_CHAR);
    act ("$n summons a shadowdemon from the depths of darkness!", ch, NULL, NULL, TO_ROOM);
    mob = create_mobile( get_mob_index( MOB_VNUM_SHADOWDEMON ) );
    char_to_room(mob,ch->in_room);
    mob->level      = ch->level;
    mob->max_hit    = ch->level * 20;
    mob->hit        = mob->max_hit;
    mob->hitroll    = ch->level /2;
    mob->damroll    = ch->level /2;
    mob->damage[DICE_NUMBER]	= 5;
    mob->damage[DICE_TYPE]	= 9;
    for (i = 0; i < 4; i++)
        mob->armor[i] = ch->armor[i];
    for (i = 0; i < MAX_STATS; i++)
    {
        mob->perm_stat[i]     = ch->perm_stat[i];
        mob->mod_stat[i]      = ch->mod_stat[i];
    }
    SET_BIT(mob->affected_by, AFF_CHARM);
    mob->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
    mob->summoner = ch;
    add_follower(mob,ch);
    mob->leader=ch;
}

/* ***** WARLORD POWERS ***** */
bool cyclone( CHAR_DATA*ch, CHAR_DATA* victim, int skill, int sn ){
  int chance = skill;

  chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
  chance += (ch->level - victim->level);
  chance -= get_skill(victim,gsn_dodge) / 10;
//bigger people are easier tohit
  chance -= affect_by_size(ch,victim);
  chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));

  switch(ch->in_room->sector_type){
  case(SECT_INSIDE):		chance -= 25;	break;
  case(SECT_CITY):		chance -= 15;	break;
  case(SECT_FIELD):		chance +=  5;	break;
  case(SECT_FOREST):				break;
  case(SECT_HILLS):				break;
  case(SECT_MOUNTAIN):	chance += 10;	break;
  case(SECT_WATER_SWIM):	chance  =  0;	break;
  case(SECT_WATER_NOSWIM):	chance  =  0;	break;
  case(SECT_AIR):		chance  =  0;  	break;
  case(SECT_DESERT):		chance += 25;   break;
  }

  if (chance == 0){
    send_to_char("There isn't any dirt here.\n\r",ch);
    return FALSE;
  }

  chance = chance - 25 + UMIN(ch->level, 25);

  if (is_affected(victim, gsn_mist_form)){
      act("The dirt flies right through the mist.", ch, NULL, victim, TO_CHAR);
      act("The dirt flies right through $N.", ch, NULL, victim, TO_NOTVICT);
      act("The dirt flies through your non-corporeal form.", ch, NULL, victim, TO_CHAR);
      chance = 0;
  }

  if (number_percent() < chance){
    bool fDirtProt =  (victim->race == race_lookup("slith") && number_percent() < victim->level);
    AFFECT_DATA af;

    act("$n is blinded by the dirt in $s eyes!",victim,NULL,NULL,TO_ROOM);
    act("$n kicks dirt in your eyes!",ch,NULL,victim,TO_VICT);
    send_to_char("You can't see a thing!\n\r",victim);
    check_improve(ch,gsn_dirt,TRUE,1);

    if (fDirtProt){
      send_to_char("Your lizard ancestry serves you well and you easly recover.\n\r", victim);
    }
    else if (!is_affected(victim, gsn_dirt)){
      af.where	= TO_AFFECTS;
      af.type 	= gsn_dirt;
      af.level 	= ch->level;
      af.duration	= 0;
      af.location	= APPLY_HITROLL;
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
      if (is_affected(victim, gsn_battletrance))
	af.modifier      = 0;
      else
	af.modifier      = -3;
      }
      else
	af.modifier      = -6;
      af.bitvector 	= AFF_BLIND;

      affect_to_char(victim,&af);

      af.location     = APPLY_AC;
      if (!IS_NPC(victim) && number_percent() < get_skill(victim,gsn_blind_fighting)){
      if (is_affected(victim, gsn_battletrance))
	af.modifier      = 0;
      else
	af.modifier      = +5;
      }
      else
	af.modifier      = +15;
      affect_to_char(victim,&af);
    }
    damage(ch, victim, number_range(ch->level, 3 * ch->level / 2), sn, DAM_PIERCE, TRUE );
    if (victim && victim->in_room)
      return TRUE;
    else
      return FALSE;
  }
  else{
    damage(ch,victim,0,sn,DAM_NONE,TRUE);
    check_improve(ch,gsn_dirt,FALSE,1);
    return FALSE;
  }
}
void do_cyclone(CHAR_DATA *ch, char *argument){
  CHAR_DATA *victim, *vch, *vch_next;
  AFFECT_DATA af, *paf;
  char buf[MSL];

  const int sn = gsn_cyclone;
  const int skill = get_skill(ch, sn );
  const int lag = skill_table[sn].beats;
  const int cost = skill_table[sn].min_mana;

  if ( skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument)){
    if ( (victim = ch->fighting) == NULL){
      send_to_char("But you aren't in combat!\n\r",ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, NULL, argument)) == NULL){
    send_to_char("They aren't here.\n\r",ch);
    return;
  }
  else if (is_safe(ch,victim))
    return;

  if (ch->mana < cost ){
    send_to_char("You cannot concentrate sufficiently.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  handle_skill_cost( ch, ch->pCabal, sn);

  /* all safe to go */
  WAIT_STATE2( ch, lag );

  if (number_percent() > skill){
    send_to_char("You failed to execute the form.\n\r", ch);
    check_improve(ch, sn, FALSE, 5);
    return;
  }
  else
    check_improve(ch, sn, TRUE, 1);

  act("$n executes the 'Cyclone'.", ch, NULL, victim, TO_ROOM);
  send_to_char("You execute the 'Cyclone'.\n\r", ch);

  if (ch->fighting != victim && victim->fighting != ch){
    sprintf(buf, "Help! %s blew dirt into my eyes!",PERS(ch,victim));
    j_yell(victim,buf);
  }

  a_yell(ch, victim );
  /* target first */
  if (cyclone(ch, victim, skill, sn )){
    /* attach the skill marker on the user for dblow */
    af.type	= sn;
    af.level	= ch->level;
    af.duration	= 3;
    af.where	= TO_NONE;
    af.bitvector	= 0;
    af.location	= APPLY_NONE;
    af.modifier	= 0;
    affect_to_char( ch, &af );

    /* maelstrom refresh */
    if ( (paf = affect_find(ch->affected, gsn_maelstrom)) != NULL)
      paf->duration = af.duration;
  }

  /* we run through all the group mates kicking dirt and damaging */
  for (vch = ch->in_room->people; vch; vch = vch_next){
    vch_next = vch->next_in_room;

    if (victim == vch
	|| IS_AFFECTED(vch, AFF_BLIND)
	|| !is_same_group(vch, victim)
	|| is_area_safe_quiet(ch, vch )){
      continue;
    }

    /* check if we can dirt */
    if ( vch->in_room->sector_type != SECT_WATER_SWIM
	 && vch->in_room->sector_type != SECT_WATER_NOSWIM
	 && vch->in_room->sector_type != SECT_AIR
	 && !is_affected(vch, gsn_dirt )){
      if (ch->fighting != vch && vch->fighting != ch){
	sprintf(buf, "Help! %s blew dirt into my eyes!",PERS(ch,vch));
	j_yell(vch, buf);
      }
      a_yell(ch, vch );
      cyclone(ch, vch, skill, sn );
    }
  }
}

void do_maelstrom(CHAR_DATA *ch, char *argument){
  CHAR_DATA *victim;
  AFFECT_DATA af, *paf;
  OBJ_DATA* wield;
  char buf[MSL];

  const int sn = gsn_maelstrom;
  const int skill = get_skill(ch, sn );
  const int lag = skill_table[sn].beats;
  const int cost = skill_table[sn].min_mana;
  const int hits = 4;
  int dam_type = DAM_SLASH;

  int i = 0;

  if ( skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument)){
    if ( (victim = ch->fighting) == NULL){
      send_to_char("But you aren't in combat!\n\r",ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, NULL, argument)) == NULL){
    send_to_char("They aren't here.\n\r",ch);
    return;
  }
  else if (is_safe(ch,victim))
    return;

  if (ch->mana < cost ){
    send_to_char("You cannot concentrate sufficiently.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  handle_skill_cost( ch, ch->pCabal, sn);

  /* all safe to go */
  WAIT_STATE2( ch, lag );

  if (number_percent() > skill){
    send_to_char("You failed to execute the form.\n\r", ch);
    check_improve(ch, sn, FALSE, 5);
    return;
  }
  else
    check_improve(ch, sn, TRUE, 1);

  act("$n executes the 'Maelstrom'.", ch, NULL, victim, TO_ROOM);
  send_to_char("You execute the 'Maelstrom'!\n\r", ch);

  if (ch->fighting != victim && victim->fighting != ch){
    sprintf(buf, "Help! %s is trying to chop me into bits!",PERS(ch,victim));
    j_yell(victim, buf);
  }
  a_yell(ch, victim );

  if ( (wield = get_eq_char( ch, WEAR_WIELD)) != NULL){
    dam_type = attack_table[wield->value[3]].damage;
  }
  for (i = 0; i < hits; i ++){
    int dam = number_range(ch->level / 3, 3 * ch->level / 4);

    /* occasionaly we try to disarm stuff */
    if (number_percent() < (skill - 75) || i == 2){
      if (get_eq_char(victim, WEAR_WIELD) != NULL)
	disarm( ch, victim );
      else if (get_eq_char(victim, WEAR_SHIELD) != NULL)
	shield_disarm( ch, victim );
      else if (get_eq_char(victim, WEAR_SECONDARY) != NULL)
	disarm_offhand(ch, victim );
    }
    damage( ch, victim, dam, sn, dam_type,  TRUE );
    if (victim->fighting == NULL)
      break;
  }
  /* attach the skill marker on the user for dblow */
  af.type	= sn;
  af.level	= ch->level;
  af.duration	= 3;
  af.where	= TO_NONE;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( ch, &af );

  /* check for cyclone refresh */
  if ( (paf = affect_find(ch->affected, gsn_cyclone)) != NULL)
    paf->duration = af.duration;
}

/* DEATHBLOW
   NORMAL: no gsn_cyclone, and no gsn_maelstrom
   STRIP : gsn_maelstrom present
   SPLIT : gsn_cyclone present
   FULL  : both gsn_cyclone and gsn_maelstrom present
*/
void deathblow( CHAR_DATA* ch, CHAR_DATA* victim, int type, bool fMiss ){
  AFFECT_DATA af;
  OBJ_DATA* wield;
  int dam = 0, i= 0;
  int dam_type = DAM_SLASH;
  bool fStrip = FALSE;

  if (ch == NULL || victim == NULL)
    return;
  wield = get_eq_char( ch, WEAR_WIELD );

  switch( type ){
  default:
  case DB_NORMAL:
    send_to_char("You execute 'Power Strike'!\n\r", ch );
    act("$n executes 'Power Strike'!", ch, NULL, victim, TO_ROOM);
    if (fMiss)
      dam = 0;
    else{
      dam = get_wepdam( wield, ch, victim, gsn_deathblow, &dam_type );
      dam = 150 * dam / 100;
    }
    damage(ch, victim, dam, gsn_deathblow, dam_type, TRUE);
    WAIT_STATE2(ch, skill_table[gsn_deathblow].beats);
    break;
  case DB_SPLIT:
    send_to_char("You execute the 'Ground Splitter'!\n\r", ch );
    act("$n executes the 'Ground Splitter'!", ch, NULL, victim, TO_ROOM);
    if (fMiss)
      dam = 0;
    else{
      dam = get_wepdam( wield, ch, victim, gsn_deathblow, &dam_type );
      dam = 150 * dam / 100;
    }
    if (!is_affected(victim, gsn_deathblow)){
      af.type		= gsn_deathblow;
      af.level		= ch->level;
      af.duration	= 1;
      af.where		= TO_SKILL;
      af.bitvector	= 0;
      af.location	= skill_lookup("second attack");
      af.modifier	= -50;
      affect_to_char( victim, &af );
    }
    damage(ch, victim, dam, gsn_trophy, dam_type, TRUE);
    WAIT_STATE2(ch, skill_table[gsn_deathblow].beats / 2);
    break;
  case DB_STRIP:
    send_to_char("You execute the 'Flesh Stripper'!\n\r", ch );
    act("$n executes the 'Flesh Stripper'!", ch, NULL, victim, TO_ROOM);
    for ( i = 0; i < 4; i++){
      dam = get_wepdam( wield, ch, victim, wield->value[3], &dam_type );
      if (fMiss)
	dam = 15 * dam / 100;
      else
	dam = 40 * dam / 100;

      /* increases the rate of losing concentration */
      dam = damage(ch, victim, dam, wield->value[3] + TYPE_HIT, dam_type, TRUE);
      if (victim->fighting == NULL)
	break;
      if (dam > 1){
	AFFECT_DATA* paf;
	if  ( (paf = affect_find(victim->affected, gsn_mind_disrupt)) == NULL
	      || paf->modifier > -40){
	  af.type		= gsn_mind_disrupt;
	  af.level		= ch->level;
	  af.duration		= 1;
	  af.where		= TO_AFFECTS;
	  af.bitvector		= 0;
	  af.location		= APPLY_NONE;
	  af.modifier		= -10;
	  affect_join( victim, &af );
	  fStrip = TRUE;
	}
      }
    }
    if (fStrip)
      act("You've hampered $N's ability to concentrate on spells.", ch, NULL, victim, TO_CHAR);
    WAIT_STATE2(ch, skill_table[gsn_deathblow].beats / 2);
    break;
  case DB_FULL:
    send_to_char("You've delivered a Deathblow'!\n\r", ch );
    act("$n delivers a Deathblow!", ch, NULL, victim, TO_ROOM);
    if (fMiss)
      dam = 0;
    else{
      dam = get_wepdam( wield, ch, victim, gsn_deathblow, &dam_type );
      dam = 250 * dam / 100;
      if (dam > 450)
	dam = 450;
    }
    damage(ch, victim, dam, gsn_forms, dam_type, TRUE);
    WAIT_STATE2(ch, skill_table[gsn_deathblow].beats / 2);
    break;
  }
}
void do_deathblow(CHAR_DATA *ch, char *argument){
  CHAR_DATA *victim;
  AFFECT_DATA *paf;

  const int sn = gsn_deathblow;
  const int skill = get_skill(ch, sn );
  const int cost = skill_table[sn].min_mana;

  bool fMiss = FALSE;
  bool fMae = FALSE;
  bool fCyc = FALSE;

  if ( skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if ( (victim = ch->fighting) == NULL){
    send_to_char("But you aren't in combat!\n\r",ch);
    return;
  }
  else if (is_safe(ch,victim))
    return;

  if (ch->mana < cost ){
    send_to_char("You cannot concentrate sufficiently.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  handle_skill_cost( ch, ch->pCabal, sn);

  /* all safe to go */
//  (lag done in void deathblow)
  if (number_percent() > 9 * skill / 10)
    fMiss = TRUE;
  check_improve(ch, sn, TRUE, 5 );

  if ( (paf = affect_find(ch->affected, gsn_cyclone)) != NULL)
    fCyc = TRUE;
  if ( (paf = affect_find(ch->affected, gsn_maelstrom)) != NULL)
    fMae = TRUE;

  if (fMae && fCyc)
    deathblow(ch, victim, DB_FULL, fMiss );
  else if (fMae)
    deathblow(ch, victim, DB_STRIP, fMiss );
  else if (fCyc)
    deathblow(ch, victim, DB_SPLIT, fMiss );
  else
    deathblow(ch, victim, DB_NORMAL, fMiss );
}

void do_cognizance(CHAR_DATA *ch, char *argument)
{

    if (is_affected(ch,gsn_cognizance))
    {
        send_to_char("Your awareness is as sharp as it will get.\n\r",ch);
        return;
    }
    if (ch->mana < 50)
    {
        send_to_char("You cannot seem to focus enough.\n\r",ch);
	return;
    }
    WAIT_STATE( ch, skill_table[gsn_cognizance].beats );
    if (!IS_NPC(ch) && number_percent() < get_skill(ch,gsn_cognizance))
    {
        AFFECT_DATA af;
	af.where  = TO_AFFECTS;
        af.type   = gsn_cognizance;
        af.level  = ch->level;
        af.duration = ch->level;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_DETECT_INVIS;
        affect_to_char(ch,&af);
        af.bitvector = AFF_DETECT_MAGIC;
	affect_to_char(ch,&af);
        ch->mana -= 50;
        act("You feel more alive as your awareness improves!",ch,NULL,NULL,TO_CHAR);
        act("$n looks more enlightened.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_cognizance,TRUE,3);
    }
    else
    {
        ch->mana -= 25;
        act("You look about sharply, but you don't see anything new.",ch,NULL,NULL,TO_CHAR);
        act("$n looks around sharply but doesn't seem enlightened.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_cognizance,FALSE,3);
    }
}

void do_stance(CHAR_DATA *ch, char *argument){
  CHAR_DATA* vch;
  int cost = skill_table[gsn_stance].min_mana;
  int lag = skill_table[gsn_stance].beats;
  int skill = get_skill(ch, gsn_stance );

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (is_affected(ch,gsn_stance)){
    check_dispel( -99, ch, gsn_stance);
    return;
  }

  if (ch->mana < cost){
    send_to_char("You cannot muster up the energy.\n\r",ch);
    return;
  }

  if (ch->class == class_lookup("monk")
      && !is_empowered_quiet(ch,0)
      && !is_empowered_quiet(ch,1)){
    send_to_char("You must be in either offensive, or defensive empowerment.\n\r", ch);
    return;
  }

  if ( (vch = get_group_room( ch, TRUE)) != NULL){
    act("$N gets in your way.", ch, NULL, vch, TO_CHAR);
    return;
  }

  if (ch->fighting)
    WAIT_STATE( ch, lag / 2 );
  else
    WAIT_STATE( ch, lag );

  if (number_percent() < get_skill(ch,gsn_stance)){
    AFFECT_DATA af;
    int monk_def = FALSE;	//0 = offense, 1 = defense;

    af.type   = gsn_stance;
    af.level  = ch->level;
    af.duration = number_range(3, 4);
    af.where = TO_AFFECTS;
    af.bitvector = 0;

    if (ch->class == class_lookup("monk") && is_empowered_quiet(ch,1)){
      af.duration += 5;
      monk_def = TRUE;
    }

    /* add the effects */
    if (monk_def){
      af.location = APPLY_SAVING_SPELL;
      af.modifier = -1 * number_range(3, 6);
    }
    else{
      af.location = APPLY_DAMROLL;
      af.modifier = 1 + (skill - 75) / 6;
    }
    affect_to_char(ch,&af);
    if (monk_def){
      af.location = APPLY_HITROLL;
      af.modifier = 1 + (skill - 75) / 8;
    }
    else{
      af.location = APPLY_HITROLL;
      af.modifier = 1 + (skill - 75) / 6;
    }
    affect_to_char(ch,&af);

    ch->mana -= cost;
    /* subtract cp's */
    handle_skill_cost_percent( ch, ch->pCabal, gsn_stance, 100 );

    act("You drop into a combat stance.",ch,NULL,NULL,TO_CHAR);
    act("$n drops into a combat stance.",ch,NULL,NULL,TO_ROOM);
    check_improve(ch,gsn_stance,TRUE,3);
  }
  else{
    ch->mana -= cost / 2;
    /* subtract cp's */
    handle_skill_cost_percent( ch, ch->pCabal, gsn_stance, 50);
    act("Your fail to assume the correct stance.",ch,NULL,NULL,TO_CHAR);
    act("$n's tries to assume a combat stance, but fails.",ch,NULL,NULL,TO_ROOM);
    check_improve(ch,gsn_stance,FALSE,3);
  }
}

/* this is a sort of fake duplication of things in one_hit */
int get_wepdam( OBJ_DATA* wield, CHAR_DATA* ch, CHAR_DATA* vch, int dt, int* dam_t ){
  AFFECT_DATA* paf;
  int dam = GET_DAMROLL(ch);
  int diceroll, dam_type;


  if (wield)
    dam += dice(wield->value[1], wield->value[2]);
  else
    dam = dice(1 + (ch->level / 8), 6);

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

//ENHANCED DAMAGE
  if ( get_skill(ch, gsn_enhanced_damage) > 0 && !vamp_check(ch)){
    diceroll = number_percent();
    diceroll -= 2*(get_curr_stat(ch,STAT_LUCK) - 16);
    if (diceroll <= get_skill(ch,gsn_enhanced_damage)){
      check_improve(ch,gsn_enhanced_damage,TRUE,0);
      dam += (dam * URANGE(20,diceroll,80)) /100;

      /* Extra race bonus */
      if (ch->race == race_lookup("ogre") )
	dam += (dam * URANGE(0,diceroll,20)) /100;
      else if ( ch->race == race_lookup("fire") && wield
		&& (dam_type == DAM_FIRE || !str_cmp("fire", wield->material)) )
	dam += (dam * URANGE(0,diceroll,25)) /100;
    }
  }
//DOUBLE GRIP /2handed (not staffs )
  if  (is_affected(ch,gsn_double_grip)
       && get_skill(ch,gsn_double_grip) > 0)
    {
      diceroll = number_percent();
      diceroll -= (get_curr_stat(ch,STAT_LUCK) - 16);
      if (diceroll <= get_skill(ch,gsn_double_grip)){
	dam += (dam * number_range(15, 30)) /100;
	check_improve(ch, gsn_double_grip, TRUE, 1);
      }
    }
  else if ( wield
	    && (IS_WEAPON_STAT(wield, WEAPON_TWO_HANDS) || is_affected(ch, gsn_double_grip))
	    && wield->value[0] != WEAPON_STAFF)
    dam += (dam * number_range(5, 15)) /100;


  //ANATOMY
  dam += dam * check_anatomy(ch, vch) / 100;

  *dam_t = dam_type;
  return dam;
}

/* FORM FUNCTIONS USED IN DO_FORMS */
void form_sword( OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, bool fAutoFail){
  int dam;
  int dt = obj ? obj->value[3] : gsn_martial_arts_a;
  int dam_type;

  if (obj == NULL)
    return;
  else if (fAutoFail)
    dam = 0;
  else{
    dam = get_wepdam( obj, ch, vch, dt + TYPE_HIT, &dam_type);
    dam = 130 * dam / 100;
//    sendf(ch, "dam: %d\n\r", dam);
  }
  damage(ch, vch, dam, dt + TYPE_NOBLOCKHIT, dam_type, TRUE );
}

void form_mace( OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, bool fAutoFail){
  CHAR_DATA* wch, *wch_next;
  int dam;
  int dt = obj ? obj->value[3] : gsn_martial_arts_a;
  int dam_type;

  if (obj == NULL)
    return;
  else if (fAutoFail)
    dam = 0;

  for (wch = vch->in_room->people; wch; wch = wch_next ){
    wch_next = wch->next_in_room;
    if ((!is_same_group(wch, vch) || is_area_safe_quiet(ch, wch))
	&& (wch->fighting == NULL || !is_same_group(wch->fighting, ch))
	)
      continue;
    if (!fAutoFail){
      dam = get_wepdam( obj, ch, wch, dt + TYPE_NOBLOCKHIT, &dam_type);
      if (!IS_NPC(wch)
	  && get_lagprot( wch ) == LAG_SHIELD_NONE
	  && !saves_spell( ch->level, wch, DAM_BASH, SPELL_AFFLICTIVE )){
	send_to_char("You've been stunned by the shockwave!\n\r", wch);
	act("$N is stunned by the shockwave.", ch, NULL, wch, TO_CHAR);
	WAIT_STATE2(wch, 2 * PULSE_VIOLENCE);
      }
    }
    else
      dam = 0;
    damage(ch, wch, dam, dt + TYPE_NOBLOCKHIT, dam_type, TRUE );
  }
  return;
}
void form_dagger( OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, bool fAutoFail){
  OBJ_DATA* wield;
  int dam;
  int dt = obj ? obj->value[3] : gsn_martial_arts_a;
  int dam_type;

  if (obj == NULL)
    return;
  else if (fAutoFail)
    dam = 0;
  else{
    int ful_dam = get_wepdam( obj, ch, vch, dt + TYPE_HIT, &dam_type);

    //first hit
    dam = 10 * ful_dam / 100;
    damage(ch, vch, dam, dt + TYPE_NOBLOCKHIT, dam_type, TRUE );
    if (ch->fighting == NULL)
      return;
    //second hit
    dam = 20 * ful_dam / 100;
    damage(ch, vch, dam, dt + TYPE_NOBLOCKHIT, dam_type, TRUE );
    if (ch->fighting == NULL)
      return;
    //disarm
    switch(number_range(0, 5)){
    case 0:
    case 1:
      if ( (wield = get_eq_char( vch, WEAR_WIELD )) != NULL)
	disarm(ch, vch);
      break;
    case 2:
    case 3:
      if ( (wield = get_eq_char( vch, WEAR_SECONDARY )) != NULL )
	disarm_offhand(ch, vch);
      break;
    default:
      if ( (wield = get_eq_char( vch, WEAR_SHIELD )) != NULL )
	shield_disarm(ch, vch);
      break;
    }
    if (ch->fighting == NULL)
      return;
    //third
    dam = 30 * ful_dam / 100;
    damage(ch, vch, dam, dt + TYPE_NOBLOCKHIT, dam_type, TRUE );
    if (ch->fighting == NULL)
      return;
    //kick
    kick(ch, vch, number_range(50, 100));
    //fourth
    dam = 40 * ful_dam / 100;
    damage(ch, vch, dam, dt + TYPE_NOBLOCKHIT, dam_type, TRUE );
    if (ch->fighting == NULL)
      return;
    if (vch->mana > 0){
      send_to_char("Dazed by the attacks you lose some mana.\n\r", vch);
      act("Dazed by the attacks, $n loses some mana.", vch, NULL,NULL, TO_ROOM);
    }
    vch->mana -= number_range(35, 70);
    if (vch->mana < 0)
      vch->mana = 0;
  }
}

void form_axe( OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, bool fAutoFail){
  AFFECT_DATA af, *paf;
  int dam;
  int dt = obj ? obj->value[3] : gsn_martial_arts_a;
  int dam_type;

  if (obj == NULL)
    return;
  else if (fAutoFail)
    dam = 0;
  else{
    dam = get_wepdam( obj, ch, vch, dt, &dam_type);
    dam = 75 * dam / 100;
  }

  if ( (paf = affect_find(vch->affected, gsn_scourge)) == NULL
       || paf->modifier < 150){
    af.type	= gsn_scourge;
    af.level	= ch->level;
    af.duration	= number_range(2, 4);

    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location	= APPLY_AC;
    af.modifier	= number_range(10, 25);
    affect_join(vch, &af);
  }
  dt = gsn_apierce;
  damage(ch, vch, dam, dt, dam_type, TRUE );
}

void form_staff( OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, bool fAutoFail ){
  int dam;
  int dt = obj ? obj->value[3] : gsn_martial_arts_a;
  int dam_type;
  int hits = 0;		//twohits make for poison/paralyze on second

  if (obj == NULL)
    return;
  else if (fAutoFail)
    dam = 0;
  else{
    dam = get_wepdam( obj, ch, vch, dt, &dam_type);
    dam = 40 * dam / 100;
  }
  if( damage(ch, vch, dam, dt + TYPE_HIT, dam_type, TRUE ) > 0)
    hits ++;
  if (ch->fighting == NULL)
    return;

  if (fAutoFail)
    dam = 0;
  else{
    dam = get_wepdam( obj, ch, vch, dt, &dam_type);
    dam = 40 * dam / 100;
  }
  if( damage(ch, vch, dam, dt + TYPE_HIT, dam_type, TRUE ) > 0)
    hits ++;

  if (hits > 0)
    spell_poison(skill_lookup("poison"), ch->level - 3, ch, vch, TARGET_CHAR);
  if (hits > 1){
    paralyze_effect(vch, ch->level - 3,  dam,  TARGET_CHAR);
  }
}

void form_spear( OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, int chance ){
  int dam;
  int dt = obj ? obj->value[3] : gsn_martial_arts_a;
  int dam_type;
  int attacks = 3 + (get_curr_stat(ch,STAT_LUCK) - 12) / 5;
  int hits = 0, i;
  int total = 0; //(debug)
  if (obj == NULL)
    return;
  for (i = 0; i  < attacks; i++){
    if (number_percent() > chance)
      dam = 0;
    else{
      dam = get_wepdam( obj, ch, vch, dt, &dam_type);
      dam = (5 + 13 * hits) * dam / 100;
      total+= dam;
    }

    if (damage(ch, vch, dam, dt + TYPE_HIT, dam_type, TRUE ) > 1){
      if (ch->fighting == NULL)
	break;
      if (hits < 4)
	hits++;
      if (hits >= 3 && !IS_AFFECTED(vch, AFF_BLIND))
	spell_blindness( gsn_blindness, ch->level - 3, ch, vch, 0);
    }
    if (ch->fighting == NULL)
      break;
  }
//  sendf(ch, "total:%d\n\r", total);
}

void form_flail( OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, bool fAutoFail){
  int dam;
  int dt = attack_lookup("smash");
  int dam_type;

  if (obj == NULL)
    return;
  else if (fAutoFail)
    dam = 0;
  else{
    dam = get_wepdam( obj, ch, vch, dt, &dam_type);
    dam = 115 * dam / 100;
  }
  dam = damage(ch, vch, dam,  dt + TYPE_HIT, dam_type, TRUE );
  if (ch->fighting && dam && !IS_AFFECTED(vch, AFF_CURSE))
    spell_curse( gsn_curse, ch->level - 3, ch, vch, TARGET_CHAR);

}

void form_pole( OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, bool fAutoFail){
  int dam;
  int dt = obj ? obj->value[3] : gsn_martial_arts_a;
  int dam_type;

  if (obj == NULL)
    return;
  else if (fAutoFail)
    dam = 0;
  else{
    dam = get_wepdam( obj, ch, vch, dt + TYPE_HIT, &dam_type);
    dam = 175 * dam / 100;
  }
  damage(ch, vch, dam, dt + TYPE_HIT, dam_type, TRUE );
}

void form_whip( OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, bool fAutoFail){
  AFFECT_DATA af;
  int dam;
  int dt = obj ? obj->value[3] : gsn_martial_arts_a;
  int dam_type;

  if (obj == NULL)
    return;
  else if (fAutoFail)
    dam = 0;
  else{
    dam = get_wepdam( obj, ch, vch, dt, &dam_type);
  }

  if (damage(ch, vch, dam, dt + TYPE_HIT, dam_type, TRUE ) > 1){
    if (ch->fighting == NULL)
      return;

    if (is_affected(ch, gsn_forms) && number_percent() < 50)
      return;
    if (number_percent() < 50)
      return;
    act("Your weapon has been pulled aside!", ch, NULL, vch, TO_VICT);
    act("$n's weapon has been pulled aside!", vch, NULL, NULL,TO_ROOM);

    af.type		= gsn_rake;
    af.level		= ch->level;
    af.duration		= 1;
    af.where		= TO_AFFECTS;
    af.bitvector	= 0;
    af.location		= APPLY_NONE;
    af.modifier		= 1;

    affect_to_char(vch ,&af);
  }
}

void form_exotic(OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, bool fAutoFail){
  AFFECT_DATA af, *paf;
  int dam;
  int dt = obj ? obj->value[3] : gsn_martial_arts_a;
  int dam_type;

  if (obj == NULL)
    return;
  else if (fAutoFail)
    dam = 0;
  else{
    dam = 50 * get_wepdam( obj, ch, vch, dt, &dam_type) / 100;
  }

  if (damage(ch, vch, dam, dt + TYPE_HIT, DAM_INTERNAL, TRUE ) > 1){
    if (ch->fighting == NULL)
      return;

    if (number_percent() < 50)
      return;

    if ( (paf = affect_find(vch->affected, gen_bleed)) == NULL){
      af.type = gen_bleed;
      af.level = ch->level;
      af.duration = number_range(2, 3);
      af.where = TO_NONE;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      /* modifier controls duration of damage (10/tick)*/
      af.modifier = number_range(8, 15);
      paf = affect_to_char(vch, &af);
      string_to_affect(paf, ch->name);
      act("A spray of blood shoots forth from $n's belly!", vch, NULL, NULL, TO_ROOM);
      act("A spray of blood shoots forth from your stomach!", vch, NULL, NULL, TO_CHAR);
    }
    else{
      act("You send some more blood flying from $n's wound!", vch, NULL, NULL, TO_ROOM);
      /* modifier controls duration of damage (10/tick)*/
      paf->modifier = number_range(10, 25);
      string_to_affect(paf, ch->name);
    }
  }
}

void form_hth( OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, bool fAutoFail){
  AFFECT_DATA af;

  if (fAutoFail){
    send_to_char("You've failed to achive the proper focus.\n\r", ch);
    return;
  }

  if (!is_affected(ch, gsn_enhanced_damage)){
    af.type	= gsn_enhanced_damage;
    af.level	= ch->level;
    af.duration	= 0;

    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location	= APPLY_NONE;
    af.modifier	= 0;
    affect_join(ch, &af);
    act("You begin to bypass $N's inherited resistances.", ch, NULL, vch, TO_CHAR);
  }
}
void form_hth2( OBJ_DATA* obj, CHAR_DATA* ch, CHAR_DATA* vch, bool fAutoFail){
  AFFECT_DATA af;
  int sn = gsn_invigorate;

  if (fAutoFail){
    send_to_char("You've failed to achive the proper focus.\n\r", ch);
    return;
  }
  if (!is_affected(ch, sn)){
    af.type	= gsn_invigorate;
    af.level	= ch->level;
    af.duration	= 0;

    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location	= APPLY_NONE;
    af.modifier	= 0;
    affect_join(ch, &af);
    act("Your chakera strikes magnify in power.", ch, NULL, vch, TO_CHAR);
  }
}


/* used above and for automatic forms */
bool forms(CHAR_DATA *ch, char *argument, AFFECT_DATA* paf){
  CHAR_DATA* vch;
  OBJ_DATA* wield;

  int skill = get_skill(ch, gsn_forms );

  int wep_type = -1;	//-1 means hth
  int chance;
  bool fAdv = get_skill(ch, skill_lookup("form mastery")) > 2;
  bool fAutoFail = FALSE;

  if (skill < 1){
    if (!paf)
      send_to_char("Huh?\n\r", ch);
    return FALSE;
  }

  /* get type of form we are doing */
  if (fAdv && !IS_NULLSTR(argument)){
    wield = get_eq_char(ch, WEAR_SECONDARY);
  }
  else
    wield = get_eq_char(ch, WEAR_WIELD);

  if (wield)
    wep_type = wield->value[0];

  if (ch->fighting == NULL){
    if (!paf)
      send_to_char("But you're not fighting!\n\r", ch);
    return FALSE;
  }
  else if ( (vch = get_group_room( ch, TRUE)) != NULL){
    if (!paf)
      act("$N gets in your way.", ch, NULL, vch, TO_CHAR);
    return FALSE;
  }
  else if (wep_type == -1 && ch->class != class_lookup("monk")){
    if (!paf)
      send_to_char("You lack the perfection of technique to attempt this form.\n\r", ch);
    return FALSE;
  }
  else if (get_weapon_skill_obj(ch, wield) < 100){
    if (!paf)
      sendf(ch, "You must be a master of %ss before attempting this form.\n\r",
	  wield ? weapon_name(wep_type) : "fist");
    return FALSE;
  }
  vch = ch->fighting;

  chance = 4 * skill / 5;
  chance += (get_curr_stat(ch, STAT_DEX) - 20) * 3;

  if (vch){
    chance += (ch->level - vch->level) * 3;
    chance += affect_by_size(ch, vch) * 2;
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(vch,STAT_LUCK))* 2;
  }

  if (number_percent() > chance)
    fAutoFail = TRUE;


  /* begin doing the forms */
  switch (wep_type){
/* SWORD */
  case WEAPON_SWORD:
    send_to_char("You execute the 'Rising Sun'.\n\r", ch);
    act("$n executes the 'Rising Sun'.", ch, NULL, vch, TO_ROOM);
    form_sword(wield, ch, vch, fAutoFail );
    break;
/* MACE */
  case WEAPON_MACE:
    send_to_char("You execute the 'Armaggedon Smash'.\n\r", ch);
    act("$n executes the 'Armaggedon Smash'.", ch, NULL, vch, TO_ROOM);
    form_mace(wield, ch, vch, fAutoFail );
    break;
/* DAGGER */
  case WEAPON_DAGGER:
    send_to_char("You execute 'Steel Storm'.\n\r", ch);
    act("$n executes 'Steel Storm'.", ch, NULL, vch, TO_ROOM);
    form_dagger(wield, ch, vch, fAutoFail );
    break;
/* AXE */
  case WEAPON_AXE:
    send_to_char("You execute the 'Troll Scourge'.\n\r", ch);
    act("$n executes the 'Troll Scourge'.", ch, NULL, vch, TO_ROOM);
    form_axe(wield, ch, vch, fAutoFail );
    break;
/* STAFF */
  case WEAPON_STAFF:
    send_to_char("You execute the 'Twin Cobra'.\n\r", ch);
    act("$n executes the 'Twin Cobra'.", ch, NULL, vch, TO_ROOM);
    form_staff(wield, ch, vch, fAutoFail );
    break;
/* SPEAR */
  case WEAPON_SPEAR:
    send_to_char("You execute the 'Raven Swarm'.\n\r", ch);
    act("$n executes the 'Raven Swarm'.", ch, NULL, vch, TO_ROOM);
    form_spear(wield, ch, vch, chance );
    break;
/* FLAIL */
  case WEAPON_FLAIL:
    act("You execute the '$g's Hand'.", ch, NULL, NULL, TO_CHAR);
    act("$n executes the '$g's Hand'.", ch, NULL, vch, TO_ROOM);
    form_flail(wield, ch, vch, fAutoFail );
    break;
/* WHIP */
  case WEAPON_WHIP:
    send_to_char("You execute 'The Karakken'.\n\r", ch);
    act("$n executes 'The Karakken'.", ch, NULL, vch, TO_ROOM);
    form_whip(wield, ch, vch, fAutoFail );
    break;
/* POLE */
  case WEAPON_POLEARM:
    send_to_char("You execute the 'Dragon Tail'.\n\r", ch);
    act("$n executes the 'Dragon Tail'.", ch, NULL, vch, TO_ROOM);
    form_pole(wield, ch, vch, fAutoFail );
    break;
/* EXO */
  case WEAPON_EXOTIC:
    send_to_char("You execute the 'Bowel Raker'.\n\r", ch);
    act("$n executes the 'Bowel Raker'.", ch, NULL, vch, TO_ROOM);
    form_exotic(wield, ch, vch, fAutoFail );
    break;
/* HTH */
  default:
    if (fAdv && !IS_NULLSTR(argument)){
      send_to_char("You execute the 'Atemi Strike'.\n\r", ch);
      act("$n executes the 'Atemi Strike'.", ch, NULL, vch, TO_ROOM);
      form_hth(wield, ch, vch, fAutoFail );
    }
    else{
      send_to_char("You execute the 'Eagle Eye'.\n\r", ch);
      act("$n executes the 'Eagle Eye'.", ch, NULL, vch, TO_ROOM);
      form_hth2(wield, ch, vch, fAutoFail );
    }
    break;
  }
  return TRUE;
}

/* GLADIATOR FORMS */
void do_forms(CHAR_DATA *ch, char *argument){
  AFFECT_DATA af, *paf;
  int cost = skill_table[gsn_forms].min_mana;
  int lag = skill_table[gsn_forms].beats;
  int skill = get_skill(ch, gsn_forms );

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return ;
  }
  else if (ch->fighting == NULL){
    send_to_char("But you're not fighting!\n\r", ch);
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You cannot muster up the energy.\n\r",ch);
    return;
  }
  else
    ch->mana -= cost;

  handle_skill_cost_percent( ch, ch->pCabal, gsn_forms, 100);
  WAIT_STATE2(ch, lag );

  affect_strip(ch, gsn_forms);
  if (!forms(ch, argument, NULL ))
    return;

  af.type = gsn_forms;
  af.level = ch->level;
  af.duration = number_range(0, 1);
  af.location = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  paf = affect_to_char( ch, &af );
  string_to_affect( paf, argument );
}

/* ***** JUSTICE POWERS ***** */


void spell_raise_morale( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn )){
    send_to_char("You are already raising morale of your Justice troops!\n\r", ch);
    return;
  }

  send_to_char("You begin to activly raise morale of your Justice troops.\n\r", ch);

  af.type	= sn;
  af.level	= level;
  af.duration	= 24;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;

  af.location	= APPLY_HITROLL;
  af.modifier	= number_fuzzy(level / 7);
  affect_to_char( victim, &af);

  af.location	= APPLY_DAMROLL;
  af.modifier	= number_fuzzy(level / 8);
  affect_to_char( victim, &af);

  af.location	= APPLY_MOVE_GAIN;
  af.modifier	= UMAX(0, 20 + (level - 30) * 2);
  affect_to_char( victim, &af);
}

void do_judge(CHAR_DATA *ch, char *argument){
  char arg[MIL];
  CHAR_DATA *victim;
  int counter;
  int max_pen;
  one_argument(argument, arg);
  if (get_skill(ch,skill_lookup("judge")) == 0)
    {
      send_to_char("Huh?\n\r",ch);
      return;
    }
  if ( arg[0] == '\0' )
    {
      send_to_char("Judge whom?\n\r", ch);
      return;
    }
  if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
  if (IS_NPC(victim))
    {
      send_to_char("Mobs don't care for the laws sentients set down.\n\r", ch);
        return;
    }
/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, gsn_judge );

  if ( victim->pcdata->ethos == CH_ETHOS_LAWFUL)
    sendf(ch,"%s upholds the law.\n\r",victim->name);
  else if ( victim->pcdata->ethos == CH_ETHOS_NEUTRAL)
    sendf(ch,"%s is ambivalent to the law.\n\r",victim->name);
  else if ( victim->pcdata->ethos == CH_ETHOS_CHAOTIC)
    sendf(ch,"%s does not believe in laws.\n\r",victim->name);
  else
    send_to_char("There's something wrong.  Notify an immortal.\n\r", ch);
  counter = victim->pcdata->flagged;
  if (counter == 0)
    sendf(ch,"%s is a model citizen.\n\r",victim->name);
  else if (counter < 3)
    sendf(ch,"%s does a good job observing the laws.\n\r",victim->name);
  else if (counter < 5)
    sendf(ch,"%s has had %s share of brush-ins with the law.\n\r",victim->name, (victim->sex == 2) ? "her" : "his");
  else if (counter < 10)
    sendf(ch,"%s is a common criminal.\n\r",victim->name);
  else
    sendf(ch,"%s is a die-hard criminal.\n\r",victim->name);

  max_pen = show_crimes( ch, victim, TRUE, FALSE );
  switch ( max_pen ){
  case CRIME_LIGHT:
    send_to_char( "The penalties allowed under the law are: Normal, Bail.\n\r", ch );	break;
  case CRIME_NORMAL:
    send_to_char( "The penalties allowed under the law are: Normal, Bail, Extended.\n\r", ch );	break;
  case CRIME_HEAVY:
    send_to_char( "The penalties allowed under the law are: Normal, Bail, Extended, Death\n\r", ch ); break;
  case CRIME_DEATH:
    send_to_char( "The penalties allowed under the law are: Death\n\r", ch ); break;
  }
}

void do_wanted( CHAR_DATA *ch, char *argument ){

  char arg1[MIL], arg2[MIL],buf[MSL];
  CHAR_DATA *victim;
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if (get_skill(ch,skill_lookup("wanted")) < 2){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if ( arg1[0] == '\0' || arg2[0] == '\0' ){
    send_to_char( "Syntax: wanted <character> <y/n>.\n\r", ch );
    return;
  }
  if ( ( victim = get_char( arg1 ) ) == NULL ){
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if (victim->level >= LEVEL_IMMORTAL && (ch->level < victim->level)){
    sendf(ch, "You do not have the power to arrest %s.\n\r", PERS2(victim));
    return;
  }
  if ( IS_NPC(victim) ){
    send_to_char( "You cannot arrest mobs!.n\r", ch );
    return;
  }
  if (ch == victim){
    send_to_char("You cannot do that.\n\r", ch);
    return;
  }
  if (is_affected(victim, gen_jail) ||
      (IS_SET(victim->in_room->room_flags2, ROOM_JAILCELL)
       && jail_check(victim->in_room)) ){
    send_to_char("Not on prisoners.\n\r", ch);
    return;
  }
  if ( !str_cmp( arg2, "y" ) ){
    AFFECT_DATA* paf;
    AREA_DATA* pArea;
    bool fFound = FALSE;

    /* check if the victim is fighting a JUSTICE mob in lawful area */
    if (victim->fighting
	&& IS_NPC(victim->fighting) && victim->fighting->pCabal
	&& IS_CABAL(victim->fighting->pCabal, CABAL_JUSTICE)
	&& IS_AREA(ch->in_room->area, AREA_LAWFUL)){
      AFFECT_DATA af;
      set_crime(victim, victim->fighting,  ch->in_room->area, CRIME_OBSTRUCT);
      af.type		= gsn_timer;
      af.level		= 60;
      af.duration	= 1;
      af.where		= TO_NONE;
      af.bitvector	= 0;
      af.location	= APPLY_NONE;
      af.modifier	= 0;
      affect_to_char(victim, &af );
    }

/* check if there are any crimes to flag for, if there are, move crimes from level to bitvector */
    for (paf = victim->affected; paf; paf = paf->next ){
      if (paf->type == gsn_wanted && paf->modifier){
	paf->bitvector |= paf->modifier;
	paf->modifier = 0;
	fFound = TRUE;
	/* crime found/wanted we increase the relations but need area first */
	if ( (pArea = get_area_data( paf->location )) == NULL){
	  bug("do_wanted: could not get area for vnum %d.", paf->location);
	  continue;
	}
	else if (pArea->pCabal)
	  affect_justice_relations(pArea->pCabal, 2);
      }
    }
/* subtract cp's */
    handle_skill_cost( ch, ch->pCabal, gsn_wanted );

/* check if the wanted command was deserved */
    if (!fFound){
/* Penalty for wrongful accusation */
      char buf[MIL];
/* safety for multiple justices flagging at same time */
      if (is_affected(victim, gsn_timer)){
	send_to_char("This person has been recently marked wanted.\n\r", ch);
	return;
      }
      sprintf( buf, "%s has attempted to mark %s WANTED without due evidence!", PERS2(ch), PERS2(victim));
      cabal_echo_flag( CABAL_JUSTICE, buf );
      cp_event(ch, victim, NULL, CP_EVENT_WANTED_FALSE );
      /* if person is out of cps we demote them */
      if (GET_CP( ch ) < 1){
	if ( ch->pcdata->rank > RANK_NEWBIE ){
	  CABAL_DATA* pParent = get_parent( ch->pCabal );
	  char sub[MIL];
	  sprintf( buf, "Due to lack of care when considering evidence of crimes, %s has been demoted.", PERS2(ch) );
	  cabal_echo( ch->pCabal, buf );
	  strcat( buf, "\n\r");
	  sprintf( sub, "%s's demotion.", PERS2(ch));
	  do_hal( pParent->name, sub, buf, NOTE_NEWS );
	  promote( ch, ch->pCabal, -1 );
	  if (ch->pcdata->member)
	    ch->pcdata->member->time_stamp = mud_data.current_time;
	}
	return;
      }
    }
    else if ( !IS_SET(victim->act, PLR_WANTED) ){
      AFFECT_DATA af;
      SET_BIT( victim->act, PLR_WANTED );
      victim->pcdata->flagged ++;
      sendf(ch, "%s is now a criminal.\n\r", PERS2(victim) );
      send_to_char( "You are now WANTED!\n\r", victim);
      sprintf(buf,"NOTICE: %s has been marked a criminal by %s.",victim->name, ch->name);
      cabal_echo_flag( CABAL_JUSTICE, buf );
      cp_event(ch, victim, NULL, CP_EVENT_WANTED );

      af.type		= gsn_timer;
      af.level		= 60;
      af.duration	= 1;
      af.where		= TO_NONE;
      af.bitvector	= 0;
      af.location	= APPLY_NONE;
      af.modifier	= 0;
      affect_to_char(victim, &af );
      return;
    }
    else{
      if (is_affected(victim, gsn_timer)){
	send_to_char("This person has been recently marked wanted.\n\r", ch);
	return;
      }
      sendf(ch, "You record %s's new offense against the Law.\n\r", PERS2(victim) );
      sendf(victim, "%s has marked you for another transgression against the Law!\n\r", PERS2(ch));
      victim->pcdata->flagged++;
      sprintf(buf,"NOTICE: %s has been marked for another offense by %s.",victim->name, ch->name);
      cabal_echo_flag( CABAL_JUSTICE, buf );
    }
    return;
  }
  if ( !str_cmp( arg2, "n" ) ){
    if (IS_OUTLAW(victim)){
      send_to_char("Not on outlaws.\n\r", ch);
      return;
    }
    if ( IS_SET(victim->act, PLR_WANTED) ){
      REMOVE_BIT( victim->act, PLR_WANTED );
      victim->pcdata->flagged -= 1;
      sendf(ch, "%s is no longer a criminal.\n\r", PERS2(victim) );
      send_to_char( "You are no longer WANTED!\n\r", victim );
      sprintf(buf,"NOTICE: %s has been removed as a criminal by %s.",victim->name, ch->name);
      cabal_echo_flag( CABAL_JUSTICE, buf );
      affect_strip( victim, gsn_wanted );
    }
    else
      sendf(ch, "%s isn't a criminal.\n\r", PERS2(victim));
    return;
  }
  send_to_char( "Syntax: wanted <character> <y/n>.\n\r", ch );
}

/* grants detect invis */
void spell_third_eye( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if ( IS_AFFECTED(victim, AFF_DETECT_INVIS) )
    {
        if (victim == ch)
            act("You can already see invisible objects.",ch,NULL,victim,TO_CHAR);
        else
            act("$N can already see invisible things.",ch,NULL,victim,TO_CHAR);
        return;
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level / 2;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_DETECT_INVIS;
    affect_to_char( victim, &af );
    act_new( "You become very sensitive to things unseen.", ch,NULL,victim,TO_CHARVICT,POS_DEAD );
    if ( ch != victim )
      act("$N becomes very sensitive to things unseen.",ch,NULL,victim,TO_CHAR);
}

/* builds a justice watchtower */
void do_watchtower( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af;
  int sn = skill_lookup("watchtower");
  int cost = skill_table[sn].min_mana;
  int lag = skill_table[sn].beats;

  if (get_skill(ch, sn) < 2){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  else if (!ch->in_room || !IS_AREA(ch->in_room->area, AREA_LAWFUL)){
    send_to_char("You may only order construction of watchtowers in lawful areas.\n\r", ch);
    return;
  }
  else if (!IS_OUTSIDE(ch)){
    send_to_char("You cannot build a watchtower indoors!\n\r", ch);
    return;
  }
  else if (IS_AFFECTED2(ch, AFF_SILENCE)){
    send_to_char("As you try to give the order, no sound comes out!\n\r",ch);
    return;
  }
  if (is_affected(ch, gsn_gag)){
    send_to_char("You try to work your mouth but the gag holds fast.\n\r", ch);
    return;
  }
  if (is_affected(ch,gsn_uppercut)){
    send_to_char("Your jaws hurts too much to move.\n\r",ch);
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You are too tired to order your guards about effectivly.\n\r", ch );
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2( ch, lag );

/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, sn );

/* start the gen */
  af.type	= gen_watchtower;
  af.level	= ch->level;
  af.duration	= -1;

  af.where	= TO_AFFECTS;
  af.bitvector	= AFF_SANCTUARY;
  af.location	= APPLY_NONE;
  af.modifier	= 0;

  affect_to_char(ch, &af );

}

/* rallies justice troops in a city */
void do_rally_justice( CHAR_DATA *ch, char *argument ){
  char * const his_her [] = { "its", "his", "her" };
  AFFECT_DATA af;
  char buf[MIL];
  int sn = skill_lookup("rally call");
  int cost = skill_table[sn].min_mana;
  int lag = skill_table[sn].beats;

  if (get_skill(ch, sn) < 2){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  else if (!ch->in_room || !IS_AREA(ch->in_room->area, AREA_LAWFUL)){
    send_to_char("You may only rally Justice troops in lawful areas.\n\r", ch);
    return;
  }
  else if (IS_AFFECTED2(ch, AFF_SILENCE)){
    send_to_char("As you let forth a mighty call, no sound comes out!\n\r",ch);
    return;
  }
  else if (is_affected(ch, gsn_gag)){
    send_to_char("You try to work your mouth but the gag holds fast.\n\r", ch);
    return;
  }
  else if (is_affected(ch,gsn_uppercut)){
    send_to_char("Your jaws hurts too much to move.\n\r",ch);
    return;
  }
  else if (is_affected(ch, sn )){
    send_to_char("You are not yet ready to rally your troops again.\n\r",ch );
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You are too tired to rally your troops.\n\r", ch );
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2( ch, lag );
  sprintf( buf, "The strong notes of a war horn pass through the area as %s rallies %s troops!",
	   PERS2( ch ), his_her[URANGE(0,ch->sex,2)]);
  do_mpzecho( ch, buf );

/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, sn );

  affect_strip( ch, gen_mob_gen );

/* start the gen */
  af.type	= gen_mob_gen;
  af.level	= MOB_VNUM_JUSTICE_PATROL;	//mob to be spawned
  af.duration	= 4;				//how many times to spawn the mobs

  af.where	= TO_NONE;
  af.bitvector	= IS_LEADER( ch ) ? 1 : 1;	//troops to be spawned per tick
  af.location	= ch->in_room->area->vnum;	//area to rally troops in
  af.modifier	= IS_LEADER( ch ) ? 6 : 3;	//max troops to be spawned

  affect_to_char(ch, &af );
/* attach wait period */
  af.type	= sn;
  af.level	= ch->level;
  af.duration	= 18;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= 0;
  af.modifier	= 0;
  affect_to_char(ch, &af );

}


void spell_shield_justice( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA *shield;
    AFFECT_DATA *paf;

    if (IS_ELDER(ch))
      shield = create_object( get_obj_index(OBJ_VNUM_JUSTICE_SHIELD2), level );
    else
      shield = create_object( get_obj_index(OBJ_VNUM_JUSTICE_SHIELD), level );
    shield->timer = level;
    paf = new_affect();
    paf->type       = sn;
    paf->level      = level;
    paf->duration   = -1;
    paf->location   = APPLY_HITROLL;
    paf->modifier   = level/6;
    paf->bitvector  = 0;
    paf->next       = shield->affected;
    shield->affected   = paf;
    paf = new_affect();
    paf->type       = sn;
    paf->level      = level;
    paf->duration   = -1;
    paf->location   = APPLY_DAMROLL;
    paf->modifier   = level/7;
    paf->bitvector  = 0;
    paf->next       = shield->affected;
    shield->affected   = paf;
    paf = new_affect();
    paf->type       = sn;
    paf->level      = level;
    paf->duration   = -1;
    paf->location   = APPLY_HIT;
    paf->modifier   = level;
    paf->bitvector  = 0;
    paf->next       = shield->affected;
    shield->affected   = paf;
    obj_to_char(shield, ch);
    act("You create $p!",ch,shield,NULL,TO_CHAR);
    act("$n creates $p!",ch,shield,NULL,TO_ROOM);
}

/* gets next step to a criminal */
void do_hunt( CHAR_DATA *ch, char *argument )
{
    extern char * const dir_name[];
    char arg[MSL];
    CHAR_DATA *victim;
    int direction, maxdist, roll;
    int skill = get_skill(ch, gsn_hunt);
    const int cost = skill_table[gsn_hunt].min_mana;

    one_argument( argument, arg );

/* Checks to see if the caster is a Hunter */
    if (skill < 2){
      send_to_char("Huh?\n\r", ch);
      return;
    }
    else if( arg[0] == '\0' )
    {
        send_to_char( "Whom are you trying to hunt?\n\r", ch );
        return;
    }
    victim = get_char( arg );
    if( victim == NULL )
    {
        send_to_char("No-one around by that name.\n\r", ch );
        return;
    }
    else if (IS_NPC(victim)){
      send_to_char("Why would you want to hunt a mob?\n\r", ch);
      return;
    }
    else if( ch->in_room == victim->in_room )
    {
        sendf(ch, "%s is here!", PERS2(victim));
        return;
    }
    else if (!IS_WANTED(victim)){
      send_to_char("You may only use this to track criminals.\n\r", ch);
      return;
    }
    else if (ch->mana < cost){
      send_to_char( "You can't concentrate sufficiently\n\r", ch );
      return;
    }
    ch->mana -= cost;
    WAIT_STATE2( ch, skill_table[gsn_hunt].beats );

/* subtract cp's */
    handle_skill_cost( ch, ch->pCabal, gsn_hunt );

    /* skill check */
    if (number_percent() > skill){
      send_to_char("You can't seem to pick up a trail.\n\r", ch);
      return;
    }

    /* defense check */
    if (victim->in_room->vnum == ROOM_VNUM_LIMBO
	|| IS_OUTLAW(victim)
	|| IS_SET(victim->in_room->room_flags, ROOM_NOWHERE)
	|| IS_SET(victim->in_room->room_flags, ROOM_NO_SCAN)
	|| is_affected(victim, gsn_coffin)
	|| is_affected(victim, gsn_burrow)
	|| is_affected(victim, gsn_catalepsy)){
      if (victim->in_room->vnum != ROOM_VNUM_LIMBO)
	send_to_char("You can barely sense the trail.\n\r", ch);
      maxdist = 15;
    }
    else
      maxdist = 512;

    roll = (number_range(1,100));
    if (roll < 33)
        act( "$n carefully examines the area.", ch, NULL, NULL, TO_ROOM );
    else if (roll < 66)
       act( "$n carefully checks the ground for tracks.", ch, NULL, NULL, TO_ROOM );
    else
        act( "$n listens carefully for some sounds.", ch, NULL, NULL, TO_ROOM );


    direction = find_first_step(ch->in_room, victim->in_room, maxdist, TRUE, NULL);
    switch(direction)
    {
    case BFS_ERROR:
        send_to_char("Hmm... something seems to be wrong.\n\r", ch);
        break;
    case BFS_ALREADY_THERE:
        send_to_char("You're already in the same room!\n\r", ch);
        break;
    case BFS_NO_PATH:
        send_to_char("You can't sense a trail from here.\n\r", ch);
        check_improve(ch,gsn_hunt,FALSE,0);
        break;
    default:
        if( ( IS_NPC (ch) && number_percent () > 75) || (!IS_NPC (ch) && number_percent () > get_skill(ch,gsn_hunt) )
        || IS_AFFECTED2(ch,AFF_MISDIRECTION))
        {
            do
            {
                direction = number_door();
            }
            while( ( ch->in_room->exit[direction] == NULL ) || ( ch->in_room->exit[direction]->to_room == NULL) );
       }
        sendf( ch, "%s is %s from here.\n\r", PERS2(victim), dir_name[direction] );
        check_improve(ch,gsn_hunt,TRUE,0);
        break;
    }
}

/* throws a bolo, causes difficult movement or delayed sleep at higher skill */
void do_bolo( CHAR_DATA* ch, char* argument ){
  CHAR_DATA* victim;
  int skill = get_skill( ch, gsn_bolo );
  int chance = skill;
  int neck_chance = UMAX(0, chance - 85) * 3 + 20;

  bool fForce = FALSE;
  bool fAttack = FALSE;

  const int cost = skill_table[gsn_bolo].min_mana;
  const int lag = skill_table[gsn_bolo].beats;

  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  /* get the target */
  if (IS_NULLSTR( argument)){
    if (ch->fighting)
      victim = ch->fighting;
    /* check if a pet wolfhound exists (justice mob) and take its target */
    else if (ch->pet && ch->pet->pIndexData->vnum == MOB_VNUM_HOUND
	     && ch->pet->fighting
	     && ch->pet->in_room
	     && ch->pet->in_room == ch->in_room ){
      victim = ch->pet->fighting;
      if (IS_WANTED( victim) && is_safe_quiet( ch, victim ))
	fForce = TRUE;
    }
    else{
      send_to_char("You are not fighting anyone.\n\r", ch);
      return;
    }
  }
  else if ( (victim = get_char_room(ch, NULL, argument )) == NULL){
    send_to_char("They aren't here.\n\r", ch );
    return;
  }
  else if (!fForce && is_safe(ch, victim ))
    return;

  /* misc. checks */
  if (!IS_AWAKE(victim) || is_affected(victim, gsn_ecstacy )){
    send_to_char("What would be the point of that?\n\r", ch);
    return;
  }
  else if (ch == victim){
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You cannot concentrate sufficiently to aim.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2( ch, lag );

/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, gsn_hunt );

  /* modify chance slightly */
  chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX)) * 3;
  chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);

  /* safety */
  if (IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG) )
    chance = 0;

/* do the yells */
  if ( !fForce && ch->fighting != victim && victim->fighting != ch){
    char buf[MIL];
    a_yell(ch,victim);
    sprintf(buf, "Help! %s just threw a bolo at me!",PERS(ch,victim));
    j_yell(victim,buf);
    fAttack = TRUE;
  }

  /* check if skill high enugh to throw at the neck */
  if (skill > 84 && number_percent() < neck_chance){
    if (number_percent() > chance
	|| saves_spell( victim->level - 3, victim, DAM_OTHER ,skill_table[gsn_bolo].spell_type) ){
      dam_message( ch, victim, NULL, 0, gsn_bolo, FALSE, FALSE, gsn_bolo, TRUE );
    }
    else{
      AFFECT_DATA af;

      act("You throw a bolo at $N's neck.", ch, NULL, victim, TO_CHAR);
      act("$n throws a bolo at $N's neck.", ch, NULL, victim, TO_NOTVICT);
      act("$n throws a bolo at your neck.", ch, NULL, victim, TO_VICT);
      dam_message( ch, victim, NULL, 5, gsn_bolo, FALSE, FALSE, gsn_bolo, TRUE );

      if (!is_affected(victim, gen_suffocate )){
	af.type		= gen_suffocate;
	af.level	= ch->level;
	af.duration	= 4;		//how long before person falls asleep for 1 tick

	af.where	= TO_AFFECTS;
	af.bitvector	= 0;
	af.location	= APPLY_NONE;
	af.modifier	= FALSE;	//NO DAMAGE
	affect_to_char( victim, &af );
      }
    }
  }
/* hit the legs */
  else{
    if (number_percent() > chance ){
      dam_message( ch, victim, NULL, 0, gsn_bolo, FALSE, FALSE, gsn_bolo, TRUE );
    }
    else{
      AFFECT_DATA af;
      act("You throw a bolo at $N's legs.", ch, NULL, victim, TO_CHAR);
      act("$n throws a bolo at $N's legs.", ch, NULL, victim, TO_NOTVICT);
      act("$n throws a bolo at your legs.", ch, NULL, victim, TO_VICT);
      dam_message( ch, victim, NULL, 5, gsn_bolo, FALSE, FALSE, gsn_bolo, TRUE );

      if (!is_affected(victim, gsn_enfeeblement)){
	af.type		= gsn_enfeeblement;
	af.level	= ch->level;
	af.duration	= 5;

	af.where	= TO_AFFECTS;
	af.bitvector	= 0;
	af.location	= APPLY_DEX;
	af.modifier	= -2;
	affect_to_char( victim, &af );
      }
    }
  }
  if ( fAttack && IS_AWAKE(victim) && victim->fighting == NULL)
    multi_hit(victim,ch,TYPE_UNDEFINED);
}



/* marks a person with mark of prey */
void spell_justice_sigil( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *victim;
  bool fForce = FALSE;

  if (IS_NULLSTR( target_name)){
    if (ch->fighting)
      victim = ch->fighting;
    /* check if a pet wolfhound exists (justice mob) and take its target */
    else if (ch->pet && ch->pet->pIndexData->vnum == MOB_VNUM_HOUND
	     && ch->pet->fighting
	     && ch->pet->in_room
	     && ch->pet->in_room == ch->in_room){
      victim = ch->pet->fighting;
      if (IS_WANTED( victim) && is_safe_quiet( ch, victim ))
	fForce = TRUE;
    }
    else{
      send_to_char("Place the Justice Sigil on who?\n\r", ch);
      return;
    }
  }
  else if ( (victim = get_char_room(ch, NULL, target_name )) == NULL){
    send_to_char("They aren't here.\n\r", ch );
    return;
  }
  else if (!fForce && is_safe(ch, victim ))
    return;

  if ( !fForce && ch->fighting != victim && victim->fighting != ch){
    m_yell( ch, victim, FALSE );
    if (IS_AWAKE(victim) && victim->fighting == NULL)
      multi_hit(victim,ch,TYPE_UNDEFINED);
  }

  /* cast the actual mark of prey here */
  spell_mark_prey( sn, level, ch, victim, target );

}

/* sumons the hound used by ENFORCER justice */
void spell_hound_call( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
    char buf[MSL];
    CHAR_DATA *hound;
    AFFECT_DATA af;
    int i;


    if (get_skill(ch,sn) < 2){
      send_to_char("Huh?\n\r", ch );
      return;
    }
    else if (is_affected(ch, sn)){
      send_to_char("You are not yet ready to call a new hound.\n\r", ch);
      return;
    }
    else if (is_captured(ch->pCabal)){
      send_to_char("The hounds will not answer without your Standard.\n\r", ch);
      return;
    }
    sprintf(buf, "The strong clear notes of %s's hunting horn pierce the area.", PERS2( ch ));
    do_mpzecho( ch, buf );
    if (ch->pet ){
      if (ch->pet->pIndexData->vnum ==  MOB_VNUM_HOUND)
	do_say(ch->pet, "Woof? Woof woof woof?");
      else
	do_say(ch->pet, "What? I'm not good enough?");
      return;
    }

    hound = create_mobile( get_mob_index(MOB_VNUM_HOUND) );
    for (i = 0; i < MAX_STATS; i++)
        hound->perm_stat[i] = UMIN(25, (ch->level / 10) + 17);

    hound->alignment = 0;
    hound->level = ch->level;
    hound->sex = ch->sex;

    char_to_room(hound,ch->in_room);

    SET_BIT(hound->affected_by, AFF_CHARM);
    SET_BIT(hound->act, ACT_PET);

    hound->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;

    ch->pet = hound;
    add_follower(hound, ch);
    hound->leader = ch;
    hound->summoner = ch;

    /* Hunter cannot hide ghosted */
    ch->pcdata->ghost = (time_t)NULL;

    act("$N heels to your side!",ch, NULL, hound,TO_CHAR);
    act("$N heels to $n's side!",ch,NULL, hound,TO_ROOM);
    check_social( hound, "growl", "" );
    do_mpasound(hound, "You hear the baying of a hound nearby.");


    af.type               = sn;
    af.level              = level;
    af.duration           = 2;
    af.where		  = TO_AFFECTS;
    af.bitvector          = 0;
    af.modifier           = 0;
    af.location           = APPLY_NONE;
    affect_to_char(ch, &af);
}

void do_hound_obey( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    char arg2[MIL];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if (ch->pet == NULL || ch->pet->pIndexData->vnum != MOB_VNUM_HOUND)
    {
      send_to_char("You have no hound to direct.\n\r", ch );
      return;
    }
    else
      SET_BIT(ch->pet->affected_by, AFF_CHARM);

    if (!str_cmp(arg, "stay")){
      SET_BIT(ch->pet->special, SPECIAL_SENTINEL);
      REMOVE_BIT(ch->pet->special, SPECIAL_RETURN);
      REMOVE_BIT(ch->pet->special, SPECIAL_COWARD);
      send_to_char("Your hound will stay in this room.\n\r",ch);
      return;
    }
    else if (!str_cmp(arg, "come")){
      SET_BIT(ch->pet->special, SPECIAL_RETURN);
      REMOVE_BIT(ch->pet->special, SPECIAL_SENTINEL);
      REMOVE_BIT(ch->pet->special, SPECIAL_COWARD);
      ch->pet->hunttime = 0;
      ch->pet->hunting = NULL;
      send_to_char("Your hound is returning to you.\n\r",ch);
      return;
    }
    else if (!str_cmp(arg, "hunt")){
      if( arg2[0] == '\0' ) {
	send_to_char( "Who do you want your hound to hunt?\n\r", ch );
	return;
      }
      else if( (victim = get_char_world( ch, arg2 )) == NULL){
	send_to_char("No one around by that name.\n\r", ch );
	return;
      }
      else if( ch->in_room == victim->in_room )
      {
	sendf(ch, "%s is already here!\n\r", PERS2(victim));
	return;
      }
      else if (IS_NPC(victim)){
	sendf(ch, "You cannot set your hound on mobs!\n\r");
	return;
      }
      if (!is_pk(ch,victim) && !IS_WANTED(victim))
      {
        sendf(ch, "You cannot set your hound on those whom you cannot harm!\n\r");
        return;
      }

      REMOVE_BIT(ch->pet->special, SPECIAL_SENTINEL);
      REMOVE_BIT(ch->pet->special, SPECIAL_COWARD);
      REMOVE_BIT(ch->pet->special, SPECIAL_RETURN);

      sendf(ch, "Your hound races off to find %s.\n\r",PERS2(victim));
      ch->pet->hunttime = 999;
      REMOVE_BIT( ch->pet->affected_by, AFF_CHARM );
      ch->pet->hunting = victim;
      return;
    }
    else if (!str_cmp(arg, "coward"))
    {
      SET_BIT(ch->pet->special, SPECIAL_COWARD);
      REMOVE_BIT(ch->pet->special, SPECIAL_SENTINEL);
      REMOVE_BIT(ch->pet->special, SPECIAL_RETURN);
      send_to_char("Your hound will stay in this room until attacked.\n\r",ch);
      return;
    }
    send_to_char( "You may command your hound to stay, come, hunt, or coward.\n\r",ch);
}


void spell_guard_call( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    char buf[MSL];
    CHAR_DATA *guard;
    int i;


    if (get_skill(ch,sn) < 2){
      send_to_char("Huh?\n\r", ch );
      return;
    }
    else if (is_affected(ch, sn)){
      send_to_char("The guardhouse has still not processed your request.\n\r", ch);
      return;
    }
    else if (is_captured(ch->pCabal)){
      send_to_char("The guards will not answer without your Standard.\n\r", ch);
      return;
    }
    sprintf(buf, "Guards! Guards!");
    do_yell(ch, buf);

    if (ch->pet ){
      do_say(ch->pet, "What? I'm not good enough?");
      return;
    }

    guard = create_mobile( get_mob_index(MOB_VNUM_SPECIAL_GUARD) );
    for (i = 0; i < MAX_STATS; i++)
      guard->perm_stat[i] = UMIN(25, (ch->level / 10) + 17);

    guard->alignment = 0;
    guard->level = ch->level;
    guard->sex = ch->sex;

    char_to_room(guard,ch->in_room);

    SET_BIT(guard->affected_by, AFF_CHARM);
    SET_BIT(guard->act, ACT_PET);

    guard->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;

    ch->pet = guard;
    ch->summoner = ch;

    add_follower(guard,ch);

    guard->leader = ch;
    guard->master = ch;

    /* Justice cannot hide ghosted */
    ch->pcdata->ghost = (time_t)NULL;
    act("$N arrives to aid you.", ch, NULL, guard, TO_CHAR);
    act("$N arrives to aid $n.", ch, NULL, guard, TO_ROOM);
}

void do_ensnare(CHAR_DATA *ch, char *argument)
{
    char arg[MIL], buf[MIL];
    CHAR_DATA *victim;
    AFFECT_DATA af, *paf;
    int cost = skill_table[gsn_ensnare].min_mana;
    int lag = skill_table[gsn_ensnare].beats;
    int skill = get_skill(ch, gsn_ensnare);

    one_argument(argument, arg);

    if ( skill < 2)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (arg[0] == '\0'){
      victim = ch->fighting;
      if (victim == NULL){
	send_to_char("But you aren't in combat!\n\r",ch);
	return;
      }
    }
    else if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
      send_to_char("They aren't here.\n\r",ch);
      return;
    }
    if (victim == ch)
    {
	send_to_char("Why would you want to do that?\n\r",ch);
	return;
    }
    if (IS_NPC(victim) || !IS_SET(victim->act, PLR_WANTED))
    {
        send_to_char("You can only ensnare criminals.\n\r", ch);
        return;
    }
    else if (IS_AREA(victim->in_room->area, AREA_CABAL)){
      send_to_char("You cannot ensnare in cabals.\n\r", ch);
      return;
    }
    if (is_affected(victim,gen_ensnare))
    {
	send_to_char("You have already ensnared that person.\n\r",ch);
	return;
    }
    else if (is_ghost(victim, 600)){
      send_to_char("Not on ghosts.\n\r", ch);
      return;
    }
    if (IS_OUTLAW(victim)){
      act("Your guards aware of $N's  status as an Outlaw refuse your orders!", ch, NULL, victim, TO_CHAR);
      act("$n's guards aware of your reputation as an Outlaw refuse to ensnare you!", ch, NULL, victim, TO_VICT);
      return;
    }
    else if (ch->mana < cost){
      send_to_char("You are too tired to order your guards about effectivly.\n\r", ch );
      return;
    }
    else
      ch->mana -= cost;

/* subtract cp's */
    handle_skill_cost( ch, ch->pCabal, gsn_ensnare );

    WAIT_STATE(ch, lag);

    /* Justice cannot hide ghosted */
    ch->pcdata->ghost = (time_t)NULL;

    if (number_percent() > skill ){
      send_to_char("You failed.\n\r", ch );
      check_improve(ch, gsn_ensnare, FALSE, 0);
      return;
    }
    check_improve(ch, gsn_ensnare, TRUE, 0);

    sendf(ch,"You call onto the guards to prevent %s from leaving the area.\n\r",PERS(victim,ch));
    sendf(victim,"%s ensnares you, preventing your from leaving the area temporarily.\n\r",PERS(ch,victim));

    af.type               = gen_ensnare;
    af.level              = ch->level;
    af.duration           = number_range(6, 12);
    af.where		  = TO_AFFECTS;
    af.bitvector          = 0;
    af.modifier           = 0;
    af.location           = 0;
    paf = affect_to_char(victim, &af);
    string_to_affect( paf, ch->name );

    sprintf( buf, "%s has been ensnared in %s.", PERS2(victim), ch->in_room->area->name);
    cabal_echo_flag(CABAL_HEARGUARD, buf );
}


/* ***** ASSASSIN POWERS ***** */
void do_spy( CHAR_DATA *ch, char *argument ){
  char arg[MIL];
  CHAR_DATA *victim;
  AFFECT_DATA af;
  argument = one_argument( argument, arg);

  if (get_skill(ch, gsn_eavesdrop) < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (IS_NPC(ch))
    return;
  if ( arg[0] == '\0' ){
    if (ch->pcdata->eavesdropping != NULL){
      act("You are spying on $N.",ch,NULL,ch->pcdata->eavesdropping,TO_CHAR);
      return;
    }
    else{
      send_to_char("Whom do you wish to spy on?\n\r",ch);
      return;
    }
  }
  if (!str_cmp(arg,"none")){
    if (ch->pcdata->eavesdropping != NULL){
      act("You recall the spies from $N's location.",ch,NULL,ch->pcdata->eavesdropping,TO_CHAR);
      affect_strip(ch,gsn_eavesdrop);
      ch->pcdata->eavesdropping->pcdata->eavesdropped = NULL;
      ch->pcdata->eavesdropping = NULL;
      return;
    }
    else{
      send_to_char("You are not spying anyone.\n\r",ch);
      return;
    }
  }
  if (is_affected(ch,gsn_eavesdrop)){
    send_to_char("You may only spy on one person at a time.\n\r",ch);
    return;
  }
  if ( ( victim = get_char_world( ch, arg ) ) == NULL ){
    send_to_char( "You fail to locate them.\n\r", ch );
    return;
  }
  if (ch == victim){
    send_to_char("That is pointless.\n\r",ch);
    return;
  }
  if (IS_NPC(victim)){
    send_to_char("Why would you want to spy on a mob?\n\r",ch);
    return;
  }
  if (IS_IMMORTAL(victim)){
    send_to_char("Spy on gods? Who do you think you are?\n\r", ch);
    return;
  }
  if (ch->mana < skill_table[gsn_eavesdrop].min_mana){
    send_to_char("You lack the power to communicate with the spies.\n\r", ch);
    return;
  }
  else
    ch->mana -=  skill_table[gsn_eavesdrop].min_mana;

  WAIT_STATE(ch,skill_table[gsn_eavesdrop].beats);

  if (number_percent() > get_skill(ch,gsn_eavesdrop)
      || victim->pcdata->eavesdropped != NULL){
    act("The spies report lack of success.", ch, NULL, victim, TO_CHAR);
    check_improve(ch, gsn_eavesdrop, 20, FALSE);
    return;
  }
/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, gsn_eavesdrop );

  act("Your spies begin to keep track of $N's every communication.", ch, NULL, victim, TO_CHAR);

  ch->pcdata->eavesdropping = victim;
  victim->pcdata->eavesdropped = ch;
  af.where     = TO_AFFECTS;
  af.type      = gsn_eavesdrop;
  af.level     = ch->level;
  af.duration  = 24;
  af.location  = APPLY_NONE;
  af.modifier  = 0; //timesince last report (must wait 4 ticks)
  af.bitvector = 0;
  affect_to_char(ch,&af);
  check_improve(ch, gsn_eavesdrop, 20, FALSE);
}

/* eavesdropped room reports */
void spy_report( CHAR_DATA* ch ){
  AREA_DATA* pArea;
  AFFECT_DATA* paf;
  CHAR_DATA* och = NULL;
  char buf[MIL];
  char* text;

  if (IS_NPC(ch))
    return;
  else if ( (och = ch->pcdata->eavesdropped) == NULL)
    return;
  else if (ch->pcdata->pbounty == NULL)
    return;
  else if ( (paf = affect_find(och->affected, gsn_eavesdrop)) == NULL)
    return;
  else if (paf->modifier > 0){
    paf->modifier--;
    return;
  }
  else if (number_percent() < 60)
    return;

  /* location */
  if (IS_AFFECTED2(ch,AFF_SHADOWFORM)
      || IS_IMMORTAL(ch)
      || IS_AFFECTED2(ch,AFF_TREEFORM)
      || IS_AFFECTED2(ch,AFF_CATALEPSY)
      || IS_ROOM(ch->in_room, ROOM_NOWHERE)
      || IS_ROOM(ch->in_room, ROOM_NO_SCAN)
      || is_affected(ch,gsn_bat_form)
      || is_affected(ch,gsn_coffin)
      || is_affected(ch,gsn_entomb)){
    return;
  }
  /* report location */
  /* get random area to mention */
  if ( (pArea = get_rand_aexit(ch->in_room->area)) == NULL)
    pArea = ch->in_room->area;

/* compose the text to say */
  switch (ch->in_room->sector_type){
  default:		text = "on the ground"	; break;
  case SECT_INSIDE:	text = "indoors"	; break;
  case SECT_CITY:	text = "a sidewalk"	; break;
  case SECT_FIELD:	text = "in grass"	; break;
  case SECT_FOREST:	text = "in bush"	; break;
  case SECT_HILLS:	text = "on a hill"	; break;
  case SECT_MOUNTAIN:	text = "amongst stones"	; break;
  case SECT_WATER_SWIM:	text = "in depths"	; break;
  case SECT_WATER_NOSWIM:text = "in water"	; break;
  case SECT_SWAMP:	text = "in mud"		; break;
  case SECT_AIR:	text = "flying"		; break;
  case SECT_DESERT:	text = "in sand"	; break;
  case SECT_LAVA:	text = "in fire"	; break;
  case SECT_SNOW:	text = "in ice"		; break;
  }
  sprintf(buf, "Spies report %s %s near %s.\n\r", PERS2(ch), text, pArea->name);
  send_to_char(buf, och);
  paf->modifier = number_range(3, 5);
}

void do_spyold( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *ori_room;

    if (ch->mana < 70 )
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    ch->mana -= 70;
    if ( (victim = get_char_world(ch, argument)) == NULL)
    {
      act("Not even $g's power can locate them.", ch, NULL, NULL, TO_CHAR);
      return;
    }
    if ( (victim->level > ch->level + 7) && saves_spell(ch->level, victim, DAM_OTHER, skill_table[gsn_spy].spell_type))
    {
	send_to_char("Your minions have failed to reach them.\n\r",ch);
	return;
    }
    if (ch == victim)
        do_look( ch, "auto" );

    else
    {
	ori_room = ch->in_room;
	char_from_room( ch );
	char_to_room( ch, victim->in_room );
	do_look( ch, "auto" );
	char_from_room( ch );
	char_to_room( ch, ori_room );
    }
    WAIT_STATE(ch,skill_table[gsn_spy].beats);
}

/* ***** MYSTIC POWERS ***** */

/* SPELL GIFT
   General effects: sends one magical item to another savant
   Coded By:        Athaekeetha
*/


void spell_gift( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char arg[MIL], arg2[MIL];
    bool char_has_obj = FALSE;

/* Get the obj and victim names */
    target_name = one_argument(target_name, arg);
    target_name = one_argument(target_name, arg2);


/* If caster is an NPC */
    if (IS_NPC(ch)) {
      return;
    }

/* Usage message if not enough args */
    if (arg2[0] == '\0') {
      send_to_char ("Syntax: gift <target> <item>\n\r", ch);
      return;
    }

/* Get the obj from room or caster, else fail */
    obj = get_obj_list (ch, arg2, ch->in_room->contents);
    if (obj == NULL) {
	obj = get_obj_list (ch, arg2, ch->carrying);
	if (obj == NULL) {
	  sendf(ch, "I see no %s here.\n\r", arg2);
	  return;
	}

/* We need to know if obj is in room or held */
	char_has_obj = TRUE;
    }

/* Check if caster can find the victim */
    victim = get_char_world (ch, arg);
    if (victim == NULL) {
      sendf (ch, "%s does not seem to be around.\n\r", arg);
      return;
    }
    /* check fo  NO_SUMMON being off */
    if (IS_SET(victim->act,PLR_NOSUMMON)){
      send_to_char("Target player must have NO_SUMMON turned OFF.\n\r", ch);
      return;
    }

/* Check if caster is high enough lev to send this item */
    if (obj->level > ch->level) {
      send_to_char( "You are not powerful enough to make a gift "\
		    "of this item.\n\r", ch );
      return;
    }

/* Make sure the item is not being worn */
    if (obj->wear_loc != -1) {
      sendf (ch, "You must remove the %s first.\n\r", arg2);
      return;
    }

/* Make sure the caster can drop the item */
    if (!can_drop_obj (ch, obj)) {
	send_to_char( "You can't let go of it.\n\r", ch );
	return;
    }

/* Check if the victim can carry more items */
    if (victim->carry_number + get_obj_number (obj) > can_carry_n (victim)) {
      act("$N has $S hands full.", ch, NULL, victim, TO_CHAR);
      return;
    }

/* Check if the victim can carry more weight */
    if (get_carry_weight (victim) + get_obj_weight_char (victim, obj) >
	can_carry_w (victim)) {
      sendf(ch, "%s can't carry that much weight.\n\r", PERS(victim,ch));
      return;
    }

/* Check if the victim can see the item */
    if (!can_see_obj (victim, obj)) {
      sendf (ch, "%s can't see it.\n\r", PERS(victim,ch));
      return;
    }

/* Check if victim OR caster is in NO_SUMM or NO_GATE */
    if (IS_SET (victim->in_room->room_flags, ROOM_NO_SUMMONIN) ||
	IS_SET (ch->in_room->room_flags, ROOM_NO_SUMMONOUT) ||
	IS_SET (victim->in_room->room_flags, ROOM_NO_GATEIN) ||
	IS_SET (ch->in_room->room_flags, ROOM_NO_GATEOUT)) {
      send_to_char ("A magical force greater than your own prevents "\
		    "your gift.\n\r", ch);
      return;
    }

/* Take the item from the caster */
    if (char_has_obj == TRUE) {
      obj_from_char (obj);
    }
    else {
      obj_from_room (obj);
    }

/* Random chance (Very small) of blowing up the item */
    if (number_percent() + (ch->level / 5) <= 15) {
      sendf(ch, "The %s dissapears with a loud -pop-.\n\r", arg2);
      extract_obj (obj);
      return;
    }

/* Random chance of going elsewhere other than victim */
    if (number_percent() + (ch->level / 5) >= 20) {
      obj_to_room (obj, victim->in_room);

/* Show everyone the action - spell worked properly */
      act ("$p arrives as a gift from $N.", victim, obj, ch, TO_CHAR);
      act ("$N gives a gift of $p to $n.", victim, obj, ch, TO_ROOM);
      act ("You send $p to $N as a gift.", ch, obj, victim, TO_CHAR);
    }

/* Show the caster there was a problem, and hint to the victim */
    else {
      RID* pR = get_rand_room(0,0,	//area range (0 to ignore)
			      0,0,	//room ramge (0 to ignore)
			      NULL,0,	//areas to choose from
			      NULL,0,	//areas to exclude
			      NULL,0,	//sectors required
			      NULL,0,	//sectors to exlude
			      NULL,0,	//room1 flags required
			      NULL,0,	//room1 flags to exclude
			      NULL,0,	//room2 flags required
			      NULL,0,	//room2 flags to exclude
			      5,	//number of seed areas
			      FALSE,	//exit required?
			      FALSE,	//Safe?
			      NULL);	//Character for room checks
      if (pR)
	obj_to_room (obj, pR );
      send_to_char ("You sense a disruption in your spell.\n\r", ch);
      send_to_char ("You feel a touch of deja-vu.\n\r", victim);
    }
}

//servant commands for monsters created through awaken life
//current command is stored in "status"
void do_servant_awaken_life( CHAR_DATA *ch, CHAR_DATA* servant, char *argument ){
  AFFECT_DATA af;
  const int gsn_awaken_life = skill_lookup("awaken life");
  int order_wait = 0;

  if (ch == NULL || servant == NULL || servant->pCabal == NULL)
    return;
  else if (IS_NPC(ch) || !IS_NPC(servant))
    return;
  else if (IS_NULLSTR(argument)){
    send_to_char("Your servant can: Rampage, Destroy, Move, Retreat and Report.\n\r", ch);
    return;
  }
  else if (is_affected(servant, gsn_awaken_life) && !str_prefix(argument, "report")){
    send_to_char("It seems to ignore your orders.\n\r", ch);
    return;
  }

  //RAMPAGE (pick random rooms to destroy in the area monster is in)
  if (!str_prefix(argument, "rampage")){
    char buf1[MIL], buf[MIL], name[MIL];
    CHAR_DATA* vch;

    if (servant->in_room->area->pCabal && is_friendly(servant->in_room->area->pCabal, servant->pCabal) != CABAL_ENEMY){
      send_to_char("You require a vendetta before committing such a hostile action.\n\r", ch);
      return;
    }
    servant->status = AWAKENLIFE_RAMP;
    sendf( ch, "%s will now rampage through %s.\n\r", capitalize(PERS2(servant)), servant->in_room->area->name);
    order_wait = 24;

    sprintf( name, "%s", servant->short_descr);
    name[0] = UPPER(name[0]);
    sprintf( buf, "A messenger yells '`6%s spotted rampaging through %s`6!``'\n\r",
	     name,
	     servant->in_room->area->name);
    sprintf( buf1, "%s's roar echos through the area as it begins its rampage!\n\r", name);

    for (vch = player_list; vch; vch = vch->next_player){
      if (!IS_AWAKE( vch))
	continue;
      else if (vch->in_room->area == servant->in_room->area)
	send_to_char( buf1, vch );
      else
	send_to_char( buf, vch );
    }
  }
  //DESTROY (servant will move to current room and then destroy it)
  else if (!str_prefix(argument, "destroy")){
    if (ch->in_room->area->pCabal && is_friendly(ch->in_room->area->pCabal, ch->pCabal) != CABAL_ENEMY){
      send_to_char("You require a vendetta before committing such a hostile action.\n\r", ch);
      return;
    }
    servant->status = AWAKENLIFE_DEST;
    if (set_path(servant, servant->in_room, ch->in_room, 256, NULL) < 0)
      sendf( ch, "%s cannot find its way here.\n\r", capitalize(PERS2(servant)));
    else
      sendf( ch, "%s will try to destroy this room.\n\r", capitalize(PERS2(servant)));
    order_wait = 1;
  }
  //MOVE will cause the servant to move here
  else if (!str_prefix(argument, "move")){
    servant->status = AWAKENLIFE_MOVE;
    if (set_path(servant, servant->in_room, ch->in_room, 256, NULL) < 0)
      sendf( ch, "%s cannot find its way here.\n\r", capitalize(PERS2(servant)));
    else
      sendf( ch, "%s will now head to this room.\n\r", capitalize(PERS2(servant)));
    order_wait = 3;
  }
  //RETREAT will cause the servant to head for the cabal
  else if (!str_prefix( argument, "retreat")){
    CABAL_DATA* pCab = get_parent( servant->pCabal);
    if (pCab == NULL || pCab->anchor == NULL){
      sendf(ch, "%s has no cabal to return to.\n\r", capitalize(PERS2(servant)));
      return;
    }
    servant->status = AWAKENLIFE_RETR;
    if (set_path( servant, servant->in_room, pCab->anchor, 512, NULL) < 0)
      sendf( ch, "%s cannot find its way back to the cabal.\n\r", capitalize(PERS2(servant)));
    else
      sendf( ch, "%s will now retreat to [%s].\n\r", capitalize(PERS2(servant)), pCab->who_name);
    order_wait = 12;
  }
  //Report reports on the servant's status
  else if (!str_prefix(argument, "report")){
    int percent = 100 * servant->hit / servant->max_hit;
    char* hp_string;

    if (percent > 99)		hp_string = "in excellent condition";
    else if (percent > 90)	hp_string = "has a few scratches";
    else if (percent > 75)	hp_string = "has some small wounds and bruises";
    else if (percent > 50)	hp_string = "has quite a few wounds";
    else if (percent > 30)	hp_string = "has some big nasty wounds and scratches";
    else if (percent > 15)	hp_string = "is preatty hurt";
    else			hp_string = "is in awful condition";

    sendf( ch, "%s is in %s, %sfighting and %s.\n\r",
	   capitalize(PERS2(servant)),
	   servant->in_room->area->name,
	   servant->fighting ? "" : "not ",
	   hp_string);
    return;
  }
  else
    do_servant_awaken_life( ch, servant, "");

  af.type	= gsn_awaken_life;
  af.level	= 60;
  af.duration	= order_wait;
  af.where	= TO_NONE;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char(servant, &af);
}



//general servant commands.
void do_servant( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA* servant = NULL;
  char arg[MIL];
  argument = one_argument( argument, arg );
  if (IS_NPC(ch))
    return;


  if (ch->pcdata->familiar != NULL)
    servant = ch->pcdata->familiar;
  else if (ch->pet
	   && (ch->pet->pIndexData->vnum == MOB_VNUM_KNIGHT_1
	       || ch->pet->pIndexData->vnum == MOB_VNUM_KNIGHT_2
	       || ch->pet->pIndexData->vnum == MOB_VNUM_KNIGHT_3
	       || ch->pet->pIndexData->vnum == MOB_VNUM_VANGUARD))
    servant = ch->pet;

  if (servant == NULL){
    send_to_char("You do not have any servants.\n\r",ch);
    return;
  }
  else if (servant->pIndexData->vnum == MOB_VNUM_MONSTER){
    do_servant_awaken_life( ch, servant, arg );
    return;
  }
  if (!str_cmp(arg, "sentinel")){
    SET_BIT(servant->special, SPECIAL_SENTINEL);
    REMOVE_BIT(servant->special, SPECIAL_RETURN);
    REMOVE_BIT(servant->special, SPECIAL_COWARD);
    send_to_char("Your servant will stay in this room.\n\r",ch);
    return;
  }
  if (!str_cmp(arg, "return")){
    SET_BIT(servant->special, SPECIAL_RETURN);
    REMOVE_BIT(servant->special, SPECIAL_SENTINEL);
    REMOVE_BIT(servant->special, SPECIAL_COWARD);
    servant->hunttime = 0;
    send_to_char("Your servant will slowly return to you.\n\r",ch);
    return;
  }
  if (!str_cmp(arg, "watch")){
    if (IS_SET(servant->special, SPECIAL_WATCH))
      {
	REMOVE_BIT(servant->special, SPECIAL_WATCH);
	send_to_char("Your servant stops watching.\n\r",ch);
      }
    else
      {
	SET_BIT(servant->special, SPECIAL_WATCH);
	send_to_char("Your servant starts watching.\n\r",ch);
      }
    return;
  }
  if (!str_cmp(arg, "coward")){
    SET_BIT(servant->special, SPECIAL_COWARD);
    REMOVE_BIT(servant->special, SPECIAL_SENTINEL);
    REMOVE_BIT(servant->special, SPECIAL_RETURN);
    send_to_char("Your servant will stay in this room until attacked.\n\r",ch);
    return;
  }
  send_to_char( "You may command your servant to sentinel, return, watch, or coward.\n\r",ch);
}

void do_invoke( CHAR_DATA *ch, char *argument )
{
  char arg1[MIL];
  CHAR_DATA *victim;
  OBJ_DATA *artifact, *obj = NULL;
  int damage = 0;
  int skill = 0;
  /* useless conditional */
  if( skill != 0 )
    skill = 0;

  if (get_skill(ch, skill_lookup("invoke artifact")) < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  argument = one_argument( argument, arg1 );

  if ( arg1[0] == '\0' )
    {
      send_to_char("Invoke what?\n\r",ch);
      return;
    }
  if ( (artifact = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
      send_to_char( "You do not have that artifact.\n\r", ch );
      return;
    }
  if ( artifact->item_type != ITEM_ARTIFACT )
    {
      send_to_char( "You can invoke only artifacts.\n\r", ch );
      return;
    }
  if ( ch->level < artifact->level)
    {
      send_to_char("This artifact is beyond your capabilities.\n\r",ch);
        return;
    }
  if ( argument[0] == '\0' )
    victim = ch;
  else if ( ( victim = get_char_room ( ch, NULL, argument ) ) == NULL && ( obj = get_obj_here ( ch, NULL, argument ) ) == NULL )
    {
      send_to_char( "You can't find it.\n\r", ch );
      return;
    }
  act( "$n invokes $p.", ch, artifact, NULL, TO_ROOM );
  act( "You invoke $p.", ch, artifact, NULL, TO_CHAR );
  target_name = argument;
  WAIT_STATE(ch,12);

  skill = get_skill(ch, gsn_invoke) + (get_curr_cond( artifact ) - 100) / 2;

  if (number_percent() >= 10 + get_skill(ch,gsn_invoke) * 4/5)
    {
      send_to_char("The damn thing won't even produce a spark.\n\r",ch);
      check_improve(ch,gsn_invoke,FALSE,2);
    }
  else
    {
      obj_cast_spell( artifact->value[1], artifact->value[0], ch,victim,obj);
      obj_cast_spell( artifact->value[2], artifact->value[0], ch,victim,obj);
      obj_cast_spell( artifact->value[3], artifact->value[0], ch,victim,obj);
      check_improve(ch,gsn_invoke,TRUE,2);
    }
  if (number_percent() > get_skill(ch, gsn_invoke)){
    send_to_char("Your concentration slips severly damaging the artifact.\n\r", ch);
    damage = 25;
  }
  else
    damage = adjust_condition( artifact, 9);
  artifact->condition -= damage;

  if (get_curr_cond( artifact ) < 5){
    act("With a loud thunderclap $p explodes into fine ash!", ch, artifact, NULL, TO_ALL);
      extract_obj( artifact );
  }
}

void spell_mana_transfer( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if (victim == ch)
    {
	send_to_char("You would implode if you tried to transfer mana to yourself.\n\r",ch);
	return;
    }
    if ( !is_same_cabal(ch->pCabal, victim->pCabal))
    {
	send_to_char("You may only cast this spell on fellow cabal members.\n\r",ch);
	return;
    }
    if (victim->class == class_lookup("monk"))
    {
	send_to_char("Monks cannot use that.\n\r",ch);
	return;
    }
    if (ch->hit < 50)
	damage(ch,ch,50,sn,DAM_NONE, TRUE);
    else
    {
	damage(ch,ch,50,sn,DAM_NONE, TRUE);
	victim->mana = UMIN(victim->max_mana, victim->mana + number_range(20,60));
    }
}

void spell_create_artifact( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA *obj = (OBJ_DATA *) vo;
    int skill = get_skill( ch, sn );
    char buf[MSL];

    if (obj->item_type != ITEM_RELIC)
    {
        send_to_char("Only relics can be transformed into artifacts.\n\r",ch);
        return;
    }
    if (obj->wear_loc != -1)
    {
        send_to_char("The item must be carried.\n\r",ch);
        return;
    }
    if ( number_percent() > skill)
    {
	act("$p explodes into fragments!",ch,obj,NULL,TO_ALL);
	extract_obj(obj);
	return;
    }
    act("You transform $p into an artifact.", ch, obj, NULL, TO_CHAR);
    act("$n creates an artifact!",ch,NULL,NULL,TO_ROOM);
    obj->item_type = ITEM_ARTIFACT;
    sprintf(buf, "artifact %s", obj->name);
    str_replace(buf," relic", "");
    free_string( obj->name );
    obj->name = str_dup( buf );
    sprintf(buf, "an artifact of %s", obj->short_descr);
    free_string( obj->short_descr );
    obj->short_descr = str_dup( buf );
    sprintf(buf, "A strange artifact lies on the ground.");
    free_string( obj->description );
    obj->description = str_dup( buf );
}

void do_truesight(CHAR_DATA *ch, char *argument){
  CHAR_DATA* vch;
  int cost = skill_table[gsn_truesight].min_mana;
  int lag = skill_table[gsn_truesight].beats;
  int skill = get_skill(ch, gsn_truesight );

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (is_affected(ch,gsn_truesight)){
    send_to_char("Your eyes are as sharp as they will get.\n\r",ch);
    return;
  }
  if (ch->mana < cost){
    send_to_char("You cannot seem to focus enough.\n\r",ch);
    return;
  }
  if ( (vch = get_group_room( ch, TRUE)) != NULL){
    act("$N gets in your way.", ch, NULL, vch, TO_CHAR);
    return;
  }
  WAIT_STATE( ch, lag );

  if (number_percent() < 4 * skill / 5){
    AFFECT_DATA af;

    af.type   = gsn_truesight;
    af.level  = ch->level;
    af.duration = ch->level / 2;
    af.where  = TO_AFFECTS;
    af.bitvector = AFF_DETECT_INVIS;
    af.location	= APPLY_NONE;
    af.modifier = 0;
    affect_to_char(ch,&af);

    ch->mana -= cost;
    handle_skill_cost_percent( ch, ch->pCabal, gsn_truesight, 100 );

    act("You begin to pay great attention to your surroundings.", ch, NULL, NULL, TO_CHAR);
    act("$n seems to be taking great care to look everywhere at once.",ch,NULL,NULL,TO_ROOM);
    check_improve(ch,gsn_truesight,TRUE,1);
  }
  else{
    ch->mana -= cost / 2;
    act("You look about sharply, but you don't see anything new.",ch,NULL,NULL,TO_CHAR);
    act("$n looks around sharply but doesn't seem enlightened.",ch,NULL,NULL,TO_ROOM);
    check_improve(ch,gsn_truesight,FALSE,1);
  }
}

bool check_ccombat_actions( CHAR_DATA* ch, CHAR_DATA* victim, int type ){
  const int sn = skill_lookup("close combat");
  int skill = get_skill(ch, sn );
  int dam = 0;

  if (ch == NULL || victim == NULL || skill < 2 || ch->position < POS_RESTING)
    return FALSE;
  else if (get_group_room( ch, TRUE) != NULL)
    return FALSE;

  if (ch->mana < skill_table[sn].min_mana)
    return FALSE;
  else if (number_percent() > skill){
    check_improve(ch, sn, FALSE, 0);
    return FALSE;
  }
  else{
    check_improve(ch, sn, TRUE, 0);
  }

  /* we perform individual actions now */
  switch( type ){
  case CCOMBAT_TRIPPED:
    skill = (4 * skill / 5);
    if (ch->size < SIZE_LARGE && number_percent() < skill){
      send_to_char("You easly avoid the trip.\n\r", ch);
      act("$n easly avoids the trip.", ch, NULL, ch, TO_ROOM );
      ch->mana -= skill_table[sn].min_mana;
      return TRUE;
    }
    else
      return FALSE;
  case CCOMBAT_TRIP:
    kick(ch, victim, skill );
    ch->mana -= skill_table[sn].min_mana;
    break;
  case CCOMBAT_DISARM:
    damage(ch, victim, number_range(5, 15), sn, DAM_BASH, TRUE);
    ch->mana -= skill_table[sn].min_mana;
    WAIT_STATE(victim, PULSE_VIOLENCE);
    break;
  case CCOMBAT_BASHED:
    dam = number_range(victim->level / 5, 2 * victim->level / 3) * victim->size / 2;

    if (ch->class == class_lookup("monk"))
      damage(ch, victim, dam, gsn_kickboxing_b, DAM_INTERNAL, TRUE);
    else if (ch->class == class_lookup("thief"))
      damage(ch, victim, dam, gsn_sheath, DAM_INTERNAL, TRUE);
    else
      damage(ch, victim, dam, gsn_kickboxing_a, DAM_INTERNAL, TRUE);
    ch->mana -= skill_table[sn].min_mana;
    break;
  case CCOMBAT_DPARRY:
    dam = number_range(5, 35);
    if (number_percent() < 4 * skill / 5)
      dam = 0;
    if (ch->class == class_lookup("monk"))
      damage(ch, victim, dam, gsn_martial_arts_b, DAM_BASH, TRUE);
    else
      damage(ch, victim, dam, gsn_dual_parry, DAM_BASH, TRUE);
    ch->mana -= skill_table[sn].min_mana;
    break;
/* THIS IS BASICLY ANOTHER DEFENSE, run from inside check_dodge */
  case CCOMBAT_EVADE:
    if (ch->class != class_lookup("thief"))
      break;
    if (number_percent() > skill / 3)
      break;

    dam = number_range(5, 15);

    /* show messeges and hit them */
    act("You sidestep the attack.", ch, NULL, victim, TO_CHAR);
    act("$n sidesteps your attack.", ch, NULL, victim, TO_VICT);
    act("$n sidesteps $N's attack.", ch, NULL, victim, TO_NOTVICT);
    damage(ch, victim, dam, gsn_sheath, DAM_INTERNAL, TRUE);
    ch->mana -= skill_table[sn].min_mana;
    return TRUE;
    /* we return true since this is used in check dodge and if we dodged
       that function will return anyway, regardless of combat status
    */
  }

  return (ch->fighting == NULL);
}

/* performs melee actions */
bool check_melee_actions( CHAR_DATA* ch){
  OBJ_DATA* wield = NULL;
  CHAR_DATA* vch = ch->fighting;
  int skill = 0;
  int choice = 0;
  int chance = 0;

  if (vch == NULL || !is_affected(ch, gsn_melee))
    return FALSE;
  else
    skill = get_skill(ch, gsn_melee );

  if (skill < 2 )
    return FALSE;
  /* get the chance (melee is done once per combat) */

  if (get_group_room( ch, TRUE) != NULL)
    chance = skill / 5;
  else
    chance = 7 * skill / 10;

//DEBUG  sendf(ch, "skill: %d, chance %d\n\r", skill , chance);

  if (number_percent() > chance){
    check_improve(ch, gsn_melee, FALSE, 1);
    return FALSE;
  }
  else
    check_improve(ch, gsn_melee, TRUE, 1);

  /* select which action 0 =DISARM, 1 =DIRT, 2 =DAMAGE */
  choice =  number_range(0, 2);

//DEBUG  sendf(ch, "choice: %d\n\r", choice);

  wield = get_eq_char(vch, WEAR_WIELD);

  if (choice == 1
      && (is_affected(vch, gsn_dirt)
	  || get_skill(ch, gsn_dirt) < 2
	  || ch->in_room->sector_type == SECT_WATER_SWIM
	  || ch->in_room->sector_type == SECT_WATER_NOSWIM
	  || ch->in_room->sector_type == SECT_AIR)
      ){
    if (wield)
      choice = 0;
    else
      choice = 2;
  }
  else if (choice == 0 && wield == NULL){
    if (!is_affected(vch, gsn_dirt)
	&& ch->in_room->sector_type != SECT_WATER_SWIM
	&& ch->in_room->sector_type != SECT_WATER_NOSWIM
	&& ch->in_room->sector_type != SECT_AIR)
      choice = 1;
    else
      choice = 2;
  }

  switch (choice){
    /* DISARM */
  case 0:
    send_to_char("You spot an opening for a disarm!\n\r", ch);
    if (number_percent() < 2 * skill / 3){
      /* fake fail messages */
      act("You fail to disarm $N.",ch,NULL,vch,TO_CHAR);
      act("$n tries to disarm you, but fails.",ch,NULL,vch,TO_VICT);
      act("$n tries to disarm $N, but fails.",ch,NULL,vch,TO_NOTVICT);
    }
    else
      disarm(ch, vch );
    break;
    /* DIRT */
  case 1:
    act("You catch $N looking down!", ch, NULL, vch, TO_CHAR);
    do_dirting(ch, FALSE, "");
    break;
    /* DAMAGE */
  case 2:
  default:
    send_to_char("You spot a lucrative opportunity!\n\r", ch);
    if (number_percent() < 2 * skill / 3){
      if (vch->sex == SEX_MALE)
	damage(ch, vch, 0, gsn_melee, DAM_BASH, TRUE );
      else if (vch->sex == SEX_FEMALE)
	damage(ch, vch, 0, gsn_stance, DAM_BASH, TRUE );
      else if (vch->sex == SEX_NEUTRAL)
	damage(ch, vch, 0, gsn_truesight, DAM_BASH, TRUE );
    }
    else{
      int dam = number_range(ch->level / 2, ch->level);
      dam = UMAX(5, dam * ch->size / 2);

      if (vch->sex == SEX_MALE)
	damage(ch, vch, dam, gsn_melee, DAM_BASH, TRUE );
      else if (vch->sex == SEX_FEMALE)
	damage(ch, vch, dam, gsn_stance, DAM_BASH, TRUE );
      else if (vch->sex == SEX_NEUTRAL)
	damage(ch, vch, dam, gsn_truesight, DAM_BASH, TRUE );
      if (number_percent() < 10){
	act("You groan in pain.", vch, NULL, NULL, TO_CHAR);
	act("$n groans in pain.", vch, NULL, NULL, TO_ROOM);
      }
    }
  }
  return (ch->fighting == NULL);
}

void do_melee(CHAR_DATA *ch, char *argument){
  AFFECT_DATA af;
  int cost = skill_table[gsn_melee].min_mana;
  int lag = skill_table[gsn_melee].beats;
  int skill = get_skill(ch, gsn_melee );

  if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (is_affected(ch,gsn_melee)){
    send_to_char("You are already prepare to melee in combat!\n\r",ch);
    return;
  }
  if (ch->mana < cost){
    send_to_char("You cannot seem to focus enough.\n\r",ch);
    return;
  }
  if (ch->fighting )
    WAIT_STATE( ch, lag / 2 );
  else
    WAIT_STATE( ch, lag );

  af.type   = gsn_melee;
  af.level  = ch->level;
  af.duration = 12;

  af.where  = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_AC;
  af.modifier = -(10 + ch->level/2);
  affect_to_char(ch,&af);


  ch->mana -= cost;
  handle_skill_cost_percent( ch, ch->pCabal, gsn_truesight, 100 );

  act("You will now melee in combat.", ch, NULL, NULL, TO_CHAR);
  act("$n switches into a slightly different stance.",ch,NULL,NULL,TO_ROOM);
}


/* DO ANGER
   General effects: causes warmaster to be agro to all in pk
   Coded By:        Athaekeetha
*/

void do_anger(CHAR_DATA *ch, char *argument) {

  AFFECT_DATA af;

  const int sn = skill_lookup("anger");
  const int lag = skill_table[sn].beats;
  const int cost = skill_table[sn].min_mana;
  int skill = get_skill(ch, sn );


  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (is_affected(ch, gen_anger)){
    send_to_char ("You are as angry as you are likely to get!\n\r", ch);
    return;
  }

  if (number_percent() > skill){
    send_to_char("You cannot seem to gather your ferocity.\n\r",ch);
    check_improve(ch, skill_lookup("anger"), FALSE, 1);
    return;
  }
  else
    check_improve(ch, skill_lookup("anger"), TRUE, 1);

  if ( ch->mana < cost) {
    send_to_char("You cannot seem to gather your ferocity.\n\r",ch);
    return;
  }
  else
    ch->mana -= cost;
  WAIT_STATE2( ch, lag );
  handle_skill_cost( ch, ch->pCabal, sn );

  send_to_char("You gather your ferocity.\n\r",ch);

  af.where        = TO_AFFECTS;
  af.type         = gen_anger;
  af.level        = 0;
  af.duration     = 1 + number_range(1, 3);
  af.location     = APPLY_DAMROLL;
  af.modifier     = number_fuzzy(3);
  af.bitvector    = 0;
  affect_to_char(ch,&af);
  af.location     = APPLY_HITROLL;
  affect_to_char(ch,&af);
}

/* enchant weapon skill */
void do_sharpen (CHAR_DATA *ch, char *argument) {
  AFFECT_DATA *paf;
  OBJ_DATA* obj;

  const int gsn_sharpen = skill_lookup("sharpen");
  const int sn = skill_lookup("enchant weapon");
  const int lag = skill_table[gsn_sharpen].beats;
  const int cost = skill_table[gsn_sharpen].min_mana;
  const int level = ch->level;

  int skill = get_skill(ch, gsn_sharpen );
  int wskill = 0;


  char buf [MIL];

//Data for the process
  int enchant_bonus = 0;
  int total_enchant = 0;
  int hit_bonus = 0;
  int dam_bonus = 0;
  int tot_dam = 0;
  int tot_hit = 0;
  int chance = number_percent();
//spell fails if chance is greater then fail
  int fail = 5;

//modifiers
  const int lvl_mod = 1;
  const int glow_mod = 10;
  const int hum_mod = 5;
  const int strdex_mod = 2;
  const int luck_mod = 2;
  const int skill_mod = 1;   //Skill mod
  const int limit_mod = 5;   //modifier for over 6 enchant (over 6)^2*limit_mod

//levels in percent
  const int shatter_lvl = 3;//destroy
  const int fry_lvl = 15;//lower DAMTYPE or DAMDICE
  const int fade_lvl = 30;//fade weapon
  const int lower2_lvl = 45;//lower affect by 1 or 2
  const int lower_lvl = 65;//lower affect by 1 or 2
  const int nothing_lvl = 100;//nothin

//flags
  bool blow = FALSE;
  bool lower = FALSE;
  bool hitroll_found = FALSE;
  bool damroll_found = FALSE;

  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument)){
    send_to_char("Sharpen which weapon?\n\r",ch);
    return;
  }
  else if ( (obj = get_obj_carry(ch, argument, ch)) == NULL){
    send_to_char("You do not seem to be carrying that.\n\r", ch);
    return;
  }
  else if (obj->item_type != ITEM_WEAPON){
    send_to_char("You can only sharpen meele weapons.\n\r", ch);
    return;
  }
  else if ( (wskill = get_weapon_skill_obj(ch, obj)) < 95){
    send_to_char("You do not know enough about these type of weapons.\n\r", ch);
    return;
  }
  else if (is_affected(ch, skill_lookup("calm"))){
    send_to_char("You are in far to peaceful mood to think of violence.", ch);
    return;
  }
  //Malform weapon check.
  else if (affect_find(obj->affected, gen_malform)
	   || obj->pIndexData->vnum == OBJ_VNUM_WEAPONCRAFT){
    act("$p's edge resists all your efforts.", ch, obj, NULL, TO_ALL);
    return;
  }
  else if ( ch->mana < cost) {
    send_to_char("You cannot seem to be able to concentrate on the task.\n\r",ch);
    return;
  }
  else
    ch->mana -= cost;
  handle_skill_cost( ch, ch->pCabal, sn );
  WAIT_STATE2(ch, lag );

  /* bonus for high weapons kills */
  if (wskill > 100){
    sendf(ch, "You make good use of your advanced knowledge of %ss.\n\r",
	  weapon_name( obj->value[0]));
    skill += (wskill - 100) * 2;
  }

  /* actual enchanting */
  chance = number_percent();


//we check if an enchatement exits already
  if (!obj->enchanted){
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
      if ( paf->location == APPLY_DAMROLL ){
	damroll_found = TRUE;
	//we add all the existing enchants up.
	dam_bonus = paf->modifier;
	tot_dam += dam_bonus;
	fail += 4 * (dam_bonus * dam_bonus);
      }
      else if ( paf->location == APPLY_HITROLL ){
	hitroll_found = TRUE;
	hit_bonus = paf->modifier;
	tot_hit += hit_bonus;
	fail += 4 * (hit_bonus * hit_bonus);
      }
  }

//We scan regular way now to get the total of effects on obj.
  for ( paf = obj->affected; paf != NULL; paf = paf->next ){
    if ( paf->location == APPLY_DAMROLL ){
      damroll_found = TRUE;
      //we add all the existing enchants up.
      dam_bonus = paf->modifier;
      tot_dam += dam_bonus;
      fail += 4 * (dam_bonus * dam_bonus);
    }
    else if ( paf->location == APPLY_HITROLL ){
      hitroll_found = TRUE;
      //we add all the existing enchants up.
      hit_bonus = paf->modifier;
      tot_hit += hit_bonus;
      fail += 4 * (hit_bonus * hit_bonus);
    }
  }
  //we average out the enchants
  total_enchant = (tot_hit + tot_dam)/2;

  //we now have total enchants already on the item.
  //start caluclating chances.
  //the higher "fail" the smaller chance to enchant and greater risk.

//level bonus
  fail -= level * lvl_mod;

 //skill mod
  fail -= (skill - 75) / skill_mod;

//dex bonus
  fail -= ( ((get_curr_stat(ch, STAT_DEX) - 21) + (get_curr_stat(ch, STAT_STR) - 21)) / 2) * strdex_mod;
//penalty for objects of higher level then caster.
  if (obj->level > level)
    fail += (obj->level - level) * lvl_mod;

  if (IS_OBJ_STAT(obj,ITEM_GLOW))
    fail += glow_mod;
  if (IS_OBJ_STAT(obj,ITEM_HUM))
    fail += hum_mod;


//penalty for superduper lvl enchant.
  if (total_enchant >= 5 || (tot_hit >= 5 || tot_dam >= 5))
    fail+= (total_enchant - 4) * (total_enchant - 4) * limit_mod;

//ceiling for super dupers enchants.
  if (total_enchant >= 7 || (tot_hit >=7 || tot_dam >= 7))
    blow = TRUE;

//check for manacharge
  if ((paf = affect_find(obj->affected, gen_mana_charge)) != NULL)
    blow = TRUE;


//luck bonus
  fail -= (get_curr_stat(ch,STAT_LUCK) - 16) * luck_mod;

//ceiling
  fail = URANGE(5,fail,90);
  chance = URANGE(0,chance,100);
//begin enchanting, from worst to best.

//shatter
  if ( (chance < (shatter_lvl * fail /100)) || (chance <= 3) || (blow && (chance >= nothing_lvl * fail / 100)) )
    {
      act("$p breaks in half!",ch,obj,NULL,TO_ALL);
      obj->condition = 1;
      return;
    }
//fry ie: lower weapon dice.
  else if  (chance < (fry_lvl * fail /100) || (chance <= 8)){
    int lost = 25;
    act("A large crack appears in $p.",ch,obj,NULL,TO_ALL);
    obj->condition -= lost;
    return;
  }
//fade, remove all enchants
  else if  (chance < (fade_lvl * fail /100)){
    AFFECT_DATA *paf_next;

    act("You manage to accidently remove all the improvements.",ch, obj, NULL, TO_CHAR);
    act("$n manages to accidently removes all the improvements on $p.", ch, obj, NULL, TO_ROOM);
    obj->enchanted = TRUE;
    for (paf = obj->affected; paf != NULL; paf = paf_next){
      paf_next = paf->next;
      //not the owner effect.
      if (paf->type == gen_has_owner)
	continue;
      free_affect(paf);
    }
    obj->affected = NULL;
    REMOVE_BIT(obj->extra_flags,ITEM_GLOW);
    REMOVE_BIT(obj->extra_flags,ITEM_HUM);
    REMOVE_BIT(obj->extra_flags,ITEM_MAGIC);
    return;
  }
//lower, the enchant is inverted
  else if  (chance < (lower_lvl * fail /100))
    lower = TRUE;
//nothing
  else if  (chance < (nothing_lvl * fail /100))
    {
      send_to_char("You didn't achive anything notable.\n\r",ch);
      return;
    }


//begin enchanting, inverse or regular.notice these are not elseif now.
  //double enchant with a check for double fade.
  if (lower && (chance < (lower2_lvl * fail /100)) ){
    enchant_bonus = 2;
    if (lower){
      enchant_bonus *= -1;
      sprintf(buf, "$p develops a huge flaw in its blade.");
    }
    else
      sprintf(buf, "$p develops a small flaw in its blade.");
    //we shoot out the message.
    act(buf,ch,obj,NULL,TO_ALL);
  }//end double enchant
//regular 1 point enchant,
  else if ( (chance >= fail) || lower){
    enchant_bonus = 1;
    if (lower){
      enchant_bonus *= -1;
      sprintf(buf, "$p develops a small flaw in its blade.");
    }
    else
      sprintf(buf, "$p's edge gleams in the light.");

//we shoot out the message.
    act(buf,ch,obj,NULL,TO_ALL);
  }//end single enchant

  obj->pCabal = ch->pCabal;

  if (obj->level < LEVEL_HERO - 1)
    obj->level = UMIN(LEVEL_HERO - 1,obj->level + 1);

//set if total enchant is more then 4 on obj,
  if (total_enchant >= 4)
    SET_BIT(obj->value[4], WEAPON_SHARP);

//now we slap down the effects.
//If object was not previously affected we copy affects.
  if (!obj->enchanted){
    AFFECT_DATA *af_new;
    obj->enchanted = TRUE;
    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next){
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
  if (hitroll_found){
    for ( paf = obj->affected; paf != NULL; paf = paf->next){
      if ( paf->location == APPLY_HITROLL){
	paf->type = sn;
	paf->modifier += enchant_bonus;
	paf->level = UMAX(paf->level,level);
      }
    }
  }
  else{
    paf = new_affect();
    paf->where= TO_OBJECT;
    paf->type= sn;
    paf->level= level;
    paf->location  = APPLY_HITROLL;
    paf->duration= -1;
    paf->modifier = enchant_bonus;
    paf->bitvector  = 0;
    paf->next= obj->affected;
    obj->affected= paf;
  }//done if no similiar enchants.

  if (damroll_found){
    for ( paf = obj->affected; paf != NULL; paf = paf->next){
      if ( paf->location == APPLY_DAMROLL){
	paf->type = sn;
	paf->modifier += enchant_bonus;
	paf->level = UMAX(paf->level,level);
      }
    }
  }
  else{
    paf = new_affect();
    paf->where= TO_OBJECT;
    paf->type= sn;
    paf->level= level;
    paf->location  = APPLY_DAMROLL;
    paf->duration= -1;
    paf->modifier = enchant_bonus;
    paf->bitvector  = 0;
    paf->next= obj->affected;
    obj->affected= paf;
  }//done if no similiar enchants.
}//end spell_enchant_weapon


void spell_catalyst( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    int max_gain = 1000;
    int gain = 0;


    if (is_affected(ch, gen_sav_cata))
    {
	send_to_char("You have already been infused with arcane "\
		     "energy.\n\r",ch);
	return;
    }
    if (ch->hit > ch->max_hit){
      send_to_char("You health is beyond your limits as it is!\n\r", ch);
      return;
    }
    gain = UMIN(350, ch->mana / 3);
    /* check for bonus items in savant cabal */
    if (get_skill(ch, sn) > 100)
      max_gain += 50;
    if (ch->max_hit + gain  > max_gain)
      gain = UMAX(0, max_gain - ch->max_hit);
    ch->hit += gain;
    af.type               = gen_sav_cata;
    af.level              = level;
    af.duration           = 24;
    af.location           = APPLY_HIT;
    af.modifier           = gain;
    af.bitvector          = 0;
    affect_to_char(ch,&af);
}


void spell_tesseract( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim, *wch, *wch_next;
    ROOM_INDEX_DATA *new_room;
    bool gate_pet;
    const int cost = 50;


    if( (victim = get_char_world(ch, target_name )) == NULL
	|| victim == ch
	|| victim->in_room == NULL
	|| ch->in_room == NULL)
    {
	send_to_char("Your target unreachable, the spell fails.\n\r",ch);
	return;
    }

    if ( (is_safe_quiet(ch,victim) && IS_SET(victim->act,PLR_NOSUMMON))
	 || victim == ch || victim->in_room == NULL || !can_see_room(ch,victim->in_room)
	 || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	 || ((IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	      || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	      || IS_SET(victim->in_room->room_flags, ROOM_NO_GATEIN)
	      || IS_SET(ch->in_room->room_flags, ROOM_NO_GATEOUT)
	      || IS_SET(victim->in_room->room_flags, ROOM_NO_INOUT)
	      || IS_SET(ch->in_room->area->area_flags, AREA_NOMORTAL)
	      || IS_SET(victim->in_room->area->area_flags, AREA_NOMORTAL)
	      || IS_SET(ch->in_room->area->area_flags, AREA_RESTRICTED)
	      || IS_SET(victim->in_room->area->area_flags, AREA_RESTRICTED)
	      || IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)
	      || (!IS_NPC(victim) && victim->level > LEVEL_HERO)
	      || (!IS_IMMORTAL(ch) && IS_IMMORTAL(victim))
	      || (IS_NPC(victim) && (IS_AFFECTED(victim,AFF_CHARM)
				     && victim->master != NULL
				     && victim->in_room == victim->master->in_room
				     && !is_pk(ch,victim->master)) )
	      || (!IS_NPC(victim) && IS_SET(victim->act,PLR_NOSUMMON)
		  && !is_pk(ch,victim))
	      || is_affected(victim,gsn_coffin)
	      || is_affected(victim,gsn_entomb)
	      || is_affected(ch, gsn_tele_lock)
	      || is_affected(victim,gsn_catalepsy) )
	     && !IS_IMMORTAL(ch)) )
      {
	send_to_char( "Something seems to block your spell.\n\r", ch );
	return;
      }
    else if (is_affected(ch,gen_ensnare)){
      send_to_char("You sense a powerful magical field preventing your departure.\n\r", ch);
      return;
    }
    if (saves_spell( level - 2, victim, DAM_OTHER,skill_table[sn].spell_type )){
      send_to_char("You failed.\n\r", ch);
      if (number_percent() < 40)
	send_to_char("You sense the world stop about you for a moment.", victim);
      return;
    }

    new_room = victim->in_room;
    if (ch->pet != NULL && ch->in_room == ch->pet->in_room)
	gate_pet = TRUE;
    else
	gate_pet = FALSE;
    for (wch = ch->in_room->people; wch != NULL; wch = wch_next)
    {
        wch_next = wch->next_in_room;

        if (is_same_group(wch, ch) && wch != ch)
	{
	  if (is_affected(wch, gsn_tele_lock)){
	    send_to_char("The teleportation lock stops you short.\n\r", wch);
	    continue;
}
	  /* bonus for savant items */
	  if (get_skill(ch, sn)  <= 100){
	    if (ch->mana < (IS_NPC(wch) ? cost/2 : cost)){
	      send_to_char("You have no power left for additional followers.\n\r", ch);
	      break;
	    }
	    else
	      ch->mana -= (IS_NPC(wch) ? cost/2 : cost);
	  }
	  act("$n utters some strange words and, with a sickening lurch, you feel time and space shift around you.",ch,NULL,wch,TO_VICT);
	  char_from_room(wch);
	  char_to_room(wch,new_room);
	  act("$n arrives suddenly.",wch,NULL,NULL,TO_ROOM);
	  do_look(wch,"auto");
	}
    }
    act("With a sudden flash of light, $n and $s friends disappear!",ch,NULL,NULL,TO_ROOM);
    act("As you utter the words, time and space seem to blur.  You feel as though space and time are shifting all around you while you remain motionless.",ch,NULL,NULL,TO_CHAR);
    char_from_room(ch);
    char_to_room(ch,new_room);
    act("$n arrives suddenly.",ch,NULL,NULL,TO_ROOM);
    do_look(ch,"auto");
    if (gate_pet)
    {
	char_from_room(ch->pet);
	char_to_room(ch->pet,victim->in_room);
	act("$n arrives suddenly.",ch->pet,NULL,NULL,TO_ROOM);

    }
}

void spell_scourge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *tmp_vict, *tmp_next;
    char buf[MSL];
    int dam;
    static const sh_int dam_each[] =
    {
	  0,
	  0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
	  0,   0,   0,   0,  25,	 30,  35,  40,  45,  50,
	 55,  60,  65,  70,  75,	 77,  79,  81,  83,  85,
	 87,  89,  91,  93,  95,	 97,  98,  99, 100, 101,
	102, 103, 104, 105, 106,	107, 108, 109, 110, 111
    };
    if (!IS_IMMORTAL(ch))
	return;

    level = UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level = UMAX(0, level);
    dam	= number_range( dam_each[level] / 2, dam_each[level] * 2 );
    for (tmp_vict = ch->in_room->people; tmp_vict != NULL; tmp_vict = tmp_next)
    {
	tmp_next = tmp_vict->next_in_room;
	if ( !is_area_safe(ch,tmp_vict))
	{
	    if (!IS_NPC(ch) && tmp_vict != ch && ch->fighting != tmp_vict && tmp_vict->fighting != ch
	    && (IS_SET(tmp_vict->affected_by,AFF_CHARM) || !IS_NPC(tmp_vict)))
	    {
	        if (!can_see(tmp_vict, ch))
		    j_yell(tmp_vict, "Help someone is attacking me!");
		else
		{
		    sprintf(buf,"Die, %s, you sorcerous dog!",ch->name);
		    j_yell(tmp_vict,buf);
		}
	    }
	    if (!is_affected(tmp_vict,sn))
	    {
		if (number_percent() < 2 * level)
		    spell_poison(gsn_poison, level, ch, (void *) tmp_vict, TARGET_CHAR);
		if (number_percent() < 2 * level);
		    spell_blindness(gsn_blindness,level,ch,(void *) tmp_vict,TARGET_CHAR);
		if (number_percent() < 2 * level)
		    spell_weaken(skill_lookup("weaken"), level, ch, (void *) tmp_vict, TARGET_CHAR);
		if (saves_spell(level,tmp_vict,DAM_FIRE,skill_table[sn].spell_type))
		    dam /= 2;
		damage( ch, tmp_vict, dam, sn, DAM_FIRE, TRUE );
	    }
	}
    }
}

void spell_nightwalker( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *walker;
    AFFECT_DATA af;
    char buf[100];
    int i;
    if (!IS_IMMORTAL(ch))
	return;

    if (is_affected(ch,sn))
    {
	send_to_char("You feel too weak to summon a Nightwalker now.\n\r", ch);
	return;
    }
    act("You attempt to summon a Nightwalker.",ch,NULL,NULL,TO_CHAR);
    act("$n attempts to summon a Nightwalker.",ch,NULL,NULL,TO_ROOM);
    if (get_control(ch, MOB_VNUM_NIGHTWALKER) > 0)
    {
	send_to_char("Two nightwalkers are more than you can control.\n\r",ch);
	return;
    }
    walker = create_mobile( get_mob_index(MOB_VNUM_NIGHTWALKER) );
    for (i=0;i < MAX_STATS; i++)
	walker->perm_stat[i] = ch->perm_stat[i];
    walker->max_hit = IS_NPC(ch)? URANGE(ch->max_hit,1 * ch->max_hit,30000) : URANGE(ch->pcdata->perm_hit,1*ch->pcdata->perm_hit,30000);
    walker->hit = walker->max_hit;
    walker->max_mana = ch->max_mana;
    walker->mana = walker->max_mana;
    walker->level = ch->level;
    for (i=0; i < 3; i++)
	walker->armor[i] = interpolate(walker->level,100,-100);
    walker->armor[3] = interpolate(walker->level,100,0);
    walker->hitroll    = ch->hitroll;
    walker->damroll    = ch->damroll;
    walker->damage[DICE_NUMBER]	= 5;
    walker->damage[DICE_TYPE]	= (ch->level /10) +3;
    walker->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
    char_to_room(walker,ch->in_room);
    act("A Nightwalker rises from the shadows!",ch,NULL,NULL,TO_ALL);
    sprintf(buf, "kneel %s", ch->name);
    interpret(walker, buf);
    af.type               = sn;
    af.level              = level;
    af.duration           = 24;
    af.bitvector          = 0;
    af.modifier           = 0;
    af.location           = APPLY_NONE;
    affect_to_char(ch, &af);
    SET_BIT(walker->affected_by, AFF_CHARM);
    add_follower(walker,ch);
    walker->leader = ch;
    walker->summoner = ch;
}

void spell_nightfall( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *vch;
    OBJ_DATA  *light;
    if (!IS_IMMORTAL(ch))
	return;

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
	for (light = vch->carrying; light != NULL; light = light->next_content)
	    if ( light->item_type == ITEM_LIGHT && light->value[2] != 0 && !is_same_group(ch, vch))
	    {
		if (light->value[2] != -1 || saves_spell(level, vch, DAM_LIGHT,skill_table[sn].spell_type))
		{
		    act("$p flickers and goes out!",ch,light,NULL,TO_ALL);
		    light->value[2] = 0;
		    if (light->wear_loc == WEAR_LIGHT)
			ch->in_room->light--;
	        }
		else
		    act("$p momentarily dims.",ch,light,NULL,TO_ALL);
	    }
    for (light = ch->in_room->contents; light != NULL; light=light->next_content)
	if (light->item_type == ITEM_LIGHT && light->value[2] != 0)
	{
	    if (light->value[2] != -1)
	    {
		act("$p flickers and goes out!",ch,light,NULL,TO_ALL);
		light->value[2] = 0;
	    }
	    else
		act("$p momentarily dims.",ch,light,NULL,TO_ALL);
	}
}

void spell_garble( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (!IS_IMMORTAL(ch))
	return;
    if (ch == victim)
    {
	send_to_char("Garble whose speech?\n\r",ch);
	return;
    }
    if (is_affected(victim,sn))
    {
	sendf(ch,"%s's speech is already garbled.",PERS(victim,ch));
	return;
    }
    if (saves_spell(level*1.5,victim,DAM_OTHER,skill_table[sn].spell_type))
	return;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char(victim,&af);
    act("You have garbled $N's speech!",ch,NULL,victim,TO_CHAR);
    act_new("You feel your tongue contort.",ch,NULL,victim,TO_CHARVICT,POS_DEAD);
}

/*void spell_confuse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *vch;
    AFFECT_DATA af;
    if (is_affected(victim,gsn_confuse))
    {
        sendf(ch, "%s is already thoroughly confused.", PERS(victim,ch));
        return;
    }
    if (saves_spell(level,victim,))
        return;
    af.type      = sn;
    af.level     = level;
    af.duration  = number_fuzzy(level);
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char(victim,&af);
    vch = get_char_room(victim, NULL, ch->name);
    do_kill(victim,vch->name);
}*/

/*void spell_terangreal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (IS_NPC(victim))
	return;
    af.type      = sn;
    af.level     = level;
    af.duration  = 10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SLEEP;
    affect_join( victim, &af );
    if ( IS_AWAKE(victim) )
    {
	act( "You are overcome by a sudden surge of fatigue.", ch,NULL,victim,TO_CHARVICT );
	act( "$N falls into a deep sleep.", ch,NULL,victim,TO_ROOM );
	victim->position = POS_SLEEPING;
    }
}*/

/*void spell_sedai_heal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    if ( is_affected( ch, sn ) )
    {
	send_to_char("The sa'angreal has been used for this purpose too recently.\n\r", ch);
	return;
    }
    af.type      = sn;
    af.level     = level;
    af.duration  = 50;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    ch->hit = UMIN( ch->hit + 150, ch->max_hit );
    update_pos( ch );
    act_new("A warm feeling fills your body.", ch,NULL,NULL,TO_CHAR,POS_DEAD );
    act("$n looks better.", ch,NULL,NULL,TO_ROOM);
}*/

/*void spell_sedai_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    if ( is_affected( ch, sn ) )
    {
        send_to_char("The sa'angreal has been used for that too recently.\n\r",ch);
        return;
    }
    af.type      = sn;
    af.level     = level;
    af.duration  = level;
    af.location  = APPLY_AC;
    af.modifier  = -30;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    act("$n is surrounded by a mystical shield.",ch,NULL,NULL,TO_ROOM );
    act_new("You are surrounded by a mystical shield.,ch,NULL,NULL,TO_CHAR,POS_DEAD );
}*/

/*void spell_saidar( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    AFFECT_DATA af;
    if ( is_affected( ch, sn ) )
    {
        send_to_char("The sa'angreal has been used for this purpose too recently.\n\r", ch);
        return;
    }
    af.type      = sn;
    af.level     = level;
    af.duration  = 70;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    dam = dice(level, 7);
    damage(ch,victim,dam,sn,DAM_HOLY);
}*/

/*void spell_amnesia( int sn, int level, CHAR_DATA *ch, void *vo )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int i;
    if (IS_NPC(victim))
        return;
    for (i = 0; i < MAX_SKILL; i++)
        victim->pcdata->learned[i] /= 2;
    act_new("You feel your memories slip away.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n gets a blank look on $s face.",victim,NULL,NULL,TO_ROOM);
}*/

void spell_randomizer(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    ROOM_INDEX_DATA *pRoomIndex;
    EXIT_DATA *pexit;
    int d0, d1;
    AFFECT_DATA af;
    if (!IS_IMMORTAL(ch))
	return;

    if ( is_affected( ch, sn ) )
    {
        send_to_char("Your power of randomness has been exhausted for now.\n\r", ch);
        return;
    }
/*    if (IS_SET(ch->in_room->room_flags, ROOM_LAW))
    {
        send_to_char("This room is far too orderly for your powers to work on it.\n\r", ch);
        return;
    }*/
    af.type      = sn;
    af.level     = UMIN(level + 15, 60);
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    pRoomIndex = get_room_index(ch->in_room->vnum);
    if (number_bits(1) == 0)
    {
        send_to_char("Despite your efforts, the universe resisted chaos.\n\r", ch);
        if (ch->trust >= 56)
            af.duration  = 1;
        else
	    af.duration = 2*level;
        affect_to_char(ch, &af);
        return;
    }
    for (d0 = 0; d0 < 5; d0++)
    {
      d1 = number_range(d0, 5);
      pexit = pRoomIndex->exit[d0];
      pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
      pRoomIndex->exit[d1] = pexit;
    }
    if (ch->trust >= 56)
        af.duration = 1;
    else
        af.duration = 4*level;
    affect_to_char(ch, &af);
    send_to_char("The room was successfully randomized!\n\r", ch);
    send_to_char("You feel very drained from the effort.\n\r", ch);
    ch->hit -= UMIN(200, ch->hit/2);
    nlogf( "%s used randomizer in room %d", ch->name, ch->in_room->vnum);
}

void spell_shadowlife( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    CHAR_DATA *shadow;
    AFFECT_DATA af;
    int i;
    char buf[MSL];
    char *name;
    if (!IS_IMMORTAL(ch))
	return;

    if (IS_NPC(victim))
    {
        send_to_char("Now, why would you want to do that?!?\n\r", ch);
        return;
    }
    if (is_affected(ch,sn))
    {
        send_to_char("You don't have the strength to raise a Shadow now.\n\r", ch);
        return;
    }
    act("You give life to $N's shadow!",ch, NULL, victim, TO_CHAR);
    act("$n gives life to $N's shadow!",ch,NULL,victim,TO_NOTVICT);
    act("$n gives life to your shadow!", ch, NULL, victim, TO_VICT);
    shadow = create_mobile( get_mob_index(MOB_VNUM_SHADOW) );
    for (i=0;i < MAX_STATS; i++)
        shadow->perm_stat[i] = victim->perm_stat[i];
    shadow->max_hit = (3 * victim->max_hit) / 4;
    shadow->hit = shadow->max_hit;
    shadow->max_mana = (3 * victim->max_mana) / 4;
    shadow->mana = shadow->max_mana;
    shadow->level = victim->level;
    for (i=0; i < 3; i++)
        shadow->armor[i] = interpolate(shadow->level,100,-100);
    shadow->armor[3] = interpolate(shadow->level,100,0);
    shadow->sex = victim->sex;
    shadow->gold = 0;
    name = IS_NPC(victim) ? victim->short_descr : victim->name;
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
    do_murder(shadow, victim->name);
    af.type               = sn;
    af.level              = level;
    af.duration           = 24;
    af.bitvector          = 0;
    af.modifier           = 0;
    af.location           = APPLY_NONE;
    affect_to_char(ch, &af);
}

/*void spell_dragon_strength(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    AFFECT_DATA af;
    if (is_affected(ch, sn))
    {
        send_to_char("You are already full of the strength of the dragon.\n\r",	ch);
        return;
    }
    af.type = sn;
    af.level = level;
    af.duration = level / 3;
    af.bitvector = 0;
    af.modifier = 2;
    af.location = APPLY_HITROLL;
    affect_to_char(ch, &af);
    af.modifier = 2;
    af.location = APPLY_DAMROLL;
    affect_to_char(ch, &af);
    af.modifier = 10;
    af.location = APPLY_AC;
    affect_to_char(ch, &af);
    af.modifier = 2;
    af.location = APPLY_STR;
    affect_to_char(ch, &af);
    af.modifier = -2;
    af.location = APPLY_DEX;
    affect_to_char(ch, &af);
    act("The strength of the dragon enters you.", ch,NULL,NULL,TO_CHAR);
    act("$n looks a bit meaner now.", ch,NULL,NULL,TO_ROOM);
}*/

/*void spell_dragon_breath(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dam;
    static const sh_int dam_each[] =
    {
      0,
      2,  5,  9,  11, 14,  17, 20, 23, 26, 29,
      29, 30, 30, 31, 32,  32, 33, 34, 34, 35,
      35, 36, 37, 38, 39,  40, 40, 41, 41, 42,
      43, 43, 44, 45, 45,  46, 47, 48, 48, 49,
      50, 51, 52, 53, 54,  55, 56, 57, 58, 60

    };
    level = UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
    level = UMAX(0, level);
    dam = number_range(dam_each[level] / 2, dam_each[level] * 2 );
    if (saves_spell(level, victim,skill_table[sn].spell_type))
        dam /= 2;
    damage(ch, victim, dam, sn, DAM_FIRE);
}*/

void spell_golden_aura( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *vch = vo;

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
        if (!is_same_group(vch, ch))
	  continue;
	else if ( vch->pCabal && IS_CABAL(get_parent(vch->pCabal), CABAL_NOMAGIC))
	  continue;
        if ( is_affected( vch, sn ) )
	  {
	    if (vch == ch)
	      send_to_char("You are already protected.\n\r",ch);
	    else
	      sendf(ch, "%s is already protected.\n \r", PERS2(vch));
	    continue;
	  }
        af.type      = sn;
        af.level     = level;
        af.duration  = level;
	af.where = TO_AFFECTS;
        af.bitvector = AFF_PROTECT_EVIL;
        af.modifier = level/8;
        af.location = APPLY_HITROLL;
        affect_to_char( vch, &af );

        af.modifier = 0 - level/6;
        af.location = APPLY_SAVING_SPELL;
        affect_to_char(vch, &af);

        act_new( "You feel a golden aura around you.", ch,NULL,vch,TO_CHARVICT,POS_DEAD );
        if ( ch != vch )
	  act("A golden aura surrounds $N.",ch,NULL,vch,TO_CHAR);
      }
}

/*void spell_dragonplate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    int plate_vnum;
    OBJ_DATA *plate;
    plate_vnum = OBJ_VNUM_PLATE;
    plate = create_object( get_obj_index(plate_vnum), level + 5);
    plate->timer = 2 * level;
    plate->cost  = 0;
    obj_to_char(plate, ch);
    act("You create $p!",ch,plate,NULL,TO_CHAR);
    act("$n creates $p!",ch,plate,NULL,TO_ROOM);
}*/


void hound_hunt( CHAR_DATA *ch, CHAR_DATA *victim )
{
    extern char * const dir_name[];
    int roll, dir;

    WAIT_STATE( ch, skill_table[gsn_hunt].beats );
    roll = (number_range(1,100));
    if (roll < 33)
      act( "$n carefully sniffs the air.", ch, NULL, NULL, TO_ROOM );
    else if (roll < 66)
        act( "$n carefully checks the ground for tracks.", ch, NULL, NULL, TO_ROOM );
    else
        act( "$n listens carefully for some sounds.", ch, NULL, NULL, TO_ROOM );
    dir = find_first_step(ch->in_room, ch->hunting->in_room, 1024, TRUE, NULL);
    return;
    if (dir<0)
    {
      if (ch->pIndexData == get_mob_index(MOB_VNUM_SHADOW))
        act("The shadow grows still as if awating something.", ch, NULL, NULL, TO_ROOM );
      else
        {
          act( "$n says '`#Damn!  Lost $M!``'", ch, NULL, ch->hunting, TO_ROOM );
          ch->hunting = NULL;
          if (IS_SET(ch->act,ACT_AGGRESSIVE) && ch->in_room != NULL)
            {
              act( "$n slowly disappears.",ch,NULL,NULL,TO_ROOM );
              extract_char( ch, TRUE );
            }
          return;
        }
    }
    if( number_percent () > 75 || IS_AFFECTED2(ch,AFF_MISDIRECTION))
    {
        do
        {
            dir = number_door();
        }
        while( ( ch->in_room->exit[dir] == NULL ) || ( ch->in_room->exit[dir]->to_room == NULL ) );
    }
    if ( IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED ) && !(IS_AFFECTED(ch,AFF_PASS_DOOR) && !IS_SET(ch->in_room->exit[dir]->exit_info,EX_NOPASS)))
    {
        do_open( ch, (char *) dir_name[dir] );
        return;
    }
    if (!IS_SET(ch->in_room->exit[dir]->to_room->room_flags, ROOM_NO_MOB)
    && ( !IS_SET(ch->act, ACT_STAY_AREA) || ch->in_room->exit[dir]->to_room->area == ch->in_room->area ) )
        move_char( ch, dir, FALSE );
}

void knight_vanguard( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *squire, *vch;
    OBJ_DATA* wield;

    AFFECT_DATA af;
    int i;
    char buf[MSL];

/* Checks to see if the caster is a Knight */
    act("You kneel as you ask for $g's servant in your cause.",ch,NULL,NULL,TO_CHAR);
    act("$n kneels and closes $s eyes solemnly.",ch,NULL,NULL,TO_ROOM);

    if (get_summoned(ch, MOB_VNUM_VANGUARD) > 0)
    {
      for (vch = ch->in_room->people; vch ;vch = vch->next){
	if (IS_NPC(vch)
	    && vch->pIndexData->vnum == MOB_VNUM_VANGUARD
	    && vch->master == ch){
	  do_say(vch, "Think you need more help then I provide?");
	  act("$n towers over $N momentarly as if disgusted then fades away.", vch, NULL, ch, TO_NOTVICT);
	  act("$n towers over you momentarly as if disgusted then fades away.", vch, NULL, ch, TO_ROOM);
	  act("You tower over $N momentarly disgusted at $s actions and leave.", vch, NULL, ch, TO_CHAR);
	  die_follower(ch, TRUE);
	}
      }
      send_to_char("You have already been given a holy servant!\n\r",ch);
      return;
    }

    if (is_affected(ch,sn))
    {
        send_to_char("You send your prayers for a Vanguard yet the heavens ring silent.\n\r", ch);
        return;
    }

    squire = create_mobile( get_mob_index(MOB_VNUM_VANGUARD) );
    for (i=0;i < MAX_STATS; i++)
        squire->perm_stat[i] = ch->perm_stat[i];

    squire->max_hit = ch->max_hit;
    squire->hit = squire->max_hit;

    squire->max_mana = ch->max_mana;
    squire->mana = squire->max_mana;

    squire->hitroll    = ch->level / 4;
    squire->damroll    = ch->level / 3;
    squire->damage[DICE_NUMBER]	= 7;
    squire->damage[DICE_TYPE]	= 7;
    squire->alignment = ch->alignment;
    squire->level = level + 1;

    for (i=0; i < 3; i++)
        squire->armor[i] = interpolate(squire->level,100,-200);
    squire->armor[3] = interpolate(squire->level,100,-100);
    squire->gold = 0;


    sprintf( buf, squire->short_descr, ch->name );
    free_string( squire->short_descr );
    squire->short_descr = str_dup( buf );
    sprintf( buf, squire->long_descr, ch->name );
    free_string( squire->long_descr );
    squire->long_descr = str_dup( buf );


    sprintf( buf, squire->description, ch->name );
    free_string( squire->description );
    squire->description = str_dup( buf );

    /* equip it */
    wield = create_object( get_obj_index(OBJ_VNUM_VANGUARD_WEP), squire->level );
    obj_to_ch(wield, squire);
    wear_obj(squire, wield, TRUE, FALSE);
    SET_BIT(squire->affected_by, AFF_CHARM);
    squire->summoner = ch;
    add_follower(squire,ch);
    squire->leader=ch;
    ch->pet = squire;

    char_to_room(squire,ch->in_room);

    act("Crackling with $g's power $N fades into existance!",ch,NULL,squire,TO_ALL);
    if (ch->race == race_lookup("ogre")){
      sprintf(buf, "An ogre master? This is ridiculous, they can't even talk properly!");
      check_social(squire, "spit", "");
    }
    else
      sprintf(buf, "What heathen shall fall before me this day %s?", ch->name);
    do_say(squire, buf);

    af.type               = sn;
    af.level              = level;
    af.duration           = 96;
    af.bitvector          = 0;
    af.modifier           = 0;
    af.location           = APPLY_NONE;
    affect_to_char(ch, &af);
    squire->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;

}


/* upgrades a knight servant to a new level */
void knight_pet_upgrade( CHAR_DATA* ch, int sn ){
  MOB_INDEX_DATA* pMobIndex;
  CHAR_DATA* mob, *old;
  AFFECT_DATA af;
  char buf [MSL];

  const int level = ch->level;

  /* values that will be used for mobs hit/dam, dice and level */
  int hp_mod = 100;
  int hit = level / 5;
  int dam = level / 5;
  int die1 = 6;
  int die2 = 6;
  int mob_level = UMAX(1, ch->level - 5);
  int mob_vnum = MOB_VNUM_KNIGHT_1;
  int i;

  if (ch->pet == NULL){
    send_to_char("You do not have a servant to upgrade.\n\r", ch );
    return;
  }
  if (IS_NPC(ch) || ch->pCabal == NULL || ch->pcdata->rank < 1){
    send_to_char("You cannot upgrade servants.\n\r", ch );
    return;
  }

  switch( ch->pet->pIndexData->vnum){
  case MOB_VNUM_KNIGHT_1:
    ch->pet->level = mob_level;
    if (ch->pcdata->rank < 2){
      send_to_char("You may not upgrade your servant further at this rank.\n\r", ch);
      return;
    }
    mob_vnum = MOB_VNUM_KNIGHT_2;
    hp_mod = 115;
    hit = level / 5;
    dam = level / 4;
    die1 = 6;
    die2 = 7;
    mob_level = level - 2;
    break;
  case MOB_VNUM_KNIGHT_2:
    mob_level = level - 2;
    ch->pet->level = mob_level;

    if (ch->pcdata->rank < 3){
      send_to_char("You may not upgrade your servant further at this rank.\n\r", ch);
      return;
    }
    hp_mod = 130;
    mob_vnum = MOB_VNUM_KNIGHT_3;
    hit = level / 4;
    dam = level / 4;
    die1 = 6;
    die2 = 7;
    mob_level = level;
    break;
  case MOB_VNUM_KNIGHT_3:
    mob_level = level;
    ch->pet->level = mob_level;

    if (ch->pcdata->rank < 4){
      send_to_char("You may not upgrade your servant further at this rank.\n\r", ch);
      return;
    }
    old = ch->pet;
    die_follower( ch->pet, TRUE );
    extract_char( old, TRUE );

    knight_vanguard(sn, level + 1, ch, ch, 0);
    return;
    break;
  case MOB_VNUM_VANGUARD:
    ch->pet->level = level + 1;
    send_to_char("You may not upgrade your servant further.\n\r", ch  );
    return;
  default:
    send_to_char("Error: Servant type not found.\n\r", ch);
    return;
  }

/* now we try to get the mob index */
  if ( (pMobIndex = get_mob_index( mob_vnum )) == NULL){
    bug("knight_pet_upgrade: could not get mob index %d", mob_vnum );
    return;
  }
/* load the mob */
  if ( (mob = create_mobile( pMobIndex)) == NULL){
    bug("knight_pet_upgrade: could not load mob %d", mob_vnum );
    return;
  }

  for (i=0;i < MAX_STATS; i++)
    mob->perm_stat[i] = ch->perm_stat[i];

  mob->max_hit = hp_mod * ch->max_hit / 100;
  mob->hit = mob->max_hit;

  mob->max_mana = ch->max_mana;
  mob->mana = mob->max_mana;

  mob->hitroll    = hit;
  mob->damroll    = dam;
  mob->damage[DICE_NUMBER]	= die1;
  mob->damage[DICE_TYPE]	= die2;
  mob->alignment = ch->alignment;
  mob->level = mob_level;

  for (i=0; i < 3; i++)
    mob->armor[i] = interpolate(mob->level,100,-100);
  mob->armor[3] = interpolate(mob->level,100,0);
  mob->gold = 0;

  sprintf( buf, mob->short_descr, ch->name );
  free_string( mob->short_descr );

  mob->short_descr = str_dup( buf );
  sprintf( buf, mob->long_descr, ch->name );

  free_string( mob->long_descr );
  mob->long_descr = str_dup( buf );

  sprintf( buf, mob->description, ch->name );
  free_string( mob->description );

  mob->description = str_dup( buf );
  mob->trust = 6969;
  char_to_room(mob,ch->in_room);
  mob->trust = level;;

  old = ch->pet;
  die_follower( ch->pet, TRUE );
  extract_char( old, TRUE );

  act("$N arrives at a galop and halts beside $n!",ch, NULL, mob, TO_ROOM);
  act("$N arrives at a galop and halts beside you!",ch, NULL, mob, TO_CHAR);

  af.type               = sn;
  af.level              = level;
  af.duration           = IS_ELDER(ch)? 24 : 48;
  af.bitvector          = 0;
  af.modifier           = 0;
  af.location           = APPLY_NONE;
  affect_to_char(ch, &af);

  SET_BIT(mob->affected_by, AFF_CHARM);
  mob->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
  mob->summoner = ch;
  add_follower(mob,ch);
  mob->leader = ch;
  ch->pet = mob;
}

void spell_squire( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *squire;
//  AFFECT_DATA af;
  int i;
  char buf[MSL];

/* cap to ignore spell level boost */
  level = UMIN(ch->level, level);

  if (!can_follow(ch, ch))
    return;

  if (ch->pet){
    if (ch->pet->pIndexData->vnum != MOB_VNUM_KNIGHT_1
	&& ch->pet->pIndexData->vnum != MOB_VNUM_KNIGHT_2
	&& ch->pet->pIndexData->vnum != MOB_VNUM_KNIGHT_3
	&& ch->pet->pIndexData->vnum != MOB_VNUM_VANGUARD){
      sendf(ch, "You must first get rid of %s.\n\r", PERS2(ch->pet));
      return;
    }
    if (is_affected(ch,sn)){
      send_to_char("You are not ready to upgrade your servant yet.\n\r", ch );
      return;
    }
    knight_pet_upgrade( ch, sn );
    return;
  }
  if (is_affected(ch,sn)){
    send_to_char("You cannot call onto another servant yet.\n\r", ch);
    return;
  }

  squire = create_mobile( get_mob_index(MOB_VNUM_KNIGHT_1) );
  for (i=0;i < MAX_STATS; i++)
    squire->perm_stat[i] = ch->perm_stat[i];

  squire->max_hit = ch->max_hit;
  squire->hit = squire->max_hit;

  squire->max_mana = ch->max_mana;
  squire->mana = squire->max_mana;

  squire->hitroll    = ch->level / 5;
  squire->damroll    = ch->level / 5;
  squire->damage[DICE_NUMBER]	= 6;
  squire->damage[DICE_TYPE]	= 6;
  squire->alignment = ch->alignment;
  squire->level = level - 5;

  for (i=0; i < 3; i++)
    squire->armor[i] = interpolate(squire->level,100,-100);
  squire->armor[3] = interpolate(squire->level,100,0);
  squire->gold = 0;

  sprintf( buf, squire->short_descr, ch->name );
  free_string( squire->short_descr );

  squire->short_descr = str_dup( buf );
  sprintf( buf, squire->long_descr, ch->name );

  free_string( squire->long_descr );
  squire->long_descr = str_dup( buf );

  sprintf( buf, squire->description, ch->name );
  free_string( squire->description );

  squire->description = str_dup( buf );
  squire->race = ch->race;
  squire->pCabal = ch->pCabal;

  char_to_room(squire,ch->in_room);

  act("$N arrives at a galop and halts beside $n!",ch, NULL, squire, TO_ROOM);
  act("$N arrives at a galop and halts beside you!",ch, NULL, squire, TO_CHAR);

/* allow them to call on them right away
  af.type               = sn;
  af.level              = level;
  af.duration           = IS_ELDER(ch)? 12 : 24;
  af.bitvector          = 0;
  af.modifier           = 0;
  af.location           = APPLY_NONE;
  affect_to_char(ch, &af);
*/
  SET_BIT(squire->affected_by, AFF_CHARM);
  squire->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
  squire->summoner = ch;
  add_follower(squire,ch);
  squire->leader = ch;
  ch->pet = squire;
}

/*void spell_dragonsword( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    int sword_vnum;
    OBJ_DATA *sword;
    char arg[MIL];
    target_name = one_argument(target_name, arg);
    sword_vnum = 0;
    if (!str_cmp(arg, "sword"))
        sword_vnum = OBJ_VNUM_DRAGONSWORD;
    else if (!str_cmp(arg, "mace"))
        sword_vnum = OBJ_VNUM_DRAGONMACE;
    else if (!str_cmp(arg, "dagger"))
        sword_vnum = OBJ_VNUM_DRAGONDAGGER;
    else
    {
        send_to_char("You can't make a DragonSword like that!", ch);
        return;
    }
    sword = create_object( get_obj_index(sword_vnum), level);
    sword->timer = level;
    sword->cost  = 0;
    obj_to_char(sword, ch);
    act("You create $p!",ch,sword,NULL,TO_CHAR);
    act("$n creates $p!",ch,sword,NULL,TO_ROOM);
}*/

/*void spell_entangle( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    int dam;
    if (ch->in_room->sector_type == SECT_INSIDE || ch->in_room->sector_type == SECT_CITY || ch->in_room->sector_type == SECT_AIR)
    {
        send_to_char("No plants can grow here.\n\r", ch);
        return;
    }
    dam = number_range(level, 4 * level);
    if ( saves_spell( level, victim ) )
        dam /= 2;
    damage(ch,victim,ch->level,gsn_entangle,DAM_PIERCE);
    act("The thorny plants spring up around $n, entangling $s legs!", victim, NULL, NULL, TO_ROOM);
    act("The thorny plants spring up around you, entangling your legs!", victim, NULL, NULL, TO_CHAR);
    if (!is_affected(victim,gsn_entangle))
    {
        af.type = gsn_entangle;
        af.level = level;
        af.duration = level / 10;
        af.location = APPLY_DEX;
        af.modifier = -3;
        af.bitvector = 0;
        affect_to_char( victim, &af);
    }
}*/

/*void spell_love_potion( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    af.type               = sn;
    af.level              = level;
    af.duration           = 50;
    af.bitvector          = 0;
    af.modifier           = 0;
    af.location           = APPLY_NONE;
    affect_to_char(ch, &af);
    send_to_char("You feel like looking at people.\n\r", ch);
}*/

void spell_disperse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *vch, *vch_next;
    AFFECT_DATA af;


    if ( is_affected( ch, sn ) )
    {
        send_to_char("You aren't up to dispersing this crowd.\n\r",ch);
        return;
    }

    act("You attempt to disperse the crowds.", ch, NULL, NULL, TO_CHAR);
    act("$n attempts to disperse the crowds.", ch, NULL, NULL, TO_ROOM);

    /* blink any person in the room out if the do not save*/
    for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next_in_room;
	//anti abuse check
	if (IS_NPC(vch) && !(IS_SET(vch->act, ACT_TOO_BIG)
			     || IS_SET(vch->act, ACT_PRACTICE)
			     || IS_SET(vch->act, ACT_TRAIN)
			     || IS_SET(vch->act, ACT_NOPURGE)
			     || IS_SET(vch->act, ACT_IS_HEALER)
			     || IS_SET(vch->act, ACT_GAIN)
			     || IS_IMMORTAL(vch)
			     || IS_SET(vch->act, ACT_NO_WANDER)))
	  continue;
        if ( vch->in_room != NULL
	     && !IS_SET(vch->in_room->room_flags, ROOM_NO_RECALL)
	     && (( IS_NPC(vch) && !IS_SET(vch->act, ACT_AGGRESSIVE) )
		 || (!IS_NPC(vch)  && (vch->level < level )
		     && (!saves_spell(level,vch, DAM_OTHER,skill_table[sn].spell_type)
			 || IS_SET(vch->vuln_flags,VULN_SUMMON))))
	     && vch != ch && !IS_SET(vch->imm_flags, IMM_SUMMON))
	  {
	    a_yell( ch, vch );
	    spell_blink(sn, level, vch, vch, TARGET_CHAR);
	  }
    }
    af.type      = sn;
    af.level	 = level;
    af.duration  = 12;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char( ch, &af );
}

/*void spell_honor_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;
    if (is_affected(victim, sn))
    {
        if (victim == ch)
            send_to_char("But you're already protected by your honor.\n\r", ch);
        else
	    sendf(ch, "%s is already protected by their honor.\n\r", PERS2(victim));
        return;
    }
    af.type      = sn;
    af.level	 = level;
    af.duration  = 24;
    af.modifier  = -30;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    spell_remove_curse(skill_lookup("remove curse"), level, ch, victim);
    spell_bless(skill_lookup("bless"), level, ch, victim);
    if (ch = victim)
    {
        act("Your honor protects you.", ch,NULL,victim,TO_CHAR);
        act("$n's honor protects $m.", ch,NULL,victim,TO_ROOM);
    }
    else
    {
        act_new("$n's honor protects you.", ch,NULL,victim,TO_VICT,POS_DEAD);
        act("Your honor protects $N.", ch,NULL,victim,TO_CHAR);
        act("$n's honor protects $N.", ch,NULL,victim,TO_NOTVICT);
    }
}*/

void do_spellbane(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    if (!IS_IMMORTAL(ch))
	return;

    if (is_affected(ch,gsn_spellbane))
    {
        send_to_char("You are already deflecting spells.\n\r",ch);
        return;
    }
    if (ch->mana < 50)
    {
        send_to_char("You cannot muster up the energy.\n\r",ch);
        return;
    }
    WAIT_STATE( ch, skill_table[gsn_spellbane].beats );
    if (number_percent() < get_skill(ch,gsn_spellbane))
    {
        af.type   = gsn_spellbane;
        af.level  = ch->level;
        af.duration = ch->level / 4;
        af.location = APPLY_SAVING_SPELL;
        af.modifier = -ch->level/4;
        af.bitvector = 0;
        affect_to_char(ch,&af);
        ch->mana -= 50;
        act("Your hatred of magic surrounds you.",ch,NULL,NULL,TO_CHAR);
        act("$n fills the air with $s hatred of magic.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_spellbane,TRUE,1);
    }
    else
    {
        ch->mana -= 25;
        act("You get red in the face, but nothing happens.",ch,NULL,NULL,TO_CHAR);
        act("$n makes some wild and angry gestures, but nothing happens.", ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_spellbane,FALSE,1);
    }
}

void do_ram( CHAR_DATA *ch, char *argument )
{
    int door = -1, np = number_percent(), chance = 0;
    char arg[MIL], buf[MSL];
    ROOM_INDEX_DATA *to_room;
    CHAR_DATA *gch;
    EXIT_DATA *pexit, *pexit_rev;
    one_argument( argument, arg );
    chance = 3*get_skill(ch,gsn_ram)/4;
    if (chance < 1){
      send_to_char("Huh?\n\r", ch);
      return;
    }
    if (arg[0] == '\0')
    {
	send_to_char("Ram what?\n\r",ch);
	return;
    }
    door = find_door( ch, arg );
    if ( door < 0 )
        return;
    pexit = ch->in_room->exit[door];
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
	{
            act( "$N is standing too close to the door for you to ram it.",ch, NULL, gch, TO_CHAR );
	    return;
	}
    WAIT_STATE( ch, skill_table[gsn_ram].beats );

    if (chance > 0)
        chance += 2*(get_curr_stat(ch,STAT_LUCK) - 16) ;
    if ( !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL(ch))
        { send_to_char( "Why would you want to ram an open door?\n\r",        ch ); return; }
    if ( pexit->key < 0 && !IS_IMMORTAL(ch))
        { send_to_char( "The door seems impervious to physical attacks.\n\r",     ch ); return; }
    if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
        { send_to_char( "No need to ram an unlocked door.\n\r",  ch ); return; }
    if ( IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL(ch))
        { send_to_char( "You bounce off the door with a clatter. Ouch! No luck.\n\r", ch ); return; }
    if ( !IS_SET(pexit->exit_info, EX_EASY) )
	chance = 3*chance/2;
    if ( !IS_SET(pexit->exit_info, EX_HARD) )
	chance = 2*chance/3;
    if ( !IS_SET(pexit->exit_info, EX_INFURIATING) )
	chance /= 2;
    if ( !IS_NPC(ch) && chance < np)
    {
    	act( "You smack into $d, but it remains standing.", ch, NULL, pexit->keyword, TO_CHAR );
    	act( "$n smacks into $d, but it remains standing.", ch, NULL, pexit->keyword, TO_ROOM );
        check_improve(ch,gsn_ram,FALSE,2);
        return;
    }
    REMOVE_BIT(pexit->exit_info, EX_LOCKED);
    REMOVE_BIT(pexit->exit_info, EX_CLOSED);
    act( "You smash headlong into the $d, and slam it wide open!", ch, NULL, pexit->keyword, TO_CHAR );
    act( "$n smashes headlong into the $d, and slams it wide open!", ch, NULL, pexit->keyword, TO_ROOM );
    if ( ( to_room = pexit->to_room ) != NULL && ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
    && pexit_rev->to_room == ch->in_room )
    {
        CHAR_DATA *rch;
        REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
        REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
        sprintf(buf,"The $d %s slams open!",dir_name2[rev_dir[door]]);
        for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
            act( buf, rch, NULL, pexit_rev->keyword, TO_CHAR );
    }
    check_improve(ch,gsn_ram,TRUE,2);
}

void do_challenge( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    AFFECT_DATA* paf;
    int j = 0;
    one_argument(argument,arg);
    /* first part is anyone, rest is cabal only */
    if (IS_NPC(ch))
	return;

    if (arg[0] == '\0')
    {
	send_to_char("Syntax:\n\rchallenge <char>\n\r"\
		     "challenge <accept/decline>\n\r"\
		     "\"ignore\" will prevent challenges.\n\r\n\r",ch);
	send_to_char("Current challenges:\n\r",ch);
	for (paf = ch->affected; paf; paf = paf->next){
	  if (paf->type == gen_challenge && paf->has_string && paf->bitvector){
	    j++;
	    sendf(ch, "%d.  %s%s\n\r", j, paf->string,
		  paf->duration >= 0 ? " Accept/Decline?" : "");
	  }
	}
	if (j == 0)
	    send_to_char("No one",ch);
	send_to_char("\n\r",ch);
	return;
    }
    else if (!str_cmp("decline", arg)){
      AFFECT_DATA* paf_next;
      CHAR_DATA* och;
      /* check if any gen's set duration as those are the ones waiting to be approved */
      /* decline one at a time */
      /* Kaslai:    This was 'for (paf = ch->affected; paf; paf = paf = paf->next)', but I removed a 'paf ='.
                    Feel free to restore it if unintended consequences occur.                                   */
	for (paf = ch->affected; paf; paf = paf_next){
	  paf_next = paf->next;
	  if (paf->type == gen_challenge && paf->duration >= 0
	      && paf->bitvector){
	    if ( (och = get_char(paf->string)) != NULL){
	      act_new("`&$N has declined your challenge.``", och, NULL, ch, TO_CHAR, POS_DEAD);
	      act_new("`&You have declined $n's challenge.``", och, NULL, ch, TO_VICT, POS_DEAD);
	      if (ch->pCabal != NULL && IS_SET(ch->pCabal->progress, PROGRESS_CHALL))
		update_challenge( ch, ch->name, och->name, 0, 0, 0, 1);
	    }
	    affect_remove(ch, paf);
	    break;
	  }
	}
	return;
    }
    else if (!str_cmp("accept", arg)){
      CHAR_DATA* och;
      AFFECT_DATA* baf;
      /* we grab the first gen with non negative duration *
      check if that name is around in the game, if so, we set
      infinite gens on both. */

      /* get the gen with original challenger */
      /* Kaslai:    This was 'for (paf = ch->affected; paf; paf = paf = paf->next)', but I removed a 'paf ='.
                    Feel free to restore it if unintended consequences occur.                                   */
      for (paf = ch->affected; paf; paf = paf->next){
	if (paf->type != gen_challenge || paf->duration < 0 || !paf->bitvector)
	  continue;
	/* check for original challenger */
	if ( (och = get_char(paf->string)) == NULL){
	  sendf(ch, "%s is no longer around to takeup your challenge.\n\r", paf->string);
	  return;
	}
	/*  check if we have already challaned that person */
	for (baf = ch->affected; baf; baf = baf->next){
	  if (baf->type != gen_challenge || baf->duration >= 0
	      || !baf->bitvector)
	    continue;
	  if (str_cmp(baf->string, och->name))
	    continue;
	  act_new("You have already challenged $N!", ch, NULL, och, TO_CHAR, POS_DEAD);
	  /* remove the challange waiting to be acepted */
	  affect_remove(ch, baf);
	  return;
	}

	/* check if challnger is not already in challange with acceptor */
	act_new("`&You have accepted $N's challenge, $E may not initiate attack for 5 hours.``", ch, NULL, och, TO_CHAR, POS_DEAD);
	act_new("`&$N has accepted your challenge, you may not initiate attack for 5 hours.``", och, NULL, ch, TO_CHAR, POS_DEAD);
	/* we set this paf to infinite duration, make another on on original
	   challanger, and set modifier to 5 so he cannot initiate combat. */
	paf->duration = -1;
	af.type = gen_challenge;
	af.level = ch->level;
	af.duration = -1;
	af.where = TO_NONE;
	af.bitvector = 0;
	af.location = APPLY_NONE;
	af.modifier = 5;
	paf = affect_to_char(och, &af);
	string_to_affect(paf, ch->name);
	return;
      }
    }

    /* We allowa non cabal member to challnage a cabal member but only
       at good oods */

    if ((victim = get_char_world(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }
    if (ch == victim)
    {
	send_to_char("Why would you challenge yourself?\n\r",ch);
	return;
    }
    if (IS_NPC(victim))
    {
	send_to_char("Why would you challenge a mob?\n\r",ch);
	return;
    }

    if (is_area_safe_quiet(ch,victim))
    {
	sendf(ch, "You can't challenge %s.\n\r", PERS2(victim));
	return;
    }
    if (is_ignore(victim,ch) && !IS_IMMORTAL(ch))
    {
	sendf(ch, "%s is ignoring you.\n\r", PERS2(victim));
	return;
    }

    if (is_affected(ch,gsn_mortally_wounded) )
    {
	send_to_char("You obtain no glory from challenging a mortally wounded person.\n\r",ch);
	return;
    }
    if (ch->desc == NULL)
    {
	sendf(ch, "%s is not all here right now.\n\r", PERS2(victim));
	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("They are currently in combat.  Wait your turn.\n\r",ch);
	return;
    }
    if (is_affected(victim, gen_challenge))
    {
        sendf(ch, "%s is already being challenged!\n\r", PERS2(victim));
        return;
    }
    if (ch->pCabal && ch->pcdata->member->rank < 2
	&& is_same_cabal(ch->pCabal, victim->pCabal)){
      send_to_char("You must challenge yourself with those not of your cabal first.\n\r", ch);
      return;
    }

    act_new("`&You have challenged $N to a fight, $e has 5 hours to decline.``",ch,NULL,victim,TO_CHAR,POS_DEAD);
    act_new("`&$n has challenged $N to a fight.``",ch,NULL,victim,TO_NOTVICT,POS_DEAD);
    act_new("`&$n has challenged you to a fight. Use \"challenge accept/decline\" to confirm or reject.``",ch,NULL,victim,TO_VICT,POS_DEAD);

    /* attach the gen onto  victim so he can decline / accept */
    af.type             = gen_challenge;
    af.level            = ch->level;
    af.duration         = 5;
    af.where		= TO_NONE;
    af.location         = APPLY_NONE;
    af.modifier         = 0;
    af.bitvector        = TRUE;//challenge
    paf = affect_to_char(victim,&af);
    string_to_affect(paf, ch->name);
}

/* smiliar to challange but gives no chance to accept and is more limited */
void do_deathmark( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    AFFECT_DATA* paf;
    int j = 0;
    one_argument(argument,arg);
    /* first part is anyone, rest is cabal only */
    if (IS_NPC(ch))
	return;

    if (ch->pCabal == NULL || !IS_SET(ch->pCabal->progress, PROGRESS_CHALL)){
      send_to_char("You cannot deathmark!\n\r", ch);
      return;
    }

    if (arg[0] == '\0')
    {
      send_to_char("Syntax:	dmark <char>\n\r", ch);
      send_to_char("Current deathmarks:\n\r",ch);
      for (paf = ch->affected; paf; paf = paf->next){
	if (paf->type == gen_challenge && paf->has_string && !paf->bitvector){
	  j++;
	  sendf(ch, "%d.  %s\n\r", j, paf->string);
	}
      }
      if (j == 0)
	send_to_char("None",ch);
      send_to_char("\n\r",ch);
      return;
    }

   if ((victim = get_char_world(ch,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }
    if (ch == victim)
    {
	send_to_char("Why would you place a deathmark on yourself?\n\r",ch);
	return;
    }
    if (IS_NPC(victim))
    {
	send_to_char("Why would you place a deathmark on a mob?\n\r",ch);
	return;
    }
    if (is_area_safe_quiet(ch,victim))
    {
	sendf(ch, "You can't place a deathmark on %s.\n\r", PERS2(victim));
	return;
    }
    if (is_affected(ch,gsn_mortally_wounded) )
    {
	send_to_char("You obtain no glory from placing a deathmark on a mortally wounded person.\n\r",ch);
	return;
    }
    if (ch->desc == NULL)
    {
	sendf(ch, "%s is not all here right now.\n\r", PERS2(victim));
	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("Wait your turn.\n\r",ch);
	return;
    }

    if (is_affected(victim, gen_challenge))
    {
        sendf(ch, "%s is already being challenged!\n\r", PERS2(victim));
        return;
    }
    if (ch->pCabal && ch->pcdata->member->rank < 2
	&& is_same_cabal(ch->pCabal, victim->pCabal)){
      send_to_char("You must challenge yourself with those not of your cabal first.\n\r", ch);
      return;
    }
    act_new("`&You have placed a deathmark on $N!``",ch,NULL,victim,TO_CHAR,POS_DEAD);
    act_new("`&$n has placed a deathmark on $N!``",ch,NULL,victim,TO_NOTVICT,POS_DEAD);
    act_new("`&$n has placed a deathmark on you!``",ch,NULL,victim,TO_VICT,POS_DEAD);
    act_new("`&You may not initiate attack for 1 hour.``", ch, NULL, ch, TO_CHAR, POS_DEAD);

    af.type             = gen_challenge;
    af.level            = ch->level;
    af.duration         = 12;
    af.where		= TO_AFFECTS;
    af.location         = APPLY_NONE;
    af.modifier         = 1;//must wait till end of tick
    af.bitvector        = 0;
    paf = affect_to_char(ch,&af);
    string_to_affect(paf, victim->name);
    /* set on victim */
    af.modifier = 0;
    paf = affect_to_char(victim,&af);
    string_to_affect(paf, ch->name);
}

void collect_trophy( CHAR_DATA *ch, CHAR_DATA* victim ){
  OBJ_DATA *obj;
  char buf[MSL];

  if (!is_affected(victim, gsn_mortally_wounded) ){
    sendf(ch, "%s is not standing still enough!\n\r",
	  PERS2(victim));
    return;
  }
  else if (ch->in_room && IS_ROOM2(ch->in_room, ROOM_MERCY)){
    send_to_char("Not in this room.\n\r", ch );
    return;
  }

  sprintf(buf,"AAAAAIIIIIIIEEEEE!!!!");
  do_yell(victim, buf);

  switch(number_range(0,4)){
  case (0) :
    obj = create_object( get_obj_index( OBJ_VNUM_WARLORD_BELT),ch->level);
    if (obj == NULL)
      return;
    act("`!You grab $N by the head and rip $S spine out.``",ch,NULL,victim,TO_CHAR);
    act("`!$n grabs you by the head and rips your spine out!``",ch,NULL,victim,TO_VICT);
    act("`!$n grabs $N by the head and rips $S spine out.``",ch,NULL,victim,TO_NOTVICT);
    act("`!$n's headless body twitches violently.``",victim,NULL,NULL,TO_ROOM);
    break;
    case (1) :
      obj = create_object( get_obj_index(OBJ_VNUM_WARLORD_EARRING),ch->level);
      if (obj == NULL)
	return;
      act("`!You strike $N's head with a blunt object and knock the eyes from their sockets.``",ch,NULL,victim,TO_CHAR);
      act("`!$n strikes you with a blunt object, knocking your eyes from their sockets!``",ch,NULL,victim,TO_VICT);
      act("`!$n strikes $N with a blunt object, knocking $S eyes from their sockets.``",ch,NULL,victim,TO_NOTVICT);
      act("`!$n's head splits open and blood spills all over you.``",victim,NULL,NULL,TO_ROOM);
      act("`!You strikes $N with a blunt object, knocking $S eyes from their sockets.``",ch,NULL,victim,TO_CHAR);
      break;
  case (2) :
    obj = create_object( get_obj_index(OBJ_VNUM_WARLORD_PONCHO),ch->level);
    if (obj == NULL)
      return;
    act("`!$N screams wildly as you skin $M alive.``",ch,NULL,victim,TO_CHAR);
    act("`!You scream wildly as $n skins you alive!``",ch,NULL,victim,TO_VICT);
    act("`!$N screams wildly as $n skins $M alive.``",ch,NULL,victim,TO_NOTVICT);
    act("`!$n's skinless body rolls around wildly.``",victim,NULL,NULL,TO_ALL);
    break;
  case (3) :
    obj = create_object( get_obj_index(OBJ_VNUM_WARLORD_SKULL),ch->level);
    if (obj == NULL)
      return;
    act("`!You chop $N's head off and quickly extract the skull.``",ch,NULL,victim,TO_CHAR);
    act("`!$n cuts $N's head off and quickly extracts the skull.``",ch,NULL,victim,TO_NOTVICT);
    act("`!$n cuts your head off at which point you stop paying attention.``",ch,NULL,victim,TO_NOTVICT);

    act("`!$n's headless body kicks madly into the air.``",victim,NULL,NULL,TO_ROOM);
    break;
  default:
  case (4) :
    obj = create_object( get_obj_index(OBJ_VNUM_WARLORD_LIGHT),ch->level);
    if (obj == NULL)
      return;
    act("`!You scalp $N's head and hold the hair triumphantly.``",ch,NULL,victim,TO_CHAR);
    act("`!$n scalps $N's head and holds the hair triumphantly.``",ch,NULL,victim,TO_NOTVICT);
    act("`!$n scalps your head and holds the hair triumphantly!``",ch,NULL,victim,TO_VICT);
    break;
  }

  /* set the desc. up */
  sprintf( buf, obj->name, victim->name );
  free_string( obj->name );
  obj->name = str_dup ( buf );

  sprintf( buf, obj->short_descr, victim->name );
  free_string( obj->short_descr );
  obj->short_descr = str_dup ( buf );

  sprintf( buf, obj->description, victim->name );
  free_string( obj->description );
  obj->description = str_dup ( buf );

  /* owner it */
  set_owner( ch, obj, ch->pcdata->deity );

  /* set timer */
  obj->timer = 32768;	//30h real time.

  act("$n's lifeless body slumps to the floor.",victim,NULL,NULL,TO_ROOM);
  act( "$n is DEAD!!", victim, 0, 0, TO_ROOM );
  act("$n creates $p.",ch,obj,victim,TO_NOTVICT);
  obj_to_char(obj, ch);
  send_to_char( "You have been `1KILLED``!!\n\r", victim );
  raw_kill( ch, victim );
  WAIT_STATE(ch,24);
}

/*
   Syntax: trophy <character> :extracts trophy
	   trophy <object> <object> joings two trophies
*/

void do_trophy( CHAR_DATA* ch, char* argument){
  CHAR_DATA* victim;
  OBJ_DATA* src, *dest;
  AFFECT_DATA af, *paf;

  const int lag = skill_table[gsn_trophy].beats;
  const int cost = skill_table[gsn_trophy].min_mana;
  char arg1[MIL];

  if (ch->pCabal == NULL || !IS_SET(ch->pCabal->progress, PROGRESS_CHALL)
      || get_skill(ch, gsn_trophy) < 100){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  else if (IS_NULLSTR(argument)){
    send_to_char("Trophy who or what?\n\r",ch);
    return;
  }
/* COLLECT TROPHY */
  else if ( (victim = get_char_room( ch, NULL, argument)) != NULL){
    collect_trophy( ch, victim);
    return;
  }

  argument = one_argument( argument, arg1 );

  if (is_affected(ch, gsn_trophy)){
    send_to_char("But you've just made a trophy!\n\r", ch);
    return;
  }
  else if ( (src = get_obj_carry( ch, arg1, ch)) == NULL){
    sendf(ch, "You don't seem to be carrying \"%s\".\n\r", arg1 );
    return;
  }
  else if ( (dest = get_obj_carry( ch, argument, ch)) == NULL){
    sendf(ch, "You don't seem to be carrying \"%s\".\n\r", argument );
    return;
  }
  else if (src->vnum < OBJ_VNUM_WARLORD_BELT
	   || src->vnum > OBJ_VNUM_WARLORD_LIGHT){
    act("$p is not a trophy!", ch, src, NULL, TO_CHAR);
    return;
  }
  else if (dest->vnum < OBJ_VNUM_WARLORD_BELT
	   || dest->vnum > OBJ_VNUM_WARLORD_LIGHT){
    act("$p is not a trophy!", ch, dest, NULL, TO_CHAR);
    return;
  }
  else if (src == dest){
    send_to_char("You cannot use the object itself as parts!\n\r", ch);
    return;
  }
  else if (ch->move < cost){
    send_to_char("You lack the stamina to attempt it.\n\r", ch);
    return;
  }
  else
    ch->move -= cost;
  handle_skill_cost_percent( ch, ch->pCabal, gsn_trophy, 100 );

  /* we have to trophies, boost dest, destroy src */
  /* need to move affects over if not done yet */
  if (!dest->enchanted){
    AFFECT_DATA *af_new;
    dest->enchanted = TRUE;
    for (paf = dest->pIndexData->affected; paf != NULL; paf = paf->next) {
      af_new = new_affect();
      af_new->next = dest->affected;
      dest->affected = af_new;
      af_new->where= paf->where;
      af_new->type = UMAX(0,paf->type);
      af_new->level= paf->level;
      af_new->duration= paf->duration;
      af_new->location= paf->location;
      af_new->modifier= paf->modifier;
      af_new->bitvector= paf->bitvector;
    }
  }

  /* now we boost effects */
  for ( paf = dest->affected; paf; paf = paf->next){
    if (paf->location == APPLY_HIT && paf->modifier < 30){
      paf->modifier = 30;
      break;
    }
    else if (paf->location == APPLY_HITROLL && paf->modifier < 2){
      paf->modifier = 2;
      break;
    }
    else if (paf->location == APPLY_DAMROLL && paf->modifier < 2){
      paf->modifier = 2;
      break;
    }
  }
  dest->timer = 32768;

  act("You take apart $P and strengthen $p.", ch, dest, src, TO_CHAR);
  act("$n takes apart $P and strengthens $p.", ch, dest, src, TO_ROOM);
  obj_from_char( src );
  extract_obj( src);

  WAIT_STATE2( ch, lag );
  set_owner(ch, dest, NULL );

  af.type = gsn_trophy;
  af.level	= ch->level;
  af.duration	= 48;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( ch, &af);
}

void do_pixie_dust( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *ich;
    int chance;


    if ((chance = get_skill(ch, gsn_pixie_dust)) == 0)
    {
        send_to_char("Huh?\n\r",ch);
        return;
    }
    if (ch->mana < 15)
    {
        send_to_char("You don't have enough mana.\n\r",ch);
        return;
    }
    WAIT_STATE(ch, skill_table[gsn_pixie_dust].beats);
    if ( number_percent( ) > chance)
    {
	send_to_char("The pixies do not respond to your call.\n\r",ch);
	act("$n's beckon for pixies fails.",ch,NULL,NULL,TO_ROOM);
        ch->mana -= 7;
        check_improve(ch, gsn_pixie_dust, FALSE, 1);
	return;
    }
    ch->mana -= 15;
    act("You summon a pixie, sprinkling the room with golden dust.",ch,NULL,NULL,TO_CHAR);
    act("A summoned pixie covers the room with golden dust.",ch,NULL,NULL,TO_ROOM);
    for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
    {
        AFFECT_DATA af;
        if ( IS_AFFECTED(ich,AFF_FAERIE_FOG))
            continue;
        if (ich->invis_level > 0)
            continue;
        if ( ich == ch || is_safe(ch,ich) || saves_spell( ch->level-3, ich,DAM_OTHER,skill_table[gsn_pixie_dust].spell_type) )
            continue;
	a_yell( ch, ich );
        affect_strip ( ich, gsn_invis                   );
        affect_strip ( ich, gsn_mass_invis              );
        if ( IS_NPC(ich) || !IS_SET(race_table[ich->race].aff,AFF_SNEAK) )
        {
            affect_strip ( ich, gsn_sneak                       );
            REMOVE_BIT   ( ich->affected_by, AFF_SNEAK  );
        }
        REMOVE_BIT   ( ich->affected_by, AFF_HIDE       );
        REMOVE_BIT   ( ich->affected_by, AFF_INVISIBLE  );
        affect_strip(ich,gsn_camouflage);
        REMOVE_BIT   ( ich->affected2_by, AFF_CAMOUFLAGE        );
        af.where     = TO_AFFECTS;
        af.type      = gsn_pixie_dust;
        af.level     = ch->level;
        af.duration  = number_range(0,2);
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = AFF_FAERIE_FOG;
        affect_to_char( ich, &af );
        act_new( "The pixie dust causes you to start glowing.", ich, NULL, NULL, TO_CHAR, POS_DEAD );
        act( "$n starts glowing from the pixie dust.", ich, NULL, NULL, TO_ROOM );
    }
    check_improve(ch, gsn_pixie_dust, TRUE, 1);
}

void do_apprehend( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *och, *och_next;
    bool found = FALSE;

    if (ch->pCabal == NULL || !IS_CABAL(get_parent(ch->pCabal), CABAL_JUSTICE)){
      send_to_char("Huh?\n\r", ch);
      return;
    }
    act("You give out the command to apprehend criminals.",ch,NULL,NULL,TO_CHAR);
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
      och_next = och->next_in_room;
      if ( !IS_NPC(och) )
	continue;
      if ( (och->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD || och->pIndexData->vnum == MOB_VNUM_HOUND)
	   && IS_AFFECTED(och, AFF_CHARM) && och->master == ch){
	found = TRUE;
	spec_special_guard(och);
      }
      else if (IS_SET(och->act2, ACT_LAWFUL) && !IS_AFFECTED(och, AFF_CHARM) && IS_AWAKE(och)){
	found = TRUE;
	spec_special_guard(och);
      }
    }
    if (!found)
      send_to_char("No one is here to take your command.\n\r",ch);
}



/* NEXUS CABAL */

/* binds power of two demons to a chaos blade */
void spell_torment_bind( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  OBJ_DATA* blade;

  if ( (blade = get_eq_char( ch, WEAR_WIELD)) == NULL){
    send_to_char("Without a Chaos Blade present, your call fails.\n\r", ch );
    return;
  }
  else if (is_affected(ch, sn )){
    send_to_char("You've already bound Torment's power to your blade!\n\r", ch );
    return;
  }
  if (affect_find(blade->affected, gen_malform) == NULL
      && blade->vnum != OBJ_VNUM_CHAOS_BLADE){
    send_to_char("You cannot bind a torment to this weapon.\n\r",ch);
    return;
  }
  /* setup the common stuff */
  af.type = sn;
  af.level = level;
  af.duration = blade->timer > 0 ? blade->timer : 12;
  af.where = TO_AFFECTS;
  af.bitvector = 0;


  if (!str_prefix( target_name, "Bael" )){
    act("$n raises $p as Bael's power embraces it!", ch, blade, NULL, TO_ROOM );
    act("You raise $p as Bael's power embraces it!", ch, blade, NULL, TO_CHAR );

    af.location = APPLY_HITROLL;
    af.modifier = number_fuzzy(8);
  }
  else{
    act("$n raises $p as Moloch's power embraces it!", ch, blade, NULL, TO_ROOM );
    act("You raise $p as Moloch's power embraces it!", ch, blade, NULL, TO_CHAR );

    af.location = APPLY_AFFL_LEVEL;
    af.modifier = 1;
  }
  affect_to_char( ch, & af );
}


void spell_serpent( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA *serpent;
  AFFECT_DATA af;
  int i;
  int rank = 0;
  char buf[MSL];

  if (IS_NPC(ch))
    return;
  if (!can_follow(ch, ch))
    return;

/* cap to ignore spell level boost */
  level = UMIN(ch->level, level);

  if (ch->pet){
    sendf(ch, "You must first get rid of %s.\n\r", PERS2(ch->pet));
    return;
  }
  else if (is_affected(ch,sn)){
    send_to_char("You cannot call onto another steed yet.\n\r", ch);
    return;
  }

  rank = URANGE(1, ch->pcdata->rank, 3);

  if (IS_UNDEAD(ch))
    i = UMIN(MOB_VNUM_SERPENT_U3, MOB_VNUM_SERPENT_U1 + rank - 1);
  else if (IS_DEMON(ch))
    i = UMIN(MOB_VNUM_SERPENT_D3, MOB_VNUM_SERPENT_D1 + rank - 1);
  else
    i = UMIN(MOB_VNUM_SERPENT_3,  MOB_VNUM_SERPENT_1 + rank - 1);

  serpent = create_mobile( get_mob_index( i ) );

  for (i=0;i < MAX_STATS; i++)
    serpent->perm_stat[i] = ch->perm_stat[i];

  serpent->max_hit = 2 * ch->max_hit * rank;
  serpent->hit = serpent->max_hit;

  serpent->hitroll   = ch->level / 10;
  serpent->damroll    = ch->level / 10;
  serpent->damage[DICE_NUMBER]	= 2 + rank;
  serpent->damage[DICE_TYPE]	= 2 + rank;
  serpent->alignment = ch->alignment;
  serpent->level = level - 6 + (2 * rank);

  for (i=0; i < 3; i++)
    serpent->armor[i] = interpolate(serpent->level,100,-100);
  serpent->armor[3] = interpolate(serpent->level,100,0);
  serpent->gold = 0;

  sprintf( buf, serpent->short_descr, ch->name );
  free_string( serpent->short_descr );

  serpent->short_descr = str_dup( buf );
  sprintf( buf, serpent->long_descr, ch->name );

  free_string( serpent->long_descr );
  serpent->long_descr = str_dup( buf );

  sprintf( buf, serpent->description, ch->name );
  free_string( serpent->description );

  serpent->description = str_dup( buf );
  serpent->pCabal = ch->pCabal;

  char_to_room(serpent,ch->in_room);

  act("$N swoops from above and halts beside $n!",ch, NULL, serpent, TO_ROOM);
  act("$N swoops from above and halts beside you!",ch, NULL, serpent, TO_CHAR);

  af.type               = sn;
  af.level              = level;
  af.duration           = IS_ELDER(ch)? 12 : 24;
  af.bitvector          = 0;
  af.modifier           = 0;
  af.location           = APPLY_NONE;
  affect_to_char(ch, &af);

  SET_BIT(serpent->affected_by, AFF_CHARM);
  serpent->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
  serpent->summoner = ch;
  add_follower(serpent,ch);
  serpent->leader = ch;
  ch->pet = serpent;
}

/* allows to change the chaos blade */
void do_spawn_blade(CHAR_DATA *ch, char *argument){
  OBJ_DATA* blade;
  AFFECT_DATA af, *paf;
  const int sn = skill_lookup("spawn blade");
  const int lag = skill_table[sn].beats;
  const int cost = skill_table[sn].min_mana;

  if (get_skill(ch, sn) < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if ( (blade = get_eq_char( ch, WEAR_WIELD)) == NULL || blade->pIndexData->vnum != OBJ_VNUM_CHAOS_BLADE){
    send_to_char("You must first create a chaos blade.\n\r", ch );
    return;
  }
  else if (is_affected_obj( blade, sn )){
    act("You cannot change $p any further.", ch, blade, NULL, TO_CHAR);
    return;
  }

  if (IS_NULLSTR(argument)){
    send_to_char("Do you wish to Fragment, Elongate, or Fortify the Chaos Blade?\n\r", ch);
    return;
  }
  else if (ch->mana < cost ){
    send_to_char("You lack the mana neccessary.", ch );
    return;
  }
  else if (!str_prefix(argument, "fortify" )){
    blade->timer += 48;
    act("You've significantly extended $p's duration.", ch, blade, NULL, TO_CHAR);
    act("The aura of power around $p grows much stronger.", ch, blade, NULL, TO_ALL);
  }
  else if (!str_prefix(argument, "elongate" )){
    char *name;
    char *type;
    char buf[MIL];
    AFFECT_DATA* paf;

    if (IS_WEAPON_STAT(blade, WEAPON_TWO_HANDS)){
      send_to_char("You cannot elongate this weapon any further.",ch );
      return;
    }
    else if ( (get_eq_char( ch, WEAR_SHIELD ) || get_eq_char( ch, WEAR_HOLD )) && ch->size < SIZE_LARGE ){
      send_to_char("You must have a free hand to hold the larger weapon with.", ch);
      return;
    }

    /* get new name */
    switch( blade->value[0] ){
      case WEAPON_EXOTIC:	name = "Scythe";	break;
      case WEAPON_SWORD:	name = "Greatsword";	break;
      case WEAPON_DAGGER:	name = "Shortsword";	break;
      case WEAPON_SPEAR:	name = "Pike";		break;
      case WEAPON_MACE: 	name = "Deathstar";	break;
      case WEAPON_AXE:		name = "Battleaxe";	break;
      case WEAPON_FLAIL:	name = "Scourge";	break;
      case WEAPON_WHIP: 	name = "Knout";		break;
      case WEAPON_POLEARM: 	name = "Glaive";	break;
    default: name = "Blade";
    }
    /* get old type */

    if (blade->value[3] == attack_lookup("spray"))	type =  "Weeping";
    else if (blade->value[3] == attack_lookup("flame")) type =  "Flaming";
    else if (blade->value[3] == attack_lookup("chill"))	type =  "Frosted";
    else if (blade->value[3] == attack_lookup("divine"))type =  "Chaos";
    else						type =	"Chaos";

    act("Your pour the power of Nexus into $p and watch it grow.", ch, blade, NULL, TO_CHAR );
    act("$p begins to grow and double in size!", ch, blade, NULL, TO_ROOM );

    blade->value[4] |= (flag_value( weapon_type2, "twohands" ) != NO_FLAG
			? flag_value( weapon_type2, "twohands" ) : 0 );
    sprintf( buf, "%s chaos %s", type, name );
    free_string( blade->name );
    blade->name = str_dup( buf );

    sprintf( buf, "the %s %s", type, name );
    free_string( blade->short_descr );
    blade->short_descr = str_dup( buf );

    sprintf( buf, "the %s %s lies on the ground.", type, name );
    free_string( blade->description );
    blade->description = str_dup( buf );

    /* add timer */
    blade->timer += 24;

    unequip_char( ch, blade );
    /* add +3 hit/dam */
    for (paf = blade->affected; paf; paf = paf->next ){
      if (paf->location == APPLY_HITROLL){
	paf->modifier += 3;
	break;
      }
    }
    for (paf = blade->affected; paf; paf = paf->next ){
      if (paf->location == APPLY_DAMROLL){
	paf->modifier += 3;
	break;
      }
    }
    equip_char( ch, blade, WEAR_WIELD );
  }
  else if (!str_prefix(argument, "fragment" )){
    OBJ_DATA* frag;
    char buf[MIL];

    if (IS_WEAPON_STAT(blade, WEAPON_TWO_HANDS)){
      send_to_char("You cannot fragment this weapon.",ch );
      return;
    }
    else if ( (get_eq_char( ch, WEAR_SHIELD ) || get_eq_char( ch, WEAR_HOLD )) && ch->size < SIZE_LARGE ){
      send_to_char("You must have a free hand to hold the new weapon with.", ch);
      return;
    }
    else if (get_skill(ch, gsn_dual_wield) < 2){
      send_to_char("You do not know how to dual wield a weapon.\n\r", ch );
      return;
    }

    /* boost modifier */
    blade->timer += 12;

    /* we cut hit/dam modifiers in half */
    unequip_char( ch, blade );

    for (paf = blade->affected; paf; paf = paf->next ){
      if (paf->location == APPLY_HITROLL){
	paf->modifier /= 2 + 1;
	break;
      }
    }
    for (paf = blade->affected; paf; paf = paf->next ){
      if (paf->location == APPLY_DAMROLL){
	paf->modifier /= 2 + 1;
	break;
      }
    }

    /* get a blank obj */
    frag = create_object( get_obj_index(OBJ_VNUM_CHAOS_BLADE), ch->level);

    /* clone the new one */
    clone_object( blade, frag );

    /* adjust weight */
    frag->weight = blade->weight / 2;
    obj_to_char( frag, ch );

    /* add fragment word */
    sprintf( buf, "%s shard", blade->name );
    free_string( frag->name );
    frag->name = str_dup( buf );

    sprintf( buf, "%s shard", blade->short_descr );
    free_string( frag->short_descr );
    frag->short_descr = str_dup( buf );


    /* equip both */
    equip_char(ch, blade, WEAR_WIELD );
    equip_char(ch, frag, WEAR_SECONDARY );

    /* attach wait modifier to fragment */
    af.type = sn;
    af.level = ch->level;
    af.duration = -1;
    af.bitvector = 0;
    af.where = TO_NONE;
    af.modifier = 0;
    af.location = APPLY_NONE;
    affect_to_obj( frag, &af );

    act("On your command $p splits in twain!",ch, blade, NULL, TO_CHAR );
    act("On $n's command $p splits in twain!",ch, blade, NULL, TO_ROOM );

  }

  ch->mana -=  cost;
  WAIT_STATE2(ch, lag );

/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, sn);

  /* add the effect to the blade */
  af.type = sn;
  af.level = ch->level;
  af.duration = -1;
  af.bitvector = 0;
  af.where = TO_NONE;
  af.modifier = 0;
  af.location = APPLY_NONE;
  affect_to_obj( blade, &af );

}

void spell_chaos_blade( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  OBJ_DATA *blade;
  AFFECT_DATA af;

  static char * const material  [] = { "liquid",  "fire",    "ice",   "chaos" };
  static char * const dam_type  [] = { "spray",   "flame",   "chill", "divine" };
  static char * const spec_type [] = { "vorpal",  "sharp",    "frost", "shocking" };
  static char * const name [] = { "Weeping",  "Burning",  "Frosted", "Chaos" };
  char *wep_name;
  char buf[MIL];
  char arg[MIL];

  //Data
  int skill = get_skill(ch, sn);
  int type = number_range(0, 3);

  int dice1 = 3;
  int dice2 = 4;

  int dam =  0;//+3 if 2h
  int hit = -3;//+3 if 2h

  int wep_type = class_table[ch->class].weapon;
  int weight = 0;

  const int dur = 24;
  /*
    We create a chaos blade with dam roll /bonues based on level and skill.
    The blade is randomly made:
    vorpal/spray/water
    poisoned/disease/flesh
    icy/chill/ice
    shocking/divine/chaos

    blade is no-rem/no-drop/no-uncurse by default, and poofs on drop.
    weight is 10 to forstall removing by dropping str
    poofs after 24 ticks

    Elders can specify the blade, (first arg)

    Blade created is always of the same type as newbie weapon
  */

  if (IS_NPC(ch))
    return;

  if (is_affected(ch, sn)){
    send_to_char("Lacking the full power of Nexus the blade crumbles to dust.\n\r",ch);
      return;
  }

  target_name = one_argument(target_name, arg);
  if (IS_ELDER(ch) && arg[0] != '0'){
    bool fFound = FALSE;
    for (type = 0; type < 4; type ++){
      if (!str_prefix(material[type], arg)){
	  fFound = TRUE;
	  break;
      }
    }
    if (!fFound){
      send_to_char("Material should be one of:\n\r", ch);
      for (type = 0; type < 4; type ++)
	sendf(ch, "%s, ", material[type]);
      send_to_char("\n\r", ch);
      return;
    }
  }

  /* replace ice material for fire giants */
  if (ch->race == grn_fire && !str_cmp("ice",material[type]))
    type = 1;

/* choose what weapon type this is */
  if (get_skill(ch, gsn_mace_mastery) > 1){
    wep_type = WEAPON_MACE;
    wep_name = "Star";
    weight = 150;
  }
  else if (get_skill(ch, gsn_sword_mastery) > 1){
    wep_type = WEAPON_SWORD;
    wep_name = "Blade";
    weight = 180;
  }
  else if (get_skill(ch, gsn_spear_mastery) > 1){
    wep_type = WEAPON_SPEAR;
    wep_name = "Rod";
    weight = 170;
  }
  else if (get_skill(ch, gsn_staff_mastery) > 1){
    wep_type = WEAPON_STAFF;
    wep_name = "Cane";
    weight = 80;
  }
  else if (get_skill(ch, gsn_axe_mastery) > 1){
    wep_type = WEAPON_AXE;
    wep_name = "Cleaver";
    weight = 250;
  }
  else if (get_skill(ch, gsn_flail_mastery) > 1){
    wep_type = WEAPON_FLAIL;
    wep_name = "Chain";
    weight = 190;
  }
  else if (get_skill(ch, gsn_whip_mastery) > 1){
    wep_type = WEAPON_WHIP;
    wep_name = "Wire";
    weight = 90;
  }
  else if (get_skill(ch, gsn_polearm_mastery) > 1){
    wep_type = WEAPON_POLEARM;
    wep_name = "Glaive";
    weight = 280;
  }
  else if (get_skill(ch, gsn_dagger_mastery) > 1){
    wep_type = WEAPON_DAGGER;
    wep_name = "Shard";
    weight = 40;
  }
  else if (get_skill(ch, gsn_exotic_mastery) > 1){
    wep_type = WEAPON_EXOTIC;
    if (number_percent() < 5)
      wep_name = "Blonkeywan";
    else
      wep_name = "Katar";
    weight = 40;
  }
  else{
    wep_type = weapon_table[UMAX(0, ch->pcdata->start_wep)].type;
    switch (wep_type){
    default:
    case WEAPON_SWORD:	wep_name = "Blade";	weight = 180;	break;
    case WEAPON_MACE:	wep_name = "Star";	weight = 150;	break;
    case WEAPON_SPEAR:	wep_name = "Rod";	weight = 170;	break;
    case WEAPON_STAFF:	wep_name = "Cane";	weight = 60;	break;
    case WEAPON_AXE:	wep_name = "Cleaver";	weight = 130;	break;
    case WEAPON_FLAIL:  wep_name = "Chain";     weight = 90;	break;
    case WEAPON_WHIP:	wep_name = "Wire";	weight = 30;	break;
    case WEAPON_POLEARM:wep_name = "Glaive";    weight = 280;	break;
    case WEAPON_DAGGER: wep_name = "Shard";	weight = 40;	break;
    }
  }
  if (get_eq_char(ch, WEAR_WIELD) != NULL
      || (get_eq_char(ch, WEAR_HOLD) != NULL && get_eq_char(ch, WEAR_SHIELD) != NULL)){
    send_to_char("Your hands must be ready to wield the blade the moment it is created.\n\r", ch);
    return;
  }
  if ( (wep_type == WEAPON_POLEARM || wep_type == WEAPON_STAFF)
       && (get_eq_char(ch, WEAR_SHIELD) != NULL
	   || get_eq_char(ch, WEAR_HOLD) != NULL
	   || get_eq_char(ch, WEAR_SECONDARY) != NULL) ){
    send_to_char("Your hands must be ready to wield the blade the moment it is created.\n\r", ch);
    return;
  }

  blade = create_object( get_obj_index(OBJ_VNUM_CHAOS_BLADE),level);

  /* Set hit /dam roll*/
  dice1 += (level > 33? 1 :0)
    + (level > 38? 2 :0) + (level > 42? 1 :0);
  dice2 += (skill > 79? 1 : 0) + (skill > 84? 1 : 0) + (skill > 99? 2 : 0);

  dam += UMAX(1, dice1 - 1);
  hit += dice2 + (skill > 99? 2 : 0);

  /* set timer */
  blade->timer = dur + 1;

  /* set dice */
  blade->value[1] = dice1;
  blade->value[2] = dice2;

  /* set level, weight, etc */
  blade->level = UMIN(60, 10 + level);
  blade->weight = weight;
  blade->cost = 0;

  /* set evil/bless */
  if (IS_GOOD(ch))
    blade->extra_flags |= flag_value(extra_flags, "bless" );
  else if (IS_EVIL(ch))
    blade->extra_flags |= flag_value(extra_flags, "evil" );
  else
    blade->timer *= 2;


  /* set type, random material, attack, and special */
  blade->value[0] = wep_type;
  //material
  free_string(blade->material);
  blade->material = str_dup(material[type]);
  //dam type
  blade->value[3] = attack_lookup(dam_type[type]);
  //special
  blade->value[4] |= flag_value(weapon_type2,spec_type[type]);
/* staffs spears and polearms */
  if (wep_type == WEAPON_STAFF
      || wep_type == WEAPON_POLEARM){
    blade->value[4] |= flag_value(weapon_type2, "twohands");
    dam += 3;
    hit += 3;
  }


  /* set name, short/room desc */

  sprintf(buf, blade->name, name[type], wep_name);
  free_string(blade->name);
  blade->name = str_dup(buf);

  sprintf(buf, blade->short_descr, name[type], wep_name);
  free_string(blade->short_descr);
  blade->short_descr = str_dup(buf);

  sprintf(buf, blade->description, name[type], wep_name);
  free_string(blade->description);
  blade->description = str_dup(buf);

  /* set dam / hit */
  af.type = sn;
  af.level = level;
  af.duration = -1;

  af.where = TO_NONE;
  af.bitvector = 0;

  af.location = APPLY_DAMROLL;
  af.modifier = dam;
  affect_to_obj(blade, &af);

  af.location = APPLY_HITROLL;
  af.modifier = hit;
  affect_to_obj(blade, &af);


  /* set effect on character */
  af.type = sn;
  af.level = level;
  af.duration = dur;

  af.where = TO_AFFECTS;
  af.bitvector = 0;

  af.location = APPLY_NONE;
  af.modifier = 0;

  /* final check if we can hold the chaos blade */
  if (IS_WEAPON_STAT(blade, WEAPON_TWO_HANDS)
      && (get_eq_char(ch, WEAR_SHIELD) != NULL
	  || get_eq_char(ch, WEAR_SECONDARY) != NULL
	  || get_eq_char(ch, WEAR_HOLD) != NULL)){
    send_to_char("You must have both hands free to recive this weapon.\n\r", ch);
    return;
  }

  act("You create $p!",ch,blade,NULL,TO_CHAR);
  act("$n creates $p!",ch,blade,NULL,TO_ROOM);
  obj_to_char(blade,ch);

/* check if it zapped and dropped */
  if (blade->carried_by != ch)
    extract_obj( blade );
  else
    equip_char(ch, blade, WEAR_WIELD);

  if (!IS_IMMORTAL(ch))
    affect_to_char(ch, &af);
}

/* Nexus: Fixed transportation spell */
void spell_nexus( int sn, int level, CHAR_DATA *ch,void *vo,int target)
{
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *from_room = ch->in_room;

    CHAR_DATA* vch;
    CHAR_DATA* vch_next;

    int dam = 20 + number_range(level / 2, 3 * level / 2);

    if (IS_NPC(ch))
        return;

    location = get_room_index( ROOM_VNUM_DARK_RIFT );

    if (IS_SET(ch->in_room->room_flags,ROOM_NO_VORTEX) )
      {
        send_to_char( "Something prevents you from creating a rift.\n\r", ch );
        return;
      }
    else if (is_affected(ch,gen_ensnare)){
      send_to_char("You sense a powerful magical field preventing your departure.\n\r", ch);
      return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("You cannot open a vortex while fighting.\n\r",ch );
	return;
    }

    broadcast(ch, "Your vision ripples slightly.\n\rStrange indeed...\n\r");
    if (number_percent() > 2 * get_skill(ch, sn) / 3)
    {
      act("Reality seems to bend and sheer around you then realigns itself.", ch, NULL, NULL, TO_ALL);
	return;
    }

    act("Reality sheers then unravels as you open a gate to the Nexus.",ch,NULL,NULL,TO_CHAR);
    act("Reality sheers and parts around $n as $e fashions a gate to the Nexus.",ch,NULL,NULL,TO_ROOM);

    char_from_room(ch);
    char_to_room(ch,location);
    act("$n appears in the room.",ch,NULL,NULL,TO_ROOM);
    do_look(ch,"auto");

    send_to_char("The forces of Chaos tear into you!\n\r", ch);
    if (ch->race == race_lookup("demon")){
      send_to_char("You manage to slightly resist the onslaught.\n\r", ch);
      damage(ch, ch, 2 * dam / 3, attack_lookup("pain") + TYPE_HIT, DAM_INTERNAL, TRUE);
    }
    else
      damage(ch, ch, dam, attack_lookup("pain") + TYPE_HIT, DAM_INTERNAL, TRUE);

    /* We move the charmies/followers */
    for (vch = from_room->people; vch != NULL; vch = vch_next){
      vch_next = vch->next_in_room;
      if ( !vch->master
	   || !vch->leader
	   || vch->master != ch
	   || vch->leader != ch)
	continue;
      act("You follow $N into the land of Chaos.", vch, NULL, ch, TO_CHAR);
      act("$n follows $N into the land of Chaos.", vch, NULL, ch, TO_ROOM);

      char_from_room(vch);
      char_to_room(vch,location);
      act("$n appears in the room.",vch,NULL,NULL,TO_ROOM);
      do_look(vch,"auto");

      send_to_char("The forces of Chaos tear into you!\n\r", vch);
      if (ch->race == race_lookup("demon")){
	send_to_char("You manage to slightly resist the onslaught.\n\r", vch);
	damage(vch, vch, 2 * dam / 3, attack_lookup("pain") + TYPE_HIT, DAM_INTERNAL, TRUE);
      }
      else
	damage(vch, vch, dam, attack_lookup("pain") + TYPE_HIT, DAM_INTERNAL, TRUE);
    }

}

/* Defensive spell that can togle between reduce/increase damage on tick. */
void spell_discord( int sn, int level, CHAR_DATA *ch,void *vo,int target){

  AFFECT_DATA af, *paf;
  int dur = 24;

  if ((paf = affect_find(ch->affected, gen_discord)) != NULL){
    if (IS_ELDER(ch)){
      /* switch the protection/damage (TRUE = Protect)*/
      if (paf->modifier){
	send_to_char("The field of chaos around you pulsates angry red then fades.\n\r", ch);
	act("The air around $n pulsates angry red then fades.", ch, NULL, NULL, TO_ROOM);
	paf->modifier = FALSE;
      }
      else {
	send_to_char("The field of chaos around you emits a blue aura then fades.\n\r", ch);
	act("The air around $n emits a blue aura then fades.", ch, NULL, NULL, TO_ROOM);
	paf->modifier = TRUE;
      }
      return;
    }
    send_to_char("You have already surrounded yourself with chaos.\n\r", ch);
    return;
  }

  af.type = gen_discord;
  af.level = level;
  af.duration = dur;

  af.location = APPLY_NONE;
  af.modifier = 0;

  af.where = TO_AFFECTS;
  af.bitvector = 0;
  affect_to_char(ch, &af);
}


/* CHAOS ability for intangible charmie */
void do_mold( CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA af;
  const  int sn = skill_lookup("mold chaos");
  const int dur = 8;
  int skill = get_skill(ch, sn);
  int cost = _mana_cost(ch, sn);

  if (skill < 2){
    send_to_char("huh?.\n\r",ch);
    return;
  }

  if (ch->fighting){
    send_to_char("Not while in combat.\n\r", ch);
    return;
  }
  else if (ch->in_room->pCabal == NULL
	   || !is_same_cabal(ch->pCabal, ch->in_room->pCabal )
	   || ch->in_room->pCabal->anchor == NULL
	   || ch->in_room->pCabal->anchor->area != ch->in_room->area){
    send_to_char("You may only mold chaos within your own cabal.\n\r", ch);
    return;
  }
  if (is_affected(ch, sn))
    {
      send_to_char("You are not yet ready for another attempt.\n\r", ch);
      return;
    }

  if (ch->mana < cost)
    {
      send_to_char("You lack the power to shape chaos.\n\r", ch);
      return;
    }
  else
    ch->mana -= cost;

  WAIT_STATE2(ch,skill_table[sn].beats);

/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, sn);

  if (number_percent() < skill){
    af.type 	= sn;
    af.level 	= ch->level;
    af.duration	= dur;

    af.where	= TO_AFFECTS;
    af.bitvector = 0;

    af.location	= APPLY_NONE;
    af.modifier = 0;
    affect_to_char(ch,&af);

    /* start gen */
    af.type = gen_cspawn;
    af.where = TO_AFFECTS;
    affect_to_char(ch,&af);
    check_improve(ch, sn, TRUE, 1);
    return;
  }//END SUCCESS
  check_improve(ch, sn, FALSE, 1);
  send_to_char("You failed to do anything usefull.\n\r", ch);
}

/* Offensive spell, casts powerfull area attack coupled with dispel magic. */
void spell_void( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  CHAR_DATA *vch, *vch_next;
  AFFECT_DATA* paf;

  char buf[MSL];
  int dam;
  static const sh_int dam_each[] =
  {
    0,
    0,   0,   0,   0,   0,	  0,   0,   0,   0,   0,
    0,   0,   0,   0,  25,	 30,  35,  40,  45,  50,
    55,  60,  65,  70,  75,	 77,  79,  81,  83,  85,
    87,  89,  91,  93,  95,	 97,  98,  99, 100, 101,
    105, 110, 115, 120, 125,	127, 128, 129, 130, 140
  };

  level = UMIN(level, sizeof(dam_each)/sizeof(dam_each[0]) - 1);
  level = UMAX(0, level);
  dam	= number_range( dam_each[level], 3 * dam_each[level] / 2 );

  act("The very fabric of reality unravels itself as the Nexus swallows the area!", ch, NULL, NULL, TO_ALL);

  /* show message in the area */

  for ( vch = char_list; vch != NULL; vch = vch->next )
    {
      if ( vch->in_room == NULL )
	continue;

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
	   )
	send_to_char("The ground rumbles and shakes as reality ripples around you.\n\r", vch );
    }//end if same area.

  for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
      vch_next = vch->next_in_room;
      if (is_area_safe(ch,vch)
	  || vch == ch
	  || vch->race == race_lookup("demon")
	  )
	continue;

      if (ch->fighting != vch && vch->fighting != ch)
	{
	  sprintf(buf, "%s protect me, %s has called the Void!",
		  IS_NPC(vch)? "The One God" : vch->pcdata->deity,
		  PERS(ch,vch));
	  j_yell(vch, buf);
	}

      if (saves_spell(level,vch,DAM_NEGATIVE, skill_table[sn].spell_type))
	dam /= 2;
      if (number_percent() < level)
	spell_dispel_magic(skill_lookup("dispel magic"), level - 5, ch, (void *) vch, TARGET_CHAR);
      damage( ch, vch, dam, sn, DAM_NEGATIVE, TRUE );
    }
}


/* chooses which demon to conjure, actual work done in gen_conjure */
void do_conjure( CHAR_DATA* ch, char* argument ){
  AFFECT_DATA* paf, af;
  const int sn = skill_lookup("conjuration");
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;
  const int max_demon = 6;

  char* demons[] = {"Nyogtha", "Gromel", "Miscreant", "Aberrant", "Azazel", "Malignant"};
  int ranks[] = {2,2,2,3,3,3};

  int choice =  -1;
  int i = 0;
  if (get_skill(ch, sn ) < 2){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (IS_NULLSTR(argument)){
    bool fFound = FALSE;
    send_to_char("You can attempt to conjure following allies:\n\r", ch );
    for (i = 0; i < max_demon; i++){
      if (ch->pcdata->rank >= ranks[i]){
	sendf(ch, "%-20s level %d\n\r", demons[i], 30 + 5 * i);
	fFound = TRUE;
      }
    }
    if (fFound)
      send_to_char("None\n\r", ch );
    return;
  }

  if (is_affected(ch, sn )){
    send_to_char("You are not ready for another attempt.\n\r", ch);
    return;
  }

  /* try to chose a demon */
  for (i = 0; i < max_demon; i++){
    if (ch->pcdata->rank >= ranks[i] && !str_prefix(argument, demons[i]))
      choice = i;
  }

  if (choice < 0){
    send_to_char("You cannot call upon that ally.\n\r", ch);
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You cannot concentrate sufficiently to begin.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2( ch, lag );

/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, sn);

  act("You draw the Pentagram and begin the conjuration of $t.", ch, demons[choice], NULL, TO_CHAR );
  act("$n meticulously draws a Pentagram and begins the conjuration of $t.", ch, demons[choice], NULL, TO_ROOM);

  /* setup the gen */
  af.type	= gen_conjure;
  af.level	= choice * 5 + 30;
  af.duration	= 4;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= choice;
  paf = affect_to_char( ch, &af );
  string_to_affect( paf, demons[choice]);

}

/* places a victim in a virtual maze */
void spell_maze( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;


  if (is_affected(ch, sn)){
    send_to_char("You are not yet ready to fashion another maze.", ch );
    return;
  }
  else if (!IS_AWAKE(victim)){
    send_to_char("They must be concious for the magic to take hold.\n\r", ch );
    return;
  }
  else if (is_affected(victim, gen_maze)){
    send_to_char("They've already been mazed!\n\r", ch );
    return;
  }
  else if ( saves_spell(level,victim, DAM_CHARM, skill_table[sn].spell_type)){
    act("You fail to maze $N.",ch,NULL,victim,TO_CHAR);
    return;
  }

  /* success */
  af.type	= gen_maze;
  af.level	= level;
  af.duration	= level / 8;	//!same as protective shield, NO MORE
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= victim->in_room->vnum;
  affect_to_char(victim, &af);

  af.type	= sn;
  af.duration	= 24;
  af.modifier	= 0;
  affect_to_char(ch, &af );
}

/* orb charges up on each combat round, released on recast */
void spell_orb_chaos( int sn, int level, CHAR_DATA *ch, void *vo, int target ){


  AFFECT_DATA *paf;
  AFFECT_DATA af;

  CHAR_DATA* victim = ch->fighting;

  int per_level = effect_table[GN_POS(gen_orb)].value3[0];
  int lvl = 0;
  int dam = 0;


  /* two cases, one no charge, and one charge */
  if ( (paf = affect_find(ch->affected, gen_orb)) == NULL){
    af.type = gen_orb;
    af.level = level;
    af.duration = 24;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_char(ch, & af);
    return;
  }

  /* charge is there we release it */
  if (!victim){
    send_to_char("You must be in combat to release Moloch's wrath.\n\r", ch);
    return;
  }
  lvl = paf->modifier / per_level;
  dam = level + dice (5, 2 * paf->modifier / 3);

  af.type = sn;
  af.level = level;
  af.duration = 0;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.modifier = APPLY_NONE;
  af.location = 0;

  act("You call upon Moloch to release its infernal wrath.", ch, NULL, NULL, TO_CHAR);
  act("$n call upon Moloch to release its infernal wrath!", ch, NULL, NULL, TO_ROOM);
  broadcast(ch, "The area flares crimson as forces of Nexus are unleashed.");

  /* run effects based on level */
  if (lvl > 0){
    /* burn the ac */
    af.type = gsn_rust;
    af.duration = 6;
    af.location = APPLY_AC;
    af.modifier -= paf->modifier / 2;
    if(!is_affected(ch, gsn_rust))
      affect_to_char(victim, &af);
  }
  if (lvl > 1){
    af.type = sn;
    af.duration = 6;
    af.location = APPLY_SAVING_SPELL;
    af.modifier = paf->modifier / 10;
    if(!is_affected(ch, sn))
      affect_to_char(victim, &af);
  }
  if (lvl > 2
      && check_immune(victim, DAM_DISEASE, TRUE) != IS_IMMUNE
      && !is_affected(victim, gsn_plague)){
    af.where     = TO_AFFECTS;
    af.type 	 = gsn_plague;
    af.level	 = level;
    af.duration  = 6;
    af.location  = APPLY_STR;
    af.modifier  = -(level/10);
    af.bitvector = AFF_PLAGUE;
    affect_to_char(victim,&af);

    /* now damage counter */
    af.where     = TO_NONE;
    af.type 	 = gsn_plague;
    af.level	 = level/2;
    af.location  = APPLY_NONE;
    if (ch->class == class_lookup("shaman"))
      af.modifier  = 5;
    else
      af.modifier  = 0;
    af.bitvector = 0;
    paf = affect_to_char(victim,&af);
   if (!IS_NPC(ch))
     string_to_affect(paf, ch->name);
   act_new("You scream in agony as plague sores erupt from your skin.",victim,NULL,NULL,TO_CHAR,POS_DEAD);
   act("$n screams in agony as plague sores erupt from $s skin.",victim,NULL,NULL,TO_ROOM);
  }
  damage( ch, victim, dam, sn, DAM_NEGATIVE, TRUE );
  affect_strip(ch, gen_orb);
}

/* releases the nemesis */
void do_nemesis( CHAR_DATA*ch, char* argument ){
  const int sn = skill_lookup("nemesis");
  const int cost = skill_table[sn].min_mana;
  char buf[MIL];
  OBJ_DATA* wield;
  CHAR_DATA* nemesis = NULL, *vch, *victim;
  int i = 0;


  if (get_skill(ch, sn) < 2 ){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  /* try to find nemesis */
  for (vch = char_list; vch; vch = vch->next ){
    if (IS_NPC(vch) && vch->pIndexData->vnum == MOB_VNUM_NEMESIS){
      nemesis = vch;
      break;
    }
  }

  if (nemesis == NULL){
    send_to_char("The Faceless has not been summoned yet.\n\r", ch );
    return;
  }
  else if (nemesis->hunting != NULL){
    act("$N is already loose!", ch, NULL, nemesis, TO_CHAR);
    return;
  }
  else if (nemesis->in_room == NULL || nemesis->in_room->area->pCabal == NULL
	   || nemesis->in_room->area->pCabal->anchor->area != nemesis->in_room->area){
    act("$N will not obey untill it has been recaptured and bount to its prison.", ch, NULL, nemesis, TO_CHAR);
  }

  /* now we have a nemesis, not hunting anyone, within the cabal grounds */
  if (IS_NULLSTR(argument)){
    send_to_char("Whom do you wish to release the Faceless upon?\n\r", ch );
    return;
  }
  else if ( (victim = get_char_world( ch, argument )) == NULL){
    send_to_char("They do not appear to be present.\n\r", ch );
    return;
  }
  else if (IS_NPC(victim) || is_ghost(victim, 600) || !is_pk(victim, victim)){
    act("$G protects $N from your servants.", ch, NULL, victim, TO_CHAR );
    return;
  }

  if (ch->pCabal && GET_CP(ch) < cost){
    sendf( ch, "You will need at least %d %ss to command the Faceless.", cost, ch->pCabal->currency );
    return;
  }
  else if (ch->pCabal)
    handle_skill_cost( ch, ch->pCabal, sn );

  /* set nemesis to hunt the person */
  nemesis->hunting = victim;

  nemesis->max_hit = 2 * victim->max_hit;
  nemesis->hit = nemesis->max_hit;
  nemesis->max_mana = 1000;
  nemesis->mana = nemesis->max_mana;

  nemesis->level = victim->level + 3;
  for (i=0; i < 4; i++)
    nemesis->armor[i] = victim->armor[i];

  nemesis->sex = victim->sex;
  nemesis->gold = 0;

//Now we copy over little things like weapon stats.
  nemesis->damroll = UMAX(10, victim->damroll);
  nemesis->hitroll = UMAX(10, victim->hitroll);

  if ( (wield = get_eq_char(victim,WEAR_WIELD)) != NULL){
    nemesis->damage[DICE_NUMBER] = UMAX(1, wield->value[1]);
    nemesis->damage[DICE_TYPE] = UMAX(1, wield->value[2]);
    nemesis->dam_type = wield->value[3];
  }

  act("$n's features begin to change and with a clank the chains release!", nemesis, NULL, NULL, TO_ROOM);
  /* name */
  sprintf( buf, "anti%s anti-%s nemesis", victim->name, victim->name );
  free_string( nemesis->name );
  nemesis->name = str_dup( buf );

  /* short */
  sprintf( buf, "Anti-%s", victim->name );
  free_string( nemesis->short_descr );
  nemesis->short_descr = str_dup( buf );

  /* long */
  sprintf( buf, "Anti-%s is here, seeking its victim.\n\r", victim->name );
  free_string( nemesis->long_descr );
  nemesis->long_descr = str_dup( buf );

  act("$n grins a terrible smile as $e begins to leave.", nemesis, NULL, NULL, TO_ROOM);

  /* flags */
  REMOVE_BIT(nemesis->act, ACT_SENTINEL );
  REMOVE_BIT(nemesis->act, ACT_NO_WANDER );
  REMOVE_BIT(nemesis->act, ACT_NONCOMBAT );

  nemesis->size = victim->size;

  /* desc */
  free_string( nemesis->description );
  nemesis->description = str_dup( victim->description );

  if (ch->pCabal ){
    sprintf( buf, "%s: 'As you wish.. Master.'", PERS2(nemesis));
    cabal_echo(ch->pCabal, buf );
  }
}

/* CONCLAVE */

/* Written by: Virigoth							*
 * Returns: FALSE on failure to pass gaurds				*
 * Used: special.c,							*
 * Comment: fade check for shops etc					*
 * sneaking at 1/2 chance						*/

bool guard_fade_check(CHAR_DATA* ch){
  int chance = (get_skill(ch, gsn_fade) - 50) * 2;
  chance += (room_has_sun(ch->in_room)? -20:0);

  if (chance < 1)
    return FALSE;
  else if (IS_NPC(ch) && !IS_AFFECTED(ch, AFF_CHARM))
    return FALSE;
  else if (number_percent() < chance){
    check_improve(ch, gsn_fade, TRUE, 1);
    return TRUE;
  }

  check_improve(ch, gsn_fade, FALSE, 1);
  return FALSE;
}


/* Written by: Virigoth							*
 * Returns: FALSE on failure to sneak					*
 * Used: act_move.c,							*
 * Comment: fade check for sneaking					*/

bool sneak_fade_check(CHAR_DATA* Ch){
  CHAR_DATA* ch = Ch;
  int chance = 0;

  if (IS_NPC(Ch) && IS_AFFECTED(Ch, AFF_CHARM) && Ch->master)
    ch = Ch->master;

  chance = (get_skill(ch, gsn_fade) - 50);
  chance += (room_has_sun(Ch->in_room)? -10:0);
  chance *= 2;

  if (chance < 1)
    return FALSE;
  else if (IS_NPC(ch) && !IS_AFFECTED(ch, AFF_CHARM))
    return FALSE;
  else if (!IS_NPC(Ch) && number_range(0, 1000) < 200)
    check_improve(Ch, gsn_fade, TRUE, 1);
  if (number_percent() < chance){
    return TRUE;
  }
  return FALSE;
}


void spell_doppelganger( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;

    if ( (victim = get_char_room(ch, NULL, target_name)) == NULL){
      send_to_char("They do not appear to be here.\n\r", ch);
      return;
    }

    if ( (ch == victim) || (is_affected(ch,sn) && (ch->doppel == victim)) )
    {
	sendf(ch, "The illusion of %s already surrounds you.\n\r", PERS2(victim));
	return;
    }
    if (IS_NPC(victim))
    {
      act("$N features are too bland to be used for a true simulacra.", ch, NULL, victim, TO_CHAR);
	return;
    }
    if (IS_IMMORTAL(victim))
    {
	send_to_char("That is simply beyond you.\n\r",ch);
	return;
    }
    affect_strip(ch, gen_conceal);
    do_revert(ch, "");
    affect_strip(ch, skill_lookup("shadowform"));
    affect_strip(ch, skill_lookup("treeform"));

    act("Shadows cloak you as $g's power meld your features into $N's.",ch,NULL,victim,TO_CHAR);

    if (!IS_AFFECTED(ch, AFF_HIDE)
	&& !IS_AFFECTED2(ch, AFF_CAMOUFLAGE)
	&& !is_affected(ch, gsn_death_shroud))
      act("$n's visage ripples shimmers and $e is no more.", ch, NULL, NULL, TO_ROOM);

    af.where		  = TO_AFFECTS;
    af.type               = gsn_doppelganger;
    af.level              = level;
    af.duration           = 72;
    af.location           = APPLY_NONE;
    af.modifier           = 0;
    af.bitvector          = 0;
    affect_to_char(ch,&af);
    ch->doppel = victim;
}

/* Written by: Virigoth							*
 * Returns: NULL							*
 * Used: cabal.c (affeccts: act_info.c,					*
 * Comment: changes desc as set by "concealment" in effect.c		*
 *	user can specify if he/she can be recognised by cabal mates.    */

void do_conceal(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;
  char arg[MIL];

  const int sn = skill_lookup("conceal");
  const int cost = _mana_cost(ch, sn);
  const int lag = skill_table[sn].beats;
  int chance = get_skill(ch, sn);
  af.modifier = 1;

  one_argument(argument, arg);

  if (!IS_IMMORTAL(ch))
    return;

  if ( is_affected( ch, gen_conceal ) ){
    send_to_char("You banish the cloak concealing you.\n\r", ch);
    affect_strip(ch, gen_conceal);
    WAIT_STATE2(ch, lag/4);
    return;
  }

  if (is_affected(ch, gsn_doppelganger)){
    send_to_char("The illusion about you shatters\n\r.", ch);
    affect_strip(ch, gsn_doppelganger);
  }

  if (arg[0]){
    if (str_prefix("full", arg)){
      send_to_char("conceal:\n\r  syntax: conceal <full>\n\r"\
		   "Full concealment will hide your idenity\n\r"\
		   "even from other Conclave memebers.\n\r\n\r"\
		   "use \"conceal\" to remove your guise at your will.\n\r", ch);

      return;
    }
    else
      af.modifier = 0;
  }

  if (ch->mana < cost){
    send_to_char("You have not the power.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2(ch, lag);
  if (number_percent() > chance){
    send_to_char("You fail.\n\r", ch);
    check_improve(ch, sn, FALSE, 1);
    return;
  }
  else
    check_improve(ch, sn, TRUE, 1);

  af.type      = gen_conceal;
  af.level	 = ch->level;
  af.duration  = 24;
  af.location  = APPLY_NONE;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  affect_to_char( ch, &af );

}

/* actual abduction function, paf holds the room/player target */

void abduct_char(CHAR_DATA* ch, CHAR_DATA* victim, AFFECT_DATA* paf){
  ROOM_INDEX_DATA *pRoomIndex = NULL;
  CHAR_DATA* vch, *vch_next;
  AFFECT_DATA* baf = NULL, af;

  int area_pool = 5;
  int e_r1[] = {ROOM_NO_TELEPORTIN, ROOM_NO_GATEIN, ROOM_NO_RECALL, ROOM_NO_INOUT};

  int sn = skill_lookup("abduct");	//gsn
  int skill = get_skill(ch, sn);	//skill in spell
  int save_mod = (skill - 75)/5;	//pen to save based on skill

  bool fSet = FALSE;			//set if preset location used

  /* safety */
  if (!ch || !victim){
    bug("abduct_char: NULL character or victim passed.", 0);
    return;
  }
  if (IS_NPC(victim)){
    send_to_char("Why not just ask nicely?\n\r", ch);
    return;}
  else if (ch == victim){
    send_to_char("Use a blindfold and bottle of rum instead.\n\r", ch);
    return;
  }
  else if (!IS_AWAKE(victim) || is_affected(victim,gsn_ecstacy)){
    send_to_char("Your victim must be fully conscious.\n\r", ch );
    return;
  }
  /* DISABLE DIPLOMACY FOR DURATION OF is_safe HERE */
  if ( (baf = affect_find(victim->affected, gen_diplomacy)) != NULL)
    baf->modifier = FALSE;
  /* check for safety */
  if (is_safe(ch, victim)){
    /* ENABLE DIPLOMACY HERE IF WAS DISABLED */
    if (baf)
      baf->modifier = TRUE;
    return;
  }
  else if (baf)
    baf->modifier = TRUE;
/* we strip diplomacy ONLY if we succeed in abduction */


  WAIT_STATE2(ch, skill_table[sn].beats);

  /* Current room check */
  if (IS_SET(victim->in_room->room_flags, ROOM_NO_INOUT)
      || IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)){
    send_to_char("Something is preventing the abduction.\n\r", ch);
    return;
  }

  if ( victim->in_room == NULL
       || is_affected(victim, gsn_tele_lock)
       || saves_spell( victim->level + save_mod, victim, DAM_OTHER,skill_table[sn].spell_type) )
    {
      act("You failed to abduct $N.",ch,NULL,victim,TO_CHAR);
      check_improve(ch,sn,FALSE,15);
      m_yell(ch,victim,FALSE);
      /* if there was diplo protection we do not set fighting */
      /* set fighting */
      if (baf == NULL && victim->fighting != ch )
	{
	  if (IS_AWAKE(victim)
	      && !is_affected(victim,gsn_ecstacy)
	      && !is_ghost(victim,600))
	    {
	      set_fighting( ch, victim );
	      set_fighting( victim, ch );
	      multi_hit( victim, ch, TYPE_UNDEFINED );
	    }
	}
      return;
    }

  /* set fighting */
  if (victim->fighting != ch )
    {
      m_yell(ch,victim,FALSE);
      if (IS_AWAKE(victim)
	  && !is_affected(victim,gsn_ecstacy)
	  && !is_ghost(victim,600))
	{
	  set_fighting( ch, victim );
	  set_fighting( victim, ch );
	  multi_hit( victim, ch, TYPE_UNDEFINED );
	}
    }

  /* check for paf first */
  if (paf){
/* ROOM */
    if (paf->modifier > 0){
      if ( (pRoomIndex = get_room_index(paf->modifier)) != NULL)
	fSet = TRUE;
    }//END room
/* PLAYER */
    else if (paf->has_string){
      CHAR_DATA* ch_dest;
      if ( (ch_dest = get_char(paf->string)) != NULL){
	pRoomIndex = ch_dest->in_room;
	fSet = TRUE;
      }
      else
	sendf(ch, "%s absent, your control over the magic fails.\n\r", paf->string);
    }//END Player
  }//End if PAF

  /* we have the room in the area to abduct to, get a random room */
  if (fSet){
    pRoomIndex =  get_rand_room(pRoomIndex->area->vnum,pRoomIndex->area->vnum,	//area range (0 to ignore)
				0,0,	//room ramge (0 to ignore)
				NULL,0,	//areas to choose from
				NULL,0,	//areas to exclude
				NULL,0,	//sectors required
				NULL,0,	//sectors to exlude
				NULL,0,	//room1 flags required
				e_r1,4,	//room1 flags to exclude
				NULL,0,	//room2 flags required
				NULL,0,	//room2 flags to exclude
				area_pool,//number of seed areas
				TRUE,	//exit required?
				FALSE ,	//Safe?
				ch);	//Character for room checks
    /* if we did not get a raond room we fail */
    if (pRoomIndex == NULL ){
      sendf(ch, "Your chosen area unreachable, you lose control!.\n\r");
      fSet = FALSE;
    }
  }//END room CHECK

  /* if Preset room is not used (!fSet) we choose random one */
  if (!fSet){
    pRoomIndex =  get_rand_room(0,0,	//area range (0 to ignore)
				0,0,	//room ramge (0 to ignore)
				NULL,0,//areas to choose from
				NULL,0,//areas to exclude
				NULL,0,//sectors required
				NULL,0,//sectors to exlude
				NULL,0,//room1 flags required
				e_r1,4,//room1 flags to exclude
				NULL,0,//room2 flags required
				NULL,0,//room2 flags to exclude
				area_pool,//number of seed areas
				TRUE,//exit required?
				FALSE ,//Safe?
				ch);	//Character for room checks


  }//END if not preselect
 /* check if we have a room now */
  if (pRoomIndex == NULL)
    return;

  //timer on reuse
  af.type = skill_lookup("gate");
  af.duration = 3;
  af.level = 60;
  af.where = TO_NONE;
  af.bitvector = 0;
  af.location = 0;
  af.modifier = 0;
  affect_to_char( ch, &af );

  /* Now we have a proper destination */

  act("With crackle of dark magic $n and $N disappear!",
      ch, NULL, victim, TO_NOTVICT);
  act("You have been ABDUCTED!",
      ch, NULL, victim, TO_VICT);
/* Get rid of diplomacy */
  affect_strip(victim, gen_diplomacy);

  act("You feel the dark magics take hold of $N and carry you off!",
      ch, NULL, victim, TO_CHAR);
  if (pRoomIndex->people)
    act("With crackle of dark magics $n and $N appear!", ch, NULL, victim, TO_ALL);

/* now move user and his charmies */
  for (vch = ch->in_room->people; vch; vch = vch_next){
    vch_next = vch->next_in_room;
    if (IS_AFFECTED(vch, AFF_CHARM) && vch->master
	&& vch->master == ch){
      char_from_room( vch );
      char_to_room( vch, pRoomIndex );
    }
  }
  char_from_room( ch );
  char_to_room( ch, pRoomIndex );

  char_from_room( victim );
  char_to_room( victim, pRoomIndex );

  if (IS_AWAKE(ch))
    do_look( ch, "auto" );

  if (IS_AWAKE(victim))
    do_look( victim, "auto" );

  /* autoimprove if less then 85 */
  if (skill < 85)
    check_improve(ch,sn,TRUE,-99);
  else
    check_improve(ch,sn,TRUE,5);

  if (victim->fighting && victim->fighting != ch){
    stop_fighting(victim, TRUE);
    set_fighting(victim, ch);
  }
}


/* Written by: Virigoth							*
 * Returns: NULL							*
 * Used: cabal.c							*
 * Comment: various support function to spell_abduction			*/

/* shows current abduction location */
char* abduct_info(CHAR_DATA* ch){
  static char output[MIL];
  AFFECT_DATA* paf;
  ROOM_INDEX_DATA *pRoomIndex;

  output[0] = 0;
  if ((paf = affect_find(ch->affected, gsn_abduct)) == NULL)
    return output;

  if (paf->modifier
      && (pRoomIndex = get_room_index(paf->modifier)) != NULL){
    sprintf(output, "Future abductions directed to: %s",
	    pRoomIndex->area->name);
  }
  else if (paf->has_string){
    sprintf(output, "Future abductions directed to: %s",
	    paf->string);
  }
  else{
    sprintf(output, "Future abductions directed to: %s",
	    "Unknown");
    bug("abduct_info: No preset data in paf.", 0);
  }
  return (output);
}

/* Sets destination room */
void abduct_setroom(CHAR_DATA *ch){
  AFFECT_DATA af;
  ROOM_INDEX_DATA *pRoomIndex = ch->in_room;

  //const
  const int room_dur = 72;		//duration dest. is effective

  //data
  int sn = gsn_abduct;	//gsn for abduction

  /* check for room flags */
  if ( ch->in_room == NULL
       || IS_SET(pRoomIndex->room_flags, ROOM_NO_INOUT)
       || IS_SET(pRoomIndex->room_flags, ROOM_NO_RECALL)
       || IS_AREA(pRoomIndex->area, AREA_RESTRICTED)
       || IS_AREA(pRoomIndex->area, AREA_NOMORTAL)
       || IS_AREA(pRoomIndex->area, AREA_MUDSCHOOL)){
    send_to_char("The area is shielded from any future abduction attempts.\n\r", ch);
    return;
  }

  /* Viri: no longer needed
  for (door = 0; door < MAX_DOOR; door++ ){
    if (pRoomIndex->exit[door]
	&& !IS_SET(pRoomIndex->exit[door]->exit_info, EX_CLOSED))
      fNoExit = FALSE;
  }
  if (fNoExit){
    send_to_char("The shades require a visible entrance in order to abduct.\n\r", ch);
    return;
  }
  */
  sendf(ch, "Future abductions now directed to %s.\n\r", pRoomIndex->area->name);

  /* Room looks good */
  af.type = sn;
  af.level = ch->level;
  af.duration = room_dur;

  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = pRoomIndex->vnum;
  /* strip any previous ones */
  affect_strip(ch, sn);
  affect_to_char(ch, &af);
}

/* sets the destination to a player */
void abduct_setplayer(CHAR_DATA *ch, CHAR_DATA *victim){
  AFFECT_DATA af;
  AFFECT_DATA* paf;


  //const
  const int room_dur = 48;		//duration dest. is effective

  //data
  int sn = gsn_abduct;	//gsn for abduction

  //bool
  bool fOk = FALSE;			//used to check if grouped/pk

  if (IS_NPC(victim)){
    send_to_char("Not on mobs.\n\r", ch);
    return;
  }

  /* check if grouped and in pk */
  if (is_same_group(ch, victim)
      && !is_safe_quiet(ch, victim))
    fOk = TRUE;

  if (!fOk){
    act("$N must be in your PK range, and grouped.", ch, NULL, victim, TO_CHAR);
    return;
  }
  else{
    act("Future abduction attempts now directed to $N's area.", ch, NULL, victim, TO_CHAR);
    act("$n has directed $s abduction attempts to your future location.", ch, NULL, victim, TO_VICT);
  }

  /* set target */
  af.type = sn;
  af.level = ch->level;
  af.duration = room_dur;

  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  /* strip any previous ones */
  affect_strip(ch, sn);
  paf = affect_to_char(ch, &af);
  string_to_affect(paf, victim->name);
}



/* Written by: Virigoth							*
 * Returns: NULL							*
 * Used: cabal.c							*
 * Comment: calls abduct_char to do the actual abducting		*
 * Syntax:								*
 * abduct <target>: attempts abduction					*
 * abduct <set>:    sets area as destination (85%)			*
 * abduct <set> <player> sets target player's area as destination (95%)	*
 * calls: abduct_setroom, abduct_setplayer, abduct_char		*/

void do_abduction( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA* victim;
  char arg1[MIL];
  char arg2[MIL];

  const int sn = gsn_abduct;
  const int skill = get_skill(ch, sn);		//skill in the spell
  const int rm_skill = 85;			//skill for room dest.
  const int plr_skill = 95;			//skill for player dest.

  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  /* Regular checks */
  if (IS_NPC(ch))
    return;
  else if (skill < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }

  /* Begins parsing the commandline */
/* Syntax */
  if (arg1[0] == '\0'){
    send_to_char("Abduction:\n\r Syntax:\n\r  abduct <target>\n\r"\
		 "  abduct <set>		(room)\n\r"\
		 "  abduct <set> <target>	(player)\n\r", ch);
    return;
  }

/* SET */
  if (!str_cmp("set", arg1)){
  /* ROOM */
    if (arg2[0] == '\0'){
      /* check for skill */
      if (skill < rm_skill){
	send_to_char("You are not skilled enough to set a destination.\n\r", ch);
	return;
      }
      /* set the room */
      abduct_setroom(ch);
      return;
    }
  /* PLAYER */
    if (skill < plr_skill){
      send_to_char("You are not skilled enough to coordinate an abduction.\n\r", ch);
      return;
    }
    /* look for target destination */
    if ( (victim = get_char_room(ch, NULL, arg2)) == NULL){
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
    abduct_setplayer(ch, victim);
    return;
  }//END SET

  /* ABDUCT */
  if ( (victim = get_char_room(ch, NULL, arg1)) == NULL){
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  else if (!IS_NPC(victim) && victim->pcdata->pbounty == NULL){
    send_to_char("They do not have a price on their head.\n\r", ch);
    return;
  }
  else if (is_affected(ch, skill_lookup("gate"))){
    send_to_char("You're not ready to abduct once more.\n\r", ch);
    return;
  }
  if (ch->mana < cost){
    send_to_char("You cannot concentrate sufficiently.\n\r", ch);
    return;
  }
  else
    ch->mana -= cost;
  WAIT_STATE2(ch, lag );

  /* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, sn );
  abduct_char(ch, victim, affect_find(ch->affected, sn));
}


/* Written by: Virigoth							*
 * Returns: NULL							*
 * Used: cabal.c							*
 * Comment: uses soem wrapper functions to show anything in the room	*
 * The spell basicly shows all contentsof the room regadless of can_see's*/

void do_unseen( CHAR_DATA *ch, char *argument ){

  CHAR_DATA *victim;
  CHAR_DATA *rch;
  ROOM_INDEX_DATA *in_room;
  OBJ_DATA *corpse, *c_next;


  char buf[MIL];
  char arg[MIL];

  const int sn = skill_lookup("unseen");
  const int cost = _mana_cost(ch, sn);
  //data
  int save_mod = (IS_UNDEAD(ch)? 5: 0);
  int skill = get_skill(ch, sn);
  int chance = 20;
  int percent = 0;
  int found_corpse = FALSE;

  if (!IS_IMMORTAL(ch))
    return;

  if (skill < 1){
    send_to_char("Maybe get a ouija board first.\n\r", ch);
    return;
  }

  argument = one_argument(argument, arg);
  if (!arg[0]){
    send_to_char("You must specify player to send the Unseen to.\n\r", ch);
    return;
  }

  if (ch->mana < cost){
    send_to_char("You lack the power to communicate with the dead.\n\r", ch);
    return;
  }

  /* Look for a corpse to use in the caster's room */
  if (get_skill(ch, sn) < 100){
    for (corpse = ch->in_room->contents; corpse != NULL; corpse = c_next) {
      c_next = corpse->next_content;
      if ( corpse->item_type != ITEM_CORPSE_NPC && corpse->item_type != ITEM_CORPSE_PC ) {
	continue;
      }
      else {
	found_corpse = TRUE;
	act( "$n mutters a command and $p eyes open suddenly.", ch, corpse, NULL, TO_ROOM );
	act( "$p eyes open suddenly to lend you their sight.", ch, corpse, NULL, TO_ROOM );
      }
    }

    if (found_corpse != TRUE) {
      send_to_char("You see no corpses to communicate with.\n\r", ch);
      return;
    }
  }
  ch->mana -= cost;
  WAIT_STATE2(ch, skill_table[sn].beats);

  /* check for target */
  if ( (victim = get_char_world(ch, arg)) == NULL)
    {
      send_to_char("The victim must be seen to you before you can instruct the dead.\n\r", ch);
      return;
    }
  /* get destination room */
  in_room = victim->in_room;
  if (in_room->vnum == ROOM_VNUM_LIMBO
      || IS_SET(in_room->room_flags, ROOM_NOWHERE)
      || is_affected(victim, gsn_coffin)
      || is_affected(victim, gsn_burrow)
      || is_affected(victim, gsn_catalepsy)){
    send_to_char("You sense strange reluctance from your unliving spies.\n\r", ch);
    return;
  }

  /* saves check only if target is  higher levels then caster or undead*/
  if ( (victim->level > ch->level || IS_UNDEAD(victim))
       && saves_spell(victim->level + save_mod, victim,
		      DAM_OTHER,skill_table[gsn_spy].spell_type) ){
    act("You sense the unholy minions failed to reach $N.", ch, NULL, victim, TO_CHAR);
    return;
  }

  /* we show victim room */
  send_to_char("Through the eyes of the dead, you gaze upon your victim.\n\r\n\r", ch);

  /* ROOM NAME */
  send_to_char( in_room->name, ch );
  send_to_char("\n\r", ch);
  /* ROOM DESC */
  send_to_char( "  ",ch);
  if ((mud_data.time_info.hour >=5 && mud_data.time_info.hour < 20)
      || !in_room->description2[0])
    send_to_char( in_room->description, ch );
  else
    send_to_char( in_room->description2, ch );
  send_to_char( "\n\r", ch );

  /* Contents */
  list_to_char( in_room->contents, ch, FALSE, FALSE, TRUE );
  /* people */
  for ( rch = in_room->people; rch != NULL; rch = rch->next_in_room ){
    if ( rch == ch )
      continue;
    if ( get_trust(ch) < rch->invis_level)
      continue;
    char_to_char( rch, ch, TRUE );
  }

  /* health of target */
  if ( victim->max_hit > 0 )
    percent = ( 100 * victim->hit ) / victim->max_hit;
  else
    percent = -1;
  strcpy( buf, victim->name);
  if (percent >= 100)
    strcat( buf, " is in excellent condition.\n\r");
  else if (percent >= 90)
    strcat( buf, " has a few scratches.\n\r");
  else if (percent >= 75)
    strcat( buf, " has some small wounds and bruises.\n\r");
  else if (percent >=  50)
    strcat( buf, " has quite a few wounds.\n\r");
  else if (percent >= 30)
    strcat( buf, " has some big nasty wounds and scratches.\n\r");
  else if (percent >= 15)
    strcat( buf, " looks pretty hurt.\n\r");
  else if (percent >= 0 )
    strcat( buf, " is in awful condition.\n\r");
  else
    strcat( buf, " is bleeding to death.\n\r");
  buf[0] = UPPER(buf[0]);
  send_to_char( buf, ch );

  /* chance to slip */
  chance += (victim->level - ch->level) * 5;
  chance -= save_mod;
  chance += victim->level > ch->level + 4 ? 25 : 0;

  if (number_percent() < chance){
    if (IS_UNDEAD(victim)){
      send_to_char("You sense presence of Unseen invade the area.\n\r", victim);
      act("You feel deathly, unnatural chill pass through the area.",
	  victim, NULL, NULL, TO_ROOM);
    }
    else
      act("You feel deathly, unnatural chill pass through the area.",
	  victim, NULL, NULL, TO_ALL);
  }
}


/* Written by: Virigoth							*
 * Returns: NULL							*
 * Used: cabal.c							*
 * Comment: caster becomes invisible to all except other cabal memebrs	*
 * stripped on movement and anything that strips hide			*/

void spell_death_shroud( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  AFFECT_DATA af;
  const int dur = 12;

  if (!IS_IMMORTAL(ch))
    return;

  if (is_affected(ch, gen_d_shroud)){
    send_to_char("You have already placed yourself in the lands of unliving.\n\r", ch);
    return;
  }
  if ( IS_AFFECTED(ch, AFF_FAERIE_FOG) || IS_AFFECTED(ch, AFF_FAERIE_FIRE))
    {
      send_to_char("The glowing aura around you seems to banish the shroud.\n\r", ch);
      return;
    }
  af.type = gen_d_shroud;
  af.duration = dur;
  af.level = level;

  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char(ch, &af);
}




/* Written by: Virigoth							*
 * Returns: NULL							*
 * Used: cabal.c							*
 * Comment: enhances torso armor if any worn, armor gives poison/disase	*
 * resistance								*/


void spell_holy_armor( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{

    OBJ_DATA *obj;
    AFFECT_DATA af;

    const int skill = get_skill(ch, sn);


    const int hit = 1 + (skill > 80? 1 : 0) + (skill > 85? 1 : 0);
    const int dam = 1 + (skill > 85? 1 : 0) + (skill > 99? 1 : 0);

    const int dur = 24;

    if (is_affected(ch, sn)){
      send_to_char("You are not yet ready to bless another armor\n\r.", ch);
      return;
    }

    /* check if any torso is worn */
    if ((obj = get_eq_char(ch,WEAR_BODY)) == NULL){
      send_to_char("You must wear a torso armor before applying an enchantement.\n\r", ch);
      return;
    }

    if (is_affected_obj(obj, sn)){
      act("$p already holds $g's power.", ch, obj, NULL, TO_CHAR);
      return;
    }
    act("$p begins to shimmer with $g's power!", ch, obj, NULL, TO_ALL);
    /* setup the effects on ch*/
    af.type = sn;
    af.level = level;
    af.duration = dur;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_char(ch, &af);

    /* set effects on obj */
    af.where = TO_RESIST;
    af.bitvector = RES_DISEASE;
    af.location = APPLY_HITROLL;
    af.modifier = hit;
    affect_to_obj(obj, &af);
    /* make sure what we put on armor is stuck on char */
/*modify_obj does that now
  affect_modify(ch, &af, TRUE);
*/
    send_to_char("You are now resistant to disease.\n\r", ch);

    af.where = TO_RESIST;
    af.bitvector = RES_POISON;
    af.location = APPLY_DAMROLL;
    af.modifier = dam;
    affect_to_obj(obj, &af);
    /* make sure what we put on armor is stuck on char */
/*modify_obj does that now
  affect_modify(ch, &af, TRUE);
*/
    af.where = TO_OBJECT;
    af.bitvector = 0;
    af.location = APPLY_AC;
    af.modifier = -40;
    affect_to_obj(obj, &af);
}


/* Written by: Virigoth							*
 * Returns: character to take the blow (victim on fail)			*
 * Used: cabal.c, fight.c						*
 * Comment: An automatic ablity of KNIGHT Leaders which forces them to  *
 * attempt a rescue on a PC which is about to die.  The leader pushes   *
 * the target out of the room (if any exits are there) and if successful*
 * takes the blow.  If Leader is at hp < 20 - 30% he is dropped to small*
 * amount of hp and stunned.	(Non-Spell only)
 * ch is the attacker, victim is the person being rescued.  Functino	*
 * searches the room for leader.					*/

CHAR_DATA* knight_rescue(CHAR_DATA* ch, CHAR_DATA* victim, int dam, int dt){

  /*
Prerequsites for an attempt, in order:
- dam > 0
- ch != victim
- dt < TYPE_HIT (non-spell)
- victim is PC
- victim->hp - damage < 1
- victim/ch are in same room

- lch (leader) is found
- lch is knight
- lch is leader
- lch can move (pos > sitting)
- lch cansee victim
- lch can see a door

prerequisisted for a save:
- victim has moved out of room (pushed out)

NOTE: Skill always works if conditions are met.
  */
  CHAR_DATA* lch, *lch_next;	//leader doing the rescue.
  EXIT_DATA* pexit;	//room exit

  ROOM_INDEX_DATA* in_room;//Original room beofre the move.

  int doors[MAX_DOOR];	// arrays of doors to be picked out of.
  int max_door = 0;	//total doors found.
  int min_hp = 100;	//minimum hp needed to survive saving.
  int i = 0;		//counter

  bool fFound = FALSE;	//set if leader was found.


  /* check preconditions */
  if (!ch || !victim){
    bug("knight_rescue: NULL argument passed. (cabal.c)", 0);
    return victim;
  }
  /* set victims room. */
  in_room = victim->in_room;

  if ((victim->hit - dam) > 0
      || dam < 1
      || ch == victim
      || dt < TYPE_HIT
      || IS_NPC(victim)
      || ch->in_room != in_room
      || IS_IMMORTAL(victim)
      )
    return victim;
  /* we should attempt a save, check for leader in room. */

  for (lch = in_room->people; lch != NULL; lch = lch_next){
    lch_next = lch->next_in_room;
    if (IS_NPC(lch))
      continue;

/* one god's trimph */
    if (is_affected(lch, gsn_ogtrium)){
      if (lch == victim
	  || IS_IMMORTAL(lch)
	  || !is_same_group(lch, victim)
	  || is_safe(lch, ch)
	  )
	continue;
    }
    /* non-healrs must be knight elders */
    else if (lch->class != class_lookup("healer")){
      if (lch == victim
	  || IS_IMMORTAL(lch)
	  || lch->pCabal != get_cabal("knight")
	  || !IS_ELDER(lch)
	  || !is_same_group(lch, victim)
	  || is_safe(lch, ch)
	  )
	continue;
    }
    /* if is healer */
    else {
      if (lch == victim
	  || IS_IMMORTAL(lch)
	  || !is_same_group(lch, victim)
	  || is_safe(lch, ch)
	  )
	continue;
    }
    /* and healer or knight leader mustbe able to rescue */
    if (lch->position < POS_FIGHTING
	|| !can_see(lch, victim)
	|| !is_same_group(lch, victim)
	)
      continue;
    fFound = TRUE;
    break;
  }

  /* check if there is a leader around. */
  if (!fFound)
    return victim;


  /* if hysteric he cannot rescue */
  if (is_affected(lch, gsn_hysteria)){
    act("In your hysteria you give $N over to Virigoth!", lch, NULL, victim, TO_CHAR);
    act("$n makes a half-hearted attempt at saving $N then panics!", lch, NULL, victim, TO_NOTVICT);
    act("$n makes a half-hearted attempt to save you then panics!", lch, NULL, victim, TO_VICT);
    return victim;
  }
  if (is_affected(ch, gsn_forget)){
    act("If you could only remeber how to save $N...", lch, NULL, victim, TO_CHAR);
    return victim;
  }

  /* leader is here, check if we have a door to push out of. */
  /* get all doors in aray to be picked out of. */
  for (i=0; i < MAX_DOOR; i++){
    if ( (pexit = in_room->exit[i]) == 0
	 || pexit->to_room == NULL
	 || ( IS_SET(pexit->exit_info, EX_CLOSED) && !IS_AFFECTED(victim, AFF_PASS_DOOR))
	 || !can_see_room(victim, pexit->to_room)
	 )
      continue;
    doors[max_door++] = i;
  }
  /* now we have all valid doors in an array. */
  if (!max_door){
    act("Lacking any safe space to push $N to, you vainly attempt to save $m.", lch, NULL, victim, TO_CHAR);
    act("Lacking any safe space to push $N to, $n vainly attempt to save $m.", lch, NULL, victim, TO_NOTVICT);
    act("Lacking any safe space to push you to, $n vainly attempts to save you.", lch, NULL, victim, TO_VICT);
    return victim;
  }
  /* try to move the victim out. */
  move_char(victim,  doors[number_range(0, max_door - 1)], FALSE);

  /* check if still in same room */
  if (victim->in_room == in_room){
    act("You vainly attempt to save $m.", lch, NULL, victim, TO_CHAR);
    act("$n vainly attempts to save $m.", lch, NULL, victim, TO_NOTVICT);
    act("$n vainly attempts to save you.", lch, NULL, victim, TO_VICT);
    return victim;
  }
  act("$n bravely pushes $N out of the way using $s flesh to stop the killing blow!", lch, NULL, victim, TO_NOTVICT);
  act("$n bravely pushes you out of the way using $s flesh to stop the killing blow!", lch, NULL, victim, TO_VICT);
  act("You bravely push $N out of the way using your flesh to stop the killing blow!", lch, NULL, victim, TO_CHAR);


  /* victim has moved out of the room, we juggle some combat variables
     to make this seemless */
  /* stop the stunned person from fighting */
  stop_fighting(victim, TRUE);
  /* attacker attacks the savior */
  set_fighting(ch, lch);
  /* if savior not in combat he attakcs attacker */
  if (!lch->fighting){
    char buf[MIL];
    a_yell(lch, ch);
    sprintf(buf, "Die %s you blood thirsty heathen!",PERS(lch,ch));
    j_yell(ch,buf);
    set_fighting(lch, ch);
  }

  /* check if the savior dies */
  if (dam > 140)
    min_hp = 35;
  else if (dam > 65)
    min_hp= 25;
  else if (dam > 32)
    min_hp= 20;
  else if (dam > 15)
    min_hp= 15;
  else
    min_hp = 5;

  if (lch->class == class_lookup("healer"))
    min_hp /= 2;

  /* if lch is not healthy enough to withstand the blow he dies instantly */
  if ( lch->hit < (min_hp * lch->max_hit / 100)){
    act("As $N's weapon skeweres $n through $e collapses in a pool of blood.", lch, NULL, victim, TO_ROOM);
    act("Unable to stand the damage you collapse spraying blood about!", lch, NULL, NULL, TO_CHAR);
    make_item_char(lch, OBJ_VNUM_BLOOD_POOL, 24);
    lch->hit = 0;
    return lch;
  }

  /* victim is stunned */
  victim->hit = 0;
  dam_update(ch, victim, dam);

  /* damage the savior */
  dam = min_hp * lch->max_hit / 200;
  /* do damage manualy */
  lch->hit -= dam;
  dam_message(ch, lch, NULL, dam, dt, FALSE, FALSE, 0, TRUE);
  return lch;
}


/* Written by: Virigoth							*
 * Returns: void							*
 * Used: cabal.c, fight.c						*
 * Comment: Savant damage shield, allows to set a ceiling on damage done*
 * all damage processin etc. done by gen_sav_ward.			*
 * as damage ceiling is stored in gen. chance to dispell after damage *
 * done is greater then level. duration is infinte.			*/

void spell_sav_ward( int sn, int level, CHAR_DATA *ch, void *vo,int target){
  AFFECT_DATA af;
  char arg[MIL];
  const int dur = 48;

  target_name = one_argument(target_name, arg);

  if (!IS_IMMORTAL(ch))
    return;
  if (arg[0] == '\0'){
    send_to_char("Efuzan's warding  :\n\r"\
		 "Syntax: cast 'Efuzan's Warding' <magical/mundane>\n\r", ch);
    return;
  }
  affect_strip(ch, gen_sav_ward);

  af.type = gen_sav_ward;
  af.level = level;
  af.duration = dur;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;

  /* set type of prtection */
  if (!str_prefix(arg, "magical"))
    af.modifier = TRUE;
  else
    af.modifier = FALSE;
  affect_to_char(ch, &af);
}


/* Written by: Virigoth							*
 * Returns: void							*
 * Used: cabal.c,							*
 * Comment: Savant elder ability to summon one of 4 elemental servants  *
 * each servant has differnet effects.					*/


void spell_servant( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    static char * const material  [] = { "fire",  "water",  "air", "earth" };
    /* seperate mobs as they vary considerably */
    static int const mob_vnum  [] = { MOB_VNUM_SERVANT_FIRE,
				      MOB_VNUM_SERVANT_WATER,
				      MOB_VNUM_SERVANT_AIR,
				      MOB_VNUM_SERVANT_EARTH };

    CHAR_DATA *mob;
    AFFECT_DATA af;

    char buf[MSL], arg[MIL];

    int type = -1;	//type of elemntal being created.
    int i;

    target_name = one_argument(target_name, arg);

/* cap to ignore spell level boost */
    level = UMIN(ch->level, level);

    if ( !IS_IMMORTAL(ch))
        return;
    if (IS_NPC(ch))
	return;
    buf[0]='\0';
    /* Useless contitional */
    if( buf[0] != '\0' )
        buf[0] = '\0';

    if (arg[0] == '\0')
    {
	send_to_char("syntax: cast 'servant' <type>.\n\rFollowing are avaliable:",ch);
	for (i=0; i < 4; i++)
	  sendf(ch, " %s", material[i]);
	send_to_char("\n\r", ch);
	return;
    }

    /* select the servant */
    for (i=0; i < 4; i++){
      if (str_prefix(arg, material[i]))
	continue;
      else
	type = i;
    }

    /* check for match */
    if (type == -1){
	send_to_char("syntax: cast 'servant' <type>.\n\rFollowing are avaliable:",ch);
	for (i=0; i < 4; i++)
	  sendf(ch, " %s", material[i]);
	send_to_char("\n\r", ch);
	return;
    }

    /* check for existing familiar */
    if (ch->pcdata->familiar){
      send_to_char("You already have a servant!\n\r", ch);
      return;
    }

    /* check for pet */
    if (ch->pet){
      send_to_char("You already have a pet!\n\r", ch);
      return;
    }
    if (get_control(ch, MOB_VNUM_SERVANT_FIRE) > 0
	|| get_control(ch, MOB_VNUM_SERVANT_WATER) > 0
	|| get_control(ch, MOB_VNUM_SERVANT_AIR) > 0
	|| get_control(ch, MOB_VNUM_SERVANT_EARTH) > 0)
    {
	send_to_char("You can only control one servant at a time.\n\r",ch);
	return;
    }

    act("You call on elemental forces of $t and summon a servant!", ch, material[type], NULL,TO_CHAR);
    act("$n calls on elemental forces of $t and summons a servant!", ch, material[type],NULL,TO_ROOM);

    /* safety */
    if (is_affected( ch, gsn_familiar_link)){
	send_to_char("Something is wrong (Link already exists).  Notify an imp.",ch);
	return;
    }
    else if (is_affected( ch, sn ) ){
      send_to_char("You do not have enough power to fully link with your familiar\n\r.",ch);
      return;
    }

    /* setup the basics */
    mob = create_mobile( get_mob_index(mob_vnum[type]) );

    /* setup stats */
    mob->level      = ch->level;
    mob->sex	    = ch->sex;
    mob->alignment  = ch->alignment;
    mob->max_hit    = ch->level * 10;
    mob->hit        = mob->max_hit;
    mob->hitroll    = ch->hitroll/4;
    mob->damroll    = ch->damroll/3;
    mob->damage[DICE_NUMBER]	= 4;
    mob->damage[DICE_TYPE]	= level /10 + 1;

    for (i = 0; i < 4; i++)
        mob->armor[i] = ch->armor[i];
    for (i = 0; i < MAX_STATS; i++)
    {
        mob->perm_stat[i]     = ch->perm_stat[i];
        mob->mod_stat[i]      = ch->mod_stat[i];
    }
    SET_BIT(mob->affected_by, AFF_CHARM);
    mob->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;

    /* setup basics of the link */
    af.type		= gsn_familiar_link;
    af.level	= level;
    af.duration	= -1;
    af.location	= APPLY_NONE;
    af.modifier	= 0;
    af.bitvector	= 0;

    /* begin the individual differences */
    switch (type){
      /* FIRE (mana + small svs aff) */
    case 0:
      af.location = APPLY_MANA;
      af.modifier = number_range(ch->pcdata->perm_mana/6,ch->pcdata->perm_mana/4) + ch->level/2;
      ch->mana += af.modifier;
      affect_to_char(ch,&af);
      af.location = APPLY_SAVING_AFFL;
      af.modifier = -level/8;
      affect_to_char(ch,&af);
      break;

      /* WATER (Hp + svs) */
    case 1:
      af.location = APPLY_HIT;
      af.modifier = level;
      ch->hit += af.modifier;
      affect_to_char(ch,&af);
      af.location = APPLY_SAVING_SPELL;
      af.modifier = -3;
      affect_to_char(ch,&af);
      break;

      /* AIR (moves, is invisible, has spec_servant_hide) */
    case 2:
      af.location = APPLY_MOVE;
      af.modifier = number_range(ch->pcdata->perm_move/6,ch->pcdata->perm_move/3) + ch->level/2;
      ch->move += af.modifier;
      affect_to_char(ch,&af);
      break;

      /* EARTH (ac, off_Area attack, lots of hp, heavy hitter) */
    default:
    case 3:
      af.location = APPLY_AC;
      af.modifier = -50;
      affect_to_char(ch,&af);
      mob->level      = UMAX(1, ch->level - 3);
      mob->max_hit = 30 * level;
      mob->hit = mob->max_hit;
      mob->hitroll = level / 3;
      mob->damroll = level / 2;
      break;
    }

    /* all done, put mob in the room */
    char_to_room(mob,ch->in_room);
    add_follower(mob,ch);
    mob->summoner = ch;
    mob->leader=ch;
    ch->pcdata->familiar = mob;

    if (!is_affected( ch, sn ) )
    {
        af.type   = sn;
        af.level  = level;
        af.duration = 24;
        af.location = 0;
        af.modifier = 0;
        af.bitvector = 0;
        affect_to_char(ch,&af);
    }
}




void spell_sentinel_call( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    CHAR_DATA *guard;
    int i;

    if (is_affected(ch, sn)){
      send_to_char("No new observers are yet avaliable.\n\r", ch);
      return;
    }
    else if (get_leadered(ch, MOB_VNUM_JUSTICE_SENTINEL) >= 4){
      send_to_char("You have reached the limit of your leadership abilities.\n\r", ch);
      return;
    }

    guard = create_mobile( get_mob_index(MOB_VNUM_JUSTICE_SENTINEL) );
    for (i = 0; i < MAX_STATS; i++)
	guard->perm_stat[i] = UMIN(25, (ch->level / 10) + 17);
    guard->max_hit = 4 * ch->level;
    guard->hit = guard->max_hit;
    guard->max_mana = 4 * ch->level;
    guard->mana = guard->max_mana;
    guard->alignment = 0;
    guard->level = ch->level;
    for (i = 0; i < 3; i++)
	guard->armor[i] = interpolate(guard->level,100,-100);
    guard->armor[3] = interpolate(guard->level,100,0);
    guard->hitroll    = (ch->level /10)+ 3;
    guard->damroll    = (ch->level /5) + 3;
    guard->damage[DICE_NUMBER]	= (ch->level /10);
    guard->damage[DICE_TYPE]	= 5;
    guard->sex = ch->sex;
    char_to_room(guard,ch->in_room);
    guard->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;

    /* Only leader, no charm etc. Hence cannot be ordered */
    guard->leader = ch;

    /* expires in 72h */
    af.where     = TO_AFFECTS;
    af.type      = gsn_timer;
    af.level     = level;
    af.duration  = 72;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( guard, &af );

    af.type = sn;
    af.level = level;
    af.duration = 12;

    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = 0;
    af.modifier = 0;
    affect_to_char(ch, &af);
    act("A cloaked figure slinks into the room, ready to report to your cause.", ch, NULL, NULL, TO_CHAR);
}



void do_call_roc( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af;
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *location;
  char buf[MSL];
  int drop_chance = 0;

  int sn = gsn_call_roc;
  int skill = get_skill(ch, sn);

  const int lag = skill_table[sn].beats;
  const int cost = skill_table[sn].min_mana;

  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (IS_NPC(ch)){
    send_to_char("Why would a mob call a roc?\n\r",ch);
    return;
  }
  if (is_affected(ch, sn)) {
    send_to_char("You do not feel up to calling another roc.\n\r",ch);
    return;
  }
  if ( ch->mana < cost) {
    send_to_char("You lack the energy to call a roc.\n\r",ch);
    return;
  }
  else
    ch->mana -= cost;
  WAIT_STATE2(ch, lag);
  if ((number_percent() > (skill - 55) * 2)){
    send_to_char("You cannot seem to summon a roc.\n\r",ch);
    check_improve(ch, sn, FALSE, 1);
    return;
  }
  else if (is_affected(ch,gen_ensnare)){
    send_to_char("You sense a powerful magical field preventing your departure.\n\r", ch);
    return;
  }
  handle_skill_cost( ch, ch->pCabal, sn );

  location = get_room_index(get_temple(ch));
  if (location == NULL){
    send_to_char( "You are completely lost.\n\r", ch );
    return;
  }
  if ( ch->in_room == location ){
    send_to_char( "You are already home.\n\r", ch );
    return;
  }
  if ( ( victim = ch->fighting ) != NULL )
    {
      send_to_char("You can't call a roc while you are fighting.\n\r", ch );
      return;
    }

  act( "$n attempts to call a roc.", ch, 0, 0, TO_ROOM );
  sprintf(buf, "From the depths of the chasms, I call thee, Giant Roc!");
  do_yell(ch, buf);

  if ( IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)
       || IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
       || IS_AFFECTED(ch, AFF_CURSE) || IS_SET(ch->act, PLR_DOOF)
       || is_affected(ch, gsn_damnation))
    {
      send_to_char( "The roc doesn't seem to answer your call.\n\r", ch );
      return;
    }
  ch->move /= 2;

  act( "$n's voice is quieted as a giant roc appears on the horizon!",ch,NULL,NULL,TO_ROOM );
  act( "$n mounts on the back of a giant roc.", ch,NULL,NULL,TO_ROOM );
  act( "You mount on the back of a giant roc.", ch,NULL,NULL,TO_CHAR );

  //init the chance. higher it is, the worse off you'll be
  drop_chance = 3;

  //base the chance on size of ch
  if (ch->size == SIZE_HUGE){
    drop_chance += 5;
  }
  if (ch->size == SIZE_LARGE){
    drop_chance += 2;
  }

  // tweak chances here based on certain spells
  if (is_affected(ch, skill_lookup("blindness"))
      || is_affected(ch, skill_lookup("dirt kicking")))
    {
      drop_chance += 1;
    }
  if (is_affected(ch, skill_lookup("curse")))
    {
      drop_chance += 1;
    }

  //throw in a little luck
  drop_chance -= (get_curr_stat(ch,STAT_LUCK) - 14);

  //add in the affects
  af.where        = TO_AFFECTS;
  af.type         = gsn_call_roc;
  af.level        = ch->level;
  af.duration     = 8;
  af.location     = APPLY_NONE;
  af.modifier     = 0;
  af.bitvector    = 0;
  affect_to_char(ch,&af);

  check_improve(ch, sn, TRUE, 1);

  //first one if failed will just teleport you
  if (number_percent() < drop_chance ){
    ROOM_INDEX_DATA *pRoomIndex;
    int e_r1[] = {ROOM_NO_TELEPORTIN};
    int area_pool = 10;
    pRoomIndex =  get_rand_room(0,0,		//area range (0 to ignore)
				0,0,		//room ramge (0 to ignore)
				NULL, 0,//areas to choose from
				NULL,0,		//areas to exclude
				NULL,0,		//sectors required
				NULL,0,		//sectors to exlude
				NULL,0,		//room1 flags required
				e_r1,1,		//room1 flags to exclude
				NULL,0, 	//room2 flags required
				NULL,0,		//room2 flags to exclude
				area_pool,	//number of seed areas
				TRUE,		//exit required?
				TRUE,		//Safe?
				ch);//Character for room checks
    if (pRoomIndex == NULL){
      /* he gets lucky ;) */
      pRoomIndex = location;
    }

    send_to_char("You have been dropped by the Giant Roc!\n\r",ch);
    char_from_room( ch );
    char_to_room( ch, pRoomIndex );
    act( "$n falls from the sky and lands, with a 'thud' in the room.",ch,NULL,NULL,TO_ROOM );
    do_look( ch, "auto" );
    act("That fall from the roc really hurt!",ch,NULL,NULL,TO_CHAR);
    damage(ch, ch, 40, gsn_bash, DAM_BASH, TRUE);
    broadcast(ch, "You hear a something come crashing to the ground nearby.\n\r");
    WAIT_STATE2(ch, lag);
  }
  //otherwise your brought home safely
  else{
    	char_from_room( ch );
    	char_to_room( ch, location );
        act("$n flies in on the back of a Giant Roc.",ch,NULL,NULL,TO_ROOM);
        do_look( ch, "auto" );
    	act("You dismount from a Giant Roc as you reach your home temple.",ch,NULL,NULL,TO_CHAR);
    	act( "$n dismounts from the giant roc as it wings off back into the sky.",ch,NULL,NULL,TO_ROOM );
        broadcast(ch, "You hear the rustling of giant wings nearby.\n\r");
        WAIT_STATE2(ch, PULSE_VIOLENCE * 2);
  }
}


/* ROYAL POWERS */
//summons an assassin to stalk a victim
void spell_stalker( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
    CHAR_DATA *victim, *stalker;
    AFFECT_DATA af;
    int i;

    if (is_affected(ch, sn)){
      send_to_char("You cannot order a new assassination yet.\n\r", ch);
      handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
      return;
    }
    else if (ch->class == class_lookup("healer")){
      send_to_char("Healers may not order assassinations.\n\r", ch);
      handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
      return;
    }
    else if ( ( victim = get_char_world( ch, target_name ) ) == NULL
	      || victim == ch
	      || victim->in_room == NULL
	      || IS_IMMORTAL(victim)
	      || IS_NPC(victim) ){
      send_to_char("No such target is present.\n\r", ch);
      handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
      return;
    }
    if (victim->level < 20){
      send_to_char("The assassins refuse to murder one so young.\n\r", ch);
      handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
      return;
    }
    else if (ch->in_room->area != victim->in_room->area){
      send_to_char("You must be in the same area as your victim.\n\r", ch);
      handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
      return;
    }
    else if (is_ghost(victim, 600)){
      send_to_char("Not on ghosts.\n\r", ch);
      handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
      return;
    }
    act("You order assassination of $N.", ch, NULL, victim, TO_CHAR);

    stalker = create_mobile( get_mob_index(MOB_VNUM_STALKER) );
    for (i=0;i < MAX_STATS; i++)
	stalker->perm_stat[i] = victim->perm_stat[i];

    stalker->max_hit = UMIN(30000, 2 * victim->max_hit);
    stalker->hit = stalker->max_hit;
    stalker->max_mana = victim->max_mana;
    stalker->mana = stalker->max_mana;
    stalker->level = victim->level + 3;
    stalker->hitroll = 10 + level / 2;
    stalker->damroll = 10 + level / 2;
    stalker->race = ch->race;
    stalker->alignment = 0;

    for (i=0; i < 3; i++)
      stalker->armor[i] = interpolate(stalker->level,100,-100);
    stalker->armor[3] = interpolate(stalker->level,100,0);

    stalker->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
    stalker->hunting = victim;
    act("You sense a movement in the shadows..", victim, NULL,NULL,TO_ALL);

    af.type = sn;
    af.level = level;
    af.duration = 12;
    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_char( ch, &af);
    af.type = gsn_timer;
    affect_to_char( stalker, &af);

    char_to_room(stalker,victim->in_room);
}

void spell_marshall( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA *tunic;
  char buf[MSL];
  AFFECT_DATA af, *paf;

  if (is_affected(ch, sn) || ch->pCabal == NULL || ch->pCabal->city == NULL){
    send_to_char("You're not ready to marshal your troops yet.\n\r", ch);
    return;
  }

  tunic = create_object( get_obj_index(OBJ_VNUM_ROYAL_FLAG), level );
  tunic->timer = ch->level * 2;
  paf = new_affect();
  paf->type       = sn;
  paf->level      = level;
  paf->duration   = -1;
  paf->location   = APPLY_HITROLL;
  paf->modifier   = number_fuzzy(level / 10);
  paf->bitvector  = 0;
  paf->next       = tunic->affected;
  tunic->affected   = paf;
  paf = new_affect();
  paf->type       = sn;
  paf->level      = level;
  paf->duration   = -1;
  paf->location   = APPLY_DAMROLL;
  paf->modifier   = number_fuzzy(level / 10);
  paf->bitvector  = 0;
  paf->next       = tunic->affected;
  tunic->affected   = paf;
  paf = new_affect();
  paf->type       = sn;
  paf->level      = level;
  paf->duration   = -1;
  paf->location   = APPLY_HIT;
  paf->modifier   = level / 2;
  paf->bitvector  = 0;
  paf->next       = tunic->affected;
  tunic->affected   = paf;

  sprintf( buf, "a banner bearing the Royal Crest of %s", ch->pCabal->city->name);
  free_string( tunic->short_descr );
  tunic->short_descr = str_dup( buf );
  sprintf( buf, "A banner bearing the Royal Crest of %s is lying here.", ch->pCabal->city->name);
  free_string( tunic->description );
  tunic->description = str_dup( buf );

  if (ch->alignment >= GOOD_THRESH){
    tunic->extra_flags = ITEM_ANTI_EVIL | ITEM_ANTI_NEUTRAL; }
  else if ((ch->alignment < GOOD_THRESH) && (ch->alignment > EVIL_THRESH)){
    tunic->extra_flags = ITEM_ANTI_GOOD | ITEM_ANTI_EVIL; }
  else{
    tunic->extra_flags = ITEM_ANTI_GOOD | ITEM_ANTI_NEUTRAL; }

  tunic->pCabal = ch->pCabal;

  obj_to_char(tunic, ch);

  af.type               = sn;
  af.level              = level;
  af.duration           = level*2;
  af.bitvector          = 0;
  af.modifier           = 0;
  af.location           = APPLY_NONE;
  affect_to_char(ch, &af);
  act("You unfurl and hoist $p!",ch,tunic,NULL,TO_CHAR);
  act("You begin to marshall your troops under $p.",ch,tunic,NULL,TO_CHAR);
  act("$n unfurls and hoists $p!",ch,tunic,NULL,TO_ROOM);

}

struct royal_guards{
  int act;
  char* race;
  char* attack;
  char* name;
  char* short_descr;
  char* long_descr;
};

const struct royal_guards royal_guard_table [] = {
  /* VALMIRAN */
  {0,
   "human",
   "punch",
   "farmers",
   "a farmer",
   "A bunch of farmers stand nearby.\n\r"
  },
  {ACT_WARRIOR,
   "storm",
   "smash",
   "storm giants",
   "a storm giant",
   "A group of storm giants towers over the area.\n\r"
  },
  {ACT_MAGE,
   "elf",
   "magic",
   "elven mages",
   "An elven mage",
   "A group of elven mages stands whispering amongst themselves.\n\r"
  },

  /* RHEYDIN */
  {0,
   "goblin",
   "scratch",
   "goblin vagabonds",
   "a goblin vagabond",
   "A pack of goblin vagabonds litters the area.\n\r"
  },
  {ACT_WARRIOR,
   "dwarf",
   "cleave",
   "dwarven guard",
   "a dwarven guard",
   "A group of grumpy dwarves gives you a long quiet stare.\n\r"
  },
  {ACT_WARRIOR | ACT_UNDEAD,
   "golem",
   "punch",
   "steam men",
   "a steam man",
   "Hissing and whistling a pair of steam golems stands here.\n\r"
  },

  /* MIRUVHOR */
  {0,
   "human",
   "slap",
   "Miruvhor's \"ladies\"",
   "a prostitute",
   "A group of Miruvhor's \"ladies\" smile in your direction.\n\r"
  },
  {ACT_WARRIOR | ACT_MAGE,
   "human",
   "black knight",
   "black knights",
   "a black knight",
   "A squad of grim knights dressed in black stands here.\n\r"
  },
  {ACT_WARRIOR | ACT_UNDEAD,
   "vampire",
   "taint",
   "death knights",
   "a death knight",
   "A pair of death knights stand near their eyes devoid of any emotion.\n\r"
  },
};

//allows player to set their royal guard strings
void do_royal( CHAR_DATA *ch, char *argument ){
  if (!IS_ROYAL(ch) && !IS_NOBLE(ch)){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (!IS_NULLSTR(ch->pcdata->royal_guards)
	   && !ch->pcdata->royal_confirm){
    send_to_char("Once set, you cannot change your royal guards.", ch);
    return;
  }
  //two possibilities, either this is to confirm or enter a new string
  if (ch->pcdata->royal_confirm){
    if(str_cmp("confirm", argument)){
      free_string(ch->pcdata->royal_guards);
      ch->pcdata->royal_guards = NULL;
      ch->pcdata->royal_confirm = FALSE;
      send_to_char("Royal guards cleared.  Use \"royal\" to try again.\n\r", ch);
      return;
    }
    //otherise set the string permamently
    ch->pcdata->royal_confirm = FALSE;
    send_to_char("Royal guard name set.\n\r", ch);
    return;
  }
  if (IS_NULLSTR(argument)){
    send_to_char("Enter a singular form of the creature you wish to be your guard.\n\rExample \"royal a farmer\"\n\r", ch);
    return;
  }
  else{
    //show the strings and ask for confirm
    char* pref;
    if (LOWER(argument[0]) == 'a'
	|| LOWER(argument[0]) == 'e'
	|| LOWER(argument[0]) == 'i'
	|| LOWER(argument[0]) == 'o'
	|| LOWER(argument[0]) == 'u')
      pref = "An";
    else
      pref = "A";
    sendf(ch, "Using singular of \"%s\" here are example results:\n\r"\
	  "Name: %ss\n\r"\
	  "%ss come to protect you!\n\r"\
	  "%s %s's charge `1wounds`` you.\n\r"\
	  "%s %s bows.\n\r"\
	  "A few %ss bearing the crest of %s are here.\n\r",
	  argument,
	  argument,
	  capitalize(argument),
	  pref, argument,
	  pref, argument,
	  argument,
	  hometown_table[ch->hometown].name);
    send_to_char("\n\rIf this is to your liking use \"royal confirm\" to set.\n\r"\
		 "`!WARNING``: You can set your royal guards only ONCE.\n\r"\
		 "Use \"royal\" to select a new string.\n\r", ch);
    free_string(ch->pcdata->royal_guards );
    ch->pcdata->royal_guards = str_dup( argument );
    ch->pcdata->royal_confirm = TRUE;
  }
}

void create_royal_guard( CHAR_DATA* ch, int align, int level ){
  CHAR_DATA *gch = NULL;
  CHAR_DATA *guard;
  AFFECT_DATA* paf, af;
  AREA_DATA* city;
  ROOM_INDEX_DATA* room;
  char buf[MIL];
  char* pref;
  int i, table;
  const int gsn_royal_call = skill_lookup("royal escort");

  if ( (!IS_ROYAL(ch) && !IS_NOBLE(ch))
       || !(IS_AREA(ch->in_room->area, AREA_CITY))
       || !is_affected(ch, gsn_royal_call)){
    return;
  }
  //check for an existing royal guard we might have
  for (gch = char_list; gch; gch = gch->next){
    if (IS_NPC(gch) && gch->pIndexData->vnum == MOB_VNUM_ROYAL_GUARD
	&& gch->summoner == ch)
      break;
  }
  if (gch != NULL)
    return;

  /* check for hometown */
  if ( (room = get_room_index(hometown_table[ch->hometown].g_vnum)) != NULL)
    city = room->area;
  else if ( (room = get_room_index(hometown_table[ch->hometown].n_vnum)) != NULL)
    city = room->area;
  else if ( (room = get_room_index(hometown_table[ch->hometown].e_vnum)) != NULL)
    city = room->area;
  else{
    send_to_char("You lack a homecity to draft your escort from.\n\r",ch);
    return;
  }

  //select the level of the mob
  level = URANGE(1, level, 3) - 1;
  table = level;

  //offset the level into the proper align spot in the table
  if (IS_NEUTRAL(ch))
    table += 3;
  else if (IS_EVIL(ch))
    table += 6;


  guard = create_mobile( get_mob_index(MOB_VNUM_ROYAL_GUARD) );

  guard->race = UMAX(0, race_lookup(royal_guard_table[table].race));
  if (ch->pcdata->royal_guards)
    guard->dam_type = UMAX(0, attack_lookup("charge"));
  else
    guard->dam_type = UMAX(0, attack_lookup(royal_guard_table[table].attack));

  //set the stats
  for (i = 0; i < MAX_STATS; i++)
    guard->perm_stat[i] = 20 + level * 2;
  guard->max_hit = (15 + 15 * level)  * ch->level;
  guard->hit = guard->max_hit;
  guard->max_mana = (15 + 15 * level)  * ch->level;
  guard->mana = guard->max_mana;
  guard->alignment = ch->alignment;
  guard->level = ch->level - 2 + (2 * level);

  for (i = 0; i < 3; i++)
    guard->armor[i] = interpolate(guard->level,100,-100);
  guard->armor[3] = interpolate(guard->level,100,0);

  guard->hitroll    = (ch->level / 10) * 2 +  5 * level;
  guard->damroll    = (ch->level / 10) * 2 + 10 * level;

  guard->damage[DICE_NUMBER]  = 5;
  guard->damage[DICE_TYPE]    = 5;

  guard->sex = ch->sex;
  guard->pCabal = ch->pCabal;

  free_string( guard->name );
  if (ch->pcdata->royal_guards){
    sprintf( buf, "%s", ch->pcdata->royal_guards);
  }
  else
    sprintf( buf, "%ss", royal_guard_table[table].name);
  guard->name = str_dup(  buf );

  free_string( guard->long_descr );
  if (ch->pcdata->royal_guards){
    sprintf( buf, "A few %ss bearing the crest of %s are here.\n\r",
	     ch->pcdata->royal_guards,
	     city->name);
  }
  else
    sprintf( buf, "%s", royal_guard_table[table].long_descr);
  guard->long_descr = str_dup( buf );

  sprintf( buf, "%s", ch->pcdata->royal_guards);
  if (LOWER(buf[0]) == 'a'
      || LOWER(buf[0]) == 'e'
      || LOWER(buf[0]) == 'i'
      || LOWER(buf[0]) == 'o'
      || LOWER(buf[0]) == 'u')
    pref = "an";
  else
    pref = "a";

  free_string( guard->short_descr );
  if (ch->pcdata->royal_guards){
    sprintf( buf, "%s %s", pref, ch->pcdata->royal_guards);
  }
  else
    sprintf( buf, "%s", royal_guard_table[table].short_descr);
  guard->short_descr = str_dup( buf );

  char_to_room(guard, ch->in_room);
  SET_BIT(guard->affected_by, AFF_CHARM);
  guard->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
  guard->summoner = ch;
  add_follower(guard,ch);
  guard->leader = ch;


  //set timer on guard according to duration of spellon user
  if ( (paf = affect_find(ch->affected, gsn_royal_call)) != NULL)
    af.duration = paf->duration;
  else
    af.duration = 12;;
  af.type = gsn_timer;
  af.level = ch->level;
  af.where = TO_AFFECTS;
  af.bitvector = 0;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char( guard, &af);

  /* Royal cannot hide ghosted */
  ch->pcdata->ghost = (time_t)NULL;
  act("$t come to protect you!",ch, guard->name, NULL,TO_CHAR);
  act("A group of $t arrive at $n's side!",ch, guard->name, NULL,TO_ROOM);
}

void do_escort( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af;
  int rank = number_range(0, RANK_LEADER);
  const int sn = skill_lookup("royal escort");

  if (!IS_ROYAL(ch) && !IS_NOBLE(ch)){
    send_to_char("You are not worthy of such an honor.\n\r",ch);
    return;
  }
  else if (is_affected(ch, sn)){
    send_to_char("You are already protected by loyal subjects.\n\r", ch);
    return;
  }

  af.type               = sn;
  af.level              = ch->level;
  af.duration           = 24;
  af.bitvector          = 0;
  af.modifier           = 0;
  af.location           = APPLY_NONE;
  affect_to_char(ch, &af);

  send_to_char("You send an order for additional guards.\n\r",ch);
  act("$n sends for additional guards to protect $n.", ch, NULL, NULL, TO_ROOM);
  create_royal_guard( ch, ch->alignment, rank );
}

/* checks if the character has been exiled from given area */
AFFECT_DATA* is_exiled( CHAR_DATA* ch, int vnum ){
  CABAL_DATA* pCab = get_parent( ch->pCabal );
  AFFECT_DATA* paf;

  if (ch == NULL)
    return NULL;
  if (pCab && pCab->anchor && pCab->anchor->area->vnum == vnum)
    return NULL;
  for (paf = ch->affected; paf; paf = paf->next ){
    if (paf->type == gsn_mark_fear)
      return paf;
    else if (paf->type == gsn_exile && paf->modifier == vnum )
      return paf;
  }
  return NULL;
}

/* Exiles a character from character's royal cabal's area */
void do_exile( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af;
  CHAR_DATA* victim, *vch;
  AREA_DATA* city;
  ROOM_INDEX_DATA* room;
  char buf[MIL];
  int cost = skill_table[gsn_exile].min_mana;
  int lag = skill_table[gsn_exile].beats;

  if (!IS_ROYAL(ch) && !IS_NOBLE(ch)){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  else if (IS_NULLSTR(argument)){
    send_to_char("Exile whom?\n\r", ch);
    return;
  }
  else if ( (victim = get_char_world(ch, argument)) == NULL){
    send_to_char("You must be able to see the one you are about to exile.\n\r", ch );
    return;
  }
  else if (victim == ch){
    send_to_char("Exile yourself? Just leave the city instead.\n\r", ch);
    return;
  }
  else if (IS_AFFECTED2(ch, AFF_SILENCE)){
    send_to_char("As you try to give the order, no sound comes out!\n\r",ch);
    return;
  }
  if (is_affected(ch, gsn_gag)){
    send_to_char("You try to work your mouth but the gag holds fast.\n\r", ch);
    return;
  }
  if (is_affected(ch,gsn_uppercut)){
    send_to_char("Your jaws hurts too much to move.\n\r",ch);
    return;
  }

  /* get the area vnum of this char's city */
  if ( (room = get_room_index(hometown_table[ch->hometown].g_vnum)) != NULL)
    city = room->area;
  else if ( (room = get_room_index(hometown_table[ch->hometown].n_vnum)) != NULL)
    city = room->area;
  else if ( (room = get_room_index(hometown_table[ch->hometown].e_vnum)) != NULL)
    city = room->area;
  else{
    send_to_char("You lack a homecity to exile from.\n\r",ch);
    return;
  }

  if (is_exiled( victim, city->vnum)){
    send_to_char("They've already been exiled from your lands.\n\r", ch );
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You are too tired to order your guards about effectivly.\n\r", ch );
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2( ch, lag );

/* start the gen */
  af.type	= gsn_exile;
  af.level	= ch->level;
  af.duration	= 600;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.modifier	= city->vnum;
  af.location	= APPLY_NONE;
  affect_to_char(victim, &af );


  /* echo the exile message */
  sprintf(buf, "A royal page hands you a scroll:\n\r"\
	  "By the royal order of %s, %s has been exiled from %s!\n\r",
	  PERS2( ch ),
	  PERS2( victim),
	  city->name);

  for (vch = player_list; vch; vch = vch->next_player){
    if (vch->desc && IS_AWAKE(vch))
      send_to_char( buf, vch );
  }
}

/* moves the royal out of pk as long as:
1) They are in their home city/Cabal
2) They are surrounded by rank - 2 guards whom are of their cabal or allied cabal

Abduct ignores effects of diplomacy
*/
void do_diplomacy( CHAR_DATA* ch, char* argument ){
  AFFECT_DATA af;
  CABAL_DATA* pCabal = get_parent( ch->pCabal );
  const int gsn_diplomacy = skill_lookup("diplomacy");
  const int lag = skill_table[gsn_diplomacy].beats;
  char buf[MIL];

  if (!IS_ROYAL(ch) && !IS_NOBLE(ch) && get_skill(ch, gsn_diplomacy) < 2){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  else if (is_affected(ch, gen_diplomacy)){
    send_to_char("You have already declared your diplomatic immunity.\n\r", ch);
    return;
  }
  else if (!IS_AREA(ch->in_room->area, AREA_CITY) && !IS_AREA(ch->in_room->area, AREA_CABAL)){
    send_to_char("You may only declare diplomatic immunity in a city, or a cabal.\n\r", ch );
    return;
  }
  else if (is_fight_delay(ch, 120)){
    send_to_char("There is still blood on your hands!\n\r", ch );
    return;
  }


  sprintf( buf, "%s has declared Diplomatic Immunity.", PERS2(ch));
  if (pCabal){
    cabal_echo(get_parent(pCabal), buf );
    /* subtract cp's */
    handle_skill_cost( ch, ch->pCabal, gsn_diplomacy );
  }
  cabal_echo_flag(CABAL_HEARGUARD, buf );

  WAIT_STATE2(ch, lag );

  af.type	= gen_diplomacy;
  af.level	= ch->level;
  af.duration	= -1;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= TRUE; //turned on.
  affect_to_char( ch, &af );

  act("A royal entourage surrounds $n, a large white flag clearly visible.", ch, NULL, NULL, TO_ROOM);
  act("A royal entourage surrounds you, a large white flag clearly visible.", ch, NULL, NULL, TO_CHAR);
}


/* SYNDICATE POWERS */
void do_bribe( CHAR_DATA *ch, char *argument ){
  AFFECT_DATA af;

  int cost = skill_table[gsn_bribe].min_mana;
  int lag = skill_table[gsn_bribe].beats;

  if (get_skill(ch, gsn_bribe) < 2){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You are too tired to charm the guards into taking the bribe.\n\r", ch );
    return;
  }
  else
    ch->mana -= cost;

  WAIT_STATE2( ch, lag );

/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, gsn_bribe );


  send_to_char("You send a sizable bribe for your safe passage through lawful areas.\n\r", ch);
  af.type	= gsn_bribe;
  af.level	= ch->level;
  af.duration	= 120;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;

  affect_to_char( ch, &af );

}


/* performs secret_arts actions */
bool check_secart_actions( CHAR_DATA* ch){
  CHAR_DATA* victim = ch->fighting;
  int skill = 0;
  int chance = 0;
  const int sn_throat = skill_lookup("throat cut");
  int max = 5;

  if (victim == NULL || !is_affected(ch, gsn_secarts))
    return FALSE;
  else
    skill = get_skill(ch, gsn_secarts );

  if (skill < 2 )
    return FALSE;
  /* get the chance (done once per combat) */

  if (get_group_room( ch, TRUE) != NULL)
    chance = skill / 5;
  else
    chance = 5 * skill / 10;

//DEBUG  sendf(ch, "skill: %d, chance %d\n\r", skill , chance);

  if (number_percent() > chance){
    check_improve(ch, gsn_secarts, FALSE, 1);
    return FALSE;
  }
  else
    check_improve(ch, gsn_secarts, TRUE, 1);

  /* miss */
  if (number_percent() > 4 * skill / 5){
    act("$n's form blurs with speed!",ch, NULL, NULL, TO_NOTVICT);
    damage(ch, victim, 0, gsn_secarts, DAM_INTERNAL, TRUE);
    return FALSE;
  }
  if (get_skill(ch, sn_throat) > 1 && !is_affected(victim, gen_bleed)){
    max = 7;
  }
  else
    max = 5;

  switch (number_range(0, max)){
/* LEGS */
  case 0:
  case 1:
    act("With unnatural speed you strike at $N's legs.", ch, NULL, victim, TO_CHAR);
    act("$n's form blurs with speed!",ch, NULL, NULL, TO_NOTVICT);
    send_to_char("Sharp pain explodes in your leg!\n\r", victim);
    damage(ch, victim, number_range(5, 15), gsn_secarts, DAM_INTERNAL, TRUE);
    victim->move = UMAX(0, victim->move - number_range(25, 35));
    break;
/* BLIND */
  case 2:
  case 3:
    act("With unnatural speed you strike at $N's eyes.", ch, NULL, victim, TO_CHAR);
    act("$n's form blurs with speed!",ch, NULL, NULL, TO_NOTVICT);
    if (!IS_AFFECTED(victim, AFF_BLIND)){
      /* blind target */
      AFFECT_DATA af;
      af.type = gsn_blindness;
      af.level = ch->level;
      af.duration = number_range(0, 1);
      af.where     = TO_AFFECTS;
      af.bitvector = AFF_BLIND;
      af.location  = APPLY_HITROLL;
      if (!IS_NPC(victim)
	  && number_percent() < get_skill(victim,gsn_blind_fighting))
	af.modifier      = -5;
      else
	af.modifier      = -10;
      affect_to_char(victim,&af);
      af.location     = APPLY_AC;
      if (!IS_NPC(victim)
	  && number_percent() < get_skill(victim,gsn_blind_fighting))
	af.modifier      = +15;
      else
	af.modifier      = +25;
      affect_to_char(victim,&af);
      act_new( "You are blinded!", victim,NULL,NULL,TO_CHAR,POS_DEAD );
      act("$n appears to be blinded.",victim,NULL,NULL,TO_ROOM);
    }
    damage(ch, victim, 5, gsn_secarts, DAM_INTERNAL, TRUE);
    break;
/* DAMAGE */
  case 4:
  case 5:
    act("With unnatural speed you strike at $N's torso.", ch, NULL, victim, TO_CHAR);
    act("$n's form blurs with speed!",ch, NULL, NULL, TO_NOTVICT);
    send_to_char("Sharp pain explodes in your torso!\n\r", victim);
    damage(ch, victim, number_range(15, 30), gsn_secarts, DAM_INTERNAL, TRUE);
    break;
  case 6:
  case 7:
    if (!is_affected(victim, gen_bleed)){
      throatcut(ch, ch->fighting, FALSE);
    }
  }
  return (ch->fighting == NULL);
}

void do_secret_arts(CHAR_DATA* ch, char* argument){
  AFFECT_DATA af;
  const int cost =skill_table[gsn_secarts].min_mana;
  const int lag = skill_table[gsn_secarts].beats;
  const int skill = get_skill(ch,gsn_secarts);

  if (get_skill(ch, gsn_secarts) < 2){
    send_to_char("Huh?\n\r", ch );
    return;
  }
  else if (is_affected(ch, gsn_secarts)){
    send_to_char("You are already using the secret arts!\n\r", ch);
    return;
  }
  else if (ch->mana < cost){
    send_to_char("You cannot concentrate sufficiently.\n\r", ch );
    return;
  }
  else
    ch->mana -= cost;


  WAIT_STATE2( ch, lag );

/* ON FAIL */
  if (number_percent() > skill ){
    send_to_char("You failed to recall the ancient teachings.\n\r", ch);
    check_improve(ch, gsn_secarts, FALSE, 1 );
    return;
  }
/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, gsn_secarts );
/* ON SUCCESS */
  send_to_char("You will now use the secret arts.\n\r", ch);
  check_improve(ch, gsn_secarts, TRUE, 1 );

  af.type	= gsn_secarts;
  af.level	= ch->level;
  af.duration	= 24;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char(ch, &af);
}


/* causes few hours of bleeding on success */
void do_throatcut(CHAR_DATA* ch, char* argument){
  CHAR_DATA* victim;

  const int sn = skill_lookup("throat cut");
  const int cost = skill_table[sn].min_mana;
  const int lag = skill_table[sn].beats;

  int  chance = 4 * get_skill(ch, sn) / 5;

  if ( chance < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument) && !ch->fighting){
    send_to_char("Cut who's throat?\n\r", ch);
    return;
  }

  /* get target */
  if (ch->fighting)
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
  else if (is_safe(ch,victim))
    return;
  else if (!IS_NPC(ch) && ch->mana < cost){
    send_to_char("You lack the strength.\n\r", ch);
    return;
  }
  else ch->mana -= cost;

/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, sn );
  WAIT_STATE2( ch, lag );

  throatcut(ch, victim, TRUE );
}


void spell_asphyxiate( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af, *paf;
  int mod = get_curr_stat(victim, STAT_CON) - 20;

  if (is_affected(victim, gen_suffocate)){
    act("$N is already suffocating.\n\r", ch, NULL, victim, TO_CHAR);
    return;
  }
  if (!IS_AWAKE( victim ))
    mod -= 5;

  if (saves_spell( level - mod, victim, DAM_DROWNING, skill_table[sn].spell_type) ){
    send_to_char("You failed.\n\r", ch );
    return;
  }
  act("You can't draw any air into your lungs!\n\r", ch, NULL, victim, TO_VICT);
  act("$n begins to choke and claw at $s throat!", victim, NULL, victim, TO_ROOM);
  af.type	= gen_suffocate;
  af.level	= level - mod;
  af.duration	= number_range(3, 5);		//how long before person falls asleep for 1 tick

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 2;	//DO DAMAGE AFTER 1 TICK
  paf = affect_to_char( victim, &af );
  string_to_affect( paf, ch->name );
}

/* causes person to be exiled from all areas */
void spell_mark_fear( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim;
  AFFECT_DATA af;
  char buf[MIL];

  if (ch->pCabal == NULL )
    return;
  if (IS_NULLSTR(target_name)){
    send_to_char("Place the bloodmark on who?\n\r", ch );
    return;
  }
  else if ( (victim = get_char( target_name)) == NULL){
      send_to_char("They do not seem to be about.\n\r", ch );
      return;
  }
  else if (IS_NPC(victim)){
    send_to_char("Not on mobs.\n\r", ch);
    return;
  }
  else if (GET_CP(ch) < 350){
    sendf(ch, "You will need 350 %ss first.\n\r", ch->pCabal->currency );
    return;
  }
  else if (is_affected(victim, gsn_mark_fear)) {
    send_to_char("They've already been marked.\n\r", ch);
    return;
  }
  CP_GAIN(ch, -350, TRUE );
  af.where     = TO_AFFECTS;
  af.type      = gsn_mark_fear;
  af.level     = level;
  af.duration  = 720;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = 0;
  affect_to_char(victim, &af );
  act_new( "You sense you have been marked somehow.", victim, NULL, NULL, TO_CHAR, POS_DEAD );

  sprintf( buf, "%s has placed the Bloodmark upon %s!", PERS2(ch), PERS2(victim));
  cabal_echo(get_parent(ch->pCabal), buf );
}

/* SAVANT POWERS */
void spell_rev_time( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn)){
    send_to_char("You are not ready to revrese the timestream once again!\n\r", ch );
    return;
  }

  act("Wielding the powers of Time, you reverse its flow about yourself!", ch, NULL, NULL, TO_CHAR);
  act("You notice a piece of dirt fly up from the ground towards $n's clothes.", ch, NULL, NULL, TO_ROOM);

  broadcast( ch, "A drop of moisture flies up from the ground and rises towards the sky." );

  if (IS_AFFECTED(ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_HASTE)){
    affect_strip(ch, skill_lookup("slow") );
    affect_strip(ch, skill_lookup("haste") );
    send_to_char("Some of your spells have been disrupted.\n\r", ch );
  }

  af.type	= gsn_ancient_lore; //dummy gsn just for the effect
  af.level	= level;
  af.duration	= 2 + UMAX(0, (get_skill(ch, sn) - 60)) / 10;
  af.where	= TO_NONE;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= ch->in_room->vnum;
  affect_to_char( ch, &af );

  af.type = sn;
  af.where = TO_AFFECTS;
  af.duration = 1;
  af.modifier = 0;
  affect_to_char( ch, &af );
}

/* setups up the gen_chron */
void spell_chrono_shield( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  AFFECT_DATA af;
  CHAR_DATA* victim = (CHAR_DATA*) vo;

  if (is_affected(victim, gen_chron)){
    if (ch == victim)
      send_to_char("You've already created a Chrono Shield.\n\r", ch );
    else
      act("$N is already protected by a Chrono Shield.", ch, NULL, victim, TO_CHAR );
  }

  if (ch == victim)
    act("You fashion an energy conduit to yourself in near future.", ch, NULL, NULL, TO_CHAR );
  else
    act("You fashion an energy conduit to $N in near future.", ch, NULL, victim, TO_CHAR );
  act("A strange vortex of translucent bright colors flickers before $n.", victim, NULL, NULL, TO_ROOM);


 if (IS_AFFECTED(ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_HASTE)){
    affect_strip(ch, skill_lookup("slow") );
    affect_strip(ch, skill_lookup("haste") );
    send_to_char("Some of your spells have been disrupted.\n\r", ch );
  }

  af.type	= gen_chron;
  af.level	= level;
  af.duration	= 12;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;	//stores damage

  affect_to_char( victim, &af );
}


void spell_temporal_storm( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(ch, gen_temp_storm)){
    send_to_char("You've already created a temporal storm.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_HASTE)){
    affect_strip(ch, skill_lookup("slow") );
    affect_strip(ch, skill_lookup("haste") );
    send_to_char("Some of your spells have been disrupted.\n\r", ch );
    }
  if (IS_AFFECTED(victim, AFF_SLOW)){
    int sn = skill_lookup("slow");
    act(skill_table[sn].msg_off, victim, NULL, NULL, TO_CHAR);
    act(skill_table[sn].msg_off2, victim, NULL, NULL, TO_ROOM);
    affect_strip(victim, sn );
  }
  if (IS_AFFECTED(victim, AFF_HASTE)){
    int sn = skill_lookup("haste");
    act(skill_table[sn].msg_off, victim, NULL, NULL, TO_CHAR);
    act(skill_table[sn].msg_off2, victim, NULL, NULL, TO_ROOM);
    affect_strip(victim, sn );
  }
  act("A large vortex of temporal energies forms over $n!", victim, NULL, NULL, TO_ROOM);
  act("A large vortex of temporal energies forms over you!", victim, NULL, NULL, TO_CHAR);

  af.type	= gen_temp_storm;
  af.duration	= number_fuzzy(level / 6);
  af.level	= level;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_SPELL_LEVEL;
  af.modifier	= 1;

  affect_to_char(ch, &af);
}


/* savant wards */
void spell_ward_faith( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  AFFECT_DATA af, *paf;

  if (is_affected(ch, gen_ward)){
    send_to_char("You've already been warded!\n\r", ch);
    return;
  }

  act("You fashion a ward protecting you from blasphemy.", ch, NULL, NULL, TO_CHAR);
  act("$n fashions a ward protecting $m from blasphemy.", ch, NULL, NULL, TO_ROOM);

  af.type	= gen_ward;
  af.level	= level;
  af.duration	= 48;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  paf = affect_to_char(ch, &af);
  string_to_affect(paf, "blasphemy");
}

/* savant wards */
void spell_vocalize( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  AFFECT_DATA af, *paf;

  if (is_affected(ch, gen_ward)){
    send_to_char("You've already been warded!\n\r", ch);
    return;
  }

  act("You fashion a ward protecting you from silence.", ch, NULL, NULL, TO_CHAR);
  act("$n fashions a ward protecting $m from silence.", ch, NULL, NULL, TO_ROOM);

  af.type	= gen_ward;
  af.level	= level;
  af.duration	= 48;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  paf = affect_to_char(ch, &af);
  string_to_affect(paf, "silence");
}

/* savant wards */
void spell_ward_curse( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  AFFECT_DATA af, *paf;

  if (is_affected(ch, gen_ward)){
    send_to_char("You've already been warded!\n\r", ch);
    return;
  }

  act("You fashion a ward protecting you from a curse.", ch, NULL, NULL, TO_CHAR);
  act("$n fashions a ward protecting $m from a curse.", ch, NULL, NULL, TO_ROOM);

  af.type	= gen_ward;
  af.level	= level;
  af.duration	= 48;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  paf = affect_to_char(ch, &af);
  string_to_affect(paf, "curse");
}

/* paradox (timed, inverse gate, gen does all the work) */
void spell_paradox( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  AFFECT_DATA af;

  if (is_affected(ch, gen_paradox)){
    send_to_char("You've already initiated a paradox!\n\r", ch);
    return;
  }

  if ( IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
       || IS_SET(ch->in_room->room_flags, ROOM_SAFE)
       || IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
       || IS_SET(ch->in_room->room_flags, ROOM_NO_GATEIN)
       || IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)
       || IS_SET(ch->in_room->area->area_flags, AREA_NOMORTAL)
       || IS_SET(ch->in_room->area->area_flags, AREA_RESTRICTED)){
    send_to_char("You cannot create a paradox in this location.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }

  send_to_char("With a slight nudge to the timestream you cause a Paradox.\n\r", ch );
  if (IS_AFFECTED(ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_HASTE)){
    affect_strip(ch, skill_lookup("slow") );
    affect_strip(ch, skill_lookup("haste") );
    send_to_char("Some of your spells have been disrupted.\n\r", ch );
  }

  af.type	= gen_paradox;
  af.level	= level;
  af.duration	= 12;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= ch->in_room->vnum;
  affect_to_char( ch, &af );
}

/* age */
int rand_skill( CHAR_DATA* ch ){
  int max_skill = 0;
  int skills[MAX_SKILL];
  int i;

  if (IS_NPC(ch))
    return 0;

  for (i = 0; i < MAX_SKILL; i++){
    if (ch->pcdata->learned[i] > 50 && get_skill(ch, i) > 60)
      skills[max_skill++] = i;
  }

  if (max_skill < 1)
    return 0;
  else
    return (skills[number_range(0, max_skill)]);
}

void spell_age(int sn,int level,CHAR_DATA *ch,void *vo,int target){
  AFFECT_DATA af, *paf;
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  int mod = 1;
  int last_dur = 4;
  int forgotten = 0;
  char skills[MIL];


  /* count how many skills they have forgotten */
  for (paf = victim->affected; paf; paf = paf->next){
    if (paf->type == sn && paf->where == TO_SKILL){
      forgotten++;
      last_dur = paf->duration;
    }
  }
  if (last_dur >= 12){
    act("You've aged $N as far as you can.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (IS_AFFECTED(ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_HASTE)){
    affect_strip(ch, skill_lookup("slow") );
    affect_strip(ch, skill_lookup("haste") );
    send_to_char("Some of your spells have been disrupted.\n\r", ch );
  }
  if (IS_AFFECTED(victim, AFF_SLOW)){
    int sn = skill_lookup("slow");
    act(skill_table[sn].msg_off, victim, NULL, NULL, TO_CHAR);
    act(skill_table[sn].msg_off2, victim, NULL, NULL, TO_ROOM);
    affect_strip(victim, sn );
  }
  if (IS_AFFECTED(victim, AFF_HASTE)){
    int sn = skill_lookup("haste");
    act(skill_table[sn].msg_off, victim, NULL, NULL, TO_CHAR);
    act(skill_table[sn].msg_off2, victim, NULL, NULL, TO_ROOM);
    affect_strip(victim, sn );
  }


  if (!saves_spell(level,victim, DAM_OTHER, skill_table[sn].spell_type)){
    mod = 2;
    act("$n suddenly looks lot older.", victim, NULL, NULL, TO_ROOM );
    send_to_char("You suddenly feel a lot older.\n\r", victim );
  }
  else{
    act("$n suddenly looks older.", victim, NULL, NULL, TO_ROOM );
    send_to_char("You suddenly feel older.\n\r", victim );
    mod = 1;
  }
  /* setup the common things */
  af.type	= sn;
  af.level	= level;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.duration	= 0;

  if (!is_affected(victim, sn)){
    if (!IS_NPC(victim)){
      af.location	= APPLY_AGE;
      af.modifier	= 60 * pc_race_table[victim->race].lifespan / 100;
      affect_join(victim, &af );
    }
    af.location	= APPLY_SAVING_SPELL;
    af.modifier	= level / 5;
    affect_to_char(victim, &af );
  }

  skills[0] = 0;
  if (forgotten < 5){
    //forget 1 or 2 (no save) skills and increase duration
    //first skill forgotten
    af.location	= APPLY_NONE;
    af.where	= TO_SKILL;
    af.location	= rand_skill(victim);
    if (af.location){
      af.modifier	= -50;
      strcat(skills, skill_table[af.location].name);
      affect_to_char(victim, &af );
    }
  }
  if (mod > 1 && forgotten < 5){
    if (skills[0])
      strcat(skills, " and " );
    //second skill forgotten
    af.location	= APPLY_NONE;
    af.where	= TO_SKILL;
    af.location	= rand_skill(victim);
    if (af.location){
      af.modifier	= -50;
      strcat(skills, skill_table[af.location].name);
      affect_to_char(victim, &af );
    }
  }


  //increase durations if this is multiple cast
  for (paf = victim->affected; paf; paf = paf->next){
    if (paf->type == sn)
      paf->duration = last_dur + 2;
  }

  if (skills[0]){
    sendf(ch, "%s has forgotten %s!\n\r", PERS(victim, ch), skills);
    sendf(victim, "You've forgotten %s!\n\r", skills);
  }
  damage( ch, victim, number_range(5, 15), sn, DAM_INTERNAL, TRUE );
}


/* temporal avenger: sets up the gen which does most of the work */
void spell_temporal_avenger(int sn,int level,CHAR_DATA *ch,void *vo,int target){
  AFFECT_DATA af, *paf;
  CHAR_DATA* victim = (CHAR_DATA*) vo;

  if (is_affected(ch, sn)){
    send_to_char("You've already setup a temporal conduit!\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_HASTE)){
    affect_strip(ch, skill_lookup("slow") );
    affect_strip(ch, skill_lookup("haste") );
    send_to_char("Some of your spells have been disrupted.\n\r", ch );
  }
  if (IS_AFFECTED(victim, AFF_SLOW)){
    int sn = skill_lookup("slow");
    act(skill_table[sn].msg_off, victim, NULL, NULL, TO_CHAR);
    act(skill_table[sn].msg_off2, victim, NULL, NULL, TO_ROOM);
    affect_strip(victim, sn );
  }
  if (IS_AFFECTED(victim, AFF_HASTE)){
    int sn = skill_lookup("haste");
    act(skill_table[sn].msg_off, victim, NULL, NULL, TO_CHAR);
    act(skill_table[sn].msg_off2, victim, NULL, NULL, TO_ROOM);
    affect_strip(victim, sn );
  }

  if (!saves_spell(level,victim, DAM_OTHER, skill_table[sn].spell_type)
      && !IS_NPC(victim)){
    act("You've managed to create a temporal conduit to $N!", ch, NULL, victim, TO_CHAR);
  }
  else{
    send_to_char("You failed.\n\r", ch);
    return;
  }

  af.type	= gen_temp_avenger;
  af.level	= level;
  af.duration	= number_range(6, 9);

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  paf = affect_to_char( ch, &af);
  string_to_affect( paf, victim->name );
}


void spell_town_gate( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  ROOM_INDEX_DATA *room;
  CHAR_DATA* vch, *vch_next;
  int town = -1;
  int room_vnum = 0;

/* check for area */
  if (IS_SET(ch->in_room->room_flags, ROOM_NO_GATEOUT)
      || IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)
      || IS_SET(ch->in_room->area->area_flags, AREA_NOMORTAL)
      || IS_SET(ch->in_room->area->area_flags, AREA_RESTRICTED)
      || is_affected(ch, gsn_tele_lock) ){
    send_to_char( "Something seems to block your spell.\n\r", ch );
    /* subtract cp's */
    handle_skill_cost_percent( ch, ch->pCabal, sn, 100 );
    return;
  }
  else if (is_affected(ch,gen_ensnare)){
    send_to_char("You sense a powerful magical field preventing your departure.\n\r", ch);
    /* subtract cp's */
    handle_skill_cost_percent( ch, ch->pCabal, sn, 100 );
    return;
  }
  else if (is_fight_delay(ch, 60)){
    send_to_char("You cannot concentrate with the adrenaline rushing through your blood.\n\r",ch);
    /* subtract cp's */
    handle_skill_cost_percent( ch, ch->pCabal, sn, 100 );
    return;
  }

/* can gate, try to get the hometown */
  if ( (town = hometown_lookup( target_name )) < 0){
    int hometown;
    send_to_char("You mage gate to following towns.\n\r", ch);

    for ( hometown = 0; hometown_table[hometown].name != NULL && hometown < MAX_HOMETOWN; hometown++){
      sendf( ch, "%-2d. %s\n\r", hometown + 1, hometown_table[hometown].name);
    }
    /* subtract cp's */
    handle_skill_cost_percent( ch, ch->pCabal, sn, 100 );
    return;
  }

  /* got the town and can gate, get the room to gate to */
  if (IS_GOOD(ch))
    room_vnum = hometown_table[town].g_vnum;
  else if (IS_EVIL(ch))
    room_vnum = hometown_table[town].e_vnum;
  else
    room_vnum = hometown_table[town].n_vnum;

  /* if we did not get valid vnum above, we select first valid one */
  if (room_vnum < 1){
    if (hometown_table[town].g_vnum)
      room_vnum = hometown_table[town].g_vnum;
    else if (hometown_table[town].n_vnum)
      room_vnum = hometown_table[town].n_vnum;
    else if (hometown_table[town].e_vnum)
      room_vnum = hometown_table[town].e_vnum;
    else
      room_vnum = ch->in_room->vnum;
  }

  /* final check */
  if ( (room = get_room_index( room_vnum )) == NULL){
    send_to_char("Your destination did not exist!\n\r", ch);
    /* subtract cp's */
    handle_skill_cost_percent( ch, ch->pCabal, sn, 100 );
    return;
  }

  if (IS_AFFECTED(ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_HASTE)){
    affect_strip(ch, skill_lookup("slow") );
    affect_strip(ch, skill_lookup("haste") );
    send_to_char("Some of your spells have been disrupted.\n\r", ch );
  }

  for (vch = ch->in_room->people; vch != NULL; vch = vch_next){
    vch_next = vch->next_in_room;

    if ( vch->pCabal && IS_CABAL(get_parent(vch->pCabal), CABAL_NOMAGIC))
      continue;

    if (is_same_group(vch, ch) && vch != ch){
      send_to_char("Your surroundings begin to grow very insubstantial and change.\n\r", vch);
      if (is_affected(vch, gsn_tele_lock)){
	send_to_char("The teleportation lock stops you short.\n\r", vch);
	continue;
      }

      if (IS_AFFECTED(vch, AFF_SLOW)){
	int sn = skill_lookup("slow");
	act(skill_table[sn].msg_off, vch, NULL, NULL, TO_CHAR);
	act(skill_table[sn].msg_off2, vch, NULL, NULL, TO_ROOM);
	affect_strip(vch, sn );
      }
      if (IS_AFFECTED(vch, AFF_HASTE)){
	int sn = skill_lookup("haste");
	act(skill_table[sn].msg_off, vch, NULL, NULL, TO_CHAR);
	act(skill_table[sn].msg_off2, vch, NULL, NULL, TO_ROOM);
	affect_strip(vch, sn );
      }

      char_from_room(vch);
      char_to_room(vch, room);
      act("$n arrives suddenly.",vch,NULL,NULL,TO_ROOM);
      do_look(vch,"auto");
    }
  }
  send_to_char("Your surroundings begin to grow very insubstantial and change.\n\r", ch);
  char_from_room(ch);
  char_to_room(ch, room);
  act("$n arrives suddenly.",ch,NULL,NULL,TO_ROOM);
  do_look(ch,"auto");
}

/* checks character for given contingency flag,
 * if paf == NULL the function checks for paf
 * returns if contingency was triggered and paf removed
 */
bool check_contingency( CHAR_DATA* ch, AFFECT_DATA* pCon, int flag ){
  AFFECT_DATA* paf = pCon;
  char arg[MIL];

  if (IS_NPC(ch) || ch == NULL || flag < 1)
    return FALSE;
  else if (paf == NULL){
    if ( (paf = affect_find(ch->affected, gsn_cont)) == NULL)
      return FALSE;
  }
  if (!IS_SET(paf->modifier, flag))
    return FALSE;

  /* we have a matching flag, cast the spell */
  sprintf( arg, "'%s'", skill_table[paf->level].name);
  affect_strip(ch, gsn_cont);

  send_to_char("Your contingency spell activates!\n\r", ch);
  act("$n's contingency spell activates.\n\r", ch, NULL, NULL, TO_ROOM);

  if (ch->class != class_lookup("healer") &&
      ch->class != class_lookup("shaman") &&
      ch->class != class_lookup("cleric") &&
      ch->class != class_lookup("druid")  &&
      ch->class != class_lookup("paladin")){
    cast_new(ch, arg, FALSE, FALSE);
  }
  else
    cast_new(ch, arg, TRUE, FALSE);

  return TRUE;
}

/* syntax: contingency <condition> <spell> */
void do_contingency( CHAR_DATA* ch, char* argument ){
  AFFECT_DATA af;
  char cond[MIL];

  const int cost = skill_table[gsn_cont].min_mana;
  const int lag = skill_table[gsn_cont].beats;

  int flag = 0;
  int spell = 0;
  int skill = get_skill(ch, gsn_cont );

  argument = one_argument( argument, cond );
  if (skill < 2){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if (is_affected(ch, gsn_cont)){
    send_to_char("You already have a contingency ready.\n\r", ch);
    return;
  }
  else if (IS_NULLSTR(argument)){
    int cond = 0;
    send_to_char("Syntax: contingency <condition> <spell>\n\r", ch);
    send_to_char("You may choose following conditions:\n\r", ch);

    for (cond = 0; cont_table[cond].name != NULL; cond ++){
      sendf(ch, "%-2d. %s\n\r", cond + 1, cont_table[cond].name);
    }
    return;
  }
  else if ( (flag = flag_lookup_abs( cond, cont_table )) == NO_FLAG){
    int cond = 0;
    send_to_char("You may choose following conditions:\n\r", ch);

    for (cond = 0; cont_table[cond].name != NULL; cond ++){
      sendf(ch, "%-2d. %s\n\r", cond + 1, cont_table[cond].name);
    }
    return;
  }
  else if ( (spell = skill_lookup( argument)) < 1
	    || get_skill(ch, spell) < 1){
    send_to_char("You are not skilled in that.\n\r", ch);
    return;
  }
  else if (skill_table[spell].spell_fun == spell_null){
    sendf(ch, "%s is not a spell.\n\r", skill_table[spell].name);
    return;
  }
  else if (skill_table[spell].target != TAR_CHAR_DEFENSIVE
	   && skill_table[spell].target != TAR_CHAR_SELF){
    send_to_char("The spell must be defensive in nature.\n\r", ch);
    return;
  }

  if (ch->mana < cost ){
    send_to_char("You cannot concentrate sufficiently.\n\r", ch );
    return;
  }
  else
    ch->mana -= cost;

  sendf(ch, "You setup a contingency for casting %s.\n\r", skill_table[spell].name);
  act("$n seems to meditate for a moment while muttering an incanation.", ch, NULL, NULL, TO_ROOM);

/* subtract cp's */
  handle_skill_cost( ch, ch->pCabal, gsn_cont );
  WAIT_STATE2(ch, lag );

  /* we have the bit and the spell form the af now */
  af.type	= gsn_cont;
  af.level	= spell;//holds the spell vnum to cast
  af.duration	= 36;

  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= flag;//holds the condition
  affect_to_char( ch, &af );
}



/* Written by: Virigoth							*
 * Returns: void							*
 * Used: cabal.c,							*
 * Comment: Savant Elder ability to cause victim's spell cost to increase *
 * the extra mana is transfered to owner on each cast			*
*/
void spell_ocularis( int sn, int level, CHAR_DATA *ch, void *vo,int target){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af, *paf;

  if (is_affected(victim, sn)){
    act("$N is already marked with the Eye.", ch, NULL, victim, TO_CHAR);
    return;
  }
  else if (saves_spell(level, victim, DAM_OTHER, skill_table[sn].spell_type) ){
    send_to_char("You failed.\n\r", ch);
    send_to_char("Your skin tingles with strange energies.\n\r", victim);
    return;
  }

  if (IS_AFFECTED(ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_HASTE)){
    affect_strip(ch, skill_lookup("slow") );
    affect_strip(ch, skill_lookup("haste") );
    send_to_char("Some of your spells have been disrupted.\n\r", ch );
  }
  if (IS_AFFECTED(victim, AFF_SLOW)){
    int sn = skill_lookup("slow");
    act(skill_table[sn].msg_off, victim, NULL, NULL, TO_CHAR);
    act(skill_table[sn].msg_off2, victim, NULL, NULL, TO_ROOM);
    affect_strip(victim, sn );
  }
  if (IS_AFFECTED(victim, AFF_HASTE)){
    int sn = skill_lookup("haste");
    act(skill_table[sn].msg_off, victim, NULL, NULL, TO_CHAR);
    act(skill_table[sn].msg_off2, victim, NULL, NULL, TO_ROOM);
    affect_strip(victim, sn );
  }

  act("$n inscribes an eerie rune of an open eye.", ch, NULL, victim, TO_ROOM);
  act("You focus the forces of Eye of Magic onto $N!", ch, NULL, victim, TO_CHAR);

  af.type = sn;
  af.level = level;
  af.duration = number_fuzzy(level / 6);

  af.where = TO_AFFECTS;
  af.bitvector = 0;

  af.modifier = -15;
  af.location = APPLY_MANA_GAIN;
  paf = affect_to_char(victim, &af);
  string_to_affect( paf, ch->name );
}


/* cause the victim to randomly enter the same room they existed on move */
void spell_ripple( int sn, int level, CHAR_DATA *ch, void *vo,int target){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;

  damage(ch, victim, number_range(5, 25), sn, DAM_INTERNAL, TRUE );

  if (is_affected(victim, sn )){
    act("The space-time around $N has already been rippled.", ch, NULL, victim, TO_CHAR);
    return;
  }
  else if (saves_spell(level, victim, DAM_OTHER, skill_table[sn].spell_type) ){
    send_to_char("You failed.\n\r", ch);
    send_to_char("Your vision ripples momentarly.\n\r", victim);
    return;
  }

  if (IS_AFFECTED(ch, AFF_SLOW) || IS_AFFECTED(ch, AFF_HASTE)){
    affect_strip(ch, skill_lookup("slow") );
    affect_strip(ch, skill_lookup("haste") );
    send_to_char("Some of your spells have been disrupted.\n\r", ch );
  }
  if (IS_AFFECTED(victim, AFF_SLOW)){
    int sn = skill_lookup("slow");
    act(skill_table[sn].msg_off, victim, NULL, NULL, TO_CHAR);
    act(skill_table[sn].msg_off2, victim, NULL, NULL, TO_ROOM);
    affect_strip(victim, sn );
  }
  if (IS_AFFECTED(victim, AFF_HASTE)){
    int sn = skill_lookup("haste");
    act(skill_table[sn].msg_off, victim, NULL, NULL, TO_CHAR);
    act(skill_table[sn].msg_off2, victim, NULL, NULL, TO_ROOM);
    affect_strip(victim, sn );
  }

  send_to_char("Your vision begins to ripple strangely.\n\r", victim);
  act("The space around $n begins to ripple strangely.", victim, NULL, NULL, TO_ROOM);

  af.type = sn;
  af.level = level;
  af.duration = number_range(3, 6);

  af.where = TO_AFFECTS;
  af.bitvector = 0;

  af.modifier = number_range(5, 10);//number of ripples before fading
  af.location = APPLY_NONE;
  affect_to_char(victim, &af);
}


/* HERALD POWERS */
void spell_recharge( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA* obj = (OBJ_DATA*) vo;
  int skill = get_skill(ch, sn );

  if (obj->item_type != ITEM_ARTIFACT
      && obj->item_type != ITEM_WAND
      && obj->item_type != ITEM_STAFF){
    send_to_char("You can only recharge wands, staffs and artifacts.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }
  else if (obj->item_type != ITEM_ARTIFACT && obj->value[1] < 2){
    send_to_char("This object cannot withstand another attempt.\n\r", ch );
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }
  else if (obj->item_type != ITEM_ARTIFACT && obj->value[2] >= obj->value[1]){
    send_to_char("This object is at full charge.\n\r", ch );
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }
  handle_skill_cost_percent( ch, ch->pCabal, sn, 100 );

  if (obj->item_type == ITEM_ARTIFACT){
    int gain = 25;

    if (number_percent() < skill){
      act("You manage to infuse $p with extra energy.", ch, obj, NULL, TO_CHAR);
      act("$n infuses $p with extra energy.", ch, obj, NULL, TO_ROOM);
      obj->condition = UMIN(100, obj->condition + gain );
      check_improve(ch, sn, TRUE, 10);
    }
    else{
      send_to_char("Your concentration slips severly damaging the artifact!\n\r", ch);
      act("$n's concentration slips causing plumes of smoke to rise from $p.", ch, obj, NULL, TO_ROOM);
      obj->condition -= 2 * gain;
      if (get_curr_cond( obj ) < 5){
	act("With a loud thunderclap $p explodes into fine ash!", ch, obj, NULL, TO_ALL);
	extract_obj( obj );
      }
      check_improve(ch, sn, FALSE, 50);
    }
  }
  else{
    obj->value[1] -= 1;
    if (number_percent() < skill){
      act("You manage to infuse $p with extra energy.", ch, obj, NULL, TO_CHAR);
      act("$n infuses $p with extra energy.", ch, obj, NULL, TO_ROOM);
      obj->value[2] = obj->value[1];
      check_improve(ch, sn, TRUE, 10);
    }
    else{
      send_to_char("Your concentration slips and you waste the charge.\n\r", ch);
      act("$n's concentration slips causing plumes of smoke to rise from $p.", ch, obj, NULL, TO_ROOM);
      check_improve(ch, sn, FALSE, 20);
    }
  }
}

void spell_overload( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  OBJ_DATA* obj = (OBJ_DATA*) vo;
  AFFECT_DATA af;
  int skill = get_skill(ch, sn );

  if (obj->item_type != ITEM_WAND
      && obj->item_type != ITEM_STAFF){
    send_to_char("You can only overload wands and staffs.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }
  else if (is_affected_obj(obj, sn )){
    send_to_char("This object cannot withstand another attempt.\n\r", ch );
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }
  handle_skill_cost_percent( ch, ch->pCabal, sn, 100 );

  if (number_percent() < skill){
    act("You manage to overload $p with extra energy.", ch, obj, NULL, TO_CHAR);
    act("$n overloads $p with extra energy.", ch, obj, NULL, TO_ROOM);
    obj->value[2] = obj->value[1] = 3 * obj->value[1] / 2;
    check_improve(ch, sn, TRUE, 10);

    af.type = sn;
    af.where = TO_NONE;
    af.level = level;
    af.duration = -1;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_obj( obj, &af);
  }
  else{
    send_to_char("Your concentration slips and you waste the charge.\n\r", ch);
    act("$n's concentration slips causing plumes of smoke to rise from $p.", ch, obj, NULL, TO_ROOM);
    check_improve(ch, sn, FALSE, 20);
  }
}



/* WATCHER POWERS */
void spell_elemental_call( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;
  int bit = 0;
  char* force, *pro;

  if (is_affected(victim, sn )){
    send_to_char("You are still protected by the elements.\n\r", victim);
    return;
  }
  if (victim->in_room->sector_type == SECT_AIR
      || victim->in_room->sector_type == SECT_MOUNTAIN
      || victim->in_room->sector_type == SECT_SNOW
      || victim->in_room->sector_type == SECT_DESERT){
    force = "air";
    pro = "cold";
    bit = RES_COLD;
  }
  else if (victim->in_room->sector_type == SECT_WATER_SWIM
	   || victim->in_room->sector_type == SECT_WATER_NOSWIM
	   || victim->in_room->sector_type == SECT_SWAMP){
    force = "water";
    pro = "fire";
    bit = RES_FIRE;
  }
  else{
    force = "earth";
    pro = "lightning";
    bit = RES_LIGHTNING;
  }

  act("You call on the forces of $t to your aid.", victim, force, NULL, TO_CHAR);
  act("$n calls on the forces of $t to $s aid.", victim, force, NULL, TO_ROOM);

  if (victim->in_room->sector_type == SECT_INSIDE
      || victim->in_room->sector_type == SECT_CITY
      || victim->in_room->sector_type == SECT_LAVA){
    act("Nothing happends.", victim, NULL, NULL, TO_ALL);
    return;
  }

  act("You have been granted protection from $t!", victim, pro, NULL, TO_CHAR);
  act("$n has been granted protection from $t!", victim, pro, NULL, TO_ROOM);

  af.type = sn;
  af.duration = 6 * (IS_NPC(victim) ? 3 : victim->pcdata->rank);
  af.level = level;

  af.where = TO_RESIST;
  af.bitvector = bit;
  af.location = APPLY_NONE;
  af.modifier = 0;
  affect_to_char( victim, &af );
}

void spell_webbing( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;
  int dam = number_range(10, 25);
  int mod = (get_skill(ch, sn) - 75) / 5;
  if (is_affected(victim, sn)){
    send_to_char("They're already covered in webs!\n\r", ch);
    return;
  }

  if ( saves_spell( level+mod, victim, DAM_AIR, skill_table[sn].spell_type) ){
    damage( ch, victim, 0, sn, DAM_AIR, TRUE );
    return;
  }
  act("Sticky webs shoot from your hands covering $N.", ch, NULL, victim, TO_CHAR);
  act("Sticky webs shoot from $n's hands covering $N.", ch, NULL, victim, TO_NOTVICT);
  act("Sticky webs shoot from $n's hands covering you.", ch, NULL, victim, TO_VICT);

  if (!IS_NPC(victim)
      && victim->pcdata->pStallion ){
    do_dismount(victim, "");
  }

  if (!is_affected(victim, gsn_thrash)){
    af.type	= gsn_thrash;
    af.level	= ch->level;
    af.duration	= 1;
    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location	= APPLY_NONE;
    af.modifier	= 0;
    affect_to_char(victim,&af);
    if (IS_AFFECTED(victim, AFF_FLYING)){
      act("You can't seem to get back into the air",victim,NULL,NULL,TO_CHAR);
      act("$n can't seem to get back into the air",victim,NULL,NULL,TO_ROOM);
    }
  }
  af.type	= sn;
  af.duration	= number_range(2, 4);
  af.level	= level;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_DEX;
  af.modifier	= -3;
  affect_to_char( victim, &af );

  damage( ch, victim, dam, sn, DAM_AIR, TRUE );
}

const struct awaken_type{
  char* name;
  char* race;
  char* attack;
  char* short_descr;
  char* long_descr;
  char* desc;
} awaken_table [] = {
  {	"typhon giant serpent",	"lizard",	"claw",
	"the Typhon",
	"A giant serpent with eyes of white hot flame.\n\r",
	"A giant dark green serpent towers above the area its eyes lit up\n\r"\
	"with an infernal blaze.  Its maw glitters with rows and rows of\nr"\
	"teeth and reeks of carrion and decay."
  },
  {	"wyvern",		"lizard",	"claw",
	"the Wyvern",
	"A giant wyvern drips poison from its mouth.\n\r",
	"A massive wyvern is before you, its serpentine body held in the\n\r"\
	"in the by a pair of leathery wings.  Its long neck bulges with\n\r"\
	"muscles as it swings back and forth seeking new prey.\n\r"
  },

  {	"basilisk lizard",	"lizard",	"gaze",
	"the Basilisk",
	"Its yellows eyes shining with eerie light the Basilisk is here.\n\r",
	"A huge lizard is before you bearing much in common with a giant\n\r"\
	"komodo dragon.  Its eyes are of pule golden and shine with eerie\n\r"\
	"magical light.\n\r"
  },
  {	"medusa",		"human",	"gaze",
	"the Medusa",
	"Crowned by a mass of snakes the Medusa stands before you.\n\r",
	"Bearing the looks of a tall female human it has the lower\n\r"\
	"body of a serpent, and a mass of writhing snakes for hair.\n\r"\
	"Her eyes shine with eerie golden light as she slithers onward.\n\r",
  },

  {	"craw wyrm giant",	"dragon",	"bite",
	"the Craw Wyrm",
	"Towering above the area the Craw Wyrm notices you.\n\r",
	"Easly dwarfing most dragons this giant serpent towers over you.\n\r"\
	"Tiny beaded eyes search for its next victim as its huge mouth\n\r"\
	"opens and closes.\n\r",
  },
  {	"karakken octopus",	"lizard",	"tentacle",
	"the Karakken",
	"An giant octopus is here, its tentacles thrashing in the air.\n\r",
	"An octopus easily the size of a small ship its 8 tentacles weave\n\r"\
	"a deadly tapestry in the air.  Each thick as a tree and lined\n\r"\
	"not only with the deadly suction cups, but with hooks capable\n\r"\
	"of shredding man to bits.\n\r",
  },

  {	"hydra",		"lizard",	"psbite",
	"the Hydra",
	"Its nine heads snarling with fury the Hydra is here.\n\r",
	"Nine heads armed with fangs and poisonous breath extend from\n\r"\
	"the giant lizard body and tower above you.  Puddles of poison\n\r"\
	"smoke on the ground before it as it moves its huge bulk forward.\n\r",
  },
  {	"green dragon",		"dragon",	"psbite",
	"the Green Dragon",
	"Its emerald scales shining, the light the Green Dragon is here.\n\r",
	"This enormous winged serpent has dark green scales covering most\n\r"\
	"of its colossal body.  Fumes of poisonous gas issue forth from\n\r"\
	"its maw.\n\r",
  },

  {	"blue dragon",		"dragon",	"frbite",
	"the Blue Dragon",
	"Its pale blue scales shimmering, the Blue Dragon is here.\n\r",
	"Possessing a slimmer body then rest of its cousins does not\n\r"\
	"rob this colossal beast of its devestating nature. The ground\n\r"\
	"before it is covered in frost, and each breath it takes results\n\r"\
	"in clouds of mist and frost forming around it.\n\r",
  },
  {	"black dragon",		"dragon",	"kiss",
	"the Black Dragon",
	"The Black Dragon towers over the area in its deadly glory.\n\r",
	"Before you, one of the largest, and meanest specimens of dragons\n\r"\
	"slithers onwards. A dragon of which legends are made its massive,\n\r"\
	"sinewy black body is covered with armored scales, and his claws\n\r"\
	"look fit to shave with.\n\r",
  }
};

void spell_awaken_life( int sn, int level, CHAR_DATA *ch, void *vo, int target ){
  CHAR_DATA* mob, *vch;
  ROOM_INDEX_DATA* to;
  AFFECT_DATA af;
  EXIT_DATA* pExit;
  char buf[MIL], buf1[MIL];
  const int dur = 180;
  int type = 0;
  int rank = 0;

  if (IS_NPC(ch))
    return;
  else
    rank = ch->pcdata->rank;

  if (get_summoned(ch, MOB_VNUM_MONSTER) > 0 || ch->pcdata->familiar){
    send_to_char("You may only awaken one servant at a time.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }
  else if (is_affected( ch, sn)){
    send_to_char("You're not yet ready to awaken a new creature.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }
  else if ( (mob = create_mobile( get_mob_index( MOB_VNUM_MONSTER ))) == NULL){
    send_to_char("You failed.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }

  //get type of the creature
  type = UMAX(0, rank - 1) * 2 + number_bits( 1 );

  //set mob strings
  free_string( mob->name );
  sprintf(buf, "watchermonster %s", awaken_table[type].name);
  mob->name = str_dup( buf );

  free_string( mob->short_descr);
  mob->short_descr = str_dup(awaken_table[type].short_descr);

  free_string( mob->long_descr );
  mob->long_descr = str_dup(awaken_table[type].long_descr);

  free_string( mob->description );
  mob->description = str_dup(awaken_table[type].desc);

  //other stats
  mob->dam_type = UMAX(0, attack_lookup(awaken_table[type].attack));
  mob->race = UMAX(0, race_lookup(awaken_table[type].race ));
  mob->level = 50 + rank * 2;
  mob->pCabal = get_parent(ch->pCabal);

  mob->max_hit += rank * mob->max_hit / RANK_LEADER;
  mob->hit	= mob->max_hit;
  mob->damroll += rank * 5;
  mob->hitroll += rank * 5;
  mob->act     |= ACT_RAIDER;


  af.type	= sn;
  af.level	= level;
  af.duration	= dur;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( ch, &af );

  af.type	= gsn_timer;
  af.duration	= af.duration / 3;
  affect_to_char( mob, &af);

  mob->summoner = ch;
  ch->pcdata->familiar = mob;


  /* we summon the creature to this room if non of the connecting areas are a city or cabal */
  for (pExit = ch->in_room->area->exits; pExit; pExit = pExit->next_in_area ){
    //skip on blind exit, or exit to same area
    if (pExit->to_room == NULL || pExit->to_room->area == ch->in_room->area)
      continue;
    else if (IS_AREA(pExit->to_room->area, AREA_CABAL))
      break;
    else if (IS_AREA(pExit->to_room->area, AREA_CITY))
      break;
  }

  /* if found a city or cabal attached to this area, summon to anchor */
  if (pExit == NULL && !IS_AREA(ch->in_room->area, AREA_CITY) && !IS_AREA(ch->in_room->area, AREA_CABAL))
    to = ch->in_room;
  else if (ch->pCabal && get_parent(ch->pCabal)->anchor)
    to = get_parent(ch->pCabal)->anchor;
  else
    to = ch->in_room;

  mob->trust	= 6969;
  char_to_room( mob, to );
  mob->trust	= mob->level;

  act("$n summons a champion of Nature to do $s bidding!", ch, NULL, NULL, TO_ROOM);
  act("You summon a champion of Nature to do your bidding!", ch, NULL, NULL, TO_ROOM);
  sprintf( buf, "The ground shaking underneath it %s enters the area.", mob->short_descr);
  do_zecho( mob, buf);

  sprintf(buf1, "%s", capitalize( mob->short_descr));
  sprintf( buf, "A messenger yells '`6%s has been sighted near %s!``'\n\r",
	   buf1,
	   to->area->name);

  for (vch = player_list; vch; vch = vch->next_player){
    if (!IS_AWAKE( vch))
      continue;
    else if (vch->in_room->area != to->area)
      send_to_char( buf, vch );
  }

}

//gives some bonuses and prot good/evil
void spell_greenheart( int sn, int level, CHAR_DATA *ch, void *vo,int target){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn)){
    if (ch == victim)
      send_to_char("The power of nature already courses through your veins.\n\r", ch);
    else
      send_to_char("The power of nature already courses through their veins.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }

  af.type	= sn;
  af.level	= level;
  af.duration	= number_fuzzy(level / 4);
  af.where	= TO_AFFECTS;

  af.bitvector	= AFF_PROTECT_GOOD;
  af.location	= APPLY_SAVING_SPELL;
  af.modifier	= -number_fuzzy(level / 10);
  affect_to_char( victim, &af);

  af.bitvector	= AFF_PROTECT_EVIL;
  af.location	= APPLY_HIT;
  af.modifier	= 4 * level / 5;
  affect_to_char( victim, &af);

  act("$n's skin takes on a green tint.", ch, NULL, NULL, TO_ROOM);
  send_to_char("Your skin becomes slightly tinted with green.\n\r", ch);
}

/* causes unsavable tele-lock and:
UNDEAD = Hit/Move/Mana regen penalty (-25%)
DEMON  = Cannot corrupt, -3 Str
AVATAR = Cannot call, -3 Dex
*/
void spell_planar_seal( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, gsn_tele_lock)){
    if (ch == victim)
      send_to_char("You're already unable to leave this plane.\n\r", ch);
    else
      send_to_char("They are already unable to leave this plane.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }
  else if (is_affected(ch, sn)){
    send_to_char("You are unable to form the Planar Seal.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }

  af.type	= gsn_tele_lock;
  af.level	= level;
  af.duration	= number_fuzzy( level / 4 );

  af.where     = TO_AFFECTS;
  af.bitvector = 0;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  affect_to_char( victim, &af );

  af.type      = sn;

  if (IS_UNDEAD(victim)){
    af.modifier	= -25;
    af.location	= APPLY_HIT_GAIN;
    affect_to_char( victim, &af );
    af.location	= APPLY_MANA_GAIN;
    affect_to_char( victim, &af );
    af.location	= APPLY_MOVE_GAIN;
    affect_to_char( victim, &af );
  }
  else if (IS_DEMON(victim)){
    af.modifier = -3;
    af.location	= APPLY_STR;
    affect_to_char( victim, &af );
  }
  else if (IS_AVATAR(victim)){
    af.modifier = -3;
    af.location	= APPLY_DEX;
    affect_to_char( victim, &af );
  }

  act("The Planar Seal forms around $n.", victim, NULL, NULL, TO_ROOM);
  send_to_char("You have been barred from all travel and contact out of this plane.\n\r", victim);

  /* wait timer */
  af.type	= sn;
  af.where	= TO_AFFECTS;
  af.duration	= 24;
  af.location	= APPLY_NONE;
  af.modifier	= 0;
  affect_to_char( ch, &af);
}


/* caues the user to have breath attack in combat (gen) */
void spell_dragon_blood( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  AFFECT_DATA af;
  char* color;
  int element = number_range( 0, 3);

  if (is_affected(ch, gen_dragon_blood)){
    send_to_char("Dragon blood still courses through your veins.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }

  af.type	= gen_dragon_blood;
  af.level	= level;
  af.duration	= number_fuzzy( level / 8 );
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;

  switch( element ){
  default:
  case 0:	color = "crimson";	af.modifier = DAM_FIRE;	break;
  case 1:	color = "blue";		af.modifier = DAM_COLD;	break;
  case 2:	color = "white";	af.modifier = DAM_LIGHTNING;	break;
  case 3:	color = "green";	af.modifier = DAM_ACID;	break;
  }
  affect_to_char( ch, &af);

  af.type	= sn;
  af.location	= APPLY_AC;
  af.modifier	= -level;
  affect_to_char(ch, &af);

  send_to_char("You quaff a vial of dragon's blood.\n\r", ch);
  act("$n quaffs a vial of dragon's blood.", ch, NULL, NULL, TO_ROOM);

  sendf( ch, "A %s taint floods your vision.\n\r", color );
  act("$n's eyes turn bright $t!", ch, color, NULL, TO_ROOM);
}


void spell_whirlwind( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  CHAR_DATA* victim = (CHAR_DATA*) vo;
  CHAR_DATA* vch, *vch_next;
  ROOM_INDEX_DATA* room;

  int area_pool = 10;
  int e_r1[] = {ROOM_NO_TELEPORTIN, ROOM_NO_GATEIN, ROOM_NO_RECALL, ROOM_NO_INOUT};
  int r_s1[] = {SECT_FOREST, SECT_FIELD, SECT_HILLS, SECT_SNOW, SECT_MOUNTAIN};

  if (ch == victim || IS_NPC(victim)){
    send_to_char("You failed.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }
  else if (victim->fighting){
    send_to_char("They are moving about too much for you to focus the spell.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }
  else if ( saves_spell( level, victim, DAM_AIR, skill_table[sn].spell_type) ){
    act("A few strong gusts of wind whip through the area.\n\r", ch, NULL, NULL, TO_ALL);
    damage( ch, victim, 0, sn, DAM_AIR, TRUE );
    return;
  }
  else if (ch->in_room->sector_type == SECT_FOREST
	   || ch->in_room->sector_type == SECT_FIELD
	   || ch->in_room->sector_type == SECT_HILLS
	   || ch->in_room->sector_type == SECT_SNOW
	   || ch->in_room->sector_type == SECT_MOUNTAIN){
    send_to_char("You may not call a whirlwind here least you damage the fauna around you.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }

  /* try to get a non city/inside room */
  room =  get_rand_room(0,0,	//area range (0 to ignore)
			0,0,	//room ramge (0 to ignore)
			NULL,0,//areas to choose from
			NULL,0,//areas to exclude
			r_s1,5,//sectors required
			NULL,0,//sectors to exlude
			NULL,0,//room1 flags required
			e_r1,4,//room1 flags to exclude
			NULL,0,//room2 flags required
			NULL,0,//room2 flags to exclude
			area_pool,//number of seed areas
			TRUE,//exit required?
			FALSE,//Safe?
			ch);  //Character for room checks
  if (room == NULL){
    send_to_char("You failed.\n\r", ch);
    handle_skill_cost_percent( ch, ch->pCabal, sn, -100 );
    return;
  }

  act("A massive whirlwind forms around $N and carries you off!", ch, NULL, victim, TO_CHAR);
  act("A massive whirlwind forms around $N and carries you off!", victim, NULL, ch, TO_CHAR);
  act("A massive whirlwind forms around %n and $N and carries them off!", ch, NULL, victim, TO_NOTVICT);

  aecho(ch->in_room->area, "Howling mightly a massive whirwlind forms in the area.\n\r" );
  aecho(room->area, "Howling mightly a massive whirwlind tears through the area.\n\r" );

/* now move user and his charmies */
  for (vch = ch->in_room->people; vch; vch = vch_next){
    vch_next = vch->next_in_room;
    if (IS_AFFECTED(vch, AFF_CHARM)
	&& vch->master
	&& vch->master == ch){
      char_from_room( vch );
      char_to_room( vch, room );
    }
  }

  char_from_room( ch );
  char_to_room( ch, room );
  if (ch && ch->in_room && IS_AWAKE(ch))
    do_look( ch, "auto" );

  char_from_room( victim );
  char_to_room( victim, room );
  if (victim && victim->in_room && IS_AWAKE(victim)){
    do_look( victim, "auto" );
    if (ch
	&& ch->in_room
	&& victim->fighting
	&& victim->fighting != ch){
      stop_fighting(victim, TRUE);
      set_fighting(victim, ch);
    }
  }

  if (ch && ch->in_room)
    damage( ch, ch, number_range(30, 130), sn, DAM_AIR, TRUE );
  if (victim && victim->in_room)
    damage( ch, victim, number_range(30, 130), sn, DAM_AIR, TRUE );
}

//causes person to be healed on vtick
void spell_regenerate( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  AFFECT_DATA af;

  if (is_affected(ch, gen_regenerate )){
    send_to_char("You're already regenerating.\n\r", ch);
    return;
  }
  else if (is_affected(ch, sn)){
    send_to_char("You're not ready to cast this spell again.\n\r", ch);
    return;
  }

  send_to_char("You sense your metabolism go haywire!\n\r", ch);

  af.type	= gen_regenerate;
  af.level	= level;
  af.duration	= 3;
  af.where	= TO_AFFECTS;
  af.bitvector	= 0;
  af.location	= APPLY_NONE;
  //times to cast cure light
  af.modifier	= skill_table[sn].min_mana / skill_table[gsn_cure_light].min_mana;
  affect_to_char(ch, &af );

  af.type	= sn;
  af.duration	= 12;
  af.modifier	= 0;
  affect_to_char(ch, &af );
}

void spell_forestmeld( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  AFFECT_DATA af;

  if (is_affected(ch, sn)){
    send_to_char("You're not ready to cast this spell again.\n\r", ch);
    return;
  }

  send_to_char("You begin to blend in with the undergrowth.\n\r", ch);
  act("$n's skin takes on a strange green glow.", ch, NULL, NULL, TO_ROOM );

  af.where     = TO_AFFECTS2;
  af.type      = sn;
  af.level     = level;
  af.duration  = 12;
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_BLUR;
  affect_to_char( ch, &af );
}

void spell_eye_of_nature( int sn, int level, CHAR_DATA *ch, void *vo,int target ){
  AFFECT_DATA* paf, af;

  if ( (paf = affect_find(ch->affected, sn)) == NULL){
    af.type	= sn;
    af.duration = 24;
    af.level	= level;
    af.where	= TO_AFFECTS;
    af.bitvector= 0;
    af.location = APPLY_NONE;
    af.modifier = ch->in_room->area->vnum;
    affect_to_char(ch, &af );
    send_to_char("You focus the eye of nature on this area.\n\r", ch);
  }
  else{
    AREA_DATA* pa = get_area_data( paf->modifier );

    if (pa == NULL)
      send_to_char("You failed.\n\r", ch);
    else{
      sendf(ch, "You gaze through the eye of nature at %s\n\r", pa->name );
      where( ch, pa, target_name );
    }
  }
}
