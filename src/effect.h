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

/* defines */
#define STUDY_LEARN        0
#define STUDY_MEMORIZE     1

/* Types of Living Weapon */
#define LIVWEP_NONE		0
#define LIVWEP_MAL		1
#define LIVWEP_CON		2
#define LIVWEP_HOL		3


/* The system flags of gen effects, stored in flag1 */
/* GLOBALED IN MERC.H
#define        EFF_F1_NONE         0     //No flags.
#define        EFF_F1_NOSAVE       (A)     //Effect is not saved.
#define        EFF_F1_NOEXTRACT    (B)     //UPDATE_KILL isn't run on extraction.
#define        EFF_F1_ALLOW_EXTEND (C)     //Spell is a normal spell effect and duration may be extended for benefit.
#define        EFF_F1_CMDBLOCK     (D)     //Blocks Commands from execution (high priority).
*/


/* definition of gen numbers, most will also need an extern in merc.h  */
sh_int			gen_firestorm;
sh_int			gen_mana_charge;
sh_int                  gen_gold_loss;
sh_int                  gen_guise;
sh_int                  gen_study;
//5
sh_int                  gen_insect_swarm;
sh_int                  gen_panic;
sh_int                  gen_malform;
sh_int                  gen_has_owner;
sh_int                  gen_dark_meta;
//10
sh_int                  gen_soul_reaver;
sh_int                  gen_handof_chaos;
sh_int                  gen_request;
sh_int                  gen_unlife;
sh_int                  gen_seremon;
//15
sh_int                  gen_holy_hands;
sh_int                  gen_voodoo_doll;
sh_int                  gen_pc_charm;
sh_int                  gen_blades;
sh_int                  gen_dan_blade;
//20
sh_int                  gen_terra_shield;
sh_int                  gen_plumbum;
sh_int                  gen_ref_shield;
sh_int			gen_anger;
sh_int			gen_discord;
//25
sh_int			gen_orb;
sh_int			gen_conceal;
sh_int			gen_d_shroud;
sh_int			gen_cspawn;
sh_int			gen_visit;
//30
sh_int			gen_sav_ward;
sh_int			gen_sav_cata;
sh_int			gen_challenge;
sh_int			gen_circle_of_protection;
sh_int			gen_yearning;
//35
sh_int			gen_ensnare;
sh_int			gen_mob_gen;
sh_int			gen_jail;
sh_int			gen_watchtower;
sh_int			gen_batter;
//40
sh_int			gen_addiction;
sh_int			gen_levitation;
sh_int			gen_siege;
sh_int			gen_photon_wall;
sh_int			gen_crusade;
//45
sh_int			gen_avenger;
sh_int			gen_bleed;
sh_int			gen_apierce;
sh_int			gen_chant;
sh_int			gen_dvoid;
//50
sh_int			gen_strium;
sh_int			gen_ward;
sh_int			gen_acraft;
sh_int			gen_wcraft;
sh_int			gen_hwep;
//55
sh_int			gen_approach;
sh_int			gen_suffocate;
sh_int			gen_contract;
sh_int			gen_spirit;
sh_int			gen_coup;
//60
sh_int			gen_diplomacy;
sh_int			gen_recruit;
sh_int			gen_vengeance;
sh_int			gen_conjure;
sh_int			gen_maze;
//65
sh_int			gen_chron;
sh_int			gen_paradox;
sh_int			gen_temp_avenger;
sh_int			gen_move_dam;
sh_int			gen_kinesis;
//70
sh_int			gen_create_object;
sh_int			gen_ainfection;
sh_int			gen_dreamwalk;
sh_int			gen_svise;
sh_int			gen_upgrade;
//75
sh_int			gen_metabolic_boost;
sh_int			gen_phantasm;
sh_int			gen_dragon_blood;
sh_int			gen_regenerate;
sh_int			gen_temp_storm;



/* definition of the gen EFF_UPDATE functions */
DECLARE_EFF_UPDATE(	general_kill		);

DECLARE_EFF_UPDATE(	firestorm_init		);
DECLARE_EFF_UPDATE(	firestorm_kill		);
DECLARE_EFF_UPDATE(     firestorm_precombat     );
DECLARE_EFF_UPDATE(     firestorm_enter         );
DECLARE_EFF_UPDATE(     firestorm_leave         );

DECLARE_EFF_UPDATE(     mana_charge_init        );
DECLARE_EFF_UPDATE(     mana_charge_kill        );
DECLARE_EFF_UPDATE(     mana_charge_prespell    );
DECLARE_EFF_UPDATE(     mana_charge_postspell   );

DECLARE_EFF_UPDATE(     gold_loss_init          );
DECLARE_EFF_UPDATE(     gold_loss_kill          );

DECLARE_EFF_UPDATE(     guise_init              );
DECLARE_EFF_UPDATE(     guise_kill              );

DECLARE_EFF_UPDATE(     study_none              );
DECLARE_EFF_UPDATE(     study_precommand	);
DECLARE_EFF_UPDATE(     study_init              );
DECLARE_EFF_UPDATE(     study_kill              );
DECLARE_EFF_UPDATE(     study_tick              );

DECLARE_EFF_UPDATE(     insect_swarm_init       );
DECLARE_EFF_UPDATE(     insect_swarm_kill       );
DECLARE_EFF_UPDATE(     insect_swarm_tick       );

DECLARE_EFF_UPDATE(	watch_init		);
DECLARE_EFF_UPDATE(	watch_kill		);
DECLARE_EFF_UPDATE(	watch_leave	);

DECLARE_EFF_UPDATE(     panic_init              );
DECLARE_EFF_UPDATE(     panic_postcombat        );
DECLARE_EFF_UPDATE(     panic_precommand        );
DECLARE_EFF_UPDATE(     panic_tick              );
DECLARE_EFF_UPDATE(     panic_kill              );

DECLARE_EFF_UPDATE(     malform_none            );
DECLARE_EFF_UPDATE(     malform_init	        );
DECLARE_EFF_UPDATE(     malform_predeath        );
DECLARE_EFF_UPDATE(     malform_postkill        );
DECLARE_EFF_UPDATE(     malform_objtick	        );

DECLARE_EFF_UPDATE(     has_owner_none	        );
DECLARE_EFF_UPDATE(     has_owner_init		);
DECLARE_EFF_UPDATE(     has_owner_kill		);
DECLARE_EFF_UPDATE(     has_owner_precommand    );
DECLARE_EFF_UPDATE(     has_owner_objtick       );

DECLARE_EFF_UPDATE(     dark_met_init		);
DECLARE_EFF_UPDATE(     dark_met_kill		);
DECLARE_EFF_UPDATE(     dark_met_postdamage	);

DECLARE_EFF_UPDATE(     soul_reaver_init	);
DECLARE_EFF_UPDATE(     soul_reaver_kill	);
DECLARE_EFF_UPDATE(     soul_reaver_postdamage	);
DECLARE_EFF_UPDATE(     soul_reaver_parry	);


DECLARE_EFF_UPDATE(     handof_chaos_init	);
DECLARE_EFF_UPDATE(     handof_chaos_kill	);
DECLARE_EFF_UPDATE(     handof_chaos_postdamage	);
DECLARE_EFF_UPDATE(     handof_chaos_parry	);

DECLARE_EFF_UPDATE(     request_none            );
DECLARE_EFF_UPDATE(     request_init            );
DECLARE_EFF_UPDATE(     request_kill            );
DECLARE_EFF_UPDATE(     request_end             );
DECLARE_EFF_UPDATE(     request_tick            );
DECLARE_EFF_UPDATE(     request_precommand      );

DECLARE_EFF_UPDATE(     unlife_init             );
DECLARE_EFF_UPDATE(     unlife_kill             );
DECLARE_EFF_UPDATE(     unlife_tick             );

DECLARE_EFF_UPDATE(     seremon_init            );
DECLARE_EFF_UPDATE(     seremon_kill            );
DECLARE_EFF_UPDATE(     seremon_tick		);

DECLARE_EFF_UPDATE(     holy_hands_init		);
DECLARE_EFF_UPDATE(     holy_hands_kill		);
DECLARE_EFF_UPDATE(     holy_hands_postdamage	);

DECLARE_EFF_UPDATE(     voodoo_doll_init	);
DECLARE_EFF_UPDATE(     voodoo_doll_kill	);


DECLARE_EFF_UPDATE(     pc_charm_init           );
DECLARE_EFF_UPDATE(     pc_charm_precommand     );
DECLARE_EFF_UPDATE(     pc_charm_predamage      );
DECLARE_EFF_UPDATE(     pc_charm_kill           );


DECLARE_EFF_UPDATE(     blades_init             );
DECLARE_EFF_UPDATE(     blades_kill             );
DECLARE_EFF_UPDATE(     blades_previo           );
DECLARE_EFF_UPDATE(     blades_tick             );
DECLARE_EFF_UPDATE(     blades_precombat        );
DECLARE_EFF_UPDATE(     blades_predamage        );

DECLARE_EFF_UPDATE(     dan_blade_init          );
DECLARE_EFF_UPDATE(     dan_blade_kill         );
DECLARE_EFF_UPDATE(     dan_blade_tick         );
DECLARE_EFF_UPDATE(     dan_blade_previo       );
DECLARE_EFF_UPDATE(     dan_blade_postvio      );
DECLARE_EFF_UPDATE(     dan_blade_precombat    );

DECLARE_EFF_UPDATE(	anger_search_room	);
DECLARE_EFF_UPDATE(	anger_kill		);
DECLARE_EFF_UPDATE(     anger_leave		);

DECLARE_EFF_UPDATE(	terra_shield_init	);
DECLARE_EFF_UPDATE(	terra_shield_kill	);
DECLARE_EFF_UPDATE(     terra_shield_enter      );
DECLARE_EFF_UPDATE(     terra_shield_leave      );
DECLARE_EFF_UPDATE(	terra_shield_predamage	);
DECLARE_EFF_UPDATE(	terra_shield_tick	);


DECLARE_EFF_UPDATE(	ref_shield_init		);
DECLARE_EFF_UPDATE(	ref_shield_kill		);
DECLARE_EFF_UPDATE(	ref_shield_predamage	);
DECLARE_EFF_UPDATE(	ref_shield_tick		);
DECLARE_EFF_UPDATE(	ref_shield_precombat	);

DECLARE_EFF_UPDATE(	plumbum_init		);
DECLARE_EFF_UPDATE(	plumbum_kill		);
DECLARE_EFF_UPDATE(	plumbum_precombat	);
DECLARE_EFF_UPDATE(	plumbum_tick		);
DECLARE_EFF_UPDATE(	plumbum_postcombat	);


DECLARE_EFF_UPDATE(	discord_init		);
DECLARE_EFF_UPDATE(	discord_kill		);
DECLARE_EFF_UPDATE(	discord_tick		);

DECLARE_EFF_UPDATE(	orb_init		);
DECLARE_EFF_UPDATE(	orb_kill		);
DECLARE_EFF_UPDATE(	orb_precombat		);

DECLARE_EFF_UPDATE(	conceal_init		);
DECLARE_EFF_UPDATE(	conceal_kill		);
DECLARE_EFF_UPDATE(	conceal_precombat	);

DECLARE_EFF_UPDATE(	d_shroud_init		);
DECLARE_EFF_UPDATE(	d_shroud_kill		);
DECLARE_EFF_UPDATE(	d_shroud_precommand	);

DECLARE_EFF_UPDATE(     cspawn_init		);
DECLARE_EFF_UPDATE(     cspawn_kill		);
DECLARE_EFF_UPDATE(     cspawn_tick		);
DECLARE_EFF_UPDATE(     cspawn_previo		);
DECLARE_EFF_UPDATE(     cspawn_precombat	);

DECLARE_EFF_UPDATE(     visit_init		);
DECLARE_EFF_UPDATE(     visit_kill		);
DECLARE_EFF_UPDATE(     visit_tick		);
DECLARE_EFF_UPDATE(     visit_viol		);

DECLARE_EFF_UPDATE(     sav_ward_init		);
DECLARE_EFF_UPDATE(     sav_ward_kill		);
DECLARE_EFF_UPDATE(     sav_ward_predamage	);

DECLARE_EFF_UPDATE(     sav_cata_init		);
DECLARE_EFF_UPDATE(     sav_cata_kill		);
DECLARE_EFF_UPDATE(     sav_cata_tick		);

DECLARE_EFF_UPDATE(	circle_of_protection_init      );
DECLARE_EFF_UPDATE(	circle_of_protection_kill      );
DECLARE_EFF_UPDATE(	circle_of_protection_predamage );

DECLARE_EFF_UPDATE(     challenge_init		);
DECLARE_EFF_UPDATE(     challenge_kill		);
DECLARE_EFF_UPDATE(     challenge_tick		);

DECLARE_EFF_UPDATE(     yearning_tick		);

DECLARE_EFF_UPDATE(	ensnare_tick		);
DECLARE_EFF_UPDATE(	ensnare_kill		);

DECLARE_EFF_UPDATE(	mob_gen_tick		);

DECLARE_EFF_UPDATE(     jail_tick		);
DECLARE_EFF_UPDATE(     jail_kill		);

DECLARE_EFF_UPDATE(     batter_kill		);
DECLARE_EFF_UPDATE(     batter_postdamage	);
DECLARE_EFF_UPDATE(     batter_predamage	);
DECLARE_EFF_UPDATE(     batter_precombat	);

DECLARE_EFF_UPDATE(	addiction_tick		);

DECLARE_EFF_UPDATE(	levitation_init		);
DECLARE_EFF_UPDATE(	levitation_kill		);

DECLARE_EFF_UPDATE(	photon_init		);
DECLARE_EFF_UPDATE(	photon_kill		);

DECLARE_EFF_UPDATE(	crusade_init		);
DECLARE_EFF_UPDATE(	crusade_kill		);
DECLARE_EFF_UPDATE(	crusade_tick		);
DECLARE_EFF_UPDATE(	crusade_postkill	);

DECLARE_EFF_UPDATE(	avenger_init		);
DECLARE_EFF_UPDATE(	avenger_kill		);
DECLARE_EFF_UPDATE(	avenger_postkill	);

DECLARE_EFF_UPDATE(     bleed_init		);
DECLARE_EFF_UPDATE(     bleed_vtick		);
DECLARE_EFF_UPDATE(     bleed_enter		);
DECLARE_EFF_UPDATE(     bleed_kill		);

DECLARE_EFF_UPDATE(     apierce_init		);
DECLARE_EFF_UPDATE(     apierce_predamage	);
DECLARE_EFF_UPDATE(     apierce_postviolence	);

DECLARE_EFF_UPDATE(     chant_none		);
DECLARE_EFF_UPDATE(     chant_end		);
DECLARE_EFF_UPDATE(     chant_init		);
DECLARE_EFF_UPDATE(     chant_kill		);

DECLARE_EFF_UPDATE(     dvoid_init		);
DECLARE_EFF_UPDATE(     dvoid_kill		);
DECLARE_EFF_UPDATE(     dvoid_tick		);

DECLARE_EFF_UPDATE(     strium_init		);
DECLARE_EFF_UPDATE(     strium_kill		);
DECLARE_EFF_UPDATE(     strium_predam		);

DECLARE_EFF_UPDATE(     ward_init		);
DECLARE_EFF_UPDATE(     ward_kill		);
DECLARE_EFF_UPDATE(     ward_tick		);

DECLARE_EFF_UPDATE(     acraft_init		);
DECLARE_EFF_UPDATE(     acraft_kill		);
DECLARE_EFF_UPDATE(     acraft_none		);
DECLARE_EFF_UPDATE(     acraft_end		);

DECLARE_EFF_UPDATE(     wcraft_init		);
DECLARE_EFF_UPDATE(     wcraft_kill		);
DECLARE_EFF_UPDATE(     wcraft_none		);
DECLARE_EFF_UPDATE(     wcraft_end		);

DECLARE_EFF_UPDATE(     hwep_objtick		);
DECLARE_EFF_UPDATE(     hwep_none		);

DECLARE_EFF_UPDATE(     approach_tick		);
DECLARE_EFF_UPDATE(     approach_none		);
DECLARE_EFF_UPDATE(     approach_kill		);

DECLARE_EFF_UPDATE(     suffocate_kill		);
DECLARE_EFF_UPDATE(     suffocate_tick		);
DECLARE_EFF_UPDATE(     suffocate_vtick		);

DECLARE_EFF_UPDATE(     contract_kill		);
DECLARE_EFF_UPDATE(     contract_tick		);

DECLARE_EFF_UPDATE(     spirit_kill		);
DECLARE_EFF_UPDATE(     spirit_precombat	);

DECLARE_EFF_UPDATE(     coup_kill		);
DECLARE_EFF_UPDATE(     coup_postkill		);
DECLARE_EFF_UPDATE(     coup_postdeath		);

DECLARE_EFF_UPDATE(     diplo_kill		);
DECLARE_EFF_UPDATE(     diplo_none		);
DECLARE_EFF_UPDATE(     diplo_tick		);

DECLARE_EFF_UPDATE(	veng_init		);
DECLARE_EFF_UPDATE(	veng_kill		);
DECLARE_EFF_UPDATE(	veng_predamage	);
DECLARE_EFF_UPDATE(	veng_precombat	);

DECLARE_EFF_UPDATE(     conjure_init		);
DECLARE_EFF_UPDATE(     conjure_kill		);
DECLARE_EFF_UPDATE(     conjure_none		);
DECLARE_EFF_UPDATE(     conjure_end		);

DECLARE_EFF_UPDATE(     maze_init		);
DECLARE_EFF_UPDATE(     maze_kill		);

DECLARE_EFF_UPDATE(     chron_kill		);
DECLARE_EFF_UPDATE(     chron_tick		);
DECLARE_EFF_UPDATE(	chron_predamage		);

DECLARE_EFF_UPDATE(     paradox_kill		);
DECLARE_EFF_UPDATE(     paradox_tick		);

DECLARE_EFF_UPDATE(     temp_avenger_tick	);

DECLARE_EFF_UPDATE(     movedam_enter		);

DECLARE_EFF_UPDATE(     kinesis_kill		);
DECLARE_EFF_UPDATE(     kinesis_vtick		);

DECLARE_EFF_UPDATE(     create_object_init	);
DECLARE_EFF_UPDATE(     create_object_kill	);

DECLARE_EFF_UPDATE(     ainfect_init		);
DECLARE_EFF_UPDATE(     ainfect_kill		);
DECLARE_EFF_UPDATE(     ainfect_tick		);


DECLARE_EFF_UPDATE(     dreamwalk_none		);
DECLARE_EFF_UPDATE(     dreamwalk_kill		);
DECLARE_EFF_UPDATE(     dreamwalk_tick		);
DECLARE_EFF_UPDATE(     dreamwalk_vtick		);

DECLARE_EFF_UPDATE(     svise_kill		);
DECLARE_EFF_UPDATE(     svise_vtick		);

DECLARE_EFF_UPDATE(	mboost_init		);
DECLARE_EFF_UPDATE(	mboost_kill		);

DECLARE_EFF_UPDATE(	phantasm_vtick		);

DECLARE_EFF_UPDATE(	dragon_blood_precom	);

DECLARE_EFF_UPDATE(	regenerate_vtick	);

DECLARE_EFF_UPDATE(	temp_storm_vupdate	);
DECLARE_EFF_UPDATE(	temp_storm_tick		);

/* Various definitions for MALFORM WEAPON  */

#define MAX_MALFORM		    9    //Max level of malforemd weapon.

//bitwise flags for each selectable effect.

#define		WEP_EFF_NONE	0
#define		WEP_EFF_1	(A)
#define		WEP_EFF_2	(B)
#define		WEP_EFF_3	(C)
#define		WEP_EFF_4	(D)
#define		WEP_EFF_5	(E)
#define		WEP_EFF_6	(F)
#define		WEP_EFF_7	(G)
#define		WEP_EFF_8	(H)
#define		WEP_EFF_9	(I)
#define		WEP_EFF_10	(J)
#define		WEP_EFF_11	(K)
#define		WEP_EFF_12	(L)
#define		WEP_EFF_13	(M)
#define		WEP_EFF_14	(N)
#define		WEP_EFF_15	(O)
#define		WEP_EFF_16	(P)
#define		WEP_EFF_17	(Q)
#define		WEP_EFF_18	(R)
#define		WEP_EFF_19	(S)
#define		WEP_EFF_20	(T)
#define		WEP_EFF_21	(U)
#define		WEP_EFF_22	(V)
#define		WEP_EFF_23	(W)
#define		WEP_EFF_24	(X)
#define		WEP_EFF_25	(Y)
#define		WEP_EFF_26	(Z)
#define		WEP_EFF_27	(aa)
#define		WEP_EFF_28	(bb)
#define		WEP_EFF_29	(cc)
#define		WEP_NAMED	(dd)


/*percent frequency for spell effects. double on 2h weapons. */
#define LVL5_SPELL_FRE		9
#define LVL7_SPELL_FRE		8
#define LVL9_SPELL_FRE		7

/* table for the two gods */
#define	MAL_DEATH	0
#define MAL_CHAOS	1
#define MAL_KNOW	2
#define MAL_FAITH	3

const struct mal_name_type     mal_name_table [] =
{
  //DEATH
  {"Heart of Decay",	 "Soul Reaver"},
  //CHAOS
  {"Bane of Order",	"Hand of Chaos"},
  //KNOWLEDGE
  {"Shard of Wisdom",	"Final Verdict"},
  //FAITH
  {"Hand of God",	"Wrath of God"}
};

const struct mal_name_type     hol_name_table [] =
{
/* exotic */
  {"Holy Relic",	"Reliquary of Truth"},
/* sword */
  {"Blade of Purity",	"Blade of Atonement"},
/* dagger */
  {"Dagger of Benevolence","Blood of Martyrs"},
/* Spear */
  {"Arm of God", 	"Crucifix of Heathens"},
/* Staff */
  {"Staff of Purgation","Banisher of Heretics"},
/* Mace */
  {"Mace of Piety",	"Purifier of Impiety"},
/* Axe */
  {"Will of the Devout","Bane of Miscreants"},
/* Flail */
  {"Purveyor of Remorse","Cleanser of Sacrilege"},
/* Whip */
  {"Ray of Light",	"Flagellator of the Damned"},
/* Polearm */
  {"Harvester of Sorrow","Wrath of God"},
};


