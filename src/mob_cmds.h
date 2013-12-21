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


struct	mob_cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
};

struct obj_cmd_type
{
    char * const	name;
    OBJ_FUN *		obj_fun;
};

struct room_cmd_type
{
    char * const	name;
    ROOM_FUN *		room_fun;
};

/* the command table itself */
extern	const	struct	mob_cmd_type	mob_cmd_table	[];

/*
 * MOBcommand functions.
 * Defined in mob_cmds.c
 */
DECLARE_DO_FUN(	do_mpasound	);
DECLARE_DO_FUN(	do_mpgecho	);
DECLARE_DO_FUN(	do_mpzecho	);
DECLARE_DO_FUN(	do_mpkill	);
DECLARE_DO_FUN(	do_mpassist	);
DECLARE_DO_FUN(	do_mpjunk	);
DECLARE_DO_FUN(	do_mpechoaround	);
DECLARE_DO_FUN(	do_mpecho	);
DECLARE_DO_FUN(	do_mpechoat	);
DECLARE_DO_FUN(	do_mpmload	);
DECLARE_DO_FUN(	do_mpoload	);
DECLARE_DO_FUN(	do_mppurge	);
DECLARE_DO_FUN(	do_mpgoto	);
DECLARE_DO_FUN(	do_mpat		);
DECLARE_DO_FUN(	do_mptransfer	);
DECLARE_DO_FUN(	do_mpgtransfer	);
DECLARE_DO_FUN(	do_mprtransfer	);
DECLARE_DO_FUN(	do_mpforce	);
DECLARE_DO_FUN(	do_mpgforce	);
DECLARE_DO_FUN(	do_mpvforce	);
DECLARE_DO_FUN(	do_mpcast	);
DECLARE_DO_FUN(	do_mpchown	);
DECLARE_DO_FUN(	do_mpdamage	);
DECLARE_DO_FUN(	do_mpremember	);
DECLARE_DO_FUN(	do_mpforget	);
DECLARE_DO_FUN(	do_mpdelay	);
DECLARE_DO_FUN(	do_mpcancel	);
DECLARE_DO_FUN(	do_mpcall	);
DECLARE_DO_FUN(	do_mpflee	);
DECLARE_DO_FUN(	do_mpotransfer	);
DECLARE_DO_FUN(	do_mpremove	);
DECLARE_DO_FUN(	do_mpstop	);
DECLARE_DO_FUN(	do_mpobey	);
DECLARE_DO_FUN(	do_mpattrib	);
DECLARE_DO_FUN(	do_mprattrib	);
DECLARE_DO_FUN(	do_mpgain	);
DECLARE_DO_FUN(	do_mppermquest	);
DECLARE_DO_FUN(	do_mpinviquest	);
DECLARE_DO_FUN(	do_mptempquest	);
DECLARE_DO_FUN(	do_mpremquest	);
DECLARE_DO_FUN(	do_mprempsalm	);
DECLARE_DO_FUN(	do_mpaddpsalm	);
DECLARE_DO_FUN(	do_mpfor	);
DECLARE_DO_FUN(	do_mpmount	);
DECLARE_DO_FUN(	do_mpdestroy	);
DECLARE_DO_FUN(	do_mppath	);
DECLARE_DO_FUN(	do_mpaquest	);
DECLARE_DO_FUN(	do_mpcquest	);


/*
 * OBJcommand functions
 * Defined in mob_cmds.c
 */
DECLARE_OBJ_FUN( do_opgecho	 );
DECLARE_OBJ_FUN( do_opzecho	 );
DECLARE_OBJ_FUN( do_opecho	 );
DECLARE_OBJ_FUN( do_opechoaround );
DECLARE_OBJ_FUN( do_opechoat	 );
DECLARE_OBJ_FUN( do_opmload	 );
DECLARE_OBJ_FUN( do_opoload	 );
DECLARE_OBJ_FUN( do_oppurge	 );
DECLARE_OBJ_FUN( do_opgoto	 );
DECLARE_OBJ_FUN( do_optransfer	 );
DECLARE_OBJ_FUN( do_opgtransfer	 );
DECLARE_OBJ_FUN( do_oprtransfer	 );
DECLARE_OBJ_FUN( do_opotransfer	 );
DECLARE_OBJ_FUN( do_opforce	 );
DECLARE_OBJ_FUN( do_opgforce	 );
DECLARE_OBJ_FUN( do_opvforce	 );
DECLARE_OBJ_FUN( do_opdamage	 );
DECLARE_OBJ_FUN( do_opremember	 );
DECLARE_OBJ_FUN( do_opforget	 );
DECLARE_OBJ_FUN( do_opdelay	 );
DECLARE_OBJ_FUN( do_opcancel	 );
DECLARE_OBJ_FUN( do_opcall	 );
DECLARE_OBJ_FUN( do_opremove	 );
DECLARE_OBJ_FUN( do_opattrib	 );
DECLARE_OBJ_FUN( do_oprattrib	 );
DECLARE_OBJ_FUN( do_opcast	 );
DECLARE_OBJ_FUN( do_opobey	 );
DECLARE_OBJ_FUN( do_opequipself	 );
DECLARE_OBJ_FUN( do_opstop	);
DECLARE_OBJ_FUN( do_opgain	);
DECLARE_OBJ_FUN( do_oppermquest	);
DECLARE_OBJ_FUN( do_opinviquest	);
DECLARE_OBJ_FUN( do_optempquest	);
DECLARE_OBJ_FUN( do_opremquest	);
DECLARE_OBJ_FUN( do_oprempsalm	);
DECLARE_OBJ_FUN( do_opaddpsalm	);
DECLARE_OBJ_FUN( do_opfor	);
DECLARE_OBJ_FUN( do_opaffstrip	);
DECLARE_OBJ_FUN( do_opmount	);
DECLARE_OBJ_FUN( do_opscoreboard);
DECLARE_OBJ_FUN( do_opdestroy	);
DECLARE_OBJ_FUN( do_oppath	);


/*
 * ROOMcommand functions
 * Defined in mob_cmds.c
 */
DECLARE_ROOM_FUN( do_rpasound	 );
DECLARE_ROOM_FUN( do_rpgecho	 );
DECLARE_ROOM_FUN( do_rpzecho	 );
DECLARE_ROOM_FUN( do_rpecho	 );
DECLARE_ROOM_FUN( do_rpechoaround);
DECLARE_ROOM_FUN( do_rpechoat	 );
DECLARE_ROOM_FUN( do_rpmload	 );
DECLARE_ROOM_FUN( do_rpoload	 );
DECLARE_ROOM_FUN( do_rppurge	 );
DECLARE_ROOM_FUN( do_rptransfer	 );
DECLARE_ROOM_FUN( do_rpgtransfer );
DECLARE_ROOM_FUN( do_rprtransfer );
DECLARE_ROOM_FUN( do_rpotransfer );
DECLARE_ROOM_FUN( do_rpforce	 );
DECLARE_ROOM_FUN( do_rpgforce	 );
DECLARE_ROOM_FUN( do_rpvforce	 );
DECLARE_ROOM_FUN( do_rpdamage	 );
DECLARE_ROOM_FUN( do_rpremember	 );
DECLARE_ROOM_FUN( do_rpforget	 );
DECLARE_ROOM_FUN( do_rpdelay	 );
DECLARE_ROOM_FUN( do_rpcancel	 );
DECLARE_ROOM_FUN( do_rpcall	 );
DECLARE_ROOM_FUN( do_rpremove	 );
DECLARE_ROOM_FUN( do_rpstop	 );
DECLARE_ROOM_FUN( do_rpobey	 );
DECLARE_ROOM_FUN( do_rpgain	 );
DECLARE_ROOM_FUN( do_rppermquest );
DECLARE_ROOM_FUN( do_rpinviquest );
DECLARE_ROOM_FUN( do_rptempquest );
DECLARE_ROOM_FUN( do_rpremquest  );
DECLARE_ROOM_FUN( do_rprempsalm  );
DECLARE_ROOM_FUN( do_rpaddpsalm  );
DECLARE_ROOM_FUN( do_rpfor	 );
DECLARE_ROOM_FUN( do_rprattrib	 );
DECLARE_ROOM_FUN( do_rpmount	 );
DECLARE_ROOM_FUN( do_rpdestroy	 );
DECLARE_ROOM_FUN( do_rppath	 );
