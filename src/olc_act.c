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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"
#include "cabal.h"
#include "vote.h"
#include "recycle.h"
#include "jail.h"
#include "armies.h"
#include "clan.h"

char * mprog_type_to_name ( int type );

#define REDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define OEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define MEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define ARMEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define AEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define TEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define HEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define CEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )

struct olc_help_type
{
    char *command;
    const void *structure;
    char *desc;
};


#define MAX_PROJ_DAM 64
int rounds_per_projectile( int projectile ){

  switch( projectile ){
  case PROJECTILE_ARROW:	return 2;
  case PROJECTILE_BOLT:		return 1;
  case PROJECTILE_STONE:	return 4;
  case PROJECTILE_QUARREL:	return 2;
  case PROJECTILE_DART:		return 6;
  case PROJECTILE_SPEAR:	return 1;
  case PROJECTILE_BLADE:	return 1;
  case PROJECTILE_BULLET:	return 1;
  default:			return 1;
  }
}
bool show_version( CHAR_DATA *ch, char *argument )
{
    send_to_char( VERSION, ch );
    send_to_char( "\n\r", ch );
    send_to_char( AUTHOR, ch );
    send_to_char( "\n\r", ch );
    send_to_char( DATE, ch );
    send_to_char( "\n\r", ch );
    send_to_char( CREDITS, ch );
    send_to_char( "\n\r", ch );
    return FALSE;
}

/* This table contains help commands and a brief description of each. */
const struct olc_help_type help_table[] =
{
    {	"area",		area_flags,	 "Area attributes."		 },
    {	"room",		room_flags,	 "Room attributes."		 },
    {	"room2",	room_flags2,	 "New Room attributes."		 },
    {	"sector",	sector_flags,	 "Sector types, terrain."	 },
    {	"exit",		exit_flags,	 "Exit types."			 },
    {	"type",		type_flags,	 "Types of objects."		 },
    {	"extra",	extra_flags,	 "Object attributes."		 },
    {	"wear",		wear_flags,	 "Where to wear object."	 },
    {	"spec",		spec_table,	 "Available special programs." 	 },
    {	"sex",		sex_flags,	 "Sexes."			 },
    {	"act",		act_flags,	 "Mobile attributes."		 },
    {	"act2",		act2_flags,	 "Mobile attributes."		 },
    {	"affect1",	affect_flags,	 "Mobile affects."		 },
    {	"affect2",	affect2_flags,	 "Mobile affects2."		 },
    {	"wear-loc",	wear_loc_flags,	 "Where mobile wears object."	 },
    {	"spells",	skill_table,	 "Names of current spells." 	 },
    {	"container",	container_flags, "Container status."		 },
    /* ROM specific bits: */
    {	"armor",	ac_type,	 "Ac for different attacks."	 },
    {   "apply",	apply_flags,	 "Apply flags"			 },
    {	"imm",		imm_flags,	 "Mobile immunity."		 },
    {	"res",		res_flags,	 "Mobile resistance."	         },
    {	"vuln",		vuln_flags,	 "Mobile vulnerability."	 },
    {	"off",		off_flags,	 "Mobile offensive behaviour."	 },
    {	"size",		size_flags,	 "Mobile size."			 },
    {   "position",     position_flags,  "Mobile positions."             },
    {   "wclass",       weapon_class,    "Weapon class."                 },
    {   "wtype",        weapon_type2,    "Special weapon type."          },
    {   "ammo",         projectile_type, "Ammo type."			 },
    {   "range_type",   ranged_type,	 "Special ranged flags."	 },
    {   "projtype",     proj_spec_type,  "Special projectile type."      },
    {   "liquid",       liq_table,       "Liquid types."                 },
    {   "apptype",      apply_types,     "Apply types."                  },
    {   "weapon",       attack_table,    "Weapon types."		 },
    {   "race",		race_table,	 "Races"			 },
    {   "trap",		trap_table,	 "Traps"			 },
    {   "trap_flag",	trap_flags,	 "Trap Flags"			 },
    {   "cabal_flag",	cabal_flags,	 "Cabal Flags"			 },
    {   "cabal_flag2",	cabal_flags2,	 "Cabal Flags2"			 },
    {   "cabal_progress_flag",	cabal_progress_flags,	 "Cabal Progress Flags"			 },
    {   "croom_flag",	croom_flags,	 "CRoom Flags"			 },
    {	"army_flags",	army_flags,	 "Army flags"			},
    {	"army_types",	army_types,	 "Army types"			},

    /* PROGS */
    {	"mprog",	mprog_flags,	 "MobProgram flags."		 },
    {	"oprog",	oprog_flags,	 "ObjProgram flags."		 },
    {	"rprog",	rprog_flags,	 "RoomProgram flags."		 },
    {	NULL,		NULL,		 NULL				 }
};


/* Displays settable flags and stats. *
 * Called by show_help(olc_act.c).    */
void show_flag_cmds( CHAR_DATA *ch, const struct flag_type *flag_table )
{
    char buf  [ MSL ];
    char buf1 [ MSL ];
    int  flag;
    int  col;
    buf1[0] = '\0';
    col = 0;
    for (flag = 0; flag_table[flag].name != NULL; flag++)
	if ( flag_table[flag].settable )
	{
	    sprintf( buf, "%-19.18s", flag_table[flag].name );
	    strcat( buf1, buf );
	    if ( ++col % 4 == 0 )
		strcat( buf1, "\n\r" );
	}
    if ( col % 4 != 0 )
	strcat( buf1, "\n\r" );
    send_to_char( buf1, ch );
    return;
}

/* Displays all skill functions.                      *
 * Does remove those immortal commands from the list. *
 * Called by show_help(olc_act.c).                    */
void show_skill_cmds( CHAR_DATA *ch, int tar )
{
    char buf  [ MSL ];
    char buf1 [ MSL*2 ];
    int  sn;
    int  col;
    buf1[0] = '\0';
    col = 0;
    for (sn = 0; sn < MAX_SKILL; sn++)
    {
	if ( !skill_table[sn].name )
	    break;
        if ( !str_cmp( skill_table[sn].name, "reserved" ) || skill_table[sn].spell_fun == spell_null )
	    continue;
	if ( tar == -1 || skill_table[sn].target == tar )
	{
	    sprintf( buf, "%-19.18s", skill_table[sn].name );
	    strcat( buf1, buf );
	    if ( ++col % 4 == 0 )
		strcat( buf1, "\n\r" );
	}
    }
    if ( col % 4 != 0 )
	strcat( buf1, "\n\r" );
    send_to_char( buf1, ch );
    return;
}

/* Displays settable special functions. *
 * Called by show_help(olc_act.c).      */
void show_spec_cmds( CHAR_DATA *ch )
{
    char buf  [ MSL ];
    char buf1 [ MSL ];
    int  spec;
    int  col;
    buf1[0] = '\0';
    col = 0;
    send_to_char( "Preceed special functions with 'spec_'\n\r\n\r", ch );
    for (spec = 0; spec_table[spec].function != NULL; spec++)
    {
	sprintf( buf, "%-19.18s", &spec_table[spec].name[5] );
	strcat( buf1, buf );
	if ( ++col % 4 == 0 )
	    strcat( buf1, "\n\r" );
    }
    if ( col % 4 != 0 )
	strcat( buf1, "\n\r" );
    send_to_char( buf1, ch );
    return;
}

void show_racelist(CHAR_DATA *ch)
{
    int race;
    BUFFER *buffer;
    char buf[MSL];
    buffer = new_buf();
    for ( race = 0; race_table[race].name != NULL; race++)
    {
       if ( (race % 21) == 0 )
           add_buf(buffer,"Name                 \n\r");
       sprintf(buf, "%-20s\n\r", race_table[race].name);
       add_buf(buffer,buf);
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
    return;
}

/* Displays help for many tables used in OLC. *
 * Called by olc interpreters.                */
bool show_help( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    char arg[MIL];
    char spell[MIL];
    int cnt;
    argument = one_argument( argument, arg );
    one_argument( argument, spell );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  ? [command]\n\r\n\r", ch );
	send_to_char( "[command]  [description]\n\r", ch );
	for (cnt = 0; help_table[cnt].command != NULL; cnt++)
	{
            sprintf( buf, "%-10.10s -%s\n\r", capitalize( help_table[cnt].command ), help_table[cnt].desc );
	    send_to_char( buf, ch );
	}
	return FALSE;
    }
    for (cnt = 0; help_table[cnt].command != NULL; cnt++)
        if (  arg[0] == help_table[cnt].command[0] && !str_prefix( arg, help_table[cnt].command ) )
	{
	    if ( help_table[cnt].structure == spec_table )
	    {
		show_spec_cmds( ch );
		return FALSE;
	    }
            else if ( help_table[cnt].structure == liq_table )
            {
                show_liqlist( ch );
                return FALSE;
            }
            else if ( help_table[cnt].structure == attack_table )
            {
                show_damlist( ch );
                return FALSE;
            }
            else if ( help_table[cnt].structure == race_table )
            {
                show_racelist( ch );
                return FALSE;
            }
            else if ( help_table[cnt].structure == skill_table )
	    {
		if ( spell[0] == '\0' )
		{
                    send_to_char( "Syntax:  ? spells [ignore/attack/defend/self/object/all]\n\r", ch );
		    return FALSE;
		}
                if      ( !str_prefix( spell, "all" ) ) show_skill_cmds( ch, -1 );
                else if ( !str_prefix( spell, "ignore" ) ) show_skill_cmds( ch, TAR_IGNORE );
                else if ( !str_prefix( spell, "attack" ) ) show_skill_cmds( ch, TAR_CHAR_OFFENSIVE );
                else if ( !str_prefix( spell, "defend" ) ) show_skill_cmds( ch, TAR_CHAR_DEFENSIVE );
                else if ( !str_prefix( spell, "self" ) )   show_skill_cmds( ch, TAR_CHAR_SELF );
                else if ( !str_prefix( spell, "inventory"))show_skill_cmds( ch, TAR_OBJ_INV );
                else if ( !str_prefix( spell, "object" ) ) show_skill_cmds( ch, TAR_OBJ_ROOM );
                else send_to_char( "Syntax:  ? spell [ignore/attack/defend/self/object/all]\n\r", ch );
		return FALSE;
	    }
	    else
	    {
		show_flag_cmds( ch, help_table[cnt].structure );
		return FALSE;
	    }
	}
    show_help( ch, "" );
    return FALSE;
}

REDIT( redit_rlist )
{
    ROOM_INDEX_DATA	*pRoomIndex;
    AREA_DATA		*pArea;
    char		buf  [ MSL   ];
    BUFFER		*buf1;
    char		arg  [ MIL    ];
    bool found;
    int vnum;
    int  col = 0;
    bool fRprog = FALSE;

    one_argument( argument, arg );
    if (!str_cmp(arg, "prog"))
      fRprog = TRUE;

    pArea = ch->in_room->area;
    buf1=new_buf();
    found   = FALSE;
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
      if ( ( pRoomIndex = get_room_index( vnum ) ) ){
	if (fRprog
	    && (pRoomIndex->rprogs == NULL || pRoomIndex->rprogs->vnum == 0) )
	  continue;
	found = TRUE;
	sprintf( buf, "[%5d] <%d> %-17.16s``", vnum,
		 pRoomIndex->rprogs ? pRoomIndex->rprogs->vnum : 0,
		 capitalize( pRoomIndex->name ) );
	add_buf( buf1, buf );
	if ( ++col % 3 == 0 )
	  add_buf( buf1, "\n\r" );
      }
    if ( !found )
      {
	send_to_char( "Room(s) not found in this area.\n\r", ch);
	return FALSE;
      }
    if ( col % 3 != 0 )
      add_buf( buf1, "\n\r" );
    page_to_char( buf_string(buf1), ch );
    free_buf(buf1);
    return FALSE;
}

REDIT( redit_mlist )
{
    MOB_INDEX_DATA	*pMobIndex;
    AREA_DATA		*pArea;
    char		buf  [ MSL   ];
    BUFFER		*buf1;
    char		arg  [ MIL    ];
    bool fAll, found;
    int vnum;
    int  col = 0;
    bool fProg = FALSE;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  mlist <all/name/prog>\n\r", ch );
	return FALSE;
    }
    buf1=new_buf();
    pArea = ch->in_room->area;
    fAll    = !str_cmp( arg, "all" );
    fProg    = !str_cmp( arg, "prog" );
    found   = FALSE;
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
      if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL ){
	if (fProg ){
	  if (pMobIndex->mprogs == NULL
	      || pMobIndex->mprogs->vnum == 0)
	    continue;
	}
	else  if ( !fAll && !is_name( arg, pMobIndex->player_name ) )
	  continue;

	found = TRUE;
	sprintf( buf, "[%5d] <%d> %-17.16s", pMobIndex->vnum,
		 pMobIndex->mprogs ? pMobIndex->mprogs->vnum : 0,
		 capitalize( pMobIndex->short_descr ) );
	add_buf( buf1, buf );
	if ( ++col % 3 == 0 )
	  add_buf( buf1, "\n\r" );
      }
    if ( !found )
      {
	send_to_char( "Mobile(s) not found in this area.\n\r", ch);
	return FALSE;
      }
    if ( col % 3 != 0 )
      add_buf( buf1, "\n\r" );
    page_to_char( buf_string(buf1), ch );
    free_buf(buf1);
    return FALSE;
}

REDIT( redit_olist )
{
    OBJ_INDEX_DATA	*pObjIndex;
    AREA_DATA		*pArea;
    char		buf  [ MSL   ];
    BUFFER		*buf1;
    char		arg  [ MIL    ];
    bool fAll, fProg = FALSE, found;
    int vnum;
    int  col = 0;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Syntax:  olist <all/name/item_type/prog>\n\r", ch );
	return FALSE;
    }
    pArea = ch->in_room->area;
    buf1=new_buf();
    fAll    = !str_cmp( arg, "all" );
    fProg   = !str_cmp( arg, "prog" );
    found   = FALSE;
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
      if ( ( pObjIndex = get_obj_index( vnum ) ) ){
	if ( fProg ){
	  if (pObjIndex->oprogs == NULL
	      || pObjIndex->oprogs->vnum == 0 )
	    continue;
	}
	else if ( !fAll
		  && !is_name( arg, pObjIndex->name )
		  && flag_value( type_flags, arg ) != pObjIndex->item_type )
	  continue;
	found = TRUE;
	sprintf( buf, "[%5d] <%d> %-17.16s``", pObjIndex->vnum,
		 pObjIndex->oprogs ? pObjIndex->oprogs->vnum : 0,
		 capitalize( pObjIndex->short_descr ) );
	add_buf( buf1, buf );
	if ( ++col % 3 == 0 )
	  add_buf( buf1, "\n\r" );
      }
    if ( !found )
      {
	send_to_char( "Object(s) not found in this area.\n\r", ch);
	    return FALSE;
      }
    if ( col % 3 != 0 )
      add_buf( buf1, "\n\r" );
    page_to_char( buf_string(buf1), ch );
    free_buf(buf1);
    return FALSE;
}

REDIT( redit_mshow )
{
    MOB_INDEX_DATA *pMob;
    int value;
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  mshow <vnum>\n\r", ch );
	return FALSE;
    }
    if ( !is_number( argument ) )
    {
       send_to_char( "REdit: Nonnumeric argument.\n\r", ch);
       return FALSE;
    }
    if ( is_number( argument ) )
    {
	value = atoi( argument );
	if ( !( pMob = get_mob_index( value ) ))
	{
	    send_to_char( "REdit:  That mobile does not exist.\n\r", ch );
	    return FALSE;
	}
	ch->desc->pEdit = (void *)pMob;
    }
    medit_show( ch, argument );
    ch->desc->pEdit = (void *)ch->in_room;
    return FALSE;
}

REDIT( redit_oshow )
{
    OBJ_INDEX_DATA *pObj;
    int value;
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  oshow <vnum>\n\r", ch );
	return FALSE;
    }
    if ( !is_number( argument ) )
    {
        send_to_char( "REdit: Ingresa un numero.\n\r", ch);
        return FALSE;
    }
    if ( is_number( argument ) )
    {
	value = atoi( argument );
	if ( !( pObj = get_obj_index( value ) ))
	{
	    send_to_char( "REdit:  That object does not exist.\n\r", ch );
	    return FALSE;
	}
	ch->desc->pEdit = (void *)pObj;
    }
    oedit_show( ch, argument );
    ch->desc->pEdit = (void *)ch->in_room;
    return FALSE;
}

/* Ensures the range spans only one area. *
 * Called by aedit_vnum(olc_act.c).       */
bool check_range( int lower, int upper )
{
    AREA_DATA *pArea;
    int cnt = 0;
    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
        if ( ( lower <= pArea->min_vnum && pArea->min_vnum <= upper ) || ( lower <= pArea->max_vnum && pArea->max_vnum <= upper ) )
	    ++cnt;
	if ( cnt > 1 )
	    return FALSE;
    }
    return TRUE;
}

AREA_DATA *get_vnum_area( int vnum )
{
    AREA_DATA *pArea;
    for ( pArea = area_first; pArea; pArea = pArea->next )
        if ( vnum >= pArea->min_vnum && vnum <= pArea->max_vnum )
            return pArea;
    return 0;
}

/* Area Editor Functions. */
AEDIT( aedit_show )
{
    AREA_DATA *pArea;
    EXIT_DATA* pe;
    char buf  [MSL];
    int i = 0;


    EDIT_AREA(ch, pArea);
    sprintf( buf, "Name:     [%5d] %s\n\r", pArea->vnum, pArea->name );
    send_to_char( buf, ch );
    sprintf( buf, "File:     %s\n\r", pArea->file_name );
    send_to_char( buf, ch );
    sprintf( buf, "Vnums:    [%d-%d]\n\r", pArea->min_vnum, pArea->max_vnum );
    send_to_char( buf, ch );
    sprintf( buf, "Bastions: [%d/%d]  (current/max_allowed)\n\r", pArea->bastion_current,
	     pArea->bastion_max );
    sendf(ch, "Support: [%d]\n\r", pArea->support);
    send_to_char( buf, ch );
    if (pArea->pCabal){
      sprintf( buf, "Cabal:    [%s]\n\r", pArea->pCabal->who_name);
      send_to_char( buf, ch );
    }
    sprintf( buf, "Startroom:[%d]\n\r",	pArea->startroom );
    send_to_char( buf, ch );
    sprintf( buf, "Age:      [%d]\n\r",	pArea->age );
    send_to_char( buf, ch );
    if (pArea->raid){
      sprintf( buf, "Raid:     %d/%dh %d/%d mobs (vnum %d)\n\r",
	       pArea->raid->duration,
	       pArea->raid->lifetime,
	       pArea->raid->mob_spawned,
	       pArea->raid->mob_total,
	       pArea->raid->mob_vnum);
      send_to_char( buf, ch );
    }
    sprintf( buf, "Players:  [%d]\n\r", pArea->nplayer );
    send_to_char( buf, ch );
    sprintf( buf, "Security: [%d]\n\r", pArea->security );
    send_to_char( buf, ch );
    sprintf( buf, "Builders: [%s]\n\r", pArea->builders );
    send_to_char( buf, ch );
    sprintf( buf, "Credits: [%s]\n\r", pArea->credits );
    send_to_char( buf, ch );
    sprintf( buf, "Flags:    [%s]\n\r", flag_string( area_flags, pArea->area_flags ) );
    send_to_char( buf, ch );
    send_to_char("Crimes:", ch );
    for (i = 0; i < MAX_CRIME; i++){
      sendf( ch, " %s(%d)", crime_table[i].name, pArea->crimes[i] );
    }
    send_to_char("\n\r", ch);
    sprintf(buf, "Prefix: %s\n\r", pArea->prefix);
    send_to_char( buf, ch );
    send_to_char("Area exits:\n\r", ch );
    for (pe = pArea->exits; pe; pe = pe->next_in_area ){
      if (pe->to_room  == NULL)
	continue;
      else
	sendf(ch, "%-20s [%d]\n\r", pe->to_room->area->name, pe->to_room->vnum);
    }
    return FALSE;
}

AEDIT( aedit_reset )
{
    AREA_DATA *pArea;
    EDIT_AREA(ch, pArea);
    reset_area( pArea );
    send_to_char( "Area reset.\n\r", ch );
    return FALSE;
}

AEDIT( aedit_create )
{
    AREA_DATA *pArea;

    if (get_trust(ch) < IMPLEMENTOR){
      sendf(ch, "Requires level %d trust.\n\r", IMPLEMENTOR);
      return FALSE;
    }
    pArea               =   new_area();
    area_last->next     =   pArea;
    area_last           =   pArea;
    ch->desc->pEdit     =   (void *)pArea;
    SET_BIT( pArea->area_flags, AREA_ADDED );
    send_to_char( "Area Created.\n\r", ch );
    return FALSE;
}

AEDIT( aedit_name )
{
    AREA_DATA *pArea;
    EDIT_AREA(ch, pArea);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:   name [$name]\n\r", ch );
	return FALSE;
    }
    free_string( pArea->name );
    pArea->name = str_dup( argument );
    send_to_char( "Name set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_prefix )
{
    AREA_DATA *pArea;
    EDIT_AREA(ch, pArea);
    if (IS_NULLSTR(argument)){
      send_to_char("Syntax: prefix <string> or none to clear.\n\r", ch);
      return FALSE;
    }
    free_string( pArea->prefix );
    if (str_cmp("none", argument))
      pArea->prefix = str_dup( argument );
    else
      pArea->prefix = str_dup( "" );
    send_to_char( "Prefix set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_credits )
{
    AREA_DATA *pArea;
    EDIT_AREA(ch, pArea);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:   credits [$credits]\n\r", ch );
	return FALSE;
    }
    free_string( pArea->credits );
    pArea->credits = str_dup( argument );
    send_to_char( "Credits set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_file )
{
    AREA_DATA *pArea;
    char file[MSL];
    int i, length;
    EDIT_AREA(ch, pArea);
    one_argument( argument, file );
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  filename [$file]\n\r", ch );
	return FALSE;
    }
    length = strlen( argument );
    if ( length > 8 )
    {
	send_to_char( "No more than eight characters allowed.\n\r", ch );
	return FALSE;
    }
    for ( i = 0; i < length; i++ )
	if ( !isalnum( file[i] ) )
	{
	    send_to_char( "Only letters and numbers are valid.\n\r", ch );
	    return FALSE;
	}
    free_string( pArea->file_name );
    strcat( file, ".are" );
    pArea->file_name = str_dup( file );
    send_to_char( "Filename set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_age )
{
    AREA_DATA *pArea;
    char age[MSL];
    EDIT_AREA(ch, pArea);
    one_argument( argument, age );
    if ( !is_number( age ) || age[0] == '\0' )
    {
	send_to_char( "Syntax:  age [#xage]\n\r", ch );
	return FALSE;
    }
    pArea->age = atoi( age );
    send_to_char( "Age set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_startroom )
{
    AREA_DATA *pArea;
    int vnum;

    EDIT_AREA(ch, pArea);
    if ( (vnum = atoi( argument )) < 1){
      send_to_char( "Syntax:  startroom <vnum>\n\r", ch );
      return FALSE;
    }
    else if (vnum < pArea->min_vnum || vnum > pArea->max_vnum){
      send_to_char("Vnum must be within the area range.\n\r",ch);
      return FALSE;
    }
    else if (get_room_index(vnum) == NULL){
      send_to_char("Room not found.\n\r", ch);
      return FALSE;
    }
    pArea->startroom = vnum;
    send_to_char( "Startroom set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_bastion )
{
    AREA_DATA *pArea;
    char bastion[MSL];
    EDIT_AREA(ch, pArea);
    one_argument( argument, bastion );
    if ( !is_number( bastion ) || bastion[0] == '\0' )
    {
	send_to_char( "Syntax:  bastion [#max_bastions]\n\r", ch );
	return FALSE;
    }
    pArea->bastion_max = atoi( bastion );
    send_to_char( "Bastion max set.\n\r", ch );
    send_to_char("Refreshing support values across areas...\n\r", ch );
    refresh_area_support();
    send_to_char("Done.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_security )
{
    AREA_DATA *pArea;
    char sec[MSL];
    char buf[MSL];
    int  value;
    EDIT_AREA(ch, pArea);
    one_argument( argument, sec );
    if ( !is_number( sec ) || sec[0] == '\0' )
    {
	send_to_char( "Syntax:  security [#xlevel]\n\r", ch );
	return FALSE;
    }
    value = atoi( sec );
    if ( value > ch->pcdata->security || value < 0 )
    {
	if ( ch->pcdata->security != 0 )
	{
	    sprintf( buf, "Security is 0-%d.\n\r", ch->pcdata->security );
	    send_to_char( buf, ch );
	}
	else
	    send_to_char( "Security is 0 only.\n\r", ch );
	return FALSE;
    }
    pArea->security = value;
    send_to_char( "Security set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_builder )
{
    AREA_DATA *pArea;
    char name[MSL];
    char buf[MSL];
    EDIT_AREA(ch, pArea);
    one_argument( argument, name );
    if ( name[0] == '\0' )
    {
	send_to_char( "Syntax:  builder [$name]  -toggles builder\n\r", ch );
	send_to_char( "Syntax:  builder All      -allows everyone\n\r", ch );
	return FALSE;
    }
    name[0] = UPPER( name[0] );
    if ( strstr( pArea->builders, name ) != '\0' )
    {
	pArea->builders = string_replace( pArea->builders, name, "\0" );
	pArea->builders = string_unpad( pArea->builders );
	if ( pArea->builders[0] == '\0' )
	{
	    free_string( pArea->builders );
	    pArea->builders = str_dup( "None" );
	}
	send_to_char( "Builder removed.\n\r", ch );
	return TRUE;
    }
    else
    {
	buf[0] = '\0';
	if ( strstr( pArea->builders, "None" ) != '\0' )
	{
	    pArea->builders = string_replace( pArea->builders, "None", "\0" );
	    pArea->builders = string_unpad( pArea->builders );
	}
	if (pArea->builders[0] != '\0' )
	{
	    strcat( buf, pArea->builders );
	    strcat( buf, " " );
	}
	strcat( buf, name );
	free_string( pArea->builders );
	pArea->builders = string_proper( str_dup( buf ) );
	send_to_char( "Builder added.\n\r", ch );
	send_to_char( pArea->builders,ch);
	return TRUE;
    }
    return FALSE;
}

AEDIT( aedit_vnum )
{
    AREA_DATA *pArea;
    char lower[MSL];
    char upper[MSL];
    int  ilower;
    int  iupper;
    EDIT_AREA(ch, pArea);
    argument = one_argument( argument, lower );
    one_argument( argument, upper );
    if ( !is_number( lower ) || lower[0] == '\0' || !is_number( upper ) || upper[0] == '\0' )
    {
	send_to_char( "Syntax:  vnum [#xlower] [#xupper]\n\r", ch );
	return FALSE;
    }
    if ( ( ilower = atoi( lower ) ) > ( iupper = atoi( upper ) ) )
    {
	send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
	return FALSE;
    }
    if ( !check_range( atoi( lower ), atoi( upper ) ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }
    if ( get_vnum_area( ilower ) && get_vnum_area( ilower ) != pArea )
    {
	send_to_char( "AEdit:  Lower vnum already assigned.\n\r", ch );
	return FALSE;
    }
    pArea->min_vnum = ilower;
    send_to_char( "Lower vnum set.\n\r", ch );
    if ( get_vnum_area( iupper ) && get_vnum_area( iupper ) != pArea )
    {
	send_to_char( "AEdit:  Upper vnum already assigned.\n\r", ch );
        return TRUE;
    }
    pArea->max_vnum = iupper;
    send_to_char( "Upper vnum set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_lvnum )
{
    AREA_DATA *pArea;
    char lower[MSL];
    int  ilower;
    int  iupper;
    EDIT_AREA(ch, pArea);
    one_argument( argument, lower );
    if ( !is_number( lower ) || lower[0] == '\0' )
    {
	send_to_char( "Syntax:  min_vnum [#xlower]\n\r", ch );
	return FALSE;
    }
    if ( ( ilower = atoi( lower ) ) > ( iupper = pArea->max_vnum ) )
    {
	send_to_char( "AEdit:  Value must be less than the max_vnum.\n\r", ch );
	return FALSE;
    }
    if ( !check_range( ilower, iupper ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }
    if ( get_vnum_area( ilower ) && get_vnum_area( ilower ) != pArea )
    {
	send_to_char( "AEdit:  Lower vnum already assigned.\n\r", ch );
	return FALSE;
    }
    pArea->min_vnum = ilower;
    send_to_char( "Lower vnum set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_uvnum )
{
    AREA_DATA *pArea;
    char upper[MSL];
    int  ilower;
    int  iupper;
    EDIT_AREA(ch, pArea);
    one_argument( argument, upper );
    if ( !is_number( upper ) || upper[0] == '\0' )
    {
	send_to_char( "Syntax:  max_vnum [#xupper]\n\r", ch );
	return FALSE;
    }
    if ( ( ilower = pArea->min_vnum ) > ( iupper = atoi( upper ) ) )
    {
	send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
	return FALSE;
    }
    if ( !check_range( ilower, iupper ) )
    {
	send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
	return FALSE;
    }
    if ( get_vnum_area( iupper ) && get_vnum_area( iupper ) != pArea )
    {
	send_to_char( "AEdit:  Upper vnum already assigned.\n\r", ch );
	return FALSE;
    }
    pArea->max_vnum = iupper;
    send_to_char( "Upper vnum set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_flags )
{
    AREA_DATA *pArea;
    int  value;
    EDIT_AREA(ch, pArea);
    if ( argument[0] != '\0' )
    {
	if ( ( value = flag_value( area_flags, argument ) ) != NO_FLAG )
	{
	    pArea->area_flags ^= value;
	    send_to_char( "Area flag toggled.\n\r", ch);
	    return TRUE;
	}
    }
    send_to_char( "Syntax: aflag [flag]\n\rType '? area' for a list of flags.\n\r", ch );
    return TRUE;
}


AEDIT( aedit_crime )
{
    AREA_DATA *pArea;
    int  value, crime;
    char arg[MIL];
    EDIT_AREA(ch, pArea);

    argument = one_argument( argument, arg );

    if (IS_NULLSTR(arg)){
      send_to_char("crime <crime> <value>\n\r", ch);
      return FALSE;
    }
    else if ( (crime = crime_lookup( arg )) < 0){
      send_to_char("No such crime category.\n\r", ch);
      return FALSE;
    }
    else if ( (value = atoi( argument)) < 0 || value > 4){
      send_to_char("Penalty level must be between 0 and 4.\n\r", ch);
      return FALSE;
    }
    else
      pArea->crimes[crime] = value;

    sendf( ch, "%s now has max. penalty of %s.", crime_table[crime].name, punish_table[value].name);
    return TRUE;
}

/* Room Editor Functions. */
REDIT( redit_show )
{
    ROOM_INDEX_DATA	*pRoom;
    char		buf  [MSL];
    char		buf1 [2*MSL];
    OBJ_DATA		*obj;
    CHAR_DATA		*rch;
    PROG_LIST		*list;
    int			door;
    bool		fcnt;
    Double_List *tmp_list;
    EDIT_ROOM(ch, pRoom);
    buf1[0] = '\0';
    sprintf( buf, "Day Description:\n\r%s", pRoom->description );
    strcat( buf1, buf );
    if (pRoom->description2 != NULL )
    {
        sprintf( buf, "Night Description:\n\r%s", pRoom->description2 );
        strcat( buf1, buf );
    }
    sprintf( buf, "Name:       [%s]\n\rArea:       [%5d] %s\n\r",
      pRoom->name, pRoom->area->vnum, pRoom->area->name );
    strcat( buf1, buf );
    sprintf( buf, "Vnum:       [%5d]\n\rSector:     [%s]\n\r",
      pRoom->vnum, flag_string( sector_flags, pRoom->sector_type ) );
    strcat( buf1, buf );
    sprintf( buf, "Watched By: [");
    strcat( buf1, buf );
    tmp_list = pRoom->watch_vnums;
    while (tmp_list != NULL) {
      sprintf( buf, " %lld", (long long) tmp_list->cur_entry);
      strcat( buf1, buf );
      tmp_list = tmp_list->next_node;
    }
    sprintf (buf, " ]\n\r");
    strcat (buf1, buf);
    sprintf( buf, "Room flags: [%s]\n\r", flag_string( room_flags, pRoom->room_flags ) );
    strcat( buf1, buf );
    sprintf( buf, "Room2 flags: [%s]\n\r", flag_string( room_flags2, pRoom->room_flags2 ) );
    strcat( buf1, buf );
    sprintf( buf, "Health recovery:[%d]\n\rMana recovery  :[%d]\n\r",
      pRoom->heal_rate , pRoom->mana_rate );
    strcat( buf1, buf );
    sprintf( buf, "Temperature:[%d]\n\r", pRoom->temp );
    strcat( buf1, buf );
    if (pRoom->pCabal){
      sprintf( buf, "Cabal:     [%d] %s\n\r" , pRoom->pCabal->vnum, pRoom->pCabal->name);
      strcat( buf1, buf );
    }
    if ( pRoom->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;
	strcat( buf1, "Desc Kwds:  [" );
	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    strcat( buf1, ed->keyword );
	    if ( ed->next )
		strcat( buf1, " " );
	}
	strcat( buf1, "]\n\r" );
    }
    strcat( buf1, "Characters: [" );
    fcnt = FALSE;
    for ( rch = pRoom->people; rch; rch = rch->next_in_room )
    {
	one_argument( rch->name, buf );
	strcat( buf1, buf );
	strcat( buf1, " " );
	fcnt = TRUE;
    }
    if ( fcnt )
    {
	int end;
	end = strlen(buf1) - 1;
	buf1[end] = ']';
	strcat( buf1, "\n\r" );
    }
    else
	strcat( buf1, "none]\n\r" );
    strcat( buf1, "Objects:    [" );
    fcnt = FALSE;
    for ( obj = pRoom->contents; obj; obj = obj->next_content )
    {
	one_argument( obj->name, buf );
	strcat( buf1, buf );
	strcat( buf1, " " );
	fcnt = TRUE;
    }
    if ( fcnt )
    {
	int end;
	end = strlen(buf1) - 1;
	buf1[end] = ']';
	strcat( buf1, "\n\r" );
    }
    else
	strcat( buf1, "none]\n\r" );
    for ( door = 0; door < MAX_DIR; door++ )
    {
	EXIT_DATA *pexit;
	if ( ( pexit = pRoom->exit[door] ) )
	{
	    char word[MIL];
	    char reset_state[MSL];
	    char *state;
	    int i, length;
	    sprintf( buf, "-%-5s to [%5d] Key: [%5d] ",
              capitalize(dir_name[door]),
              pexit->to_room ? pexit->to_room->vnum : 0,
              pexit->key );
	    strcat( buf1, buf );
	    strcpy( reset_state, flag_string( exit_flags, pexit->rs_flags ) );
	    state = flag_string( exit_flags, pexit->exit_info );
	    strcat( buf1, " Exit flags: [" );
	    for (; ;)
	    {
		state = one_argument( state, word );
		if ( word[0] == '\0' )
		{
		    int end;
                    end = strlen(buf1) - 1;
		    buf1[end] = ']';
		    strcat( buf1, "\n\r" );
		    break;
		}
		if ( str_infix( word, reset_state ) )
		{
		    length = strlen(word);
		    for (i = 0; i < length; i++)
			word[i] = UPPER(word[i]);
		}
		strcat( buf1, word );
		strcat( buf1, " " );
	    }
	    if ( pexit->keyword && pexit->keyword[0] != '\0' )
	    {
		sprintf( buf, "Kwds: [%s]\n\r", pexit->keyword );
		strcat( buf1, buf );
	    }
	    if ( pexit->description && pexit->description[0] != '\0' )
	    {
		sprintf( buf, "%s", pexit->description );
		strcat( buf1, buf );
	    }
	    /* TRAP info */
	    if (pexit->traps){
	      TRAP_DATA* pTrap = pexit->traps;
	      while(pTrap){
		sprintf(buf, "Trap [%d]: %s%s%s %s, type: %s, lvl: %d, dur: %d, %s\n\r",
			pTrap->vnum,
			pTrap->owner ? "(" : "",
			pTrap->owner ? pTrap->owner->name : "",
			pTrap->owner ? ")" : "",
			pTrap->name, trap_table[pTrap->type].name,
			pTrap->level,
			pTrap->duration, pTrap->armed ? "ARMED" :  "disarmed");
		strcat(buf1, buf);
		pTrap = pTrap->next_trap;
	      }
	    }
	}
    }
    send_to_char( buf1, ch );
    if ( pRoom->rprogs )
    {
	int cnt;
	sprintf(buf, "\n\rROOMPrograms for [%5d]:\n\r", pRoom->vnum);
	send_to_char( buf, ch );

	for (cnt=0, list=pRoom->rprogs; list; list=list->next)
	{
		if (cnt ==0)
		{
			send_to_char ( " Number Vnum Trigger Phrase\n\r", ch );
			send_to_char ( " ------ ---- ------- ------\n\r", ch );
		}
		sprintf(buf, "[%5d] %4d %7s %s\n\r", cnt,
			list->vnum,prog_type_to_name(list->trig_type),
			list->trig_phrase);
		send_to_char( buf, ch );
		cnt++;
	}
    }
    return FALSE;
}

/* Local function. */
bool change_exit( CHAR_DATA *ch, char *argument, int door )
{
    ROOM_INDEX_DATA *pRoom;
    char command[MIL];
    char arg[MIL];
    int  value;
    EDIT_ROOM(ch, pRoom);
    argument = one_argument( argument, command );
    one_argument( argument, arg );
    if ( command[0] == '\0' && argument[0] == '\0' )
    {
        move_char( ch, door, TRUE );
	return FALSE;
    }
    if ( command[0] == '?' )
    {
	do_help( ch, "EXIT" );
	return FALSE;
    }
    if ( !str_cmp( command, "trap") ){
      TRAP_DATA* pTrap;
      if (pRoom->exit[door] == NULL
	  || (pTrap = pRoom->exit[door]->traps) == NULL){
	send_to_char("No trap.\n\r", ch);
	return FALSE;
      }
      if (IS_NULLSTR(argument)){
	send_to_char("trap <delete/disarm/arm>\n\r", ch);
	return FALSE;
      }
      if ( !str_cmp( argument, "delete") ){
	extract_trap( pTrap );
	send_to_char("Trap removed.\n\r", ch);
	return TRUE;
      }
      if ( !str_cmp( argument, "arm") ){
	pTrap->armed = TRUE;
	send_to_char("Trap armed.\n\r", ch);
	return TRUE;
      }
      if ( !str_cmp( argument, "disarm") ){
	pTrap->armed = FALSE;
	send_to_char("Trap disarmed.\n\r", ch);
	return TRUE;
      }
      else{
	send_to_char("trap <delete/disarm/arm>\n\r", ch);
	return FALSE;
      }
    }
    else if ( !str_cmp( command, "delete" ) )
    {
	ROOM_INDEX_DATA *pToRoom;
        sh_int rev;
	if ( !pRoom->exit[door] )
	{
	    send_to_char( "REdit:  Cannot delete a null exit.\n\r", ch );
	    return FALSE;
	}
	rev = rev_dir[door];
        pToRoom = pRoom->exit[door]->to_room;
	SET_BIT( pToRoom->area->area_flags, AREA_CHANGED );
	if ( pToRoom->exit[rev] )
	{
	  pToRoom->exit[rev]->rs_flags = 0;
	  pToRoom->exit[rev]->exit_info = 0;
	  free_exit( pToRoom->exit[rev] );
	  pToRoom->exit[rev] = NULL;
	}
	pRoom->exit[door]->rs_flags = 0;
	pRoom->exit[door]->exit_info = 0;
	free_exit( pRoom->exit[door] );
	pRoom->exit[door] = NULL;
	send_to_char( "Exit unlinked.\n\r", ch );
	return TRUE;
    }
    else if ( !str_cmp( command, "link" ) )
    {
	EXIT_DATA *pExit;
	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax:  [direction] link [vnum]\n\r", ch );
	    return FALSE;
	}
	value = atoi( arg );
	if ( !get_room_index( value ) )
	{
	    send_to_char( "REdit:  Cannot link to non-existant room.\n\r", ch );
	    return FALSE;
	}
	if ( !IS_BUILDER( ch, get_room_index( value )->area ) )
	{
	    send_to_char( "REdit:  Cannot link to that area.\n\r", ch );
	    return FALSE;
	}
	if ( get_room_index( value )->exit[rev_dir[door]] )
	{
	    send_to_char( "REdit:  Remote side's exit already exists.\n\r", ch );
	    return FALSE;
	}
	else if (IS_VIRVNUM( value )){
	  send_to_char("Cannot link to virtual rooms.\n\r", ch);
	  return FALSE;
	}
	if ( !pRoom->exit[door] )
	    pRoom->exit[door] = new_exit();
        pRoom->exit[door]->to_room = get_room_index( value );
	pRoom->exit[door]->orig_door = door;
	pRoom                   = get_room_index( value );
	door                    = rev_dir[door];
	pExit                   = new_exit();
	pExit->to_room       = ch->in_room;
	pExit->orig_door	= door;
	pRoom->exit[door]       = pExit;
	send_to_char( "Two-way link established.\n\r", ch );
	SET_BIT( pRoom->exit[door]->to_room->area->area_flags, AREA_CHANGED );
	return TRUE;
    }
    else if ( !str_cmp( command, "dig" ) )
    {
	char buf[MSL];
	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax: [direction] dig <vnum>\n\r", ch );
	    return FALSE;
	}
	redit_create( ch, arg );
	sprintf( buf, "link %s", arg );
	change_exit( ch, buf, door);
	return TRUE;
    }
    else if ( !str_cmp( command, "room" ) )
    {
	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax:  [direction] room [vnum]\n\r", ch );
	    return FALSE;
	}
	if ( !pRoom->exit[door] )
	    pRoom->exit[door] = new_exit();
	value = atoi( arg );
	if ( !get_room_index( value ) )
	{
	    send_to_char( "REdit:  Cannot link to non-existant room.\n\r", ch );
	    return FALSE;
	}
	else if (IS_VIRVNUM( value )){
	  send_to_char("Cannot link to virtual rooms.\n\r", ch);
	  return FALSE;
	}
        pRoom->exit[door]->to_room = get_room_index( value );
	pRoom->exit[door]->orig_door = door;
	send_to_char( "One-way link established.\n\r", ch );
	return TRUE;
    }
    else if ( !str_cmp( command, "key" ) )
    {
	if ( arg[0] == '\0' || !is_number( arg ) )
	{
	    send_to_char( "Syntax:  [direction] key [vnum]\n\r", ch );
	    return FALSE;
	}
        if ( !pRoom->exit[door] )
        {
            send_to_char("Door does not exist.\n\r",ch);
            return FALSE;
        }
        value = atoi( arg );
	if (value < 1){
	  pRoom->exit[door]->key = 0;
	  return TRUE;
	}
	if ( !get_obj_index( value ) )
	{
	    send_to_char( "REdit:  Item doesn't exist.\n\r", ch );
	    return FALSE;
	}
	if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY )
	{
	    send_to_char( "REdit:  Key doesn't exist.\n\r", ch );
	    return FALSE;
	}
	pRoom->exit[door]->key = value;
	send_to_char( "Exit key set.\n\r", ch );
	return TRUE;
    }
    else if ( !str_cmp( command, "name" ) )
    {
      if ( IS_NULLSTR( argument ))
	{
	  send_to_char( "Syntax:  [direction] name [string]\n\r", ch );
	  return FALSE;
	}
      if ( !pRoom->exit[door] )
        {
	  send_to_char("Door does not exist.\n\r",ch);
	  return FALSE;
        }
      free_string( pRoom->exit[door]->keyword );
      pRoom->exit[door]->keyword = str_dup( argument );
      send_to_char( "Exit name set.\n\r", ch );
      return TRUE;
    }
    else if ( !str_cmp( command, "desc" ) )
    {
	if ( arg[0] == '\0' )
	{
	    if ( !pRoom->exit[door] )
            {
                send_to_char("Door does not exist.\n\r",ch);
                return FALSE;
            }
	    string_append( ch, &pRoom->exit[door]->description );
	    return TRUE;
	}
	send_to_char( "Syntax:  [direction] desc\n\r", ch );
	return FALSE;
    }
    else if ( ( value = flag_value( exit_flags, command ) ) != NO_FLAG )
      {
	ROOM_INDEX_DATA *pToRoom;
        sh_int rev;
	if ( !pRoom->exit[door] )
	  {
            send_to_char("Door does not exist.\n\r",ch);
            return FALSE;
	  }
	TOGGLE_BIT(pRoom->exit[door]->rs_flags,  value);
	pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;
        pToRoom = pRoom->exit[door]->to_room;
	rev = rev_dir[door];
	if (pToRoom->exit[rev] != NULL && pToRoom->exit[rev]->to_room == pRoom){
	  if (IS_SET(pRoom->exit[door]->rs_flags, value)){
            SET_BIT(pToRoom->exit[rev]->rs_flags,  value);
	    pToRoom->exit[rev]->exit_info = pToRoom->exit[rev]->rs_flags;
	  }
	  else{
            REMOVE_BIT(pToRoom->exit[rev]->rs_flags,  value);
	    pToRoom->exit[rev]->exit_info = pToRoom->exit[rev]->rs_flags;
	  }
	}
	send_to_char( "Exit flag toggled.\n\r", ch );
	return TRUE;
      }
    return FALSE;
}

REDIT( redit_north )
{
    if ( change_exit( ch, argument, DIR_NORTH ) )
	return TRUE;
    return FALSE;
}

REDIT( redit_south )
{
    if ( change_exit( ch, argument, DIR_SOUTH ) )
	return TRUE;
    return FALSE;
}

REDIT( redit_east )
{
    if ( change_exit( ch, argument, DIR_EAST ) )
	return TRUE;
    return FALSE;
}

REDIT( redit_west )
{
    if ( change_exit( ch, argument, DIR_WEST ) )
	return TRUE;
    return FALSE;
}

REDIT( redit_up )
{
    if ( change_exit( ch, argument, DIR_UP ) )
	return TRUE;
    return FALSE;
}

REDIT( redit_down )
{
    if ( change_exit( ch, argument, DIR_DOWN ) )
	return TRUE;
    return FALSE;
}

REDIT( redit_ed )
{
    ROOM_INDEX_DATA *pRoom;
    EXTRA_DESCR_DATA *ed;
    char command[MIL];
    char keyword[MIL];
    EDIT_ROOM(ch, pRoom);
    argument = one_argument( argument, command );
    strcpy( keyword, argument );
    if ( command[0] == '\0' || keyword[0] == '\0' )
    {
	send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	send_to_char( "         ed edit [keyword]\n\r", ch );
	send_to_char( "         ed delete [keyword]\n\r", ch );
	send_to_char( "         ed format [keyword]\n\r", ch );
	return FALSE;
    }
    if ( !str_cmp( command, "add" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	    return FALSE;
	}
	ed			=   new_extra_descr();
	ed->keyword		=   str_dup( keyword );
	ed->description		=   str_dup( "" );
	ed->next		=   pRoom->extra_descr;
	pRoom->extra_descr	=   ed;
	string_append( ch, &ed->description );
	return TRUE;
    }
    if ( !str_cmp( command, "edit" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
	    return FALSE;
	}
	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	    if ( is_name( keyword, ed->keyword ) )
		break;
	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}
	string_append( ch, &ed->description );
	return TRUE;
    }
    if ( !str_cmp( command, "delete" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed delete [keyword]\n\r", ch );
	    return FALSE;
	}
	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	    ped = ed;
	}
	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}
	if ( !ped )
	    pRoom->extra_descr = ed->next;
	else
	    ped->next = ed->next;
	free_extra_descr( ed );
	send_to_char( "Extra description deleted.\n\r", ch );
	return TRUE;
    }
    if ( !str_cmp( command, "format" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed format [keyword]\n\r", ch );
	    return FALSE;
	}
	for ( ed = pRoom->extra_descr; ed; ed = ed->next )
	    if ( is_name( keyword, ed->keyword ) )
		break;
	if ( !ed )
	{
	    send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}
	ed->description = format_string( ed->description );
        send_to_char( "Extra description formatted.\n\r", ch );
	return TRUE;
    }
    redit_ed( ch, "" );
    return FALSE;
}

REDIT( redit_create )
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    int value;
    int iHash;
    EDIT_ROOM(ch, pRoom);
    value = atoi( argument );
    if ( argument[0] == '\0' || value <= 0 )
    {
	send_to_char( "Syntax:  create [vnum > 0]\n\r", ch );
	return FALSE;
    }
    pArea = get_vnum_area( value );
    if ( !pArea )
    {
	send_to_char( "REdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }
    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "REdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }
    if ( get_room_index( value ) )
    {
	send_to_char( "REdit:  Room vnum already exists.\n\r", ch );
	return FALSE;
    }
    if ( value >= MAX_ROOM_VNUM ){
      send_to_char("You cannot create virtual rooms.\n\r", ch );
      return FALSE;
    }
    pRoom			= new_room_index();
    pRoom->area			= pArea;
    pRoom->vnum			= value;
    pRoom->watch_vnums		= NULL;
    if ( value > top_vnum_room )
        top_vnum_room = value;
    iHash			= value % MAX_KEY_HASH;
    pRoom->next			= room_index_hash[iHash];
    room_index_hash[iHash]	= pRoom;
    ch->desc->pEdit		= (void *)pRoom;
    send_to_char( "Room created.\n\r", ch );
    return TRUE;
}

REDIT( redit_name )
{
    ROOM_INDEX_DATA *pRoom;
    EDIT_ROOM(ch, pRoom);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [name]\n\r", ch );
	return FALSE;
    }
    format_room_name(pRoom, argument);
    send_to_char( "Name set.\n\r", ch );
    return TRUE;
}

REDIT( redit_desc )
{
    ROOM_INDEX_DATA *pRoom;
    EDIT_ROOM(ch, pRoom);
    if ( argument[0] == '\0' )
    {
	string_append( ch, &pRoom->description );
	return TRUE;
    }
    send_to_char( "Syntax:  desc\n\r", ch );
    return FALSE;
}

REDIT( redit_ndesc )
{
    ROOM_INDEX_DATA *pRoom;
    EDIT_ROOM(ch, pRoom);
    if ( argument[0] == '\0' )
    {
	string_append( ch, &pRoom->description2 );
	return TRUE;
    }
    send_to_char( "Syntax:  ndesc\n\r", ch );
    return FALSE;
}

REDIT( redit_heal )
{
    ROOM_INDEX_DATA *pRoom;
    EDIT_ROOM(ch, pRoom);
    if (is_number(argument))
    {
        pRoom->heal_rate = atoi ( argument );
        send_to_char ( "Heal rate set.\n\r", ch);
        return TRUE;
    }
    send_to_char ( "Syntax : heal <#xnumber>\n\r", ch);
    return FALSE;
}

REDIT( redit_mana )
{
    ROOM_INDEX_DATA *pRoom;
    EDIT_ROOM(ch, pRoom);
    if (is_number(argument))
    {
        pRoom->mana_rate = atoi ( argument );
        send_to_char ( "Mana rate set.\n\r", ch);
        return TRUE;
    }
    send_to_char ( "Syntax : mana <#xnumber>\n\r", ch);
    return FALSE;
}

REDIT( redit_watch )
{
    ROOM_INDEX_DATA *pRoom;
    Double_List * tmp_list;
    Double_List * tmp_list2;
    int new_vnum=0;

    EDIT_ROOM(ch, pRoom);
    if (is_number(argument))
    {
      new_vnum = atoi (argument);
      if ( !get_room_index( new_vnum)) {
	send_to_char( "REdit:  Cannot be watched by non-existant room.\n\r", ch );
	return FALSE;
      }
/* testing if we can make mirror rooms
      if ( new_vnum == ch->in_room->vnum) {
	send_to_char( "REdit:  Cannot watch the current room (loops).\n\r", ch );
	return FALSE;
      }
*/
      if (pRoom->watch_vnums == NULL) {
	pRoom->watch_vnums = (Double_List *) malloc (sizeof (Double_List));
	pRoom->watch_vnums->cur_entry = (void *) (long) new_vnum;
	pRoom->watch_vnums->prev_node = NULL;
	pRoom->watch_vnums->next_node = NULL;
	sendf (ch, "Now watched by room %d.\n\r", new_vnum);
	return TRUE;
      }

      tmp_list = pRoom->watch_vnums;
      while (tmp_list != NULL) {
	if ( new_vnum == (long long) tmp_list->cur_entry) {
	  if (tmp_list->prev_node == NULL) {
	    tmp_list2 = pRoom->watch_vnums;
	    pRoom->watch_vnums = tmp_list->next_node;
	    if (pRoom->watch_vnums != NULL) {
	      pRoom->watch_vnums->prev_node = NULL;
	    }
	    free (tmp_list2);
	  }
	  else {
	    tmp_list2 = tmp_list;
	    tmp_list->prev_node->next_node = tmp_list->next_node;
	    if (tmp_list->next_node != NULL) {
	      tmp_list->next_node->prev_node = tmp_list->prev_node;
	    }
	    free (tmp_list2);
	  }
	  sendf (ch, "No Longer watched by room %d.\n\r", new_vnum);
	  return TRUE;
	}
	if (tmp_list->next_node == NULL) {
	  tmp_list->next_node = (Double_List *) malloc (sizeof (Double_List));
	  tmp_list->next_node->cur_entry = (void *) (long) new_vnum;
	  tmp_list->next_node->prev_node = tmp_list;
	  tmp_list->next_node->next_node = NULL;
	  tmp_list = tmp_list->next_node;
	  sendf (ch, "Now watched by room %d.\n\r", new_vnum);
	  return TRUE;
	}
	tmp_list = tmp_list->next_node;
      }
    }
    send_to_char ( "Toggles watched by room # on/off\n\rSyntax : watch <#xnumber>\n\r", ch);
    return FALSE;
}

REDIT( redit_temp )
{
    ROOM_INDEX_DATA *pRoom;
    EDIT_ROOM(ch, pRoom);
    if (is_number(argument))
    {
        pRoom->temp = atoi ( argument );
        send_to_char ( "Temperature set.\n\r", ch);
        return TRUE;
    }
    send_to_char ( "Syntax : temp <#xnumber>\n\r", ch);
    return FALSE;
}

REDIT( redit_cabal )
{
    ROOM_INDEX_DATA *pRoom;
    CABAL_DATA* pc;
    EDIT_ROOM(ch, pRoom);

    if ( (pc = get_cabal(argument)) == NULL){
      pRoom->pCabal = pc;
      send_to_char("Cabal cleared.\n\r", ch);
      return TRUE;
    }
    else{
      pRoom->pCabal = get_parent(pc);
      send_to_char ( "Cabal set.\n\r", ch);
      return TRUE;
    }
}

REDIT( redit_format )
{
    ROOM_INDEX_DATA *pRoom;
    EDIT_ROOM(ch, pRoom);
    pRoom->description = format_string( pRoom->description );
    if (pRoom->description2 && pRoom->description2[0])
      pRoom->description2 = format_string( pRoom->description2 );
    send_to_char( "String formatted.\n\r", ch );
    return TRUE;
}

REDIT( redit_mreset )
{
    ROOM_INDEX_DATA	*pRoom;
    MOB_INDEX_DATA	*pMobIndex;
    CHAR_DATA		*newmob;
    char		arg [ MIL ];
    char		arg2 [ MIL ];
    RESET_DATA		*pReset;
    char		output [ MSL ];
    EDIT_ROOM(ch, pRoom);
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    if ( arg[0] == '\0' || !is_number( arg ) )
    {
	send_to_char ( "Syntax:  mreset <vnum> <max #x> <mix #x>\n\r", ch );
	return FALSE;
    }
    if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
    {
	send_to_char( "REdit: No mobile has that vnum.\n\r", ch );
	return FALSE;
    }
    if ( pMobIndex->area != pRoom->area )
    {
	send_to_char( "REdit: No such mobile in this area.\n\r", ch );
	return FALSE;
    }
    pReset              = new_reset_data();
    pReset->command	= 'M';
    pReset->arg1	= pMobIndex->vnum;
    pReset->arg2	= is_number( arg2 ) ? atoi( arg2 ) : MAX_MOB;
    pReset->arg3	= pRoom->vnum;
    pReset->arg4	= is_number( argument ) ? atoi (argument) : 1;
    add_reset( pRoom, pReset, 0 );
    newmob = create_mobile( pMobIndex );
    char_to_room( newmob, pRoom );
    sprintf( output, "%s (%d) has been loaded and added to resets.\n\rThere will be a maximum of %d loaded to this room.\n\r",
      capitalize( pMobIndex->short_descr ), pMobIndex->vnum, pReset->arg4 );
    send_to_char( output, ch );
    act( "$n has created $N!", ch, NULL, newmob, TO_ROOM );
    return TRUE;
}

REDIT( redit_treset ){
  ROOM_INDEX_DATA	*pRoom;
  TRAP_INDEX_DATA	*pTrap;
  TRAP_DATA		*pT;
  OBJ_DATA		*pObj = NULL;
  AREA_DATA		*ed;
  RESET_DATA		*pReset;
  char			arg [MIL];
  char			arg2[MIL];
  char			buf[MIL];
  int			type = 0;
  int			exit = 0;
  EDIT_ROOM(ch, pRoom);

  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );

/* check for syntax */
  if ( IS_NULLSTR(arg) || IS_NULLSTR(arg2) || !is_number( arg ) ){
    send_to_char ( "Syntax:  treset <vnum> <obj/exit> <target>\n\r", ch );
    return FALSE;
  }
/* check for vnum */
  if ( (pTrap = get_trap_index(atoi( arg ))) == NULL ){
    send_to_char( "REdit: No trap has that vnum.\n\r", ch );
    return FALSE;
  }
/* area check */
  if ( (ed = get_vnum_area(pTrap->vnum)) == NULL
       || (ed != pRoom->area && get_trust(ch) < IMPLEMENTOR)){
    send_to_char( "REdit: No such trap in this area.\n\r", ch );
    return FALSE;
  }
/* target check */
  if (!str_cmp("obj", arg2)){
    type = TRAP_ON_OBJ;
    if ( IS_NULLSTR(argument)
	 || (pObj = get_obj_list(ch, argument, pRoom->contents)) == NULL){
      send_to_char("Trap onto what object?\n\r", ch);
      return FALSE;
    }
    else
      exit = pObj->pIndexData->vnum;
  }
  else if (!str_cmp("exit", arg2)){
    type = TRAP_ON_EXIT;
    if (IS_NULLSTR(argument)){
      send_to_char("Trap which exit?\n\r", ch);
      return FALSE;
    }
    if ( (exit = dir_lookup(argument)) < 0 || exit >= MAX_DOOR){
      send_to_char("Invalid exit.\n\r", ch);
      return FALSE;
    }
    if (pRoom->exit[exit] == NULL){
      send_to_char("Exit is nonexistant.\n\r", ch);
      return FALSE;
    }
  }
  else{
    send_to_char("\"treset obj\" or \"treset exit <dir>\"\n\r", ch);
    return FALSE;
  }

  if ( (pT = create_trap(pTrap, NULL)) == NULL){
    send_to_char("Could not load a trap.\n\r", ch);
    return FALSE;
  }
  if (type == TRAP_ON_OBJ){
    if (pObj == NULL){
      send_to_char("treset <vnum> obj <obj>\n\r", ch);
      return FALSE;
    }
    if (pObj->traps){
      send_to_char("That object already has a trap.\n\r", ch);
      return FALSE;
    }
    trap_to_obj( pT, pObj);
    sprintf(buf, "%s", pObj->short_descr);
  }
  else if (type == TRAP_ON_EXIT){
    if (pRoom->exit[exit]->traps){
      send_to_char("That exit already has a trap.\n\r", ch);
      return FALSE;
    }
    trap_to_exit( pT, pRoom->exit[exit]);
    sprintf(buf, "%s door", dir_name[exit]);
  }
  act("$n create a trap on $t.", ch, buf, NULL, TO_ROOM);
  act("You've created a trap on $t.", ch, buf, NULL, TO_CHAR);

  pReset		= new_reset_data();
  pReset->command	= 'T';
  pReset->arg1		= pTrap->vnum;
  pReset->arg2		= type;
  pReset->arg3		= pRoom->vnum;
  pReset->arg4		= exit;
  add_reset( pRoom, pReset, 0 );
  return TRUE;
}

struct wear_type
{
    int	wear_loc;
    int	wear_bit;
};

const struct wear_type wear_table[] =
{
    {	WEAR_FINGER_L,	ITEM_WEAR_FINGER	},
    {	WEAR_FINGER_R,	ITEM_WEAR_FINGER	},
    {	WEAR_NECK_1,	ITEM_WEAR_NECK		},
    {	WEAR_NECK_2,	ITEM_WEAR_NECK		},
    {	WEAR_BODY,	ITEM_WEAR_BODY		},
    {	WEAR_HEAD,	ITEM_WEAR_HEAD		},
    {	WEAR_LEGS,	ITEM_WEAR_LEGS		},
    {	WEAR_FEET,	ITEM_WEAR_FEET		},
    {	WEAR_HANDS,	ITEM_WEAR_HANDS		},
    {	WEAR_ARMS,	ITEM_WEAR_ARMS		},
    {	WEAR_SHIELD,	ITEM_WEAR_SHIELD	},
    {	WEAR_ABOUT,	ITEM_WEAR_ABOUT		},
    {	WEAR_WAIST,	ITEM_WEAR_WAIST		},
    {	WEAR_WRIST_L,	ITEM_WEAR_WRIST		},
    {	WEAR_WRIST_R,	ITEM_WEAR_WRIST		},
    {	WEAR_WIELD,	ITEM_WIELD		},
    {	WEAR_HOLD,	ITEM_HOLD		},
    {   WEAR_SECONDARY, ITEM_WIELD              },
    {   WEAR_TATTOO,    ITEM_WEAR_TATTOO        },
    {   WEAR_SHROUD,    ITEM_WEAR_SHROUD        },
    {   WEAR_EARRING,   ITEM_WEAR_EARRING       },
    {   WEAR_FACE,	ITEM_WEAR_FACE		},
    {   WEAR_RANGED,	ITEM_WEAR_RANGED	},
    {   WEAR_QUIVER,	ITEM_WEAR_QUIVER	},
    {	WEAR_LIGHT,	ITEM_LIGHT		},
    {	WEAR_NONE,	ITEM_TAKE		},
    {	NO_FLAG,	NO_FLAG			}
};

/* Returns the location of the bit that matches the count. *
 * Called by oedit_reset(olc_act.c).                       */
int wear_loc(int bits, int count)
{
    int flag;
    for (flag = 0; wear_table[flag].wear_bit != NO_FLAG; flag++)
        if ( IS_SET(bits, wear_table[flag].wear_bit) && --count < 1)
            return wear_table[flag].wear_loc;
    return NO_FLAG;
}

/* Converts a wear_loc into a bit.    *
 * Called by redit_oreset(olc_act.c). */
int wear_bit(int loc)
{
    int flag;
    for (flag = 0; wear_table[flag].wear_loc != NO_FLAG; flag++)
        if ( loc == wear_table[flag].wear_loc )
            return wear_table[flag].wear_bit;
    return 0;
}

REDIT( redit_oreset )
{
    ROOM_INDEX_DATA	*pRoom;
    OBJ_INDEX_DATA	*pObjIndex;
    OBJ_DATA		*newobj;
    OBJ_DATA		*to_obj;
    CHAR_DATA		*to_mob;
    char		arg1 [ MIL ];
    char		arg2 [ MIL ];
    int			olevel = 0;
    RESET_DATA		*pReset;
    char		output [ MSL ];
    EDIT_ROOM(ch, pRoom);
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
	send_to_char ( "Syntax:  oreset <vnum> <args>\n\r", ch );
	send_to_char ( "        -no_args               = into room\n\r", ch );
	send_to_char ( "        -<obj_name>            = into obj\n\r", ch );
	send_to_char ( "        -<mob_name> <wear_loc> = into mob\n\r", ch );
	return FALSE;
    }
    if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
    {
	send_to_char( "REdit: No object has that vnum.\n\r", ch );
	return FALSE;
    }
    if ( pObjIndex->area != pRoom->area )
    {
	send_to_char( "REdit: No such object in this area.\n\r", ch );
	return FALSE;
    }
    if ( arg2[0] == '\0' )
    {
	pReset		= new_reset_data();
	pReset->command	= 'O';
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= 0;
	pReset->arg3	= pRoom->vnum;
	pReset->arg4	= 0;
	add_reset( pRoom, pReset, 0/* Last slot*/ );
	newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
	obj_to_room( newobj, pRoom );
	sprintf( output, "%s (%d) has been loaded and added to resets.\n\r",
          capitalize( pObjIndex->short_descr ), pObjIndex->vnum );
	send_to_char( output, ch );
    }
    else if ( argument[0] == '\0' && ( ( to_obj = get_obj_list( ch, arg2, pRoom->contents ) ) != NULL ) )
    {
	pReset		= new_reset_data();
	pReset->command	= 'P';
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= 0;
	pReset->arg3	= to_obj->pIndexData->vnum;
	pReset->arg4	= 1;
        add_reset( pRoom, pReset, 0 );
	newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
	newobj->cost = 0;
	obj_to_obj( newobj, to_obj );
        sprintf( output, "%s (%d) has been loaded into %s (%d) and added to resets.\n\r",
          capitalize( newobj->short_descr ), newobj->pIndexData->vnum,
          to_obj->short_descr, to_obj->pIndexData->vnum );
	send_to_char( output, ch );
    }
    else if ( ( to_mob = get_char_room( ch, NULL, arg2 ) ) != NULL )
    {
	int	wear_loc;
	if ( (wear_loc = flag_value( wear_loc_flags, argument )) == NO_FLAG )
	{
	    send_to_char( "REdit: Invalid wear_loc.  '? wear-loc'\n\r", ch );
	    return FALSE;
	}
	if ( !IS_SET( pObjIndex->wear_flags, wear_bit(wear_loc) ) )
	{
            sprintf( output, "%s (%d) has wear flags: [%s]\n\r",
              capitalize( pObjIndex->short_descr ), pObjIndex->vnum,
              flag_string( wear_flags, pObjIndex->wear_flags ) );
	    send_to_char( output, ch );
	    return FALSE;
	}
	if ( wear_loc >= 0 && get_eq_char( to_mob, wear_loc ) )
	{
	    send_to_char( "REdit:  Object already equipped.\n\r", ch );
	    return FALSE;
	}
	pReset		= new_reset_data();
	pReset->arg1	= pObjIndex->vnum;
	pReset->arg2	= wear_loc;
	if ( pReset->arg2 == WEAR_NONE )
	    pReset->command = 'G';
	else
	    pReset->command = 'E';
	pReset->arg3	= wear_loc;
	add_reset( pRoom, pReset, 0/* Last slot*/ );
	olevel  = URANGE( 0, to_mob->level - 2, LEVEL_HERO );
        newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
        if ( to_mob->pIndexData->pShop )
	{
	    switch ( pObjIndex->item_type )
	    {
	    default:		olevel = 0;				break;
	    case ITEM_PILL:	olevel = number_range(  0, 10 );	break;
	    case ITEM_POTION:	olevel = number_range(  0, 10 );	break;
	    case ITEM_ARTIFACT:
	    case ITEM_RELIC:	olevel = number_range(  0, 10 );	break;
	    case ITEM_SCROLL:	olevel = number_range(  5, 15 );	break;
	    case ITEM_WAND:	olevel = number_range( 10, 20 );	break;
	    case ITEM_STAFF:	olevel = number_range( 15, 25 );	break;
	    case ITEM_SOCKET:   olevel = number_range(  0, 10 );        break;
	    case ITEM_ARMOR:	olevel = number_range(  5, 15 );	break;
	    case ITEM_WEAPON:	if ( pReset->command == 'G' )
	    			    olevel = number_range( 5, 15 );
				else
				    olevel = number_fuzzy( olevel );
                                break;
	    }
	    newobj = create_object( pObjIndex, olevel );
	    if ( pReset->arg2 == WEAR_NONE )
		SET_BIT( newobj->extra_flags, ITEM_INVENTORY );
	}
	else
	    newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
	obj_to_char( newobj, to_mob );
	if ( pReset->command == 'E' )
	    equip_char( to_mob, newobj, pReset->arg3 );
        sprintf( output, "%s (%d) has been loaded %s of %s (%d) and added to resets.\n\r",
          capitalize( pObjIndex->short_descr ), pObjIndex->vnum,
	    flag_string( wear_loc_strings, pReset->arg3 ),
            to_mob->short_descr, to_mob->pIndexData->vnum );
	send_to_char( output, ch );
    }
    else
    {
	send_to_char( "REdit:  That mobile isn't here.\n\r", ch );
	return FALSE;
    }
    act( "$n has created $p!", ch, newobj, NULL, TO_ROOM );
    return TRUE;
}

/* shows trap values */
void show_trap_values( CHAR_DATA* ch, TRAP_INDEX_DATA* trap){
  char buf [MSL];

  switch( trap->type){
  default:
  case TTYPE_DAMAGE:
    sprintf( buf,
	     "[v0] Weapon:          [%s]\n\r"
	     "[v1] Base Damage:     [%d]\n\r"
	     "[v2] Dice Number:     [%d] Avg [%d]\n\r"
	     "[v3] Dice Type:       [%d]\n\r",
	     attack_table[trap->value[0]].name,
	     trap->value[1],
	     trap->value[2],
	     trap->value[1] + (1 + trap->value[3]) * (trap->value[2] / 2),
	     trap->value[3]);
    send_to_char( buf, ch );
    break;
  case TTYPE_XDAMAGE:
    sprintf( buf,
	     "[v0] Skill verb:      [%s]\n\r"
	     "[v1] Base Damage:     [%d]\n\r"
	     "[v2] Dice Number:     [%d] Avg [%d]\n\r"
	     "[v3] Dice Type:       [%d]\n\r"
	     "[v4] Dam Type:        [%s]\n\r",
	     get_vir_attack(trap->value[0]),
	     trap->value[1],
	     trap->value[2],
	     trap->value[1] + (1 + trap->value[3]) * (trap->value[2] / 2),
	     trap->value[3],
	     damtype_table[trap->value[4]].name);
    send_to_char( buf, ch );
    break;
  case TTYPE_SPELL:
    sprintf( buf,
	     "[v0] First Spell:     [%s]\n\r"
	     "[v1] Lvl. modifier:   [%d]\n\r"
	     "[v2] Second Spell:    [%s]\n\r"
	     "[v3] Lvl. modifier:   [%d]\n\r"
	     "[v4] Dam Type:        [%s]\n\r",
	     trap->value[0] < 1 ? "none" : skill_table[trap->value[0]].name,
	     trap->value[1],
	     trap->value[2] < 1 ? "none" : skill_table[trap->value[2]].name,
	     trap->value[3],
	     damtype_table[trap->value[4]].name);
    send_to_char( buf, ch );
    break;
  case TTYPE_MOB:
    sprintf( buf,
	     "[v0] First Mob:       [%d]\n\r"
	     "[v1] %%Hp. modifier:  [%d]\n\r"
	     "[v2] Second Mob:      [%d]\n\r"
	     "[v3] %%Hp. modifier:  [%d]\n\r"
	     "[v4] Duration:        [%d]\n\r",
	     trap->value[0],
	     trap->value[1],
	     trap->value[2],
	     trap->value[3],
	     trap->value[4]);
    send_to_char( buf, ch );
    break;
  case TTYPE_DUMMY:
    sprintf( buf,
	     "[v0] First Room :      [%d]\n\r"
	     "[v1] Second Room:      [%d]\n\r"
	     "[v2] Third Room :      [%d]\n\r"
	     "[v3] Fourth Room:      [%d]\n\r"
	     "[v4] Delay Only?:      [%s]\n\r",
	     trap->value[0],
	     trap->value[1],
	     trap->value[2],
	     trap->value[3],
	     trap->value[4] == 0 ? "N" : "Y");
    send_to_char( buf, ch );
    break;
  }
}


/* Object Editor Functions. */
void show_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *obj )
{
    char buf[MSL];
    switch( obj->item_type )
    {
    default:
        break;
    case ITEM_LIGHT:
        if ( obj->value[2] == -1 || obj->value[2] == 999 )
            sprintf( buf, "[v2] Light:  Infinite[-1]\n\r" );
        else
            sprintf( buf, "[v2] Light:  [%d]\n\r", obj->value[2] );
        send_to_char( buf, ch );
        break;
    case ITEM_WAND:
    case ITEM_STAFF:
        sprintf( buf,
          "[v0] Level:          [%d]\n\r"
          "[v1] Charges Total:  [%d]\n\r"
          "[v2] Charges Left:   [%d]\n\r"
          "[v3] Spell:          %s\n\r",
          obj->value[0], obj->value[1], obj->value[2],
          obj->value[3] != -1 ? skill_table[obj->value[3]].name : "none" );
        send_to_char( buf, ch );
        break;
    case ITEM_THROW:
        sprintf( buf,
          "[v0] Quantity Left:  [%d]\n\r"
          "[v1] Number of dice: [%d]\n\r"
          "[v2] Type of dice:   [%d]\n\r"
          "[v3] Type:           %s\n\r"
          "[v4] Special type:   %s\n\r",
          obj->value[0], obj->value[1], obj->value[2],attack_table[obj->value[3]].name, flag_string( weapon_type2,obj->value[4]) );
        send_to_char( buf, ch );
        break;
    case ITEM_RELIC:
    case ITEM_ARTIFACT:
    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
        sprintf( buf,
          "[v0] Level:  [%d]\n\r"
          "[v1] Spell:  %s\n\r"
          "[v2] Spell:  %s\n\r"
          "[v3] Spell:  %s\n\r"
          "[v4] Spell:  %s\n\r",
          obj->value[0],
          obj->value[1] != -1 ? skill_table[obj->value[1]].name : "none",
          obj->value[2] != -1 ? skill_table[obj->value[2]].name : "none",
          obj->value[3] != -1 ? skill_table[obj->value[3]].name : "none",
          obj->value[4] != -1 ? skill_table[obj->value[4]].name : "none" );
        send_to_char( buf, ch );
        break;
    case ITEM_HERB:
        sprintf( buf,
          "[v0] Timer:  [%d]\n\r"
          "[v1] Eaten:  %s\n\r"
          "[v2] Herb:   %s\n\r"
          "[v3] Brew:   %s\n\r"
          "[v4] Smoked: %s\n\r",
          obj->value[0],
          obj->value[1] != -1 ? skill_table[obj->value[1]].name : "none",
          obj->value[2] != -1 ? skill_table[obj->value[2]].name : "none",
          obj->value[3] != -1 ? skill_table[obj->value[3]].name : "none",
          obj->value[4] != -1 ? skill_table[obj->value[4]].name : "none" );
        send_to_char( buf, ch );
        break;
    case ITEM_ARMOR:
        sprintf( buf,
          "[v0] Ac pierce       [%d]\n\r"
          "[v1] Ac bash         [%d]\n\r"
          "[v2] Ac slash        [%d]\n\r"
          "[v3] Ac exotic       [%d]\n\r",
          obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
        send_to_char( buf, ch );
        break;
    case ITEM_WEAPON:
        sprintf( buf, "[v0] Weapon class:   %s\n\r", flag_string( weapon_class, obj->value[0] ) );
        send_to_char( buf, ch );
        sprintf( buf, "[v1] Number of dice: [%d]\n\r", obj->value[1] );
        send_to_char( buf, ch );
        sprintf( buf, "[v2] Type of dice:   [%d]       Avg [%d]\n\r", obj->value[2], (1+obj->value[2])*obj->value[1]/2);
        send_to_char( buf, ch );
        sprintf( buf, "[v3] Type:           %s\n\r", attack_table[obj->value[3]].name );
        send_to_char( buf, ch );
        sprintf( buf, "[v4] Special type:   %s\n\r", flag_string( weapon_type2,  obj->value[4] ) );
        send_to_char( buf, ch );
        break;
    case ITEM_CONTAINER:
        sprintf( buf,
          "[v0] Weight:     [%d kg]\n\r"
          "[v1] Flags:      [%s]\n\r"
          "[v2] Key:     %s [%d]\n\r"
          "[v3] Capacity    [%d]\n\r"
          "[v4] Weight Mult [%d]\n\r",
          obj->value[0], flag_string( container_flags, obj->value[1] ),
          get_obj_index(obj->value[2]) ? get_obj_index(obj->value[2])->short_descr : "none",
          obj->value[2], obj->value[3], obj->value[4] );
        send_to_char( buf, ch );
        break;
    case ITEM_DRINK_CON:
        sprintf( buf,
          "[v0] Liquid Total: [%d]\n\r"
          "[v1] Liquid Left:  [%d]\n\r"
          "[v2] Liquid:       %s\n\r"
          "[v3] Poisoned:     %s\n\r",
          obj->value[0], obj->value[1], liq_table[obj->value[2]].liq_name, obj->value[3] != 0 ? "Yes" : "No" );
        send_to_char( buf, ch );
        break;
    case ITEM_FOUNTAIN:
        sprintf( buf,
          "[v0] Liquid Total: [%d]\n\r"
          "[v1] Liquid Left:  [%d]\n\r"
          "[v2] Liquid:       %s\n\r",
          obj->value[0], obj->value[1], liq_table[obj->value[2]].liq_name);
        send_to_char( buf,ch );
        break;
    case ITEM_FOOD:
        sprintf( buf,
          "[v0] Food hours: [%d]\n\r"
          "[v1] Full hours: [%d]\n\r"
          "[v3] Poisoned:   %s\n\r"
          "[v4] Food lasts: [%d]\n\r",
          obj->value[0], obj->value[1], obj->value[3] != 0 ? "Yes" : "No", obj->value[4] );
        send_to_char( buf, ch );
        break;
    case ITEM_MONEY:
        sprintf( buf, "[v0] Gold:   [%d]\n\r", obj->value[0] );
        send_to_char( buf, ch );
        break;
    case ITEM_INSTRUMENT:
        sprintf( buf, "[v4] Special type:   %s\n\r", flag_string( weapon_type2,  obj->value[4] ) );
        send_to_char( buf, ch );
        break;
    case ITEM_SOCKET:
      sendf( ch, "[v0] Socket:   %s\n\r", flag_string( socket_flags,  obj->value[0] ) );
      sendf( ch, "[v1] Extra :   %s\n\r", flag_string( extra_flags,  obj->value[1] ) );
      if (IS_SOC_STAT(obj, SOCKET_WEAPON)){
	sprintf( buf, "[v4] Weapon:   %s\n\r", flag_string( weapon_type2,  obj->value[4] ) );
	send_to_char( buf, ch );
      }
      break;
    case ITEM_RANGED:
        sprintf( buf, "[v0] Ammo Type:   [%s]\n\r", flag_string(projectile_type, obj->value[0] ) );
        send_to_char( buf, ch );
	sprintf( buf, "[v1] Accuracy:    [%d]\n\r", obj->value[1]);
        send_to_char( buf, ch );
	sprintf( buf, "[v2] Shots/Max: [%d/%d]\n\r", obj->value[2], obj->value[3] < 1 ? rounds_per_projectile( obj->value[0]) : 999);
        send_to_char( buf, ch );
	sprintf( buf, "[v3] Ammo (Vnum): ");
        send_to_char( buf, ch );
	if (obj->value[3] > 0){
	  sprintf( buf, "[%d]\n\r", obj->value[3]);
	}
	else{
	  sprintf( buf, "Any\n\r");
	}
        send_to_char( buf, ch );
	sprintf( buf, "[v4] Flags:       [%s]\n\r", flag_string(ranged_type, obj->value[4] ) );
        send_to_char( buf, ch );
        break;

    case ITEM_PROJECTILE:
        sprintf( buf, "[v0] Ammo Type:   %s\n\r", flag_string( projectile_type, obj->value[0] ) );
        send_to_char( buf, ch );
        sprintf( buf, "[v1] Number of dice: [%d]\n\r", obj->value[1] );
        send_to_char( buf, ch );
        sprintf( buf, "[v2] Type of dice:   [%d]       Avg [%d]\n\r",obj->value[2], (1+obj->value[2])*obj->value[1]/2);
        send_to_char( buf, ch );
        sprintf( buf, "[v3] Type:           %s\n\r", attack_table [obj->value[3]].name );
        send_to_char( buf, ch );
        sprintf( buf, "[v4] Special Type:   %s\n\r", flag_string( proj_spec_type, obj->value[4] ) );
        send_to_char( buf, ch );
	break;
    }
    return;
}

bool set_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, int value_num, char *argument)
{
    switch( pObj->item_type )
    {
    default:
        break;
    case ITEM_LIGHT:
        switch ( value_num )
        {
        default:
            do_help( ch, "ITEM_LIGHT" );
            return FALSE;
        case 2:
            send_to_char( "HOURS OF LIGHT SET.\n\r\n\r", ch );
            pObj->value[2] = atoi( argument );
            break;
        }
        break;
    case ITEM_WAND:
    case ITEM_STAFF:
        switch ( value_num )
        {
        default:
            do_help( ch, "ITEM_STAFF_WAND" );
            return FALSE;
        case 0:
            send_to_char( "SPELL LEVEL SET.\n\r\n\r", ch );
            pObj->value[0] = atoi( argument );
            break;
        case 1:
            send_to_char( "TOTAL NUMBER OF CHARGES SET.\n\r\n\r", ch );
            pObj->value[1] = atoi( argument );
            break;
        case 2:
            send_to_char( "CURRENT NUMBER OF CHARGES SET.\n\r\n\r", ch );
            pObj->value[2] = atoi( argument );
            break;
        case 3:
            send_to_char( "SPELL TYPE SET.\n\r", ch );
            pObj->value[3] = skill_lookup( argument );
            break;
        }
        break;
    case ITEM_SOCKET:
      switch ( value_num ){
      default:
	do_help( ch, "ITEM_SOCKET" );
	return FALSE;
      case 0:
	send_to_char( "SOCKET FLAG TOGGLED.\n\r\n\r", ch );
	pObj->value[0] ^= (flag_value( socket_flags, argument ) != NO_FLAG
			   ? flag_value( socket_flags, argument ) : 0 );
	break;
      case 1:
	send_to_char( "EXTRA FLAG TOGGLED.\n\r\n\r", ch );
	pObj->value[1] ^= (flag_value( extra_flags, argument ) != NO_FLAG
			   ? flag_value( extra_flags, argument ) : 0 );
	break;
      case 4:
	if (!IS_SOC_STAT(pObj, SOCKET_WEAPON)){
	  send_to_char("Socket needs \"weapon\" socket flag first.\n\r", ch);
	  break;
	}
	send_to_char( "WEAPON FLAG TOGGLED.\n\r\n\r", ch );
	pObj->value[4] ^= (flag_value( weapon_type2, argument ) != NO_FLAG
			   ? flag_value( weapon_type2, argument ) : 0 );
	break;
      }
      break;
    case ITEM_SCROLL:
    case ITEM_RELIC:
    case ITEM_ARTIFACT:
    case ITEM_POTION:
    case ITEM_PILL:
        switch ( value_num )
        {
        default:
            do_help( ch, "ITEM_SCROLL_POTION_PILL" );
            return FALSE;
        case 0:
            send_to_char( "SPELL LEVEL SET.\n\r\n\r", ch );
            pObj->value[0] = atoi( argument );
            break;
        case 1:
            send_to_char( "SPELL TYPE 1 SET.\n\r\n\r", ch );
            pObj->value[1] = skill_lookup( argument );
            break;
        case 2:
            send_to_char( "SPELL TYPE 2 SET.\n\r\n\r", ch );
            pObj->value[2] = skill_lookup( argument );
            break;
        case 3:
            send_to_char( "SPELL TYPE 3 SET.\n\r\n\r", ch );
            pObj->value[3] = skill_lookup( argument );
            break;
        case 4:
            send_to_char( "SPELL TYPE 4 SET.\n\r\n\r", ch );
            pObj->value[4] = skill_lookup( argument );
            break;
        }
        break;
    case ITEM_HERB:
        switch ( value_num )
        {
        default:
            do_help( ch, "ITEM_HERB" );
            return FALSE;
        case 0:
            send_to_char( "TIMER SET.\n\r\n\r", ch );
            pObj->value[0] = atoi( argument );
            break;
        case 1:
            send_to_char( "EATEN SET.\n\r\n\r", ch );
            pObj->value[1] = skill_lookup( argument );
            break;
        case 2:
            send_to_char( "HERB SET.\n\r\n\r", ch );
            pObj->value[2] = skill_lookup( argument );
            break;
        case 3:
            send_to_char( "BREW SET.\n\r\n\r", ch );
            pObj->value[3] = skill_lookup( argument );
            break;
        case 4:
            send_to_char( "SMOKED SET.\n\r\n\r", ch );
            pObj->value[4] = skill_lookup( argument );
            break;
        }
        break;
    case ITEM_ARMOR:
        switch ( value_num )
        {
        default:
            do_help( ch, "ITEM_ARMOR" );
            return FALSE;
        case 0:
            send_to_char( "AC PIERCE SET.\n\r\n\r", ch );
            pObj->value[0] = atoi( argument );
            break;
        case 1:
            send_to_char( "AC BASH SET.\n\r\n\r", ch );
            pObj->value[1] = atoi( argument );
            break;
        case 2:
            send_to_char( "AC SLASH SET.\n\r\n\r", ch );
            pObj->value[2] = atoi( argument );
            break;
        case 3:
            send_to_char( "AC EXOTIC SET.\n\r\n\r", ch );
            pObj->value[3] = atoi( argument );
            break;
        }
        break;
    case ITEM_THROW:
        switch ( value_num )
        {
        default:
            do_help( ch, "ITEM_THROW" );
            return FALSE;
        case 0:
            send_to_char( "QUANTITY LEFT SET.\n\r\n\r", ch );
            pObj->value[0] = atoi( argument );
            break;
        case 1:
            send_to_char( "NUMBER OF DICE SET.\n\r\n\r", ch );
            pObj->value[1] = atoi( argument );
            break;
        case 2:
            send_to_char( "TYPE OF DICE SET.\n\r\n\r", ch );
            pObj->value[2] = atoi( argument );
            break;
        case 3:
            send_to_char( "WEAPON TYPE SET.\n\r\n\r", ch );
            pObj->value[3] = attack_lookup( argument );
            break;
        case 4:
            send_to_char( "SPECIAL WEAPON TYPE TOGGLED.\n\r\n\r", ch );
            pObj->value[4] ^= (flag_value( weapon_type2, argument ) != NO_FLAG
              ? flag_value( weapon_type2, argument ) : 0 );
            break;
        }
        break;
    case ITEM_WEAPON:
        switch ( value_num )
        {
        default:
            do_help( ch, "ITEM_WEAPON" );
            return FALSE;
        case 0:
            send_to_char( "WEAPON CLASS SET.\n\r\n\r", ch );
            pObj->value[0] = weapon_type( argument );
            break;
        case 1:
            send_to_char( "NUMBER OF DICE SET.\n\r\n\r", ch );
            pObj->value[1] = atoi( argument );
            break;
        case 2:
            send_to_char( "TYPE OF DICE SET.\n\r\n\r", ch );
            pObj->value[2] = atoi( argument );
            break;
        case 3:
            send_to_char( "WEAPON TYPE SET.\n\r\n\r", ch );
            pObj->value[3] = attack_lookup( argument );
            break;
        case 4:
            send_to_char( "SPECIAL WEAPON TYPE TOGGLED.\n\r\n\r", ch );
            pObj->value[4] ^= (flag_value( weapon_type2, argument ) != NO_FLAG
              ? flag_value( weapon_type2, argument ) : 0 );
            break;
        }
        break;
    case ITEM_CONTAINER:
        switch ( value_num )
        {
        int value;
        default:
            do_help( ch, "ITEM_CONTAINER" );
            return FALSE;
        case 0:
            send_to_char( "WEIGHT CAPACITY SET.\n\r\n\r", ch );
            pObj->value[0] = atoi( argument );
            break;
        case 1:
            if ( ( value = flag_value( container_flags, argument ) ) != NO_FLAG )
                TOGGLE_BIT(pObj->value[1], value);
            else
            {
                do_help ( ch, "ITEM_CONTAINER" );
                return FALSE;
            }
            send_to_char( "CONTAINER TYPE SET.\n\r\n\r", ch );
            break;
        case 2:
            if ( atoi(argument) != 0 )
            {
                if ( !get_obj_index( atoi( argument ) ) )
                {
                    send_to_char( "THERE IS NO SUCH ITEM.\n\r\n\r", ch );
                    return FALSE;
                }
                if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY )
                {
                    send_to_char( "THAT ITEM IS NOT A KEY.\n\r\n\r", ch );
                    return FALSE;
                }
            }
            send_to_char( "CONTAINER KEY SET.\n\r\n\r", ch );
            pObj->value[2] = atoi( argument );
            break;
        case 3:
            send_to_char( "CONTAINER MAX WEIGHT SET.\n\r", ch);
            pObj->value[3] = atoi( argument );
            break;
        case 4:
            send_to_char( "WEIGHT MULTIPLIER SET.\n\r\n\r", ch );
            pObj->value[4] = atoi ( argument );
            break;
	    }
        break;
    case ITEM_DRINK_CON:
        switch ( value_num )
        {
        default:
            do_help( ch, "ITEM_DRINK" );
            return FALSE;
        case 0:
            send_to_char( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
            pObj->value[0] = atoi( argument );
            break;
        case 1:
            send_to_char( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
            pObj->value[1] = atoi( argument );
            break;
        case 2:
            send_to_char( "LIQUID TYPE SET.\n\r\n\r", ch );
            pObj->value[2] = ( liq_lookup(argument) != -1 ? liq_lookup(argument) : 0 );
            break;
        case 3:
            send_to_char( "POISON VALUE TOGGLED.\n\r\n\r", ch );
            pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
            break;
        }
        break;
    case ITEM_FOUNTAIN:
        switch (value_num)
        {
        default:
            do_help( ch, "ITEM_FOUNTAIN" );
            return FALSE;
        case 0:
            send_to_char( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
            pObj->value[0] = atoi( argument );
            break;
        case 1:
            send_to_char( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
            pObj->value[1] = atoi( argument );
            break;
        case 2:
            send_to_char( "LIQUID TYPE SET.\n\r\n\r", ch );
            pObj->value[2] = ( liq_lookup(argument) != -1 ? liq_lookup(argument) : 0 );
            break;
        }
        break;
    case ITEM_FOOD:
        switch ( value_num )
        {
        default:
            do_help( ch, "ITEM_FOOD" );
            return FALSE;
        case 0:
            send_to_char( "HOURS OF FOOD SET.\n\r\n\r", ch );
            pObj->value[0] = atoi( argument );
            break;
        case 1:
            send_to_char( "HOURS OF FULL SET.\n\r\n\r", ch );
            pObj->value[1] = atoi( argument );
            break;
        case 3:
            send_to_char( "POISON VALUE TOGGLED.\n\r\n\r", ch );
            pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
            break;
        case 4:
            send_to_char( "HOURS OF LASTS SET.\n\r\n\r", ch );
            pObj->value[4] = atoi( argument );
            break;
            }
        break;
    case ITEM_MONEY:
        switch ( value_num )
        {
        default:
            do_help( ch, "ITEM_MONEY" );
            return FALSE;
        case 0:
            send_to_char( "GOLD AMOUNT SET.\n\r\n\r", ch );
            pObj->value[0] = atoi( argument );
            break;
        case 1:
            send_to_char( "SILVER AMOUNT SET.\n\r\n\r", ch );
            pObj->value[1] = atoi( argument );
            break;
	    }
        break;
    case ITEM_INSTRUMENT:
        switch ( value_num )
        {
        default:
            do_help( ch, "ITEM_INSTRUMENT" );
            return FALSE;
        case 4:
            send_to_char( "SPECIAL WEAPON TYPE TOGGLED.\n\r\n\r", ch );
            pObj->value[4] ^= (flag_value( weapon_type2, argument ) != NO_FLAG
              ? flag_value( weapon_type2, argument ) : 0 );
            break;
        }
        break;
    case ITEM_RANGED:
      switch ( value_num ){
      default:
	do_help( ch, "ITEM_RANGED" );
	return FALSE;
      case 0:
	send_to_char( "AMMO TYPE SET.\n\r\n\r", ch );
        pObj->value[0] ^= (flag_value( projectile_type, argument ) != NO_FLAG
			   ? flag_value( projectile_type, argument ) : 0 );
	if (pObj->value[3] < 1 && pObj->value[2] > rounds_per_projectile(pObj->value[0])){
	  send_to_char("SHOTS PER ROUND LIMITED BY AMMO TYPE\n\r", ch);
	  pObj->value[2] = rounds_per_projectile(pObj->value[0]);
	}
	break;
      case 1:
	send_to_char( "ACCURACY SET.\n\r", ch);
	pObj->value[1] = atoi( argument );
	break;
      case 2:
	send_to_char( "SHOTS PER ROUND SET.\n\r", ch);
	pObj->value[2] = atoi( argument );
	if (pObj->value[3] < 1 && pObj->value[2] > rounds_per_projectile(pObj->value[0])){
	  send_to_char("SHOTS PER ROUND LIMITED BY AMMO TYPE\n\r", ch);
	  pObj->value[2] = rounds_per_projectile(pObj->value[0]);
	}
	break;
      case 3:
	send_to_char( "SPECIFIC AMMO SET SET.\n\r", ch);
	pObj->value[3] = atoi( argument );
	break;
      case 4:
	send_to_char( "RANGED FLAG SET.\n\r\n\r", ch );
        pObj->value[4] ^= (flag_value( ranged_type, argument ) != NO_FLAG
              ? flag_value( ranged_type, argument ) : 0 );
	break;
      }
      break;

    case ITEM_PROJECTILE:
      switch ( value_num )
        {
        default:
	  do_help( ch, "ITEM_PROJECTILE" );
	  return FALSE;
        case 0:
	  send_to_char( "AMMO TYPE SET.\n\r\n\r", ch );
	  pObj->value[0] = (flag_value( projectile_type, argument ) != NO_FLAG
			    ? flag_value( projectile_type, argument ) : 0 );
	  if (pObj->value[1] * pObj->value[2] > MAX_PROJ_DAM / rounds_per_projectile(pObj->value[0])){
	    sendf( ch, "This projectile type can only have max damage of %d.", MAX_PROJ_DAM / rounds_per_projectile(pObj->value[0]));
	    pObj->value[1] = (MAX_PROJ_DAM / rounds_per_projectile(pObj->value[0])) / pObj->value[2];
	  }
	  break;
        case 1:
	  send_to_char( "NUMBER OF DICE SET.\n\r\n\r", ch );
	  if (atoi( argument) * pObj->value[2] > MAX_PROJ_DAM / rounds_per_projectile(pObj->value[0])){
	    sendf( ch, "This projectile type can only have max damage of %d.", MAX_PROJ_DAM / rounds_per_projectile(pObj->value[0]));
	    break;
	  }
	  pObj->value[1] = atoi( argument );
	  break;
        case 2:
	  send_to_char( "TYPE OF DICE SET.\n\r\n\r", ch );
	  if (atoi( argument) * pObj->value[1] > MAX_PROJ_DAM / rounds_per_projectile(pObj->value[0])){
	    sendf( ch, "This projectile type can only have max damage of %d.", MAX_PROJ_DAM / rounds_per_projectile(pObj->value[0]));
	    break;
	  }
	  pObj->value[2] = atoi( argument );
	  break;
        case 3:
	  send_to_char( "WEAPON TYPE SET.\n\r\n\r", ch );
	  pObj->value[3] = attack_lookup( argument );
	  break;
        case 4:
	  send_to_char( "SPECIAL TYPE SET.\n\r\n\r", ch );
	  pObj->value[4] ^= (flag_value( proj_spec_type, argument ) !=NO_FLAG
			     ? flag_value( proj_spec_type, argument ) : 0 );
	  break;
        }
      break;
    }
    show_obj_values( ch, pObj );
    return TRUE;
}

void cedit( CHAR_DATA *ch, char *argument)
{
  CABAL_INDEX_DATA* pCab = EDIT_CABAL(ch, pCab );
  char arg[MIL];
  char command[MIL];
  int cmd;

  smash_tilde(argument);
  strcpy(arg, argument);
  argument = one_argument( argument, command);

  if (command[0] == '\0'){
    cedit_show(ch, argument);
    tail_chain( );
    return;
  }

  if (!str_cmp(command, "done") ){
    edit_done(ch);
    return;
  }
  if ( !IS_BUILDER( ch, pCab->pArea ) ){
    send_to_char( "CEdit:  Insufficient security to modify area.\n\r", ch );
    edit_done( ch );
    return;
  }

  for (cmd = 0; cedit_table[cmd].name != NULL; cmd++){
    if (!str_prefix(command, cedit_table[cmd].name) ){
      if (pCab && (*cedit_table[cmd].olc_fun) (ch, argument) == TRUE){
	SET_BIT(pCab->pArea->area_flags, AREA_CHANGED);
      }
      return;
    }
  }
  interpret(ch, arg);
  return;
}

void hedit( CHAR_DATA *ch, char *argument)
{
  char arg[MIL];
  char command[MIL];
  int cmd;

  smash_tilde(argument);
  strcpy(arg, argument);
  argument = one_argument( argument, command);

  if (command[0] == '\0'){
    hedit_show(ch, argument);
    return;
  }

  if (!str_cmp(command, "done") ){
    edit_done(ch);
    return;
  }

  if (ch->pcdata->security < 8){
    sendf(ch, "Requires level %d security.\n\r", 8);
    return;
  }

  for (cmd = 0; hedit_table[cmd].name != NULL; cmd++){
    if (!str_prefix(command, hedit_table[cmd].name) ){
      (*hedit_table[cmd].olc_fun) (ch, argument);
      return;
    }
  }
  interpret(ch, arg);
  return;
}

CEDIT(cedit_create)
{
  CABAL_INDEX_DATA *pCab;
  AREA_DATA* pArea;
  int value = top_cabal_index + 1;
  int i = 0;

  if ( get_cabal_index(value) ){
    send_to_char("CEdit: Code vnum already exists.\n\r",ch);
    return FALSE;
  }
  pArea = get_vnum_area( value );
  if ( !pArea ){
    send_to_char( "CEdit:  That vnum is not assigned an area.\n\r", ch );
    return FALSE;
  }
  if ( !IS_BUILDER( ch, pArea ) ){
    send_to_char( "CEdit:  Vnum in an area you cannot build in.\n\r", ch );
    return FALSE;
  }
  top_cabal_index++;
  pCab			= new_cabal_index();
  pCab->pArea		= pArea;
  SET_BIT(pCab->pArea->area_flags, AREA_CHANGED);

  pCab->next		= cabal_index_list;
  cabal_index_list	= pCab;

/* init values */
  pCab->vnum		= value;
  pCab->version		= SAVECAB_VER;

  pCab->file_name	= str_dup("cabal");
  pCab->who_name	= str_dup("WHO NAME");
  pCab->immortal	= str_dup( ch->name );
  pCab->currency	= str_dup( "coin" );

  pCab->min_lvl		= 20;
  pCab->max_lvl		= 50;

  pCab->max_member	= 25;
  pCab->max_room	= 50;

  for (i = 0; i < RANK_MAX && i < MAX_CLANR; i++){
    pCab->mranks[i] = str_dup( clanr_table[i][1] );

  }
  for (i = 0; i < RANK_MAX && i < MAX_CLANR; i++){
    pCab->franks[i] = str_dup( clanr_table[i][1] );
  }

  for (i = 0; i < MAX_CLASS; i++){
    pCab->class[i] = 0;
  }
  pCab->class[0] = -1;

  for (i = 0; i < MAX_PC_RACE; i++){
    pCab->race[i] = 0;
  }
  pCab->race[0] = -1;
  pCab->fAvatar = TRUE;

  pCab->align		= ALIGN_GOOD | ALIGN_EVIL | ALIGN_NEUTRAL;
  pCab->ethos		= ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC;

  ch->desc->pEdit	= (void *)pCab;
  ch->desc->editor	= ED_CABAL;

  send_to_char("CABAL Created.\n\r", ch);
  return TRUE;
}

CEDIT( cedit_show ){
  CABAL_INDEX_DATA *pCab, *pParent;

  char buf[MSL];
  char string_buf[MIL];
  int i = 0;

  EDIT_CABAL(ch, pCab);

  pParent = get_cabal_index( pCab->parent_vnum );

  sprintf( string_buf, "%s", flag_string( cabal_flags, pCab->flags ));
  sprintf( buf, "Vnum:    [%-5d] (V.%d) Parent: [%-10s] Clan: [%-10s]\n\r"\
	        "Name:    [%-15s]     WhoName :    [%-15s]  File: %s.cab\n\r"\
		"Imm :    [%-15s]     Currency:    [%-15s]\n\r"\
		"City:	 [%-15s]     Anchor  :    [%d]\n\r"\
	        "MaxRoom: [%-5d] Enemy [%-15s]\n\r"\
	        "Guard:   [%-5d] Pit [%-5d] Army[%-5d] Army2[%-5d] Tower[%-5d] Tower2[%-5d]\n\r"\
	        "Flags:   [%-15s]\n\r",
	   pCab->vnum,
	   pCab->version,
	   pParent ? pParent->name : "None",
	   pCab->clan ? pCab->clan : "None",
	   pCab->name,
	   pCab->who_name,
	   pCab->file_name,
	   pCab->immortal,
	   pCab->currency,
	   pCab->city_name,
	   pCab->anchor_vnum,
	   pCab->max_room,
	   IS_NULLSTR(pCab->enemy_name) ? "none" : pCab->enemy_name,
	   pCab->guard_vnum,
	   pCab->pit_vnum,
	   pCab->army, pCab->army_upgrade,
	   pCab->tower, pCab->tower_upgrade,
	   string_buf );
  send_to_char( buf, ch );
  sprintf( string_buf, "%s", flag_string( cabal_flags2, pCab->flags2 ));
  sprintf( buf, "Flags2:  [%-15s]\n\r", string_buf);
  send_to_char( buf, ch );

  sendf( ch, "Prefix:  [%-10s] [%-10s] [%10s]\n\r",
	 IS_NULLSTR(pCab->mprefix) ? "none" : pCab->mprefix,
	 IS_NULLSTR(pCab->fprefix) ? "none" : pCab->fprefix,
	 IS_NULLSTR(pCab->sprefix) ? "none" : pCab->sprefix);

  if (pParent == NULL)
    sendf( ch, "OnGate:  %s\n\rOffGate: %s\n\rMsgGate: %s\n\r\n\r",
	   pCab->gate_on,
	   pCab->gate_off,
	   pCab->gate_msg );

  sprintf( buf,	"Level: [%d to %d] with Members: [%d]\n\r",
	   pCab->min_lvl,
	   pCab->max_lvl,
	   pCab->max_member);

  send_to_char( buf, ch );
  if (pParent){
    sprintf( buf, "Rank required to join: %d\n\r", pCab->min_rank );
    send_to_char( buf, ch );
  }
  sendf( ch, "Progress requirements: %s\n\r", flag_string(cabal_progress_flags, pCab->progress));

  sprintf( buf, "Align:    [%s%s%s]    Ethos:    [%s%s%s]\n\r",
	   IS_SET(pCab->align, ALIGN_GOOD) ? "G" : " ",
	   IS_SET(pCab->align, ALIGN_NEUTRAL) ? "N" : " ",
	   IS_SET(pCab->align, ALIGN_EVIL) ? "E" : " ",
	   IS_SET(pCab->ethos, ETHOS_LAWFUL) ? "L" : " ",
	   IS_SET(pCab->ethos, ETHOS_NEUTRAL) ? "N" : " ",
	   IS_SET(pCab->ethos, ETHOS_CHAOTIC) ? "C" : " ");
  send_to_char( buf, ch );

  sprintf( buf, "Avatar: %s\n\r", pCab->fAvatar ? "Yes" : "No" );
  send_to_char( buf, ch );

  if (pCab->race[0] < 0){
    sprintf(buf, "Races: All");
  }
  else{
    sprintf(buf, "Race: ");
    for (i = 0; i < MAX_PC_RACE; i++){
      char temp[MIL];
      if (pCab->race[i] > 0){
	sprintf(temp, "%-5s ", race_table[i].name);
	strcat(buf, temp);
      }
    }
  }
  send_to_char( buf, ch );
  send_to_char("\n\r", ch);

  if (pCab->class[0] < 0){
    sprintf(buf, "Class: All");
  }
  else{
    sprintf(buf, "Class: ");
    for (i = 0; i < MAX_CLASS; i++){
      char temp[MIL];
      if (pCab->class[i] > 0 ){
	sprintf(temp, "%-5s ", class_table[i].who_name);
	strcat(buf, temp);
      }
    }
  }

  send_to_char( buf, ch );
  send_to_char("\n\r", ch);
  sprintf(buf, "MRanks: ");
  for (i = 0; i < RANK_MAX; i++){
    if (!IS_NULLSTR(pCab->mranks[i])){
      char temp[MIL];
      sprintf(temp, "%d = %-10s[%s] ", i, pCab->mranks[i], rank_letters[i]);
      strcat(buf, temp);
    }
  }
  send_to_char( buf, ch );
  send_to_char("\n\r", ch);
  sprintf(buf, "FRanks: ");
  for (i = 0; i < RANK_MAX; i++){
    if (!IS_NULLSTR(pCab->franks[i])){
      char temp[MIL];
      sprintf(temp, "%d = %-10s[%s] ", i, pCab->franks[i], rank_letters[i]);
      strcat(buf, temp);
    }
  }
  send_to_char( buf, ch );
  send_to_char("\n\r", ch);

  send_to_char("(Use skill, vote, room commands to see those data lists.)\n\r", ch );

  tail_chain( );
  return FALSE;
}

CEDIT( cedit_name ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What name?\n\r", ch);
    return FALSE;
  }
  else{
    if (pCab->name != NULL)
      free_string(pCab->name);
    pCab->name = str_dup( argument );
    send_to_char("Name set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_clan ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What clan?\n\r", ch);
    return FALSE;
  }
  else if (GetClanHandle( argument ) < 1){
    send_to_char("That clan does not exist.\n\r", ch);
    return FALSE;
  }
  else{
    if (pCab->clan != NULL)
      free_string(pCab->clan);
    pCab->clan = str_dup( argument );
    send_to_char("Clan set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_prefix ){
  CABAL_INDEX_DATA *pCab;
  char arg[MIL], arg2[MIL];;
  EDIT_CABAL(ch, pCab);

  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );

  if ( IS_NULLSTR(argument) ){
    send_to_char("prefix <male> <female> <sponsor>\n\r", ch);
    return FALSE;
  }
  else{
    if (pCab->mprefix != NULL)
      free_string(pCab->mprefix);
    if (!str_cmp(arg, "none"))
      pCab->mprefix = NULL;
    else
      pCab->mprefix = str_dup( capitalize(arg) );

    if (pCab->fprefix != NULL)
      free_string(pCab->fprefix);
    if (!str_cmp(arg2, "none"))
      pCab->fprefix = NULL;
    else
    pCab->fprefix = str_dup( capitalize(arg2) );

    if (pCab->sprefix != NULL)
      free_string(pCab->sprefix);
    if (!str_cmp(argument, "none"))
      pCab->sprefix = NULL;
    else
      pCab->sprefix = str_dup( capitalize(argument) );

    send_to_char("Prefixes set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_file_name ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What file name?\n\r", ch);
    return FALSE;
  }
  else{
    if (pCab->file_name != NULL)
      free_string(pCab->file_name);
    argument[8] = '\0';
    pCab->file_name = str_dup( argument );
    send_to_char("File name set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_ongate ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("Message cleared.\n\r", ch);
    if (pCab->gate_on != NULL)
      free_string(pCab->gate_on);
    pCab->gate_on = str_dup("");
    return FALSE;
  }
  else{
    if (pCab->gate_on != NULL)
      free_string(pCab->gate_on);
    pCab->gate_on = str_dup( argument );
    send_to_char("OnGate string set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_msggate ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("Message cleared.\n\r", ch);
    if (pCab->gate_msg != NULL)
      free_string(pCab->gate_msg);
    pCab->gate_msg = str_dup("");
    return FALSE;
  }
  else{
    if (pCab->gate_msg != NULL)
      free_string(pCab->gate_msg);
    pCab->gate_msg = str_dup( argument );
    send_to_char("MsgGate string set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_offgate ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What string is to be echoed?\n\r", ch);
    return FALSE;
  }
  else{
    if (pCab->gate_off != NULL)
      free_string(pCab->gate_off);
    pCab->gate_off = str_dup( argument );
    send_to_char("OffGate string set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_city ){
  CABAL_INDEX_DATA *pCab;
  AREA_DATA* pArea;
  EDIT_CABAL(ch, pCab);


  if ( IS_NULLSTR(argument) ){
    send_to_char("What city?\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp("none", argument)){
    free_string(pCab->city_name);
    pCab->city_name = str_dup("");
    send_to_char("City cleared.\n\r", ch);
    return TRUE;
  }
  else if (!IS_CABAL(pCab, CABAL_ROYAL)){
    send_to_char("Only for ROYAL cabals.\n\r", ch);
    return FALSE;
  }
  else if ( (pArea = get_area_data_str( argument )) == NULL){
    send_to_char("No such area.\n\r", ch);
    return FALSE;
  }
  else{
    free_string(pCab->city_name);
    pCab->city_name = str_dup(pArea->name);
    send_to_char("City set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_enemy ){
  CABAL_INDEX_DATA *pCab;
  CABAL_DATA* pEnemy;
  EDIT_CABAL(ch, pCab);


  if ( IS_NULLSTR(argument) ){
    send_to_char("What cabal for enemy?\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp("none", argument)){
    free_string(pCab->enemy_name);
    pCab->enemy_name = str_dup("");
    send_to_char("Enemy cleared.\n\r", ch);
    return TRUE;
  }
  else if ( (pEnemy = get_cabal( argument )) == NULL){
    send_to_char("No such cabal.\n\r", ch);
    return FALSE;
  }
  else{
    free_string(pCab->enemy_name);
    pCab->enemy_name = str_dup(pEnemy->name);
    send_to_char("Enemy set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_anchor ){
  CABAL_INDEX_DATA *pCab;
  ROOM_INDEX_DATA* pRoom;
  EDIT_CABAL(ch, pCab);


  if ( IS_NULLSTR(argument) ){
    send_to_char("What room vnum?\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp("none", argument)){
    pCab->anchor_vnum = 0;
    send_to_char("Anchor room cleared.\n\r", ch);
    return TRUE;
  }
  else if (IS_CABAL(pCab, CABAL_SUBCABAL)){
    send_to_char("Only for PARENT cabals.\n\r", ch);
    return FALSE;
  }
  else if ( (pRoom = get_room_index( atoi(argument) )) == NULL || IS_VIRROOM(pRoom)){
    send_to_char("No such room.\n\r", ch);
    return FALSE;
  }
  else{
    pCab->anchor_vnum = pRoom->vnum;
    send_to_char("Anchor room set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_guard ){
  CABAL_INDEX_DATA *pCab;
  MOB_INDEX_DATA* pMob;
  EDIT_CABAL(ch, pCab);


  if ( IS_NULLSTR(argument) ){
    send_to_char("What mob vnum?\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp("none", argument)){
    pCab->guard_vnum = 0;
    send_to_char("Guard mob cleared.\n\r", ch);
    return TRUE;
  }
  else if (IS_CABAL(pCab, CABAL_SUBCABAL)){
    send_to_char("Only for PARENT cabals.\n\r", ch);
    return FALSE;
  }
  else if ( (pMob = get_mob_index( atoi(argument) )) == NULL){
    send_to_char("No such mob.\n\r", ch);
    return FALSE;
  }
  else{
    pCab->guard_vnum = pMob->vnum;
    send_to_char("Guard mob set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_pit ){
  ROOM_INDEX_DATA* pRoom;
  CABAL_INDEX_DATA* pCab;
  EDIT_CABAL(ch, pCab);


  if ( IS_NULLSTR(argument) ){
    send_to_char("What room vnum?\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp("none", argument)){
    pCab->pit_vnum = 0;
    send_to_char("Pit room cleared.\n\r", ch);
    return TRUE;
  }
  else if (IS_CABAL(pCab, CABAL_SUBCABAL)){
    send_to_char("Only for PARENT cabals.\n\r", ch);
    return FALSE;
  }
  else if ( (pRoom = get_room_index( atoi(argument) )) == NULL){
    send_to_char("No such room.\n\r", ch);
    return FALSE;
  }
  else{
    pCab->pit_vnum = pRoom->vnum;
    send_to_char("Pitroom set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_army ){
  CABAL_INDEX_DATA *pCab;
  char arg[MIL];
  int val, val2;

  EDIT_CABAL(ch, pCab);
  argument = one_argument( argument, arg );

  if ( IS_NULLSTR(argument) ){
    send_to_char("army <normal> <upgraded>\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp("none", argument)){
    pCab->army = 0;
    pCab->army_upgrade = 0;
    send_to_char("Armies cleared.\n\r", ch);
    return TRUE;
  }
  val = atoi( arg );
  if ( get_army_index( val) == NULL){
    sendf( ch, "No such army: %d\n\r", val );
    return FALSE;
  }

  val2 = atoi( argument );
  if ( get_army_index( val) == NULL){
    sendf( ch, "No such army: %d\n\r", val );
    return FALSE;
  }

  pCab->army = val;
  pCab->army_upgrade = val2;
  return TRUE;

}

CEDIT( cedit_tower ){
  CABAL_INDEX_DATA *pCab;
  char arg[MIL];
  int val, val2;

  EDIT_CABAL(ch, pCab);
  argument = one_argument( argument, arg );

  if ( IS_NULLSTR(argument) ){
    send_to_char("tower <normal> <upgraded>\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp("none", argument)){
    pCab->tower = 0;
    pCab->tower_upgrade = 0;
    send_to_char("Towers cleared.\n\r", ch);
    return TRUE;
  }
  val = atoi( arg );
  if ( get_army_index( val) == NULL){
    sendf( ch, "No such army: %d\n\r", val );
    return FALSE;
  }

  val2 = atoi( argument );
  if ( get_army_index( val) == NULL){
    sendf( ch, "No army mob: %d\n\r", val );
    return FALSE;
  }

  pCab->tower = val;
  pCab->tower_upgrade = val2;
  return TRUE;

}


CEDIT( cedit_whoname ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What who name?\n\r", ch);
    return FALSE;
  }
  else{
    if (pCab->who_name != NULL)
      free_string(pCab->who_name);
    pCab->who_name = str_dup( argument );
    send_to_char("Who Name set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_imm ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("Which Immortal will oversee the cabal?\n\r", ch);
    return FALSE;
  }
  else{
    if (pCab->immortal != NULL)
      free_string(pCab->immortal);
    pCab->immortal = str_dup( argument );
    send_to_char("Immortal set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_currency ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What currency (singular)?\n\r", ch);
    return FALSE;
  }
  else{
    if (pCab->currency != NULL)
      free_string(pCab->currency);
    pCab->currency = str_dup( argument );
    send_to_char("Currency set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_members ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What is the maximum of members allowed?\n\r", ch);
    return FALSE;
  }
  else{
    pCab->max_member = atoi( argument );
    send_to_char("Member maximum set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_tax ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What is the tax?\n\r", ch);
    return FALSE;
  }
  else{
    pCab->max_member = URANGE(0, atoi( argument ), 50);
    send_to_char("Tax set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_max_room ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What is the maximum of rooms allowed?\n\r", ch);
    return FALSE;
  }
  else{
    pCab->max_room = atoi( argument );
    send_to_char("Room maximum set.\n\r", ch);
  }
  return TRUE;
}

CEDIT( cedit_rank ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("Rank <number> (Set minimum cabal rank to join)\n\r", ch);
    return FALSE;
  }
  pCab->min_rank = URANGE(0, atoi( argument ), RANK_MAX);
  sendf(ch, "Cabal child rank req. now %d (%s)\n\r", pCab->min_rank, clanr_table[pCab->min_rank][0]);
  return TRUE;
}

CEDIT( cedit_levels ){
  CABAL_INDEX_DATA *pCab;
  char min[MIL];
  EDIT_CABAL(ch, pCab);


  if ( IS_NULLSTR(argument) ){
    send_to_char("level <min> <max> (set min and max level requirements)\n\r", ch);
    return FALSE;
  }
  argument = one_argument( argument, min );
  pCab->min_lvl = atoi( min );
  pCab->max_lvl = atoi( argument );
  sendf(ch, "Cabal level req. now from %d to %d.\n\r", pCab->min_lvl, pCab->max_lvl);
  return TRUE;
}

CEDIT( cedit_mrank ){
  CABAL_INDEX_DATA *pCab;
  char value[MIL];
  int rank = 0;

  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("rank <rank> <male title>\n\r", ch);
    return FALSE;
  }
  argument = one_argument( argument, value );
  rank = atoi(value);

  if (!IS_NULLSTR(pCab->mranks[rank])){
    free_string(pCab->mranks[rank]);
  }
  pCab->mranks[rank] = str_dup( argument );
  sendf(ch, "Rank set. (%d. %s[%s])\n\r", rank, pCab->mranks[rank], rank_letters[rank]);
  return TRUE;
}

CEDIT( cedit_frank ){
  CABAL_INDEX_DATA *pCab;
  char value[MIL];
  int rank = 0;

  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("rank <rank> <female title>\n\r", ch);
    return FALSE;
  }
  argument = one_argument( argument, value );
  rank = atoi(value);

  if (!IS_NULLSTR(pCab->franks[rank])){
    free_string(pCab->franks[rank]);
  }
  pCab->franks[rank] = str_dup( argument );
  sendf(ch, "Rank set. (%d. %s[%s])\n\r", rank, pCab->franks[rank], rank_letters[rank]);
  return TRUE;
}

CEDIT( cedit_flag ){
  CABAL_INDEX_DATA *pCab;
  int flag = flag_value( cabal_flags, argument );
  int flag2 = flag_value( cabal_flags2, argument );
  EDIT_CABAL(ch, pCab);

  if (flag == NO_FLAG && flag2 == NO_FLAG){
    send_to_char("No such flag. Use \"? cabal_flag or ? cabal_flag2\" for list.\n\r", ch);
    return FALSE;
  }
  if (flag != NO_FLAG)
    pCab->flags ^= flag;
  else if (flag2 != NO_FLAG)
    pCab->flags2 ^= flag2;

  send_to_char("Flag toggled.\n\r", ch);
  return TRUE;
}

CEDIT( cedit_progress ){
  CABAL_INDEX_DATA *pCab;
  int flag = flag_value( cabal_progress_flags, argument );
  EDIT_CABAL(ch, pCab);

  if (flag == NO_FLAG){
    send_to_char("No such flag. Use \"? cabal_progress_flag\" for list.\n\r", ch);
    return FALSE;
  }
  pCab->progress ^= flag;
  send_to_char("Flag toggled.\n\r", ch);
  return TRUE;
}

HEDIT(hedit_create)
{
  HELP_DATA *pHelp;
  int value = top_help + 1;


  if ( get_help_index(value) ){
    send_to_char("HEdit: Code vnum already exists.\n\r",ch);
    return FALSE;
  }

  pHelp			= new_help_index();
  if ( help_first == NULL )
    help_first = pHelp;
  if ( help_last  != NULL )
    help_last->next = pHelp;
  help_last	= pHelp;
  pHelp->next	= NULL;
  pHelp->vnum		= value;
  pHelp->level		= HELP_ALL;
  pHelp->type		= 0;
  pHelp->keyword	= str_dup( "none" );
  pHelp->text		= str_dup( "" );

  ch->desc->pEdit	= (void *)pHelp;
  ch->desc->editor	= ED_HELP;

  send_to_char("HELP Created.\n\r",ch);
  return TRUE;
}

CEDIT( cedit_align ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("Toggle what align? (good/netural/evil)\n\r", ch);
    return FALSE;
  }
  else if (!str_prefix(argument, "good")){
    TOGGLE_BIT(pCab->align, ALIGN_GOOD);
    send_to_char("Good align toggled.\n\r", ch);
  }
  else if (!str_prefix(argument, "evil")){
    TOGGLE_BIT(pCab->align, ALIGN_EVIL);
    send_to_char("Evil align toggled.\n\r", ch);
  }
  else if (!str_prefix(argument, "neutral")){
    TOGGLE_BIT(pCab->align, ALIGN_NEUTRAL);
    send_to_char("Neutral align toggled.\n\r", ch);
  }
  else
    send_to_char("Toggle what align?\n\r", ch);
  return TRUE;
}

CEDIT( cedit_ethos ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("Toggle what ethos? (lawful/netural/chaotic)\n\r", ch);
    return FALSE;
  }
  else if (!str_prefix(argument, "lawful")){
    TOGGLE_BIT(pCab->ethos, ETHOS_LAWFUL);
    send_to_char("Lawful ethos toggled.\n\r", ch);
  }
  else if (!str_prefix(argument, "neutral")){
    TOGGLE_BIT(pCab->ethos, ETHOS_NEUTRAL);
    send_to_char("Neutral ethos toggled.\n\r", ch);
  }
  else if (!str_prefix(argument, "chaotic")){
    TOGGLE_BIT(pCab->ethos, ETHOS_CHAOTIC);
    send_to_char("Chaotic ethos toggled.\n\r", ch);
  }
  else
    send_to_char("Toggle what ethos?\n\r", ch);
  return TRUE;
}

CEDIT( cedit_race ){
  CABAL_INDEX_DATA *pCab;
  int race = 0;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("Toggle what race?\n\r", ch);
    return FALSE;
  }
  if (!str_cmp(argument, "all") ){
    pCab->race[0] = -1;
    send_to_char("All races now allowed.\n\r", ch);
  }
  else if ( (race = race_lookup( argument )) < 1
	    || race >= MAX_PC_RACE){
    send_to_char("No such player race.\n\r", ch);
    return TRUE;
  }
  else{
    pCab->race[race] = !pCab->race[race];
    if (pCab->race[race] && pCab->race[0] < 0)
      pCab->race[0] = 0;
    send_to_char("Race toggled.\n\r", ch);
  }
  return TRUE;
}


CEDIT( cedit_class ){
  CABAL_INDEX_DATA *pCab;
  int class = 0;
  EDIT_CABAL(ch, pCab);

  if ( IS_NULLSTR(argument) ){
    send_to_char("Toggle what class?\n\r", ch);
    return FALSE;
  }
  if (!str_cmp(argument, "all") ){
    pCab->class[0] = -1;
    send_to_char("All classes now allowed.\n\r", ch);
  }
  else if ( (class = class_lookup( argument )) < 0
	    || class >= MAX_CLASS){
    send_to_char("No such player class.\n\r", ch);
    return TRUE;
  }
  else{
    pCab->class[class] = !pCab->class[class];
    if (pCab->class[class] && pCab->class[0] < 0)
      pCab->class[0] = 0;
    send_to_char("Class toggled.\n\r", ch);
  }
  return TRUE;
}


CEDIT( cedit_skill ){
  CABAL_INDEX_DATA *pCab;
  CSKILL_DATA* cSkill;
  CSKILL_DATA* cPrev;
  int i = 0;
  char arg1[MIL];
  char arg2[MIL];
  int val = 0;
  EDIT_CABAL(ch, pCab);

  argument = one_argument(argument, arg1);
  val = atoi(arg1);

  if (IS_NULLSTR(argument) && str_cmp(arg1, "show")){
    send_to_char("Sytnax:\n\r"\
		 "skill add <name>\n\r"\
		 "skill <#> <show/del/name/level/rating/skill/min/max/mcost/cost> [value]\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp(arg1, "show")){
    int cnt;
    CSKILL_DATA* cSkill;
    char buf[MSL];

    for ( cnt = 0, cSkill = pCab->skills; cSkill; cSkill = cSkill->next ){
      if ( cnt == 0 ){
	send_to_char("\n\rSKILLS:\n\r", ch);
	send_to_char( "Number     Name       Level   Ranks   Skill   Rate   MinCp   Cost   Flags\n\r", ch );
	send_to_char( "------ ------------- ------- ------- ------- ------ ------- ------ --------\n\r", ch );
      }
      sprintf( buf, "[%4d] %-15s %-6d %d-%-6d %-8d %-6d %-6d %-6d %-15s\n\r",
	       cnt,
	       skill_table[cSkill->pSkill->sn].name,
	       cSkill->pSkill->level,
	       cSkill->min_rank, cSkill->max_rank,
	       cSkill->pSkill->learned,
	       cSkill->pSkill->rating,
	       cSkill->min_cost,
	       cSkill->cost,
	       cSkill->flag ? "error" : "none");
      send_to_char( buf, ch );
      cnt++;
    }
    return FALSE;
  }
  else if (!str_cmp(arg1, "add")){
    if ( (val = skill_lookup(argument)) < 1 || val > MAX_SKILL){
      send_to_char("No such skill.\n\r", ch);
      return FALSE;
    }
    else{
      CSKILL_DATA* cSkill = malloc( sizeof( *cSkill ) );
      cSkill->pSkill = new_skill();

      cSkill->next		= NULL;
      cSkill->pSkill->sn	= val;
      cSkill->pSkill->level	= pCab->min_lvl;
      cSkill->pSkill->rating	= 1;
      cSkill->pSkill->learned	= 2;
      cSkill->min_rank		= RANK_NEWBIE;
      cSkill->max_rank		= RANK_LEADER;
      cSkill->min_cost		= 0;
      cSkill->cost		= skill_table[val].min_mana / 5;
      cSkill->flag		= 0;

      if (pCab->skills == NULL)
	pCab->skills = cSkill;
      else{
	cSkill->next = pCab->skills;
	pCab->skills = cSkill;
      }
      send_to_char("Skill added.\n\r", ch);
      return TRUE;
    }
  }
  else if (val < 0){
    send_to_char("Act on which cabal skill?\n\r", ch );
    return FALSE;
  }
  /* check rest of the options */
  argument = one_argument(argument, arg2);

  /* from now on we always need a pointer to the skill we are working on */
  cSkill = pCab->skills;
  cPrev = NULL;
  if (pCab->skills == NULL){
    send_to_char("No skills found.\n\r", ch);
    return FALSE;
  }
  else{
    cPrev = NULL;
    cSkill = pCab->skills;
  }
  /* get the right skill based on value passed */
  while (cSkill && i < val){
    cPrev = cSkill;
    cSkill = cSkill->next;
    i++;
  }

  if (cSkill == NULL){
    sendf(ch, "Skill #%d not found.\n\r", val);
    return FALSE;
  }


  if (!str_prefix(arg2, "del")){
    /* unlink the pointers */
    if (cPrev == NULL){
      pCab->skills = cSkill->next;
    }
    else{
      cPrev->next = cSkill->next;
      cSkill->next = NULL;
    }

    /* free the data */
    free_skill( cSkill->pSkill );
    free( cSkill );

    send_to_char("Skill deleted.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "name")){
    if ( (val = skill_lookup(argument)) < 1 || val > MAX_SKILL){
      send_to_char("No such skill.\n\r", ch);
      return FALSE;
    }
    cSkill->pSkill->sn = val;
    send_to_char("Value changed.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "level")){
    if ( (val = atoi( argument )) < 1){
      send_to_char("Level must be greater then 1.\n\r", ch);
      return FALSE;
    }
    cSkill->pSkill->level = val;
    send_to_char("Value changed.\n\r", ch);
    return TRUE;
  }
  else if(!str_cmp(arg2, "rating")){
    if ( (val = atoi( argument )) < 1 || val > 10){
      send_to_char("Rating must be between 1 - 10.\n\r", ch);
      return FALSE;
    }
    cSkill->pSkill->rating = val;
    send_to_char("Value changed.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "skill")){
    if ( (val = atoi( argument )) < 1 || val > 100){
      send_to_char("Starting skill must be between 1 - 100.\n\r", ch);
      return FALSE;
    }
    cSkill->pSkill->learned = val;
    send_to_char("Value changed.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "min")){
    if ( (val = atoi( argument )) < 0 || val > RANK_LEADER ){
      sendf(ch, "Minimum rank must be %d to %d.\n\r", RANK_NEWBIE, RANK_LEADER);
      return FALSE;
    }
    cSkill->min_rank = val;
    send_to_char("Value changed.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "max")){
    if ( (val = atoi( argument )) < 1 || val > RANK_LEADER ){
      sendf(ch, "Maximum rank must be %d to %d.\n\r", RANK_NEWBIE, RANK_LEADER);
      return FALSE;
    }
    cSkill->max_rank = val;
    send_to_char("Value changed.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "mcost")){
    if ( (val = atoi( argument )) < 0){
      sendf(ch, "Minimum %s level must be 0 or greater.\n\r", pCab->currency);
      return FALSE;
    }
    cSkill->min_cost = val;
    send_to_char("Value changed.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "cost")){
    if ( (val = atoi( argument )) < 0){
      sendf(ch, "Cost of use (in %s) must be 0 or greater.\n\r", pCab->currency);
      return FALSE;
    }
    cSkill->cost = val;
    send_to_char("Value changed.\n\r", ch);
    return TRUE;
  }
  else{
    send_to_char("Unkown command, use skill alone for syntax.\n\r", ch);
    return FALSE;
  }
}

CEDIT( cedit_vote ){
  CABAL_INDEX_DATA *pCab;
  CVOTE_DATA* cVote;
  CVOTE_DATA* cPrev;
  int i = 0;
  char arg1[MIL];
  char arg2[MIL];
  int val = 0;
  EDIT_CABAL(ch, pCab);

  argument = one_argument(argument, arg1);
  val = atoi(arg1);

  if (IS_NULLSTR(argument) && str_cmp(arg1, "show")){
    send_to_char("Syntax:\n\r"\
		 "vote add <name>\n\r"\
		 "vote <#> <show/del/name/min/max/cost> [value]\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp(arg1, "show")){
    int cnt = 0;
    char buf[MSL];
    CVOTE_DATA* cVote;

    for ( cnt = 0, cVote = pCab->votes; cVote; cVote = cVote->next ){
      if ( cnt == 0 ){
	send_to_char("\n\rVOTES:\n\r", ch);
      send_to_char( "Number     Name                 Ranks   Cost   Dur (days)\n\r", ch );
      send_to_char( "------ ----------------------- ------- ------- ----------\n\r", ch );
      }
      sprintf( buf, "[%4d] %-25s %d-%-6d %-8d %-6d\n\r",
	       cnt,
	       vote_table[cVote->vote].name,
	       cVote->min, cVote->max,
	     cVote->cost,
	       vote_table[cVote->vote].life_time / VOTE_DAY);
    send_to_char( buf, ch );
    cnt++;
    }
    return FALSE;
  }
  else if (!str_cmp(arg1, "add")){
    if ( (val = vote_lookup(argument)) < 1){
      send_to_char("Avaliable votes:\n\r", ch);
      show_votes( ch );
      return FALSE;
    }
    else{
      CVOTE_DATA* cVote = malloc( sizeof( *cVote ) );

      cVote->vote		= val;
      cVote->next		= NULL;
      cVote->min		= RANK_NEWBIE;
      cVote->max		= RANK_LEADER;
      cVote->cost		= 10;

      if (pCab->votes == NULL)
	pCab->votes = cVote;
      else{
	CVOTE_DATA* last = pCab->votes;
	while (last->next)
	  last = last->next;
	cVote->next = NULL;
	last->next = cVote;
      }
      send_to_char("Vote added.\n\r", ch);
      return TRUE;
    }
  }
  else if (val < 0){
    send_to_char("Act on which cabal vote?\n\r", ch );
    return FALSE;
  }
  /* check rest of the options */
  argument = one_argument(argument, arg2);

  /* from now on we always need a pointer to the skill we are working on */
  cVote = pCab->votes;
  cPrev = NULL;
  if (pCab->votes == NULL){
    send_to_char("No votes found.\n\r", ch);
    return FALSE;
  }
  else{
    cPrev = NULL;
    cVote = pCab->votes;
  }
  /* get the right skill based on value passed */
  while (cVote && i < val){
    cPrev = cVote;
    cVote = cVote->next;
    i++;
  }

  if (cVote == NULL){
    sendf(ch, "Vote #%d not found.\n\r", val);
    return FALSE;
  }


  if (!str_prefix(arg2, "del")){
    /* unlink the pointers */
    if (cPrev == NULL){
      pCab->votes = cVote->next;
    }
    else{
      cPrev->next = cVote->next;
      cVote->next = NULL;
    }

    /* free the data */
    free( cVote );

    send_to_char("Vote deleted.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "name")){
    if ( (val = vote_lookup(argument)) < 1){
      send_to_char("Avaliable votes:\n\r", ch);
      show_votes( ch );
      return FALSE;
    }
    cVote->vote = val;
    send_to_char("Vote changed.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "min")){
    if ( (val = atoi( argument )) < 0 || val > RANK_LEADER ){
      sendf(ch, "Minimum rank must be %d to %d.\n\r", RANK_NEWBIE, RANK_LEADER);
      return FALSE;
    }
    cVote->min = val;
    send_to_char("Value changed.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "max")){
    if ( (val = atoi( argument )) < 1 || val > RANK_LEADER ){
      sendf(ch, "Maximum rank must be %d to %d.\n\r", RANK_NEWBIE, RANK_LEADER);
      return FALSE;
    }
    cVote->max = val;
    send_to_char("Value changed.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "cost")){
    if ( (val = atoi( argument )) < 0){
      sendf(ch, "Cost of use (in %s) must be 0 or greater.\n\r", pCab->currency);
      return FALSE;
    }
    cVote->cost = val;
    send_to_char("Value changed.\n\r", ch);
    return TRUE;
  }
  else{
    send_to_char("Unkown command, use vote alone for syntax.\n\r", ch);
    return FALSE;
  }
}

CEDIT( cedit_room ){
  CABAL_INDEX_DATA *pCab;
  ROOM_INDEX_DATA *pRoom;
  CROOM_DATA* cRoom;
  char arg1[MIL];
  char arg2[MIL];
  int val = 0, vnum;
  EDIT_CABAL(ch, pCab);

  argument = one_argument(argument, arg1);
  val = atoi(arg1);

  if (IS_NULLSTR(argument) && str_cmp(arg1, "show")){
    send_to_char("Syntax:\n\r"\
		 "room show\n\r"\
		 "room  <level> <add/del/vnum> [value]\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp(arg1,"show")){
    int level = 0;
    char buf[MSL];
//    char string_buf[MIL];
    CROOM_DATA* cRoom;
    send_to_char("\n\rROOMS:\n\r", ch);
    send_to_char( "Level  Vnum   Type          Name\n\r", ch );
    send_to_char( "-------------------------------------------------\n\r", ch);
    for ( level = 0; level < CROOM_LEVELS; level++ ){
      cRoom = pCab->crooms[level];
      sprintf( buf, " %2d   %5d %-12.12s %s\n\r",
	       level,
	       cRoom ? cRoom->pRoomIndex->vnum : 0,
	       croom_table[level].name,
	       cRoom ? cRoom->pRoomIndex->name : "none");
      send_to_char( buf, ch );
    }
    return FALSE;
  }
  else if (val < 0 || val >= CROOM_LEVELS){
    send_to_char("Act on which level room?\n\r", ch );
    return FALSE;
  }
  /* check rest of the options */
  argument = one_argument(argument, arg2);

  if (!str_prefix(arg2, "add")){
    if ( (cRoom = pCab->crooms[val]) != NULL){
      send_to_char("That level already has a room attached.\n\r", ch );
      return FALSE;
    }
    else if ( (vnum = atoi( argument )) < 1
	      || (pRoom = get_room_index(vnum)) == NULL
	      || IS_VIRROOM(pRoom)){
      send_to_char("No such room vnum.\n\r", ch);
      return FALSE;
    }
    pCab->crooms[val] = new_croom();
    pCab->crooms[val]->pRoomIndex = pRoom;
    pCab->crooms[val]->level = val;

    send_to_char("Room added.\n\r", ch);
    return TRUE;
  }
  else if (!str_prefix(arg2, "del")){
    if ( (cRoom = pCab->crooms[val]) == NULL){
      send_to_char("That level has no room attached.\n\r", ch );
      return FALSE;
    }

    free_croom( cRoom );
    pCab->crooms[val] = NULL;
    send_to_char("Room deleted.\n\r", ch);
    return TRUE;
  }
  else if(!str_prefix(arg2, "vnum")){
    if ( (cRoom = pCab->crooms[val]) == NULL){
      send_to_char("That level has no room attached.\n\r", ch );
      return FALSE;
    }
    else if ( (vnum = atoi( argument )) < 1
	      || (pRoom = get_room_index(vnum)) == NULL
	      || IS_VIRROOM(pRoom)){
      send_to_char("No such room vnum.\n\r", ch);
      return FALSE;
    }
    cRoom->pRoomIndex	= pRoom;
    cRoom->level	= val;
    send_to_char("Room vnum changed.\n\r", ch);
    return TRUE;
  }
  else{
    send_to_char("Unkown command, use room alone for syntax.\n\r", ch);
    return FALSE;
  }
}


CEDIT( cedit_avatar ){
  CABAL_INDEX_DATA *pCab;
  EDIT_CABAL(ch, pCab);

  pCab->fAvatar = !pCab->fAvatar;
  sendf( ch, "Avatars now %s\n\r", pCab->fAvatar ? "allowed" : "disallowed");
  return TRUE;
}


CEDIT( cedit_parent ){
  CABAL_INDEX_DATA *pCab;
  CABAL_INDEX_DATA* pPar = NULL;
  EDIT_CABAL(ch, pCab);

  pPar = get_cabal_index( atoi(argument) );

  if (!str_cmp("none", argument)){
    pCab->parent_vnum = 0;
    send_to_char("Parent cleared.\n\r", ch);
    return TRUE;
  }
  else if ( pPar == NULL || pPar == pCab){
    send_to_char("Syntax: parent <cabal vnum>\n\r", ch);
    return FALSE;
  }
  else{
    int i = 0;
    pCab->parent_vnum = pPar->vnum;

    /* some stuff gets inherited */
    if (!IS_NULLSTR( pCab->currency))
      free_string( pCab->currency );
    pCab->currency = str_dup( pPar->currency );

    pCab->max_member = pPar->max_member;

    for (i = 0; i < MAX_PC_RACE; i++){
      pCab->race[i] = pPar->race[i];
    }
    for (i = 0; i < MAX_CLASS; i++){
      pCab->class[i] = pPar->class[i];
    }
    pCab->fAvatar = pPar->fAvatar;


    send_to_char("Parent set.\n\r", ch);
  }
  return TRUE;
}





HEDIT( hedit_show ){

  HELP_DATA *pHelp;
  char buf[MSL];
  EDIT_HELP(ch, pHelp);

  send_to_char("Text:\n\r", ch );
  send_to_char(pHelp->text, ch );

/* rest of the info */
  sprintf( buf, "\n\rVnum:    [%-5d]\n\r"\
	        "Keyword: [%-10s]\n\r",
	   pHelp->vnum,
	   pHelp->keyword);
  send_to_char( buf, ch );

  if (pHelp->level >= HELP_ALL){
    sprintf(buf,"Level:   [%-5d]  (Type: [none])\n\r", pHelp->level);
    send_to_char( buf, ch );
  }
  else{
    char* type;
    CABAL_INDEX_DATA* pCab;

    switch (pHelp->level){
    case HELP_RACE:
      /* special races helps */
      if (pHelp->type < 0){
	type = "avatar";
      }
      else
	type = race_table[pHelp->type].name;
      break;
    case HELP_CLASS:
      type = class_table[pHelp->type].name;
      break;
    case HELP_CABAL:
      if ( (pCab = get_cabal_index( pHelp->type )) == NULL)
	type = "error";
      else
	type = pCab->name;
      break;
    case HELP_PSALM:
      type = psalm_table[pHelp->type].name;
      break;
    case HELP_ALL:
    default:
      type = "";
    }
    sprintf( buf, "Type: [%-10s]\n\r", type);
    send_to_char(buf, ch);
  }
  return FALSE;
}

HEDIT( hedit_level ){
  HELP_DATA *pHelp;
  int value = atoi( argument );
  EDIT_HELP(ch, pHelp);

  if (pHelp->level < HELP_ALL){
    send_to_char("Use \"type none\" to clear type first.\n\r", ch);
    return FALSE;
  }
  else if ( value < -1 ){
    send_to_char("Level must be greater then -1 (all).\n\r", ch);
    return FALSE;
  }
  else
    pHelp->level = value;
  send_to_char("Help level set.\n\r", ch);
  return TRUE;
}

HEDIT( hedit_key ){
  HELP_DATA *pHelp;
  EDIT_HELP(ch, pHelp);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What keyword?\n\r", ch);
    return FALSE;
  }
  else{
    if (pHelp->keyword != NULL)
      free_string(pHelp->keyword);
    pHelp->keyword = str_dup( argument );
    send_to_char("Keyword set.\n\r", ch);
  }
  return TRUE;
}

HEDIT( hedit_type ){
  HELP_DATA *pHelp;
  CABAL_DATA* pCab;
  int value = race_lookup( argument );
  EDIT_HELP(ch, pHelp);

  if (!str_cmp( argument, "none") ){
    pHelp->type = 0;
    pHelp->level = HELP_ALL;
    send_to_char("Type cleared.\n\r", ch);
    return TRUE;
  }
  else if (value > 0){
    pHelp->type = value;
    pHelp->level = HELP_RACE;
    sendf(ch, "Help now avaliable only to race: %s\n\r", race_table[value].name);
    return TRUE;
  }
  else if ( (value = class_lookup( argument )) >= 0 ){
    pHelp->type = value;
    pHelp->level = HELP_CLASS;
    sendf(ch, "Help now avaliable only to class: %s\n\r", class_table[value].name);
    return TRUE;
  }
  else if ( (pCab = get_cabal( argument )) != NULL){
    pHelp->type = pCab->pIndexData->vnum;
    pHelp->level = HELP_CABAL;
    sendf(ch, "Help now avaliable only to cabal: %s\n\r", pCab->name);
    return TRUE;
  }
  else if ( (value = psalm_lookup( argument )) >= 0 ){
    pHelp->type = value;
    pHelp->level = HELP_PSALM;
    sendf(ch, "Help now avaliable only to holders of psalm: %s\n\r", psalm_table[value].name);
    return TRUE;
  }
  else if (!str_cmp( argument, "avatar") ){
    pHelp->type = -1;
    pHelp->level = HELP_RACE;
    sendf(ch, "Help now avaliable only to race: %s\n\r", "avatar");
    return TRUE;
  }
  else{
    send_to_char("Type must be a race, class, psalm, or cabal or \"none\".\n\r", ch);
    return FALSE;
  }
}


HEDIT( hedit_text )
{
    HELP_DATA *pHelp;
    EDIT_HELP(ch, pHelp);
    if ( argument[0] == '\0' ){
	string_append( ch, &pHelp->text );
	return TRUE;
    }
    send_to_char( "Syntax:  text    - line edit\n\r", ch );
    return FALSE;
}


void tedit( CHAR_DATA *ch, char *argument)
{
  TRAP_INDEX_DATA *pTrap;
  char arg[MIL];
  char command[MIL];
  int cmd;
  AREA_DATA *ad;

  smash_tilde(argument);
  strcpy(arg, argument);
  argument = one_argument( argument, command);

  EDIT_TRAP(ch, pTrap);

  if (pTrap){

    ad = get_vnum_area( pTrap->vnum );

    if ( ad == NULL ){
      edit_done(ch);
      return;
    }

    if ( !IS_BUILDER(ch, ad) ){
      send_to_char("TEdit: Insufficient security to modify code.\n\r", ch);
      edit_done(ch);
      return;
    }
  }

  if (command[0] == '\0'){
    tedit_show(ch, argument);
    return;
  }

  if (!str_cmp(command, "done") ){
    edit_done(ch);
    return;
  }

  for (cmd = 0; tedit_table[cmd].name != NULL; cmd++){
    if (!str_prefix(command, tedit_table[cmd].name) ){
      if ((*tedit_table[cmd].olc_fun) (ch, argument) && pTrap)
	if ((ad = get_vnum_area(pTrap->vnum)) != NULL)
	  SET_BIT(ad->area_flags, AREA_CHANGED);
      return;
    }
  }
  if (flag_value( trap_flags, command) != NO_FLAG){
    if (tedit_flag(ch, command) && pTrap){
      if ((ad = get_vnum_area(pTrap->vnum)) != NULL)
	SET_BIT(ad->area_flags, AREA_CHANGED);
    }
    return;
  }
  interpret(ch, arg);
  return;
}


bool set_trap_values( CHAR_DATA *ch, TRAP_INDEX_DATA *pTrap, int value_num, char *argument){
  switch( pTrap->type ){
  default:
  case TTYPE_DAMAGE:
    switch ( value_num ){
    default:
      do_help( ch, "TTYPE_DAMAGE" );
      return FALSE;
    case 0:
      send_to_char( "DAMAGE TYPE (weapon) SET.\n\r\n\r", ch );
      pTrap->value[0] = attack_lookup(argument);
      break;
    case 1:
      send_to_char( "BASE DAMAGE SET.\n\r\n\r", ch );
      pTrap->value[1] = atoi( argument );
      break;
    case 2:
      send_to_char( "DICE NUMER SET.\n\r\n\r", ch );
      pTrap->value[2] = atoi( argument );
      break;
    case 3:
      send_to_char( "DICE TYPE SET.\n\r\n\r", ch );
      pTrap->value[3] = atoi( argument );
      break;
    }
    break;
  case TTYPE_XDAMAGE:
    switch ( value_num ){
    default:
      do_help( ch, "TTYPE_XDAMAGE" );
      return FALSE;
    case 0:
      send_to_char( "DAMAGE verb (skill) SET.\n\r\n\r", ch );
	pTrap->value[0] = UMAX(0, skill_lookup(argument));
	break;
    case 1:
      send_to_char( "BASE DAMAGE SET.\n\r\n\r", ch );
      pTrap->value[1] = atoi( argument );
      break;
    case 2:
      send_to_char( "DICE NUMER SET.\n\r\n\r", ch );
      pTrap->value[2] = atoi( argument );
      break;
    case 3:
      send_to_char( "DICE TYPE SET.\n\r\n\r", ch );
      pTrap->value[3] = atoi( argument );
      break;
    case 4:
      send_to_char( "DAMAGE TYPE SET.\n\r\n\r", ch );
      pTrap->value[4] = UMAX(0, damtype_lookup( argument ));
      break;
    }
    break;
  case TTYPE_SPELL:
    switch ( value_num ){
    default:
      do_help( ch, "TTYPE_SPELL" );
      return FALSE;
    case 0:
      send_to_char( "FIRST SPELL SET.\n\r\n\r", ch );
      pTrap->value[0] = UMAX(0, skill_lookup(argument));
	break;
    case 1:
      send_to_char( "FIRST SPELL LEVEL MODIFIER SET.\n\r\n\r", ch );
      pTrap->value[1] = atoi( argument );
      break;
    case 2:
      send_to_char( "SECOND SPELL SET.\n\r\n\r", ch );
	pTrap->value[2] = UMAX(0, skill_lookup(argument));
	break;
    case 3:
      send_to_char( "SECOND SPELL LEVEL MODIFIER SET.\n\r\n\r", ch );
      pTrap->value[3] = atoi( argument );
      break;
    case 4:
      send_to_char( "DAMAGE TYPE SET.\n\r\n\r", ch );
      pTrap->value[4] = UMAX(0, damtype_lookup( argument ));
      break;
    }
    break;
  case TTYPE_MOB:
    switch ( value_num ){
    default:
      do_help( ch, "TTYPE_MOB" );
      return FALSE;
    case 0:
      send_to_char( "FIRST MOB SET.\n\r\n\r", ch );
	pTrap->value[0] = atoi(argument);
	break;
    case 1:
      send_to_char( "FIRST MOB HEALTH MODIFIER SET.\n\r\n\r", ch );
      pTrap->value[1] = atoi( argument );
      break;
    case 2:
      send_to_char( "SECOND MOB SET.\n\r\n\r", ch );
	pTrap->value[2] = atoi(argument);
	break;
    case 3:
      send_to_char( "SECOND MOB HEALTH MODIFIER SET.\n\r\n\r", ch );
      pTrap->value[3] = atoi( argument );
      break;
    case 4:
      send_to_char( "DURATION SET.\n\r\n\r", ch );
      pTrap->value[4] = atoi( argument );
      break;
    }
    break;
  case TTYPE_DUMMY:
    switch ( value_num ){
    default:
      do_help( ch, "TTYPE_DUMMY" );
      return FALSE;
    case 0:
    case 1:
    case 2:
    case 3:
      send_to_char( "TRAP RESET ROOM SET.\n\r\n\r", ch );
      pTrap->value[value_num] = atoi( argument );
      break;
    case 4:
      send_to_char( "DELAY ONLY SET.\n\r\n\r", ch );
      pTrap->value[4] = (!str_prefix( "yes", argument));
      break;
    }
    break;
  }
  show_trap_values(ch, pTrap);
  return TRUE;
}

TEDIT (tedit_create)
{
  TRAP_INDEX_DATA *pTrap;
  int value = atoi(argument);
  AREA_DATA *ad;

  if (IS_NULLSTR(argument) || value < 1){
    send_to_char( "Syntax : tedit create [vnum]\n\r", ch );
    return FALSE;
  }

  ad = get_vnum_area(value);

  if ( ad == NULL ){
    send_to_char( "TEdit : Vnum is not assigned an area.\n\r", ch );
    return FALSE;
  }

  if ( !IS_BUILDER(ch, ad) ){
    send_to_char("TEdit : Insufficient security to create MobProgs.\n\r", ch);
    return FALSE;
  }

  if ( get_trap_index(value) ){
    send_to_char("TEdit: Code vnum already exists.\n\r",ch);
    return FALSE;
  }

  pTrap			= new_trap_index();
  pTrap->area		= ad;
  pTrap->vnum		= value;

  pTrap->next		= trap_index_list;
  trap_index_list	= pTrap;
  ch->desc->pEdit	= (void *)pTrap;
  ch->desc->editor	= ED_TRAP;

  send_to_char("Trap Created.\n\r",ch);

  return TRUE;
}

TEDIT( tedit_show ){

  TRAP_INDEX_DATA *pTrap;
  char buf[MSL];
  EDIT_TRAP(ch, pTrap);

  sprintf( buf, "Area:  [%-5d] %s\n\r", pTrap->area->vnum, pTrap->area->name);
  send_to_char(buf, ch);
  sprintf( buf, "Vnum:  [%-5d]\n\r"\
	        "Name:  [%-10s]\n\r"\
	        "Type:  [%s]\n\r"\
	        "Echo:  %s\n\r"\
	        "oEcho:  %s\n\r",
	   pTrap->vnum,
	   pTrap->name,
	   trap_table[pTrap->type].name,
	   pTrap->echo,
	   pTrap->oEcho);
  send_to_char( buf, ch );
  sprintf( buf, "Level: [%-5d]\n\r", pTrap->level );
  send_to_char( buf, ch );
  sprintf( buf, "Flags: [%s]\n\r\n\r",
	   flag_string( trap_flags, pTrap->flags ) );
  send_to_char( buf, ch );
  show_trap_values(ch, pTrap);

  return FALSE;
}

TEDIT( tedit_level ){
  TRAP_INDEX_DATA *pTrap;
  int value = atoi( argument );
  EDIT_TRAP(ch, pTrap);

  if ( value < 1 ){
    send_to_char("Level must be greater then 0.\n\r", ch);
    return FALSE;
  }
  else
    pTrap->level = value;
  send_to_char("Trap level set.\n\r", ch);
  return TRUE;
}

TEDIT( tedit_name ){
  TRAP_INDEX_DATA *pTrap;
  EDIT_TRAP(ch, pTrap);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What name?\n\r", ch);
    return FALSE;
  }
  else{
    if (pTrap->name != NULL)
      free_string(pTrap->name);
    pTrap->name = str_dup( argument );
    send_to_char("Trap name set.\n\r", ch);
  }
  return TRUE;
}

TEDIT( tedit_echo ){
  TRAP_INDEX_DATA *pTrap;
  EDIT_TRAP(ch, pTrap);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What echo?\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp( "none", argument)){
    if (pTrap->echo != NULL)
      free_string(pTrap->echo);
    pTrap->echo = str_dup( "" );
    send_to_char("Echo cleared.\n\r", ch);
    return TRUE;
  }
  else{
    if (pTrap->echo != NULL)
      free_string(pTrap->echo);
    pTrap->echo = str_dup( capitalize(argument) );
    send_to_char("The following will be echoed to victim:\n\r", ch);
    act( pTrap->echo, ch, "\n\r", ch, TO_CHAR);
  }
  return TRUE;
}

TEDIT( tedit_oecho ){
  TRAP_INDEX_DATA *pTrap;
  EDIT_TRAP(ch, pTrap);

  if ( IS_NULLSTR(argument) ){
    send_to_char("What room echo?\n\r", ch);
    return FALSE;
  }
  else if (!str_cmp( "none", argument)){
    if (pTrap->oEcho != NULL)
      free_string(pTrap->oEcho);
    pTrap->oEcho = str_dup( "" );
    send_to_char("Room echo cleared.\n\r", ch);
    return TRUE;
  }
  else{
    if (pTrap->oEcho != NULL)
      free_string(pTrap->oEcho);
    pTrap->oEcho = str_dup( capitalize(argument) );
    send_to_char("The following will be echoed to the room:\n\r", ch);
    act( pTrap->oEcho, ch, "\n\r", NULL, TO_CHAR);
  }
  return TRUE;
}

TEDIT( tedit_type ){
  TRAP_INDEX_DATA *pTrap;
  int value = flag_lookup( argument, trap_table );
  int i = 0;
  EDIT_TRAP(ch, pTrap);

  if ( value < 1 ){
    send_to_char("Not a valid trap, use \"? trap\" for list.\n\r", ch);
    return FALSE;
  }
  else
    pTrap->type = value;
  send_to_char("Trap type set.\n\r", ch);
  for (i = 0; i < MAX_TRAPVAL; i++){
    if (pTrap->type == TTYPE_DUMMY)
      pTrap->value[i] = 0;
    else
      pTrap->value[i] = 1;
  }
  return TRUE;
}

TEDIT( tedit_v0 ){
  TRAP_INDEX_DATA *pTrap;
  EDIT_TRAP(ch, pTrap);

  return set_trap_values(ch, pTrap, 0, argument);
}

TEDIT( tedit_v1 ){
  TRAP_INDEX_DATA *pTrap;
  EDIT_TRAP(ch, pTrap);

  return set_trap_values(ch, pTrap, 1, argument);
}

TEDIT( tedit_v2 ){
  TRAP_INDEX_DATA *pTrap;
  EDIT_TRAP(ch, pTrap);

  return set_trap_values(ch, pTrap, 2, argument);
}

TEDIT( tedit_v3 ){
  TRAP_INDEX_DATA *pTrap;
  EDIT_TRAP(ch, pTrap);

  return set_trap_values(ch, pTrap, 3, argument);
}

TEDIT( tedit_v4 ){
  TRAP_INDEX_DATA *pTrap;
  EDIT_TRAP(ch, pTrap);

  return set_trap_values(ch, pTrap, 4, argument);
}

TEDIT( tedit_flag ){
  TRAP_INDEX_DATA *pTrap;
  int flag = flag_value( trap_flags, argument );
  EDIT_TRAP(ch, pTrap);

  if (flag == NO_FLAG){
    send_to_char("No such flag. Use \"? trap_flag\" for list.\n\r", ch);
    return FALSE;
  }
  pTrap->flags ^= flag;
  send_to_char("Flag toggled.\n\r", ch);
  return TRUE;
}

OEDIT( oedit_show )
{
    OBJ_INDEX_DATA *pObj;
    char buf[MSL];
    AFFECT_DATA *paf;
    PROG_LIST	*list;
    int cnt;
    EDIT_OBJ(ch, pObj);
    sprintf( buf, "Name:        [%s]\n\rArea:        [%5d] %s\n\r",
      pObj->name, !pObj->area ? -1        : pObj->area->vnum,
      !pObj->area ? "No Area" : pObj->area->name );
    send_to_char( buf, ch );
    sprintf( buf, "Vnum:        [%5d]\n\rType:        [%s]\n\r",
      pObj->vnum, flag_string( type_flags, pObj->item_type ) );
    send_to_char( buf, ch );
    sprintf( buf, "Level:       [%5d]\n\r", pObj->level );
    send_to_char( buf, ch );
    sprintf( buf, "Cabal Only:  [%5s]\n\r",
	     pObj->pCabal? pObj->pCabal->name : "None");
    send_to_char( buf, ch );
    sprintf( buf, "Class Only:  [%5s]   Race Only:  [%5s]\n\r",
	     pObj->class >= 0 ? class_table[pObj->class].name : "None",
	     pObj->race? race_table[pObj->race].name : "None");
    send_to_char( buf, ch );
    sprintf( buf, "Wear flags:  [%s]\n\r", flag_string( wear_flags, pObj->wear_flags ) );
    send_to_char( buf, ch );
    sprintf( buf, "Extra flags: [%s]\n\r", flag_string( extra_flags, pObj->extra_flags ) );
    send_to_char( buf, ch );
    sprintf( buf, "Material:    [%s]\n\r", pObj->material );
    send_to_char( buf, ch );
    sprintf( buf, "Weight:      [%5d]\n\rCost:        [%5d]\n\r%s:       [%5d]\n\r", pObj->weight, pObj->cost,
	     pObj->item_type == ITEM_PROJECTILE ? "Shots" : "Cond ", pObj->condition);
    send_to_char( buf, ch );
    if ( pObj->extra_descr )
    {
	EXTRA_DESCR_DATA *ed;
	send_to_char( "Ex desc kwd: ", ch );
	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    send_to_char( "[", ch );
	    send_to_char( ed->keyword, ch );
	    send_to_char( "]", ch );
	}
	send_to_char( "\n\r", ch );
    }
    sprintf( buf, "Short desc:  %s\n\rLong desc:\n\r     %s\n\r", pObj->short_descr, pObj->description );
    send_to_char( buf, ch );
    /* check on/off messages if any */
    if (pObj->message){
      if (!pObj->message->onself
	  && !pObj->message->offself
	  && !pObj->message->onother
	  && !pObj->message->offother)
	sendf( ch, "Messages:  None\n\r");
      else{
	sendf( ch, "Messages:\n\r");
	if (pObj->message->onself)
	  sendf( ch, " OnSelf: %s\n\r", pObj->message->onself);
	if (pObj->message->offself)
	  sendf( ch, " OffSelf: %s\n\r", pObj->message->offself);
	if (pObj->message->onother)
	  sendf( ch, " OnOther: %s\n\r", pObj->message->onother);
	if (pObj->message->offother)
	  sendf( ch, " OffOther: %s\n\r", pObj->message->offother);
      }
    }

    for ( cnt = 0, paf = pObj->affected; paf; paf = paf->next )
    {
	if ( cnt == 0 )
	{
	    send_to_char( "Number Modifier Affects\n\r", ch );
	    send_to_char( "------ -------- -------\n\r", ch );
	}
        sprintf( buf, "[%4d] %-8d %s\n\r",
		 cnt,
		 paf->modifier,
		 paf->where == TO_SKILL ? skill_table[paf->location].name : flag_string( apply_flags, paf->location ));
	send_to_char( buf, ch );
	cnt++;
    }
    show_obj_values( ch, pObj );
    if ( pObj->oprogs )
    {
	int cnt;

	sprintf(buf, "\n\rOBJPrograms for [%5d]:\n\r", pObj->vnum);
	send_to_char( buf, ch );

	for (cnt=0, list=pObj->oprogs; list; list=list->next)
	{
		if (cnt ==0)
		{
			send_to_char ( " Number Vnum Trigger Phrase\n\r", ch );
			send_to_char ( " ------ ---- ------- ------\n\r", ch );
		}

		sprintf(buf, "[%5d] %4d %7s %s\n\r", cnt,
			list->vnum,prog_type_to_name(list->trig_type),
			list->trig_phrase);
		send_to_char( buf, ch );
		cnt++;
	}
    }
    return FALSE;
}

OEDIT( oedit_addaffect )
{
    int value;
    int imod = 0;
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    char loc[MSL];
    char mod[MSL];
    EDIT_OBJ(ch, pObj);
    argument = one_argument( argument, loc );
    one_argument( argument, mod );
    if ( loc[0] == '\0' || mod[0] == '\0' || !is_number( mod ) )
    {
        send_to_char( "Syntax:  addaffect [location] [#xmod]\n\r"\
		      "         addaffect [skill] [#xmod]\n\r", ch );
	return FALSE;
    }

    /* we select between regular affects or skills etc.*/
    if ( ( value = flag_value( apply_flags, loc ) ) == NO_FLAG &&
	 (value = skill_lookup(loc)) > 0
	 && value < MAX_SKILL){
      if (get_trust(ch) < CREATOR){
	sendf(ch, "Requires level %d trust.\n\r", CREATOR);
	return FALSE;
      }
      pAf             =   new_affect();
      pAf->where      =   TO_SKILL;
    }
    else if ( ( value = flag_value( apply_flags, loc ) ) == NO_FLAG)
    {
        send_to_char( "Valid affects are skill names and:\n\r", ch );
	show_help( ch, "? affect" );
	return FALSE;
    }
    else{
      pAf             =   new_affect();
      pAf->where      =   TO_OBJECT;
    }
    imod = atoi( mod );
    /* set floor on % modifiers */
    if (value >= APPLY_WAIT_STATE && value <= APPLY_SPELL_COST
	&& imod < -90){
      imod = -90;
      sendf(ch, "Limiting lower bound to: %d.\n\r", imod);
    }

    pAf->location   =   value;
    pAf->modifier   =   imod;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->level      =   pObj->level;

    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;
    send_to_char( "Affect added.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_delaffect )
{
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    AFFECT_DATA *pAf_next;
    char affect[MSL];
    int  value;
    int  cnt = 0;
    EDIT_OBJ(ch, pObj);
    one_argument( argument, affect );
    if ( !is_number( affect ) || affect[0] == '\0' )
    {
	send_to_char( "Syntax:  delaffect [#xaffect]\n\r", ch );
	return FALSE;
    }
    value = atoi( affect );
    if ( value < 0 )
    {
	send_to_char( "Only non-negative affect-numbers allowed.\n\r", ch );
	return FALSE;
    }
    if ( !( pAf = pObj->affected ) )
    {
	send_to_char( "OEdit:  Non-existant affect.\n\r", ch );
	return FALSE;
    }
    if( value == 0 )
    {
	pAf = pObj->affected;
	pObj->affected = pAf->next;
	free_affect( pAf );
    }
    else
    {
	while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
	     pAf = pAf_next;
        if( pAf_next )
	{
	    pAf->next = pAf_next->next;
	    free_affect( pAf_next );
	}
        else
	{
	     send_to_char( "No such affect.\n\r", ch );
	     return FALSE;
	}
    }
    send_to_char( "Affect removed.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_name )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [string]\n\r", ch );
	return FALSE;
    }
    free_string( pObj->name );
    pObj->name = str_dup( argument );
    send_to_char( "Name set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_short )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  short [string]\n\r", ch );
	return FALSE;
    }
    free_string( pObj->short_descr );
    pObj->short_descr = str_dup( argument );
    /* For flexibility */
    if (get_trust(ch) < IMPLEMENTOR )
      pObj->short_descr[0] = LOWER( pObj->short_descr[0] );
    send_to_char( "Short description set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_long )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  long [string]\n\r", ch );
	return FALSE;
    }
    else if (!str_cmp(argument, "none")){
      if (!IS_NULLSTR(pObj->description)){
	free_string( pObj->description );
      }
      pObj->description = strdup( "" );
      return TRUE;
    }
    if (!IS_NULLSTR(pObj->description)){
      free_string( pObj->description );
    }
    pObj->description = str_dup( argument );
    pObj->description[0] = UPPER( pObj->description[0] );
    send_to_char( "Long description set.\n\r", ch);
    return TRUE;
}

bool set_value( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, char *argument, int value )
{
    if ( argument[0] == '\0' )
    {
        set_obj_values( ch, pObj, -1, "" );
	return FALSE;
    }
    if ( set_obj_values( ch, pObj, value, argument ) )
	return TRUE;
    return FALSE;
}

/* Finds the object and sets its value.       *
 * Called by The four valueX functions below. */
bool oedit_values( CHAR_DATA *ch, char *argument, int value )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);
    if ( set_value( ch, pObj, argument, value ) )
        return TRUE;
    return FALSE;
}

OEDIT( oedit_value0 )
{
    if ( oedit_values( ch, argument, 0 ) )
        return TRUE;
    return FALSE;
}

OEDIT( oedit_value1 )
{
    if ( oedit_values( ch, argument, 1 ) )
        return TRUE;
    return FALSE;
}

OEDIT( oedit_value2 )
{
    if ( oedit_values( ch, argument, 2 ) )
        return TRUE;
    return FALSE;
}

OEDIT( oedit_value3 )
{
    if ( oedit_values( ch, argument, 3 ) )
        return TRUE;
    return FALSE;
}

OEDIT( oedit_value4 )
{
    if ( oedit_values( ch, argument, 4 ) )
        return TRUE;
    return FALSE;
}

OEDIT( oedit_weight )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  weight [number]\n\r", ch );
	return FALSE;
    }
    pObj->weight = atoi( argument );
    send_to_char( "Weight set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_condition )
{
  OBJ_INDEX_DATA *pObj;
  EDIT_OBJ(ch, pObj);
  if ( argument[0] == '\0' || !is_number( argument ) ){
    send_to_char( "Syntax:  condition [number]\n\r", ch );
    return FALSE;
  }
  pObj->condition = URANGE(0, atoi( argument ), 100);
  send_to_char( "Condition set.\n\r", ch);
  return TRUE;
}

OEDIT( oedit_cost )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  cost [number]\n\r", ch );
	return FALSE;
    }
    pObj->cost = atoi( argument );
    send_to_char( "Cost set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_message )
{
    OBJ_INDEX_DATA *pObj;

    char arg1[MIL];

    EDIT_OBJ(ch, pObj);
    argument = one_argument(argument, arg1);
    //    argument = one_argument(argument, arg2);

    if (get_trust(ch) < AVATAR){
      sendf(ch, "Requires level %d trust.\n\r", AVATAR);
      return FALSE;
    }

    if (arg1[0] == '\0'){
      send_to_char("Syntax:  message [type] [message]\n\r"\
		   "Types: onself:  Shown to holder when equiped.\n\r"\
		   "	   offself: Shown to holder when removed.\n\r"\
		   "	   onother: Shown to room when equiped.\n\r"\
		   "	   offother:Shown to room when removed.\n\r"\
		   ",      message remove: Removes all messages\n\r", ch);
      return FALSE;
    }
    /* check for remove */
    if (!str_cmp("remove", arg1)){
      OBJ_MESSAGE_DATA *new_message;
      if (!pObj->message){
	send_to_char("No messages to remove.\n\r", ch);
	return FALSE;
      }
      if (pObj->message->onself){
	free_string(pObj->message->onself);
	pObj->message->onself = NULL;
      }
      if (pObj->message->offself){
	free_string(pObj->message->offself);
	pObj->message->offself = NULL;
      }
      if (pObj->message->onother){
	free_string(pObj->message->onother);
	pObj->message->onother = NULL;
      }
      if (pObj->message->offother){
	free_string(pObj->message->offother);
	pObj->message->offother = NULL;
      }
      free_mem(pObj->message, sizeof(*new_message));
      pObj->message = NULL;
      send_to_char("Message removed.\n\r", ch);
      return TRUE;
    }


    /* check for blank message */
    if (!pObj->message){
      OBJ_MESSAGE_DATA *new_message;
      new_message	= alloc_perm( sizeof( *new_message ) );
      pObj->message	= new_message;
      pObj->message->onself = str_dup("");
      pObj->message->offself = str_dup("");
      pObj->message->onother = str_dup("");
      pObj->message->offother = str_dup("");
    }
    if (!str_prefix("onself", arg1)){
      if (pObj->message->onself)
	free_string(pObj->message->onself);
      pObj->message->onself = str_dup(argument);
    }
    else if (!str_prefix("offself", arg1)){
      if (pObj->message->offself)
	free_string(pObj->message->offself);
      pObj->message->offself = str_dup(argument);
    }
    else if (!str_prefix("onother", arg1)){
      if (pObj->message->onother)
	free_string(pObj->message->onother);
      pObj->message->onother = str_dup(argument);
    }
    else if (!str_prefix("offother", arg1)){
      if (pObj->message->offother)
	free_string(pObj->message->offother);
      pObj->message->offother = str_dup(argument);
    }
    else{
      send_to_char("Invalid message type. Use \"message\" with not arguments for more info.\n\r", ch);
      return FALSE;
    }


    return TRUE;
}


OEDIT( oedit_cabal )
{
    OBJ_INDEX_DATA *pObj;
    CABAL_INDEX_DATA *pc;
    EDIT_OBJ(ch, pObj);

    if (get_trust(ch) < DEITY){
      sendf(ch, "Requires level %d trust.\n\r", DEITY);
      return FALSE;
    }
    else if (argument[0] && !str_prefix("none", argument)){
      pObj->pCabal = NULL;
      send_to_char("Cabal cleared.\n\r", ch);
      return TRUE;
    }
    else if ( (pc = get_cabal_index_str( argument )) == NULL){
      pObj->pCabal = NULL;
      send_to_char("Cabal cleared.\n\r", ch);
      return TRUE;
    }
    else if ( argument[0] == '\0' )
      {
	send_to_char( "Syntax:  cabal [name]\n\r", ch );
	return FALSE;
      }
    pObj->pCabal = pc;
    send_to_char( "Cabal set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_race )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);

    if (get_trust(ch) < DEITY){
      sendf(ch, "Requires level %d trust.\n\r", DEITY);
      return FALSE;
    }
    if (argument[0] && !str_prefix("none", argument)){
      pObj->race = 0;
      send_to_char("Race cleared.\n\r", ch);
      return TRUE;
    }

    if ( argument[0] == '\0' || !race_lookup(argument) )
    {
	send_to_char( "Syntax:  race [name]\n\r", ch );
	return FALSE;
    }

    pObj->race = race_lookup(argument);
    send_to_char( "Race set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_class )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);

    if (get_trust(ch) < DEITY){
      sendf(ch, "Requires level %d trust.\n\r", DEITY);
      return FALSE;
    }
    if (argument[0] && !str_prefix("none", argument)){
      pObj->class = -1;
      send_to_char("Class cleared.\n\r", ch);
      return TRUE;
    }

    if ( argument[0] == '\0' || class_lookup(argument) < 0 )
    {
	send_to_char( "Syntax:  class [name]\n\r", ch );
	return FALSE;
    }

    pObj->class = class_lookup(argument);
    send_to_char( "Class set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_create )
{
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    int  value;
    int  iHash;
    value = atoi( argument );
    if ( argument[0] == '\0' || value == 0 )
    {
	send_to_char( "Syntax:  oedit create [vnum]\n\r", ch );
	return FALSE;
    }
    pArea = get_vnum_area( value );
    if ( !pArea )
    {
	send_to_char( "OEdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }
    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "OEdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }
    if ( get_obj_index( value ) )
    {
	send_to_char( "OEdit:  Object vnum already exists.\n\r", ch );
	return FALSE;
    }
    pObj			= new_obj_index();
    pObj->vnum			= value;
    pObj->area			= pArea;
    if ( value > top_vnum_obj )
        top_vnum_obj = value;
    iHash			= value % MAX_KEY_HASH;
    pObj->next			= obj_index_hash[iHash];
    obj_index_hash[iHash]	= pObj;
    ch->desc->pEdit		= (void *)pObj;
    send_to_char( "Object Created.\n\r", ch );
    return TRUE;
}

OEDIT( oedit_ed )
{
    OBJ_INDEX_DATA *pObj;
    EXTRA_DESCR_DATA *ed;
    char command[MIL];
    char keyword[MIL];
    EDIT_OBJ(ch, pObj);
    argument = one_argument( argument, command );
    strcpy( keyword, argument );
    if ( command[0] == '\0' )
    {
	send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	send_to_char( "         ed delete [keyword]\n\r", ch );
	send_to_char( "         ed edit [keyword]\n\r", ch );
	send_to_char( "         ed format [keyword]\n\r", ch );
	return FALSE;
    }
    if ( !str_cmp( command, "add" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
	    return FALSE;
	}
	ed                  =   new_extra_descr();
	ed->keyword         =   str_dup( keyword );
	ed->next            =   pObj->extra_descr;
	pObj->extra_descr   =   ed;
	string_append( ch, &ed->description );
	return TRUE;
    }
    if ( !str_cmp( command, "edit" ) )
    {
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
	    return FALSE;
	}
	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	    if ( is_name( keyword, ed->keyword ) )
		break;
	if ( !ed )
	{
	    send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}
	string_append( ch, &ed->description );
	return TRUE;
    }
    if ( !str_cmp( command, "delete" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed delete [keyword]\n\r", ch );
	    return FALSE;
	}
	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	    ped = ed;
	}
	if ( !ed )
	{
	    send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
	    return FALSE;
	}
	if ( !ped )
	    pObj->extra_descr = ed->next;
	else
	    ped->next = ed->next;
	free_extra_descr( ed );
	send_to_char( "Extra description deleted.\n\r", ch );
	return TRUE;
    }
    if ( !str_cmp( command, "format" ) )
    {
	EXTRA_DESCR_DATA *ped = NULL;
	/* Useless conditional */
	if( ped != NULL )
        ped = NULL;
	if ( keyword[0] == '\0' )
	{
	    send_to_char( "Syntax:  ed format [keyword]\n\r", ch );
	    return FALSE;
	}
	for ( ed = pObj->extra_descr; ed; ed = ed->next )
	{
	    if ( is_name( keyword, ed->keyword ) )
		break;
	    ped = ed;
	}
	if ( !ed )
	{
            send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
            return FALSE;
	}
	ed->description = format_string( ed->description );
	send_to_char( "Extra description formatted.\n\r", ch );
	return TRUE;
    }
    oedit_ed( ch, "" );
    return FALSE;
}

/* ROM object functions */
OEDIT( oedit_extra )
{
    OBJ_INDEX_DATA *pObj;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_OBJ(ch, pObj);
	if ( ( value = flag_value( extra_flags, argument ) ) != NO_FLAG )
	{
	  if (value == flag_value ( extra_flags, "holdsrare")) {
	    if (get_trust(ch) < IMPLEMENTOR){
	      sendf(ch, "Requires level %d trust.\n\r", IMPLEMENTOR);
	      return FALSE;
	    }
	  }

	  TOGGLE_BIT(pObj->extra_flags, value);
	  send_to_char( "Extra flag toggled.\n\r", ch);
	  return TRUE;
	}
    }
    send_to_char( "Syntax:  extra [flag]\n\rType '? extra' for a list of flags.\n\r", ch );
    return FALSE;
}

OEDIT( oedit_wear )
{
    OBJ_INDEX_DATA *pObj;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_OBJ(ch, pObj);
	if ( ( value = flag_value( wear_flags, argument ) ) != NO_FLAG )
	{
	    TOGGLE_BIT(pObj->wear_flags, value);
	    send_to_char( "Wear flag toggled.\n\r", ch);
	    return TRUE;
	}
    }
    send_to_char( "Syntax:  wear [flag]\n\rType '? wear' for a list of flags.\n\r", ch );
    return FALSE;
}

OEDIT( oedit_type )
{
    OBJ_INDEX_DATA *pObj;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_OBJ(ch, pObj);
	if ( ( value = flag_value( type_flags, argument ) ) != NO_FLAG )
	{
	  pObj->item_type = value;
	  send_to_char( "Type set.\n\r", ch);
	  if (value == ITEM_STAFF){
	    pObj->value[0] = 0;
	    pObj->value[1] = 0;
	    pObj->value[2] = 0;
	    pObj->value[3] = -1;
	    pObj->value[4] = -1;
	  }
	  else if (value == ITEM_SCROLL
		   || value == ITEM_PILL
		   || value == ITEM_POTION){
	    pObj->value[0] = 0;
	    pObj->value[1] = -1;
	    pObj->value[2] = -1;
	    pObj->value[3] = -1;
	    pObj->value[4] = -1;
	  }
	  else if (value == ITEM_RANGED){
	    pObj->value[1] = 100;
	  }
	  else{
	    pObj->value[0] = 0;
	    pObj->value[1] = 0;
	    pObj->value[2] = 0;
	    pObj->value[3] = 0;
	    pObj->value[4] = 0;
	  }
	  return TRUE;
	}
    }
    send_to_char( "Syntax:  type [flag]\n\rType '? type' for a list of flags.\n\r", ch );
    return FALSE;
}

OEDIT( oedit_level )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  level [number]\n\r", ch );
	return FALSE;
    }
    pObj->level = atoi( argument );
    send_to_char( "Level set.\n\r", ch);
    return TRUE;
}

/* Mobile Editor Functions. */
MEDIT( medit_show )
{
    MOB_INDEX_DATA *pMob;
    PROG_LIST		*list;
    char buf[MSL];
    EDIT_MOB(ch, pMob);
    sprintf( buf, "Name:        [%s]\n\rArea:        [%5d] %s\n\r",
      pMob->player_name, !pMob->area ? -1        : pMob->area->vnum,
      !pMob->area ? "No Area" : pMob->area->name );
    send_to_char( buf, ch );
    sprintf( buf, "Act:         [%s]\n\r", flag_string( act_flags, pMob->act ) );
    send_to_char( buf, ch );
    sprintf( buf, "Act2:        [%s]\n\r", flag_string( act2_flags, pMob->act2 ) );
    send_to_char( buf, ch );
    sprintf( buf, "Cabal:       [%s]\n\r", pMob->pCabal ? pMob->pCabal->name : "None");
    send_to_char( buf, ch );
    sprintf( buf, "Vnum:        [%5d]\n\rSex:         [%s]\n\r",
      pMob->vnum, pMob->sex == SEX_MALE ? "male" : pMob->sex == SEX_FEMALE ? "female" : pMob->sex == 3 ? "random" : "neutral" );
    send_to_char( buf, ch );
    sprintf( buf, "Group:       [%5d]\n\r", pMob->group );
    send_to_char( buf, ch );
    sprintf( buf, "Race:        [%s]\n\r", race_table[pMob->race].name );
    send_to_char( buf, ch );
    sprintf( buf, "Level:       [%2d]\n\rAlign:       [%4d]\n\r", pMob->level, pMob->alignment );
    send_to_char( buf, ch );
    sprintf( buf, "Hitroll:     [%d]\n\r", pMob->hitroll );
    send_to_char( buf, ch );
    sprintf( buf, "Dam Type:    [%s]\n\r", attack_table[pMob->dam_type].name );
    send_to_char( buf, ch );
    sprintf( buf, "Hit dice:    [%2dd%-3d+%4d]\n\r",
      pMob->hit[DICE_NUMBER], pMob->hit[DICE_TYPE], pMob->hit[DICE_BONUS] );
    send_to_char( buf, ch );
    sprintf( buf, "Damage dice: [%2dd%-3d+%4d]\n\r",
      pMob->damage[DICE_NUMBER], pMob->damage[DICE_TYPE], pMob->damage[DICE_BONUS] );
    send_to_char( buf, ch );
    sprintf( buf, "Mana dice:   [%2dd%-3d+%4d]\n\r",
      pMob->mana[DICE_NUMBER], pMob->mana[DICE_TYPE], pMob->mana[DICE_BONUS] );
    send_to_char( buf, ch );
    sprintf( buf, "Affected by: [%s]\n\r", flag_string( affect_flags, pMob->affected_by ) );
    send_to_char( buf, ch );
    sprintf( buf, "Also Affected by: [%s]\n\r", flag_string( affect2_flags, pMob->affected2_by ) );
    send_to_char( buf, ch );
    sprintf( buf, "Armor:       [pierce: %d  bash: %d  slash: %d  magic: %d]\n\r",
      pMob->ac[AC_PIERCE], pMob->ac[AC_BASH], pMob->ac[AC_SLASH], pMob->ac[AC_EXOTIC] );
    send_to_char( buf, ch );
    sprintf( buf, "Imm:         [%s]\n\r", flag_string( imm_flags, pMob->imm_flags ) );
    send_to_char( buf, ch );
    sprintf( buf, "Res:         [%s]\n\r", flag_string( res_flags, pMob->res_flags ) );
    send_to_char( buf, ch );
    sprintf( buf, "Vuln:        [%s]\n\r", flag_string( vuln_flags, pMob->vuln_flags ) );
    send_to_char( buf, ch );
    sprintf( buf, "Off:         [%s]\n\r", flag_string( off_flags,  pMob->off_flags ) );
    send_to_char( buf, ch );
    sprintf( buf, "Size:        [%s]\n\r", flag_string( size_flags, pMob->size ) );
    send_to_char( buf, ch );
    sprintf( buf, "Start pos.   [%s]\n\r", flag_string( position_flags, pMob->start_pos ) );
    send_to_char( buf, ch );
    sprintf( buf, "Default pos  [%s]\n\r", flag_string( position_flags, pMob->default_pos ) );
    send_to_char( buf, ch );
    sprintf( buf, "Gold:        [%ld]\n\r", pMob->gold );
    send_to_char( buf, ch );
    if ( pMob->spec_fun )
    {
	sprintf( buf, "Spec fun:    [%s]\n\r",  spec_name( pMob->spec_fun ) );
	send_to_char( buf, ch );
    }
    sprintf( buf, "Short descr: %s\n\rLong descr:\n\r%s", pMob->short_descr, pMob->long_descr );
    send_to_char( buf, ch );
    sprintf( buf, "Description:\n\r%s", pMob->description );
    send_to_char( buf, ch );
    if ( pMob->pShop )
    {
	SHOP_DATA *pShop;
	int iTrade;
	pShop = pMob->pShop;
	sprintf( buf,
	  "Shop data for [%5d]:\n\r"
	  "  Markup for purchaser: %d%%\n\r"
	  "  Markdown for seller:  %d%%\n\r",
	    pShop->keeper, pShop->profit_buy, pShop->profit_sell );
	send_to_char( buf, ch );
        sprintf( buf, "  Hours: %d to %d.\n\r", pShop->open_hour, pShop->close_hour );
	send_to_char( buf, ch );
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    if ( pShop->buy_type[iTrade] != 0 )
	    {
                if ( iTrade == 0 )
                {
		    send_to_char( "  Number Trades Type\n\r", ch );
		    send_to_char( "  ------ -----------\n\r", ch );
		}
                sprintf( buf, "  [%4d] %s\n\r", iTrade, flag_string( type_flags, pShop->buy_type[iTrade] ) );
		send_to_char( buf, ch );
	    }
    }
    if ( pMob->mprogs )
    {
	int cnt;
	sprintf(buf, "\n\nMOBPrograms for [%5d]:\n\r", pMob->vnum);
	send_to_char( buf, ch );

	for (cnt=0, list=pMob->mprogs; list; list=list->next)
	{
		if (cnt ==0)
		{
			send_to_char ( " Number Vnum Trigger Phrase\n\r", ch );
			send_to_char ( " ------ ---- ------- ------\n\r", ch );
		}
		sprintf(buf, "[%5d] %4d %7s %s\n\r", cnt,
			list->vnum,prog_type_to_name(list->trig_type),
			list->trig_phrase);
		send_to_char( buf, ch );
		cnt++;
	}
    }
    return FALSE;
}

MEDIT( medit_create )
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    int  value;
    int  iHash;
    value = atoi( argument );
    if ( argument[0] == '\0' || value == 0 )
    {
	send_to_char( "Syntax:  medit create [vnum]\n\r", ch );
	return FALSE;
    }
    pArea = get_vnum_area( value );
    if ( !pArea )
    {
	send_to_char( "MEdit:  That vnum is not assigned an area.\n\r", ch );
	return FALSE;
    }
    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "MEdit:  Vnum in an area you cannot build in.\n\r", ch );
	return FALSE;
    }
    if ( get_mob_index( value ) )
    {
	send_to_char( "MEdit:  Mobile vnum already exists.\n\r", ch );
	return FALSE;
    }
    pMob			= new_mob_index();
    pMob->vnum			= value;
    pMob->area			= pArea;
    if ( value > top_vnum_mob )
	top_vnum_mob = value;
    pMob->act			= ACT_IS_NPC;
    pMob->act2			= 0;
    iHash			= value % MAX_KEY_HASH;
    pMob->next			= mob_index_hash[iHash];
    mob_index_hash[iHash]	= pMob;
    ch->desc->pEdit		= (void *)pMob;
    send_to_char( "Mobile Created.\n\r", ch );
    return TRUE;
}

MEDIT( medit_spec )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB(ch, pMob);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  spec [special function]\n\r", ch );
	return FALSE;
    }
    if ( !str_cmp( argument, "none" ) )
    {
        pMob->spec_fun = NULL;
        send_to_char( "Spec removed.\n\r", ch);
        return TRUE;
    }
    if ( spec_lookup( argument ) )
    {
	pMob->spec_fun = spec_lookup( argument );
	send_to_char( "Spec set.\n\r", ch);
	return TRUE;
    }
    send_to_char( "MEdit: No such special function.\n\r", ch );
    return FALSE;
}

MEDIT( medit_damtype )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB(ch, pMob);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  damtype [damage message]\n\r", ch );
        send_to_char( "For a list of damtypes, type '? weapon'.\n\r",ch);
	return FALSE;
    }
    pMob->dam_type = attack_lookup(argument);
    send_to_char( "Damage type set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_align )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB(ch, pMob);
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  alignment [number]\n\r", ch );
	return FALSE;
    }
    pMob->alignment = atoi( argument );
    send_to_char( "Alignment set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_level )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB(ch, pMob);
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  level [number]\n\r", ch );
	return FALSE;
    }
    pMob->level = atoi( argument );
    send_to_char( "Level set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_desc )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB(ch, pMob);
    if ( argument[0] == '\0' )
    {
	string_append( ch, &pMob->description );
	return TRUE;
    }
    send_to_char( "Syntax:  desc    - line edit\n\r", ch );
    return FALSE;
}

MEDIT( medit_long )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB(ch, pMob);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  long [string]\n\r", ch );
	return FALSE;
    }
    else if (!str_cmp(argument, "none")){
      if (!IS_NULLSTR(pMob->long_descr)){
	free_string( pMob->long_descr );
	pMob->long_descr = strdup( "" );
      }
      return TRUE;
    }
    free_string( pMob->long_descr );
    strcat( argument, "\n\r" );
    pMob->long_descr = str_dup( argument );
    pMob->long_descr[0] = UPPER( pMob->long_descr[0]  );
    send_to_char( "Long description set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_short )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB(ch, pMob);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  short [string]\n\r", ch );
	return FALSE;
    }
    free_string( pMob->short_descr );
    pMob->short_descr = str_dup( argument );
    send_to_char( "Short description set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_name )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB(ch, pMob);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  name [string]\n\r", ch );
	return FALSE;
    }
    free_string( pMob->player_name );
    pMob->player_name = str_dup( argument );
    send_to_char( "Name set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_shop )
{
    MOB_INDEX_DATA *pMob;
    char command[MIL];
    char arg1[MIL];
    argument = one_argument( argument, command );
    argument = one_argument( argument, arg1 );
    EDIT_MOB(ch, pMob);
    if ( command[0] == '\0' )
    {
	send_to_char( "Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch );
	send_to_char( "         shop profit [#xbuying%] [#xselling%]\n\r", ch );
	send_to_char( "         shop type [#x0-4] [item type]\n\r", ch );
	send_to_char( "         shop assign\n\r", ch );
	send_to_char( "         shop remove\n\r", ch );
	return FALSE;
    }
    if ( !str_cmp( command, "hours" ) )
    {
        if ( arg1[0] == '\0' || !is_number( arg1 ) || argument[0] == '\0' || !is_number( argument ) )
	{
	    send_to_char( "Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch );
	    return FALSE;
	}
	if ( !pMob->pShop )
	{
	    send_to_char("MEdit: Use shop assign.\n\r",ch);
	    return FALSE;
	}
	pMob->pShop->open_hour = atoi( arg1 );
	pMob->pShop->close_hour = atoi( argument );
	send_to_char( "Shop hours set.\n\r", ch);
	return TRUE;
    }
    if ( !str_cmp( command, "profit" ) )
    {
        if ( arg1[0] == '\0' || !is_number( arg1 ) || argument[0] == '\0' || !is_number( argument ) )
	{
	    send_to_char( "Syntax:  shop profit [#xbuying%] [#xselling%]\n\r", ch );
	    return FALSE;
	}
	if ( !pMob->pShop )
	{
	    send_to_char("MEdit: Use shop assign.\n\r",ch);
	    return FALSE;
	}
	pMob->pShop->profit_buy     = atoi( arg1 );
	pMob->pShop->profit_sell    = atoi( argument );
	send_to_char( "Shop profit set.\n\r", ch);
	return TRUE;
    }
    if ( !str_cmp( command, "type" ) )
    {
	char buf[MIL];
	int value;
        if ( arg1[0] == '\0' || !is_number( arg1 ) || argument[0] == '\0' )
        {
	    send_to_char( "Syntax:  shop type [#x0-4] [item type]\n\r", ch );
	    return FALSE;
	}
	if ( atoi( arg1 ) >= MAX_TRADE )
	{
	    sprintf( buf, "MEdit:  May sell %d items max.\n\r", MAX_TRADE );
	    send_to_char( buf, ch );
	    return FALSE;
	}
	if ( !pMob->pShop )
	{
	    send_to_char("MEdit: Use shop assign.\n\r", ch );
	    return FALSE;
	}
	if ( ( value = flag_value( type_flags, argument ) ) == NO_FLAG )
	{
	    send_to_char("MEdit: Use shop assign.\n\r", ch );
	    return FALSE;
	}
	pMob->pShop->buy_type[atoi( arg1 )] = value;
	send_to_char( "Shop type set.\n\r", ch);
	return TRUE;
    }
    if ( !str_prefix(command, "remove") )
    {
        SHOP_DATA *pShop;
        pShop           = pMob->pShop;

// Ath 11-27: Added to remove core dump when removing shop from non-shoppie
	if (pShop == NULL) {
	  send_to_char ("Mobile is not a shopkeeper!\n\r", ch);
	  return TRUE;
	}

        pMob->pShop     = NULL;

        if ( pShop == shop_first )
        {
            if ( !pShop->next )
            {
                shop_first = NULL;
                shop_last = NULL;
            }
            else
                shop_first = pShop->next;
        }
        else
        {
            SHOP_DATA *ipShop;
            for ( ipShop = shop_first; ipShop; ipShop = ipShop->next )
                if ( ipShop->next == pShop )
                {
                    if ( !pShop->next )
                    {
                        shop_last = ipShop;
                        shop_last->next = NULL;
                    }
                    else
                        ipShop->next = pShop->next;
                }
        }
	top_shop--;
        free_shop(pShop);
        send_to_char("Mobile is no longer a shopkeeper.\n\r", ch);
        return TRUE;
    }
    if ( !str_prefix(command, "assign") )
    {
        SHOP_DATA *pShop;
        pShop           = new_shop();
        pShop->keeper           = pMob->vnum;
        if ( pShop->keeper == 0 )
	{
	    free_shop(pShop);
            return FALSE;
	}
        pMob->pShop     = pShop;
        send_to_char("Mobile is now a shopkeeper.\n\r", ch);
        return TRUE;
    }
    medit_shop( ch, "" );
    return FALSE;
}

/* ROM medit functions: */
MEDIT( medit_sex )
{
    MOB_INDEX_DATA *pMob;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );
	if ( ( value = flag_value( sex_flags, argument ) ) != NO_FLAG )
	{
	    pMob->sex = value;
	    send_to_char( "Sex set.\n\r", ch);
	    return TRUE;
	}
    }
    send_to_char( "Syntax: sex [sex]\n\rType '? sex' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_cabal )
{
    MOB_INDEX_DATA *pMob;
    CABAL_INDEX_DATA *pc;

    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );
	if ( (pc = get_cabal_index_str( argument )) == NULL){
	  send_to_char("Cabal cleared.\n\r", ch);
	  pMob->pCabal = NULL;
	  return TRUE;
	}
	else{
	  pMob->pCabal = pc;
	  send_to_char( "Cabal set.\n\r", ch);
	  return TRUE;
	}
    }
    send_to_char( "Syntax: cabal [cabal_name]\n\r", ch );
    return FALSE;
}

MEDIT( medit_act )
{
    MOB_INDEX_DATA *pMob;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );
	if ( ( value = flag_value( act_flags, argument ) ) != NO_FLAG )
	{
	    pMob->act ^= value;
	    SET_BIT( pMob->act, ACT_IS_NPC );
	    send_to_char( "Act flag toggled.\n\r", ch);
	    return TRUE;
	}
    }
    send_to_char( "Syntax: act [flag]\n\rType '? act' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_act2 )
{
    MOB_INDEX_DATA *pMob;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );
	if ( ( value = flag_value( act2_flags, argument ) ) != NO_FLAG )
	{
	    pMob->act2 ^= value;
	    send_to_char( "Act2 flag toggled.\n\r", ch);
	    return TRUE;
	}
    }
    send_to_char( "Syntax: act2 [flag]\n\rType '? act2' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_affect )
{
    MOB_INDEX_DATA *pMob;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );
	if ( ( value = flag_value( affect_flags, argument ) ) != NO_FLAG )
	{
	    pMob->affected_by ^= value;
	    send_to_char( "Affect flag toggled.\n\r", ch);
	    return TRUE;
	}
    }
    send_to_char( "Syntax: affect1 [flag]\n\rType '? affect' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_affect2 )
{
    MOB_INDEX_DATA *pMob;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );
	if ( ( value = flag_value( affect2_flags, argument ) ) != NO_FLAG )
	{
	    pMob->affected2_by ^= value;
	    send_to_char( "Affect2 flag toggled.\n\r", ch);
	    return TRUE;
	}
    }
    send_to_char( "Syntax: affect2 [flag]\n\rType '? affect2' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_ac )
{
    MOB_INDEX_DATA *pMob;
    char arg[MIL];
    int pierce, bash, slash, exotic;
    do
    {
        if ( argument[0] == '\0' )
            break;
	EDIT_MOB(ch, pMob);
	argument = one_argument( argument, arg );
        if ( !is_number( arg ) )
            break;
	pierce = atoi( arg );
	argument = one_argument( argument, arg );
	if ( arg[0] != '\0' )
	{
            if ( !is_number( arg ) )
                break;
	    bash = atoi( arg );
	    argument = one_argument( argument, arg );
	}
	else
	    bash = pMob->ac[AC_BASH];
	if ( arg[0] != '\0' )
	{
            if ( !is_number( arg ) )
                break;
	    slash = atoi( arg );
	    argument = one_argument( argument, arg );
	}
	else
	    slash = pMob->ac[AC_SLASH];
	if ( arg[0] != '\0' )
	{
            if ( !is_number( arg ) )
                break;
	    exotic = atoi( arg );
	}
	else
	    exotic = pMob->ac[AC_EXOTIC];
	pMob->ac[AC_PIERCE] = pierce;
	pMob->ac[AC_BASH]   = bash;
	pMob->ac[AC_SLASH]  = slash;
	pMob->ac[AC_EXOTIC] = exotic;
	send_to_char( "Ac set.\n\r", ch );
	return TRUE;
    }
    while ( FALSE );
    send_to_char( "Syntax:  ac [ac-pierce [ac-bash [ac-slash [ac-exotic]]]]\n\r"
      "help MOB_AC  gives a list of reasonable ac-values.\n\r", ch );
    return FALSE;
}

MEDIT( medit_imm )
{
    MOB_INDEX_DATA *pMob;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );
	if ( ( value = flag_value( imm_flags, argument ) ) != NO_FLAG )
	{
	    pMob->imm_flags ^= value;
	    send_to_char( "Immunity toggled.\n\r", ch );
	    return TRUE;
	}
    }
    send_to_char( "Syntax: imm [flags]\n\rType '? imm' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_res )
{
    MOB_INDEX_DATA *pMob;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );
	if ( ( value = flag_value( res_flags, argument ) ) != NO_FLAG )
	{
	    pMob->res_flags ^= value;
	    send_to_char( "Resistance toggled.\n\r", ch );
	    return TRUE;
	}
    }
    send_to_char( "Syntax: res [flags]\n\rType '? res' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_vuln )
{
    MOB_INDEX_DATA *pMob;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );
	if ( ( value = flag_value( vuln_flags, argument ) ) != NO_FLAG )
	{
	    pMob->vuln_flags ^= value;
	    send_to_char( "Vulnerability toggled.\n\r", ch );
	    return TRUE;
	}
    }
    send_to_char( "Syntax: vuln [flags]\n\rType '? vuln' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_off )
{
    MOB_INDEX_DATA *pMob;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );
	if ( ( value = flag_value( off_flags, argument ) ) != NO_FLAG )
	{
	    pMob->off_flags ^= value;
	    send_to_char( "Offensive behaviour toggled.\n\r", ch );
	    return TRUE;
	}
    }
    send_to_char( "Syntax: off [flags]\n\rType '? off' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_size )
{
    MOB_INDEX_DATA *pMob;
    int value;
    if ( argument[0] != '\0' )
    {
	EDIT_MOB( ch, pMob );
	if ( ( value = flag_value( size_flags, argument ) ) != NO_FLAG )
	{
	    pMob->size = value;
	    send_to_char( "Size set.\n\r", ch );
	    return TRUE;
	}
    }
    send_to_char( "Syntax: size [size]\n\rType '? size' for a list of sizes.\n\r", ch );
    return FALSE;
}

MEDIT( medit_hitdice )
{
    static char syntax[] = "Syntax:  hitdice <number> d <type> + <bonus>\n\r";
    char *num, *type, *bonus, *cp;
    MOB_INDEX_DATA *pMob;
    EDIT_MOB( ch, pMob );
    if ( argument[0] == '\0' )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }
    num = cp = argument;
    while ( isdigit( *cp ) )
        ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )
        *(cp++) = '\0';
    type = cp;
    while ( isdigit( *cp ) )
        ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )
        *(cp++) = '\0';
    bonus = cp;
    while ( isdigit( *cp ) )
        ++cp;
    if ( *cp != '\0' )
        *cp = '\0';
    if ( ( !is_number( num   ) || atoi( num   ) < 1 )
    || ( !is_number( type  ) || atoi( type  ) < 1 )
    || ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }
    pMob->hit[DICE_NUMBER] = atoi( num   );
    pMob->hit[DICE_TYPE]   = atoi( type  );
    pMob->hit[DICE_BONUS]  = atoi( bonus );
    send_to_char( "Hitdice set.\n\r", ch );
    return TRUE;
}

MEDIT( medit_manadice )
{
    static char syntax[] = "Syntax:  manadice <number> d <type> + <bonus>\n\r";
    char *num, *type, *bonus, *cp;
    MOB_INDEX_DATA *pMob;
    EDIT_MOB( ch, pMob );
    if ( argument[0] == '\0' )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }
    num = cp = argument;
    while ( isdigit( *cp ) )
        ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )
        *(cp++) = '\0';
    type = cp;
    while ( isdigit( *cp ) )
        ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )
        *(cp++) = '\0';
    bonus = cp;
    while ( isdigit( *cp ) )
        ++cp;
    if ( *cp != '\0' )
        *cp = '\0';
    if ( !( is_number( num ) && is_number( type ) && is_number( bonus ) ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }
    if ( ( !is_number( num   ) || atoi( num   ) < 0 )
    || ( !is_number( type  ) || atoi( type  ) < 0 )
    || ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }
    pMob->mana[DICE_NUMBER] = atoi( num   );
    pMob->mana[DICE_TYPE]   = atoi( type  );
    pMob->mana[DICE_BONUS]  = atoi( bonus );
    send_to_char( "Manadice set.\n\r", ch );
    return TRUE;
}

MEDIT( medit_damdice )
{
    static char syntax[] = "Syntax:  damdice <number> d <type> + <bonus>\n\r";
    char *num, *type, *bonus, *cp;
    MOB_INDEX_DATA *pMob;
    EDIT_MOB( ch, pMob );
    if ( argument[0] == '\0' )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }
    num = cp = argument;
    while ( isdigit( *cp ) )
        ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )
        *(cp++) = '\0';
    type = cp;
    while ( isdigit( *cp ) )
        ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )
        *(cp++) = '\0';
    bonus = cp;
    while ( isdigit( *cp ) )
        ++cp;
    if ( *cp != '\0' )
        *cp = '\0';
    if ( !( is_number( num ) && is_number( type ) && is_number( bonus ) ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }
    if ( ( !is_number( num   ) || atoi( num   ) < 1 )
    || ( !is_number( type  ) || atoi( type  ) < 1 )
    || ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }
    pMob->damage[DICE_NUMBER] = atoi( num   );
    pMob->damage[DICE_TYPE]   = atoi( type  );
    pMob->damage[DICE_BONUS]  = atoi( bonus );
    send_to_char( "Damdice set.\n\r", ch );
    return TRUE;
}

MEDIT( medit_race )
{
    MOB_INDEX_DATA *pMob;
    int race;
    if ( argument[0] != '\0' && ( race = race_lookup( argument ) ) != -1 )
    {
	EDIT_MOB( ch, pMob );
	pMob->race = race;
        pMob->act         |= race_table[race].act;
        pMob->act2        |= race_table[race].act2;
        pMob->affected_by |= race_table[race].aff;
	pMob->off_flags   |= race_table[race].off;
	pMob->imm_flags   |= race_table[race].imm;
	pMob->res_flags   |= race_table[race].res;
	pMob->vuln_flags  |= race_table[race].vuln;
	send_to_char( "Race set.\n\r", ch );
	return TRUE;
    }
    if ( argument[0] == '?' )
    {
	char buf[MSL];
	send_to_char( "Available races are:", ch );
	for ( race = 0; race_table[race].name != NULL; race++ )
	{
	    if ( ( race % 3 ) == 0 )
		send_to_char( "\n\r", ch );
	    sprintf( buf, " %-15s", race_table[race].name );
	    send_to_char( buf, ch );
	}
	send_to_char( "\n\r", ch );
	return FALSE;
    }
    send_to_char( "Syntax:  race [race]\n\rType 'race ?' for a list of races.\n\r", ch );
    return FALSE;
}

MEDIT( medit_position )
{
    MOB_INDEX_DATA *pMob;
    char arg[MIL];
    int value;
    argument = one_argument( argument, arg );
    switch ( arg[0] )
    {
    default:
	break;
    case 'S':
    case 's':
	if ( str_prefix( arg, "start" ) )
	    break;
	if ( ( value = flag_value( position_flags, argument ) ) == NO_FLAG )
	    break;
	EDIT_MOB( ch, pMob );
	pMob->start_pos = value;
	send_to_char( "Start position set.\n\r", ch );
	return TRUE;
    case 'D':
    case 'd':
	if ( str_prefix( arg, "default" ) )
	    break;
	if ( ( value = flag_value( position_flags, argument ) ) == NO_FLAG )
	    break;
	EDIT_MOB( ch, pMob );
	pMob->default_pos = value;
	send_to_char( "Default position set.\n\r", ch );
	return TRUE;
    }
    send_to_char( "Syntax:  position [start/default] [position]\n\r"
      "Type '? position' for a list of positions.\n\r", ch );
    return FALSE;
}

MEDIT( medit_gold )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB(ch, pMob);
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        send_to_char( "Syntax:  gold [number]\n\r", ch );
	return FALSE;
    }
    pMob->gold = atoi( argument );
    send_to_char( "Gold set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_hitroll )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB(ch, pMob);
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  hitroll [number]\n\r", ch );
	return FALSE;
    }
    pMob->hitroll = atoi( argument );
    send_to_char( "Hitroll set.\n\r", ch);
    return TRUE;
}

void show_liqlist(CHAR_DATA *ch)
{
    int liq;
    BUFFER *buffer;
    char buf[MSL];
    buffer = new_buf();
    for ( liq = 0; liq_table[liq].liq_name != NULL; liq++)
    {
       if ( (liq % 21) == 0 )
           add_buf(buffer,"Name                 Color          Proof Full Thirst Food Ssize\n\r");
       sprintf(buf, "%-20s %-14s %5d %4d %6d %4d %5d\n\r",
         liq_table[liq].liq_name,liq_table[liq].liq_color,
         liq_table[liq].liq_affect[0],liq_table[liq].liq_affect[1],
         liq_table[liq].liq_affect[2],liq_table[liq].liq_affect[3],
         liq_table[liq].liq_affect[4] );
       add_buf(buffer,buf);
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
    return;
}

void show_damlist(CHAR_DATA *ch)
{
    int att;
    BUFFER *buffer;
    char buf[MSL];
    buffer = new_buf();
    for ( att = 0; attack_table[att].name != NULL; att++)
    {
       if ( (att % 21) == 0 )
           add_buf(buffer,"Name                 Noun\n\r");
       sprintf(buf, "%-20s %-20s\n\r", attack_table[att].name,attack_table[att].noun );
       add_buf(buffer,buf);
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
    return;
}

OEDIT( oedit_addapply )
{
    int value,bv,typ, imod;
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    char loc[MSL];
    char mod[MSL];
    char type[MSL];
    char bvector[MSL];
    EDIT_OBJ(ch, pObj);
    argument = one_argument( argument, type );
    argument = one_argument( argument, loc );
    argument = one_argument( argument, mod );
    one_argument( argument, bvector );
    if (get_trust(ch) < IMPLEMENTOR){
      sendf(ch, "Requires level %d trust.\n\r", IMPLEMENTOR);
      return FALSE;
    }
    if ( type[0] == '\0' || ( typ = flag_value( apply_types, type ) ) == NO_FLAG )
    {
        send_to_char( "Invalid apply type. Valid apply types are:\n\r", ch);
        show_help( ch, "apptype" );
        return FALSE;
    }
    if ( loc[0] == '\0' || ( value = flag_value( apply_flags, loc ) ) == NO_FLAG )
    {
        send_to_char( "Valid applys are:\n\r", ch );
        show_help( ch, "apply" );
        return FALSE;
    }
    if ( bvector[0] == '\0' || ( bv = flag_value( bitvector_type[typ].table, bvector ) ) == NO_FLAG )
    {
        send_to_char( "Invalid bitvector type.\n\r", ch );
        send_to_char( "Valid bitvector types are:\n\r", ch );
        show_help( ch, bitvector_type[typ].help );
        return FALSE;
    }
    if ( mod[0] == '\0' || !is_number( mod ) )
    {
        send_to_char( "Syntax:  addapply [type] [location] [#xmod] [bitvector]\n\r", ch );
        return FALSE;
    }
    imod = atoi( mod );
    pAf             =   new_affect();
    pAf->location   =   value;
    pAf->modifier   =   imod;
    pAf->where      =   apply_types[typ].bit;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   bv;
    pAf->level      =   pObj->level;
    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;
    send_to_char( "Apply added.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_material )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);
    if ( argument[0] == '\0' )
    {
	send_to_char( "Syntax:  material [name]\n\r", ch );
	return FALSE;
    }
    free_string( pObj->material );
    pObj->material = str_dup( argument );
    send_to_char( "Material set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_group )
{
    MOB_INDEX_DATA *pMob;
    MOB_INDEX_DATA *pMTemp;
    char arg[MSL];
    char buf[MSL];
    int temp;
    BUFFER *buffer;
    bool found = FALSE;
    EDIT_MOB(ch, pMob);
    if ( argument[0] == '\0' )
    {
       send_to_char( "Syntax: group [number]\n\r", ch);
       send_to_char( "        group show [number]\n\r", ch);
       return FALSE;
    }
    if (is_number(argument))
    {
       pMob->group = atoi(argument);
       send_to_char( "Group set.\n\r", ch );
       return TRUE;
    }
    argument = one_argument( argument, arg );
    if ( !str_cmp( arg, "show" ) && is_number( argument ) )
    {
        if (atoi(argument) == 0)
        {
            send_to_char( "Are you crazy?\n\r", ch);
            return FALSE;
        }
        buffer = new_buf ();
        for (temp = 0; temp < 65536; temp++)
        {
            pMTemp = get_mob_index(temp);
            if ( pMTemp && ( pMTemp->group == atoi(argument) ) )
            {
                found = TRUE;
                sprintf( buf, "[%5d] %s\n\r", pMTemp->vnum, pMTemp->player_name );
                add_buf( buffer, buf );
            }
        }
        if (found)
            page_to_char( buf_string(buffer), ch );
        else
            send_to_char( "No mobs in that group.\n\r", ch );
        free_buf( buffer );
        return FALSE;
    }
    return FALSE;
}

REDIT( redit_sector )
{
    ROOM_INDEX_DATA *pRoom;
    int value;
    char buf[MIL];
    EDIT_ROOM(ch, pRoom);
    if ( argument[0] != '\0' )
    {
	if ( ( value = flag_value( sector_flags, argument ) ) != NO_FLAG )
	{
	    pRoom->sector_type = value;
	    send_to_char( "Sector set.\n\r", ch);
	    strcpy( buf, pRoom->name);
	    format_room_name(pRoom, buf);
	    return TRUE;
	}
    }
    send_to_char( "Syntax: sector [sector]\n\rType '? sector' for a list of flags.\n\r", ch );
    return FALSE;
}

REDIT( redit_room )
{
    ROOM_INDEX_DATA *pRoom;
    int value;
    EDIT_ROOM(ch, pRoom);
    if ( argument[0] != '\0' )
    {
	if ( ( value = flag_value( room_flags, argument ) ) != NO_FLAG )
	{
	    TOGGLE_BIT(pRoom->room_flags, value);
	    send_to_char( "Room flag toggled.\n\r", ch);
	    return TRUE;
	}
	if ( ( value = flag_value( room_flags2, argument ) ) != NO_FLAG )
	{
	    TOGGLE_BIT(pRoom->room_flags2, value);
	    send_to_char( "Room2 flag toggled.\n\r", ch);
	    return TRUE;
	}
    }
    send_to_char( "Syntax: room [flag]\n\rType '? room' for a list of flags.\n\r", ch );
    return FALSE;
}

REDIT( redit_copy )
{
    ROOM_INDEX_DATA *pRoom;
    ROOM_INDEX_DATA *pRoom2;
    int vnum;
    if ( argument[0] == '\0' )
    {
        send_to_char("Syntax: copy <vnum> \n\r",ch);
        return FALSE;
    }
    if ( !is_number(argument) )
    {
        send_to_char("REdit: You must enter a number (vnum).\n\r",ch);
        return FALSE;
    }
    else
    {
        vnum = atoi(argument);
        if( !( pRoom2 = get_room_index(vnum) ) )
        {
	    send_to_char("REdit: That room does not exist.\n\r",ch);
	    return FALSE;
        }
    }
    EDIT_ROOM(ch, pRoom);
    clone_room( pRoom2, pRoom, FALSE );
    send_to_char( "Room info copied.\n\r", ch );
    return TRUE;
}

OEDIT( oedit_copy )
{
    OBJ_INDEX_DATA *pObj;
    OBJ_INDEX_DATA *pObj2;
    AFFECT_DATA* new, *paf;
    int vnum, i;
    if ( argument[0] == '\0' )
    {
        send_to_char("Syntax: copy <vnum> \n\r",ch);
        return FALSE;
    }
    if ( !is_number(argument) )
    {
        send_to_char("OEdit: You must enter a number (vnum).\n\r",ch);
        return FALSE;
    }
    else
    {
        vnum = atoi(argument);
        if( !( pObj2 = get_obj_index(vnum) ) )
        {
 	    send_to_char("OEdit: That object does not exist.\n\r",ch);
	    return FALSE;
        }
    }
    EDIT_OBJ(ch, pObj);
    free_string( pObj->name );
    pObj->name = str_dup( pObj2->name );
    pObj->item_type = pObj2->item_type;
    pObj->level = pObj2->level;
    pObj->wear_flags  = pObj2->wear_flags;
    pObj->extra_flags = pObj2->extra_flags;
    free_string( pObj->material );
    pObj->material = str_dup( pObj2->material );
    pObj->condition = pObj2->condition;
    pObj->weight = pObj2->weight;
    pObj->pCabal = pObj2->pCabal;
    pObj->cost   = pObj2->cost;
    free_string( pObj->short_descr );
    pObj->short_descr = str_dup( pObj2->short_descr );
    free_string( pObj->description );
    pObj->description = str_dup( pObj2->description );
    for (i = 0; i < 5; i++)
        pObj->value[i] = pObj2->value[i];

    /* clear existing affects */
    while (pObj->affected){
      new = pObj->affected->next;
      free_affect( pObj->affected);
      pObj->affected = new;
    }
    for (paf = pObj2->affected; paf; paf = paf->next){
      new = new_affect();
      memcpy( new, paf, sizeof( * new ));
      new->next = pObj->affected;
      pObj->affected = new;
    }
    send_to_char( "Object info copied.\n\r", ch );
    return TRUE;
}

MEDIT( medit_copy )
{
    MOB_INDEX_DATA *pMob;
    MOB_INDEX_DATA *pMob2;
    PROG_LIST *list, *pCur;
    int vnum;
    if ( argument[0] == '\0' )
    {
        send_to_char("Syntax: copy <vnum> \n\r",ch);
        return FALSE;
    }
    if ( !is_number(argument) )
    {
        send_to_char("MEdit: You must enter a number (vnum).\n\r",ch);
        return FALSE;
    }
    else
    {
        vnum = atoi(argument);
        if( !( pMob2 = get_mob_index(vnum) ) )
        {
	    send_to_char("MEdit: That mob does not exist.\n\r",ch);
	    return FALSE;
        }
    }
    EDIT_MOB(ch, pMob);
    free_string( pMob->player_name );
    pMob->player_name = str_dup( pMob2->player_name );
    pMob->new_format = pMob2->new_format;
    pMob->act = pMob2->act;
    pMob->act2 = pMob2->act2;
    pMob->affected_by = pMob2->affected_by;
    pMob->affected2_by = pMob2->affected2_by;
    pMob->sex = pMob2->sex;
    pMob->pCabal = pMob2->pCabal;
    pMob->race = pMob2->race;
    pMob->level = pMob2->level;
    pMob->alignment = pMob2->alignment;
    pMob->hitroll = pMob2->hitroll;
    pMob->dam_type = pMob2->dam_type;
    pMob->group = pMob2->group;
    pMob->hit[DICE_NUMBER] = pMob2->hit[DICE_NUMBER];
    pMob->hit[DICE_TYPE]   = pMob2->hit[DICE_TYPE];
    pMob->hit[DICE_BONUS]  = pMob2->hit[DICE_BONUS];
    pMob->damage[DICE_NUMBER] = pMob2->damage[DICE_NUMBER];
    pMob->damage[DICE_TYPE]   = pMob2->damage[DICE_TYPE];
    pMob->damage[DICE_BONUS]  = pMob2->damage[DICE_BONUS];
    pMob->mana[DICE_NUMBER] = pMob2->mana[DICE_NUMBER];
    pMob->mana[DICE_TYPE]   = pMob2->mana[DICE_TYPE];
    pMob->mana[DICE_BONUS]  = pMob2->mana[DICE_BONUS];
    pMob->ac[AC_PIERCE] = pMob2->ac[AC_PIERCE];
    pMob->ac[AC_BASH]   = pMob2->ac[AC_BASH];
    pMob->ac[AC_SLASH]  = pMob2->ac[AC_SLASH];
    pMob->ac[AC_EXOTIC] = pMob2->ac[AC_EXOTIC];
    pMob->form  = pMob2->form;
    pMob->parts = pMob2->parts;
    pMob->imm_flags  = pMob2->imm_flags;
    pMob->res_flags  = pMob2->res_flags;
    pMob->vuln_flags = pMob2->vuln_flags;
    pMob->off_flags  = pMob2->off_flags;
    pMob->size     = pMob2->size;
    pMob->start_pos   = pMob2->start_pos;
    pMob->default_pos = pMob2->default_pos;
    pMob->gold = pMob2->gold;
    pMob->spec_fun = pMob2->spec_fun;
    free_string( pMob->short_descr );
    pMob->short_descr = str_dup( pMob2->short_descr );
    free_string( pMob->long_descr );
    pMob->long_descr = str_dup( pMob2->long_descr   );
    free_string( pMob->description );
    pMob->description = str_dup( pMob2->description );

    //remove all existing progs
    while (pMob->mprogs){
      pCur = pMob->mprogs;
      pMob->mprogs = pMob->mprogs->next;
      free_mprog( pCur );
    }
    pMob->mprogs = NULL;

    //copy progs
    for (pCur = pMob2->mprogs; pCur; pCur = pCur->next){
      list                  = new_mprog();
      list->vnum            = pCur->vnum;
      list->trig_type       = pCur->trig_type;
      list->trig_phrase     = str_dup(pCur->trig_phrase);
      list->code            = pCur->code;
      list->next            = pMob->mprogs;
      pMob->mprogs          = list;
    }
    SET_BIT(pMob->mprog_flags, pMob2->mprog_flags);
    send_to_char( "Mob info copied.\n\r", ch );
    return FALSE;
}

MEDIT ( medit_addmprog )
{
  int value;
  MOB_INDEX_DATA* pMob;
  PROG_LIST *list;
  PROG_CODE *code;
  char trigger[MAX_STRING_LENGTH];
  char phrase[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];

  EDIT_MOB(ch, pMob);
  argument=one_argument(argument, num);
  argument=one_argument(argument, trigger);
  argument=one_argument(argument, phrase);

  if (!is_number(num) || trigger[0] =='\0' || phrase[0] =='\0' )
  {
        send_to_char("Syntax:   addmprog [vnum] [trigger] [phrase]\n\r",ch);
        return FALSE;
  }

  if ( (value = flag_value (mprog_flags, trigger) ) == NO_FLAG)
  {
        send_to_char("Valid flags are:\n\r",ch);
        show_help( ch, "mprog");
        return FALSE;
  }

  if ( ( code = get_prog_index (atoi(num), PRG_MPROG ) ) == NULL)
  {
        send_to_char("No such MOBProgram.\n\r",ch);
        return FALSE;
  }

  list                  = new_mprog();
  list->vnum            = atoi(num);
  list->trig_type       = value;
  list->trig_phrase     = str_dup(phrase);
  list->code            = code->code;
  SET_BIT(pMob->mprog_flags,value);
  list->next            = pMob->mprogs;
  pMob->mprogs          = list;

  send_to_char( "Mprog Added.\n\r",ch);
  return TRUE;
}

MEDIT ( medit_delmprog )
{
    MOB_INDEX_DATA *pMob;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char mprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_MOB(ch, pMob);

    one_argument( argument, mprog );
    if (!is_number( mprog ) || mprog[0] == '\0' )
    {
       send_to_char("Syntax:  delmprog [#mprog]\n\r",ch);
       return FALSE;
    }

    value = atoi ( mprog );

    if ( value < 0 )
    {
        send_to_char("Only non-negative mprog-numbers allowed.\n\r",ch);
        return FALSE;
    }

    if ( !(list = pMob->mprogs) )
    {
        send_to_char("MEdit:  Non existant mprog.\n\r",ch);
        return FALSE;
    }

    if ( value == 0 )
    {
	REMOVE_BIT(pMob->mprog_flags, pMob->mprogs->trig_type);
        list = pMob->mprogs;
        pMob->mprogs = list->next;
        free_mprog( list );
    }
    else
    {
        while ( (list_next = list->next) && (++cnt < value ) )
                list = list_next;

        if ( list_next )
        {
		REMOVE_BIT(pMob->mprog_flags, list_next->trig_type);
                list->next = list_next->next;
                free_mprog(list_next);
        }
        else
        {
                send_to_char("No such mprog.\n\r",ch);
                return FALSE;
        }
    }

    send_to_char("Mprog removed.\n\r", ch);
    return TRUE;
}

OEDIT ( oedit_addoprog )
{
  int value;
  OBJ_INDEX_DATA *pObj;
  PROG_LIST *list;
  PROG_CODE *code;
  char trigger[MAX_STRING_LENGTH];
  char phrase[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];

  EDIT_OBJ(ch, pObj);
  argument=one_argument(argument, num);
  argument=one_argument(argument, trigger);
  argument=one_argument(argument, phrase);

  if (!is_number(num) || trigger[0] =='\0' || phrase[0] =='\0' )
  {
        send_to_char("Syntax:   addoprog [vnum] [trigger] [phrase]\n\r",ch);
        return FALSE;
  }

  if ( (value = flag_value (oprog_flags, trigger) ) == NO_FLAG)
  {
        send_to_char("Valid flags are:\n\r",ch);
        show_help( ch, "oprog");
        return FALSE;
  }

  if ( ( code =get_prog_index (atoi(num), PRG_OPROG ) ) == NULL)
  {
        send_to_char("No such OBJProgram.\n\r",ch);
        return FALSE;
  }

  list                  = new_oprog();
  list->vnum            = atoi(num);
  list->trig_type       = value;
  list->trig_phrase     = str_dup(phrase);
  list->code            = code->code;
  SET_BIT(pObj->oprog_flags,value);
  list->next            = pObj->oprogs;
  pObj->oprogs          = list;

  send_to_char( "Oprog Added.\n\r",ch);
  return TRUE;
}

OEDIT ( oedit_deloprog )
{
    OBJ_INDEX_DATA *pObj;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char oprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_OBJ(ch, pObj);

    one_argument( argument, oprog );
    if (!is_number( oprog ) || oprog[0] == '\0' )
    {
       send_to_char("Syntax:  deloprog [#oprog]\n\r",ch);
       return FALSE;
    }

    value = atoi ( oprog );

    if ( value < 0 )
    {
        send_to_char("Only non-negative oprog-numbers allowed.\n\r",ch);
        return FALSE;
    }

    if ( !(list= pObj->oprogs) )
    {
        send_to_char("OEdit:  Non existant oprog.\n\r",ch);
        return FALSE;
    }

    if ( value == 0 )
    {
	REMOVE_BIT(pObj->oprog_flags, pObj->oprogs->trig_type);
        list = pObj->oprogs;
        pObj->oprogs = list->next;
        free_oprog( list );
    }
    else
    {
        while ( (list_next = list->next) && (++cnt < value ) )
                list = list_next;

        if ( list_next )
        {
		REMOVE_BIT(pObj->oprog_flags, list_next->trig_type);
                list->next = list_next->next;
                free_oprog(list_next);
        }
        else
        {
                send_to_char("No such oprog.\n\r",ch);
                return FALSE;
        }
    }

    send_to_char("Oprog removed.\n\r", ch);
    return TRUE;
}

REDIT ( redit_addrprog )
{
  int value;
  ROOM_INDEX_DATA *pRoom;
  PROG_LIST *list;
  PROG_CODE *code;
  char trigger[MAX_STRING_LENGTH];
  char phrase[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];

  EDIT_ROOM(ch, pRoom);
  argument=one_argument(argument, num);
  argument=one_argument(argument, trigger);
  argument=one_argument(argument, phrase);

  if (!is_number(num) || trigger[0] =='\0' || phrase[0] =='\0' )
  {
        send_to_char("Syntax:   addrprog [vnum] [trigger] [phrase]\n\r",ch);
        return FALSE;
  }

  if ( (value = flag_value (rprog_flags, trigger) ) == NO_FLAG)
  {
        send_to_char("Valid flags are:\n\r",ch);
        show_help( ch, "rprog");
        return FALSE;
  }

  if ( ( code =get_prog_index (atoi(num), PRG_RPROG ) ) == NULL)
  {
        send_to_char("No such ROOMProgram.\n\r",ch);
        return FALSE;
  }

  list                  = new_rprog();
  list->vnum            = atoi(num);
  list->trig_type       = value;
  list->trig_phrase     = str_dup(phrase);
  list->code            = code->code;
  SET_BIT(pRoom->rprog_flags,value);
  list->next            = pRoom->rprogs;
  pRoom->rprogs          = list;

  send_to_char( "Rprog Added.\n\r",ch);
  return TRUE;
}

REDIT ( redit_delrprog )
{
    ROOM_INDEX_DATA *pRoom;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char rprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_ROOM(ch, pRoom);

    one_argument( argument, rprog );
    if (!is_number( rprog ) || rprog[0] == '\0' )
    {
       send_to_char("Syntax:  delrprog [#rprog]\n\r",ch);
       return FALSE;
    }

    value = atoi ( rprog );

    if ( value < 0 )
    {
        send_to_char("Only non-negative rprog-numbers allowed.\n\r",ch);
        return FALSE;
    }

    if ( !(list= pRoom->rprogs) )
    {
        send_to_char("REdit:  Non existant rprog.\n\r",ch);
        return FALSE;
    }

    if ( value == 0 )
    {
	REMOVE_BIT(pRoom->rprog_flags, pRoom->rprogs->trig_type);
        list = pRoom->rprogs;
        pRoom->rprogs = list->next;
        free_rprog( list );
    }
    else
    {
        while ( (list_next = list->next) && (++cnt < value ) )
                list = list_next;

        if ( list_next )
        {
		REMOVE_BIT(pRoom->rprog_flags, list_next->trig_type);
                list->next = list_next->next;
                free_rprog(list_next);
        }
        else
        {
                send_to_char("No such rprog.\n\r",ch);
                return FALSE;
        }
    }

    send_to_char("Rprog removed.\n\r", ch);
    return TRUE;
}

/* ARMIES */
ARMEDIT( armedit_show ){
  ARMY_INDEX_DATA* pai;

  EDIT_ARMY( ch, pai );
  show_army_index( ch, pai );

  return FALSE;
}

ARMEDIT( armedit_create ){
  ARMY_INDEX_DATA *pai;
  AREA_DATA *pArea;
  int  value;

  value = atoi( argument );
  if ( IS_NULLSTR( argument ) || value < 1){
    send_to_char( "Syntax:  medit create [vnum]\n\r", ch );
    return FALSE;
  }

  pArea = get_vnum_area( value );
  if ( !pArea ){
    send_to_char( "ArmEdit:  That vnum is not assigned an area.\n\r", ch );
    return FALSE;
  }
  if ( !IS_BUILDER( ch, pArea ) ){
    send_to_char( "ArmEdit:  Vnum in an area you cannot build in.\n\r", ch );
    return FALSE;
  }
  if ( get_army_index( value ) ){
    send_to_char( "ArmEdit:  Mobile vnum already exists.\n\r", ch );
    return FALSE;
  }
  pai			= new_army_index();
  add_army_index( pai );
  pai->vnum		= value;
  pai->area		= pArea;
  ch->desc->pEdit	= (void *)pai;

  pai->noun = str_dup("Unit");
  pai->short_desc = str_dup("An army unit.");
  pai->long_desc = str_dup("An army unit is camped here.");
  pai->desc = str_dup("You see nothing special about them.");

  send_to_char( "Army Created.\n\r", ch );
  return TRUE;
}

ARMEDIT( armedit_type ){
  ARMY_INDEX_DATA *pai;
  int value;
  if ( !IS_NULLSTR(argument) ){
    EDIT_ARMY(ch, pai);
    if ( ( value = flag_lookup2( argument, army_types ) ) != NO_FLAG ){
      pai->type = value;
      sendf(ch, "Type now %s\n\r", flag_string( army_types, pai->type));
      return TRUE;
    }
    else
      send_to_char("No such type, use ""? army_types"" for list.\n\r", ch);
  }
  return TRUE;
}

ARMEDIT( armedit_cost ){
  ARMY_INDEX_DATA *pai;
  int value = 0;

  if (!IS_NULLSTR(argument) && (value = atoi(argument)) >= 0){
    EDIT_ARMY(ch, pai);
    pai->cost = value;
    sendf(ch, "Cost now %d cps.\n\r", pai->cost );
    return TRUE;
  }
  return FALSE;
}

ARMEDIT( armedit_support ){
  ARMY_INDEX_DATA *pai;
  int value = 0;

  if (!IS_NULLSTR(argument) && (value = atoi(argument)) >= 0){
    if (value > 50){
      send_to_char("You cannot set support cost to more then 50%\n\r", ch );
      return FALSE;
    }
    EDIT_ARMY(ch, pai);
    pai->support = value;
    sendf( ch,  "Support set to %d%% of amount gained per bastion in the area.\n\r", pai->support);
    return TRUE;
  }
  return FALSE;
}

ARMEDIT( armedit_flags ){
  ARMY_INDEX_DATA *pai;
  int value;

  if ( !IS_NULLSTR(argument) ){
    EDIT_ARMY(ch, pai);
    if ( ( value = flag_value( army_flags, argument ) ) != NO_FLAG ){
      pai->army_flags ^= value;
      send_to_char("Flag toggled.\n\r",ch);
      return TRUE;
    }
    else
      send_to_char("No such flags, use ""? army_flags"" for list.\n\r", ch);
  }
  return FALSE;
}


ARMEDIT( armedit_offense ){
  static char syntax[] = "Syntax:  offense die <number> d <type> + <bonus>\n\r";
  char *num, *type, *bonus, *cp;
  ARMY_INDEX_DATA *pai;

  EDIT_ARMY( ch, pai );

  if ( IS_NULLSTR(argument)){
    send_to_char( syntax, ch );
    return FALSE;
  }

  num = cp = argument;
  while ( isdigit( *cp ) )
    ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) )
    *(cp++) = '\0';
  type = cp;
  while ( isdigit( *cp ) )
    ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) )
    *(cp++) = '\0';
  bonus = cp;
  while ( isdigit( *cp ) )
    ++cp;
  if ( *cp != '\0' )
    *cp = '\0';
  if ( ( !is_number( num   ) || atoi( num   ) < 1 )
       || ( !is_number( type  ) || atoi( type  ) < 1 )
       || ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }
  pai->off_dice[DICE_NUMBER] = atoi( num   );
  pai->off_dice[DICE_TYPE]   = atoi( type  );
  pai->off_dice[DICE_BONUS]  = atoi( bonus );
  send_to_char( "Offense dice set.\n\r", ch );
  return TRUE;
}

ARMEDIT( armedit_hitpoint ){
  static char syntax[] = "Syntax:  hitpoint die <number> d <type> + <bonus>\n\r";
  char *num, *type, *bonus, *cp;
  ARMY_INDEX_DATA *pai;

  EDIT_ARMY( ch, pai );

  if ( IS_NULLSTR(argument)){
    send_to_char( syntax, ch );
    return FALSE;
  }

  num = cp = argument;
  while ( isdigit( *cp ) )
    ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) )
    *(cp++) = '\0';
  type = cp;
  while ( isdigit( *cp ) )
    ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) )
    *(cp++) = '\0';
  bonus = cp;
  while ( isdigit( *cp ) )
    ++cp;
  if ( *cp != '\0' )
    *cp = '\0';
  if ( ( !is_number( num   ) || atoi( num   ) < 1 )
       || ( !is_number( type  ) || atoi( type  ) < 1 )
       || ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }
  pai->hit_dice[DICE_NUMBER] = atoi( num   );
  pai->hit_dice[DICE_TYPE]   = atoi( type  );
  pai->hit_dice[DICE_BONUS]  = atoi( bonus );
  send_to_char( "Hitpoint dice set.\n\r", ch );
  return TRUE;
}

ARMEDIT( armedit_armor ){
  static char syntax[] = "Syntax:  armor die <number> d <type> + <bonus>\n\r";
  char *num, *type, *bonus, *cp;
  ARMY_INDEX_DATA *pai;

  EDIT_ARMY( ch, pai );

  if ( IS_NULLSTR(argument)){
    send_to_char( syntax, ch );
    return FALSE;
  }

  num = cp = argument;
  while ( isdigit( *cp ) )
    ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) )
    *(cp++) = '\0';
  type = cp;
  while ( isdigit( *cp ) )
    ++cp;
  while ( *cp != '\0' && !isdigit( *cp ) )
    *(cp++) = '\0';
  bonus = cp;
  while ( isdigit( *cp ) )
    ++cp;
  if ( *cp != '\0' )
    *cp = '\0';
  if ( ( !is_number( num   ) || atoi( num   ) < 1 )
       || ( !is_number( type  ) || atoi( type  ) < 1 )
       || ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
	send_to_char( syntax, ch );
	return FALSE;
    }
  pai->arm_dice[DICE_NUMBER] = atoi( num   );
  pai->arm_dice[DICE_TYPE]   = atoi( type  );
  pai->arm_dice[DICE_BONUS]  = atoi( bonus );
  send_to_char( "Armor dice set.\n\r", ch );
  return TRUE;
}


ARMEDIT( armedit_noun ){
  ARMY_INDEX_DATA *pai;
  EDIT_ARMY(ch, pai);

  if ( IS_NULLSTR(argument)){
    send_to_char( "Syntax:  noun [string]\n\r", ch );
    return FALSE;
  }
  else if (strchr(argument, ' ') != NULL){
    send_to_char("The nound must be a single word.\n\r", ch);
    return FALSE;
  }
  free_string( pai->noun);
  pai->noun = str_dup( argument );
  send_to_char( "Noun description set.\n\r", ch);
  return TRUE;
}

ARMEDIT( armedit_short ){
  ARMY_INDEX_DATA *pai;
  EDIT_ARMY(ch, pai);

  if ( IS_NULLSTR(argument)){
    send_to_char( "Syntax:  short [string]\n\r", ch );
    return FALSE;
  }
  free_string( pai->short_desc );
  pai->short_desc = str_dup( argument );
  send_to_char( "Short description set.\n\r", ch);
  return TRUE;
}


ARMEDIT( armedit_long ){
  ARMY_INDEX_DATA *pai;
  EDIT_ARMY(ch, pai);

  if ( IS_NULLSTR(argument)){
    send_to_char( "Syntax:  long [string]\n\r", ch );
    return FALSE;
  }
  free_string( pai->long_desc );
  pai->long_desc = str_dup( argument );
  send_to_char( "Long description set.\n\r", ch);
  return TRUE;
}

ARMEDIT( armedit_descr )
{
    ARMY_INDEX_DATA *pai;

    EDIT_ARMY(ch, pai);

    if ( IS_NULLSTR(argument)){
      string_append( ch, &pai->desc );
      return TRUE;
    }
    send_to_char( "Syntax:  desc    - line edit\n\r", ch );
    return FALSE;
}

ARMEDIT( armedit_copy ){
  ARMY_INDEX_DATA *pai;
  ARMY_INDEX_DATA *pai2;
  int vnum, i;

  if ( IS_NULLSTR(argument)){
    send_to_char("Syntax: copy <vnum> \n\r",ch);
    return FALSE;
  }
  if ( !is_number(argument) ){
    send_to_char("ArmEdit: You must enter a number (vnum).\n\r",ch);
    return FALSE;
  }
  else{
    vnum = atoi(argument);
    if( !( pai2 = get_army_index(vnum) ) ){
      send_to_char("ArmEdit: That army does not exist.\n\r",ch);
      return FALSE;
    }
  }
  EDIT_ARMY(ch, pai);

  pai->type		= pai2->type;
  pai->cost		= pai2->cost;
  pai->support		= pai2->support;
  pai->army_flags	= pai2->army_flags;

  for (i = 0; i <= DICE_BONUS; i++){
    pai->off_dice[i]	= pai2->off_dice[i];
    pai->arm_dice[i]	= pai2->arm_dice[i];
    pai->hit_dice[i]	= pai2->hit_dice[i];
  }

  free_string(pai->noun );
  free_string(pai->short_desc);
  free_string(pai->long_desc);
  free_string(pai->desc);

  pai->noun		= str_dup( pai2->noun );
  pai->short_desc	= str_dup( pai2->short_desc );
  pai->long_desc	= str_dup( pai2->long_desc );
  pai->desc		= str_dup( pai2->desc );
  return TRUE;
}
