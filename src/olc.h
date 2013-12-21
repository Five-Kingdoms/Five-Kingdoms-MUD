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


#define VERSION	"ILAB Online Creation [Beta 1.0, ROM 2.3 modified]\n\r" \
		"     Port a ROM 2.4 v1.00\n\r"
#define AUTHOR	"     By Jason(jdinkel@mines.colorado.edu)\n\r" \
                "     Modified for use with ROM 2.3\n\r"        \
                "     By Hans Birkeland (hansbi@ifi.uio.no)\n\r" \
                "     Modificado para uso en ROM 2.4v4\n\r"	\
                "     Por Birdie (itoledo@ramses.centic.utem.cl)\n\r"
#define DATE	"     (Apr. 7, 1995 - ROM mod, Apr 16, 1995)\n\r" \
		"     (Port a ROM 2.4 - Nov 2, 1996)\n\r"
#define CREDITS "     Original by Surreality(cxw197@psu.edu) and Locke(locke@lm.com)"

/* New typedefs. */
typedef	bool OLC_FUN		args( ( CHAR_DATA *ch, char *argument ) );
#define DECLARE_OLC_FUN( fun )	OLC_FUN    fun

/* Command procedures needed ROM OLC */
DECLARE_DO_FUN(    do_help    );
DECLARE_SPELL_FUN( spell_null );

/* Connected states for editor. */
#define ED_AREA		1
#define ED_ROOM		2
#define ED_OBJECT	3
#define ED_MOBILE	4
#define ED_MPCODE       5
#define ED_OPCODE       6
#define ED_RPCODE       7
#define ED_TRAP		8
#define ED_HELP		9
#define ED_CABAL	10
#define ED_ARMY		11


/* Interpreter Prototypes */
void    aedit           args( ( CHAR_DATA *ch, char *argument ) );
void    redit           args( ( CHAR_DATA *ch, char *argument ) );
void    medit           args( ( CHAR_DATA *ch, char *argument ) );
void    armedit         args( ( CHAR_DATA *ch, char *argument ) );
void    oedit           args( ( CHAR_DATA *ch, char *argument ) );
void    tedit           args( ( CHAR_DATA *ch, char *argument ) );
void    hedit           args( ( CHAR_DATA *ch, char *argument ) );
void    cedit           args( ( CHAR_DATA *ch, char *argument ) );
void    mpedit          args( ( CHAR_DATA *ch, char *argument ) );
void    opedit          args( ( CHAR_DATA *ch, char *argument ) );
void    rpedit          args( ( CHAR_DATA *ch, char *argument ) );


/* OLC Constants */
#define MAX_MOB	1		/* Default maximum number for resetting mobs */

/* Structure for an OLC editor command. */
struct olc_cmd_type
{
    char * const	name;
    OLC_FUN *		olc_fun;
};

/* Structure for an OLC editor startup command. */
struct	editor_cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
};

/* Utils. */
AREA_DATA *get_vnum_area	args ( ( int vnum ) );
AREA_DATA *get_area_data	args ( ( int vnum ) );
AREA_DATA *get_area_data_str	( char* name );
int flag_value                  args ( ( const struct flag_type *flag_table, char *argument) );
char *flag_string               args ( ( const struct flag_type *flag_table, int bits ) );
void add_reset                  args ( ( ROOM_INDEX_DATA *room, RESET_DATA *pReset, int index ) );

/* Interpreter Table Prototypes */
extern const struct olc_cmd_type	aedit_table[];
extern const struct olc_cmd_type	redit_table[];
extern const struct olc_cmd_type	oedit_table[];
extern const struct olc_cmd_type	medit_table[];
extern const struct olc_cmd_type	tedit_table[];
extern const struct olc_cmd_type	hedit_table[];
extern const struct olc_cmd_type	cedit_table[];
extern const struct olc_cmd_type        mpedit_table[];
extern const struct olc_cmd_type        opedit_table[];
extern const struct olc_cmd_type        rpedit_table[];

/* Editor Commands. */
DECLARE_DO_FUN( do_aedit        );
DECLARE_DO_FUN( do_redit        );
DECLARE_DO_FUN( do_oedit        );
DECLARE_DO_FUN( do_medit        );
DECLARE_DO_FUN( do_armedit      );
DECLARE_DO_FUN( do_tedit        );
DECLARE_DO_FUN( do_hedit        );
DECLARE_DO_FUN( do_cedit        );
DECLARE_DO_FUN( do_mpedit       );
DECLARE_DO_FUN( do_opedit       );
DECLARE_DO_FUN( do_rpedit       ); 

void save_area( AREA_DATA *pArea );

/* General Functions */
bool show_commands		args ( ( CHAR_DATA *ch, char *argument ) );
bool show_help			args ( ( CHAR_DATA *ch, char *argument ) );
bool edit_done			args ( ( CHAR_DATA *ch ) );
bool show_version		args ( ( CHAR_DATA *ch, char *argument ) );

/* Area Editor Prototypes */
DECLARE_OLC_FUN( aedit_show		);
DECLARE_OLC_FUN( aedit_create		);
DECLARE_OLC_FUN( aedit_crime		);
DECLARE_OLC_FUN( aedit_name		);
DECLARE_OLC_FUN( aedit_prefix		);
DECLARE_OLC_FUN( aedit_file		);
DECLARE_OLC_FUN( aedit_age		);
DECLARE_OLC_FUN( aedit_bastion		);
DECLARE_OLC_FUN( aedit_reset		);
DECLARE_OLC_FUN( aedit_security		);
DECLARE_OLC_FUN( aedit_builder		);
DECLARE_OLC_FUN( aedit_vnum		);
DECLARE_OLC_FUN( aedit_lvnum		);
DECLARE_OLC_FUN( aedit_uvnum		);
DECLARE_OLC_FUN( aedit_credits		);
DECLARE_OLC_FUN( aedit_flags		);
DECLARE_OLC_FUN( aedit_startroom	);

/* Room Editor Prototypes */
DECLARE_OLC_FUN( redit_show		);
DECLARE_OLC_FUN( redit_create		);
DECLARE_OLC_FUN( redit_name		);
DECLARE_OLC_FUN( redit_desc		);
DECLARE_OLC_FUN( redit_ndesc		);
DECLARE_OLC_FUN( redit_ed		);
DECLARE_OLC_FUN( redit_format		);
DECLARE_OLC_FUN( redit_north		);
DECLARE_OLC_FUN( redit_south		);
DECLARE_OLC_FUN( redit_east		);
DECLARE_OLC_FUN( redit_west		);
DECLARE_OLC_FUN( redit_up		);
DECLARE_OLC_FUN( redit_down		);
DECLARE_OLC_FUN( redit_mreset		);
DECLARE_OLC_FUN( redit_oreset		);
DECLARE_OLC_FUN( redit_treset		);
DECLARE_OLC_FUN( redit_mlist		);
DECLARE_OLC_FUN( redit_rlist		);
DECLARE_OLC_FUN( redit_olist		);
DECLARE_OLC_FUN( redit_mshow		);
DECLARE_OLC_FUN( redit_oshow		);
DECLARE_OLC_FUN( redit_heal		);
DECLARE_OLC_FUN( redit_mana		);
DECLARE_OLC_FUN( redit_watch		);
DECLARE_OLC_FUN( redit_temp		);
DECLARE_OLC_FUN( redit_cabal            );
DECLARE_OLC_FUN( redit_owner		);
DECLARE_OLC_FUN( redit_sector		);
DECLARE_OLC_FUN( redit_room		);
DECLARE_OLC_FUN( redit_copy		);
DECLARE_OLC_FUN( redit_addrprog		);
DECLARE_OLC_FUN( redit_delrprog		);

/* Object Editor Prototypes */
DECLARE_OLC_FUN( oedit_show		);
DECLARE_OLC_FUN( oedit_create		);
DECLARE_OLC_FUN( oedit_name		);
DECLARE_OLC_FUN( oedit_short		);
DECLARE_OLC_FUN( oedit_long		);
DECLARE_OLC_FUN( oedit_addaffect	);
DECLARE_OLC_FUN( oedit_addapply         );
DECLARE_OLC_FUN( oedit_delaffect	);
DECLARE_OLC_FUN( oedit_value0		);
DECLARE_OLC_FUN( oedit_value1		);
DECLARE_OLC_FUN( oedit_value2		);
DECLARE_OLC_FUN( oedit_value3		);
DECLARE_OLC_FUN( oedit_value4           );
DECLARE_OLC_FUN( oedit_weight		);
DECLARE_OLC_FUN( oedit_condition	);
DECLARE_OLC_FUN( oedit_material		);
DECLARE_OLC_FUN( oedit_cost		);
DECLARE_OLC_FUN( oedit_cabal		);
DECLARE_OLC_FUN( oedit_race		);
DECLARE_OLC_FUN( oedit_class		);
DECLARE_OLC_FUN( oedit_message		);
DECLARE_OLC_FUN( oedit_ed		);
DECLARE_OLC_FUN( oedit_extra            );  
DECLARE_OLC_FUN( oedit_wear             );  
DECLARE_OLC_FUN( oedit_type             );  
DECLARE_OLC_FUN( oedit_affect           );  
DECLARE_OLC_FUN( oedit_material         );  
DECLARE_OLC_FUN( oedit_level            );  
DECLARE_OLC_FUN( oedit_copy		);
//DECLARE_OLC_FUN( oedit_trap		);
DECLARE_OLC_FUN( oedit_addoprog		);
DECLARE_OLC_FUN( oedit_deloprog		);

/* Mobile Editor Prototypes */
DECLARE_OLC_FUN( medit_show		);
DECLARE_OLC_FUN( medit_create		);
DECLARE_OLC_FUN( medit_name		);
DECLARE_OLC_FUN( medit_short		);
DECLARE_OLC_FUN( medit_long		);
DECLARE_OLC_FUN( medit_shop		);
DECLARE_OLC_FUN( medit_desc		);
DECLARE_OLC_FUN( medit_level		);
DECLARE_OLC_FUN( medit_align		);
DECLARE_OLC_FUN( medit_spec		);
DECLARE_OLC_FUN( medit_sex              );  
DECLARE_OLC_FUN( medit_cabal            );  
DECLARE_OLC_FUN( medit_act              );  
DECLARE_OLC_FUN( medit_act2             );  
DECLARE_OLC_FUN( medit_affect           );  
DECLARE_OLC_FUN( medit_affect2          );  
DECLARE_OLC_FUN( medit_ac               );  
DECLARE_OLC_FUN( medit_imm              );  
DECLARE_OLC_FUN( medit_res              );  
DECLARE_OLC_FUN( medit_vuln             );  
DECLARE_OLC_FUN( medit_material         );  
DECLARE_OLC_FUN( medit_off              );  
DECLARE_OLC_FUN( medit_size             );  
DECLARE_OLC_FUN( medit_hitdice          );  
DECLARE_OLC_FUN( medit_manadice         );  
DECLARE_OLC_FUN( medit_damdice          );  
DECLARE_OLC_FUN( medit_race             );  
DECLARE_OLC_FUN( medit_position         );  
DECLARE_OLC_FUN( medit_gold             );  
DECLARE_OLC_FUN( medit_tarifa           );  
DECLARE_OLC_FUN( medit_hitroll          );  
DECLARE_OLC_FUN( medit_camino           );  
DECLARE_OLC_FUN( medit_damtype          );  
DECLARE_OLC_FUN( medit_group		);
DECLARE_OLC_FUN( medit_copy		);
DECLARE_OLC_FUN( medit_addmprog		);
DECLARE_OLC_FUN( medit_delmprog		);

/* army functions */
DECLARE_OLC_FUN( armedit_show		);
DECLARE_OLC_FUN( armedit_copy		);
DECLARE_OLC_FUN( armedit_create		);
DECLARE_OLC_FUN( armedit_type		);
DECLARE_OLC_FUN( armedit_cost		);
DECLARE_OLC_FUN( armedit_support	);
DECLARE_OLC_FUN( armedit_flags		);
DECLARE_OLC_FUN( armedit_offense	);
DECLARE_OLC_FUN( armedit_hitpoint	);
DECLARE_OLC_FUN( armedit_armor		);
DECLARE_OLC_FUN( armedit_noun		);
DECLARE_OLC_FUN( armedit_short		);
DECLARE_OLC_FUN( armedit_long		);
DECLARE_OLC_FUN( armedit_descr		);

/* trap functions */
DECLARE_OLC_FUN( tedit_create		);
DECLARE_OLC_FUN( tedit_show		);
DECLARE_OLC_FUN( tedit_name		);
DECLARE_OLC_FUN( tedit_echo		);
DECLARE_OLC_FUN( tedit_oecho		);
DECLARE_OLC_FUN( tedit_level		);
DECLARE_OLC_FUN( tedit_type		);
DECLARE_OLC_FUN( tedit_flag		);
DECLARE_OLC_FUN( tedit_v0		);
DECLARE_OLC_FUN( tedit_v1		);
DECLARE_OLC_FUN( tedit_v2		);
DECLARE_OLC_FUN( tedit_v3		);
DECLARE_OLC_FUN( tedit_v4		);

/* help functions */
DECLARE_OLC_FUN( hedit_create		);
DECLARE_OLC_FUN( hedit_show		);
DECLARE_OLC_FUN( hedit_key		);
DECLARE_OLC_FUN( hedit_level		);
DECLARE_OLC_FUN( hedit_type		);
DECLARE_OLC_FUN( hedit_text		);

/* cabal functions */
DECLARE_OLC_FUN( cedit_create		);
DECLARE_OLC_FUN( cedit_show		);
DECLARE_OLC_FUN( cedit_name		);
DECLARE_OLC_FUN( cedit_file_name	);
DECLARE_OLC_FUN( cedit_msggate		);
DECLARE_OLC_FUN( cedit_ongate		);
DECLARE_OLC_FUN( cedit_offgate		);
DECLARE_OLC_FUN( cedit_city		);
DECLARE_OLC_FUN( cedit_anchor		);
DECLARE_OLC_FUN( cedit_guard		);
DECLARE_OLC_FUN( cedit_whoname		);
DECLARE_OLC_FUN( cedit_imm		);
DECLARE_OLC_FUN( cedit_currency		);
DECLARE_OLC_FUN( cedit_clan		);
DECLARE_OLC_FUN( cedit_members		);
DECLARE_OLC_FUN( cedit_max_room		);
DECLARE_OLC_FUN( cedit_levels		);
DECLARE_OLC_FUN( cedit_rank		);
DECLARE_OLC_FUN( cedit_mrank		);
DECLARE_OLC_FUN( cedit_frank		);
DECLARE_OLC_FUN( cedit_flag		);
DECLARE_OLC_FUN( cedit_ethos		);
DECLARE_OLC_FUN( cedit_align		);
DECLARE_OLC_FUN( cedit_race		);
DECLARE_OLC_FUN( cedit_class		);
DECLARE_OLC_FUN( cedit_skill		);
DECLARE_OLC_FUN( cedit_vote		);
DECLARE_OLC_FUN( cedit_room		);
DECLARE_OLC_FUN( cedit_avatar		);
DECLARE_OLC_FUN( cedit_parent		);
DECLARE_OLC_FUN( cedit_progress		);
DECLARE_OLC_FUN( cedit_pit		);
DECLARE_OLC_FUN( cedit_army		);
DECLARE_OLC_FUN( cedit_enemy		);
DECLARE_OLC_FUN( cedit_tower		);
DECLARE_OLC_FUN( cedit_prefix		);
DECLARE_OLC_FUN( cedit_tax		);

/* MACROS */
#define IS_SWITCHED( ch )       ( ch->desc->original )
#define IS_BUILDER(ch, Area)   ( !IS_SWITCHED( ch ) && (ch->pcdata->security >= Area->security  || strstr( Area->builders, ch->name ) || strstr( Area->builders, "All" ) ) )
#define EDIT_MPCODE(Ch, Code)   ( Code = (PROG_CODE*)Ch->desc->pEdit )
#define EDIT_OPCODE(Ch, Code)   ( Code = (PROG_CODE*)Ch->desc->pEdit )
#define EDIT_RPCODE(Ch, Code)   ( Code = (PROG_CODE*)Ch->desc->pEdit )

/* Return pointers to what is being edited. */
#define EDIT_MOB(Ch, Mob)	( Mob = (MOB_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_ARMY(Ch, Army)	( Army = (ARMY_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_OBJ(Ch, Obj)	( Obj = (OBJ_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_HELP(Ch, Help)	( Help = (HELP_DATA	*)Ch->desc->pEdit )
#define EDIT_CABAL(Ch, Cabal)	( Cabal = (CABAL_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_TRAP(Ch, Trap)	( Trap = (TRAP_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_ROOM(Ch, Room)	( Room = Ch->in_room )
#define EDIT_AREA(Ch, Area)	( Area = (AREA_DATA *)Ch->desc->pEdit )


/* Prototypes */
/* mem.c - memory prototypes. */
#define ED	EXTRA_DESCR_DATA
RESET_DATA	*new_reset_data		args ( ( void ) );
void		free_reset_data		args ( ( RESET_DATA *pReset ) );
AREA_DATA	*new_area		args ( ( void ) );
void		free_area		args ( ( AREA_DATA *pArea ) );
EXIT_DATA	*new_exit		args ( ( void ) );
void		free_exit		args ( ( EXIT_DATA *pExit ) );
ED 		*new_extra_descr	args ( ( void ) );
void		free_extra_descr	args ( ( ED *pExtra ) );
ROOM_INDEX_DATA *new_room_index		args ( ( void ) );
void		free_room_index		args ( ( ROOM_INDEX_DATA *pRoom ) );
void		free_oprog              args ( ( PROG_LIST *op ) );
void		free_rprog              args ( ( PROG_LIST *rp ) );

AFFECT_DATA	*new_affect		args ( ( void ) );
SKILL_DATA	*new_skill		args ( ( void ) );
void		free_affect		args ( ( AFFECT_DATA* pAf ) );
SHOP_DATA	*new_shop		args ( ( void ) );
void		free_shop		args ( ( SHOP_DATA *pShop ) );
OBJ_INDEX_DATA	*new_obj_index		args ( ( void ) );
void		free_obj_index		args ( ( OBJ_INDEX_DATA *pObj ) );
MOB_INDEX_DATA	*new_mob_index		args ( ( void ) );
void		free_mob_index		args ( ( MOB_INDEX_DATA *pMob ) );
#undef	ED
void           show_liqlist            args ( ( CHAR_DATA *ch ) );
void           show_damlist            args ( ( CHAR_DATA *ch ) );
char *         prog_type_to_name       args ( ( int type ) );
PROG_LIST	*new_mprog		args ( ( void ) );
void		free_mprog		args ( ( PROG_LIST *op ) );
PROG_LIST	*new_oprog		args ( ( void ) );
void		free_oprog		args ( ( PROG_LIST *op ) );
PROG_LIST	*new_rprog		args ( ( void ) );
void		free_rprog		args ( ( PROG_LIST *rp ) );

PROG_CODE	*new_mpcode		args ( ( void ) );
void		free_mpcode		args ( ( PROG_CODE *pOcode ) );
PROG_CODE	*new_opcode		args ( ( void ) );
void		free_opcode		args ( ( PROG_CODE *pOcode ) );
PROG_CODE	*new_rpcode		args ( ( void ) );
void		free_rpcode		args ( ( PROG_CODE *pRcode ) );

/* Mobprog editor */
DECLARE_OLC_FUN( mpedit_create		);
DECLARE_OLC_FUN( mpedit_code		);
DECLARE_OLC_FUN( mpedit_show		);
DECLARE_OLC_FUN( mpedit_list		);

/* Objprog editor */
DECLARE_OLC_FUN( opedit_create		);
DECLARE_OLC_FUN( opedit_code		);
DECLARE_OLC_FUN( opedit_show		);
DECLARE_OLC_FUN( opedit_list		);

/* Roomprog editor */
DECLARE_OLC_FUN( rpedit_create		);
DECLARE_OLC_FUN( rpedit_code		);
DECLARE_OLC_FUN( rpedit_show		);
DECLARE_OLC_FUN( rpedit_list		);

/* other */
void		write_room		( FILE* fp, ROOM_INDEX_DATA* pRoomIndex );
void		write_resets		( FILE* fp, ROOM_INDEX_DATA* pRoom );
int		flag_lookup_abs		(const char *name, const struct flag_type *flag_table);
