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


/***************************************************************************
*	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  Based on MERC 2.2 MOBprograms by N'Atas-ha.                            *
 *  Written and adapted to ROM 2.4 by                                      *
 *          Markku Nylander (markku.nylander@uta.fi)                       *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "cabal.h"
#include "mob_cmds.h"


DECLARE_DO_FUN( do_look 	);
extern char *target_name;
extern void do_old_mpstat( CHAR_DATA *ch, char *argument );
extern ROOM_INDEX_DATA *find_location( CHAR_DATA *, char * );
extern void do_permquest(CHAR_DATA* ch, char* argument);
extern void do_inviquest(CHAR_DATA* ch, char* argument);
extern void do_tempquest(CHAR_DATA* ch, char* argument);
extern void do_remquest(CHAR_DATA* ch, char* argument);

extern void do_addpsalm(CHAR_DATA* ch, char* argument);
extern void do_rempsalm(CHAR_DATA* ch, char* argument);

extern const char * name_expand (CHAR_DATA *ch);
/*
 * MOBcommand table.
 */
const	struct	mob_cmd_type	mob_cmd_table	[] =
{
    {	"asound", 	do_mpasound	},
    {	"gecho",	do_mpgecho	},
    {	"zecho",	do_mpzecho	},
    {	"kill",		do_mpkill	},
    {	"assist",	do_mpassist	},
    {	"junk",		do_mpjunk	},
    {	"echo",		do_mpecho	},
    {	"echoaround",	do_mpechoaround	},
    {	"echoat",	do_mpechoat	},
    {	"mload",	do_mpmload	},
    {	"oload",	do_mpoload	},
    {	"purge",	do_mppurge	},
    {	"goto",		do_mpgoto	},
    {	"at",		do_mpat		},
    {	"transfer",	do_mptransfer	},
    {	"gtransfer",	do_mpgtransfer	},
    {	"otransfer",	do_mpotransfer	},
    {	"rtransfer",	do_mprtransfer	},
    {	"force",	do_mpforce	},
    {	"gforce",	do_mpgforce	},
    {	"vforce",	do_mpvforce	},
    {	"cast",		do_mpcast	},
    {	"damage",	do_mpdamage	},
    {	"remember",	do_mpremember	},
    {	"forget",	do_mpforget	},
    {	"delay",	do_mpdelay	},
    {	"cancel",	do_mpcancel	},
    {	"call",		do_mpcall	},
    {	"flee",		do_mpflee	},
    {	"remove",	do_mpremove	},
    {	"stop",		do_mpstop	},
    {	"obey",		do_mpobey	},
    {	"attrib",	do_mpattrib 	},
    {	"rattrib",	do_mprattrib 	},
    {	"gain",		do_mpgain 	},
    {	"permquest",	do_mppermquest 	},
    {	"inviquest",	do_mpinviquest 	},
    {	"tempquest",	do_mptempquest 	},
    {	"remquest",	do_mpremquest 	},
    {	"addpsalm",	do_mpaddpsalm 	},
    {	"rempsalm",	do_mprempsalm 	},
    {	"pfor",		do_mpfor 	},
    {	"chown",	do_mpchown 	},
    {	"mount",	do_mpmount 	},
    {	"destroy",	do_mpdestroy 	},
    {	"path",		do_mppath 	},
    {	"aquest",	do_mpaquest 	},
    {	"cquest",	do_mpcquest 	},
    {	"",		0		}
};

/*
 * OBJcommand table.
 */
const	struct	obj_cmd_type	obj_cmd_table	[] =
{
    {	"gecho",       	do_opgecho	},
    {	"zecho",	do_opzecho	},
    {	"echo",		do_opecho	},
    {	"echoaround",	do_opechoaround	},
    {	"echoat",	do_opechoat	},
    {	"mload",	do_opmload	},
    {	"oload",	do_opoload	},
    {	"purge",	do_oppurge	},
    {	"goto",		do_opgoto	},
    {	"transfer",	do_optransfer	},
    {	"gtransfer",	do_opgtransfer	},
    {	"rtransfer",	do_oprtransfer	},
    {	"otransfer",	do_opotransfer	},
    {	"force",	do_opforce	},
    {	"gforce",	do_opgforce	},
    {	"vforce",	do_opvforce	},
    {	"damage",	do_opdamage	},
    {	"remember",	do_opremember	},
    {	"forget",	do_opforget	},
    {	"delay",	do_opdelay	},
    {	"cancel",	do_opcancel	},
    {	"call",		do_opcall	},
    {	"remove",	do_opremove	},
    {	"attrib",	do_opattrib 	},
    {	"rattrib",	do_oprattrib 	},
    {	"cast",		do_opcast	},
    {	"obey",		do_opobey	},
    {	"equipself",	do_opequipself	},
    {	"stop",		do_opstop	},
    {	"gain",		do_opgain	},
    {	"permquest",	do_oppermquest 	},
    {	"inviquest",	do_opinviquest 	},
    {	"tempquest",	do_optempquest 	},
    {	"remquest",	do_opremquest 	},
    {	"addpsalm",	do_opaddpsalm 	},
    {	"rempsalm",	do_oprempsalm 	},
    {	"pfor",		do_opfor 	},
    {	"affstrip",	do_opaffstrip 	},
    {	"mount",	do_opmount 	},
    {	"scoreboard",	do_opscoreboard	},
    {	"destroy",	do_opdestroy 	},
    {	"path",		do_oppath 	},
    {	"",		0		}
};

/*
 * ROOMcommand table.
 */
const	struct  room_cmd_type	room_cmd_table	[] =
{
    {	"asound",	do_rpasound	},
    {	"gecho",	do_rpgecho	},
    {	"zecho",	do_rpzecho	},
    {	"echo",		do_rpecho 	},
    {	"echoaround",	do_rpechoaround },
    {	"echoat",	do_rpechoat 	},
    {	"mload",	do_rpmload 	},
    {	"oload",	do_rpoload 	},
    {	"purge",	do_rppurge 	},
    {	"transfer",	do_rptransfer 	},
    {	"gtransfer",	do_rpgtransfer 	},
    {	"rtransfer",	do_rprtransfer	},
    {	"otransfer",	do_rpotransfer 	},
    {	"force",	do_rpforce 	},
    {	"gforce",	do_rpgforce 	},
    {	"vforce",	do_rpvforce 	},
    {	"damage",       do_rpdamage 	},
    {	"remember",	do_rpremember 	},
    {	"forget",	do_rpforget 	},
    {	"delay",	do_rpdelay 	},
    {	"cancel",	do_rpcancel 	},
    {	"call",		do_rpcall 	},
    {	"remove",	do_rpremove 	},
    {	"obey",		do_rpobey	},
    {	"stop",		do_rpstop	},
    {	"gain",		do_rpgain	},
    {	"permquest",	do_rppermquest 	},
    {	"inviquest",	do_rpinviquest 	},
    {	"tempquest",	do_rptempquest 	},
    {	"remquest",	do_rpremquest 	},
    {	"addpsalm",	do_rpaddpsalm 	},
    {	"addpsalm",	do_rprempsalm 	},
    {	"pfor",		do_rpfor 	},
    {	"rattrib",	do_rprattrib 	},
    {	"mount",	do_rpmount 	},
    {	"destroy",	do_rpdestroy 	},
    {	"path",		do_rppath 	},
    {	"",		0 		},
};

char *prog_type_to_name( int type )
{
    switch ( type )
    {
    case TRIG_ACT:             	return "ACT";
    case TRIG_SPEECH:          	return "SPEECH";
    case TRIG_RANDOM:          	return "RANDOM";
    case TRIG_FIGHT:           	return "FIGHT";
    case TRIG_HPCNT:           	return "HPCNT";
    case TRIG_DEATH:           	return "DEATH";
    case TRIG_ENTRY:           	return "ENTRY";
    case TRIG_GREET:           	return "GREET";
    case TRIG_GRALL:        	return "GRALL";
    case TRIG_GIVE:            	return "GIVE";
    case TRIG_BRIBE:           	return "BRIBE";
    case TRIG_KILL:	      	return "KILL";
    case TRIG_DELAY:           	return "DELAY";
    case TRIG_SURR:	      	return "SURRENDER";
    case TRIG_EXIT:	      	return "EXIT";
    case TRIG_EXALL:	      	return "EXALL";
    case TRIG_GET:		return "GET";
    case TRIG_PUT:		return "PUT";
    case TRIG_DROP:		return "DROP";
    case TRIG_SIT:		return "SIT";
    case TRIG_TELL:          	return "TELL";
    case TRIG_USE:          	return "USE";
    case TRIG_SPECIAL:          return "SPECIAL";
    case TRIG_LOOK:		return "LOOK";
    case TRIG_SHOOT:		return "SHOOT";
    default:                  	return "ERROR";
    }
}



/* local used below */
bool change_doorflag(ROOM_INDEX_DATA* pRoom, char* value, int door, char oper){
  int val = 0;
  if ( ( val = flag_value( exit_flags, value ) ) != NO_FLAG ){
    ROOM_INDEX_DATA *pToRoom;
    sh_int rev;
    if ( !pRoom->exit[door] ){
      bug("change_doorflag: Door does not exist.", door);
      return FALSE;
    }
    switch (oper){
    case '+' :
      SET_BIT(pRoom->exit[door]->exit_info,  val);	break;
    case '-' :
      REMOVE_BIT(pRoom->exit[door]->exit_info,  val);	break;
    case '=' :
      pRoom->exit[door]->exit_info = val;		break;
    default:
      TOGGLE_BIT(pRoom->exit[door]->exit_info,  val);	break;
    }
    pToRoom = pRoom->exit[door]->to_room;
    if (pToRoom == NULL)
      return FALSE;
    rev = rev_dir[door];
    if (pToRoom->exit[rev] != NULL){
      switch (oper){
      case '+' :
	SET_BIT(pToRoom->exit[rev]->exit_info,  val);	break;
      case '-' :
	REMOVE_BIT(pToRoom->exit[rev]->exit_info,  val);	break;
      case '=' :
	pToRoom->exit[rev]->exit_info = val;		break;
      default:
	TOGGLE_BIT(pToRoom->exit[rev]->exit_info,  val);	break;
      }
    }
    return TRUE;
  }
  else
    bug("change_doorflag: Door flag does not exist.", door);
  return FALSE;
}



/* Allows to change room atrributes */
/* rattrib <attribute> <=,-,+,/,*> <value> [location] */
/* location is the vnum of room to affect */
/* when operating on bits the operators are as follows:
   '+'   bitset
   '-'   bitremove
   '='   set bit as the only value (clear then setbit)
   otherwise a bit toggle is performed
*/
void rattrib(ROOM_INDEX_DATA* room, char* argument){
  ROOM_INDEX_DATA* pRoom = room;

  char attrib[MIL];
  char oper[MIL];
  char value[MIL];

  int val = 0;

  if (IS_NULLSTR(argument))
    return;

  argument = one_argument(argument, attrib);
  argument = one_argument(argument, oper);
  argument = one_argument(argument, value);



  /* get the location */
  if (!IS_NULLSTR(argument)){
    int loc = atoi(argument);
    if ( (pRoom = get_room_index( loc )) == NULL){
      bug("rattrib: could not find location passed.", val);
      return;
    }
  }
  if (pRoom == NULL){
    bug("rattrib: could not find location passed (2).", val);
    return;
  }

  /* get the value */
  if ( (val = atoi(value)) < 0){
    bug("rattrib: value passed was not an integer.", val);
    return;
  }

  /* do the actual changing and testing of the room stats*/
  if (!str_cmp(attrib, "name")){
    free_string( pRoom->name );
    pRoom->name = str_dup( value );
    return;
  }
  else if (!str_cmp(attrib, "sector")){
    if ( ( val = flag_value( sector_flags, value ) ) != NO_FLAG )
      pRoom->sector_type = val;
    else
      bug("rattrib: No such sector flag.", val);
    return;
  }
  else if (!str_cmp(attrib, "room")){
    if ( ( val = flag_value( room_flags, value ) ) != NO_FLAG ){
      switch (oper[0]){
      case '+' :
	SET_BIT(pRoom->room_flags, val);	break;
      case '-' :
	REMOVE_BIT(pRoom->room_flags, val);	break;
      case '=' :
	pRoom->room_flags = val;		break;
      default:
	TOGGLE_BIT(pRoom->room_flags, val);
      }
    }
    else
      bug("rattrib: No such room flag.", val);
    return;
  }
  else if (!str_cmp(attrib, "room2")){
    if ( ( val = flag_value( room_flags2, value ) ) != NO_FLAG )
      switch (oper[0]){
      case '+' :
	SET_BIT(pRoom->room_flags2, val);	break;
      case '-' :
	REMOVE_BIT(pRoom->room_flags2, val);	break;
      case '=' :
	pRoom->room_flags2 = val;		break;
      default:
	TOGGLE_BIT(pRoom->room_flags2, val);
    }
    else
      bug("rattrib: No such room flag2.", val);
    return;
  }
  else if (!str_cmp(attrib, "delay")){
    val = atoi( value );
    if (val >= 0){
      switch (oper[0]){
      case '+' :
	pRoom->rprog_delay += val;	break;
      case '-' :
	pRoom->rprog_delay -= val;	break;
      case '=' :
	pRoom->rprog_delay = val;		break;
      case '*' :
	pRoom->rprog_delay *= val;	break;
      case '/' :
	pRoom->rprog_delay /= val;	break;
      default:
	pRoom->rprog_delay = val;		break;
      }
      pRoom->rprog_delay = URANGE(0, pRoom->rprog_delay, 255);
    }
    else
      bug("rattrib: Incorrect delay value.", val);
    return;
  }
  else if (!str_cmp(attrib, "health")){
    val = atoi( value );
    if (val >= 0){
      switch (oper[0]){
      case '+' :
	pRoom->heal_rate += val;	break;
      case '-' :
	pRoom->heal_rate -= val;	break;
      case '=' :
	pRoom->heal_rate = val;		break;
      case '*' :
	pRoom->heal_rate *= val;	break;
      case '/' :
	pRoom->heal_rate /= val;	break;
      default:
	pRoom->heal_rate = val;		break;
      }
      pRoom->heal_rate = URANGE(0, pRoom->heal_rate, 255);
    }
    else
      bug("rattrib: Incorrect health value.", val);
    return;
  }
  else if (!str_cmp(attrib, "mana")){
    val = atoi( value );
    if (val >= 0){
      switch (oper[0]){
      case '+' :
	pRoom->mana_rate += val;	break;
      case '-' :
	pRoom->mana_rate -= val;	break;
      case '=' :
	pRoom->mana_rate = val;		break;
      case '*' :
	pRoom->mana_rate *= val;	break;
      case '/' :
	pRoom->mana_rate /= val;	break;
      default:
	pRoom->mana_rate = val;		break;
      }
      pRoom->mana_rate = URANGE(0, pRoom->mana_rate, 255);
    }
    else
      bug("rattrib: Incorrect mana value.", val);
    return;
  }
  else if (!str_cmp(attrib, "temp")){
    val = atoi( value );
    if (val >= 0){
      switch (oper[0]){
      case '+' :
	pRoom->temp += val;	break;
      case '-' :
	pRoom->temp -= val;	break;
      case '=' :
	pRoom->temp = val;	break;
      case '*' :
	pRoom->temp *= val;	break;
      case '/' :
	pRoom->temp /= val;	break;
      default:
	pRoom->temp = val;	break;
      }
      pRoom->temp = URANGE(0, pRoom->temp, 255);
    }
    else
      bug("rattrib: Incorrect temp (temperature) value.", val);
    return;
  }
  else if (!str_cmp(attrib, "cabal")){
    CABAL_DATA* pc = get_cabal( value );
    if (pc)
      pRoom->pCabal = pc;
    else
      bug("rattrib: Incorrect cabal name.", val);
    return;
  }
  else if (!str_cmp(attrib, "north")){
    change_doorflag(pRoom, value, DIR_NORTH, oper[0]);
    return;
  }
  else if (!str_cmp(attrib, "south")){
    change_doorflag(pRoom, value, DIR_SOUTH, oper[0]);
    return;
  }
  else if (!str_cmp(attrib, "east")){
    change_doorflag(pRoom, value, DIR_EAST, oper[0]);
    return;
  }
  else if (!str_cmp(attrib, "west")){
    change_doorflag(pRoom, value, DIR_WEST, oper[0]);
    return;
  }
  else if (!str_cmp(attrib, "up")){
    change_doorflag(pRoom, value, DIR_UP, oper[0]);
    return;
  }
  else if (!str_cmp(attrib, "down")){
    change_doorflag(pRoom, value, DIR_DOWN, oper[0]);
    return;
  }
  bug("rattrib: invalid attrib", 0);
  return;
}


/*
 * MOBprog section
 */

void do_mob( CHAR_DATA *ch, char *argument )
{
    /*
     * Security check!
     */
    if ( ch->desc != NULL && get_trust(ch) < MAX_LEVEL )
	return;
    mob_interpret( ch, argument );
}
/*
 * Mob command interpreter. Implemented separately for security and speed
 * reasons. A trivial hack of interpret()
 */
void mob_interpret( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH], command[MAX_INPUT_LENGTH];
    int cmd;
    argument = one_argument( argument, command );

    /*
     * Look for command in command table.
     */
    for ( cmd = 0; mob_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == mob_cmd_table[cmd].name[0]
	&&   !str_prefix( command, mob_cmd_table[cmd].name ) )
	{
	  (*mob_cmd_table[cmd].do_fun) ( ch, argument );
	  tail_chain( );
	  return;
	}
    }
    sprintf( buf, "Mob_interpret: invalid cmd from mob %d: '%s'",
	IS_NPC(ch) ? ch->pIndexData->vnum : 0, command );
    bug( buf, 0 );
}

/*
 * Displays MOBprogram triggers of a mobile
 *
 * Syntax: mpstat [name]
 */
void do_mpstat( CHAR_DATA *ch, char *argument )
{
    char        arg[ MAX_STRING_LENGTH  ];
    PROG_LIST  *mprg;
    CHAR_DATA   *victim;
    int i;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mpstat whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "No such creature.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "That is not a mobile.\n\r", ch);
	return;
    }
    /* switch between old /new mobprogs */
    if (victim->pIndexData->progtypes )
     do_old_mpstat(ch, argument);

    sprintf( arg, "Mobile #%-6d [%s]\n\r",
	victim->pIndexData->vnum, victim->short_descr );
    send_to_char( arg, ch );

    sprintf( arg, "Delay   %-6d [%s]\n\r",
	victim->mprog_delay,
	victim->mprog_target == NULL
		? "No target" : victim->mprog_target->name );
    send_to_char( arg, ch );

    if ( !victim->pIndexData->mprog_flags )
    {
	send_to_char( "[No programs set]\n\r", ch);
	return;
    }

    for ( i = 0, mprg = victim->pIndexData->mprogs; mprg != NULL;
	 mprg = mprg->next )

    {
	sprintf( arg, "[%2d] Trigger [%-8s] Program [%4d] Phrase [%s]\n\r",
	      ++i,
	      prog_type_to_name( mprg->trig_type ),
	      mprg->vnum,
	      mprg->trig_phrase );
	send_to_char( arg, ch );
    }

    return;

}

/*
 * Displays the source code of a given MOBprogram
 *
 * Syntax: mpdump [vnum]
 */
void do_mpdump( CHAR_DATA *ch, char *argument )
{
   char buf[ MAX_INPUT_LENGTH ];
   PROG_CODE *mprg;

   one_argument( argument, buf );
   if ( ( mprg = get_prog_index( atoi(buf), PRG_MPROG ) ) == NULL )
   {
	send_to_char( "No such MOBprogram.\n\r", ch );
	return;
   }
   page_to_char( mprg->code, ch );
}

/*
 * Prints the argument to all active players in the game
 *
 * Syntax: mob gecho [string]
 */
void do_mpgecho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "MpGEcho: missing argument from vnum %d",
	    IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Mob echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the argument to all players in the same area as the mob
 *
 * Syntax: mob zecho [string]
 */
void do_mpzecho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "MpZEcho: missing argument from vnum %d",
	    IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ch->in_room == NULL )
	return;

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character->in_room != NULL
	&&   d->character->in_room->area == ch->in_room->area )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Mob echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the argument to all the rooms aroud the mobile
 *
 * Syntax: mob asound [string]
 */
void do_mpasound( CHAR_DATA *ch, char *argument )
{

    ROOM_INDEX_DATA *was_in_room;
    int              door;

    if ( argument[0] == '\0' )
	return;

    was_in_room = ch->in_room;
    for ( door = 0; door < 6; door++ )
    {
    	EXIT_DATA       *pexit;

      	if ( ( pexit = was_in_room->exit[door] ) != NULL
	  &&   pexit->to_room != NULL
	  &&   pexit->to_room != was_in_room )
      	{
	    ch->in_room = pexit->to_room;
	    MOBtrigger  = FALSE;
	    act( argument, ch, NULL, NULL, TO_ROOM );
	    MOBtrigger  = TRUE;
	}
    }
    ch->in_room = was_in_room;
    return;

}

/*
 * Lets the mobile kill any player or mobile without murder
 *
 * Syntax: mob kill [victim]
 */
void do_mpkill( CHAR_DATA *ch, char *argument )
{
    char      arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return;

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	return;

    if ( victim == ch || IS_NPC(victim) || ch->position == POS_FIGHTING )
	return;

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
	bug( "MpKill - Charmed mob attacking master from vnum %d.",
	    IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}

/*
 * Lets the mobile assist another mob or player
 *
 * Syntax: mob assist [character]
 */
void do_mpassist( CHAR_DATA *ch, char *argument )
{
    char      arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return;

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	return;

    if ( victim == ch || ch->fighting != NULL || victim->fighting == NULL )
	return;

    multi_hit( ch, victim->fighting, TYPE_UNDEFINED );
    return;
}

/*
 * Lets the mobile destroy an object in its inventory
 * it can also destroy a worn object and it can destroy
 * items using all.xxxxx or just plain all of them
 *
 * Syntax: mob junk [item]
 * Syntax: mob junk all
 */

void do_mpjunk( CHAR_DATA *ch, char *argument )
{
    char      arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    one_argument( argument, arg );

    if ( arg[0] == '\0')
	return;

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
    	if ( ( obj = get_obj_wear( ch, arg, ch) ) != NULL )
      	{
      	    unequip_char( ch, obj );
	    extract_obj( obj );
    	    return;
     	}
      	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	    return;
	extract_obj( obj );
    }
    else
      	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
      	{
            obj_next = obj->next_content;
	    if ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
            {
          	if ( obj->wear_loc != WEAR_NONE)
	    	unequip_char( ch, obj );
          	extract_obj( obj );
            }
      	}

    return;

}

/*
 * Prints the message to everyone in the room other than the mob and victim
 *
 * Syntax: mob echoaround [victim] [string]
 */

void do_mpechoaround( CHAR_DATA *ch, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0'  || ch == NULL)
	return;

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
      return;

    act( argument, ch, NULL, victim, TO_NOTVICT );
}

/*
 * Prints the message to only the victim
 *
 * Syntax: mob echoat [victim] [string]
 */
void do_mpechoat( CHAR_DATA *ch, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
	return;

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	return;

    act_new(argument, ch, NULL, victim, TO_VICT, POS_DEAD );
}

/*
 * Prints the message to the room at large
 *
 * Syntax: mpecho [string]
 */
void do_mpecho( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
	return;
    act( argument, ch, NULL, NULL, TO_ROOM );
}

/*
 * Lets the mobile load another mobile.
 *
 * Syntax: mob mload [vnum]
 */
void do_mpmload( CHAR_DATA *ch, char *argument )
{
    char            arg[ MAX_INPUT_LENGTH ];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA      *victim;
    int vnum;

    one_argument( argument, arg );

    if ( ch->in_room == NULL || arg[0] == '\0' || !is_number(arg) )
	return;

    vnum = atoi(arg);
    if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
    {
	sprintf( arg, "Mpmload: bad mob index (%d) from mob %d",
	    vnum, IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	bug( arg, 0 );
	return;
    }
    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    return;
}

/*
 * Lets the mobile load an object
 *
 * Syntax: mob oload [vnum] [level] {R}
 */
void do_mpoload( CHAR_DATA *ch, char *argument )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA       *obj;
    int             level;
    bool            fToroom = FALSE, fWear = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        bug( "Mpoload - Bad syntax from vnum %d.",
	    IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
        return;
    }

    if ( arg2[0] == '\0' )
    {
      level = get_trust( ch );
    }
    else
    {
	/*
	 * New feature from Alander.
	 */
        if ( !is_number( arg2 ) )
        {
	    bug( "Mpoload - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	    return;
        }
	level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
	{
	    bug( "Mpoload - Bad level from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	    return;
	}
    }

    /*
     * Added 3rd argument
     * omitted - load to mobile's inventory
     * 'R'     - load to room
     * 'W'     - load to mobile and force wear
     */
    if ( arg3[0] == 'R' || arg3[0] == 'r' )
	fToroom = TRUE;
    else if ( arg3[0] == 'W' || arg3[0] == 'w' )
	fWear = TRUE;

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	bug( "Mpoload - Bad vnum arg from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    obj = create_object( pObjIndex, level );
    if ( (fWear || !fToroom) && CAN_WEAR(obj, ITEM_TAKE) )
    {
	obj_to_char( obj, ch );
	if ( fWear )
	    wear_obj( ch, obj, TRUE, FALSE );
    }
    else
    {
	obj_to_room( obj, ch->in_room );
    }

    return;
}

/*
 * Lets the mobile purge all objects and other npcs in the room,
 * or purge a specified object or mob in the room. The mobile cannot
 * purge itself for safety reasons.
 *
 * syntax mob purge {target}
 */
void do_mppurge( CHAR_DATA *ch, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        /* 'purge' */
        CHAR_DATA *vnext;
        OBJ_DATA  *obj_next;

	for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC( victim ) && victim != ch
	    &&   !IS_SET(victim->act, ACT_NOPURGE) )
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( !IS_SET(obj->extra_flags, ITEM_NOPURGE) )
		extract_obj( obj );
	}

	return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	if ( ( obj = get_obj_here( ch, NULL, arg ) ) )
	{
	    extract_obj( obj );
	}
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	bug( "Mppurge - Purging a PC from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    extract_char( victim, TRUE );
    return;
}


/*
 * Lets the mobile goto any location it wishes that is not private.
 *
 * Syntax: mob goto [location]
 */
void do_mpgoto( CHAR_DATA *ch, char *argument )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "Mpgoto - No argument from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	bug( "Mpgoto - No such location from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ch->fighting != NULL )
	stop_fighting( ch, TRUE );

    char_from_room( ch );
    char_to_room( ch, location );

    return;
}

/*
 * Lets the mobile do a command at another location.
 *
 * Syntax: mob at [location] [commands]
 */
void do_mpat( CHAR_DATA *ch, char *argument )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    CHAR_DATA       *wch;
    OBJ_DATA 	    *on;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "Mpat - Bad argument from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	bug( "Mpat - No such location from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    original = ch->in_room;
    on = ch->on;
    char_from_room( ch );
    char_to_room( ch, location );

    if (IS_NPC(ch))
      ch->trust = 6969;
    interpret( ch, argument );
    if (IS_NPC(ch))
      ch->trust = ch->level;


    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    ch->on = on;
	    break;
	}
    }

    return;
}

/*
 * Lets the mobile transfer people.  The 'all' argument transfers
 *  everyone in the current room to the specified location
 *
 * Syntax: mob transfer [target|'all'] [location]
 */
void do_mptransfer( CHAR_DATA *ch, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    CHAR_DATA       *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Mptransfer - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	CHAR_DATA *victim_next;

	for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
	{
	    victim_next = victim->next_in_room;
	    if (IS_NPC(victim) && victim->master == NULL)
	      continue;

	    sprintf( buf, "'%s' %s", victim->name, arg2 );
	    do_mptransfer( ch, buf );
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    bug( "Mptransfer - No such location from vnum %d.",
	        IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	    return;
	}

	if ( room_is_private( location ) )
	    return;
    }

    if ( ( victim = get_char_world( NULL, arg1 ) ) == NULL )
	return;

    if ( victim->in_room == NULL )
	return;
    if (location == NULL)
      return;
    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    char_from_room( victim );
    char_to_room( victim, location );
    do_look( victim, "auto" );

    return;
}

/*
 * Lets the mobile transfer all chars in same group as the victim.
 * Syntax: mob gtransfer [victim] [location] <room>
 */
void do_mpgtransfer( CHAR_DATA *ch, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    CHAR_DATA       *who, *victim, *victim_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Mpgtransfer - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( (who = get_char_room( ch, NULL, arg1 )) == NULL )
	return;

    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
    	victim_next = victim->next_in_room;
    	if( is_same_group( who,victim ) )
    	{
	    sprintf( buf, "%s %s", victim->name, arg2 );
	    do_mptransfer( ch, buf );
    	}
    }
    return;
}

/*
 * Lets the mobile transfer all chars in victim's room to given room
 * Usefull for scripted transfers in which you do not want char's left
 * behind.
 * unles victim is "all" victim is not transfered.
  * Syntax: mob rtransfer [victim] [room]
 * Syntax: mob rtransfer "all" [room]
 */
void do_mprtransfer( CHAR_DATA *ch, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    CHAR_DATA       *who = NULL, *victim, *victim_next;
    bool fAll = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Mprtransfer - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    if (!str_cmp(arg1, "all"))
      fAll = TRUE;
    else if ( (who = get_char_room( ch, NULL, arg1 )) == NULL )
      return;

    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
    	victim_next = victim->next_in_room;
    	if(fAll
	   || who == NULL
	   || who != victim){
	  sprintf( buf, "%s %s", victim->name, arg2 );
	  do_mptransfer( ch, buf );
	}
    }
    return;
}

/*
 * Lets the mobile force someone to do something. Must be mortal level
 * and the all argument only affects those in the room with the mobile.
 *
 * Syntax: mob force [victim] [commands]
 */
void do_mpforce( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "Mpforce - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( vch->in_room == ch->in_room
		&& get_trust( vch ) < get_trust( ch )
		&& can_see( ch, vch ) )
	    {
		interpret( vch, argument );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	    return;

	if ( victim == ch )
	    return;

	interpret( victim, argument );
    }

    return;
}

/*
 * Lets the mobile force a group something. Must be mortal level.
 *
 * Syntax: mob gforce [victim] [commands]
 */
void do_mpgforce( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim, *vch, *vch_next;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "MpGforce - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	return;

    if ( victim == ch )
	return;

    for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( is_same_group(victim,vch) )
        {
	    interpret( vch, argument );
	}
    }
    return;
}

/*
 * Forces all mobiles of certain vnum to do something (except ch)
 *
 * Syntax: mob vforce [vnum] [commands]
 */
void do_mpvforce( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim, *victim_next;
    char arg[ MAX_INPUT_LENGTH ];
    int vnum;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "MpVforce - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( !is_number( arg ) )
    {
	bug( "MpVforce - Non-number argument vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    vnum = atoi( arg );

    for ( victim = char_list; victim; victim = victim_next )
    {
	victim_next = victim->next;
	if ( IS_NPC(victim) && victim->pIndexData->vnum == vnum
	&&   ch != victim && victim->fighting == NULL )
	    interpret( victim, argument );
    }
    return;
}


/*
 * Lets the mobile cast spells --
 * Beware: this does only crude checking on the target validity
 * and does not account for mana etc., so you should do all the
 * necessary checking in your mob program before issuing this cmd!
 *
 * Syntax: mob cast [spell] {target}
 */

void do_mpcast( CHAR_DATA *ch, char *argument )
{
  ROOM_INDEX_DATA* room;
    CHAR_DATA *vch;
    OBJ_DATA *obj;
    void *victim = NULL;
    char spell[ MAX_INPUT_LENGTH ],
	 target[ MAX_INPUT_LENGTH ];
    int sn;

    argument = one_argument( argument, spell );
    one_argument( argument, target );
    target_name = argument;
    if ( spell[0] == '\0' )
    {
	bug( "MpCast - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
    {
	bug( "MpCast - No such spell from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    room  = ch->in_room;
    vch = get_char_room( NULL, room, target );
    obj = get_obj_here( NULL, room, target );
    switch ( skill_table[sn].target )
    {
	default: return;
	case TAR_IGNORE:
	    break;
	case TAR_CHAR_OFFENSIVE:
	    if ( vch == NULL || vch == ch )
		return;
	    victim = ( void * ) vch;
	    break;
	case TAR_CHAR_DEFENSIVE:
	    victim = vch == NULL ? ( void *) ch : (void *) vch; break;
	case TAR_CHAR_SELF:
	    victim = ( void *) ch; break;
	case TAR_OBJ_CHAR_DEF:
	case TAR_OBJ_CHAR_OFF:
	case TAR_OBJ_INV:
	    if ( obj == NULL )
		return;
	    victim = ( void * ) obj;
    }
    (*skill_table[sn].spell_fun)( sn, ch->level, ch, victim,
	skill_table[sn].target );
    return;
}

/*
 * Lets mob cause unconditional damage to someone. Nasty, use with caution.
 * Also, this is silent, you must show your own damage message...
 *
 * Syntax: mob damage [victim] [min] [max] {kill}
 */
void do_mpdamage( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim = NULL, *victim_next;
    char target[ MAX_INPUT_LENGTH ],
	 min[ MAX_INPUT_LENGTH ],
	 max[ MAX_INPUT_LENGTH ];
    int low, high;
    bool fAll = FALSE, fKill = FALSE;

    argument = one_argument( argument, target );
    argument = one_argument( argument, min );
    argument = one_argument( argument, max );

    if ( target[0] == '\0' )
    {
	bug( "MpDamage - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    if( !str_cmp( target, "all" ) )
	fAll = TRUE;
    else if( ( victim = get_char_room( NULL, ch->in_room, target ) ) == NULL )
	return;

    if ( is_number( min ) )
	low = atoi( min );
    else
    {
	bug( "MpDamage - Bad damage min vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    if ( is_number( max ) )
	high = atoi( max );
    else
    {
	bug( "MpDamage - Bad damage max vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    one_argument( argument, target );

    /*
     * If kill parameter is omitted, this command is "safe" and will not
     * kill the victim.
     */

    if ( target[0] != '\0' )
	fKill = TRUE;
    if ( fAll )
    {
	for( victim = ch->in_room->people; victim; victim = victim_next )
	{
	    victim_next = victim->next_in_room;
	    if ( victim != ch )
    		damage( victim, NULL,
		    fKill ?
		    number_range(low,high) : UMIN(victim->hit - 1, number_range(low,high)),
	        TYPE_UNDEFINED, DAM_NONE, FALSE );
	}
    }
    else
    	damage( victim, NULL,
	    fKill ?
	    number_range(low,high) : UMIN(victim->hit - 1, number_range(low,high)),
        TYPE_UNDEFINED, DAM_NONE, FALSE );
    return;
}

/*
 * Lets the mobile to remember a target. The target can be referred to
 * with $q and $Q codes in MOBprograms. See also "mob forget".
 *
 * Syntax: mob remember [victim]
 */
void do_mpremember( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    one_argument( argument, arg );
    if ( arg[0] != '\0' )
	ch->mprog_target = get_char( arg );
    else
	bug( "MpRemember: missing argument from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
}

/*
 * Reverse of "mob remember".
 *
 * Syntax: mob forget
 */
void do_mpforget( CHAR_DATA *ch, char *argument )
{
    ch->mprog_target = NULL;
}

/*
 * Sets a delay for MOBprogram execution. When the delay time expires,
 * the mobile is checked for a MObprogram with DELAY trigger, and if
 * one is found, it is executed. Delay is counted in PULSE_MOBILE
 *
 * Syntax: mob delay [pulses]
 */
void do_mpdelay( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );
    if ( !is_number( arg ) )
    {
	bug( "MpDelay: invalid arg from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    ch->mprog_delay = atoi( arg );
}

/*
 * Reverse of "mob delay", deactivates the timer.
 *
 * Syntax: mob cancel
 */
void do_mpcancel( CHAR_DATA *ch, char *argument )
{
   ch->mprog_delay = 0;
}
/*
 * Lets the mobile to call another MOBprogram withing a MOBprogram.
 * This is a crude way to implement subroutines/functions. Beware of
 * nested loops and unwanted triggerings... Stack usage might be a problem.
 * Characters and objects referred to must be in the same room with the
 * mobile.
 *
 * Syntax: mob call [vnum] [victim|'null'] [object1|'null'] [object2|'null']
 *
 */
void do_mpcall( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *vch;
    OBJ_DATA *obj1, *obj2;
    PROG_CODE *prg;
    extern void program_flow( sh_int, char *, CHAR_DATA *, OBJ_DATA *, ROOM_INDEX_DATA *, CHAR_DATA *, const void *, const void * );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "MpCall: missing arguments from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    if ( ( prg = get_prog_index( atoi(arg), PRG_MPROG ) ) == NULL )
    {
	bug( "MpCall: invalid prog from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    if (ch->in_room == NULL)
      return;
    vch = NULL;
    obj1 = obj2 = NULL;
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
        vch = get_char_room( NULL, ch->in_room, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj1 = get_obj_here( NULL, ch->in_room, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj2 = get_obj_here( NULL, ch->in_room, arg );
    program_flow( prg->vnum, prg->code, ch, NULL, NULL, vch, (void *)obj1, (void *)obj2 );
}

/*
 * Forces the mobile to flee.
 *
 * Syntax: mob flee
 *
 */
void do_mpflee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    EXIT_DATA *pexit;
    int door, attempt;

    if ( ch->fighting != NULL )
	return;

    if ( (was_in = ch->in_room) == NULL )
	return;

    for ( attempt = 0; attempt < 6; attempt++ )
    {
        door = number_door( );
        if ( ( pexit = was_in->exit[door] ) == 0
        ||   pexit->to_room == NULL
        ||   IS_SET(pexit->exit_info, EX_CLOSED)
        || ( IS_NPC(ch)
        &&   IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB) ) )
            continue;

        move_char( ch, door, FALSE );
        if ( ch->in_room != was_in )
	    return;
    }
}

/*
 * Lets the mobile to transfer an object. The object must be in the same
 * room with the mobile.
 *
 * Syntax: mob otransfer [item name] [location/person]
 */
void do_mpotransfer( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    CHAR_DATA* victim = NULL;
    ROOM_INDEX_DATA *location = NULL;
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "MpOTransfer - Missing argument from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    one_argument( argument, buf );
    if ( (victim = get_char_world(ch, buf)) == NULL
	 && ( location = find_location( ch, buf ) ) == NULL )
    {
	bug( "MpOTransfer - No such location from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    if ( (obj = get_obj_carry( ch, arg, NULL )) == NULL
	 && (obj = get_obj_here( ch, NULL, arg )) == NULL )
      return;
    if ( obj->carried_by == NULL )
	obj_from_room( obj );
    else
    {
	if ( obj->wear_loc != WEAR_NONE )
	    unequip_char( ch, obj );
	obj_from_char( obj );
    }
    if ( victim )
      obj_to_char( obj, victim );
    else if (location)
      obj_to_room( obj, location );
    else
      extract_obj ( obj );
}

/*
 * Lets the mobile to strip an object or all objects from the victim.
 * Useful for removing e.g. quest objects from a character.
 *
 * Syntax: mob remove [victim] [object vnum|'all']
 */
void do_mpremove( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj, *obj_next;
    sh_int vnum = 0;
    bool fAll = FALSE;
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	return;

    one_argument( argument, arg );
    if ( !str_cmp( arg, "all" ) )
	fAll = TRUE;
    else if ( !is_number( arg ) )
    {
	bug ( "MpRemove: Invalid object from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    else
	vnum = atoi( arg );

    for ( obj = victim->carrying; obj; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( fAll || obj->pIndexData->vnum == vnum )
	{
	     unequip_char( victim, obj );
	     obj_from_char( obj );
	     extract_obj( obj );
	}
    }
}

/*
 * Lets the mobile stop char who triggered the prog from
 * exiting the room. (TRIG_EXIT/TRIG_EXALL only)
 * Aiming at char who did not trigger might have unpredictable
 * results.
 * Syntax: mob stop <char>
 */
void do_mpstop( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    if ( ( victim = get_char_room( NULL, ch->in_room, argument ) ) == NULL )
      return;
    /* flag is cleared after TRIG_EX** ends */
    SET_BIT(victim->affected_by, AFF_FLAG);
}

/*
 * Lets the mobile become a charmed follower of target
 * if timer specifedthe mob will expire after that many ticks
 * Syntax: mob obey <mob> <owner> <timer>
 * Syntax: mob obey <mob> <owner> pet
 */
void do_mpobey( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch, *victim;
    char arg[MIL];
    if (ch->in_room == NULL)
      return;
    argument = one_argument( argument, arg );
    if ( ( vch = get_char_room( ch, NULL, arg ) ) == NULL
	 || !IS_NPC(vch))
      return;
    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
      return;

    /*set charmie */
    if ( is_number(argument)){
      AFFECT_DATA af;

      af.type = gsn_timer;
      af.level= vch->level;
      af.duration = UMAX(0, atoi(argument));
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier =  0;
      affect_to_char(vch, &af);
    }
    if (!str_cmp("pet", argument)){
      vch->master = victim;
      vch->leader = victim;
      vch->summoner = victim;

      if (victim->pet)
	stop_follower( victim->pet );
      victim->pet = vch;
      SET_BIT(vch->affected_by, AFF_CHARM);
      SET_BIT(vch->act, ACT_PET);
    }
    else{
      vch->master = victim;
      vch->leader = victim;
      vch->summoner = victim;
      SET_BIT(vch->affected_by, AFF_CHARM);
    }
}


/*
 * Lets the mob change its stats, can be related to the
 * target's level by using "level"
 *
 * Syntax: mob attrib [target] [field] [+-* / =] [value/"level"]
 */
void do_mpattrib(CHAR_DATA* ch, char *argument ){
  CHAR_DATA *victim;
  char target[MIL],
    arg1[MIL],
    arg2[MIL],
    arg3[MIL];
  char mod;
  int value;

  argument = one_argument( argument, target );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  /* grab the character target */
  if (!str_cmp(target, "self"))
    victim = ch;
  else if ( ( victim = get_char_room( NULL, ch->in_room, target ) ) == NULL )
    victim = ch;

  /*
     get the integer value for field
     check if operator was skipped
  */
  if (IS_NULLSTR(arg3)){
    if (!str_cmp("level", arg2))
      value = ch->level;
    else if (!is_number(arg2))
      return;
    else
      value = atoi(arg2);
  }
  else{
    if (!str_cmp("level", arg3))
      value = victim->level;
    else if (!is_number(arg3))
      return;
    else
      value = atoi(arg3);
  }

  /*
    we have the value, time to get
    the operator
  */
  if(IS_NULLSTR(arg3))
    mod = '=';
  else
    mod = arg2[0];

  /* start operating */
  if (!str_cmp("level", arg1)){
    switch (mod){
    case '=': victim->level = value;	break;
    case '+': victim->level += value;	break;
    case '-': victim->level -= value;	break;
    case '/': victim->level /= value;	break;
    case '*': victim->level *= value;	break;
    }
  }
  if (!str_cmp("delay", arg1)){
    switch (mod){
    case '=': victim->mprog_delay = value;	break;
    case '+': victim->mprog_delay += value;	break;
    case '-': victim->mprog_delay -= value;	break;
    case '/': victim->mprog_delay /= value;	break;
    case '*': victim->mprog_delay *= value;	break;
    }
  }
  else if (!str_cmp("gold", arg1)){
    switch (mod){
    case '=': victim->gold	= value;	break;
    case '+': victim->gold	 += value;	break;
    case '-': victim->gold	 -= value;	break;
    case '/': victim->gold	 /= value;	break;
    case '*': victim->gold	 *= value;	break;
    }
  }
  else if (!str_cmp("timer", arg1)){
    AFFECT_DATA af;
    switch (mod){
    case '=': af.duration = value;	break;
    case '+': af.duration += value;	break;
    case '-': af.duration -= value;	break;
    case '/': af.duration /= value;	break;
    case '*': af.duration *= value;	break;
    }
    af.type = gsn_timer;
    af.level = ch->level;
    af.where = TO_NONE;
    af.bitvector = 0;
    af.modifier = 0;
    af.location= APPLY_NONE;
    affect_join(victim, &af);
  }


}

/* Allows prog to set the "mounted" affect on a victim giving him ability to mount
 * Syntax: mount <vnum> <dur> <victim>
 * duration of < 0 means infinite
 */

void prog_mount( ROOM_INDEX_DATA* room, char* argument ){
  MOB_INDEX_DATA* mount;
  CHAR_DATA* victim;
  char arg1[MIL];
  char arg2[MIL];
  int dur = 0;

  argument = one_argument( argument, arg1);
  argument = one_argument( argument, arg2);

  dur = atoi( arg2 );

  if ( (mount = get_mob_index( atoi( arg1 ))) == NULL){
    bug("prog_mount: could not get index for vnum %d", atoi( arg1));
    return;
  }
  else if ( (victim = get_char_room( NULL, room, argument)) == NULL){
    bug("prog_mount: could not find victim.", 0);
    return;
  }
  else{
    AFFECT_DATA af;
    affect_strip(victim, gsn_mounted);

    af.type      = gsn_mounted;
    af.level	 = victim->level;
    af.duration  = dur;
    af.where	 = TO_AFFECTS;
    af.bitvector = 0;
    af.location  = APPLY_NONE;
    af.modifier  = mount->vnum;
    affect_to_char( victim, &af );
  }
}




/*
 * Adjusts given statistic on the given mob/pc
 *
 * Syntax: mob gain [stat] [all/exc/group/pc/mob] [victim] [min] [max]
 * Stats supported: hp, php, mana, pmana, move, pmove, exp, gold, cp, dfavor, train, prac
 */
void prog_gain(ROOM_INDEX_DATA* in_room, char *argument ){
  ROOM_INDEX_DATA* room = in_room;
  CHAR_DATA* victim, *vch, *vch_next;

  char stat[MIL];
  char target[MIL];
  char arg[MIL];

  int min = 0;
  int max = 0;

/* stats to change */
  bool fhp = FALSE;
  bool fphp = FALSE;  //permament hp
  bool fmana = FALSE;
  bool fpmana = FALSE; //perm mana
  bool fmove = FALSE;
  bool fpmove = FALSE; //perm move
  bool fexp = FALSE;
  bool fgold = FALSE;
  bool fcp = FALSE;
  bool fdfav = FALSE;
  bool ftra = FALSE;
  bool fpra = FALSE;

/* target types */
  bool fAll = FALSE;   //all in room with victim
  bool fExc = FALSE;   //all in room with victim but NOT victim
  bool fGroup = FALSE; //all in group of victim
  bool fPc = FALSE;	//pc victim
  bool fMob = FALSE;	//npc victim

  if (room == NULL)
    return;

  /* Useless conditional */
  if( fpmove != FALSE )
    fpmove = FALSE;
  /* Useless conditional */
  if( fAll != FALSE )
    fAll = FALSE;

/* start parsing arguments */
  argument = one_argument(argument, stat);
  if (IS_NULLSTR(stat)){
    bug ( "prog_gain: Invalid argument: stat", 0);
    return;
  }
  if (!str_cmp(stat, "hp"))
    fhp = TRUE;
  else if (!str_cmp(stat, "php"))
    fphp = TRUE;
  else if (!str_cmp(stat, "mana"))
    fmana = TRUE;
  else if (!str_cmp(stat, "pmana"))
    fpmana = TRUE;
  else if (!str_cmp(stat, "move"))
    fmove = TRUE;
  else if (!str_cmp(stat, "pmove"))
    fpmove = TRUE;
  else if (!str_cmp(stat, "exp"))
    fexp = TRUE;
  else if (!str_cmp(stat, "gold"))
    fgold = TRUE;
  else if (!str_cmp(stat, "cp"))
    fcp = TRUE;
  else if (!str_cmp(stat, "dfavor"))
    fdfav = TRUE;
  else if (!str_cmp(stat, "train"))
    ftra = TRUE;
  else if (!str_cmp(stat, "prac"))
    fpra = TRUE;
  else{
    bug ( "prog_gain: Invalid stat.", 0);
    return;
  }

/* parse target types */
  argument = one_argument(argument, target);
  if (IS_NULLSTR(target)){
    bug ( "prog_gain: Invalid argument: target type.", 0);
    return;
  }
  if (!str_cmp(target, "all"))
    fAll = TRUE;
  else if (!str_cmp(target, "exc"))
    fExc = TRUE;
  else if (!str_cmp(target, "group"))
    fGroup = TRUE;
  else if (!str_cmp(target, "pc"))
    fPc = TRUE;
  else if (!str_cmp(target, "mob"))
    fMob = TRUE;
  else{
    bug ( "prog_gain: Invalid target type", 0);
    return;
  }

/* now we grab the victim */
  argument = one_argument(argument, target);
  if (IS_NULLSTR(target)){
    bug ( "prog_gain: Invalid argument: target", 0);
    return;
  }
  if ( (victim = get_char_room(NULL, room, target)) == NULL)
    return;

/* get the amounts */
  argument = one_argument(argument, arg);
  min = atoi(arg);
  argument = one_argument(argument, arg);
  max = atoi(arg);

/* start doing things */
  for (vch = victim->in_room->people; vch; vch = vch_next){
    int n = number_range(min, max);
    vch_next = vch->next_in_room;

/* start rejection process */
    if ( (fExc && victim == vch)
	 || (fGroup && !is_same_group(vch, victim))
	 || (fPc && (IS_NPC(vch) || victim != vch))
	 || (fMob && (!IS_NPC(vch) || victim != vch))
	 )
      continue;

/* we now have right switchion, start adjusting stats */
    if (fhp){
      vch->hit = URANGE(0, vch->hit + n, vch->max_hit);
      update_pos( victim );
    }
    else if (fphp){
      vch->pcdata->perm_hit = vch->pcdata->perm_hit + n;
      vch->max_hit += n;
      vch->hit = URANGE(0, vch->hit + n, vch->max_hit);
      update_pos( victim );
    }
    else if (fmana)
      vch->mana = URANGE(0, vch->mana + n, vch->max_mana);
    else if (fpmana){
      vch->pcdata->perm_mana += n;
      vch->max_mana += n;
      vch->mana = URANGE(0, vch->mana + n, vch->max_mana);
    }
    else if (fmove)
      vch->move = URANGE(0, vch->move + n, vch->max_move);
    else if (fmove){
      vch->max_move += n;
      vch->pcdata->perm_move += n;
      vch->move = URANGE(0, vch->move + n, vch->max_move);
    }
    else if (fexp && !IS_NPC(vch))
      gain_exp(vch, n);
    else if (fgold)
      vch->gold += n;
    else if (fcp && !IS_NPC(vch) && vch->pCabal != NULL)
      CP_GAIN(vch, n, TRUE);
    else if (fdfav && !IS_NPC(vch) && IS_AVATAR(vch))
        divfavor_gain(vch,  n);
    else if (ftra && !IS_NPC(vch))
      vch->train += n;
    else if (fpra && !IS_NPC(vch))
      vch->practice += n;
  }
/* end looping */
}

/* uses above */
void do_mpgain(CHAR_DATA* ch, char *argument ){
  prog_gain(ch->in_room, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_mppermquest(CHAR_DATA* ch, char* argument){
  do_permquest(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_mpinviquest(CHAR_DATA* ch, char* argument){
  do_inviquest(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_mptempquest(CHAR_DATA* ch, char* argument){
  do_tempquest(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_mpremquest(CHAR_DATA* ch, char* argument){
  do_remquest(ch, argument);
}

void do_mprattrib(CHAR_DATA* ch, char* argument){
  rattrib(ch->in_room, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_mpaddpsalm(CHAR_DATA* ch, char* argument){
  do_addpsalm(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_mprempsalm(CHAR_DATA* ch, char* argument){
  do_rempsalm(ch, argument);
}

/* allows mob to give character a mount */
void do_mpmount( CHAR_DATA* ch, char* argument ){
  if (ch->in_room == NULL)
    return;
  else
    prog_mount( ch->in_room, argument );
}

/* Allows a mob to take an item from a mob/pc
 * mob chown <object> <victim>
 * mob chown all <victim>
 */
void do_mpchown(CHAR_DATA* ch, char* argument){
  char arg1 [MIL];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  argument = one_argument( argument, arg1 );

  if ( arg1[0] == '\0' || argument[0] == '\0' )
    {
      bug("mpchown: null arguments for mob %d", ch->pIndexData->vnum);
      return;
    }
  if ( ( victim = get_char_room( ch, NULL, argument ) ) == NULL )
    {
      bug("mpchown: victim not found for mob %d", ch->pIndexData->vnum);
      return;
    }
  if (!str_cmp(arg1, "all")){
    OBJ_DATA* obj_next;

    for (obj = victim->carrying; obj; obj = obj_next){
      obj_next = obj->next_content;

        obj_from_char( obj );
	obj_to_char( obj, ch );
    }
    return;
  }
  if (((obj = get_obj_wear(victim,arg1,ch)) == NULL) && ((obj = get_obj_carry(victim,arg1,ch)) == NULL))
    {
      return;
    }
  obj_from_char( obj );
  obj_to_char( obj, ch );
}

/* used in prog_Destroy, recursivly destroys objects with given name/vnum on list
 */
int _destroy_items( OBJ_DATA* list, char* name, int vnum, int number, int count ){
  OBJ_DATA* obj, *obj_next;

  if (list == NULL || count >= number)
    return 0;
  for (obj = list; obj; obj = obj_next){
    obj_next = obj->next_content;

    /* check contents first, that way we can destroy an object that contains
       objects to destroy */
    if (obj->contains){
      count = _destroy_items( obj->contains, name, vnum, number, count );
      if (count >= number)
	return count;
    }

    if ( (vnum && obj->pIndexData->vnum == vnum) || is_name(name, obj->name) || !str_cmp(name, "*") || !str_cmp(name, "all")){
      extract_obj( obj );
      if (++count >= number)
	return count;
    }
  }
  return count;
}

/* allows a prog command to destroy number of items on target victim *
 * Syntax: destroy [obj/ch] <target> <item> <number>
 * item can be name or vnum
 * ex: mob destroy obj chest vial 2
 */
void prog_destroy( ROOM_INDEX_DATA* room, char* argument ){
  OBJ_DATA* list = NULL;
  char type[MIL];
  char target[MIL];
  char item[MIL];

  void* tar = NULL;
  CHAR_DATA* ch_tar = NULL;

  if (IS_NULLSTR(argument) || room == NULL){
    return;
  }
  argument = one_argument( argument, type );
  argument = one_argument( argument, target );
  argument = one_argument( argument, item );

  if (IS_NULLSTR(type)){
    bug("prog_destroy: null type", 0);
    return;
  }
  else if (IS_NULLSTR(target)){
    bug("prog_destroy: null target", 0);
    return;
  }
  else if (IS_NULLSTR(item)){
    bug("prog_destroy: null item", 0);
    return;
  }
  else if (room == NULL){
    bug("prog_destroy: null room", 0);
    return;
  }

/* get our target */
  if (!str_cmp( type, "obj")){
    if ( (tar = (OBJ_DATA*) get_obj_here( NULL, room, target)) == NULL){
      bug("prog_destroy: obj target not found.", 0);
      return;
    }
    else{
      list = (OBJ_DATA*) tar;
      list = list->contains;
    }
  }
  else if (!str_cmp( type, "ch")){
    if ( (tar = (CHAR_DATA*) get_char_room( NULL, room, target)) == NULL){
      bug("prog_destroy: ch target not found.", 0);
      return;
    }
    else{
      ch_tar = (CHAR_DATA*) tar;
      list = ch_tar->carrying;
    }
  }
  else{
    bug("prog_destroy: invalid target, must be obj or ch", 0);
    return;
  }
/* get our items to destroy */
  if (list == NULL){
    bug("prog_destroy: List was NULL!", 0);
    return;
  }
  _destroy_items( list, item, atoi(item), atoi(argument), 0 );
}

/* allows a prog to set a path to target room on the mobile.
   speed of travel is decided by the mobs DICE_NUMBER
*/
/* prog_path <room vnum> <target mob> */
/* prog_path random <taret mob> */
void prog_path( CHAR_DATA* ch,  char* argument ){
  CHAR_DATA* mob;
  ROOM_INDEX_DATA* to;
  PATH_QUEUE* path;

  char vnum[MIL];
  int dist, room_vnum;

  argument = one_argument( argument, vnum );

  if (!str_cmp( argument, "self"))
    mob = ch;
  else
    mob = get_char_world( NULL, argument );


  if (mob == NULL || mob->in_room == NULL){
    bug("prog_path: null mob passed.", 0);
    return;
  }

  if (str_cmp( vnum, "random"))
    room_vnum = atoi( vnum );
  else{
    ROOM_INDEX_DATA* pTo, *pFrom = mob->in_room;
    int area_pool = 10;

    pTo =  get_rand_room(pFrom->area->vnum,pFrom->area->vnum,//area range (0 to ignore)
			 0,0,			//room ramge (0 to ignore)
			 0,0,			//areas to choose from
			 NULL,0,		//areas to exclude
			 NULL,0,		//sectors required
			 NULL,0,		//sectors to exlude
			 NULL,0,		//room1 flags required
			 0,0,			//room1 flags to exclude
			 NULL,0,		//room2 flags required
			 NULL,0,		//room2 flags to exclude
			 area_pool,		//number of seed areas
			 TRUE,			//exit required?
			 FALSE,			//Safe?
			 NULL);			//Character for room checks
    if (pTo == NULL)
      return;
    else
      room_vnum = pTo->vnum;
  }

  if ( (to = get_room_index( room_vnum)) == NULL){
    bug("prog_path: cannot find room: %d.", room_vnum);
    return;
  }
  else if (IS_VIRROOM(to)){
    bug("prog_path: path to virtual room: %d.", room_vnum);
    return;
  }

  path = generate_path( mob->in_room, to, 128, TRUE, &dist, NULL);
  clean_path();

  if (path == NULL)
    return;
  else{
    if (mob->spec_path)
      clean_path_queue( mob->spec_path );
    mob->spec_path = path;
  }
}

/* allows to repeat a prog command across people in the room
 * '#' gets substituted for current char name
 * pfor <room/exc/group/mob/pc/fight> <target> <command>
 * ex: mob pfor room $n echoat # This is just like mob echo
 */
/* this version takes a room/obj/mob and arguments and is reused */
void prog_for(CHAR_DATA* ch,
	    OBJ_DATA* obj,
	    ROOM_INDEX_DATA* room,
	    char* argument){

  char range[MIL], target[MIL];
  CHAR_DATA* victim;
  CHAR_DATA *p, *p_next;
  bool fRoom = FALSE, fGroup = FALSE, fMob = FALSE, fPc = FALSE;
  bool fFight = FALSE, fExc = FALSE;
  bool fSub = FALSE;

  /* Useless conditional */
  if( fRoom != FALSE )
    fRoom = FALSE;

  argument = one_argument (argument, range);
  if (IS_NULLSTR(range)){
    bug("prog_for: NULL range passed", 0);
    return;
  }
  argument = one_argument (argument, target);
  if (IS_NULLSTR(target)){
    bug("prog_for: NULL target passed", 0);
    return;
  }
  if (IS_NULLSTR(argument)){
    bug("prog_for: NULL argument passed", 0);
    return;
  }
  if (!str_prefix("pfor", argument))
    return;

  /* check what range this is for */
  if (!str_cmp (range, "room"))
    fRoom = TRUE;
  else if (!str_cmp (range, "exc"))
    fExc = TRUE;
  else if (!str_cmp (range, "group"))
    fGroup = TRUE;
  else if (!str_cmp (range, "mob"))
    fMob = TRUE;
  else if (!str_cmp (range, "pc"))
    fPc = TRUE;
  else if (!str_cmp (range, "fight"))
    fFight = TRUE;
  else{
    bug ( "prog_For: Invalid argument: range", 0);
    return;
  }

  /* get target */
  if ( (victim = get_char(target)) == NULL){
    bug("prog_for: Target victim  not found.", 0);
    return;
  }

  /* check for name substitution */
  if (strchr (argument, '#'))
    fSub = TRUE;

  /* begin doing things based on the range */
  for (p = victim->in_room->people; p; p = p_next){
    p_next = p->next_in_room;

    if (p->invis_level > 1 || p->incog_level > 1)
      continue;

    if ((fExc && p == victim)
	|| (fGroup && !is_same_group(victim, p))
	|| (fMob && !IS_NPC(p))
	|| (fPc && IS_NPC(p))
	|| (fFight && (!p->fighting || p->fighting != victim)) )
      continue;

    if (fSub){
      char buf[MSL];
      char *pSource = argument, *pDest = buf;
      while (*pSource){
	if (*pSource == '#'){
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
      if (room)
	room_interpret(room, buf);
      else if (obj){
	obj_interpret(obj, buf);
	if (obj == NULL )
	  return;
      }
      else if (ch){
	mob_interpret(ch, buf);
	if (ch == NULL || ch->in_room == NULL)
	  return;
      }
    }//End subsititute
  }//END cycle room
}

/* mob for command using above */
void do_mpfor(CHAR_DATA* ch, char* argument){
  prog_for(ch, NULL, NULL, argument);
}

/* mob destroy command using code from above */
void do_mpdestroy( CHAR_DATA* ch, char* argument ){
  prog_destroy( ch->in_room, argument );
}

/* allows actor to set path on taret mob */
void do_mppath( CHAR_DATA* ch, char* argument ){
  prog_path( ch, argument );
}

/* allows actor to give an automatic quest */
void do_mpaquest( CHAR_DATA* ch, char* argument ){
  aquest( ch, argument );
}

/* allows actor to give an clan a quest */
void do_mpcquest( CHAR_DATA* ch, char* argument ){
  cquest( ch, argument );
}

/*
 * OBJprog section
 */
/*
 * Lets the mobile stop char who triggered the prog from
 * exiting the room. (TRIG_EXIT/TRIG_EXALL only)
 * Aiming at char who did not trigger might have unpredictable
 * results.
 * Syntax: obj stop <char>
 */
void do_opstop( OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *victim;
    if (obj->in_room == NULL)
      return;

    if ( ( victim = get_char_room( NULL, obj->in_room, argument ) ) == NULL )
      return;
    /* flag is cleared after TRIG_EX** ends */
    SET_BIT(victim->affected_by, AFF_FLAG);
}

void do_obj( OBJ_DATA *obj, char *argument )
{
    /*
     * Security check!
     */
    if ( obj->level < MAX_LEVEL )
	return;
    obj_interpret( obj, argument );
}
/*
 * Obj command interpreter. Implemented separately for security and speed
 * reasons. A trivial hack of interpret()
 */
void obj_interpret( OBJ_DATA *obj, char *argument )
{
    char buf[MAX_STRING_LENGTH], command[MAX_INPUT_LENGTH];
    int cmd;

    argument = one_argument( argument, command );

    /*
     * Look for command in command table.
     */
    for ( cmd = 0; obj_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == obj_cmd_table[cmd].name[0]
	&&   !str_prefix( command, obj_cmd_table[cmd].name ) )
	{
	    (*obj_cmd_table[cmd].obj_fun) ( obj, argument );
	    tail_chain( );
	    return;
	}
    }
    sprintf( buf, "Obj_interpret: invalid cmd from obj %d: '%s'",
	obj->pIndexData->vnum, command );
    bug( buf, 0 );
}

/*
 * Displays OBJprogram triggers of an object
 *
 * Syntax: opstat [name]
 */
void do_opstat( CHAR_DATA *ch, char *argument )
{
    char        arg[ MAX_STRING_LENGTH  ];
    PROG_LIST  *oprg;
    OBJ_DATA   *obj;
    int i;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Opstat what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    {
	send_to_char( "No such object.\n\r", ch );
	return;
    }

    sprintf( arg, "Object #%-6d [%s]\n\r",
	obj->pIndexData->vnum, obj->short_descr );
    send_to_char( arg, ch );

    sprintf( arg, "Delay   %-6d [%s]\n\r",
	obj->oprog_delay,
	obj->oprog_target == NULL
		? "No target" : obj->oprog_target->name );
    send_to_char( arg, ch );

    if ( !obj->pIndexData->oprog_flags )
    {
	send_to_char( "[No programs set]\n\r", ch);
	return;
    }

    for ( i = 0, oprg = obj->pIndexData->oprogs; oprg != NULL;
	 oprg = oprg->next )

    {
	sprintf( arg, "[%2d] Trigger [%-8s] Program [%4d] Phrase [%s]\n\r",
	      ++i,
	      prog_type_to_name( oprg->trig_type ),
	      oprg->vnum,
	      oprg->trig_phrase );
	send_to_char( arg, ch );
    }

    return;

}

/*
 * Displays the source code of a given OBJprogram
 *
 * Syntax: opdump [vnum]
 */
void do_opdump( CHAR_DATA *ch, char *argument )
{
   char buf[ MAX_INPUT_LENGTH ];
   PROG_CODE *oprg;

   one_argument( argument, buf );
   if ( ( oprg = get_prog_index( atoi(buf), PRG_OPROG ) ) == NULL )
   {
	send_to_char( "No such OBJprogram.\n\r", ch );
	return;
   }
   page_to_char( oprg->code, ch );
}

/*
 * Prints the argument to all active players in the game
 *
 * Syntax: obj gecho [string]
 */
void do_opgecho( OBJ_DATA *obj, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "OpGEcho: missing argument from vnum %d",
	    obj->pIndexData->vnum );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Obj echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the argument to all players in the same area as the mob
 *
 * Syntax: obj zecho [string]
 */
void do_opzecho( OBJ_DATA *obj, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "OpZEcho: missing argument from vnum %d",
	    obj->pIndexData->vnum );
	return;
    }

    if ( obj->in_room == NULL && (obj->carried_by == NULL || obj->carried_by->in_room == NULL) )
	return;

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character->in_room != NULL
	&&   ( (obj->in_room && d->character->in_room->area == obj->in_room->area)
	|| (obj->carried_by && d->character->in_room->area == obj->carried_by->in_room->area) ) )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Obj echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the message to everyone in the room other than the mob and victim
 *
 * Syntax: obj echoaround [victim] [string]
 */

void do_opechoaround( OBJ_DATA *obj, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return;

    if ( ( victim=get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL )
	return;
    act(argument, victim, obj, victim, TO_ROOM);
}

/*
 * Prints the message to only the victim
 *
 * Syntax: obj echoat [victim] [string]
 */
void do_opechoat( OBJ_DATA *obj, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    CHAR_DATA* ch = NULL;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
	return;

    if ( ( victim = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL )
	return;
    ch = obj->carried_by ? obj->carried_by : obj->in_room->people;
    if (ch)
      act( argument, ch, obj, victim, ch == victim ? TO_CHAR : TO_VICT);
}

/*
 * Prints the message to the room at large
 *
 * Syntax: obj echo [string]
 */
void do_opecho( OBJ_DATA *obj, char *argument )
{
  ROOM_INDEX_DATA* room = NULL;
    if ( argument[0] == '\0' )
	return;
    if (obj->carried_by)
      room = obj->carried_by->in_room;
    else if (obj->in_obj)
      room = obj->in_obj->in_room;
    else if (obj->in_room)
      room = obj->in_room;
    else
      room = NULL;

    if ( !room || !room->people )
      return;

    act( argument, room->people, NULL, NULL, TO_ROOM );
    act( argument, room->people, NULL, NULL, TO_CHAR );
}

/*
 * Lets the object load a mobile.
 *
 * Syntax: obj mload [vnum]
 */
void do_opmload( OBJ_DATA *obj, char *argument )
{
    char            arg[ MAX_INPUT_LENGTH ];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA      *victim;
    int vnum;

    one_argument( argument, arg );

    if ( (obj->in_room == NULL
      && (obj->carried_by == NULL || obj->carried_by->in_room == NULL))
      || arg[0] == '\0' || !is_number(arg) )
	return;

    vnum = atoi(arg);
    if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
    {
	sprintf( arg, "Opmload: bad mob index (%d) from obj %d",
	    vnum, obj->pIndexData->vnum );
	bug( arg, 0 );
	return;
    }
    victim = create_mobile( pMobIndex );
    char_to_room( victim, obj->in_room?obj->in_room:obj->carried_by->in_room );
    return;
}

/*
 * Lets the object load another object
 *
 * Syntax: obj oload [vnum] [level] [character/object]
 */
void do_opoload( OBJ_DATA *obj, char *argument )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];
    OBJ_INDEX_DATA *pObjIndex;
    CHAR_DATA*	   ch = NULL;
    OBJ_DATA*	   container = NULL;
    OBJ_DATA       *nobj;
    int             level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        bug( "Opoload - Bad syntax from vnum %d.",
	    obj->pIndexData->vnum );
        return;
    }

    if ( arg2[0] == '\0' )
    {
	level = obj->level;
    }
    else
    {
	/*
	 * New feature from Alander.
	 */
        if ( is_number( arg2 ) )
        {
	  level = atoi( arg2 );
	  if ( level < 0 || level > obj->level )
	    {
	      bug( "Opoload - Bad level from vnum %d.",
		obj->pIndexData->vnum );
	      return;
	    }
	  /* viri: check for mob to load to */
	  if (!IS_NULLSTR(arg3)){
	    ROOM_INDEX_DATA* room = NULL;
	    if (obj->in_room)
	      room = obj->in_room;
	    else if (obj->carried_by && obj->carried_by->in_room)
	      room = obj->carried_by->in_room;
	    if ( room){
	      if ( (ch = get_char_room(NULL, room, arg3)) == NULL)
		container = get_obj_list( NULL, arg3, room->contents);
	    }
	  }
	}
	else{
	  level = obj->level;
	  /* viri: check for mob to load to */
	  if (!IS_NULLSTR(arg2)){
	    ROOM_INDEX_DATA* room = NULL;
	    if (obj->in_room)
	      room = obj->in_room;
	    else if (obj->carried_by && obj->carried_by->in_room)
	      room = obj->carried_by->in_room;
	    if ( room)
	      ch = get_char_room(NULL, room, arg2);
	  }
	}
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	bug( "Opoload - Bad vnum arg from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    nobj = create_object( pObjIndex, level );
    if (ch)
      obj_to_char(nobj, ch);
    else if (container)
      obj_to_obj( nobj, container );
    else
      obj_to_room( nobj, obj->in_room?obj->in_room:obj->carried_by->in_room );
    return;
}

/*
 * Lets the object purge all other objects and npcs in the room,
 * or purge a specified object or mob in the room. The object cannot
 * purge itself for safety reasons.
 *
 * syntax obj purge {target}
 */
void do_oppurge( OBJ_DATA *obj, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_DATA  *vobj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        /* 'purge' */
        CHAR_DATA *vnext;
        OBJ_DATA  *obj_next;

	if ( obj->in_room && obj->in_room->people )
	    victim = obj->in_room->people;
	else if ( obj->carried_by && obj->carried_by->in_room )
	    victim = obj->carried_by->in_room->people;
	else
	    victim = NULL;

	for ( ; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC( victim )
	    &&   !IS_SET(victim->act, ACT_NOPURGE) )
		extract_char( victim, TRUE );
	}

	if ( obj->in_room )
	    vobj = obj->in_room->contents;
	else
	    vobj = obj->carried_by->in_room->contents;

	for ( ; vobj != NULL; vobj = obj_next )
	{
	    obj_next = vobj->next_content;
	    if ( !IS_SET(vobj->extra_flags, ITEM_NOPURGE) && vobj != obj )
		extract_obj( vobj );
	}

	return;
    }

    if ( ( victim = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL )
    {
	if ( ( vobj = get_obj_here( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) )
	{
	    extract_obj( vobj );
	}
	else if ( obj->carried_by && (vobj = get_obj_carry( obj->carried_by, arg, NULL )) != NULL)
	{
	    extract_obj( vobj );
	}
	else if ( obj->carried_by && (vobj = get_obj_wear( obj->carried_by, arg, FALSE )) != NULL )
	{
	    unequip_char( vobj->carried_by, vobj );
	    extract_obj( vobj );
	}
	else
	{
	    bug( "Oppurge - Bad argument from vnum %d.",
		obj->pIndexData->vnum );
	}
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	bug( "Oppurge - Purging a PC from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    extract_char( victim, TRUE );
    return;
}


/*
 * Lets the object goto any location it wishes that is not private.
 *
 * Syntax: obj goto [location] {I}
 */
void do_opgoto( OBJ_DATA *obj, char *argument )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim = NULL;
    OBJ_DATA *dobj;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "Opgoto - No argument from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( is_number(arg) )
	location = get_room_index( atoi( arg ) );
    else if ( (victim = get_char_world( NULL, arg )) != NULL )
	location = victim->in_room;
    else if ( ( dobj = get_obj_world( NULL, arg )) != NULL )
	location = dobj->in_room;
    else
    {
	bug( "Opgoto - No such location from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( obj->in_room != NULL )
	obj_from_room( obj );
    else if ( obj->carried_by != NULL )
	obj_from_char( obj );
    else if (obj->in_obj != NULL)
      obj_from_obj(obj);
/* if "I" passed as last argument we put in inventory */
    if (victim && !IS_NULLSTR(argument) && LOWER(argument[0]) == 'i')
      obj_to_char(obj, victim);
    else
      obj_to_room( obj, location );
    return;
}

/*
 * Lets the object transfer people.  The 'all' argument transfers
 *  everyone in the current room to the specified location
 *
 * Syntax: obj transfer [target|'all'] [location]
 */
void do_optransfer( OBJ_DATA *obj, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    CHAR_DATA       *victim;
    OBJ_DATA	    *dobj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Optransfer - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	CHAR_DATA *victim_next;

	if ( obj->in_room && obj->in_room->people )
	    victim = obj->in_room->people;
	else if ( obj->carried_by )
	    victim = obj->carried_by->in_room->people;
	else
	    victim = NULL;
	for ( ; victim != NULL; victim = victim_next )
	{
	    victim_next = victim->next_in_room;
	    if (IS_NPC(victim) && victim->master == NULL)
	      continue;

	    sprintf( buf, "'%s' %s", victim->name, arg2 );
	    do_optransfer( obj, buf );

	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = obj->in_room?obj->in_room:obj->carried_by->in_room;
    }
    else
    {
	if ( is_number(arg2))
	    location = get_room_index( atoi(arg2) );
	else if ( (victim = get_char_world( NULL, arg2 )) != NULL )
	    location = victim->in_room;
	else if ( ( dobj = get_obj_world( NULL, arg2 )) != NULL )
	    location = dobj->in_room;
	else
	{
	    bug( "Optransfer - No such location from vnum %d.",
	        obj->pIndexData->vnum );
	    return;
	}

	if ( room_is_private( location ) )
	    return;
    }

    if ( ( victim = get_char_world( NULL, arg1 ) ) == NULL )
	return;

    if ( victim->in_room == NULL )
	return;
    if (location == NULL)
      return;
    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    char_from_room( victim );
    char_to_room( victim, location );
    do_look( victim, "auto" );

    return;
}

/*
 * Lets the object transfer all chars in same group as the victim.
 *
 * Syntax: obj gtransfer [victim] [location]
 */
void do_opgtransfer( OBJ_DATA *obj, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    CHAR_DATA       *who, *victim, *victim_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Opgtransfer - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( (who = get_char_room( NULL, (obj->in_room)?obj->in_room:obj->carried_by->in_room, arg1 )) == NULL )
	return;

    if ( obj->in_room && obj->in_room->people )
	victim = obj->in_room->people;
    else if ( obj->carried_by && obj->carried_by->in_room)
	victim = obj->carried_by->in_room->people;
    else
	victim = NULL;

    for ( ; victim; victim = victim_next )
    {
    	victim_next = victim->next_in_room;
    	if( who != victim && is_same_group( who,victim ) )
    	{
	    sprintf( buf, "'%s' %s", victim->name, arg2 );
	    do_optransfer( obj, buf );
    	}
    }
    sprintf( buf, "'%s' %s", who->name, arg2 );
    do_optransfer( obj, buf );
    return;
}

/*
 * Lets the object transfer all chars in same room as the victim except victim
 * unless victim is "all"
 * Usefull for scripted transfers
 * Syntax: obj rtransfer [victim] [location]
 * Syntax: obj rtransfer "all" [location]
 */
void do_oprtransfer( OBJ_DATA *obj, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    CHAR_DATA       *who = NULL, *victim, *victim_next;
    bool fAll = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Oprtransfer - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    if (!str_cmp(arg1, "all"))
      fAll = TRUE;
    else if ( (who = get_char_room( NULL, (obj->in_room)?obj->in_room:obj->carried_by->in_room, arg1 )) == NULL )
	return;

    if ( obj->in_room && obj->in_room->people )
	victim = obj->in_room->people;
    else if ( obj->carried_by && obj->carried_by->in_room)
	victim = obj->carried_by->in_room->people;
    else
	victim = NULL;

    for ( ; victim; victim = victim_next )
    {
    	victim_next = victim->next_in_room;
    	if( fAll
	    || who == NULL
	    || who != victim){
	  sprintf( buf, "%s %s", victim->name, arg2 );
	  do_optransfer( obj, buf );
	}

    }
    return;
}

/*
 * Lets the object force someone to do something. Must be mortal level
 * and the all argument only affects those in the room with the object.
 *
 * Syntax: obj force [victim] [commands]
 */
void do_opforce( OBJ_DATA *obj, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "Opforce - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( !obj->in_room && !obj->carried_by )
	return;
    if ( obj->carried_by && !obj->carried_by->in_room )
	return;

    if ( !str_cmp( arg, "all" ) )
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( ((obj->in_room && vch->in_room == obj->in_room)
		|| (obj->carried_by && vch->in_room == obj->carried_by->in_room))
		&& get_trust( vch ) < obj->level )
	    {
		interpret( vch, argument );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_room( NULL, (obj->in_room)?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL ){
	  if ( (victim = get_char( arg )) == NULL)
	    return;
	}
	interpret( victim, argument );
    }

    return;
}

/*
 * Lets the object force a group something. Must be mortal level.
 *
 * Syntax: obj gforce [victim] [commands]
 */
void do_opgforce( OBJ_DATA *obj, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim, *vch, *vch_next;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "OpGforce - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( ( victim = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL )
	return;

    for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( is_same_group(victim,vch) )
        {
	    interpret( vch, argument );
	}
    }
    return;
}

/*
 * Forces all mobiles of certain vnum to do something
 *
 * Syntax: obj vforce [vnum] [commands]
 */
void do_opvforce( OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *victim, *victim_next;
    char arg[ MAX_INPUT_LENGTH ];
    int vnum;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "OpVforce - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( !is_number( arg ) )
    {
	bug( "OpVforce - Non-number argument vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    vnum = atoi( arg );

    for ( victim = char_list; victim; victim = victim_next )
    {
	victim_next = victim->next;
	if ( IS_NPC(victim) && victim->pIndexData->vnum == vnum
	&& victim->fighting == NULL )
	    interpret( victim, argument );
    }
    return;
}

/*
 * Lets obj cause unconditional damage to someone. Nasty, use with caution.
 * Also, this is silent, you must show your own damage message...
 *
 * Syntax: obj damage [victim] [min] [max] {kill}
 */
void do_opdamage( OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *victim = NULL, *victim_next;
    char target[ MAX_INPUT_LENGTH ],
	 min[ MAX_INPUT_LENGTH ],
	 max[ MAX_INPUT_LENGTH ];
    int low, high;
    bool fAll = FALSE, fKill = FALSE;

    argument = one_argument( argument, target );
    argument = one_argument( argument, min );
    argument = one_argument( argument, max );

    if ( target[0] == '\0' )
    {
	bug( "OpDamage - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    if( !str_cmp( target, "all" ) )
	fAll = TRUE;
    else if( ( victim = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, target ) ) == NULL )
	return;

    if ( is_number( min ) )
	low = atoi( min );
    else
    {
	bug( "OpDamage - Bad damage min vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    if ( is_number( max ) )
	high = atoi( max );
    else
    {
	bug( "OpDamage - Bad damage max vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    one_argument( argument, target );

    /*
     * If kill parameter is omitted, this command is "safe" and will not
     * kill the victim.
     */

    if ( target[0] != '\0' )
	fKill = TRUE;
    if ( fAll )
    {
	if ( obj->in_room && obj->in_room->people )
	    victim = obj->in_room->people;
	else if ( obj->carried_by )
	    victim = obj->carried_by->in_room->people;
	else
	    victim = NULL;

	for( ; victim; victim = victim_next )
	{
	    victim_next = victim->next_in_room;
	    if ( obj->carried_by && victim == obj->carried_by )
	      continue;
	    damage( victim, NULL,
		    fKill ?
		    number_range(low,high) : UMIN(victim->hit - 1, number_range(low,high)),
		    TYPE_UNDEFINED, DAM_NONE, FALSE );
	}
    }
    else
      damage( victim, NULL,
	      fKill ?
	      number_range(low,high) : UMIN(victim->hit - 1, number_range(low,high)),
	      TYPE_UNDEFINED, DAM_NONE, FALSE );
    return;
}

/*
 * Lets the object to remember a target. The target can be referred to
 * with $q and $Q codes in OBJprograms. See also "obj forget".
 *
 * Syntax: obj remember [victim]
 */
void do_opremember( OBJ_DATA *obj, char *argument )
{
  //   char arg[ MAX_INPUT_LENGTH ];
    //    one_argument( argument, arg );
    if ( argument[0] != '\0' )
	obj->oprog_target = get_char( argument );
    else
	bug( "OpRemember: missing argument from vnum %d.",
		obj->pIndexData->vnum );
}

/*
 * Reverse of "obj remember".
 *
 * Syntax: obj forget
 */
void do_opforget( OBJ_DATA *obj, char *argument )
{
    obj->oprog_target = NULL;
}

/*
 * Sets a delay for OBJprogram execution. When the delay time expires,
 * the object is checked for a OBJprogram with DELAY trigger, and if
 * one is found, it is executed. Delay is counted in PULSE_TICK
 *
 * Syntax: obj delay [pulses]
 */
void do_opdelay( OBJ_DATA *obj, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );
    if ( !is_number( arg ) )
    {
	bug( "OpDelay: invalid arg from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    obj->oprog_delay = atoi( arg );
}

/*
 * Reverse of "obj delay", deactivates the timer.
 *
 * Syntax: obj cancel
 */
void do_opcancel( OBJ_DATA *obj, char *argument )
{
   obj->oprog_delay = 0;
}
/*
 * Lets the object to call another OBJprogram withing a OBJprogram.
 * This is a crude way to implement subroutines/functions. Beware of
 * nested loops and unwanted triggerings... Stack usage might be a problem.
 * Characters and objects referred to must be in the same room with the
 * mobile.
 *
 * Syntax: obj call [vnum] [victim|'null'] [object1|'null'] [object2|'null']
 *
 */
void do_opcall( OBJ_DATA *obj, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *vch;
    OBJ_DATA *obj1, *obj2;
    PROG_CODE *prg;
    extern void program_flow( sh_int, char *, CHAR_DATA *, OBJ_DATA *, ROOM_INDEX_DATA *, CHAR_DATA *, const void *, const void * );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "OpCall: missing arguments from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    if ( ( prg = get_prog_index( atoi(arg), PRG_OPROG ) ) == NULL )
    {
	bug( "OpCall: invalid prog from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    vch = NULL;
    obj1 = obj2 = NULL;
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
        vch = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj1 = get_obj_here( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj2 = get_obj_here( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg );
    program_flow( prg->vnum, prg->code, NULL, obj, NULL, vch, (void *)obj1, (void *)obj2 );
}

/*
 * Lets the object to transfer an object. The object must be in the same
 * room with the object.  Addition of "inv" places object into inventory
 * of character location
 *
 * Syntax: obj otransfer [item name] [location] ["inv"]
 */
void do_opotransfer( OBJ_DATA *obj, char *argument )
{
    OBJ_DATA *obj1, *dobj;
    ROOM_INDEX_DATA *location;
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim = NULL;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "OpOTransfer - Missing argument from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    argument = one_argument( argument, buf );
    if (IS_NULLSTR(buf))
      location = obj->in_room?obj->in_room:obj->carried_by->in_room;
    else if ( is_number( buf ) )
	location = get_room_index( atoi(buf) );
    else if ( (victim = get_char_world( NULL, buf )) != NULL )
	location = victim->in_room;
    else if ( ( dobj = get_obj_world( NULL, arg )) != NULL )
	location = dobj->in_room;
    else
    {
	bug( "OpOTransfer - No such location from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    if (obj->carried_by){
      if ( (obj1 = get_obj_here( obj->carried_by, NULL, arg )) == NULL )
	return;
    }
    else if (obj->in_obj){
      if ( (obj1 = get_obj_here( NULL, in_obj(obj)->in_room, arg )) == NULL )
	return;
    }
    else{
      if ( (obj1 = get_obj_here( NULL, obj->in_room, arg )) == NULL )
	return;
    }
    if ( obj1->carried_by == NULL )
      obj_from_room( obj1 );
    else
      {
	if ( obj1->wear_loc != WEAR_NONE )
	    unequip_char( obj1->carried_by, obj1 );
	obj_from_char( obj1 );
    }
/* check for optional "inv" word */
    if (!str_cmp("inv", argument) && victim)
      obj_to_char( obj1, victim);
    else
      obj_to_room( obj1, location );
}

/*
 * Lets the object to strip an object or all objects from the victim.
 * Useful for removing e.g. quest objects from a character.
 *
 * Syntax: obj remove [victim] [object vnum|'all']
 */
void do_opremove( OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj1, *obj_next;
    sh_int vnum = 0;
    bool fAll = FALSE;
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL )
	return;

    one_argument( argument, arg );
    if ( !str_cmp( arg, "all" ) )
	fAll = TRUE;
    else if ( !is_number( arg ) )
    {
	bug ( "OpRemove: Invalid object from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    else
	vnum = atoi( arg );

    for ( obj1 = victim->carrying; obj1; obj1 = obj_next )
    {
	obj_next = obj->next_content;
	if ( fAll || obj1->pIndexData->vnum == vnum )
	{
	     unequip_char( victim, obj1 );
	     obj_from_char( obj1 );
	     extract_obj( obj1 );
	}
    }
}

/*
 * Lets the object change another object's stats
 * target's level by using "level"
 *
 * Syntax: obj attrib [targetobj] [field] [+-* / =] [value/"level"]
 */
void do_opattrib( OBJ_DATA *obj, char *argument ){
  OBJ_DATA* vobj = obj;
  char target[MIL],
    arg1[MIL],
    arg2[MIL];
  char mod;
  int value;

  argument = one_argument( argument, target );
  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  /* grab the character target */
  if ( !strcmp( target, "self" ) ){
    vobj = obj;
  }
  else if (!strcmp( target, "none" ) )
    vobj = obj;
  else if (obj->in_room){
    if ( (vobj = get_obj_here(NULL, obj->in_room, target )) == NULL)
      return;
  }
  else if (obj->carried_by){
    if ( (vobj = get_obj_here(NULL, obj->carried_by->in_room, target )) == NULL)
      return;
  }
  else if (obj->in_obj){
    if (in_char(obj)){
      if ( (vobj = get_obj_here(NULL, in_char(obj)->in_room, target )) == NULL)
	return;
    }
    else{
      if ( (vobj = get_obj_here(NULL, in_obj(obj)->in_room, target )) == NULL)
	return;
    }
  }
  /*
     get the integer value for field
     check if operator was skipped
  */
  if (IS_NULLSTR(argument)){
    if (!str_cmp("level", arg2))
      value = obj->level;
    else
      value = atoi(arg2);
  }
  else{
    if (!str_cmp("level", argument))
      value = obj->level;
    else
      value = atoi(argument);
  }

  /*
    we have the value, time to get
    the operator
  */
  if(IS_NULLSTR(argument))
    mod = '=';
  else
    mod = arg2[0];

  /* if there was a vobj at this point it becomes the modified obj */
  /* start operating */
  if (!str_cmp("v0", arg1)){
    switch (mod){
    case '=': vobj->value[0] = value;	break;
    case '+': vobj->value[0] += value;	break;
    case '-': vobj->value[0] -= value;	break;
    case '/': vobj->value[0] /= value;	break;
    case '*': vobj->value[0] *= value;	break;
    }
  }
  else if (!str_cmp("v1", arg1)){
    switch (mod){
    case '=': vobj->value[1] = value;	break;
    case '+': vobj->value[1] += value;	break;
    case '-': vobj->value[1] -= value;	break;
    case '/': vobj->value[1] /= value;	break;
    case '*': vobj->value[1] *= value;	break;
    }
  }
  else if (!str_cmp("v2", arg1)){
    switch (mod){
    case '=': vobj->value[2] = value;	break;
    case '+': vobj->value[2] += value;	break;
    case '-': vobj->value[2] -= value;	break;
    case '/': vobj->value[2] /= value;	break;
    case '*': vobj->value[2] *= value;	break;
    }
  }
  else if (!str_cmp("v3", arg1)){
    switch (mod){
    case '=': vobj->value[3] = value;	break;
    case '+': vobj->value[3] += value;	break;
    case '-': vobj->value[3] -= value;	break;
    case '/': vobj->value[3] /= value;	break;
    case '*': vobj->value[3] *= value;	break;
    }
  }
  else if (!str_cmp("v4", arg1)){
    switch (mod){
    case '=': vobj->value[4] = value;	break;
    case '+': vobj->value[4] += value;	break;
    case '-': vobj->value[4] -= value;	break;
    case '/': vobj->value[4] /= value;	break;
    case '*': vobj->value[4] *= value;	break;
    }
  }
  else if (!str_cmp("delay", arg1)){
    switch (mod){
    case '=': vobj->oprog_delay  = value;	break;
    case '+': vobj->oprog_delay += value;	break;
    case '-': vobj->oprog_delay -= value;	break;
    case '/': vobj->oprog_delay /= value;	break;
    case '*': vobj->oprog_delay *= value;	break;
    }
  }
  else if (!str_cmp("wear", arg1)){
    int val = 0;
    if ( ( val = flag_value( wear_flags, argument ) ) != NO_FLAG ){
      switch (mod){
      case '+' :
	SET_BIT(obj->wear_flags, val);	break;
      case '-' :
	REMOVE_BIT(obj->wear_flags, val);	break;
      case '=' :
	obj->wear_flags = val;		break;
      default:
	TOGGLE_BIT(obj->wear_flags, val);
      }
    }
    else
      bug("opattrib: No such wear flag.", val);
    return;
  }
  else if (!str_cmp("extra", arg1)){
    int val = 0;
    if ( ( val = flag_value( extra_flags, argument ) ) != NO_FLAG ){
      switch (mod){
      case '+' :
	SET_BIT(obj->extra_flags, val);	break;
      case '-' :
	REMOVE_BIT(obj->extra_flags, val);	break;
      case '=' :
	obj->extra_flags = val;		break;
      default:
	TOGGLE_BIT(obj->extra_flags, val);
      }
    }
    else
      bug("opattrib: No such extra flag.", val);
    return;
  }
  else if (!str_cmp("level", arg1)){
    switch (mod){
    case '=': vobj->level = value;	break;
    case '+': vobj->level += value;	break;
    case '-': vobj->level -= value;	break;
    case '/': vobj->level /= value;	break;
    case '*': vobj->level *= value;	break;
    }
  }
  else if (!str_cmp("cost", arg1)){
    switch (mod){
    case '=': vobj->cost = value;	break;
    case '+': vobj->cost += value;	break;
    case '-': vobj->cost -= value;	break;
    case '/': vobj->cost /= value;	break;
    case '*': vobj->cost *= value;	break;
    }
  }
  else if (!str_cmp("timer", arg1)){
    switch (mod){
    case '=': vobj->timer = value;	break;
    case '+': vobj->timer += value;	break;
    case '-': vobj->timer -= value;	break;
    case '/': vobj->timer /= value;	break;
    case '*': vobj->timer *= value;	break;
    }
  }
  else if (!str_cmp("delay", arg1)){
    switch (mod){
    case '=': vobj->oprog_delay = value;	break;
    case '+': vobj->oprog_delay += value;	break;
    case '-': vobj->oprog_delay -= value;	break;
    case '/': vobj->oprog_delay /= value;	break;
    case '*': vobj->oprog_delay *= value;	break;
    }
  }
  else if (!str_cmp("short", arg1)){
    free_string(obj->short_descr);
    if (IS_NULLSTR(argument))
      obj->short_descr = str_dup( arg2 );
    else
      obj->short_descr = str_dup( argument );
  }
}

void do_oprattrib(OBJ_DATA* obj, char* argument){
  ROOM_INDEX_DATA* room = NULL;
  if (obj->in_room)
    room = obj->in_room;
  else if (obj->carried_by)
    room = obj->carried_by->in_room;
  else if (obj->in_obj)
    room = obj->in_obj->in_room;
  else
    room = NULL;
  rattrib(room, argument);
}

/*
 * Lets an object cast spells using the character object is on as
 * the caster--
 * if object is not on any char it uses target as caster
 * Beware: this does only crude checking on the target validity
 * and does not account for mana etc., so you should do all the
 * necessary checking in your mob program before issuing this cmd!
 *
 * Syntax: obj cast [spell] {target}
 */

void do_opcast( OBJ_DATA* Obj, char *argument )
{
  CHAR_DATA* ch = NULL;
  CHAR_DATA *vch;
  ROOM_INDEX_DATA* in_room = NULL;

  OBJ_DATA *obj;
  void *victim = NULL;
  char spell[ MAX_INPUT_LENGTH ],
    target[ MAX_INPUT_LENGTH ];
  int sn;

  argument = one_argument( argument, spell );
  one_argument( argument, target );

  if (!Obj->carried_by){
    if (Obj->in_room && !IS_NULLSTR(target))
      in_room = Obj->in_room;
    else
      return;
  }
  else{
    ch = Obj->carried_by;
    in_room = ch->in_room;
  }

  if ( spell[0] == '\0' )
    {
      bug( "OpCast - Bad syntax from vnum %d.",
	   Obj->pIndexData->vnum);
      return;
    }

  if ( ( sn = skill_lookup( spell ) ) < 0 )
    {
      bug( "OpCast - No such spell from vnum %d.",
	   Obj->pIndexData->vnum);
      return;
    }
  if (!IS_NULLSTR(target) && skill_table[sn].target != TAR_IGNORE){
    vch = get_char_room( NULL, in_room, target );
    if (Obj->in_room)
      ch = vch;
    if (ch)
      obj = get_obj_here( ch, NULL, target );
    else
      obj = get_obj_here( NULL, in_room, target );
  }
  else{
    vch = NULL;
    obj = NULL;
  }
  /* safety */
  if (!ch)
    return;
  switch ( skill_table[sn].target )
    {	default: return;
    case TAR_IGNORE:
      break;
    case TAR_CHAR_OFFENSIVE:
      if ( vch == NULL){
	if (ch->fighting)
	  vch = ch->fighting;
	else
	  vch = ch;
      }
      victim = ( void * ) vch;
      break;
    case TAR_CHAR_DEFENSIVE:
      victim = vch == NULL ? ( void *) ch : (void *) vch; break;
    case TAR_CHAR_SELF:
      victim = ( void *) ch; break;
    case TAR_OBJ_CHAR_DEF:
    case TAR_OBJ_CHAR_OFF:
    case TAR_OBJ_INV:
    case TAR_OBJ_ROOM:
      if ( obj == NULL )
	return;
      victim = ( void * ) obj;
    }
  (*skill_table[sn].spell_fun)( sn,
				Obj->level > 1 ? UMIN(ch->level, ch->level/3 + 2*Obj->level/3) : 1,
				ch,
				victim,
				skill_table[sn].target );
  return;
}

/*
 * Lets the mobile become a charmed follower of target
 * if timer specifedthe mob will expire after that many ticks
 * Syntax: mob obey <char> <victim> <timer>
 */
void do_opobey( OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *vch, *victim;
    ROOM_INDEX_DATA* room = obj->in_room ? obj->in_room :
      obj->carried_by ? obj->carried_by->in_room : NULL;
      char arg[MIL];

    if (!room){
      bug("opobey: object not in room or carried.", obj->pIndexData->vnum);
      return;
    }
    argument = one_argument( argument, arg );
    if ( ( vch = get_char_room( NULL, room, arg ) ) == NULL
	 || !IS_NPC(vch))
      return;
    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
      return;

    /*set charmie */
    if ( is_number(argument)){
      AFFECT_DATA af;

      af.type = gsn_timer;
      af.level= vch->level;
      af.duration = UMAX(0, atoi(argument));
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier =  0;
      affect_to_char(vch, &af);

      vch->master = victim;
      vch->leader = victim;
      SET_BIT(vch->affected_by, AFF_CHARM);

    }
    else if (!str_cmp("pet", argument)){
      vch->master = victim;
      vch->leader = victim;
      if (victim->pet)
	stop_follower( victim->pet );
      victim->pet = vch;
      SET_BIT(vch->affected_by, AFF_CHARM);
      SET_BIT(vch->act, ACT_PET);
    }

}


/*
 * Lets an object equip itsself onto given character
 * char needs to be in room with object
 * Syntax: obj equip <char> <loc>
 */
void do_opequipself( OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *victim;
    ROOM_INDEX_DATA* room = obj->in_room ? obj->in_room :
      obj->carried_by ? obj->carried_by->in_room : NULL;
      char arg[MIL];

    if (!room){
      bug("opobey: object not in room or carried.", obj->pIndexData->vnum);
      return;
    }
    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
      return;

    /* move object to victim */
    if (obj->in_room)
      obj_from_room(obj);
    else if (obj->carried_by)
      obj_from_char(obj);
    else{
      bug("do_opequip: object was not on char or in room."
	  , obj->pIndexData->vnum);
      return;
    }
    obj_to_char(obj, victim);
    /* equip it */
    wear_obj(victim, obj, TRUE, FALSE);
}

/* uses prog_gain from above */
void do_opgain(OBJ_DATA* obj, char *argument ){
  ROOM_INDEX_DATA* room = obj->in_room ? obj->in_room :
    obj->carried_by ? obj->carried_by->in_room : NULL;

  prog_gain(room, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_oppermquest(OBJ_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_permquest(ch, argument);
}

void do_opinviquest(OBJ_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_inviquest(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_optempquest(OBJ_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_tempquest(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_opremquest(OBJ_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_remquest(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_oprempsalm(OBJ_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_rempsalm(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_opaddpsalm(OBJ_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_addpsalm(ch, argument);
}

/* look for command using function above */
void do_opfor(OBJ_DATA* obj, char* argument){
  prog_for(NULL, obj, NULL, argument);
}

/* allows mob to give character a mount */
void do_opmount( OBJ_DATA* obj, char* argument ){
  OBJ_DATA* tobj = in_obj( obj);
  if (tobj->in_room == NULL)
    return;
  else
    prog_mount( tobj->in_room, argument );
}

/* obj affstrip <spell> <target>*/
/* ex: obj affstrip 'dirt kick' $n  */
void do_opaffstrip(OBJ_DATA* obj, char* argument){
  ROOM_INDEX_DATA* room = NULL;
  char arg[MIL];
  CHAR_DATA* victim;
  int sn = 0;

  if (obj->in_room)
    room = obj->in_room;
  else if (obj->carried_by)
    room = obj->carried_by->in_room;
  else if (obj->in_obj)
    room = obj->in_obj->in_room;
  else
    room = NULL;

  if (room == NULL)
    return;

  argument = one_argument(argument, arg);
  if ( (victim = get_char_room(NULL, room, argument)) == NULL){
    bug("opaffstrip: victim not found", 0);
    return;
  }
  if ( ( sn = skill_lookup( arg ) ) < 0 ){
    bug( "opaffstrip: No such skill or spell.", 0);
    return;
  }
  affect_strip(victim,sn);
  update_pos(victim);
}

/* shows syndicate score board to given char.: scoreboard <cabal> <character> */
void do_opscoreboard( OBJ_DATA* obj, char* argument ){
  const int max_member = 36;

  CHAR_DATA* ch = NULL;
  ROOM_INDEX_DATA* room = NULL;
  CABAL_DATA* pc = NULL;
  CMEMBER_DATA* cm, *cm_next;
  CMEMBER_DATA* cm_list[max_member];

  int last = 0;
  int i = 0;

  char buf[MIL];

  argument = one_argument(argument, buf);

  if (in_char( obj ))
    room = in_char( obj )->in_room;
  else if (in_obj( obj ))
    room = in_obj( obj )->in_room;
  else
    return;


  if ( (pc = get_cabal( buf )) == NULL){
    bug("do_opscoreboard: cabal not found.\n\r", 0);
    return;
  }
  else
    pc = get_parent( pc );

  if ( (ch = get_char_room( NULL, room, argument)) == NULL){
    bug("do_opscoreboard: character not found.\n\r", 0);
    return;
  }

  for (cm = pc->members; cm; cm = cm->next ){
    if (cm->level >= LEVEL_IMMORTAL)
      continue;
    cm_list[last++] = cm;
  }

  if (last < 1)
    return;

/* sort based on size */
/* sort the rares from highest level to lowest */
  for (i = 1; i < last; i++){
    int j = 0;
    for (j = 0; j  < last - i; j ++ ){
      cm = cm_list[j];
      cm_next = cm_list[j + 1];

      if (cm_next->kills > cm->kills){
	cm_list[j + 1] = cm;
	cm_list[j] = cm_next;
      }
    }
  }

/* print the list */
  for (i = 0; i < last; i++){
    sendf( ch, "%-2d. %-25s %5d\n\r",
	   i + 1, cm_list[i]->name, cm_list[i]->kills);
  }
}
/* obj destroy command using code from above */
void do_opdestroy( OBJ_DATA* obj, char* argument ){
  ROOM_INDEX_DATA* in_room = NULL;
  OBJ_DATA* in_ob = NULL;

  in_ob = in_obj( obj );

  if (in_ob->in_room)
    in_room = in_ob->in_room;
  else if (in_ob->carried_by)
    in_room = in_ob->carried_by->in_room;
  else{
    bug("opdestroy: obj not carried or in room.", 0);
    return;
  }
  prog_destroy( in_room, argument );
}

/*
 * ROOMprog section
 */

void do_room( ROOM_INDEX_DATA *room, char *argument )
{
    room_interpret( room, argument );
}

/*
 * Room command interpreter. Implemented separately for security and speed
 * reasons. A trivial hack of interpret()
 */
void room_interpret( ROOM_INDEX_DATA *room, char *argument )
{
    char buf[MAX_STRING_LENGTH], command[MAX_INPUT_LENGTH];
    int cmd;

    argument = one_argument( argument, command );

    /*
     * Look for command in command table.
     */
    for ( cmd = 0; room_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == room_cmd_table[cmd].name[0]
	&&   !str_prefix( command, room_cmd_table[cmd].name ) )
	{
	    (*room_cmd_table[cmd].room_fun) ( room, argument );
	    tail_chain( );
	    return;
	}
    }
    sprintf( buf, "Room_interpret: invalid cmd from room %d: '%s'",
	room->vnum, command );
    bug( buf, 0 );
}

/* allows actor to set path on taret mob */
void do_oppath( OBJ_DATA*obj, char* argument ){
  prog_path( NULL, argument );
}

/*
 * Displays ROOMprogram triggers of a room
 *
 * Syntax: rpstat [name]
 */
void do_rpstat( CHAR_DATA *ch, char *argument )
{
    char        arg[ MAX_STRING_LENGTH  ];
    PROG_LIST  *rprg;
    ROOM_INDEX_DATA   *room;
    int i;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	room = ch->in_room;
    else if ( !is_number( arg ) )
    {
	send_to_char( "You must provide a number.\n\r", ch );
	return;
    }
    else if ( ( room = get_room_index( atoi(arg) ) ) == NULL )
    {
	send_to_char( "No such room.\n\r", ch );
	return;
    }

    sprintf( arg, "Room #%-6d [%s]\n\r",
	room->vnum, room->name );
    send_to_char( arg, ch );

    sprintf( arg, "Delay   %-6d [%s]\n\r",
	room->rprog_delay,
	room->rprog_target == NULL
		? "No target" : room->rprog_target->name );
    send_to_char( arg, ch );

    if ( !room->rprog_flags )
    {
	send_to_char( "[No programs set]\n\r", ch);
	return;
    }

    for ( i = 0, rprg = room->rprogs; rprg != NULL;
	 rprg = rprg->next )

    {
	sprintf( arg, "[%2d] Trigger [%-8s] Program [%4d] Phrase [%s]\n\r",
	      ++i,
	      prog_type_to_name( rprg->trig_type ),
	      rprg->vnum,
	      rprg->trig_phrase );
	send_to_char( arg, ch );
    }

    return;

}

/*
 * Displays the source code of a given ROOMprogram
 *
 * Syntax: rpdump [vnum]
 */
void do_rpdump( CHAR_DATA *ch, char *argument )
{
   char buf[ MAX_INPUT_LENGTH ];
   PROG_CODE *rprg;

   one_argument( argument, buf );
   if ( ( rprg = get_prog_index( atoi(buf), PRG_RPROG ) ) == NULL )
   {
	send_to_char( "No such ROOMprogram.\n\r", ch );
	return;
   }
   page_to_char( rprg->code, ch );
}

/*
 * Prints the argument to all active players in the game
 *
 * Syntax: room gecho [string]
 */
void do_rpgecho( ROOM_INDEX_DATA *room, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "RpGEcho: missing argument from vnum %d",
	    room->vnum );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Room echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the argument to all players in the same area as the room
 *
 * Syntax: room zecho [string]
 */
void do_rpzecho( ROOM_INDEX_DATA *room, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "RpZEcho: missing argument from vnum %d",
	    room->vnum );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character->in_room != NULL
	&&   d->character->in_room->area == room->area )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Room echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the argument to all the rooms aroud the room
 *
 * Syntax: room asound [string]
 */
void do_rpasound( ROOM_INDEX_DATA *room, char *argument )
{
    int              door;

    if ( argument[0] == '\0' )
	return;

    for ( door = 0; door < 6; door++ )
    {
    	EXIT_DATA       *pexit;

      	if ( ( pexit = room->exit[door] ) != NULL
	  &&   pexit->to_room != NULL
	  &&   pexit->to_room != room
	  &&   pexit->to_room->people != NULL )
      	{
	    act( argument, pexit->to_room->people, NULL, NULL, TO_ROOM );
	    act( argument, pexit->to_room->people, NULL, NULL, TO_CHAR );
	}
    }
    return;

}

/*
 * Prints the message to everyone in the room other than the victim
 *
 * Syntax: room echoaround [victim] [string]
 */

void do_rpechoaround( ROOM_INDEX_DATA *room, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return;

    if ( ( victim=get_char_room( NULL, room, arg ) ) == NULL )
	return;

    act( argument, victim, NULL, victim, TO_NOTVICT );
}

/*
 * Prints the message to only the victim
 *
 * Syntax: room echoat [victim] [string]
 */
void do_rpechoat( ROOM_INDEX_DATA *room, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
	return;

    if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
	return;

    act( argument, victim, NULL, NULL, TO_CHAR );
}

/*
 * Prints the message to the room at large
 *
 * Syntax: rpecho [string]
 */
void do_rpecho( ROOM_INDEX_DATA *room, char *argument )
{
    if ( argument[0] == '\0' )
	return;

    if ( !room->people )
	return;

    act( argument, room->people, NULL, NULL, TO_ROOM );
    act( argument, room->people, NULL, NULL, TO_CHAR );
}

/*
 * Lets the room load a mobile.
 *
 * Syntax: room mload [vnum]
 */
void do_rpmload( ROOM_INDEX_DATA *room, char *argument )
{
    char            arg[ MAX_INPUT_LENGTH ];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA      *victim;
    int vnum;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number(arg) )
	return;

    vnum = atoi(arg);
    if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
    {
	sprintf( arg, "Rpmload: bad mob index (%d) from room %d",
	    vnum, room->vnum );
	bug( arg, 0 );
	return;
    }
    victim = create_mobile( pMobIndex );
    char_to_room( victim, room );
    return;
}

/*
 * Lets the room load an object
 *
 * Syntax: room oload [vnum] [level]
 */
void do_rpoload( ROOM_INDEX_DATA *room, char *argument )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA       *obj;
    int             level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) || arg2[0] == '\0'
      || !is_number( arg2 ) )
    {
        bug( "Rpoload - Bad syntax from vnum %d.",
	    room->vnum );
        return;
    }

    level = atoi( arg2 );
    if ( level < 0 || level > LEVEL_IMMORTAL )
    {
	bug( "Rpoload - Bad level from vnum %d.", room->vnum );
	return;
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	bug( "Rpoload - Bad vnum arg from vnum %d.", room->vnum );
	return;
    }

    obj = create_object( pObjIndex, level );
    obj_to_room( obj, room );

    return;
}

/*
 * Lets the room purge all objects npcs in the room,
 * or purge a specified object or mob in the room.
 *
 * syntax room purge {target}
 */
void do_rppurge( ROOM_INDEX_DATA *room, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        /* 'purge' */
        CHAR_DATA *vnext;
        OBJ_DATA  *obj_next;

	for ( victim = room->people; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC( victim )
	    &&   !IS_SET(victim->act, ACT_NOPURGE) )
		extract_char( victim, TRUE );
	}

	for ( obj = room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( !IS_SET(obj->extra_flags, ITEM_NOPURGE) )
		extract_obj( obj );
	}

	return;
    }

    if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
    {
	if ( ( obj = get_obj_here( NULL, room, arg ) ) )
	{
	    extract_obj( obj );
	}
	else
	{
	    bug( "Rppurge - Bad argument from vnum %d.", room->vnum );
	}
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	bug( "Rppurge - Purging a PC from vnum %d.", room->vnum );
	return;
    }
    extract_char( victim, TRUE );
    return;
}

/*
 * Lets the room transfer people.  The 'all' argument transfers
 *  everyone in the room to the specified location
 *
 * Syntax: room transfer [target|'all'] [location]
 */
void do_rptransfer( ROOM_INDEX_DATA *room, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    CHAR_DATA       *victim;
    OBJ_DATA	    *tobj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Rptransfer - Bad syntax from vnum %d.", room->vnum );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	CHAR_DATA *victim_next;

	for ( victim = room->people; victim != NULL; victim = victim_next )
	{
	    victim_next = victim->next_in_room;

	    if (IS_NPC(victim) && victim->master == NULL)
	      continue;

	    sprintf( buf, "'%s' %s", victim->name, arg2 );
	    do_rptransfer( room, buf );
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = room;
    }
    else
    {
	if ( is_number(arg2))
	    location = get_room_index( atoi(arg2) );
	else if ( (victim = get_char_world( NULL, arg2 )) != NULL )
	    location = victim->in_room;
	else if ( ( tobj = get_obj_world( NULL, arg2 )) != NULL )
	    location = tobj->in_room;
	else
	{
	    bug( "Rptransfer - No such location from vnum %d.", room->vnum );
	    return;
	}

	if ( room_is_private( location ) )
	    return;
    }

    if ( ( victim = get_char_world( NULL, arg1 ) ) == NULL )
	return;

    if ( victim->in_room == NULL )
	return;
    if (location == NULL)
      return;

    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    char_from_room( victim );
    char_to_room( victim, location );
    do_look( victim, "auto" );

    return;
}

/*
 * Lets the room transfer all chars in same group as the victim.
 *
 * Syntax: room gtransfer [victim] [location]
 */
void do_rpgtransfer( ROOM_INDEX_DATA *room, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    CHAR_DATA       *who, *victim, *victim_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Rpgtransfer - Bad syntax from vnum %d.", room->vnum );
	return;
    }

    if ( (who = get_char_room( NULL, room, arg1 )) == NULL )
	return;

    for ( victim = room->people; victim; victim = victim_next )
    {
    	victim_next = victim->next_in_room;
    	if( is_same_group( who,victim ) )
    	{
	    sprintf( buf, "'%s' %s", victim->name, arg2 );
	    do_rptransfer( room, buf );
    	}
    }
    return;
}

/*
 * Lets the room transfer all chars in it except the victim IF not "all".
 *
 * Syntax: room gtransfer [victim] [location]
 * Syntax: room gtransfer "all" [location]
 */
void do_rprtransfer( ROOM_INDEX_DATA *room, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    CHAR_DATA       *who = NULL, *victim, *victim_next;
    bool fAll = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Rprtransfer - Bad syntax from vnum %d.", room->vnum );
	return;
    }
    if (!str_cmp(arg1, "all"))
      fAll = TRUE;
    else if ( (who = get_char_room( NULL, room, arg1 )) == NULL )
      return;

    for ( victim = room->people; victim; victim = victim_next )
    {
    	victim_next = victim->next_in_room;
    	if( fAll
	    || who == NULL
	    || who != victim){
	  sprintf( buf, "%s %s", victim->name, arg2 );
	  do_rptransfer( room, buf );
	}

    }
    return;
}

/*
 * Lets the room force someone to do something. Must be mortal level
 * and the all argument only affects those in the room.
 *
 * Syntax: room force [victim] [commands]
 */
void do_rpforce( ROOM_INDEX_DATA *room, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "Rpforce - Bad syntax from vnum %d.", room->vnum );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( vch->in_room == room && !IS_IMMORTAL( vch ) )
		interpret( vch, argument );
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
	    return;

	interpret( victim, argument );
    }

    return;
}

/*
 * Lets the room force a group something. Must be mortal level.
 *
 * Syntax: room gforce [victim] [commands]
 */
void do_rpgforce( ROOM_INDEX_DATA *room, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim, *vch, *vch_next;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "RpGforce - Bad syntax from vnum %d.", room->vnum );
	return;
    }

    if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
	return;

    for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( is_same_group(victim,vch) )
	    interpret( vch, argument );
    }
    return;
}

/*
 * Forces all mobiles of certain vnum to do something
 *
 * Syntax: room vforce [vnum] [commands]
 */
void do_rpvforce( ROOM_INDEX_DATA *room, char *argument )
{
    CHAR_DATA *victim, *victim_next;
    char arg[ MAX_INPUT_LENGTH ];
    int vnum;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "RpVforce - Bad syntax from vnum %d.", room->vnum );
	return;
    }

    if ( !is_number( arg ) )
    {
	bug( "RpVforce - Non-number argument vnum %d.", room->vnum );
	return;
    }

    vnum = atoi( arg );

    for ( victim = char_list; victim; victim = victim_next )
    {
	victim_next = victim->next;
	if ( IS_NPC(victim) && victim->pIndexData->vnum == vnum
	  && victim->fighting == NULL )
	    interpret( victim, argument );
    }
    return;
}

/*
 * Lets room cause unconditional damage to someone. Nasty, use with caution.
 * Also, this is silent, you must show your own damage message...
 *
 * Syntax: room damage [victim] [min] [max] {kill} {dam_type}
 */
void do_rpdamage( ROOM_INDEX_DATA *room, char *argument )
{
    CHAR_DATA *victim = NULL, *victim_next;
    char target[ MAX_INPUT_LENGTH ],
	 min[ MAX_INPUT_LENGTH ],
	 max[ MAX_INPUT_LENGTH ];
    int low, high;
    sh_int damage_type = DAM_NONE;
    int att = 0;
    bool fAll = FALSE, fKill = FALSE;

    argument = one_argument( argument, target );
    argument = one_argument( argument, min );
    argument = one_argument( argument, max );

    if ( target[0] == '\0' )
    {
	bug( "RpDamage - Bad syntax from vnum %d.", room->vnum );
	return;
    }
    if( !str_cmp( target, "all" ) ) {
	fAll = TRUE;
    }
    else if( ( victim = get_char_room( NULL, room, target ) ) == NULL ) {
      return;
    }

    if ( is_number( min ) ) {
      low = atoi( min );
    }
    else {
	bug( "RpDamage - Bad damage min vnum %d.", room->vnum );
	return;
    }
    if ( is_number( max ) )
	high = atoi( max );
    else
    {
	bug( "RpDamage - Bad damage max vnum %d.", room->vnum );
	return;
    }
    one_argument( argument, target );

    /*
     * If kill parameter is omitted, this command is "safe" and will not
     * kill the victim.
     */

    if( !str_cmp( target, "kill" ) ) {
      fKill = TRUE;
      one_argument (argument, target);
    }

/* else this may be a damage type */
    for ( att = 0; attack_table[att].name != NULL; att++) {
      if (!str_cmp( target, attack_table[att].name)) {
	damage_type = attack_table[att].damage;
	one_argument (argument, target);
      }
    }

    if ( fAll ) {
      for( victim = room->people; victim; victim = victim_next ) {
	victim_next = victim->next_in_room;
	damage( victim, NULL, fKill ?
		    number_range(low,high) : UMIN(victim->hit,number_range(low,high)),
		    TYPE_UNDEFINED, damage_type, FALSE );
      }
    }
    else {
      damage( victim, NULL, fKill ?
	      number_range(low,high) : UMIN(victim->hit,number_range(low,high)),
	      TYPE_UNDEFINED, damage_type, FALSE );
    }
    return;
}

/*
 * Lets the room remember a target. The target can be referred to
 * with $q and $Q codes in ROOMprograms. See also "room forget".
 *
 * Syntax: room remember [victim]
 */
void do_rpremember( ROOM_INDEX_DATA *room, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    one_argument( argument, arg );
    if ( arg[0] != '\0' )
	room->rprog_target = get_char( arg );
    else
	bug( "RpRemember: missing argument from vnum %d.", room->vnum );
}

/*
 * Reverse of "room remember".
 *
 * Syntax: room forget
 */
void do_rpforget( ROOM_INDEX_DATA *room, char *argument )
{
    room->rprog_target = NULL;
}

/*
 * Sets a delay for ROOMprogram execution. When the delay time expires,
 * the room is checked for a ROOMprogram with DELAY trigger, and if
 * one is found, it is executed. Delay is counted in PULSE_AREA
 *
 * Syntax: room delay [pulses]
 */
void do_rpdelay( ROOM_INDEX_DATA *room, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );
    if ( !is_number( arg ) )
    {
	bug( "RpDelay: invalid arg from vnum %d.", room->vnum );
	return;
    }
    room->rprog_delay = atoi( arg );
}

/*
 * Reverse of "room delay", deactivates the timer.
 *
 * Syntax: room cancel
 */
void do_rpcancel( ROOM_INDEX_DATA *room, char *argument )
{
   room->rprog_delay = 0;
}
/*
 * Lets the room call another ROOMprogram within a ROOMprogram.
 * This is a crude way to implement subroutines/functions. Beware of
 * nested loops and unwanted triggerings... Stack usage might be a problem.
 * Characters and objects referred to must be in the room.
 *
 * Syntax: room call [vnum] [victim|'null'] [object1|'null'] [object2|'null']
 *
 */
void do_rpcall( ROOM_INDEX_DATA *room, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *vch;
    OBJ_DATA *obj1, *obj2;
    PROG_CODE *prg;
    extern void program_flow( sh_int, char *, CHAR_DATA *, OBJ_DATA *, ROOM_INDEX_DATA *, CHAR_DATA *, const void *, const void * );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "RpCall: missing arguments from vnum %d.", room->vnum );
	return;
    }
    if ( ( prg = get_prog_index( atoi(arg), PRG_RPROG ) ) == NULL )
    {
	bug( "RpCall: invalid prog from vnum %d.", room->vnum );
	return;
    }
    vch = NULL;
    obj1 = obj2 = NULL;
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
        vch = get_char_room( NULL, room, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj1 = get_obj_here( NULL, room, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj2 = get_obj_here( NULL, room, arg );
    program_flow( prg->vnum, prg->code, NULL, NULL, room, vch, (void *)obj1, (void *)obj2 );
}

/*
 * Lets the room transfer an object. The object must be in the room.
 *
 * Syntax: room otransfer [item name] [location]
 */
void do_rpotransfer( ROOM_INDEX_DATA *room, char *argument )
{
    OBJ_DATA *obj, *tobj;
    ROOM_INDEX_DATA *location;
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "RpOTransfer - Missing argument from vnum %d.", room->vnum );
	return;
    }
    one_argument( argument, buf );

    if ( is_number( buf ) )
	location = get_room_index( atoi(buf) );
    else if ( (victim = get_char_world( NULL, buf )) != NULL )
	location = victim->in_room;
    else if ( ( tobj = get_obj_world( NULL, arg )) != NULL )
	location = tobj->in_room;
    else
    {
	bug( "RpOTransfer - No such location from vnum %d.", room->vnum );
	return;
    }

    if ( (obj = get_obj_here( NULL, room, arg )) == NULL )
	return;

    if ( obj->carried_by == NULL )
	obj_from_room( obj );
    else
    {
	if ( obj->wear_loc != WEAR_NONE )
	    unequip_char( obj->carried_by, obj );
	obj_from_char( obj );
    }
    obj_to_room( obj, location );
}

/*
 * Lets the room strip an object or all objects from the victim.
 * Useful for removing e.g. quest objects from a character.
 *
 * Syntax: room remove [victim] [object vnum|'all']
 */
void do_rpremove( ROOM_INDEX_DATA *room, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj, *obj_next;
    sh_int vnum = 0;
    bool fAll = FALSE;
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
	return;

    one_argument( argument, arg );
    if ( !str_cmp( arg, "all" ) )
	fAll = TRUE;
    else if ( !is_number( arg ) )
    {
	bug ( "RpRemove: Invalid object from vnum %d.", room->vnum );
	return;
    }
    else
	vnum = atoi( arg );

    for ( obj = victim->carrying; obj; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( fAll || obj->pIndexData->vnum == vnum )
	{
	     unequip_char( victim, obj );
	     obj_from_char( obj );
	     extract_obj( obj );
	}
    }
}


/*
 * Lets the room stop char who triggered the prog from
 * exiting the room. (TRIG_EXIT/TRIG_EXALL only)
 * Aiming at char who did not trigger might have unpredictable
 * results.
 * Syntax: room stop <char>
 */
void do_rpstop( ROOM_INDEX_DATA *room, char *argument )
{
    CHAR_DATA *victim;
    if ( ( victim = get_char_room( NULL, room, argument ) ) == NULL )
      return;
    /* flag is cleared after TRIG_EX** ends */
    SET_BIT(victim->affected_by, AFF_FLAG);
}

/*
 * Lets the mobile become a charmed follower of target
 * if timer specifed the mob will expire after that many ticks
 * Syntax: room obey <char> <victim> <timer>
 */
void do_rpobey( ROOM_INDEX_DATA *room, char *argument )
{
    CHAR_DATA *vch, *victim;
    char arg[MIL];

    argument = one_argument( argument, arg );
    if ( ( vch = get_char_room( NULL, room,  arg ) ) == NULL
	 || !IS_NPC(vch))
      return;
    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
      return;


    /*set charmie */
    if ( is_number(argument)){
      AFFECT_DATA af;

      af.type = gsn_timer;
      af.level= vch->level;
      af.duration = UMAX(0, atoi(argument));
      af.where = TO_AFFECTS;
      af.bitvector = 0;
      af.location = APPLY_NONE;
      af.modifier =  0;
      affect_to_char(vch, &af);
    }
    else if (!str_cmp("pet", argument)){
      vch->master = victim;
      vch->leader = victim;
      if (victim->pet)
	stop_follower( victim->pet );
      victim->pet = vch;
      SET_BIT(vch->affected_by, AFF_CHARM);
      SET_BIT(vch->act, ACT_PET);
    }
    else{
      vch->master = victim;
      vch->leader = victim;
      SET_BIT(vch->affected_by, AFF_CHARM);
    }
}




/* uses prog_gain from above */
void do_rpgain(ROOM_INDEX_DATA* room, char *argument ){
  prog_gain(room, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_rppermquest(ROOM_INDEX_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_permquest(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_rpinviquest(ROOM_INDEX_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_inviquest(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_rptempquest(ROOM_INDEX_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_tempquest(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_rpremquest(ROOM_INDEX_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_remquest(ch, argument);
}


/* This is re-implementation of same command in "misc.c" */
void do_rprempsalm(ROOM_INDEX_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_rempsalm(ch, argument);
}

/* This is re-implementation of same command in "misc.c" */
void do_rpaddpsalm(ROOM_INDEX_DATA* obj, char* argument){
  char arg[MIL];
  CHAR_DATA* ch;
  one_argument(argument, arg);

  if ( (ch = get_char(arg)) != NULL)
    do_addpsalm(ch, argument);
}



/* room for command using function above */
void do_rpfor(ROOM_INDEX_DATA* room, char* argument){
  prog_for(NULL, NULL, room, argument);
}

void do_rprattrib(ROOM_INDEX_DATA* room, char* argument){
  rattrib(room, argument);
}

/* allows mob to give character a mount */
void do_rpmount( ROOM_INDEX_DATA* room, char* argument ){
  if (room == NULL)
    return;
  else
    prog_mount( room, argument );
}
void do_rpdestroy( ROOM_INDEX_DATA* room, char* argument){
  prog_destroy( room, argument );
}
/* allows actor to set path on taret mob */
void do_rppath( ROOM_INDEX_DATA* room, char* argument){
  prog_path( NULL, argument );
}
