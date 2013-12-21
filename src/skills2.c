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
#include "jail.h"


void do_rake( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MIL];
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    AFFECT_DATA af, *paf;
    bool PRIMARY = TRUE;

    const int lag = skill_table[gsn_rake].beats;
    const int cost = skill_table[gsn_rake].min_mana;

    int chance, hth = 0, ch_weapon, vict_weapon, ch_vict_weapon;
    if ((chance = get_skill(ch,gsn_rake)) == 0)
    {
        send_to_char( "You don't know how to deflect weapons.\n\r", ch );
	return;
    }
    argument = one_argument(argument,arg);
    if (arg[0] |= '\0')
    {
	if (!strcasecmp(arg,"primary"))
	    PRIMARY = TRUE;
	else if (!strcasecmp(arg,"secondary"))
	    PRIMARY = FALSE;
	else
	{
	    send_to_char("Deflect primary or secondary?\n\r",ch);
	    return;
	}
    }
    if ( (obj = get_eq_char( ch, WEAR_WIELD )) == NULL
	 && (obj = get_eq_char( ch, WEAR_SECONDARY )) == NULL
	 && ((hth = get_skill(ch,gsn_hand_to_hand)) == 0
	     || (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_DISARM))))
      {
        send_to_char( "You must wield a weapon to deflect.\n\r", ch );
	return;
    }
    if (has_twohanded(ch) == NULL){
      send_to_char("You must hold your weapon with both hands.\n\r", ch);
      return;
    }
    if ( ( victim = ch->fighting ) == NULL )
    {
	send_to_char( "You aren't fighting anyone.\n\r", ch );
	return;
    }
    if ( (paf = affect_find(victim->affected,gsn_rake)) != NULL )
    {
	if (PRIMARY && paf->modifier == 1)
	{
	    send_to_char("Their primary weapon has already been deflected.\n\r",ch);
	    return;
	}
	else if (!PRIMARY && paf->modifier == 0)
	{
	    send_to_char("Their secondary weapon is already been deflected.\n\r",ch);
	    return;
	}
	else if (paf->modifier > 1)
	{
	    send_to_char("Error.\n\r",ch);
	    return;
	}
    }
    if ( PRIMARY && ( obj2 = get_eq_char( victim, WEAR_WIELD ) ) == NULL
	 && !IS_NPC(victim))
    {
	sendf(ch, "%s is not wielding a primary weapon.\n\r", PERS(victim,ch) );
	return;
    }
    if (!PRIMARY && ( obj2 = get_eq_char( victim, WEAR_SECONDARY ) ) == NULL
	&& !IS_NPC(victim))
    {
	sendf(ch, "%s is not wielding a secondary weapon.\n\r", PERS(victim,ch) );
	return;
    }
    if (ch->mana < cost){
      send_to_char("You lack the strength of mind to execute the manouver.\n\r", ch);
      return;
    }
    else ch->mana -=  cost;
    WAIT_STATE2(ch, lag);

    ch_weapon = get_weapon_skill_obj(ch, obj);
    if (obj2){
      vict_weapon = get_weapon_skill_obj(victim, obj2);
      ch_vict_weapon = get_weapon_skill_obj(ch, obj2);
    }else{
      vict_weapon = get_weapon_skill(victim, gsn_hand_to_hand, FALSE);
      ch_vict_weapon = get_weapon_skill(ch, gsn_hand_to_hand, FALSE);
    }


    if ( get_eq_char(ch,WEAR_WIELD) == NULL)
      chance = chance * hth/150;
    else
      chance = chance * ch_weapon/100;
    chance += (ch_vict_weapon - vict_weapon) / 2;
    chance += 2*(get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_STR));
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += (ch->level - victim->level);
    chance -= get_skill(victim,gsn_parry)/6;
    chance += affect_by_size(ch,victim);
    chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    if (!PRIMARY)
	chance += 15;

    if (is_affected(victim,gsn_double_grip))
        chance = 2 * chance /3;
    if (IS_NPC(victim))
        chance -= victim->level/4;
    if ( number_percent() < chance)
    {
      act("With a mighty blow you deflect $N's weapon aside!",ch,NULL,victim,TO_CHAR);
      act("With a mighty blow $n deflects your weapon aside!",ch,NULL,victim,TO_VICT);
      act("With a mighty blow $n deflects $N's weapon aside!",ch,NULL,victim,TO_NOTVICT);
      af.where		= TO_AFFECTS;
      af.type			= gsn_rake;
      af.level		= ch->level;
      af.duration		= 1;
      af.location		= APPLY_NONE;
      af.modifier		= PRIMARY;
      af.bitvector		= 0;
      affect_to_char(victim,&af);
      check_improve(ch,gsn_rake,TRUE,1);
      /* check for auto apierce */
      if (number_percent() < get_skill(ch, gsn_apierce)){
	affect_strip(ch, gen_apierce);
	af.type = gen_apierce;
	af.duration = 0;
	af.level = ch->level;
	af.where = TO_AFFECTS;
	af.bitvector = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	affect_to_char(ch, &af);
	check_improve(ch,gsn_apierce,TRUE,1);
      }
    }
    else
      {
        act("You miss $N's weapon.",ch,NULL,victim,TO_CHAR);
        act("$n tries to deflect your weapon, but misses.",ch,NULL,victim,TO_VICT);
        act("$n tries to deflect $N's weapon, but misses.",ch,NULL,victim,TO_NOTVICT);
        check_improve(ch,gsn_rake,FALSE,1);
      }
}

void do_blackjack( CHAR_DATA *ch, char *argument)
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int chance, bj_duration;
    one_argument(argument,arg);
    if ( (chance = get_skill(ch,gsn_blackjack)) == 0 )
    {
	send_to_char("You don't know how to blackjack.\n\r",ch);
	return;
    }
    if (arg[0] == '\0')
    {
	send_to_char("Blackjack who?\n\r",ch);
	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("You can't do that while fighting.\n\r",ch);
	return;
    }
    if ((victim = get_char_room(ch, NULL,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }
    if (victim->fighting != NULL)
    {
        sendf(ch, "%s won't hold still long enough.\n\r", PERS(victim,ch));
	return;
    }
    if (IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG) )
    {
        sendf(ch, "%s is too alert for you to attempt such an act.\n\r", PERS(victim,ch));
	return;
    }
    if (victim == ch)
	return;
    if (is_safe(ch,victim))
	return;
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
        return;
    }
    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
    {
	send_to_char("You can't seem to get up enough speed.\n\r",ch);
	return;
    }
    if (is_affected(victim,gsn_blackjack))
    {
	act("$E's already dazed from the blow.",ch,NULL,victim,TO_CHAR);
	return;
    }
    chance = get_skill(ch, gsn_mace) * chance / 100;
    if (ch->race == race_lookup("halfling"))
      chance += 10;
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += URANGE(-10, (ch->level - victim->level), 10) * 5;
    chance += GET_AC2(victim,AC_BASH)/25;
    chance += 2*(get_curr_stat(ch,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK));
    chance += affect_by_size(ch,victim) * 3;

    if (!IS_NPC(victim) && victim->pcdata->pStallion != NULL && get_skill(victim, gsn_mounted) > 2){
      chance -= 12;
    }
    if (is_affected(victim, gen_watchtower)){
      sendf( ch, "%s is shielded within the Watchtower.\n\r", PERS(victim, ch ));
      chance = 0;
    }
    if (victim->level > ch->level + 10)
      chance = 0;

    WAIT_STATE2(ch,skill_table[gsn_blackjack].beats);

    bj_duration = number_range(1, 2);
    af.type		= gsn_blackjack;
    af.level		= ch->level;
    af.duration		= bj_duration + 2;
    af.location		= APPLY_DEX;
    af.modifier		= -2;
    af.bitvector	= 0;
    affect_to_char(victim,&af);

    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "blackjack", skill_table[gsn_blackjack].name))
      return;

//MANTIS CHECK
    if ( (is_affected(victim,gsn_mantis) || get_skill(victim, gsn_shadowmaster) > 1) && IS_AWAKE(victim)){
      act("You sense $n's movement and throw $m to the ground!",ch,NULL,victim,TO_VICT);
      act("$N senses your attack, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
      act("$N senses $n's attack, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
      WAIT_STATE2(ch, URANGE(2, 1 + number_range(1, victim->size), 4) * PULSE_VIOLENCE);
      do_visible(ch, "");

      sprintf(buf, "Help!  %s just tried to blackjack me!",PERS(ch,victim));
      j_yell(victim,buf);
      affect_strip(victim,gsn_mantis);
      damage(victim,ch,10,gsn_mantis,DAM_BASH,TRUE);
      return;
    }//END MATIS


    act("You hit $N on the back of the head with a lead filled sack.",ch,NULL,victim,TO_CHAR);
    act("$n hits $N on the back of the head with a lead filled sack.",ch,NULL,victim,TO_NOTVICT);
    if ((number_percent( ) < chance || 	 (IS_IMMORTAL(ch) && ch->wimpy == 69))
	&& !IS_IMMORTAL(victim) && !IS_UNDEAD(victim)){
      af.where		= TO_AFFECTS;
      af.location		= APPLY_NONE;
      af.modifier		= 0;
      af.duration		= bj_duration;
      af.bitvector	= AFF_SLEEP;
      affect_to_char(victim,&af);
      if (IS_AWAKE(victim)){
	send_to_char("A sudden pain erupts in your skull.\n\r",victim);
	do_sleep(victim,"");
      }
      set_delay(ch, victim);
      check_improve(ch,gsn_blackjack,TRUE,1);
    }
    else{
      act("$n hits you on the back of the head with a lead filled sack.",ch,NULL,victim,TO_VICT);
      do_visible(ch, "");
      sprintf(buf, "Help!  %s just tried to blackjack me!",PERS(ch,victim));
      j_yell(victim,buf);
      damage(ch,victim,number_range(10, 20),gsn_blackjack,DAM_BASH,TRUE);
      check_improve(ch,gsn_blackjack,FALSE,1);
    }
}

void do_strangle( CHAR_DATA *ch, char *argument)
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int chance, str_duration;
    int skill = 4 * get_skill(ch,gsn_strangle) / 5;
    one_argument(argument,arg);

    if (skill < 1)
    {
	send_to_char("You don't know how to strangle.\n\r",ch);
	return;
    }

    if (arg[0] == '\0')
    {
	send_to_char("Strangle who?\n\r",ch);
	return;
    }

    if (ch->fighting != NULL)
    {
	send_to_char("You can't do that while fighting.\n\r",ch);
	return;
    }

    else if ((victim = get_char_room(ch, NULL,arg)) == NULL)
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }

    if (victim->fighting != NULL)
    {

        sendf(ch, "%s won't hold still long enough.\n\r", PERS(victim,ch));
	return;
    }

    if (IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG) )
    {
        sendf(ch, "%s is too alert for you to attempt such an act.\n\r", PERS(victim,ch));
	return;
    }

    if (victim == ch){
      send_to_char("Why not buy a noose while you are at it?\n\r", ch);
      return;
    }
    if (is_safe(ch,victim))
	return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
        return;
    }
    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
    {
	send_to_char("You can't seem to get up enough speed.\n\r",ch);
	return;
    }
    if (is_affected(victim,gsn_strangle))
    {
	act("$E's too cautious right now for you to get close enough to strangle.",ch,NULL,victim,TO_CHAR);
	return;
    }
    chance = skill * (get_skill(ch, gsn_hand_to_hand) - 50) / 50;
    if (get_eq_char(ch, WEAR_WIELD) == NULL)
      chance += 5;
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += URANGE(-10, (ch->level - victim->level), 10);
    chance -= get_skill(victim,gsn_dodge)/25;
    chance -= get_skill(victim,gsn_counter)/25;
    chance +=  2 * (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK));
    chance += affect_by_size(ch,victim) * 3;

    if (!IS_NPC(victim) && victim->pcdata->pStallion != NULL)
	chance -= 12;

    if (is_affected(victim, gen_watchtower)){
      sendf( ch, "%s is shielded within the Watchtower.\n\r", PERS(victim, ch ));
      chance = 0;
    }
    else if(!IS_AWAKE(victim))
      chance = 100;

    WAIT_STATE2(ch,skill_table[gsn_strangle].beats);

    a_yell(ch,victim);
    //predict interdict
    if (predictCheck(ch, victim, "strangle", skill_table[gsn_strangle].name))
      return;

    str_duration = number_range(1, 2);
    af.type		= gsn_strangle;
    af.level		= ch->level;
    af.duration		= str_duration + 2;
    af.location		= APPLY_NONE;
    af.modifier		= 0;
    af.bitvector	= 0;
    affect_to_char(victim,&af);
    chance = URANGE(5, chance, 95);

//MANTIS CHECK
    if ( (is_affected(victim,gsn_mantis) || get_skill(victim, gsn_shadowmaster) > 1) && IS_AWAKE(victim)){
      act("You sense $n's movement and throw $m to the ground!",ch,NULL,victim,TO_VICT);
      act("$N senses your attack, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
      act("$N senses $n's attack, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
      WAIT_STATE2(ch, URANGE(2, 1 + number_range(1, victim->size), 4) * PULSE_VIOLENCE);
      do_visible(ch, "");

      sprintf(buf, "Help!  %s just tried to strangle me!",PERS(ch,victim));
      j_yell(victim,buf);
      affect_strip(victim,gsn_mantis);
      damage(victim,ch,10,gsn_mantis,DAM_BASH,TRUE);
      return;
    }//END MATIS

    if ( (number_percent( ) < chance || (IS_IMMORTAL(ch) && ch->wimpy == 69) ) && !IS_IMMORTAL(victim) && !IS_UNDEAD(victim))
      {
        act("$n grabs a hold of your neck and puts you to sleep.",ch,NULL,victim,TO_VICT);
	act("You grab a hold of $N's neck and put $M to sleep.",ch,NULL,victim,TO_CHAR);
	act("$n grabs a hold of $N's neck and puts $M to sleep.",ch,NULL,victim,TO_NOTVICT);
	af.where		= TO_AFFECTS;
	af.duration		= str_duration;
	af.bitvector	= AFF_SLEEP;
	affect_to_char(victim,&af);
	do_sleep(victim,"");
	set_delay(ch, victim);
	check_improve(ch,gsn_strangle,TRUE,1);
    }
    else
    {
	sprintf(buf, "Help!  %s just tried to strangle me!",PERS(ch,victim));
	j_yell(victim,buf);
	damage(ch,victim,0,gsn_strangle,DAM_NONE,TRUE);
	check_improve(ch,gsn_strangle,FALSE,1);
    }
}

void do_acupuncture( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    int level = ch->level - 5;
    char arg[MIL];
    sh_int chance;
    AFFECT_DATA af;
    one_argument (argument,arg);
    if ((chance = get_skill(ch,gsn_acupuncture)) == 0)
    {
	send_to_char("You don't know the secrets of acupuncture.\n\r",ch);
	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("You can't do that while fighting.\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_acupuncture))
    {
	send_to_char("You can't use that art again so soon.\n\r",ch);
	return;
    }
    victim = get_char_room(ch, NULL,arg);
    if (arg[0] == '\0')
	victim = ch;
    if (victim != ch)
    {
	send_to_char("You can't use your talents to aid others.\n\r",ch);
	return;
    }
    if (ch->mana < 70)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_BLIND) && !is_affected(ch, gsn_battletrance))
    {
	send_to_char("Sticking needles in yourself while you're blind isn't such a great idea.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_acupuncture].beats);
    af.type		= gsn_acupuncture;
    af.level		= ch->level;
    af.duration		= 6;
    af.location		= APPLY_DEX;
    af.modifier		= -2;
    af.bitvector	= 0;
    affect_to_char(ch,&af);
    if (number_percent( ) > chance)
    {
	ch->mana -= 35;
	af.location	= APPLY_HITROLL;
	af.modifier	= -4;
	affect_to_char(ch,&af);
	af.location	= APPLY_DAMROLL;
	af.modifier	= -4;
	affect_to_char(ch,&af);
	act("You apply pressure to the wrong spots and fail.",ch,NULL,NULL,TO_CHAR);
	act("$n applies pressure to $s wounds, but doesn't seem to look any better.",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_acupuncture,FALSE,1);
    }
    else
    {
	ch->mana -= 70;
	ch->hit = UMIN(ch->max_hit, ch->hit + (ch->level *5));
	update_pos(ch);
	act("You feel relieved as the tension drains from your muscles.",ch,NULL,NULL,TO_CHAR);
	if (IS_AFFECTED(ch,AFF_POISON)
	    && check_dispel(level,victim,gsn_poison))
	{
	    act("You feel the toxin leave your blood.",ch,NULL,NULL,TO_CHAR);
	    act("$n looks much better.",victim,NULL,NULL,TO_ROOM);
	}
	if (IS_AFFECTED(ch, AFF_PLAGUE)
	    && check_dispel(level,victim,gsn_plague)){
	  act("You feel the disease subside.",ch,NULL,NULL,TO_CHAR);
	  act("$n looks relieved as $s sores vanish.",victim,NULL,NULL,TO_ROOM);
	}
	act("$n applies pressure to $s wounds and looks much more relieved.",ch,NULL,NULL,TO_ROOM);
	check_improve(ch,gsn_acupuncture,TRUE,1);
    }
}

void do_herb( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MIL];
    sh_int chance, heal;
    AFFECT_DATA af;
    one_argument (argument,arg);

    const int gsn_group_herb = skill_lookup("group herb");
    int gherb = get_skill(ch, gsn_group_herb );

    if ((chance = get_skill(ch,gsn_herb)) == 0)
    {
	send_to_char("You don't know where to look for herbs.\n\r",ch);
	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("You can't do that while fighting.\n\r",ch);
	return;
    }
    if (IS_WEREBEAST(ch->race))
	chance += 10;
    switch(ch->in_room->sector_type)
    {
	case(SECT_INSIDE):	chance = 0;	break;
	case(SECT_CITY):	chance = 0;	break;
	case(SECT_FIELD):	chance -= 25;	break;
	case(SECT_FOREST):	chance += 10;	break;
	case(SECT_HILLS):	chance -= 10;	break;
	case(SECT_MOUNTAIN):	chance -= 40;	break;
	case(SECT_WATER_SWIM):	chance = 0;	break;
	case(SECT_WATER_NOSWIM):chance = 0;	break;
	case(SECT_AIR):		chance = 0;	break;
	case(SECT_DESERT):	chance = 0;	break;
    }
    if (chance == 0)
    {
	send_to_char("There aren't any herbs here to gather.\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_herb))
    {
	send_to_char("You have to let the herbs replenish first.\n\r",ch);
	return;
    }
    victim = get_char_room(ch, NULL,arg);
    if (arg[0] == '\0')
	victim = ch;
    if ( victim == NULL )
    {
	send_to_char("They aren't here.\n\r",ch);
	return;
    }
    if (ch->mana < 50)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch,AFF_BLIND))
    {
	send_to_char("You can't see what you're picking.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_herb].beats);
    act("You search the area for some herbs.",ch,NULL,NULL,TO_CHAR);
    act("$n searches the area for some herbs.",ch,NULL,NULL,TO_ROOM);

    if (number_percent( ) < chance){
      bool fGroup = FALSE;
      CHAR_DATA* vch;

      af.type		= gsn_herb;
      af.level	= ch->level;
      af.duration	= 9;
      af.location	= APPLY_NONE;
      af.modifier	= 0;
      af.bitvector	= 0;
      affect_to_char(ch,&af);
      ch->mana -= 50;

      if (number_percent() < gherb){
	check_improve(ch, gsn_group_herb, TRUE, 1);
	fGroup = TRUE;
      }
      else
	check_improve(ch, gsn_group_herb, FALSE, 5);


      for (vch = victim->in_room->people; vch; vch = vch->next_in_room){
	if (!fGroup && vch != victim)
	  continue;
	else if (fGroup && !is_same_group( victim, vch))
	  continue;
	else if (IS_UNDEAD(vch))
	  continue;

	heal = vch->hit + (ch->level * 5);
    	if (IS_WEREBEAST(ch->race))
	    heal += ch->level;

        vch->hit = UMIN(vch->max_hit,heal);
        update_pos(vch);

        act_new("You feel much better.",vch,NULL,NULL,TO_CHAR,POS_DEAD);
        act("$n looks much better.",vch,NULL,NULL,TO_ROOM);

        if (IS_AFFECTED(vch,AFF_PLAGUE)){
	    affect_strip(vch,gsn_plague);
	    act_new("You feel more relieved as your sores vanish.",vch, NULL,NULL,TO_CHAR,POS_DEAD);
	    act("$n looks more relieved as $s sores vanish.",vch, NULL,NULL,TO_ROOM);
	}
      }
      check_improve(ch,gsn_herb,TRUE,1);
    }
    else
    {
        ch->mana -= 25;
        send_to_char("You could not find any suitable herbs.\n\r",ch);
        check_improve(ch,gsn_herb,FALSE,1);
    }
}

void do_beast_call( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *mob, *vch;
    AFFECT_DATA af;
    int chance, beastroll, i = 0;
    bool have_mammoth = FALSE, have_leopard = FALSE, have_displacer = FALSE, have_wyvern = FALSE;
    bool fAdv = get_skill(ch, skill_lookup("beastmaster")) > 1;
    bool fHorde = get_skill(ch, skill_lookup("horde")) > 1;
    int beast = 0;
    int max = 3, cur;

    const int gsn_horn = skill_lookup("horn of nature");
    bool fHorn = FALSE;

    if ((chance = get_skill(ch, gsn_beast_call)) == 0)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (!can_follow(ch, ch))
      return;
    if (ch->mana < 80)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    if ( (cur = get_charmed_by(ch)) >= max){
      send_to_char("Are you sure you need a whole army at your command?\n\r", ch);
      return;
    }

    if (!IS_NULLSTR(argument)){
      if (!str_prefix(argument, "mammoth"))
	beast = MOB_VNUM_MAMMOTH;
      else if (!str_prefix(argument, "raven"))
	beast = MOB_VNUM_RAVEN;
      else if (!str_prefix(argument, "leopard"))
	beast = MOB_VNUM_LEOPARD;
      else if (!str_prefix(argument, "wyvern"))
	beast = MOB_VNUM_WYVERN;
      else if (!str_prefix(argument, "displacer"))
	beast = MOB_VNUM_DISPLACER;
      else{
	send_to_char("You can call no such beast.\n\r", ch );
	return;
      }
    }
    else
      beast = 0;


    for (vch = char_list; vch; vch = vch->next)
    {
        if (IS_AFFECTED (vch, AFF_CHARM) && vch->master == ch)
	{
	  if (IS_NPC(vch) && vch->pIndexData->vnum == MOB_VNUM_MAMMOTH){
	    i++;
	    have_mammoth = TRUE;
	  }
	  if (IS_NPC(vch) && vch->pIndexData->vnum == MOB_VNUM_RAVEN){
	    i++;
	    have_mammoth = TRUE;
	  }
	  else if (IS_NPC(vch) && vch->pIndexData->vnum == MOB_VNUM_LEOPARD){
	    i++;
	    have_leopard = TRUE;
	  }
	  else if (IS_NPC(vch) && vch->pIndexData->vnum == MOB_VNUM_WYVERN){
	    i++;
	    have_wyvern = TRUE;
	  }
	  else if (IS_NPC(vch) && vch->pIndexData->vnum == MOB_VNUM_DISPLACER){
	    i++;
	    have_displacer = TRUE;
	  }
	}
    }
    if (i > 1){
      if (i < 3 && fHorde){
	beast = MOB_VNUM_RAVEN;
      }
      else{
	send_to_char("You already have too many beasts.\n\r",ch);
	return;
      }
    }
    if (is_affected(ch,gsn_beast_call))
    {
	send_to_char("You aren't up to calling more beasts yet.\n\r",ch);
	return;
    }

    if (number_percent() < get_skill(ch, gsn_horn)){
      check_improve(ch, gsn_horn, TRUE, 1);
      act("You sound the Horn of Nature.",ch,NULL,NULL,TO_CHAR);
      act("$n sounds the Horn of Nature.",ch,NULL,NULL,TO_ROOM);
      aecho(ch->in_room->area, "A loud note of the Horn of Nature pierces the air.\n\r");
      fHorn = TRUE;
    }
    else{
      act("You shout a beast call.",ch,NULL,NULL,TO_CHAR);
      act("$n shouts a wild call.",ch,NULL,NULL,TO_ROOM);
    }
    WAIT_STATE2(ch,skill_table[gsn_beast_call].beats);
    switch(ch->in_room->sector_type)
    {
	case(SECT_INSIDE):	if (fHorn) chance -= 15;else chance = 0;break;
	case(SECT_CITY):	if (fHorn) chance -= 15;else chance = 0;break;
	case(SECT_FIELD):	if (fHorn) chance -=  5;else chance = 0;break;
	case(SECT_FOREST):	chance += 10;	break;
	case(SECT_HILLS):	chance += 5;	break;
	case(SECT_MOUNTAIN):	if (fHorn) chance -=  0;else chance = 0;break;
	case(SECT_WATER_SWIM):	if (fHorn) chance -=  0;else chance = 0;break;
	case(SECT_WATER_NOSWIM):if (fHorn) chance -=  0;else chance = 0;break;
	case(SECT_AIR):		if (fHorn) chance -=  0;else chance = 0;break;
	case(SECT_DESERT):	if (fHorn) chance -=  0;else chance = 0;break;
    default:			if (fHorn) chance -=  0;else chance = 0;break;
    }
    if (chance == 0 && (!IS_IMMORTAL(ch)))
    {
	send_to_char("There aren't any beasts to call in this area.\n\r",ch);
	return;
    }
    if (IS_SET(ch->act,PLR_DOOF))
    {
	send_to_char("They refuse to answer to your call.\n\r",ch);
	return;
    }
    beastroll = number_percent();
    if (IS_WEREBEAST(ch->race) && chance > 0)
	chance += 10;
    if (fAdv)
      chance += 10;
    if (beastroll < chance)
    {
    	beastroll += ch->level /5;
    	beastroll += 2*(get_curr_stat(ch,STAT_LUCK) - 16);
    	if (IS_WEREBEAST(ch->race))
	    beastroll += 10;
	if (fAdv)
	  beastroll += 20;
	if (!IS_IMMORTAL(ch))
	{
	ch->mana -= 80;
	af.type			= gsn_beast_call;
	af.level		= ch->level;
	af.duration		= beast != 0 ? 6 : 12;
	af.location		= 0;
	af.modifier		= 0;
	af.bitvector		= 0;
	affect_to_char(ch, &af);
	check_improve(ch,gsn_beast_call,TRUE,1);
	}
    	if (beastroll > 90 && ch->level >= 40 && !have_mammoth && (beast == 0 || beast == MOB_VNUM_MAMMOTH))
	{
	    act("A large mammoth comes to your rescue.",ch,NULL,NULL,TO_CHAR);
	    act("A large mammoth arrives to heed $n's call.",ch,NULL,NULL,TO_ROOM);
	    mob = create_mobile( get_mob_index( MOB_VNUM_MAMMOTH));
	    mob->max_hit	= ch->level * 13;
	    mob->hitroll	= ch->level/5 + 7;
	    mob->damroll	= 2*ch->level/5 + 7;
	    mob->damage[DICE_NUMBER]	= 4;
	    mob->damage[DICE_TYPE]	= (ch->level/10) +4;
        }
        else if (beastroll > 70 && ch->level > 35 && !have_leopard && (beast == 0 || beast == MOB_VNUM_LEOPARD))
        {
	    act("A spotted leopard comes to your rescue.",ch,NULL,NULL,TO_CHAR);
	    act("A spotted leopard arrives to heed $n's call.",ch,NULL,NULL,TO_ROOM);
	    mob = create_mobile( get_mob_index( MOB_VNUM_LEOPARD));
	    mob->max_hit	= ch->level * 12;
	    mob->hitroll	= ch->level/5 + 12;
	    mob->damroll	= ch->level/5 + 5;
	    mob->damage[DICE_NUMBER]	= 4;
	    mob->damage[DICE_TYPE]	= (ch->level/10) +3;
        }
        else if (beastroll > 50 && ch->level >= 30 && !have_wyvern && (beast == 0 || beast == MOB_VNUM_WYVERN))
        {
	    act("A large wyvern comes to your rescue.",ch,NULL,NULL,TO_CHAR);
	    act("A large wyvern arrives to heed $n's call.",ch,NULL,NULL,TO_ROOM);
	    mob = create_mobile( get_mob_index( MOB_VNUM_WYVERN));
	    mob->max_hit	= ch->level * 11;
	    mob->hitroll	= ch->level/5 + 10;
	    mob->damroll	= ch->level/5;
	    mob->damage[DICE_NUMBER]	= 4;
	    mob->damage[DICE_TYPE]	= (ch->level/10) +2;
	}
	else if (beastroll > 30 && ch->level >= 25 && !have_displacer && (beast == 0 || beast == MOB_VNUM_DISPLACER))
	{
	    act("A snarling displacer beast comes to your rescue.",ch,NULL,NULL,TO_CHAR);
	    act("A snarling displacer beast arrives to heed $n's call.",ch,NULL,NULL,TO_ROOM);
	    mob = create_mobile( get_mob_index( MOB_VNUM_DISPLACER));
	    mob->max_hit	= ch->level * 10;
	    mob->hitroll	= ch->level/5 + 5;
	    mob->damroll	= ch->level/5 + 7;
	    mob->damage[DICE_NUMBER]	= 4;
	    mob->damage[DICE_TYPE]	= (ch->level/10) + 1;
	}
	else if (beast == 0 || beast == MOB_VNUM_RAVEN)
	{
	    act("A large, black raven comes to your rescue.",ch,NULL,NULL,TO_CHAR);
	    act("A large, black raven arrives to heed $n's call.",ch,NULL,NULL,TO_ROOM);
	    mob = create_mobile( get_mob_index( MOB_VNUM_RAVEN));
	    mob->max_hit	= ch->level * 9;
	    mob->hitroll	= ch->level/5 + 20;
	    mob->damroll	= ch->level/5 + 3;
	    mob->damage[DICE_NUMBER]	= 4;
	    mob->damage[DICE_TYPE]	= (ch->level/10);
	}
	else{
	  send_to_char("You failed to locate that animal.\n\r", ch );
	  return;
	}
	char_to_room(mob,ch->in_room);
	mob->max_hit 		+= 200;
	mob->level		= ch->level;
	if (fAdv){
	mob->level		+= 1;
	  mob->max_hit = 3 * mob->max_hit / 2;
	}
	mob->hit		= mob->max_hit;
	mob->alignment = ch->alignment;
	SET_BIT(mob->affected_by, AFF_CHARM);
	mob->comm = COMM_NOTELL | COMM_NOYELL | COMM_NOCHANNELS;
	mob->summoner = ch;
	add_follower(mob,ch);
	mob->leader=ch;
    }
    else
    {
	ch->mana -= 40;
	send_to_char("Your call goes unanswered.\n\r",ch);
	check_improve( ch, gsn_beast_call, FALSE, 1);
    }
}

void do_rage( CHAR_DATA *ch, char *argument)
{
    sh_int chance;
    AFFECT_DATA af;
    int level, str = 0, con, dur = 0;
    char buf[MIL];

    /* Useless conditional */
    if( str != 0 )
        str = 0;

    if ((chance = get_skill(ch,gsn_rage)) < 2)
    {
	send_to_char("You're already fighting as hard as you can.\n\r",ch);
	return;
    }
    if (IS_AFFECTED2(ch, AFF_RAGE))
    {
	send_to_char("The madness has already begun!\n\r",ch);
	return;
    }
    if (ch->fighting == NULL)
    {
	send_to_char("You can't get worked up enough without a good fight.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_AFFECTED(ch, AFF_HASTE))
    {
	send_to_char("You feel too sluggish to get worked up.\n\r",ch);
	return;
    }
    if (IS_AFFECTED(ch, AFF_CALM))
    {
	send_to_char("You're too calm to get worked up.\n\r",ch);
	return;
    }
    affect_strip(ch,gsn_prayer);
    level = ch->level;
    str = get_curr_stat(ch, STAT_STR);
    con = ch->perm_stat[STAT_CON];
    chance /= 2;
    chance = UMAX(10, (chance + level));
    if (IS_AFFECTED(ch, AFF_BERSERK))
	chance += 5;
    if (ch->race == race_lookup("dwarf"))
	chance += 5;
    if (IS_AFFECTED(ch,AFF_HASTE))
	chance += 5;
    chance += 35 - ((ch->hit / ch->max_hit) * 50);
    if (number_percent( ) < chance)
    {
      char* msg1, *msg2;
      if (get_skill(ch, gsn_path_dev)){
	msg1 = "your mind descends into oblivion!";
	msg2 = "$s mind descends into oblivion!";
	dur = UMAX(0, number_fuzzy(level / 6));
      }
      else if (get_skill(ch, gsn_path_fury)){
	msg1 = "the madness engulfs you!";
	msg2 = "$s madness engulfs $m!";
	dur = UMAX(1, 6 - number_fuzzy(level / 9));
      }
      else if (get_skill(ch, gsn_path_anger)){
	msg1 = "your anger clouds your mind.";
	msg2 = "$s anger clouds $s mind.";
	dur = UMAX(0, 4 - number_fuzzy(level / 11));
      }
      else{
	msg1 = "the rage begins.";
	msg2 = "$s rage begins.";
	dur = UMAX(0, 4 - number_fuzzy(level / 11));
      }
      sprintf(buf, "You feel your veins course with power as %s", msg1);
      act(buf, ch, NULL, NULL, TO_CHAR);
      sprintf(buf, "$n's eyes begin to blaze as %s", msg2);
      act(buf, ch, NULL, NULL, TO_ROOM);

      af.where		= TO_AFFECTS2;
      af.type			= gsn_rage;
      af.level		= level;
      af.duration		= dur;
      af.location		= APPLY_STR;
      af.bitvector		= AFF_RAGE;
      af.modifier		= ch->size *2;
      affect_to_char(ch,&af);
      af.location		= APPLY_HITROLL;
      af.modifier		= str_app[get_curr_stat(ch,STAT_STR)].tohit + ch->level/4;
      affect_to_char(ch,&af);
      af.location		= APPLY_DAMROLL;
      af.modifier		= str_app[get_curr_stat(ch,STAT_STR)].todam + ch->level/4;
      affect_to_char(ch,&af);
      af.location		= APPLY_HIT;
      af.modifier		= (level * con) / 3;
      affect_to_char(ch,&af);
      ch->hit += (level * con) /3;
      WAIT_STATE2(ch,12);
      check_improve(ch,gsn_rage,TRUE,1);
    }
    else
      {
	act("You tense up for a moment, but relax again.",ch,NULL,NULL,TO_CHAR);
	act("$n tenses up for a brief moment.",ch,NULL,NULL,TO_ROOM);
    	WAIT_STATE2(ch,24);
	check_improve(ch,gsn_rage,FALSE,1);
      }
}

void do_relax( CHAR_DATA *ch, char *argument)
{
    sh_int chance;
    int level, str = 0, con = 0;
    bool fAdv = get_skill(ch, gsn_path_anger) > 1;

    /* Useless conditional */
    if( str != con )
        str = con = 0;

    if ((chance = get_skill(ch,gsn_relax)) == 0)
    {
	send_to_char("Breathe in... breathe out...\n\r",ch);
	return;
    }
    if (!IS_AFFECTED2(ch, AFF_RAGE))
    {
	send_to_char("You're as relaxed as you can get.\n\r",ch);
	return;
    }
    if (get_skill(ch, gsn_path_dev) > 1){
      send_to_char("Blood! More BLOOD!\n\r",ch);
      return;
    }
    if (!fAdv && ch->fighting != NULL)
    {
	send_to_char("You can't relax while fighting!\n\r",ch);
	return;
    }
    level = ch->level;
    str = get_curr_stat(ch, STAT_STR);
    con = ch->perm_stat[STAT_CON];
    chance = (chance /2) + level;
    if (IS_AFFECTED(ch, AFF_BERSERK))
	chance -= 5;
    if (ch->race == race_lookup("dwarf"))
	chance += 5;
    if (IS_AFFECTED(ch,AFF_SLOW) && !IS_AFFECTED(ch,AFF_HASTE))
	chance += 5;
    chance += get_curr_stat(ch,STAT_LUCK) - 16;
/* path of anger ALWAYS relaxes and faster */
    if (fAdv){
      chance = 100;
      WAIT_STATE2(ch, skill_table[gsn_relax].beats / 2);
    }
    else
      WAIT_STATE2(ch,skill_table[gsn_relax].beats);

    if (number_percent( ) < chance){
      act("You take a deep breath and manage to relax.",ch,NULL,NULL,TO_CHAR);
      act("$n takes a deep breath and calms down.",ch,NULL,NULL,TO_ROOM);
      REMOVE_BIT(ch->affected2_by,AFF_RAGE);
      affect_strip(ch,gsn_rage);
      ch->hit -= (ch->level * ch->perm_stat[STAT_CON]) /3;
      check_improve(ch,gsn_relax,TRUE,1);
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
	      act_new("You are stunned, but will probably recover.",ch, NULL, NULL, TO_CHAR, POS_DEAD );
            }
	  else
            {
	      act_new("Your body could not sustain the injuries you've suffered.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
	      act("$n suddenly clutches $s wounds and slumps to the ground.",ch,NULL,NULL,TO_ROOM);
	      act( "$n is DEAD!!", ch, NULL, NULL, TO_ROOM );
	      act_new( "You have been KILLED!!", ch, NULL, NULL, TO_CHAR, POS_DEAD );
	      raw_kill( ch, ch );
	    }
	}
    }
    else
      {
	send_to_char("It's no good, all you want is MORE BLOOD!\n\r",ch);
	check_improve(ch,gsn_relax,FALSE,1);
      }
}

void do_vanish( CHAR_DATA *ch, char *argument)
{
    int chance;
    ROOM_INDEX_DATA *pRoomIndex;
    int e_r1[] = {ROOM_NO_TELEPORTIN, ROOM_NO_GATEIN};
    int va = ch->in_room->area->vnum;
    int area_pool = 1;

    if ((chance = get_skill(ch, gsn_vanish)) == 0){
      send_to_char("Huh?\n\r",ch);
      return;
    }
    if (ch->mana < 30){
      send_to_char("You don't have enough mana.\n\r",ch);
      return;
    }
    if (IS_AFFECTED(ch,AFF_CHARM)){
      send_to_char("Not while someone else controls your thoughts.\n\r",ch);
      return;
    }
    if (!IS_NPC(ch) && ch->pcdata->pStallion != NULL)
      {
	sendf(ch, "You cannot vanish on %s!\n\r",ch->pcdata->pStallion->short_descr);
	return;
      }

    chance += get_curr_stat(ch, STAT_DEX) - 20;
    chance += get_curr_stat(ch, STAT_INT) + get_curr_stat(ch, STAT_WIS) - 40;
    chance += 2*(get_curr_stat(ch,STAT_LUCK)-16);
    if ( ch->fighting != NULL)
      chance /= 3;
    WAIT_STATE2(ch, skill_table[gsn_vanish].beats);
    if (IS_SET(ch->in_room->room_flags, ROOM_NO_TELEPORTOUT)
	|| IS_SET(ch->in_room->room_flags, ROOM_NO_GATEOUT)
	|| IS_SET(ch->in_room->room_flags, ROOM_NO_SUMMONOUT)
	|| IS_SET(ch->in_room->room_flags, ROOM_NO_INOUT)
	|| IS_SET(ch->in_room->area->area_flags, AREA_NOMORTAL)
	)
      chance = 0;
/* get the random room */
    pRoomIndex =  get_rand_room(va,va,		//area range (0 to ignore)
				0,0,		//room ramge (0 to ignore)
				NULL,0,		//areas to choose from
				NULL,0,		//areas to exclude
				NULL,0,		//sectors required
				NULL,0,		//sectors to exlude
				NULL,0,		//room1 flags required
				e_r1,2,		//room1 flags to exclude
				NULL,0,		//room2 flags required
				NULL,0,		//room2 flags to exclude
				area_pool,	//number of seed areas
				TRUE,		//exit required?
				FALSE,		//Safe?
				ch);		//Character for room checks
    if ( pRoomIndex == NULL
	 || IS_SET(pRoomIndex->area->area_flags, AREA_NOMORTAL)
	 || number_percent( ) > chance ){
      ch->mana -= 15;
      act("You throw down a handful of smoke.",ch, NULL, NULL, TO_CHAR);
      act("$n throws down a handful of smoke.", ch, NULL, NULL, TO_ROOM);
      check_improve(ch, gsn_vanish, FALSE, 1);
      return;
    }
    else{
      ch->mana -= 30;
      if (ch->fighting != NULL){
	if ( IS_NPC(ch->fighting) && !IS_SET(ch->fighting->off_flags,CABAL_GUARD)){
	  if (!IS_NPC(ch))
	    ch->fighting->hunting = ch;
	  else if (ch->master != NULL)
	    ch->fighting->hunting = ch->master;
	  else
	    ch->fighting->hunting = ch;
	  ch->fighting->hunttime = 0;
	}
	stop_fighting(ch, TRUE);
      }
      act("You throw down a handful of smoke and vanish.",ch, NULL, NULL, TO_CHAR);
      act("$n throws down a handful of smoke and vanishes.", ch, NULL, NULL, TO_ROOM);
      char_from_room( ch);
      char_to_room( ch, pRoomIndex);
      check_improve(ch, gsn_vanish, TRUE, 1);
      act("$n suddenly appears.", ch, NULL, NULL, TO_ROOM);
      do_look(ch, "auto" );
    }
}

void do_blindness_dust( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *vch, *vch_next;
    char buf[MSL];
    int chance;
    if ((chance = get_skill(ch, gsn_blindness_dust)) == 0)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (ch->mana < 15)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch, skill_table[gsn_blindness_dust].beats);
    if ( number_percent( ) < chance)
    {
	act("You fill the room with a cloud of dust.",ch,NULL,NULL,TO_CHAR);
	act("$n fills the room with a cloud of dust.",ch,NULL,NULL,TO_ROOM);
	ch->mana -= 15;
	check_improve(ch, gsn_blindness_dust, TRUE, 1);
    	for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    	{
	    vch_next = vch->next_in_room;
            if ( vch != ch && !is_area_safe(ch, vch) )
	    {
	      a_yell(ch, vch);
	      //predict interdict
	      if (predictCheck(ch, vch, "blindness_dust", skill_table[gsn_blindness_dust].name))
		continue;
	      spell_blindness(gsn_blindness, ch->level - 4, ch, (void *) vch,TARGET_CHAR);
	      if ( ch->fighting != vch && vch->fighting != ch)
	    	{

		  sprintf(buf, "Help! %s just threw dust in my eyes!",PERS(ch,vch));
		  j_yell(vch,buf);
		  if (IS_AWAKE(vch) && vch->fighting == NULL)
		    multi_hit(vch,ch,TYPE_UNDEFINED);
	    	}
	    }
    	}
    }
    else
    {
	ch->mana -= 7;
	act("The dust billows away in the wind.",ch, NULL, NULL, TO_CHAR);
	act("A cloud of dust billows away in the wind.", ch, NULL, NULL, TO_ROOM);
	check_improve(ch, gsn_blindness_dust, FALSE, 1);
    }
}

void do_poison_smoke( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *vch, *vch_next;
    char buf[MSL];
    int chance;
    if ((chance = get_skill(ch, gsn_poison_smoke)) == 0)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (ch->mana < 15)
    {
	send_to_char("You don't have enough mana.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch, skill_table[gsn_poison_smoke].beats);
    if ( number_percent( ) < chance)
    {
	act("You fill the room with a cloud of smoke.",ch,NULL,NULL,TO_CHAR);
	act("$n fills the room with a cloud of smoke.",ch,NULL,NULL,TO_ROOM);
	ch->mana -= 15;
	check_improve(ch, gsn_poison_smoke, TRUE, 1);
    	for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    	{
	    vch_next = vch->next_in_room;
            if ( vch != ch && !is_area_safe(ch, vch) )
	    {
	      a_yell(ch, vch);
	      //predict interdict
	      if (predictCheck(ch, vch, "poison_smoke", skill_table[gsn_poison_smoke].name))
		continue;

	    	spell_poison(gsn_poison, ch->level, ch, (void *) vch,TARGET_CHAR);
 	    	if ( ch->fighting != vch && vch->fighting != ch)
	    	{
		    sprintf(buf, "Help! %s just filled the room with smoke!",PERS(ch,vch));
		    j_yell(vch,buf);
		    if (IS_AWAKE(vch) && vch->fighting == NULL)
		    	multi_hit(vch,ch,TYPE_UNDEFINED);
	    	}
	    }
    	}
    }
    else
    {
	ch->mana -= 7;
	act("The smoke billows away in the wind.",ch, NULL, NULL, TO_CHAR);
	act("A cloud of smoke billows away in the wind.", ch, NULL, NULL, TO_ROOM);
	check_improve(ch, gsn_poison_smoke, FALSE, 1);
    }
}

void do_butcher(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *obj;
    char buf[MSL], arg[MSL];
    int chance;
    one_argument(argument,arg);
    if ( arg[0]=='\0' )
    {
        send_to_char("What do you want to butcher?\n\r",ch);
        return;
    }
    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    if ( (obj->item_type != ITEM_CORPSE_PC
	  && obj->item_type != ITEM_CORPSE_NPC)
	 || obj->condition < 2)
      {
        send_to_char("You can't butcher that.\n\r",ch);
	return;
      }
    if (!can_loot(ch, obj)){
      act("$g's power shields $p from your greedy hands.", ch, obj, NULL, TO_CHAR);
      return;
    }
    if (obj->item_type == ITEM_CORPSE_PC && (ch->level < 15 || obj->level < 15) && !IS_IMMORTAL(ch))
    {
	send_to_char("Not that corpse.\n\r",ch);
	return;
    }
    if (obj->pIndexData->vnum == OBJ_VNUM_FAKE_CORPSE)
    {
	send_to_char("Not that corpse.\n\r",ch);
	return;
    }
    if ((chance = get_skill(ch, gsn_butcher)) == 0)
    {
        send_to_char("You wouldn't know where to begin butchering.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_butcher].beats);
    if ( number_percent() < chance + get_curr_stat(ch,STAT_LUCK) - 16)
    {
        int numsteaks = number_bits(2) + 1, i;
        OBJ_DATA *steak;
        if (numsteaks > 1)
	{
	    sprintf(buf, "$n butchers $p and creates %i steaks.",numsteaks);
	    act(buf,ch,obj,NULL,TO_ROOM);
	    sprintf(buf, "You butcher $p and create %i steaks.",numsteaks);
	    act(buf,ch,obj,NULL,TO_CHAR);
	}
        else
	{
	    act("$n butchers $p and creates a steak." ,ch,obj,NULL,TO_ROOM);
	    act("You butcher $p and create a steak."  ,ch,obj,NULL,TO_CHAR);
	}
        check_improve(ch,gsn_butcher,TRUE,1);
        for (i=0; i < numsteaks; i++)
	{
	    buf[0]='\0';
	    strcat(buf,"A steak of ");
	    strcat(buf,str_dup(obj->short_descr));
	    steak = create_object(get_obj_index(OBJ_VNUM_STEAK),0);
 	    steak->value[0] = 10 + ch->level/2;
	    steak->value[1] = 10 + ch->level/2;
	    free_string( steak->short_descr );
	    steak->short_descr = str_dup( buf );
	    strcat(buf," is here.");
	    free_string( steak->description );
	    steak->description = str_dup( buf );
	    obj_to_room(steak,ch->in_room);
	}
    }
    else
    {
        act("You fail and destroy $p.",ch,obj,NULL,TO_CHAR);
        act("$n fails to butcher $p and destroys it.",ch,obj,NULL,TO_ROOM);
        check_improve(ch,gsn_butcher,FALSE,1);
    }
    free_string(obj->description);
    obj->description = str_dup("A butchered corpse lies here.");
    obj->condition = 1;
}

void do_caltraps(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int chance;
    if (IS_NPC(ch) || (chance = get_skill(ch,gsn_caltraps)) == 0)
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
    act("You throw a handful of sharp spikes at the feet of $N.",ch,NULL,victim,TO_CHAR);
    act("$n throws a handful of sharp spikes at your feet!",ch,NULL,victim,TO_VICT);
    WAIT_STATE2(ch,skill_table[gsn_caltraps].beats);
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += ch->level - victim->level;
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    if(!IS_NPC(ch) && number_percent( ) >= chance)
    {
        damage(ch,victim,0,gsn_caltraps,DAM_PIERCE,TRUE);
        check_improve(ch,gsn_caltraps,FALSE,1);
        return;
    }
    if (!is_affected(victim,gsn_caltraps))
    {
        act("$N starts limping.",ch,NULL,victim,TO_CHAR);
        act("You start to limp.",ch,NULL,victim,TO_VICT);
        af.type		= gsn_caltraps;
        af.level	= ch->level;
        af.bitvector	= 0;
	af.where = TO_AFFECTS;
        af.duration	= ch->level /10;
        af.location	= APPLY_HITROLL;
        af.modifier	= -5;
        affect_to_char(victim,&af);
        af.location	= APPLY_DAMROLL;
        af.modifier	= -5;
        affect_to_char(victim,&af);
        af.location	= APPLY_DEX;
        af.modifier	= -5;
        affect_to_char(victim,&af);
        check_improve(ch,gsn_caltraps,TRUE,1);
    }
    damage(ch,victim,2 * ch->level /3,gsn_caltraps,DAM_PIERCE,TRUE);
}

void do_nerve(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    int chance;
    if ((chance = get_skill(ch,gsn_nerve)) == 0)
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    if (ch->fighting == NULL)
    {
        send_to_char("You aren't fighting anyone.\n\r",ch);
        return;
    }
    victim = ch->fighting;
    if (is_affected(victim,gsn_nerve))
    {
        sendf(ch, "You cannot weaken %s any more.\n\r", PERS(victim,ch));
        return;
    }
    WAIT_STATE2( ch, skill_table[gsn_nerve].beats );
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_STR);
    chance += ch->level - victim->level;
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    if (number_percent() < chance)
    {
        AFFECT_DATA af;
        af.type   = gsn_nerve;
        af.level  = ch->level;
        af.duration = ch->level * PULSE_VIOLENCE/PULSE_TICK;
        af.location = APPLY_STR;
        af.modifier = - ch->level / 10;
        af.bitvector = 0;
        affect_to_char(victim,&af);
        act("You weaken $N with your nerve pressure.",ch,NULL,victim,TO_CHAR);
        act("$n weakens you with $s nerve pressure.",ch,NULL,victim,TO_VICT);
        act("$n weakens $N with $s nerve pressure.",ch,NULL,victim,TO_NOTVICT);
        check_improve(ch,gsn_nerve,TRUE,1);
    }
    else
    {
        send_to_char("You press the wrong points and fail.\n\r",ch);
        act("$n tries to weaken you with nerve pressure, but fails.",ch,NULL,victim,TO_VICT);
        act("$n tries to weaken $N with nerve pressure, but fails.",ch,NULL,victim,TO_NOTVICT);
        check_improve(ch,gsn_nerve,FALSE,1);
    }
}

void weapon_cleave( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;
    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL && ( obj = get_eq_char( victim, WEAR_SECONDARY ) ) == NULL )
	return;
    act( "$n destroys your $p!", ch, obj, victim, TO_VICT    );
    act( "You destroy $N's $p!",  ch, obj, victim, TO_CHAR    );
    act( "$n destroys $N's $p!",  ch, obj, victim, TO_NOTVICT );
    extract_obj( obj );
}

void do_weapon_cleave( CHAR_DATA *ch, char *argument )
{
  /*
    This is the new and improved weapon cleave.
    The chance is calculated by getting the breakin power
    of the cahracter, and subtracting the "strength" of an item.
    Resultant is the chance.

    Breaking power of char:
    - Str > 20 is a bonus.
    - Weapon profficency is a bonus.
    - 2h weapon is a bonus
    ----Daggers cannot be cleaved by 2h weapons.
    - Axes are bonus
    - Weight of weapon is a bonus.
    - luck is small bonus.
    - level is small bonus.
    The above is calculated for primary AND secondary.
    Secondary only contributing 1/3, 2/3 if minotaur.

    Resistance of the weapon being broken.
    - Level of the weapon
    - Weight of the weapon
    - Skill of weapon.
    - magica/blessed
  */

  char arg[MIL];
  CHAR_DATA *victim;

//weapons on attacker
  OBJ_DATA *prim_obj = NULL;
  OBJ_DATA *sec_obj = NULL;

//weapon being cleaved
  OBJ_DATA *tar_obj = NULL;

//Data:
  int prim_chance = 0; //holds chance of the primary iweapon
  int sec_chance = 0;  //secondary item chance (fraction is added to total)
  int chance = 0;//holds final chance.

  int prim_skill = 0;//skil with primary weapon
  int sec_skill = 0;//skill with sec. weapon
  int tar_skill = 0;//skill with targets weapon
  int cleave_skill = 0;//skill with weapon cleave
  int str_bonus = 0;//str bonus (mult. by weight bonus)
  int weight_bonus = 0;

//const
  const int rage_mod = 15;
  const int str_med = 18;//midpoint for str bonus
  const int str_mod = 400;//multiplier for str bonus

  const int cleave_med = 85;//midpoint for cleave bonus.
  const int skill_med =  100;//midpoint for weapon bonus.

  const int two_mod = 8;//bonus for 2h weapon
  const int axe_mod = 8;//bonus for axes.
  const int pole_mod = 12;//bonus for polearms.

  const int weight_med = 100;//midpoint for weight bonus
  const int weight_mod = 10;//multiplier for weight bonus
  const int weight_cap = 400;

  const int luck_mod = 2;
  const int lvl_mod = 1;
  const int wep_lvl_mod = 75;//precentage bonus

  const int magic_mod = 10; //(magic items blow easier)
  const int bless_tar_mod = 50; //(blessed/evil items dont break as easy..)
  const int bless_mod = 130;// (blessed/evil items destroy better.)

//booleans
  bool fNPC = FALSE;
  bool f2H = TRUE;//used to check if char is using both 2h weapons.
  bool fAxeMsg = TRUE;//used to check for extra axe/pole message
  bool fPrim = TRUE;
  bool fSec = TRUE;
  bool fWhip = TRUE;//used to check if cha only ha a whip.
//check for skill
  if ((cleave_skill = get_skill(ch,gsn_weapon_cleave)) == 0)
    {
      send_to_char( "Huh?\n\r", ch );
      return;
    }

  //Get victim
  one_argument( argument, arg );

  if ( ( victim = ch->fighting ) == NULL )
    {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
    }

  //attacker can just cleave who he is fighting or redirect to new.
  if (arg[0] != '\0')
    if ((victim = get_char_room(ch, NULL,arg)) == NULL)
      {
	send_to_char("They aren't here.\n\r",ch);
	return;
      }
      victim = ch->fighting;

      if  ((prim_obj = get_eq_char( ch, WEAR_WIELD )) == NULL)
	fPrim = FALSE;
      if  ((sec_obj = get_eq_char( ch, WEAR_SECONDARY )) == NULL )
	fSec = FALSE;

//check for weapon being wielded.
  if ( (!fPrim && !fSec) || ((fNPC = IS_NPC(ch)) && !IS_SET(ch->off_flags,OFF_DISARM)) )
    {
      send_to_char( "You must wield a weapon.\n\r", ch );
      return;
    }

  if ( (tar_obj = get_eq_char( victim, WEAR_WIELD )) == NULL)
    if ( (tar_obj = get_eq_char( victim, WEAR_SECONDARY )) == NULL )
      {
	sendf(ch, "%s is not wielding a weapon.\n\r", PERS(victim,ch) );
	return;
      }

  //DEBUG
  /*
 sendf(ch, "`#Prim: %s, Sec: %s, Tar: %s, Npc?: %d``\n\r",
(prim_obj == NULL ? "NULL" : prim_obj->name),
(sec_obj == NULL ? "NULL" : sec_obj->name),
(tar_obj == NULL ? "NULL" : tar_obj->name),
fNPC);
*/
//Start chance calculation:
  if (fNPC)
    chance = ch->level;
  else
    {
//Chance calculation:

     // COMMON //

//The str bonus: (used as a multiplier later for weight..)
     chance = str_bonus = (str_mod * (get_curr_stat(ch, STAT_STR) - str_med) / 100);

//size:
     chance += affect_by_size(ch, victim);

//luck
     chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK))/luck_mod;

//level bonus
     chance += (ch->level - victim->level) * lvl_mod;

//rage bonus
     if (is_affected(ch, gsn_rage))
       chance += rage_mod;
     //DEBUG
     //     sendf(ch, "`#Common: %d\n\r``", chance);

     // PRIMARY //
     if (fPrim)
       {

	 //weapon weigth: (30 at 400 weight)
	 weight_bonus = (weight_mod * ((get_obj_weight(prim_obj) > weight_cap? weight_cap : get_obj_weight(prim_obj)) - weight_med) / 100);
	 if (prim_obj->value[0] != WEAPON_WHIP)
	   fWhip = FALSE;

	 prim_chance += weight_bonus;
	 //	 sendf(ch, "`@Mod 1: %d``", prim_chance);//DEBUG

	 //2h bonus:
	 if ( (IS_WEAPON_STAT(prim_obj,WEAPON_TWO_HANDS))
	      || is_affected(ch,gsn_double_grip) )
	   {
	     //bonus message.
	     if (!fSec)
	       act("Both of your hands on $p you strike with a mighty force!", ch, prim_obj, NULL,  TO_CHAR);
	     prim_chance += two_mod;
	   }
	 else
	   f2H = FALSE;//check for 2h only attack for daggers.
	 //	 sendf(ch, "`@Mod 2: %d``", prim_chance);//DEBUG

	 //Axe bonus
	 if (prim_obj->value[0] == WEAPON_AXE)
	   {
	     //we give a little bonus message here.
	     fAxeMsg =  TRUE;
	     prim_chance += axe_mod;
	     if (ch->race == race_lookup("minotaur"))
	       {
		 //		 sendf(ch, "`$Race bonues!``");
		 prim_chance += axe_mod/2;
	       }
	   }
	 //	 sendf(ch, "`@Mod 3: %d``", prim_chance);//DEBUG

	 //Polearm
	 if (prim_obj->value[0] == WEAPON_POLEARM)
	   {
	     //bonus message.
	     fAxeMsg = TRUE;
	     prim_chance += pole_mod;
	   }
	 //	 sendf(ch, "`@Mod 4: %d``", prim_chance);//DEBUG

//MULTIPLIERS
	 //skill with cleave: (get a bonus over 85 here)
	 prim_chance = cleave_skill * prim_chance / cleave_med;
	 //	 sendf(ch, "`@Mod 5: %d``", prim_chance);//DEBUG

	 //Skill with current weapon
	 prim_skill = get_weapon_skill_obj(ch, prim_obj);
	 prim_chance =  prim_skill * prim_chance / skill_med;
	 //	 sendf(ch, "`@Mod 6: %d``", prim_chance);//DEBUG

	 //Skill with opponent weapon
	 tar_skill = 75 + (get_weapon_skill_obj(ch, tar_obj) / 4);
	 prim_chance = tar_skill * prim_chance / skill_med;
	 //	 sendf(ch, "`@Mod 7: %d``", prim_chance);//DEBUG

	 //check for blessing
	 if ( (ch->alignment > GOOD_THRESH)
	      && (IS_OBJ_STAT(prim_obj,ITEM_BLESS)) )
	   prim_chance = bless_mod * prim_chance / 100;
	 else if ( (ch->alignment < EVIL_THRESH)
		   && (IS_OBJ_STAT(prim_obj,ITEM_EVIL)) )
	   prim_chance = bless_mod * prim_chance / 100;
	 //DEBUG:
	 //     sendf(ch, "`#Prim: %d\n\r``", prim_chance);

     if (!f2H || tar_obj->value[0] != WEAPON_DAGGER)
       chance += prim_chance;
       }//end prim weapon.

     // SECONDARY //
     if (fSec)
       {
	 if (sec_obj->value[0] != WEAPON_WHIP)
	   fWhip = FALSE;

//BONUSES
	 //weapon weigth: (20 at 400 weight)
	 weight_bonus = (weight_mod * ((get_obj_weight(sec_obj) > weight_cap? weight_cap : get_obj_weight(sec_obj)) - weight_med) / 100);

	 sec_chance = weight_bonus;

	 //2h bonus:
	 if ( (IS_WEAPON_STAT(sec_obj,WEAPON_TWO_HANDS))
	      || is_affected(ch,gsn_double_grip))
	   sec_chance += two_mod;
	 else
	   f2H = FALSE;//check for 2h only attack for daggers.

	 //Axe bonus
	 if (sec_obj->value[0] == WEAPON_AXE)
	   {
	     prim_chance += axe_mod;
	     if (ch->race == race_lookup("minotaur"))
	       prim_chance += axe_mod;
	   }

	 //Polearm
	 if (sec_obj->value[0] == WEAPON_POLEARM)
	   sec_chance += pole_mod;

//MULTIPLIERS
	 //skill with cleave: (get a bonus over 85 here)
	 sec_chance = cleave_skill * sec_chance / cleave_med;

	 //Skill with current weapon
	 sec_skill = get_weapon_skill_obj(ch, sec_obj);
	 sec_chance =  sec_skill * sec_chance / skill_med;

	 //Skill with opponent weapon
	 tar_skill = 75 + (get_weapon_skill_obj(ch, tar_obj) / 4);
	 sec_chance = tar_skill * sec_chance / skill_med;


	 //check for blessing
	 if ( (ch->alignment > GOOD_THRESH)
	      && (IS_OBJ_STAT(sec_obj,ITEM_BLESS)) )
	   sec_chance = bless_mod * sec_chance / 100;
	 else if ( (ch->alignment < EVIL_THRESH)
		   && (IS_OBJ_STAT(sec_obj,ITEM_EVIL)) )
	   sec_chance = bless_mod * sec_chance / 100;
	 //DEBUG
	 //     sendf(ch, "`#Sec: %d\n\r``", sec_chance);
     //We give a bonus to minotarus.
     if (ch->race == race_lookup("minotaur"))
       chance += sec_chance / 2;
     else
       chance += sec_chance / 3;
       }//end SEC weapon.
   }//end calculation of the chance.

  //whip check
  if (fWhip && !f2H)
    {
      send_to_char("With just a tiny whip?!\n\r", ch);
      return;
    }

//dagger check:
 if ( (tar_obj->value[0] == WEAPON_DAGGER)
      && ch->race != race_lookup("minotaur")
      && f2H)
   {
     send_to_char("Your weapon is far to clumsy to hit such small target.\n\r", ch);
     act("$n tries to destroy $p and misses by a mile.", ch, tar_obj, victim, TO_ROOM);
     return;
   }
 /* check for axe message */
 if (fAxeMsg){
   OBJ_DATA* obj = (prim_obj ? prim_obj : (sec_obj ? sec_obj : NULL));
   if (obj){
     if (obj->value[0] == WEAPON_POLEARM)
       act("$p sings with deadly force as you strike $N's weapon!", ch, obj, victim, TO_CHAR);
     else
       act("$p makes a satisfying chomp  as it chews into $N's weapon!", ch, obj, victim, TO_CHAR);
   }
 }

//Now we deal with quality of the target weapon:
 chance -= (wep_lvl_mod * tar_obj->level / 100);

//Bonuses for types of weapons:
 if ( (IS_WEAPON_STAT(tar_obj,WEAPON_TWO_HANDS)) || is_affected(victim,gsn_double_grip))
   chance -= two_mod;

//magic items are very fragile
 if ( (IS_OBJ_STAT(tar_obj,ITEM_GLOW) || IS_OBJ_STAT(tar_obj,ITEM_HUM) || tar_obj->enchanted ) )
      chance += magic_mod;

//we floor the chance
 chance = URANGE(5, chance, 80);

 //different types are harder to hack.

 switch (tar_obj->value[0])
   {
     // swords and such are regular standard //
   case (WEAPON_EXOTIC):
   case WEAPON_SWORD:
     break;

     // small weapons are extremely easy to hit. //
   case  WEAPON_DAGGER:
   case  WEAPON_WHIP:
     chance = 135 * chance / 100;break;

// haft weapons are easier to cleave due to the shaft //
   case WEAPON_SPEAR:
   case WEAPON_POLEARM:
   case WEAPON_STAFF:
     chance = 120 * chance / 100;break;

     // flail's chain get be hacked :) //
   case WEAPON_FLAIL:
     chance = 110 * chance / 100;break;

     // these are the sturdy weapons //
   case WEAPON_AXE:
   case WEAPON_MACE:
     chance = 80 * chance / 100;
   }//end select
 //DEBUG
 //     sendf(ch, "`#Chance after target: %d\n\r``", chance);


//blessed itmes are harder to break
 if ( (victim->alignment > GOOD_THRESH) && IS_OBJ_STAT(tar_obj,ITEM_BLESS) )
   chance = bless_tar_mod * chance / 100;
 else if ( (victim->alignment < EVIL_THRESH) && (IS_OBJ_STAT(tar_obj,ITEM_EVIL)) )
   chance = bless_tar_mod * chance / 100;


 //Final check, burproof items cannot be destroyed.
 if (IS_OBJ_STAT(tar_obj, ITEM_BURN_PROOF)
     || IS_OBJ_STAT(tar_obj, ITEM_HAS_OWNER))
   chance = 0;
 //DEBUG
 //     sendf(ch, "`#Final Chance: %d\n\r``", chance);


//DONE now we can try to cleave..

 WAIT_STATE2( ch, skill_table[gsn_weapon_cleave].beats );
 if (number_percent() < chance)
   {
     weapon_cleave( ch, victim );
     check_improve(ch,gsn_weapon_cleave,TRUE,1);
   }
 else
   {
     act("You fail to destroy $N's $p.",ch,tar_obj,victim,TO_CHAR);
     act("$n tries to destroy your $p, but fails.",ch,tar_obj,victim,TO_VICT);
     act("$n tries to destroy $N's $p, but fails.",ch,tar_obj,victim,TO_NOTVICT);
      check_improve(ch,gsn_weapon_cleave,FALSE,1);
    }

}


void shield_cleave( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;
    if ( (obj = get_eq_char( victim, WEAR_SHIELD ) )== NULL)
	return;
    act( "$n destroys your shield!", ch, NULL, victim, TO_VICT    );
    act( "You destroy $N's shield!",  ch, NULL, victim, TO_CHAR    );
    act( "$n destroys $N's shield!",  ch, NULL, victim, TO_NOTVICT );
    extract_obj(obj);
}

void do_shield_cleave( CHAR_DATA *ch, char *argument )
{

  char arg[MIL];
  CHAR_DATA *victim;

//weapons on attacker
  OBJ_DATA *prim_obj = NULL;
  OBJ_DATA *sec_obj = NULL;

//weapon being cleaved
  OBJ_DATA *tar_obj = NULL;

//Data:
  int prim_chance = 0; //holds chance of the primary iweapon
  int sec_chance = 0;  //secondary item chance (fraction is added to total)
  int chance = 0;//holds final chance.

  int prim_skill = 0;//skil with primary weapon
  int sec_skill = 0;//skill with sec. weapon
  int tar_skill = 0;//skill with targets weapon
  int cleave_skill = 0;//skill with weapon cleave
  int str_bonus = 0;//str bonus (mult. by weight bonus)
  int weight_bonus = 0;

//const
  const int rage_mod = 15;
  const int str_med = 18;//midpoint for str bonus
  const int str_mod = 400;//multiplier for str bonus

  const int cleave_med = 85;//midpoint for cleave bonus.
  const int skill_med = 100;//midpoint for weapon bonus

  const int two_mod = 8;//bonus for 2h weapon
  const int axe_mod = 8;//bonus for axes.
  const int pole_mod = 12;//bonus for polearms.

  const int weight_med = 100;//midpoint for weight bonus
  const int weight_mod = 10;//multiplier for weight bonus
  const int weight_cap = 400;

  const int luck_mod = 2;
  const int lvl_mod = 1;
  const int wep_lvl_mod = 75;//precentage bonus

  const int magic_mod = 10; //(magic items blow easier)
  const int bless_tar_mod = 50; //(blessed/evil items dont break as easy..)
  const int bless_mod = 130;// (blessed/evil items destroy better.)

//booleans
  bool fNPC = FALSE;
  bool fPrim = TRUE;
  bool fSec = TRUE;

//check for skill
  if ((cleave_skill =get_skill(ch,gsn_shield_cleave)) == 0)
    {
      send_to_char( "Huh?\n\r", ch );
      return;
    }

  //Get victim
  one_argument( argument, arg );

  if ( ( victim = ch->fighting ) == NULL )
    {
      send_to_char( "You aren't fighting anyone.\n\r", ch );
      return;
    }

  //attacker can just cleave who he is fighting or redirect to new.
  if (arg[0] != '\0')
    if ((victim = get_char_room(ch, NULL,arg)) == NULL)
      {
	send_to_char("They aren't here.\n\r",ch);
	return;
      }
      victim = ch->fighting;

      if  ((prim_obj = get_eq_char( ch, WEAR_WIELD )) == NULL)
	fPrim = FALSE;
      if  ((sec_obj = get_eq_char( ch, WEAR_SECONDARY )) == NULL )
	fSec = FALSE;

//check for weapon being wielded.
      if ( (!fPrim && !fSec) || ((fNPC = IS_NPC(ch)) && !IS_SET(ch->off_flags,OFF_DISARM)) )
    {
      send_to_char( "You must wield a weapon.\n\r", ch );
      return;
    }

    if ( ( tar_obj = get_eq_char( victim, WEAR_SHIELD ) ) == NULL )
    {
	sendf(ch, "%s is not wearing a shield.\n\r", PERS(victim,ch) );
	return;
    }


  //DEBUG
    /*
    sendf(ch, "`#Prim: %s, Sec: %s, Tar: %s, Npc?: %d``\n\r",
	  (prim_obj == NULL ? "NULL" : prim_obj->name),
	  (sec_obj == NULL ? "NULL" : sec_obj->name),
	  (tar_obj == NULL ? "NULL" : tar_obj->name),
	  fNPC);
    */
//Start chance calculation:
 if (fNPC)
   chance = ch->level;
 else
   {
//Chance calculation:

     // COMMON //

//The str bonus: (used as a multiplier later for weight..)
     chance = str_bonus = (str_mod * (get_curr_stat(ch, STAT_STR) - str_med) / 100);

//size:
     chance += affect_by_size(ch, victim);

//luck
     chance += (get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK))/luck_mod;

//level bonus
     chance += (ch->level - victim->level) * lvl_mod;

//rage
     if (is_affected(ch, gsn_rage))
       chance += rage_mod;
     //DEBUG
     //     sendf(ch, "`#Common: %d\n\r``", chance);

     // PRIMARY //
     if (fPrim)
       {
	 //debug
	 //weapon weigth: (30 at 400 weight)
	 weight_bonus = (weight_mod * ((get_obj_weight(prim_obj) > weight_cap? weight_cap : get_obj_weight(prim_obj)) - weight_med) / 100);
	 prim_chance += weight_bonus;
	 //	 sendf(ch, "`@Mod 1: %d``", prim_chance);//DEBUG

	 //2h bonus:
	 if ( (IS_WEAPON_STAT(prim_obj,WEAPON_TWO_HANDS))
	      || is_affected(ch,gsn_double_grip) )
	   {
	     //bonus message.
	     if (!fSec)
	       act("Both of your hands on $p you strike with a mighty force!", ch, prim_obj, NULL,  TO_CHAR);
	     prim_chance += two_mod;
	   }
	 //	 sendf(ch, "`@Mod 2: %d``", prim_chance);//DEBUG

	 //Axe bonus
	 if (prim_obj->value[0] == WEAPON_AXE)
	   {
	     //we give a little bonus message here.
	     act("$p makes a satisfying chomp  as it chews into $N's shield!.", ch, prim_obj, victim, TO_CHAR);
	     prim_chance += axe_mod;
	     if (ch->race == race_lookup("minotaur"))
	       {
//		 sendf(ch, "`$Race bonues!``");//DEBUG
		 prim_chance += axe_mod/2;
	       }
	   }
	 //	 sendf(ch, "`@Mod 3: %d``", prim_chance);//DEBUG

	 //Polearm
	 if (prim_obj->value[0] == WEAPON_POLEARM)
	   {
	     //bonus message.
	     act("$p sings with deadly force as you strike $N's shield!", ch, prim_obj, victim, TO_CHAR);
	     prim_chance += pole_mod;
	   }
	 //	 sendf(ch, "`@Mod 4: %d``", prim_chance);//DEBUG

//MULTIPLIERS
	 //skill with cleave: (get a bonus over 85 here)
	 prim_chance = cleave_skill * prim_chance / cleave_med;
	 //	 sendf(ch, "`@Mod 5: %d``", prim_chance);//DEBUG

	 //Skill with current weapon
	 prim_skill = get_weapon_skill_obj(ch, prim_obj);
	 prim_chance =  prim_skill * prim_chance / skill_med;
	 //	 sendf(ch, "`@Mod 6: %d``", prim_chance);//DEBUG

	 //Opponents ability to block
	 tar_skill = 75 + (get_skill(victim, gsn_shield_block) / 4);
	 prim_chance =  skill_med * prim_chance / tar_skill;
	 //	 sendf(ch, "`@Mod 7: %d``", prim_chance);//DEBUG

	 //check for blessing
	 if ( (ch->alignment > GOOD_THRESH)
	      && (IS_OBJ_STAT(prim_obj,ITEM_BLESS)) )
	   prim_chance = bless_mod * prim_chance / 100;
	 else if ( (ch->alignment < EVIL_THRESH)
		   && (IS_OBJ_STAT(prim_obj,ITEM_EVIL)) )
	   prim_chance = bless_mod * prim_chance / 100;
	 //DEBUG:
	 //     sendf(ch, "`#Prim: %d\n\r``", prim_chance);

	 chance += prim_chance;
       }//end prim weapon.

     // SECONDARY //
     if (fSec)
       {
//BONUSES
	 //weapon weigth: (20 at 400 weight)
	 weight_bonus = (weight_mod * ((get_obj_weight(sec_obj) > weight_cap? weight_cap : get_obj_weight(sec_obj)) - weight_med) / 100);

	 sec_chance = weight_bonus;

	 //2h bonus:
	 if ( (IS_WEAPON_STAT(sec_obj,WEAPON_TWO_HANDS))
	      || is_affected(ch,gsn_double_grip))
	   sec_chance += two_mod;

	 //Axe bonus
	 if (sec_obj->value[0] == WEAPON_AXE)
	   {
	     prim_chance += axe_mod;
	     if (ch->race == race_lookup("minotaur"))
	       prim_chance += axe_mod;
	   }

	 //Polearm
	 if (sec_obj->value[0] == WEAPON_POLEARM)
	   sec_chance += pole_mod;

//MULTIPLIERS
	 //skill with cleave: (get a bonus over 85 here)
	 sec_chance = cleave_skill * sec_chance / cleave_med;

	 //Skill with current weapon
	 sec_skill = get_weapon_skill_obj(ch, sec_obj);
	 sec_chance =  sec_skill * sec_chance / skill_med;

	 //Opponents abilit with a shield
	 tar_skill = 75 + (get_skill(victim, gsn_shield_block) / 4);
	 sec_chance =  skill_med * prim_chance / tar_skill;

	 //check for blessing
	 if ( (ch->alignment > GOOD_THRESH)
	      && (IS_OBJ_STAT(sec_obj,ITEM_BLESS)) )
	   sec_chance = bless_mod * sec_chance / 100;
	 else if ( (ch->alignment < EVIL_THRESH)
		   && (IS_OBJ_STAT(sec_obj,ITEM_EVIL)) )
	   sec_chance = bless_mod * sec_chance / 100;
	 //DEBUG
	 //     sendf(ch, "`#Sec: %d\n\r``", sec_chance);
     //We give a bonus to minotarus.
     if (ch->race == race_lookup("minotaur"))
       chance += sec_chance / 2;
     else
       chance += sec_chance / 3;
       }//end SEC weapon.

     //     sendf(ch, "`#Total Chance before target: %d\n\r``", chance);
   }//end calculation of the chance.

//Now we deal with quality of the target shield
 chance -= (wep_lvl_mod * tar_obj->level / 100);

//magic items are very fragile
 if ( (IS_OBJ_STAT(tar_obj,ITEM_GLOW) || IS_OBJ_STAT(tar_obj,ITEM_HUM) || tar_obj->enchanted ) )
   chance += magic_mod;

//we floor the chance
 chance = URANGE(5, chance, 80);

 //DEBUG
 //     sendf(ch, "`#Chance after target: %d\n\r``", chance);

//blessed itmes are harder to break
 if ( (victim->alignment > GOOD_THRESH) && IS_OBJ_STAT(tar_obj,ITEM_BLESS) )
   chance = bless_tar_mod * chance / 100;
 else if ( (victim->alignment < EVIL_THRESH) && (IS_OBJ_STAT(tar_obj,ITEM_EVIL)) )
   chance = bless_tar_mod * chance / 100;

 //Final check, burproof items cannot be destroyed.
 if (IS_OBJ_STAT(tar_obj, ITEM_BURN_PROOF)
     || IS_OBJ_STAT(tar_obj, ITEM_HAS_OWNER))
   chance = 0;

 //DEBUG
 //     sendf(ch, "`#Final Chance: %d\n\r``", chance);

//DONE now we can try to cleave..

     if (number_percent() < chance)
       {
	 WAIT_STATE2( ch, skill_table[gsn_shield_cleave].beats );
	 shield_cleave( ch, victim );
	 check_improve(ch,gsn_shield_cleave,TRUE,1);
       }
     else
       {
	 WAIT_STATE2(ch,skill_table[gsn_shield_cleave].beats);
	 act("You fail to cleave $N's shield.",ch,NULL,victim,TO_CHAR);
	 act("$n tries to cleave your shield, but fails.",ch,NULL,victim,TO_VICT);
	 act("$n tries to cleave $N's shield, but fails.",ch,NULL,victim,TO_NOTVICT);
	 check_improve(ch,gsn_shield_cleave,FALSE,1);
       }
}

/* approach function used for pry and plant to ease the process */
/* sets the gen_approach which is cleared when you leave the person */
CHAR_DATA* approach(CHAR_DATA* ch, char* argument, int dur, int tries){
  CHAR_DATA* victim;
  AFFECT_DATA* paf, af;


  if ( (victim = get_char_room(ch, NULL, argument)) == NULL)
    return NULL;

  if ( (paf = affect_find(ch->affected, gen_approach)) != NULL){
    if (str_cmp(victim->name, paf->string)){
      paf->modifier = tries;
      paf->level = TRUE;
    }
    string_to_affect(paf, victim->name );
  }
  else{
    af.type = gen_approach;
    af.duration = dur;
    af.level = TRUE; //approached
    af.where = TO_NONE;
    af.bitvector = 0;
    af.location = APPLY_NONE;
/* modifier stores how many attempts we can make at pry/plant before waking */
    af.modifier = tries;
    paf = affect_to_char(ch, &af);
    string_to_affect(paf, victim->name);
  }
  return victim;
}

/* checks if the victim that ch has approached is here*/
CHAR_DATA* has_approached(CHAR_DATA* ch){
  AFFECT_DATA* paf;
  CHAR_DATA* vch;

  if ( (paf = affect_find(ch->affected, gen_approach)) == NULL)
    return NULL;

  if (!paf->has_string || IS_NULLSTR(paf->string))
    return NULL;

  if ( (vch = get_char_room(ch, NULL, paf->string)) == NULL){
    paf->level = FALSE;
    return NULL;
  }
  if (!paf->level )
    return NULL;
  return vch;
}

void do_approach( CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim;
  const int dur = 3;	//duration of approach
  const int tries = (ch->level / 10);

  if (get_skill(ch,gsn_pry) < 1
      && get_skill(ch,gsn_plant) < 1
      && get_skill(ch, gsn_telepathy < 1)){
    send_to_char("You're in the wrong profession.\n\r",ch);
    return;
  }

  if (IS_NULLSTR(argument)){
    send_to_char("Approach who?\n\r", ch);
    return;
  }

  if ( (victim = approach(ch, argument, dur, tries)) == NULL){
    send_to_char("They aren't here.\n\r",ch);
  }
  else if (ch == victim){
    send_to_char("Yes, approaching yourself has truly amazing uses!\n\r", ch);
  }
  else{
    act("You approach $N.", ch, NULL, victim, TO_CHAR);
    if (!IS_AFFECTED(ch, AFF_SNEAK)){
      act("$n approaches $N.", ch, NULL, victim, TO_ROOM);
    }
  }
}

/* Viri: pry function for thieves to get things from knocked out victims*
 * there is two stages:							*
 * 1) approach <victim>:approaches the victim in order to start prying	*
 * 2) pry <weight> <object>: tries to remove the object based on guess of*
 * the weight.								*
 * approach, and has_approached are both called to check for approaches */
void do_pry( CHAR_DATA *ch, char *argument ){
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  AFFECT_DATA* paf;

  char buf[MSL], arg1[MIL];

  int percent;
  const int max_weights = 4;
  /*			     0-1     1-10    10-25    >25   */
  const char* weights[] = {"weightless", "light", "medium", "heavy" };
  int weight = -1;		//weight guessed
  int guess = 0;		//how far off from real weight in %
  int i;

  bool fSuccess;

  argument = one_argument( argument, arg1 );
/* skill check */
  if ((percent = get_skill(ch,gsn_pry) / 2) == 0){
    send_to_char("You're in the wrong profession.\n\r",ch);
    return;
  }
/* check for approach */
  if ( (victim = has_approached(ch)) == NULL){
    send_to_char("You must first approach your victim.\n\r", ch);
    return;
  }
  if ( victim == ch ){
    send_to_char( "That's pointless.\n\r", ch );
    return;
  }
  /* get the paf */
  if ( (paf = affect_find(ch->affected, gen_approach)) == NULL){
    bug("do_pry: paf with approach not found!.", 0);
    send_to_char("You must first approach your victim.\n\r", ch);
    return;
  }
/* check for arguments */
  if (IS_NULLSTR(arg1) || IS_NULLSTR(argument)){
    send_to_char( "How heavy, and what object?\n\r", ch );
    send_to_char("Weight must be one of: ", ch );
    for (i = 0; i < max_weights; i++){
      sendf( ch, "%s ", weights[i]);
    }
    send_to_char(".\n\r", ch);
    return;
  }
/* check which weight we picked */
  for (i = 0; i < max_weights; i++){
    if (!str_prefix(arg1, weights[i])){
      weight = i;
      break;
    }
  }
  if ( weight  < 0){
    send_to_char("Weight must be one of: ", ch );
    for (i = 0; i < max_weights; i++){
      sendf( ch, "%s ", weights[i]);
    }
    send_to_char(".\n\r", ch);
    return;
  }

  /* pk check */
  if (is_safe(ch,victim))
    return;
  /* sleep check */
  if ( victim->position != POS_SLEEPING){
    send_to_char( "Not while they're wide awake.\n\r",ch);
    return;
  }
  if ( ( obj = get_obj_wear(victim, argument, ch) ) == NULL ){
    if (( obj = get_obj_carry(victim, argument, ch)) == NULL){
      send_to_char("You do not see that item on them.\n\r",ch);
      return;
    }
  }
  /* object checks */
  if (obj->wear_loc == WEAR_NONE){
    send_to_char("Why don't you just try stealing instead.\n\r",ch);
    return;
  }
  if (CAN_WEAR(obj, ITEM_WEAR_TATTOO)){
    send_to_char("You can't pry a tattoo.\n\r",ch);
    return;
  }
  if ( !can_drop_obj( victim, obj )
       || IS_SET(obj->wear_flags, ITEM_HAS_OWNER)
       || IS_SET(obj->extra_flags, ITEM_INVENTORY)
       || IS_SET(obj->extra_flags, ITEM_NOREMOVE)){
    send_to_char( "You can't pry it away.\n\r", ch );
    return;
  }
  if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) ){
    send_to_char( "You have your hands full.\n\r", ch );
    return;
  }
  if ( ch->carry_weight +get_obj_weight_char(ch, obj ) > can_carry_w( ch ) ){
    send_to_char( "You can't carry that much weight.\n\r", ch );
    return;
  }
  /* lag */
  WAIT_STATE2( ch, skill_table[gsn_pry].beats );
  set_delay(ch, NULL);

  /* standard skill calculation */
  if (ch->race == race_lookup("halfling") )
    percent += 5;
  percent += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
  percent += URANGE(-10, (ch->level - victim->level), 10);
  percent += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);

  /* weight bonus calculation						*
   * the way this work is as follows:					*
   * If the guess was within 10% of true weight, we boost the chance	*
   * otherwise we decrease it linearly by the 'incorrectness' of the	*
   * guess.								*/


/* get the weight of the object by weight cateogry */
  if (obj->weight <= 10)		/* WEIGHTLESS:	0  - 1  */
    guess = 0;
  else if (obj->weight <= 100)	/*LIGHT:	1  - 10 */
    guess = 1;
  else if (obj->weight <= 250)	/*MEDIUM:	10 - 25 */
    guess = 2;
  else				/*HEAVY:	25 and up */
    guess = 3;

/* debug
  sendf(ch, "chance before: %d\n\r", percent);
*/
  /* bonus for exact guesses */
  if (guess == weight )
    percent = 5 * percent / 3;
  else if (!IS_NPC(ch))
    percent -= abs(guess - weight) * 15;

/* debug
  sendf(ch, "chance after: %d\n\r", percent);
*/
  if (ch->level + 8 < victim->level)
    percent = 0;
  else if (IS_NPC(ch))
    percent = 100;
  else
    percent = URANGE(0, percent, 90);

  if (number_percent() < percent){
    if (obj->wear_loc != WEAR_NONE)
      unequip_char(victim, obj);
    obj_from_char( obj );
    obj_to_char( obj, ch );
    send_to_char( "Got it!\n\r", ch );
    if (!IS_AFFECTED(ch, AFF_SNEAK)){
      act("$n pries $p away from $N.", ch, obj, victim, TO_ROOM);
/* THEFT CRIME CHECK */
      if (ch->in_room && is_crime(ch->in_room, CRIME_THEFT, victim)){
	set_crime(ch, victim, ch->in_room->area, CRIME_THEFT);
      }
    }
    check_improve(ch,gsn_pry,TRUE,1);
    fSuccess = TRUE;
  }
  else{
    send_to_char("You fumble the attempt.\n\r", ch);
    if (!IS_AFFECTED(ch, AFF_SNEAK)){
      act("$n fails to pry $p away from $N.", ch, obj, victim, TO_ROOM);
/* THEFT CRIME CHECK */
      if (ch->in_room && is_crime(ch->in_room, CRIME_THEFT, victim)){
	set_crime(ch, victim, ch->in_room->area, CRIME_THEFT);
      }
    }
    check_improve(ch,gsn_pry,FALSE,1);
    fSuccess = FALSE;
  }

/* update for imms */
  if (!IS_NPC(victim)){
    sprintf(buf,"$N tried to steal from %s and %s.",
	    victim->name, fSuccess ? "succeeded" : "failed");
    wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
  }

  /* now we check if this try woke a person up */
  paf->modifier -= (fSuccess ? 1 : 2);
  if (paf->modifier > 0)
    return;

  /* wake up and other bad things */
  affect_strip(ch, gen_approach);
  send_to_char( "Oops.\n\r", ch );
  affect_strip(ch,gsn_sneak);

  switch(number_range(0,3)){
  case 0 :
    sprintf( buf, "%s is a lousy thief!", PERS(ch,victim));
    break;
  case 1 :
    sprintf( buf, "%s couldn't rob %s way out of a paper bag!",PERS(ch,victim),(ch->sex == 2) ? "her" : "his");
    break;
  case 2 :
    sprintf( buf,"%s tried to rob me!",PERS(ch,victim));
    break;
  case 3 :
    sprintf(buf,"Keep your hands out of there, %s!",PERS(ch,victim));
    break;
  }
  a_yell(ch,victim);
  if ( IS_AFFECTED(victim, AFF_SLEEP) ){
    REMOVE_BIT(victim->affected_by,AFF_SLEEP);
    affect_strip( victim, gsn_sleep );
  }
  victim->position = POS_STANDING;
  act( "$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT    );
  act( "$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
  act("$n wakes and stands up.",victim,NULL,NULL,TO_ROOM);
  if (IS_NPC(victim)){
    REMOVE_BIT(victim->comm,COMM_NOCHANNELS);
    REMOVE_BIT(victim->comm,COMM_NOYELL);
    j_yell(victim,buf);
    SET_BIT(victim->comm,COMM_NOYELL);
    SET_BIT(victim->comm,COMM_NOCHANNELS);
    multi_hit(victim,ch,TYPE_UNDEFINED);
  }
  else
    j_yell(victim, buf );
}


/* Viri: swap function for thieves to trade things from knocked out victims*
 * there is two stages:							*
 * 1) approach <victim>:approaches the victim in order to start prying	*
 * 2) plant <object> <object>: tries to trade two objects, if weights are*
 * similiar enough chance is 100					*
 * or									*
 * 2) plant <object> [secondary]: tries to put object in normal or sec. *
 * location								*
 * approach, and has_approached are both called to check for approaches */
void do_plant( CHAR_DATA *ch, char *argument ){
  CHAR_DATA *victim;
  OBJ_DATA *obj = NULL, *vobj;
  AFFECT_DATA* paf;

  char buf[MSL], arg1[MIL];

  int percent;
  int guess = 0;		//how far off from real weight in %
  int w_loc = 0;
  bool fSuccess;
  bool fPlant = FALSE;		//are we planting or switching
  bool fSec = FALSE;		//if planting, which spot

  argument = one_argument( argument, arg1 );
/* skill check */
  if ((percent = get_skill(ch,gsn_plant) / 3) == 0){
    send_to_char("You're in the wrong profession.\n\r",ch);
    return;
  }
/* check for approach */
  if ( (victim = has_approached(ch)) == NULL){
    send_to_char("You must first approach your victim.\n\r", ch);
    return;
  }
  /* get the paf */
  if ( (paf = affect_find(ch->affected, gen_approach)) == NULL){
    bug("do_swap: paf with approach not found!.", 0);
    send_to_char("You must first approach your victim.\n\r", ch);
    return;
  }
  if (IS_NPC(victim)
      && (victim->leader || victim->master || IS_AFFECTED(victim, AFF_CHARM))
      ){
    act("$n doesn't allow to plant anything.", ch, NULL, victim, TO_CHAR );
    return;
  }
/* check for arguments */
  if (IS_NULLSTR(arg1)){
    send_to_char( "Plant or swap which of yours objects?\n\r", ch );
    return;
  }
  if (IS_NULLSTR(argument) || ( fSec = !str_prefix(argument, "secondary"))){
    fPlant = TRUE;
  }
  if ( victim == ch ){
    send_to_char( "That's pointless.\n\r", ch );
    return;
  }

  /* pk check */
  if (is_safe(ch,victim))
    return;
  /* sleep check */
  if ( victim->position != POS_SLEEPING){
    send_to_char( "Not while they're wide awake.\n\r",ch);
    return;
  }
  /* vobj Is what WE have, obj is what victim has */
  if (( vobj = get_obj_carry(ch, arg1, ch)) == NULL){
    sendf(ch, "You do not seem to be carrying %s.\n\r", arg1);
    return;
  }
  if (!fPlant){
    if ( (obj = get_obj_wear(victim, argument, ch) ) == NULL ){
      sendf(ch, "%s does not seem to be wearing %s.\n\r", PERS(victim, ch), argument);
      return;
    }
  }
  /* object checks */
  if (vobj->wear_loc != WEAR_NONE){
    send_to_char("You cannot be wearing the item you are switching.\n\r",ch);
    return;
  }
  if ( !fPlant && (w_loc = obj->wear_loc) == WEAR_NONE){
    send_to_char("The item you are about to switch with must be worn.\n\r",ch);
    return;
  }

  if (fPlant && (w_loc = wear_loc( vobj->wear_flags, fSec + 1)) == NO_FLAG){
    if (fSec){
      act("$p seems to not if on that spot.", ch, vobj, victim, TO_CHAR);
    }
    else{
      act("$p seems to not if anywhere on $N.", ch, vobj, victim, TO_CHAR);
    }
    return;
  }
  /* check for planting on taken wear loc */
  if (fPlant && (obj = get_eq_char(victim, w_loc)) != NULL){
    /* we turn this into a regular "swap" */
    fPlant = FALSE;
    fSec = FALSE;
  }
  if ( (!fPlant && CAN_WEAR(obj, ITEM_WEAR_TATTOO))
       || CAN_WEAR(vobj, ITEM_WEAR_TATTOO)){
    send_to_char("You can't switch a tattoo.\n\r",ch);
    return;
  }
  /* check wear locations */
  if (!fPlant){
    w_loc = obj->wear_loc;
    if (!CAN_WEAR(vobj, wear_bit( w_loc ))){
      act("$p cannot be worn in the same place as $P.", ch, vobj, obj, TO_CHAR);
      return;
    }
    if ( !can_drop_obj( victim, obj )
	 || IS_SET(obj->wear_flags, ITEM_HAS_OWNER)
	 || IS_SET(obj->extra_flags, ITEM_INVENTORY)
	 || IS_SET(obj->extra_flags, ITEM_NOREMOVE)
	 || IS_SET(obj->extra_flags, ITEM_NODROP)){
      act("$p seems to resist any attempt of moving it.", ch, obj, NULL, TO_CHAR);
      return;
    }
  }
  if (!can_drop_obj( ch, vobj )
      || IS_SET(vobj->wear_flags, ITEM_HAS_OWNER)
      || IS_SET(vobj->extra_flags, ITEM_INVENTORY)
      || IS_SET(vobj->extra_flags, ITEM_NOREMOVE)
      || IS_SET(vobj->extra_flags, ITEM_NODROP)){
    act("$p seems to resist any attempt of moving it.", ch, vobj, NULL, TO_CHAR);
    return;
  }
  if (!fPlant && obj && !can_take(ch, obj)){
    act("You cannot touch $p safely!", ch, obj, NULL, TO_CHAR);
    return;
  }
  if (vobj && !can_take(victim, vobj)){
    act("$N cannot touch $p safely!", ch, vobj, victim, TO_CHAR);
    return;
  }
  if ( !fPlant && ch->carry_weight + get_obj_weight_char(ch, obj ) > can_carry_w( ch ) ){
    send_to_char( "You can't carry that much weight.\n\r", ch );
    return;
  }
  /* lag */
  WAIT_STATE2( ch, skill_table[gsn_plant].beats );
  set_delay(ch, NULL);

  /* standard skill calculation */
  if (ch->race == race_lookup("halfling") )
    percent += 5;
  percent += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
  percent += URANGE(-10, (ch->level - victim->level), 10);
  percent += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);

  /* weight bonus calculation						*
   * the way this work is as follows:					*
   * If the items were within 10% of their weights, chance is 100	*
   * otherwise we decrease it linearly by the 'incorrectness' of the	*
   * guess.								*/
  if (!fPlant){
/* debug
    sendf(ch, "chance: %d, vweight: %d, oweight: %d\n\r",
	  percent, vobj->weight, obj->weight);
*/
    guess = 100 * abs(obj->weight- vobj->weight) / UMAX(1, obj->weight);
    /* bonus for close guesses */
    if (guess <= 10)
      percent = 100;
    else if (!IS_NPC(ch))
      percent -= guess * percent / 20;
  }
  else
    percent += 25;

/* debug
  sendf(ch, "chance after: %d\n\r", percent);
*/
  if (ch->level + 8 < victim->level)
    percent = 0;
  else if (IS_NPC(ch))
    percent = 100;
  else
    percent = URANGE(0, percent, 100);

  if (number_percent() < percent){
    /* try to replace items */
    if (!fPlant){
      if (obj->wear_loc != WEAR_NONE)
	unequip_char(victim, obj);
      /* check if it was removed */
      if (obj->wear_loc != WEAR_NONE){
	act("$p did not even budge!", ch, obj, NULL, TO_CHAR);
	return;
      }
    }
    /* try to wear the replaement */
    obj_from_char( vobj );
    obj_to_char( vobj, victim );
    if (fPlant)
      wear_obj(victim, vobj, FALSE, FALSE);
    else
      equip_char(victim, vobj, w_loc);
    if (vobj->wear_loc != w_loc){
      act("For some reason you cannot fit $p onto $N.", ch, vobj, victim, TO_CHAR);
      if (vobj->wear_loc != WEAR_NONE)
	unequip_char(victim, vobj);
      /* clean up */
      obj_from_char( vobj );
      obj_to_char( vobj, ch );
      if (!fPlant)
	equip_char(victim, obj, w_loc);
      return;
    }
    /* move the replaced object to ch */
    if (!fPlant){
      obj_from_char( obj );
      obj_to_char( obj, ch);
      act("You swap $p for $P.", ch, vobj, obj, TO_CHAR);
      if (!IS_AFFECTED(ch, AFF_SNEAK))
	act("$n swaps $p for $P.", ch, vobj, obj, TO_ROOM);
    }
    else{
      act("You plant $p on $N.", ch, vobj, victim, TO_CHAR);
      if (!IS_AFFECTED(ch, AFF_SNEAK))
	act("$n plants $p on $N.", ch, vobj, victim, TO_ROOM);
    }
    check_improve(ch,gsn_plant,TRUE,1);
    fSuccess = TRUE;
  }
  else{
    send_to_char("You fumble the attempt.\n\r", ch);
    if (!IS_AFFECTED(ch, AFF_SNEAK))
      act("$n fails to swap $p.", ch, vobj, victim, TO_ROOM);
    check_improve(ch,gsn_plant,FALSE,1);
    fSuccess = FALSE;
  }

/* update for imms */
  if (!IS_NPC(victim)){
    sprintf(buf,"$N tried to swap something on %s and %s.",
	    victim->name, fSuccess ? "succeeded" : "failed");
    wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
  }

  /* now we check if this try woke a person up */
  if (percent >= 100)
    paf->modifier -= (fSuccess ? 1 : 2);
  if (paf->modifier > 0)
    return;

  /* wake up and other bad things */
  affect_strip(ch, gen_approach);
  send_to_char( "Oops.\n\r", ch );
  affect_strip(ch,gsn_sneak);

  switch(number_range(0,3)){
  case 0 :
    sprintf( buf, "%s is a lousy thief!", PERS(ch,victim));
    break;
  case 1 :
    sprintf( buf, "%s couldn't rob %s way out of a paper bag!",PERS(ch,victim),(ch->sex == 2) ? "her" : "his");
    break;
  case 2 :
    sprintf( buf,"%s tried to rob me!",PERS(ch,victim));
    break;
  case 3 :
    sprintf(buf,"Keep your hands out of there, %s!",PERS(ch,victim));
    break;
  }
  a_yell(ch,victim);
  if ( IS_AFFECTED(victim, AFF_SLEEP) ){
    REMOVE_BIT(victim->affected_by,AFF_SLEEP);
    affect_strip( victim, gsn_sleep );
  }
  victim->position = POS_STANDING;
  act( "$n tried to swap something of yours.\n\r", ch, NULL, victim, TO_VICT    );
  act( "$n tried to swap something on $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
  act("$n wakes and stands up.",victim,NULL,NULL,TO_ROOM);
  if (IS_NPC(victim))
    multi_hit(victim,ch,TYPE_UNDEFINED);
  else
    j_yell(victim, buf );
}

void do_gag( CHAR_DATA *ch, char *argument)
{
  char arg[MIL];
  CHAR_DATA *victim;
  AFFECT_DATA af;
  int chance;
  one_argument(argument,arg);

  if ((chance = get_skill(ch,gsn_gag)) <= 1 ){
    send_to_char("You do not know where to start.\n\r",ch);
    return;
  }
  if (arg[0] == '\0'){
    send_to_char("Gag who?\n\r",ch);
    return;
  }
  if (ch->fighting != NULL){
    send_to_char("You can't do that while fighting.\n\r",ch);
    return;
  }
  if ((victim = get_char_room(ch, NULL,arg)) == NULL){
    send_to_char("They aren't here.\n\r",ch);
    return;
  }
  if (victim->position > POS_SLEEPING){
    send_to_char("Kind of tough when they're awake don't you think.\n\r",ch);
    return;
  }
  if (victim == ch){
    send_to_char("Why don't you just shut up for a minute?\n\r", ch);
    return;
  }
  if (is_safe(ch,victim))
      return;
  if (ch->race == race_lookup("halfling"))
    chance += 5;
  chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
  chance += URANGE(-10, (ch->level - victim->level), 10);
  chance += 2*(get_curr_stat(victim,STAT_LUCK) - get_curr_stat(ch,STAT_LUCK));
  WAIT_STATE2(ch,skill_table[gsn_gag].beats);
  a_yell(ch,victim);

  act("You put a gag on $N and hope it holds.", ch, NULL, victim, TO_CHAR);
  act("$n places a gag over $N's mouth.", ch, NULL, victim, TO_ROOM);

  af.type			= gsn_gag;
  af.level			= ch->level;
  af.duration		= number_range(1,2);
  af.location			= APPLY_NONE;
  af.bitvector		= 0;
  affect_to_char(victim,&af);
  check_improve(ch,gsn_gag,TRUE,1);
}


void do_tame(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MIL];
    int chance;
    one_argument(argument,arg);
    if ((chance = get_skill(ch, gsn_tame)) == 0)
    {
	send_to_char("You don't have the skills for that.\n\r",ch);
	return;
    }
    if (arg[0] == '\0')
    {
        send_to_char("Tame who?\n\r",ch);
        return;
    }
    if ( (victim = get_char_room(ch, NULL,arg)) == NULL)
    {
        send_to_char("They're not here.\n\r",ch);
        return;
    }
    if (IS_NPC(ch))
    {
        send_to_char("You're a mob!",ch);
        return;
    }
    if (!IS_NPC(victim))
    {
        send_to_char("How do you tame players?",ch);
        return;
    }
    if (!IS_SET(victim->act,ACT_AGGRESSIVE))
    {
        sendf(ch, "%s does not have an aggressive nature.", PERS(victim,ch));
        return;
    }
    if (IS_NPC(victim) && IS_SET(victim->act,ACT_TOO_BIG) )
    {
        sendf(ch, "%s is too wild for you to attempt such an act.\n\r", PERS(victim,ch));
	return;
    }
    if (IS_WEREBEAST(ch->race))
	chance += 10;
    WAIT_STATE2( ch, skill_table[gsn_tame].beats );
    if (number_percent() < chance + UMIN(10,5*(ch->level - victim->level)))
    {
        REMOVE_BIT(victim->act,ACT_AGGRESSIVE);
        SET_BIT(victim->affected_by,AFF_CALM);
        act_new("You calm down.",ch,NULL,victim,TO_VICT,POS_DEAD);
        act("You tame $N.",ch,NULL,victim,TO_CHAR);
        act("$n tames $N.",ch,NULL,victim,TO_NOTVICT);
        stop_fighting(victim,TRUE);
        check_improve(ch,gsn_tame,TRUE,1);
    }
    else
    {
        act_new("You failed to tame $N.",ch,NULL,victim,TO_CHAR,POS_DEAD);
        act("$n tries to tame $N but fails.",ch,NULL,victim,TO_NOTVICT);
        act("$n tries to tame you but fails.",ch,NULL,victim,TO_VICT);
        check_improve(ch,gsn_tame,FALSE,1);
    }
}

void do_camp( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int chance;
    int np = number_percent();
    if ((chance = get_skill(ch, gsn_camp)) == 0)
    {
	send_to_char("You don't know how to make camp.\n\r",ch);
	return;
    }
    if (ch->fighting != NULL)
    {
	send_to_char("You can't camp while you're fighting!\n\r",ch);
	return;
    }
    if (is_affected(ch, gsn_camp))
    {
	send_to_char("You are already in camp.\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_insomnia))
    {
        send_to_char("You cannot manage to sleep due to the insomnia.\n\r",ch);
        return;
    }
    if (is_fight_delay(ch,15))
    {
	send_to_char("You are too worked up to start camping.\n\r",ch);
	return;
    }
    if (ch->in_room->sector_type != SECT_FOREST && ch->in_room->sector_type != SECT_HILLS
    && ch->in_room->sector_type != SECT_MOUNTAIN && ch->in_room->sector_type != SECT_FIELD)
    {
        if (ch->in_room->sector_type == SECT_AIR || ch->in_room->sector_type == SECT_DESERT)
        {
            send_to_char("You can't camp here!\n\r",ch);
            return;
        }
        else
        {
            send_to_char("You don't see any place suitable to set up a camp.\n\r",ch);
            return;
        }
    }
    if (ch->mana < 20)
    {
	send_to_char("Rest up some energy before you set up camp.\n\r",ch);
	return;
    }
    WAIT_STATE(ch,skill_table[gsn_camp].beats);
    act("You attempt to set up a camp.",ch,NULL,NULL,TO_CHAR);
    act("$n attempts to set up a camp.",ch,NULL,NULL,TO_ROOM);
    ch->mana -= 20;
    if ( UMAX(1,np - 2*(get_curr_stat(ch,STAT_LUCK)-16)) < chance)
    {
        af.type             = gsn_camp;
        af.level            = ch->level;
        af.duration         = 3;
        af.location         = APPLY_NONE;
        af.modifier         = 0;
        af.bitvector        = AFF_SLEEP;
        affect_to_char(ch,&af);
	send_to_char("You start dozing off...\n\r",ch);
	do_sleep(ch,"");
        check_improve(ch,gsn_camp,TRUE,1);
    }
    else
    {
	send_to_char("Your camp looks like a disaster, start over again.\n\r", ch);
	check_improve(ch,gsn_camp,FALSE,2);
    }
}

void do_ranger_staff( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *staff;
    AFFECT_DATA *paf;
    int chance;
    if ((chance = get_skill(ch, gsn_ranger_staff)) == 0)
    {
	send_to_char("You don't know how to make a ranger staff.\n\r",ch);
	return;
    }
    if (ch->mana < 50)
    {
	send_to_char("You don't have enough energy to make a staff.\n\r",ch);
	return;
    }
    WAIT_STATE(ch,skill_table[gsn_ranger_staff].beats);
    if ( number_percent( ) < chance)
    {
	check_improve(ch,gsn_ranger_staff,TRUE,1);
	ch->mana -= 50;
        staff = create_object( get_obj_index( OBJ_VNUM_RANGER_STAFF ), 0);
        staff->value[1] = number_range(ch->level -6, ch->level)/3 + 2;
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
        paf->type       = gsn_ranger_staff;
        paf->level      = ch->level;
        paf->duration   = -1;
        paf->location   = APPLY_HITROLL;
        paf->modifier   = ch->level/5;
        paf->bitvector  = 0;
        paf->next       = staff->affected;
        staff->affected   = paf;
        paf = new_affect();
        paf->type       = gsn_ranger_staff;
        paf->level      = ch->level;
        paf->duration   = -1;
        paf->location   = APPLY_DAMROLL;
        paf->modifier   = ch->level/5;
        paf->bitvector  = 0;
        paf->next       = staff->affected;
        staff->affected   = paf;
        obj_to_char(staff, ch);
        act( "You create $p.", ch, staff, NULL, TO_CHAR );
        act( "$n creates $p.", ch, staff, NULL, TO_ROOM );
    }
    else
    {
	ch->mana -= 25;
	check_improve(ch,gsn_ranger_staff,FALSE,2);
	send_to_char("You failed to make a ranger staff.\n\r",ch);
    }
}

void throw(CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj, int throw, bool fThief, bool fAutoMiss){
  CHAR_DATA* dam_ch = ch, *dam_victim = victim;
  int dam_type = DAM_PIERCE;
  int dam = 0;
  int chance = 0;
  int hit = 0;

  bool fPoison = FALSE;
  char buf[MSL];
  char* msg;

  if (!ch || !victim || !obj)
    return;

/* We calculate lump damage for all the throws/ammo */
  while (throw-- > 0 && obj->value[0]-- > 0){
    chance = get_skill(ch,gsn_throw);
    chance = 4 * chance / 5;
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    chance += GET_AC2(victim,AC_BASH) / 40;

    if (fAutoMiss)
      chance = 0;

    /* if hit we add up the damage */
    if (number_percent() > chance){
      check_improve(ch, gsn_throw, FALSE, 2);
      continue;
    }
    else
      check_improve(ch, gsn_throw, TRUE, 2);

    hit++;

    /* hit, we add up damage */
    dam += dice(obj->value[1],obj->value[2]);
    if (fThief)
      dam += GET_DAMROLL(ch);

  }//END WHILE

  /* show throw messages */
  if (hit > 9)
    msg = "a cloud";
  else if (hit > 4)
    msg = "a mass";
  else if (hit > 3)
    msg = "several";
  else if (hit > 2)
    msg = "a few";
  else if (hit > 1)
    msg = "a couple";
  else if (hit > 0){
      msg = "one";
  }
  else
    msg = "none";

  sprintf(buf, "You throw your $p and hit $N with %s of them.", msg);
  act(buf, ch, obj, victim, TO_CHAR);
  sprintf(buf, "$n throws $s $p and hits $N with %s of them.", msg);
  act(buf, ch, obj, victim, TO_NOTVICT);
  sprintf(buf, "$n throws $s $p and hits you with %s of them.", msg);
  act(buf, ch, obj, victim, TO_VICT);

  if (victim->class == gcn_blademaster && is_affected(victim, gsn_iron_curtain)
      && get_eq_char(victim, WEAR_SECONDARY) != NULL){
    act("$n deflects the projectile!", victim, NULL, NULL, TO_ROOM);
    send_to_char("You deflect the projectile!\n\r", victim);
    dam_victim = ch;
    dam_ch = victim;
  }

  /* check for poison and such */
  if (dam_victim && IS_WEAPON_STAT(obj,WEAPON_WOUNDS)){
    wound_effect( dam_ch, dam_victim, obj->level + 3, 0);
  }
  if ( IS_WEAPON_STAT(obj,WEAPON_POISON) ){
    AFFECT_DATA* paf, *poison, af;
    int level = 1;
    /* check for poison affect */
    if ( (poison = affect_find(obj->affected, gsn_poison)) == NULL)
      level = UMAX(obj->level,1);
    else
      level = poison->level;

    /* check for save */
    if (!saves_spell(level, dam_victim, DAM_POISON,
		     skill_table[gsn_poison].spell_type)){

      fPoison = TRUE;
      af.where     = TO_AFFECTS;
      af.type      = gsn_poison;
      af.level     = 3 * level / 4;
      af.duration  = IS_AFFECTED(victim, AFF_POISON) ? 1 : level / 10;
      af.location  =  APPLY_STR;
      af.modifier  = IS_AFFECTED(victim, AFF_POISON) ? 0 : -1;
      af.bitvector = AFF_POISON;
      paf = affect_join( dam_victim, &af );
      if (!IS_NPC(ch))
	string_to_affect(paf, dam_ch->name);

      /* cut duration of poison down */
      if (poison != NULL){
	poison->level = UMAX(0,poison->level - 1);
	poison->duration = UMAX(0,poison->duration - 1);
	if (poison->level == 0 || poison->duration == 0){
	  act("The poison on $p has worn off.",ch,obj,NULL,TO_CHAR);
	  affect_strip_obj(obj, gsn_poison);
	}
      }
    }
  }//END POISONED WEAPON

  /* check if was poisoned in throws */

  if (fPoison){
    act_new("You feel poison coursing through your veins.",dam_ch,obj,dam_victim,TO_VICT,POS_DEAD);
    act("$n is poisoned by the venom on $p.",dam_victim,obj,NULL,TO_ROOM);
  }

  if ( IS_SET(dam_victim->vuln_flags, VULN_IRON) && strstr(obj->material,"iron"))
    dam_type = DAM_IRON;
  else if ( IS_SET(dam_victim->vuln_flags, VULN_SILVER) && strstr(obj->material,"silver"))
    dam_type = DAM_SILVER;
  else if ( IS_SET(dam_victim->vuln_flags, VULN_MITHRIL) && strstr(obj->material,"mithril"))
    dam_type = DAM_MITHRIL;
  else if ( IS_SET(dam_victim->vuln_flags, VULN_WOOD) && strstr(obj->material,"wood"))
    dam_type = DAM_WOOD;
  else if ( IS_SET(dam_victim->vuln_flags, VULN_WOOD) && strstr(obj->material,"wood"))
    dam_type = DAM_WOOD;
  else if ( IS_SET(dam_victim->vuln_flags, VULN_COLD)
	    && (strstr(obj->material,"ice") || strstr(obj->material,"cold")))
    dam_type = DAM_COLD;
  else
    dam_type = attack_table[obj->value[3]].damage;

  damage(dam_ch, dam_victim, dam, gsn_throw, dam_type, TRUE);

  if (obj->value[0] < 1){
    act("You depleted your supply of $p.",ch,obj,NULL,TO_CHAR);
    obj_from_char( obj );
    extract_obj( obj );
  }
}

void do_type_throw( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj ){
  bool fThief = FALSE;
  bool fBard  = FALSE;
  bool fNinja = FALSE;

  /* Useless conditional */
  if( fBard != FALSE || fNinja != FALSE )
    fBard = fNinja = FALSE;

  const int lag = skill_table[gsn_throw].beats;
  int throws = (get_skill(ch, gsn_throw) - 60) / 8;
  int i = 1;

  char buf[MSL];

  if (ch->level < obj->level && obj->level < 50){
    send_to_char("That object is beyond your mastery.\n\r",ch);
    return;
  }
  if (ch->fighting != victim && victim->fighting != ch){
    sprintf(buf, "Help! %s is attacking me!",PERS(ch,victim));
    j_yell(victim,buf);
  }
  /* check for right throwing object */
  /* NINJA */
  if (ch->class == class_lookup("ninja")){
    fNinja = TRUE;
    if (obj->cost != 0){
      send_to_char("That is not your type of throwing weapons.\n\r",ch);
      return;
    }
    i = number_range(1, throws);
  }
  /* THIEF */
  if (ch->class == class_lookup("thief")){
    fThief = FALSE;
    if (obj->cost != 1){
      send_to_char("That is not your type of throwing weapons.\n\r",ch);
      return;
    }
    i = 1;
  }
  /* BARD */
  if (ch->class == class_lookup("bard")){
    fBard = FALSE;
    if (obj->cost != 2){
      send_to_char("That is not your type of throwing weapon.\n\r",ch);
      return;
    }
    i = number_range(3, 2 * throws);
  }
  WAIT_STATE2(ch, lag);
  throw(ch, victim, obj, i, fThief, FALSE);
}

void do_std_throw(  CHAR_DATA *ch, char *argument ){
  OBJ_DATA* obj;
  CHAR_DATA* victim;

  bool fThief = ch->class == class_lookup("thief");
  bool fNinja = ch->class == class_lookup("ninja");
  bool fBard = ch->class == class_lookup("Bard");

  if (get_skill(ch, gsn_throw) < 1){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (IS_NULLSTR(argument)){
    if (ch->fighting == NULL){
      send_to_char("But you aren't in combat!\n\r",ch);
      return;
    }
    else
      victim = ch->fighting;
  }
  else if ( (victim = get_char_room(ch, NULL, argument)) == NULL){
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (is_safe(ch,victim))
    return;
/* check for thrown items */
  for (obj = ch->carrying; obj; obj = obj->next_content){
    if (obj->item_type != ITEM_THROW)
      continue;
    if (fNinja && obj->cost != 0)
      continue;
    if (fThief && obj->cost != 1)
      continue;
    if (fBard && obj->cost != 2)
      continue;
    break;
  }
  if (obj == NULL || obj->value[0] < 1){
    send_to_char("You lack anything to throw.\n\r", ch);
    return;
  }
  do_type_throw(ch, victim, obj);
}


void do_throw( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char arg[MIL], arg1[MIL], buf[MSL];
    int chance, dam;
    int dam_type;
    int shield = LAG_SHIELD_NONE;
    if ( ch->class == class_lookup("monk")){
	do_throw_monk(ch, argument);
	return;
    }
    if ( ch->class == class_lookup("ninja")){
	do_std_throw(ch, argument);
	return;
    }
    if ( ch->class == class_lookup("thief")){
	do_std_throw(ch, argument);
	return;
    }
    if ( ch->class == class_lookup("bard")){
	do_std_throw(ch, argument);
	return;
    }
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg1 );
    if ( (chance = get_skill(ch,gsn_throw)) == 0)
    {
        send_to_char("You are not skilled enough to throw.\n\r",ch );
        return;
    }
    if ( arg[0] == '\0' ){
      send_to_char( "Throw what?\n\r",ch);
      return;
    }
    if ((obj = get_obj_carry(ch, arg, ch)) == NULL){
      send_to_char("You are not carrying that.\n\r", ch);
      return;
    }
    if ( arg1[0] == '\0' )
    {
	victim =  ch->fighting;
	if (victim == NULL)
	{
            send_to_char( "You aren't fighting anyone.\n\r", ch );
     	    return;
	}
    }
    else if ( (victim = get_char_room(ch, NULL,arg1)) == NULL)
    {
        send_to_char("They're not here.\n\r",ch);
        return;
    }
    if (is_safe(ch,victim))
	return;
    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        sendf(ch, "%s is your beloved master.\n\r", PERS(victim,ch) );
        return;
    }
    chance = 4 * chance /5;
    chance += get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX);
    chance += (ch->level - victim->level);
    WAIT_STATE2( ch, skill_table[gsn_throw].beats );
    if (obj != NULL && obj->item_type == ITEM_THROW)
      {
	do_type_throw(ch,victim,obj);
	return;
      }
    if (ch->class != class_lookup("ranger")){
      send_to_char("You don't know how to throw that.\n\r",ch);
      return;
    }
    if (!CAN_WEAR(obj, ITEM_WEAR_SHIELD)
	&& !CAN_WEAR(obj, ITEM_WEAR_SECONDARY)
	&& !CAN_WEAR(obj, ITEM_WIELD) ){
      send_to_char("You don't know how to throw that.\n\r",ch);
      return;
    }

    if (!can_drop_obj(ch, obj) || IS_SET(obj->extra_flags, ITEM_NOREMOVE))
    {
	send_to_char("You can't throw it!\n\r",ch);
	return;
    }

    chance += 2*(get_curr_stat(ch,STAT_LUCK) - 16);
    act("You throw $p at $N.",ch,obj,victim,TO_CHAR);
    act("$n throws $p at $N.",ch,obj,victim,TO_NOTVICT);
    act("$n throws $p at you!",ch,obj,victim,TO_VICT);
    obj_from_char( obj );

    if (ch->fighting != victim && victim->fighting != ch)
    {
        sprintf(buf, "Help! %s is attacking me!",PERS(ch,victim));
        j_yell(victim,buf);
    }

    shield = get_lagprot(victim);
    if ( shield == LAG_SHIELD_PROT || shield == LAG_SHIELD_FFIELD)
    {
        act("$p bounces off your shield.",ch,obj,victim,TO_VICT);
        act("$p bounces off $n's shield.",victim,obj,NULL,TO_ROOM);
	check_improve(ch,gsn_throw,TRUE,10);
	damage(ch,victim,0,gsn_throw,DAM_PIERCE,TRUE);
    }
    else if ( number_percent( ) < chance )
    {
      check_improve(ch,gsn_throw,TRUE,0);

      if (victim->class == gcn_blademaster && is_affected(victim, gsn_iron_curtain)
	  && get_eq_char(victim, WEAR_SECONDARY) != NULL){
	CHAR_DATA* temp = victim;
	act("$n deflects the projectile!", victim, NULL, NULL, TO_ROOM);
	send_to_char("You deflect the projectile!\n\r", victim);
	victim = ch;
	ch = temp;
      }

      if (obj->item_type == ITEM_WEAPON)
	{
	    dam = 3 * ch->level / 2 + dice(obj->value[1],obj->value[2]) * number_range(1,3);
	    if ( victim->race == race_lookup("elf") && !str_cmp(obj->material,"iron"))
	    	dam_type = DAM_IRON;
	    else if ( victim->race == race_lookup("drow") && !str_cmp(obj->material,"silver"))
    		dam_type = DAM_SILVER;
	    else if ( victim->race == race_lookup("drow") && !str_cmp(obj->material,"mithril"))
    		dam_type = DAM_MITHRIL;
	    else if ( victim->race == race_lookup("storm") && !str_cmp(obj->material,"wood"))
    		dam_type = DAM_WOOD;
	    else if ( victim->race == race_lookup("fire") && !str_cmp(obj->material,"ice"))
    		dam_type = DAM_COLD;
	    else
            	dam_type = attack_table[obj->value[3]].damage;
	    damage(ch,victim,dam,gsn_throw,dam_type,TRUE);
	}
	else if (obj->item_type == ITEM_ARMOR && CAN_WEAR(obj, ITEM_WEAR_SHIELD))
	{
	    act("$p smashes into $n's face, knocking $m senseless.",victim,obj,NULL,TO_ROOM);
	    act("$p smashes into your face, knocking you senseless.",victim,obj,NULL,TO_CHAR);
	    dam = UMAX(1, obj->level);
	    if ( victim->race == race_lookup("elf") && !str_cmp(obj->material,"iron"))
	    	dam_type = DAM_IRON;
	    else if ( victim->race == race_lookup("drow") && !str_cmp(obj->material,"silver"))
    		dam_type = DAM_SILVER;
	    else if ( victim->race == race_lookup("drow") && !str_cmp(obj->material,"mithril"))
    		dam_type = DAM_MITHRIL;
	    else if ( victim->race == race_lookup("storm") && !str_cmp(obj->material,"wood"))
    		dam_type = DAM_WOOD;
	    else if ( victim->race == race_lookup("fire") && !str_cmp(obj->material,"ice"))
    		dam_type = DAM_COLD;
	    else
		dam_type = DAM_BASH;
	    damage(ch,victim,dam,gsn_throw,dam_type,TRUE);
	    WAIT_STATE2( victim, number_range(1, 2) * PULSE_VIOLENCE);
	}
	else
	{
	    act("$p bounces off $n's head.",victim,obj,NULL,TO_ROOM);
	    act("$p bounces off your head.",victim,obj,NULL,TO_CHAR);
	    dam = obj->level*1.5;
	    if ( victim->race == race_lookup("elf") && !str_cmp(obj->material,"iron"))
	    	dam_type = DAM_IRON;
	    else if ( victim->race == race_lookup("drow") && !str_cmp(obj->material,"silver"))
    		dam_type = DAM_SILVER;
	    else if ( victim->race == race_lookup("drow") && !str_cmp(obj->material,"mithril"))
    		dam_type = DAM_MITHRIL;
	    else if ( victim->race == race_lookup("storm") && !str_cmp(obj->material,"wood"))
    		dam_type = DAM_WOOD;
	    else if ( victim->race == race_lookup("fire") && !str_cmp(obj->material,"ice"))
    		dam_type = DAM_COLD;
	    else
		dam_type = DAM_BASH;
	    WAIT_STATE2( victim, 1 * PULSE_VIOLENCE);
	    damage(ch,victim,dam,gsn_throw,dam_type,TRUE);
	}
    }
    else
    {
	check_improve(ch,gsn_throw,FALSE,2);
	act("$p flies by $n.",victim,obj,NULL,TO_ROOM);
	act("$p flies by you.",victim,obj,NULL,TO_CHAR);
	damage(ch,victim,0,gsn_throw,DAM_NONE,TRUE);
    }
    act("$p falls to the ground.",ch,obj,NULL,TO_ALL);
    obj_to_room( obj, ch->in_room );
}


void do_edge_craft( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj, *throw = NULL;
    char arg[MIL], name[MIL], buf[MIL];
    int chance;
    int skill;

    one_argument( argument, arg );
    if ( (chance = get_skill(ch,gsn_edge_craft)) == 0){
      send_to_char("You don't know where to start.\n\r",ch );
      return;
    }
    else
      skill = UMAX(0, 20 + (chance - 60) * 2);

    if (ch->mana < 40){
      send_to_char("You don't have enough energy to create edged weapons.\n\r",ch);
      return;
    }

    if ( arg[0] == '\0' ){
        send_to_char( "Edge what?\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "You do not have that on you.\n\r", ch );
        return;
    }

    if (obj->item_type != ITEM_TREASURE
	&& obj->item_type != ITEM_KEY
	&& obj->item_type != ITEM_WEAPON
	&& obj->item_type != ITEM_FURNITURE
	&& obj->item_type != ITEM_ARMOR)
    {
        send_to_char("That can't be sharpened into a throwable weapon.\n\r",ch);
        return;
    }

    if (obj->wear_loc != -1){
      send_to_char("The item must be carried to be sharpened.\n\r",ch);
      return;
    }
    ch->mana-=40;

    WAIT_STATE2( ch, skill_table[gsn_edge_craft].beats );

    throw = create_object( get_obj_index(OBJ_VNUM_THROW), ch->level);

    //move material/damage if it is rare
    if (IS_LIMITED(obj)){
      free_string( throw->material );
      throw->material = str_dup(obj->material);
      sprintf( buf, "%s %%s", obj->material );
      if (obj->item_type == ITEM_WEAPON)
	throw->value[3] = obj->value[3];
    }
    else
      sprintf( buf, "%%s");

    if (number_percent() < chance){
      //NINJA
      if (ch->class == class_lookup("ninja")){

	//amount of ammo per set
	throw->value[0] = number_range(skill / 10, skill / 6);
	//first die (second is decided by OLC, usualy 5)
	throw->value[1] = skill / 14;
	//set the name to be used in short/long desc and name
	sprintf( name, buf, "shurikens" );
      }
      //BARD
      else if (ch->class == class_lookup("bard")){
	//amount of ammo per set
	throw->value[0] = 52;
	throw->value[1] = skill / 20;
	throw->cost = 2;
	//set the name to be used in short/long desc and name
	sprintf( name, buf, "cards" );
      }
      //THIEF
      else{
	//amount of ammo per set
	throw->value[0] = number_range(skill / 10, skill / 6);
	//first die (second is decided by OLC, usualy 5)
	throw->value[1] = skill / 10;
	//second die
	throw->value[2] = 10;

	//set the name to be used in short/long desc and name
	sprintf( name, buf, "daggers" );

	throw->cost = 1;

	//set the wound flag
	SET_BIT(throw->value[4], WEAPON_WOUNDS);
      }

      //set name short and long
      sprintf(buf, throw->name, name );
      free_string(throw->name );
      throw->name = strdup( buf );

      sprintf(buf, throw->short_descr, name );
      free_string(throw->short_descr );
      throw->short_descr = strdup( buf );

      sprintf(buf, throw->description, name );
      free_string(throw->description );
      throw->description = strdup(buf );

      throw->level = skill * ch->level / 90;
      act("You sharpen a handful of $p.", ch, throw, NULL, TO_CHAR);
      act("$n sharpens a handful of $p.", ch, throw, NULL, TO_ROOM);
      obj_to_char(throw, ch);
      check_improve(ch,gsn_edge_craft,TRUE,1);
    }
    else
      {
        send_to_char("You failed and destroyed it.\n\r", ch);
	check_improve(ch,gsn_edge_craft,FALSE,2);
      }
    obj_from_char(obj);
    extract_obj(obj);
}

void do_roar( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int chance;
    if ( (chance = get_skill(ch,gsn_roar)) == 0)
    {
        send_to_char("You scream loudly with no effect.\n\r",ch );
        return;
    }
    if ((victim = ch->fighting) == NULL)
    {
	send_to_char("You are not fighting anyone to roar at.\n\r",ch);
	return;
    }
    if (is_affected(victim,gsn_roar))
    {
	sendf(ch, "%s already looks shaken up.\n\r", PERS(victim,ch));
	return;
    }
    chance = 4*chance/5;
    chance += (ch->level - victim->level);
    chance += (ch->size - victim->size)*10;
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    act("You let loose a bestial roar at $N.",ch,NULL,victim,TO_CHAR);
    act("$n lets loose a bestial roar at you.",ch,NULL,victim,TO_VICT);
    act("$n lets loose a bestial roar at $N.",ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE2( ch, skill_table[gsn_roar].beats );
    if (number_percent() < chance)
    {
    	af.type			= gsn_roar;
    	af.level		= ch->level;
    	af.duration		= 1;
    	af.location		= APPLY_STR;
    	af.modifier		= -2;
    	af.bitvector		= 0;
    	affect_to_char(victim,&af);
	act("$n looks shaken up.",victim,NULL,NULL,TO_ROOM);
	act("You feel intimidated by $n's roar.",ch,NULL,victim,TO_VICT);
	check_improve(ch,gsn_roar,TRUE,1);
    }
    else
    {
	act("$N does not seem to be affected by your roar.",ch,NULL,victim,TO_CHAR);
	act("You are not intimidated by $n's roar",ch,NULL,victim,TO_VICT);
	check_improve(ch,gsn_roar,FALSE,2);
    }
}

void do_counterfeit( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj, *clone = NULL;
    char arg[MIL];
    int chance;
    one_argument( argument, arg );
    if ( (chance = get_skill(ch,gsn_counterfeit)) == 0)
    {
        send_to_char("You don't know where to start.\n\r",ch );
        return;
    }
    if (ch->mana < 100)
    {
	send_to_char("Get some rest first.\n\r",ch);
	return;
    }
    if ( arg[0] == '\0' )
    {
        send_to_char( "Counterfeit what?\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "You do not have that on you.\n\r", ch );
        return;
    }
    if (obj->wear_loc != -1)
    {
        send_to_char("The item must be carried.\n\r",ch);
        return;
    }
    if (obj->item_type == ITEM_CABAL)
    {
	send_to_char("You can't counterfeit cabal items!\n\r",ch);
	return;
    }
    if (obj->item_type == ITEM_TRASH)
    {
	send_to_char("There's no point in creating more trash.\n\r",ch);
	return;
    }
    ch->mana -= 100;
    chance = 3 * chance / 5;
    chance += ch->level - obj->level;
    WAIT_STATE2( ch, skill_table[gsn_counterfeit].beats );
    if (number_percent() < chance)
    {
	clone = create_object( get_obj_index(OBJ_VNUM_CLONE), obj->level);
	free_string( clone->name );
	clone->name = str_dup(obj->name);
	free_string( clone->material );
	clone->material = str_dup(obj->material);
	clone->wear_flags = obj->wear_flags;
	clone->weight = obj->weight;
	clone->timer = 100;
	clone->cost = obj->pIndexData->vnum;
	free_string( clone->description );
	clone->description = str_dup(obj->description);
	free_string( clone->short_descr );
	clone->short_descr = str_dup(obj->short_descr);
	if (IS_SET(obj->extra_flags,ITEM_GLOW))
	    SET_BIT(clone->extra_flags,ITEM_GLOW);
	if (IS_SET(obj->extra_flags,ITEM_HUM))
	    SET_BIT(clone->extra_flags,ITEM_HUM);
	if (obj->item_type == ITEM_WEAPON
	    || obj->item_type == ITEM_RANGED
	    || obj->item_type == ITEM_PROJECTILE){
	  clone->value[0] = obj->value[0];
	  clone->value[1] = 1;
	  clone->value[2] = 0;
	  clone->value[3] = obj->value[3];
	  clone->value[4] = 0;
	}
    	act("You counterfeit $p!", ch, obj, NULL, TO_CHAR);
        act("$n counterfeits $p!", ch, obj, NULL, TO_ROOM);
	obj_to_char(clone, ch);
	check_improve(ch,gsn_counterfeit,TRUE,1);
    }
    else
    {
    	act("You failed to counterfeit $p.", ch, obj, NULL, TO_CHAR);
        act("$n failed to counterfeit $p.", ch, obj, NULL, TO_ROOM);
	check_improve(ch,gsn_counterfeit,FALSE,2);
    }
}


void do_healer_staff( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *staff;
    AFFECT_DATA *paf;
    int chance;
    if ((chance = get_skill(ch, gsn_healer_staff)) == 0)
    {
	send_to_char("You don't know how to make a healer staff.\n\r",ch);
	return;
    }
    if (ch->mana < 80)
    {
	send_to_char("You don't have enough energy to make a staff.\n\r",ch);
	return;
    }
    WAIT_STATE2(ch,skill_table[gsn_healer_staff].beats);
    if ( number_percent( ) < chance)
    {
	check_improve(ch,gsn_healer_staff,TRUE,1);
	ch->mana -= 80;
        staff = create_object( get_obj_index( OBJ_VNUM_HEALER_STAFF ), 0);
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
        paf->type       = gsn_healer_staff;
        paf->level      = ch->level;
        paf->duration   = -1;
        paf->location   = APPLY_HIT;
        paf->modifier   = ch->level/2;
        paf->bitvector  = 0;
        paf->next       = staff->affected;
        staff->affected   = paf;
        paf = new_affect();
        paf->type       = gsn_healer_staff;
        paf->level      = ch->level;
        paf->duration   = -1;
        paf->location   = APPLY_MANA;
        paf->modifier   = ch->level/2;
        paf->bitvector  = 0;
        paf->next       = staff->affected;
        staff->affected   = paf;
        obj_to_char(staff, ch);
        act( "You create $p.", ch, staff, NULL, TO_CHAR );
        act( "$n creates $p.", ch, staff, NULL, TO_ROOM );
    }
    else
    {
	ch->mana -= 40;
	check_improve(ch,gsn_healer_staff,FALSE,2);
	send_to_char("You failed to make a healer staff.\n\r",ch);
    }
}

void do_stalk( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim, *bird, *och, *och_next;
    DESCRIPTOR_DATA *orig;
    AFFECT_DATA af;
    char arg[MIL];
    int chance;
    bool found = FALSE;
    argument = one_argument( argument, arg );
    if ( (chance = get_skill(ch,gsn_mind_link)) == 0)
    {
        send_to_char("You have no control over your beasts.\n\r",ch );
        return;
    }
    if (is_affected(ch,gsn_mind_link))
    {
	send_to_char("You are not ready to stalk another.\n\r",ch);
	return;
    }
    if ( arg[0] == '\0' )
    {
	send_to_char( "Stalk who?\n\r",ch);
	return;
    }
    if ( (victim = get_char_world(ch,arg)) == NULL)
    {
        send_to_char("They're not here.\n\r",ch);
        return;
    }
    if (victim->in_room->area != ch->in_room->area)
    {
	send_to_char("They are not in the area.\n\r",ch);
	return;
    }
    bird = NULL;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
        och_next = och->next_in_room;
        if ( IS_AFFECTED(och, AFF_CHARM) && IS_NPC(och) && och->master == ch && och->pIndexData->vnum == MOB_VNUM_RAVEN)
        {
	    found = TRUE;
	    bird = och;
	}
    }
    if (!found)
    {
	send_to_char("You do not have a raven with you.\n\r",ch);
	return;
    }
    if (IS_SET(bird->in_room->room_flags,ROOM_INDOORS))
    {
	send_to_char("Not indoors.\n\r",ch);
	return;
    }
    if (victim->in_room == ch->in_room)
    {
	send_to_char("That is pointless.\n\r",ch);
	return;
    }
    chance = 4*chance/5;
    chance += ch->level - victim->level;
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    if (IS_WEREBEAST(ch->race))
	chance += 10;
    act("$n flies out of the room.",bird,NULL,NULL,TO_ROOM);
    char_from_room( bird );
    WAIT_STATE2( ch, 48 );
    if (number_percent() > chance || (IS_IMMORTAL(victim) && !IS_IMMORTAL(ch)) || IS_SET(victim->in_room->room_flags,ROOM_INDOORS))
    {
    	char_to_room( bird, ch->in_room );
	send_to_char("Unable to find its target the raven returns.\n\r",ch);
    	act("$n flies in.",bird,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_mind_link,FALSE,2);
	return;
    }
    af.type		= gsn_mind_link;
    af.level		= ch->level;
    af.duration		= 12;
    af.location		= APPLY_NONE;
    af.modifier		= 0;
    af.bitvector	= 0;
    affect_to_char(ch,&af);
    char_to_room( bird, victim->in_room );
    act("$n swoops in and circles above you.",bird,NULL,NULL,TO_ROOM);
    orig = bird->desc;
    bird->desc = ch->desc;
    do_look( bird, "" );
    bird->desc = orig;
    check_improve(ch,gsn_mind_link,TRUE,1);
}

void do_control( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    char arg[MIL];
    int chance;
    argument = one_argument( argument, arg );
    if (ch->desc == NULL)
	return;
    if ( (chance = get_skill(ch,gsn_mind_link)) == 0)
    {
        send_to_char("You have no control over your beasts.\n\r",ch );
        return;
    }
    if (is_affected(ch,gsn_mind_link))
    {
	send_to_char("You are not ready to control another.\n\r",ch);
	return;
    }
    if ( ch->desc->original != NULL )
    {
        send_to_char( "You are already switched.\n\r", ch );
        return;
    }
    if ( arg[0] == '\0' )
    {
	send_to_char( "Control who?\n\r",ch);
	return;
    }
    if ( (victim = get_char_room(ch, NULL,arg)) == NULL)
    {
        send_to_char("They're not here.\n\r",ch);
        return;
    }
    if (!IS_NPC(victim) || !( IS_AFFECTED(victim, AFF_CHARM) && victim->master == ch && victim->pIndexData->vnum == MOB_VNUM_LEOPARD))
    {
	sendf(ch,"You are unable to control %s.\n\r",PERS(victim,ch));
	return;
    }
    chance = 4*chance/5;
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    if (IS_WEREBEAST(ch->race))
	chance += 10;
    WAIT_STATE2( ch, 48 );
    if (number_percent() > chance || victim->desc != NULL )
    {
	sendf(ch,"You are unable to link minds with %s.\n\r",PERS(victim,ch));
    	check_improve(ch,gsn_mind_link,FALSE,2);
	return;
    }
    check_improve(ch,gsn_mind_link,TRUE,1);
    sendf(ch, "You have switched into %s.\n\r", PERS2(victim) );
    ch->position = POS_SITTING;
    update_pos(ch);
    af.type		= gsn_linked_mind;
    af.level		= ch->level;
    af.duration		= 12;
    af.location		= APPLY_NONE;
    af.modifier		= 0;
    af.bitvector	= 0;
    affect_to_char(ch,&af);
    affect_to_char(victim,&af);
    ch->desc->character = victim;
    ch->desc->original  = ch;
    victim->desc        = ch->desc;
    ch->desc            = NULL;
    if (ch->prompt != NULL)
        victim->prompt = str_dup(ch->prompt);
    victim->comm = ch->comm;
    victim->lines = ch->lines;
}

void do_break( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    if ( ch->desc == NULL )
        return;
    if ( ch->desc->original == NULL )
    {
	if (is_affected(ch,gsn_mind_link))
	{
	    affect_strip(ch,gsn_linked_mind);
	    send_to_char("You break off the constraints of mind link.\n\r",ch);
	}
	else
            send_to_char( "You aren't switched.\n\r", ch );
        return;
    }
    send_to_char( "Your mind returns to your original body.\n\r", ch );
    if (ch->prompt != NULL)
    {
        free_string(ch->prompt);
        ch->prompt = NULL;
    }
    affect_strip(ch->desc->original,gsn_linked_mind);
    affect_strip(ch->desc->character,gsn_linked_mind);
    af.type		= gsn_mind_link;
    af.level		= ch->level;
    af.duration		= 12;
    af.location		= APPLY_NONE;
    af.modifier		= 0;
    af.bitvector	= 0;
    affect_to_char(ch->desc->original,&af);
    ch->comm                  = COMM_NOCHANNELS|COMM_NOYELL|COMM_NOTELL;
    ch->desc->character       = ch->desc->original;
    ch->desc->original        = NULL;
    ch->desc->character->desc = ch->desc;
    ch->desc                  = NULL;
}

void do_coffin( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    if (ch->class != class_lookup("vampire"))
    {
	send_to_char("You are not a vampire.\n\r",ch);
	return;
    }
    if (is_affected(ch, gsn_coffin))
    {
	send_to_char("You are already in a coffin.\n\r",ch);
	return;
    }
    if (mud_data.time_info.hour < 6 || mud_data.time_info.hour > 18)
    {
	send_to_char("Vampires do not rest at night!\n\r",ch);
	return;
    }
    if (!IS_SET(ch->in_room->room_flags,ROOM_INDOORS)
	&& ch->in_room->sector_type != SECT_INSIDE)
    {
	send_to_char("It is not a wise idea to place your coffin outdoors.\n\r",ch);
	return;
    }

    WAIT_STATE(ch,skill_table[gsn_coffin].beats);
    send_to_char("You summon your coffin from the shadows.\n\r",ch);
    act("$n summons $s coffin from the shadows.",ch,NULL,NULL,TO_ROOM);
    af.where		= TO_AFFECTS;
    af.type             = gsn_coffin;
    af.level            = ch->level;
    af.duration         = 3;
    af.location         = 0;
    af.modifier         = 0;
    af.bitvector        = AFF_SLEEP;
    affect_to_char(ch,&af);
    send_to_char("You climb in your coffin and start resting.\n\r",ch);
    do_sleep(ch,"");
}

void do_tear( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  AFFECT_DATA af;    int dam;
  char buf[MIL];
  char arg[MIL];

//modifiers
  int base_chance = ch->level*3 / 2;
  const int dex_mod = 6;
  const int lvl_mod = 3;
  const int haste_mod = 30;
  const int slow_mod = 50;

  bool haste = FALSE;
  bool slow = FALSE;
  bool fAuto = FALSE;
  bool fBlind = FALSE;
  bool fElder = get_skill(ch, gsn_unholy_gift) > 1;

//chance for different effects.
  int  chance = number_percent();


//First few quick checks.
  if ( !is_affected(ch,gsn_wolf_form) && !IS_NPC(ch))
    {
      send_to_char("Huh?\n\r", ch );        return;
    }
//we check if the target is valid.
  one_argument(argument,arg);
  if (arg[0] == '\0')
    {
     if ( (victim = ch->fighting) == NULL)
       {
	 send_to_char("But you aren't fighting anyone!\n\r",ch);
	 return;
       }
    }
 else if ((victim = get_char_room(ch, NULL,arg)) == NULL)
   {
     send_to_char("They aren't here.\n\r",ch);
     return;
   }

  if ( is_safe( ch, victim ) )
    return;

//Check if hasted/slowed
  if (IS_AFFECTED(ch, AFF_HASTE))
    haste = TRUE;

//day is equivalent to being slow.
  if (IS_AFFECTED(ch, AFF_SLOW) || vamp_day_check(ch))
    slow = TRUE;
  if(haste && slow)
    haste = slow = FALSE;


//Now we calculate the chance.
  base_chance += (get_curr_stat(ch, STAT_DEX) - get_curr_stat(victim, STAT_DEX)) * dex_mod;
  base_chance += (ch->level - victim->level) * lvl_mod;
  if (slow)base_chance-=slow_mod;
  if(haste)base_chance+=haste_mod;
  base_chance+= get_skill(ch, gsn_wolf_form) - 80;

  //if not awake chance is 100 always
  if (!IS_AWAKE(victim) || is_affected(victim, gsn_ecstacy)){
    if (!is_affected(victim, gsn_embrace_poison)){
      base_chance = 100;
      fAuto = TRUE;
    }
  }
  else if (fElder && !IS_AFFECTED(victim, AFF_BLIND)){
    base_chance = 100;
    fBlind = TRUE;
  }

//set the waitstate
  WAIT_STATE2( ch, 24 );

  a_yell(ch,victim);
  //predict interdict
  if (predictCheck(ch, victim, "tear", skill_table[gsn_tear].name))
    return;

//we make a check if hit.
//and check for immunites etc..
//FOR BALANCING ONLY
  //    sprintf(buf, "`@Chance to hit: %d``\n\r", base_chance);
  //    send_to_char(buf, ch);
  if ( (chance <= base_chance) && get_spell_damage(ch, victim, gsn_tear, DAM_PIERCE) > 0)
    {
      check_improve(ch, gsn_wolf_form, TRUE, 1);
      if (fAuto)
	chance = 100;
      else if (fBlind)
	chance = 80;
      else
	chance = number_percent();

//We hit, now we just check how good of a hit this was.
      if (chance <= 60)
//regular hit, just try to plauge.
	{
	  act("Your teeth tear deeply into $N's flesh.",ch,NULL,victim,TO_CHAR);
	  act("$n's teeth tear deeply into $N's flesh.",ch,NULL,victim,TO_NOTVICT);
	  act("$n's teeth tear deeply into your flesh.",ch,NULL,victim,TO_VICT);
	  spell_plague( gsn_plague, UMIN(60, ch->level + 5) , ch, victim,TARGET_CHAR);
	}//end regular
//Blinding hit!
      else if (chance <= 80)
	{
	  if (!is_affected(victim, gsn_blindness))
	    {
	      act("You bury your teeth in $N's face.",ch,NULL, victim, TO_CHAR);
	      act("$n claws and teeth dig deep into $N's face.\n\rOh god the `!BLOOD``!!",ch,NULL,victim,TO_NOTVICT);
	      act("$n's fangs bite deep into your face. Blood pours down your face, as you kiss your good looks goodbye.",ch,NULL,victim,TO_VICT);
	      //we blind the sucker.
	      af.where     = TO_AFFECTS;
	      af.type      = gsn_blindness;
	      af.level     = ch->level;
	      af.location  = APPLY_HITROLL;
	      af.duration  = fElder ? 0 : 5;//elder is shorter since its auto
	      af.bitvector = AFF_BLIND;
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
	      act_new( "You are blinded!", victim,NULL,NULL,TO_CHAR,POS_DEAD );
	      act("$n appears to be blinded.",victim,NULL,NULL,TO_ROOM);
	    }//end if not blind
	  else
	    {
	      act("Your teeth tear deeply into $N's flesh.",ch,NULL,victim,TO_CHAR);
	      act("$n's teeth tear deeply into $N's flesh.",ch,NULL,victim,TO_NOTVICT);
	      act("$n's teeth tear deeply into your flesh.",ch,NULL,victim,TO_VICT);
	      spell_plague( gsn_plague, UMIN(60, ch->level + 5) , ch, victim,TARGET_CHAR);
	    }//end alternative blind
	}//end blind
//bite his hand.
      else if(chance <= 98)
	{
	  act("You lock your jaws on $N's wrist.",ch,NULL,victim,TO_CHAR);
	  act("Sounds of ground bone are heard as $n's jaw locks tight around $N's wrist.",ch,NULL,victim,TO_NOTVICT);
	  act("The horrible jaws snap tight around your wrist shredding flesh and bone.", ch, NULL, victim, TO_VICT);
	  if (!is_affected(victim, gsn_tear))
	    {
//apply our dex affect.
	      af.where = TO_AFFECTS;
	      af.type = gsn_tear;
	      af.level = ch->level;
	      af.duration = ch->level/10;
	      af.location = APPLY_DEX;
	      af.modifier = -3;
	      af.bitvector = 0;
	      affect_to_char(victim,&af);
	    }//end gimp dex.
//we roll for disarm
	  if (number_percent() < 75)
//we disarm weapon
	    disarm( ch, victim );
	  else
//we disarm shield/offhand
	    {
	      if (get_eq_char( ch, WEAR_SHIELD ) != NULL )
		shield_disarm( ch, victim );
	      else
		disarm_offhand( ch, victim );
	    }
	}//end disarm effect.
//and finaly we do throat.
      else if (chance <= 100
	       && !IS_UNDEAD(victim)
	       && !is_affected(victim, gsn_embrace_poison)
	       && !is_affected(victim, gen_unlife) )
	{
	  act("You snap your teeth right on $N's throat and feel the blood pour down your throat!",ch,NULL,victim,TO_CHAR);
	  act("$n's teeth sink deep into $N's throat sending sprays of blood.",ch,NULL,victim,TO_NOTVICT);
	  act("The slavering jaws lock around your throat as you choke on your own blood.", ch, NULL, victim, TO_VICT);
	  if (!IS_UNDEAD(victim)){
	    AFFECT_DATA* paf, af;
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
	    embrace_poison(ch, victim);
	  }
	}//done embrace poison.
      else
	{
	  act("Your teeth tear deeply into $N's flesh.",ch,NULL,victim,TO_CHAR);
	  act("$n's teeth tear deeply into $N's flesh.",ch,NULL,victim,TO_NOTVICT);
	  act("$n's teeth tear deeply into your flesh.",ch,NULL,victim,TO_VICT);
	  spell_plague( gsn_plague, UMIN(60, ch->level + 5) , ch, victim,TARGET_CHAR);
	}//end altertnative embrace
//Yell now and check improv
//yell if not already in combat.
      if (ch->fighting != victim && victim->fighting != ch)
	{
	  char buf[MIL];
	  sprintf(buf, "%s", "Help! Hounds of hell are after me!");
	  j_yell(victim, buf);
	}
      check_improve(ch,gsn_wolf_form,TRUE,1);
//ANd do damage
      dam = number_range( ch->level/2, (3 * ch->level)/2);
      damage(ch,victim,dam,gsn_tear,DAM_PIERCE,TRUE);

    }//End hit.
  else
    {
//MISS
//attack yell
      if (ch->fighting != victim && victim->fighting != ch)
	{
	  sprintf(buf, "Die %s you spinless mutt!", PERS(ch, victim));
	  j_yell(victim,buf);
	}
      check_improve(ch,gsn_wolf_form,FALSE,1);
      damage(ch,victim,0,gsn_tear,DAM_PIERCE,TRUE);
    }//end miss
}//end tear



void do_stake( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MIL];
    int chance;
    argument = one_argument( argument, arg );
    if ( (chance = get_skill(ch,gsn_stake)) == 0)
    {
        send_to_char("You have no clue what to do with a stake.\n\r",ch );
        return;
    }
    if ( arg[0] == '\0' )
    {
	send_to_char( "Stake who?\n\r",ch);
	return;
    }
    if ( (victim = get_char_room(ch, NULL,arg)) == NULL)
    {
        send_to_char("They're not here.\n\r",ch);
        return;
    }
    if (!is_affected(victim,gsn_coffin))
    {
	send_to_char("Your victim must be resting in a coffin.\n\r",ch);
	return;
    }
    if ( is_safe( ch, victim ) )
        return;
    chance = 3*chance/4;
    chance += (ch->level - victim->level);
    chance += get_curr_stat(ch,STAT_LUCK) - get_curr_stat(victim,STAT_LUCK);
    WAIT_STATE2( ch, 12 );
    if (chance > number_percent())
    {
    	act("You drive a stake into $N's heart!",ch,NULL,victim,TO_CHAR);
    	act("$n drive a stake into $N's heart!",ch,NULL,victim,TO_ROOM);
    	act("You wake up in agonizing pain as $N drives a steak into your heart!",victim,NULL,ch,TO_CHAR);
    	damage(ch,victim, 3 * victim->hit / 4, gsn_stake,DAM_NONE,TRUE);
    	victim->mana/=4;
    	victim->move/=4;
    }
    else
    {
    	act("You miss $N's heart by a mile and drive a stake into $N's body!",ch,NULL,victim,TO_CHAR);
    	act("$n drive a stake into $N's body!",ch,NULL,victim,TO_ROOM);
    	act("You wake up in agonizing pain as $N drives a steak into your body!",victim,NULL,ch,TO_CHAR);
    	damage(ch,victim,victim->hit/2,gsn_stake,DAM_NONE,TRUE);
    	victim->mana = victim->mana/2;
    	victim->move = victim->move/2;
    }
    check_improve(ch,gsn_stake,TRUE,-99);
}

void do_toxify( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch, *vch_next;
    char buf[MSL];
    if (!is_affected(ch,gsn_mist_form))
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }
    act("Your mist form becomes toxic and permeates throughout the room.",ch,NULL,NULL,TO_CHAR);
    act("Suddenly toxic fumes fill the room!.",ch,NULL,NULL,TO_ROOM);
    WAIT_STATE2(ch, 12);
    	for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    	{
	    vch_next = vch->next_in_room;
            if ( vch != ch && !is_area_safe(ch, vch))
	    {
 	    	if ( ch->fighting != vch && vch->fighting != ch)
	    	{
		    a_yell(ch,vch);
		    sprintf(buf, "Help! %s just became a toxic hazard!",PERS(ch,vch));
		    j_yell(vch,buf);

	    	}
		    damage(ch, vch, UMIN(ch->level+50, ch->level*2+50), gsn_mist_form, DAM_POISON, TRUE);
	    	spell_poison(gsn_poison, ch->level, ch, (void *) vch,TARGET_CHAR);
	    }
    	}
}


void do_tackle( CHAR_DATA *ch, char *argument ){
  char arg[MIL];
  CHAR_DATA *victim, *vch;

//Data
  int chance = 7 * get_skill(ch,gsn_wolf_form) /10;
  int dam = number_range( ch->level/2, (2 * ch->level));
  int vic_lag  = PULSE_VIOLENCE;
  int ch_lag = 2 * PULSE_VIOLENCE;

//bools
  bool fSuccess = FALSE;
  bool fYell = FALSE;

//modifiers for chance to hit.
  int dex_mod = 2;
  int str_mod = 2;
  int size_mod = 4;
  int lvl_mod = 1;

  one_argument(argument,arg);

  if ( !is_affected(ch,gsn_wolf_form) && !IS_NPC(ch))
    {
      check_social(ch, "tackle", argument);
      return;
    }

  if (arg[0] == '\0')
    {
      victim = ch->fighting;
      if (victim == NULL)
	{
	  send_to_char("But you aren't fighting anyone!\n\r",ch);
	  return;
	}
    }
  else if ((victim = get_char_room(ch, NULL,arg)) == NULL)
    {
      send_to_char("They aren't here.\n\r",ch);
      return;
    }

  if ( victim == ch )
    {
      send_to_char("Self mutilation is not your style.\n\r",ch);
      return;
    }

  if (is_safe(ch,victim))
    return;

  chance += (get_curr_stat(ch,STAT_STR) - get_curr_stat(victim,STAT_STR))*dex_mod;
  chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(victim,STAT_DEX))*str_mod;
  chance += GET_AC2(victim,AC_BASH) /25;
  chance -= get_skill(victim,gsn_dodge)/20;
  chance += (ch->level - victim->level) * lvl_mod;
//easier to tackle big people
  chance -= (affect_by_size(ch,victim)) * size_mod;

  //predict interdict
  if (predictCheck(ch, victim, "tackle", "tackle")){
    a_yell(ch, victim);
    return;
  }
  if (!IS_NPC(victim) && victim->pcdata->pStallion){
    if( number_percent() < get_skill(victim, gsn_mounted)){
      chance -= 6;
      check_improve(victim, gsn_mounted, TRUE, 0);
    }
    victim->pcdata->pStallion = NULL;
  }
  //check if target should yell.
  fYell = FALSE;
  //sound attack to imm
  /* do it only if there is a lawful mob in the room */
  if (victim->pCabal && IS_CABAL(victim->pCabal, CABAL_JUSTICE)){
    a_yell(ch, victim);
  }
  else{
    for (vch = ch->in_room->people; vch; vch = vch->next_in_room){
      if (!can_see(vch, victim) || !IS_AWAKE(vch) )
	continue;
      if (IS_NPC(vch) && IS_SET(vch->act, ACT_LAWFUL)){
	a_yell(ch,victim);
	break;
      }
      if (vch->pCabal && IS_CABAL(vch->pCabal, CABAL_JUSTICE)){
	a_yell(ch,victim);
      break;
      }
    }
  }

  if (number_percent() < chance)
    {
      vic_lag  = 3 * PULSE_VIOLENCE;
      fSuccess = TRUE;
    }


//MANTIS CHECK
  if (is_affected(victim,gsn_mantis) && fSuccess)
    {
      act("You use $n's momentum against him, and throw $m to the ground!",ch,NULL,victim,TO_VICT);
      act("$N reverses your attack, and throws you to the ground!",ch,NULL,victim,TO_CHAR);
      act("$N reversed $n's attack, and throws $m to the ground.",ch,NULL,victim,TO_NOTVICT);
      WAIT_STATE2(ch, URANGE(1, number_range(1, victim->size), 2) * PULSE_VIOLENCE);
      damage(victim,ch,dam,gsn_mantis,DAM_BASH,TRUE);
      affect_strip(victim,gsn_mantis);
      return;
    }//END MANTIS

//check for lag protect
  if (!do_lagprot(ch, victim, chance, &dam, &vic_lag, gsn_wolf_form, fSuccess, fYell))
    {
      if (fSuccess)
	{

	  act("You tackle $N onto the ground!",ch,NULL,victim,TO_CHAR);
	  act("$n tackles $N onto the ground.",ch,NULL,victim,TO_NOTVICT);
	  act("$n tackles you onto the ground!",ch,NULL,victim,TO_VICT);
	}
    }//END IF NO PROTECTION


  if (!fSuccess || dam == 0 || vic_lag == 0)
    {
      check_improve(ch, gsn_wolf_form, FALSE, 1);
      dam = 0;
      if (!vamp_can_sneak(ch, ch->in_room) && !IS_NPC(ch))
	{
	  send_to_char("Your claws slip and slide in unfamilar terrain.\n\r", ch);
	  ch_lag = 3 * ch_lag / 2;
	}
    }
  else
    check_improve(ch, gsn_wolf_form, TRUE, 1);

  WAIT_STATE2(ch, ch_lag);
  if (vic_lag != 0)
    WAIT_STATE2(victim, vic_lag);
  damage(ch,victim,dam,gsn_wolf_form,DAM_BASH,TRUE);
}










//04-24-00 Viri: Modified shield and weapon cleave so they dont remove the object from char before destroying it.
//04-24-00 Viri: ReMade weapon/shield cleaves.
