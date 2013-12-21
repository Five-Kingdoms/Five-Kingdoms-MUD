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
#ifndef __COMM_H__
#define __COMM_H__

void char_gen( DESCRIPTOR_DATA *d, char *argument );

extern struct chargen_choice_s chargen_body_table[CGEN_MAX_BODY];
extern struct chargen_choice_s chargen_face_table[CGEN_MAX_FACE];
extern struct chargen_choice_s chargen_skin_table[CGEN_MAX_SKIN];
extern struct chargen_choice_s chargen_eyes_table[CGEN_MAX_EYES];
extern struct chargen_choice_s chargen_hair_table[CGEN_MAX_HAIR];
extern struct chargen_choice_s chargen_hairl_table[CGEN_MAX_HAIRL];
extern struct chargen_choice_s chargen_hairt_table[CGEN_MAX_HAIRT];
extern struct chargen_choice_s chargen_hairf_table[CGEN_MAX_HAIRF];
extern struct chargen_choice_s chargen_perk_table[CGEN_MAX_PERK];



typedef void CHARGEN_FUN	( DESCRIPTOR_DATA* d, char* argument );
#define CHARGEN_FUN( func )	void func( DESCRIPTOR_DATA* d, char* argument )

extern const struct chargen_type {
  char*		name;	//name of the state
  char*		prompt;	//standart prompt
  CHARGEN_FUN*	func;	//function to run
}chargen_table[];

struct chargen_choice_s{
  char*		name;
  char*		string;
  int		int1;
  sh_int*	int2;
};

CHARGEN_FUN(	chargen_null	);
CHARGEN_FUN(	chargen_greet	);
CHARGEN_FUN(	chargen_main	);
CHARGEN_FUN(	chargen_help	);

CHARGEN_FUN(	chargen_enter	);
CHARGEN_FUN(	chargen_pass	);
CHARGEN_FUN(	chargen_immpass	);
CHARGEN_FUN(	chargen_motd	);
CHARGEN_FUN(	chargen_ready	);

CHARGEN_FUN(	chargen_create	);
CHARGEN_FUN(	chargen_con_name);
CHARGEN_FUN(	chargen_new_pass);
CHARGEN_FUN(	chargen_con_pass);
CHARGEN_FUN(	chargen_new_last);
CHARGEN_FUN(	chargen_con_last);
CHARGEN_FUN(	chargen_new_sex );
CHARGEN_FUN(	chargen_new_race);
CHARGEN_FUN(	chargen_con_race);
CHARGEN_FUN(	chargen_new_alig);
CHARGEN_FUN(	chargen_con_alig);
CHARGEN_FUN(	chargen_new_etho);
CHARGEN_FUN(	chargen_con_etho);
CHARGEN_FUN(	chargen_new_clas);
CHARGEN_FUN(	chargen_con_clas);
CHARGEN_FUN(	chargen_roll	);
CHARGEN_FUN(	chargen_new_reli);
CHARGEN_FUN(	chargen_con_reli);
CHARGEN_FUN(	chargen_new_home);
CHARGEN_FUN(	chargen_con_home);
CHARGEN_FUN(	chargen_ask_deta);
CHARGEN_FUN(	chargen_new_done);
CHARGEN_FUN(	chargen_con_break);

CHARGEN_FUN(	chargen_det_body);
CHARGEN_FUN(	chargen_det_face);
CHARGEN_FUN(	chargen_det_skin);
CHARGEN_FUN(	chargen_det_eyes);
CHARGEN_FUN(	chargen_det_hair);
CHARGEN_FUN(	chargen_det_haiL);
CHARGEN_FUN(	chargen_det_haiT);
CHARGEN_FUN(	chargen_det_haiF);
CHARGEN_FUN(	chargen_det_perk);
CHARGEN_FUN(	chargen_con_perk);
CHARGEN_FUN(	chargen_det_hand);
CHARGEN_FUN(	chargen_det_age);
CHARGEN_FUN(	chargen_con_age);
CHARGEN_FUN(	chargen_det_weap);

CHARGEN_FUN(	chargen_new_tele);
CHARGEN_FUN(	chargen_new_enemy);
CHARGEN_FUN(	chargen_new_adve);

CHARGEN_FUN(	chargen_nag_screen);
CHARGEN_FUN(	chargen_nag_lag);




#endif
