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
#include "merc.h"
#include "magic.h"
#include "song.h"
#include "jail.h"
#include "cabal.h"
#include "interp.h"

const   struct  song_type      song_table     [MAX_SONG]     =
{
    /* Songs for bards */
    {   
        "reserved",
     /* song level,     song rating */
	52, 		10,
     /* song function,  min position,       rafrain,	instrument? duet? */
        0,              POS_STANDING,	    FALSE,	FALSE,	    FALSE,
     /* point to gsn,   spell_type,	mana,   beats */
        NULL,           SPELL_OTHER,	0,      0,
     /* dam message,		mesg_off,	mesg_off2 */
        "",                     "",             ""
    },
    {   
        "Test Song",
	52, 		10,
        song_test,   	POS_FIGHTING,	TRUE,	TRUE,	TRUE,
        NULL,           SPELL_OTHER,	50,      12,
        "", 		"Test ended.", ""
    },
    {   
        "Lullaby",
	10, 		1,
        song_lullaby,   POS_STANDING,	FALSE,	FALSE,   FALSE,
        &gsn_lullaby,   SPELL_OTHER,	30,      12,
        "", 		"The lullaby wears off.", ""
    },
    {   
        "Herald of the Sky",
	13, 		1,
        song_wake,   	POS_STANDING,	FALSE,	FALSE,   TRUE,
        NULL,   SPELL_OTHER,	30,      12,
        "", 		"", ""
    },
    {   
        "Broken Spirit",
	52, 		1,
        song_calm,   	POS_FIGHTING,	FALSE,	TRUE,   TRUE,
        NULL,   SPELL_MENTAL,	65,      12,
        "", 		"You no longer feel as down spirited.", "$n doesn't look as down spirited."
    },
    {   
        "Ode to God",
	52, 		1,
        song_stryth,   	POS_STANDING,	FALSE,	TRUE,   FALSE,
        NULL,   SPELL_OTHER,	50,      12,
        "", 		"", ""
    },
    {   
        "A Soothing Melody",
	52, 		1,
        song_soothing,   POS_FIGHTING,	TRUE,	FALSE,   TRUE,
        NULL,   SPELL_OTHER,	20,      12,
        "", 		"", ""
    },
    {   
        "Rest Thy Feet",
	1, 		1,
        song_refresh,   POS_STANDING,	FALSE,	FALSE,   TRUE,
        NULL,   SPELL_OTHER,	25,      12,
        "", 		"", ""
    },
    {   
        "Travel of Solitary",
	52, 		1,
        song_disband,   POS_STANDING,	FALSE,	TRUE,   TRUE,
        NULL,   SPELL_MENTAL,	60,      24,
        "", 		"You no longer feel so solitary.", ""
    },
    {   
        "Weak of Mind and Body",
	42, 		1,
        song_weaken,   	POS_FIGHTING,	TRUE,	FALSE,   TRUE,
        NULL,   SPELL_MALEDICTIVE,	40,      12,
        "", 		"You don't feel as weak.", "$n looks stronger."
    },
    {   
        "Devils Dance",
	52, 		1,
        song_dance,   	POS_STANDING,	FALSE,	TRUE,   TRUE,
        &gsn_dance_song,   SPELL_MENTAL,	 50,      12,
        "", 		"You finally stop dancing.", "$n stops dancing."
    },
    {   
        "Four Seasons",
	45, 		1,
        song_four_seasons, POS_FIGHTING, TRUE,	TRUE,   TRUE,
        &gsn_seasons,   SPELL_AFFLICTIVE,	65,      12,
        "", 		"The four seasons ends.", ""
    },
    {   
        "Vertigo",
	52, 		1,
        song_disorient,   POS_FIGHTING,	TRUE,	TRUE,   TRUE,
        &gsn_disorient,   SPELL_MENTAL,	 45,      24,
        "", 		"You no longer feel as disoriented.", "$n no longer looks so disoriented."
    },
    {   
        "Paranoia",
	52, 		1,
        song_hysteria,   POS_FIGHTING,	TRUE,	TRUE,   TRUE,
        NULL,   SPELL_MENTAL,	45,      12,
        "", 		"", ""
    },
    {   
        "Water to Wine",
	 1, 		1,
        song_waterwine,   POS_STANDING,	FALSE,	FALSE,   FALSE,
        NULL,   SPELL_OTHER,	15,      12,
        "", 		"", ""
    },
    {   
        "Intoxify My Mind",
	52, 		1,
        song_intoxify,   POS_STANDING,	FALSE,	FALSE,   TRUE,
        NULL,   SPELL_OTHER,	20,      12,
        "", 		"", ""
    },
    {   
        "Clash of Titans",
	30, 		1,
        song_titan,   	POS_FIGHTING,	TRUE,	TRUE,   TRUE,
        NULL,   SPELL_AFFLICTIVE,	35,      12,
        "earthquake", 		"", ""
    },
    {   
        "Twilight of Faeries",
	52, 		1,
        song_faeries,   POS_STANDING,	FALSE,	TRUE,   TRUE,
        NULL,   SPELL_OTHER,	50,      12,
        "", 		"", ""
    },
    {   
        "Flight of the Condor",
	33, 		1,
        song_condor,   	POS_STANDING,	FALSE,	TRUE,   FALSE,
        &gsn_condor,   SPELL_OTHER,	45,      12,
        "", 		"The condors soar away.", "The condors soar away."
    },
    {   
        "Battle Hymn",
	21, 		1,
        song_battle,   	POS_FIGHTING,	FALSE,	TRUE,   FALSE,
        NULL,   SPELL_OTHER,	35,      12,
        "", 		"The battle hymn leaves your mind.", ""
    },
    {   
        "Shield of Words",
	16, 		1,
        song_shield_words,   	POS_STANDING,	FALSE,	FALSE,   FALSE,
        NULL,   SPELL_OTHER,	30,      12,
        "", 		"The verses of the ancients no longer protects you.", ""
    },
    {   
        "Good Tidings",
	4, 		1,
        song_tidings,   POS_STANDING,	FALSE,	FALSE,   FALSE,
        NULL,   SPELL_OTHER,	20,      12,
        "", 		"You don't feel as lucky.", ""
    },
    {   
        "Legend of Hercules",
	19, 		1,
        song_hercules,   POS_STANDING,	FALSE,	FALSE,   FALSE,
        NULL,   SPELL_OTHER,	25,      12,
        "", 		"You no longer feel as strong.", "$n doesn't look as strong."
    },
    {   
        "Weight of Burden",
	52, 		1,
        song_burden,   	POS_STANDING,	FALSE,	FALSE,   TRUE,
        NULL,   SPELL_MALEDICTIVE,	35,      12,
        "", 		"Your load feels lighter.", "$n looks more relieved."
    },
    {   
        "Marriage Song",
	3, 		1,
        song_marriage,   POS_STANDING,	FALSE,	TRUE,   TRUE,
        NULL,   SPELL_OTHER,	100,      12,
        "", 		"", ""
    },
    {   
        "Mourn for the Lost",
	52, 		1,
        song_mourn,   	POS_STANDING,	FALSE,	TRUE,   FALSE,
        NULL,   SPELL_OTHER,	35,      12,
        "", 		"The death of your comrade no longer affects you.", ""
    },
    {   
        "Rest in Peace",
	39, 		1,
        song_rest,   	POS_STANDING,	FALSE,	TRUE,   TRUE,
        NULL,   SPELL_OTHER,	50,      12,
        "", 		"Your are animated once more.", "$n is animated once again."
    },
    {   
        "Birth of the Phoenix",
	52, 		1,
        song_phoenix,   POS_STANDING,	FALSE,	TRUE,   TRUE,
        &gsn_phoenix,   SPELL_OTHER,	200,      24,
        "", 		"", ""
    },
    {   
        "Emancipation",
	52, 		1,
        song_eman,   	POS_STANDING,	FALSE,	TRUE,   TRUE,
        NULL,   SPELL_MENTAL,	55,      12,
        "", 		"", ""
    },
    {   
        "Healing of Laughter",
	52, 		1,
        song_laughter,   POS_STANDING,	FALSE,	FALSE,   TRUE,
        NULL,   SPELL_OTHER,	40,      24,
        "", 		"", ""
    },
    {   
        "Scourge of Black Widow",
	52, 		1,
        song_spider,   	POS_FIGHTING,	FALSE,	TRUE,   TRUE,
        NULL,   SPELL_AFFLICTIVE,	65,      24,
        "", 		"", ""
    },
    {   
        "Mystery of the Unicorn",
	48, 		1,
        song_unicorn,   POS_STANDING,	FALSE,	TRUE,   FALSE,
        NULL,   SPELL_OTHER,	45,      12,
        "", 		"You are no longer protected by the magic of the unicorns.", ""
    },
    {
        "Complete Focus",
	52, 		10,
        song_accuracy,   POS_STANDING,	FALSE,	TRUE,   FALSE,
        &gsn_concen,   SPELL_OTHER,	60,      24,
        "", 		"", ""
    },
    {
        "Sharp Discord",
	52, 		1,
        song_shriek,   	POS_FIGHTING,	TRUE,	FALSE,   TRUE,
        NULL,   SPELL_AFFLICTIVE,	25,      24,
        "shriek", 		"", ""
    },
    {
        "Remembrance",
	36, 		1,
        song_memories,   POS_STANDING,	FALSE,	TRUE,   TRUE,
        NULL,   SPELL_OTHER,	40,      12,
        "", 		"", ""
    },
    {
        "Serenade",
	2, 		1,
        song_serenade,   POS_STANDING,	FALSE,	TRUE,   TRUE,
        NULL,   SPELL_OTHER,	5,      12,
        "", 		"", ""
    },
    {
        "Leviathans Passing",
	7, 		1,
        song_leviathon,  POS_STANDING,	FALSE,	FALSE,   FALSE,
        NULL,   SPELL_OTHER,	15,      12,
        "", 		"", ""
    },
    {
        "Lust Seduction Desire",
	52, 		1,
        song_lust,   	POS_STANDING,	FALSE,	TRUE,   TRUE,
        &gsn_lust,   SPELL_OTHER,	40,      12,
        "", 		"Your hormones level off.", ""
    },
    {
        "A Kings Feast",
	24, 		1,
        song_feast,   	POS_STANDING,	FALSE,	FALSE,   TRUE,
        NULL,   SPELL_OTHER,	30,      12,
        "", 		"", ""
    },
    {
        "Let it be Known",
	27, 		1,
        song_reputation,   POS_STANDING, FALSE,	FALSE,   TRUE,
        &gsn_reputation,   SPELL_OTHER,	20,      12,
        "", 		"Your reputation is no longer as well known.", ""
    },
    {
        "Victory Be Ours",
	52, 		1,
        song_victory,   POS_STANDING,	FALSE,	TRUE,   TRUE,
        NULL,   SPELL_OTHER,	20,      12,
        "", 		"", ""
    },
    {
        "Unforgiven",
	52, 		1,
        song_unforgiven, POS_STANDING, FALSE,	TRUE,   TRUE,
        &gsn_unforgiven,   SPELL_MENTAL,	50,      18,
        "", 		"", ""
    }
};

char * random_garble(int length)
{
    int pos = 0, word = 0, i = 0, j = 0, 
	wlength = number_range(UMAX(2,UMIN(4,length/7)),UMAX(3,UMIN(7,length/4)));
    char buf[MSL], temp;
    for (i = 0;i < wlength;i++)
    {
	word = number_range(UMAX(2,UMIN(4,length/7)),UMAX(4,UMIN(7,length/4)));
	for (j = 0;j < word;j++)
	{
	    if (number_percent() < 50)
		temp = 'A' + number_range (0,25);
	    else
		temp = 'a' + number_range (0,25);
	    buf[pos++] = temp;
	}
	buf[pos++] = ' ';
    }
    buf[pos] = '\0';
    return (str_dup(buf));
}

void song_to_room(CHAR_DATA *ch, char *argument, bool brief, bool sleep, bool duet)
{
    CHAR_DATA *vch;
    if (duet && ch->pcdata->duet)
	ch = ch->pcdata->duet;
    if (ch->in_room == NULL)
	return;
    makedrunk(argument,ch);
    act("You sing '`6$t``'",ch,argument,NULL,TO_CHAR);
    for (vch = ch->in_room->people;vch != NULL; vch = vch->next_in_room)
    {
	if (!sleep && !IS_AWAKE(vch))
	    continue;
	if (brief && IS_SET(vch->comm,COMM_BRIEF))
	    continue;
	if (vch == ch)
	    continue;
	act("$n sings '`6$t``'",ch,argument,vch,TO_VICT);
    }
}

void duet_song_to_room(CHAR_DATA *ch, CHAR_DATA *partner, char *argument, bool brief, bool sleep)
{
    CHAR_DATA *vch;
    char buf[MSL];
    if (ch->in_room == NULL || partner->in_room == NULL)
	return;
    makedrunk(argument,ch);
    makedrunk(argument,partner);
    act("You sing together with $N '`6$t``'",ch,argument,partner,TO_CHAR);
    act("You sing together with $N '`6$t``'",partner,argument,ch,TO_CHAR);
    for (vch = ch->in_room->people;vch != NULL; vch = vch->next_in_room)
    {
	if (!sleep && !IS_AWAKE(vch))
	    continue;
	if (brief && IS_SET(vch->comm,COMM_BRIEF))
	    continue;
	if (vch == ch || vch == partner)
	    continue;
	sprintf(buf,"%s and %s sings together '`6%s``'",PERS(ch,vch),PERS(partner,vch),argument);
	act(buf,vch,NULL,NULL,TO_CHAR);
    }
}

void s_yell( CHAR_DATA *ch, CHAR_DATA *victim, bool force )
{
  char buf[MSL];
  if (ch == NULL || victim == NULL)
    return;
  if (ch->fighting != victim && victim->fighting != ch && (!IS_NPC(ch) || ch->master != NULL)){
    if (!IS_NPC(victim)){
      sprintf(buf,"%s attacked %s at %d", IS_NPC(ch) ? ch->short_descr : ch->name, 
	      victim->name,ch->in_room->vnum);
      log_string(buf);
      wiznet(buf,NULL,NULL,WIZ_FIGHTING,0,0);
      continuum(victim, CONTINUUM_ATTACK);
     
/* MURDER CRIME CHECK */
      if (ch->in_room && is_crime(ch->in_room, CRIME_MURDER, victim)){
	set_crime(ch, victim, ch->in_room->area, CRIME_MURDER);
	
	/* check of simulacra for justices */
	if (is_affected(ch, gsn_doppelganger)){
	  char buf[MIL];
	  sprintf( buf, "Warning! Someone seems to be masquerading as %s!",
		   PERS2(ch));
	  cabal_echo_flag( CABAL_JUSTICE | CABAL_HEARGUARD, buf );
	}
      }
/* ATTACKING JUSTICE IN LAWFUL AREA IS ALWAYS AN OBSTRUCTION */
      else if (victim->pCabal && IS_CABAL(victim->pCabal, CABAL_JUSTICE) && IS_AREA(ch->in_room->area, AREA_LAWFUL)){
	set_crime(ch, victim, ch->in_room->area, CRIME_OBSTRUCT);
      }
      /* CONTINENCY */
      check_contingency(ch, NULL, CONT_ATTACK);
      check_contingency(victim, NULL, CONT_ATTACKED);

    }//END PC CHECK
    else {
      bool fCrime = FALSE;
/* ASSAULT CRIME CHECK */
      if (ch->in_room && is_crime(ch->in_room, CRIME_ASSAULT, victim)){
	set_crime(ch, victim, ch->in_room->area, CRIME_ASSAULT);
	fCrime = TRUE;
      }
/* ATTACKING JUSTICE MOBS IN LAWFUL AREA IS ALWYAS A CRIME */
      else if (victim->pCabal && IS_CABAL(victim->pCabal, CABAL_JUSTICE) && IS_AREA(ch->in_room->area, AREA_LAWFUL)){
	set_crime(ch, victim, ch->in_room->area, CRIME_OBSTRUCT);
	fCrime = TRUE;
      }
/* attackign charmed mobs requiers a yell as well */
      else if (IS_NPC(victim) && IS_AFFECTED(victim, AFF_CHARM))
	fCrime = TRUE;

      if (IS_SET(victim->off_flags,GUILD_GUARD) && victim->pCabal && IS_CABAL(victim->pCabal, CABAL_JUSTICE)){
	sprintf(buf, "%s: Alert! The jail is being attacked!", ch->in_room->area->name);
	cabal_echo_flag( CABAL_JUSTICE, buf );
	sprintf(buf, "Alert! The jail is being attacked by %s!",PERS(ch,victim));
      }
      else if (IS_SET(victim->off_flags,GUILD_GUARD))
	sprintf(buf, "Alert! The guild is being attacked by %s!",PERS(ch,victim));
      else
	sprintf(buf, "Die! %s, you singing fool!",PERS(ch,victim));
      
      if (fCrime){
	REMOVE_BIT(victim->comm,COMM_NOCHANNELS);
	REMOVE_BIT(victim->comm,COMM_NOYELL);
	j_yell(victim,buf);
	SET_BIT(victim->comm,COMM_NOYELL);
	SET_BIT(victim->comm,COMM_NOCHANNELS);
      }
    }//END NPC CHECK
  }
}

int sglevel(CHAR_DATA *ch, int sn)
{
    if IS_NPC(ch)
        return 1000;
    else if (song_table[sn].skill_level != 69)
        return UMIN(52,song_table[sn].skill_level);
    else
	return 1000;
}

int song_lookup( const char *name )
{
    int sn;
    for ( sn = 0; sn < MAX_SONG; sn++ )
    {
        if ( song_table[sn].name == NULL )
            break;
        if ( LOWER(name[0]) == LOWER(song_table[sn].name[0]) && !str_prefix( name, song_table[sn].name ) )
            return sn;
    }
    return -1;
}  

int get_song(CHAR_DATA *ch, int sn)
{
    int skill;
    if (sn == -1)
        skill = ch->level * 5 / 2;
    else if (sn < -1 || sn > MAX_SONG)
    {   
        bug("Bad sn %d in get_song.",sn);
        skill = 0;
    }
    else if (ch->trust == 6969)
        skill = 100;
    else if (is_affected(ch,gsn_ecstacy))
        return 0;
    else if (!IS_NPC(ch))
    {
        if (sglevel(ch,sn) > ch->level)
            skill = 0;
        else
            skill = ch->pcdata->songlearned[sn];
    }
    else
    {
        if (song_table[sn].song_fun != song_null)
            skill = 40 + 2 * ch->level;
	else
	    skill = 0;
    }
    if ( !IS_NPC(ch) && (is_affected(ch,gsn_drunken) || ch->pcdata->condition[COND_DRUNK]  > 10 ))
        skill = 9 * skill / 10;
    if ( is_affected(ch,gsn_hysteria) )
        skill /= 2;
    return URANGE(0,skill,100);
}

int find_song( CHAR_DATA *ch, const char *name )
{
    int sn, found = -1;
    if (IS_NPC(ch))
        return song_lookup(name);
    for ( sn = 0; sn < MAX_SONG; sn++ )
    {
        if (song_table[sn].name == NULL)
            break;
        if (LOWER(name[0]) == LOWER(song_table[sn].name[0])
        && !str_prefix(name,song_table[sn].name))
        {
            if ( found == -1)
                found = sn;
            if (ch->level >= sglevel(ch,sn) && ch->pcdata->songlearned[sn] > 0)
                return sn;
        }
    }
    return found; 
}

void check_improve_song( CHAR_DATA *ch, int sn, bool success, int multiplier )
{
    int chance;
    if (IS_NPC(ch))
        return;
    if (ch->level < sglevel(ch,sn) || song_table[sn].rating == 0
    || ch->pcdata->songlearned[sn] == 0 || ch->pcdata->songlearned[sn] == 100)
        return;
    if (multiplier == -99)
    {
        sendf(ch,"You have become better at %s!\n\r", song_table[sn].name);
        ch->pcdata->songlearned[sn]++;
        gain_exp(ch,song_table[sn].rating);
        return;
    }
    chance = 10 * int_app[get_curr_stat(ch,STAT_INT)].learn;
    chance /= ( multiplier * song_table[sn].rating * 10);
    chance += ch->level / 2;
    chance += get_curr_stat(ch,STAT_WIS) / 2;
    if (!success)
        chance /=2;
    if (number_range(1,1000) > chance)
        return;   
    if (success)
    {
        chance = URANGE(5,100 - ch->pcdata->songlearned[sn], 95);
        if (IS_IMMORTAL(ch) && ch->wimpy == 69)
            chance += 50;
        if (number_percent() < chance)
        {
            sendf(ch,"You have become better at %s!\n\r", song_table[sn].name);
            ch->pcdata->songlearned[sn]++;
            gain_exp(ch,song_table[sn].rating);
        }
    }
    else
    {
        chance = URANGE(5,ch->pcdata->songlearned[sn]/2,30);
        if (number_percent() < chance)
        {
            sendf(ch,"You learn from your mistakes, and your %s song improves.\n\r",song_table[sn].name);
            ch->pcdata->songlearned[sn] += number_range(1,3);
            ch->pcdata->songlearned[sn] = UMIN(ch->pcdata->songlearned[sn],100);
            gain_exp(ch,song_table[sn].rating);
        }
    }   
}

void song_strip( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf, *paf_next;
    for ( paf = ch->affected2; paf != NULL; paf = paf_next )
    {
        paf_next = paf->next;  
        if ( paf->type == sn )
            song_affect_remove( ch, paf);
    }
}

bool check_instrument( CHAR_DATA *ch )
{
    OBJ_DATA *obj;
    if ((obj = get_eq_char(ch,WEAR_WIELD)) != NULL
	&& obj->item_type == ITEM_INSTRUMENT)
      return TRUE;
    if ((obj = get_eq_char(ch,WEAR_HOLD)) != NULL
	&& obj->item_type == ITEM_INSTRUMENT)
      return TRUE;
    return FALSE;
}

void do_sing( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch, *partner = NULL;
    char arg1[MIL];
    int mana, mana2 = 50, sn, concen, concen2 = 0, dchance, sn2 = skill_lookup("duet");
    bool duet = FALSE, found = FALSE, passed = TRUE, sdouble = FALSE;
    if (!IS_IMMORTAL(ch) && ch->class != class_lookup("bard"))
    {
	send_to_char("You think you're a bard?\n\r",ch);
	return;
    }
    if (is_affected(ch, gsn_forget))
    {
        send_to_char("You seem to have forgotten all your songs.\n\r",ch);   
        return;
    }
    if (is_affected(ch, gsn_garble))
    {
        send_to_char("You can't get the right lyrics.\n\r", ch);
        return;
    }
    if (is_affected(ch, gsn_headbutt))
    {
        send_to_char("Your head is still spinning, you can't focus enough.\n\r",ch);
        return;
    }
    if (IS_AFFECTED2(ch, AFF_SILENCE))
    {
	send_to_char("You try to sing, but no words come out of your mouth!\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_uppercut))
    {
	send_to_char("Your jaws hurt too much to move!\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_refrain) || is_song_affected(ch,gsn_seasons))
    {
	send_to_char("You mind is not clear enough to sing another song right now.\n\r",ch);
	return;
    }
    argument = one_argument( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Sing which what where?\n\r", ch );
        return;
    }
    if (!str_cmp(arg1,"freeverse"))
    {
	song_to_room(ch,argument,FALSE,FALSE,FALSE);
	return;
    }
    if ((sn = find_song(ch,arg1)) < 1 || song_table[sn].song_fun == song_null  
    || (!IS_NPC(ch) && (ch->level < sglevel(ch,sn) || ch->pcdata->songlearned[sn] == 0)))
    {
        send_to_char( "You don't know any songs of that name.\n\r", ch );
        return;
    }
    if ( ch->position < song_table[sn].minimum_position )
    {
        send_to_char( "You can't concentrate enough.\n\r", ch );    
        return;
    }
    if (song_table[sn].instrument && !check_instrument( ch ) )
    {
	send_to_char("You require an instrument to sing this song.\n\r",ch);
	return;
    }
    if (ch->level + 2 == sglevel(ch,sn))
      mana = 50;
    else
      mana = UMAX( song_table[sn].min_mana, 100 / ( 2 + ch->level - sglevel(ch,sn) ) );
    
    if (song_table[sn].instrument){
      OBJ_DATA* obj = get_eq_char(ch,WEAR_WIELD);
      if (obj){
	act("$n strikes a sweet melody on $p.", ch, obj, NULL, TO_ROOM);
	act("You strike a sweet melody on $p.", ch, obj, NULL, TO_CHAR);
	mana /= 2;
      }
    }

    if ( !IS_NPC(ch) && ch->mana < mana && !IS_IMMORTAL(ch)) 
    {
        send_to_char( "You don't have enough mana.\n\r", ch );
        return; 
    }
    if (!IS_NPC(ch) && (partner = ch->pcdata->duet) != NULL)
    {
	for (vch = ch->in_room->people;vch != NULL;vch = vch->next_in_room)
	{
	    if (!IS_NPC(vch) && partner == vch 
		&& vch->pcdata->duet != NULL && vch->pcdata->duet == ch)
	    {
		found = TRUE;
		break;
	    }
	}
    }
    while (found)
    {
    	act("$n prepares to sing '$t' in a duet.",ch,song_table[sn].name,NULL,TO_ROOM);
    	act("You prepare to sing '$t' in a duet.",ch,song_table[sn].name,NULL,TO_CHAR);
    	if (is_affected(partner, gsn_forget))
    	{
    	    send_to_char("You seem to have forgotten all your songs.\n\r",partner);   
	    act("$N has difficulties singing this song.",ch,NULL,partner,TO_CHAR);
	    passed = FALSE;
	    break;
    	}
    	if (is_affected(partner, gsn_garble))
    	{
    	    send_to_char("You can't get the right lyrics.\n\r", partner);
	    act("$N has difficulties singing this song.",ch,NULL,partner,TO_CHAR);
	    passed = FALSE;
	    break;
    	}
    	if (is_affected(partner, gsn_headbutt))
    	{
    	    send_to_char("Your head is still spinning, you can't focus enough.\n\r",partner);
	    act("$N has difficulties singing this song.",ch,NULL,partner,TO_CHAR);
	    passed = FALSE;
	    break;
    	}
    	if (IS_AFFECTED2(partner, AFF_SILENCE))
    	{
	    send_to_char("You try to sing, but no words come out of your mouth!\n\r",partner);
	    act("$N has difficulties singing this song.",ch,NULL,partner,TO_CHAR);
	    passed = FALSE;
	    break;
    	}
    	if (is_affected(ch,gsn_uppercut))
    	{
	    send_to_char("Your jaws hurt too much to move!\n\r",partner);
	    act("$N has difficulties singing this song.",ch,NULL,partner,TO_CHAR);
	    passed = FALSE;
	    break;
    	}
    	if (is_affected(partner,gsn_refrain))
    	{
	    send_to_char("You mind is not clear enough to sing another song right now.\n\r",partner);
	    act("$N has difficulties singing this song.",ch,NULL,partner,TO_CHAR);
	    passed = FALSE;
	    break;
    	}
        if (partner->level < sglevel(partner,sn) || partner->pcdata->songlearned[sn] == 0)
    	{
            act( "$N doesn't know that song.",ch,NULL,partner,TO_CHAR);
            act( "You don't know that song.",ch,NULL,partner,TO_VICT);
	    passed = FALSE;
	    break;
    	}
    	if ( partner->position < song_table[sn].minimum_position )
    	{
    	    act( "$N can't concentrate enough.", ch,NULL,partner,TO_CHAR );    
    	    act( "You can't concentrate enough.", ch,NULL,partner,TO_VICT );    
	    passed = FALSE;
	    break;
    	}
    	if (song_table[sn].instrument && !check_instrument( partner ) )
    	{
	    act("$N requires an instrument to sing this song.",ch,NULL,partner,TO_CHAR);
	    act("You require an instrument to sing this song.",ch,NULL,partner,TO_VICT);
	    passed = FALSE;
	    break;
    	}
	dchance = number_percent();
	if (get_skill(ch,sn2) < dchance || get_skill(partner,sn2) < dchance)
	{
	    act("You failed to harmonize with $N.",ch,NULL,partner,TO_CHAR);
	    act("You failed to harmonize with $n.",ch,NULL,partner,TO_VICT);
	    passed = FALSE;
	    break;
	}
    	concen2 = get_song(partner,sn);
    	if (concen2 > 0)
    	{
    	    concen2 += get_curr_stat(partner,STAT_LUCK) - 16;
    	    if (is_affected(partner,gsn_prayer))
    	        concen2 += UMIN(5,.3*(100 - concen2));
	    if (is_song_affected(partner,gsn_concen))
	    {
	    	concen2 += concen2 / 3;
	    	song_strip(partner,gsn_concen);
	    }
    	}
        if (partner->level + 2 == sglevel(partner,sn))
            mana2 = 50;
        else
            mana2 = UMAX( song_table[sn].min_mana, 100 / ( 2 + partner->level - sglevel(partner,sn) ) );
    	if ( !IS_NPC(partner) && partner->mana < mana2 && !IS_IMMORTAL(partner)) 
    	{
    	    send_to_char( "You don't have enough mana.\n\r", partner );
	    act("$N doesn't have enough mana to sing with you.",ch,NULL,partner,TO_CHAR);
	    passed = FALSE;
    	    break; 
    	}
	break;
    }
    if (!found)
    {
    	act("$n prepares to sing '$t'.",ch,song_table[sn].name,NULL,TO_ROOM);
    	act("You prepare to sing '$t'.",ch,song_table[sn].name,NULL,TO_CHAR);
    }
    if (!IS_IMMORTAL(ch))
        WAIT_STATE2( ch, song_table[sn].beats );
    concen = get_song(ch,sn);
    if (concen > 0)
    {
        concen += get_curr_stat(ch,STAT_LUCK) - 16;
        if (is_affected(ch,gsn_prayer))
            concen += UMIN(5,.3*(100 - concen));
	if (is_song_affected(ch,gsn_concen))
	{
	    concen += concen/3;
	    song_strip(ch,gsn_concen);
	}
    }
    if ( (IS_NPC(ch) && ch->desc == NULL && !ch->pIndexData->progtypes) || number_percent() > concen )
    {
        send_to_char( "You lost your concentration.\n\r", ch );
	if (found)
	    act("$n lost $s concentration.",ch,NULL,partner,TO_VICT);
        check_improve_song(ch,sn,FALSE,1);
        if (!IS_IMMORTAL(ch))
             ch->mana -= mana / 2;
    }
    else 
    {
	if ( found && passed && number_percent() > concen2 )
    	{
    	    send_to_char( "You lost your concentration.\n\r", partner );
    		act("$N lost $S concentration.",ch,NULL,partner,TO_CHAR);
    	    check_improve_song(partner,sn,FALSE,1);
	    passed = FALSE;
    	    if (!IS_IMMORTAL(partner))
            	partner->mana -= mana2 / 2;
	}
    	if (found)
    	{
	    if (passed)
	    {
		duet = TRUE;
        	check_improve(ch,sn2,TRUE,1);
        	check_improve(partner,sn2,TRUE,1);
		sdouble = song_table[sn].duet;
	    }
	    else
		send_to_char("You'll have to sing this song yourself.\n\r",ch);
	}
	if (duet)
	{
	    if (sdouble)
	    {
                if (!IS_IMMORTAL(ch))
            	    ch->mana -= 2*mana/3;
		if (!IS_IMMORTAL(partner))
            	    partner->mana -= 2*mana2/3;
	    }
	    else
	    {
		if (!IS_IMMORTAL(ch))
            	    ch->mana -= mana/3;
        	if (!IS_IMMORTAL(partner))
            	    partner->mana -= mana2/3;
	    }
	}
	else if (!IS_IMMORTAL(ch))
            ch->mana -= mana;
        (*song_table[sn].song_fun) ( sn, ch->level, ch, duet, TRUE, argument);
	if (duet && sdouble)
	{
            (*song_table[sn].song_fun) ( sn, partner->level, partner, duet, FALSE, argument);	
            check_improve_song(partner,sn,TRUE,1);
	}
        check_improve_song(ch,sn,TRUE,1);
    }
}

bool refrain_sing( CHAR_DATA *ch, int sn )
{
    int mana, concen;
    if (is_affected(ch, gsn_forget))
    {
        send_to_char("You seem to have forgotten all your songs.\n\r",ch);   
        return FALSE;
    }
    if (is_affected(ch, gsn_garble))
    {
        send_to_char("You can't get the right lyrics.\n\r", ch);
        return FALSE;
    }
    if (is_affected(ch, gsn_headbutt))
    {
        send_to_char("Your head is still spinning, you can't focus enough.\n\r",ch);
        return FALSE;
    }
    if (IS_AFFECTED2(ch, AFF_SILENCE))
    {
	send_to_char("You try to sing, but no words come out of your mouth!\n\r",ch);
	return FALSE;
    }
    if (is_affected(ch,gsn_uppercut))
    {
	send_to_char("Your jaws hurt too much to move!\n\r",ch);
	return FALSE;
    }
    if ( ch->position < song_table[sn].minimum_position )
    {
        send_to_char( "You can't concentrate enough.\n\r", ch );    
        return FALSE;
    }
    if (song_table[sn].instrument && !check_instrument( ch ) )
    {
	send_to_char("You require an instrument to sing this song.\n\r",ch);
	return FALSE;
    }
    if (ch->level + 2 == sglevel(ch,sn))
        mana = 50;
    else
        mana = UMAX( song_table[sn].min_mana, 100 / ( 2 + ch->level - sglevel(ch,sn) ) );
    if ( !IS_NPC(ch) && ch->mana < mana && !IS_IMMORTAL(ch)) 
    {
        send_to_char( "You don't have enough mana.\n\r", ch );
        return FALSE; 
    }
    act("$n refrains '$t'.",ch,song_table[sn].name,NULL,TO_ROOM);
    act("You refrain '$t'.",ch,song_table[sn].name,NULL,TO_CHAR);
    concen = get_song(ch,sn);
    if (concen > 0)
    {
        concen += get_curr_stat(ch,STAT_LUCK) - 16;
        if (is_affected(ch,gsn_prayer))
            concen += UMIN(5,.3*(100 - concen));
	if (is_song_affected(ch,gsn_concen))
	{
	    concen += concen/3;
	    song_strip(ch,gsn_concen);
	}
    }
    if ( (IS_NPC(ch) && ch->desc == NULL && !ch->pIndexData->progtypes) || number_percent() > concen )
    {
        send_to_char( "You lost your concentration.\n\r", ch );
        check_improve_song(ch,sn,FALSE,1);
        if (!IS_IMMORTAL(ch))
             ch->mana -= mana / 2;
	return TRUE;
    }
    else
    {
        if (!IS_IMMORTAL(ch))
            ch->mana -= mana;
        (*song_table[sn].song_fun) ( sn, ch->level, ch, FALSE, TRUE, "");
        check_improve_song(ch,sn,TRUE,1);
	return TRUE;
    }
    return TRUE;
}

void do_refrain( CHAR_DATA *ch, char *argument)
{
    char arg[MIL];
    int chance, sn;
    AFFECT_DATA af;
    one_argument(argument,arg);
    if ( IS_NPC(ch) )
	return;
    if ( (chance = get_skill(ch,gsn_refrain) ) == 0 )
    {
	send_to_char("You cannot keep any melodies in your head.\n\r",ch);
	return;
    }
    if (is_affected(ch,gsn_refrain) || is_song_affected(ch,gsn_seasons))
    {
	send_to_char("You cannot refrain another song at the moment.\n\r",ch);
	return;
    }
    if ( arg[0] == '\0' )
    {
        send_to_char( "Sing which what where?\n\r", ch );
        return;
    }
    if ((sn = find_song(ch,arg)) < 1 || song_table[sn].song_fun == song_null  
    || (!IS_NPC(ch) && (ch->level < sglevel(ch,sn) || ch->pcdata->songlearned[sn] == 0)))
    {
        send_to_char( "You don't know any songs of that name.\n\r", ch );
        return;
    }
    if ( ch->position < song_table[sn].minimum_position )
    {
        send_to_char( "You can't concentrate enough.\n\r", ch );    
        return;
    }
    if (!song_table[sn].refrain)
    {
	send_to_char("This song cannot be refrained.\n\r",ch);
	return;
    }
    if (!IS_IMMORTAL(ch))
        WAIT_STATE2( ch, song_table[sn].beats );
    if ( chance < number_percent() || !refrain_sing(ch,sn) )
    {
	send_to_char( "You failed.\n\r",ch);
	check_improve(ch,gsn_refrain,FALSE,1);
	return;
    }
    else
    {
    	af.where     = TO_AFFECTS;
    	af.type      = gsn_refrain;
    	af.level     = ch->level;
    	af.duration  = 0;
    	af.modifier  = sn;  
    	af.location  = APPLY_NONE;
    	af.bitvector = 0;
    	affect_to_char( ch, &af );
	send_to_char("You refrain the song in your head.\n\r",ch);
    }
}

void song_null( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    send_to_char( "That's not a song!\n\r", ch );
}

void song_test( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
//    AFFECT_DATA af;
    char buf[MSL];
//    CHAR_DATA *vch;
    if (sing)
    {
    	sprintf(buf,"This is a test...");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    }
}

void song_lullaby( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    AFFECT_DATA af;
    char buf[MSL];
    CHAR_DATA *vch;
    if (sing)
    {
    	sprintf(buf,"Lay at rest and feel the peace,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"With a song, the pain will cease.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Close thy eyes, the wounds will heal,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Breathe real slow, the cuts will seal.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"This deep sleep, with lullaby,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"You'll awake, with time gone by.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_song_affected(vch,sn) || ch == vch)
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
    	    af.where     = TO_AFFECTS;
    	    af.type      = sn;
    	    af.level     = level;
    	    af.duration  = 8;
    	    af.modifier  = 0;  
    	    af.location  = 0;
    	    af.bitvector = AFF_SLEEP;
    	    song_affect_to_char( vch, &af );
	    if (is_affected(ch,gsn_insomnia))
	      affect_strip(ch,gsn_insomnia);
            if ( IS_AWAKE(vch) ){
	      send_to_char( "Your eyelids feel heavy and you drift into a peaceful slumber.\n\r", vch);
	      do_sleep(vch,"");
	      act("$n is drained and falls asleep.",vch,NULL,NULL,TO_ROOM);
            }
	    else
	      act("$n falls into a deep slumber.",vch,NULL,NULL,TO_ROOM);
	}
    }
}
 
void song_wake( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"No longer rest your weary bones,");
    	song_to_room(ch,buf,TRUE,TRUE,FALSE);
    	sprintf(buf,"Wake now to these warming tones.");
    	song_to_room(ch,buf,TRUE,TRUE,TRUE);
    	sprintf(buf,"From deep sleep your dreams unfurled,");
    	song_to_room(ch,buf,TRUE,TRUE,FALSE);
    	sprintf(buf,"And open your eyes to the world.");
    	song_to_room(ch,buf,TRUE,TRUE,TRUE);
    	sprintf(buf,"So stand hence from where you lie,");
    	song_to_room(ch,buf,FALSE,TRUE,FALSE);
    	sprintf(buf,"Pay heed to the Herald of the Sky.");
    	song_to_room(ch,buf,FALSE,TRUE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (ch == vch)
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
	  affect_strip(ch,gsn_sleep);
	  affect_strip(ch,gsn_lullaby);
	  affect_strip(ch,gsn_ecstacy);
	  affect_strip(ch,gsn_blackjack);
	  affect_strip(ch,gsn_strangle);
	  affect_strip(ch,gsn_hypnosis);

	  REMOVE_BIT(vch->affected_by,AFF_SLEEP);
	  do_stand(vch,"");
	  if ( IS_AWAKE(vch) )
            {
	      send_to_char( "Your eyes open to a wonderful melody.\n\r", vch);
	      act("$N wakes to $n's melody.",ch,NULL,vch,TO_NOTVICT);
	      act("$N wakes to your melody.",ch,NULL,vch,TO_CHAR);
            }
	}
    }
}

void song_refresh( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"My eyes have traveled far ahead,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"and saw what riches bode!");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"So come my lads, arise and go,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Continue on the Road.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (!IS_AWAKE(vch))
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
	    vch->move = UMIN( vch->move + level, vch->max_move );
	    if (vch->max_move == vch->move)
	        send_to_char("You feel fully refreshed!\n\r",vch);
	    else
	        send_to_char( "You feel less tired.\n\r", vch );
	}
    }
}

void song_disband( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Abandoned without a friend,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Stranded until the very end.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"There are none that you can trust,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Solitary is a must.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"With just your shadow by your side.");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Let lonelyness be your ally.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_area_safe(ch,vch) || IS_NPC(vch))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
        s_yell(ch,vch,TRUE);
	if (vch->fighting == NULL)
	    multi_hit(vch,ch,TYPE_UNDEFINED);
	if (saves_spell(level,vch,DAM_OTHER,song_table[sn].spell_type))
	    continue;
	send_to_char("You feel solitary.\n\r",vch);
	stop_follower(vch);
    }
}

void song_weaken( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    AFFECT_DATA af;
    if (sing)
    {
    	sprintf(buf,"Piercing music drowns your head,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Slowly snapping each logic's thread.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Course of music seeps to the bones,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Slowly adding its weight in stone.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Mind and body fail at each tone,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Sense and strength no longer your own.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_area_safe(ch,vch))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
        s_yell(ch,vch,FALSE);
	if (vch->fighting == NULL)
	    multi_hit(vch,ch,TYPE_UNDEFINED);
	if (is_song_affected(vch,sn))
	    continue;
	if (saves_spell(level,vch,DAM_OTHER,song_table[sn].spell_type)){
	  act("$N resists the song's spell.", ch, NULL, vch, TO_CHAR);
	  continue;
	}

    	af.where     = TO_AFFECTS;
    	af.type      = sn;
    	af.level     = level;
    	af.duration  = level/5;
    	af.modifier  = - level/10;  
    	af.location  = APPLY_STR;
    	af.bitvector = 0;
    	song_affect_to_char( vch, &af );
    	af.location  = APPLY_INT;
    	song_affect_to_char( vch, &af );
	send_to_char("You feel weak...\n\r",vch);
    }
}

void song_dance( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    AFFECT_DATA af;
    if (sing)
    {
    	sprintf(buf,"We'll dance all night while the moon runs free,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"And dandle the lasses upon our knee,");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"And then you'll ride along with me,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"To dance with the Jack o' Shadows.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_area_safe(ch,vch) || IS_NPC(vch))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
        s_yell(ch,vch,TRUE);
	if (vch->fighting == NULL)
	    multi_hit(vch,ch,TYPE_UNDEFINED);
	if (saves_spell(level-5,vch,DAM_OTHER,song_table[sn].spell_type))
	    continue;
	if (is_song_affected(vch,sn))
	   continue;
    	af.where     = TO_AFFECTS;
    	af.type      = sn;
    	af.level     = level;
    	af.duration  = number_fuzzy(level/8);
    	af.modifier  = 0;  
    	af.location  = APPLY_NONE;
    	af.bitvector = 0;
    	song_affect_to_char( vch, &af );
	send_to_char("You feel dancing!\n\r",vch);
	act("$n starts dancing uncontrollably!",vch,NULL,NULL,TO_ROOM);
    }
}

void song_calm( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    AFFECT_DATA af;
    if (sing)
    {
    	sprintf(buf,"Here we stand day by day, searching blindly for the way.");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"All you find is another gray, wishing back the days of play.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Here we stand day by day, dwelling in life's long sorrow.");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Walking lost in the barrows, with no hope of a better tomorrow.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Here we stand day by day, life's only truth I have spoken.");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Dark the day before we're awoken, our spirits do be broken.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (!IS_AWAKE(vch))
	    continue;
        if (IS_NPC(vch) && (IS_SET(vch->imm_flags,IMM_MAGIC) || IS_SET(vch->act,ACT_UNDEAD) 
	    || IS_SET(vch->act,ACT_TOO_BIG) || IS_SET(vch->off_flags,GUILD_GUARD) || IS_SET(vch->off_flags,CABAL_GUARD)))
            continue;
        if (IS_AFFECTED(vch,AFF_BERSERK) || is_affected(vch,skill_lookup("frenzy")) || IS_AFFECTED2(vch,AFF_RAGE))
            continue;
	if (!is_same_group(ch,vch) && !(vch->fighting != NULL && is_same_group(ch,vch->fighting)))
	    continue;
	if (!is_same_group(ch,vch) && saves_spell(level-4,vch,DAM_NONE,song_table[sn].spell_type))
	    continue;
        if (vch->fighting || vch->position == POS_FIGHTING)
            stop_fighting(vch,FALSE);
        send_to_char("A wave of calm depression over you.\n\r",vch);
	if (is_song_affected(vch,sn))
	    continue;

    	af.type      = sn;
    	af.level     = level;
    	af.duration  = 2;
    	af.where     = TO_AFFECTS;
	af.bitvector = AFF_CALM;
    	af.modifier  = - level/10;  
    	af.location  = APPLY_HITROLL;
	song_affect_to_char( vch, &af );

    	af.location  = APPLY_DAMROLL;
    	af.bitvector = 0;
    	song_affect_to_char( vch, &af );

	send_to_char("You don't feel like fighting.\n\r",vch);
	act("A depressed look passes over $n's face.",vch,NULL,NULL,TO_ROOM);
    }
}

void song_four_seasons( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Ever changing weather marks a cycle of life,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Each passing season will unleash its might.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 2;
    af.modifier  = 0;  
    af.location  = 0;
    af.bitvector = 0;
    song_affect_to_char( ch, &af );
}

void song_disorient( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    AFFECT_DATA af;
    if (sing)
    {
    	sprintf(buf,"Downward spiral, eyes at cross,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Lose thy rival, direction loss.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"With no target, who to choose?");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Soon to forget, destined to lose.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Where to run, what to find?");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Eyes wide shut, senses blind.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Order's bane, Shadowspawns's curse,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Made insane, do its worst.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_area_safe(ch,vch) || IS_NPC(vch))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
        s_yell(ch,vch,FALSE);
	if (vch->fighting == NULL)
	    multi_hit(vch,ch,TYPE_UNDEFINED);
	if (saves_spell(level-6,vch,DAM_OTHER,song_table[sn].spell_type))
	    continue;
	if (is_song_affected(vch,sn))
	    continue;
    	af.where     = TO_AFFECTS;
    	af.type      = sn;
    	af.level     = number_range(0,5);
    	af.duration  = 2;
    	af.modifier  = - level/10;
    	af.location  = APPLY_INT;
    	af.bitvector = 0;
    	song_affect_to_char( vch, &af );
    	af.level     = number_range(0,5);
    	af.location  = APPLY_DEX;
    	song_affect_to_char( vch, &af );
	send_to_char("You feel disoriented...\n\r",vch);
	act("$n looks disoriented.",vch,NULL,NULL,TO_ROOM);
    }
}

void song_stryth( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"The Name unmatched throughout the land,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Who turns the world with a flick of the hand.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"From above the clouds he stares down,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"To abolish his foes with but a frown.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"To him who's heart is midnight's cold,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"T'is maybe too late to save your soul.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Thus I wish to beg his blessing,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"So each day may keep on passing.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	act("You kneel on one foot and give thanks to $g.",ch,NULL,NULL,TO_ALL);
    }
}

void song_soothing( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    int heal = dice(1, 8) + level / 3;
    if (sing)
    {
    	sprintf(buf,"The hidden spring sings to the wind,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Where running nymphs had been.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Let soreness go and wounds subside,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Let life renew within.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (!IS_AWAKE(vch))
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
            if ( vch->hit < vch->max_hit )
	        act_new("The melody makes you feel a little better.", ch,NULL,vch,TO_CHARVICT,POS_DEAD );
	    vch->hit = UMIN( vch->hit + heal, vch->max_hit );
	    update_pos( vch );
	}
    }
}

void song_intoxify( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Let us raise our glasses high,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Never let our cups run dry.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Our mouth resides the taste of ale,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"The stench of alchohol inhaled.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"These feelings I cannot leave behind,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Thoughts that shall intoxify my mind.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (IS_NPC(vch))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
	gain_condition(vch,COND_DRUNK,5);
	send_to_char("Your mind feels lighter.\n\r",vch);	
    }
}

void song_hysteria( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    AFFECT_DATA af;
    if (sing)
    {
    	sprintf(buf,"The feel of shadows move behind,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Waves of chills runs down your spine.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Ghostly voices echo in your head,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Only to find emptiness instead.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Under the bed at night they hide,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Or maybe it is just in your mind.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);

    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_area_safe(ch,vch) || IS_NPC(vch))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
        s_yell(ch,vch,FALSE);
	if (vch->fighting == NULL)
	    multi_hit(vch,ch,TYPE_UNDEFINED);
	if (is_affected(vch,gsn_hysteria))
	    continue;
	if (saves_spell(level-6,vch,DAM_OTHER,song_table[sn].spell_type))
	    continue;
    	af.where     = TO_AFFECTS;
    	af.type      = gsn_hysteria;
    	af.level     = level;
    	af.duration  = 2;
    	af.modifier  = - level/10;
    	af.location  = APPLY_INT;
    	af.bitvector = 0;
    	affect_to_char( vch, &af );
	send_to_char("You start to panic!\n\r",vch);
	act("$n starts to panic!",vch,NULL,NULL,TO_ROOM);
    }
}

void song_titan( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch, *vch_next;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"From the grounds the titans emerge,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"A tremor through the earth it surge.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"The birds all escape by taking flight,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Up and away from the Titan's might.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"As the Titans soon rumble and clash,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"he mountains start to fall and crash.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
	vch_next = vch->next_in_room;
        if ( !is_area_safe(ch,vch))
        {
            s_yell(ch,vch,FALSE);
            if (IS_AFFECTED(vch,AFF_FLYING) && !is_affected(vch,gsn_thrash))
                damage(ch,vch,0,sn + 5000,DAM_BASH,TRUE);
            else if (is_affected(vch,gsn_burrow))
                damage( ch,vch,2*(level + dice(3, 8)), sn + 5000, DAM_BASH,TRUE);
            else
                damage( ch,vch,level + dice(3, 8), sn + 5000, DAM_BASH,TRUE);
        }
    }
}

void song_faeries( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    int heal = dice(2, 9) + level /2 ;
    if (sing)
    {
    	sprintf(buf,"Twilight of faeries fly through the skies,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Granting men's dreams of forbidden sights.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Up above our heads they play away,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Sounds of laughter brightens up the day.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Close your eyes with a gentle kneel,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"With fairy dust our wounds will heal.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    }
    act("Hundreds of faeries attracted by the music dance merrily above you.",ch,NULL,NULL,TO_ALL);
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (vch->master == ch || is_same_group(vch, ch))
	{
            if ( vch->hit < vch->max_hit )
	        act_new("The faerie dust lands on you and heals your wounds.", ch,NULL,vch,TO_CHARVICT,POS_DEAD );
	    vch->hit = UMIN( vch->hit + heal, vch->max_hit );
	    update_pos( vch );
	}
    }
}

void song_condor( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"From high above the clouds I call,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Majestic birds of endless awe.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Accompany us I bid thee respond,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Carry us to unreachable beyonds.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_song_affected(vch,sn) || IS_NPC(vch))
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
    	    af.where     = TO_AFFECTS;
    	    af.type      = sn;
    	    af.level     = level;
    	    af.duration  = 6;
    	    af.modifier  = 0;  
    	    af.location  = 0;
    	    af.bitvector = 0;
    	    song_affect_to_char( vch, &af );
	    act("A large condor arrives and circles over your head, waiting for your bidding.",vch,NULL,NULL,TO_CHAR);
	    act("A large condor arrives and circles over $n's head.",vch,NULL,NULL,TO_ROOM);
	}
    }
}

void song_battle( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"You stand ready on your feet,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Marching rhythmly to the beat.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"The sounds of battle fill your ears,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Fuels the fire to drown your fears.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Rushing foward to your crusade,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Longing for their blood on your blade.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_song_affected(vch,sn) || is_affected(vch,skill_lookup("frenzy")) 
	    || IS_AFFECTED(vch,AFF_BERSERK) || IS_AFFECTED(vch,AFF_CALM))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
    	    af.where     = TO_AFFECTS;
    	    af.type      = sn;
    	    af.level     = level;
    	    af.duration  = level/4;
    	    af.modifier  = level/7;  
    	    af.bitvector = AFF_BERSERK;
    	    af.location  = APPLY_HITROLL;
    	    song_affect_to_char( vch, &af );
    	    af.location  = APPLY_DAMROLL;
    	    song_affect_to_char( vch, &af );
	    send_to_char("You feel your heart beating faster to the rhythm of battle!\n\r",vch);
	    act("$n looks more ready to do battle.",vch,NULL,NULL,TO_ROOM);
	}
    }
}

void song_shield_words( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Words of healing words of pain,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Some words cure and some words maim.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"So as we venture land and sea,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Verses of ancients, protects thee.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_song_affected(vch,sn))
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
    	    af.where     = TO_AFFECTS;
    	    af.type      = sn;
    	    af.level     = level;
    	    af.duration  = level;
    	    af.modifier  = - 20;  
    	    af.location  = APPLY_AC;
    	    af.bitvector = 0;
    	    song_affect_to_char( vch, &af );
	    send_to_char("Verses of the ancients protects you.\n\r",vch);
	    act("$n is surrounded by a shield of words.",vch,NULL,NULL,TO_ROOM);
	}
    }
}

void song_tidings( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"A rabbit's foot, a shooting star,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Good omens for a brand new start.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"A golden horseshoe, a four leaf clover,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Sounds of good tidings ever so closer.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"The future looking ever so bright,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Lady luck, stay by my side tonight.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_song_affected(vch,sn) || IS_NPC(vch))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
    	    af.where     = TO_AFFECTS;
    	    af.type      = sn;
    	    af.level     = level;
    	    af.duration  = level/2;
    	    af.modifier  = UMAX(1,level/10);  
    	    af.location  = APPLY_LUCK;
    	    af.bitvector = 0;
    	    song_affect_to_char( vch, &af );
	    send_to_char("You feel a little luckier.\n\r",vch);
	}
    }
}

void song_hercules( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Born of heaven and endowed with might,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Split the mountain of rushing tides'.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Ten acts of Legend do men recite,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Penance given to set him right.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Now we venture beyond the light,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Let his strength aide our fight.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_song_affected(vch,sn))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
    	    af.where     = TO_AFFECTS;
    	    af.type      = sn;
    	    af.level     = level;
    	    af.duration  = level/2;
    	    af.modifier  = 1 + level/15;  
    	    af.location  = APPLY_STR;
    	    af.bitvector = 0;
    	    song_affect_to_char( vch, &af );
	    send_to_char("Your muscles surge with heightened power!\n\r",vch);
	    act("$n's muscles surge with heightened power.",vch,NULL,NULL,TO_ROOM);
	}
    }
}

void song_burden( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"A guilt so heavy it sinks the heart,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"A journey so gruelsome to depart,");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"A promise so tough to uphold,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"A silent vow not to be told.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Let the pressures of burden manifest,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Added weight to carry on your chest.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
        if ( is_area_safe(ch,vch) || IS_NPC(vch))
	    continue;
        s_yell(ch,vch,FALSE);
	if (vch->fighting == NULL)
   	    multi_hit(vch,ch,TYPE_UNDEFINED);
	if (is_song_affected(vch,sn))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
	if (saves_spell(level-5,vch,DAM_OTHER,song_table[sn].spell_type))
	    continue;
    	af.where     = TO_AFFECTS;
    	af.type      = sn;
    	af.level     = level;
    	af.duration  = level/2;
    	af.modifier  = 2*level;  
    	af.location  = APPLY_WEIGHT;
    	af.bitvector = 0;
    	song_affect_to_char( vch, &af );
	send_to_char("You feel more burdened than usual.\n\r",vch);
	act("$n looks more burdened.",vch,NULL,NULL,TO_ROOM);
    }
}

void song_marriage( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Give your hand to the one you love,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"And sing praises to those above,");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"For giving you someone to hold dear,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"This beautiful day so bright and clear,");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Now at the altar the time is here.");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"You share a bond, a love so true,");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"A spark is felt between the two,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"High on love's wings of joy you flew,");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Witness the souls union at the bell's chime,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
	sprintf(buf,"[`#Bells can be heard ringing in the background``]");
	do_recho(ch,buf);
    	sprintf(buf,"Exchange vowes that lasts the test of time.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
}

void song_mourn( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch, *t_vch;
    OBJ_DATA *obj;
    AFFECT_DATA af;
    char buf[MSL], arg[MIL];
    static char * const his_her [] = { "its", "his", "her" };
    one_argument(argument,arg);
    if ( arg[0]=='\0' )
    {
        send_to_char("What do you want to mourn over?\n\r",ch);
        return;
    }
    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    if (obj->item_type != ITEM_CORPSE_PC)
    {
        send_to_char("Why would you mourn over that?.\n\r",ch);
        return;
    }
    for (t_vch = player_list; t_vch != NULL; t_vch = t_vch->next_player)
        if (t_vch->id == obj->owner)
            break;
    if (t_vch == NULL)
    {
	send_to_char("It is too late to mourn over that corpse.\n\r",ch);
	return;
    }
    if (sing)
    {
    	sprintf(buf,"At this place where %s has fell,",PERS2(t_vch));
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Deep in our hearts a place is held,");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"For our companion who fought so well,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Yet bound by the fate of death's bell.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Over %s corpse mourn our farewells,", his_her[URANGE(0,t_vch->sex,2)]);
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Let %s spirit inside us dwell.", his_her[URANGE(0,t_vch->sex,2)]);
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_song_affected(vch,sn) || IS_NPC(vch) || vch == t_vch)
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
	if (is_same_group(vch, t_vch))
	{
    	    af.where     = TO_AFFECTS;
    	    af.type      = sn;
    	    af.level     = vch->level;
    	    af.duration  = vch->level*2;
    	    af.modifier  = 3*vch->level/2;  
    	    af.location  = APPLY_HIT;
    	    af.bitvector = 0;
    	    song_affect_to_char( vch, &af );
    	    af.modifier  = - vch->level*2;  
    	    af.location  = APPLY_AC;
    	    song_affect_to_char( vch, &af );
	    vch->hit += 3*vch->level/2;
	    send_to_char("Seeing the fall of your comrade strengthens your will to continue living.\n\r",vch);
	}
	
    }
}

void song_rest( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Life so sweet and bitter death,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Both of which you've had a taste.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Rotting flesh of forsaken soul,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Neither of which yours to control.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"The final rest a step away.");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Close your eyes with this breath.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (!IS_SET(vch->act,ACT_UNDEAD) || IS_AFFECTED(vch,AFF_SLEEP))
	    continue;
	if (vch->fighting != NULL)
	    continue;
	if (is_area_safe(ch,vch))
	    continue;
        s_yell(ch,vch,TRUE);
	if (saves_spell(level-5,vch,DAM_OTHER,song_table[sn].spell_type))
	{
	    if (vch->fighting == NULL)
   	    	multi_hit(vch,ch,TYPE_UNDEFINED);
	    continue;
	}
    	af.where     = TO_AFFECTS;
    	af.type      = sn;
    	af.level     = level;
    	af.duration  = number_fuzzy(3);
    	af.modifier  = 0;  
    	af.location  = 0;
    	af.bitvector = AFF_SLEEP;
    	song_affect_to_char( vch, &af );

	if (is_affected(ch,gsn_insomnia))
	    affect_strip(ch,gsn_insomnia);

        if ( IS_AWAKE(vch) ){
	  send_to_char( "Your eyelids feel heavy and you drift into a peaceful slumber.\n\r", vch);
	  do_sleep(vch,"");
	  act("$n is drained and falls asleep.",vch,NULL,NULL,TO_ROOM);
        }
	else
	  act("$n falls into a deep slumber.",vch,NULL,NULL,TO_ROOM);
    }
}

void song_phoenix( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *mob;
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Sleeping deep within the earth,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Waiting dormant for rebirth.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Rise from your resting chamber,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Let my song wake your slumber.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Out of the ashes of the old,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Manifest your form as foretold.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    if (get_summoned(ch, MOB_VNUM_PHOENIX) > 0)
    {
        send_to_char("Your phoenix is already in existence.\n\r",ch);
        return;
    }
    act ("The ground erupts, giving birth to a baby phoenix!", ch, NULL, NULL, TO_ALL);
    mob = create_mobile( get_mob_index( MOB_VNUM_PHOENIX ) );
    char_to_room(mob,ch->in_room);
    SET_BIT(mob->affected_by, AFF_CHARM);
    mob->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
    mob->summoner = ch;
    add_follower(mob,ch);
    mob->leader=ch;
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 100;
    af.modifier  = 0;  
    af.location  = 0;
    af.bitvector = 0;
    song_affect_to_char( mob, &af );
}

void song_eman( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch, *victim;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Blood and sweat you have laid,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Iron chains once bound your fate.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Standing together we break free,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Never again do another's deed.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (!IS_AFFECTED(vch,AFF_CHARM) || (victim = vch->master) == NULL)
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
	if (is_safe(ch,victim))
	    continue;
        s_yell(ch,victim,TRUE);
	if (saves_spell(level-6,victim,DAM_OTHER,song_table[sn].spell_type))
	{
	    if (victim->fighting == NULL)
   	    	multi_hit(victim,ch,TYPE_UNDEFINED);
	    continue;
	}
	affect_strip(vch,gsn_charm_person);
	REMOVE_BIT(vch->affected_by,AFF_CHARM);
	stop_follower(vch);
	if ((IS_NPC(vch) && IS_SET(vch->act2,ACT_DUPLICATE)) || IS_SET(vch->act,ACT_UNDEAD))
	{
	    act("You are freed from captivity!",vch,NULL,NULL,TO_CHAR);
	    act("$n is freed from captivity!",vch,NULL,NULL,TO_ROOM);
	    if (IS_NPC(vch))
	    {
		char_from_room(vch);
		extract_char(vch,TRUE);
	    }
	}
	else
	{
	    act("$n is freed from captivity and turns against $N!",vch,NULL,victim,TO_NOTVICT);
	    act("You are freed from captivity and turn against $N!",vch,NULL,victim,TO_CHAR);
	    act("$n is freed from captivity and turns against you!",vch,NULL,victim,TO_VICT);
	    multi_hit(vch,victim,TYPE_UNDEFINED);	
	}
    }
}

void song_laughter( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Forget about your troubles and pains,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Why let your life be bound in chains.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Let your smile lift your spirit,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Laugh out loud for all to hear it.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"With this you'll get the cure you're after,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Because the most effective medicine is laughter.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (!IS_AWAKE(vch))
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
	    act("The song brings a laughter to your face.",vch,NULL,NULL,TO_CHAR);
	    act("The song brings a laughter to $n's face.",vch,NULL,NULL,TO_ROOM);
	    if (!is_affected(vch,gsn_plague))
	    	continue;
	    if (check_dispel(level-3,vch,gsn_plague))
	    {
	        act_new("Your sores vanish.",vch,NULL,NULL,TO_CHAR,POS_DEAD);
	        act("$n looks relieved as $s sores vanish.",vch,NULL,NULL,TO_ROOM);
	    }
	}
    }
}

void song_spider( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch, *vch_next, *mob, *mob2;
    AFFECT_DATA af;
    char buf[MSL];
    int i = 0;
    if (sing)
    {
    	sprintf(buf,"In the darkest corners they build their nest,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Anticipating an unsuspecting guest.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"With preying eyes upon you they spy,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Venom dripping for the tasty fly.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Descending around you they converge,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Feel the sting of black widow's scourge");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    if (get_summoned(ch, MOB_VNUM_SPIDER) > ch->level/10)
    {
	send_to_char("You've summoned as many as you can!\n\r",ch);
	return;
    }

    mob = create_mobile( get_mob_index(MOB_VNUM_SPIDER) );
    for (i = 0; i < MAX_STATS; i++)
        mob->perm_stat[i] = UMIN(25, (ch->level / 10) + 15);
    mob->max_hit = ch->level;
    mob->hit = mob->max_hit;
    mob->level = ch->level;
    for (i = 0; i < 3; i++)
        mob->armor[i] = interpolate(mob->level/2,100,-100);
    mob->armor[3] = interpolate(mob->level/2,100,0);
    mob->damroll    = (ch->level /10);
    mob->damage[DICE_NUMBER]  = (ch->level /12);
    mob->damage[DICE_TYPE]    = 3;
    mob->sex = number_range(1,2);
    mob->comm = COMM_NOTELL|COMM_NOYELL|COMM_NOCHANNELS;
    mob->summoner = ch;
    af.where     = TO_AFFECTS;
    af.type      = gsn_timer;
    af.level     = level;
    af.duration  = 3;
    af.modifier  = 0;  
    af.location  = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char( mob, &af );

    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
	int number = number_range(1,2);
	vch_next = vch->next_in_room;
        if (is_area_safe(ch,vch))
	    continue;
	if (IS_NPC(vch) && vch->pIndexData->vnum == MOB_VNUM_SPIDER)
	    continue;
        s_yell(ch,vch,FALSE);
	if (vch->fighting == NULL)
	    multi_hit(vch,ch,TYPE_UNDEFINED);
	for (i = 0;i < number;i++)
	{
	    mob2 = create_mobile(mob->pIndexData);
	    clone_mobile(mob,mob2);
	    mob2->summoner = ch;
	    mob2->master = ch;
	    SET_BIT(mob2->affected_by, AFF_CHARM);
	    char_to_room(mob2,vch->in_room);
	    act("$N drops in from above and starts attacking you!",vch,NULL,mob2,TO_CHAR);
	    act("$N drops in from above and starts attacking $n!",vch,NULL,mob2,TO_ROOM);
	    set_fighting(mob2,vch);
	}
    }
    extract_char(mob,TRUE);
}

void song_unicorn( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    int temp = 0;
    if (sing)
    {
    	sprintf(buf,"Travel the distant lands far and wide,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"We search for mystery in great stride.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"A fair steed with wild and fiery mane,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Whose enigma has sparked its fame.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"For those who find this fabled beast,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Its mysteries will be released.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"The secret is abound within its horn,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Alas we have found the great unicorn.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/4;
    af.modifier  = - level/3;
    af.bitvector = 0;
    temp = number_range(0,2);
    switch (temp)
    {
	default:
    	    af.location  = APPLY_SAVING_SPELL;
	    break;
	case (0):
    	    af.location  = APPLY_SAVING_AFFL;
	    break;
	case (1):
    	    af.location  = APPLY_SAVING_MALED;
	    break;
	case (2):
    	    af.location  = APPLY_SAVING_MENTAL;
	    break;
    }
    act("A unicorn runs through, filling the air with mystic energy.",ch,NULL,NULL,TO_ALL);
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_song_affected(vch,sn) || IS_NPC(vch))
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
    	    song_affect_to_char( vch, &af );
	    send_to_char("You feel protected by the unicorn's mystic energy.\n\r",vch);
	}
    }
}

void song_accuracy( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Accuracy song...");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_song_affected(vch,sn) || IS_NPC(vch))
	    continue;
	if (vch->master == ch || is_same_group(vch, ch))
	{
    	    af.where     = TO_AFFECTS;
    	    af.type      = sn;
    	    af.level     = level;
    	    af.duration  = 6;
    	    af.modifier  = 0;  
    	    af.location  = 0;
    	    af.bitvector = 0;
    	    song_affect_to_char( vch, &af );
	    send_to_char("Your mind is in full concentration.\n\r",vch);
	}
    }
}

void song_shriek( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch, *vch_next;
    int dam = 0;
    char buf[MSL];
    sprintf(buf,"%s",random_garble(level));
    if (sing)
    {
    	if (duet && ch->pcdata->duet != NULL)
    	    duet_song_to_room(ch,ch->pcdata->duet,buf,FALSE,FALSE);
    	else
    	    song_to_room(ch,buf,FALSE,FALSE,FALSE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
	vch_next = vch->next_in_room;
        if ( vch->in_room == NULL )
            continue;
        if ( vch->in_room == ch->in_room )
        {
	  if ( !is_area_safe(ch,vch))
            {
                s_yell(ch,vch,FALSE);
		dam = strlen(buf) + UMAX(5,UMIN(15,number_range(level/4,level/2)));

		if (saves_spell(level,vch,DAM_SOUND,song_table[sn].spell_type))
		    dam /= 2;
		//deafen if vuln to sound. (damage or spell)
		if (check_immune(vch,DAM_SOUND, TRUE) == IS_VULNERABLE
		    || check_immune(vch,DAM_SOUND,FALSE) == IS_VULNERABLE)
		  spell_silence(gsn_silence, level + 5, ch, vch, TARGET_CHAR );
		damage(ch,vch,dam,sn + 5000,DAM_SOUND,TRUE);
		act("Your eardrums rupture from $n's shriek.",ch,NULL,vch,TO_VICT);
             }
        }
    }
}

void song_memories( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"A distant memory locked away,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Seems like a dream from yesterday.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"A thought in fragments you almost grasped,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Perhaps a clue that reveals your past.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Heed my song and the nightmare will end,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Let your memory surface once again.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (vch->master == ch || is_same_group(vch, ch))
	{
	    if (!IS_AWAKE(vch))
	    	continue;
	    if (check_dispel(level-2,vch,gsn_forget))
	    {
	    	act("The song brings back your memories.",vch,NULL,NULL,TO_CHAR);
	    	act("The song brings a familiar look to $n's face.",vch,NULL,NULL,TO_ROOM);
	    }
	    if (check_dispel(level-2,vch,gsn_hysteria))
	    {
	    	act("The song brings back your senses.",vch,NULL,NULL,TO_CHAR);
	    	act("The song brings a relaxed look to $n's face.",vch,NULL,NULL,TO_ROOM);
	    }
	}
    }
}

void song_leviathon( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    OBJ_DATA *spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), 0);
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Ancient creature that lives beneath,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Who nurtures the life we receive.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Here we stand dried of mouth,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Without a river north or south.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Bring the gift of infinite worth,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"The sweet essence of all birth.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    spring->timer = 1;
    obj_to_room( spring, ch->in_room );
    act("You summon the Leviathon, leaving springs flowing from the ground in its path.",ch,NULL,NULL,TO_CHAR);
    act("$n summons the Leviathon, leaving springs flowing from the ground in its path.",ch,NULL,NULL,TO_ROOM);
}

void song_serenade( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"My heart aches when you are not here,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"There is nothing else I hold dear.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"My mind has never been so clear,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"I always want you to stay near.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"When with me you'll never have to fear,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"I will never let you shed a tear.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"On my knees to show that I'm sincere,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"A love that hold trues throughout the years.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
}

void song_lust( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Lust, Seduction, and Desire,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Your heart beats faster, your loin's on fire.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Unlock the urge that sleeps within,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Fall to the temptations of mortal sin.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Enjoy a feeling so refresh,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Indulge in the pleasures of the flesh.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);

    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (is_song_affected(vch,sn))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
	if (IS_IMMORTAL(vch))
	    continue;
	if (saves_spell(vch->level-3,vch,DAM_OTHER,song_table[sn].spell_type))
	    continue;
    	af.where     = TO_AFFECTS;
    	af.type      = sn;
    	af.level     = level;
    	af.duration  = 6;
    	af.modifier  = 0;  
    	af.location  = 0;
    	af.bitvector = 0;
    	song_affect_to_char( vch, &af );
    }
}

void song_feast( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    OBJ_DATA *obj;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Here I stand to prepare a feast,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"A sight so grand to say the least.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Ingredients I use of the old,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Enhancing flavor to a taste so bold,");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"ealing in the freshness of spring,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"A meal truly fitting for a king.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    }
    for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
    	if (obj->item_type != ITEM_FOOD)
	    continue;
    	if (obj->wear_loc != -1)
	    continue;
	if (obj->value[1] > 40 || obj->timer > 99)
	    continue;
	act("$p looks fit for a king!",ch,obj,NULL,TO_CHAR);
	obj->value[1] = 48;
	obj->timer = 100;
    }
}

void song_reputation( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Let it be known throughout the land,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"For I am the greatest, the most grand.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Let it be known and all take heed,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Young and old know my heroic deeds.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    	sprintf(buf,"Let it be known with all my glory,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Ages to come will tell my story.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Let it be known my ample fame,");
    	song_to_room(ch,buf,TRUE,FALSE,FALSE);
    	sprintf(buf,"Man and beast do speak my name.");
    	song_to_room(ch,buf,TRUE,FALSE,TRUE);
    }
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 24;
    af.modifier  = 0;  
    af.location  = 0;
    af.bitvector = 0;
    song_affect_to_char( ch, &af );
    send_to_char("Your reputation is well known throughout the lands.\n\r",ch);
}

void song_victory( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
  //    CHAR_DATA *vch;
//    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Oh great beauty, oh lovely sky,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"On watch duty, sitting up high.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Victory Goddess, grant us fate,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"This I promise, homage of great.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Give us a boon, from the stars,");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"With this tune, Victory be ours.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }

}

void song_unforgiven( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    AFFECT_DATA af;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Unforgiven the gods turn away.");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Unforgiven the deaths you lay.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Unforgiven the game you play.");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Unforgiven those you betray.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Unforgiven the sin this day.");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Unforgiven the curse shall stay.");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (IS_NPC(vch) || !is_area_safe(ch,vch) || !is_affected(vch,gsn_curse))
	    continue;
	if (!IS_AWAKE(vch))
	    continue;
	s_yell(ch,vch,FALSE);
	if (vch->fighting == NULL)
	    multi_hit(vch,ch,TYPE_UNDEFINED);
	if (is_song_affected(vch,sn))
	    continue;
	if (saves_spell(level-5,vch,DAM_OTHER,song_table[sn].spell_type))
	    continue;
    	af.where     = TO_AFFECTS;
    	af.type      = sn;
    	af.level     = level;
    	af.duration  = 6;
    	af.modifier  = 0;  
    	af.location  = 0;
    	af.bitvector = 0;
    	song_affect_to_char( vch, &af );
	send_to_char("Your curse is unforgivable!.\n\r",vch);
    }
}

void song_waterwine( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    OBJ_DATA *obj;
    int liquid = 0;
    char buf[MSL], arg[MIL];
    one_argument(argument,arg);
    if ( arg[0]=='\0' )
    {
        send_to_char("Syntax: sing 'water to wine' <drink container>\n\r",ch);
        return;
    }
    obj = get_obj_list( ch, arg, ch->carrying );
    if ( obj == NULL )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    if ( obj->wear_loc != WEAR_NONE )
    {
	send_to_char( "It must be in your inventory.\n\r",ch);
	return;
    }
    if (obj->item_type != ITEM_DRINK_CON)
    {
        send_to_char("That isn't a drink container.\n\r",ch);
        return;
    }
    if ( obj->value[1] <= 0 )
    {
        send_to_char( "It is already empty.\n\r", ch );
        return;
    }
    liquid = obj->value[2];
    if ( liquid != 0 )
    {
        send_to_char("You can only turn pure water into wine.\n\r",ch);
        return;
    }
    if (sing)
    {
    	sprintf(buf,"From a liquid so clear and pure");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"Add in the ages to mature");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    	sprintf(buf,"Sprinkle a dash of morning dew");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    	sprintf(buf,"A concoction of the finest brew");
    	song_to_room(ch,buf,FALSE,FALSE,TRUE);
    }
    switch (number_range(0,4))
    {
	case (0):
	    obj->value[2] = 3;
	    break;
	case (1):
	    obj->value[2] = 18;
	    break;
	case (2):
	    obj->value[2] = 23;
	    break;
	case (3):
	    obj->value[2] = 31;
	    break;
	case (4):
	    obj->value[2] = 35;
	    break;
    }		
    obj->value[3] = 0;
    act("The water in $p slowly changes color.",ch,obj,NULL,TO_ALL);
}

/*
void song_format( int sn, int level, CHAR_DATA *ch, bool duet, bool sing, char *argument)
{
    CHAR_DATA *vch;
    char buf[MSL];
    if (sing)
    {
    	sprintf(buf,"Lyrics...");
    	song_to_room(ch,buf,FALSE,FALSE,FALSE);
    }
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
    }
}
*/

