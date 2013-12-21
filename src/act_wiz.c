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
#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "s_skill.h"
#include "recycle.h"
#include "interp.h"
#include "magic.h"
#include "cabal.h"
#include "vote.h"
#include "tome.h"
#include "bounty.h"
#include "dndspell.h"
#include "comm.h"
#include "clan.h"

extern void show_flag_cmds( CHAR_DATA *ch, const struct flag_type *flag_table );
extern char *flag_string               args ( ( const struct flag_type *flag_table, int bits ) );
extern bool hide_note(CHAR_DATA *ch, NOTE_DATA *pnote);
extern void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote, bool delete);
extern bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote );
extern NOTE_DATA *application_list;
extern void trap_update();

void do_wepfind( CHAR_DATA *ch, char *argument );


void do_wiznet( CHAR_DATA *ch, char *argument )
{
    int flag;
    char buf[MSL];
    if ( argument[0] == '\0' )
    {
      	if (IS_SET(ch->wiznet,WIZ_ON))
      	{
            send_to_char("Signing off of Wiznet.\n\r",ch);
            REMOVE_BIT(ch->wiznet,WIZ_ON);
      	}
      	else
      	{
            send_to_char("Welcome to Wiznet!\n\r",ch);
            SET_BIT(ch->wiznet,WIZ_ON);
      	}
      	return;
    }
    if (!str_prefix(argument,"on"))
    {
	send_to_char("Welcome to Wiznet!\n\r",ch);
	SET_BIT(ch->wiznet,WIZ_ON);
	return;
    }
    if (!str_prefix(argument,"off"))
    {
	send_to_char("Signing off of Wiznet.\n\r",ch);
	REMOVE_BIT(ch->wiznet,WIZ_ON);
	return;
    }
    if (!str_prefix(argument,"status")) 
    {
	buf[0] = '\0';
	if (!IS_SET(ch->wiznet,WIZ_ON))
	    strcat(buf,"off ");
	for (flag = 0; wiznet_table[flag].name != NULL; flag++)
	    if (IS_SET(ch->wiznet,wiznet_table[flag].flag))
	    {
		strcat(buf,wiznet_table[flag].name);
		strcat(buf," ");
	    }
	strcat(buf,"\n\r");
	send_to_char("Wiznet status:\n\r",ch);
	send_to_char(buf,ch);
	return;
    }
    if (!str_prefix(argument,"show"))
    {
	buf[0] = '\0';
	for (flag = 0; wiznet_table[flag].name != NULL; flag++)
	    if (wiznet_table[flag].level <= get_trust(ch))
	    {
	    	strcat(buf,wiznet_table[flag].name);
	    	strcat(buf," ");
	    }
	strcat(buf,"\n\r");
	send_to_char("Wiznet options available to you are:\n\r",ch);
	send_to_char(buf,ch);
	return;
    }
    flag = wiznet_lookup(argument);
    if (flag == -1 || get_trust(ch) < wiznet_table[flag].level)
    {
	send_to_char("No such option.\n\r",ch);
	return;
    }
    if (IS_SET(ch->wiznet,wiznet_table[flag].flag))
    {
        sendf(ch,"You will no longer see %s on wiznet.\n\r", wiznet_table[flag].name);
	REMOVE_BIT(ch->wiznet,wiznet_table[flag].flag);
    	return;
    }
    else
    {
        sendf(ch,"You will now see %s on wiznet.\n\r", wiznet_table[flag].name);
    	SET_BIT(ch->wiznet,wiznet_table[flag].flag);
	return;
    }
}

void wiznet(char *string, CHAR_DATA *ch, OBJ_DATA *obj,long flag, long flag_skip, int min_level) 
{
    char buf[MSL], newtime[9];
    char *strtime = (char *) ctime( &mud_data.current_time );
    DESCRIPTOR_DATA *d;
    int pos = 0, i = 1;
    do
    {
	if (i > 11)
            buf[pos++] = *strtime;
    }
    while (*strtime++ && i++ && pos < 8);
    buf[pos] = '\0';
    strcpy(newtime, buf);
    sprintf(buf,"`&W`8i`&ZNET`8 ``%s`8: ``%s", newtime, string);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->connected == CON_PLAYING && IS_IMMORTAL(d->character) && IS_SET(d->character->wiznet,WIZ_ON) 
        && (!flag || IS_SET(d->character->wiznet,flag)) && (!flag_skip || !IS_SET(d->character->wiznet,flag_skip))
        && get_trust(d->character) >= min_level && d->character != ch)
	    act_new(buf,d->character,obj,ch,TO_CHAR,POS_DEAD);
    }
}

void do_doof( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL];
    CHAR_DATA *victim;
    argument = one_argument( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: forsake <char>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Why would you want to forsaken a mob?\n\r",ch);
	return;
    }
    SET_BIT(victim->act, PLR_DOOF);
    sendf(ch, "%s has been Forsaken.\n\r", PERS2(victim));
    send_to_char( "You have been Forsaken by the Gods.\n\r",victim);
}


// set the most on counter (IMM only)

void do_most( CHAR_DATA *ch, char *argument )
{
  int prev = mud_data.max_on;
  mud_data.max_on = atoi (argument);
  sendf(ch, "Previous Maximum on: %d\n\rNew Maximum on: %d\n\r", prev, mud_data.max_on);
}


// toggle the coding flag in the title (IMM ONLY)

void do_coding( CHAR_DATA *ch, char *argument )
{

/* if immortal and coding, turn coding off */
  if ( IS_IMMORTAL(ch) && IS_SET(ch->act2, PLR_CODING)) {
    REMOVE_BIT(ch->act2, PLR_CODING);
    sendf(ch, "You are no longer CODING.\n\r");
  }

/* else if immortal, set coding */
  else if ( IS_IMMORTAL(ch)) {
    SET_BIT(ch->act2, PLR_CODING);
    sendf(ch, "You are now CODING.\n\r");
  }
}

void do_plag( CHAR_DATA *ch, char *argument )
{
    int lagtime = 120;
    char arg1[MIL], arg2[MIL];
    CHAR_DATA *victim;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: plag <char> <(optional) seconds>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Why would you want to lag a mob?\n\r",ch);
	return;
    }
    if ( arg2[0] != '\0' )
	lagtime = atoi(arg2) * 4;	
    WAIT_STATE(victim,lagtime);
    sendf(ch, "%s is experiencing some technical difficulties.\n\r",PERS2(victim)); 
}

void do_nochannels( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Nochannel whom?", ch );
        return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    if ( get_trust( victim ) > get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }
    if ( IS_SET(victim->comm, COMM_NOCHANNELS) )
    {
        REMOVE_BIT(victim->comm, COMM_NOCHANNELS);
        send_to_char( "The gods have restored your channel priviliges.\n\r",victim );
        sendf(ch, "NOCHANNELS removed from %s.\n\r", PERS2(victim) );
	sprintf(buf,"$N restores channels to %s",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
    else
    {
        SET_BIT(victim->comm, COMM_NOCHANNELS);
        send_to_char( "The gods have revoked your channel priviliges.\n\r", victim );
        sendf(ch, "NOCHANNELS set on %s.\n\r", PERS2(victim) );
	sprintf(buf,"$N revokes %s's channels.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
}


void do_bamfin( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) )
    {
	smash_tilde( argument );
	if (argument[0] == '\0')
	{
	    sendf(ch,"Your poofin is %s\n\r",ch->pcdata->bamfin);
	    return;
	}
	if ( strstr(argument,ch->name) == NULL)
	{
	    send_to_char("You must include your name.\n\r",ch);
	    return;
	}
	free_string( ch->pcdata->bamfin );
	ch->pcdata->bamfin = str_dup( argument );
        sendf(ch,"Your poofin is now %s\n\r",ch->pcdata->bamfin);
    }
}

void do_bamfout( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) )
    {
        smash_tilde( argument );
        if (argument[0] == '\0')
        {
            sendf(ch,"Your poofout is %s\n\r",ch->pcdata->bamfout);
            return;
        }
        if ( strstr(argument,ch->name) == NULL)
        {
            send_to_char("You must include your name.\n\r",ch);
            return;
        }
        free_string( ch->pcdata->bamfout );
        ch->pcdata->bamfout = str_dup( argument );
        sendf(ch,"Your poofout is now %s\n\r",ch->pcdata->bamfout);
    }
}

void do_deny( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Deny whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) > get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    SET_BIT(victim->act, PLR_DENY);
    send_to_char( "You are denied access!\n\r", victim );
    sprintf(buf,"$N denies access to %s",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    sendf(ch, "You have denied %s.\n\r", PERS2(victim));

    if (victim->pcdata->pbounty){
      rem_bounty( victim->pcdata->pbounty );
      free_bounty( victim->pcdata->pbounty );      
    }
    save_char_obj(victim);
    stop_fighting(victim,TRUE);
    do_quit( victim, "" );
}

void do_disconnect( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Disconnect whom?\n\r", ch );
	return;
    }
    if (is_number(arg))
    {
	int desc;
	desc = atoi(arg);
    	for ( d = descriptor_list; d != NULL; d = d->next )
            if ( d->descriptor == desc )
            {
            	sendf(ch, "Disconnected %d\n\r", desc);
            	close_socket( d );
            	return;
            }
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim->desc == NULL )
    {
	sendf(ch, "%s does not have a descriptor.\n\r", arg);
	return;
    }
    for ( d = descriptor_list; d != NULL; d = d->next )
	if ( d == victim->desc )
	{
	    sendf(ch, "Disconnected %s\n\r", arg );
	    close_socket( d );
	    return;
	}
    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char( "Descriptor not found!\n\r", ch );
}

void do_pardon( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL];
    CHAR_DATA *victim;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: pardon <character> <wanted>.\n\r", ch );
        send_to_char( "Syntax: pardon <character> <forsaken>.\n\r"\
		      "Syntax: pardon <character> <pk>.\n\r"\
		      "Syntax: pardon <character> <stain>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( IS_NPC(ch) )
    {
	send_to_char( "Not by NPC's.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg2, "wanted" ) )
    {
        if ( IS_SET(victim->act, PLR_WANTED) )
	{
            REMOVE_BIT( victim->act, PLR_WANTED );
            sendf(ch, "%s is no longer WANTED.\n\r", PERS2(victim) );
            send_to_char( "You are no longer WANTED.\n\r", victim );
	}
	return;
    }
    if ( !str_cmp( arg2, "forsaken" ) )
    {
        if ( IS_SET(victim->act, PLR_DOOF) )
	{
            REMOVE_BIT( victim->act, PLR_DOOF );
            sendf(ch, "%s is no longer Forsaken.\n\r", PERS2(victim) );
            send_to_char( "You have regained the favor of the Gods.\n\r", victim );
	}
	return;
    }
    if ( !str_cmp( arg2, "pk" ) )
    {
      if ( IS_GAME(victim, GAME_PKWARN) ){
	REMOVE_BIT( victim->game, GAME_PKWARN );
	sendf(ch, "%s is no longer marked PK.\n\r", PERS2(victim) );
	send_to_char( "You have been pardoned of your trashy acts.\n\r", victim );
      }
      return;
    }
    else if (!str_cmp(arg2, "stain")){
      AFFECT_DATA af;
      AFFECT_DATA* paf;
      int sn = skill_lookup("pardon");
      
      if (IS_NPC(ch)){
	send_to_char("Not mobs\n\r", ch);
	return;
      }
      if (!ch->desc){
	send_to_char("No while link dead\n\r", ch);
	return;
      }
      if (!IS_GAME(victim, GAME_STAIN)){
	send_to_char("That person is not <`!STAINED``>\n\r", ch);
	return;
      }
      if ( (paf = affect_find(victim->affected, sn)) == NULL){
	char buf[MIL];
	af.type = sn;
	af.level = 0;
	af.duration = 5;
	af.where = TO_NONE;
	af.bitvector = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	paf = affect_to_char(victim, &af);
	string_to_affect(paf, ch->name);
	sprintf(buf, "In order for STAIN to be cleared another immortal must use \"pardon\" command on %s within 5 ticks.\n\r", victim->name);
	wiznet(buf, ch, NULL,WIZ_DEATHS,WIZ_SECURE, LEVEL_HERO);
	send_to_char(buf, ch);
	sprintf(buf, "%s attempted to pardon %s\n\r", ch->name, victim->name);
	log_string(buf);
	return;
      }
      if (paf->has_string && str_cmp(paf->string, ch->name)){
	OBJ_DATA* obj;
	CHAR_DATA* vch;
	char buf[MIL];

	/* get original pardon character to verify ip's */
	if ( (vch = get_char(paf->string)) == NULL){
	  send_to_char("Original pardon owner not found. Aborting...\n\r", ch);
	  return;
	}
	/* check ip's */
	if (!str_cmp(vch->desc->ident, ch->desc->ident)){
	  send_to_char("Your ip address matches original pardon.  Aborting...\n\r", ch);
	  return;
	}

	sprintf(buf, "Stain removed from %s\n\r", victim->name);
	wiznet(buf, ch, NULL,WIZ_DEATHS,WIZ_SECURE, LEVEL_HERO);
	send_to_char(buf, ch);
	sprintf(buf, "%s: Stain removed on %s.", ch->name, victim->name);
	log_string(buf);
	/* strip affects from pardon */
	affect_strip(victim, sn);

	/* remove stains on object char. has */
	REMOVE_BIT(victim->game, GAME_STAIN);
	for ( obj = victim->carrying; obj != NULL; obj = obj->next_content ){
	    REMOVE_BIT(obj->extra_flags, ITEM_STAIN);
	  }
      }
      else{
	send_to_char("You need two different Immortals to remove the stain.\n\r", ch);
	return;
      }
      return;
    }
    do_pardon(ch, "");
}

void do_echo( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    if ( argument[0] == '\0' )
    {
	send_to_char( "Global echo what?\n\r", ch );
	return;
    }
    for ( d = descriptor_list; d; d = d->next )
	if ( d->connected == CON_PLAYING )
	{
            if (get_trust(d->character) >= get_trust(ch))
		send_to_char( "global> ",d->character);
	    sendf( d->character, "%s\n\r", argument );
	}
}

//echos in an area
void aecho( AREA_DATA* area, char* echo ){
  DESCRIPTOR_DATA *d;

  for ( d = descriptor_list; d; d = d->next ){
    if ( d->connected == CON_PLAYING 
	 && d->character->in_room->area
	 && d->character->in_room->area == area
	 && IS_AWAKE(d->character)){
	send_to_char(echo,d->character);
    }
  }
}

//echos to playes in same area as ch
void do_zecho( CHAR_DATA *ch, char *argument )
{
  DESCRIPTOR_DATA *d;
    if ( argument[0] == '\0' )
    {
	send_to_char( "Global echo what?\n\r", ch );
	return;
    }
    for ( d = descriptor_list; d; d = d->next )
	if ( d->connected == CON_PLAYING && d->character->in_room->area
	     && d->character->in_room->area == ch->in_room->area)
	  {
            if (get_trust(d->character) >= get_trust(ch))
	      send_to_char( "global> ",d->character);
	    sendf( d->character, "%s\n\r", argument );
	  }
}

void do_immecho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    if ( argument[0] == '\0' )
    {
	send_to_char( "Global echo what?\n\r", ch );
	return;
    }
    for ( d = descriptor_list; d; d = d->next )
	if ( d->connected == CON_PLAYING )
	{
	    if (!IS_IMMORTAL(d->character))
		continue;
            if (get_trust(d->character) >= get_trust(ch))
		send_to_char( "global> ",d->character);
	    sendf( d->character, "%s\n\r", argument );
	}
}

ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    if ( is_number(arg) )
        return (atoi(arg) < 0 ? NULL : get_room_index( atoi( arg ) ) );
    if ( ( victim = get_char_world( ch, arg ) ) != NULL )
	return victim->in_room;
    if ( ( obj = get_obj_world( ch, arg ) ) != NULL )
	return obj->in_room;
    return NULL;
}

/* Written by: Virigoth							*
 * Returns: void							*
 * Used: act_wiz.c,							*
 * Comment: trans target AND his charmies				*/
void do_gtransfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL];
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim, *vch, *vch_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );


    if ( arg1[0] == '\0' )
    {
	send_to_char( "Transfer whom (and where)?\n\r", ch );
	return;
    }

    if ( arg2[0] == '\0' )
	location = ch->in_room;
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No such location.\n\r", ch );
	    return;
	}
        if ( room_is_private( location ) &&  get_trust(ch) < MAX_LEVEL)
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
        if ( !have_access(ch,location))
        {
            send_to_char( "Don't think so.\n\r",ch);
            return;
        }
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim->in_room == NULL )
    {
	sendf( ch, "%s is in limbo.\n\r", PERS2(victim) );
	return;
    }
    /* We move the charmies/followers */
    for (vch = victim->in_room->people; vch != NULL; vch = vch_next){
      vch_next = vch->next_in_room;

      if (vch->master != victim)
	continue;
      if ( vch->fighting != NULL )
	continue;
      if (!IS_NPC(vch))
	vch->pcdata->transnum = vch->in_room->vnum;
      if ( ch->desc )
	act( "$n disappears in a mushroom cloud.", vch, NULL, NULL, TO_ROOM );
      char_from_room( vch );
      char_to_room( vch, location );
      if ( ch->desc )
        act( "$n arrives from a puff of smoke.", vch, NULL, NULL, TO_ROOM );
      else
	act( "$n has arrived.", vch, NULL, NULL, TO_ROOM );
      if ( ch != vch && ch->desc )
	act( "$n has transferred you.", ch, NULL, vch, TO_VICT );
      do_look( vch, "auto" );
      sendf( ch, "You have transferred %s.\n\r", PERS2(vch) );
    }

    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    if (!IS_NPC(victim))
	victim->pcdata->transnum = victim->in_room->vnum;
    if ( ch->desc )
	act( "$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    if ( ch->desc )
        act( "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM );
    else
	act( "$n has arrived.", victim, NULL, NULL, TO_ROOM );
    if ( ch != victim && ch->desc )
	act( "$n has transferred you.", ch, NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    sendf( ch, "You have transferred %s.\n\r", PERS2(victim) );
}


void do_transfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL];
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim, *player;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );


    if ( arg1[0] == '\0' )
    {
	send_to_char( "Transfer whom (and where)?\n\r", ch );
	return;
    }
    if ( !str_cmp( arg1, "all" ) )
    {
	for ( player = player_list; player != NULL; player = player->next_player )
	    if ( player != ch && player->in_room != NULL && can_see( ch, player ) )
	    {
		char buf[MSL];
		sprintf( buf, "%s %s", player->name, arg2 );
		do_transfer( ch, buf );
	    }
	return;
    }
    if ( arg2[0] == '\0' )
	location = ch->in_room;
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No such location.\n\r", ch );
	    return;
	}
        if ( room_is_private( location ) &&  get_trust(ch) < MAX_LEVEL)
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
        if ( !have_access(ch,location))
        {
            send_to_char( "Don't think so.\n\r",ch);
            return;
        }
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim->in_room == NULL )
    {
	sendf( ch, "%s is in limbo.\n\r", PERS2(victim) );
	return;
    }
    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    if (!IS_NPC(victim))
	victim->pcdata->transnum = victim->in_room->vnum;
    if ( ch->desc )
	act( "$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    if ( ch->desc )
        act( "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM );
    else
	act( "$n has arrived.", victim, NULL, NULL, TO_ROOM );
    if ( ch != victim && ch->desc )
	act( "$n has transferred you.", ch, NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    sendf( ch, "You have transferred %s.\n\r", PERS2(victim) );
}

void do_untransfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL];
    CHAR_DATA *victim, *player;
    argument = one_argument( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Untransfer whom?\n\r", ch );
	return;
    }
    if ( !str_cmp( arg1, "all" ) )
    {
	for ( player = player_list; player != NULL; player = player->next_player )
	    if ( player != ch && player->in_room != NULL && can_see( ch, player ) )
	    {
		char buf[MSL];
		sprintf( buf, "%s %s", player->name, arg1);
		do_untransfer( ch, buf );
	    }
	return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't in this room.\n\r", ch );
	return;
    }
    if ( victim->in_room == NULL )
    {
	sendf( ch, "%s is in limbo.\n\r", PERS2(victim) );
	return;
    }
    if ( !IS_NPC(victim) && victim->pcdata->transnum == 0)
    {
	sendf( ch, "%s has nowhere to return to.\n\r", PERS2(victim) );
	return;
    }
    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    if ( ch->desc )
	act( "$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    if (!IS_NPC(victim))
    {
      if (get_room_index(victim->pcdata->transnum) == NULL){
	send_to_char("They have no where to go.\n\r", ch);
	return;
      }
      char_to_room( victim, get_room_index(victim->pcdata->transnum));
      victim->pcdata->transnum = 0;
    }
    else
	char_to_room( victim, get_room_index(victim->homevnum));
    if ( ch->desc )
        act( "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM );
    else
	act( "$n has arrived.", victim, NULL, NULL, TO_ROOM );
    if ( ch != victim && ch->desc )
	act( "$n has transferred you.", ch, NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    sendf( ch, "You have untransferred %s.\n\r", PERS2(victim) );
}

void do_at( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    ROOM_INDEX_DATA *location, *original;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "At where what?\n\r", ch );
	return;
    }
    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }
    if ( room_is_private( location ) && get_trust(ch) < MAX_LEVEL)
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }
    if ( !have_access(ch,location))
    {
        send_to_char( "Don't think so.\n\r",ch);
        return;
    }
    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );
    char_from_room( ch );
    char_to_room( ch, original );
}

void do_goto( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;
    int count = 0;
    if ( argument[0] == '\0' )
    {
	send_to_char( "Goto where?\n\r", ch );
	return;
    }
    if ( ( location = find_location( ch, argument ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }
    count = 0;
    for ( rch = location->people; rch != NULL; rch = rch->next_in_room )
        count++;
    if (room_is_private(location) && (count > 1 || get_trust(ch) < MAX_LEVEL))
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }
    if ( !have_access(ch,location))
    {
        send_to_char( "Don't think so.\n\r",ch);
        return;
    }
    if ( ch->fighting != NULL )
	stop_fighting( ch, TRUE );
    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
	if (get_trust(rch) >= ch->invis_level && ch->desc && !IS_SET(ch->act2,PLR_NOWHO))
	{
	    if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0' )
		act("$t",ch,ch->pcdata->bamfout,rch,TO_VICT);
	    else
		act("$n leaves in a swirling mist.",ch,NULL,rch,TO_VICT);
	}
    char_from_room( ch );
    char_to_room( ch, location );
    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
        if (get_trust(rch) >= ch->invis_level && ch->desc && !IS_SET(ch->act2,PLR_NOWHO))
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
                act("$t",ch,ch->pcdata->bamfin,rch,TO_VICT);
            else
                act("$n appears in a swirling mist.",ch,NULL,rch,TO_VICT);
        }
    do_look( ch, "auto" );
}

void do_violate( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;
    if ( argument[0] == '\0' )
    {
        send_to_char( "Goto where?\n\r", ch );
        return;
    }
    if ( ( location = find_location( ch, argument ) ) == NULL )
    {
        send_to_char( "No such location.\n\r", ch );
        return;
    }
    if (!room_is_private( location ))
    {
        send_to_char( "That room isn't private, use goto.\n\r", ch );
        return;
    }
    if ( !have_access(ch,location))   
    {
        send_to_char( "Don't think so.\n\r",ch);
        return;
    }
    if ( ch->fighting != NULL )
        stop_fighting( ch, TRUE );
    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
        if (get_trust(rch) >= ch->invis_level)
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
                act("$t",ch,ch->pcdata->bamfout,rch,TO_VICT);
            else
                act("$n leaves in a swirling mist.",ch,NULL,rch,TO_VICT);
        }
    char_from_room( ch );
    char_to_room( ch, location );
    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
        if (get_trust(rch) >= ch->invis_level)
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
                act("$t",ch,ch->pcdata->bamfin,rch,TO_VICT);
            else
                act("$n appears in a swirling mist.",ch,NULL,rch,TO_VICT);
        }
    do_look( ch, "auto" );
}

void do_sstat ( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    int col = 0;
    int sn = 0;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Stat whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (IS_NPC(victim)){
      send_to_char("Not on mobs.\n\r", ch);
      return;
    }
    sendf( ch, "Name: %s's progress:\n\r", victim->name);

    for ( sn = 0; sn < MAX_SKILL; sn++ )
      {
	if ( skill_table[sn].name == NULL )
	  break;
	if ( victim->pcdata->learned[sn] > 0 && sklevel(victim,sn) <= victim->level )
	    {
	      sendf( ch, "%-19s %3d(%d)%%  ",skill_table[sn].name, victim->pcdata->learned[sn],
		     URANGE(0, victim->pcdata->learned[sn] + victim->pcdata->to_learned[sn], 110));
	      if ( ++col % 3 == 0 )
		send_to_char( "\n\r", ch );
	    }
      }
    if ( col % 3 != 0 )
      send_to_char( "\n\r", ch );
    sendf( ch, "%d practice sessions left.\n\r",victim->practice );


}

void do_sngstat ( CHAR_DATA *ch, char *argument )
{

  char arg[MIL];
  CHAR_DATA *victim;
  int col = 0;
  int sn = 0;

  one_argument( argument, arg );
  if ( arg[0] == '\0' )
    {
      send_to_char( "Stat whom?\n\r", ch );
      return;
    }
  if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }
  if (IS_NPC(victim)){
    send_to_char("Not on mobs.\n\r", ch);
    return;
  }
  sendf( ch, "Name: %s's songs:\n\r", victim->name);
  
  for ( sn = 0; sn < MAX_SONG; sn++ )
    {
      if ( song_table[sn].name == NULL )
	break;
      if ( victim->pcdata->songlearned[sn] > 0 && sglevel(ch,sn) <= victim->level )
	{
	  sendf( ch, "%-19s %3d%%  ",song_table[sn].name, victim->pcdata->songlearned[sn] );
	  if ( ++col % 3 == 0 )
	    send_to_char( "\n\r", ch );
	}
    }
  if ( col % 3 != 0 )
    send_to_char( "\n\r", ch );
  sendf( ch, "%d rehearse sessions left.\n\r",victim->practice );
}

void do_stat ( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim;
    char *string = one_argument(argument, arg);
    if ( arg[0] == '\0')
    {
        send_to_char("Syntax:\n\r",ch);
        send_to_char("  stat <name>\n\r",ch);
        send_to_char("  stat obj <name>\n\r",ch);
        send_to_char("  stat mob/char <name>\n\r",ch);
 	send_to_char("  stat room <number>\n\r"\
		     "  stat skill <name>\n\r"\
		     "  stat song  <name>\n\r",ch);
	return;
    }
    if (!str_cmp(arg,"skill"))
    {
        do_sstat(ch,string);
	return;
    }
    if (!str_cmp(arg,"song"))
    {
        do_sngstat(ch,string);
	return;
    }
    if (!str_cmp(arg,"room"))
    {
        do_rstat(ch,string);
	return;
    }
    if (!str_cmp(arg,"obj"))
    {
	do_ostat(ch,string);
	return;
    }
    if(!str_cmp(arg,"char")  || !str_cmp(arg,"mob"))
    {
	do_mstat(ch,string);
	return;
    }
    if(!str_cmp(arg,"cabal")){
	do_cstat(ch,string);
	return;
    }
    obj = get_obj_world(ch,argument);
    if (obj != NULL)
    {
        do_ostat(ch,argument);
        return;
    }
    victim = get_char_world(ch,argument);
    if (victim != NULL)
    {
        do_mstat(ch,argument);
        return;
    }
    location = find_location(ch,argument);
    if (location != NULL)
    {
        do_rstat(ch,argument);
        return;
    }
    send_to_char("Nothing by that name found anywhere.\n\r",ch);
}

void do_rstat( CHAR_DATA *ch, char *argument )
{
    char buf[MSL], arg[MIL];
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj;
    CHAR_DATA *rch;
    int door;
    one_argument( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( location == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }
    if (ch->in_room != location && room_is_private( location ) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return;
    }
    if ( !have_access(ch,location))   
    {
        send_to_char( "Don't think so.\n\r",ch);
        return;
    }
    sendf( ch, "Name: '%s'\n\rArea: '%s'\n\r", location->name, location->area->name );
    sendf( ch, "Vnum: %d  Sector: %s  Light: %d  Healing: %d  Mana: %d  Temperature: %d\n\r",
      location->vnum, sector_bit_name(location->sector_type), location->light,
      location->heal_rate, location->mana_rate, location->temp );
    sendf( ch, "Room flags: %s  Room flags2: %s\n\rDay Description:\n\r%s", room_bit_name(location->room_flags),room2_bit_name(location->room_flags2), location->description );
    if (location->description2[0])
	sendf( ch, "Night Description:\n\r%s", location->description2);
    if ( location->extra_descr != NULL )
    {
	EXTRA_DESCR_DATA *ed;
	send_to_char( "Extra description keywords: '", ch );
	for ( ed = location->extra_descr; ed; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
		send_to_char( " ", ch );
	}
	send_to_char( "'.\n\r", ch );
    }
    send_to_char( "Characters:", ch );
    for ( rch = location->people; rch; rch = rch->next_in_room )
	if (can_see(ch,rch))
        {
	    send_to_char( " ", ch );
	    one_argument( rch->name, buf );
	    send_to_char( buf, ch );
	}
    send_to_char( ".\n\rObjects:   ", ch );
    for ( obj = location->contents; obj; obj = obj->next_content )
    {
	send_to_char( " ", ch );
	one_argument( obj->name, buf );
	send_to_char( buf, ch );
    }
    send_to_char( ".\n\r", ch );
    for ( door = 0; door <= 5; door++ )
    {
	EXIT_DATA *pexit;
	if ( ( pexit = location->exit[door] ) != NULL )
	{
            sendf( ch,"Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",
              door, (pexit->to_room == NULL ? -1 : pexit->to_room->vnum), pexit->key, pexit->exit_info,
	      pexit->keyword, pexit->description[0] != '\0' ? pexit->description : "(none).\n\r" );
	}
    }
}

/* shows cabal info, some parts are shown to Imms only */
void do_cstat( CHAR_DATA* ch, char* argument ){
  CABAL_DATA* pCab = NULL;
  CABAL_DATA* pPar = NULL;
  CMEMBER_DATA* cMem;

  char buf[MSL];
  int i = 0;
  int cnt = 0;
  bool fEld = FALSE;

  if (IS_NULLSTR( argument )){
    send_to_char("Stat what cabal?\n\r", ch);
    return;
  }
  else if ( (pCab = get_cabal( argument )) == NULL
	    && (pCab = get_cabal_vnum( atoi(argument))) == NULL){
    send_to_char("No such cabal found.\n\r", ch);
    return;
  }
  else
    pPar = get_parent( pCab );

/* leader/imm check */
  if (IS_IMMORTAL(ch) && (!str_cmp(ch->name, pCab->immortal) || !str_cmp(ch->name, pPar->immortal)))
    fEld = TRUE;
  else if (IS_ELDER( ch ) && ch->pCabal && get_parent(ch->pCabal) == pPar)
    fEld = TRUE;
  else if (get_trust(ch) > CREATOR)
    fEld = TRUE;

/* start the cabal info */
  sendf( ch, "The following is the current status of %s", pCab->who_name );
  if (pPar != pCab){
    sendf(ch, " and %s.\n\r", pPar->who_name );
  }
  else
    send_to_char(".\n\r", ch);

  sendf(ch, "Flags: %s\n\r", flag_string( cabal_flags, pCab->flags ));  
  sendf(ch, "Flags2: %s\n\r\n\r", flag_string( cabal_flags2, pCab->flags2 ));  
  if (pPar->city)
    sendf(ch, "City: %s\n\r", pPar->city->name );
/* PRESENT */
  sendf( ch, "There are currently %d members present in %s", pCab->present, pCab->who_name );
  if (pPar != pCab){
    sendf( ch, " out of %d in all of %s.\n\r", pPar->present, pPar->who_name );
  }
  else
    send_to_char(".\n\r", ch);
  if (pCab->present_stamp && mud_data.current_time - pCab->present_stamp < CABAL_PRESENT_CAPUTREWAIT){
    int num = mud_data.current_time - pCab->present_stamp;
    char* str;
    
    if ( (num = num / 60) < 60)
      str = "minutes";
    else if ( (num = num / 60) < 24)
      str = "hours";
    else{
      num = num / 24;
      str = "days";
    }
    sendf(ch, "Last attack with members present %d %s ago.\n\r", num, str);
  }
  send_to_char("\n\r", ch);

/* ECONOMY INFO */
  sendf( ch, "Coffers: %d cps\n\r", GET_CAB_CP( pPar ));
  sendf( ch, 
	 "Support: Income: %d (Trade: %d) (Raid: %ld) Armies: -%d Rooms: -%d  Corr: -%d Gain: %d\n\r",
	 pPar->support[ECO_INCOME],
	 pPar->support[ECO_TRADE],
	 pPar->bonus_sup,
	 pPar->support[ECO_LAST_ARMY],
	 pPar->support[ECO_LAST_RENT],
	 pPar->support[ECO_LAST_CORR],
	 pPar->support[ECO_GAIN]
	 );
  sendf( ch, 
	 "Cps: Income: %ld Trade: %ld Gain: %ld\n\r",
	 pPar->cp_gain[ECO_INCOME] / CPTS,
	 pPar->cp_gain[ECO_TRADE] / CPTS,
	 pPar->cp_gain[ECO_GAIN] / CPTS
	 );
  sendf( ch, "TAX: %d%%\n\r\n\r",  pPar->cp_tax);

  sprintf( buf, "[%s] Members: [%d/%d] Elders: [%d/%d]  Leaders [%d/%d]\n\r", 
	   pPar->who_name,
	   pPar->cur_member, pPar->pIndexData->max_member,
	   pPar->cur_elder, pPar->max_elder,
	   pPar->cur_leader, pPar->max_leader);
  send_to_char( buf, ch );
    
  if (pCab->parent){
    sprintf( buf, "Cabal rank required to join: %d\n\r", pCab->pIndexData->min_rank );
    send_to_char( buf, ch );
  }
  sprintf( buf, "Align:    [%s%s%s]    Ethos:    [%s%s%s]\n\r", 
	   IS_SET(pCab->pIndexData->align, ALIGN_GOOD) ? "G" : " ",
	   IS_SET(pCab->pIndexData->align, ALIGN_NEUTRAL) ? "N" : " ",
	   IS_SET(pCab->pIndexData->align, ALIGN_EVIL) ? "E" : " ",
	   IS_SET(pCab->pIndexData->ethos, ETHOS_LAWFUL) ? "L" : " ",
	   IS_SET(pCab->pIndexData->ethos, ETHOS_NEUTRAL) ? "N" : " ",
	   IS_SET(pCab->pIndexData->ethos, ETHOS_CHAOTIC) ? "C" : " ");
  send_to_char( buf, ch );
  
  
  send_to_char("Entrance and member status:\n\r", ch );
  sprintf( buf, "Avatar: %s\n\r", pCab->pIndexData->fAvatar ? "Yes" : "No" );
  send_to_char( buf, ch );
  
  if (pCab->pIndexData->race[0] < 0){
    sprintf(buf, "Races: All");
  }
  else{
    sprintf(buf, "Race: ");
    for (i = 0; i < MAX_PC_RACE; i++){
      char temp[MIL];
      if (pCab->pIndexData->race[i] > 0){
	sprintf(temp, "%-5s ", race_table[i].name);
	strcat(buf, temp);
      }
      }
    }
  send_to_char( buf, ch );
  send_to_char("\n\r", ch);
  
  if (pCab->pIndexData->class[0] < 0){
    sprintf(buf, "Class: All");
  }
  else{
    sprintf(buf, "Class: ");
    for (i = 0; i < MAX_CLASS; i++){
      char temp[MIL];
      if (pCab->pIndexData->class[i] > 0 ){
	sprintf(temp, "%-5s ", class_table[i].who_name);
	strcat(buf, temp);
      }
    }
  }
  
  send_to_char( buf, ch );
  send_to_char("\n\r", ch);
  
  /* show members that you can see */
  for ( cnt = 0, cMem = pPar->members; cMem; cMem = cMem->next ){
    if (cMem->level > 50 && !IS_IMMORTAL(ch))
      continue;
    if ( cnt == 0 ){
      send_to_char("\n\rMEMBERS:\n\r", ch);
      send_to_char( "Number     Name       Class   Race    Lvl   Rank    Cp   Month  Days Ago    Cabal\n\r", ch );
      send_to_char( "------ ------------- ------- ------- ----- ------  ---- ------- ------- -------\n\r", ch );
    }
    sprintf( buf, "[%4d] %-15.15s %-6s %-8.8s %-6d %-5d %-5ld %-5d %-5ld %-10s\n\r", 
	     cnt, 
	     cMem->name,
	     class_table[cMem->class].who_name,
	     race_table[cMem->race].name,
	     cMem->level,
	     cMem->rank,
	     cMem->cp,
	     cMem->mhours / 3600,
	     (mud_data.current_time - cMem->last_login) / VOTE_DAY,
	     cMem->pCabal->who_name);
    send_to_char( buf, ch );
    cnt++;
  }
  send_to_char("\n\r", ch );
  if (IS_CABAL(pPar, CABAL_TRADE) || IS_CABAL(pPar, CABAL_NAPACT) 
      || IS_CABAL(pPar, CABAL_ALLIANCE) || IS_CABAL(pPar, CABAL_VENDETTA)){
    send_to_char("PACTS:\n\r", ch );
    show_pacts( ch, pPar );
  }
}

void do_ostat( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;
    OBJ_SPELL_DATA *os;
    int i;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Stat what?\n\r", ch );
	return;
    }
    if ( ( obj = get_obj_world( ch, argument ) ) == NULL )
    {
	send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
	return;
    }
    sendf( ch, "Name(s): %s\n\r", obj->name );
    sendf( ch, "Vnum: %d (%d) Format: %s  Type: %s  Material: %s\n\r",
      obj->vnum, obj->pIndexData->vnum, obj->pIndexData->new_format ? "new" : "old", item_name(obj->item_type), obj->material);
    sendf( ch, "Resets: %d  Factor: %d  Allowed %d  Count: %d  Home: %d  Recall %d  Pit %d\n\r",
	   obj->pIndexData->reset_num, 
	   IS_LIMITED(obj) ? obj->pIndexData->cost : 0,
	   get_maxcount( obj->pIndexData ),
	   obj->pIndexData->count, 
	   obj->homevnum,
	   obj->recall,
	   get_temple_pit(obj->recall));
    sendf( ch, "Short description: %s\n\rLong description: %s\n\r", obj->short_descr, obj->description );
    sendf( ch, "Wear bits: %s\n\rExtra bits: %s\n\r", wear_bit_name(obj->wear_flags), extra_bit_name( obj->extra_flags ) );
    if (obj->pCabal)
      sendf(ch, "Cabal: %-10s ", obj->pCabal->name);
    if (obj->race)
      sendf(ch, "Race: %-10s ", race_table[obj->race].name);
    if (obj->class >= 0)
      sendf(ch, "Class: %-10s ", class_table[obj->class].name);
    if (obj->class >=0  || obj->race || obj->pCabal)
      send_to_char("\n\r", ch);
    sendf( ch, "Number: %d/%d  Weight: %d/%d/%d (10th pounds)\n\r",
      1, get_obj_number( obj ), obj->weight, get_obj_weight( obj ),get_true_weight(obj) );
    sendf( ch, "Level: %d  Cost: %d  Condition: %d  Timer: %d  Idle: %d\n\r", obj->level, obj->cost, obj->condition, obj->timer, obj->idle );
    sendf( ch, "In room: %d  In object: %s  Carried by: %s  Wear_loc: %d\n\r",
      obj->in_room    == NULL ?        0 : obj->in_room->vnum, obj->in_obj     == NULL ? "(none)" : obj->in_obj->short_descr,
      obj->carried_by == NULL ? "(none)" : can_see(ch,obj->carried_by) ? obj->carried_by->name : "someone", obj->wear_loc );
    sendf( ch, "Values: %d %d %d %d %d\n\r", obj->value[0], obj->value[1], obj->value[2], obj->value[3], obj->value[4] );
    switch ( obj->item_type )
    {
    case ITEM_SOCKET:
      sendf(ch, "Has following socket flags: %s\n\r",
	    flag_string( socket_flags,  obj->value[0] ) );
      sendf(ch, "Adds following to extra flags: %s\n\r",
	    flag_string( extra_flags,  obj->value[1] ) );
      sendf(ch, "Adds following to weapon flags: %s\n\r",
	    flag_string( weapon_type2,  obj->value[4] ) );
      break;
    	case ITEM_SCROLL: 
	case ITEM_ARTIFACT:
    	case ITEM_POTION:
    	case ITEM_PILL:
	    sendf( ch, "Level %d spells of:", obj->value[0] );
            for ( i = 1; i <= 4; i++ )
                if ( obj->value[i] >= 0 && obj->value[i] < MAX_SKILL && skill_table[obj->value[i]].name != NULL)
                    sendf( ch, " '%s'", skill_table[obj->value[i]].name);
	    send_to_char( ".\n\r", ch );
	break;
    	case ITEM_WAND: 
    	case ITEM_STAFF: 
            sendf( ch, "Has %d(%d) charges of level %d", obj->value[1], obj->value[2], obj->value[0] );
	    if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL && skill_table[obj->value[3]].name != NULL)
	    	sendf( ch, " '%s'", skill_table[obj->value[3]].name);
	    send_to_char( ".\n\r", ch );
	break;
	case ITEM_DRINK_CON:
	    sendf(ch,"It holds %s-colored %s.\n\r", liq_table[obj->value[2]].liq_color, liq_table[obj->value[2]].liq_name);
	    break;
    	case ITEM_WEAPON:
 	    send_to_char("Weapon type is ",ch);
	    switch (obj->value[0])
	    {
                case(WEAPON_EXOTIC): send_to_char("exotic\n\r",ch);      break;
                case(WEAPON_SWORD):  send_to_char("sword\n\r",ch);       break; 
                case(WEAPON_DAGGER): send_to_char("dagger\n\r",ch);      break;
                case(WEAPON_SPEAR):  send_to_char("spear\n\r",ch); break;
                case(WEAPON_STAFF):  send_to_char("staff\n\r",ch); break;
                case(WEAPON_MACE):   send_to_char("mace/club\n\r",ch);   break;
                case(WEAPON_AXE):    send_to_char("axe\n\r",ch);         break;
                case(WEAPON_FLAIL):  send_to_char("flail\n\r",ch);       break;
                case(WEAPON_WHIP):   send_to_char("whip\n\r",ch);        break;
                case(WEAPON_POLEARM):send_to_char("polearm\n\r",ch);     break;
                default:             send_to_char("unknown\n\r",ch);     break;
            }
	    if (obj->pIndexData->new_format)
	    	sendf(ch,"Damage is %dd%d (average %d)\n\r", obj->value[1],obj->value[2], (1+obj->value[2])*obj->value[1]/2);
	    else
	    	sendf(ch,"Damage is %d to %d (average %d)\n\r", obj->value[1], obj->value[2], (obj->value[1]+obj->value[2])/2);
            sendf(ch,"Damage noun is %s.\n\r",
	      (obj->value[3] > 0 && obj->value[3] < MAX_DAMAGE_MESSAGE) ? attack_table[obj->value[3]].noun : "undefined");
            if (obj->value[4])
                sendf(ch,"Weapons flags: %s\n\r", weapon_bit_name(obj->value[4]));
	break;
    	case ITEM_ARMOR:
            sendf( ch, "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic\n\r",
              obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
	break;
        case ITEM_CONTAINER:
            sendf(ch,"Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
	      obj->value[3],obj->value[0],cont_bit_name(obj->value[1]));
            if (obj->value[4] != 100)
                sendf(ch,"Weight multiplier: %d%%\n\r", obj->value[4]);
        break;
    	case ITEM_THROW:
    	    sendf( ch, "Has %d %s left.\n\r",obj->value[0],obj->short_descr);
    	    if (obj->pIndexData->new_format)
    	        sendf(ch,"Damage is %dd%d (average %d).\n\r",obj->value[1],obj->value[2], (1+obj->value[2])*obj->value[1]/2);
    	    else
    	        sendf(ch,"Damage is %d to %d (average %d).\n\r",obj->value[1], obj->value[2], (obj->value[1]+obj->value[2])/2);
            if (obj->value[4])
                sendf(ch,"Weapons flags: %s\n\r", weapon_bit_name(obj->value[4]));
    	    break;
    }
/* TRAP info */
    if (obj->traps){
      TRAP_DATA* pTrap = obj->traps;
      while(pTrap){
	sendf(ch, "Trap [%d] %s%s%s: %s, type: %s, lvl: %d, dur: %d, %s\n\r",
	      pTrap->vnum, 
	      pTrap->owner ? "(" : "",
	      pTrap->owner ? pTrap->owner->name : "",
	      pTrap->owner ? ")" : "",
	      pTrap->name, trap_table[pTrap->type].name,
	      pTrap->level, pTrap->duration, 
	      pTrap->armed ? "ARMED" :  "disarmed");
	pTrap = pTrap->next_trap;
      }
    }
    if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
    {
	EXTRA_DESCR_DATA *ed;
	send_to_char( "Extra description keywords: '", ch );
	for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
	    	send_to_char( " ", ch );
	}
	for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
	{
	    send_to_char( ed->keyword, ch );
	    if ( ed->next != NULL )
		send_to_char( " ", ch );
	}
	send_to_char( "'\n\r", ch );
    }
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	if (IS_GEN(paf->type))
	  sendf( ch, "Effect: '%s' modifies %s by %d for %d hours with bits "\
		 "%s or %s, level %d%s%s\n\r",
		 effect_table[GN_POS(paf->type)].name, 
		 (paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location )), 
		 paf->modifier,
		 paf->duration,
		 affect_bit_name( paf->bitvector ), 
		 affect2_bit_name( paf->bitvector ), 
		 paf->level,
		 (paf->has_string && paf->string != NULL? ".\n\r String: " :  ""),
		 (paf->has_string && paf->string != NULL? paf->string: "") );
//Else we do it the regular way.
	else{
	  sendf( ch, "Spell: '%s' modifies %s by %d for %d hours with bits %s"\
		 " or %s, level %d%s%s\n\r",
		 skill_table[(int) paf->type].name, 
		 (paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location )), 
		 paf->modifier,
		 paf->duration,
		 affect_bit_name( paf->bitvector ),
		 affect2_bit_name( paf->bitvector ), 
		 paf->level,
		 (paf->has_string && paf->string != NULL? ".\n\r String: " :  ""),
		 (paf->has_string && paf->string != NULL? paf->string: "") );
	  
	  if (paf->bitvector){
	    switch(paf->where){
            case TO_AFFECTS: sendf(ch,"Adds %s affect.\n\r", affect_bit_name(paf->bitvector));          break;
            case TO_WEAPON:  sendf(ch,"Adds %s weapon flags.\n\r", weapon_bit_name(paf->bitvector));    break;
            case TO_OBJECT:  sendf(ch,"Adds %s object flag.\n\r", extra_bit_name(paf->bitvector));      break;
            case TO_IMMUNE:  sendf(ch,"Adds immunity to %s.\n\r", imm_bit_name(paf->bitvector));        break;
            case TO_RESIST:  sendf(ch,"Adds resistance to %s.\n\r", imm_bit_name(paf->bitvector));    break;
            case TO_VULN:    sendf(ch,"Adds vulnerability to %s.\n\r", imm_bit_name(paf->bitvector)); break;
	    case TO_SKILL:   /* handled above */ break;
            default:         sendf(ch,"Unknown bit %d: %d\n\r", paf->where,paf->bitvector);           break;
	    }
	  }
	}
    }
    if (!obj->enchanted)
        for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
        {
	  if ( paf->location != APPLY_NONE && paf->modifier != 0 )
	    sendf( ch, "Affects %s by %d, level %d.\n\r",
		   (paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location )), 
		   paf->modifier, 
		   paf->level );
	  
            if (paf->bitvector)
            {
                switch(paf->where)
                {
                case TO_AFFECTS: sendf(ch,"Adds %s affect.\n\r", affect_bit_name(paf->bitvector));          break;
                case TO_OBJECT:  sendf(ch,"Adds %s object flag.\n\r", extra_bit_name(paf->bitvector));      break;
                case TO_IMMUNE:  sendf(ch,"Adds immunity to %s.\n\r", imm_bit_name(paf->bitvector));        break;
                case TO_RESIST:  sendf(ch,"Adds resistance to %s.\n\r", imm_bit_name(paf->bitvector));    break;
                case TO_VULN:    sendf(ch,"Adds vulnerability to %s.\n\r", imm_bit_name(paf->bitvector)); break;
		case TO_SKILL:   /* handled above */ break;
                default:         sendf(ch,"Unknown bit %d: %d\n\r", paf->where,paf->bitvector);           break;
            }
        }
    }
//Select betwen Indexed or dynamic.
    if (!obj->eldritched)
      os = obj->pIndexData->spell;
    else
      os = obj->spell;

    for (; os != NULL; os = os->next )
        if (os->target == SPELL_TAR_SELF || os->target == SPELL_TAR_VICT )
            sendf(ch, "Cast spells %s at %s\n\r",
	      skill_table[os->spell].name, os->target == SPELL_TAR_SELF ? "character" : os->target == SPELL_TAR_VICT ? "victim" : "none");
    if (obj->pIndexData->message)
    {
	sendf(ch, "%s\n\r", obj->pIndexData->message->onself);
	sendf(ch, "%s\n\r", obj->pIndexData->message->onself);
	sendf(ch, "%s\n\r", obj->pIndexData->message->offself);
	sendf(ch, "%s\n\r", obj->pIndexData->message->offother);
    }
    if (CAN_WEAR(obj, ITEM_UNIQUE))
        send_to_char("Unique item.\n\r",ch);
    if (CAN_WEAR(obj, ITEM_RARE))
        send_to_char("Rare item.\n\r",ch);
    if (CAN_WEAR(obj, ITEM_PARRY))
        send_to_char("Can be dual parried.\n\r",ch);
}

void do_mstat( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    AFFECT_DATA *paf;
    CHAR_DATA *victim;
    int i;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Stat whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    sendf( ch, "Name: %s (T: %d)", victim->name, get_trust( victim) );
    if (!IS_NPC(victim))
      sendf(ch, " (SaveVer %d)", victim->save_ver);
    if (!IS_NPC(victim))
      sendf(ch, "    Rel:  %s.", deity_table[victim->pcdata->way].way);
    if(!IS_NPC(victim) && IS_SET(victim->act2, PLR_MASQUERADE))
      sendf(ch, "  `5[`%%MASQUE`5]``\n\r");
    else
      send_to_char("\n\r",ch);
    if (!IS_NPC(victim) && victim->pcdata->alias && victim->pcdata->alias[0] != '\0')
    	sendf( ch, "Alias: The last thing you see is %s.\n\r", victim->pcdata->alias);
    sendf( ch, "Vnum: %d  Format: %s  Race: %s %s%s%sGroup: %d  Sex/P: %s/%s  Room: %d\n\r",
	   IS_NPC(victim) ? victim->pIndexData->vnum : 0,
	   IS_NPC(victim) ? victim->pIndexData->new_format ?"new":"old":"pc",
	   IS_QRACE(victim) ? victim->pcdata->race : race_table[victim->race].name,
	   IS_QRACE(victim) ? "(" : "",
	   IS_QRACE(victim) ? race_table[victim->race].name: "",
	   IS_QRACE(victim) ? ") " : "",
	   IS_NPC(victim) ? victim->group : 0, 
	   victim->sex == 0 ? "sexless" : victim->sex == 1 ? "male" : "female",
	   !IS_NPC(victim) ? (victim->pcdata->true_sex == 0 ? "sexless" : victim->pcdata->true_sex == 1 ? "male" : "female") : "mob",
	   victim->in_room == NULL    ?        0 : victim->in_room->vnum);
    if (!IS_NPC(victim)){
      sendf(ch, "Hometown: %s,   Ethos: %s,   Tattoo: %s\n\r",
	    hometown_table[victim->hometown].name, 
	    ethos_table[victim->pcdata->ethos].name, 
	    deity_table[victim->tattoo].god);
    }
    else{
      sendf(ch, "Count: %d  Killed: %d  Home: %d  Cabal: %s\n\r",
	    victim->pIndexData->count,
	    victim->pIndexData->killed, 
	    victim->homevnum,
	    victim->pCabal ? victim->pCabal->name : "None");
    }
    sendf( ch, "Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
      victim->perm_stat[STAT_STR], get_curr_stat(victim,STAT_STR),
      victim->perm_stat[STAT_INT], get_curr_stat(victim,STAT_INT),
      victim->perm_stat[STAT_WIS], get_curr_stat(victim,STAT_WIS),
      victim->perm_stat[STAT_DEX], get_curr_stat(victim,STAT_DEX),
      victim->perm_stat[STAT_CON], get_curr_stat(victim,STAT_CON) );
    /* PC ONLY SPECIALS */
    if (!IS_NPC(victim))
    {
    	sendf( ch, "Luck: %d(%d)", victim->perm_stat[STAT_LUCK], get_curr_stat(victim,STAT_LUCK));
	/* Beast Type */
	if (!IS_NPC(victim) && victim->race == grn_werebeast){
	  sendf(ch,"   Beast Type: %d",victim->pcdata->beast_type);
	}
	/* Divine Favor */
	if (victim->pcdata->divfavor != 0)
	    sendf(ch,"   Divine Favor: %d", victim->pcdata->divfavor);
	send_to_char("\n\r",ch);
    }
    sendf( ch, "Hp: %d/%d  Mana: %d/%d  Move: %d/%d  Practices: %d  Trains %d\n\r",
      victim->hit,  victim->max_hit, victim->mana, victim->max_mana, victim->move, victim->max_move,
      IS_NPC(ch) ? 0 : victim->practice,      IS_NPC(ch) ? 0 : victim->train );
    sendf( ch, "Lv: %d  Class: %s  Align: %d  Gold: %ld CabalPts: %d\n\r",
      victim->level, IS_NPC(victim) ? "mobile" : class_table[victim->class].name, victim->alignment, victim->gold,
	   GET_CP(victim));
    if (!IS_NPC(victim) && victim->pcdata->member && IS_CABAL(get_parent(victim->pCabal), CABAL_AREA)){
      sendf(ch, "Armies: %d.%d\n\r", GARR_NOR( victim ), GARR_UPG( victim ));
    }
    if (!IS_NPC(victim))
        sendf( ch, "Exp: %ld  Exp to level: %ld Max Exp: %ld Pk Range: %ld to %ld \n\r",
      	  victim->exp, total_exp(victim) - victim->exp, victim->max_exp, 7 * victim->max_exp /10, 13 * victim->max_exp / 10);
    sendf(ch,"Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
      GET_AC(victim,AC_PIERCE), GET_AC(victim,AC_BASH), GET_AC(victim,AC_SLASH),  GET_AC(victim,AC_EXOTIC));
    sendf(ch, "Hit: %d  Dam: %d  Size: %s  Position: %s  Wimpy: %d\n\r",
	  GET_HITROLL(victim), GET_DAMROLL(victim), size_table[victim->size].name,
	  position_table[victim->position].name, victim->wimpy );
    if (!IS_NPC(victim)){
      sendf(ch, "Weapon Position: %s.\n\r",
	    victim->pcdata->wep_pos == WEPPOS_HIGH ? "high" :
	    victim->pcdata->wep_pos == WEPPOS_LOW ? "low" : 
	    victim->pcdata->wep_pos == WEPPOS_NORMAL? "normal" : "unkown");
    }
    
    sendf(ch, "Affl: %d  Maled: %d  Mental: %d  Breath: %d  Spell: %d\n\r",
	victim->savingaffl, victim->savingmaled, victim->savingmental, victim->savingbreath, victim->savingspell);
    sendf(ch, "Level Modifier: Affl %d  Maled  %d Mental %d  Spell %d.\n\r",
	  victim->aff_mod[MOD_AFFL_LEVEL],
	  victim->aff_mod[MOD_MALED_LEVEL],
	  victim->aff_mod[MOD_MENTAL_LEVEL],
	  victim->aff_mod[MOD_SPELL_LEVEL]);
    sendf(ch, "Waitstate: %d(%%)  Spel/Skill Rate: %d/%d(%%) Spell Cost: %d(%%) Hit/Mana/Move Rate: %d/%d/%d(%%).\n\r",
	  (100 + victim->aff_mod[MOD_WAIT_STATE]),
	  (100 + victim->aff_mod[MOD_SPELL_LEARN]),
	  (100 + victim->aff_mod[MOD_SKILL_LEARN]),
	  (100 + victim->aff_mod[MOD_SPELL_COST]),
	  (100 + victim->aff_mod[MOD_HIT_GAIN]),
	  (100 + victim->aff_mod[MOD_MANA_GAIN]),
	  (100 + victim->aff_mod[MOD_MOVE_GAIN]));
    if (IS_NPC(victim) && victim->pIndexData->new_format)
	sendf(ch, "Damage: %dd%d ",victim->damage[DICE_NUMBER],victim->damage[DICE_TYPE]);
    sendf(ch, "Message:  %s\n\r",attack_table[victim->dam_type].noun);
    sendf( ch, "Fighting: %s\n\r",victim->fighting ? victim->fighting->name : "(none)" );


    if ( !IS_NPC(victim) )
    {
        sendf( ch, "Thirst: %d  Hunger: %d  Drunk: %d\n\r",
          victim->pcdata->condition[COND_THIRST], victim->pcdata->condition[COND_HUNGER], victim->pcdata->condition[COND_DRUNK] );
        sendf( ch, "Carry number: %d  Carry weight: %d  Guarding: %s  Guarded by: %s\n\r",
	  victim->carry_number, get_carry_weight(victim) / 10, victim->pcdata->guarding ? victim->pcdata->guarding->name : "(none)",
	  victim->pcdata->guarded_by ? victim->pcdata->guarded_by->name : "(none)" );
        sendf( ch, "Age: %d  Played: %d  Last Level: %d  Timer: %d\n\r",
	    get_age(victim), (int) (victim->played + mud_data.current_time - victim->logon) / 3600, 
	    victim->pcdata->last_level, victim->timer );
    }

    sendf(ch, "Act: %s\n\r",act_bit_name(victim->act,TRUE,IS_NPC(victim) ? TRUE : FALSE));
    sendf(ch, "Act2: %s\n\r",act_bit_name(victim->act2,FALSE,IS_NPC(victim) ? TRUE : FALSE));
    if (victim->game)
      sendf(ch,"Game: %s\n\r",game_bit_name(victim->game));
    if (!IS_NPC(victim) && victim->pcdata->perk_bits)
      sendf(ch,"Perks: %s\n\r", perk_bit_name( victim->pcdata->perk_bits ));
    if (victim->comm)
    	sendf(ch,"Comm: %s\n\r",comm_bit_name(victim->comm));
    if (IS_COLOR(victim, COLOR_ON))
      sendf(ch,"Color: %s\n\r",color_bit_name(victim->color));
    else
      sendf(ch,"Color: OFF (%s)\n\r", color_bit_name(victim->color));
    if (IS_NPC(victim) && victim->special)
    	sendf(ch, "Special: %s\n\r",special_bit_name(victim->special));
    if (IS_NPC(victim) && victim->off_flags)
    	sendf(ch, "Offense: %s\n\r",off_bit_name(victim->off_flags));
    if (victim->imm_flags)
	sendf(ch, "Immune: %s\n\r",imm_bit_name(victim->imm_flags));
    if (victim->res_flags)
	sendf(ch, "Resist: %s\n\r", imm_bit_name(victim->res_flags));
    if (victim->vuln_flags)
	sendf(ch, "Vulnerable: %s\n\r", imm_bit_name(victim->vuln_flags));
    if (victim->affected_by)
        sendf(ch, "Affected by %s\n\r", affect_bit_name(victim->affected_by));
    if (victim->affected2_by)
        sendf(ch, "Also affected by %s\n\r", affect2_bit_name(victim->affected2_by));
    if (victim->form)
        sendf(ch, "Form: %s\n\r", form_bit_name(victim->form));
    if (victim->parts)
        sendf(ch, "Parts: %s\n\r", part_bit_name(victim->parts));
    sendf( ch, "Master: %s  Leader: %s  Pet: %s Summoner: %s Path: %d\n\r",
      victim->master ? victim->master->name : "(none)", victim->leader ? victim->leader->name : "(none)",
	   victim->pet    ? victim->pet->name    : "(none)",
	   victim->summoner ? victim->summoner->name : "none",
	   victim->spec_path ? get_path_vnum(victim) : 0);
    if (!IS_NPC(victim))
    {
	int total = victim->pcdata->mplayed;
	if (victim->logon != 0)
	    total += ((int) ((mud_data.current_time - victim->logon)/60));
	sendf( ch, "Month: %d -- %d Hours and %d Mins -- Total: %d\n\r",victim->pcdata->month, total/60, total % 60, total );
        sendf( ch, "Security: %d.\n\r", victim->pcdata->security );	
        sendf( ch, "Times flagged: %d.\n\r", victim->pcdata->flagged );
        sendf( ch, "Player Rank: %d.\n\r", victim->pcdata->rank );
	sendf( ch, "Player Kills: %d, Player Deaths: %d, Mob Kills: %d, Mob Deaths: %d, Total Deaths: %d\n\r",
	  victim->pcdata->kpc, victim->pcdata->dpc, victim->pcdata->knpc, victim->pcdata->dnpc, victim->pcdata->dall);
    }



    if (IS_NPC(victim) || IS_IMMORTAL(victim))
        sendf( ch, "Short description: %s\n\rLong  description: %s",
          victim->short_descr, victim->long_descr[0] != '\0' ? victim->long_descr : "(none)\n\r" );
    if ( IS_NPC(victim) && victim->spec_fun != 0 )
        sendf(ch,"Mobile has special procedure %s.\n\r", spec_name(victim->spec_fun));
    if ( IS_NPC(victim) && victim->pIndexData->progtypes )
    {
	TRIGFILE_DATA *ptrigfile;
	for ( ptrigfile = victim->pIndexData->trigfile; ptrigfile; ptrigfile = ptrigfile->next )
            sendf(ch,"Mobile program %s at %d.\n\r",ptrigfile->name,ptrigfile->dowhen);
    }
    if ( IS_NPC(victim) && victim->hunting != NULL )
        sendf(ch, "Hunting victim [%d]: %s (%s)\n\r",
	   5 - victim->level/10 - victim->hunttime,
	   IS_NPC(victim->hunting) ? victim->hunting->short_descr : victim->hunting->name,
           IS_NPC(victim->hunting) ? "MOB" : "PLAYER" );
    
    if ( !IS_NPC(victim) && is_ghost(victim,300) )
        send_to_char("Is a ghost right now.\n\r",ch);
    if (victim->class == class_lookup("monk"))
    {
	OBJ_DATA *obj;
        send_to_char("Anatomy:", ch);
        for (i = 0; anatomy_table[i].name != NULL; i++)
            sendf(ch, "  %s: %d", anatomy_table[i].name, victim->pcdata->anatomy[i]);
        send_to_char("\n\r", ch);
        send_to_char("Encumberance:", ch);
        for (i = 0; i < 8; i++)
            if ((obj = get_eq_char(victim, monk_table[i].value)) != NULL && obj->weight > monk_table[i].weight * 10)            
                sendf(ch, " %s", monk_table[i].wear);
        send_to_char("\n\r", ch);
    }
    if (is_affected(victim, gen_study))
      sendf(ch, "%s", study_info(victim, TRUE, ch));
/* TRAP info */
    for (i = 0; i < MAX_TRAPS; i++){
      TRAP_DATA* pTrap = victim->traps[i];
      if (pTrap == NULL)
	continue;
      if (pTrap->on_obj)
	sendf(ch, "Trap O: %s [%d] ", pTrap->on_obj->short_descr,
	      pTrap->on_obj->vnum);
      else if (pTrap->on_exit)
	sendf(ch, "Trap E: %s [%d] ",
	      pTrap->on_exit->to_room ? pTrap->on_exit->to_room->name : "error",
	      pTrap->on_exit->to_room ? pTrap->on_exit->to_room->vnum : 0); 
      else
	sendf(ch, "Trap ");
      
      sendf(ch, "Vn:[%d] %s%s%s, type: %s, lvl: %d, dur: %d, %s\n\r",
	    pTrap->vnum, 
	    pTrap->owner ? "(" : "",
	    pTrap->owner ? pTrap->owner->name : "",
	    pTrap->owner ? ")" : "",
	    trap_table[pTrap->type].name,
	    pTrap->level, pTrap->duration, 
	      pTrap->armed ? "ARMED" :  "disarmed");
      pTrap = pTrap->next_trap;
    }

    if (!IS_NPC(victim) && victim->pcdata->fight_delay != (time_t)NULL && victim->pcdata->fight_delay >= 0)
        if (difftime(mud_data.current_time,victim->pcdata->fight_delay) < 120)
	    sendf(ch, "Lagged %d second fight time so far.\n\r",(int)difftime(mud_data.current_time,victim->pcdata->fight_delay));
    if (!IS_NPC(victim) && victim->pcdata->pk_delay != (time_t)NULL && victim->pcdata->pk_delay >= 0)
        if (difftime(mud_data.current_time,victim->pcdata->pk_delay) < 360)
	    sendf(ch, "Lagged %d second pk time so far.\n\r",(int)difftime(mud_data.current_time,victim->pcdata->pk_delay));
    if ( !IS_NPC(victim) )
	sendf(ch, "EXP COST: %d\n\r", pc_race_table[victim->race].class_mult - 1500 + class_table[victim->class].extra_exp);

//Show SPELLS ON character
    for ( paf = victim->affected; paf != NULL; paf = paf->next )
      {
      //We show spells and effects diferently.
	if (IS_GEN(paf->type))
	  sendf( ch, "Effect: '%s' modifies %s by %d for %d hours with bits"\
		 "%s or %s, level %d%s%s\n\r",
		 effect_table[GN_POS(paf->type)].name, 
		 (paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location )), 
		 paf->modifier,
		 paf->duration,
		 affect_bit_name( paf->bitvector ), 
		 affect2_bit_name( paf->bitvector ), 
		 paf->level,
		 (paf->has_string && paf->string != NULL? ".\n\r String: " :  ""),
		 (paf->has_string && paf->string != NULL? paf->string: "") );
//Else we do it the regular way.
	else
	  sendf( ch, "Spell: '%s' modifies %s by %d for %d hours with bits %s"\
		 " or %s, level %d%s%s\n\r",
		 skill_table[(int) paf->type].name, 
		 (paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location )), 
		 paf->modifier,
		 paf->duration,
		 affect_bit_name( paf->bitvector ),
		 affect2_bit_name( paf->bitvector ), 
		 paf->level,
		 (paf->has_string && paf->string != NULL? ".\n\r String: " :  ""),
		 (paf->has_string && paf->string != NULL? paf->string: "") );

      }//end for

    for ( paf = victim->affected2; paf != NULL; paf = paf->next )
        sendf( ch, "Song: '%s' modifies %s by %d for %d hours with bits %s or %s, level %d.\n\r",
          song_table[(int) paf->type].name, 
	       (paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location )), 
	       paf->modifier, 
	       paf->duration,
	       affect_bit_name( paf->bitvector ), 
	       affect2_bit_name( paf->bitvector ), paf->level ); 
}//end mstat
	      
void do_tfind( CHAR_DATA *ch, char *argument ){
  char buf[MSL];
  BUFFER *buffer;
  TRAP_INDEX_DATA *pTrap;
  bool fAll = FALSE, found = FALSE;

/* arg check */
  if ( IS_NULLSTR(argument)){
    send_to_char( "Find what trap?\n\r", ch );
    return;
  }
  if (!str_cmp("all", argument)){
    fAll = TRUE;
  }
/* start the search */
  buffer=new_buf();
  for ( pTrap = trap_index_list; pTrap; pTrap = pTrap->next ){
    if ( fAll || is_name( argument, pTrap->name ) ){
      found = TRUE;
      sprintf( buf, "[%5d] %s\n\r", pTrap->vnum, pTrap->name );
      add_buf( buffer, buf );
    }
  }
  if ( !found )
    send_to_char( "No traps by that name.\n\r", ch );
  else
    page_to_char(buf_string(buffer),ch);
  free_buf(buffer);
}

void do_hfind( CHAR_DATA *ch, char *argument ){
  char buf[MSL];
  BUFFER *buffer;
  HELP_DATA *pHelp;
  bool found = FALSE;

/* arg check */
  if ( IS_NULLSTR(argument)){
    send_to_char( "Find what help?\n\r", ch );
    return;
  }

/* start the search */
  buffer=new_buf();
  for ( pHelp = help_first; pHelp; pHelp = pHelp->next ){
    if ( is_name( argument, pHelp->keyword ) ){
      found = TRUE;
      sprintf( buf, "[%5d] %s\n\r", pHelp->vnum, pHelp->keyword );
      add_buf( buffer, buf );
    }
  }
  if ( !found )
    send_to_char( "No helps with that keyword.\n\r", ch );
  else
    page_to_char(buf_string(buffer),ch);
  free_buf(buffer);
}

void do_cfind( CHAR_DATA *ch, char *argument ){
  char buf[MSL];
  BUFFER *buffer;
  CABAL_INDEX_DATA *pCab;
  bool found = FALSE;
  bool fAll = FALSE;

/* arg check */
  if ( IS_NULLSTR(argument)){
    send_to_char( "Find what cabal?\n\r", ch );
    return;
  }
  if (!str_cmp( "all", argument)){
    fAll = TRUE;
  }

/* start the search */
  buffer=new_buf();
  for ( pCab = cabal_index_list; pCab; pCab = pCab->next ){
    if ( fAll || is_name( argument, pCab->name ) ){
      found = TRUE;
      sprintf( buf, "[%5d] %s\n\r", pCab->vnum, pCab->name );
      add_buf( buffer, buf );
    }
  }
  if ( !found )
    send_to_char( "No cabals with that name.\n\r", ch );
  else
    page_to_char(buf_string(buffer),ch);
  free_buf(buffer);
}


void do_vnum(CHAR_DATA *ch, char *argument)
{
    char arg[MIL];
    char *string;
    string = one_argument(argument,arg);
    if (arg[0] == '\0')
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  vnum obj [material] <name>\n\r",ch);
	send_to_char("  vnum weapon <type>\n\r",ch);
	send_to_char("  vnum mob <name>\n\r",ch);
	send_to_char("  vnum trap <name>\n\r",ch);
	send_to_char("  vnum help <keyword>\n\r",ch);
	send_to_char("  vnum skill <skill or spell>\n\r",ch);
	send_to_char("  vnum unique\n\r",ch);
	send_to_char("  vnum rare\n\r",ch);
	send_to_char("  vnum weight\n\r",ch);
	send_to_char("  vnum extra <extra flag>\n\r",ch);
	send_to_char("  vnum affect <affect number>\n\r",ch);
	send_to_char("  vnum special\n\r",ch);
	send_to_char("  vnum shop\n\r",ch);
	send_to_char("  vnum wear <wear number>\n\r",ch);
	send_to_char("  vnum gold\n\r",ch);
	send_to_char("  vnum immune\n\r",ch);
	return;
    }
    if (!str_cmp(arg,"obj"))
    {	do_ofind(ch,string); 	return;    }
    if (!str_cmp(arg,"weapon"))
    {	do_wepfind(ch,string); 	return;    }
    if (!str_cmp(arg,"trap"))
    {	do_tfind(ch,string); 	return;    }
    if (!str_cmp(arg,"help"))
    {	do_hfind(ch,string); 	return;    }
    if (!str_cmp(arg,"cabal"))
    {	do_cfind(ch,string); 	return;    }
    if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
    { 	do_mfind(ch,string);	return;    }
    if (!str_cmp(arg,"skill") || !str_cmp(arg,"spell") ||  !str_cmp(arg,"effect") )
    {	do_slookup(ch,string);	return;    }
    if (!str_cmp(arg,"unique"))
    {   do_lfind(ch,argument);	return;    }
    if (!str_cmp(arg,"rare"))
    {   do_rfind(ch,argument);	return;    }
    if (!str_cmp(arg,"weight"))
    {	do_wfind(ch,argument);return;    }
    if (!str_cmp(arg,"extra"))
    {	do_efind(ch,string);	return;    }
    if (!str_cmp(arg,"affect"))
    {	do_sfind(ch,string);	return;    }
    if (!str_cmp(arg,"special"))
    {	do_ssfind(ch,string);	return;    }
    if (!str_cmp(arg,"shop"))
    {	do_skfind(ch,string);	return;    }
    if (!str_cmp(arg,"wear"))
    {	do_wefind(ch,string);	return;    }
    if (!str_cmp(arg,"gold"))
    {	do_gofind(ch,string);	return;    }
    if (!str_cmp(arg,"immune"))
    {	do_imfind(ch,string);	return;    }
    do_mfind(ch,argument);
    do_ofind(ch,argument);
}

void do_mfind( CHAR_DATA *ch, char *argument )
{
    extern int top_mob_index;
    char buf[MSL];
    BUFFER *buffer;
    char arg[MIL];
    MOB_INDEX_DATA *pMobIndex;
    int vnum, nMatch = 0;
    bool fAll = FALSE, found = FALSE;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Find whom?\n\r", ch );
	return;
    }
    buffer=new_buf();
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
	if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( fAll || is_name( argument, pMobIndex->player_name ) )
	    {
		found = TRUE;
                sprintf( buf, "[%5d] %s\n\r", pMobIndex->vnum, pMobIndex->short_descr );
                add_buf( buffer, buf );
	    }
	}
    if ( !found )
	send_to_char( "No mobiles by that name.\n\r", ch );
    else
        page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_ofind( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    char buf[MSL], buf2[MSL], arg[MIL];
    BUFFER *buffer;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, nMatch = 0;
    bool found = FALSE;
    bool fMat = FALSE;
    bool fOprog = FALSE;
    bool fType = FALSE;

    if ( IS_NULLSTR(argument))
    {
	send_to_char( "ofind [material/oprog/type] <name>?\n\r", ch );
	return;
    }
    argument = one_argument( argument, arg );

    if (!str_cmp(arg, "material")){
      fMat = TRUE;
    }
    else if (!str_cmp(arg, "type")){
      fType = TRUE;
      if (flag_value( type_flags, argument )  == NO_FLAG) {
	send_to_char("No such item type found.\n\r", ch);
	return;
      }
    }
    else if (!str_cmp(arg, "oprog")){
      fOprog = TRUE;
      if (!is_number(argument)){
	send_to_char("This option requires a numerical argument.\n\r", ch);
	return;
      }
    }

    buffer=new_buf();
    for ( vnum = 0; nMatch < top_obj_index; vnum++ ){
      if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL ){
	nMatch++;
	if ( fMat ){
	  if (str_cmp(pObjIndex->material, argument))
	    continue;
	}
	else if ( fOprog ){
	  if (pObjIndex->oprogs == NULL
	      || pObjIndex->oprogs->vnum != atoi(argument))
	    continue;
	}
	else if ( fType ){
	  if (pObjIndex->item_type != flag_value( type_flags, argument ) )
	    continue;
	}
	else if (!is_name( arg, pObjIndex->name ) )
	  continue;
	found = TRUE;
	if (CAN_WEAR(pObjIndex, ITEM_UNIQUE))
	  sprintf(buf2, "u");
	else if (CAN_WEAR(pObjIndex, ITEM_RARE))
	  sprintf(buf2, "r");
	else
	  sprintf(buf2, " ");
	sprintf( buf, "[%5d] [%1s] %s\n\r", pObjIndex->vnum, buf2, pObjIndex->short_descr );
	add_buf( buffer, buf );
      }
    }
    
    if ( !found )
      send_to_char( "No objects by that name.\n\r", ch );
    else
      page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_wepfind( CHAR_DATA *ch, char *argument ){
    extern int top_obj_index;
    OBJ_INDEX_DATA *pObjIndex;

    char buf[MSL], buf2[MSL], arg[MIL], arg2[MIL], arg3[MIL];
    BUFFER *buffer;
    int vnum, nMatch = 0, type = 0, wear = 0, extra = 0, spec = 0;
    bool found = FALSE;

    argument = one_argument(argument, arg);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if ( IS_NULLSTR(arg)){
      send_to_char( "wfind <type> [wearflags] [extraflags]\n\r", ch );
      return;
    }
    else if (!str_prefix(arg, "exotic"))
      type = WEAPON_EXOTIC;
    else if ( (type = weapon_table[weapon_lookup(arg)].type) < 0){
      send_to_char("No such weapon type.\n\r", ch);
      return;
    }

    if ( (wear = flag_lookup2(arg2, wear_flags)) == NO_FLAG)
      wear = 0;
    if ( (extra = flag_lookup2(arg3, extra_flags)) == NO_FLAG)
      extra = 0;
    if ( (spec = flag_lookup2(argument, weapon_type2)) == NO_FLAG)
      spec = 0;

    buffer=new_buf();

    for ( vnum = 0; nMatch < top_obj_index; vnum++ ){
      if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL)
	continue;
      nMatch++;
      if (pObjIndex->value[0] != type 	   
	  || pObjIndex->item_type != ITEM_WEAPON)
	continue;
      if (wear && !CAN_WEAR(pObjIndex, wear))
	continue;
      if (extra && !IS_SET(pObjIndex->extra_flags, extra))
	continue;
      if (spec && !IS_SET(pObjIndex->value[4], spec))
	continue;

      found = TRUE;

      if (CAN_WEAR(pObjIndex, ITEM_UNIQUE))
	sprintf(buf2, "u");
      else if (CAN_WEAR(pObjIndex, ITEM_RARE))
	sprintf(buf2, "r");
      else
	sprintf(buf2, " ");
      sprintf( buf, "[%5d] [%1s] %s\n\r", pObjIndex->vnum, buf2, pObjIndex->short_descr );
      add_buf( buffer, buf );
    }
    if ( !found )
      send_to_char( "No weapons of that type.\n\r", ch );
    else
      page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}


void do_owhere(CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    BUFFER *buffer;
    OBJ_DATA *obj, *in_obj;
    bool found = FALSE;
    int number = 0, max_found = 200;
    buffer = new_buf();
    if (argument[0] == '\0')
    {
	send_to_char("Find what?\n\r",ch);
	return;
    }
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
      /* skip stained stuff on imms */
      if (!str_cmp(argument, "stained") 
	  && obj->carried_by
	  && IS_IMMORTAL(obj->carried_by))
	continue;
        if ( can_see_obj( ch, obj ) 
	     && obj->level <= 60
	     && (is_name( argument, obj->name )
		 || (is_number(argument) && atoi(argument) == obj->pIndexData->vnum)
		 || (!str_cmp(argument, "stained") 
		     && IS_SET(obj->extra_flags, ITEM_STAIN))))
	{
            found = TRUE;
            number++;
            for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj );
            {
                if ( in_obj->carried_by != NULL && can_see(ch,in_obj->carried_by) && in_obj->carried_by->in_room != NULL)
                    sprintf( buf, "%3d) %s is carried by %s [Room %d]\n\r",
                      number, obj->short_descr, PERS2(in_obj->carried_by), in_obj->carried_by->in_room->vnum );
                else if (in_obj->in_room != NULL && can_see_room(ch,in_obj->in_room))
                    sprintf( buf, "%3d) %s is in %s [Room %d]\n\r",
		      number, obj->short_descr, in_obj->in_room->name,in_obj->in_room->vnum);
                else
                    sprintf( buf, "%3d) %s is somewhere\n\r",number, obj->short_descr);
            }
            buf[0] = UPPER(buf[0]);
            add_buf(buffer,buf);
            if (number >= max_found)
                break;
	}
    }
    if ( !found )
        send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
    else
        page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_twhere(CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    BUFFER *buffer;
    TRAP_DATA *pTrap;

    bool found = FALSE;
    bool fAll = FALSE;

    int number = 0, max_found = 200;
    buffer = new_buf();

    if (argument[0] == '\0'){
      send_to_char("Find what trap or \"all\"?\n\r",ch);
      return;
    }
    if (!str_cmp("all", argument))
      fAll = TRUE;
    for ( pTrap = trap_list; pTrap != NULL; pTrap = pTrap->next ){
      if (!fAll && !is_name( argument, pTrap->name )
	  && (!is_number(argument) || atoi(argument) != pTrap->vnum))
	continue;
      found = TRUE;
      number++;
      if (pTrap->on_obj){
	OBJ_DATA* obj = pTrap->on_obj;
	CHAR_DATA* carried = in_char( obj );
	OBJ_DATA* stored = in_obj( obj );
	sprintf( buf, "%3d) %s is on object %s [Room %d]\n\r",
		 number, 
		 pTrap->name, 
		 obj->short_descr, 
		 carried ? carried->in_room->vnum : stored->in_room->vnum);
      }
      else if (pTrap->on_exit){
	EXIT_DATA* pExit = pTrap->on_exit;
	sprintf( buf, "%3d) %s is on exit to %s [to Room %d]\n\r",
		 number, 
		 pTrap->name, 
		 pExit->to_room ? pExit->to_room->name : "somewhere", 
		 pExit->to_room ? pExit->to_room->vnum : 0); 

      }
      else 
	sprintf( buf, "%3d) %s is nowhere.\n\r", number, pTrap->name);
      buf[0] = UPPER(buf[0]);
      add_buf(buffer,buf);
      if (number >= max_found)
	break;
    }
    if ( !found )
      send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
    else
      page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

/* Viri:
Searches for rooms with given flag in "room" or "room2" amogst given
range of vnums
*/
void do_rwhere( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    char arg1[MIL];
    char arg2[MIL];
    char arg3[MIL];

    const struct flag_type *flag_table = room_flags;
    ROOM_INDEX_DATA* room;

    BUFFER *buffer;
    int min = 0;
    int max = 0;
    int flag = 0;
    int tot_flag = 0;
    int i = 0;
    int count = 0;
    int value = 0;

    bool fFlag = FALSE;
    bool fHeal = FALSE;
    bool fMana = FALSE;
    bool fLessThen = FALSE;

    if ( argument[0] == '\0' ){
      send_to_char("rwhere [room/room2/heal/mana] <flag1 flag2 flag3..flagN> <min_vnum max_vnum>\n\r"\
		   "searches rooms between min_vnum and max_vnum for given flags.\n\r"\
		   "Displays a match.  If min_vnum is ommited, searches ALL rooms in the mud.\n\r", ch);
      return;
    }

    /* get the type of flags we will be looking for */
    argument = one_argument(argument, arg1);
    if (!str_cmp(arg1, "room")){
      fFlag = TRUE;
      flag_table  = room_flags;
    }
    else if (!str_cmp(arg1, "room2")){
      fFlag = TRUE;
      flag_table  = room_flags2;
    }
    else if (!str_cmp(arg1, "heal"))
      fHeal = TRUE;
    else if (!str_cmp(arg1, "mana"))
      fMana = TRUE;
    else{
      do_rwhere(ch,"");
      return;
    }

    /* get the actual flags we are matching */
    argument = one_argument(argument, arg2);
    if (arg2[0] == '\0'){
      do_rwhere(ch, "");
      return;
    }

    if (fFlag){
      while ( arg2[0] != '\0' && (flag = atoi(arg2)) == 0){
	if ( (flag = flag_value( flag_table, arg2)) == NO_FLAG){
	  sendf(ch, "\"%s\" No such flag found in %s.\n\r", arg2, arg1);
	  show_flag_cmds( ch, flag_table);
	  tot_flag = 0;
	  break;
	}
	tot_flag |= flag;
	argument = one_argument(argument, arg2);
      }
    }
    else if ( (value = atoi (arg2)) < 0){
      value *= -1;
      fLessThen = TRUE;
    }


    /* Get the vnum bounds for search now */
    argument = one_argument(argument, arg3);
    min = atoi(arg2);
    max = atoi(arg3);

    /* small logic check */
    if (min > max){
      int temp = max;
      max = min;
      min = temp;
    }
    /* In case of failure to find flags */
    if (fFlag && tot_flag < 1){
      send_to_char("Incorrect flag passed.\n\r", ch);
      return;
    }

    if (min)
      sendf(ch, "Running search on given flags from %d through to %d.\n\r", min, max);
    else
      send_to_char("Running search on given flags on all rooms!\n\r", ch);

    /* start running through allrooms matching correct flag */
    buf[0] = '\0'; /* Lazyman's flag for found match */
    buffer = new_buf();
    for (i = 0; i < MAX_KEY_HASH; i++){
      for (room = room_index_hash[i]; room != NULL; room = room->next){

	/* if min/max specified we match them now */
	if (min && (room->vnum < min || room->vnum > max))
	  continue;

	/* match flags */
	if (fFlag && flag_table == room_flags && tot_flag == (room->room_flags & tot_flag)){
	  sprintf(buf, "%3d) [%5d] %-38s [%5d] %s\n\r",
		  ++count, room->vnum, room->name,
		  room->area->vnum, room->area->name);
	  add_buf(buffer,buf);
	}
	else if (fFlag && flag_table == room_flags2 && tot_flag == (room->room_flags2 & tot_flag)){
	  sprintf(buf, "%3d) [%5d] %-38s [%5d] %s\n\r",
		  ++count, room->vnum, room->name,
		  room->area->vnum, room->area->name);
	  add_buf(buffer,buf);
	}
	else if (fMana && ( (fLessThen && room->mana_rate < value)
			    || (!fLessThen && room->mana_rate >= value))){
	  sprintf(buf, "%3d) [%5d] %-38s [%5d] %s\n\r",
		  ++count, room->vnum, room->name,
		  room->area->vnum, room->area->name);
	  add_buf(buffer,buf);
	}
	else if (fHeal && ( (fLessThen && room->heal_rate < value)
			    || (!fLessThen && room->heal_rate >= value))){
	  sprintf(buf, "%3d) [%5d] %-38s [%5d] %s\n\r",
		  ++count, room->vnum, room->name,
		  room->area->vnum, room->area->name);
	  add_buf(buffer,buf);
	}
      }//END for (room = 
    }//END for (i = 0

    if ( buf[0] == '\0' )
	send_to_char("No Match.\n\r", ch);
    else
    	page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}


void do_mwhere( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    BUFFER *buffer;
    CHAR_DATA *victim;
    bool found = FALSE;
    int count = 0;
    if ( argument[0] == '\0' )
	return;
    buffer = new_buf();
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
	if ( victim->in_room == NULL )
	    continue;
        if ((is_name( argument, victim->name ) && can_see(ch,victim))
	|| (IS_NPC(victim) && is_number(argument) && atoi(argument) == victim->pIndexData->vnum))
	{
	    found = TRUE;
	    count++;
	    sprintf( buf, "%3d) [%5d] %-28s [%5d] %s\n\r",
	      count, IS_NPC(victim) ? victim->pIndexData->vnum : 0, IS_NPC(victim) ? victim->short_descr : victim->name,
              victim->in_room->vnum, victim->in_room->name );
	    add_buf(buffer,buf);
	}
    }
    if ( !found )
	sendf(ch, "You didn't find any %s.\n\r",argument);
    else
    	page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_reboo( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
}

void do_reboot( CHAR_DATA *ch, char *argument )
{
  char buf[MSL], arg[MIL];
  extern bool merc_down;
  CHAR_DATA *player, *player_next;
  int num_ticks;
  bool found = FALSE;
  AREA_DATA *pArea;
  argument = one_argument(argument,arg);
  
  if (argument[0] && reboot_buffer[0] == '\0')
    strcpy(reboot_buffer, argument);
  
  num_ticks = is_number (arg) ? atoi (arg) : -1979;
  if ( num_ticks == -1979)
    {
      send_to_char ("Value must be numeric.\n\r", ch);
      return;
    }
  else if (num_ticks == 9999)
    {
      sprintf( buf, "Reboot by %s.", ch->name );
      do_immecho( ch, buf );
      merc_down = TRUE;
      for ( player = player_list; player != NULL; player = player_next )
        {
            player_next = player->next_player;
            save_char_obj(player);
	    if (player->desc)
	      close_socket(player->desc);
        }
      return;
    }
  else if (num_ticks == 9000)
    {
      sprintf( buf, "[`&The Voice of God`7]`8 - Shared strings near max allowed.  Rebooting soon.``" );
      do_immecho(NULL,buf);
      do_reboot(NULL,"5");
      return;
    }
    else if (num_ticks == 9001)
      {
        sprintf( buf, "[`&The Voice of God`7]`8 - Perm blocks near max allowed.  Shifting world soon.``" );
        do_immecho(NULL,buf);
        do_reboot(NULL,"5");
	return;
      }
  if (ch != NULL && num_ticks >= 0)
    for (player = player_list; player != NULL; player = player_next)
      {
	player_next = player->next_player;
	if (player->pcdata->pnote && player->pcdata->pnote->text != NULL && player->pcdata->pnote->text[0] != '\0')
	  {
	    sprintf( buf, "[`&The Voice of God`7]`8 - `&%s`8 is writing a note.  World shift delayed by 2 hours.``", player->name);
	    do_immecho(NULL,buf);
	    num_ticks+=2;
	  }
	else if (player->pcdata->pvote && !IS_NULLSTR(player->pcdata->pvote->string))
	  {
	    sprintf( buf, "[`&The Voice of God`7]`8 - `&%s`8 is creating a vote.  World shift delayed by 2 hours.``", player->name);
	    do_immecho(NULL,buf);
	    num_ticks+=2;
	  }
	else if (player->pcdata->ptome != NULL
		 && !IS_NULLSTR(player->pcdata->ptome->text))
	  {
	    sprintf( buf, "[`&The Voice of God`7]`8 - `&%s`8 is scribing a tome.  World shift delayed by 2 hours.``", player->name);
	    do_immecho(NULL,buf);
	    num_ticks+=2;
	  }
	else if (player->pcdata->pcvroom)
	  {
	    sprintf( buf, "[`&The Voice of God`7]`8 - `&%s`8 is constructing a room.  World shift delayed by 2 hours.``", player->name);
	    do_immecho(NULL,buf);
	    num_ticks+=2;
	  }
	else if (player->pcdata->pbbid )
	  {
	    sprintf( buf, "[`&The Voice of God`7]`8 - `&%s`8 is setting a bounty.  World shift delayed by 2 hours.``", player->name);
	    do_immecho(NULL,buf);
	    num_ticks+=2;
	  }
      }
  if (ch != NULL && num_ticks >= 0)
    {
      CABAL_INDEX_DATA* pCab;

      /* check areas */
      for( pArea = area_first; pArea; pArea = pArea->next )
	if ( IS_SET(pArea->area_flags, AREA_CHANGED) )
	  found = TRUE;
      if (found)
	{
	  sprintf( buf, "[`&The Voice of God`7]`8 - A modified area is unsaved.  World shift delayed by 2 hours.``");
	  do_immecho(NULL,buf);
	  num_ticks+=2;
	}
      /* check cabals */
      found = FALSE;
      for(pCab = cabal_index_list; pCab; pCab = pCab->next){
	if (IS_SET(pCab->flags, CABAL_CHANGED)){
	  found = TRUE;
	  break;
	}
      }
      if (found)
	{
	  sprintf( buf, "[`&The Voice of God`7]`8 - A modified cabal is unsaved.  World shift delayed by 2 hours.``");
	  do_immecho(NULL,buf);
	  num_ticks+=2;
	}
    }
  if (num_ticks > 0)
    {
      if (reboot_tick_counter < 1 )
        {
            if (ch == NULL || ch->invis_level > LEVEL_HERO)
	      sprintf( buf, "[`&The Voice of God`7]`8 - World shift initiated by someone.``");
            else
	      sprintf( buf, "[`&The Voice of God`7]`8 - World shift initiated by %s.``", PERS2(ch));
            do_echo(NULL,buf);
        }
      reboot_tick_counter = num_ticks;
      sprintf( buf, "[`&The Voice of God`7]`8 - World shift in %d hour%s.``", reboot_tick_counter, reboot_tick_counter == 1 ? "" : "s");
      do_echo(NULL,buf);
      return;
    }
  else if (num_ticks == 0)
    {
      sprintf(buf,"[`&The Voice of God`7]`8 - World shifting now.``");
      do_echo(NULL,buf);
      do_hotreboot(ch, "");
    }
  else if (reboot_tick_counter > 0 )
    {
      if (ch == NULL || ch->invis_level > LEVEL_HERO)
	sprintf( buf, "[`&The Voice of God`7]`8 - World shift halted by someone.``");
      else
	sprintf( buf, "[`&The Voice of God`7]`8 - World shift halted by %s.``", ch->name);
      reboot_buffer[0] = '\0';
      do_echo(NULL,buf);
      reboot_tick_counter = -1;
      return;
    }
}

void do_shutdow( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
}

void do_shutdown( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    extern bool merc_down;
    CHAR_DATA *player, *player_next;
    if (ch->invis_level <= LEVEL_HERO)
    sprintf( buf, "Shutdown by %s.", ch->name );
    append_file( ch, SHUTDOWN_FILE, buf );
    strcat( buf, "\n\r" );
    if (ch->invis_level <= LEVEL_HERO)
    	do_echo( ch, buf );
    merc_down = TRUE;
    for ( player = player_list; player != NULL; player = player_next)
    {
	player_next = player->next_player;
	save_char_obj(player);
	if (player->desc)
	    close_socket(player->desc);
    }
}

void do_protect( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    if (argument[0] == '\0')
    {
	send_to_char("Protect whom from snooping?\n\r",ch);
	return;
    }
    if ((victim = get_char_world(ch,argument)) == NULL)
    {
	send_to_char("You can't find them.\n\r",ch);
	return;
    }
    if (IS_SET(victim->comm,COMM_SNOOP_PROOF))
    {
	sendf(ch, "%s is no longer snoop-proof.\n\r", PERS2(victim));
	send_to_char("Your snoop-proofing was just removed.\n\r",victim);
	REMOVE_BIT(victim->comm,COMM_SNOOP_PROOF);
    }
    else
    {
	sendf(ch, "%s is now snoop-proof.\n\r", PERS2(victim));
	send_to_char("You are now immune to snooping.\n\r",victim);
	SET_BIT(victim->comm,COMM_SNOOP_PROOF);
    }
}

int check_snoop_loop (Double_List *snoop, CHAR_DATA *victim) {
  Double_List * tmp_list;
  Double_List * tmp_list2;
  
  tmp_list = snoop;
  while (tmp_list != NULL) {
    if ((((DESCRIPTOR_DATA *)tmp_list->cur_entry)->character == victim) || 
	(((DESCRIPTOR_DATA *)tmp_list->cur_entry)->original == victim)) {
      return (-1);
    }
    tmp_list2 = ((DESCRIPTOR_DATA *)tmp_list->cur_entry)->snoop_by;
    while (tmp_list2 != NULL) {
      if (check_snoop_loop (tmp_list2, victim) != 0) {
	return (-1);
      }
      tmp_list2 = tmp_list2->next_node;
    }
    tmp_list = tmp_list->next_node;
  }
  
  return (0);
}
  
void do_snoop( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    Double_List * tmp_list;
    Double_List * tmp_list2;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Snoop whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim->desc == NULL )
    {
	sendf(ch, "%s has no descriptor to snoop.\n\r", PERS2(victim));
	return;
    }
    if ( victim == ch )
    {
      send_to_char( "Cancelling all snoops.\n\r", ch );
      wiznet("$N stops being such a snoop.", ch,NULL,WIZ_SNOOPS,WIZ_SECURE,get_trust(ch));

/* loop over all the descriptors in the game */
      for ( d = descriptor_list; d != NULL; d = d->next ) {
	tmp_list = d->snoop_by;

/* - loop over all the people snooping this descriptor */
	while (tmp_list != NULL) {

/* -- if one of those people is us, remove us from the list of snoopers */
	  if (((DESCRIPTOR_DATA *) tmp_list->cur_entry) == ch->desc) {

/* --- if we are first on the list, handle the remove */
	    if (tmp_list == d->snoop_by) {
	      d->snoop_by = tmp_list->next_node;
	      if (tmp_list->next_node != NULL) {
		tmp_list->next_node->prev_node = NULL;
	      }
	      tmp_list2 = tmp_list;
	      tmp_list = tmp_list->next_node;
	      free (tmp_list2);
	    }

/* --- otherwise we are somewhere else in the list, handle the remove */
	    else {
	      tmp_list->prev_node->next_node = tmp_list->next_node;
	      if (tmp_list->next_node != NULL) {
		tmp_list->next_node->prev_node = tmp_list->prev_node;
	      }
	      tmp_list2 = tmp_list;
	      tmp_list = tmp_list->next_node;
	      free (tmp_list2);
	    }
	  }
	  else {
	    tmp_list = tmp_list->next_node;
	  }
	}
      }
      return;
    }
    if ( ch->in_room != victim->in_room && room_is_private(victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
        sendf(ch, "%s is in a private room.\n\r", PERS2(victim));
        return;
    }
    if ( !have_access(ch,victim->in_room))   
    {
        send_to_char( "Don't think so.\n\r",ch);
        return;
    }
    if ( get_trust( victim ) > get_trust( ch ) || IS_SET(victim->comm,COMM_SNOOP_PROOF))
    {
	sendf(ch, "You failed to snoop %s.\n\r", PERS2(victim) );
	return;
    }

    if ( ch->desc != NULL ) {
      if (check_snoop_loop (ch->desc->snoop_by, victim) != 0) {
	send_to_char( "No snoop loops.\n\r", ch );
	return;
      }
    }

/* if victim is not being snooped yet, set up a new snoop node */
    if (victim->desc->snoop_by == NULL) {
      victim->desc->snoop_by = (Double_List *) malloc (sizeof (Double_List));
      tmp_list = victim->desc->snoop_by;
      tmp_list->prev_node = NULL;
      tmp_list->next_node = NULL;
      tmp_list->cur_entry = (void *) ch->desc;
    }

/* otherwise, add a new snoop node to the end of the list */
    else {
      tmp_list = victim->desc->snoop_by;
      if ((DESCRIPTOR_DATA *)tmp_list->cur_entry == ch->desc) {
	sendf (ch, "You are already snooping %s.\n\r", PERS2(victim));
	return;
      }
      while (tmp_list->next_node != NULL) {
	if ((DESCRIPTOR_DATA *)tmp_list->cur_entry == ch->desc) {
	  sendf (ch, "You are already snooping %s.\n\r", PERS2(victim));
	  return;
	}
	tmp_list = tmp_list->next_node;
      }
      tmp_list->next_node = (Double_List *) malloc (sizeof (Double_List));
      tmp_list->next_node->prev_node = tmp_list;
      tmp_list->next_node->next_node = NULL;
      tmp_list->next_node->cur_entry = (void *) ch->desc;
    }

    sprintf(buf,"$N starts snooping on %s", (IS_NPC(ch) ? victim->short_descr : victim->name));
    wiznet(buf,ch,NULL,WIZ_SNOOPS,WIZ_SECURE,get_trust(ch));
    sendf(ch, "You are now snooping %s.\n\r", PERS2(victim) );
}

void do_switch( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Switch into whom?\n\r", ch );
	return;
    }
    if ( ch->desc == NULL )
	return;
    if ( ch->desc->original != NULL )
    {
	send_to_char( "You are already switched.\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	send_to_char( "Yah, switch into yourself.\n\r", ch );
	return;
    }
    if (!IS_NPC(victim))
    {
	send_to_char("You can only switch into mobiles.\n\r",ch);
	return;
    }
    if ( ch->in_room != victim->in_room && room_is_private(victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
	sendf(ch, "%s is in a private room.\n\r", PERS2(victim));
	return;
    }
    if ( !have_access(ch,victim->in_room))   
    {
        send_to_char( "Don't think so.\n\r",ch);
        return;
    }
    if ( victim->desc != NULL )
    {
	sendf(ch, "Somebody has already switched into %s.\n\r", PERS2(victim) );
	return;
    }
    sprintf(buf,"$N switches into %s",victim->short_descr);
    wiznet(buf,ch,NULL,WIZ_SWITCHES,WIZ_SECURE,get_trust(ch));
    ch->desc->character = victim;
    ch->desc->original  = ch;
    victim->desc        = ch->desc;
    ch->desc            = NULL;
    if (ch->prompt != NULL)
        victim->prompt = str_dup(ch->prompt);
    victim->comm = ch->comm;
    victim->lines = ch->lines;
    sendf(victim, "You have switched into %s.\n\r", PERS2(victim) );
    return;
}

void do_return( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    if ( ch->desc == NULL )
	return;
    if ( ch->desc->original == NULL )
    {
	send_to_char( "You aren't switched.\n\r", ch );
	return;
    }
    send_to_char( "You return to your original body.\n\r", ch );
    if (buf_string(ch->desc->original->pcdata->buffer)[0] != '\0')
        send_to_char("Type replay to see missed tells.\n\r", ch );
    if (ch->prompt != NULL)
    {
	free_string(ch->prompt);
	ch->prompt = NULL;
    }
    sprintf(buf,"$N returns from %s.",ch->short_descr);
    wiznet(buf,ch->desc->original,0,WIZ_SWITCHES,WIZ_SECURE,get_trust(ch));
    ch->comm                  = COMM_NOCHANNELS|COMM_NOYELL|COMM_NOTELL;
    ch->desc->character       = ch->desc->original;
    ch->desc->original        = NULL;
    ch->desc->character->desc = ch->desc; 
    ch->desc                  = NULL;
}

void recursive_clone(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone)
{
    OBJ_DATA *c_obj, *t_obj;
    for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content)
    {
        t_obj = create_object(c_obj->pIndexData,0);
        clone_object(c_obj,t_obj);
        obj_to_obj(t_obj,clone);
        recursive_clone(ch,c_obj,t_obj);
    }
}

void do_clone(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mob;
    OBJ_DATA  *obj;
    char arg[MIL];
    char *rest = one_argument(argument,arg);
    if (arg[0] == '\0')
    {
	send_to_char("Clone what?\n\r",ch);
	return;
    }
    if (!str_prefix(arg,"object"))
    {
	mob = NULL;
	obj = get_obj_here(ch, NULL,rest);
	if (obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }
    else if (!str_prefix(arg,"mobile") || !str_prefix(arg,"character"))
    {
	obj = NULL;
	mob = get_char_room(ch, NULL, rest);
	if (mob == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }
    else
    {
	mob = get_char_room(ch, NULL, argument);
	obj = get_obj_here(ch, NULL,argument);
	if (mob == NULL && obj == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}
    }
    if (obj != NULL)
    {
	OBJ_DATA *clone;
        clone = create_object(obj->pIndexData,0); 
	clone_object(obj,clone);
	if (obj->carried_by != NULL)
	    obj_to_char(clone,ch);
	else
	    obj_to_room(clone,ch->in_room);
 	recursive_clone(ch,obj,clone);
	act("$n has created $p.",ch,clone,NULL,TO_ROOM);
	act("You clone $p.",ch,clone,NULL,TO_CHAR);
	wiznet("$N clones $p.",ch,clone,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
	return;
    }
    else if (mob != NULL)
    {
	CHAR_DATA *clone;
	OBJ_DATA *new_obj;
	char buf[MSL];
	if (!IS_NPC(mob))
	{
	    send_to_char("You can only clone mobiles.\n\r",ch);
	    return;
	}
	clone = create_mobile(mob->pIndexData);
	clone_mobile(mob,clone); 
	for (obj = mob->carrying; obj != NULL; obj = obj->next_content)
	{
            new_obj = create_object(obj->pIndexData,0);
            clone_object(obj,new_obj);
            recursive_clone(ch,obj,new_obj);
            obj_to_char(new_obj,clone);
            new_obj->wear_loc = obj->wear_loc;
	}
	char_to_room(clone,ch->in_room);
        act("$n has created $N.",ch,NULL,clone,TO_ROOM);
        act("You clone $N.",ch,NULL,clone,TO_CHAR);
	sprintf(buf,"$N clones %s.",clone->short_descr);
	wiznet(buf,ch,NULL,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
    }
}

void do_load(CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    argument = one_argument(argument,arg);
    if (arg[0] == '\0')
    {
	send_to_char("Syntax:\n\r",ch);
	send_to_char("  load mob <vnum>\n\r",ch);
	send_to_char("  load obj <vnum> <level>\n\r",ch);
	return;
    }
    if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
    {
	do_mload(ch,argument);
	return;
    }
    if (!str_cmp(arg,"obj"))
    {
	do_oload(ch,argument);
	return;
    }
    do_load(ch,"");
}

void do_mload( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' || !is_number(arg) )
    {
	send_to_char( "Syntax: load mob <vnum>.\n\r", ch );
	return;
    }
    if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
    {
	send_to_char( "No mob has that vnum.\n\r", ch );
	return;
    }
    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    act( "$n has created $N!", ch, NULL, victim, TO_ROOM );
    act( "You have created $N!", ch, NULL, victim, TO_CHAR );
    sprintf(buf,"$N loads %s.",victim->short_descr);
    wiznet(buf,ch,NULL,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
}

void do_oload( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    int level = get_trust(ch);
    argument = one_argument( argument, arg1 );
    if ( arg1[0] == '\0' || !is_number(arg1))
    {
	send_to_char( "Syntax: load obj <vnum>.\n\r", ch );
	return;
    }
    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	send_to_char( "No object has that vnum.\n\r", ch );
	return;
    }
    obj = create_object( pObjIndex, level);
    if ( CAN_WEAR(obj, ITEM_TAKE) )
	obj_to_char( obj, ch );
    else
	obj_to_room( obj, ch->in_room );
    act( "$n has created $p!", ch, obj, NULL, TO_ROOM );
    act( "You have created $p!", ch, obj, NULL, TO_CHAR );
    wiznet("$N loads $p.",ch,obj,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
}

void purge( CHAR_DATA* victim ){
  DESCRIPTOR_DATA *d;

  if ( !IS_NPC(victim) ){
    save_char_obj( victim );
    d = victim->desc;
    extract_char( victim, TRUE );
    if ( d != NULL )
      close_socket( d );
  }
  else
    extract_char( victim, TRUE );
}

void do_purge( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	CHAR_DATA *vnext;
	OBJ_DATA  *obj_next;
	for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
            if ( IS_NPC(victim) && !IS_SET(victim->act,ACT_NOPURGE) && victim != ch )
		extract_char( victim, TRUE );
	}
	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if (!IS_OBJ_STAT(obj,ITEM_NOPURGE))
                extract_obj( obj );
	}
  	act( "$n purges the room!", ch, NULL, NULL, TO_ROOM);
	act( "You have purged the room!",ch, NULL, NULL, TO_CHAR);
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( !IS_NPC(victim) )
    {
	if (ch == victim)
	{
	    send_to_char("Ho ho ho.\n\r",ch);
	    return;
	}
	if (get_trust(ch) < get_trust(victim))
	{
	    send_to_char("Maybe that wasn't a good idea...\n\r",ch);
	    sendf(victim,"%s tried to purge you!\n\r",PERS2(ch));
	    return;
	}
	act("$n disintegrates $N.",ch,0,victim,TO_NOTVICT);
	purge( victim );
	return;
    }
    act( "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
    act( "You have purged $N.", ch, NULL, victim, TO_CHAR );
    extract_char( victim, TRUE );
}

void do_advance( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL];
    char buf [MIL];
    CHAR_DATA *victim;
    int level, iLevel;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: advance <char> <level>.\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( ( level = atoi( arg2 ) ) < 1 || level > 60 )
    {
	send_to_char( "Level must be 1 to 60.\n\r", ch );
	return;
    }
    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust level.\n\r", ch );
	return;
    }
    if ( level == victim->level )
    {
	send_to_char("What's the point of that?\n\r",ch);
	return;
    }

    /* anti cheat */
    SET_BIT(victim->game, GAME_STAIN);

    if ( level < victim->level )
    {
        int temp_prac;
	send_to_char( "Lowering a player's level!\n\r", ch );
	send_to_char( "**** sniff ****\n\r", victim );
	temp_prac = victim->practice;
	victim->level    = 1;
        victim->exp      = exp_per_level(victim,victim->level);
        victim->max_exp  = victim->exp;
	victim->pcdata->perm_hit  = pc_race_table[victim->race].hit;
	victim->pcdata->perm_mana = pc_race_table[victim->race].mana;
	victim->pcdata->perm_move = 100;
	victim->train    = 3;
	victim->max_hit      = victim->pcdata->perm_hit;
	victim->max_mana     = victim->pcdata->perm_mana;
	victim->max_move     = victim->pcdata->perm_move;
	victim->hit      = victim->max_hit;
	victim->mana     = victim->max_mana;
	victim->move     = victim->max_move;
	sprintf( buf, "the %s", title_table [victim->class] [victim->level] [victim->sex == SEX_FEMALE ? 1 : 0] );
	set_title( victim, buf );
	advance_level( victim, TRUE );
	victim->practice = temp_prac;
    }
    else
    {
	send_to_char( "Raising a player's level!\n\r", ch );
	send_to_char( "**** yay ****\n\r", victim );
    }
    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
	victim->level += 1;
	advance_level( victim,FALSE);
    }
    sendf(victim,"You are now level %d.\n\r",victim->level);
    victim->exp     = total_exp(victim)-exp_per_level(victim,victim->level);
    victim->max_exp = victim->exp;
    victim->trust   = 0;
    save_char_obj(victim);
}

void do_trust( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL];
    CHAR_DATA *victim;
    int level;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }
    if ( ( level = atoi( arg2 ) ) < 0 || level > 60 )
    {
	send_to_char( "Level must be 0 (reset) or 1 to 60.\n\r", ch );
	return;
    }
    if ( level > get_trust( ch ) )
    {
	send_to_char( "Limited to your trust.\n\r", ch );
	return;
    }
    victim->trust = level;
}

void do_restore( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if (arg[0] == '\0' || !str_cmp(arg,"room"))
    {
        for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
        {
	    affect_strip(victim, 186);
	    affect_strip(victim, 187);
	    affect_strip(victim, 138);
	    affect_strip(victim, 168);
            affect_strip(victim,gsn_plague);
            affect_strip(victim,gsn_poison);
            affect_strip(victim,gsn_blindness);
            affect_strip(victim,gsn_sleep);
            affect_strip(victim,gsn_curse);
	    affect_strip(victim,gsn_blackjack);
	    affect_strip(victim,gsn_strangle);
            victim->hit 	= victim->max_hit;
            victim->mana	= victim->max_mana;
            victim->move	= victim->max_move;
	    if (!IS_NPC(victim)){
	      victim->pcdata->condition[COND_HUNGER] = 100;
	      victim->pcdata->condition[COND_THIRST] = 100;
	      //dndspells
	      if (IS_DNDS(victim)){
		forget_dndtemplates( ch->pcdata );
		add_memticks(victim->pcdata, 1000, victim->class, victim->level);
	      }
	    }
            update_pos( victim);
            sendf(victim, "%s has restored you.\n\r", PERS(ch,victim));
        }
        sprintf(buf,"$N restored room %d.",ch->in_room->vnum);
        wiznet(buf,ch,NULL,WIZ_RESTORE,WIZ_SECURE,get_trust(ch));
        send_to_char("Room restored.\n\r",ch);
        return;
    }
    if ( get_trust(ch) >=  MAX_LEVEL - 1 && !str_cmp(arg,"all"))
    {
        for (victim = player_list; victim != NULL; victim = victim->next_player)
        {
	    if (victim == NULL)
		continue;
	    affect_strip(victim, 186);
	    affect_strip(victim, 187);
	    affect_strip(victim, 138);
	    affect_strip(victim, 168);
            affect_strip(victim,gsn_plague);
            affect_strip(victim,gsn_poison);
            affect_strip(victim,gsn_blindness);
            affect_strip(victim,gsn_sleep);
            affect_strip(victim,gsn_curse);
	    affect_strip(victim,gsn_blackjack);
	    affect_strip(victim,gsn_strangle);
            victim->hit 	= victim->max_hit;
            victim->mana	= victim->max_mana;
            victim->move	= victim->max_move;	   
	    if (!IS_NPC(victim)){
	      victim->pcdata->condition[COND_HUNGER] = 100;
	      victim->pcdata->condition[COND_THIRST] = 100;
	      //dndspells
	      if (IS_DNDS(victim)){
		add_memticks(victim->pcdata, 1000, victim->class, victim->level);
	      }
	    }
            update_pos( victim);
	    if (victim->in_room != NULL)
                sendf(victim, "%s has restored you.\n\r",PERS(ch,victim));
        }
	send_to_char("All active players restored.\n\r",ch);
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    affect_strip(victim, 186);
    affect_strip(victim, 187);
    affect_strip(victim, 138);
    affect_strip(victim, 168);
    affect_strip(victim,gsn_plague);
    affect_strip(victim,gsn_poison);
    affect_strip(victim,gsn_blindness);
    affect_strip(victim,gsn_sleep);
    affect_strip(victim,gsn_curse);
    affect_strip(victim,gsn_blackjack);
    affect_strip(victim,gsn_strangle);
    victim->hit  = victim->max_hit;
    victim->mana = victim->max_mana;
    victim->move = victim->max_move;
    if (!IS_NPC(victim)){
      victim->pcdata->condition[COND_HUNGER] = 100;
      victim->pcdata->condition[COND_THIRST] = 100;
      //dndspells
      if (IS_DNDS(victim)){
	add_memticks(victim->pcdata, 1000, victim->class, victim->level);
      }
    }
    update_pos( victim );
    sendf(victim, "%s has restored you.\n\r", PERS(ch,victim));
    sprintf(buf,"$N restored %s", IS_NPC(victim) ? victim->short_descr : victim->name);
    wiznet(buf,ch,NULL,WIZ_RESTORE,WIZ_SECURE,get_trust(ch));
    sendf(ch, "You have restored %s.\n\r", PERS2(victim) );
}
 	
void do_freeze( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Freeze whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if (ch == victim){
      send_to_char("Are you sure you want to freeze yourself?\n\r", ch);
      return;
    }
    if ( get_trust( victim ) > get_trust( ch ) )
    {
	sendf(ch, "You failed to freeze %s.\n\r", PERS2(victim) );
	return;
    }
    if ( IS_SET(victim->act, PLR_FREEZE) )
    {
	REMOVE_BIT(victim->act, PLR_FREEZE);
        send_to_char( "You have been thawed, you can play again.\n\r", victim );
	sendf(ch, "FREEZE removed from %s.\n\r", PERS2(victim) );
	sprintf(buf,"$N thaws %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
    else
    {
	SET_BIT(victim->act, PLR_FREEZE);
        send_to_char( "You have been frozen, you can't do anything!\n\r", victim );
	sendf(ch, "FREEZE set on %s.\n\r", PERS2(victim) );
	sprintf(buf,"$N puts %s in the deep freeze.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
    save_char_obj( victim );
}

void do_log( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Log whom?\n\r", ch );
	return;
    }
    if ( !str_cmp( arg, "all" ) )
    {
	if ( fLogAll )
	{
	    fLogAll = FALSE;
	    send_to_char( "Log ALL off.\n\r", ch );
	}
	else
	{
	    fLogAll = TRUE;
	    send_to_char( "Log ALL on.\n\r", ch );
	}
	return;
    }
    if ( !str_cmp( arg, "loop" ) )
    {
	if ( fLogLoop )
	{
	    fLogLoop = FALSE;
	    send_to_char( "Log LOOPS off.\n\r", ch );
	}
	else
	{
	    fLogLoop = TRUE;
	    send_to_char( "Log LOOP on.\n\r`!This causes large I/O activity.  Use with Caution!``\n\r", ch );
	}
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( IS_SET(victim->act, PLR_LOG) )
    {
	REMOVE_BIT(victim->act, PLR_LOG);
	sendf(ch, "LOG removed from %s.\n\r", PERS2(victim) );
    }
    else
    {
	SET_BIT(victim->act, PLR_LOG);
	sendf(ch, "LOG set on %s.\n\r", PERS2(victim) );
    }
}

void do_noemote( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Noemote whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) > get_trust( ch ) )
    {
	sendf(ch, "You failed to noemote %s.\n\r", PERS2(victim) );
	return;
    }
    if ( IS_SET(victim->comm, COMM_NOEMOTE) )
    {
	REMOVE_BIT(victim->comm, COMM_NOEMOTE);
	send_to_char( "You can emote again.\n\r", victim );
	sendf(ch, "NOEMOTE removed from %s.\n\r", PERS2(victim) );
	sprintf(buf,"$N restores emotes to %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
    else
    {
	SET_BIT(victim->comm, COMM_NOEMOTE);
	send_to_char( "You can't emote!\n\r", victim );
	sendf(ch, "NOEMOTE set on %s.\n\r", PERS2(victim) );
	sprintf(buf,"$N revokes %s's emotes.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
}

void do_noyel( CHAR_DATA *ch, char *argument )
{
    send_to_char("You want to noyell, type it out!.\n\r"\
		 "Be warned that once you type it out, "\
		 "you'll never yell again.\n\r"\
		 "If you bother an immortal about it, he'll "\
		 "most likely just ignore you.\n\r",ch);
    return;
}

void do_noyell( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if (IS_NPC(ch))
	return;
    if ( !IS_IMMORTAL(ch))
    {
    	if ( arg[0] == '\0' || str_cmp(arg,"yes"))
	{
            send_to_char( "This command is irreversible.\n\r"\
			  "To complete the command, type: "\
			  "'noyell yes'.\n\r",ch);
	  
	    return;
	}
	else
	{
	    if ( IS_SET(ch->comm, COMM_NOYELL) )
	        send_to_char( "You already won't yell.\n\r", ch );
	    else
	    {
	        SET_BIT(ch->comm, COMM_NOYELL);
	        send_to_char( "You won't yell anymore.\n\r", ch );
	    }
	    return;
	}
    }
    else if ( arg[0] == '\0' )
    {
        send_to_char( "Noyell whom?\n\r",ch);
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) > get_trust( ch ) )
    {
	sendf(ch, "You failed to noyell %s.\n\r", PERS2(victim) );
	return;
    }
    if ( IS_SET(victim->comm, COMM_NOYELL) )
    {
        REMOVE_BIT(victim->comm, COMM_NOYELL);
        send_to_char( "You can yell again.\n\r", victim );
	sendf(ch, "NOYELL removed from %s.\n\r", PERS2(victim) );
        sprintf(buf,"$N restores yells to %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
    else
    {
        SET_BIT(victim->comm, COMM_NOYELL);
        send_to_char( "You can't yell!\n\r", victim );
	sendf(ch, "NOYELL set on %s.\n\r", PERS2(victim) );
        sprintf(buf,"$N revokes %s's yells.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
}

void do_notell( CHAR_DATA *ch, char *argument )
{
    char arg[MIL],buf[MSL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Notell whom?", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) > get_trust( ch ) )
    {
	sendf(ch, "You failed to notell %s.\n\r", PERS2(victim) );
	return;
    }
    if ( IS_SET(victim->comm, COMM_NOTELL) )
    {
	REMOVE_BIT(victim->comm, COMM_NOTELL);
	send_to_char( "You can tell again.\n\r", victim );
	sendf(ch, "NOTELL removed from %s.\n\r", PERS2(victim) );
	sprintf(buf,"$N restores tells to %s.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
    else
    {
	SET_BIT(victim->comm, COMM_NOTELL);
	send_to_char( "You can't tell!\n\r", victim );
	sendf(ch, "NOTELL set on %s.\n\r", PERS2(victim) );
	sprintf(buf,"$N revokes %s's tells.",victim->name);
	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    }
}

void do_peace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
	if ( rch->fighting != NULL )
	    stop_fighting( rch, TRUE );
	if (IS_NPC(rch) && IS_SET(rch->act,ACT_AGGRESSIVE))
	    REMOVE_BIT(rch->act,ACT_AGGRESSIVE);
	if (!IS_NPC(rch) && IS_AFFECTED2(rch, AFF_RAGE))
	{
            REMOVE_BIT(rch->affected2_by,AFF_RAGE);
            affect_strip(rch,gsn_rage);
            rch->hit -= (rch->level * rch->perm_stat[STAT_CON]) /3;
            rch->hit = UMAX(1,rch->hit);
	}
	if (IS_NPC(rch))
	  rch->hunting = NULL;
    }
    send_to_char( "You have stopped all fighting in this room.\n\r", ch );
}

void do_wizlock( CHAR_DATA *ch, char *argument )
{
    mud_data.wizlock = !mud_data.wizlock;
    if ( mud_data.wizlock )
    {
	wiznet("$N has wizlocked the realms.",ch,NULL,0,0,get_trust(ch));
	send_to_char( "Realms wizlocked.\n\r", ch );
    }
    else
    {
	wiznet("$N removes wizlock.",ch,NULL,0,0,get_trust(ch));
	send_to_char( "Realms un-wizlocked.\n\r", ch );
    }
}

void do_newlock( CHAR_DATA *ch, char *argument )
{
    mud_data.newlock = !mud_data.newlock;
    if ( mud_data.newlock )
    {
	wiznet("$N locks out new characters.",ch,NULL,0,0,get_trust(ch));
        send_to_char( "New characters have been locked out.\n\r", ch );
    }
    else
    {
	wiznet("$N allows new characters back in.",ch,NULL,0,0,get_trust(ch));
        send_to_char( "Newlock removed.\n\r", ch );
    }
}

void do_slookup( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    int sn;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Lookup which skill or spell?\n\r", ch );
	return;
    }
    if ( !str_cmp( arg, "all" ) )
	for ( sn = 0; sn < MAX_SKILL + MAX_EFFECTS; sn++ )
	{
	    if ( skill_table[sn].name == NULL )
		break;
	    if (IS_GEN(sn))
	      sendf( ch, "Gen: %3d  Effect: '%s'\n\r", sn, effect_table[GN_POS(sn)].name );
	    else
	      sendf( ch, "Sn: %3d  Type: %3d  Skill/spell: '%s'\n\r", sn, skill_table[sn].spell_type, skill_table[sn].name );
	}
    else
    {
	if ( ( sn = skill_lookup( arg ) ) < 0 )
	{
	    send_to_char( "No such skill or spell.\n\r", ch );
	    return;
	}
	if (IS_GEN(sn))
	  sendf( ch, "Gen: %3d  Effect: '%s'\n\r", sn, effect_table[GN_POS(sn)].name );
	else
	  sendf( ch, "Sn: %3d  Type: %3d  Skill/spell: '%s'\n\r", sn, skill_table[sn].spell_type, skill_table[sn].name );
    }
}

void do_set( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    smash_tilde(argument);
    argument = one_argument(argument,arg);
    if (arg[0] == '\0')
    {
	send_to_char("Syntax:\n\r"\
	"  set mob/char <name> <field> <value>\n\r"\
	"  set most     <number>\n\r"\
	"  set obj      <name> <field> <value>\n\r"\
	"  set pfiles   <number>\n\r"\
	"  set poofin   <string>\n\r"\
	"  set poofout  <string>\n\r"\
	"  set room     <room> <field> <value>\n\r"\
        "  set skill    <name> <spell or skill> <value>\n\r"\
        "  set song     <name> <song> <value>\n\r"\
        "  set string   <name> <field> <string>\n\r",ch);
	return;
    }
    if (!str_prefix(arg,"pfiles"))
    {
	argument = one_argument(argument,arg);
	if (arg[0] == '\0'){
	  sendf(ch, "Pfiles currently set to: %d\n\r", mud_data.pfiles);
	  return;
	}
	if (get_trust(ch) < 60){
	  send_to_char("You don't have the access.\n\r", ch);
	  return;
	}
	mud_data.pfiles = atoi (arg);
	return;
    }
    if (!str_prefix(arg,"mob") || !str_prefix(arg,"char"))
    {	do_mset(ch,argument);	return;    }
    if (!str_prefix(arg,"skill") || !str_prefix(arg,"spell"))
    {	do_sset(ch,argument);	return;    }
    if (!str_prefix(arg,"song"))
    {	do_sgset(ch,argument);	return;    }
    if (!str_prefix(arg,"object"))
    {	do_oset(ch,argument);	return;    }
    if (!str_prefix(arg,"room"))
    {	do_rset(ch,argument);	return;    }
    if (!str_prefix(arg,"most"))
    {	do_most(ch,argument);	return;    }
    if (!str_prefix(arg,"poofin"))
    {	do_bamfin(ch,argument);	return;    }
    if (!str_prefix(arg,"poofout"))
    {	do_bamfout(ch,argument); return;    }
    if (!str_prefix(arg,"string"))
    {	do_string(ch,argument); return;    }
    do_set(ch,"");
}

void do_sset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MIL], arg2 [MIL], arg3 [MIL];
    CHAR_DATA *victim;
    int value, sn = 0;
    bool fAll, fClass;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r",ch);
	send_to_char( "  set skill <name> <spell or skill> <value>\n\r", ch);
	send_to_char( "  set skill <name> all <value>\n\r",ch);  
	send_to_char( "  set skill <name> class <value>\n\r",ch);  
	send_to_char( "  set skill <name> group <groupname> <value>\n\r",ch);  
	send_to_char( "  set skill <name> anat <anatomy> <value>\n\r",ch);  
	send_to_char("   (use the name of the skill, not the number)\n\r",ch);
	return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if (!str_cmp(arg2, "anat")){
      int anat = anatomy_lookup(arg3);
      int value = atoi(argument);
      if (!str_cmp(arg3, "all")){
	int i = 0;
	for (i = 0; i < MAX_ANATOMY; i++){
	  victim->pcdata->anatomy[i] = value;
	}
	send_to_char("All anatomies set.\n\r", ch);
	return;
      }
      if (!is_number(argument)){
	send_to_char("Value must be numeric.\n\r", ch);
	return;
      }
      victim->pcdata->anatomy[anat] = value;
      sendf(ch, "%s's %s now at %d\n\r", PERS2(victim), anatomy_table[anat].name, victim->pcdata->anatomy[anat]);
      return;
    }
    else if (!str_cmp(arg2, "group")){
      int gr = ss_group_lookup(arg3);
      int value = atoi(argument);
      if (!str_cmp(arg3, "all")){
	int i = 0;
	for (i = 0; i < MAX_GROUPS; i++){
	  victim->pcdata->ss_picked[i] = value;
	}
	send_to_char("All groups set.\n\r", ch);
	return;
      }
      if (!gr){
	int i = 0;
	send_to_char("Invalid group\n\rValid groups are:\n\r", ch);
	for (i = 0; i < MAX_GROUPS && ss_group_table[i].name; i++){
	  sendf(ch, "%d %s\n\r", i, ss_group_table[i].name);
	}
	return;
      }
      if (!is_number(argument)){
	send_to_char("Value must be numeric.\n\r", ch);
	return;
      }
      victim->pcdata->ss_picked[gr] = value;
      sendf(ch, "Skills picked for group \"%s \" now: %d\n\r",
	    ss_group_table[gr].name, victim->pcdata->ss_picked[gr]);
      return;
    }
    fAll = !str_cmp( arg2, "all" );
    fClass = !str_cmp( arg2, "class" );
    if ( !fAll && !fClass && ( sn = skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such skill or spell.\n\r", ch );
	return;
    }
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }
    value = atoi( arg3 );
    if ( value < 0 )
    {
	send_to_char( "Value must be greater then 0.\n\r", ch );
	return;
    }
    if ( fAll )
	for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
	  SKILL_DATA* nsk;
	  if ( skill_table[sn].name == NULL )
	    break;
	  if ( (nsk = nskill_find(victim->pcdata->newskills,sn)) != NULL){
	    if (value == 0)
	      nskill_remove(victim, nsk);
	    else
	      nsk->learned = UMIN(get_skillmax(victim, sn), value);
	  }
	  else
	    victim->pcdata->learned[sn] =UMIN(get_skillmax(victim, sn), value);
        }
    else if ( fClass )
	for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
	  SKILL_DATA* nsk;

	  if ( skill_table[sn].name != NULL)
	    {
	      if (skill_table[sn].skill_level[0] != 69 
		  && skill_table[sn].skill_level[victim->class] < 51
		  && skill_table[sn].skill_level[victim->class] > 0
		  ){
		if ((nsk = nskill_find(victim->pcdata->newskills,sn)) != NULL){
		  if (value == 0)
		    nskill_remove(victim, nsk);
		  else
		    nsk->learned = UMIN(get_skillmax(victim, sn), value);
		}
		else
		  victim->pcdata->learned[sn] = UMIN(get_skillmax(victim, sn), value);
	      }
	      else
		victim->pcdata->learned[sn] = 0;
	    }
	  if (value != 0)
	      {
		set_cleric_skillset(victim, UMIN(100, value));
		set_race_skills(victim, UMIN(100, value));
	      }
	  
        }
    else{
      SKILL_DATA* nsk;
      if ( (nsk = nskill_find(victim->pcdata->newskills,sn)) != NULL){
	if (value == 0)
	  nskill_remove(victim, nsk);
	else
	  nsk->learned = UMIN(get_skillmax(victim, sn), value);
      }
      else
	victim->pcdata->learned[sn] = UMIN(get_skillmax(victim, sn), value);
    }
}

void do_sgset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MIL], arg2 [MIL], arg3 [MIL];
    CHAR_DATA *victim;
    int value, sn = 0;
    bool fAll;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r",ch);
	send_to_char( "  set song <name> <song> <value>\n\r", ch);
	send_to_char( "  set song <name> all <value>\n\r",ch);  
	send_to_char("   (use the name of the song, not the number)\n\r",ch);
	return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    fAll = !str_cmp( arg2, "all" );
    if ( !fAll && ( sn = song_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such skill or spell.\n\r", ch );
	return;
    }
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }
    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
	send_to_char( "Value range is 0 to 100.\n\r", ch );
	return;
    }
    if ( fAll )
	for ( sn = 0; sn < MAX_SONG; sn++ )
        {
	    if ( song_table[sn].name != NULL )
                victim->pcdata->songlearned[sn] = value;
        }
    else
	victim->pcdata->songlearned[sn] = value;
}

void do_mset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MIL], arg2 [MIL], arg3 [MIL];
    CHAR_DATA *victim;
    int value;
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
    if ( arg1[0] == '\0' || arg2[0] == '\0' 
	 || (arg3[0] == '\0' 
	     && (str_cmp(arg2, "maxstat") 
		 && str_cmp(arg2, "newbie")
		 && str_cmp(arg2, "telepath")
		 && str_cmp(arg2, "dndspell")
		 && str_cmp(arg2, "nospells"))
	     ) 
	 )
    {
      send_to_char("Syntax:\n\r"\
		   "  set mob  <name> <field> <value>\n\r"\
		   "      char <name> <field> <value>\n\r"\
		   "  Field being one of:\n\r"\
		   "    str    int    wis     dex    con   sex   class   "\
		   "level maxstat\n\r"\
		   "    race   group  gold    hp     mana  move  prac    "\
		   "deity\n\r"\
		   "    align  train  thirst  hunger drunk hunt  hometown"\
		   "security\n\r"\
		   "    rank   leader master  pet    annoy gimp  nopurge "\
		   "beast_type alias\n\r"\
		   "    garble nowho  mplayed exp    DivineFavor cabalpts "\
		   "newbie nospells enemy\n\r"\
		   "cabalhours telepath dndspell noble royal\n\r", ch);
	return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;
    //ROYAL
    if (!str_prefix(arg2, "royal")){
      if ( IS_NPC(victim) ){
        send_to_char( "Why would you want to make a mob a royal?\n\r",ch);
        return;
      }
      else  if ( IS_SET(victim->act2, PLR_ROYAL)){
	sendf(ch, "%s is no longer royalty.\n\r", PERS2(victim));
	send_to_char( "You have been stripped of your royality!\n\r",victim);
	REMOVE_BIT(victim->act2, PLR_ROYAL);
	return;
      }
      SET_BIT(victim->act2, PLR_ROYAL);
      sendf(ch, "%s is now of the royal blood.\n\r", PERS2(victim));
      send_to_char( "You are now a member of the royality.\n\r",victim);
      return;
    }                                     
    //NOBLE
    if (!str_prefix(arg2, "noble")){
      if ( IS_NPC(victim) ){
        send_to_char( "Why would you want to make a mob a noble?\n\r",ch);
        return;
      }
      else  if ( IS_SET(victim->act2, PLR_NOBLE)){
	sendf(ch, "%s is no longer nobility.\n\r", PERS2(victim));
	send_to_char( "You have been stripped of your nobility!\n\r",victim);
	REMOVE_BIT(victim->act2, PLR_NOBLE);
	return;
      }
      SET_BIT(victim->act2, PLR_NOBLE);
      sendf(ch, "%s is now of the nobility.\n\r", PERS2(victim));
      send_to_char( "You are now a member of the nobility.\n\r",victim);
      return;
    }                                     
//Cabal Points
    if ( !str_prefix( arg2, "cabalpts" ) ){
      if (get_trust(ch) < 60){
	send_to_char("You don't have the access.\n\r", ch);
	return;
      }
      if (IS_NPC(victim)){
	send_to_char("Not on Mobiles.\n\r", ch);
	return;
      }
      if (value < 0)
	value = 0;
      victim->pcdata->cpoints = value * CPTS;
      sendf(ch, "Cabal Points set to: %d.\n\r", GET_CP(victim));
      /* Anti Cheating */
      if (!IS_IMMORTAL(victim))
	SET_BIT(victim->game, GAME_STAIN);
      return;
    }
    if ( !str_prefix( arg2, "cabalhours" ) ){
      if (get_trust(ch) < CREATOR){
	send_to_char("You don't have the access.\n\r", ch);
	return;
      }
      if (IS_NPC(victim)){
	send_to_char("Not on Mobiles.\n\r", ch);
	return;
      }
      else if (victim->pCabal == NULL){
	send_to_char("They must be in a cabal.\n\r", ch );
	return;
      }
      else if (victim->pcdata->member == NULL){
	send_to_char("They must be in a cabal.\n\r", ch );
	return;
      }
	
      if (value < 0)
	value = 0;
      victim->pcdata->member->mhours += 3600 * value - victim->pcdata->member->hours;
      victim->pcdata->member->hours = value * 3600;
      sendf(ch, "Cabal Hours set to: %d, Month: %d.\n\r", victim->pcdata->member->hours / 3600, 
	    victim->pcdata->member->mhours / 3600);
      /* Anti Cheating */
      if (!IS_IMMORTAL(victim))
	SET_BIT(victim->game, GAME_STAIN);
      return;
    }
// Alias
    else if ( !str_prefix( arg2, "alias" ) )
      {
	if (IS_NPC(victim))
	{
	    send_to_char("Not on mobiles.\n\r",ch);
	    return;
	}
	if (!str_prefix(arg3, "none"))
	  {
	    free_string(victim->pcdata->alias);
	    victim->pcdata->alias = str_dup("");
	    send_to_char("Alias has been cleared\n\r", ch);
	    send_to_char("Your alias has been cleared\n\r", victim);
	    return;
	  }
	free_string(victim->pcdata->alias);
	victim->pcdata->alias = str_dup(argument);
	send_to_char("Alias set to:\n\r", ch);
	sendf(ch, "The last thing you see is %s.\n\r", victim->pcdata->alias);
      }
//Deity
    else if ( !str_prefix( arg2, "religion" ) )
      {
	int god;
	int ethos = 0;
	int align = 0;

	if (IS_NPC(victim))
	{
	  send_to_char("Mobiles follow no religion.\n\r",ch);
	  return;
	}
	if (!str_prefix(arg3, "none"))
	  {
	    victim->pcdata->way = 0;
	    ch->pcdata->deity = deity_table[0].god;
	    send_to_char("Religion has been cleared\n\r", ch);
	    return;
	  }
	//get align.
	if (victim->alignment <= EVIL_THRESH)
	  align = DIETY_ALIGN_EVIL;
	else if (victim->alignment >= GOOD_THRESH)
	  align = DIETY_ALIGN_GOOD;
	else
	  align = DIETY_ALIGN_NEUTRAL;

	//get ethos.
	switch (victim->pcdata->ethos)
	  {
	  case 0: ethos = DIETY_ETHOS_LAWFUL;break;
	  case 1: ethos = DIETY_ETHOS_NEUTRAL;break;
	  case 2: ethos = DIETY_ETHOS_CHAOTIC;break;
	  }
	
	if ( (god = way_lookup(arg3)) < 0 )
	  {
	    char buf[MIL];
	    char out[MSL];
	    strcpy(out, "Their choices are: \n\r" );
	    for ( god = 0; (deity_table[god].way != NULL && god < MAX_DIETY); god++ ){
	      //print only dieties that are right for the char.
	      if (!IS_SET(deity_table[god].align, align) || !IS_SET(deity_table[god].ethos, ethos) )
		continue;  
	      sprintf( buf, "%-12s: %-15s (%s)\n\r", 
		       path_table[deity_table[god].path].name, 
		       deity_table[god].way,
		       deity_table[god].god);
	      strcat(out, buf);
	    }
	    send_to_char( out, ch );
	    return;
	  }
	victim->pcdata->deity = deity_table[god].god;
	victim->pcdata->way = god;
	sendf(ch, "Religion set to the way of %s under %s's guidance.\n\r", deity_table[victim->pcdata->way].way, 
	      victim->pcdata->deity);	
	return;
      }
    if ( !str_cmp( arg2, "leader" ) )
    {
	CHAR_DATA *target;
	if (!str_cmp(arg3,"none"))
	{
	    act("Leader for $N is removed.",ch,NULL,victim,TO_CHAR);
	    victim->leader = NULL;
	    return;
	}
    	if ( ( target = get_char_world( ch, arg3 ) ) == NULL )
    	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
    	}
	act("Leader for $N is set.",ch,NULL,victim,TO_CHAR);
	victim->leader = target;
	return;
    }
    if ( !str_cmp( arg2, "master" ) )
    {
	CHAR_DATA *target;
	if (!str_cmp(arg3,"none"))
	{
	    act(",Master for $N is removed.",ch,NULL,victim,TO_CHAR);
	    victim->master = NULL;
	    return;
	}
    	if ( ( target = get_char_world( ch, arg3 ) ) == NULL )
    	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
    	}
	act("Master for $N is set.",ch,NULL,victim,TO_CHAR);
	victim->master = target;
	return;
    }
    if ( !str_cmp( arg2, "pet" ) )
    {
	CHAR_DATA *target;
	if (IS_NPC(victim))
	{
	    send_to_char("Not on NPC's.\n\r",ch);
	    return;
	}
    	if ( ( target = get_char_world( ch, arg3 ) ) == NULL )
    	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
    	}
	if (!IS_NPC(target))
	{
	    send_to_char("Pc's can't be pets.\n\r",ch);
	    return;
	}
	act("Pet for $N is set.",ch,NULL,victim,TO_CHAR);
        SET_BIT(target->affected_by, AFF_CHARM);
        SET_BIT(target->act, ACT_PET);
	add_follower(target,victim);
	target->leader = victim;
	victim->pet = target;
	return;
    }
    if ( !str_cmp( arg2, "annoy" ) )
    {
	if (IS_NPC(victim))
	{
	    send_to_char("Not on Mobiles.\n\r",ch);
	    return;
	}
	if (get_trust(ch) < 60){
	  send_to_char("You don't have the access.\n\r", ch);
	  return;
	}
	if (IS_SET(victim->act2,PLR_ANNOY))
	{
	    REMOVE_BIT(victim->act2,PLR_ANNOY);
	    act("Annoy removed from $N.",ch,NULL,victim,TO_CHAR);
	}
	else	
	{
	    SET_BIT(victim->act2,PLR_ANNOY);
	    act("Annoy set on $N.",ch,NULL,victim,TO_CHAR);
	}
	return;
    }
    if ( !str_cmp( arg2, "nopurge" ) )
    {
	if (IS_NPC(victim))
	{
	    send_to_char("Not on Mobiles.\n\r",ch);
	    return;
	}
	if (get_trust(ch) < 60){
	  send_to_char("You don't have the access.\n\r", ch);
	  return;
	}
	if (IS_SET(victim->act2,PLR_EQLIMIT))
	{
	    REMOVE_BIT(victim->act2,PLR_EQLIMIT);
	    act("$N is no longer safe from rare purge.",ch,NULL,victim,TO_CHAR);
	}
	else	
	{
	    SET_BIT(victim->act2,PLR_EQLIMIT);
	    act("$N is now safe from rare purge.",ch,NULL,victim,TO_CHAR);
	}
	return;
    }
    if ( !str_cmp( arg2, "nopurge" ) )
    {
	if (IS_NPC(victim))
	{
	    send_to_char("Not on Mobiles.\n\r",ch);
	    return;
	}
	if (IS_SET(victim->act2,PLR_NOPURGE))
	{
	    REMOVE_BIT(victim->act2,PLR_NOPURGE);
	    act("$N will now be checked for rare purge.",ch,NULL,victim,TO_CHAR);
	}
	else	
	{
	    SET_BIT(victim->act2,PLR_NOPURGE);
	    act("$N will no longer be affected by rare purges.",ch,NULL,victim,TO_CHAR);
	}
	return;
    }
    if ( !str_cmp( arg2, "gimp" ) )
    {
	if (IS_NPC(victim))
	{
	    send_to_char("Not on Mobiles.\n\r",ch);
	    return;
	}
	if (get_trust(ch) < 60){
	  send_to_char("You don't have the access.\n\r", ch);
	  return;
	}
	if (IS_SET(victim->act2,PLR_GIMP))
	{
	    REMOVE_BIT(victim->act2,PLR_GIMP);
	    act("Gimp mode removed from $N.",ch,NULL,victim,TO_CHAR);
	}
	else	
	{
	    SET_BIT(victim->act2,PLR_GIMP);
	    act("Gimp mode set on $N.",ch,NULL,victim,TO_CHAR);
	}
	return;
    }
    if ( !str_cmp( arg2, "garble" ) )
    {
	if (IS_NPC(victim))
	{
	    send_to_char("Not on Mobiles.\n\r",ch);
	    return;
	}
	if (get_trust(ch) < 60){
	  send_to_char("You don't have the access.\n\r", ch);
	  return;
	}
	if (IS_SET(victim->act2,PLR_GARBLE))
	{
	    REMOVE_BIT(victim->act2,PLR_GARBLE);
	    act("Garble mode removed from $N.",ch,NULL,victim,TO_CHAR);
	}
	else	
	{
	    SET_BIT(victim->act2,PLR_GARBLE);
	    act("Garble mode set on $N.",ch,NULL,victim,TO_CHAR);
	}
	return;
    }
    if ( !str_cmp( arg2, "nowho" ) )
    {
	if (IS_NPC(victim))
	{
	    send_to_char("Not on Mobiles.\n\r",ch);
	    return;
	}
	if (get_trust(ch) < 60){
	  send_to_char("You don't have the access.\n\r", ch);
	  return;
	}
	if (IS_SET(victim->act2,PLR_NOWHO))
	{
	    REMOVE_BIT(victim->act2,PLR_NOWHO);
	    act("Nowho removed from $N.",ch,NULL,victim,TO_CHAR);
	}
	else	
	{
	    SET_BIT(victim->act2,PLR_NOWHO);
	    act("Nowho set on $N.",ch,NULL,victim,TO_CHAR);
	}
	return;
    }
    if ( !str_cmp( arg2, "mplayed" ) )
    {
	if (IS_NPC(victim))
	{
	    send_to_char("Pc's only.\n\r",ch);
	    return;
	}
	victim->pcdata->mplayed = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
        return;
    }
    if ( !str_cmp( arg2, "dall" ) )
    {
	if (IS_NPC(victim))
	{
	    send_to_char("Pc's only.\n\r",ch);
	    return;
	}
	victim->pcdata->dall = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
        return;
    }
    if ( !str_cmp( arg2, "maxstat" ) ){
      victim->perm_stat[STAT_STR] =  get_max_train(victim,STAT_STR);
      victim->perm_stat[STAT_INT] =  get_max_train(victim,STAT_INT);
      victim->perm_stat[STAT_WIS] =  get_max_train(victim,STAT_WIS);
      victim->perm_stat[STAT_DEX] =  get_max_train(victim,STAT_DEX);
      victim->perm_stat[STAT_CON] =  get_max_train(victim,STAT_CON);
      act("$N's stats have been maxed.", ch, NULL, victim, TO_CHAR);
      send_to_char("Stats set to max.\n\r", victim);
      /* Anti Cheating */
      if (!IS_IMMORTAL(victim))
	SET_BIT(victim->game, GAME_STAIN);
      return;
    }
    if ( !str_cmp( arg2, "str" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_STR) )
	{
            sendf(ch,"Strength range is 3 to %d\n\r.",get_max_train(victim,STAT_STR));
	    return;
	}
	victim->perm_stat[STAT_STR] = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
        return;
    }
    if ( !str_cmp( arg2, "int" ) )
    {
        if ( value < 3 || value > get_max_train(victim,STAT_INT) )
        {
            sendf(ch,"Intelligence range is 3 to %d.\n\r",get_max_train(victim,STAT_INT));
            return;
        }
        victim->perm_stat[STAT_INT] = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
        return;
    }
    if ( !str_cmp( arg2, "wis" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_WIS) )
	{
            sendf(ch,"Wisdom range is 3 to %d.\n\r",get_max_train(victim,STAT_WIS));
	    return;
	}
	victim->perm_stat[STAT_WIS] = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_cmp( arg2, "dex" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_DEX) )
	{
            sendf(ch,"Dexterity ranges is 3 to %d.\n\r",get_max_train(victim,STAT_DEX));
	    return;
	}
	victim->perm_stat[STAT_DEX] = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_cmp( arg2, "con" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_CON) )
	{
            sendf(ch,"Constitution range is 3 to %d.\n\r",get_max_train(victim,STAT_CON));
	    return;
	}
	victim->perm_stat[STAT_CON] = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_cmp( arg2, "luck" ) )
    {
	if ( value < 3 || value > get_max_train(victim,STAT_LUCK) )
	{
            sendf(ch,"Luck range is 3 to %d.\n\r",get_max_train(victim,STAT_LUCK));
	    return;
	}
	victim->perm_stat[STAT_LUCK] = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_prefix( arg2, "sex" ) )
    {
	if ( value < 0 || value > 2 )
	{
	    send_to_char( "Sex range is 0 to 2.\n\r", ch );
	    return;
	}
	victim->sex = value;
	if (!IS_NPC(victim))
	    victim->pcdata->true_sex = value;
	return;
    }
    if ( !str_prefix( arg2, "class" ) )
    {
	int class;
	if (IS_NPC(victim))
	{
	    send_to_char("Mobiles have no class.\n\r",ch);
	    return;
	}
	class = class_lookup(arg3);
	if ( class == -1 )
 	{
	    char buf[MSL];
 	    send_to_char("Invalid class.\n\r",ch);
            strcpy( buf, "Possible classes are: " );
            for ( class = 0; class_table[class].name != NULL && class < MAX_CLASS; class++ )
            {
                if ( class > 0 )
                    strcat( buf, " " );
                strcat( buf, class_table[class].name );
            }
            strcat( buf, ".\n\r" );
	    send_to_char(buf,ch);
	    return;
	}
	victim->class = class;
	return;
    }
    if ( !str_prefix( arg2, "level" ) )
    {
	if ( !IS_NPC(victim) )
	{
	    send_to_char( "Not on PC's.\n\r", ch );
	    return;
	}
        if ( value < 0 || value > 100 )
	{
            send_to_char( "Level range is 0 to 100.\n\r", ch );
	    return;
	}
	victim->level = value;
	return;
    }
    if ( !str_prefix( arg2, "exp" ) )
    {
	if (IS_NPC(victim))
	{
	    send_to_char("Not on Mobiles.\n\r",ch);
	    return;
	}
	if (get_trust(ch) < 60){
	  send_to_char("You don't have the access.\n\r", ch);
	  return;
	}
	victim->exp = value;
    	victim->max_exp = UMAX( victim->max_exp, victim->exp );
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_prefix( arg2, "gold" ) )
    {
	victim->gold = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_prefix( arg2, "hp" ) )
    {
	if ( value < -10 || value > 30000 )
	{
	    send_to_char( "Hp range is -10 to 30,000 hit points.\n\r", ch );
	    return;
	}
	victim->max_hit = value;
        if (!IS_NPC(victim))
            victim->pcdata->perm_hit = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_prefix( arg2, "mana" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Mana range is 0 to 30,000 mana points.\n\r", ch );
	    return;
	}
	victim->max_mana = value;
        if (!IS_NPC(victim))
            victim->pcdata->perm_mana = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_prefix( arg2, "move" ) )
    {
	if ( value < 0 || value > 30000 )
	{
	    send_to_char( "Move range is 0 to 30,000 move points.\n\r", ch );
	    return;
	}
	victim->max_move = value;
        if (!IS_NPC(victim))
            victim->pcdata->perm_move = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_prefix( arg2, "practice" ) )
    {
	if ( value < 0 || value > 250 )
	{
	    send_to_char( "Practice range is 0 to 250 sessions.\n\r", ch );
	    return;
	}
	victim->practice = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_prefix( arg2, "train" ))
    {
	if (value < 0 || value > 50 )
	{
	    send_to_char("Training session range is 0 to 50 sessions.\n\r",ch);
	    return;
	}
	victim->train = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_prefix( arg2, "align" ) )
    {
        if ( !IS_NPC(victim) )
	{
            if ( value != -750 && value != 0 && value != 750 )
            {
                send_to_char( "Alignment range is -750, 0, or 750.\n\r", ch );
                return;
            }
	}
        else if ( value < -1000 || value > 1000 )
        {
            send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
            return;
        }
        victim->alignment = value;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if ( !str_prefix( arg2, "thirst" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}
	if ( value < -99 || value > 100 )
	{
	    send_to_char( "Thirst range is -99 to 100.\n\r", ch );
	    return;
	}
	victim->pcdata->condition[COND_THIRST] = value;
	return;
    }
    if ( !str_prefix( arg2, "drunk" ) )
    {
	if ( IS_NPC(victim) )
	{
	    send_to_char( "Not on NPC's.\n\r", ch );
	    return;
	}
	if ( value < -1 || value > 100 )
	{
	    send_to_char( "Drunk range is -1 to 100.\n\r", ch );
	    return;
	}
	victim->pcdata->condition[COND_DRUNK] = value;
	return;
    }
    if ( !str_prefix( arg2, "hunger" ) )
    {
        if ( IS_NPC(victim) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }
        if ( value < -99 || value > 100 )
        {
            send_to_char( "Full range is -99 to 100.\n\r", ch );
            return;
        }
        victim->pcdata->condition[COND_HUNGER] = value;
        return;
    }
    if ( !str_prefix( arg2, "age" ) ){
      if ( IS_NPC(victim) ){
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
      }
      if ( value < 0 || value > 9999 ){
	send_to_char( "Age must be between 0 and 9999.\n\r", ch );
	return;
      }
      else{
	int diff = value - get_age( victim );
	diff = diff * MAX_MONTHS * MAX_WEEKS * MAX_DAYS * MAX_HOURS * 30;
	victim->pcdata->birth_date -= diff;
	sendf( ch, "Age now %d\n\r", get_abs_age( victim ));
	return;
      }
    }
    if (!str_prefix( arg2, "race" ) )
    {
	int race  = race_lookup(arg3);
        if ( race == -1)
	{
	    send_to_char("That is not a valid race.\n\r",ch);
	    return;
	}
	if (!IS_NPC(victim) && !race_table[race].pc_race)
	{
	    send_to_char("That is not a valid player race.\n\r",ch);
	    return;
	}
	victim->race = race;
	return;
    }
    if (!str_prefix( arg2, "enemy" ) )
    {
	int race  = race_lookup(arg3);
	if (IS_NPC(victim)){
	  send_to_char("Not on mobs.\n\r", ch);
	  return;
	}
        if ( race == -1)
	{
	    send_to_char("That is not a valid race.\n\r",ch);
	    return;
	}
	if (race >= MAX_PC_RACE){
	    send_to_char("That is not a valid player race.\n\r",ch);
	    return;
	}
	victim->pcdata->enemy_race = race;
	victim->pcdata->race_kills[race] = 255;
	return;
    }
    if (!str_prefix( arg2, "beast_type" ) )
    {
	int btype  = 0;

	if (IS_NPC(victim)){
	  send_to_char("Not on NPC's.\n\r",ch);
	  return;
	}

	if (!str_cmp(arg3,"none"))
	  btype = BEAST_RACE_MAX;
	else if (!str_cmp(arg3,"tiger"))
	  btype = BEAST_RACE_TIGER;
	else if (!str_cmp(arg3,"wolf"))
	  btype = BEAST_RACE_WOLF;
	else if (!str_cmp(arg3,"bear"))
	  btype = BEAST_RACE_BEAR;
	else if (!str_cmp(arg3,"falcon"))
	  btype = BEAST_RACE_FALCON; 
	else{
	  send_to_char("Beast type may be: tiger, wolf, bear, falcon.\n\r", ch);
	  return;
	}
	victim->pcdata->beast_type = btype;
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
    if (!str_prefix( arg2, "hometown" ) )
    {
        int hometown = hometown_lookup(arg3);
        if (IS_NPC(victim))
        {
            send_to_char("Not on NPC's.\n\r",ch);
            return;
        }
        if ( hometown == -1)
	{
            send_to_char("That is not a valid hometown.\n\r",ch);
	    return;
	}
        victim->hometown = hometown;
	return;
    }
    if (!str_prefix(arg2,"group"))
    {
	if (!IS_NPC(victim))
	{
	    send_to_char("Only on NPCs.\n\r",ch);
	    return;
	}
	victim->group = value;
	return;
    }
    if (!str_cmp(arg2, "hunt"))
    {
        CHAR_DATA *hunted = 0;
        if ( !IS_NPC(victim) )
        {
            send_to_char( "Not on PC's.\n\r", ch );
            return;
        }
        if ( !str_cmp( arg3, "none" ) )
	{
	    victim->hunting = NULL;
	    return;
	}
        else if ( (hunted = get_char_world(victim, arg3)) == NULL )
        {
            send_to_char("Mob couldn't locate the victim to hunt.\n\r", ch);
            return;
        }
        victim->hunting = hunted;
        return;
    }
    if (!str_prefix( arg2, "security" ) )
    {
        if (IS_NPC(victim))
        {
            send_to_char("Not on NPC's.\n\r",ch);
            return;
        }
	if (get_trust(ch) < 60){
	  send_to_char("You don't have the access.\n\r", ch);
	  return;
	}
	if ( value < 0 || value > 9 )
	{
	    send_to_char("Between 0 and 9.\n\r",ch);
	    return;
	}
	victim->pcdata->security = value;
	return;
    }
    if (!str_prefix( arg2, "rank" ) )
    {
        if (IS_NPC(victim))
        {
            send_to_char("Not on NPC's.\n\r",ch);
            return;
        }
	if ( value < 0 || value > 5 )
	{
	    send_to_char("Between 0 and 5.\n\r",ch);
	    return;
	}
	victim->pcdata->rank = value;
	return;
    }
    if ( !str_prefix( arg2, "divinefavor" ) )
    {
	if (IS_NPC(victim))
	{
	    send_to_char("Not on Mobiles.\n\r",ch);
	    return;
	}
	if (get_trust(ch) < 60){
	  send_to_char("You don't have the access.\n\r", ch);
	  return;
	}
	victim->pcdata->divfavor = value;
	update_avatar(victim, FALSE);
	/* Anti Cheating */
	if (!IS_IMMORTAL(victim))
	  SET_BIT(victim->game, GAME_STAIN);
	return;
    }
//newbie chat access
    if ( !str_cmp( arg2, "newbie" ) ){
      if (IS_NPC(victim)){
	send_to_char("Not on Mobiles.\n\r",ch);
	return;
      }
      if (IS_GAME(victim, GAME_NEWBIE)){
	act("Newbie access for $N revoked.", ch, NULL, victim, TO_CHAR);
	act("Your Newbie access has been revoked.", ch, NULL, victim, TO_VICT);
	REMOVE_BIT(victim->game, GAME_NEWBIE);
      }
      else{
	act("Newbie access for $N granted.", ch, NULL, victim, TO_CHAR);
	act("Your now have Newbie access.", ch, NULL, victim, TO_VICT);
	SET_BIT(victim->game, GAME_NEWBIE);
      }
      return;
    }
    if ( !str_cmp( arg2, "nospells" ) ){
      if (get_trust(ch) < IMPLEMENTOR){
	send_to_char("You don't have the access.\n\r", ch);
	return;
      }
      if (IS_NPC(victim)){
	send_to_char("Not on Mobiles.\n\r",ch);
	return;
      }
      if (IS_GAME(victim, GAME_NOSPELLS)){
	act("You have given $N his powers back.", ch, NULL, victim, TO_CHAR);
	act("You have regained your magical powers!", ch, NULL, victim, TO_VICT);
	REMOVE_BIT(victim->game, GAME_NOSPELLS);
      }
      else{
	act("You have taken $N's magical powers away.", ch, NULL, victim, TO_CHAR);
	act("You have lost your magical powers!", ch, NULL, victim, TO_VICT);
	SET_BIT(victim->game, GAME_NOSPELLS);
      }
      return;
    }
    if ( !str_cmp( arg2, "telepath" ) ){
      if (get_trust(ch) < IMPLEMENTOR){
	send_to_char("You don't have the access.\n\r", ch);
	return;
      }
      if (IS_NPC(victim)){
	send_to_char("Not on Mobiles.\n\r",ch);
	return;
      }
      if (IS_SET(victim->comm, COMM_TELEPATH)){
	act("You have removed $N's telepathy.", ch, NULL, victim, TO_CHAR);
	act("You have lost your telepathy!", ch, NULL, victim, TO_VICT);
	REMOVE_BIT(victim->comm, COMM_TELEPATH);
      }
      else{
	act("You have given $N telepathy.", ch, NULL, victim, TO_CHAR);
	act("You have gained telepathy!", ch, NULL, victim, TO_VICT);
	SET_BIT(victim->comm, COMM_TELEPATH);
      }
      return;
    }
    if ( !str_cmp( arg2, "dndspell" ) ){
      if (get_trust(ch) < IMPLEMENTOR){
	send_to_char("You don't have the access.\n\r", ch);
	return;
      }
      if (IS_NPC(victim)){
	send_to_char("Not on Mobiles.\n\r",ch);
	return;
      }
      if (IS_DNDS(victim)){
	act("You have removed $N's dnd spell casting.", ch, NULL, victim, TO_CHAR);
	act("You have had your spell casting abilities altered.", ch, NULL, victim, TO_VICT);
	REMOVE_BIT(victim->game, GAME_DNDS);
      }
      else{
	act("You have given $N dnd spell casting.", ch, NULL, victim, TO_CHAR);
	act("You have had your spell casting abilities altered.", ch, NULL, victim, TO_VICT);
	SET_BIT(victim->game, GAME_DNDS);
      }
      return;
    }
    do_mset( ch, "" );
}

void do_string( CHAR_DATA *ch, char *argument )
{
    char type [MIL], arg1 [MIL], arg2 [MIL], arg3 [MIL];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    smash_tilde( argument );
    argument = one_argument( argument, type );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
    if ( type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char("Syntax:\n\r"\
		     "  string mob/char  <name> <field> <string>\n\r"\
		     "    fields: name short long desc title spec last guard\n\r"\
		     "  string     obj   <name> <field> <string>\n\r"\
		     "    fields: name short long extended\n\r"\
		     " To set a default title, use: set string char <name> title default\n\r", ch);
	return;
    }
    if (!str_prefix(type,"char") || !str_prefix(type,"mob"))
    {
    	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
    	}
     	if ( !str_prefix( arg2, "guard" ) )
    	{
	    if ( IS_NPC(victim) )
	    {
	    	send_to_char( "Not on MOBs.\n\r", ch );
	    	return;
	    }
	    free_string( victim->pcdata->royal_guards );
	    if (!str_cmp(arg3, "none"))
	      victim->pcdata->royal_guards = NULL;
	    else
	      victim->pcdata->royal_guards = str_dup( arg3 );
	    victim->pcdata->royal_confirm = FALSE;
	    sendf(ch, "Royal guard string set to: %s\n\r", arg3);
	    return;
    	}
     	if ( !str_prefix( arg2, "name" ) )
    	{
	    if ( !IS_NPC(victim) )
	    {
	    	send_to_char( "Not on PC's.\n\r", ch );
	    	return;
	    }
	    free_string( victim->name );
	    victim->name = str_dup( arg3 );
	    return;
    	}
     	if ( !str_prefix( arg2, "last" ) )
    	{
	    if ( IS_NPC(victim) )
	    {
	    	send_to_char( "Not on mobs.\n\r", ch );
	    	return;
	    }
	    free_string( victim->pcdata->family );
	    if ( !str_cmp( arg3, "none" ))
		victim->pcdata->family = str_dup ( "" );
	    else
	        victim->pcdata->family = str_dup( arg3 );
	    return;
    	}
    	if ( !str_prefix( arg2, "description" ) )
    	{
    	    free_string(victim->description);
    	    victim->description = str_dup(arg3);
    	    return;
    	}
    	if ( !str_prefix( arg2, "short" ) )
    	{
	    free_string( victim->short_descr );
	    if ( !str_cmp( arg3, "none" ))
		victim->short_descr = str_dup ( "" );
	    else
	    	victim->short_descr = str_dup( arg3 );
	    return;
    	}
    	if ( !str_prefix( arg2, "long" ) )
    	{
	    free_string( victim->long_descr );
	    if ( !str_cmp( arg3, "none" ))
		victim->long_descr = str_dup ( "" );
	    else
	    {
	        strcat(arg3,"\n\r");
	    	victim->long_descr = str_dup( arg3 );
	    }
	    return;
    	}
    	if ( !str_prefix( arg2, "title" ) )
    	{
	    if ( IS_NPC(victim) )
	    {
	    	send_to_char( "Not on NPC's.\n\r", ch );
	    	return;
	    }
	    set_title( victim, arg3 );
	    return;
    	}
    	if ( !str_prefix( arg2, "ethos" ) )
    	{
	    int ethos = ethos_lookup(arg3);
	    if ( IS_NPC(victim) )
	    {
	    	send_to_char( "Not on NPC's.\n\r", ch );
	    	return;
	    }
	    if (ethos == -1)
	    {
		send_to_char( "That's not an ethos, ethos set to neutral.\n\r",ch);
		victim->pcdata->ethos = ETHOS_NEUTRAL;
		return;
	    }
	    victim->pcdata->ethos = ethos;
	    return;
    	}
    	if ( !str_prefix( arg2, "spec" ) )
    	{
	    if ( !IS_NPC(victim) )
	    {
	    	send_to_char( "Not on PC's.\n\r", ch );
	    	return;
	    }
	    if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
	    {
	    	send_to_char( "No such spec fun.\n\r", ch );
	    	return;
	    }
	    return;
    	}
    }
    if (!str_prefix(type,"object"))
    {
   	if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    	{
	    send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
	    return;
    	}
        if ( !str_prefix( arg2, "name" ) )
    	{
	    free_string( obj->name );
	    obj->name = str_dup( arg3 );
	    return;
    	}
    	if ( !str_prefix( arg2, "short" ) )
    	{
	    free_string( obj->short_descr );
	    obj->short_descr = str_dup( arg3 );
	    return;
    	}
    	if ( !str_prefix( arg2, "long" ) )
    	{
	    free_string( obj->description );
	    obj->description = str_dup( arg3 );
	    return;
    	}
    	if ( !str_prefix( arg2, "ed" ) || !str_prefix( arg2, "extended"))
    	{
	    EXTRA_DESCR_DATA *ed;
	    argument = one_argument( argument, arg3 );
	    if ( argument == NULL )
	    {
                send_to_char( "Syntax: string <object> ed <keyword> <string>\n\r",ch );
	    	return;
	    }
 	    strcat(argument,"\n\r");
	    ed = new_extra_descr();
	    ed->keyword		= str_dup( arg3     );
	    ed->description	= str_dup( argument );
	    ed->next		= obj->extra_descr;
	    obj->extra_descr	= ed;
	    return;
    	}
    }
    do_string(ch,"");
}

void do_oset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MIL], arg2 [MIL], arg3 [MIL];
    OBJ_DATA *obj;
    int value;
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char("Syntax:\n\r"\
		     "  set obj <object> <field> <value>\n\r"\
		     "  Field being one of:\n\r"\
		     "    value0 value1 value2 value3 value4 (v1-v4)\n\r"\
		     "    extra wear level weight cost timer\n\r"\
		     "    owner exp condition vnum\n\r", ch );
	return;
    }
    if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
	return;
    }
    if (!str_prefix("owner", arg2))
      {
	CHAR_DATA* vch;
	/*CLEAR OWNER*/
	if (!str_prefix("none", arg3))
	  {
	    if (!clear_owner(obj))
	      send_to_char("Error clearing owner.", ch);
	    return;
	  }
	/*SET OWNER */
	if ( (vch = get_char(arg3)) == NULL)
	  {
	    send_to_char("That character is not in the realms.\n\r", ch);
	    return;
	  }
	if (!set_owner(vch, obj, NULL))
	  send_to_char("Error setting owner.\n\r", ch);
	return;
      }//end ONWER
    value = atoi( arg3 );
/* Living weapon Experience */
    if ( !str_cmp( arg2, "exp" ) )
      {
	AFFECT_DATA* paf;
	if (get_trust(ch) < 60){
	  send_to_char("You don't have the access.\n\r", ch);
	  return;
	}
	if ( (paf = affect_find(obj->affected, gen_malform)) == NULL
	     && (paf = affect_find(obj->affected, gen_hwep)) == NULL)
	  {
	    send_to_char("That item has no Living Weapon effect.\n\r", ch);
	    return;
	  }
	sendf(ch, "Set %s exp from %d to %d.\n\r", 
	      obj->short_descr, 
	      paf->modifier,
	      value);
	paf->modifier = value;
	return;
      }
    if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
    {	obj->value[0] = UMIN(50,value);	return;    }
    if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
    {	obj->value[1] = value;		return;    }
    if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
    {	obj->value[2] = value;		return;    }
    if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
    {	obj->value[3] = value;		return;    }
    if ( !str_cmp( arg2, "value4" ) || !str_cmp( arg2, "v4" ) )
    {	obj->value[4] = value;		return;    }
    if ( !str_prefix( arg2, "extra" ) )
    {	
	obj->extra_flags ^= value;
	return;
    }
    if ( !str_prefix( arg2, "wear" ) )
    {	obj->wear_flags ^= value;	return;    }
    if ( !str_prefix( arg2, "level" ) )
    {	obj->level = value;		return;    }
    if ( !str_prefix( arg2, "weight" ) )
    {	obj->weight = value;		return;    }
    if ( !str_prefix( arg2, "cost" ) && !IS_LIMITED(obj))
    {	obj->cost = value;		return;    }
    if ( !str_prefix( arg2, "timer" ) )
    {	obj->timer = value;		return;    }
    if ( !str_prefix( arg2, "condition" ) )
    {	obj->condition = URANGE(0, value, 100);	return;    }
    if ( !str_prefix( arg2, "vnum" ) ){
      OBJ_INDEX_DATA* pIndex = get_obj_index( value );
      if (pIndex == NULL){
	send_to_char("That object does not exists.\n\r", ch);
	return;
      }
      obj->vnum = pIndex->vnum;
      send_to_char("Vnum Set.\n\r", ch);
      return;
    }
    do_oset( ch, "" );
}

void do_rset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MIL], arg2 [MIL], arg3 [MIL];
    ROOM_INDEX_DATA *location;
    int value;
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char( "Syntax:\n\r"\
		      "  set room <location> <field> <value>\n\r"\
		      "  Field being one of:\n\r"\
		      "    flags sector\n\r", ch );
	return;
    }
    if ( ( location = find_location( ch, arg1 ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }
    if ( ch->in_room != location && room_is_private(location) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
        send_to_char("That room is private right now.\n\r",ch);
        return;
    }
    if ( !have_access(ch,location))   
    {
        send_to_char( "Don't think so.\n\r",ch);
        return;
    }
    if ( !is_number( arg3 ) )
    {
	send_to_char( "Value must be numeric.\n\r", ch );
	return;
    }
    value = atoi( arg3 );
    if ( !str_prefix( arg2, "flags" ) )
    {
	location->room_flags	= value;
	return;
    }
    if ( !str_prefix( arg2, "flags2" ) )
    {
	location->room_flags2	= value;
	return;
    }
    if ( !str_prefix( arg2, "sector" ) )
    {
	location->sector_type	= value;
	return;
    }
    do_rset( ch, "" );
}

void do_sockets( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    char s[100], idle[10], buf[2 * MSL], buf2[MSL], arg[MIL];
    DESCRIPTOR_DATA *d;
    int count = 0;
    buf[0]	= '\0';
    one_argument(argument,arg);
    send_to_char("[De# CS Login Idle] Player Name     Site Number    "\
		 " Site Name\n\r"\
		 "------------------ --------------- --------------- "\
		 "---------------------------\n\r",ch);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->character != NULL && can_see( ch, d->character ) 
	&& (arg[0] == '\0' || is_name(arg,d->character->name)
        || (d->original && is_name(arg,d->original->name))))
	{
            vch = d->original ? d->original : d->character;
            strftime( s, 100, "%I:%M%p", localtime( &vch->logon ) );
            if ( vch->idle > 0 )
                sprintf( idle, "%-2d", vch->idle );
            else
                sprintf( idle, "  " );
	    count++;
            sprintf( buf + strlen(buf), "[%3d %2d %7s %2s] %-15s %-15s %-15s\n\r",
              d->descriptor, d->connected, s, idle,
	      d->original ? d->original->name : d->character ? d->character->name : "(none)", d->ident, d->host);
	}
	if ( d->character == NULL )
	    sprintf( buf + strlen(buf), "[%3d %2d         %2d]                 %-15s %-15s\n\r",
	      d->descriptor, d->connected, d->timer, d->ident, d->host);
    }
    for ( vch = player_list; vch != NULL; vch = vch->next_player )
    {
	if (vch->desc != NULL)
	    continue;
	if ( can_see( ch, vch ) 
	&& (arg[0] == '\0' || is_name(arg,vch->name)))
	{
            strftime( s, 100, "%I:%M%p", localtime( &vch->logon ) );
            if ( vch->idle > 0 )
                sprintf( idle, "%-2d", vch->idle );
            else
                sprintf( idle, "  " );
	    count++;
            sprintf( buf + strlen(buf), "[--- -- %7s %2s] %-15s\n\r",
              s, idle, vch->name);
	}
    }
    if (count == 0)
    {
	send_to_char("No one by that name is connected.\n\r",ch);
	return;
    }
    sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
    strcat(buf,buf2);
    page_to_char( buf, ch );
}

void do_force( CHAR_DATA *ch, char *argument )
{
    char buf[MSL], arg[MIL], arg2[MIL];
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Force whom to do what?\n\r", ch );
	return;
    }
    one_argument(argument,arg2);
    if (!str_cmp(arg2,"delete"))
    {
	send_to_char("That will NOT be done.\n\r",ch);
	return;
    }
    if (!str_prefix("pardon", arg2))
    {
	send_to_char("pardon cannot be forced.\n\r",ch);
	return;
    }
    sprintf( buf, "$n forces you to '%s'.", argument );
    if ( !str_cmp( arg, "all" ))
    {
	CHAR_DATA *player, *player_next;
	if (get_trust(ch) < MAX_LEVEL - 3)
	{
	    send_to_char("Not at your level!\n\r",ch);
	    return;
	}
	for ( player = player_list; player != NULL; player = player_next )
	{
	    player_next = player->next_player;
	    if ( get_trust( player ) <= get_trust( ch ) )
	    {
		act( buf, ch, NULL, player, TO_VICT );
		interpret( player, argument );
	    }
	}
    }
    else if (!str_cmp(arg,"gods"))
    {
        CHAR_DATA *player, *player_next;
        if (get_trust(ch) < MAX_LEVEL - 2)
        {
            send_to_char("Not at your level!\n\r",ch);
            return;
        }
	for ( player = player_list; player != NULL; player = player_next )
	{
	    player_next = player->next_player;
            if ( !IS_NPC(player) && get_trust( player ) <= get_trust( ch ) && player->level > LEVEL_HERO)
	    {
		act( buf, ch, NULL, player, TO_VICT );
		interpret( player, argument );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;
	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}
	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}
    	if ( ch->in_room != victim->in_room && room_is_private(victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
    	{
            sendf(ch, "%s is in a private room.\n\r", PERS2(victim));
            return;
        }
        if ( !have_access(ch,victim->in_room))   
        {
            send_to_char( "Don't think so.\n\r",ch);
            return;
        }
	if ( get_trust( victim ) > get_trust( ch ) )
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}
	if ( !IS_NPC(victim) && get_trust(ch) < MAX_LEVEL -3)
	{
	    send_to_char("Not at your level!\n\r",ch);
	    return;
	}
	act( buf, ch, NULL, victim, TO_VICT );
	interpret( victim, argument );
    }
    send_to_char( "You have forced people to do stuff.\n\r", ch );
}

void do_invis( CHAR_DATA *ch, char *argument )
{
    int level;
    char arg[MSL];
    one_argument( argument, arg );
    if ( arg[0] == '\0' ) 
    {
        if ( ch->invis_level)
        {
            ch->invis_level = 0;
            act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
            act( "You slowly fade back into existence.", ch, NULL, NULL, TO_CHAR );
        }
        else
        {
            ch->invis_level = get_trust(ch);
            act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
            act( "You slowly vanish into thin air.", ch, NULL, NULL, TO_CHAR );
        }
    }
    else
    {
        level = atoi(arg);
        if (level < 2 || level > get_trust(ch))
        {
            send_to_char("Invis level must be between 2 and your level.\n\r",ch);
            return;
        }
        else
        {
            ch->reply = NULL;
            ch->invis_level = level;
            act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
            act( "You slowly vanish into thin air.", ch, NULL, NULL, TO_CHAR );
        }
    }
}

void do_incognito( CHAR_DATA *ch, char *argument )
{
    int level;
    char arg[MSL];
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        if ( ch->incog_level)
        {
            ch->incog_level = 0;
            act( "$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM );
            act( "You are no longer cloaked.", ch, NULL, NULL, TO_CHAR );
        }
        else
        {
            ch->incog_level = get_trust(ch);
            act( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
            act( "You cloak your presence.", ch, NULL, NULL, TO_CHAR );
        }
    }
    else
    {
        level = atoi(arg);
        if (level < 2 || level > get_trust(ch))
        {
            send_to_char("Incog level must be between 2 and your level.\n\r",ch);
            return;
        }
        else
        {
            ch->reply = NULL;
            ch->incog_level = level;
            act( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
            act( "You cloak your presence.", ch, NULL, NULL, TO_CHAR );
        }
    }
}

void do_holylight( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    one_argument(argument,arg);
    if (arg[0] == '\0')
    	victim = ch;
    else if ((victim = get_char_world(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }   	
    if ( IS_NPC(victim) )
	return;
    if ( IS_SET(victim->act, PLR_HOLYLIGHT) )
    {
	REMOVE_BIT(victim->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode off.\n\r", ch );
    }
    else
    {
	SET_BIT(victim->act, PLR_HOLYLIGHT);
	send_to_char( "Holy light mode on.\n\r", ch );
    }
}

void do_hearall( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
	return;
    if ( IS_SET(ch->act, PLR_HEARALL) )
    {
	REMOVE_BIT(ch->act, PLR_HEARALL);
	send_to_char( "Hear all mode off.\n\r", ch );
    }
    else
    {
	SET_BIT(ch->act, PLR_HEARALL);
	send_to_char( "Hear all mode on.\n\r", ch );
    }
}

void do_prefix (CHAR_DATA *ch, char *argument)
{
    if (argument[0] == '\0')
    {
	if (ch->prefix[0] == '\0')
	{
	    send_to_char("You have no prefix to clear.\r\n",ch);
	    return;
	}
	send_to_char("Prefix removed.\r\n",ch);
	free_string(ch->prefix);
	ch->prefix = str_dup("");
	return;
    }
    if (ch->prefix[0] != '\0')
    {
	sendf(ch,"Prefix changed to %s.\r\n",argument);
	free_string(ch->prefix);
    }
    else
    {
	sendf(ch,"Prefix set to %s.\r\n",argument);
        ch->prefix = str_dup(argument);
    }
}

void do_olevel(CHAR_DATA *ch, char *argument)
{
    extern int top_obj_index;
    char buf[MSL], arg[MIL];
    BUFFER *buffer;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, level, nMatch = 0;
    bool found = FALSE;
    argument=one_argument(argument, arg);
    if (!is_number(arg) )
    {
        send_to_char("Syntax: olevel [level]\n\r",ch);
        return;
    }
    level=atoi(arg);
    buffer=new_buf();
    for(vnum=0;nMatch<top_obj_index;vnum++)
    {
        if ( (pObjIndex=get_obj_index(vnum)) !=NULL)
            nMatch++;
        if (pObjIndex != NULL && level == pObjIndex->level)
        {
            found=TRUE;
            sprintf(buf, "[%5d] %s\n\r",pObjIndex->vnum, pObjIndex->short_descr);
            add_buf(buffer,buf);
        }
    }
    if (!found)
        send_to_char("No objects by that level.\n\r",ch);
    else
        page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_mlevel(CHAR_DATA *ch, char *argument)
{
    extern int top_mob_index;
    char buf[MSL], arg[MIL];
    BUFFER *buffer;
    MOB_INDEX_DATA *pMobIndex;
    int vnum, level, nMatch = 0;
    bool found = FALSE;
    argument=one_argument(argument, arg);
    if (!is_number(arg) )
    {
        send_to_char("Syntax: mlevel [level]\n\r",ch);
        return;
    }
    level=atoi(arg);
    buffer=new_buf();
    for(vnum=0;nMatch<top_mob_index;vnum++)
    {
        if ( (pMobIndex=get_mob_index(vnum)) !=NULL)
            nMatch++;
        if (pMobIndex != NULL && level == pMobIndex->level)
        {
            found=TRUE;
            sprintf(buf, "[%5d] %s\n\r",pMobIndex->vnum, pMobIndex->short_descr);
            add_buf(buffer,buf);
        }
    }
    if (!found)
        send_to_char("No mobiles by that level.\n\r",ch);
    else
        page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

DESCRIPTOR_DATA* bring( ROOM_INDEX_DATA* room, char* arg ){
  DESCRIPTOR_DATA *d;
  int garbage;
  char strsave[MIL];
  FILE *fp;
  CHAR_DATA* ch;

  fclose( fpReserve );

  if ( (ch = char_file_active( arg)) != NULL && ch->desc )
    return ch->desc;

  sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( arg ) );
  if ( ( fp = fopen( strsave, "r" ) ) == NULL ){
    perror( strsave );
    fpReserve = fopen( NULL_FILE, "r" );
    return NULL;
  }
  fclose (fp);
  fpReserve = fopen( NULL_FILE, "r" );

  d = new_descriptor();
  d->editor = 69;
  garbage = load_char (d, arg);
  load_obj (d->character);
  
  d->character->next = char_list;
  char_list = d->character;
  d->character->next_player = player_list;
  player_list = d->character;
  
  reset_char(d->character);
  
  char_to_room (d->character, room ? room : get_room_index(d->character->in_room->vnum));
  d->character->logon = 0;

  return d;
}

extern		int              nAllocString;
extern		int              sAllocString;
extern int                     nAllocPerm;
extern int                     sAllocPerm;
void do_bringon( CHAR_DATA *ch, char *argument )
{
  DESCRIPTOR_DATA *d;
  char arg[MIL], buf [MIL];
  int string_count = nAllocString, perm_count = nAllocPerm, string_size = sAllocString, perm_size = sAllocPerm;
  int stringdif, permdif;
  one_argument (argument, arg);
  if ( arg[0] == '\0' )
    {
      send_to_char( "Bringon who?\n\r", ch );
      return;
    }
  if (char_file_active (arg))
    {
      send_to_char ("That character is in the realms at the moment.  Trans them maybe.\n\r", ch);
      return;
    }
  if ( (d = bring( ch->in_room, arg )) == NULL){
    send_to_char ("That character is not in the player files.\n\r", ch);
    return;
  }
  d->character->pcdata->roomnum/=-1;
  act ("$N appears before you.", ch, NULL, d->character, TO_CHAR);
  act ("$N appears before $n.", ch, NULL, d->character, TO_ROOM);
  sprintf (buf, "%s has summonned %s into the realms.\n\r", ch->name, arg);
  wiznet(buf,ch,NULL,WIZ_SNOOPS,WIZ_SECURE,get_trust(ch));
  if (string_count != nAllocString || perm_count < nAllocPerm ) {
    stringdif = sAllocString - string_size;
    permdif = sAllocPerm - perm_size;
    sprintf(buf,"Memcheck: strings %d bytes : perms %d bytes : %s\n\r", 
	    stringdif, permdif, ch->name);
    send_to_char( buf, ch );
  }


}

void do_cwhere(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *player;
    for (player = player_list; player != NULL; player = player->next_player)
        if (can_see(ch,player))
            sendf(ch,"%-35s in [%5d] %s\n\r", player->name, player->in_room->vnum, player->in_room->name);
}

void do_chown( CHAR_DATA *ch, char *argument )
{
    char arg1 [MIL], arg2 [MIL];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Steal what from whom?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    if ( get_trust( victim ) > get_trust( ch ) && victim != ch)
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    if (((obj = get_obj_wear(victim,arg1,ch)) == NULL) && ((obj = get_obj_carry(victim,arg1,ch)) == NULL))
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
        send_to_char( "You have your hands full.\n\r", ch );
        return;
    }
    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
        send_to_char( "You can't carry that much weight.\n\r", ch );
        return;
    }
    obj_from_char( obj );
    obj_to_char( obj, ch );
    act("$p flies from $N into the hands of $n.",ch,obj,victim,TO_NOTVICT);
    act("$p flies from you into the hands of $n.",ch,obj,victim,TO_VICT);
    act("$p flies from $N into your hands.",ch,obj,victim,TO_CHAR);
}

//Added 04/24/02 by Crypticant for Lower Imms
void do_surname (CHAR_DATA *ch, char *argument)
{
  char arg1[MIL], arg2[MIL];
  CHAR_DATA *vict;
  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);
  if (IS_NPC(ch) || !IS_IMMORTAL(ch))
    return;
  if (!arg1[0] || !arg2[0])
    {
      send_to_char ("Syntax: surname <player> <lastname>\n\r", ch);
      return;
    }
  if ((vict = char_file_active (arg1)) == NULL)
    {
      send_to_char ("That person is not in the realms.\n\r", ch);
      return;
    }
  else
    {
      if (!str_cmp(arg2,"none"))
	{
	  vict->pcdata->family[0] = '\0';
	  send_to_char("Last name cleared.\n\r", ch);
	  return;
	}
      if (get_trust (ch) >= get_trust (vict))
	{
	  free_string (vict->pcdata->family);
	  vict->pcdata->family = str_dup (capitalize (arg2));
	  sendf (vict, "Your last name is now %s.\n\r", capitalize(arg2));
	  send_to_char ("Last name set.\n\r", ch);
	  save_char_obj (vict);
	}
      else
	{
	  send_to_char ("You are not trusted to do that.\n\r", ch);
	}
    }
}

void do_rename (CHAR_DATA *ch, char *argument)
{
    DESCRIPTOR_DATA *d;
    char arg1[MIL], arg2[MIL], buf[MSL], filename[MIL];
    FILE *fp;
    CHAR_DATA *vict;
    int garbage, extract_them = 1, success = 1;
    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);
    if (!arg1[0] || !arg2[0])
    {
        send_to_char ("Syntax: rename <old_name> <new_name>\n\r", ch);
        return;
    }

/* check for a bunch of illegalities, including MOB names... */
    if ( !check_parse_name( arg2 ) ) {
      send_to_char ("Illegal name, try another.\n\r", ch);
      return;
    }

    sprintf (filename, "%s%s", PLAYER_DIR, capitalize (arg1));
    if ((fp = fopen (filename, "r")) == NULL)
    {
        send_to_char ("That pfile does not exist.\n\r", ch);
	fp = fopen( NULL_FILE, "r" );
        fclose (fp);
        return;
    }
    fclose (fp);
    sprintf (filename, "%s%s", PLAYER_DIR, capitalize (arg2));
    if ((fp = fopen (filename, "r")) != NULL)
    {
        send_to_char ("That player already exist.  Rename aborted.\n\r", ch);
        fclose (fp);
        return;
    }
    else
	fp = fopen( NULL_FILE, "r" );
    fclose (fp);

    if (name_ok (arg2) == FALSE) {
        send_to_char ("That player name is illegal.  Rename aborted.\n\r", ch);
	return;
    }
    
    if ((vict = char_file_active (arg1)) != NULL)
        extract_them = 0;
    if (!vict)
    {
	d = new_descriptor();
        d->editor = 69;
	garbage = load_char (d, arg1);
	load_obj (d->character);
	vict = d->character;
        vict->next = char_list;
        char_list = vict;
        vict->next_player = player_list;
        player_list = vict;
        char_to_room (vict, ch->in_room);
    }
    if (get_trust (ch) >= get_trust (vict))
    {
	//switch clan name
	if (HAS_CLAN(vict)){
	  if (!ClanRename(GET_CLAN(vict), vict->name, arg2))
	    send_to_char("There has been a problem updating the clan info.\n\r",ch);
	}
        free_string (vict->name);
        vict->name = str_dup (capitalize (arg2));
        save_char_obj (vict);
    }
    else
    {
        send_to_char ("You failed.\n\r", ch);
        success = 0;
    }
    if (extract_them)
        extract_char (vict, TRUE);
    else if (success)
        sendf (vict, "You are now %s.\n\r", capitalize(arg2));
    if (success)
    {
        sprintf( buf, "%s%s", PLAYER_DIR, capitalize( arg1 ));
        unlink (buf);
        send_to_char ("Done renaming.\n\r", ch);
    }
}

const char * name_expand (CHAR_DATA *ch)
{
    int count = 1;
    CHAR_DATA *rch;
    char name[MIL];
    static char outbuf[MIL];
    if (!IS_NPC(ch))
        return ch->name;
    one_argument (ch->name, name);
    if (!name[0])
    {
        strcpy (outbuf, "");
        return outbuf;
    }
    for (rch = ch->in_room->people; rch && (rch != ch);rch =rch->next_in_room)
        if (is_name (name, rch->name))
            count++;
    sprintf (outbuf, "%d.%s", count, name);
    return outbuf;
}

void do_for (CHAR_DATA *ch, char *argument)
{
    char range[MIL], buf[MSL];
    bool fGods = FALSE, fMortals = FALSE, fMobs = FALSE, fEverywhere = FALSE, found;
    ROOM_INDEX_DATA *room, *old_room;
    CHAR_DATA *p, *p_next;
    int i;
    argument = one_argument (argument, range);
    if (!range[0] || !argument[0])
    {
        do_help (ch, "for");
        return;
    }
    if (!str_prefix("quit", argument))
    {
        send_to_char ("Are you trying to crash the MUD or something?\n\r",ch);
        return;
    }
    if (!str_cmp (range, "all"))
    {
        fMortals = TRUE;
        fGods = TRUE;
    }
    else if (!str_cmp (range, "gods"))        fGods = TRUE;
    else if (!str_cmp (range, "mortals"))     fMortals = TRUE;
    else if (!str_cmp (range, "mobs"))        fMobs = TRUE;
    else if (!str_cmp (range, "everywhere"))  fEverywhere = TRUE;
    else				      do_help (ch, "for");
    if (fEverywhere && strchr (argument, '#'))
    {
        send_to_char ("Cannot use FOR EVERYWHERE with the #thingie.\n\r",ch);
        return;
    }
    if (strchr (argument, '#'))
    {
        for (p = char_list; p ; p = p_next)
        {
            p_next = p->next;
            found = FALSE;
            if (!(p->in_room) || (p == ch))
                continue;
            if (IS_NPC(p) && fMobs)
                found = TRUE;
            else if (!IS_NPC(p) && p->level >= LEVEL_IMMORTAL &&fGods)
                found = TRUE;
            else if (!IS_NPC(p) && p->level < LEVEL_IMMORTAL &&fMortals)
                found = TRUE;
            if (found)
            {
                char *pSource = argument, *pDest = buf;
                while (*pSource)
                {
                    if (*pSource == '#')
                    {
                        const char *namebuf = name_expand(p);
                        if (namebuf)
                        while (*namebuf)
                            *(pDest++) = *(namebuf++);
                        pSource++;
                    }
                    else
                        *(pDest++) = *(pSource++);
                }
                *pDest = '\0';
                old_room = ch->in_room;
                char_from_room (ch);
                char_to_room (ch,p->in_room);
                interpret (ch, buf);
                char_from_room (ch);
                char_to_room (ch,old_room);
            }
        }
    }
    else
    {
        for (i = 0; i < MAX_KEY_HASH; i++)
            for (room = room_index_hash[i] ; room ; room =room->next)
                {
                    found = FALSE;
                    if (fEverywhere)
                        found = TRUE;
                    else if (!room->people)
                        continue;
                    for (p = room->people; p && !found; p =p->next_in_room)
                    {
                        if (p == ch)
                            continue;
                        if (IS_NPC(p) && fMobs)
                            found = TRUE;
                        else if (!IS_NPC(p) && (p->level >= LEVEL_IMMORTAL) &&fGods)
                            found = TRUE;
                        else if (!IS_NPC(p) && (p->level <= LEVEL_IMMORTAL) &&fMortals)
                            found = TRUE;
                    }
                    if (found)
                    {
                        old_room = ch->in_room;
                        char_from_room (ch);
                        char_to_room (ch, room);
                        interpret (ch, argument);
                        char_from_room (ch);
                        char_to_room (ch, old_room);
                    }
                }
    }
}

void do_last (CHAR_DATA *ch, char *argument)
{
  DESCRIPTOR_DATA *d = new_descriptor();
  long temp, timediff = 0;
  
  if (argument[0]=='\0'){
    send_to_char("You must provide a name\n\r",ch);
    return;
  }
  
  if (!load_char(d,  capitalize( argument ) )){
    send_to_char("Player File not found.\n\r", ch);
    free_descriptor(d);
    return;
  }

  /* Make sure the room is not saved */    
  d->character->pcdata->roomnum/=-1;
  timediff = mud_data.current_time - d->character->logoff;

  if (timediff < 60){
    sendf(ch,"%s was last seen here %ld second%s ago\n\r",d->character->name,timediff, timediff == 1 ? "" : "s" );
  }
  else if (timediff < 3600){
    temp = timediff % 60;
    temp = (timediff - temp)/60;
    sendf(ch,"%s was last seen here %ld minute%s ago\n\r",d->character->name,temp, temp == 1 ? "" : "s");
  }
  else if (timediff < 86400){
    temp = timediff % 3600;
    temp = (timediff - temp)/3600;
    sendf(ch,"%s was last seen here %ld hour%s ago\n\r",d->character->name,temp, temp == 1 ? "" : "s");
  }
  else{
    temp = timediff % 86400;
    temp = (timediff - temp)/86400;
    sendf(ch,"%s was last seen here %ld day%s ago\n\r",d->character->name,temp, temp == 1 ? "" : "s");
  }

  free_char(d->character);
  /* cleanup */
  free_descriptor(d);
}

void do_flag(CHAR_DATA *ch, char *argument)
{
    char arg1[MIL], arg2[MIL], arg3[MIL], word[MIL];
    char type;
    CHAR_DATA *victim;
    long *flag, old = 0, new = 0, marked = 0, pos;
    const struct flag_type *flag_table;
    argument = one_argument(argument,arg1);
    argument = one_argument(argument,arg2);
    argument = one_argument(argument,arg3);
    type = argument[0];
    if (type == '=' || type == '-' || type == '+')
        argument = one_argument(argument,word);
    if (arg1[0] == '\0')
    {
	send_to_char("Syntax:\n\r"\
		     "  flag mob  <name> <field> <flags>\n\r"\
		     "  flag char <name> <field> <flags>\n\r"\
		     "  mob  flags: act,aff,off,imm,res,vuln,form,part\n\r"\
		     "  char flags: plr,comm, game, aff,imm,res,vuln,\n\r"\
		     "  +: add flag, -: remove flag, = set equal to\n\r"\
		     "  otherwise flag toggles the flags listed.\n\r",ch);
	return;
    }
    if (arg2[0] == '\0')
    {
	send_to_char("What do you wish to set flags on?\n\r",ch);
	return;
    }
    if (arg3[0] == '\0')
    {
	send_to_char("You need to specify a flag to set.\n\r",ch);
	return;
    }
    if (argument[0] == '\0')
    {
	send_to_char("Which flags do you wish to change?\n\r",ch);
	return;
    }
    if (!str_prefix(arg1,"mob") || !str_prefix(arg1,"char"))
    {
	victim = get_char_world(ch,arg2);
	if (victim == NULL)
	{
	    send_to_char("You can't find them.\n\r",ch);
	    return;
	}
	if (!str_prefix(arg3,"act"))
	{
	    if (!IS_NPC(victim))
	    {
		send_to_char("Use plr for PCs.\n\r",ch);
		return;
	    }
	    flag = &victim->act;
	    flag_table = act_flags;
	}
	if (!str_prefix(arg3,"off"))
	{
	    if (!IS_NPC(victim))
	    {
		send_to_char("PCs don't have this.\n\r",ch);
		return;
	    }
	    flag = &victim->off_flags;
	    flag_table = off_flags;
	}
	if (!str_prefix(arg3,"game"))
	{
	    if (IS_NPC(victim))
	    {
		send_to_char("MOBs don't have this.\n\r",ch);
		return;
	    }
	    flag = &victim->game;
	    flag_table = game_flags;
	}
	else if (!str_prefix(arg3,"plr"))
	{
	    if (IS_NPC(victim))
	    {
		send_to_char("Use act for NPCs.\n\r",ch);
		return;
	    }
	    flag = &victim->act;
	    flag_table = plr_flags;
	}
	else if (!str_prefix(arg3,"act2"))
	{
	    if (IS_NPC(victim))
	    {
		send_to_char("Not for NPCs.\n\r",ch);
		return;
	    }
	    flag = &victim->act2;
	    flag_table = plr_flags;
	}
 	else if (!str_prefix(arg3,"aff1"))
	{
	    flag = &victim->affected_by;
	    flag_table = affect_flags;
	}
 	else if (!str_prefix(arg3,"aff2"))
	{
	    flag = &victim->affected2_by;
	    flag_table = affect2_flags;
	}
  	else if (!str_prefix(arg3,"immunity"))
	{
	    flag = &victim->imm_flags;
	    flag_table = imm_flags;
	}
	else if (!str_prefix(arg3,"resist"))
	{
	    flag = &victim->res_flags;
	    flag_table = imm_flags;
	}
	else if (!str_prefix(arg3,"vuln"))
	{
	    flag = &victim->vuln_flags;
	    flag_table = imm_flags;
	}
	else if (!str_prefix(arg3,"form"))
	{
	    if (!IS_NPC(victim))
	    {
	 	send_to_char("Form can't be set on PCs.\n\r",ch);
		return;
	    }
	    flag = &victim->form;
	    flag_table = form_flags;
	}
	else if (!str_prefix(arg3,"parts"))
	{
	    if (!IS_NPC(victim))
	    {
		send_to_char("Parts can't be set on PCs.\n\r",ch);
		return;
	    }
	    flag = &victim->parts;
	    flag_table = part_flags;
	}
	else if (!str_prefix(arg3,"comm"))
	{
	    if (IS_NPC(victim))
	    {
		send_to_char("Comm can't be set on NPCs.\n\r",ch);
		return;
	    }
	    flag = &victim->comm;
	    flag_table = comm_flags;
	}
	else 
	{
	    send_to_char("That's not an acceptable flag.\n\r",ch);
	    return;
	}
	old = *flag;
	if (type != '=')
	    new = old;
        for (; ;)
        {
	    argument = one_argument(argument,word);
	    if (word[0] == '\0')
		break;
	    pos = flag_lookup(word,flag_table);
	    if (pos == 0)
	    {
		send_to_char("That flag doesn't exist!\n\r",ch);
		return;
	    }
	    else
		SET_BIT(marked,pos);
	}
	for (pos = 0; flag_table[pos].name != NULL; pos++)
	{
	    if (!flag_table[pos].settable && IS_SET(old,flag_table[pos].bit))
	    {
		SET_BIT(new,flag_table[pos].bit);
		continue;
	    }
	    if (IS_SET(marked,flag_table[pos].bit))
		switch(type)
		{
		    case '=':
		    case '+':
			SET_BIT(new,flag_table[pos].bit);
			break;
		    case '-':
			REMOVE_BIT(new,flag_table[pos].bit);
			break;
		    default:
			if (IS_SET(new,flag_table[pos].bit))
			    REMOVE_BIT(new,flag_table[pos].bit);
			else
			    SET_BIT(new,flag_table[pos].bit);
		}
	}
	*flag = new;
    }
}

void do_tattoo( CHAR_DATA *ch, char *argument ){

  char arg[MIL];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  OBJ_INDEX_DATA* tattoo;
  int religion = 0;

  if (IS_NULLSTR(argument)){
    send_to_char( "Syntax: tattoo <char> (religion/none)\n\r",ch);
    return;
  }

  argument = one_argument( argument, arg );

  if ( ( victim = get_char_world( ch, arg ) ) == NULL ){
    send_to_char( "They aren't playing.\n\r", ch );
    return;
  }
  if (IS_NPC(victim)){
    send_to_char( "Mobs don't need tattoos.\n\r",ch);
    return;
  }

  /* get religion we are changing too */
  if (IS_NULLSTR(argument) || get_trust(ch) < 56){
    religion = victim->pcdata->way;
  }
  else if ( (religion = way_lookup( argument )) < 0){
    send_to_char("No such religion.\n\r", ch);
    return;
  }

  if (deity_table[religion].tattoo < 1 ){
    sendf(ch, "%s has no tattoo avaliable.\n\r", deity_table[religion].way );
    return;
  }
  else if ( (tattoo = get_obj_index( deity_table[religion].tattoo)) == NULL){
    sendf(ch, "obj vnum %d does not exist.\n\r", deity_table[religion].tattoo );
    return;
  }

  if (victim->tattoo > 0){
    sendf(ch, "%s is now tattooless.\n\r", PERS2(victim));
    send_to_char("You have lost your tattoo!\n\r",victim);
    victim->tattoo = 0;
    obj = get_eq_char(victim,WEAR_TATTOO);
    if (tattoo != NULL)
      unequip_char(victim, obj);
    extract_obj(obj);
    return;
  }
  obj = create_object( tattoo, tattoo->level );
  sendf(ch, "%s has been tattooed.\n\r", PERS2(victim));
  send_to_char("You have been tattooed.\n\r",victim);
  victim->tattoo = religion;
  obj_to_char(obj, victim);
  equip_char(victim, obj,WEAR_TATTOO);
}

void do_lfind(CHAR_DATA *ch, char *argument)
{
    extern int top_obj_index;
    char buf[MSL];
    BUFFER *buffer;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, nMatch = 0;
    bool found = FALSE;
    buffer=new_buf();
    for(vnum=0; nMatch < top_obj_index; vnum++)
    {
        if ( (pObjIndex=get_obj_index(vnum)) !=NULL)
            nMatch++;
	if (pObjIndex != NULL && CAN_WEAR(pObjIndex, ITEM_UNIQUE))
        {
            found=TRUE;
            sprintf(buf, "[%5d] [%3d of %3d] %s\n\r",pObjIndex->vnum, pObjIndex->count, pObjIndex->cost, pObjIndex->short_descr);
            add_buf(buffer,buf);
        }
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_rfind(CHAR_DATA *ch, char *argument)
{
    extern int top_obj_index;
    char buf[MSL];
    BUFFER *buffer;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, nMatch = 0;
    bool found = FALSE;
    buffer=new_buf();
    sprintf(buf, "There are %d pfiles.\n\r", mud_data.pfiles);
    add_buf(buffer,buf);
    for(vnum=0; nMatch < top_obj_index; vnum++)
    {
        if ( (pObjIndex=get_obj_index(vnum)) !=NULL)
            nMatch++;
	if (pObjIndex != NULL && (CAN_WEAR(pObjIndex, ITEM_RARE) || CAN_WEAR(pObjIndex, ITEM_UNIQUE)))
        {
            found=TRUE;
            sprintf(buf, "[%5d] [%3d of %3d] %s\n\r",
		    pObjIndex->vnum, 
		    pObjIndex->count, 
		    get_maxcount( pObjIndex ),
		    pObjIndex->short_descr);
            add_buf(buffer,buf);
        }
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_profind(CHAR_DATA *ch, char *argument)
{
    extern int top_obj_index;
    char buf[MSL];
    BUFFER *buffer;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, nMatch = 0;
    bool found = FALSE;
    buffer=new_buf();

    add_buf( buffer, "Projectiles:\n\r");
    for(vnum=0; nMatch < top_obj_index; vnum++){
      if ( (pObjIndex=get_obj_index(vnum)) != NULL)
	nMatch++;
      else
	continue;
      if (pObjIndex->item_type == ITEM_PROJECTILE){
	char buf2[MIL];
	found=TRUE;
	sprintf( buf2, "%s", flag_string(projectile_type, pObjIndex->value[0]));

	sprintf(buf, "[%5d] %-25.25s   %-10.10s  %-3d Max %-2d %s %-4dGp %s\n\r",
		pObjIndex->vnum, 
		pObjIndex->short_descr,
		buf2,
		pObjIndex->condition, 
		pObjIndex->value[1] * pObjIndex->value[2],
		IS_SET(pObjIndex->wear_flags, ITEM_RARE) ? "R" : (IS_SET(pObjIndex->wear_flags, ITEM_UNIQUE) ? "U" : " "),
		pObjIndex->cost,
		flag_string(proj_spec_type, pObjIndex->value[4])
		);
	add_buf(buffer,buf);
      }
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}


void do_tick( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    static int pulse_area;
    static int pulse_point;
    pulse_area -= pulse_point;
    if ( --pulse_area     <= 0 )
    {
        pulse_area      = number_range( 5 * PULSE_AREA / 6, 7 * PULSE_AREA / 6 );
	area_update	( );
    }
    pulse_point -= pulse_point;
    if ( --pulse_point    <= 0 )
    {
	wiznet("TICK!",NULL,NULL,WIZ_TICKS,0,0);
        pulse_point     = number_range( 5 * PULSE_TICK / 6 , 7 * PULSE_TICK / 6 );
        reboot_tick_counter--;
        if (reboot_tick_counter>0)
        {
            sprintf( buf, "[`&The Voice of God`7]`8 - World shift in %d hour%s.``", reboot_tick_counter, reboot_tick_counter == 1 ? "" : "s");
            do_echo(NULL,buf);
        }
        else if (!reboot_tick_counter)
            do_reboot(NULL, "0");
	weather_update	( );
	char_update	( );
	obj_update	( );
	trap_update	( );
	cabal_update	( );
    }
    army_update();
    send_to_char("Tick forced.\n\r",ch);
    tail_chain( );
}

void do_wfind(CHAR_DATA *ch, char *argument)
{
    extern int top_obj_index;
    char buf[MSL];
    BUFFER *buffer;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, nMatch = 0;
    bool found = FALSE;
    buffer=new_buf();
    for( vnum=0; nMatch < top_obj_index; vnum++)
    {
        if ( (pObjIndex=get_obj_index(vnum)) !=NULL)
            nMatch++;
        if (pObjIndex != NULL && pObjIndex->item_type == ITEM_CONTAINER && pObjIndex->value[4] != 100)
        {
            found=TRUE;
            sprintf(buf, "[%5d] [%3d] %s\n\r",pObjIndex->vnum, pObjIndex->value[4], pObjIndex->short_descr);
            add_buf(buffer,buf);
        }
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_efind(CHAR_DATA *ch, char *argument)
{
    extern int top_obj_index;
    char buf[MSL];
    BUFFER *buffer;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, number, nMatch = 0;
    bool found;
    buffer=new_buf();
    number = flag_value(extra_flags,argument);
    if (number == NO_FLAG)
    {
	send_to_char("Extra flag not found.\n\r",ch);
	return;
    }
    sprintf(buf, "Extra flag: %s\n\r",extra_bit_name(number));
    add_buf(buffer,buf);
    for(vnum = 0; nMatch < top_obj_index; vnum++)
    {
        if ( (pObjIndex=get_obj_index(vnum)) !=NULL)
            nMatch++;
        if (pObjIndex != NULL && IS_OBJ_STAT(pObjIndex, number))
        {
            found=TRUE;
            sprintf(buf, "[%5d] %s\n\r",pObjIndex->vnum, pObjIndex->short_descr);
            add_buf(buffer,buf);
        }
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_anum(CHAR_DATA *ch, char *argument)
{
    char arg[MIL], buf[MSL], buf2[MIL];
    BUFFER *buffer;
    int low, high, vnum;
    OBJ_INDEX_DATA *pObjIndex;
    MOB_INDEX_DATA *pMobIndex;
    ROOM_INDEX_DATA *pRoomIndex;
    buffer=new_buf();
    argument = one_argument( argument, arg );
    low = ch->in_room->area->min_vnum;
    high = ch->in_room->area->max_vnum;
    if (arg[0] == '\0')
    {
	send_to_char("Syntax:\n\r"\
		     "  anum obj\n\r"\
		     "  anum mob\n\r"\
		     "  anum room\n\r",ch);
	return;
    }
    if (!str_cmp(arg,"obj"))
    {
        for ( vnum = low; vnum <= high; vnum++ )
	    if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
	    {
	        if (CAN_WEAR(pObjIndex, ITEM_UNIQUE))
	            sprintf(buf2, "u");
	        else if (CAN_WEAR(pObjIndex, ITEM_RARE))
	            sprintf(buf2, "r");
	        else
	            sprintf(buf2, " ");
                sprintf( buf, "[%5d] [%1s] [%2d] %s, %s\n\r", pObjIndex->vnum, buf2, pObjIndex->count, pObjIndex->material, pObjIndex->short_descr);
                add_buf( buffer, buf );
	    }
    }
    else if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
    { 
        for ( vnum = low; vnum <= high; vnum++ )
	    if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
	    {
                sprintf( buf, "[%5d] [%2d] [%2d] [%5ld] %s\n\r", pMobIndex->vnum, pMobIndex->level, pMobIndex->count, pMobIndex->gold, pMobIndex->short_descr );
                add_buf( buffer, buf );
	    }

    }
    else if (!str_cmp(arg,"room"))
    {
        for ( vnum = low; vnum <= high; vnum++ )
	    if ( ( pRoomIndex = get_room_index( vnum ) ) != NULL )
	    {
                sprintf( buf, "[%5d] %s\n\r", pRoomIndex->vnum, pRoomIndex->name );
                add_buf( buffer, buf );
	    }

    }
    else
    {
	sprintf(buf,"obj, mob, room\n\r");
	add_buf( buffer, buf);
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_wefind(CHAR_DATA *ch, char *argument)
{
    extern int top_obj_index;
    char buf[MSL], buf2[MSL], arg1[MIL], arg2[MIL];
    BUFFER *buffer;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, number, weight, nMatch = 0, include = 0, exclude = 0;
    buffer=new_buf();
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if ( ( number = flag_value( wear_flags, arg1 ) ) == NO_FLAG )
      {
	send_to_char( "Wear flag not found.\n\r", ch);
	return;
      }
    if (arg2[0] == '\0')
	weight = 99999;
    else
        weight = atoi(arg2);
    sprintf(buf, "Wear: %s  Weight: %d\n\r",arg1, weight);
    add_buf(buffer,buf);
    for(vnum = 0; nMatch < top_obj_index; vnum++)
    {
        if ( (pObjIndex=get_obj_index(vnum)) !=NULL)
	{
            nMatch++;
            if ( IS_SET(pObjIndex->wear_flags, number) && 
		 pObjIndex->weight / 10 <= weight)
	    {
		include++;
                if (CAN_WEAR(pObjIndex, ITEM_UNIQUE))
                    sprintf(buf2, "u");
                else if (CAN_WEAR(pObjIndex, ITEM_RARE))   
                    sprintf(buf2, "r");
                else
                    sprintf(buf2, " ");
                sprintf(buf, "+[%5d] [%1s] [%2d] %s\n\r",pObjIndex->vnum, buf2, pObjIndex->weight / 10, pObjIndex->short_descr);
                add_buf(buffer,buf);
            }
	}
    }
    for(vnum = 0, nMatch = 0; nMatch < top_obj_index; vnum++)
    {
        if ( (pObjIndex=get_obj_index(vnum)) !=NULL)
	{
            nMatch++;
            if ( IS_SET(pObjIndex->wear_flags, number) && 
		 pObjIndex->weight / 10 > weight)
	    {
		exclude++;
                if (CAN_WEAR(pObjIndex, ITEM_UNIQUE))
                    sprintf(buf2, "u");
                else if (CAN_WEAR(pObjIndex, ITEM_RARE))   
                    sprintf(buf2, "r");
                else
                    sprintf(buf2, " ");
                sprintf(buf, "-[%5d] [%1s] [%2d] %s\n\r",pObjIndex->vnum, buf2, pObjIndex->weight / 10, pObjIndex->short_descr);
                add_buf(buffer,buf);
            }
	}
    }
    sprintf(buf, "%d included and %d excluded.\n\r", include, exclude);
    add_buf(buffer,buf);
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_sfind(CHAR_DATA *ch, char *argument)
{
    extern int top_obj_index;
    char buf[MSL];
    BUFFER *buffer;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, number, nMatch = 0;
    AFFECT_DATA *paf;
    buffer=new_buf();
    number = atoi(argument);
    sprintf(buf, "Affect: %s\n\r",affect_loc_name(number));
    add_buf(buffer,buf);
    for(vnum = 0; nMatch < top_obj_index; vnum++)
    {
        if ( (pObjIndex=get_obj_index(vnum)) !=NULL)
	{
            nMatch++;
            for ( paf = pObjIndex->affected; paf != NULL; paf = paf->next )
                if ( paf->location == number && paf->modifier != 0 )
	        {
                    sprintf(buf, "[%5d] %s\n\r",pObjIndex->vnum, pObjIndex->short_descr);
                    add_buf(buffer,buf);
                }
	}
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_ssfind(CHAR_DATA *ch, char *argument)
{
    extern int top_obj_index;
    char buf[MSL], buf2[MSL];
    BUFFER *buffer;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, nMatch = 0, average;
    AFFECT_DATA *paf;
    OBJ_SPELL_DATA *os;
    buffer=new_buf();
    for(vnum = 0; nMatch < top_obj_index; vnum++)
    {
        if ( (pObjIndex=get_obj_index(vnum)) !=NULL)
	{
            nMatch++;
	    if (CAN_WEAR(pObjIndex, ITEM_UNIQUE))
		sprintf(buf2, "u");
	    else if (CAN_WEAR(pObjIndex, ITEM_RARE))
		sprintf(buf2, "r");
	    else
		sprintf(buf2, " ");
            for ( paf = pObjIndex->affected; paf != NULL; paf = paf->next )
	    {
	        if (paf->bitvector)
	        {
	            sprintf(buf, "[%5d] [%1s] %s ",pObjIndex->vnum, buf2, pObjIndex->short_descr);
        	    add_buf(buffer,buf);
	            switch(paf->where)
        	    {
	            case TO_AFFECTS: sprintf(buf,"Adds %s affect.\n\r", affect_bit_name(paf->bitvector)); break;
        	    case TO_OBJECT:  sprintf(buf,"Adds %s object flag.\n\r", extra_bit_name(paf->bitvector)); break;
	            case TO_IMMUNE:  sprintf(buf,"Adds immunity to %s.\n\r", imm_bit_name(paf->bitvector)); break;
        	    case TO_RESIST:  sprintf(buf,"Adds resistance to %s.\n\r", imm_bit_name(paf->bitvector)); break;
	            case TO_VULN:    sprintf(buf,"Adds vulnerability to %s.\n\r", imm_bit_name(paf->bitvector)); break;
        	    default:         sprintf(buf,"Unknown bit %d: %d\n\r", paf->where,paf->bitvector); break;
	            }
		    add_buf(buffer,buf);
		}
	    }
	    for (os = pObjIndex->spell; os; os = os->next )
		if (os->target == 1 || os->target == 2)
		{
		    sprintf(buf, "[%5d] [%1s] %s ",pObjIndex->vnum, buf2, pObjIndex->short_descr);
		    add_buf(buffer,buf);
		    sprintf(buf,"Casts %s.\n\r",skill_table[os->spell].name);
		    add_buf(buffer,buf);
		}
	    if (CAN_WEAR(pObjIndex, ITEM_PARRY))
	    {
		sprintf(buf, "[%5d] [%1s] %s Can be dual-parried.\n\r",pObjIndex->vnum, buf2, pObjIndex->short_descr);
		add_buf(buffer,buf);
	    }
	    if (pObjIndex->item_type == ITEM_WEAPON)
	    {
		average = (1+pObjIndex->value[2])*pObjIndex->value[1]/2;
		if (average >= 20)
		{
		    sprintf(buf, "[%5d] [%1s] %s Average: %d.\n\r",pObjIndex->vnum, buf2, pObjIndex->short_descr,average);
		    add_buf(buffer,buf);
		}
	    }
        }
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_crashus(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim = NULL;
    if (IS_NPC(ch)) 
        return;
    send_to_char("Here we go!\n\r",ch);
    send_to_char("Here we go!\n\r",victim);
}

void do_freezeus(CHAR_DATA *ch, char *argument)
{
    int counter, nothing;
    for ( counter = 1; counter > 0; counter++);
	nothing = counter;
}

void do_interest( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL], arg3[MIL], arg4[MIL];
    int years, lump, rate, premium, counter = 0;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' || arg4[0] == '\0'
    || !is_number( arg1 ) || !is_number( arg2 ) || !is_number( arg3 ) || !is_number( arg4 ) )
    {
	send_to_char( "Syntax: interest <lump sum> <premium> <rate> <years>.\n\r", ch );
	return;
    }
    lump = atoi( arg1);
    premium = atoi( arg2 );
    rate = atoi( arg3 );
    for ( years = atoi( arg4 ) ; years > 0; years-- )
    {
	counter ++;
	lump = (rate + 100) * (lump + premium) / 100;
	sendf(ch,"%d:  %d.\n\r", counter, lump);
    }
    sendf(ch,"TOTAL: %d.\n\r", lump);
}

void do_skfind( CHAR_DATA *ch, char *argument )
{
    extern int top_mob_index;
    char buf[MSL];
    BUFFER *buffer;
    MOB_INDEX_DATA *pMobIndex;
    int vnum, nMatch = 0;
    buffer=new_buf();
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
	if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( pMobIndex->pShop != NULL )
	    {
                sprintf( buf, "[%5d] [%5d] [%2d %2d] %s", pMobIndex->vnum, pMobIndex->group,
		  pMobIndex->pShop->open_hour, pMobIndex->pShop->close_hour,  pMobIndex->short_descr );
                add_buf( buffer, buf );
		if ( pMobIndex->spec_fun )
    		{
        	    sprintf( buf, " [%s]",  spec_name( pMobIndex->spec_fun ) );
                    add_buf( buffer, buf );
    		}
		sprintf( buf, "\n\r");
		add_buf( buffer, buf );
	    }
	}
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_gofind( CHAR_DATA *ch, char *argument )
{
    extern int top_mob_index, top_obj_index;
    char buf[MSL];
    BUFFER *buffer;
    MOB_INDEX_DATA *pMobIndex;
    OBJ_INDEX_DATA *pObjIndex;
    int vnum, nMatch = 0;
    buffer=new_buf();
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
	if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ((pMobIndex->level < 10 && pMobIndex->gold > pMobIndex->level * 10)
	    || (pMobIndex->level >= 10 && pMobIndex->gold > pMobIndex->level * pMobIndex->level ))
	    {
                sprintf( buf, "[%5d] [%5ld] %s\n\r", pMobIndex->vnum, pMobIndex->gold, pMobIndex->short_descr);
                add_buf( buffer, buf );
	    }
	}
    for(vnum = 0; nMatch < top_obj_index; vnum++)
    {
        if ( (pObjIndex=get_obj_index(vnum)) !=NULL)
        {
            nMatch++;
            if ( pObjIndex->item_type == ITEM_MONEY )
            {
                sprintf(buf, "[%5d] [%5d] %s\n\r",pObjIndex->vnum, pObjIndex->value[0], pObjIndex->short_descr);
                add_buf(buffer,buf);
            }
        }
    }   
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_imfind( CHAR_DATA *ch, char *argument )
{
    extern int top_mob_index;
    char buf[MSL];
    BUFFER *buffer;
    MOB_INDEX_DATA *pMobIndex;
    int vnum, nMatch = 0;
    buffer=new_buf();
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
	if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
	{
	    nMatch++;
	    if ( pMobIndex->imm_flags != 0 && pMobIndex->level < 35)
	    {
                sprintf( buf, "[%5d] %s - %s\n\r", pMobIndex->vnum, pMobIndex->short_descr, imm_bit_name(pMobIndex->imm_flags));
                add_buf( buffer, buf );
	    }
	}
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_affstrip( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], arg2[MIL];
    CHAR_DATA *victim;
    int sn;
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    if (arg[0] == '\0' || arg2[0] == '\0')
    {
	send_to_char("Syntax: affstrip <char> <affect>.\n\r",ch);
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( ( sn = skill_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such skill or spell.\n\r", ch );
	return;
    }
    affect_strip(victim,sn);
    update_pos(victim);        
    if (IS_GEN(sn))
      sendf(ch,"You strip the %s affect from %s.\n\r", effect_table[GN_POS(sn)].name, PERS2(victim));
    else
      sendf(ch,"You strip the %s affect from %s.\n\r", skill_table[sn].name, PERS2(victim));

}

void do_songstrip( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], arg2[MIL];
    CHAR_DATA *victim;
    int sn;
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    if (arg[0] == '\0' || arg2[0] == '\0')
    {
	send_to_char("Syntax: songstrip <char> <affect>.\n\r",ch);
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( ( sn = song_lookup( arg2 ) ) < 0 )
    {
	send_to_char( "No such song.\n\r", ch );
	return;
    }
    song_strip(victim,sn);
    update_pos(victim);        
    sendf(ch,"You strip the %s song from %s.\n\r", song_table[sn].name, PERS2(victim));
}

void do_in_range( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], arg2[MIL];
    CHAR_DATA *victim, *vch;
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    if (arg[0] == '\0' || arg2[0] == '\0')
    {
	send_to_char("Syntax: inrange <character1> <character2>.\n\r",ch);
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( ( vch = get_char_world( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (is_pk(victim,vch))
    	sendf(ch,"YES, %s and %s are in each others pk range.\n\r",victim->name,vch->name);
    else
    	sendf(ch,"NO, %s and %s are NOT in each others pk range.\n\r",victim->name,vch->name);
}

void do_doas (CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    DESCRIPTOR_DATA *orig;
    CHAR_DATA *victim;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Doas whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( argument[0] == '\0' )
    {
	send_to_char( "Do what?\n\r", ch );
	return;
    }
    if (!str_prefix("quit", argument) || !str_prefix("delete",argument))
    {
        send_to_char ("Are you trying to crash the MUD or something?\n\r",ch);
        return;
    }
    if (victim->level >= ch->level)
    {
	send_to_char ("I don't think so.\n\r",ch);
	return;
    }
    orig = victim->desc;
    victim->desc = ch->desc;
    interpret (victim, argument);
    victim->desc = orig;
}

void do_quest_race(CHAR_DATA* ch, char* argument){
  char arg[MIL], buf[MIL];

  CHAR_DATA *victim;
  argument = one_argument( argument, arg );
  if ( arg[0] == '\0' ){
    send_to_char( "Whom and to what race?\n\r", ch );
    return;
  }
  if ( ( victim = get_char_world( ch, arg ) ) == NULL ){
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if (IS_NPC(victim)){
    send_to_char("Not on mere mobs.\n\r", ch);
    return;
  }
  if (IS_NULLSTR(argument)){
    do_quest_race(ch, "");
    return;
  }

  if (!str_prefix(argument, "none")){
    free_string(victim->pcdata->race);
    victim->pcdata->race = str_dup("");
    send_to_char("QuestRace has been cleared\n\r", ch);
    sendf(victim, "Your race is once again %s.\n\r",
	  race_table[ch->race].name);
    return;
  }
  /* 5 char only */
  strcpy(buf, argument);
  buf[5] = '\0';
  free_string(victim->pcdata->race);
  victim->pcdata->race = str_dup(buf);
  sendf(ch, "Race set to %s\n\r", victim->pcdata->race);
  sendf(victim, "Your race is now: %s\n\r", victim->pcdata->race);
}

/* approved a character with a quest to become questrace/class later on */
/* used in approval functions */
bool quest_approve(CHAR_DATA* ch, char* name, int quest){
  char arg[MIL];
  CHAR_DATA* victim;
  DESCRIPTOR_DATA* d;
  bool fRemote = FALSE;
  
  if ( (victim = char_file_active (name)) == NULL){
    if ( (d = bring( ch->in_room, name )) != NULL){
      victim = d->character;
      fRemote = TRUE;
      victim->pcdata->roomnum *= -1;
    }
    else{
      send_to_char ("That character does not exist.\n\r", ch);
      return FALSE;
    }
  }

  switch( quest ){
  case QUEST_CRUS:
    sprintf(arg, "%s to seek out the Cathedral of the Order of Crusades.", name);
    break;
  case QUEST_PSI:
    sprintf(arg, "%s to seek out the Tower of Minds Eye.", name);
    break;
  case QUEST_UNDE:
    sprintf(arg, "%s to seek out the shrine of Deaths Scourge.", name);
    break;
  case QUEST_VAMP:
    sprintf(arg, "%s to seek out the shrine of Cycle of Death.", name);
    break;
  default:
    send_to_char("Error no quest for this approval.\n\r", ch );
    return FALSE;
  }
  send_to_char("`&You've been charged with a quest!``\n\r", victim);
  do_permquest(ch, arg);
  
/* check if this was remote and hence we need to purge */
  if (fRemote){
    SET_BIT(victim->pcdata->messages, MSG_QUEST);
    save_char_obj( victim );
    purge( victim );
  }
  return TRUE;
}

void do_quest_crus(CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA* room;
  int sn;
  const int crusader = class_lookup("crusader");
  int last_psalm = 0;
  int psalms[MAX_PSALM];
  char arg[MIL];
  char buf[MIL];
  
  CHAR_DATA *victim;
  argument = one_argument( argument, arg );
  if ( arg[0] == '\0' ){
    send_to_char( "Whom?\n\r", ch );
    return;
  }
  if ( ( victim = get_char_world( ch, arg ) ) == NULL ){
      send_to_char( "They aren't here.\n\r", ch );
      return;
  }
  if (IS_NPC(victim)){
    send_to_char("Not on mere mobs.\n\r", ch);
    return;
  }
  if (victim->class == class_lookup("crusader")){
    send_to_char("That person is already a Crusader.\n\r",ch);
    return;
  }
  
/* reset skills and setup new ones */
  while(victim->pcdata->newskills)
    nskill_remove(victim, victim->pcdata->newskills);

/* reset groups etc */
  for (sn = 0; sn < MAX_GROUP; sn ++ )
    victim->pcdata->ss_picked[sn] = 0;

/* reset skills */
  for ( sn = 0; sn < MAX_SKILL; sn++ ){
    victim->pcdata->learned[sn] = UMIN(victim->pcdata->learned[sn], 100);
    if ( skill_table[sn].name != NULL && skill_table[sn].skill_level[0] != 69){
/* the rating of 11 is special for crusaders to inherit a skill */
      if (skill_table[sn].rating[crusader] == 11)
	continue;
      else if (skill_table[sn].rating[crusader] > 9)
	victim->pcdata->learned[sn] = 0;
      else if (get_skill(victim,sn) == 0)
	victim->pcdata->learned[sn] = 1;
    }
  }

  set_race_skills(victim, 1);

  /* SET CLASS */
  victim->class = crusader;
  
  /* PSALM PEN. RESET */
  victim->pcdata->psalm_pen = 0;
  
  /* NUKE ALL THE GRANTED SKILLS */
  while(victim->pcdata->newskills)
    nskill_remove(victim, victim->pcdata->newskills);
  

  
  sprintf(buf, "%s has changed %s's class to %s.", IS_NPC(ch) ? PERS2(ch) : ch->name, victim->name, class_table[victim->class].name);
  log_event(ch, buf);
  do_hal("immortal", buf, buf, NOTE_PENALTY);
  send_to_char("Ok.\n\r",ch);
  
  send_to_char("You have been accepted into the Order of Crusades.\n\r",victim);
  act("$n has been accepted into the Order of Crusades.",victim,NULL,NULL,TO_ROOM);
  
/* REMORT EFFECTS OF CLASS CHANGE */
  if (victim->level > 50)
    return;
  
  /* STRIP THEM NAKED */
  while(victim->carrying){
    OBJ_DATA* obj = victim->carrying;
    obj_from_char( obj );
    extract_obj( obj );
  }
  
  /* LOWER MAX STATS */    
  for (sn = 0; sn < MAX_STATS; sn++)
    victim->perm_stat[sn] = UMAX(1, victim->perm_stat[sn] - 1);
  
  /* lower one more stat by one */
  sn = number_range(0, STAT_CON);
  victim->perm_stat[sn] = UMAX(1, victim->perm_stat[sn] - 1);
  
  /* raise condition if warranted */
  if (victim->pcdata->dall > 10){
    send_to_char("With wonder you see some of the scars dissapear from you body.\n\r", victim);
    victim->pcdata->dall -= 10;
  }
  /* reset pkils */
  victim->pcdata->kpc = 0;

  /* RESET  PSALMS */
  for (sn = 0; sn < MAX_PSALM; sn++)
    victim->pcdata->psalms[sn] = 0;
  
  /* GET A RANDOM PSALM LIST */
  for (sn = 0; psalm_table[sn].name != NULL && sn < MAX_PSALM; sn++){
    if (psalm_table[sn].fRand)
      psalms[last_psalm++] = sn;
  }
  /* ADD ONE RANDOM PSALM REGARDLESS*/
  if (last_psalm > 0)
    victim->pcdata->psalms[psalms[number_range(0, last_psalm - 1)]] = 1;
  
  /* re approval of desc */
  SET_BIT(victim->act2, PLR_DESC);
  
  /* RESET STARTING STATS */
  if (victim->race == race_lookup("illithid"))
    victim->dam_type = 28;
  else
    victim->dam_type = 17;
  victim->pcdata->perm_hit = pc_race_table[victim->race].hit;
  victim->pcdata->perm_mana= pc_race_table[victim->race].mana;
  victim->pcdata->perm_move = 100;
  victim->max_hit = victim->pcdata->perm_hit;
  victim->max_mana= victim->pcdata->perm_mana;
  victim->max_move= victim->pcdata->perm_move;
  victim->level   = 1;
  victim->exp     = exp_per_level(victim, victim->level);
  victim->max_exp = victim->exp;
  victim->hit     = victim->max_hit;
  victim->mana    = victim->max_mana;
  victim->move    = victim->max_move;
  victim->train   = 3;
  victim->practice= pc_race_table[victim->race].practice;
  sprintf( buf, "the %s", title_table [victim->class] [victim->level] [victim->sex == SEX_FEMALE ? 1 : 0] );
  set_title( victim, buf );
  if ( (room = get_room_index( ROOM_VNUM_CRUS_PORTAL )) != NULL){
    char_from_room( victim );
    char_to_room( victim, room);
  }
  do_outfit(victim,"");
  act("You lose some of your abilities as you begin your guild life anew.", victim, NULL, NULL, TO_CHAR);
  save_char_obj( victim );
}

void do_quest_psi(CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA* room;
  int sn;
  const int psi = class_lookup("psionicist");
  char arg[MIL];
  char buf[MIL];
  
  CHAR_DATA *victim;
  argument = one_argument( argument, arg );
  if ( arg[0] == '\0' ){
    send_to_char( "Whom?\n\r", ch );
    return;
  }
  if ( ( victim = get_char_world( ch, arg ) ) == NULL ){
      send_to_char( "They aren't here.\n\r", ch );
      return;
  }
  if (IS_NPC(victim)){
    send_to_char("Not on mere mobs.\n\r", ch);
    return;
  }
  if (victim->class == psi){
    send_to_char("That person is already a Psionicst.\n\r",ch);
    return;
  }
  
/* reset skills and setup new ones */
  while(victim->pcdata->newskills)
    nskill_remove(victim, victim->pcdata->newskills);

/* reset groups etc */
  for (sn = 0; sn < MAX_GROUP; sn ++ )
    victim->pcdata->ss_picked[sn] = 0;

/* reset skills */
  for ( sn = 0; sn < MAX_SKILL; sn++ ){
    victim->pcdata->learned[sn] = UMIN(victim->pcdata->learned[sn], 100);
    if ( skill_table[sn].name != NULL 
	 && skill_table[sn].rating[psi] > 0
	 && skill_table[sn].skill_level[0] != 69){
      if (skill_table[sn].rating[psi] > 9)
	victim->pcdata->learned[sn] = 0;
      else if (get_skill(victim,sn) == 0)
	victim->pcdata->learned[sn] = 1;
    }
  }
  set_race_skills(victim, 1);

  /* SET CLASS */
  victim->class = psi;
  
  /* NUKE ALL THE GRANTED SKILLS */
  while(victim->pcdata->newskills)
    nskill_remove(victim, victim->pcdata->newskills);
  

  
  sprintf(buf, "%s has changed %s's class to %s.", IS_NPC(ch) ? PERS2(ch) : ch->name, victim->name, class_table[victim->class].name);
  log_event(ch, buf);
  do_hal("immortal", buf, buf, NOTE_PENALTY);
  send_to_char("Ok.\n\r",ch);
  
  send_to_char("You have been accepted into the Tower of Mind's Eye.\n\r",victim);
  act("$n has been accepted into the Tower of Mind's Eye.",victim,NULL,NULL,TO_ROOM);
  
/* REMORT EFFECTS OF CLASS CHANGE */
  if (victim->level > 50)
    return;
  
  /* STRIP THEM NAKED */
  while(victim->carrying){
    OBJ_DATA* obj = victim->carrying;
    obj_from_char( obj );
    extract_obj( obj );
  }
  
  /* LOWER MAX STATS */    
  for (sn = 0; sn < MAX_STATS; sn++)
    victim->perm_stat[sn] = UMAX(1, victim->perm_stat[sn] - 1);
  
  /* lower one more stat by one */
  sn = number_range(0, STAT_CON);
  victim->perm_stat[sn] = UMAX(1, victim->perm_stat[sn] - 1);
  
  /* raise condition if warranted */
  if (victim->pcdata->dall > 10){
    send_to_char("With wonder you see some of the scars dissapear from you body.\n\r", victim);
    victim->pcdata->dall -= 10;
  }
  /* reset pkils */
  victim->pcdata->kpc = 0;

  /* re approval of desc */
  SET_BIT(victim->act2, PLR_DESC);
  
 /* DND SPELLS */
  SET_BIT(victim->game, GAME_DNDS);

  /* RESET STARTING STATS */
  if (victim->race == race_lookup("illithid"))
    victim->dam_type = 28;
  else
    victim->dam_type = 17;
  victim->pcdata->perm_hit = pc_race_table[victim->race].hit;
  victim->pcdata->perm_mana= pc_race_table[victim->race].mana;
  victim->pcdata->perm_move = 100;
  victim->max_hit = victim->pcdata->perm_hit;
  victim->max_mana= victim->pcdata->perm_mana;
  victim->max_move= victim->pcdata->perm_move;
  victim->level   = 1;
  victim->exp     = exp_per_level(victim, victim->level);
  victim->max_exp = victim->exp;
  victim->hit     = victim->max_hit;
  victim->mana    = victim->max_mana;
  victim->move    = victim->max_move;
  victim->train   = 3;
  victim->practice= pc_race_table[victim->race].practice;
  sprintf( buf, "the %s", title_table [victim->class] [victim->level] [victim->sex == SEX_FEMALE ? 1 : 0] );
  set_title( victim, buf );
  if ( (room = get_room_index( ROOM_VNUM_PSI_PORTAL )) != NULL){
    char_from_room( victim );
    char_to_room( victim, room);
  }
  do_outfit(victim,"");
  act("You lose some of your abilities as you begin your guild life anew.", victim, NULL, NULL, TO_CHAR);
  save_char_obj( victim );
}

//allows an adventurer to join a guild
void do_guild(CHAR_DATA *ch, char *argument ){
  CHAR_DATA* victim = ch;
  int sn;
  int c = 0, i = 0, choice = 0, new_class = -1;
  char buf[MIL];
  
  if (ch->class != gcn_adventurer){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  else if ( IS_NULLSTR(argument)){
    char out[MSL];
    out[0] = '\0';
    sprintf( out, "You may choose from following professions:\n\r\n\r");

    for (i = 0; i < MAX_CLASS; i++){
      if (IS_NULLSTR(pc_race_table[ch->race].class_choice[i]))
	continue;
      else if (ch->class == i)
	continue;
      sprintf( buf, "%2d. %-15.15s %3d Exp    ", 
	       c + 1, 
	       pc_race_table[ch->race].class_choice[i],
	       class_table[i].extra_exp + pc_race_table[ch->race].class_mult - 1500);
      strcat( out, buf );
      if (c++ % 2 == 1){
	strcat( out, "\n\r");
      }
    }
    if (c % 2 == 1)
      strcat( out, "\n\r");
    strcat( out, "\n\r" );
    send_to_char(out, ch );

    return;
  }
  else if ( (choice = atoi( argument)) < 1){
    send_to_char("Invalid choice.\n\r", ch);
    return;
  }
  new_class = -1;
  /* set the class */
  for (i = 0; i < MAX_CLASS; i++){
    if (IS_NULLSTR(pc_race_table[ch->race].class_choice[i]))
      continue;
    else if (ch->class == i)
      continue;
    else
      c++;
    if (c == choice){
      new_class = class_lookup( pc_race_table[ch->race].class_choice[i] );
      break;
    }
  }
  if (new_class < 0){
    send_to_char("Invalid choice.\n\r", ch);
    return;
  }

/* reset skills and setup new ones */
  while(victim->pcdata->newskills)
    nskill_remove(victim, victim->pcdata->newskills);

/* reset groups etc */
  for (sn = 0; sn < MAX_GROUP; sn ++ )
    victim->pcdata->ss_picked[sn] = 0;

/* reset skills */
  for ( sn = 0; sn < MAX_SKILL; sn++ ){
    victim->pcdata->learned[sn] = UMIN(victim->pcdata->learned[sn], 100);
    if ( skill_table[sn].name != NULL 
	 && skill_table[sn].rating[new_class] > 0
	 && skill_table[sn].skill_level[0] != 69){
      if (skill_table[sn].rating[new_class] > 9)
	victim->pcdata->learned[sn] = 0;
      else if (get_skill(victim,sn) == 0)
	victim->pcdata->learned[sn] = 1;
    }
  }
  
  /* SET CLASS */
  victim->class = new_class;

  //Update race skills
  set_race_skills(victim, 1);

  sendf(ch, "You have been accepted into the %s guild.\n\r", class_table[ch->class].name);
  act("$n has been accepted into %s guild.",victim, class_table[ch->class].name, NULL,TO_ROOM);
  
/* REMORT EFFECTS OF CLASS CHANGE */
  if (victim->level > 50)
    return;
  
  /* raise condition if warranted */
  if (victim->pcdata->dall > 10){
    send_to_char("With wonder you see some of the scars dissapear from you body.\n\r", victim);
    victim->pcdata->dall -= 10;
  }
  /* reset pkils */
  victim->pcdata->kpc = 0;

  /* re approval of desc */
  SET_BIT(victim->act2, PLR_DESC);
  
  /* RESET STARTING STATS */
  if (victim->race == race_lookup("illithid"))
    victim->dam_type = 28;
  else
    victim->dam_type = 17;

  victim->pcdata->perm_hit = pc_race_table[victim->race].hit;
  victim->pcdata->perm_mana= pc_race_table[victim->race].mana;
  victim->pcdata->perm_move = 100;

  victim->max_hit = victim->pcdata->perm_hit;
  victim->max_mana= victim->pcdata->perm_mana;
  victim->max_move= victim->pcdata->perm_move;

  victim->train   -= victim->level / 5;
  victim->level   = 1;
  victim->exp     = exp_per_level(victim, victim->level);
  victim->max_exp = victim->exp;
  victim->hit     = victim->max_hit;
  victim->mana    = victim->max_mana;
  victim->move    = victim->max_move;
  victim->practice= pc_race_table[victim->race].practice;
  sprintf( buf, "the %s", title_table [victim->class] [victim->level] [victim->sex == SEX_FEMALE ? 1 : 0] );
  set_title( victim, buf );

  do_outfit(victim,"");
  act("You lose some of your abilities as you begin your guild life anew.", victim, NULL, NULL, TO_CHAR);
  save_char_obj( victim );
}

void do_quest_king(CHAR_DATA *ch, char *argument ){
  OBJ_DATA* crown, *obj;
  char arg[MIL];
  
  CHAR_DATA *victim;
  argument = one_argument( argument, arg );

  if ( arg[0] == '\0' ){
    send_to_char( "Whom?\n\r", ch );
    return;
  }
  if ( ( victim = get_char_world( ch, arg ) ) == NULL ){
      send_to_char( "They aren't here.\n\r", ch );
      return;
  }
  if (IS_NPC(victim)){
    send_to_char("Not on mere mobs.\n\r", ch);
    return;
  }
  else if (victim->pCabal == NULL){
    send_to_char("They're not in a cabal!\n\r", ch);
    return;
  }

  /* set the title */
  free_string(victim->pcdata->title);
  victim->pcdata->title = str_dup(" the King of Aabahran");

  if ( (crown = create_object( get_obj_index( OBJ_VNUM_CROWN), 50 )) != NULL){
    /* load the crown onto their head */
    if ( (obj = get_eq_char( victim, WEAR_HEAD)) != NULL){
      obj_from_char( obj );
      extract_obj( obj );
    }
    obj_to_char( crown, victim );
    equip_char( victim, crown, WEAR_HEAD );
  }
  send_to_char("Your scars disappear and your body is made whole once again.\n\r", victim );
  victim->pcdata->dall = 0;
  
  send_to_char("You have been crowned as the King of Aabahran!\n\r", victim);
  act("$n has been crowned as the King of Aabahran.", victim, NULL, NULL, TO_ROOM );
  SET_BIT(victim->act2, PLR_ROYAL );
}

void do_quest_vamp(CHAR_DATA *ch, char *argument )
{
    int sn;
    char arg[MIL];
    char buf[MIL];

    CHAR_DATA *victim;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (IS_NPC(victim))
      {
	send_to_char("Not on mere mobs.\n\r", ch);
	return;
      }
    if (victim->class == class_lookup("vampire"))
    {
	send_to_char("That person is already a vampire.\n\r",ch);
	return;
    }
    victim->class = class_lookup("vampire");
    victim->race = race_lookup("vampire");

    victim->mod_stat[STAT_STR] = UMIN(get_max_train(victim, STAT_STR), victim->mod_stat[STAT_STR]);
    victim->mod_stat[STAT_INT] = UMIN(get_max_train(victim, STAT_INT), victim->mod_stat[STAT_INT]);
    victim->mod_stat[STAT_WIS] = UMIN(get_max_train(victim, STAT_WIS), victim->mod_stat[STAT_WIS]);
    victim->mod_stat[STAT_DEX] = UMIN(get_max_train(victim, STAT_DEX), victim->mod_stat[STAT_DEX]); 
    victim->mod_stat[STAT_CON] = UMIN(get_max_train(victim, STAT_CON), victim->mod_stat[STAT_CON]);

    victim->perm_stat[STAT_STR] = UMIN(get_max_train(victim, STAT_STR), victim->perm_stat[STAT_STR]);
    victim->perm_stat[STAT_INT] = UMIN(get_max_train(victim, STAT_INT), victim->perm_stat[STAT_INT]);
    victim->perm_stat[STAT_WIS] = UMIN(get_max_train(victim, STAT_WIS), victim->perm_stat[STAT_WIS]);
    victim->perm_stat[STAT_DEX] = UMIN(get_max_train(victim, STAT_DEX), victim->perm_stat[STAT_DEX]); 
    victim->perm_stat[STAT_CON] = UMIN(get_max_train(victim, STAT_CON), victim->perm_stat[STAT_CON]);


    victim->practice += 6;
    SET_BIT(victim->act2, PLR_MASQUERADE);
	
    victim->affected_by = victim->affected_by|race_table[victim->race].aff;
    victim->imm_flags   = victim->imm_flags|race_table[victim->race].imm;
    victim->res_flags   = victim->res_flags|race_table[victim->race].res;
    victim->vuln_flags  = victim->vuln_flags|race_table[victim->race].vuln;
    victim->form        = race_table[victim->race].form;
    victim->parts       = race_table[victim->race].parts;
    victim->size = pc_race_table[victim->race].size;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
        if ( skill_table[sn].name != NULL && skill_table[sn].rating[victim->class] < 10 && skill_table[sn].skill_level[0] != 69 && get_skill(victim,sn) == 0)
            victim->pcdata->learned[sn] = 1;
    SET_BIT(victim->act,PLR_UNDEAD);

    sprintf(buf, "%s has changed %s's class to %s.", IS_NPC(ch) ? PERS2(ch) : ch->name, victim->name, class_table[victim->class].name);
    log_event(ch, buf);
    do_hal("immortal", buf, buf, NOTE_PENALTY);

    send_to_char("Ok.\n\r",ch);
    send_to_char("The darkness embraces you, as you shed your mortal shell and join the undead.\n\r",victim);
    act("Darkness consumes $n as $e joins the ranks of the undead.",victim,NULL,NULL,TO_ROOM);
}


void do_quest_undead(CHAR_DATA *ch, char *argument )
{

    char arg[MIL];
    char buf[MIL];

    CHAR_DATA *victim;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (IS_NPC(victim))
      {
	send_to_char("Not on mere mobs.\n\r", ch);
	return;
      }
    if (victim->race == race_lookup("undead"))
    {
	send_to_char("That person is already undead.\n\r",ch);
	return;
    }

    if (victim->race == race_lookup("vampire"))
    {
	send_to_char("That person is already a vampire.\n\r",ch);
	return;
    }

    if (victim->race == race_lookup("lich"))
    {
	send_to_char("That person is already a lich.\n\r",ch);
	return;
    }

    victim->race = race_lookup("undead");

    victim->mod_stat[STAT_STR] = UMIN(get_max_train(victim, STAT_STR), victim->mod_stat[STAT_STR]);
    victim->mod_stat[STAT_INT] = UMIN(get_max_train(victim, STAT_INT), victim->mod_stat[STAT_INT]);
    victim->mod_stat[STAT_WIS] = UMIN(get_max_train(victim, STAT_WIS), victim->mod_stat[STAT_WIS]);
    victim->mod_stat[STAT_DEX] = UMIN(get_max_train(victim, STAT_DEX), victim->mod_stat[STAT_DEX]); 
    victim->mod_stat[STAT_CON] = UMIN(get_max_train(victim, STAT_CON), victim->mod_stat[STAT_CON]);

    victim->perm_stat[STAT_STR] = UMIN(get_max_train(victim, STAT_STR), victim->perm_stat[STAT_STR]);
    victim->perm_stat[STAT_INT] = UMIN(get_max_train(victim, STAT_INT), victim->perm_stat[STAT_INT]);
    victim->perm_stat[STAT_WIS] = UMIN(get_max_train(victim, STAT_WIS), victim->perm_stat[STAT_WIS]);
    victim->perm_stat[STAT_DEX] = UMIN(get_max_train(victim, STAT_DEX), victim->perm_stat[STAT_DEX]); 
    victim->perm_stat[STAT_CON] = UMIN(get_max_train(victim, STAT_CON), victim->perm_stat[STAT_CON]);


	
    victim->affected_by = victim->affected_by|race_table[victim->race].aff;
    victim->imm_flags   = race_table[victim->race].imm;
    victim->res_flags   = race_table[victim->race].res;
    victim->vuln_flags  = race_table[victim->race].vuln;
    victim->form        = race_table[victim->race].form;
    victim->parts       = race_table[victim->race].parts;
    victim->size = pc_race_table[victim->race].size;

    set_race_skills(victim, 1);
    SET_BIT(victim->act,PLR_UNDEAD);

    sprintf(buf, "%s has changed %s's race to %s.", IS_NPC(ch) ? PERS2(ch) : ch->name, victim->name, race_table[victim->race].name);
    log_event(ch, buf);
    do_hal("immortal", buf, buf, NOTE_PENALTY);

    send_to_char("Ok.\n\r",ch);
    send_to_char("The darkness embraces you, as you shed your mortal shell and joine the undead.\n\r",victim);
    act("Darkness consumes $n as $e joins the ranks of the undead.",victim,NULL,NULL,TO_ROOM);
}



void do_quest_avatar(CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    char buf[MIL];

    CHAR_DATA *victim;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (IS_NPC(victim))
      {
	send_to_char("Not on mere mobs.\n\r", ch);
	return;
      }
    if (IS_AVATAR(victim))
    {
	send_to_char("That person already carries the blessing!\n\r",ch);
	return;
    }

    SET_BIT(victim->act2, PLR_AVATAR);
    victim->pcdata->divfavor = 0;

    victim->mod_stat[STAT_STR] = UMIN(get_max_train(victim, STAT_STR), victim->mod_stat[STAT_STR]);
    victim->mod_stat[STAT_INT] = UMIN(get_max_train(victim, STAT_INT), victim->mod_stat[STAT_INT]);
    victim->mod_stat[STAT_WIS] = UMIN(get_max_train(victim, STAT_WIS), victim->mod_stat[STAT_WIS]);
    victim->mod_stat[STAT_DEX] = UMIN(get_max_train(victim, STAT_DEX), victim->mod_stat[STAT_DEX]); 
    victim->mod_stat[STAT_CON] = UMIN(get_max_train(victim, STAT_CON), victim->mod_stat[STAT_CON]);

    victim->perm_stat[STAT_STR] = UMIN(get_max_train(victim, STAT_STR), victim->perm_stat[STAT_STR]);
    victim->perm_stat[STAT_INT] = UMIN(get_max_train(victim, STAT_INT), victim->perm_stat[STAT_INT]);
    victim->perm_stat[STAT_WIS] = UMIN(get_max_train(victim, STAT_WIS), victim->perm_stat[STAT_WIS]);
    victim->perm_stat[STAT_DEX] = UMIN(get_max_train(victim, STAT_DEX), victim->perm_stat[STAT_DEX]); 
    victim->perm_stat[STAT_CON] = UMIN(get_max_train(victim, STAT_CON), victim->perm_stat[STAT_CON]);


    sprintf(buf, "%s has empowered %s into an Avatar.", IS_NPC(ch) ? PERS2(ch) : ch->name, victim->name);
    log_event(ch, buf);
    do_hal("immortal", buf, buf, NOTE_PENALTY);

    send_to_char("Ok.\n\r",ch);
    act("You give your body over to $g and become and Avatar!", victim, NULL, NULL, TO_CHAR);
    act("Energy crackles around the area, and $g power changes $n into an Avatar!", victim, NULL, NULL, TO_ROOM);
}//END QAVATAR


void do_quest_demon(CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    char buf[MIL];

    CHAR_DATA *victim;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (IS_NPC(victim))
      {
	send_to_char("Not on mere mobs.\n\r", ch);
	return;
      }
    if (victim->race == race_lookup("demon"))
    {
	send_to_char("That person has already been possesed.\n\r",ch);
	return;
    }

    victim->race = race_lookup("demon");

    victim->mod_stat[STAT_STR] = UMIN(get_max_train(victim, STAT_STR), victim->mod_stat[STAT_STR]);
    victim->mod_stat[STAT_INT] = UMIN(get_max_train(victim, STAT_INT), victim->mod_stat[STAT_INT]);
    victim->mod_stat[STAT_WIS] = UMIN(get_max_train(victim, STAT_WIS), victim->mod_stat[STAT_WIS]);
    victim->mod_stat[STAT_DEX] = UMIN(get_max_train(victim, STAT_DEX), victim->mod_stat[STAT_DEX]); 
    victim->mod_stat[STAT_CON] = UMIN(get_max_train(victim, STAT_CON), victim->mod_stat[STAT_CON]);

    victim->perm_stat[STAT_STR] = UMIN(get_max_train(victim, STAT_STR), victim->perm_stat[STAT_STR]);
    victim->perm_stat[STAT_INT] = UMIN(get_max_train(victim, STAT_INT), victim->perm_stat[STAT_INT]);
    victim->perm_stat[STAT_WIS] = UMIN(get_max_train(victim, STAT_WIS), victim->perm_stat[STAT_WIS]);
    victim->perm_stat[STAT_DEX] = UMIN(get_max_train(victim, STAT_DEX), victim->perm_stat[STAT_DEX]); 
    victim->perm_stat[STAT_CON] = UMIN(get_max_train(victim, STAT_CON), victim->perm_stat[STAT_CON]);

    victim->affected_by = victim->affected_by|race_table[victim->race].aff;
    victim->imm_flags   = race_table[victim->race].imm;
    victim->res_flags   = race_table[victim->race].res;
    victim->vuln_flags  = race_table[victim->race].vuln;
    victim->form        = race_table[victim->race].form;
    victim->parts       = race_table[victim->race].parts;
    victim->size = pc_race_table[victim->race].size;

    SET_BIT(victim->comm, COMM_TELEPATH);

    if (victim->pcdata->learned[gsn_sanctuary])
      act("With your aggresive nature revealed, $g takes sanctuary away from you.", victim, NULL, NULL, TO_CHAR);
    set_race_skills(victim, 1);
    victim->practice +=2;

    sprintf(buf, "%s has changed %s's race to %s.", IS_NPC(ch) ? PERS2(ch) : ch->name, victim->name, race_table[victim->race].name);
    log_event(ch, buf);
    do_hal("immortal", buf, buf, NOTE_PENALTY);

    send_to_char("Ok.\n\r",ch);
    send_to_char("You forfeit your soul and give yourself over to Chaos.\n\r",victim);
    act("Reality blurs and shift around $n as $e embraces Chaos.",victim,NULL,NULL,TO_ROOM);
}//END QDEMON

/*
void do_quest_druid(CHAR_DATA *ch, char *argument )
{
    int sn;
    char arg[MIL];
    char buf[MIL];

    CHAR_DATA *victim;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Whom?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    if (IS_NPC(victim))
       {
        send_to_char("Not on mere mobs.\n\r", ch);
        return;
      }
    if (victim->class == class_lookup("druid"))
    {
        send_to_char("That person is already a druid.\n\r",ch);
        return;
    }


    victim->mod_stat[STAT_STR] = UMIN(get_max_train(victim, STAT_STR), victim->mod_stat[STAT_STR]);
    victim->mod_stat[STAT_INT] = UMIN(get_max_train(victim, STAT_INT), victim->mod_stat[STAT_INT]);
    victim->mod_stat[STAT_WIS] = UMIN(get_max_train(victim, STAT_WIS), victim->mod_stat[STAT_WIS]);
    victim->mod_stat[STAT_DEX] = UMIN(get_max_train(victim, STAT_DEX), victim->mod_stat[STAT_DEX]);
    victim->mod_stat[STAT_CON] = UMIN(get_max_train(victim, STAT_CON), victim->mod_stat[STAT_CON]);

    victim->perm_stat[STAT_STR] = UMIN(get_max_train(victim, STAT_STR), victim->perm_stat[STAT_STR]
    victim->perm_stat[STAT_INT] = UMIN(get_max_train(victim, STAT_INT), victim->perm_stat[STAT_INT]
    victim->perm_stat[STAT_WIS] = UMIN(get_max_train(victim, STAT_WIS), victim->perm_stat[STAT_WIS]
    victim->perm_stat[STAT_DEX] = UMIN(get_max_train(victim, STAT_DEX), victim->perm_stat[STAT_DEX]
    victim->perm_stat[STAT_CON] = UMIN(get_max_train(victim, STAT_CON), victim->perm_stat[STAT_CON]


    victim->practice += 6;

    victim->affected_by = victim->affected_by|race_table[victim->race].aff;
    victim->imm_flags   = victim->imm_flags|race_table[victim->race].imm;
    victim->res_flags   = victim->res_flags|race_table[victim->race].res;
    victim->vuln_flags  = victim->vuln_flags|race_table[victim->race].vuln;
    victim->form        = race_table[victim->race].form;
    victim->parts       = race_table[victim->race].parts;
    victim->size = pc_race_table[victim->race].size;
    
     
    for ( sn = 0; sn < MAX_SKILL; sn++ )
        if ( skill_table[sn].name != NULL 
	&& skill_table[sn].rating[victim->class] < 10 
	&& skill_table[sn].rating[victim->class] > 0 
	&& skill_table[sn].skill_level[0] !=69 && get_skill(victim,sn) == 0)
            victim->pcdata->learned[sn] = 1;

    sprintf(buf, "%s has changed %s's class to %s.", IS_NPC(ch) ? PERS2(ch) : ch->name, victim->name, class].name);
    log_event(ch, buf);
    do_hal("immortal", buf, buf, NOTE_PENALTY);

    send_to_char("Ok.\n\r",ch);
    send_to_char("You enter into the secret world of the Druids, leaving population behind.\n\r",victim);
    act("$n enters into the secret world of the Druids, leaving humanity behind.",victim,NULL,NULL,TO_ROOM);
}
*/

/* changes class/skills to healer */
void to_healer(CHAR_DATA* ch)
{
  int sn = 0;
  char buf[MIL];
 if (!has_custom_title(ch))
   {
     ch->class = class_lookup("healer");
     sprintf( buf, "the %s", title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
     set_title(ch, buf);
   }
 else
   ch->class = class_lookup("healer");


  for ( sn = 0; sn < MAX_SKILL; sn++ )
    if ( skill_table[sn].name != NULL 
	 && skill_table[sn].rating[ch->class] < 10 
	 && skill_table[sn].rating[ch->class] > 0 
	 && skill_table[sn].skill_level[0] != 69 && get_skill(ch,sn) == 0)
      ch->pcdata->learned[sn] = 1;
  act("You feel $g's blessing as you dedicate your life to healing.\n\r",ch, NULL, NULL, TO_CHAR);
  act("A holy aura envelops $n as $e dedicates his life to healing of others.",ch,NULL,NULL,TO_ROOM);
  set_race_skills(ch, -1);
}

/* changes class/skills to druid */
void to_druid(CHAR_DATA* ch)
{
  int sn = 0;
  char buf[MIL];
 if (!has_custom_title(ch))
   {
     ch->class = class_lookup("druid");
     sprintf( buf, "the %s", title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
     set_title(ch, buf);
   }
 else
   ch->class = class_lookup("druid");
 
 for ( sn = 0; sn < MAX_SKILL; sn++ )
   if ( skill_table[sn].name != NULL 
	&& skill_table[sn].rating[ch->class] < 10 
	&& skill_table[sn].rating[ch->class] > 0 
	&& skill_table[sn].skill_level[0] != 69 && get_skill(ch,sn) == 0)
     ch->pcdata->learned[sn] = 1;
 
 send_to_char("You leave your aggressive naure behind and dedicate your life to nature.\n\r",ch);
 act("A green aura envelops $n as $e dedicates his life to powers of nature.",ch,NULL,NULL,TO_ROOM);
 set_race_skills(ch, -1);
}


/* changes class/skills to shaman */
void to_shaman(CHAR_DATA* ch)
{
  int sn = 0;
  char buf[MIL];

 if (!has_custom_title(ch))
   {
     ch->class = class_lookup("shaman");
     sprintf( buf, "the %s", title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
     set_title(ch, buf);
   }
 else
   ch->class = class_lookup("shaman");

 for ( sn = 0; sn < MAX_SKILL; sn++ ){
   if ( skill_table[sn].name != NULL 
	&& skill_table[sn].rating[ch->class] < 10 
	&& skill_table[sn].rating[ch->class] > 0 
	&& skill_table[sn].skill_level[0] != 69 
	&& get_skill(ch,sn) == 0)
     ch->pcdata->learned[sn] = 1;
 }
 act("$g smiles on you as you embrace evil in all its forms and join the guild of shamans.",ch, NULL, NULL, TO_CHAR);
 act("A dark evil aura envelops $n as $e dedicates his life to spreading evil amongst others.",ch,NULL,NULL,TO_ROOM);
 set_race_skills(ch, -1);
}


void do_quest_healer(CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    char buf[MIL];
    CHAR_DATA *victim;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( " whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (IS_NPC(victim))
      {
	send_to_char("Not on mere mobs.\n\r", ch);
	return;
      }
    if (victim->class == class_lookup("healer"))
    {
	send_to_char("That person is already a healer.\n\r",ch);
	return;
    }


    victim->practice += 6;
    to_healer(victim);

    sprintf(buf, "%s has changed %s's class to %s.", ch->name, victim->name, class_table[victim->class].name);
    log_event(ch, buf);

    send_to_char("Done.\n\r",ch);
}

void do_quest_druid(CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    char buf[MIL];
    CHAR_DATA *victim;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( " whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (IS_NPC(victim))
      {
	send_to_char("Not on mere mobs.\n\r", ch);
	return;
      }
    if (victim->class == class_lookup("druid"))
    {
      send_to_char("That person is already a druid.\n\r",ch);
	return;
    }
    
    while(victim->pcdata->newskills){
      nskill_remove(victim, victim->pcdata->newskills);
    }
    
    victim->practice += 6;
    to_druid(victim);

    sprintf(buf, "%s has changed %s's class to %s.", ch->name, victim->name, class_table[victim->class].name);
    do_hal("immortal", buf, buf, NOTE_PENALTY);
    log_event(ch, buf);
    send_to_char("Done.\n\r",ch);
}


void do_quest_shaman(CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    char buf[MIL];
    CHAR_DATA *victim;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( " whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if (IS_NPC(victim))
      {
	send_to_char("Not on mere mobs.\n\r", ch);
	return;
      }
    if (victim->class == class_lookup("shaman"))
    {
	send_to_char("That person is already a shaman.\n\r",ch);
	return;
    }


    victim->practice += 6;
    to_shaman(victim);

    sprintf(buf, "%s has changed %s's class to %s.", ch->name, victim->name, class_table[victim->class].name);
    log_event(ch, buf);
    send_to_char("Done.\n\r",ch);
}

/* command that allows the clerics to convert to shman/healer. */
void do_convert( CHAR_DATA *ch, char *argument)
{
  bool fGood = FALSE;
  bool fFound = FALSE;

  int iClass = 0;
  int iGuild = 0;
  int iDoor = 0;

  ROOM_INDEX_DATA *to_room;
  char buf[MIL];
  char arg1[MIL];
  one_argument(argument, arg1);

  //check if is a cleric.
  if (ch->class != class_lookup("cleric"))
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }
  //bit of redundant checking.
  if (arg1[0] == '\0')
    {
      send_to_char("convert:\n\rsyntax:convert <your name>\n\r"\
		   "Use this command at the doorstep of shaman/healer guild to take on their calling.\n\r", ch);
      return;
    }

  if (get_char(arg1) != ch)
    {
      send_to_char("YOUR NAME.\n\r", ch);
      return;
    }

  if (IS_GOOD(ch))
    fGood = TRUE;

  //now we look for guild entrance.
  for (iDoor = 0; iDoor < 6; iDoor ++)
    {
      if (ch->in_room->exit[iDoor] == NULL)
	continue;
      to_room = ch->in_room->exit[iDoor]->to_room;
      for ( iClass = 0; class_table[iClass].name != NULL && iClass < MAX_CLASS; iClass++ )
	for ( iGuild = 0; iGuild < MAX_GUILD; iGuild ++)	
	  if ( to_room->vnum == class_table[iClass].guild[iGuild] )
	    if ((iClass == class_lookup("shaman") && !fGood)
		|| (iClass == class_lookup("healer") && fGood)
		)
	      fFound = TRUE;
    }
  //check if right palce.
  if (!fFound)
    {
      sendf(ch, "You must be at the entrance of, or inside the %s guild to change your calling.\n\r", (fGood ? "Healer" : "Shaman"));
      return;
    }

  //now we check things.
  if (fFound && ch->level != 30)
    {
      send_to_char("You may only convert to new calling when at rank 30.\n\rNo more, no less.\n\r", ch);
      return;
    }

  //we convert.
  if (fGood)
    to_healer(ch);
  else
    to_shaman(ch);

  ch->practice += 6;

  sprintf(buf, "%s has changed his clerical calling to %s.", ch->name, class_table[ch->class].name);
  log_event(ch, buf);

}

void do_badname( CHAR_DATA *ch, char *argument)
{
    char strsave[MIL], namelist[MSL], nameread[MSL], name[MSL];
    FILE *fp;
    if ( argument[0] == '\0' )
    {
	send_to_char( "syntax: badname <name>.\n\r", ch );
	return;
    }
    fclose( fpReserve );
    sprintf( strsave, "%s", ILLEGAL_NAME_FILE );
    sprintf(name, "%s\n", argument);
    sprintf(namelist,"%s","");
    if ( (fp = fopen( strsave, "r" ) ) != NULL )
    {
	for ( ; ; )
	{
            fscanf (fp, "%s", nameread);
	    if ( !str_cmp( nameread, "END" ) )
           	break;
	    else
	    {
		strcat(namelist, nameread);
		strcat(namelist,"\n");
	    }
	}
    }
    else
	fp = fopen( NULL_FILE, "r" );
    fclose( fp );
    fp = fopen( strsave, "w" );
    strcat( namelist, name );
    fprintf( fp, "%s", namelist );
    fprintf( fp, "END" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    send_to_char( "All set, that name is now illegal.\n\r",ch);
}

void do_outcast( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Outcast whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }
    if ( get_trust( victim ) > get_trust( ch ) )
    {
	send_to_char( "You failed.\n\r", ch );
	return;
    }
    if(IS_SET(victim->act, PLR_OUTCAST))
    {
    	REMOVE_BIT(victim->act, PLR_OUTCAST);
    	send_to_char( "You are no longer an outcast!\n\r", victim );
    	sprintf(buf,"$N un-outcasts %s",victim->name);
    	wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    	sendf(ch, "You un-outcasted %s.\n\r", PERS2(victim));
	return;
    }
    SET_BIT(victim->act, PLR_OUTCAST);
    send_to_char( "You have been branded as an outcast!\n\r", victim );
    sprintf(buf,"$N outcasts %s",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,get_trust(ch));
    sendf(ch, "You have outcasted %s.\n\r", PERS2(victim));
}

void do_advself (CHAR_DATA *ch, char *argument)
{
  char buf[MIL];
    int iLevel = 0;
    int iCurLevel = 0;

    if (IS_NPC(ch))
      return;
    if (ch->pcdata->deity[0] == '\0')
      {
	send_to_char("You must choose a deity first. (cmd: deity)\n\r", ch);
	return;
      }
    if (ch->level >= 50)
      {
	send_to_char("Huh?\n\r", ch);
	return;
      }
    if (ch->level < 20)
      iLevel = 20;
    else if(ch->level < 30)
      iLevel = 30;
    else if(ch->level < 35)
      iLevel = 35;
    else if(ch->level < 40)
      iLevel = 40;
    else if(ch->level < 45)
      iLevel = 45;
    else 
      iLevel = 50;

//set max stats.
    ch->perm_stat[STAT_STR] =  get_max_train(ch,STAT_STR);
    ch->perm_stat[STAT_INT] =  get_max_train(ch,STAT_INT);
    ch->perm_stat[STAT_WIS] =  get_max_train(ch,STAT_WIS);
    ch->perm_stat[STAT_DEX] =  get_max_train(ch,STAT_DEX);
    ch->perm_stat[STAT_CON] =  get_max_train(ch,STAT_CON);

    send_to_char( "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", ch);
    for ( iCurLevel = ch->level ; iCurLevel < iLevel; iCurLevel++ )
    {
        ch->level += 1;
        advance_level( ch,FALSE);
    }
    sendf(ch,"You are now level %d.\n\r",ch->level);
    ch->exp     = total_exp(ch)-exp_per_level(ch,ch->level);
    ch->max_exp = ch->exp;
    ch->trust   = 0;
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
    ch->move = ch->max_move;

    //set new skills.
    sprintf(buf, "%s class 80", ch->name);
    do_sset(ch, buf);
    //set anatomies
    if (ch->class == class_lookup("monk"))
      for(iCurLevel = 0; iCurLevel < MAX_ANATOMY; iCurLevel++)
	ch->pcdata->anatomy[iCurLevel] = 75;
    save_char_obj(ch);
}



void do_matchip( CHAR_DATA *ch, char *argument )
{
/* We run through all descriptor and put them in single array to be sorted. */
  const int MAX_SIZE = 128;
  char buf[MSL];

  DESCRIPTOR_DATA* desc[MAX_SIZE];
  DESCRIPTOR_DATA *d, *d2;
  BUFFER *buffer;
  CHAR_DATA* vch;

  int max_d = 0;
  int i = 0;
  int count = 0;

  for ( d = descriptor_list; d != NULL && max_d < MAX_SIZE; d = d->next ){
    if (d == NULL)
      continue;
    if ( d->character == NULL 
	 || !can_see( ch, d->character ) )
      continue;
    desc[max_d++] = d;
  }
  /* simple sort for identical hosts */
  for (i = 0; i < max_d; i++){
    int j = 0;
    for (j = i + 1; j < max_d; j++){
      d = desc[i];
      d2 = desc[j];
      /* check for matching hosts */
      if (str_cmp(d->host, d2->host))
	continue;
      /* swap */
      d = desc[i + 1];
      desc[i + 1] = desc[j];
      desc[j] = d;
    }//END swap
  }//End sort

/* all descriptors now sorted, we print the data out */
  buffer = new_buf();
  sprintf(buf, "[De#   CS   Login Idle ] Player Name     Site  Number       Site Name\n\r");
  add_buf(buffer, buf);
  sprintf(buf, "------------------- --------------- ---------------- ----------------------------\n\r");
  add_buf(buffer, buf);

  for (i = 0; i < max_d; i++){
    bool fHigh = FALSE;
    char host[MIL], ident[MIL];


    d = desc[i];
/* get strings */
    strcpy(host,capitalize(d->host));
    host[0] = LOWER(host[0]);
    
    strcpy(ident,capitalize(d->ident));
    ident[0] = LOWER(ident[0]);

/* rejection */
    if ( d->character == NULL 
	 || !can_see( ch, d->character ) )
      continue;

/* rest of selection */
    if (IS_NULLSTR(argument) 
	|| is_name(argument, d->character->name)
	|| (d->original && is_name(argument,d->original->name)) 
	|| strstr(host,argument) != NULL || strstr(ident,argument) != NULL){
/* start printing info */
      char tim[MIL], idle[MIL];
      vch = (d->original != NULL ? d->original : d->character);
/* get more strings */
      strftime( tim, MIL, "%I:%M%p", localtime( &vch->logon ) );
      if ( vch->idle > 0 )
	sprintf( idle, "%-2d", vch->idle );
      else
	sprintf( idle, "  " );
/* format the string */

/* if this string is identical to the last one we high light it */
      if (i + 1 < max_d && !str_cmp(d->ident, desc[i + 1]->ident) && d->connected == CON_PLAYING)
	fHigh = TRUE;
      else if (i > 0 && !str_cmp(d->ident, desc[i - 1]->ident) && d->connected == CON_PLAYING)
	fHigh = TRUE;

      sprintf( buf, "[%3d %8.8s %7s %2s] %-15s %s%-20s %-20s%s\n\r",
	       d->descriptor, 
	       chargen_table[UMIN(d->connected, CGEN_MAX)].name, 
	       tim, 
	       idle,
	       d->original ? d->original->name 
	       : d->character ? d->character->name : "(none)", 
	       fHigh ? "`A" : "``",
	       d->ident, 
	       d->host,
	       fHigh ? "``" : "``");
      add_buf(buffer, buf);
      count++;
    }
  }
/* add on the link dead players */
  for ( vch = player_list; vch != NULL; vch = vch->next_player ){
    char tim[MIL], idle[MIL];
    if (vch->desc != NULL)
      continue;
    if ( !can_see( ch, vch ) 
	 || (argument[0] != '\0' && !is_name(argument, vch->name)))
      continue;
    
    strftime( tim, MIL, "%I:%M%p", localtime( &vch->logon ) );
    if ( vch->idle > 0 )
      sprintf( idle, "%-2d", vch->idle );
    else
      sprintf( idle, "  " );
    sprintf( buf, "[--- -- %7s %2s] %-15s\n\r",
	     tim, idle, vch->name);
    add_buf(buffer, buf);
    count ++;
  }
  if (count == 0){
    send_to_char("No one by that name is connected.\n\r",ch);
    free_buf(buffer);
    return;
  }
  sprintf( buf, "%d user%s\n\r", count, count == 1 ? "" : "s" );
  add_buf(buffer, buf);
  page_to_char( buf_string(buffer), ch);
}

void do_impget( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Impget what?\n\r", ch );
	return;
    }
    if ( ( location = get_room_index( 3 ) ) == NULL )
    {
	send_to_char( "No such location.\n\r", ch );
	return;
    }
    obj = get_obj_list( ch, arg, location->contents );
    obj_from_room(obj);
    obj_to_char(obj,ch);
    act("You impget $p from the forbidden room.",ch,obj,NULL,TO_CHAR);
}

void do_grant( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], arg2[MIL], arg3[MIL], arg4[MIL];
    CHAR_DATA *victim;
    SKILL_DATA *fsk, nsk;
    int sn = 0, learned = 0, level = 0;
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    if ( arg[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Grant who what?\n\r", ch );
	send_to_char( "Syntax: grant <char> <skill> <percentage> <level>\n\r",ch );
	return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    if (IS_NPC(victim))
    {
	send_to_char("You can't grant a mob new skills.\n\r",ch);
	return;
    }
    if ( (sn = skill_lookup(arg2)) < 0)
    {
	send_to_char("That skill doesn't exist!\n\r",ch);
	return;
    }
    if (arg3[0] == '\0')
	learned = 1;
    else if (is_number(arg3))
	learned = atoi(arg3);
    else
	learned = 75;
    if (arg4[0] == '\0')
	level = 1;
    else if (is_number(arg4))
	level = atoi(arg4);
    else
	learned = victim->level;
    if ( (fsk = nskill_find(victim->pcdata->newskills,sn)) == NULL )
    {
	nsk.keep 	= 1;
	nsk.sn		= sn;
	nsk.level	= level;
	nsk.rating	= 1;
	nsk.learned	= learned;
	nskill_to_char(victim,&nsk);
	act("You have granted $t to $N.",ch,skill_table[sn].name,victim,TO_CHAR);
	act("$n has granted $t to you.",ch,skill_table[sn].name,victim,TO_VICT);
    }
    else
    {
	nskill_remove(victim, fsk);
	act("You have taken $t from $N.",ch,skill_table[sn].name,victim,TO_CHAR);
	act("$n has taken $t from you.",ch,skill_table[sn].name,victim,TO_VICT);
    }
}

void do_noquit( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL];
    CHAR_DATA *victim;
    argument = one_argument( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: noquit <char>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Why would you want to noquit a mob?\n\r",ch);
	return;
    }
    if (IS_SET(victim->act2, PLR_NOQUIT))
    {
    	REMOVE_BIT(victim->act2, PLR_NOQUIT);
    	sendf(ch, "%s can quit now.\n\r", PERS2(victim));
    }
    else
    {
    	SET_BIT(victim->act2, PLR_NOQUIT);
    	sendf(ch, "%s can't quit anymore.\n\r", PERS2(victim));
    }
}

void do_noaffquit( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL];
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int duration = 12;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: noquit <char> <ticks>\n\r",ch);
        return;
    }
    if ( arg2[0] != '\0' )
	duration = 12;
    else if (!is_number(arg2))
    {
	send_to_char("That is not a number.\n\r",ch);
	return;
    }
    duration = atoi(arg2);
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }
    if ( IS_NPC(victim) )
    {
	send_to_char( "Why would you want to noquit a mob?\n\r",ch);
	return;
    }
    if (is_affected(victim,gsn_noquit))
    {
	affect_strip(victim,gsn_noquit);
    	sendf(ch, "%s can quit now.\n\r", PERS2(victim));
    }
    else
    {
    	af.where     = TO_AFFECTS;
    	af.type      = gsn_noquit;
    	af.level     = ch->level;
    	af.duration  = duration;
    	af.modifier  = 0;
    	af.location  = 0;
    	af.bitvector = 0;
    	affect_to_char( victim, &af );
    	sendf(ch, "%s can't quit anymore for %d ticks.\n\r", PERS2(victim),duration);
    }
}

void do_set_alarm( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL];
    ROOM_INDEX_DATA *location;
    argument = one_argument( argument, arg1 );
    if (ch->in_room == NULL)
	return;
    if ( arg1[0] == '\0' )
	location = ch->in_room;
    else if ( ( location = find_location( ch, arg1 ) ) == NULL )
    {
        send_to_char( "No such location.\n\r", ch );
        return;
    }
    if (IS_SET(location->room_flags2, ROOM_ALARM))
    {
    	REMOVE_BIT(location->room_flags2, ROOM_ALARM);
	send_to_char("Alarm removed.\n\r",ch);
    }
    else
    {
    	SET_BIT(location->room_flags2, ROOM_ALARM);
	send_to_char("Alarm set.\n\r",ch);
    }
}

void do_qtrans( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL], arg2[MIL];
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim, *player;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Quiet Transfer whom (and where)?\n\r", ch );
	return;
    }
    if ( !str_cmp( arg1, "all" ) )
    {
	for ( player = player_list; player != NULL; player = player->next_player )
	    if ( player != ch && player->in_room != NULL && can_see( ch, player ) )
	    {
		char buf[MSL];
		sprintf( buf, "%s %s", player->name, arg2 );
		do_transfer( ch, buf );
	    }
	return;
    }
    if ( arg2[0] == '\0' )
	location = ch->in_room;
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    send_to_char( "No such location.\n\r", ch );
	    return;
	}
        if ( room_is_private( location ) &&  get_trust(ch) < MAX_LEVEL)
	{
	    send_to_char( "That room is private right now.\n\r", ch );
	    return;
	}
        if ( !have_access(ch,location))
        {
            send_to_char( "Don't think so.\n\r",ch);
            return;
        }
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim->in_room == NULL )
    {
	sendf( ch, "%s is in limbo.\n\r", PERS2(victim) );
	return;
    }
    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    if (!IS_NPC(victim))
	victim->pcdata->transnum = victim->in_room->vnum;
    char_from_room( victim );
    char_to_room( victim, location );
    do_look( victim, "auto" );
    sendf( ch, "You have quiet transferred %s.\n\r", PERS2(victim) );
}



void do_allowname( CHAR_DATA *ch, char *argument)
{
    char strsave[MIL], namelist[MSL], nameread[MSL], name[MSL];
    FILE *fp;
    if ( argument[0] == '\0' )
    {
	send_to_char( "syntax: goodname <name>.\n\r", ch );
	return;
    }
    fclose( fpReserve );
    sprintf( strsave, "%s", APPROVED_NAME_FILE );
    sprintf(name, "%s\n", argument);
    sprintf(namelist,"%s","");
    if ( (fp = fopen( strsave, "r" ) ) != NULL )
    {
	for ( ; ; )
	{
            fscanf (fp, "%s", nameread);
	    if ( !str_cmp( nameread, "END" ) )
           	break;
	    else
	    {
		strcat(namelist, nameread);
		strcat(namelist,"\n");
	    }
	}
    }
    else
	fp = fopen( NULL_FILE, "r" );
    fclose( fp );
    fp = fopen( strsave, "w" );
    strcat( namelist, name );
    fprintf( fp, "%s", namelist );
    fprintf( fp, "END" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    send_to_char( "That name can be allowed through a newbie ban.\n\r",ch);
}


////////////////
//do_broadcast//
////////////////
//sends a string characters in all the room surroudinding.
void do_broadcast( CHAR_DATA *ch, char* argument )
{
  if ( argument[0] == '\0' )
    {
      send_to_char( "Send what to people in room around you?\n\r", ch );
      return;
    }
  send_to_char("You send the message to everyone around you\n\r", ch);
  //Args are ussualy not \r\n terminated so we tag that on.
  strcat(argument, "\n\r");
  broadcast(ch, argument);
}


/*do_quest_elder (qelder)*/
/*linked in act_wiz.c only*/
void do_quest_elder( CHAR_DATA *ch, char *argument )
{
  
//Following toggles the unholy_gift ability on and off.
  char buf [MIL];
  char arg [MIL];
  int sk;

  
//Pointers for target and skills.
  CHAR_DATA* victim;
  SKILL_DATA * fsk = NULL;

  
//Few flags.
  bool is_elder = TRUE;

  
//Fist we check for some usuals stuff.
  if (ch == NULL)
    return;
 
  
//The we do argument.
  argument = one_argument( argument, arg );
  if ( arg[0] == '\0')
    {
      send_to_char( "qvamp\n\r"\
		    "Toggles the targets \"Unholy Gift\" ability.\n\r"\
		    "\tQuest Elder syntax:\n\r"\
		    "\tqelder <player>\n\r", ch );
      return;
    }
  if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
      send_to_char( "They aren't playing.\n\r", ch );
      return;
    }

  
//Now we check if the target is a player.
  if (IS_NPC(victim))
    {
      send_to_char("Why would you want to grant such status to a mere Mob?\n\r",ch);
      return;
    }

  
//And the we check for vamp ablity.
  if ( victim->race != race_lookup("vampire") 
       || victim->class != class_lookup("vampire") 
       || !IS_SET(victim->act, ACT_UNDEAD) )
    {
      send_to_char("Elders is a \"vampire exclusive\" club.\n\r", ch);
      return;
    }

 
//Finaly we check that the player has the skill so we can take it away.
  if ( ((sk =  get_skill(victim, gsn_unholy_gift)) == 0) &&  ( (fsk = nskill_find(victim->pcdata->newskills,gsn_unholy_gift)) == NULL) )
    {
      //      sprintf(buf, "skill: %d\n\r", sk);
      //      send_to_char(buf, ch);
      is_elder = FALSE;
    }



  if (is_elder)
    {
//he has the skill, we take it away.
      victim->pcdata->learned[gsn_unholy_gift] = 0;
      if ( (fsk = nskill_find(victim->pcdata->newskills,gsn_unholy_gift)) != NULL)
	nskill_remove(victim, fsk);
      sprintf(buf, "You strip %s of his superior status.\n\r", victim->name);
      send_to_char(buf, ch);
      send_to_char("You fall from grace in the eyes of Elders and you are once again a mere Kindred.\n\r", victim);
      wiznet("$n has removed $N's Elder status.",victim,NULL,WIZ_PENALTIES,WIZ_SECURE,LEVEL_HERO);
      sprintf(buf, "%s has removed %s's Vampire Elder status.", IS_NPC(ch) ? PERS2(ch) : ch->name, victim->name);
      log_event(ch, buf);
      do_hal("immortal", buf, buf, NOTE_PENALTY);

    }
  else
    {
    
//He doesnt have the skill and we give it to him.
      victim->pcdata->learned[gsn_unholy_gift] = 100;
      sprintf(buf, "You grant %s the trust of the clan, and elevate him to true power of an Elder.\n\r", victim->name);
      send_to_char(buf, ch);
      send_to_char("The Elders have accepted you as their equal.  May mortals tremble in fear before you.\n\r", victim);
      wiznet("$n has granted $N the status of an Elder of Kindred.",victim,NULL,WIZ_PENALTIES,WIZ_SECURE,LEVEL_HERO);
      sprintf(buf, "%s has granted %s Vampire Elder status.", IS_NPC(ch) ? PERS2(ch) : ch->name, victim->name);
      do_hal("immortal", buf, buf, NOTE_PENALTY);
      log_event(ch, buf);
    }
}//end do_quest_elder


/*do_masquerade*/
/*links only to act_wiz.c*/
void do_masquerade(CHAR_DATA *ch, char *argument)
{
//Toggles the masquerade flag for immortals, strips it for mortals.
  char arg[MIL];
  char buf[MIL];
  CHAR_DATA* victim;

  bool is_mortal = TRUE;
  bool is_masked = FALSE;
  
//First we check the usual problems.
  if (ch == NULL)
    return;

  
//check if this is mortal or not.
  if (IS_IMMORTAL(ch))
    is_mortal = FALSE;

  
//check for immortal or vamp.
  if ( (is_mortal) && (ch->class != class_lookup("vampire")) )
    {
      send_to_char("Huh?\n\r", ch);
      return;
    }

  
//The we do argument.
  argument = one_argument( argument, arg );
  if ( arg[0] == '\0')
    {
//immortal reminder.
      if (!is_mortal)
	{
	  send_to_char( "masquerade\n\r"\
			"Toggles the masquerade flag.\n\r"\
			"\tMasquerade syntax:\n\r"\
			"\tmasquerade <player>\n\r", ch);
	  return;
	}
      else
	{
	  if (IS_SET(ch->act2, PLR_MASQUERADE))
	    {
//Mortal reminder.
	      send_to_char("If you shall proceed with this, "\
			   "your nature will be revealed.\n\r"\
			   "There is no going back after you complete "\
			   "this.\n\r\n\rEnter \"masquerade <your name>\""\
			   "to proceed.\n\r", ch);
	      return;
	    }
	  else
	    {
	      send_to_char("You have chosen to defay a millenia old custom.  There is no way back now.\n\r", ch);
	      return;
	    }
	}//end mortal reminder.
    }//end null arg.
  
//check if target is around.
  if ( ( victim = get_char_world(ch, arg ) ) == NULL )
    {
      if (is_mortal)
	{
	  send_to_char("YOUR name, remeber?\n\r", ch);
	  return;
	}
      else
	{
	  send_to_char( "They aren't playing.\n\r", ch );
	  return;
	}
    }

  
//Now we check if the target is a player.
  if (IS_NPC(victim))
    {
      if (is_mortal)
	{
	  send_to_char("YOUR name, remeber?\n\r", ch);
	  return;
	}
      else
	{
	  send_to_char("Enter Player name.\n\r",ch);
	  return;
	}
    }

//check for mortals naming themselfs.
  if (is_mortal && ch != victim)
    {
      send_to_char("YOUR name, remeber?\n\r", ch);
      return;
    }

  //check for mortals trying to get back in.
if (is_mortal && !IS_SET(victim->act2, PLR_MASQUERADE))
  {

    send_to_char("You have chosen to defay a millenia old custom.  There is no way back now.\n\r", ch);
    return;
  }
  
//And the we check for vamp ablity.
  if (victim->class != class_lookup("vampire"))
    {
      if(is_mortal)
	{
	  send_to_char("YOUR name, remeber?\n\r", ch);
	  return;
	}
      else
	{
	  send_to_char ("Masquerade is a \"vampire exclusive\" thing.\n\r", ch);
	  return;
	}
    }
  

//Now we know that immortals target a vamp, or mortal himself, and its not npc.
//we toggle the status now.

  
  if (IS_SET(victim->act2, PLR_MASQUERADE))
    is_masked = TRUE;

  if (is_masked)
    {
//we remove the flag and give him proper vamp title.
      REMOVE_BIT(victim->act2, PLR_MASQUERADE);
  
//Now we switch titles, only if current one is a proper DK title.
//we get a proper DK title.
      sprintf( buf, " the %s", title_table [class_lookup("dark-knight")] [victim->level] [victim->sex == SEX_FEMALE ? 1 : 0] );
//and we compare with current.
      if (!strcmp(buf, victim->pcdata->title))
	{
//match, we switch title.
	  sprintf( buf, "the %s", title_table [victim->class] [victim->level] [victim->sex == SEX_FEMALE ? 1 : 0] );
//and we set it.
	  set_title( victim, buf );
	}//end if titles matched.
      
//now we shoot out some messages.
      if (is_mortal)
	{
	  send_to_char("You defy the ancient custom and show your true face to the world.\n\r", victim);
	  wiznet("$N has revealed $S nature and defayed the Masuquerade.",victim,NULL,WIZ_PENALTIES,WIZ_SECURE,LEVEL_HERO);
	  sprintf(buf, "%s has removed masquarade status.", victim->name);
	  log_event(victim, buf);
	}
      else
	{
	  send_to_char("The Elders reject you from the Masquerade.  The world will now see you for what you are.\n\r", victim);
	  act("You have revealed $N's true nature to the world.", ch, NULL, victim, TO_CHAR);
	  sprintf(buf, "%s has revealed %s's nature and rejected him from Masquerade.", ch->name, victim->name);
	  wiznet(buf,victim,NULL,WIZ_PENALTIES,WIZ_SECURE,LEVEL_HERO);

	  sprintf(buf, "%s has removed %s's masquarade status.", ch->name, victim->name);
	  log_event(ch, buf);
	}
    }//end if masked.
  else
    {
//We set the flag, and give him a proper DK title.
      SET_BIT(victim->act2, PLR_MASQUERADE);
      
//we get a proper VAMP title.
      sprintf( buf, " the %s", title_table [victim->class] [victim->level] [victim->sex == SEX_FEMALE ? 1 : 0] );
      
//and we compare with current.
      if (!strcmp(buf, victim->pcdata->title))
	{
//match, we switch title.
	  sprintf( buf, "the %s", title_table [class_lookup("dark-knight")] [victim->level] [victim->sex == SEX_FEMALE ? 1 : 0] );
	  
//and we set it.
	  set_title( victim, buf );
	}//end if titles matched.
      
//now we shoot out some messages.
      send_to_char("The Elders forgive your transgressions and you are once again partake in the Masquerade.\n\r", victim);
      act("You allow $N to once agian be part of Masquerade.", ch, NULL, victim, TO_CHAR);
      sprintf(buf, "%s has hidden %s's nature and allowed him to take part in the Masuquerade.", ch->name, victim->name);
      wiznet(buf,victim,NULL,WIZ_PENALTIES,WIZ_SECURE,LEVEL_HERO);

	  sprintf(buf, "%s has given %s masquarade status.", ch->name, victim->name);
	  log_event(ch, buf);
    }//end if not masked.
}//end do_quest_masquerade.    



/*do_ospell*/
/*links only to act_wiz.c*/
void do_ospell(CHAR_DATA *ch, char *argument)
{

//fields
char cmd [MIL];
char arg1[MIL];
char arg2[MSL];
char arg3[MSL];
char arg4[MSL];

OBJ_DATA* obj;
OBJ_SPELL_DATA* spell;
OBJ_SPELL_DATA new_spell;

//data
int vnum = 0;
int target = SPELL_TAR_SELF;
int fre = 0;


  /*
This is the imm command to give a spell effect to an item in inventory
NOT to set it in the object database to be loaded. Do that in OLC.
Syntax:
ospell <cmd> [arguments] <item>
commands: 
- add <spell>
- set <spell> <field>
     fields:
	- target <self / victim>
	- frequency <percent>
	- selfmessage <string?
	- roommessage <string>
- view 
  */

//get the arguments.
 argument = one_argument(argument, cmd);
 argument = one_argument(argument, arg1);
 argument =  one_argument(argument, arg2);
 argument =  one_argument(argument, arg3);
 one_argument(argument, arg4);



 //DEBUG
 //sendf(ch, "cmd: %s arg1: %s arg2: %s arg3: %s arg4 %s\n\r",
 //cmd,arg2,arg2,arg3,arg4);

//Info first.
  if (cmd[0] == '\0')
    {
      send_to_char("ospell (ObjectSpell)\n\rSyntax: ospell <command>"\
		   "[paramaters] <item>\n\r"\
		   "avaliable commands are:\n\r"\
		   "\tadd\n\r"\
		   "\tdelete\n\r"\
		   "\tset\n\r"\
		   "\tview\n\r"\
		   "Type command without arguments for more info.\n\r", ch);
      return;
    }
  else
    if (arg1[0]=='\0')
      {
	if (!str_prefix(cmd, "add"))
	  {
	    send_to_char("ospell: add <spell>\n\r"\
			 "Adds the specified spell onto the item "\
			 "setting the default values for parameters.\n\r", ch);
	    return;
	  }
	else if (!str_prefix(cmd, "delete"))
	  {
	    send_to_char("ospell: delete <spell>\n\r"\
			 "Removes single instance of the specified spell"\
			 "from the item.\n\r", ch);
	    return;
	  }
	else if (!str_prefix(cmd, "set"))
	  {
	    send_to_char("ospell: set <spell>> <field> <value>\n\r"\
			 "Sets the specified value.\n\n\r"\
			 "Required fields:\n\r"\
			 "\ttarget <self/victim>\n\r"\
			 "\tfrequency <number>\n\r"\
			 "\tseemessage <string>\n\r"\
			 "\tblindmessage <string>\n\r"\
			 "Optional fields:\n\r"\
			 "\tspell <name>\n\r", ch);
	    return;
	  }
	else if (!str_prefix(cmd, "view"))
	  {
	    send_to_char("ospell: view\n\r"\
			 "Displays ALL the spells on the item with their"\
			 "respective fields.\n\r", ch);
	    return;
	  }
      }//end cmd help


  //Begins the commands.
  if (!str_prefix(cmd, "add"))
    {
      //check for valid spell.
      if ( ((vnum = skill_lookup(arg1)) == -1) || vnum > MAX_SKILL)
	{
	  send_to_char("That spell does not exist.\n\r", ch);
	  return;
	}
      if (skill_table[vnum].spell_fun == spell_null)
	{
	  send_to_char("That spell does not exist.\n\r", ch);
	  return;
	}

      //check for items specified 
      if (arg2[0] == '\0')
	{send_to_char("Need target item.\n\r", ch);return;}

      //check if item is on char.
      if ((obj = get_obj_here(ch, NULL, arg2)) == NULL)
	{
	  send_to_char("Target item must be carried, worn or in the room.\n\r", ch);
	  return;
	}

      //create dummy spell.
      new_spell.spell = vnum;
      new_spell.target = target;
      new_spell.percent = fre;
      //the strings must be set after we hook those up.
      spell_to_obj(obj, &new_spell, TRUE);

      sendf(ch, "Spell %s added onto %s.\n\r", 
	    skill_table[vnum].name, obj->short_descr);
    }
/* DELETE */
  else if  (!str_prefix(cmd, "delete"))
    {
      //we know arg2 will be an item here so we check it.
      if (arg2[0] == '\0')
	{send_to_char("Need target item.\n\r", ch);return;}
      
      //check if item is on char.
      if ((obj = get_obj_here(ch, NULL, arg2)) == NULL)
	{
	  send_to_char("Target item must be carried, worn or in the room.\n\r", ch);
	  return;
	}
      //now we get the set command's destination spell.
      //check for valid spell.
      if ( ((vnum = skill_lookup(arg1)) == -1) || vnum > MAX_SKILL)
	{
	  send_to_char("That spell does not exist.\n\r", ch);
	  return;
	}
      if (skill_table[vnum].spell_fun == spell_null)
	{
	  send_to_char("That spell does not exist.\n\r", ch);
	  return;
	}
      //then we check if the said spell is on the object.
      if ( (spell = spell_obj_find(obj->spell, vnum)) == NULL)
	{
	  //we check index.
	  for (spell = obj->pIndexData->spell; spell != NULL; spell = spell->next)
	    if (spell->spell == vnum)
	      break;
	  if (spell == NULL)
	    {
	      sendf(ch, "%s was not found on %s.\n\r", skill_table[vnum].name, obj->short_descr);
	      return;
	    }//end validate spell
	}//end spell search.

      //now we just nuke the spell.
      spell_obj_strip(obj, vnum);
      sendf(ch, "Spell %s removed from %s.\n\r", skill_table[vnum].name, obj->short_descr);
    }//END DELETE

/* VIEW */
  else if (!str_prefix(cmd, "view"))
    {
      //arg1 is the item to view.
      if (arg1[0] == '\0')
	{send_to_char("Need target item.\n\r", ch);return;}

      //check if item is on char.
      if ((obj = get_obj_here(ch, NULL, arg1)) == NULL)
	{
	  send_to_char("Target item must be carried, worn or in the room.\n\r", ch);
	  return;
	}
      //display spells.
      sendf(ch, "%s has following spells:\n\r\n\r", obj->short_descr);
      //print indexed spells.
      if (!obj->eldritched)
	{
	  OBJ_SPELL_DATA* spell;
	  if ((spell = obj->pIndexData->spell) == NULL)
	    send_to_char("No Spells found.\n\r", ch);
	  else
	    for (spell = obj->pIndexData->spell; spell != NULL; spell = spell->next)
	      sendf(ch, "Name: %-15s Target: %-10s Freq: %-10d\n\rMessage"\
		    " See: %s\n\rMessage Blind: %s\n\r\n\r",
		    skill_table[spell->spell].name, 
		    (spell->target == SPELL_TAR_SELF ? "Self" : spell->target == SPELL_TAR_VICT ? "Victim" : "UNKNOWN"),
		    spell->percent,
		    (spell->message == NULL? "NULL": spell->message),
		    (spell->message2 == NULL? "NULL": spell->message2));
	}//end list indexed spells.
      else
	{
	  OBJ_SPELL_DATA* spell;
	  if ((spell = obj->spell) == NULL)
	    send_to_char("No spells found.\n\r", ch);
	  else
	    //list dynamic spells.
	    for (spell = obj->spell; spell != NULL; spell = spell->next)
	      sendf(ch, "Name: %-15s Target: %-10s Freq: %-10d\n\rMessage See"\
		    ": %s\n\rMessage Blind: %s\n\r\n\r",
		    skill_table[spell->spell].name, 
		    (spell->target == SPELL_TAR_SELF ? "Self" : spell->target == SPELL_TAR_VICT ? "Victim" : "UNKNOWN"),
		    spell->percent,
		    (spell->message == NULL? "NULL": spell->message),
		    (spell->message2 == NULL? "NULL": spell->message2));
	}//end show spelled spells.
    }//end view
/* SET */
  else if (!str_prefix(cmd, "set"))
    {
      //we know arg4 will be an item here so we check it.
      if (arg4[0] == '\0')
	{send_to_char("Need target item.\n\r", ch);return;}

      //check if item is on char.
      if ((obj = get_obj_here(ch, NULL, arg4)) == NULL)
	{
	  send_to_char("Target item must be carried, worn or in the room.\n\r", ch);
	  return;
	}
      //now we get the set command's destination spell.
      //check for valid spell.
      if ( ((vnum = skill_lookup(arg1)) == -1) || vnum > MAX_SKILL)
	{
	  send_to_char("That spell does not exist.\n\r", ch);
	  return;
	}
      if (skill_table[vnum].spell_fun == spell_null)
	{
	  send_to_char("That spell does not exist.\n\r", ch);
	  return;
	}
      //then we check if the said spell is on the object.
      if ( (spell = spell_obj_find(obj->spell, vnum)) == NULL)
	{
	  //we check index.
	    for (spell = obj->pIndexData->spell; spell != NULL; spell = spell->next)
	      if (spell->spell == vnum)
		break;
	    if (spell == NULL)
	      {
		sendf(ch, "%s was not found on %s.\n\r", skill_table[vnum].name, obj->short_descr);
		return;
	      }
	}//end spell search.
      //we check the second argument which is the set command.
/* SPELL */
      if (!str_prefix(arg2, "spell"))
	{
	  //check for valid spell.
	  if ( ((vnum = skill_lookup(arg3)) == -1) || vnum > MAX_SKILL)
	    {
	      send_to_char("That spell does not exist.\n\r", ch);
	      return;
	    }
	  if (skill_table[vnum].spell_fun == spell_null)
	    {
	      send_to_char("That spell does not exist.\n\r", ch);
	      return;
	    }
	  spell->spell = vnum;
	  //and if not eldritched we apply, as the function will eldritch for us.
	  spell_to_obj(obj, spell, FALSE);
	  sendf(ch, "Set spell to: %s added on %s.\n\r", skill_table[vnum].name, obj->short_descr);
	}//end spell.
 /* TARGET */
      else if (!str_prefix(arg2, "target"))
	{
	  if (arg3[0] == '\0')
	    {
	      send_to_char("You must specify target: Self/Victim.\n\r", ch);
	      return;
	    }
	  //choose wich target we set.
	  if (!str_prefix(arg3, "self"))
	    target = SPELL_TAR_SELF;
	  else if (!str_prefix(arg3, "victim"))
	    target = SPELL_TAR_VICT;
	  else
	    {send_to_char("Invalid target.\n\r", ch);return;}
	  //now we change the value
	  spell->target = target;
	  //and if not eldritched we apply, as the function will eldritch for us.
	  spell_to_obj(obj, spell, FALSE);
	  sendf(ch, "Target set to %s.\n\r", (target == SPELL_TAR_SELF? "Self" : "Vicimt"));
	}
 /* FREQUENCY */
      else if (!str_prefix(arg2, "frequency"))
	{
	  if ( ((fre = atoi(arg3)) < 1)  || fre > 100)
	    {
	      send_to_char("You must specify frequency of the spell in integers (1 - 100).\n\r", ch);
	      return;
	    }
	  sendf(ch, "Frequency changed from %d to %d on %s.\n\r",spell->percent, fre, obj->short_descr);
	  spell->percent = fre;
//and if not eldritched we apply, as the function will eldritch for us.
	  spell_to_obj(obj, spell, FALSE);
	}//end frequency.
 /* SeeMessage */
      else if (!str_prefix(arg2, "seemessage"))
	{
	  if (arg3[0] == '\0')
	    {
	      send_to_char("To set an empty message use <none>.\n\r", ch);
	      return;
	    }
	  //we free the string.
	  if (spell->message != NULL)
	    free_string(spell->message);
	  //check for none.
	  if (!str_prefix(arg2, "none"))
	    spell->message = &str_empty[0];
	  else
	    spell->message = str_dup(arg3);
	  sendf(ch, "Message show when able to see:\n\r%s\n\r", spell->message);
	}//end selfstring

 /* BLINDMESSAGE */
      else if (!str_prefix(arg2, "blindmessage"))
	{
	  if (arg3[0] == '\0')
	    {
	      send_to_char("To set an empty message use <none>.\n\r", ch);
	      return;
	    }
	  //we free the string.
	  if (spell->message2 != NULL)
	    free_string(spell->message2);
	  //check for none.
	  if (!str_prefix(arg2, "none"))
	    spell->message2 = &str_empty[0];
	  else
	    spell->message2 = str_dup(arg3);
	  sendf(ch, "Message show when blind to see:\n\r%s\n\r", spell->message2);
	}//end selfstring
      else
	do_ospell(ch, "");
    }//END SET
      else
	do_ospell(ch, "");
return;
}

/* clear_owner */
/* RETURNS TRUE on sucess. */
/* By Viri */
inline bool clear_owner(OBJ_DATA* obj)
{
  AFFECT_DATA* paf;

  //check for error.
  if (obj == NULL)
    {
      bug("set_obj: Null argumetn passed.", 0);
      return FALSE;
    }

  //check if alredy affected.
  if ( (paf = affect_find(obj->affected, gen_has_owner)) == NULL)
      return TRUE;
  else
    affect_remove_obj(obj, paf);
  return TRUE;
}

/* set_owner */
/* RETURNS TRUE on sucess. */
/* By Viri */
inline bool set_owner(CHAR_DATA* ch, OBJ_DATA* obj, char* deity)
{
  /* this is just a small function that set the owner of obj. */
  /* if the deity is null or not found the modifier is set to -1,
else its the deity intable by possition.
  */
  AFFECT_DATA* paf;
  AFFECT_DATA af;

  //check for error.
  if (ch == NULL || obj == NULL)
    {
      bug("set_obj: Null argumetn passed.", 0);
      return FALSE;
    }

  //check if alredy affected.
  if ( (paf = affect_find(obj->affected, gen_has_owner)) != NULL)
    {
      string_to_affect(paf, ch->name);
      paf->duration = -1;
      return TRUE;
    }
  else
    {
      af.type = gen_has_owner;
      af.where = TO_NONE;
      af.location = APPLY_NONE;
      af.level = 0;
      af.duration = -1;
      if (deity == NULL)
	af.modifier = -1;
      else
	af.modifier = deity_lookup(deity);
      af.bitvector = 0;
      affect_to_obj(obj, &af);
      paf = affect_find(obj->affected, gen_has_owner);
      string_to_affect(paf, ch->name);
      return TRUE;
    }
  return FALSE;
}//end set_owner

/* prints all no-exits in the area. */
void do_noxlist(CHAR_DATA *ch, char *argument)
{
  AREA_DATA* pArea = ch->in_room->area;
  ROOM_INDEX_DATA* room;
  char buf[MIL];
  char out[MSL];
  int i = 0;
  int j = 0;

  sprintf(out, "Area: [%d] %s, (%d - %d)\n\rhas following no exits:\n\r\n\r",
	pArea->vnum, pArea->name,pArea->min_vnum, pArea->max_vnum);
  sprintf(buf, "NONE FOUND\n\r");
  for (i = 0; i < MAX_KEY_HASH; i++)
    for (room = room_index_hash[i]; room != NULL; room = room->next)
      {
	bool has_exit = FALSE;
	if (room == NULL)
	  continue;
	if (room->area->vnum != pArea->vnum)
	  continue;

	for(j = 0; j < 6; j++)
	  if (room->exit[j] != NULL)
	    has_exit = TRUE;
	if (!has_exit)
	  {
	    sprintf(buf, "[ %d ] %-10s\n\r", room->vnum, room->name);
	    strcat(out, buf);
	  }
	
      }

   send_to_char(out, ch);
} 

void do_appdesc(CHAR_DATA *ch, char *argument)
{
  /* All that is done here is the desc. flag is cleared
- If char is less then lvl 15 he gets bonus
- Penatly can be forced with appdesc Pen
- Bonus  can be forced with appdesc Bon
  */

  CHAR_DATA* vch;

  char arg1[MIL];
  char arg2[MIL];
  char buf1[MIL];
  char buf2[MIL];

  const int bonus_lvl = 15;
  const int bonus_hp = 5;
  const int bonus_mn = 5;
  const int bonus_mv = 5;
  const int bonus_exp = 2000;

  bool fBonus = FALSE;
  bool fPenal = FALSE;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  //check for target.
  if (arg1[0] == '\0')
    {
      send_to_char("appdesc:  (Approve Description)\n\r"\
	     "syntax: appdesc <target> [bonus/penalty/none]\n\r\n\r"\
	     "Approves the description of the player, and automaticly\n\r"\
	     "grants a bonus if below level 15.\n\r"\
	     "Bonus or penalty or nothing can be forced with the second argument.\n\r", ch);
      return;
    }

  //get target
  if ( ( vch = get_char_world(ch, arg1 ) ) == NULL )
    {
      send_to_char("Target player not found.\n\r", ch);
      return;
    }

  //not npc
  if (IS_NPC(vch))
    {
      send_to_char("Players only.\n\r", ch);
      return;
    }

  if (!IS_SET(vch->act2, PLR_DESC))
    {
      act("$N has already had his description approved.", ch, NULL, vch, TO_CHAR);
      return;
    }

  //check for automatic bonus.
  if (vch->level <= bonus_lvl)
    fBonus = TRUE;

  //check for force
  if (arg2[0] != '\0')
    {
      if (!str_prefix(arg2, "bonus"))
	{fBonus = TRUE;fPenal = FALSE;}
      else if (!str_prefix(arg2, "penalty")) 
	{fPenal = TRUE;fBonus = FALSE;}
      else if (!str_prefix(arg2, "none")) 
	{fPenal = FALSE;fBonus = FALSE;}
    }

  sprintf(buf1, "You have approved $N's description");
  sprintf(buf2, "%s has approved %s's description", ch->name, vch->name);
  //now we perform the neccesary actions.

  REMOVE_BIT(vch->act2, PLR_DESC);

  if (fBonus)
    {
      act_new("$G has judged your look worthy and grants you a blessing.\n\r"\
	  "You permamently gain some hitpoints, mana and movement!",
	  ch, NULL, vch, TO_VICT, POS_DEAD);
      sendf(vch, "You have been enlighted with %d experience!\n\r", bonus_exp);

      strcat(buf1, " with a bonus");
      strcat(buf2, " with a bonus");

      vch->pcdata->perm_hit += bonus_hp; vch->max_hit += bonus_hp;
      vch->hit += bonus_hp;
      vch->pcdata->perm_mana += bonus_mn; vch->max_mana += bonus_mn;
      vch->mana += bonus_mn;
      vch->pcdata->perm_move += bonus_mv; vch->max_move += bonus_mv;
      vch->move += bonus_mv;
      gain_exp(vch, bonus_exp);
    }
  else if (fPenal)
    {
      act_new("Your poor description has angrered $G!.\n\r"\
	  "You permamently lose some hitpoints, mana and movement!",
	  ch, NULL, vch, TO_VICT, POS_DEAD);

      strcat(buf1, " with a penalty");
      strcat(buf2, " with a bonus");

      vch->pcdata->perm_hit = UMAX(1, vch->pcdata->perm_hit - bonus_hp);
      vch->max_hit = UMAX(1, vch->max_hit - bonus_hp);

      vch->pcdata->perm_mana = UMAX(1, vch->pcdata->perm_mana - bonus_mn);
      vch->max_mana = UMAX(1, vch->max_mana - bonus_mn);

      vch->pcdata->perm_move = UMAX(1, vch->pcdata->perm_move- bonus_mv);
      vch->max_move = UMAX(1, vch->max_move- bonus_mv);
      update_pos(vch);
    }
  else
    act_new("$g has judged your look worthy.", vch, NULL, NULL, TO_CHAR, POS_DEAD);

  strcat(buf1, ".");
  strcat(buf2, ".");

  act_new(buf1, ch, NULL, vch, TO_CHAR, POS_DEAD);
  wiznet(buf2, ch, NULL,WIZ_PENALTIES,WIZ_SECURE, LEVEL_HERO);
  log_event(ch, buf2);

  return;
}


// this file checks if a name is OK.
//   by OK we mean it is:
//    a) not on the illegal name list

int name_ok (char *name) {

  char strsave[MIL], namelist[MSL], nameread[MSL];
  FILE *fp;

  // if player is on illegal name list, return FALSE
  fclose( fpReserve );
  sprintf( strsave, "%s", ILLEGAL_NAME_FILE );
  sprintf(namelist,"%s","");
  if ((fp = fopen (strsave, "r" )) != NULL) {
    for ( ; ; ) {
      fscanf (fp, "%s", nameread);
      if (!str_cmp (nameread, "END" )) { 
	  break;
      }
      else {
	if (!str_cmp (nameread, name)) {
	  return FALSE;
	}
      }
    }
  }
  else {
    fp = fopen( NULL_FILE, "r" );
  }
  fclose (fp);
  fpReserve = fopen( NULL_FILE, "r" );

  // else return TRUE
  return TRUE;
}

/* sets owner only on an item (same as set obj <item> owner */
void do_oowner(CHAR_DATA *ch, char *argument){
  char arg[MIL];
  OBJ_DATA* obj;
  CHAR_DATA* vch;


  if (IS_NULLSTR(argument)){
    send_to_char("syntax: owner <obj> <character>"\
		 "\n\rUse \"owner <obj> none\" to clear owner\n\r", ch);
    return;
  }
  argument = one_argument(argument, arg);

  /* get object */
  if ((obj = get_obj_here(ch, NULL, arg)) == NULL)
    {
      send_to_char("Target item must be carried, worn or in the room.\n\r", ch);
      return;
    }

  /*CLEAR OWNER*/
  if (!str_prefix("none", argument)){
    if (!clear_owner(obj))
      send_to_char("Error clearing owner.", ch);
    return;
  }
  /*SET OWNER */
  if ( (vch = get_char(argument)) == NULL){
    send_to_char("That character is not in the realms.\n\r", ch);
    return;
  }
  if (!set_owner(vch, obj, NULL))
    send_to_char("Error setting owner.\n\r", ch);
  return;
}//end OWNER


/* approves an application sent to char.
approve <"application"/number> [message]
message is the text to be put onto first line of the note sent to the mortal
*/
void do_approve( CHAR_DATA *ch, char *argument ){
  char arg1[MIL];
  char buf[MIL];

  int app = -1;
  int vnum = 0;

  NOTE_DATA *pnote, *pfrom = NULL;
  NOTE_DATA** list = &application_list;
  BUFFER *buffer;
  time_t last_read;

  if (IS_NPC(ch))
    return;
  else
    last_read = ch->pcdata->last_application;

  if (IS_NULLSTR(argument)){
    send_to_char("Syntax:\n\rapprove <application/number> [message]\n\r"\
		 "approve application/tapproves last app read.\n\r"\
		 "approve <number> /t approves particular app\n\r"\
		 "[messagge] is optional line of text to be included.\n\r", ch);
    return;
  }
  argument = one_argument(argument, arg1);
  if (str_prefix(arg1, "application")){
    if ( (app = atoi(arg1)) < 0){
      send_to_char("\"approve <number>\" OR \"approve app\"\n\r", ch);
      return;
    }
  }

  /* now we need the title, and original sender of app selected */
  for ( pnote = *list; pnote != NULL; pnote = pnote->next ){
    if (!is_note_to(ch, pnote))
      continue;
    if (app < 0){
      if (pnote->next == NULL
	  ||  pnote->next->date_stamp > last_read){
	pfrom = pnote;
	break;
      }
    }
    else if (vnum++ == app ){
      pfrom = pnote;
      break;
    }
  }
  if (pfrom == NULL){
    send_to_char("No such note found\n\r", ch);
    return;
  }

/* check for quest approval */
  if (is_name("crusader", pfrom->subject) && is_auto_name("app", pfrom->subject)){
    quest_approve(ch, pfrom->sender, QUEST_CRUS);
  }
  if (is_auto_name("psi", pfrom->subject) && is_auto_name("app", pfrom->subject)){
    quest_approve(ch, pfrom->sender, QUEST_PSI);
  }
  else if (is_auto_name("undead", pfrom->subject) && is_auto_name("app", pfrom->subject)){
    quest_approve(ch, pfrom->sender, QUEST_UNDE);
  }
  else if (is_auto_name("vampire", pfrom->subject) && is_auto_name("app", pfrom->subject)){
    quest_approve(ch, pfrom->sender, QUEST_VAMP);
  }

  /* now we make the note back */
  buffer = new_buf();
  add_buf(buffer, "Your application has been `@approved``.\n\r");
  if (!IS_NULLSTR(argument)){
    add_buf(buffer, argument);
    add_buf(buffer, "\n\r");
  }
  sprintf( buf, "%s has approved your %s.", ch->name, pfrom->subject);
  do_hal( pfrom->sender, buf, buffer->string, NOTE_NOTE);
  free_buf(buffer);

  sprintf( buf, "%s has approved %s's %s.", ch->name, pfrom->sender, pfrom->subject);
  sprintf( arg1, "%s %s", ch->name, "immortal" );
  do_hal( arg1, buf , pfrom->text, NOTE_PENALTY);

  /* remove the old app */
  sendf(ch, "You've approved %s's application.\n\r", pfrom->sender);
  note_remove( ch, pfrom, TRUE );
}

/* rejects an application sent to char.
rejects <"application"/number> [message]
message is the text to be put onto first line of the note sent to the mortal
*/
void do_reject( CHAR_DATA *ch, char *argument ){
  char arg1[MIL];
  char buf[MIL];

  int app = -1;
  int vnum = 0;

  NOTE_DATA *pnote, *pfrom = NULL;
  NOTE_DATA** list = &application_list;
  time_t last_read;
  BUFFER *buffer;


  if (IS_NPC(ch))
    return;
  else
    last_read = ch->pcdata->last_application;

  if (IS_NULLSTR(argument)){
    send_to_char("Syntax:\n\rreject <application/number> [message]\n\r"\
		 "reject application/t rejects last app read.\n\r"\
		 "reject <number> /t rejects particular app\n\r"\
		 "[messagge] is optional line of text to be included.\n\r", ch);
    return;
  }
  argument = one_argument(argument, arg1);
  if (str_prefix(arg1, "application")){
    if ( (app = atoi(arg1)) < 0){
      send_to_char("\"reject <number>\" OR \"reject app\"\n\r", ch);
      return;
    }
  }

  /* now we need the title, and original sender of app selected */
  for ( pnote = *list; pnote != NULL; pnote = pnote->next ){
    if (!is_note_to(ch, pnote))
      continue;
    if (app < 0){
      if (pnote->next == NULL
	  ||  pnote->next->date_stamp > last_read){
	pfrom = pnote;
	break;
      }
    }
    else if (vnum++ == app ){
      pfrom = pnote;
      break;
    }
  }
  if (pfrom == NULL){
    send_to_char("No such note found\n\r", ch);
    return;
  }

  /* now we make the note back */
  buffer = new_buf();
  add_buf(buffer, "Your application has been `!rejected``.\n\r");
  if (!IS_NULLSTR(argument)){
    add_buf(buffer, argument);
    add_buf(buffer, "\n\r");
  }

  sprintf( buf, "%s has rejected your %s.", ch->name, pfrom->subject);
  do_hal( pfrom->sender, buf, buffer->string, NOTE_NOTE);
  free_buf(buffer);

  sprintf( buf, "%s has rejected %s's %s.", ch->name, pfrom->sender, pfrom->subject);
  sprintf( arg1, "%s %s", ch->name, "immortal" );
  do_hal( arg1, buf ,pfrom->text, NOTE_PENALTY);

  /* remove the old app */
  sendf(ch, "You've rejected %s's application.\n\r", pfrom->sender);
  note_remove( ch, pfrom, TRUE );
}
// 03-28 23:00 Viri: Created do_broadcast().
// 04-10 20:00 Ath: Added do_coding
// 04-13 12:00 Viri: Added do_quest_elder
//04-13 13:00 Viri: Added do_masquerade
//04-28-00 Virir: Modified Advself 
//05-08 Ath: added do_most
//05-10-00 Viri: Added ospell.
//05-12 Ath: Cleaned up a bit, moved more stuff into do_set
//05-12 Viri: Added set_owner to set.
//05-13-00 Viri: Added clear_owner;
//06-01-00 Viri:Added noexlist
//07-05-00 Viri: Added appdesc
//2001-02-18 Ath: Added turning autocabal ON when inducting (except conclave)
//01-08-18: Viri added do_oowner
//01-09-18: Viri added do_approve/reject
