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
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define _XOPEN_SOURCE
#include <unistd.h>
#include <crypt.h>
#include <time.h>
#include <ctype.h>
#include "merc.h"
#include "cabal.h"
#include "recycle.h"
#include "interp.h"
#include "magic.h"
#include "bounty.h"
#include "tome.h"
#include "misc.h"
#include "comm.h"
#include "clan.h"

extern		int              nAllocString;
extern		int              sAllocString;
int                     nAllocPerm;
int                     sAllocPerm;

extern void save_mud();
extern void read_mud();
extern void save_helps();
extern void make_htmlwho(bool fShow);
extern void seed_objects();
extern AREA_DATA*  get_random_area   args ( (int min_av, int max_av, 
					     int min_rv, int max_rv, 
					     int* includ, int i_num,
					     int* exclud, int e_num) );

/* checks if the demon can broadcast the message */
/* returns TRUE if can. */
bool can_demon_talk(CHAR_DATA* ch, int cost)
{
  if(IS_NPC(ch))
    return TRUE;

  if (!IS_TELEPATH(ch))
    return FALSE;

  if (IS_IMMORTAL(ch))
    return TRUE;

//Forget
  if (is_affected(ch, gsn_forget))
    {
      send_to_char("You have forgotten how to communicate through telepathy.\n\r", ch);
      return FALSE;
    }

//Telelock
  if (is_affected(ch, gsn_tele_lock))
    {
      send_to_char("Your efforts seem to be blocked and you fail.\n\r", ch);
      return FALSE;
    }

//Limbo
  if (IS_SET(ch->in_room->room_flags, ROOM_DARK_RIFT))
    {
      send_to_char("You are alone.. So alone..\n\r", ch);
      return FALSE;
    }

//Cost
  if (ch->mana < cost)
    {
      send_to_char("You lack the strength to broadcast your message.\n\r", ch);
      return FALSE;
    }
  else
    ch->mana -= cost;


  return TRUE;
}


void do_delet( CHAR_DATA *ch, char *argument)
{
    send_to_char("You must type the full command to delete yourself.\n\r",ch);
}

void do_delete( CHAR_DATA *ch, char *argument)
{
    char strsave[MIL];
    if (IS_NPC(ch))
        return;
    if (ch->pcdata->confirm_delete)
    {
	if (argument[0] != '\0')
        {
            send_to_char("Delete status removed.\n\r",ch);
	    ch->pcdata->confirm_delete = FALSE;
	    return;
	}
/* start deleting */
	sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	wiznet("$N has deleted $Mself.",ch,NULL,0,0,0);
	stop_fighting(ch,TRUE);
	eq_purge(ch);
	if (ch->level > 20)
	  deny_record(ch);
	act("$n has left permanently, never to return.",ch,NULL,NULL,TO_ROOM);

	if (ch->pCabal){
	  update_cabal_skills(ch, ch->pCabal, TRUE, TRUE);
	  char_from_cabal( ch );    
	}  
	if (HAS_CLAN(ch))
	  CharFromClan( ch );
	if (ch->pcdata->pbounty){
	  rem_bounty( ch->pcdata->pbounty );
	  free_bounty( ch->pcdata->pbounty );      
	}

	extract_char( ch, TRUE );
	if ( ch->desc != NULL )
	  close_socket( ch->desc );  
	unlink(strsave); 
	return;
    }
    /* check for password */
    if (!IS_NULLSTR(ch->pcdata->pwddel)){
      if (IS_NULLSTR(argument)){
	send_to_char("delete <password>\n\r", ch);
	return;
      }
      if (strcmp( crypt( argument, ch->name ), ch->pcdata->pwddel )
	  && strcmp( crypt( argument, ch->pcdata->pwddel ), ch->pcdata->pwddel ) )
	{
	  WAIT_STATE( ch, 40 );
	  send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	  return;
	}
    }

    
    if (is_fight_delay(ch,120)){
      send_to_char("At request of players this command cannot be used after recent combat or death.\n\r", ch);
      ch->pcdata->confirm_delete = FALSE;
      return;
    }
    send_to_char("Type delete again to confirm this command.\n\r",ch);
    send_to_char("WARNING: this command is irreversible.\n\r",ch);
    send_to_char("Typing delete with an argument will undo delete status.\n\r",ch);
    ch->pcdata->confirm_delete = TRUE;
    wiznet("$N is contemplating deletion.",ch,NULL,0,0,get_trust(ch));
}

void do_channels( CHAR_DATA *ch, char *argument)
{
    send_to_char("   channel     status\n\r",ch);
    send_to_char("---------------------\n\r",ch);
    send_to_char("auction        ",ch);
    if (!IS_SET(ch->comm,COMM_NOAUCTION)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);
/*
    send_to_char("Q/A            ",ch);
    if (!IS_SET(ch->comm,COMM_NOQUESTION)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);
*/

    send_to_char("cabal          ",ch);
    if (!IS_SET(ch->comm,COMM_NOCABAL)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);

    if (IS_IMMORTAL(ch))
    {
        send_to_char("god channel    ",ch);
        if(!IS_SET(ch->comm,COMM_NOWIZ)) send_to_char("ON\n\r",ch);
        else send_to_char("OFF\n\r",ch);

        send_to_char("pray           ",ch);
        if (!IS_SET(ch->comm,COMM_NOPRAY)) send_to_char("ON\n\r",ch);
        else send_to_char("OFF\n\r",ch);
    }

    if (!is_pk(ch,ch) || IS_IMMORTAL(ch) )
    {
	send_to_char("newbie chat    ",ch);
	if (!IS_SET(ch->comm,COMM_NONEWBIE)) send_to_char("ON\n\r",ch);
	else send_to_char("OFF\n\r",ch);
    }

    if ( IS_IMMORTAL(ch) )
    {
        send_to_char("hear all       ",ch);
        if (IS_SET(ch->act,PLR_HEARALL)) send_to_char("ON\n\r",ch);
        else send_to_char("OFF\n\r",ch);
    }

    send_to_char("yells          ",ch);
    if (!IS_SET(ch->comm,COMM_YELLSOFF)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);

    send_to_char("tells          ",ch);
    if (!IS_SET(ch->comm,COMM_DEAF)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);

    send_to_char("quiet mode     ",ch);
    if (IS_SET(ch->comm,COMM_QUIET)) send_to_char("ON\n\r",ch);
    else send_to_char("OFF\n\r",ch);

    if (IS_SET(ch->comm,COMM_AFK)) send_to_char("You are AFK.\n\r",ch);
    if (IS_SET(ch->comm,COMM_SNOOP_PROOF)) send_to_char("You are immune to snooping.\n\r",ch);

    if (ch->lines != PAGELEN)
    {
	if (ch->lines)
	    sendf(ch,"You display %d lines of scroll.\n\r",ch->lines+2);
	else
	    send_to_char("Scroll buffering is off.\n\r",ch);
    }

    if (ch->prompt != NULL)
	sendf(ch,"Your current prompt is: %s\n\r",ch->prompt);

    if (IS_SET(ch->comm,COMM_NOYELL)) send_to_char("You cannot yell.\n\r",ch);
    if (IS_SET(ch->comm,COMM_NOTELL)) send_to_char("You cannot use tell.\n\r",ch);
    if (IS_SET(ch->comm,COMM_NOCHANNELS)) send_to_char("You cannot use channels.\n\r",ch);
    if (IS_SET(ch->comm,COMM_NOEMOTE)) send_to_char("You cannot show emotions.\n\r",ch);
    if (IS_SET(ch->act,PLR_DOOF)) send_to_char("You have lost the favor of the gods.\n\r",ch);
}

void do_deaf( CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_DEAF))
    {
        send_to_char("You can now hear tells again.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_DEAF);
    }
    else
    {
        send_to_char("From now on, you won't hear tells.\n\r",ch);
        SET_BIT(ch->comm,COMM_DEAF);
    }
}

void do_quiet ( CHAR_DATA *ch, char * argument)
{
    if (IS_SET(ch->comm,COMM_QUIET))
    {
        send_to_char("Quiet mode removed.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_QUIET);
    }
    else
    {
        send_to_char("From now on, you will only hear says and emotes.\n\r",ch);
        SET_BIT(ch->comm,COMM_QUIET);
    }
}

void do_afk ( CHAR_DATA *ch, char * argument)
{
    if (IS_SET(ch->comm,COMM_AFK))
    {
      
        if (buf_string(ch->pcdata->buffer)[0] == '\0')
            send_to_char("AFK mode removed.\n\r",ch);
	else
            send_to_char("AFK mode removed. Type 'replay' to see tells.\n\r",ch);
	act("$n is no longer AFK.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->comm,COMM_AFK);
    }
    else
    {
        send_to_char("You are now in AFK mode.\n\r",ch);
	act("$n is now AFK.",ch,NULL,NULL,TO_ROOM);
	clear_buf(ch->pcdata->buffer);
        SET_BIT(ch->comm,COMM_AFK);
    }
}

void do_replay (CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
	send_to_char("You can't replay.\n\r",ch);
	return;
    }

    if (buf_string(ch->pcdata->buffer)[0] == '\0')
    {
	send_to_char("You have no tells to replay.\n\r",ch);
	return;
    }
    page_to_char(buf_string(ch->pcdata->buffer),ch);
    clear_buf(ch->pcdata->buffer);
}


void do_auction( CHAR_DATA *ch, char *argument )
{
  char buf[MIL];

    DESCRIPTOR_DATA *d;
    if (argument[0] == '\0' )
    {
        if (IS_SET(ch->comm,COMM_NOAUCTION))
        {
            send_to_char("Auction channel is now ON.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_NOAUCTION);
        }
        else
        {
            send_to_char("Auction channel is now OFF.\n\r",ch);
            SET_BIT(ch->comm,COMM_NOAUCTION);
        }
	return;
    }
    strcpy(buf, argument);
    if (IS_SET(ch->comm,COMM_QUIET))
    {
        send_to_char("You must turn off quiet mode first.\n\r",ch);
        return;
    }
    if ( ch->level < 2 )
    {
	send_to_char( "Sorry, not right now due to spammers.\n\r",ch);
	return;
    }
    if (IS_SET(ch->comm,COMM_NOCHANNELS))
    {
        send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
        return;
    }
    if (IS_AFFECTED2(ch, AFF_SILENCE) )
    {
        send_to_char("You work your mouth, but no sounds escape your lips.\n\r",ch);
        return;
    }
    if (is_affected(ch, gsn_gag) )
    {
        send_to_char("You find difficultly talking.\n\r",ch);
        return;
    }
    if (is_affected(ch, gsn_uppercut) )
    {
        send_to_char("Your jaws hurt too much to talk.\n\r",ch);
        return;
    }
    REMOVE_BIT(ch->comm,COMM_NOAUCTION);
    makedrunk(argument,ch);
    sendf( ch, "You auction '`6%s``'\n\r", argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && d->character != ch && d->character->in_room != NULL
        && (d->character->in_room->area == ch->in_room->area || IS_SET(d->character->act,PLR_HEARALL))
        && !IS_SET(d->character->comm,COMM_NOAUCTION) && !IS_SET(d->character->comm,COMM_QUIET)
	&& !is_affected(d->character,gsn_silence) && !IS_AFFECTED2(d->character, AFF_SILENCE) && !is_ignore(d->character,ch))
	  act_new("$n auctions '`6$t``'", ch,
		  (is_affected(d->character, gsn_com_lan) ? buf: IS_IMMORTAL(d->character) ? buf : argument),
		  d->character,TO_VICT,POS_SLEEPING);
}

void do_pray( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char arg_buf[MIL];
    if ( (argument[0] == '\0' ) && IS_IMMORTAL(ch) )
    {
        if (IS_SET(ch->comm,COMM_NOPRAY))
        {
            send_to_char("Pray channel is now ON.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_NOPRAY);
        }
        else
        {
            send_to_char("Pray channel is now OFF.\n\r",ch);
            SET_BIT(ch->comm,COMM_NOPRAY);
        }
	return;
    }
    strcpy(arg_buf, argument);
    if (IS_SET(ch->comm,COMM_QUIET))
    {
        send_to_char("You must turn off quiet mode first.\n\r",ch);
        return;
    }

    if (IS_SET(ch->comm,COMM_NOCHANNELS))
    {
        send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
        return;
    }

    if (IS_SET(ch->act,PLR_DOOF))
    {
        send_to_char("You pray, but nobody seems to care.\n\r",ch);
        return;
    }
    REMOVE_BIT(ch->comm,COMM_NOPRAY);
    sendf( ch, "You pray '`&%s``'\n\r", argument );

    if (argument[0] == '\0')
        return;

    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && d->character != ch && IS_IMMORTAL(d->character)
	&& !IS_SET(d->character->comm,COMM_NOPRAY) && !IS_SET(d->character->comm,COMM_QUIET) && !is_ignore(d->character,ch))
            act_new( "$n prays '`&$t``'", ch,
		     (is_affected(d->character, gsn_com_lan) ? arg_buf: IS_IMMORTAL(d->character) ? arg_buf : argument),
		     d->character, TO_VICT,POS_SLEEPING );
}

void do_question( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char arg_buf[MIL];
    if (argument[0] == '\0' )
    {
        if (IS_SET(ch->comm,COMM_NOQUESTION))
        {
            send_to_char("Q/A channel is now ON.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_NOQUESTION);
        }
        else
        {
            send_to_char("Q/A channel is now OFF.\n\r",ch);
            SET_BIT(ch->comm,COMM_NOQUESTION);
        }
        return;
    }
    strcpy(arg_buf, argument);
    if (IS_SET(ch->comm,COMM_QUIET))
    {
        send_to_char("You must turn off quiet mode first.\n\r",ch);
        return;
    }
    if ( ch->level < 2 )
    {
	send_to_char( "Sorry, have to level before you can yell due to spammers.\n\r",ch);
	return;
    }
    if (IS_SET(ch->comm,COMM_NOCHANNELS))
    {
        send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
        return;
    }
    if (IS_AFFECTED2(ch, AFF_SILENCE) )
    {
        send_to_char("You work your mouth, but no sounds escape your lips.\n\r",ch);
        return;
    }
    if (is_affected(ch, gsn_gag) )
    {
        send_to_char("You find difficultly talking.\n\r",ch);
        return;
    }
    if (is_affected(ch, gsn_uppercut) )
    {
        send_to_char("Your jaws hurt too much to talk.\n\r",ch);
        return;
    }
    REMOVE_BIT(ch->comm,COMM_NOQUESTION);
    makedrunk(argument,ch);
    sendf( ch, "You question '`6%s``'\n\r", argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && d->character != ch && d->character->in_room != NULL
        && (d->character->in_room->area == ch->in_room->area || IS_SET(d->character->act,PLR_HEARALL))
	&& !IS_AFFECTED2(d->character, AFF_SILENCE) && !IS_SET(d->character->comm,COMM_NOQUESTION)
	&& !is_affected(d->character,gsn_silence) && !IS_SET(d->character->comm,COMM_QUIET) && !is_ignore(d->character,ch))
            act_new("$n questions '`6$t``'", ch, (is_affected(d->character, gsn_com_lan) ? arg_buf: IS_IMMORTAL(d->character) ? arg_buf : argument),
		    d->character,TO_VICT,POS_SLEEPING);
}

void do_answer( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char arg_buf[MIL];
    strcpy(arg_buf, argument);
    if (argument[0] == '\0' )
    {
        if (IS_SET(ch->comm,COMM_NOQUESTION))
        {
            send_to_char("Q/A channel is now ON.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_NOQUESTION);
        }
        else
        {
            send_to_char("Q/A channel is now OFF.\n\r",ch);
            SET_BIT(ch->comm,COMM_NOQUESTION);
        }
	return;
    }
   
    strcpy(arg_buf, argument);
    if (IS_SET(ch->comm,COMM_QUIET))
    {
        send_to_char("You must turn off quiet mode first.\n\r",ch);
        return;
    }
    if ( ch->level < 2 )
    {
	send_to_char( "Sorry, have to level before you can yell due to spammers.\n\r",ch);
	return;
    }
    if (IS_SET(ch->comm,COMM_NOCHANNELS))
    {
        send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
        return;
    }
    if (IS_AFFECTED2(ch, AFF_SILENCE) )
    {
        send_to_char("You work your mouth, but no sounds escape your lips.\n\r",ch);
        return;
    }
    if (is_affected(ch, gsn_gag) )
    {
        send_to_char("You find difficultly talking.\n\r",ch);
        return;
    }
    if (is_affected(ch, gsn_uppercut) )
    {
        send_to_char("Your jaws hurt too much to talk.\n\r",ch);
        return;
    }
    REMOVE_BIT(ch->comm,COMM_NOQUESTION);
    makedrunk(argument,ch);
    sendf( ch, "You answer '`6%s``'\n\r", argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && d->character != ch && d->character->in_room != NULL
        && (d->character->in_room->area == ch->in_room->area || IS_SET(d->character->act,PLR_HEARALL))
	&& !IS_AFFECTED2(d->character, AFF_SILENCE) && !IS_SET(d->character->comm,COMM_NOQUESTION)
	&& !is_affected(d->character,gsn_silence) && !IS_SET(d->character->comm,COMM_QUIET) && !is_ignore(d->character,ch))
            act_new("$n answers '`6$t``'", ch,(is_affected(d->character, gsn_com_lan)? arg_buf : IS_IMMORTAL(d->character) ? arg_buf : argument),
		    d->character,TO_VICT,POS_SLEEPING);
}

void do_cabaltalk( CHAR_DATA *ch, char *argument )
{
    char buf[MSL], buf2[MSL] = "";
    char arg_buf[MIL];
    
    DESCRIPTOR_DATA *d;
    const int demon_cost = 80;
    bool fDemon = IS_TELEPATH(ch);

    argument[0] = UPPER(argument[0]);

    if (!ch->pCabal){
      send_to_char("You aren't in a cabal.\n\r",ch);
      return;
    }
    else if ( argument[0] == '\0' ){
      if (IS_SET(ch->comm,COMM_NOCABAL)){
	send_to_char("Cabal channel is now ON\n\r",ch);
	REMOVE_BIT(ch->comm,COMM_NOCABAL);
      }
      else{
	send_to_char("Cabal channel is now OFF\n\r",ch);
	SET_BIT(ch->comm,COMM_NOCABAL);
      }
      return;
    }
    
    if (!IS_NPC(ch) && !IS_IMMORTAL(ch) && !IS_CABAL(get_parent(ch->pCabal), CABAL_CHAT)){
      send_to_char("Your cabal does not posses this ability.\n\r", ch);
      return;
    }

    strcpy(arg_buf, argument);
    if (!IS_NPC(ch) && IS_SET(ch->comm,COMM_NOCHANNELS)){
      send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
      return;
    }
    else if (IS_AFFECTED2(ch, AFF_SILENCE) && !fDemon){
      send_to_char("You work your mouth, but no sounds escape your lips.\n\r",ch);
      return;
    }
    else if (!fDemon && is_affected(ch, gsn_gag)){
      send_to_char("You find difficultly talking.\n\r",ch);
      return;
    }
    else if (!fDemon && is_affected(ch, gsn_uppercut)){
      send_to_char("Your jaws hurt too much to talk.\n\r",ch);
      return;
    }
    else if (fDemon && !can_demon_talk(ch, demon_cost))
      return;

    REMOVE_BIT(ch->comm,COMM_NOCABAL);
    makedrunk(argument,ch);

    if (!IS_NPC(ch) ){
      sprintf(buf2, "(%s) ", get_crank( ch ) );
    }
    sprintf(buf,"[%s] %s$n: '`0$t``'", ch->pCabal->who_name, buf2);
    act_new(buf,ch,argument,NULL,TO_CHAR,POS_DEAD);
    
    for ( d = descriptor_list; d != NULL; d = d->next ){
      CHAR_DATA* rch = d->character;
      if ( d->connected != CON_PLAYING 
	   || rch == ch 
	   || rch->pCabal == NULL
	   || (IS_SET(rch->comm, COMM_NOCABAL) && !IS_NPC(ch))
	   || IS_AFFECTED2(rch, AFF_SILENCE)
	   || IS_SET(rch->comm, COMM_QUIET) 
	   || !is_same_cabal(rch->pCabal, ch->pCabal)
	   || is_ignore(rch, ch)){
	continue;
      }
      act_new(buf, ch, 
	      (is_affected(rch, gsn_com_lan)? arg_buf : 
	       IS_IMMORTAL(rch) ? arg_buf :argument),
	      rch, TO_VICT, POS_DEAD);
      if (!IS_NPC(rch) && rch->pcdata->eavesdropped != NULL
	  && is_affected(rch->pcdata->eavesdropped,gsn_eavesdrop)
	  && rch != ch && rch->pcdata->eavesdropped != ch
	  && !fDemon)
	{
	  char buf[MIL];
	  send_to_char("A faint message transmits from the spies.\n\r",rch->pcdata->eavesdropped);
	  sprintf(buf, "Someone cabaltalks '`)$t``'");
	  act_new(buf,rch->pcdata->eavesdropped, (is_affected(rch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(rch) ? arg_buf :argument),
		  NULL,TO_CHAR, POS_DEAD);
	}
      if (IS_NPC(rch) && rch->pIndexData->progtypes & SPEECH_PROG)
	mprog_speech_trigger( arg_buf, ch, rch );
    }
      
    
    if (!IS_NPC(ch) && ch->pcdata->eavesdropped != NULL
	&& is_affected(ch->pcdata->eavesdropped,gsn_eavesdrop)
	&& !fDemon)
      {
	send_to_char("A faint message transmits from the spies.\n\r",ch->pcdata->eavesdropped);
	sprintf(buf,"%s cabaltalks '`)$t``'",PERS(ch,ch->pcdata->eavesdropped));
	act_new(buf,ch->pcdata->eavesdropped, (is_affected(ch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(ch) ? arg_buf : argument),
		NULL,TO_CHAR, POS_DEAD);
      }
}

void do_clantalk( CHAR_DATA *ch, char *argument )
{
    char buf[MSL], buf2[MSL] = "";
    char arg_buf[MIL];
    
    DESCRIPTOR_DATA *d;
//  const int demon_cost = 80;
    bool fDemon = IS_TELEPATH(ch);

    argument[0] = UPPER(argument[0]);

    if (IS_NPC(ch) || !HAS_CLAN(ch))
    {
        send_to_char("You aren't in a clan.\n\r",ch);
	return;
    }

    if ( argument[0] == '\0' )
    {
        if (IS_SET(ch->comm,COMM_NOCLAN))
        {
            send_to_char("Clan channel is now ON\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_NOCLAN);
        }
        else
        {
            send_to_char("Clan channel is now OFF\n\r",ch);
            SET_BIT(ch->comm,COMM_NOCLAN);
        }
        return;
    }
    strcpy(arg_buf, argument);
    if (IS_SET(ch->comm,COMM_NOCHANNELS))
    {
        send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
        return;
    }
    if (IS_AFFECTED2(ch, AFF_SILENCE) && !fDemon)
    {
	send_to_char("You work your mouth, but no sounds escape your lips.\n\r",ch);
	return;
    }

    if (is_affected(ch, gsn_gag) && !fDemon)
    {
        send_to_char("You find difficultly talking.\n\r",ch);
        return;
    }

    if (is_affected(ch, gsn_uppercut) && !fDemon)
    {
        send_to_char("Your jaws hurt too much to talk.\n\r",ch);
        return;
    }
/*
    if (fDemon) 
      if (!can_demon_talk(ch, demon_cost))
	return;
*/
    sprintf(buf2, "(%s) ",clanr_table[ch->pcdata->clan_rank][1]);
    REMOVE_BIT(ch->comm,COMM_NOCLAN);
    makedrunk(argument,ch);

    sprintf(buf,"[`8%s``]%s$n: '`0$t``'", GetClanName(GET_CLAN(ch)), buf2);
    act_new(buf,ch,argument,NULL,TO_CHAR,POS_DEAD);

    for ( d = descriptor_list; d != NULL; d = d->next )
      if ( d->connected == CON_PLAYING && d->character != ch 
	   && !IS_AFFECTED2(d->character, AFF_SILENCE)
	   && !IS_NPC(d->character)
	   && !is_affected(d->character,gsn_silence)
	   && !IS_SET(d->character->comm,COMM_NOCLAN) 
	   && !IS_SET(d->character->comm,COMM_QUIET) 
	   && !is_ignore(d->character,ch)){

	if (GET_CLAN(ch) == GET_CLAN(d->character)){
	  CHAR_DATA* rch = d->character;
	  act_new(buf,ch,(is_affected(d->character, gsn_com_lan)? arg_buf : IS_IMMORTAL(d->character) ? arg_buf : argument),
		  d->character, TO_VICT, POS_SLEEPING);

	  if (!IS_NPC(rch) && rch->pcdata->eavesdropped != NULL
	      && is_affected(rch->pcdata->eavesdropped,gsn_eavesdrop)
	      && rch != ch && rch->pcdata->eavesdropped != ch
	      && !fDemon)
	    {
	      send_to_char("A faint message transmits from the spies.\n\r",rch->pcdata->eavesdropped);
	      sprintf(buf, "Someone clantalks '`#$t``'");
	      act_new(buf,rch->pcdata->eavesdropped, (is_affected(rch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(rch) ? arg_buf : argument),
		      NULL,TO_CHAR, POS_DEAD);
	    }//END parasite
	}//END if same clan
      }//end descriptior loop
    if (!IS_NPC(ch) && ch->pcdata->eavesdropped != NULL
	&& is_affected(ch->pcdata->eavesdropped,gsn_eavesdrop)
	&& !fDemon)
      {
	send_to_char("A faint message transmits from the spies.\n\r",ch->pcdata->eavesdropped);
	sprintf(buf,"%s clantalks '`#$t``'",PERS(ch,ch->pcdata->eavesdropped));
	act_new(buf,ch->pcdata->eavesdropped, (is_affected(ch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(ch) ? arg_buf : argument),
		NULL,TO_CHAR, POS_DEAD);
      }
}

void do_jcabaltalk( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    DESCRIPTOR_DATA *d;
    char arg_buf[MIL];
    bool fDemon = IS_TELEPATH(ch);

    strcpy(arg_buf, argument);
    makedrunk(argument,ch);
    argument[0] = UPPER(argument[0]);

    sprintf(buf,"[`0JUSTICE``] $n: '`0$t``'");
    act_new(buf,ch,argument,NULL,TO_CHAR,POS_DEAD);
    for ( d = descriptor_list; d != NULL; d = d->next )
      if ( d->connected == CON_PLAYING 
	   && ch->pCabal && IS_CABAL(ch->pCabal, CABAL_JUSTICE)
	   && !IS_AFFECTED2(d->character, AFF_SILENCE) 
	   && !IS_SET(d->character->comm,COMM_NOCABAL)
	   && !is_affected(d->character,gsn_silence) 
	   && !IS_SET(d->character->comm,COMM_QUIET) 
	   && !is_ignore(d->character,ch)){
	CHAR_DATA* rch = d->character;
	act_new(buf,ch,(is_affected(d->character, gsn_com_lan)? arg_buf : 
			IS_IMMORTAL(d->character) ? arg_buf : argument),
		d->character,TO_VICT,POS_SLEEPING);
	if (!IS_NPC(rch) && rch->pcdata->eavesdropped != NULL
	    && is_affected(rch->pcdata->eavesdropped,gsn_eavesdrop)
	    && rch != ch && rch->pcdata->eavesdropped != ch
	    && !fDemon)
	  {
	    send_to_char("A faint message transmits from the spies.\n\r",rch->pcdata->eavesdropped);
	    sprintf(buf, "Someone cabaltalks '`)$t``'");
	    act_new(buf,rch->pcdata->eavesdropped, (is_affected(rch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(rch) ? arg_buf : argument),
		    NULL,TO_CHAR, POS_DEAD);
	  } 
	if (IS_NPC(rch) && rch->pIndexData->progtypes & SPEECH_PROG)
	  mprog_speech_trigger( arg_buf, ch, rch );
      }
    
    
    if (!IS_NPC(ch) && ch->pcdata->eavesdropped != NULL
	&& is_affected(ch->pcdata->eavesdropped,gsn_eavesdrop)
	&& !fDemon)
      {
	send_to_char("A faint message transmits from the spies.\n\r",ch->pcdata->eavesdropped);
	sprintf(buf,"%s cabaltalks '`)$t``'",PERS(ch,ch->pcdata->eavesdropped));
	act_new(buf,ch->pcdata->eavesdropped, (is_affected(ch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(ch) ? arg_buf : argument),
		NULL,TO_CHAR, POS_DEAD);
      }
    
}

void do_newbiechat( CHAR_DATA *ch, char *argument )
{
  char buf[MSL];
  DESCRIPTOR_DATA *d;
  argument[0] = UPPER(argument[0]);
  if (is_pk(ch,ch) 
      && (ch->pCabal == NULL || !IS_CABAL2(ch->pCabal, CABAL2_NEWBIE))
      && !IS_IMMORTAL(ch) 
      && !IS_GAME(ch, GAME_NEWBIE))
    {
      send_to_char("You're all grown up now, you can't use newbie chat anymore.\n\r",ch);
      return;
    }
  if ( argument[0] == '\0' )
    {
        if (IS_SET(ch->comm,COMM_NONEWBIE))
	  {
            send_to_char("Newbie chat is now ON\n\r",ch);
	    REMOVE_BIT(ch->comm,COMM_NONEWBIE);
	  }
        else
	  {
            send_to_char("Newbie chat is now OFF\n\r",ch);
            SET_BIT(ch->comm,COMM_NONEWBIE);
	  }
        return;
    }
  if (IS_SET(ch->comm,COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
      return;
    }
  REMOVE_BIT(ch->comm,COMM_NONEWBIE);
  sprintf(buf, "$n NEWBIE chats: '`0$t``'");
  act_new(buf,ch,argument,NULL,TO_CHAR,POS_DEAD);
  for ( d = descriptor_list; d != NULL; d = d->next )
    if ( d->connected == CON_PLAYING && d->character != ch
	 && !IS_NPC(d->character)
	 && (!is_pk(d->character,d->character) 
	     || IS_IMMORTAL(d->character)
	     || (d->character->pCabal != NULL 
		 && IS_CABAL2(d->character->pCabal, CABAL2_NEWBIE))
	     || IS_GAME(d->character, GAME_NEWBIE))
	 && !IS_SET(d->character->comm,COMM_NONEWBIE) 
	 && !IS_SET(d->character->comm,COMM_QUIET)
	 && !is_ignore(d->character,ch))
      act_new(buf,ch,argument,d->character,TO_VICT,POS_MEDITATE);
}

void do_immtalk( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    DESCRIPTOR_DATA *d;
    char arg_buf[MIL];
    argument[0] = UPPER(argument[0]);


    if ( argument[0] == '\0' )
    {
        if (IS_SET(ch->comm,COMM_NOWIZ))
        {
            send_to_char("Immortal channel is now ON\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_NOWIZ);
        }
        else
        {
            send_to_char("Immortal channel is now OFF\n\r",ch);
            SET_BIT(ch->comm,COMM_NOWIZ);
        }
        return;
    }
    strcpy(arg_buf, argument);
    REMOVE_BIT(ch->comm,COMM_NOWIZ);
    makedrunk(argument,ch);
    sprintf( buf, "$n: %s", argument );
    act_new("$n IMM_TALKS: `!$t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && IS_IMMORTAL(d->character)
        && !IS_SET(d->character->comm,COMM_NOWIZ) )
            act_new("$n IMM_TALKS: `!$t``",ch,(is_affected(d->character, gsn_com_lan)? arg_buf : IS_IMMORTAL(d->character) ? arg_buf : argument),
		    d->character,TO_VICT,POS_DEAD);
}

void do_imptalk( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    DESCRIPTOR_DATA *d;
    argument[0] = UPPER(argument[0]);

    if ( argument[0] == '\0' )
    {
        if (IS_SET(ch->comm,COMM_NOWIZ))
        {
            send_to_char("Immortal channel is now ON\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_NOWIZ);
        }
        else
        {
            send_to_char("Immortal channel is now OFF\n\r",ch);
            SET_BIT(ch->comm,COMM_NOWIZ);
        }
        return;
    }
    REMOVE_BIT(ch->comm,COMM_NOWIZ);
    sprintf( buf, "$n: %s", argument );
    act_new("$n IMP_TALKS: `!$t``",ch,argument,NULL,TO_CHAR,POS_DEAD);
    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && IS_TRUSTED(d->character,CREATOR)
        && !IS_SET(d->character->comm,COMM_NOWIZ) )
            act_new("$n IMP_TALKS: `!$t``",ch,argument,d->character,TO_VICT,POS_DEAD);
}

void do_yell( CHAR_DATA *ch, char *Argument )
{
    DESCRIPTOR_DATA *d;
    char arg_buf[MIL], argument[MIL];
    bool fDemon = IS_TELEPATH(ch);

    strcpy(argument, Argument);
    argument[0] = UPPER(argument[0]);
    if (IS_NPC(ch) && ch->pIndexData->vnum == 10)
	return;
    if (argument[0] == '\0' )
    {
        if (IS_SET(ch->comm,COMM_YELLSOFF))
      	{
            send_to_char("You can hear yells again.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_YELLSOFF);
      	}
      	else
      	{
            send_to_char("You will no longer hear yells.\n\r",ch);
            SET_BIT(ch->comm,COMM_YELLSOFF);
      	}
      	return;
    }
    strcpy(arg_buf, argument);
    if ( IS_SET(ch->comm, COMM_NOYELL) || fDemon)
    {
        send_to_char( "You can't yell.\n\r", ch );
        return;
    }

    if ( ch->level < 3 )
    {
	send_to_char( "Sorry, have to level before you can yell due to spammers.\n\r",ch);
	return;
    }
    if (IS_AFFECTED2(ch, AFF_TREEFORM))
    {
        send_to_char("Trees cannot yell!\n\r",ch);
        return;
    }
    if (IS_AFFECTED2(ch, AFF_SILENCE) )
    {
	send_to_char("You work your mouth, but no sounds escape your lips.\n\r",ch);
	return;
    }
    if (is_affected(ch, gsn_gag) )
    {
        send_to_char("You find difficultly talking.\n\r",ch);
        return;
    }
    if (is_affected(ch, gsn_uppercut) )
    {
        send_to_char("Your jaws hurt too much to talk.\n\r",ch);
        return;
    }
    REMOVE_BIT(ch->comm,COMM_YELLSOFF);
    makedrunk(argument,ch);
    act( "You yell '`6$T``'", ch, NULL, argument, TO_CHAR );
    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && d->character != ch && d->character->in_room != NULL
        && (d->character->in_room->area == ch->in_room->area || IS_SET(d->character->act,PLR_HEARALL))
	&& !IS_AFFECTED2(d->character, AFF_SILENCE) && !IS_SET(d->character->comm, COMM_YELLSOFF)
        && !is_affected(d->character,gsn_silence) && !IS_SET(d->character->comm, COMM_QUIET) && !is_ignore(d->character,ch))
	    act_new("$n yells '`6$t``'",ch,
		    (is_affected(d->character, gsn_com_lan)? arg_buf : IS_IMMORTAL(d->character) ? arg_buf : argument),
		    d->character,TO_VICT,POS_RESTING);
}

void j_yell( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char arg_buf[MIL];
    bool fDemon = IS_TELEPATH(ch);

    if (argument[0] == '\0' )
	return;
    /* if not awake or disabled we look for mob that will yell */
    if (!IS_AWAKE(ch) || is_affected(ch, gsn_ecstacy)){
      CHAR_DATA *vch, *vch_next;
      for (vch = ch->in_room->people; vch; vch = vch_next){
	vch_next = vch->next_in_room;

	/* make sure if charmed only yells if owner is the victim */
	if (IS_AFFECTED(vch, AFF_CHARM) && vch->master
	    && vch->master != ch)
	  continue;
	if (IS_AFFECTED2(vch, AFF_SILENCE) )
	  continue;

	if (IS_NPC(vch)
	    && (IS_GOOD(vch) 
		|| (vch->master && vch->master == ch) 
		|| is_name("guard", vch->name) )
	    && IS_SET(vch->in_room->area->area_flags,AREA_LAWFUL) 
	    && IS_AWAKE(vch)
	    && !is_affected(vch, gsn_ecstacy)) {
	  char buf[MIL];
	  if (vch->master && vch->master == ch)
	    sprintf(buf, "Help! My master is being attacked at %s!",
		  vch->in_room->name);
	  else 
	    sprintf(buf, "Help! A crime is being commited at %s!",
		  vch->in_room->name);

	  REMOVE_BIT(vch->comm,COMM_NOCHANNELS);
	  REMOVE_BIT(vch->comm,COMM_NOYELL);
	  act("$n tries to shake $N awake.", vch, NULL, ch, TO_ROOM);
	  send_to_char( "You sense someone trying to shake you awake.", ch);
	  j_yell(vch, buf);
	  SET_BIT(vch->comm,COMM_NOYELL);
	  SET_BIT(vch->comm,COMM_NOCHANNELS);
	  return;
	}
      }
      return;
    }
/* Viri: done in a_yell now
    if (!(IS_NPC(ch) && ch->master != NULL && IS_AFFECTED(ch,AFF_CHARM))){
*/
    if (IS_SET(ch->comm, COMM_NOYELL) || fDemon)
      {
	send_to_char( "You can't yell.\n\r", ch );
	return;
      }
/*
    }
*/
    strcpy(arg_buf, argument);
    if (IS_AFFECTED2(ch, AFF_SILENCE) )
    {
	send_to_char("You work your mouth, but no sounds escape your lips.\n\r",ch);
	return;
    }
    if (is_affected(ch, gsn_gag) )
    {
        send_to_char("You find difficultly talking.\n\r",ch);
        return;
    }
    if (is_affected(ch, gsn_uppercut) )
    {
        send_to_char("Your jaws hurt too much to talk.\n\r",ch);
        return;
    }
    REMOVE_BIT(ch->comm,COMM_YELLSOFF);
    makedrunk(argument,ch);
    act( "You yell '`6$T``'", ch, NULL, argument, TO_CHAR);
    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && d->character != ch && d->character->in_room != NULL
	&& (d->character->in_room->area == ch->in_room->area || IS_SET(d->character->act,PLR_HEARALL))
	&& !IS_AFFECTED2(d->character, AFF_SILENCE) && !IS_SET(d->character->comm, COMM_YELLSOFF)
        && !is_affected(d->character,gsn_silence) && !IS_SET(d->character->comm, COMM_QUIET) && !is_ignore(d->character,ch))
	{
	  if (!d->character->pCabal 
	      || (!IS_CABAL(d->character->pCabal, CABAL_JUSTICE) 
		  && !IS_CABAL(d->character->pCabal, CABAL_ROYAL)) 
	      )
	    act_new("$n yells '`6$t``'",ch,
		    (is_affected(d->character, gsn_com_lan)? arg_buf : IS_IMMORTAL(d->character) ? arg_buf : argument),
		    d->character,TO_VICT,POS_RESTING);
	  else{
	    char buf[MIL];
	    int cnt = IS_NPC(ch) ? 0 : ch->pcdata->flagged;
	    /* include info about offenses */
	    sprintf(buf, "$n (%s) yells wildly '`6$t``'",
		    cnt <= 0 ? "`&Model``" :
		    cnt < 3  ? "`7Abiding``" :
		    cnt < 5  ? "`#Occasional``" : 
		    cnt < 10 ? "`8Common``" : "`!Die-Hard``");
	    act_new(buf ,ch,
		    (is_affected(d->character, gsn_com_lan)? arg_buf : IS_IMMORTAL(d->character) ? arg_buf : argument),
		    d->character,TO_VICT,POS_RESTING);
	  }
	}
}
/* checks an emote argument for quotes to replace with say color */
char* parse_emote( char* string){
  static char buf[2 * MIL];
  char* ptr = buf;
  char* str;
  char c;
  bool fQuote = FALSE;
  int skip = 0;

  while (*string){
    switch (*string){
    case '\"':
      if ( (fQuote = !fQuote) == TRUE){
	str = "'`#";
	skip = 3;
	c = *(string + 1);
	c = UPPER(c);
	*(string + 1) = c;
      }
      else{
	str = "``'";
	skip = 3;
      }
      break;
    default:
      str = string;
      skip = 1;
      break;
    }
    while (skip--){
      *ptr++ = *str++;
    }
    string++;
  }

  //make sure we've ended the quote
  if (fQuote){
    *ptr++ = '`';
    *ptr++ = '`';
  }
  *ptr = 0;

  return buf;
}

void do_recho( CHAR_DATA *ch, char *arg )
{
    DESCRIPTOR_DATA *d;
    char argument[MIL];

    if ( arg[0] == '\0' )
    {
	send_to_char( "Local echo what?\n\r", ch );
	return;
    }
    strcpy( argument, parse_emote( arg ));
    for ( d = descriptor_list; d; d = d->next )
        if ( d->connected == CON_PLAYING && d->character->in_room == ch->in_room )
	{
            if (get_trust(d->character) >= get_trust(ch))
                send_to_char( "local> ",d->character);
	    sendf( d->character, "%s\n\r", argument );
	}
}



void do_smote(CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *vch;
    char *letter, *name;
    char last[MIL], temp[MSL], argument[MIL];
    int matches = 0;

    strcpy( argument, parse_emote( arg ));
    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "Smote what?\n\r", ch );
        return;
    }
    if (strstr(argument,ch->name) == NULL)
    {
	send_to_char("You must include your name in an smote.\n\r",ch);
	return;
    }
    sendf(ch, "%s\n\r", argument);

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
        if (vch->desc == NULL || vch == ch)
            continue;
        if ((letter = strstr(argument,vch->name)) == NULL)
        {
	    sendf(vch, "%s\n\r", argument);
            continue;
        }
        strcpy(temp,argument);
        temp[strlen(argument) - strlen(letter)] = '\0';
        last[0] = '\0';
        name = vch->name;
        for (; *letter != '\0'; letter++)
        {
            if (*letter == '\'' && matches == strlen(vch->name))
            {
                strcat(temp,"r");
                continue;
            }
            if (*letter == 's' && matches == strlen(vch->name))
            {
                matches = 0;
                continue;
            }
            if (matches == strlen(vch->name))
                matches = 0;
            if (*letter == *name)
            {
                matches++;
                name++;
                if (matches == strlen(vch->name))
                {
                    strcat(temp,"you");
                    last[0] = '\0';
                    name = vch->name;
                    continue;
                }
                strncat(last,letter,1);
                continue;
            }
            matches = 0;
            strcat(temp,last);
            strncat(temp,letter,1);
            last[0] = '\0';
            name = vch->name;
        }
	act(temp, vch, NULL, NULL, TO_CHAR);
	//	sendf(vch, "%s\n\r", temp);
    }
}

void do_say( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char arg[MSL];
    char buf[MSL];
    char arg_buf[MIL];
    OBJ_DATA *obj, *obj_next;
    Double_List * tmp_list;

    const int demon_cost = 30;	
    bool fDemon = IS_TELEPATH(ch);

    //for safety we copy string over as we are chaning it a bit.


    if ( argument[0] == '\0' )
    {
	send_to_char( "Say what?\n\r", ch );
	return;
    }
    /* 
       Theproblem here is that sometimes the mobs say
       things that are passed as dynamic strings, and it seems to 
       create a problem when UPPER casing it.
       So we simply stick it in array for a bit.
    */
    strcpy(arg_buf, argument);
    makedrunk(argument,ch);
    strcpy(arg, argument);

    arg[0] = UPPER(arg[0]);
    if (IS_AFFECTED2(ch, AFF_SILENCE) && !fDemon)
    {
        send_to_char("You work your mouth, but no sounds escape your lips.\n\r",ch);
        return;
    }
    if (is_affected(ch, gsn_gag) && !fDemon)
    {
        send_to_char("You find difficultly talking.\n\r",ch);
        return;
    }
    if (IS_AFFECTED2(ch, AFF_TREEFORM))
    {
        send_to_char("Trees cannot speak!\n\r",ch);
	return;
    }
    if (is_affected(ch, gsn_uppercut) && !fDemon)
    {
        send_to_char("Your jaws hurt too much to talk.\n\r",ch);
        return;
    }
    if (fDemon)
      {
	if (can_demon_talk(ch, demon_cost))
	    act( "You broadcast '`#$t``'", ch, arg, NULL, TO_CHAR );
	else
	  return;
      }
    else
      act( "You say '`#$t``'", ch, arg, NULL, TO_CHAR );


    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
      {
	if ((!IS_AFFECTED2(rch,AFF_SILENCE) && !is_affected(rch,gsn_silence))
	    || fDemon){
	  if (fDemon)
	    {
	      sprintf(buf,"[`8%s``] '`#$t``'",PERS(ch,rch));
	      act_new( buf, ch, (is_affected(rch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(rch) ? arg_buf : arg),
		       rch, TO_VICT, POS_DEAD);
	    }
	  else
	    {
	      sprintf(buf, "$n says '`#$t``'");
	      act_new( buf, ch,  (is_affected(rch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(rch) ? arg_buf : arg),
		       rch, TO_VICT, POS_RESTING + 100 );
	    }
	  
	  if (!IS_NPC(rch) && rch->pcdata->eavesdropped != NULL
	      && is_affected(rch->pcdata->eavesdropped,gsn_eavesdrop)
	      && rch != ch && rch->pcdata->eavesdropped != ch
	      && !fDemon)
	    {
	      send_to_char("A faint message transmits from the spies.\n\r",rch->pcdata->eavesdropped);
	      sprintf(buf, "Someone says '`#$t``'");
	      act_new(buf,rch->pcdata->eavesdropped, (is_affected(rch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(rch) ? arg_buf : arg),
		      NULL,TO_CHAR, POS_DEAD);
	    }
	  
	  /* OLD MPROG */
	  if (IS_NPC(rch) && rch->pIndexData->progtypes & SPEECH_PROG
	      &&   rch->position == rch->pIndexData->default_pos)
	    mprog_speech_trigger( arg_buf, ch, rch );

	  /* NEW PROG */
	  if (IS_NPC(rch) && HAS_TRIGGER_MOB(rch, TRIG_SPEECH))
	    p_act_trigger( arg_buf, rch, NULL, NULL, ch, NULL, NULL, TRIG_SPEECH );
	    
	  for ( obj = rch->carrying; obj; obj = obj_next ){
	    obj_next = obj->next_content;
	    if ( HAS_TRIGGER_OBJ( obj, TRIG_SPEECH ) )
	      p_act_trigger( arg_buf, NULL, obj, NULL, ch, NULL, NULL, TRIG_SPEECH );
	  }//END object check
	}
      }
      
    if (!IS_NPC(ch) && ch->pcdata->eavesdropped != NULL
	&& is_affected(ch->pcdata->eavesdropped,gsn_eavesdrop)
	&& !fDemon)
      {
	send_to_char("A faint message transmits from the spies.\n\r",ch->pcdata->eavesdropped);
	sprintf(buf,"%s says '`#$t``'",PERS(ch,ch->pcdata->eavesdropped));
	act_new(buf,ch->pcdata->eavesdropped, (is_affected(ch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(ch) ? arg_buf : arg),
	    NULL,TO_CHAR, POS_DEAD);
      }

    /* check rooms objects in room */
    for ( obj = ch->in_room->contents; obj; obj = obj_next ){
      obj_next = obj->next_content;
      if ( HAS_TRIGGER_OBJ( obj, TRIG_SPEECH ) )
	p_act_trigger( arg_buf, NULL, obj, NULL, ch, NULL, NULL, TRIG_SPEECH );
    }
    /* check room */
    if (HAS_TRIGGER_ROOM(ch->in_room, TRIG_SPEECH))
      p_act_trigger( arg_buf, NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_SPEECH );

/* check spectating rooms */
    rch = NULL;
    tmp_list = ch->in_room->watch_vnums;
    while (tmp_list != NULL) {
      rch = (get_room_index ((int)tmp_list->cur_entry))->people;
      while (rch != NULL) {
	if ((!IS_AFFECTED2(rch,AFF_SILENCE) && !is_affected(rch,gsn_silence)) || fDemon){
	  if (fDemon) {
	    sprintf(buf,"[%s] [`8%s``] '`#$t``'",ch->in_room->name, PERS(ch,rch));
	    act_new( buf, ch, (is_affected(rch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(rch) ? arg_buf : arg),
		     rch, TO_VICT, POS_DEAD);
	  }
	  else {
	    sprintf(buf, "[%s] $n says '`#$t``'", ch->in_room->name);
	    act_new( buf, ch,  (is_affected(rch, gsn_com_lan) ? arg_buf : IS_IMMORTAL(rch) ? 
				arg_buf : arg), rch, TO_VICT, POS_RESTING + 100 );
	  }
	}
	rch = rch->next_in_room;
      }
      tmp_list = tmp_list->next_node;
    }// END to chars loop
}

void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MIL], buf[MSL];
    char arg_buf[MIL];
    CHAR_DATA *victim;

    const int demon_cost = 60;

    bool com_lan = FALSE;
    bool fDemon =  IS_TELEPATH(ch);

    if (!IS_IMMORTAL(ch) && !IS_AWAKE(ch) && !fDemon)
    {
	send_to_char( "In your dreams, or what?\n\r", ch );
	return;
    }

    /* progs avoid all this */
    if (!IS_NPC(ch)){
      if (IS_SET(ch->comm, COMM_NOTELL))
	{
	  send_to_char( "Your message didn't get through.\n\r", ch );
	  return;
	}
      if ( IS_SET(ch->comm, COMM_QUIET))
	{
	  send_to_char( "You must turn off quiet mode first.\n\r", ch);
	  return;
	}
      if (IS_SET(ch->comm,COMM_DEAF))
	{
	  send_to_char("You must turn off deaf mode first.\n\r",ch);
	  return;
	}
      if (IS_AFFECTED2(ch, AFF_TREEFORM))
	{
	  send_to_char("Trees cannot send tells!\n\r",ch);
	  return;
	}
      if (IS_AFFECTED2(ch, AFF_SILENCE) && !fDemon)
	{
	  send_to_char("You work your mouth, but no sounds escape your lips.\n\r",ch);
	  return;
	}
      if (is_affected(ch, gsn_gag) && !fDemon)
	{
	  send_to_char("You find difficultly talking.\n\r",ch);
	  return;
	}
      if (is_affected(ch, gsn_uppercut) && !fDemon)
	{
	  send_to_char("Your jaws hurt too much to talk.\n\r",ch);
	  return;
	}
    }
    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Tell whom what?\n\r", ch );
	return;
    }
    argument[0] = UPPER(argument[0]);

    if ( ( victim = get_char_world( ch, arg ) ) == NULL
//	|| ( IS_NPC(victim) && victim->in_room != ch->in_room )
	|| (IS_AFFECTED2(victim, AFF_SHADOWFORM) && !IS_IMMORTAL(ch)) )
/*	|| (IS_AFFECTED2(victim, AFF_TREEFORM) && !IS_IMMORTAL(ch)) )*/
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    strcpy(arg_buf, argument);
    com_lan = (is_affected(victim, gsn_com_lan) || IS_IMMORTAL(victim));
     if ( (!IS_AWAKE(victim) 
	|| IS_AFFECTED2(victim, AFF_SILENCE) 
	|| is_affected(victim, gsn_silence)) 
	&& !fDemon 
	&& !IS_IMMORTAL(ch))
    {
	sendf(ch, "They can't hear you.\n\r");
	return;
    }

    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF)) && !IS_IMMORTAL(ch))
    {
	sendf(ch, "They are not receiving tells.\n\r");
  	return;
    }

    if (is_ignore(victim,ch) && !IS_IMMORTAL(ch))
    {
	sendf(ch, "They are ignoring you.\n\r");
	return;
    }

    makedrunk(argument,ch);
    if ( victim->desc == NULL && !IS_NPC(victim))
    {
	act("$N seems to have misplaced $S link...try again later.",ch,NULL,victim,TO_CHAR);
	if (fDemon)
	  {
	    /* demons talk to demons for free */
	    if (IS_TELEPATH(ch)
		|| can_demon_talk(ch, demon_cost))
	      sprintf(buf,"[`8%s``] '`2%s``'\n\r",PERS(ch, victim),
		      com_lan? arg_buf : argument);
	    else
	      return;
	  }
	else
	  sprintf(buf,"%s tells you '`2%s``'\n\r",PERS(ch,victim),
		  com_lan? arg_buf : argument);
        buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
	return;
    }


    if (IS_SET(victim->comm,COMM_AFK))
    {
	if (IS_NPC(victim))
	{
	    sendf(ch,"%s is AFK, and not receiving tells.\n\r",PERS2(victim));
	    return;
	}
	act("$N is AFK, but your tell will go through when $E returns.",ch,NULL,victim,TO_CHAR);
	if (fDemon)
	  {
	    /* demons talk free to demons */
	    if (IS_TELEPATH(ch)
		|| can_demon_talk(ch, demon_cost))
	      sprintf(buf,"[`8%s``] '`2%s``'\n\r",PERS(ch, victim) ,
		      com_lan? arg_buf : argument);
	    else
	      return;
	  }
	else
	  sprintf(buf,"%s tells you '`2%s``'\n\r",PERS(ch,victim),
		  com_lan? arg_buf : argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer,buf);
	return;
      }

    if (fDemon)
      {
	/* demons talk free to demons */
	if (IS_TELEPATH(ch)
	    || can_demon_talk(ch, demon_cost))
	  {
	    act( "You project to $N '`2$t``'", ch, argument, victim, TO_CHAR );
	    act_new("[`8$n``] '`2$t``'",ch,
		    com_lan? arg_buf : argument,
		    victim,TO_VICT,POS_DEAD);
	  }
	else
	  return;
      }
    else
      {
	act( "You tell $N '`2$t``'", ch, argument, victim, TO_CHAR );
	act_new("$n tells you '`2$t``'",ch,
		com_lan? arg_buf : argument,
		victim,TO_VICT,POS_DEAD);
      }

    /* NEW PROG */
    if (IS_NPC(victim) && HAS_TRIGGER_MOB(victim, TRIG_SPEECH) && (IS_NPC(ch) || victim->in_room == ch->in_room))
      p_act_trigger( arg_buf, victim, NULL, NULL, ch, NULL, NULL, TRIG_SPEECH );    
	
    victim->reply = ch;

    if (!IS_NPC(ch) && ch->pcdata->eavesdropped != NULL
	&& is_affected(ch->pcdata->eavesdropped,gsn_eavesdrop) 
	&& ch->pcdata->eavesdropped != victim
	&& !fDemon)
    {
	send_to_char("A faint message transmits from the spies.\n\r",ch->pcdata->eavesdropped);
	sprintf(buf,"%s tells someone '`2$t``'",PERS(ch,ch->pcdata->eavesdropped));
	act_new(buf,ch->pcdata->eavesdropped,
	    com_lan? arg_buf : argument,
	    NULL,TO_CHAR, POS_DEAD);
    }

    if (!IS_NPC(victim) && victim->pcdata->eavesdropped != NULL
	&& is_affected(victim->pcdata->eavesdropped,gsn_eavesdrop) 
	&& victim->pcdata->eavesdropped != ch
	&& !fDemon)
    {
	send_to_char("A faint message transmits from the spies.\n\r",victim->pcdata->eavesdropped);
	sprintf(buf,"Someone tells %s '`2$t``'",PERS(victim,victim->pcdata->eavesdropped));
	act_new(buf,victim->pcdata->eavesdropped,
	    com_lan? arg_buf : argument,
	    NULL,TO_CHAR, POS_DEAD);
    }
    if (IS_NPC(victim)){
      if (victim->pIndexData->progtypes & TELL_PROG)
	mprog_tell_trigger( argument, victim );
      if ( HAS_TRIGGER_MOB( victim, TRIG_TELL ) )
	p_act_trigger( argument, victim, NULL, NULL, ch, NULL, NULL, TRIG_TELL );
    }
}

void do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MSL];
    char arg_buf[MIL];
    const int demon_cost = 30;
    bool fDemon = IS_TELEPATH(ch);
    bool com_lan = FALSE;

    if ( ( victim = ch->reply ) == NULL )
    {
	send_to_char( "You have no one to reply too.\n\r", ch );
	return;
    }
    strcpy(arg_buf, argument);
    com_lan = (is_affected(victim, gsn_com_lan) || IS_IMMORTAL(victim));

    if (!IS_IMMORTAL(victim) && !IS_AWAKE(ch) && !fDemon)
    {
	send_to_char( "In your dreams, or what?\n\r", ch );
	return;
    }
    if (IS_AFFECTED2(ch, AFF_TREEFORM))
    {
        send_to_char("Trees cannot send tells!\n\r",ch);
        return;
    }
    if (IS_SET(ch->comm, COMM_NOTELL))
    {
	send_to_char( "Your message didn't get through.\n\r", ch );
	return;
    }
    if ( IS_SET(ch->comm, COMM_QUIET))
    {
	send_to_char( "You must turn off quiet mode first.\n\r", ch);
	return;
    }
    if (IS_SET(ch->comm,COMM_DEAF))
    {
	send_to_char("You must turn off deaf mode first.\n\r",ch);
	return;
    }
    if (IS_AFFECTED2(ch, AFF_SILENCE) && !fDemon)
    {
	send_to_char("You work your mouth, but no sounds escape your lips.\n\r",ch);
	return;
    }
    if (is_affected(ch, gsn_gag) && !fDemon)
    {
        send_to_char("You find difficultly talking.\n\r",ch);
        return;
    }
    if (is_affected(ch, gsn_uppercut) && !fDemon)
    {
        send_to_char("Your jaws hurt too much to talk.\n\r",ch);
        return;
    }

    if (( IS_NPC(victim) && victim->in_room != ch->in_room )
	|| (IS_AFFECTED2(victim, AFF_SHADOWFORM) && !IS_IMMORTAL(ch)) )
/*      || (IS_AFFECTED2(victim, AFF_TREEFORM) && !IS_IMMORTAL(ch)) )*/
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (argument[0] == '\0' )
    {
      send_to_char( "Reply what?\n\r", ch );
      return;
    }

    argument[0] = UPPER(argument[0]);
    makedrunk(argument,ch);

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
	act("$N seems to have misplaced $S link...try again later.",ch,NULL,victim,TO_CHAR);
	if (fDemon)
	  {
	    /* demons talk free to demons */
	    if (IS_TELEPATH(ch)
		|| can_demon_talk(ch, demon_cost))
	      sprintf(buf,"[`8%s``] '`2%s``'\n\r",PERS(ch,victim), com_lan? arg_buf : argument);
	    else
	      return;
	  }
	else
	  sprintf(buf,"%s tells you '`2%s``'\n\r",PERS(ch,victim), com_lan? arg_buf : argument);
        buf[0] = UPPER(buf[0]);
        add_buf(victim->pcdata->buffer,buf);
	return;
    }

     if ( (!IS_AWAKE(victim) 
	|| IS_AFFECTED2(victim, AFF_SILENCE) 
	|| is_affected(victim, gsn_silence)) 
	&& !fDemon 
	&& !IS_IMMORTAL(ch))
    {
	sendf(ch, "They can't hear you.\n\r");
	return;
    }

    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF)) && !IS_IMMORTAL(ch))
    {
	sendf(ch, "They are not receiving tells.\n\r");
  	return;
    }

    if (is_ignore(victim,ch) && !IS_IMMORTAL(ch))
    {
	sendf(ch, "%s is ignoring you.\n\r", PERS2(victim));
	return;
    }

    if (IS_SET(victim->comm,COMM_AFK))
    {
	if (IS_NPC(victim))
	{
	    sendf(ch,"%s is AFK, and not receiving tells.\n\r",PERS2(victim));
	    return;
	}
	act("$N is AFK, but your tell will go through when $E returns.",ch,NULL,victim,TO_CHAR);
	if (fDemon)
	  {
	    /* demons talk free to demons */
	    if (IS_TELEPATH(ch)
		|| can_demon_talk(ch, demon_cost))
	      sprintf(buf,"[`8%s``] '`2%s``'\n\r",PERS(ch,victim), com_lan? arg_buf : argument);
	    else
	      return;
	  }
	else
	  sprintf(buf,"%s tells you '`2%s``'\n\r",PERS(ch,victim), com_lan? arg_buf : argument);
	buf[0] = UPPER(buf[0]);
	add_buf(victim->pcdata->buffer,buf);
	return;
      }

    if (fDemon)
      {
	/* demons talk free to demons */
	if (IS_TELEPATH(ch)
	    || can_demon_talk(ch, demon_cost))
	  {
	    act( "You project to $N '`2$t``'", ch, argument, victim, TO_CHAR );
	    act_new("[`8$n``] '`2$t``'",ch, com_lan? arg_buf : argument,victim,TO_VICT,POS_DEAD);
	  }
	else
	  return;
      }
    else
      {
	act( "You tell $N '`2$t``'", ch, argument, victim, TO_CHAR );
	act_new("$n tells you '`2$t``'",ch, com_lan? arg_buf : argument,victim,TO_VICT,POS_DEAD);
      }
  
    victim->reply = ch;

    if (!IS_NPC(ch) && ch->pcdata->eavesdropped != NULL
	&& is_affected(ch->pcdata->eavesdropped,gsn_eavesdrop) 
	&& ch->pcdata->eavesdropped != victim
	&& !fDemon)
    {
	send_to_char("A faint message transmits from the spies.\n\r",ch->pcdata->eavesdropped);
	sprintf(buf,"%s tells someone '`2$t``'",PERS(ch,ch->pcdata->eavesdropped));
	act_new(buf,ch->pcdata->eavesdropped, com_lan? arg_buf : argument,NULL,TO_CHAR, POS_DEAD);
    }

    if (!IS_NPC(victim) && victim->pcdata->eavesdropped != NULL
	&& is_affected(victim->pcdata->eavesdropped,gsn_eavesdrop) 
	&& victim->pcdata->eavesdropped != ch
	&& !fDemon)
    {
	send_to_char("A faint message transmits from the spies.\n\r",victim->pcdata->eavesdropped);
	sprintf(buf,"Someone tells %s '`2$t``'",PERS(victim,victim->pcdata->eavesdropped));
	act_new(buf,victim->pcdata->eavesdropped, com_lan? arg_buf : argument,NULL,TO_CHAR, POS_DEAD);
    }
    if (IS_NPC(victim) 
		&& victim->pIndexData->progtypes & TELL_PROG)
	      mprog_tell_trigger( argument, victim );
}

void do_emote( CHAR_DATA *ch, char *argument ){
  char* ptr;

  if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) ){
    send_to_char( "You can't show your emotions.\n\r", ch );
    return;
  }
  if ( argument[0] == '\0' ){
    send_to_char( "Emote what?\n\r", ch );
    return;
  }
  if (IS_AFFECTED2(ch, AFF_SILENCE) || is_affected(ch,gsn_garble)){
    send_to_char("You are unable to get your point across.\n\r",ch);
    return;
  }
  if (is_affected(ch, gsn_uppercut) ){
    send_to_char("You'd better not, you might injure your jaws even more.\n\r",ch);
    return;
  }
  if (IS_AFFECTED2(ch, AFF_TREEFORM)){
    affect_strip( ch, gsn_treeform );
    REMOVE_BIT   ( ch->affected_by, AFF_TREEFORM                );
    act("You emerge from your treeformed state.",ch,NULL,NULL,TO_CHAR);
    act("$n emerges from $s treeformed state.",ch,NULL,NULL,TO_ROOM);
  }
  
  if (IS_AFFECTED(ch, AFF_HIDE)){
    affect_strip( ch, gsn_hide );
    REMOVE_BIT   ( ch->affected_by, AFF_HIDE                );
    act("You step out of the shadows.",ch,NULL,NULL,TO_CHAR);
    act("$n steps out of the shadows.",ch,NULL,NULL,TO_ROOM);
  }
  if (IS_AFFECTED2(ch, AFF_CAMOUFLAGE)){
    affect_strip ( ch, gsn_camouflage                       );
    REMOVE_BIT   ( ch->affected2_by, AFF_CAMOUFLAGE         );
    act("You step out from your cover.",ch,NULL,NULL,TO_CHAR);
    act("$n steps out from $s cover.",ch,NULL,NULL,TO_ROOM);
  }

  ptr = parse_emote( argument );

  act_new( "$n $T", ch, NULL, ptr, TO_ROOM, POS_RESTING + 100 );
  act_new( "$n $T", ch, NULL, ptr, TO_CHAR, POS_RESTING + 100 );
}

void do_pmote( CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *vch;
    char *letter, *name;
    char last[MSL], temp[MSL];

    char argument[MSL];
    int matches = 0;

    /* some strings are passed as consts. and create problems
       when worked on, we store them in array for a bit*/

    strcpy(argument, parse_emote( arg ));

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "Pmote what?\n\r", ch );
        return;
    }
    if (IS_AFFECTED2(ch, AFF_SILENCE) )
    {
	send_to_char("You are unable to get your point across.\n\r",ch);
	return;
    }
    if (is_affected(ch, gsn_uppercut) )
    {
        send_to_char("Your jaws hurt too much to talk.\n\r",ch);
        return;
    }
    act( "$n $t", ch, argument, NULL, TO_CHAR );

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
	if (vch->desc == NULL || vch == ch)
	    continue;
        if ((letter = strstr(argument,vch->name)) == NULL)
        {
            act("$N $t",vch,argument,ch,TO_CHAR);
            continue;
        }
	strcpy(temp,argument);
	temp[strlen(argument) - strlen(letter)] = '\0';
   	last[0] = '\0';
        name = vch->name;
	for (; *letter != '\0'; letter++)
	{
	    if (*letter == '\'' && matches == strlen(vch->name))
	    {
		strcat(temp,"r");
		continue;
	    }
	    if (*letter == 's' && matches == strlen(vch->name))
	    {
		matches = 0;
		continue;
	    }
 	    if (matches == strlen(vch->name))
		matches = 0;
	    if (*letter == *name)
	    {
		matches++;
		name++;
		if (matches == strlen(vch->name))
		{
		    strcat(temp,"you");
		    last[0] = '\0';
		    name = vch->name;
		    continue;
		}
		strncat(last,letter,1);
		continue;
	    }
	    matches = 0;
	    strcat(temp,last);
	    strncat(temp,letter,1);
	    last[0] = '\0';
            name = vch->name;
	}
	act("$N $t",vch,temp,ch,TO_CHAR);
    }
}

void do_bug( CHAR_DATA *ch, char *argument )
{
    append_file( ch, BUG_FILE, argument );
    send_to_char( "Bug logged.\n\r", ch );
}

void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument );
    send_to_char( "Typo logged.\n\r", ch );
}

void do_qui( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
}

void do_quit( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *vch, *vch_next, *rch;
  DESCRIPTOR_DATA *d,*d_next;
  OBJ_DATA* obj, *obj_next;
  int id, rare_count = 0, limit= 0;
  bool fCrus = FALSE;

  if ( IS_NPC(ch) )
    return;
  if ( ch->position == POS_FIGHTING )
    {
      send_to_char( "No way! You are fighting.\n\r", ch );
      return;
    }
  if ( ch->position < POS_STUNNED  )
    {
      send_to_char( "You're not DEAD yet.\n\r", ch );
      return;
    }
  if (ch->in_room->pCabal && ch->pCabal && ch->pCabal != ch->in_room->pCabal)
    {
      send_to_char( "You can't quit here!\n\r",ch);
      return;
    }
  if (is_fight_delay(ch,120) || is_affected(ch,gen_ensnare))
    {
      send_to_char( "You can't quit yet!  Your adrenaline is rushing.\n\r", ch);
      return;
    }
  if (IS_SET(ch->act2,PLR_NOQUIT) || is_affected(ch,gsn_noquit) || is_affected(ch,gen_ensnare))
    {
      send_to_char("You are not allowed to quit!\n\r",ch);
      return;
    }
  if (IS_SET(ch->in_room->room_flags, ROOM_NO_QUIT) && !IS_IMMORTAL(ch))
    {
      send_to_char( "You can't seem to leave this room!\n\r",ch);
      return;
    }
  if (is_affected(ch,gsn_bat_form) 
      || is_affected(ch,gsn_wolf_form) 
      || is_affected(ch,gsn_mist_form)
      
      || is_affected(ch,gsn_weretiger)
      || is_affected(ch,gsn_werewolf)
      || is_affected(ch,gsn_werebear)
      || is_affected(ch,gsn_werefalcon)){
    send_to_char("You must first revert to your natural form.\n\r",ch);
    return;
  }
  if (ch->pcdata->pnote && ch->pcdata->pnote->text != NULL && ch->pcdata->pnote->text[0] != '\0' && ch->timer < 10)
    {
      send_to_char("You have a note in progress, either post it or clear it before you quit out.\n\r",ch);
      return;
    }
  
  //check for rares over limit
  if (ch->class == class_lookup("crusader") 
      && !is_affected(ch, skill_lookup("undeniable order")))
    fCrus = TRUE;

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content){
    if (!IS_LIMITED(obj))
      continue;

    if (obj->wear_loc != -1 && !fCrus)
      continue;
    else
      rare_count ++;
  }
  limit = (ch->level / 10) - 1;

  if (fCrus)
    limit *= 2;

  if (!IS_NPC( ch) && !IS_IMMORTAL(ch) 
      && rare_count
      && rare_count > limit
      && !ch->pcdata->confirm_quit){
    send_to_char("You might lose limited items in your inventory. \"quit\" to confirm.\n\r", ch);
    ch->pcdata->confirm_quit = TRUE;
    return;
  }
  
  /* Successful quit */
  for (obj = ch->carrying; obj != NULL; obj = obj_next){
    obj_next = obj->next_content;
    if (obj->item_type == ITEM_CABAL){
      obj_from_char( obj );
      obj_to_room( obj, ch->in_room );
    }
  }
  if (!IS_NPC(ch))
    {
      CHAR_DATA* och;
      AFFECT_DATA* paf;
      if (ch->logon != 0 && ch->pcdata->member){
	ch->pcdata->member->connected = FALSE;
	ch->pcdata->member->last_update = mud_data.current_time;
      }
      if (is_affected(ch,gsn_familiar_link))
        {
	  send_to_char("Your energy drains out as you lose link with your familiar.\n\r",ch);
	  affect_strip(ch,gsn_familiar_link);
        }
      if (ch->pcdata->familiar != NULL)
	{
	  extract_char( ch->pcdata->familiar, TRUE );
	  ch->pcdata->familiar=NULL;
        }
      if (!IS_NPC(ch) && ch->pcdata->eavesdropping != NULL)
	{
	  act("You stop eavesdropping on $N.",ch,NULL,ch->pcdata->eavesdropping,TO_CHAR);
	  affect_strip(ch,gsn_eavesdrop);
	  ch->pcdata->eavesdropping->pcdata->eavesdropped = NULL;
	  ch->pcdata->eavesdropping = NULL;
	}
      if (!IS_NPC(ch) && ch->pcdata->eavesdropped != NULL)
	{
	  act("You stop eavesdropping on $N.",ch->pcdata->eavesdropped,NULL,ch,TO_CHAR);
	  affect_strip(ch->pcdata->eavesdropped,gsn_eavesdrop);
	  ch->pcdata->eavesdropped->pcdata->eavesdropping = NULL;
	  ch->pcdata->eavesdropped = NULL;
	}
      for ( paf = ch->affected; paf; paf = paf->next ){
	if (paf->type != gen_challenge)
	  continue;
	if ( (och = get_char(paf->string)) != NULL){
	  act_new("`&$N has declined your challenge.``", och, NULL, ch, TO_CHAR, POS_DEAD);
	  act_new("`&You have declined $n's challenge.``", och, NULL, ch, TO_VICT, POS_DEAD);
	  if (ch->pCabal != NULL && IS_SET(ch->pCabal->pIndexData->progress, PROGRESS_CHALL))
	    update_challenge( ch, ch->name, paf->string, 0, 0, 0, 1);
	}
      }
    }
    send_to_char( "You have escaped from the Forsaken Lands.\n\r",ch);
    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
        if (get_trust(rch) >= ch->invis_level && ch->desc)
            act_new( "$n has left the lands.", ch, NULL, rch, TO_VICT, POS_RESTING + 100 );
    /* cabal echos */
    if (ch->pCabal && !IS_IMMORTAL(ch)){
      char * const his_her [] = { "its", "his", "her" };
      if (IS_CABAL(ch->pCabal, CABAL_AREA)){
	sprintf( log_buf, "%s %s has left the fray.", 
		 get_crank_str( ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ),
		 PERS2(ch));
      }
      else if (IS_CABAL(ch->pCabal, CABAL_ROYAL)){
	sprintf( log_buf, "%s %s has withdrawn %s presence.", 
	       get_crank_str( ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ),
	       PERS2(ch),
	       his_her[URANGE(0,ch->sex,2)]);
    }
    else if (IS_CABAL(ch->pCabal, CABAL_JUSTICE)){
      sprintf( log_buf, "%s %s no longer serves the Law with us.", 
	       get_crank_str( ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ),
	       PERS2(ch));
    }
    else if (IS_CABAL(ch->pCabal, CABAL_ROUGE)){
      sprintf( log_buf, "%s %s has been noticed leaving the lands.", 
	       get_crank_str( ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ),
	       PERS2(ch));
    }
    else{
      sprintf( log_buf, "%s %s has left our forces.", 
	       get_crank_str( ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ),
	       PERS2(ch));
    }
      cabal_echo(ch->pCabal, log_buf );
  }

    if (ch->desc)
        sprintf( log_buf, "%s@%s has disconnected in room %d.", ch->name, ch->desc->host, ch->in_room->vnum);
    else
        sprintf( log_buf, "%s has disconnected.", ch->name);
    log_string( log_buf );
    wiznet(log_buf,NULL,NULL,WIZ_LOGINS,0,get_trust(ch));
    
/* After extract_char the ch is no longer valid! */
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next;
	if (is_affected(vch,gsn_doppelganger) && vch->doppel == ch)
	{
	    send_to_char("You shift to your true form as your victim leaves.\n\r",vch);
	    affect_strip(vch,gsn_doppelganger);
	}
      	if (!IS_NPC(vch) && vch->pcdata->guarding == ch)
	{
	    act("You stops guarding $N.", vch, NULL, ch, TO_CHAR);
	    act("$n stops guarding you.", vch, NULL, ch, TO_VICT);
	    act("$n stops guarding $N.", vch, NULL, ch, TO_NOTVICT);
	    vch->pcdata->guarding  = NULL;
	    ch->pcdata->guarded_by = NULL;
	}
    	if (!IS_NPC(ch) && ch->pcdata->guarded_by != NULL)
        {
	    ch->pcdata->guarded_by->pcdata->guarding = NULL;
            ch->pcdata->guarded_by = NULL;
      	}
	if (is_affected(vch,gsn_mirror_image) && is_affected(vch,gsn_doppelganger) && vch->doppel == ch)
	{
	    extract_char(vch,TRUE);
	    continue;
	}
	/* save pets during quits
	if (IS_NPC(vch) 
	    && vch->summoner != NULL 
	    && vch->summoner == ch
	    && !IS_SET(vch->act, ACT_PET))
	  {
	    act( "$n slowly disappears.",vch,NULL,NULL,TO_ROOM );
	    extract_char(vch,TRUE);
	    continue;
	  }
	*/
    }

    save_char_obj( ch );
    id = ch->id;
    d = ch->desc;

    extract_char( ch, TRUE );
    if ( d != NULL )
	close_socket( d );

    /* toast cheating people */
    for (d = descriptor_list; d != NULL; d = d_next)
    {
	CHAR_DATA *tch = d->original ? d->original : d->character;
	d_next = d->next;
	if (tch && tch->id == id)
	{
	    extract_char(tch,TRUE);
	    close_socket(d);
	}

    }
}

void do_save( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    if ( IS_NPC(ch) )
	return;
    save_char_obj( ch );
    WAIT_STATE(ch, 6);
    sprintf(buf,"%s saves",ch->name);
    wiznet(buf,ch,NULL,WIZ_SAVE,0,0);
    send_to_char("Saving...\n\r", ch);
}

bool can_follow(CHAR_DATA* ch, CHAR_DATA* victim){

  if (is_affected(ch,skill_lookup("apathey"))){
    send_to_char("You don't care to follow anyone.\n\r",ch);
    return FALSE;
  }
  else if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL ){
    sendf(ch, "But you'd rather follow %s!\n\r", PERS2(ch->master));
    return FALSE;
  }
  else if (is_affected(ch,gsn_heroism)){
    send_to_char("You cannot allow others to join your quest.\n\r",ch);
    return FALSE;
  }
  if (is_affected(ch, gen_crusade)){
    send_to_char("You cannot allow others to aid you on Crusade.\n\r",ch);
    return FALSE;
  }
  if (tarot_find(ch->affected) == 10){
    send_to_char("You feel too solutide to want companion.\n\r",ch);
    return FALSE;
  }

  /* check if we can master of a charmie */
  if (IS_NPC(victim) && victim->master && !IS_NPC(victim->master)){
    if (is_affected(victim->master, gen_crusade) && ch != victim){
      act("$N has set out upon a holy Crusade and cannot be aided.", ch, NULL, victim->master, TO_CHAR);
      return FALSE;
    }
    if(IS_SET(victim->master->act,PLR_NOFOLLOW)
       || is_affected(victim->master,gsn_heroism) 
       || is_affected(victim->master,skill_lookup("apathey")) 
       || tarot_find(victim->master->affected) == 10){
      sendf(ch,"%s doesn't seem to want any followers.\n\r", PERS2(victim->master));
      return FALSE;
    }
  }
  
  if (is_affected(victim, gen_crusade) && ch != victim){
    act("$N has set out upon a holy Crusade and cannot be aided.", ch, NULL, victim, TO_CHAR);
    return FALSE;
  }
  else if ( ch != victim 
	    && !IS_NPC(victim) 
	    && (IS_SET(victim->act,PLR_NOFOLLOW) 
		|| is_affected(victim,gsn_heroism) 
		|| is_affected(victim,skill_lookup("apathey")) 
		|| tarot_find(victim->affected) == 10) 
	    && !IS_IMMORTAL(ch))
    {
      sendf(ch,"%s doesn't seem to want any followers.\n\r", PERS2(victim));
      return FALSE;
    }
  return TRUE;
}

void do_follow( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Follow whom?\n\r", ch );
	return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( victim == ch )
    {
	if ( ch->master == NULL )
	{
	    send_to_char( "You already follow yourself.\n\r", ch );
	    return;
	}
	else
	{
	    send_to_char( "You now follow yourself.\n\r", ch );
	    stop_follower(ch);
	    return;
	}
    }
    if (!can_follow(ch, victim))
      return;

    REMOVE_BIT(ch->act,PLR_NOFOLLOW);
    if ( ch->master != NULL )
      stop_follower( ch );
    add_follower( ch, victim );
}

void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master != NULL )
    {
	bug( "Add_follower: non-null master.", 0 );
	return;
    }
    ch->master = master;
    ch->leader = NULL;
    if ( can_see( master, ch ) )
	act( "$n now follows you.", ch, NULL, master, TO_VICT    );
    act( "You now follow $N.",      ch, NULL, master, TO_CHAR    );
}

void stop_follower( CHAR_DATA *ch )
{

  if ( ch->master == NULL ) {
	return;
  }
  if (IS_NPC(ch)){
    if (empty_char(ch))
      act("$n spills some items on the ground.", ch, NULL, NULL, TO_ROOM);
  }
  if ( IS_AFFECTED(ch, AFF_CHARM) ) {
    REMOVE_BIT( ch->affected_by, AFF_CHARM );
    affect_strip( ch, gsn_charm_person );
  }
  if ( can_see( ch->master, ch ) && !IS_IMMORTAL(ch) && ch->in_room != NULL) {
    act( "$n stops following you.",     ch, NULL, ch->master, TO_VICT    );
    act( "You stop following $N.",      ch, NULL, ch->master, TO_CHAR    );
  }
  if (IS_NPC(ch) && ch->master && !IS_NPC(ch->master)
      && ch->master->pcdata->pStallion 
      && ch->master->pcdata->pStallion == ch->pIndexData){
    do_dismount(ch->master, "");
  }
  if (!IS_NPC(ch) && ch->pcdata->guarded_by != NULL) {
    act("You stop guarding $N.",ch->pcdata->guarded_by,NULL,ch,TO_CHAR);
    act("$n stops guarding you.",ch->pcdata->guarded_by,NULL,ch,TO_VICT);
    act("$n stops guarding $N.",ch->pcdata->guarded_by,NULL,ch,TO_NOTVICT);
    ch->pcdata->guarded_by->pcdata->guarding = NULL;
    ch->pcdata->guarded_by           = NULL;
  }
  if (!IS_NPC(ch) && ch->pcdata->guarding != NULL) {
    act("You stop guarding $N.", ch, NULL, ch->pcdata->guarding, TO_CHAR);
    act("$n stops guarding you.", ch, NULL, ch->pcdata->guarding, TO_VICT);
    act("$n stops guarding $N.", ch, NULL, ch->pcdata->guarding, TO_NOTVICT);
    ch->pcdata->guarding->pcdata->guarded_by = NULL;
    ch->pcdata->guarding = NULL;
  }
  if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_SPECIAL_GUARD 
      && ch->leader != NULL){
    AFFECT_DATA af;
    send_to_char("Cursing at the loss you send in a request for a new guard captain.\n\r", ch->leader);
    af.type = UMAX(0, skill_lookup("guard captain"));
    af.level = ch->level;
    af.duration = 12;
    af.where = TO_AFFECTS;
    af.bitvector = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_char(ch->leader, &af);
  }
  if (ch->master && !IS_NPC(ch->master) && ch->master->pcdata->familiar) {
    if (is_affected(ch->master,gsn_familiar_link) && ch->master->pcdata->familiar != NULL 
	&& ch->master->pcdata->familiar == ch) {
      AFFECT_DATA af;
      send_to_char("Your energy drains out as you lose link with your familiar.\n\r",ch->master);
      affect_strip(ch->master,gsn_familiar_link);
      ch->master->pcdata->familiar=NULL;
      af.type		= gsn_drained;
      af.level	= ch->level;
      af.duration	= 12;
      af.modifier	= 0;
      af.location	= 0;
      af.bitvector	= 0;
      affect_to_char(ch->master,&af);
    }
  }


  if (ch->master->pet == ch) {
    ch->master->pet = NULL;
  }
  ch->master = NULL;
  ch->leader = NULL;
  ch->summoner = NULL;

  if (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_DUMMY 
      && is_affected(ch,gsn_mirror_image)){
    act("$n shatters into tiny fragments of glass.",ch,NULL,NULL,TO_ROOM);
    extract_char( ch, TRUE );
  }
}

void nuke_pets( CHAR_DATA *ch )
{
    CHAR_DATA *pet;
    if ((pet = ch->pet) != NULL)
    {
    	stop_follower(pet);
    	if (pet->in_room != NULL)
	{
 	    act( "$n slowly disappears.", pet, NULL, NULL, TO_ROOM );
	    extract_char(pet,TRUE);
	}
    }
    ch->pet = NULL;
}

void die_follower( CHAR_DATA *ch, bool remove )
{
    CHAR_DATA *fch, *fch_next;
    if ( ch->master != NULL )
    {
        if (ch->master->pet == ch)
            ch->master->pet = NULL;
        if (!IS_NPC(ch->master) && 
	    ch->master->pcdata->familiar == ch)
            ch->master->pcdata->familiar = NULL;
	if (remove)
            stop_follower( ch );
    }
    if ( ch->leader != NULL )
    {
      if (ch->leader->pet == ch)
	ch->leader->pet = NULL;
      if (!IS_NPC(ch->leader) && 
	  ch->leader->pcdata->familiar == ch)
	ch->leader->pcdata->familiar = NULL;
    }
    for ( fch = char_list; fch != NULL; fch = fch_next )
    {
	fch_next = fch->next;
	if ( fch->master == ch && (remove || IS_NPC(fch)))
	{
	    stop_follower( fch );
	    if (IS_NPC(fch))
	    {
	      if (IS_NPC(fch)){
		if (empty_char(fch))
		  act("$n spills some items on the ground.", fch, NULL, NULL, TO_ROOM);
		if (fch->in_room != NULL)
		  act( "$n slowly disappears.",fch,NULL,NULL,TO_ROOM );
		}
	      extract_char( fch, TRUE );
	    }
	}
	if ( fch->leader == ch && (remove || IS_NPC(fch)))
	    fch->leader = fch;
    }
}

void die_follower_pc( CHAR_DATA *ch)
{
    CHAR_DATA *fch, *fch_next;

    if (IS_NPC(ch))
      return;

    if ( ch->master != NULL ){
      stop_follower( ch );
    }
    for ( fch = char_list; fch != NULL; fch = fch_next )
    {
      fch_next = fch->next;
      if (IS_NPC(fch))
	continue;
      if ( fch->master == ch )
	stop_follower( fch );
      if ( fch->leader == ch)
	fch->leader = fch;
    }
}

void do_order( CHAR_DATA *ch, char *argument )
{
    int cmd;
    char arg[MIL], arg2[MIL];
    CHAR_DATA *victim, *och, *och_next;
    bool found, fAll;
    argument = one_argument( argument, arg );
    one_argument(argument,arg2);
    if (!str_cmp(arg2,"delete") || !str_cmp(arg2,"quit") || !str_cmp(arg2,"mp"))
    {
        send_to_char("That will NOT be done.\n\r",ch);
        return;
    }
    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	send_to_char( "Order whom to do what?\n\r", ch );
	return;
    }
    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
	send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
	return;
    }
    if ( !str_cmp( arg, "all" ) )
    {
	fAll   = TRUE;
	victim = NULL;
    }
    else
    {
	fAll   = FALSE;
	if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	{
	    send_to_char( "They aren't here.\n\r", ch );
	    return;
	}
	if ( victim == ch )
	{
	    send_to_char( "Aye aye, right away!\n\r", ch );
	    return;
	}
	if (!IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch || (IS_IMMORTAL(victim) && victim->trust >= ch->trust))
	{
	    send_to_char( "Do it yourself!\n\r", ch );
	    return;
	}
    }
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( arg2[0] == cmd_table[cmd].name[0]
        && !str_prefix( arg2, cmd_table[cmd].name ) )
            break;
    }
    found = FALSE;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
	och_next = och->next_in_room;
	if ( IS_AFFECTED(och, AFF_CHARM) && och->master == ch && ( fAll || och == victim ))
	{
	    found = TRUE;
	    if (!can_see(ch,och))
	      {
		send_to_char("You must see your target to order them around.\n\r", ch);
		continue;
	      }
	    sendf( och, "%s orders you to '%s'.\n\r", PERS(ch,och), argument );
	    if ( cmd_table[cmd].order == 0 )
		break;
	    
	    /* check for pc charm effect weakeaning */
	    {
	      AFFECT_DATA* paf = affect_find(och->affected, gen_pc_charm);
	      if (paf != NULL){
		if (number_percent() < 10 * paf->modifier){
		  affect_strip(och, gen_pc_charm);
		  continue;
		}
		paf->modifier ++;
	      }
	      /* end of pc charm check */
	    }
	    interpret( och, argument );
	}
    }
    if ( found )
	send_to_char( "Ok.\n\r", ch );
    else
	send_to_char( "You have no followers here.\n\r", ch );
}

void do_group( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	CHAR_DATA *gch;
	CHAR_DATA *leader;
	leader = (ch->leader != NULL) ? ch->leader : ch;
	sendf( ch, "%s's group:\n\r", PERS(leader, ch) );
	for ( gch = char_list; gch != NULL; gch = gch->next )
	    if ( is_same_group( gch, ch ) && !(!IS_NPC(gch) && IS_SET(gch->act2,PLR_NOWHO)))
	    {
		if (gch->max_hit == 0)
		    gch->max_hit++;
		if (gch->max_mana == 0)
		    gch->max_mana++;
		if (gch->max_move == 0)
		    gch->max_move++;
		sendf( ch,"[%2d %s] %-16s %4d%% hp %4d%% mana %4d%% mv %5ld xp\n\r",
		       gch->level, 
		       (IS_NPC(gch) ? "Mob" : (IS_SET(gch->act2, PLR_MASQUERADE) && ch != gch ? 
					       class_table[class_lookup("dark-knight")].who_name : 
					       class_table[gch->class].who_name)), 
		       capitalize( PERS(gch, ch) ),
		       gch->hit*100/gch->max_hit, 
		       gch->mana*100/gch->max_mana, 
		       gch->move*100/gch->max_move, 
		       gch->exp);
	    }
	return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }
    if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
    {
	send_to_char( "But you are following someone else!\n\r", ch );
	return;
    }
    if ( victim->master != ch && ch != victim )
    {
	sendf(ch,"%s isn't following you.\n\r",PERS2(victim));
	return;
    }
    if (IS_AFFECTED(victim,AFF_CHARM))
    {
        send_to_char("You can't remove charmed mobs from your group.\n\r",ch);
        return;
    }
    if (IS_AFFECTED(ch,AFF_CHARM))
    {
        act_new("You like your master too much to leave $m!",ch,NULL,victim,TO_VICT,POS_SLEEPING);
    	return;
    }
    if ( is_same_group( victim, ch ) && ch != victim )
    {
        if (!IS_NPC(ch) && !IS_NPC(victim) && (ch->pcdata->guarding == victim || victim->pcdata->guarded_by == ch))
	{
	    act("You stop guarding $N.", ch, NULL, victim, TO_CHAR);
	    act("$n stops guarding you.", ch, NULL, victim, TO_VICT);
	    act("$n stops guarding $N.", ch, NULL, victim, TO_NOTVICT);
	    victim->pcdata->guarded_by = NULL;
	    ch->pcdata->guarding       = NULL;
	}
	victim->leader = NULL;
	act_new("$n removes $N from $s group.",ch,NULL,victim,TO_NOTVICT,POS_RESTING);
        act_new("$n removes you from $s group.",ch,NULL,victim,TO_VICT,POS_SLEEPING);
        act_new("You remove $N from your group.",ch,NULL,victim,TO_CHAR,POS_SLEEPING);
	if (IS_NPC(victim))
	{
	   if (victim->in_room != NULL)
                act( "$n slowly disappears.",victim,NULL,NULL,TO_ROOM );
           extract_char( victim, TRUE );
	}
        if (!IS_NPC(victim) && !IS_NPC(ch) && victim->pcdata->guarded_by != NULL && !is_same_group(victim,victim->pcdata->guarded_by))
	{
	    act("You stop guarding $N.",victim->pcdata->guarded_by,NULL,victim,TO_CHAR);
	    act("$n stops guarding you.",victim->pcdata->guarded_by,NULL,victim,TO_VICT);
	    act("$n stops guarding $N.",victim->pcdata->guarded_by,NULL,victim,TO_NOTVICT);
	    victim->pcdata->guarded_by->pcdata->guarding = NULL;
	    victim->pcdata->guarded_by           = NULL;
	}
	return;
    }
    if ( ch->level - victim->level >= 9 && get_trust(ch) < LEVEL_IMMORTAL)
    {
        sendf(ch, "%s is too inexperienced for you.\n\r",PERS2(victim));
        return;
    }
    else if ( victim->level - ch->level >= 9 )
    {
        sendf(ch, "%s is too experienced for you.\n\r",PERS2(victim));
        return;
    }
    if ( (ch->alignment == 750 && victim->alignment == -750) || (ch->alignment == -750 && victim->alignment == 750) )
    {
        sendf(ch, "%s is too different from you.\n\r",PERS2(victim));
        return;
    }

    victim->leader = ch;
    act_new("$N joins $n's group.",ch,NULL,victim,TO_NOTVICT,POS_RESTING);
    act_new("You join $n's group.",ch,NULL,victim,TO_VICT,POS_SLEEPING);
    act_new("$N joins your group.",ch,NULL,victim,TO_CHAR,POS_SLEEPING);
}

void do_split( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *gch;
    int members = 0, amount, share, extra;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Split how much?\n\r", ch );
	return;
    }
    amount = atoi( arg );
    if ( amount < 0 )
    {
	send_to_char( "Your group wouldn't like that.\n\r", ch );
	return;
    }
    if ( amount == 0 )
    {
	send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
	return;
    }
    if ( ch->gold < amount )
    {
	send_to_char( "You don't have that much gold.\n\r", ch );
	return;
    }
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	if ( is_same_group( gch, ch ) && !IS_AFFECTED(gch,AFF_CHARM))
	    members++;
    if ( members < 2 )
    {
	send_to_char( "Just keep it all.\n\r", ch );
	return;
    }
    share = amount / members;
    extra = amount % members;
    if ( share == 0 )
    {
	send_to_char( "Don't even bother, cheapskate.\n\r", ch );
	return;
    }
    ch->gold -= amount;
    ch->gold += share + extra;
    sendf( ch, "You split %d gold coins.  Your share is %d gold coins.\n\r",amount, share + extra );
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	if ( gch != ch && is_same_group( gch, ch ) && !IS_AFFECTED(gch,AFF_CHARM))
	{
            sendf( gch, "%s splits %d gold coins.  Your share is %d gold coins.\n\r", PERS2(ch), amount, share );
	    if (gch->class == class_lookup("monk") && (gch->gold + share) > 20000)
	    {
	      //int overflow = UMAX(0,gch->gold + share - 20000);
		act("$N can't hold that much gold so keep it.",ch,NULL,gch,TO_CHAR);
        	send_to_char("You can't hold that much gold.\n\r",gch);
	        gch->gold = 20000;
	    }
	    else
	      gch->gold += share;
	}
}

void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;
    char buf[MSL];
    char arg_buf[MSL];

    const int demon_cost = 30;	
    bool fDemon = IS_TELEPATH(ch);

    if ( argument[0] == '\0' )
    {
	send_to_char( "Tell your group what?\n\r", ch );
	return;
    }
    argument[0] = UPPER(argument[0]);

    if ( !IS_NPC(ch) && IS_SET( ch->comm, COMM_NOTELL ) )
    {
	send_to_char( "Your message didn't get through!\n\r", ch );
	return;
    }

    if ( IS_NPC(ch) && ch->master != NULL && (IS_SET( ch->master->comm, COMM_NOTELL ) || IS_AFFECTED2(ch->master, AFF_SILENCE) ))
	return;

    if (IS_AFFECTED2(ch, AFF_SILENCE) && !fDemon)
    {
	send_to_char("You work your mouth, but no sounds escape your lips.\n\r",ch);
	return;
    }
    if (is_affected(ch, gsn_gag) && !fDemon)
    {
        send_to_char("You find difficultly talking.\n\r",ch);
        return;
    }

    if (is_affected(ch, gsn_uppercut) && !fDemon)
    {
        send_to_char("Your jaws hurt too much to talk.\n\r",ch);
        return;
    }
    strcpy(arg_buf, argument);
    makedrunk(argument,ch);

    if (fDemon)
      {
	if (can_demon_talk(ch, demon_cost))
	  sendf(ch, "You broadcast '`9%s``'\n\r",argument);
	else
	  return;
      }
    else
      sendf(ch, "You tell your group '`9%s``'\n\r",argument);

    for ( gch = char_list; gch != NULL; gch = gch->next )
      if ( is_same_group( gch, ch ) 
	    && ((!is_affected(gch,gsn_silence) 
		&& !IS_AFFECTED2(gch, AFF_SILENCE))
		|| fDemon)
	   )
	{
	  if (fDemon)
	    {
	      sprintf(buf,"[`8%s``] '`9$t``'",PERS(ch,gch));
	      act_new( buf, ch, is_affected(gch, gsn_com_lan)? arg_buf : IS_IMMORTAL(gch) ? arg_buf : argument,
		       gch, TO_VICT, POS_DEAD);
	    }
	  else
	    {
	      sprintf(buf, "%s tells the group '`9$t``'", PERS(ch,gch));
	      act_new( buf, ch, is_affected(gch, gsn_com_lan)? arg_buf : IS_IMMORTAL(gch) ? arg_buf : argument, 
		       gch, TO_VICT, POS_DEAD);
	    }

	  if (!IS_NPC(gch) && gch->pcdata->eavesdropped != NULL
	      && is_affected(gch->pcdata->eavesdropped,gsn_eavesdrop)
	      && gch != ch && gch->pcdata->eavesdropped != ch 
	      && !is_same_group(gch->pcdata->eavesdropped,gch)
	      && !fDemon)
	    {
	      send_to_char("A faint message transmits from the spies.\n\r",gch->pcdata->eavesdropped);
	      sprintf(buf,"Someone tells the group '`9$t``'");
	      act_new(buf,gch->pcdata->eavesdropped,is_affected(gch->pcdata->eavesdropped, gsn_com_lan)? arg_buf : IS_IMMORTAL(gch->pcdata->eavesdropped) ? arg_buf : argument, 
		  NULL,TO_CHAR, POS_DEAD);
	    }
	}
    if (!IS_NPC(ch) && ch->pcdata->eavesdropped != NULL
	&& is_affected(ch->pcdata->eavesdropped,gsn_eavesdrop)
	&& !fDemon)
      {
	send_to_char("A faint message transmits from the spies.\n\r",ch->pcdata->eavesdropped);
	sprintf(buf,"%s tells the group '`9$t``'",PERS(ch,ch->pcdata->eavesdropped));
	act_new(buf,ch->pcdata->eavesdropped,is_affected(ch->pcdata->eavesdropped, gsn_com_lan)? arg_buf : IS_IMMORTAL(ch->pcdata->eavesdropped) ? arg_buf : argument, 
	    NULL,TO_CHAR, POS_DEAD);
      }
}


/*  It is very important that this be an equivalence relation: *
 *  (1) A ~ A                                                  *
 *  (2) if A ~ B then B ~ A                                    *
 *  (3) if A ~ B  and B ~ C, then A ~ C                        */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach == NULL || bch == NULL)
	return FALSE;
    if ( ach->leader != NULL )
	ach = ach->leader;
    if ( bch->leader != NULL )
	bch = bch->leader;
    return ach == bch;
}

void do_release(CHAR_DATA *ch, char *argument)
{
    char arg[MIL];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if (IS_NPC(ch))
	return;
    if (arg[0] == '\0')
    {
        send_to_char("Release whom?\n\r",ch);
        return;
    }
    if ((victim = get_char_room(ch, NULL, argument)) == NULL)
    {
        send_to_char("They're not here.\n\r",ch);
        return;
    }
    if (!IS_NPC(victim))
    {
        send_to_char("You can't release another player.\n\r", ch);
        return;
    }
    if (victim->master != ch || victim->leader != ch)
      {
	send_to_char("You have no power over that mob.\n\r", ch);
        return;
      }
    act("$N is released by $n!", ch, NULL, victim, TO_NOTVICT);
    act("You send $N back home.", ch, NULL, victim, TO_CHAR);
    act("You've been released!", ch, NULL, victim, TO_VICT);
    act("$N slowly fades away.",ch,NULL,victim,TO_ALL);
    
    if (is_affected(ch,gsn_familiar_link) && ch->pcdata->familiar != NULL && ch->pcdata->familiar == victim)
    {
	AFFECT_DATA af;
        send_to_char("Your energy drains out as you lose link with your familiar.\n\r",ch);
        affect_strip(ch,gsn_familiar_link);
        ch->pcdata->familiar=NULL;
	af.type		= gsn_drained;
	af.level	= ch->level;
	af.duration	= 12;
	af.modifier	= 0;
	af.location	= 0;
	af.bitvector	= 0;
	affect_to_char(ch,&af);
    }
    extract_char(victim, TRUE);
}

/* converts the str to drunk talk */
void makedrunk (char *string, CHAR_DATA * ch){
  struct struckdrunk drunk[] =
  {
    {3, 9, {"a", "a", "a", "A", "aa", "ah", "Ah", "ao", "aw", "ahh"}},
    {8, 5,  {"b", "b", "b", "B", "B", "vb"}},
    {3, 5,  {"c", "c", "C", "cj", "sj", "zj"}},
    {5, 2,  {"d", "d", "D"}},
    {3, 4,  {"e", "e", "eh", "E", "Eh"}},
    {4, 4,  {"f", "f", "F", "ff", "fF"}},
    {8, 3,  {"g", "g", "G", "gh"}},
    {9, 5,  {"h", "h", "H", "hh", "Hh", "hH"}},
    {7, 6,  {"i", "i", "I", "ii", "ii", "iI", "Ii"}},
    {9, 5,  {"j", "j", "J", "jj", "Jj", "jJ"}},
    {7, 3,  {"k", "k", "K", "ck"}},
    {3, 2,  {"l", "l", "L"}},
    {5, 6,  {"m", "m", "M", "mm", "mM", "mmm", "MmM"}},
    {6, 6,  {"n", "n", "N", "nn", "Nn", "nnn", "nNn"}},
    {3, 6,  {"o", "o", "O", "ao", "oo", "Oo", "oOo"}},
    {3, 2,  {"p", "p", "P"}},
    {5, 4,  {"q", "q", "Q", "qu", "ku"}},
    {4, 2,  {"r", "r", "R"}},
    {2, 6,  {"s", "ss", "sSs", "szs", "ZSs", "sSz", "Zsz"}},
    {5, 2,  {"t", "t", "T"}},
    {3, 6,  {"u", "u", "uh", "U", "Uh", "uhh", "uhH"}},
    {4, 2,  {"v", "v", "V"}},
    {4, 2,  {"w", "w", "W"}},
    {5, 6,  {"x", "x", "X", "xz", "kz", "kx", "iks"}},
    {3, 2,  {"y", "y", "Y"}},
    {2, 7,  {"z", "z", "Z", "zZ", "szz", "ZSz", "Zzz", "sZz"}}
  };
  char buf[MSL];
  
  char* str;//current place in the string
  char* point;//current place inthe buffer
  
  int drunklevel, randomnum, count;

  if (IS_TELEPATH(ch))
    return;

/* ANNOY */
  if (!IS_NPC(ch) && IS_SET(ch->act2,PLR_ANNOY)){
    //make random characters in string capital
    str = string;
    while(*str != '\0')
      {
	if (number_bits(1) == 0)
	  *str= UPPER(*str);
	str++;
      }
  }

    
  if (is_affected(ch,gsn_garble) || (!IS_NPC(ch) && IS_SET(ch->act2,PLR_GARBLE))){
    
    str = string;
    
    while (*str != '\0'){
      if ((*str >= 'A') && (*str <= 'Z'))
	*str++ = 'A' + number_range (0,25);
      else if ((*str >= 'a') && (*str <= 'z'))
	*str++ = 'a' + number_range (0,25);
      else if ((*str >= '0') && (*str <= '9'))
	*str++ = '0' + number_range (0, 9);
      else
	str++;
    }
  }

  /*Ogres talk drunk */
  if (IS_NPC(ch))
    drunklevel = 0;
  else if (is_affected(ch,gsn_drunken) 
	   || is_affected(ch, gsn_brain_death)
	   || ch->race == race_lookup("Ogre"))
    drunklevel = UMAX(10,ch->pcdata->condition[COND_DRUNK]);
  else
    drunklevel = ch->pcdata->condition[COND_DRUNK];

  if (drunklevel > 0){
    if (drunklevel > 10)
      drunklevel = 10 + (drunklevel-10)/2;
	
    //begin converting string to drunk talk.
    str = string;
    point = buf;//we need buffer as the result might be longer then original.
	
    while (*str != '\0'){
      if (UPPER(*str) >= 'A' && UPPER(*str) <= 'Z'){
	if (drunklevel > drunk[*str - 'A'].min_drunk_level){
	  randomnum = number_range (0, drunk[UPPER(*str) - 'A'].number_of_rep);
	  strcpy(point, drunk[UPPER(*str) - 'A'].replacement[randomnum]);
	  point += strlen(drunk[UPPER(*str) - 'A'].replacement[randomnum]);
	}
	else
	  *point++ = *str;
      }
      else if (*str == '.'){
	randomnum = number_range(1,drunklevel/5);
	for (count = 0; count < 3 && count < randomnum; count++)
	  *point++ = *str;
      }
      else if (*str == '%')
	*point++ = *str;
      else if (*str >= '0' && *str <= '9')
	*point++ = '0' + number_range (0, 9);
      else
	*point++ = *str;
      str++;
    }//end while
    *point = '\0';
    strncpy(string, buf, MIL);
  }//end if drunk
}

void do_lastname( CHAR_DATA *ch, char *argument)
{
    char arg1[MSL], arg2[MSL];
    smash_tilde(argument);
    argument = one_argument_2(argument,arg1);
    argument = one_argument_2(argument,arg2);
    if (IS_NPC(ch))
	return;
    if (ch->pcdata->family[0] != '\0')
    {
	send_to_char("You must petition to the Gods for a last name change.\n\r",ch);
	return;
    }
    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
	send_to_char("Syntax: lastname <lastname> <lastname>\n\r",ch);
	return;
    }
    if (str_cmp_2(arg1,arg2))
    {
        send_to_char("Last name doesn't match.\n\r",ch);
	return;
    }
    if ( strlen(arg1) > 12 )
    {
        send_to_char("Last name must be less than twelve characters long.\n\r",ch);
        return;
    }
    if ( !check_parse_name( arg1 ) || check_exist_name(arg1) )
    {
        send_to_char( "Illegal last name, try another.\n\r",ch);
	return;
    }
    free_string( ch->pcdata->family );
    ch->pcdata->family = str_dup( arg1 );
    sendf(ch, "Last name changed to %s.\n\r",ch->pcdata->family);
}

void do_immpass( CHAR_DATA *ch, char *argument)
{
    char arg1[MSL], arg2[MSL];
    char *argnew;
    smash_tilde(argument);
    argument = one_argument_2(argument,arg1);
    argument = one_argument_2(argument,arg2);
    if (IS_NPC(ch) || !IS_IMMORTAL(ch))
	return;
    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
	send_to_char("Syntax: immpass <password> <password>\n\r",ch);
	return;
    }
    if (str_cmp_2(arg1,arg2))
    {
        send_to_char("Passwords doesn't match.\n\r",ch);
	return;
    }
    if ( strlen(arg1) < 5 )
    {
        send_to_char("Password must be at least 5 characters long.\n\r",ch);
        return;
    }
    free_string( ch->pcdata->pwdimm );
    argnew = crypt( arg1, ch->name );
    ch->pcdata->pwdimm = str_dup( argnew );
    save_char_obj( ch );
    send_to_char("Password set.\n\r",ch);
}

void do_delpass( CHAR_DATA *ch, char *argument)
{
    char arg1[MSL], arg2[MSL];
    char *pwdnew, *p;
    smash_tilde(argument);
    argument = one_argument_2(argument,arg1);
    argument = one_argument_2(argument,arg2);
    if (IS_NPC(ch))
	return;
    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
	send_to_char("Syntax: delpass <current password> <new password>\n\r",ch);
	return;
    }
    if ( !IS_NULLSTR(ch->pcdata->pwddel)
	 && strcmp( crypt( arg1, ch->name ), ch->pcdata->pwddel )
	 && strcmp( crypt( arg1, ch->pcdata->pwddel ), ch->pcdata->pwddel ) )
    {
	WAIT_STATE( ch, 40 );
	send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
	return;
    }
    if ( strlen(arg2) < 5 )
    {
        send_to_char("New password must be at least five characters long.\n\r", ch );
	return;
    }
    pwdnew = crypt( arg2, ch->name );
    for ( p = pwdnew; *p != '\0'; p++ )
      if ( *p == '~' )
	{
	  send_to_char("New password not acceptable, try again.\n\r", ch );
	  return;
	}
    free_string( ch->pcdata->pwddel );
    ch->pcdata->pwddel = str_dup( pwdnew );
    save_char_obj( ch );
    send_to_char( "New password set.\n\r", ch );
}

bool check_exist_name( char *name )
{
    struct stat *buf;
    char path[MIL];
    sprintf( path, "%s%s", PLAYER_DIR, capitalize( name ) );
    init_malloc("check_exist_name");
    buf=malloc(sizeof(struct stat));
    if ( !(stat( path,buf )) )
    {
	free(buf);
	end_malloc("check_exist_name");
	return TRUE;
    }
    free(buf);
    end_malloc("check_exist_name");
    return FALSE;
}

void eq_purge( CHAR_DATA *ch )
{
    OBJ_DATA *obj, *obj_next;
    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
	extract_obj(obj);
    }
}

void deny_record( CHAR_DATA *ch ){
  char strsave[MIL];
  char nameread[MIL];
  
  const int max_name = 512;
  char names[max_name] [MIL];
  int last_name = 0;
  int top_name = 0;

  FILE *fp;
  
  if ( ch->desc != NULL && ch->desc->original != NULL )
    ch = ch->desc->original;
    
  fclose( fpReserve );
  sprintf( strsave, "%s", ILLEGAL_NAME_FILE );

  if ( (fp = fopen( strsave, "r" ) ) != NULL ){
      /* read in the names, looping over to start if neccessary */
    for ( ; ; ){
      /* loop over if we are at max name */
      if (last_name >= max_name)
	last_name = 0;

      fscanf (fp, "%s", nameread);
      if ( !str_cmp( nameread, "END" ) )
	break;
      else{
	strcpy(names[last_name++], nameread);
	if (last_name > top_name)
	  top_name = last_name;
      }
    }
  }
  else
    fp = fopen( NULL_FILE, "r" );
  fclose( fp );
  fp = fopen( strsave, "w" );
  
  for ( last_name = 0; last_name < top_name; last_name++){
    fprintf( fp, "%s\n", names[last_name] );
  }
  fprintf( fp, "%s\n", ch->name );
  fprintf( fp, "END" );
  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
}

/* REMOVE ME LATER */
extern AREA_QUEUE* generate_area_list( AREA_DATA *src, AREA_DATA *target);
void print_list( CHAR_DATA* ch, PATH_QUEUE* curr ){
  if (curr == NULL)
    return;
  sendf( ch, "%-8.8s to %d name %s\n\r",
	 dir_name[(int)curr->from_dir],
	 curr->room->vnum,
	 curr->room->name);
  print_list( ch, curr->next );
}


void do_test( CHAR_DATA *ch, char *argument){

  /*  FILE *fp;
  OBJ_INDEX_DATA* pIndex;
  char path[MIL];
  int index;
  int count = 0;

  sprintf(path, "%s", "item_db.txt");
  fclose( fpReserve );

  if ( (fp = fopen( path, "w" )) == NULL){
    fpReserve = fopen( NULL_FILE, "r" );
    return;
  }

  for (index = 0; count < top_obj_index; index++){
    if ( (pIndex = get_obj_index( index)) == NULL){
      continue;
    }
    count++;
    if (pIndex->item_type != ITEM_WEAPON)
      continue;
    dump_object( fp, pIndex );
  }

  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
  */

  MOB_INDEX_DATA* pIndex;
  int index;
  int count = 0;

  for (index = 0; count < top_mob_index; index++){
    if ( (pIndex = get_mob_index( index)) == NULL)
      continue;
    count++;
    if (IS_SET(pIndex->act, ACT_UNDEAD) 
	|| pIndex->level < 50
	|| pIndex->level > 52
	)
      continue;
    if (!IS_SET(pIndex->off_flags, OFF_BASH)
	|| !IS_SET(pIndex->off_flags, OFF_TRIP)
	|| !IS_SET(pIndex->off_flags, OFF_DISARM)
	|| !IS_SET(pIndex->off_flags, OFF_KICK_DIRT)
	)
      continue;
    /*
    if (!IS_SET(pIndex->act,  ACT_WARRIOR))
      continue;
    */
    sendf(ch, "[%-6d] Lvl%-2d %s\n\r",
	  pIndex->vnum,
	  pIndex->level,
	  pIndex->short_descr);
  }
  /*

  OBJ_INDEX_DATA* pIndex;
  AFFECT_DATA* paf;
  FILE* fp;
  //char buf[MIL];
  int index;
  int count = 0;
  bool fFound = FALSE;

  if ( (fp = fopen("dump.txt", "w")) == NULL)
    return;

  for (index = 0; count < top_obj_index; index++){
    int hit = 0;
    int dam = 0;
    int mana = 0;
    if ( (pIndex = get_obj_index( index)) == NULL)
      continue;
    count++;

    if (pIndex->item_type != ITEM_WEAPON || !IS_WEAPON_STAT(pIndex, WEAPON_TWO_HANDS))
      continue;
    else if (pIndex->value[0] != WEAPON_POLEARM)
      continue;
    else if (attack_table[pIndex->value[3]].damage <= DAM_SLASH)
      continue;
    
    for (paf = pIndex->affected; paf; paf = paf->next){
      if (paf->location == APPLY_HITROLL)
	hit += paf->modifier;
      if (paf->location == APPLY_DAMROLL)
	dam += paf->modifier;
      if (paf->location == APPLY_MANA)
	mana += paf->modifier;
    }
    
    fFound = TRUE;
    sendf(ch, "%-6d %-25.25s %-5d %-15.15s %s\n",
	    pIndex->vnum,
	    pIndex->short_descr,
	    mana,
	    flag_string( wear_flags,  pIndex->wear_flags & ~ITEM_TAKE & ~ITEM_NO_SAC & ~ITEM_WEAR_SECONDARY & ~ITEM_RARE & ~ITEM_UNIQUE & ~ITEM_PARRY),
	    pIndex->area->name);
  }
  */
/*  OBJ_INDEX_DATA* pIndex;
  OBJ_DATA* obj;
  AFFECT_DATA* paf;
  char buf[MIL];
  int index;
  int count = 0;

  for (index = 0; count < top_obj_index; index++){
    if ( (pIndex = get_obj_index( index)) == NULL)
      continue;
    count++;
    if (pIndex->item_type != ITEM_HERB || pIndex->value[4] < 1)
      continue;
    sendf(ch, "[%-6d]\t%-20.20s\t%-20.20s\t(%s)\n\r",
	  pIndex->vnum,
	  skill_table[pIndex->value[4]].name,
	  pIndex->short_descr,
	  pIndex->area->name);
  }
*/
/*
  int i;
  int min, max, bon, man;

  for (i = 0; i < MAX_PC_RACE; i++){
    if (!pc_race_table[i].can_pick)
      continue;

    bon = int_app[pc_race_table[i].max_stats[STAT_INT]].manap;
    bon += wis_app[pc_race_table[i].max_stats[STAT_WIS]].manap;
    min = 6 + bon;
    max = 12 + bon;
    man = 50 * (min + (max - min) / 2);
    man += pc_race_table[i].mana;
    //race, int, wis, min, max, bon, min, max, avg
    sendf( ch, "%-15.15s %4d %4d %4d %4d %4d %4d %4d %4d\n\r", 
	   pc_race_table[i].name,
	   pc_race_table[i].max_stats[STAT_INT],
	   pc_race_table[i].max_stats[STAT_WIS],
	   min,
	   max, 
	   min + (max - min) / 2,
	   pc_race_table[i].mana + min * 50,
	   pc_race_table[i].mana + max * 50,
	   man);
	   
  }
*/
/*
  MOB_INDEX_DATA* pIndex;
  int index;
  int count = 0;

  for (index = 0; count < top_mob_index; index++){
    if ( (pIndex = get_mob_index( index)) == NULL)
      continue;
    count++;
    if (IS_SET(pIndex->act, ACT_UNDEAD) && IS_SET(pIndex->act, ACT_MOUNT) ){
      sendf(ch, "[%-6d] Lvl%-2d %s\n\r",
	    pIndex->vnum,
	    pIndex->level,
	    pIndex->short_descr);
    }
  }
*/
}

bool lookup_history( CHAR_DATA *ch, char *argument )
{
    char strsave[MIL], buf[MSL], *desc;
    FILE *fp;
    if ( argument[0] == '\0' )
        return FALSE;
    fclose( fpReserve );
    sprintf( strsave, "%s%s", HISTORY_DIR, capitalize( argument ) );
    if ( ( fp = fopen( strsave, "r" ) ) == NULL )
    {
        fp = fopen( NULL_FILE, "r" );
        fclose (fp);
        fpReserve = fopen( NULL_FILE, "r" );
        return FALSE;
    }
    sprintf(buf,"You recite the history of %s:",capitalize( argument ));
    act(buf,ch,NULL,NULL,TO_CHAR);
    sprintf(buf,"$n recites the history of %s:",capitalize( argument ));
    act(buf,ch,NULL,NULL,TO_ROOM);
    for ( ; ; )
    {
	desc  = feof( fp ) ? "END" : fread_string_eol( fp );
        if ( !str_cmp( desc, "END" ) )
            break;
	sprintf(buf,"`#%s``",desc);
	act(buf,ch,NULL,NULL,TO_ALL);
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return TRUE;
}

void do_history( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    one_argument (argument, arg);
    if (!IS_IMMORTAL(ch))
    {
	send_to_char("You do not know anything of that sort.\n\r",ch);
	return;
    }
    if ( arg[0] == '\0' )
    {
        send_to_char( "Find information on who's history?\n\r", ch );
        return;
    }
    if (!lookup_history(ch,arg))
    {
        send_to_char ("You cannot find any known history on that character.\n\r", ch);
        return;
    }
}


void list_history( CHAR_DATA *ch, char *argument )
{
    char buf[MSL], command[MSL], *name;
    FILE *fp;
    if (argument[0] != '\0')
    {
	sprintf(command,"ls ~/ar/history/%s", argument);
    	sprintf(buf,"Here are a list of the '%s' people that I have information on:",argument);
    }
    else
    {
	sprintf(command,"ls ~/ar/history");
    	sprintf(buf,"Here are a list of all the information I have:");
    }
    do_say(ch,buf);
    fp = popen( command, "r" );
    for (;;)
    {
        name = feof( fp ) ? "ZZEND" : fread_word( fp );
        if ( !str_cmp( name, "ZZEND" ) || name == NULL)
            break;
        sprintf(buf,"%s",name);
	str_replace(buf,"/home/mudgroup/ar/history/","");
        do_say(ch,buf);
    }
    pclose( fp );
}

//2001-02-18 Ath: Added do_bounties to show top-10 bounties
//2001-02-18 Ath: Added section to do_bounty to add to top-10 file
//2001-05-21 Ath: Added spectator sections (for room watching)
