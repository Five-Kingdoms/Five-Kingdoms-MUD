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
#include "recycle.h"
#include "tome.h"
#include "clan.h"

#define DIF(a,b) (~((~a)|(b)))

AREA_DATA *get_area_data	args( ( int vnum ) );


extern          int                     top_reset;
extern          int                     top_area;
extern          int                     top_exit;
extern          int                     top_ed;
extern          int                     top_room;
extern		int                     top_mprog_index;
extern		int                     top_oprog_index;
extern		int                     top_rprog_index;
extern		char			*string_space;
extern		char			*top_string;
PROG_CODE              *       mpcode_free;
PROG_CODE	       *       opcode_free;
PROG_CODE	       *       rpcode_free;

AREA_DATA		*	area_free;
EXTRA_DESCR_DATA	*	extra_descr_free;
EXIT_DATA		*	exit_free;
ROOM_INDEX_DATA		*	room_index_free;
OBJ_INDEX_DATA		*	obj_index_free;
SHOP_DATA		*	shop_free;
MOB_INDEX_DATA		*	mob_index_free;
RESET_DATA		*	reset_free;
HELP_DATA		*	help_free;
HELP_DATA		*	help_last;

void free_extra_descr( EXTRA_DESCR_DATA *pExtra );
void free_affect( AFFECT_DATA *af );


/* Executed from comm.c.  Minimizes compiling when changes are made. */
bool run_olc_editor( DESCRIPTOR_DATA *d )
{
    switch ( d->editor )
    {
    case ED_AREA:   aedit( d->character, d->incomm ); break;
    case ED_ROOM:   redit( d->character, d->incomm ); break;
    case ED_OBJECT: oedit( d->character, d->incomm ); break;
    case ED_MOBILE: medit( d->character, d->incomm ); break;
    case ED_ARMY:   armedit(d->character, d->incomm ); break;
    case ED_TRAP:   tedit( d->character, d->incomm ); break;
    case ED_HELP:   hedit( d->character, d->incomm ); break;
    case ED_CABAL:  cedit( d->character, d->incomm ); break;
    case ED_MPCODE: mpedit( d->character, d->incomm );break;
    case ED_OPCODE: opedit( d->character, d->incomm );break;
    case ED_RPCODE: rpedit( d->character, d->incomm );break;
    default:        return FALSE;
    }
    return TRUE;
}

char *olc_ed_name( CHAR_DATA *ch )
{
    static char buf[10];
    buf[0] = '\0';
    switch (ch->desc->editor)
    {
    case ED_AREA:   sprintf( buf, "AEdit" ); break;
    case ED_ROOM:   sprintf( buf, "REdit" ); break;
    case ED_TRAP:   sprintf( buf, "TEdit" ); break;
    case ED_HELP:   sprintf( buf, "HEdit" ); break;
    case ED_CABAL:  sprintf( buf, "CEdit" ); break;
    case ED_OBJECT: sprintf( buf, "OEdit" ); break;
    case ED_MOBILE: sprintf( buf, "MEdit" ); break;
    case ED_MPCODE:
    	sprintf( buf, "MPEdit" );
	break;
    case ED_OPCODE:
	sprintf( buf, "OPEdit" );
	break;
    case ED_RPCODE:
	sprintf( buf, "RPEdit" );
	break;
    default:        sprintf( buf, " " ); break;
    }
    return buf;
}

char *olc_ed_vnum( CHAR_DATA *ch )
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    OBJ_INDEX_DATA *pObj;
    MOB_INDEX_DATA *pMob;
    CABAL_INDEX_DATA *pCab;
    HELP_DATA *pHelp;
    PROG_CODE *pMprog;
    PROG_CODE *pOprog;
    PROG_CODE *pRprog;

    static char buf[10];
    buf[0] = '\0';
    switch ( ch->desc->editor )
    {
    case ED_AREA:   pArea = (AREA_DATA *)ch->desc->pEdit; sprintf( buf, "%d", pArea ? pArea->vnum : 0 ); break;
    case ED_ROOM:   pRoom = ch->in_room; sprintf( buf, "%d", pRoom ? pRoom->vnum : 0 ); break;
    case ED_OBJECT: pObj = (OBJ_INDEX_DATA *)ch->desc->pEdit; sprintf( buf, "%d", pObj ? pObj->vnum : 0 ); break;
    case ED_MOBILE:
      pMob = (MOB_INDEX_DATA *)ch->desc->pEdit;
      sprintf( buf, "%d", pMob ? pMob->vnum : 0 );
      break;
    case ED_CABAL:
      pCab = (CABAL_INDEX_DATA *)ch->desc->pEdit;
      sprintf( buf, "%d", pCab ? pCab->vnum : 0 );
      break;
    case ED_HELP:
      pHelp = (HELP_DATA *)ch->desc->pEdit;
      sprintf( buf, "%d", pHelp ? pHelp->vnum : 0 );
      break;
    case ED_MPCODE:
    	pMprog = (PROG_CODE *)ch->desc->pEdit;
    	sprintf( buf, "%d", pMprog ? pMprog->vnum : 0 );
	break;
    case ED_OPCODE:
	pOprog = (PROG_CODE *)ch->desc->pEdit;
	sprintf( buf, "%d", pOprog ? pOprog->vnum : 0 );
	break;
    case ED_RPCODE:
	pRprog = (PROG_CODE *)ch->desc->pEdit;
	sprintf( buf, "%d", pRprog ? pRprog->vnum : 0 );
	break;
    default:        sprintf( buf, " " ); break;
    }
    return buf;
}

/* Format up the commands from given table. *
 * Called by show_commands(olc_act.c).      */
void show_olc_cmds( CHAR_DATA *ch, const struct olc_cmd_type *olc_table )
{
    char buf  [ MSL ];
    char buf1 [ MSL ];
    int  cmd;
    int  col;
    buf1[0] = '\0';
    col = 0;
    for (cmd = 0; olc_table[cmd].name != NULL; cmd++)
    {
	sprintf( buf, "%-15.15s", olc_table[cmd].name );
	strcat( buf1, buf );
	if ( ++col % 5 == 0 )
	    strcat( buf1, "\n\r" );
    }
    if ( col % 5 != 0 )
	strcat( buf1, "\n\r" );
    send_to_char( buf1, ch );
    return;
}

/* Display all olc commands.   *
 * Called by olc interpreters. */
bool show_commands( CHAR_DATA *ch, char *argument )
{
    switch (ch->desc->editor)
    {
    case ED_AREA:   show_olc_cmds( ch, aedit_table ); break;
    case ED_ROOM:   show_olc_cmds( ch, redit_table ); break;
    case ED_TRAP:   show_olc_cmds( ch, tedit_table ); break;
    case ED_HELP:   show_olc_cmds( ch, hedit_table ); break;
    case ED_CABAL:  show_olc_cmds( ch, cedit_table ); break;
    case ED_OBJECT: show_olc_cmds( ch, oedit_table ); break;
    case ED_MOBILE: show_olc_cmds( ch, medit_table ); break;
    case ED_MPCODE:
      show_olc_cmds( ch, mpedit_table );
      break;
    case ED_OPCODE:
      show_olc_cmds( ch, opedit_table );
      break;
    case ED_RPCODE:
      show_olc_cmds( ch, rpedit_table );
      break;
    }
    return FALSE;
}

/* Interpreter Table */
const struct olc_cmd_type aedit_table[] =
{
/*  {   command		function	}, */
    {   "age",		aedit_age	},
    {   "bastion",      aedit_bastion	},
    {   "builder",      aedit_builder   },
    {   "commands",	show_commands	},
    {   "create",	aedit_create	},
    {   "filename",	aedit_file	},
    {   "name",		aedit_name	},
    {   "prefix",	aedit_prefix	},
    {	"reset",	aedit_reset	},
    {   "security",	aedit_security	},
    {	"show",		aedit_show	},
    {   "vnum",		aedit_vnum	},
    {   "lvnum",	aedit_lvnum	},
    {   "uvnum",	aedit_uvnum	},
    {   "credits",	aedit_credits	},
    {   "?",		show_help	},
    {   "version",	show_version	},
    {   "aflag",	aedit_flags	},
    {   "crime",	aedit_crime	},
    {   "startroom",	aedit_startroom	},
    {	NULL,		0,		}
};

const struct olc_cmd_type redit_table[] =
{
/*  {   command		function	}, */
    {   "commands",	show_commands	},
    {   "create",	redit_create	},
    {   "desc",		redit_desc	},
    {   "ndesc",	redit_ndesc	},
    {   "ed",		redit_ed	},
    {   "format",	redit_format	},
    {   "name",		redit_name	},
    {	"show",		redit_show	},
    {   "heal",		redit_heal	},
    {	"mana",		redit_mana	},
    {	"watch",	redit_watch	},
    {	"temp",		redit_temp	},
    {   "cabal",        redit_cabal     },
    {   "north",	redit_north	},
    {   "south",	redit_south	},
    {   "east",		redit_east	},
    {   "west",		redit_west	},
    {   "up",		redit_up	},
    {   "down",		redit_down	},
    {   "sector",	redit_sector	},
    {   "room",		redit_room	},
    /* New reset commands. */
    {	"mreset",	redit_mreset	},
    {	"oreset",	redit_oreset	},
    {	"treset",	redit_treset	},
    {	"mlist",	redit_mlist	},
    {	"rlist",	redit_rlist	},
    {	"olist",	redit_olist	},
    {	"mshow",	redit_mshow	},
    {	"oshow",	redit_oshow	},
    {   "?",		show_help	},
    {   "version",	show_version	},
    {   "copy",		redit_copy	},
    /* New prog commands */
    {	"addprog",	redit_addrprog	},
    {	"delprog",	redit_delrprog	},
    {	NULL,		0,		}
};

const struct olc_cmd_type oedit_table[] =
{
/*  {   command		function	}, */
    {   "addaffect",	oedit_addaffect	},
    {   "addapply",	oedit_addapply	},
    {   "commands",	show_commands	},
    {   "cost",		oedit_cost	},
    {   "create",	oedit_create	},
    {   "delaffect",	oedit_delaffect	},
    {   "ed",		oedit_ed	},
    {   "long",		oedit_long	},
    {   "name",		oedit_name	},
    {   "short",	oedit_short	},
    {	"show",		oedit_show	},
    {   "v0",		oedit_value0	},
    {   "v1",		oedit_value1	},
    {   "v2",		oedit_value2	},
    {   "v3",		oedit_value3	},
    {   "v4",           oedit_value4    },
    {   "weight",	oedit_weight	},
    {   "condition",	oedit_condition	},
    {   "shots",	oedit_condition	},
    {   "material",     oedit_material  },
    {   "extra",        oedit_extra     },
    {   "wear",         oedit_wear      },
    {   "type",         oedit_type      },
    {   "level",        oedit_level     },
    {   "?",		show_help	},
    {   "version",	show_version	},
    {   "copy",		oedit_copy	},
    {   "cabal",	oedit_cabal	},
    {   "race",		oedit_race	},
    {   "class",	oedit_class	},
    {   "message",	oedit_message	},
    /* new prog commands */
    {	"addprog",	oedit_addoprog	},
    {	"delprog",	oedit_deloprog	},

    {	NULL,		0,		}
};

const struct olc_cmd_type medit_table[] =
{
/*  {   command		function	}, */
    {   "alignment",	medit_align	},
    {   "cabal",        medit_cabal     },
    {   "commands",	show_commands	},
    {   "create",	medit_create	},
    {   "desc",		medit_desc	},
    {   "level",	medit_level	},
    {   "long",		medit_long	},
    {   "name",		medit_name	},
    {   "shop",		medit_shop	},
    {   "short",	medit_short	},
    {	"show",		medit_show	},
    {   "spec",		medit_spec	},
    {   "sex",          medit_sex       },
    {   "act",          medit_act       },
    {   "act2",         medit_act2      },
    {   "affect2",      medit_affect2   },
    {   "affect1",      medit_affect    },
    {   "armor",        medit_ac        },
    {   "imm",          medit_imm       },
    {   "res",          medit_res       },
    {   "vuln",         medit_vuln      },
    {   "off",          medit_off       },
    {   "size",         medit_size      },
    {   "hitdice",      medit_hitdice   },
    {   "manadice",     medit_manadice  },
    {   "damdice",      medit_damdice   },
    {   "race",         medit_race      },
    {   "position",     medit_position  },
    {   "gold",         medit_gold      },
    {   "hitroll",      medit_hitroll   },
    {	"damtype",	medit_damtype	},
    {	"group",	medit_group	},
    {   "?",		show_help	},
    {   "version",	show_version	},
    {   "copy",		medit_copy	},
    /* New prog commands */
    {	"addprog",	medit_addmprog	},
    {	"delprog",	medit_delmprog	},

    {	NULL,		0,		}
};

const struct olc_cmd_type armedit_table[] =
{
/*{   command		function	}, */
  {   "?",		show_help	},
  {   "create",		armedit_create	},
  {   "show",		armedit_show	},
  {   "type",		armedit_type	},
  {   "cost",		armedit_cost	},
  {   "support",	armedit_support	},
  {   "flags",		armedit_flags	},
  {   "offense",	armedit_offense	},
  {   "hitpoints",	armedit_hitpoint},
  {   "armor",		armedit_armor	},
  {   "noun",		armedit_noun	},
  {   "short",		armedit_short	},
  {   "long",		armedit_long	},
  {   "description",	armedit_descr	},
  {   "copy",		armedit_copy	},
  {	NULL,		0,		}
};

/* trap table */
const struct olc_cmd_type tedit_table[] ={
/*{	command		function	}, */
  {	"?",		show_help	},
  {	"create",	tedit_create	},
  {	"show",		tedit_show	},
  {	"name",		tedit_name	},
  {	"echo",		tedit_echo	},
  {	"oecho",	tedit_oecho	},
  {	"level",	tedit_level	},
  {	"type",		tedit_type	},
  {	"flag",		tedit_flag	},
  {	"v0",		tedit_v0	},
  {	"v1",		tedit_v1	},
  {	"v2",		tedit_v2	},
  {	"v3",		tedit_v3	},
  {	"v4",		tedit_v4	},
  {	NULL,		0,		}
};

/* help table */
const struct olc_cmd_type hedit_table[] ={
/*{	command		function	}, */
  {	"?",		show_help	},
  {	"create",	hedit_create	},
  {	"show",		hedit_show	},
  {	"keyword",	hedit_key	},
  {	"level",	hedit_level	},
  {	"type",		hedit_type	},
  {	"text",		hedit_text	},
  {	NULL,		0,		}
};

/* help table */
const struct olc_cmd_type cedit_table[] ={
/*{	command		function	}, */
  {	"?",		show_help	},
  {	"create",	cedit_create	},
  {	"show",		cedit_show	},
  {	"name",		cedit_name	},
  {	"filename",	cedit_file_name	},
  {	"msggate",	cedit_msggate	},
  {	"ongate",	cedit_ongate	},
  {	"offgate",	cedit_offgate	},
  {	"city",		cedit_city	},
  {	"enemy",	cedit_enemy	},
  {	"anchor",	cedit_anchor	},
  {	"guard",	cedit_guard	},
  {	"pit",		cedit_pit	},
  {	"whoname",	cedit_whoname	},
  {	"immortal",	cedit_imm	},
  {	"currency",	cedit_currency	},
  {	"clan",		cedit_clan	},
  {	"members",	cedit_members	},
  {	"maxrooms",	cedit_max_room	},
  {	"levels",	cedit_levels	},
  {	"rank",		cedit_rank	},
  {	"franks",	cedit_frank	},
  {	"mranks",	cedit_mrank	},
  {	"flag",		cedit_flag	},
  {	"progress",	cedit_progress	},
  {	"align",	cedit_align	},
  {	"ethos",	cedit_ethos	},
  {	"race",		cedit_race	},
  {	"class",	cedit_class	},
  {	"skill",	cedit_skill	},
  {	"vote",		cedit_vote	},
  {	"room",		cedit_room	},
  {	"avatar",	cedit_avatar	},
  {	"parent",	cedit_parent	},
  {	"army",		cedit_army	},
  {	"tower",	cedit_tower	},
  {	"prefix",	cedit_prefix	},
  {	"tax",		cedit_tax	},
  {	NULL,		0,		}
};

/* Returns pointer to area with given vnum. *
 * Called by do_aedit(olc.c).               */
AREA_DATA *get_area_data( int vnum )
{
    AREA_DATA *pArea;
    for (pArea = area_first; pArea; pArea = pArea->next )
        if (pArea->vnum == vnum)
            return pArea;
    return 0;
}
/* retusn area data by name */
AREA_DATA *get_area_data_str( char* name ){
  AREA_DATA *pArea;
  for (pArea = area_first; pArea; pArea = pArea->next )
    if (!str_cmp(pArea->name, name))
      return pArea;
  return 0;
}

/* Resets builder information on completion.   *
 * Called by aedit, redit, oedit, medit(olc.c) */
bool edit_done( CHAR_DATA *ch )
{
    ch->desc->pEdit = NULL;
    ch->desc->editor = 0;
    return FALSE;
}

/* Area Interpreter.   *
 * Called by do_aedit. */
void aedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    char command[MIL];
    char arg[MIL];
    int  cmd;
    int  value;
    EDIT_AREA(ch, pArea);
    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );
    if ( !IS_BUILDER( ch, pArea ) )
    {
        send_to_char( "AEdit:  Insufficient security to modify area.\n\r", ch );
	edit_done( ch );
	return;
    }
    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }
    if ( !IS_BUILDER( ch, pArea ) )
    {
	interpret( ch, arg );
	return;
    }
    if ( command[0] == '\0' )
    {
	aedit_show( ch, argument );
	return;
    }
    if ( ( value = flag_value( area_flags, command ) ) != NO_FLAG )
    {
	TOGGLE_BIT(pArea->area_flags, value);
	send_to_char( "Flag toggled.\n\r", ch );
	SET_BIT( pArea->area_flags, AREA_CHANGED );
	return;
    }
    for ( cmd = 0; aedit_table[cmd].name != NULL; cmd++ )
	if ( !str_prefix( command, aedit_table[cmd].name ) )
	{
	    if ( (*aedit_table[cmd].olc_fun) ( ch, argument ) )
	    {
		SET_BIT( pArea->area_flags, AREA_CHANGED );
		return;
	    }
	    else
		return;
	}
    interpret( ch, arg );
    return;
}

/* Room Interpreter    *
 * Called by do_redit. */
void redit( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom;
    AREA_DATA *pArea;
    char arg[MSL];
    char command[MIL];
    int  cmd;
    int  value;
    EDIT_ROOM(ch, pRoom);
    pArea = pRoom->area;
    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );
    if ( !IS_BUILDER( ch, pArea ) )
    {
        send_to_char( "REdit:  Insufficient security to modify room.\n\r", ch );
	edit_done( ch );
	return;
    }
    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }
    if ( !IS_BUILDER( ch, pArea ) )
    {
        interpret( ch, arg );
        return;
    }
    if ( command[0] == '\0' )
    {
	redit_show( ch, argument );
	return;
    }
    if ( ( value = flag_value( room_flags, command ) ) != NO_FLAG )
    {
        TOGGLE_BIT(pRoom->room_flags, value);
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Room flag toggled.\n\r", ch );
        return;
    }
    if ( ( value = flag_value( room_flags2, command ) ) != NO_FLAG )
    {
        TOGGLE_BIT(pRoom->room_flags2, value);
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Room flag toggled.\n\r", ch );
        return;
    }
    if ( ( value = flag_value( sector_flags, command ) ) != NO_FLAG )
    {
        pRoom->sector_type  = value;
        SET_BIT( pArea->area_flags, AREA_CHANGED );
        send_to_char( "Sector type set.\n\r", ch );
        return;
    }
    for ( cmd = 0; redit_table[cmd].name != NULL; cmd++ )
	if ( !str_prefix( command, redit_table[cmd].name ) )
	{
	    if ( (*redit_table[cmd].olc_fun) ( ch, argument ) )
	    {
		SET_BIT( pArea->area_flags, AREA_CHANGED );
		return;
	    }
	    else
		return;
	}
    interpret( ch, arg );
    return;
}

/* Object Interpreter *
 * Called by do_oedit. */
void oedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    OBJ_INDEX_DATA *pObj;
    char arg[MSL];
    char command[MIL];
    int  cmd;
    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );
    EDIT_OBJ(ch, pObj);
    pArea = pObj->area;
    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "OEdit: Insufficient security to modify area.\n\r", ch );
	edit_done( ch );
	return;
    }
    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }
    if ( !IS_BUILDER( ch, pArea ) )
    {
	interpret( ch, arg );
	return;
    }
    if ( command[0] == '\0' )
    {
	oedit_show( ch, argument );
	return;
    }
    for ( cmd = 0; oedit_table[cmd].name != NULL; cmd++ )
	if ( !str_prefix( command, oedit_table[cmd].name ) )
	{
	    if ( (*oedit_table[cmd].olc_fun) ( ch, argument ) )
	    {
		SET_BIT( pArea->area_flags, AREA_CHANGED );
		return;
	    }
	    else
		return;
	}
    interpret( ch, arg );
    return;
}

/* Mobile Interpreter. *
 * called by do_medit. */
void medit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    MOB_INDEX_DATA *pMob;
    char command[MIL];
    char arg[MSL];
    int  cmd;
    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );
    EDIT_MOB(ch, pMob);
    pArea = pMob->area;
    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "MEdit: Insufficient security to modify area.\n\r", ch );
	edit_done( ch );
	return;
    }
    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }
    if ( !IS_BUILDER( ch, pArea ) )
    {
	interpret( ch, arg );
	return;
    }
    if ( command[0] == '\0' )
    {
        medit_show( ch, argument );
        return;
    }
    for ( cmd = 0; medit_table[cmd].name != NULL; cmd++ )
	if ( !str_prefix( command, medit_table[cmd].name ) )
	{
	    if ( (*medit_table[cmd].olc_fun) ( ch, argument ) )
	    {
		SET_BIT( pArea->area_flags, AREA_CHANGED );
		return;
	    }
	    else
		return;
	}
    interpret( ch, arg );
    return;
}

/* Amry Interpreter. *
 * called by do_armedit. */
void armedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    ARMY_INDEX_DATA *pai;
    char command[MIL];
    char arg[MSL];
    int  cmd;
    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    EDIT_ARMY(ch, pai);
    pArea = pai->area;

    if ( !IS_BUILDER( ch, pArea ) )
    {
	send_to_char( "MEdit: Insufficient security to modify area.\n\r", ch );
	edit_done( ch );
	return;
    }
    if ( !str_cmp(command, "done") )
    {
	edit_done( ch );
	return;
    }
    if ( !IS_BUILDER( ch, pArea ) )
    {
	interpret( ch, arg );
	return;
    }
    if ( command[0] == '\0' )
    {
        armedit_show( ch, argument );
        return;
    }
    for ( cmd = 0; armedit_table[cmd].name != NULL; cmd++ )
      if ( !str_prefix( command, armedit_table[cmd].name ) ){
	if ( (*armedit_table[cmd].olc_fun) ( ch, argument ) ){
	  SET_BIT( pArea->area_flags, AREA_CHANGED );
	  return;
	}
	else
	  return;
      }
    interpret( ch, arg );
    return;
}

const struct editor_cmd_type editor_table[] =
{
/*  {   command		function	}, */
    {   "area",		do_aedit	},
    {   "room",		do_redit	},
    {   "object",	do_oedit	},
    {   "mobile",	do_medit	},
    {   "army",		do_armedit	},
    {   "trap",		do_tedit	},
    {   "help",		do_hedit	},
    {   "cabal",	do_cedit	},
    {   "mprog",	do_mpedit	},
    {   "rprog",	do_rpedit	},
    {   "oprog",	do_opedit	},
    {	NULL,		0,		}
};

/* Entry point for all editors. */
void do_olc( CHAR_DATA *ch, char *argument )
{
    char command[MIL];
    int  cmd;
    argument = one_argument( argument, command );
    if ( command[0] == '\0' )
    {
        do_help( ch, "olc" );
        return;
    }
    if ((mud_data.mudport == 6666) && (get_trust(ch) < 60)){
      send_to_char("Main port access limited to Implementor trust.\n\r", ch);
      return;
    }
    for ( cmd = 0; editor_table[cmd].name != NULL; cmd++ )
	if ( !str_prefix( command, editor_table[cmd].name ) )
	{
	    (*editor_table[cmd].do_fun) ( ch, argument );
	    return;
	}
    do_help( ch, "olc" );
    return;
}

/* Entry point for editing area_data. */
void do_aedit( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    int value;
    char value2[MSL];
    char arg[MSL];
    pArea = ch->in_room->area;
    argument = one_argument(argument,arg);
    if ( is_number( arg ) )
    {
	value = atoi( arg );
	if ( !( pArea = get_area_data( value ) ) )
	{
	    send_to_char( "That area vnum does not exist.\n\r", ch );
	    return;
	}
    }
    else if ( !str_cmp( arg, "create" ) )
    {
        if (!IS_NPC(ch) && (ch->pcdata->security < 9) )
        {
            send_to_char("Insufficient security to modify area.\n\r",ch);
            return;
        }
        argument            =   one_argument(argument,value2);
        value = atoi (value2);
        if (get_area_data(value) != NULL)
        {
            send_to_char("Esa area ya existe!",ch);
            return;
        }
        pArea               =   new_area();
        area_last->next     =   pArea;
        area_last           =   pArea;
        SET_BIT( pArea->area_flags, AREA_ADDED );
        send_to_char("Area created.\n\r",ch);
    }
    if (!IS_BUILDER(ch,pArea))
    {
        send_to_char("Insufficient security to modify area.\n\r",ch);
    	return;
    }
    ch->desc->pEdit = (void *)pArea;
    ch->desc->editor = ED_AREA;
    return;
}

/* Entry point for editing room_index_data. */
void do_redit( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoom, *pRoom2;
    char arg1[MSL];
    argument = one_argument( argument, arg1 );
    pRoom = ch->in_room;
    if ( !str_cmp( arg1, "reset" ) )
    {
	if ( !IS_BUILDER( ch, pRoom->area ) )
	{
            send_to_char("Insufficient security to modify room.\n\r",ch);
            return;
	}
	reset_room( pRoom );
	send_to_char( "Room reset.\n\r", ch );
	return;
    }
    else if ( !str_cmp( arg1, "create" ) )
    {
	if ( argument[0] == '\0' || atoi( argument ) == 0 )
	{
	    send_to_char( "Syntax:  edit room create [vnum]\n\r", ch );
	    return;
	}
	if ( redit_create( ch, argument ) )
	{
	    char_from_room( ch );
	    char_to_room( ch, ch->desc->pEdit );
	    SET_BIT( pRoom->area->area_flags, AREA_CHANGED );
	    pRoom = ch->in_room;
	}
    }
    else
    {
        pRoom2 = get_room_index(atoi(arg1));
        if ( (pRoom2 != NULL) && IS_BUILDER(ch,pRoom2->area) )
        {
            char_from_room( ch );
            char_to_room( ch, pRoom2 );
            pRoom = ch->in_room;
        }
        else if (atoi(arg1) != 0)
        {
            send_to_char("Insufficient security to modify room.\n\r",ch);
            return;
        }
    }
    if ( !IS_BUILDER( ch, pRoom->area ) )
    {
        send_to_char("Insufficient security to modify area.\n\r",ch);
       	return;
    }
    ch->desc->pEdit = (void *)pRoom;
    ch->desc->editor = ED_ROOM;
    return;
}

/* Entry point for editing obj_index_data. */
void do_oedit( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    char arg1[MSL];
    int value;
    if ( IS_NPC(ch) )
	return;
    argument = one_argument( argument, arg1 );
    if ( is_number( arg1 ) )
    {
	value = atoi( arg1 );
	if ( !( pObj = get_obj_index( value ) ) )
	{
	    send_to_char( "OEdit:  That vnum does not exist.\n\r", ch );
	    return;
	}
	if ( !IS_BUILDER( ch, pObj->area ) )
        {
            send_to_char("Insufficient security to modify objects.\n\r",ch);
            return;
        }
	ch->desc->pEdit = (void *)pObj;
	ch->desc->editor = ED_OBJECT;
	return;
    }
    else if ( !str_cmp( arg1, "create" ) )
    {
        value = atoi( argument );
        if ( argument[0] == '\0' || value == 0 )
        {
            send_to_char( "Syntax:  edit object create [vnum]\n\r", ch );
            return;
        }
        pArea = get_vnum_area( value );
        if ( !pArea )
        {
            send_to_char( "OEdit:  That vnum is not assigned an area.\n\r", ch );
            return;
        }
        if ( !IS_BUILDER( ch, pArea ) )
        {
            send_to_char("Insufficient security to modify objects.\n\r",ch);
            return;
        }
        if ( oedit_create( ch, argument ) )
        {
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            ch->desc->editor = ED_OBJECT;
        }
        return;
    }
    send_to_char( "OEdit:  There is no default object to edit.\n\r", ch );
    return;
}

/* Entry point for editing mob_index_data. */
void do_medit( CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    int value;
    char arg1[MSL];
    argument = one_argument( argument, arg1 );
    if ( is_number( arg1 ) )
    {
	value = atoi( arg1 );
	if ( !( pMob = get_mob_index( value ) ))
	{
	    send_to_char( "MEdit:  That vnum does not exist.\n\r", ch );
	    return;
	}
	if ( !IS_BUILDER( ch, pMob->area ) )
	{
            send_to_char("Insufficient security to modify mobs.\n\r",ch);
            return;
	}
	ch->desc->pEdit = (void *)pMob;
	ch->desc->editor = ED_MOBILE;
	return;
    }
    else if ( !str_cmp( arg1, "create" ) )
    {
        value = atoi( argument );
        if ( arg1[0] == '\0' || value == 0 )
        {
            send_to_char( "Syntax:  edit mobile create [vnum]\n\r", ch );
            return;
        }
        pArea = get_vnum_area( value );
        if ( !pArea )
        {
            send_to_char( "OEdit:  That vnum is not assigned an area.\n\r", ch );
            return;
        }
        if ( !IS_BUILDER( ch, pArea ) )
        {
            send_to_char( "Insufficient security to edit mobs.\n\r" , ch );
            return;
        }
        if ( medit_create( ch, argument ) )
        {
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            ch->desc->editor = ED_MOBILE;
        }
        return;
    }
    send_to_char( "MEdit:  There is no default mobile to edit.\n\r", ch );
    return;
}

/* Entry point for editing army_index_data. */
void do_armedit( CHAR_DATA *ch, char *argument )
{
    ARMY_INDEX_DATA *pai;
    AREA_DATA *pArea;
    int value;
    char arg1[MSL];
    argument = one_argument( argument, arg1 );
    if ( is_number( arg1 ) )
    {
	value = atoi( arg1 );
	if ( !( pai = get_army_index( value ) ))
	{
	    send_to_char( "ArmEdit:  That vnum does not exist.\n\r", ch );
	    return;
	}
	if ( !IS_BUILDER( ch, pai->area ) )
	{
            send_to_char("Insufficient security to modify armies.\n\r",ch);
            return;
	}
	ch->desc->pEdit = (void *)pai;
	ch->desc->editor = ED_ARMY;
	return;
    }
    else if ( !str_cmp( arg1, "create" ) )
    {
        value = atoi( argument );
        if ( arg1[0] == '\0' || value == 0 )
        {
            send_to_char( "Syntax:  edit area create [vnum]\n\r", ch );
            return;
        }
        pArea = get_vnum_area( value );
        if ( !pArea )
        {
            send_to_char( "OEdit:  That vnum is not assigned an area.\n\r", ch );
            return;
        }
        if ( !IS_BUILDER( ch, pArea ) )
        {
            send_to_char( "Insufficient security to edit mobs.\n\r" , ch );
            return;
        }
        if ( armedit_create( ch, argument ) )
        {
            SET_BIT( pArea->area_flags, AREA_CHANGED );
            ch->desc->editor = ED_ARMY;
        }
        return;
    }
    send_to_char( "ArmEdit:  There is no default mobile to edit.\n\r", ch );
    return;
}

void do_tedit(CHAR_DATA *ch, char *argument){
  TRAP_INDEX_DATA* pTrap;
  char command[MIL];

  argument = one_argument(argument, command);

  if( is_number(command) ){
    int vnum = atoi(command);
    AREA_DATA *ad;

    if ( (pTrap = get_trap_index(vnum)) == NULL ){
      send_to_char("TEdit : That vnum does not exist.\n\r", ch);
      return;
    }
    ad = get_vnum_area(vnum);

    if ( ad == NULL ){
      send_to_char( "TEdit : Vnum is not assigned an area.\n\r", ch );
      return;
    }

    if ( !IS_BUILDER(ch, ad) ){
      send_to_char("TEdit : Insufficient security to modify area.\n\r", ch );
      return;
    }

    ch->desc->pEdit		= (void *)pTrap;
    ch->desc->editor		= ED_TRAP;
    return;
  }

  if ( !str_cmp(command, "create") ){
    if (argument[0] == '\0'){
      send_to_char( "Syntax : tedit create [vnum]\n\r", ch );
      return;
    }
    tedit_create(ch, argument);
    return;
  }

  send_to_char( "Syntax : tedit [vnum]\n\r", ch );
  send_to_char( "         tedit create [vnum]\n\r", ch );

  return;
}


void do_hedit(CHAR_DATA *ch, char *argument){
  HELP_DATA* pHelp;
  char command[MIL];

  argument = one_argument(argument, command);

  if( is_number(command) ){
    int vnum = atoi(command);

    if ( (pHelp = get_help_index(vnum)) == NULL ){
      send_to_char("HEdit : That vnum does not exist.\n\r", ch);
      return;
    }

    ch->desc->pEdit		= (void *)pHelp;
    ch->desc->editor		= ED_HELP;
    return;
  }

  if ( !str_cmp(command, "create") ){
    hedit_create(ch, argument);
    return;
  }

  send_to_char( "Syntax : hedit [vnum]\n\r", ch );
  send_to_char( "         hedit create\n\r", ch );
  return;
}

void do_cedit(CHAR_DATA *ch, char *argument){
  CABAL_INDEX_DATA* pCab;
  char command[MIL];

  argument = one_argument(argument, command);

  if( is_number(command) ){
    int vnum = atoi(command);

    if ( (pCab = get_cabal_index(vnum)) == NULL ){
      send_to_char("CEdit : That vnum does not exist.\n\r", ch);
      return;
    }

    ch->desc->pEdit		= (void *)pCab;
    ch->desc->editor		= ED_CABAL;
    return;
  }

  if ( !str_cmp(command, "create") ){
    if (get_trust(ch) < IMPLEMENTOR){
      sendf(ch, "Requires level %d trust.\n\r", IMPLEMENTOR);
      return;
    }
    cedit_create(ch, argument);
    return;
  }

  send_to_char( "Syntax : cedit [vnum]\n\r", ch );
  send_to_char( "         cedit create\n\r", ch );
  return;
}




void display_resets( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA	*pRoom;
    RESET_DATA		*pReset;
    MOB_INDEX_DATA	*pMob = NULL;
    char 		buf   [ MSL ];
    char 		final [ MSL ];
    int 		iReset = 0;
    EDIT_ROOM(ch, pRoom);
    final[0]  = '\0';
    send_to_char (
      " No.  Loads    Description       Location         Vnum   Wo Ar Description\n\r"
      "==== ======== ============= =================== ======== ===== ===========\n\r", ch );
    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
    {
	OBJ_INDEX_DATA  *pObj;
	MOB_INDEX_DATA  *pMobIndex;
	OBJ_INDEX_DATA  *pObjIndex;
	OBJ_INDEX_DATA  *pObjToIndex;
	ROOM_INDEX_DATA *pRoomIndex;
	TRAP_INDEX_DATA *pTrap;
	final[0] = '\0';
	sprintf( final, "[%2d] ", ++iReset );
	switch ( pReset->command )
	{
	default:
	    sprintf( buf, "Bad reset command: %c.", pReset->command );
	    strcat( final, buf );
            break;
	case 'M':
	    if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Load Mobile - Bad Mob %d\n\r", pReset->arg1 );
                strcat( final, buf );
                continue;
	    }
	    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
                sprintf( buf, "Load Mobile - Bad Room %d\n\r", pReset->arg3 );
                strcat( final, buf );
                continue;
	    }
            pMob = pMobIndex;
            sprintf( buf, "M[%5d] %-13.13s in room             R[%5d] %2d-%2d %-15.15s\n\r",
              pReset->arg1, pMob->short_descr, pReset->arg3,
              pReset->arg2, pReset->arg4, pRoomIndex->name );
            strcat( final, buf );
	    {
		ROOM_INDEX_DATA *pRoomIndexPrev;
		pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
                if ( pRoomIndexPrev && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
                    final[5] = 'P';
	    }
	    break;
	case 'O':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Load Object - Bad Object %d\n\r", pReset->arg1 );
                strcat( final, buf );
                continue;
	    }
            pObj       = pObjIndex;
	    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
	    {
                sprintf( buf, "Load Object - Bad Room %d\n\r", pReset->arg3 );
                strcat( final, buf );
                continue;
	    }
            sprintf( buf, "O[%5d] %-13.13s in room             R[%5d]       %-15.15s\n\r",
              pReset->arg1, pObj->short_descr, pReset->arg3, pRoomIndex->name );
            strcat( final, buf );
	    break;
	case 'P':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Put Object - Bad Object %d\n\r", pReset->arg1 );
                strcat( final, buf );
                continue;
            }
            pObj       = pObjIndex;
	    if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
	    {
                sprintf( buf, "Put Object - Bad To Object %d\n\r", pReset->arg3 );
                strcat( final, buf );
                continue;
	    }
            sprintf( buf, "O[%5d] %-13.13s inside              O[%5d] %2d    %-15.15s\n\r",
              pReset->arg1, pObj->short_descr, pReset->arg3, pReset->arg4, pObjToIndex->short_descr );
            strcat( final, buf );
	    break;
	case 'G':
	case 'E':
	    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Give/Equip Object - Bad Object %d\n\r", pReset->arg1 );
                strcat( final, buf );
                continue;
            }
            pObj       = pObjIndex;
	    if ( !pMob )
	    {
                sprintf( buf, "Give/Equip Object - No Previous Mobile\n\r" );
                strcat( final, buf );
                break;
	    }
            sprintf( buf, "O[%5d] %-13.13s %-19.19s M[%5d]       %-15.15s\n\r",
		     pReset->arg1, pObj->short_descr,
		     (pReset->command == 'G') ? flag_string( wear_loc_strings, WEAR_NONE ) : flag_string( wear_loc_strings, pReset->arg3 ),
		     pMob->vnum, pMob->short_descr );
	    strcat( final, buf );
	    break;
	case 'D':
	    pRoomIndex = get_room_index( pReset->arg1 );
	    sprintf( buf, "R[%5d] %s door of %-19.19s reset to %s\n\r",
              pReset->arg1, capitalize( dir_name[ pReset->arg2 ] ),
              pRoomIndex->name, flag_string( door_resets, pReset->arg3 ) );
	    strcat( final, buf );
	    break;
	case 'R':
	    if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
	    {
                sprintf( buf, "Randomize Exits - Bad Room %d\n\r", pReset->arg1 );
		strcat( final, buf );
		continue;
	    }
            sprintf( buf, "R[%5d] Exits are randomized in %s\n\r", pReset->arg1, pRoomIndex->name );
	    strcat( final, buf );
	    break;
	case 'T':
	  if ( (pTrap = get_trap_index( pReset->arg1 )) == NULL){
	    sprintf( buf, "Load Trap - Bad Trap %d\n\r", pReset->arg1 );
	    strcat( final, buf );
	    continue;
	  }
	  if ( (pRoomIndex = get_room_index( pReset->arg3 )) == NULL ){
	    sprintf( buf, "Load Trap - Bad Room %d\n\r", pReset->arg3 );
	    strcat( final, buf );
	    continue;
	  }
	  sprintf( buf, "T[%5d] %-13.13s ",
		   pReset->arg1, pTrap->name);
	  strcat( final, buf );
	  if (pReset->arg2 == TRAP_ON_EXIT){
	    sprintf( buf, "on %s exit        R[%5d] lv[%2d]  %s\n\r",
		     dir_name[pReset->arg4], pReset->arg3,
		     pTrap->level,
		     trap_table[pTrap->type].name);
	    strcat( final, buf );
	  }
	  else if (pReset->arg2 == TRAP_ON_OBJ){
	    sprintf( buf, "on previous obj     R[%5d] lv[%2d]  %s\n\r",
		     pReset->arg3,
		     pTrap->level,
		     trap_table[pTrap->type].name);
	    strcat( final, buf );
	  }
	  else{
	    sprintf(buf, "TRAP TYPE ERROR \n\r");
	    strcat( final, buf );
	  }
	}
	send_to_char( final, ch );
    }
    return;
}

/* Inserts a new reset in the given index slot. *
 * Called by do_resets(olc.c).                  */
void add_reset( ROOM_INDEX_DATA *room, RESET_DATA *pReset, int index )
{
    RESET_DATA *reset;
    int iReset = 0;
    if ( !room->reset_first )
    {
	room->reset_first	= pReset;
	room->reset_last	= pReset;
	pReset->next		= NULL;
	return;
    }
    index--;
    if ( index == 0 )
    {
	pReset->next = room->reset_first;
	room->reset_first = pReset;
	return;
    }
    for ( reset = room->reset_first; reset->next; reset = reset->next )
	if ( ++iReset == index )
	    break;
    pReset->next	= reset->next;
    reset->next		= pReset;
    if ( !pReset->next )
	room->reset_last = pReset;
    SET_BIT( room->area->area_flags, AREA_CHANGED );
    return;
}

void do_resets( CHAR_DATA *ch, char *argument )
{
    char arg1[MIL];
    char arg2[MIL];
    char arg3[MIL];
    char arg4[MIL];
    char arg5[MIL];
    char arg6[MIL];
    char arg7[MIL];
    RESET_DATA *pReset = NULL;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    argument = one_argument( argument, arg5 );
    argument = one_argument( argument, arg6 );
    argument = one_argument( argument, arg7 );
    if ( !IS_BUILDER( ch, ch->in_room->area ) )
    {
        send_to_char( "Resets: Invalid security for editing this area.\n\r", ch );
	return;
    }
    if (mud_data.mudport != TEST_PORT && get_trust(ch) < MASTER){
      send_to_char("This command avaliable on test port only.\n\r", ch);
      return;
    }
    if ( arg1[0] == '\0' )
    {
	if ( ch->in_room->reset_first )
	{
            send_to_char("Resets: M = mobile, R = room, O = object, P = pet, S= shopkeeper T = trap\n\r", ch );
	    display_resets( ch );
	}
	else
	    send_to_char( "No resets in this room.\n\r", ch );
    }
    if ( is_number( arg1 ) )
    {
	ROOM_INDEX_DATA *pRoom = ch->in_room;
	if ( !str_cmp( arg2, "delete" ) )
	{
	    int insert_loc = atoi( arg1 );
	    if ( !ch->in_room->reset_first )
	    {
		send_to_char( "No resets in this area.\n\r", ch );
		return;
	    }
	    if ( insert_loc-1 <= 0 )
	    {
		pReset = pRoom->reset_first;
		pRoom->reset_first = pRoom->reset_first->next;
		if ( !pRoom->reset_first )
		    pRoom->reset_last = NULL;
	    }
	    else
	    {
		int iReset = 0;
		RESET_DATA *prev = NULL;
                for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
		{
		    if ( ++iReset == insert_loc )
			break;
		    prev = pReset;
		}
		if ( !pReset )
		{
		    send_to_char( "Reset not found.\n\r", ch );
		    return;
		}
		if ( prev )
		    prev->next = prev->next->next;
		else
		    pRoom->reset_first = pRoom->reset_first->next;
                for ( pRoom->reset_last = pRoom->reset_first; pRoom->reset_last->next; pRoom->reset_last = pRoom->reset_last->next );
	    }
	    free_reset_data( pReset );
	    send_to_char( "Reset deleted.\n\r", ch );
	    SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
	}
        else if ( (!str_cmp( arg2, "mob" ) && is_number( arg3 ))
		  || (!str_cmp( arg2, "obj" ) && is_number( arg3 ))
		  || (!str_cmp( arg2, "trap" ) && is_number( arg3 )) )
	  {
	    if ( !str_cmp( arg2, "trap" ) ){
	      int type = 0;
	      int exit = 0;
	      if (get_trap_index( is_number(arg3) ? atoi( arg3 ) : 1 ) == NULL){
		send_to_char("Trap doesn't exist.\n\r",ch);
		return;
	      }
	      if (IS_NULLSTR(arg4)){
		send_to_char("OBJ or EXIT trap?\n\r", ch);
		return;
	      }
	      if (!str_cmp("obj", arg4))
		type  = TRAP_ON_OBJ;
	      else if (!str_cmp("exit", arg4)){
		type  = TRAP_ON_EXIT;
		if ( (exit = dir_lookup(arg5)) < 0 || exit >= MAX_DOOR){
		  send_to_char("Invalid exit.\n\r", ch);
		  return;
		}
	      }
	      else{
		send_to_char("OBJ or EXIT trap?\n\r", ch);
		return;
	      }

	      pReset		= new_reset_data();
	      pReset->command	= 'T';
	      pReset->arg1	= atoi( arg3 );
	      pReset->arg2	= type;
	      pReset->arg3	= ch->in_room->vnum;
	      pReset->arg4	= exit;
	    }
	    else if ( !str_cmp( arg2, "mob" ) )
	      {
		if (get_mob_index( is_number(arg3) ? atoi( arg3 ) : 1 ) == NULL)
		  {
                    send_to_char("Mob doesn't exist.\n\r",ch);
		    return;
		  }
                pReset = new_reset_data();
                pReset->command = 'M';
		pReset->arg1    = atoi( arg3 );
                pReset->arg2    = is_number( arg4 ) ? atoi( arg4 ) : 1;
		pReset->arg3    = ch->in_room->vnum;
                pReset->arg4    = is_number( arg5 ) ? atoi( arg5 ) : 1;
	      }
	    else
	      if ( !str_cmp( arg2, "obj" ) )
		{
		  pReset = new_reset_data();
		  pReset->arg1    = atoi( arg3 );
		  if ( !str_prefix( arg4, "inside" ) )
		    {
		      OBJ_INDEX_DATA *temp;
		      temp = get_obj_index(is_number(arg5) ? atoi(arg5) : 1);
		      if ( ( temp->item_type != ITEM_CONTAINER ) && ( temp->item_type != ITEM_CORPSE_NPC ) )
			{
			  send_to_char( "Object 2 is not a container\n\r.", ch);
			  return;
			}
		      pReset->command = 'P';
		      pReset->arg2    = is_number( arg6 ) ? atoi( arg6 ) : 1;
		      pReset->arg3    = is_number( arg5 ) ? atoi( arg5 ) : 1;
		      pReset->arg4    = is_number( arg7 ) ? atoi( arg7 ) : 1;
		    }
		  else if ( !str_cmp( arg4, "room" ) )
		    {
		      if (get_obj_index(atoi(arg3)) == NULL)
			{
			  send_to_char( "Vnum doesn't exist.\n\r",ch);
			  return;
			}
		      pReset->command  = 'O';
		      pReset->arg2     = 0;
		      pReset->arg3     = ch->in_room->vnum;
		      pReset->arg4     = 0;
		    }
		  else
		    {
		      if ( flag_value( wear_loc_flags, arg4 ) == NO_FLAG )
			{
			  send_to_char( "Resets: '? wear-loc'\n\r", ch );
			  return;
		    }
		    if (get_obj_index(atoi(arg3)) == NULL)
		      {
                        send_to_char( "Vnum no existe.\n\r",ch);
                        return;
		      }
		    pReset->arg1 = atoi(arg3);
		    pReset->arg3 = flag_value( wear_loc_flags, arg4 );
		    if ( pReset->arg3 == WEAR_NONE )
		      pReset->command = 'G';
		    else
		      pReset->command = 'E';
		    }
		}
	    add_reset( ch->in_room, pReset, atoi( arg1 ) );
            SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
            send_to_char( "Reset added.\n\r", ch );
	  }
        else if (!str_cmp( arg2, "random") && is_number(arg3))
        {
                if (atoi(arg3) < 1 || atoi(arg3) > 6)
                        {
                                send_to_char("Invalid argument.\n\r", ch);
                                return;
                        }
                pReset = new_reset_data ();
                pReset->command = 'R';
                pReset->arg1 = ch->in_room->vnum;
                pReset->arg2 = atoi(arg3);
                add_reset( ch->in_room, pReset, atoi( arg1 ) );
                SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
                send_to_char( "Random exits reset added.\n\r", ch);

        }
	else
	{
	send_to_char( "Syntax: RESET <number> OBJ <vnum> <wear_loc>\n\r", ch );
        send_to_char( "        RESET <number> OBJ <vnum> inside <vnum> [limit] [count]\n\r", ch );
	send_to_char( "        RESET <number> OBJ <vnum> room\n\r", ch );
        send_to_char( "        RESET <number> MOB <vnum> [max # area] [max # room]\n\r", ch );
        send_to_char( "        RESET <number> TRAP <vnum> OBJ\n\r", ch );
        send_to_char( "        RESET <number> TRAP <vnum> EXIT <dir>\n\r", ch );
	send_to_char( "        RESET <number> DELETE\n\r", ch );
        send_to_char( "        RESET <number> RANDOM [# exits]\n\r", ch );
	}

    }
    else
    {
	send_to_char( "Syntax: RESET <number> OBJ <vnum> <wear_loc>\n\r", ch );
        send_to_char( "        RESET <number> OBJ <vnum> inside <vnum> [limit] [count]\n\r", ch );
	send_to_char( "        RESET <number> OBJ <vnum> room\n\r", ch );
        send_to_char( "        RESET <number> MOB <vnum> [max # area] [max # room]\n\r", ch );
        send_to_char( "        RESET <number> TRAP <vnum> OBJ\n\r", ch );
        send_to_char( "        RESET <number> TRAP <vnum> EXIT <dir>\n\r", ch );
	send_to_char( "        RESET <number> DELETE\n\r", ch );
        send_to_char( "        RESET <number> RANDOM [# exits]\n\r", ch );
    }
    return;
}


/* Normal command to list areas and display area information. *
 * Called by interpreter(interp.c)                            */
void do_alist( CHAR_DATA *ch, char *argument )
{
  BUFFER* buffer;
  char buf    [ MSL ];
  AREA_DATA *pArea;
  buffer = new_buf();

  sprintf( buf, "[%3s] [%4s] [%-20s] (%-5s-%5s) [%-10s] %3s [%-10s]\n\r",
	   "Num", "Idle", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders" );
  add_buf(buffer, buf);

  for ( pArea = area_first; pArea; pArea = pArea->next ){
    sprintf( buf, "[%3d] [%4d] %-22.22s (%-5d-%5d) %-12.12s [%d] [%-10.10s]\n\r",
	     pArea->vnum, pArea->idle, pArea->name, pArea->min_vnum, pArea->max_vnum,
	     pArea->file_name, pArea->security, pArea->builders );
    add_buf(buffer, buf);
    }
  page_to_char(buf_string(buffer),ch);
  free_buf(buffer);
  return;
}

/* OLC_SAVE.C
 * This takes care of saving all the .are information.
 * Notes:
 * -If a good syntax checker is used for setting vnum ranges of areas
 *  then it would become possible to just cycle through vnums instead
 *  of using the iHash stuff and checking that the room or reset or
 *  mob etc is part of that area. */

/*  Verbose writes reset data in plain english into the comments
 *  section of the resets.  It makes areas considerably larger but
 *  may aid in debugging. */
/* #define VERBOSE */

/* Returns a string without \r and ~. */
char *fix_string( const char *str )
{
    static char strfix[MSL*2];
    int i;
    int o;
    if ( str == NULL )
        return '\0';
    for ( o = i = 0; str[i+o] != '\0'; i++ )
    {
        if (str[i+o] == '\r' || str[i+o] == '~')
            o++;
        strfix[i] = str[i+o];
    }
    strfix[i] = '\0';
    return strfix;
}

/* Saves the listing of files to be loaded at startup. *
 * Called by do_asave(olc_save.c).                     */
void save_area_list()
{
    FILE *fp;
    AREA_DATA *pArea;
    if ( ( fp = fopen( "area.lst", "w" ) ) == NULL )
    {
	bug( "Save_area_list: fopen", 0 );
	fp = fopen( NULL_FILE, "r" );
	fclose (fp);
	perror( "area.lst" );
    }
    else
    {
        /* Add any help files that need to be loaded at *
         * startup to this section.                     */
        fprintf( fp, "social.are\n" );
	for( pArea = area_first; pArea; pArea = pArea->next ){
	  fprintf( fp, "%s\n", pArea->file_name );
	}
	fprintf( fp, "$\n" );
	fclose( fp );
    }
    return;
}

/* Used in save_mobile and save_object below.  Writes *
 * flags on the form fread_flag reads.                *
 * buf[] must hold at least 32+1 characters.          */
char *fwrite_flag( long flags, char buf[] )
{
    char offset;
    char *cp;
    buf[0] = '\0';
    if ( flags == 0 )
    {
	strcpy( buf, "0" );
	return buf;
    }
    for ( offset = 0, cp = buf; offset < 32; offset++ )
        if ( flags & ( (long)1 << offset ) )
	{
	    if ( offset <= 'Z' - 'A' )
		*(cp++) = 'A' + offset;
	    else
		*(cp++) = 'a' + offset - ( 'Z' - 'A' + 1 );
	}
    *cp = '\0';
    return buf;
}

/* save all the helps to the file */
void save_helps(){
  FILE *fp;
  HELP_DATA *pHelp;
  CABAL_INDEX_DATA* pCab;

  int vnum = 0;
  fclose( fpReserve );
  if ( ( fp = fopen( HELP_FILE, "w" ) ) == NULL ){
    fp = fopen( NULL_FILE, "r" );
    fclose (fp);
    perror( HELP_FILE );
  }
  else{
    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next ){
      /* vnum */
      fprintf( fp, "#%d\n", ++vnum);
      /* level/type */
      fprintf( fp, "%d ", pHelp->level);
      /* decide if we print anything special for race etc.*/
      switch (pHelp->level){
      case HELP_RACE:
	/* special races helps */
	if (pHelp->type < 0){
	  if (pHelp->type == -1){
	    fprintf( fp, "%s~ ", "avatar" );
	    break;
	  }
	}
	else
	  fprintf( fp, "%s~ ", race_table[pHelp->type].name );
	break;
      case HELP_CLASS:
      if (pHelp->type < 0){
        break;
        }
	fprintf( fp, "%s~ ", class_table[pHelp->type].name );
	break;
      case HELP_CABAL:
      if (pHelp->type < 0){
        break;
        }
	pCab = get_cabal_index( pHelp->type );
	fprintf( fp, "%s~ ", pCab ? pCab->name : "cabal" );
	break;
      case HELP_PSALM:
      if (pHelp->type < 0){
        break;
        }
	fprintf( fp, "%s~ ", psalm_table[pHelp->type].name );
	break;
      case HELP_ALL:
      default:
	break;
      }
      /* print the keywords */
      fprintf( fp, "%s~\n", pHelp->keyword );
      /* print text */
      fprintf( fp, "%s~\n\n", pHelp->text );
    }
  }
  fprintf( fp, "#0" );

  fclose( fp );
  fpReserve = fopen( NULL_FILE, "r" );
}



/* Save one mobile to file, new format *
 * Called by save_mobiles (below).     */
void save_mobile( FILE *fp, MOB_INDEX_DATA *pMobIndex )
{
    PROG_LIST *pMprog;
    sh_int race = pMobIndex->race;
    char buf[MSL];
    long temp;
    fprintf( fp, "#%d\n",         pMobIndex->vnum );
    fprintf( fp, "%s~\n",         pMobIndex->player_name );
    fprintf( fp, "%s~\n",         pMobIndex->short_descr );
    fprintf( fp, "%s~\n",         fix_string( pMobIndex->long_descr ) );
    fprintf( fp, "%s~\n",         fix_string( pMobIndex->description) );
    fprintf( fp, "%s~\n",         race_table[race].name );
    fprintf( fp, "%s ",	          fwrite_flag( pMobIndex->act,         buf ) );
    fprintf( fp, "%s ",	          fwrite_flag( pMobIndex->act2,        buf ) );
    fprintf( fp, "%s ",	          fwrite_flag( pMobIndex->affected_by, buf ) );
    fprintf( fp, "%s ",	          fwrite_flag( pMobIndex->affected2_by, buf ) );
    fprintf( fp, "%d %d\n",        pMobIndex->alignment , pMobIndex->group);
    fprintf( fp, "%d ",	          pMobIndex->level );
    fprintf( fp, "%d ",	          pMobIndex->hitroll );
    fprintf( fp, "%dd%d+%d ",     pMobIndex->hit[DICE_NUMBER],
	     	     	          pMobIndex->hit[DICE_TYPE],
	     	     	          pMobIndex->hit[DICE_BONUS] );

    fprintf( fp, "%dd%d+%d ",     pMobIndex->mana[DICE_NUMBER],
	     	     	          pMobIndex->mana[DICE_TYPE],
	     	     	          pMobIndex->mana[DICE_BONUS] );

    fprintf( fp, "%dd%d+%d ",     pMobIndex->damage[DICE_NUMBER],
	     	     	          pMobIndex->damage[DICE_TYPE],
	     	     	          pMobIndex->damage[DICE_BONUS] );
    fprintf( fp, "%s\n",          attack_table[pMobIndex->dam_type].name );
    fprintf( fp, "%d %d %d %d\n", pMobIndex->ac[AC_PIERCE] / 10,
	     	     	          pMobIndex->ac[AC_BASH]   / 10,
	     	     	          pMobIndex->ac[AC_SLASH]  / 10,
	     	     	          pMobIndex->ac[AC_EXOTIC] / 10 );
    fprintf( fp, "%s ",           fwrite_flag( pMobIndex->off_flags,  buf ) );
    fprintf( fp, "%s ",	          fwrite_flag( pMobIndex->imm_flags,  buf ) );
    fprintf( fp, "%s ",           fwrite_flag( pMobIndex->res_flags,  buf ) );
    fprintf( fp, "%s\n",          fwrite_flag( pMobIndex->vuln_flags, buf ) );
    fprintf( fp, "%s %s ",        position_table[pMobIndex->start_pos].short_name,
	         	     	  position_table[pMobIndex->default_pos].short_name);
    fprintf( fp, "%s ",    	  sex_table[pMobIndex->sex].name);

    fprintf( fp, "%s~\n",    	  pMobIndex->pCabal ? pMobIndex->pCabal->name : "none");

    fprintf( fp, "%ld\n",          pMobIndex->gold );
    fprintf( fp, "%s ",           fwrite_flag( pMobIndex->form,  buf ) );
    fprintf( fp, "%s ",      	  fwrite_flag( pMobIndex->parts, buf ) );
    fprintf( fp, "%s ",           size_table[pMobIndex->size].name );
    fprintf( fp, "0\n");
    if ((temp = DIF(race_table[race].act,pMobIndex->act)))
        fprintf( fp, "F act %s\n", fwrite_flag(temp, buf) );
    if ((temp = DIF(race_table[race].act2,pMobIndex->act2)))
        fprintf( fp, "F act2 %s\n", fwrite_flag(temp, buf) );
    if ((temp = DIF(race_table[race].aff,pMobIndex->affected_by)))
        fprintf( fp, "F aff %s\n", fwrite_flag(temp, buf) );
    if ((temp = DIF(race_table[race].off,pMobIndex->off_flags)))
        fprintf( fp, "F off %s\n", fwrite_flag(temp, buf) );
    if ((temp = DIF(race_table[race].imm,pMobIndex->imm_flags)))
        fprintf( fp, "F imm %s\n", fwrite_flag(temp, buf) );
    if ((temp = DIF(race_table[race].res,pMobIndex->res_flags)))
        fprintf( fp, "F res %s\n", fwrite_flag(temp, buf) );
    if ((temp = DIF(race_table[race].vuln,pMobIndex->vuln_flags)))
        fprintf( fp, "F vul %s\n", fwrite_flag(temp, buf) );
/*    if ((temp = DIF(race_table[race].form,pMobIndex->form)))
        fprintf( fp, "F for %s\n", fwrite_flag(temp, buf) );
    if ((temp = DIF(race_table[race].parts,pMobIndex->parts)))
        fprintf( fp, "F par %s\n", fwrite_flag(temp, buf) );*/

    for (pMprog = pMobIndex->mprogs; pMprog; pMprog = pMprog->next)
    {
        fprintf(fp, "P %s %d %s~\n",
        prog_type_to_name(pMprog->trig_type), pMprog->vnum,
                pMprog->trig_phrase);
    }
    return;
}

/* Save #MOBILES secion of an area file. *
 * Called by:     save_area(olc_save.c). */

/* This function is for new mobprogs only, Will MERGE them later. */
void save_new_mobprogs( FILE *fp, AREA_DATA *pArea )
{
	PROG_CODE *pMprog;
        int i;

	//old save_mobprog below prints this.        fprintf(fp, "#MOBPROGS\n");

	for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
        {
          if ( (pMprog = get_prog_index(i, PRG_MPROG) ) != NULL)
		{
		          fprintf(fp, "#%d\n", i);
		          fprintf(fp, "%s~\n", fix_string(pMprog->code));
		}
        }
        fprintf(fp,"#0\n\n");
        return;
}

void save_mobprogs( FILE *fp, AREA_DATA *pArea )
{
    int iHash;
    TRIGFILE_DATA *ptrigfile;
    MOB_INDEX_DATA *pMobIndex;

    fprintf( fp, "#MOBPROGS\n" );

    /* Write new progs */
    save_new_mobprogs( fp, pArea);

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
            if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->progtypes )
		for ( ptrigfile = pMobIndex->trigfile; ptrigfile; ptrigfile = ptrigfile->next )
                    fprintf( fp, "M %d %d %s NL\n", pMobIndex->vnum, ptrigfile->dowhen, ptrigfile->name );
    fprintf( fp, "S\n\n\n\n\n" );
    return;
}



void save_objprogs( FILE *fp, AREA_DATA *pArea )
{
	PROG_CODE *pOprog;
        int i;

        fprintf(fp, "#OBJPROGS\n");

	for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
        {
          if ( (pOprog = get_prog_index(i, PRG_OPROG) ) != NULL)
		{
		          fprintf(fp, "#%d\n", i);
		          fprintf(fp, "%s~\n", fix_string(pOprog->code));
		}
        }

        fprintf(fp,"#0\n\n");
        return;
}

void save_roomprogs( FILE *fp, AREA_DATA *pArea )
{
	PROG_CODE *pRprog;
        int i;

        fprintf(fp, "#ROOMPROGS\n");

	for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
        {
          if ( (pRprog = get_prog_index(i,PRG_RPROG) ) != NULL)
		{
		          fprintf(fp, "#%d\n", i);
		          fprintf(fp, "%s~\n", fix_string(pRprog->code));
		}
        }

        fprintf(fp,"#0\n\n");
        return;
}

void save_mobiles( FILE *fp, AREA_DATA *pArea )
{
    int i;
    MOB_INDEX_DATA *pMob;
    fprintf( fp, "#MOBILES\n" );
    for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
      if ( (pMob = get_mob_index( i )) ){
	save_mobile( fp, pMob );
      }
    fprintf( fp, "#0\n\n\n\n" );
    return;
}

/* Save one object to file.             *
 * Called by:     save_objects (below). */
void save_object( FILE *fp, OBJ_INDEX_DATA *pObjIndex )
{
    char letter;
    AFFECT_DATA *pAf;
    EXTRA_DESCR_DATA *pEd;
    OBJ_SPELL_DATA *pOs;
    char buf[MSL];
    PROG_LIST *pOprog;

    if (pObjIndex->item_type == ITEM_PROJECTILE)
      SET_BIT( pObjIndex->wear_flags, ITEM_WEAR_QUIVER);

    fprintf( fp, "#%d\n",    pObjIndex->vnum );
    fprintf( fp, "%s~\n",    pObjIndex->name );
    fprintf( fp, "%s~\n",    pObjIndex->short_descr );
    fprintf( fp, "%s~\n",    fix_string( pObjIndex->description ) );
    fprintf( fp, "%s~\n",    pObjIndex->material );
    fprintf( fp, "%s ",      item_name(pObjIndex->item_type));
    fprintf( fp, "%s ",      fwrite_flag( pObjIndex->extra_flags, buf ) );
    fprintf( fp, "%s\n",     fwrite_flag( pObjIndex->wear_flags,  buf ) );
/*  Using fwrite_flag to write most values gives a strange *
 *  looking area file, consider making a case for each     *
 *  item type later.                                       */
    switch ( pObjIndex->item_type )
    {
        default:
            fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[0], buf ) );
            fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[1], buf ) );
            fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[2], buf ) );
            fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[3], buf ) );
            fprintf( fp, "%s\n", fwrite_flag( pObjIndex->value[4], buf ) );
	    break;
        case ITEM_LIGHT:
            fprintf( fp, "0 0 %d 0 0\n", pObjIndex->value[2] < 1 ? 999 : pObjIndex->value[2] );
	    break;
        case ITEM_MONEY:
            fprintf( fp, "%d %d 0 0 0\n", pObjIndex->value[0], pObjIndex->value[1]);
            break;
        case ITEM_DRINK_CON:
            fprintf( fp, "%d %d '%s' %d 0\n",
              pObjIndex->value[0], pObjIndex->value[1],
              liq_table[pObjIndex->value[2]].liq_name,
              pObjIndex->value[3]);
            break;
	case ITEM_FOUNTAIN:
	    fprintf( fp, "%d %d '%s' 0 0\n",
              pObjIndex->value[0], pObjIndex->value[1],
              liq_table[pObjIndex->value[2]].liq_name);
	    break;
        case ITEM_CONTAINER:
            fprintf( fp, "%d %s %d %d %d\n",
              pObjIndex->value[0], fwrite_flag( pObjIndex->value[1], buf ),
              pObjIndex->value[2], pObjIndex->value[3], pObjIndex->value[4]);
            break;
        case ITEM_FOOD:
            fprintf( fp, "%d %d 0 %s %d\n",
              pObjIndex->value[0], pObjIndex->value[1], fwrite_flag( pObjIndex->value[3], buf ), pObjIndex->value[4] );
            break;
        case ITEM_WEAPON:
            fprintf( fp, "%s %d %d %s %s\n",
              weapon_name(pObjIndex->value[0]), pObjIndex->value[1], pObjIndex->value[2],
              attack_table[pObjIndex->value[3]].name, fwrite_flag( pObjIndex->value[4], buf ) );
            break;
	case ITEM_THROW:
            fprintf( fp, "%d %d %d %s %s\n",
              pObjIndex->value[0], pObjIndex->value[1], pObjIndex->value[2],
              attack_table[pObjIndex->value[3]].name, fwrite_flag( pObjIndex->value[4], buf ) );
            break;
        case ITEM_ARMOR:
            fprintf( fp, "%d %d %d %d %d\n",
              pObjIndex->value[0], pObjIndex->value[1], pObjIndex->value[2],
              pObjIndex->value[3], pObjIndex->value[4]);
            break;
        case ITEM_PILL:
        case ITEM_POTION:
        case ITEM_SCROLL:
	case ITEM_RELIC:
	case ITEM_ARTIFACT:
	case ITEM_HERB:
		/* Timer, Eaten, Herb, Brew, None */
	    fprintf( fp, "%d '%s' '%s' '%s' '%s'\n",
              pObjIndex->value[0] > 0 ? pObjIndex->value[0] : 0,
              pObjIndex->value[1] != -1 ? skill_table[pObjIndex->value[1]].name : "",
              pObjIndex->value[2] != -1 ? skill_table[pObjIndex->value[2]].name : "",
              pObjIndex->value[3] != -1 ? skill_table[pObjIndex->value[3]].name : "",
              pObjIndex->value[4] != -1 ? skill_table[pObjIndex->value[4]].name : "");
	    break;
        case ITEM_STAFF:
        case ITEM_WAND:
	    fprintf( fp, "%d ", pObjIndex->value[0] );
	    fprintf( fp, "%d ", pObjIndex->value[1] );
	    fprintf( fp, "%d '%s' 0\n",
              pObjIndex->value[2], pObjIndex->value[3] != -1 ? skill_table[pObjIndex->value[3]].name : "(null)" );
	    break;
    case ITEM_INSTRUMENT:
      fprintf( fp, "0 0 0 0 %s\n", fwrite_flag( pObjIndex->value[4], buf ) );
      break;
    case ITEM_SOCKET:
      fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[0], buf ) );
      fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[1], buf ) );
      fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[2], buf ) );
      fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[3], buf ) );
      fprintf( fp, "%s\n",
	       fwrite_flag( IS_SOC_STAT(pObjIndex, SOCKET_WEAPON) ?
			    pObjIndex->value[4] : 0, buf ) );
      break;
    case ITEM_RANGED:
      fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[0], buf ) );
      fprintf( fp, "%d ",  pObjIndex->value[1]);
      fprintf( fp, "%d ",  pObjIndex->value[2]);
      fprintf( fp, "%d ",  pObjIndex->value[3]);
      fprintf( fp, "%s\n",  fwrite_flag( pObjIndex->value[4], buf ) );
      break;
    case ITEM_PROJECTILE:
      fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[0], buf ) );
      fprintf( fp, "%d ",  pObjIndex->value[1]);
      fprintf( fp, "%d ",  pObjIndex->value[2]);
      fprintf( fp, "%s ",  attack_table[pObjIndex->value[3]].name);
      fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[4], buf )
);
      break;
    }
    fprintf( fp, "%d ", pObjIndex->level );
    fprintf( fp, "%d ", pObjIndex->weight );
    fprintf( fp, "%d ", pObjIndex->cost );
    if      ( pObjIndex->condition >  90 ) letter = 'P';
    else if ( pObjIndex->condition >  75 ) letter = 'G';
    else if ( pObjIndex->condition >  50 ) letter = 'A';
    else if ( pObjIndex->condition >  25 ) letter = 'W';
    else if ( pObjIndex->condition >  10 ) letter = 'D';
    else if ( pObjIndex->condition >   0 ) letter = 'B';
    else                                   letter = 'R';
    fprintf( fp, "%c\n", letter );
    if (pObjIndex->pCabal)
      fprintf( fp, "C\n%s~\n", pObjIndex->pCabal->name );
    if (pObjIndex->race)
      fprintf( fp, "R\n%d\n", pObjIndex->race );
    if (pObjIndex->class >= 0)
      fprintf( fp, "L\n%d\n", pObjIndex->class );
    for( pEd = pObjIndex->extra_descr; pEd; pEd = pEd->next )
        fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword, fix_string( pEd->description ) );
    for ( pOs = pObjIndex->spell; pOs; pOs = pOs->next )
        fprintf( fp, "T\n%d %d %d\n%s~\n%s~\n", pOs->spell, pOs->target, pOs->percent,
	pOs->message, pOs->message2 );
    if (pObjIndex->message)
	fprintf( fp, "M\n%s~\n%s~\n%s~\n%s~\n", pObjIndex->message->onself, pObjIndex->message->onother,
	pObjIndex->message->offself, pObjIndex->message->offother );
    for( pAf = pObjIndex->affected; pAf; pAf = pAf->next )
        fprintf( fp, "F\n%s %d %d %s\n",
		 pAf->where == TO_IMMUNE ? "I" :
		 pAf->where==TO_RESIST ? "R"   :
		 pAf->where == TO_VULN ? "V"   :
		 pAf->where == TO_SKILL ? "S"  : "A",
		 pAf->location, pAf->modifier, fwrite_flag(pAf->bitvector, buf) );

    for (pOprog = pObjIndex->oprogs; pOprog; pOprog = pOprog->next)
    {
        fprintf(fp, "P %s %d %s~\n",
        prog_type_to_name(pOprog->trig_type), pOprog->vnum,
                pOprog->trig_phrase);
    }
    return;
}

/* Save #OBJECTS section of an area file. *
 * Called by save_area(olc_save.c).       */
void save_objects( FILE *fp, AREA_DATA *pArea )
{
    int i;
    OBJ_INDEX_DATA *pObj;
    fprintf( fp, "#OBJECTS\n" );
    for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
	if ( (pObj = get_obj_index( i )) )
	    save_object( fp, pObj );
    fprintf( fp, "#0\n\n\n\n" );
    return;
}

/* SAVE #TRAPS section of an area file	*/
/* called save_area			*/
/* This function is for new mobprogs only, Will MERGE them later. */
void save_traps( FILE *fp, AREA_DATA *pArea ){
  char buf[MIL];
  TRAP_INDEX_DATA *pTrap;
  int i;

  fprintf(fp, "#TRAPS\n");

  for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ ){
    if ( (pTrap = get_trap_index(i)) != NULL){
      fprintf(fp, "#%d\n", i);
      /* common things */
      fprintf( fp, "%s~\n%s~\n%s~\n%s %d %s\n",
	       pTrap->name,
	       pTrap->echo,
	       pTrap->oEcho,
	       trap_table[pTrap->type].name,
	       pTrap->level,
	       fwrite_flag(pTrap->flags, buf));

      /* based on type */
      switch (pTrap->type){
      default:
      case TTYPE_DUMMY:
	fprintf( fp, "%d %d %d %d %d\n",
		 pTrap->value[0],
		 pTrap->value[1],
		 pTrap->value[2],
		 pTrap->value[3],
		 pTrap->value[4]);
	break;
      case TTYPE_DAMAGE:
	fprintf( fp, "%s %d %d %d %d\n",
		 attack_table[pTrap->value[0]].name,
		 pTrap->value[1],
		 pTrap->value[2],
		 pTrap->value[3],
		 pTrap->value[4]);
	break;
      case TTYPE_XDAMAGE:
	fprintf( fp, "%s~ %d %d %d %d\n",
         /* Kaslai: I DID SOMETHING DANGEROUS - I'm subtracting MAX_SKILL from the index in effect_table
                    to get it back in bounds.                                                                   */
		 IS_GEN(pTrap->value[0]) ? effect_table[pTrap->value[0] - MAX_SKILL].name :
		 skill_table[pTrap->value[0]].name,
		 pTrap->value[1],
		 pTrap->value[2],
		 pTrap->value[3],
		 pTrap->value[4]);
	break;
      case TTYPE_SPELL:
	fprintf( fp, "%s~ %d %s~ %d %d\n",
		 skill_table[pTrap->value[0]].name,
		 pTrap->value[1],
		 skill_table[pTrap->value[2]].name,
		 pTrap->value[3],
		 pTrap->value[4]);
	break;
      case TTYPE_MOB:
	fprintf( fp, "%d %d %d %d %d\n",
		 pTrap->value[0],
		 pTrap->value[1],
		 pTrap->value[2],
		 pTrap->value[3],
		 pTrap->value[4]);
	break;
      }
    }
  }
  fprintf(fp,"#0\n\n");
  return;
}

/* writes a singel room */
void write_room( FILE* fp, ROOM_INDEX_DATA* pRoomIndex ){
  EXTRA_DESCR_DATA *pEd;
  EXIT_DATA *pExit;
  PROG_LIST *pRprog;
  int door;
  Double_List *tmp_list;
  char buf[MIL];

  fprintf( fp, "%s~\n",		pRoomIndex->name );
  fprintf( fp, "%s~\n",		fix_string( pRoomIndex->description ) );
  if (pRoomIndex->description2 != NULL)
    fprintf( fp, "%s~\n",		fix_string( pRoomIndex->description2 ) );
  else
    fprintf( fp, "~\n" );
  fprintf( fp, "0 " );
  fprintf( fp, "%d ",		pRoomIndex->room_flags );
  fprintf( fp, "%d ",		pRoomIndex->room_flags2 );
  fprintf( fp, "%d\n",		pRoomIndex->sector_type );
  for ( pEd = pRoomIndex->extra_descr; pEd; pEd = pEd->next )
    fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword, fix_string( pEd->description ) );

  for( door = 0; door < MAX_DIR; door++ ) {
    if ( ( pExit = pRoomIndex->exit[door] ) && pExit->to_room ){
      /* we check if this is a non virtual room leading to virtual */
      if (pRoomIndex->vnum > 0 && !IS_VIRROOM(pRoomIndex) && IS_VIRROOM(pExit->to_room))
	continue;
      fprintf( fp, "D%d\n",      pExit->orig_door );
      fprintf( fp, "%s~\n",      fix_string( pExit->description ) );
      fprintf( fp, "%s~\n",      pExit->keyword );
      fprintf( fp, "%s %d %d\n", fwrite_flag(pExit->rs_flags, buf),  pExit->key, pExit->to_room->vnum );
    }
  }

  if (pRoomIndex->mana_rate != 100 || pRoomIndex->heal_rate != 100)
    fprintf ( fp, "M %d H %d\n",pRoomIndex->mana_rate, pRoomIndex->heal_rate);
  if (pRoomIndex->pCabal)
    fprintf ( fp, "C %s~\n" , pRoomIndex->pCabal->name );
  if (pRoomIndex->temp != 72)
    fprintf ( fp, "T %d\n" , pRoomIndex->temp );
  tmp_list = pRoomIndex->watch_vnums;
  while (tmp_list != NULL) {
    fprintf ( fp, "W %lld\n" , (long long) tmp_list->cur_entry );
    tmp_list = tmp_list->next_node;
  }
  for (pRprog = pRoomIndex->rprogs; pRprog; pRprog = pRprog->next){
    fprintf(fp, "P %s %d %s~\n",
	    prog_type_to_name(pRprog->trig_type), pRprog->vnum,
	    pRprog->trig_phrase);
  }
  fprintf( fp, "S\n" );
}

/* Save #ROOMS section of an area file. *
 * Called by save_area(olc_save.c).     */
void save_rooms( FILE *fp, AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoomIndex;
    int iHash;

    fprintf( fp, "#ROOMS\n" );
    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ){
      for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next )
	if ( pRoomIndex->area == pArea && !IS_VIRROOM( pRoomIndex) ){
	  fprintf( fp, "#%d\n",		pRoomIndex->vnum );
	  write_room( fp, pRoomIndex );
	}
    }
    fprintf( fp, "#0\n\n\n\n" );
    return;
}

/* Save #SPECIALS section of area file. *
 * Called by save_area(olc_save.c).     */
void save_specials( FILE *fp, AREA_DATA *pArea )
{
    int iHash;
    MOB_INDEX_DATA *pMobIndex;
    fprintf( fp, "#SPECIALS\n" );
    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
            if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->spec_fun )
#if defined( VERBOSE )
                fprintf( fp, "M %d %s Load to: %s\n", pMobIndex->vnum, spec_name( pMobIndex->spec_fun ), pMobIndex->short_descr );
#else
                fprintf( fp, "M %d %s\n", pMobIndex->vnum, spec_name( pMobIndex->spec_fun ) );
#endif
    fprintf( fp, "S\n\n\n\n" );
    return;
}

/* writes a single room's resets */
void write_resets( FILE* fp, ROOM_INDEX_DATA* pRoom ){
  RESET_DATA *pReset;
  MOB_INDEX_DATA *pLastMob = NULL;
  OBJ_INDEX_DATA *pLastObj = NULL;
  TRAP_INDEX_DATA *pTrap = NULL;
  char buf[MIL];
  /* Useless conditional */
  if( pLastObj != NULL )
      pLastObj = NULL;
  /* Useless conditional */
  if( pTrap != NULL )
      pTrap = NULL;


  for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next ){
    switch ( pReset->command ){
    default:
      bug( "Save_resets: bad command %c.", pReset->command );
      break;
#if defined( VERBOSE )
    case 'M':
      pLastMob = get_mob_index( pReset->arg1 );
      fprintf( fp, "M 0 %d %d %d %d Load %s\n",
	       pReset->arg1, pReset->arg2, pReset->arg3,
	       pReset->arg4, pLastMob->short_descr );
      break;
    case 'O':
      pLastObj = get_obj_index( pReset->arg1 );
      pRoom = get_room_index( pReset->arg3 );
      fprintf( fp, "O 0 %d 0 %d %s loaded to %s\n",
	       pReset->arg1, pReset->arg3,
	       capitalize(pLastObj->short_descr), pRoom->name );
      break;
    case 'P':
      pLastObj = get_obj_index( pReset->arg1 );
      fprintf( fp, "P 0 %d %d %d %d %s put inside %s\n",
	       pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4,
	       capitalize(get_obj_index( pReset->arg1 )->short_descr),
	       pLastObj->short_descr );
      break;
    case 'G':
      fprintf( fp, "G 0 %d 0 %s is given to %s\n",
	       pReset->arg1,
	       capitalize(get_obj_index( pReset->arg1 )->short_descr),
	       pLastMob ? pLastMob->short_descr : "!NO_MOB!" );
      if ( !pLastMob )
	{
	  sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
	  bug( buf, 0 );
	}
      break;
    case 'E':
      fprintf( fp, "E 0 %d 0 %d %s is loaded %s of %s\n",
	       pReset->arg1, pReset->arg3,
	       capitalize(get_obj_index( pReset->arg1 )->short_descr),
	       flag_string( wear_loc_strings, pReset->arg3 ),
	       pLastMob ? pLastMob->short_descr : "!NO_MOB!" );
      if ( !pLastMob )
	{
	  sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
	  bug( buf, 0 );
	}
      break;
    case 'D':
      break;
    case 'R':
      pRoom = get_room_index( pReset->arg1 );
      fprintf( fp, "R 0 %d %d Randomize %s\n",
	       pReset->arg1, pReset->arg2, pRoom->name );
      break;
#endif
#if !defined( VERBOSE )
  case 'M':
    pLastMob = get_mob_index( pReset->arg1 );
    fprintf( fp, "M 0 %d %d %d %d\n",
	     pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4 );
    break;
  case 'O':
    pLastObj = get_obj_index( pReset->arg1 );
    pRoom = get_room_index( pReset->arg3 );
    fprintf( fp, "O 0 %d 0 %d\n", pReset->arg1, pReset->arg3 );
    break;
  case 'P':
    pLastObj = get_obj_index( pReset->arg1 );
    fprintf( fp, "P 0 %d %d %d %d\n", pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4 );
    break;
  case 'T':
    pTrap = get_trap_index( pReset->arg1 );
    fprintf( fp, "T 0 %d %d %d %d\n",
	     pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4 );
    break;
  case 'G':
    fprintf( fp, "G 0 %d 0\n", pReset->arg1 );
    if ( !pLastMob )
      {
	sprintf( buf, "Save_resets: !NO_MOB! in [%d]", pReset->arg1 );
	bug( buf, 0 );
      }
    break;
  case 'E':
    fprintf( fp, "E 0 %d 0 %d\n", pReset->arg1, pReset->arg3 );
    if ( !pLastMob )
      {
	sprintf( buf, "Save_resets: !NO_MOB! in [%d]", pReset->arg1 );
	bug( buf, 0 );
      }
    break;
  case 'D':
    break;
  case 'R':
    pRoom = get_room_index( pReset->arg1 );
    fprintf( fp, "R 0 %d %d\n", pReset->arg1, pReset->arg2 );
    break;
#endif
    }
  }
}

/* Saves the #RESETS section of an area file. *
 * Called by save_area(olc_save.c)            */
void save_resets( FILE *fp, AREA_DATA *pArea )
{
  ROOM_INDEX_DATA *pRoom;
  int iHash;

  fprintf( fp, "#RESETS\n" );
  for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ){
    for( pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next ){
      if ( pRoom->area == pArea && !IS_VIRROOM( pRoom ) ){
	write_resets( fp, pRoom );
      }
    }//end for room in hash cell
  }//end for each hash cell
  fprintf( fp, "S\n\n\n\n" );
  return;
}

/* Saves the #SHOPS section of an area file. *
 * Called by save_area(olc_save.c)           */
void save_shops( FILE *fp, AREA_DATA *pArea )
{
    SHOP_DATA *pShopIndex;
    MOB_INDEX_DATA *pMobIndex;
    int iHash;
    int iTrade;
    fprintf( fp, "#SHOPS\n" );
    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
	  if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->pShop)
            {
                pShopIndex = pMobIndex->pShop;
                fprintf( fp, "%d ", pShopIndex->keeper );
                for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
                {
                    if ( pShopIndex->buy_type[iTrade] != 0 )
                       fprintf( fp, "%d ", pShopIndex->buy_type[iTrade] );
                    else
                       fprintf( fp, "0 ");
                }
                fprintf( fp, "%d %d ", pShopIndex->profit_buy, pShopIndex->profit_sell );
                fprintf( fp, "%d %d\n", pShopIndex->open_hour, pShopIndex->close_hour );
            }
    fprintf( fp, "0\n\n\n\n" );
    return;
}

/* Save an area, note that this format is new. *
 * Called by do_asave(olc_save.c).             */
void save_area( AREA_DATA *pArea )
{
    FILE *fp;
    int i = 0, flag;
    fclose( fpReserve );
    if ( !( fp = fopen( pArea->file_name, "w" ) ) )
    {
	bug( "Open_area: fopen", 0 );
	fp = fopen( NULL_FILE, "r" );
	fclose (fp);
	perror( pArea->file_name );
    }
    /* clear off the flags which should not be saved */
    flag = pArea->area_flags;
    for (i = 0; area_flags[i].name; i ++){
      if (area_flags[i].settable == FALSE)
	flag &= ~area_flags[i].bit;
    }
    fprintf( fp, "#AREADATA\n" );
    fprintf( fp, "Name %s~\n",        pArea->name );
    fprintf( fp, "Pref %s~\n",        pArea->prefix );
    fprintf( fp, "Builders %s~\n",    fix_string( pArea->builders ) );
    fprintf( fp, "VNUMs %d %d\n",     pArea->min_vnum, pArea->max_vnum );
    fprintf( fp, "Max_Bastions %d\n", pArea->bastion_max);
    fprintf( fp, "Credits %s~\n",	 pArea->credits );
    fprintf( fp, "Security %d\n",         pArea->security );
    fprintf( fp, "Startroom %d\n",         pArea->startroom );
    fprintf( fp, "Flags %d\n",         flag );
    fprintf( fp, "Crimes %d ",         MAX_CRIME );

    /* print crimes */
    for (i = 0; i < MAX_CRIME; i++){
      fprintf( fp, "%d ", pArea->crimes[i] );
    }
    fprintf( fp, "\n" );
    fprintf( fp, "End\n\n\n\n" );
    save_cabal_indexes( fp, pArea );
    save_mobiles( fp, pArea );
    save_objects( fp, pArea );
    save_rooms( fp, pArea );
    save_army_indexes( fp, pArea );
    save_specials( fp, pArea );
    save_traps( fp, pArea );
    save_resets( fp, pArea );
    save_shops( fp, pArea );
    save_mobprogs( fp, pArea );
    save_objprogs( fp, pArea );
    save_roomprogs( fp, pArea );

    fprintf( fp, "#$\n" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/* Entry point for saving area data. *
 * Called by interpreter(interp.c)   */
void do_asave( CHAR_DATA *ch, char *argument )
{
    char arg1 [MIL];
    AREA_DATA *pArea;
    FILE *fp;
    int value;
    fp = NULL;

    /* Useless conditional */
    if( fp != NULL )
        fp = NULL;

    if ( !ch )
    {
	save_area_list();
	for( pArea = area_first; pArea; pArea = pArea->next )
	{
	    REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
	    save_area( pArea );
	}
	return;
    }
    smash_tilde( argument );
    strcpy( arg1, argument );
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax:\n\r", ch );
        send_to_char( "  asave <vnum>   - saves a particular area\n\r",     ch );
        send_to_char( "  asave list     - saves the area.lst file\n\r",     ch );
        send_to_char( "  asave help     - saves the help files\n\r",     ch );
        send_to_char( "  asave cabal    - saves the cabal file\n\r",     ch );
        send_to_char( "  asave area     - saves the area being edited\n\r", ch );
        send_to_char( "  asave tomes    - saves the library tomes\n\r", ch );
        send_to_char( "  asave changed  - saves all changed zones\n\r",     ch );
        send_to_char( "  asave armies   - saves all armies\n\r",     ch );
        send_to_char( "  asave world    - saves the world! (db dump)\n\r",  ch );
        send_to_char( "\n\r", ch );
        return;
    }
    value = atoi( arg1 );
    if ( !( pArea = get_area_data( value ) ) && is_number( arg1 ) )
    {
	send_to_char( "That area does not exist.\n\r", ch );
	return;
    }
    if ( is_number( arg1 ) )
    {
	if ( !IS_BUILDER( ch, pArea ) )
	{
	    send_to_char( "You are not a builder for this area.\n\r", ch );
	    return;
	}
	save_area_list();
	save_area( pArea );
	return;
    }
    if ( !str_cmp( "world", arg1 ) )
    {
	save_area_list();
	save_helps();
	send_to_char("Helps saved.\n\r", ch);
	save_armies();
	save_cabals(FALSE, ch);
	for( pArea = area_first; pArea; pArea = pArea->next )
	{
	    if ( !IS_BUILDER( ch, pArea ) )
		continue;
	    REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
	    save_area( pArea );
	}
	send_to_char( "You saved the world.\n\r", ch );
	return;
    }
    if ( !str_cmp( "changed", arg1 ) )
    {
	char buf[MIL];
	save_area_list();
	save_helps();
	save_cabals(TRUE, ch);
	send_to_char( "Saved zones:\n\r", ch );
	sprintf( buf, "None.\n\r" );
	for( pArea = area_first; pArea; pArea = pArea->next )
	{
	    if ( !IS_BUILDER( ch, pArea ) )
		continue;
	    if ( IS_SET(pArea->area_flags, AREA_CHANGED) )
	    {
		REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
		save_area( pArea );
		sprintf( buf, "%24s - '%s'\n\r", pArea->name, pArea->file_name );
		send_to_char( buf, ch );
	    }
        }
	if ( !str_cmp( buf, "None.\n\r" ) )
	    send_to_char( buf, ch );
        return;
    }
    if ( !str_cmp( arg1, "armies" ) ){
      save_armies();
      return;
    }
    if ( !str_cmp( arg1, "list" ) )
    {
	save_area_list();
	return;
    }
    if ( !str_cmp( arg1, "tomes" ) )
    {
	SaveTomes();
	return;
    }
    if ( !str_cmp( arg1, "cabal" ) || !str_cmp( arg1, "cabals" ) )
    {
	save_cabals(FALSE, ch);
	return;
    }
    if ( !str_cmp( arg1, "clan" ) || !str_cmp( arg1, "clans" ) )
    {
	SaveClans();
	return;
    }
    if ( !str_cmp( arg1, "help" ) )
    {
      save_helps();
      send_to_char("Helps saved.\n\r", ch);
      return;
    }
    if ( !str_cmp( arg1, "area" ) )
    {
	if ( ch->desc->editor == 0 )
	{
            send_to_char( "You are not editing an area, therefore an area vnum is required.\n\r", ch );
	    return;
	}
	switch (ch->desc->editor)
	{
        case ED_AREA:   pArea = (AREA_DATA *)ch->desc->pEdit; break;
        case ED_ROOM:   pArea = ch->in_room->area; break;
        case ED_OBJECT: pArea = ( (OBJ_INDEX_DATA *)ch->desc->pEdit )->area; break;
        case ED_MOBILE: pArea = ( (MOB_INDEX_DATA *)ch->desc->pEdit )->area; break;
        default:        pArea = ch->in_room->area; break;
	}
	if ( !IS_BUILDER( ch, pArea ) )
	{
	    send_to_char( "You are not a builder for this area.\n\r", ch );
	    return;
	}
	save_area_list();
	REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
	save_area( pArea );
	send_to_char( "Area saved.\n\r", ch );
	return;
    }
    do_asave( ch, "" );
    return;
}

/* The code below uses a table lookup system that is based on suggestions     *
 * from Russ Taylor.  There are many routines in handler.c that would benefit *
 * with the use of tables.  You may consider simplifying your code base by    *
 * implementing a system like below with such functions.                      */

struct flag_stat_type
{
    const struct flag_type *structure;
    bool stat;
};

/* This table catagorizes the tables following the lookup      *
 * functions below into stats and flags.  Flags can be toggled *
 * but stats can only be assigned.  Update this table when a   *
 * new set of flags is installed.                              */
const struct flag_stat_type flag_stat_table[] =
{
/*  {	structure		stat	}, */

/* OLC FLAGS */
    {	area_flags,		FALSE	},
    {   sex_flags,		TRUE	},
    {   exit_flags,		FALSE	},
    {   door_resets,		TRUE	},
    {   room_flags,		FALSE	},
    {   sector_flags,		TRUE	},
    {   type_flags,		TRUE	},
    {   extra_flags,		FALSE	},
    {   socket_flags,		FALSE	},
    {   wear_flags,		FALSE	},
    {   act_flags,		FALSE	},
    {   affect_flags,		FALSE	},
    {   affect2_flags,		FALSE	},
    {   apply_flags,		TRUE	},
    {   wear_loc_flags,		TRUE	},
    {   wear_loc_strings,	TRUE	},
    {   container_flags,	FALSE	},
    {   cabal_flags,		FALSE	},
    {   cabal_progress_flags,	FALSE	},
    {   croom_flags,		FALSE	},
    {   army_types,		TRUE	},
    {   army_flags,		FALSE	},

    /* ROM specific flags: */
    {	crime_table,		FALSE	},
    {	punish_table,		FALSE	},
    {	pact_flags,		FALSE	},
    {   form_flags,             FALSE   },
    {   part_flags,             FALSE   },
    {   ac_type,                TRUE    },
    {   size_flags,             TRUE    },
    {   position_flags,         TRUE    },
    {   off_flags,              FALSE   },
    {   imm_flags,              FALSE   },
    {   res_flags,              FALSE   },
    {   vuln_flags,             FALSE   },
    {   weapon_class,           TRUE    },
    {   weapon_type2,           FALSE   },
    {   apply_types,            TRUE    },
    {   room_flags2,		FALSE	},
    {   special_flags,		FALSE	},
    {   0,			0	}
};

/* Returns TRUE if the table is a stat table and FALSE if flag. *
 * Called by flag_value and flag_string.                        *
 * This function is local and used only in bit.c.               */
bool is_stat( const struct flag_type *flag_table )
{
    int flag;
    for (flag = 0; flag_stat_table[flag].structure; flag++)
        if ( flag_stat_table[flag].structure == flag_table && flag_stat_table[flag].stat )
	    return TRUE;
    return FALSE;
}

/* returns flag value regarldess of settable bit */
int flag_lookup_abs (const char *name, const struct flag_type *flag_table)
{
    int flag;
    for (flag = 0; flag_table[flag].name != NULL; flag++)
      if ( !str_cmp( name, flag_table[flag].name))
	return flag_table[flag].bit;
    return NO_FLAG;
}
/* This function is Russ Taylor's creation.  Thanks Russ!                  8
 * All code copyright (C) Russ Taylor, permission to use and/or distribute *
 * has NOT been granted.  Use only in this OLC package has been granted.   */

/* Returns the value of a single, settable flag from the table. *
 * Called by flag_value and flag_string.                        *
 * This function is local and used only in bit.c.               */
int flag_lookup2 (const char *name, const struct flag_type *flag_table)
{
    int flag;
    for (flag = 0; flag_table[flag].name != NULL; flag++)
        if ( !str_cmp( name, flag_table[flag].name ) && flag_table[flag].settable )
            return flag_table[flag].bit;
    return NO_FLAG;
}

/* Returns the value of the flags entered.  Multi-flags accepted. *
 * Called by olc.c and olc_act.c.                                 */
int flag_value( const struct flag_type *flag_table, char *argument)
{
    char word[MIL];
    int  bit;
    int  marked = 0;
    bool found = FALSE;
    if ( is_stat( flag_table ) )
    {
	one_argument( argument, word );
	if ( ( bit = flag_lookup2( word, flag_table ) ) != NO_FLAG )
	    return bit;
	else
	    return NO_FLAG;
    }
    for (; ;)
    {
        argument = one_argument( argument, word );
        if ( word[0] == '\0' )
            break;
        if ( ( bit = flag_lookup2( word, flag_table ) ) != NO_FLAG )
        {
            SET_BIT( marked, bit );
            found = TRUE;
        }
    }
    if ( found )
	return marked;
    else
	return NO_FLAG;
}

/* Returns string with name(s) of the flags or stat entered. *
 * Called by act_olc.c, olc.c, and olc_save.c.               */
char *flag_string( const struct flag_type *flag_table, int bits )
{
    static char buf[512];
    int  flag;
    buf[0] = '\0';
    for (flag = 0; flag_table[flag].name != NULL; flag++)
    {
	if ( !is_stat( flag_table ) && IS_SET(bits, flag_table[flag].bit) )
	{
	    strcat( buf, " " );
	    strcat( buf, flag_table[flag].name );
	}
        else if ( flag_table[flag].bit == bits )
	{
	    strcat( buf, " " );
	    strcat( buf, flag_table[flag].name );
	    break;
	}
    }
    return (buf[0] != '\0') ? buf+1 : "none";
}

RESET_DATA *new_reset_data( void )
{
    RESET_DATA *pReset;
    if ( !reset_free )
    {
        pReset          =   alloc_perm( sizeof(*pReset) );
        top_reset++;
    }
    else
    {
        pReset          =   reset_free;
        reset_free      =   reset_free->next;
    }
    pReset->next        =   NULL;
    pReset->command     =   'X';
    pReset->arg1        =   0;
    pReset->arg2        =   0;
    pReset->arg3        =   0;
    pReset->arg4        =   0;
    return pReset;
}

void free_reset_data( RESET_DATA *pReset )
{
    pReset->next            = reset_free;
    reset_free              = pReset;
    return;
}

AREA_DATA *new_area( void )
{
    AREA_DATA *pArea;
    char buf[MIL];
    int count;

    if ( !area_free )
    {
        pArea   =   alloc_perm( sizeof(*pArea) );
        top_area++;
    }
    else
    {
        pArea       =   area_free;
        area_free   =   area_free->next;
    }
    memset(pArea, 0, sizeof(*pArea ));

    pArea->next             =   NULL;
    pArea->name             =   str_dup( "New area" );
    pArea->pCabal	    = NULL;
    pArea->towers	    = NULL;
    pArea->exits	    = NULL;
    pArea->raid		    = NULL;
    pArea->last_raid	    = 0;
    pArea->area_flags       =   0;
    pArea->security         =   1;
    pArea->prefix	    =  str_empty;
    pArea->builders         =   str_dup( "None" );
    pArea->min_vnum            =   0;
    pArea->max_vnum            =   0;
    pArea->age              =   0;
    pArea->bastion_max      =   0;
    pArea->bastion_current  =   0;
    for (count = 0; count < MAX_CRIME; count++) {
      pArea->crimes[count] = 0;
    }
    pArea->idle             =   0;
    pArea->nplayer          =   0;
    pArea->empty            =   TRUE;
    sprintf( buf, "area%d.are", pArea->vnum );
    pArea->file_name        =   str_dup( buf );
    pArea->vnum             =   top_area-1;
    pArea->credits 	    = str_dup( "{ All }" );
    return pArea;
}

void free_area( AREA_DATA *pArea )
{
    free_string( pArea->name );
    free_string( pArea->file_name );
    free_string( pArea->builders );
    free_string( pArea->credits );
    pArea->next         =   area_free->next;
    area_free           =   pArea;
    return;
}

EXIT_DATA *new_exit( void )
{
    EXIT_DATA *pExit;
    if ( !exit_free )
    {
        pExit           =   alloc_perm( sizeof(*pExit) );
        top_exit++;
    }
    else
    {
        pExit           =   exit_free;
        exit_free       =   exit_free->next;
    }
    pExit->to_room   =   NULL;
    pExit->vnum	=   0;
    pExit->next         =   NULL;
    pExit->next_in_area =   NULL;
    pExit->traps	=   NULL;
    pExit->exit_info    =   0;
    pExit->key          =   0;
    pExit->orig_door	=   0;
    pExit->keyword      =   &str_empty[0];
    pExit->description  =   &str_empty[0];
    pExit->rs_flags     =   0;
    return pExit;
}

void free_exit( EXIT_DATA *pExit )
{
    free_string( pExit->keyword );
    free_string( pExit->description );
    pExit->next         =   exit_free;
    exit_free           =   pExit;
    return;
}

ROOM_INDEX_DATA *new_room_index( void )
{
    ROOM_INDEX_DATA *pRoom;
    int door;
    if ( !room_index_free )
    {
        pRoom           =   alloc_perm( sizeof(*pRoom) );
        top_room++;
    }
    else
    {
        pRoom           =   room_index_free;
        room_index_free =   room_index_free->next;
    }

    pRoom->next             =   NULL;
    pRoom->area             =   NULL;
    pRoom->vnum             =   0;
    pRoom->pCabal           =   NULL;

    pRoom->people           =   NULL;
    pRoom->contents         =   NULL;

    pRoom->name             =   &str_empty[0];
    pRoom->description      =   &str_empty[0];
    pRoom->description2     =   &str_empty[0];
    pRoom->extra_descr      =   NULL;
    pRoom->watch_vnums =	NULL;
    pRoom->rprogs	    =	NULL;

    pRoom->room_flags       =   0;
    pRoom->room_flags2      =   0;
    pRoom->sector_type      =   0;

    pRoom->light            =   0;
    pRoom->heal_rate	    =   100;
    pRoom->mana_rate	    =   100;
    pRoom->temp             =   72;

    for ( door=0; door < MAX_DIR; door++ )
        pRoom->exit[door]   =   NULL;

    memset(&pRoom->room_armies, 0, sizeof( ARMY_ROOM_DATA ));

    return pRoom;
}

void free_room_index( ROOM_INDEX_DATA *pRoom )
{
    int door;
    EXTRA_DESCR_DATA *pExtra;
    RESET_DATA *pReset;

    if (pRoom == NULL)
      return;
/* in order to free this we have to check if its a string in shared space,
or perm-allocated
*/
    if (pRoom->name > string_space && pRoom->name < top_string ){
      free_string( pRoom->name );
    }
    if (pRoom->description > string_space && pRoom->description < top_string ){
      free_string( pRoom->description );
    }
    if (pRoom->description2 > string_space && pRoom->description2 < top_string ){
    free_string( pRoom->description2 );
    }

    while (pRoom->rprogs ){
      PROG_LIST* pr = pRoom->rprogs;
      pRoom->rprogs = pr->next;
      free_rprog( pr );
    }

    for ( door = 0; door < MAX_DIR; door++ ){
      if ( pRoom->exit[door] )
	free_exit( pRoom->exit[door] );
    }
    while ( (pExtra = pRoom->extra_descr)){
      pRoom->extra_descr = pExtra->next;
      free_extra_descr( pExtra );
    }

    while ( (pReset = pRoom->reset_first)){
      pRoom->reset_first = pReset->next;
      free_reset_data( pReset );
    }

    while ( pRoom->watch_vnums ){
      Double_List * tmp_list = pRoom->watch_vnums;
      pRoom->watch_vnums = tmp_list->next_node;
      free (tmp_list);
    }

    pRoom->next     =   room_index_free;
    room_index_free =   pRoom;
    return;
}

extern AFFECT_DATA *affect_free;
extern AFFECT_DATA *skill_free;

SHOP_DATA *new_shop( void )
{
    SHOP_DATA *pShop;
    int buy;
    if ( !shop_free )
    {
        pShop           =   alloc_perm( sizeof(*pShop) );
        top_shop++;
    }
    else
    {
        pShop           =   shop_free;
        shop_free       =   shop_free->next;
    }
    if ( shop_first == NULL )
        shop_first = pShop;
    if ( shop_last  != NULL )
        shop_last->next = pShop;
    shop_last       = pShop;
    pShop->next         =   NULL;
    pShop->keeper       =   0;
    for ( buy=0; buy<MAX_TRADE; buy++ )
        pShop->buy_type[buy]    =   0;
    pShop->profit_buy   =   100;
    pShop->profit_sell  =   100;
    pShop->open_hour    =   0;
    pShop->close_hour   =   23;
    return pShop;
}

void free_shop( SHOP_DATA *pShop )
{
    pShop->next = shop_free;
    shop_free   = pShop;
    return;
}

OBJ_INDEX_DATA *new_obj_index( void )
{
    OBJ_INDEX_DATA *pObj;
    int value;
    if ( !obj_index_free )
    {
        pObj           =   alloc_perm( sizeof(*pObj) );
        top_obj_index++;
    }
    else
    {
        pObj            =   obj_index_free;
        obj_index_free  =   obj_index_free->next;
    }
    pObj->next          =   NULL;
    pObj->extra_descr   =   NULL;
    pObj->affected      =   NULL;
    pObj->area          =   NULL;
    pObj->name          =   str_dup( "no name" );
    pObj->short_descr   =   str_dup( "(no short description)" );
    pObj->description   =   str_dup( "(no description)" );
    pObj->vnum          =   0;
    pObj->item_type     =   ITEM_TRASH;
    pObj->extra_flags   =   0;
    pObj->wear_flags    =   0;
    pObj->count         =   0;
    pObj->class         =   -1;
    pObj->race          =    0;
    pObj->weight        =   0;
    pObj->cost          =   0;
    pObj->material      =   str_dup( "unknown" );
    pObj->condition     =   100;
    for ( value = 0; value < 5; value++ )
        pObj->value[value]  =   0;
    pObj->new_format    = TRUE;
    return pObj;
}

void free_obj_index( OBJ_INDEX_DATA *pObj )
{
    EXTRA_DESCR_DATA *pExtra;
    AFFECT_DATA *pAf;
    OBJ_SPELL_DATA *os, *os_next;
    int num_times = 0;

    for (os = pObj->spell; os != NULL; os = os_next)
    {
        os_next = os->next;
        free_string(os->message);
        free_string(os->message2);

/* break out of loop if stupid number of times through */
	if (num_times++ > STUPID_NUMBER_OF_SPELLS) {
	  break;
	}
    }
    if (pObj->message)
    {
        free_string(pObj->message->onself);
        free_string(pObj->message->onother);
        free_string(pObj->message->offself);
        free_string(pObj->message->offother);
    }
    free_string( pObj->name );
    free_string( pObj->short_descr );
    free_string( pObj->description );
    free_oprog( pObj->oprogs );
    free_string( pObj->material );
    for ( pAf = pObj->affected; pAf; pAf = pAf->next )
        free_affect( pAf );
    for ( pExtra = pObj->extra_descr; pExtra; pExtra = pExtra->next )
        free_extra_descr( pExtra );
    pObj->next              = obj_index_free;
    obj_index_free          = pObj;
    return;
}

MOB_INDEX_DATA *new_mob_index( void )
{
    MOB_INDEX_DATA *pMob;
    if ( !mob_index_free )
    {
        pMob           =   alloc_perm( sizeof(*pMob) );
        top_mob_index++;
    }
    else
    {
        pMob            =   mob_index_free;
        mob_index_free  =   mob_index_free->next;
    }
    pMob->next          =   NULL;
    pMob->spec_fun      =   NULL;
    pMob->pShop         =   NULL;
    pMob->area          =   NULL;
    pMob->player_name   =   str_dup( "no name" );
    pMob->short_descr   =   str_dup( "(no short description)" );
    pMob->long_descr    =   str_dup( "(no long description)\n\r" );
    pMob->description   =   &str_empty[0];
    pMob->vnum          =   0;
    pMob->count         =   0;
    pMob->killed        =   0;
    pMob->sex           =   0;
    pMob->pCabal        =   NULL;
    pMob->level         =   0;
    pMob->act           =   ACT_IS_NPC;
    pMob->act2          =   0;
    pMob->affected_by   =   0;
    pMob->alignment     =   0;
    pMob->hitroll	=   0;
    pMob->race          =   race_lookup( "human" );
    pMob->form          =   0;
    pMob->parts         =   0;
    pMob->imm_flags     =   0;
    pMob->res_flags     =   0;
    pMob->vuln_flags    =   0;
    pMob->off_flags     =   0;
    pMob->size          =   SIZE_MEDIUM;
    pMob->ac[AC_PIERCE] =   0;
    pMob->ac[AC_BASH]   =   0;
    pMob->ac[AC_SLASH]  =   0;
    pMob->ac[AC_EXOTIC] =   0;
    pMob->hit[DICE_NUMBER]      =   0;
    pMob->hit[DICE_TYPE]        =   0;
    pMob->hit[DICE_BONUS]       =   0;
    pMob->mana[DICE_NUMBER]     =   0;
    pMob->mana[DICE_TYPE]       =   0;
    pMob->mana[DICE_BONUS]      =   0;
    pMob->damage[DICE_NUMBER]   =   0;
    pMob->damage[DICE_TYPE]     =   0;
    pMob->damage[DICE_NUMBER]   =   0;
    pMob->start_pos             =   POS_STANDING;
    pMob->default_pos           =   POS_STANDING;
    pMob->gold                  =   0;
    pMob->new_format            = TRUE;
    return pMob;
}

void free_mob_index( MOB_INDEX_DATA *pMob )
{
    TRIGFILE_DATA *tf, *tf_next;
    MPROG_DATA *mp, *mp_next;
    for (tf = pMob->trigfile; tf != NULL; tf = tf_next)
    {
        tf_next = tf->next;
	free_string(tf->name);
    }
    for (mp = pMob->mobprogs; mp != NULL; mp = mp_next)
    {
        mp_next = mp->next;
        free_string(mp->arglist);
        free_string(mp->comlist);
    }
    free_string( pMob->player_name );
    free_string( pMob->short_descr );
    free_string( pMob->long_descr );
    free_string( pMob->description );
    free_shop( pMob->pShop );
    pMob->next              = mob_index_free;
    mob_index_free          = pMob;
    return;
}

/* Clears string and puts player into editing mode. */
void string_edit( CHAR_DATA *ch, char **pString )
{
    send_to_char( "-========- Entering EDIT Mode -=========-\n\r", ch );
    send_to_char( "    Type .h on a new line for help\n\r", ch );
    send_to_char( " Terminate with a ~ or @ on a blank line.\n\r", ch );
    send_to_char( "-=======================================-\n\r", ch );
    if ( *pString == NULL )
        *pString = str_dup( "" );
    else
        **pString = '\0';
    ch->desc->pString = pString;
    return;
}

/* Puts player into append mode for given string. *
 * Called by (many)olc_act.c                      */
void string_append( CHAR_DATA *ch, char **pString )
{
    send_to_char( "-=======- Entering APPEND Mode -========-\n\r", ch );
    send_to_char( "    Type .h on a new line for help\n\r", ch );
    send_to_char( " Terminate with a ~ or @ on a blank line.\n\r", ch );
    send_to_char( "-=======================================-\n\r", ch );
    if ( *pString == NULL )
        *pString = str_dup( "" );
    send_to_char( *pString, ch );
    if ( *(*pString + strlen( *pString ) - 1) != '\r' )
    send_to_char( "\n\r", ch );
    ch->desc->pString = pString;
    return;
}

/* Substitutes one string for another.                      *
 * Called by string_add(string.c) (aedit_builder)olc_act.c. */
char * string_replace( char * orig, char * old, char * new )
{
    char xbuf[MSL];
    int i;
    xbuf[0] = '\0';
    strcpy( xbuf, orig );
    if ( strstr( orig, old ) != NULL )
    {
        i = strlen( orig ) - strlen( strstr( orig, old ) );
        xbuf[i] = '\0';
        strcat( xbuf, new );
        strcat( xbuf, &orig[i+strlen( old )] );
        free_string( orig );
    }
    return str_dup( xbuf );
}

/* Interpreter for string editing.   *
 * Called by game_loop_xxxx(comm.c). */
void string_add( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    smash_tilde( argument );
    if ( *argument == '.' )
    {
        char arg1 [MIL];
        char arg2 [MIL];
        char arg3 [MIL];
        argument = one_argument( argument, arg1 );
        argument = first_arg( argument, arg2, FALSE );
        argument = first_arg( argument, arg3, FALSE );
        if ( !str_cmp( arg1, ".c" ) )
        {
            send_to_char( "String cleared.\n\r", ch );
	    free_string( *ch->desc->pString );
	    *ch->desc->pString = str_dup( "" );
            return;
        }
        if ( !str_cmp( arg1, ".s" ) )
        {
            send_to_char( "String so far:\n\r", ch );
            send_to_char( *ch->desc->pString, ch );
            return;
        }
        if ( !str_cmp( arg1, ".r" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char(
                    "usage:  .r \"old string\" \"new string\"\n\r", ch );
                return;
            }
            smash_tilde( arg3 );
            *ch->desc->pString = string_replace( *ch->desc->pString, arg2, arg3 );
            sprintf( buf, "'%s' replaced with '%s'.\n\r", arg2, arg3 );
            send_to_char( buf, ch );
            return;
        }
        if ( !str_cmp( arg1, ".f" ) )
        {
            *ch->desc->pString = format_string( *ch->desc->pString );
            send_to_char( "String formatted.\n\r", ch );
            return;
        }
        if ( !str_cmp( arg1, ".h" ) )
        {
            send_to_char( "Sedit help (commands on blank line):   \n\r", ch );
            send_to_char( ".r 'old' 'new'   - replace a substring \n\r", ch );
            send_to_char( "                   (requires '', \"\") \n\r", ch );
            send_to_char( ".h               - get help (this info)\n\r", ch );
            send_to_char( ".s               - show string so far  \n\r", ch );
            send_to_char( ".f               - (word wrap) string  \n\r", ch );
            send_to_char( ".c               - clear string so far \n\r", ch );
            send_to_char( "@                - end string          \n\r", ch );
            return;
        }
        send_to_char( "SEdit:  Invalid dot command.\n\r", ch );
        return;
    }
    if ( *argument == '~' || *argument == '@' )
    {
      if ( ch->desc->editor == ED_MPCODE ) /* for the mobprogs */
	{
	  MOB_INDEX_DATA *mob;
	  int hash;
	  PROG_LIST *mpl;
	  PROG_CODE *mpc;

	  EDIT_MPCODE(ch, mpc);

	  if ( mpc != NULL )
	    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
	      for ( mob = mob_index_hash[hash]; mob; mob = mob->next )
		for ( mpl = mob->mprogs; mpl; mpl = mpl->next )
		  if ( mpl->vnum == mpc->vnum )
		    {
		      sprintf( buf, "Fixing mob %d.\n\r", mob->vnum );
		      send_to_char( buf, ch );
		      mpl->code = mpc->code;
		    }
	}
      if ( ch->desc->editor == ED_OPCODE ) /* for the objprogs */
	{
	  OBJ_INDEX_DATA *obj;
	  int hash;
	  PROG_LIST *opl;
	  PROG_CODE *opc;

	  EDIT_OPCODE(ch, opc);

	  if ( opc != NULL )
	    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
	      for ( obj = obj_index_hash[hash]; obj; obj = obj->next )
		for ( opl = obj->oprogs; opl; opl = opl->next )
		  if ( opl->vnum == opc->vnum )
		    {
		      sprintf( buf, "Fixing object %d.\n\r", obj->vnum );
		      send_to_char( buf, ch );
		      opl->code = opc->code;
		    }
	}

      if ( ch->desc->editor == ED_RPCODE ) /* for the roomprogs */
	{
	  ROOM_INDEX_DATA *room;
	  int hash;
	  PROG_LIST *rpl;
	  PROG_CODE *rpc;

	  EDIT_RPCODE(ch, rpc);

	  if ( rpc != NULL )
	    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
	      for ( room = room_index_hash[hash]; room; room = room->next )
		for ( rpl = room->rprogs; rpl; rpl = rpl->next )
		  if ( rpl->vnum == rpc->vnum )
		    {
		      sprintf( buf, "Fixing room %d.\n\r", room->vnum );
		      send_to_char( buf, ch );
		      rpl->code = rpc->code;
		    }
	}


      ch->desc->pString = NULL;
      return;
    }
    strcpy( buf, *ch->desc->pString );
    if ( strlen( buf ) + strlen( argument ) >= ( MSL - 4 ) )
      {
        send_to_char( "String too long, last line skipped.\n\r", ch );
        ch->desc->pString = NULL;
        return;
      }
    smash_tilde( argument );
    strcat( buf, argument );
    strcat( buf, "\n\r" );
    free_string( *ch->desc->pString );
    *ch->desc->pString = str_dup( buf );
    return;
}

/* Special string formating and word-wrapping.    *
 * Called by string_add(string.c) (many)olc_act.c */
char *format_string( char *oldstring )
{
    char xbuf[MSL];
    char xbuf2[MSL];
    char *rdesc;
    int i=0;
    bool cap=TRUE;
    xbuf[0]=xbuf2[0]=0;
    i=0;

    for (rdesc = oldstring; *rdesc; rdesc++){
    /* New Line */
      if (*rdesc=='\n'){
	/* check for empty seperator lines */
	if (*(rdesc + 1) && *(rdesc + 2) && *(rdesc + 3)
	    && *(rdesc + 2) == ' ' && *(rdesc + 3) == '\n'){
	  xbuf[i++]= '\n';
	  xbuf[i++]= '\r';
	  xbuf[i++]= '\n';
	  xbuf[i++]= '\r';
	  rdesc += 4;
	}
	else if (xbuf[i-1] != ' '){
	  xbuf[i]=' ';
	  i++;
	}
      }
      /* return */

      else if (*rdesc=='\r'){
	if (xbuf[i-1] != ' '){
	  xbuf[i]=' ';
	  i++;
	}
      }
    /* Space */
      else if (*rdesc==' '){
	if (xbuf[i-1] != ' '){
	  xbuf[i]=' ';
	  i++;
        }
      }
    /* bracket */
      else if (*rdesc==')'){
	if (xbuf[i-1]==' ' && xbuf[i-2]==' '
	    && (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!')){
	  xbuf[i-2]=*rdesc;
	  xbuf[i-1]=' ';
	  xbuf[i]=' ';
	  i++;
	}
	else{
	  xbuf[i]=*rdesc;
	  i++;
	}
      }
      else if (*rdesc=='.' || *rdesc=='?' || *rdesc=='!'){
	if (xbuf[i-1]==' ' && xbuf[i-2]==' '
	    && (xbuf[i-3]=='.' || xbuf[i-3]=='?' || xbuf[i-3]=='!')) {
	  xbuf[i-2]=*rdesc;
	  if (*(rdesc+1) != '\"'){
	    xbuf[i-1]=' ';
	    xbuf[i]=' ';
	    i++;
	  }
	  else{
	    xbuf[i-1]='\"';
	    xbuf[i]=' ';
	    xbuf[i+1]=' ';
	    i+=2;
	    rdesc++;
	  }
        }
        else{
	  xbuf[i]=*rdesc;
	  if (*(rdesc+1) != '\"'){
	    xbuf[i+1]=' ';
	    xbuf[i+2]=' ';
	    i += 3;
	  }
	  else{
	    xbuf[i+1]='\"';
	    xbuf[i+2]=' ';
	    xbuf[i+3]=' ';
	    i += 4;
	    rdesc++;
	  }
        }
        cap = TRUE;
      }
      else{
	xbuf[i]=*rdesc;
	if ( cap ){
	  cap = FALSE;
	  xbuf[i] = UPPER( xbuf[i] );
	}
	i++;
      }
    }
    xbuf[i]=0;
    strcpy(xbuf2,xbuf);
    rdesc=xbuf2;
    xbuf[0]=0;

    for ( ; ; )
    {
      bool fSkip = FALSE;
        for (i=0; i<77; i++)
            if (!*(rdesc+i)) break;

        if (i<77)
            break;

	/* check for already existing returns first */
        for (i=(xbuf[0]?76:73) ; i ; i--){
	  if (*(rdesc+i)=='\r'){
            *(rdesc+i)=0;
            strcat(xbuf,rdesc);
            strcat(xbuf,"\r");
            rdesc += i+1;
            while (*rdesc == ' ') rdesc++;
	    fSkip = TRUE;
	    break;
	  }
	}
	if (fSkip)
	  continue;

	/* check for spaces now */
        for (i=(xbuf[0]?76:73) ; i ; i--)
            if (*(rdesc+i)==' ')  break;
        if (i)
        {
            *(rdesc+i)=0;
            strcat(xbuf,rdesc);
            strcat(xbuf,"\n\r");
            rdesc += i+1;
            while (*rdesc == ' ') rdesc++;
        }
        else
        {
	  //bug ("No spaces", 0);
            *(rdesc+75)=0;
            strcat(xbuf,rdesc);
            strcat(xbuf,"-\n\r");
            rdesc += 76;
        }
    }

    while (*(rdesc+i) && (*(rdesc+i)==' ' ))
      //			  || *(rdesc+i)=='\n' || *(rdesc+i)=='\r'))
      i--;
    *(rdesc+i+1)=0;
    strcat(xbuf,rdesc);
    if (xbuf[strlen(xbuf)-2] != '\n')
        strcat(xbuf,"\n\r");
    free_string(oldstring);
    return(str_dup(xbuf));
}

/* Used above in string_add.  Because this function does not *
 * modify case if fCase is FALSE and because it understands  *
 * parenthesis, it would probably make a nice replacement    *
 * for one_argument.                                         */
/* Pick off one argument from a string and return the rest.         *
 * Understands quates, parenthesis (barring ) ('s) and percentages. *
 * Called by string_add(string.c)                                   */
char *first_arg( char *argument, char *arg_first, bool fCase )
{
    char cEnd;
    while ( *argument == ' ' )
	argument++;
    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' || *argument == '%'  || *argument == '(' )
    {
        if ( *argument == '(' )
        {
            cEnd = ')';
            argument++;
        }
        else cEnd = *argument++;
    }
    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
        if ( fCase )
            *arg_first = LOWER(*argument);
        else
            *arg_first = *argument;
	arg_first++;
	argument++;
    }
    *arg_first = '\0';
    while ( *argument == ' ' )
	argument++;
    return argument;
}

/* Used in olc_act.c for aedit_builders. */
char * string_unpad( char * argument )
{
    char buf[MSL];
    char *s;
    s = argument;
    while ( *s == ' ' )
        s++;
    strcpy( buf, s );
    s = buf;
    if ( *s != '\0' )
    {
        while ( *s != '\0' )
            s++;
        s--;
        while( *s == ' ' )
            s--;
        s++;
        *s = '\0';
    }
    free_string( argument );
    return str_dup( buf );
}

/* Same as capitalize but changes the pointer's data. *
 * Used in olc_act.c in aedit_builder.                */
char * string_proper( char * argument )
{
    char *s;
    s = argument;
    while ( *s != '\0' )
    {
        if ( *s != ' ' )
        {
            *s = UPPER(*s);
            while ( *s != ' ' && *s != '\0' )
                s++;
        }
        else
            s++;
    }
    return argument;
}


PROG_CODE *new_mpcode(void)
{
     PROG_CODE *NewCode;

     if (!mpcode_free)
     {
         NewCode = alloc_perm(sizeof(*NewCode) );
         top_mprog_index++;
     }
     else
     {
         NewCode     = mpcode_free;
         mpcode_free = mpcode_free->next;
     }

     NewCode->vnum    = 0;
     NewCode->code    = str_dup("");
     NewCode->next    = NULL;

     return NewCode;
}

PROG_CODE *new_opcode(void)
{
     PROG_CODE *NewCode;

     if (!opcode_free)
     {
         NewCode = alloc_perm(sizeof(*NewCode) );
         top_oprog_index++;
     }
     else
     {
         NewCode     = opcode_free;
         opcode_free = opcode_free->next;
     }

     NewCode->vnum    = 0;
     NewCode->code    = str_dup("");
     NewCode->next    = NULL;

     return NewCode;
}

PROG_CODE *new_rpcode(void)
{
     PROG_CODE *NewCode;

     if (!rpcode_free)
     {
         NewCode = alloc_perm(sizeof(*NewCode) );
         top_rprog_index++;
     }
     else
     {
         NewCode     = rpcode_free;
         rpcode_free = rpcode_free->next;
     }

     NewCode->vnum    = 0;
     NewCode->code    = str_dup("");
     NewCode->next    = NULL;

     return NewCode;
}

void free_opcode(PROG_CODE *pOcode)
{
    free_string(pOcode->code);
    pOcode->next = opcode_free;
    opcode_free  = pOcode;
    return;
}

void free_rpcode(PROG_CODE *pRcode)
{
    free_string(pRcode->code);
    pRcode->next = rpcode_free;
    rpcode_free  = pRcode;
    return;
}


void free_mpcode(PROG_CODE *pMcode)
{
    free_string(pMcode->code);
    pMcode->next = mpcode_free;
    mpcode_free  = pMcode;
    return;
}
