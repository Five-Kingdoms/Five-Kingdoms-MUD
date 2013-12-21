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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include "merc.h"
#include "cabal.h"
#include "db.h"
#include "recycle.h"
#include "interp.h"
#include "save_mud.h"
#include "olc.h"
#include "bounty.h"
#include "armies.h"
#include "tome.h"
#include "clan.h"




#if !defined(OLD_RAND)
// asdfg long random();
int getpid();
time_t time(time_t *tloc);
#endif

/* externals for counting purposes */
extern	OBJ_DATA	*obj_free;
extern	CHAR_DATA	*char_free;
extern  DESCRIPTOR_DATA *descriptor_free;
extern	PC_DATA		*pcdata_free;
extern  AFFECT_DATA	*affect_free;
extern  AFFECT_DATA	*skill_free;
extern  EFFECT_DATA	*effect_free;

/* Globals. */
HELP_DATA *		help_first;
HELP_DATA *		help_last;
SHOP_DATA *		shop_first;
SHOP_DATA *		shop_last;
NOTE_DATA *		note_free;
char			bug_buf		[2*MIL];
CHAR_DATA *		char_list;
TRAP_DATA *		trap_list;
TRAP_INDEX_DATA *	trap_index_list;
CHAR_DATA *		player_list;
char *			help_greeting;
char			log_buf		[2*MIL];
KILL_DATA		kill_table	[MAX_LEVEL];
NOTE_DATA *		note_list;
OBJ_DATA *		object_list;
int                     reboot_tick_counter = -1;
char			reboot_buffer [MIL];

int                     restart = 0;

int			free_vir_vnums[MAX_FREE_VNUM];
int			last_vir_vnum = 0;

PROG_CODE *		mprog_list;
PROG_CODE *             oprog_list;
PROG_CODE *             rprog_list;


int			grn_illithid;
int			grn_half_elf;
int			grn_faerie;
int			grn_feral;
int			grn_undead;
int			grn_demon;
int			grn_fire;
int			grn_stone;
int			grn_werebeast;
int			grn_vampire;
int			grn_ogre;
int			grn_minotaur;

int			gcn_monk;
int			gcn_psi;
int			gcn_vampire;
int			gcn_ranger;
int			gcn_dk;
int			gcn_crusader;
int			gcn_ninja;
int			gcn_necro;
int			gcn_bard;
int			gcn_adventurer;
int			gcn_blademaster;
int			gcn_warrior;
int			gcn_shaman;
int			gcn_healer;
int			gcn_cleric;
int			gcn_druid;
int			gcn_paladin;



sh_int			gsn_backstab;
sh_int			gsn_battlesphere;
sh_int			gsn_battlefocus;
sh_int			gsn_battletrance;
sh_int			gsn_bladestorm;
sh_int			gsn_behead;
sh_int			gsn_baptize;
sh_int			gsn_blanket;
sh_int			gsn_bladework;
sh_int			gsn_battlestance;
sh_int			gsn_avatar_of_steel;
sh_int			gsn_age;
sh_int			gsn_batter;
sh_int			gsn_bolo;
sh_int			gsn_bribe;
sh_int			gsn_breach;
sh_int			gsn_2hands;
sh_int			gsn_2h_handling;
sh_int			gsn_2h_tactics;
sh_int			gsn_2h_mastery;
sh_int			gsn_dodge;
sh_int			gsn_dysentery;
sh_int			gsn_doomsinger;
sh_int			gsn_dheal;
sh_int			gsn_soul_tap;
sh_int			gsn_sretrib;
sh_int			gsn_subvocal;
sh_int			gsn_dwrath;
sh_int			gsn_sbane;
sh_int			gsn_pwater;
sh_int			gsn_probe;
sh_int			gsn_predict;
sh_int			gsn_icefire;
sh_int			gsn_icalm;
sh_int			gsn_identify;
sh_int			gsn_smight;
sh_int			gsn_dmight;
sh_int			gsn_envenom;
sh_int			gsn_enfeeblement;
sh_int			gsn_energy_drain;
sh_int			gsn_hide;
sh_int			gsn_high_sight;
sh_int                  gsn_haymaker;
sh_int                  gsn_harm;
sh_int			gsn_peek;
sh_int			gsn_protective_shield;
sh_int                  gsn_push;
sh_int                  gsn_puppet_master;
sh_int                  gsn_purity;
sh_int                  gsn_const;
sh_int                  gsn_cyclone;
sh_int                  gsn_cont;
sh_int                  gsn_epal;
sh_int			gsn_pick_lock;
sh_int			gsn_sneak;
sh_int			gsn_steal;
sh_int			gsn_steel_wall;
sh_int                  gsn_study;
sh_int			gsn_pry;
sh_int			gsn_plant;
sh_int			gsn_planar_seal;
sh_int			gsn_preserve;
sh_int			gsn_disarm;
sh_int			gsn_dispel_magic;
sh_int                  gsn_disguise;
sh_int			gsn_weapon_cleave;
sh_int			gsn_webbing;
sh_int			gsn_wlock;
sh_int			gsn_weaponcraft;
sh_int			gsn_weaken;
sh_int			gsn_shield_cleave;
sh_int			gsn_sheath;
sh_int			gsn_enhanced_damage;
sh_int			gsn_kick;
sh_int			gsn_parry;
sh_int			gsn_path_anger;
sh_int			gsn_path_fury;
sh_int			gsn_path_dev;
sh_int			gsn_fpart;
sh_int			gsn_fury;
sh_int			gsn_first_parry;
sh_int			gsn_second_parry;
sh_int			gsn_serpent;
sh_int			gsn_secret_arts;
sh_int			gsn_rapid_fire;
sh_int			gsn_rescue;
sh_int			gsn_rev_time;
sh_int			gsn_resurrection;
sh_int			gsn_second_attack;
sh_int			gsn_third_attack;
sh_int			gsn_tracker;
sh_int			gsn_transform;
sh_int			gsn_torment_bind;
sh_int			gsn_turn_undead;
sh_int			gsn_blindness;
sh_int			gsn_bloodhaze;
sh_int			gsn_bloodvow;
sh_int			gsn_chant;
sh_int			gsn_charm_person;
sh_int			gsn_curse;
sh_int			gsn_curse_weapon;
sh_int			gsn_invis;
sh_int			gsn_invigorate;
sh_int			gsn_mark_prey;
sh_int			gsn_marksman;
sh_int			gsn_maelstrom;
sh_int			gsn_mark_fear;
sh_int			gsn_mass_invis;
sh_int			gsn_masochism;
sh_int			gsn_poison;
sh_int			gsn_powerstrike;
sh_int			gsn_plague;
sh_int			gsn_sleep;
sh_int			gsn_sanctuary;
sh_int			gsn_sacred_runes;
sh_int			gsn_fly;
sh_int  		gsn_adv_handling;
sh_int  		gsn_adrenaline_rush;
sh_int  		gsn_apierce;
sh_int  		gsn_arms_main;
sh_int  		gsn_archery;
sh_int  		gsn_armorcraft;
sh_int  		gsn_armored_rush;
sh_int  		gsn_armor_enhance;
sh_int  		gsn_avenger;
sh_int  		gsn_axe;
sh_int  		gsn_abduct;
sh_int  		gsn_dagger;
sh_int			gsn_fired;
sh_int  		gsn_flail;
sh_int  		gsn_mace;
sh_int  		gsn_polearm;
sh_int			gsn_shield_block;
sh_int  		gsn_spear;
sh_int  		gsn_spirit_bind;
sh_int  		gsn_staff;
sh_int  		gsn_sword;
sh_int  		gsn_whip;
sh_int  		gsn_sword_mastery;
sh_int  		gsn_dagger_mastery;
sh_int  		gsn_spear_mastery;
sh_int  		gsn_mace_mastery;
sh_int  		gsn_axe_mastery;
sh_int  		gsn_flail_mastery;
sh_int  		gsn_whip_mastery;
sh_int  		gsn_staff_mastery;
sh_int  		gsn_exotic_mastery;
sh_int  		gsn_exile;
sh_int  		gsn_shield_expert;
sh_int  		gsn_sword_expert;
sh_int  		gsn_dagger_expert;
sh_int  		gsn_mace_expert;
sh_int  		gsn_spear_expert;
sh_int  		gsn_axe_expert;
sh_int  		gsn_flail_expert;
sh_int  		gsn_staff_expert;
sh_int  		gsn_whip_expert;
sh_int  		gsn_polearm_expert;
sh_int  		gsn_exotic_expert;
sh_int  		gsn_polearm_mastery;
sh_int  		gsn_bash;
sh_int  		gsn_basic_armor;
sh_int  		gsn_adv_armor;
sh_int  		gsn_berserk;
sh_int  		gsn_dirt;
sh_int  		gsn_hand_to_hand;
sh_int  		gsn_trip;
sh_int  		gsn_trample;
sh_int  		gsn_lash;
sh_int  		gsn_fast_healing;
sh_int  		gsn_false_weapon;
sh_int  		gsn_fade;
sh_int  		gsn_haggle;
sh_int  		gsn_lore;
sh_int  		gsn_lotus_sc;
sh_int  		gsn_meditation;
sh_int  		gsn_melee;
sh_int  		gsn_scrolls;
sh_int  		gsn_scourge;
sh_int  		gsn_staves;
sh_int  		gsn_stand_ground;
sh_int  		gsn_wands;
sh_int  		gsn_wanted;
sh_int  		gsn_recall;
sh_int                  gsn_hunt;
sh_int                  gsn_dual_wield;
sh_int                  gsn_death_grasp;
sh_int                  gsn_deathweaver;
sh_int                  gsn_death_kiss;
sh_int                  gsn_death_shroud;
sh_int                  gsn_deteriorate;
sh_int                  gsn_detect_traps;
sh_int                  gsn_phantasmal_griffon;
sh_int                  gsn_illusionary_spectre;
sh_int                  gsn_phantom_dragon;
sh_int                  gsn_trance;
sh_int                  gsn_blind_fighting;
sh_int                  gsn_shield_disarm;
sh_int                  gsn_rake;
sh_int                  gsn_raise_morale;
sh_int			gsn_misdirection;
sh_int			gsn_missile_shield;
sh_int			gsn_shadowform;
sh_int			gsn_shadowdancer;
sh_int			gsn_shadow_door;
sh_int			gsn_shadowmaster;
sh_int			gsn_shadow_strike;
sh_int			gsn_treeform;
sh_int			gsn_fourth_attack;
sh_int			gsn_footwork;
sh_int			gsn_forestmeld;
sh_int			gsn_deathblow;
sh_int			gsn_com_lan;
sh_int			gsn_counter;
sh_int			gsn_lay_on_hands;
sh_int			gsn_detect_hidden;
sh_int			gsn_cleave;
sh_int			gsn_assassinate;
sh_int			gsn_blackjack;
sh_int			gsn_gag;
sh_int			gsn_strangle;
sh_int			gsn_acupuncture;
sh_int			gsn_herb;
sh_int			gsn_hex;
sh_int			gsn_tame;
sh_int			gsn_beast_call;
sh_int			gsn_rage;
sh_int			gsn_regeneration;
sh_int			gsn_bodyslam;
sh_int			gsn_dual_backstab;
sh_int			gsn_grapple;
sh_int			gsn_greenheart;
sh_int			gsn_charge;
sh_int			gsn_shoot;
sh_int			gsn_ambush;
sh_int			gsn_endure;
sh_int			gsn_barkskin;
sh_int			gsn_warcry;
sh_int			gsn_circle;
sh_int			gsn_swing;
sh_int			gsn_headbutt;
sh_int			gsn_camouflage;
sh_int			gsn_chameleon;
sh_int			gsn_quiet_movement;
sh_int			gsn_keen_sight;
sh_int			gsn_vanish;
sh_int			gsn_vredem;
sh_int			gsn_virtues;
sh_int			gsn_vitality;
sh_int			gsn_voodoo_spell;
sh_int			gsn_chii;
sh_int			gsn_blindness_dust;
sh_int			gsn_poison_smoke;
sh_int			gsn_pugil;
sh_int			gsn_butcher;
sh_int			gsn_dual_parry;
sh_int			gsn_caltraps;
sh_int			gsn_nerve;
sh_int			gsn_nerve_amp;
sh_int			gsn_numbness;
sh_int			gsn_righteous;
sh_int			gsn_riposte;
sh_int			gsn_ripple;
sh_int			gsn_truesight;
sh_int			gsn_trophy;
sh_int			gsn_spellbane;
sh_int			gsn_spellkiller;
sh_int			gsn_spell_blast;
sh_int			gsn_spell_trap;
sh_int			gsn_mana_trap;
sh_int			gsn_doppelganger;
sh_int			gsn_mirror_image;
sh_int			gsn_mirage;
sh_int			gsn_mirror_cloak;
sh_int			gsn_cognizance;
sh_int			gsn_stance;
sh_int			gsn_forms;
sh_int			gsn_wanted;
sh_int			gsn_judge;
sh_int			gsn_spy;
sh_int			gsn_knight;
sh_int			gsn_guard;
sh_int			gsn_stallion;
sh_int			gsn_relax;
sh_int			gsn_garble;
sh_int			gsn_brew;
sh_int			gsn_challenge;
sh_int			gsn_bounty_collect;
sh_int			gsn_fatality;
sh_int			gsn_climb;
sh_int                  gsn_damnation;
sh_int                  gsn_dam_masteryB;
sh_int                  gsn_dam_masteryS;
sh_int                  gsn_dam_masteryP;
sh_int                  gsn_flashfire;
sh_int                  gsn_drained;
sh_int                  gsn_dreamprobe;
sh_int                  gsn_mortally_wounded;
sh_int                  gsn_mercy_wait;
sh_int                  gsn_mounted;
sh_int                  gsn_ancient_lore;
sh_int  		gsn_invoke;
sh_int  		gsn_mimic;
sh_int			gsn_camp;
sh_int			gsn_scout;
sh_int			gsn_throw;
sh_int			gsn_roar;
sh_int			gsn_edge_craft;
sh_int			gsn_counterfeit;
sh_int			gsn_ranger_staff;
sh_int			gsn_healer_staff;
sh_int			gsn_heal;
sh_int			gsn_familiar_link;
sh_int			gsn_extort;
sh_int			gsn_squire;
sh_int			gsn_silence;
sh_int			gsn_blasphemy;
sh_int			gsn_blink;
sh_int			gsn_defecate;
sh_int			gsn_vomit;
sh_int			gsn_insomnia;
sh_int			gsn_insight;
sh_int			gsn_mystic_healing;
sh_int			gsn_shield_of_words;
sh_int			gsn_children_sky;
sh_int			gsn_pilfer;
sh_int			gsn_mind_link;
sh_int			gsn_mindsurge;
sh_int			gsn_mind_disrupt;
sh_int			gsn_linked_mind;
sh_int			gsn_ensnare;
sh_int			gsn_timer;
sh_int			gsn_time_comp;
sh_int			gsn_pixie_dust;
sh_int			gsn_bat_form;
sh_int			gsn_wolf_form;
sh_int			gsn_mist_form;
sh_int			gsn_double_kick;
sh_int			gsn_coffin;
sh_int			gsn_stake;
sh_int			gsn_eavesdrop;
sh_int			gsn_kinetics;
sh_int			gsn_thrust;
sh_int			gsn_ecstacy;
sh_int			gsn_anatomy;
sh_int			gsn_beads;
sh_int			gsn_martial_arts_b;
sh_int			gsn_martial_arts_a;
sh_int			gsn_kickboxing_b;
sh_int			gsn_kickboxing_a;
sh_int			gsn_double_grip;
sh_int			gsn_monkey;
sh_int			gsn_tiger;
sh_int			gsn_crane;
sh_int			gsn_mantis;
sh_int			gsn_drunken;
sh_int			gsn_horse;
sh_int			gsn_holy_hands;
sh_int			gsn_dragon;
sh_int			gsn_buddha;
sh_int			gsn_aura;
sh_int			gsn_balance;
sh_int			gsn_sense_motion;
sh_int			gsn_catalepsy;
sh_int			gsn_healing;
sh_int			gsn_kinesis;
sh_int			gsn_mind_thrust;
sh_int			gsn_mind_disruption;
sh_int			gsn_mind_blast;
sh_int			gsn_psalm_master;
sh_int			gsn_psychic_crush;
sh_int			gsn_psionic_blast;
sh_int			gsn_psi_amp;
sh_int			gsn_psychic_purge;
sh_int			gsn_photon;
sh_int			gsn_flesh_armor;
sh_int			gsn_detect_aura;
sh_int			gsn_displacement;
sh_int			gsn_dvoid;
sh_int			gsn_iron_will;
sh_int			gsn_ironarm;
sh_int			gsn_iron_curtain;
sh_int			gsn_bio_manipulation;
sh_int			gsn_ego_whip;
sh_int			gsn_tele_lock;
sh_int			gsn_telepathy;
sh_int			gsn_temp_storm;
sh_int			gsn_there_not_there;
sh_int			gsn_levitate;
sh_int			gsn_leash;
sh_int			gsn_leadership;
sh_int			gsn_gravitate;
sh_int			gsn_bio_feedback;
sh_int			gsn_pyramid_of_force;
sh_int			gsn_body_weaponry;
sh_int			gsn_ectoplasm;
sh_int			gsn_hypnosis;
sh_int			gsn_forget;
sh_int			gsn_graft_weapon;
sh_int			gsn_mental_barrier;
sh_int			gsn_mental_knife;
sh_int			gsn_empower;
sh_int			gsn_ogtrium;
sh_int			gsn_onslaught;
sh_int			gsn_omnipotence;
sh_int			gsn_ocularis;
sh_int			gsn_strium;
sh_int			gsn_ptrium;
sh_int			gsn_uorder;
sh_int			gsn_unminding;
sh_int			gsn_istrength;
sh_int			gsn_offensive;
sh_int			gsn_orealts_1st;
sh_int			gsn_orealts_2nd;
sh_int			gsn_defensive;
sh_int			gsn_chii_bolt;
sh_int			gsn_thrash;
sh_int			gsn_deadfall;
sh_int			gsn_windmill;
sh_int			gsn_cusinart;
sh_int			gsn_cure_light;
sh_int			gsn_wire_delay;
sh_int			gsn_siege_delay;
sh_int			gsn_ghoul_touch;
sh_int			gsn_paralyze;
sh_int			gsn_entomb;
sh_int			gsn_insect_swarm;
sh_int			gsn_feedback;
sh_int			gsn_feign;
sh_int			gsn_con_damage;
sh_int			gsn_cone;
sh_int			gsn_dark_ritual;
sh_int			gsn_covenant;
sh_int			gsn_burrow;
sh_int			gsn_cutpurse;
sh_int			gsn_cutter;
sh_int			gsn_offhand_disarm;
sh_int			gsn_werepower;
sh_int			gsn_weretiger;
sh_int			gsn_werewolf;
sh_int			gsn_werebear;
sh_int			gsn_werefalcon;
sh_int			gsn_fasting;
sh_int			gsn_shed_skin;
sh_int			gsn_crusade;
sh_int			gsn_heroism;
sh_int			gsn_decoy;
sh_int			gsn_prayer;
sh_int			gsn_refrain;
sh_int			gsn_lullaby;
sh_int			gsn_tarot;
sh_int			gsn_tko;
sh_int			gsn_leech;
sh_int			gsn_noquit;
sh_int			gsn_nolearn;
sh_int			gsn_duplicate;
sh_int			gsn_dance_song;
sh_int			gsn_attract;
sh_int			gsn_uppercut;
sh_int			gsn_taunt;
sh_int			gsn_condor;
sh_int			gsn_disorient;
sh_int			gsn_concen;
sh_int			gsn_seasons;
sh_int			gsn_hysteria;
sh_int			gsn_lust;
sh_int			gsn_reputation;
sh_int			gsn_unforgiven;
sh_int			gsn_phoenix;
sh_int			gsn_brawl;
sh_int			gsn_brain_death;
sh_int			gsn_unholy_strength;
sh_int			gsn_ram;
sh_int			gsn_embrace;
sh_int			gsn_embrace_poison;
sh_int			gsn_secarts;
sh_int			gsn_continuum;
sh_int			gsn_tear;
sh_int			gsn_unholy_gift;
sh_int			gsn_mana_charge;
sh_int                  gsn_warmaster_code;
sh_int			gsn_call_roc;
sh_int                  gsn_corrupt;
sh_int			gsn_lifeforce;
sh_int			gsn_life_insurance;
sh_int			gsn_lifedrain;
sh_int			gsn_entangle;
sh_int			gsn_forest_mist;
sh_int			gsn_forest_walk;
sh_int			gsn_druid_staff;
sh_int			gsn_trap_silvanus;
sh_int			gsn_circle_of_protection;
sh_int			gsn_field_fear;
sh_int			gsn_fear;
sh_int			gsn_drug_use;
sh_int			gsn_research;
sh_int			gsn_decipher;

/* BATTLE MAGES */
sh_int			gsn_forcefield;
sh_int			gsn_terra_shield;
sh_int			gsn_rust;
sh_int			gsn_air_shield;
sh_int			gsn_blades;
sh_int			gsn_field_dress;
sh_int			gsn_plumbum;
sh_int			gsn_dan_blade;
sh_int			gsn_mana_lock;
sh_int			gsn_ref_shield;

/* Miners */
sh_int			gsn_sense_vein;
sh_int			gsn_dig;
sh_int			gsn_gemlore;
sh_int			gsn_extrude;

/* anatmy masteries */
sh_int			gsn_mob_expert;
sh_int			gsn_human_expert;
sh_int			gsn_elf_expert;
sh_int			gsn_giant_expert;
sh_int			gsn_dwarf_expert;
sh_int			gsn_demihuman_expert;
sh_int			gsn_flying_expert;
sh_int			gsn_beast_expert;
sh_int			gsn_unique_expert;

sh_int			gsn_mob_master;
sh_int			gsn_human_master;
sh_int			gsn_elf_master;
sh_int			gsn_dwarf_master;
sh_int			gsn_giant_master;
sh_int			gsn_beast_master;
sh_int			gsn_demihuman_master;
sh_int			gsn_flying_master;
sh_int			gsn_unique_master;

/* MOBprogram locals */
int 		mprog_name_to_type	args ( ( char* name ) );
MPROG_DATA *    mprog_file_read         args ( ( char* f, MPROG_DATA* mprg, MOB_INDEX_DATA *pMobIndex ) );
void		load_mobprogs           args ( ( FILE* fp ) );
void            mprog_read_programs     args ( ( FILE* fp, MOB_INDEX_DATA *pMobIndex ) );

/* Locals. */
MOB_INDEX_DATA *	mob_index_hash		[MAX_KEY_HASH];
OBJ_INDEX_DATA *	obj_index_hash		[MAX_KEY_HASH];
ROOM_INDEX_DATA *	room_index_hash		[MAX_KEY_HASH];
AREA_DATA *		area_first;
AREA_DATA *		area_last;
extern char             str_empty       [1];
extern int sOverFlowString;
extern int nOverFlowString;
extern bool Full;
char * daPrompt;
int			top_affect;
int			top_area;
int			top_ed;
int			top_exit;
int			top_help;
int			top_mob_index;
int			top_obj_index;
int			top_reset;
int			top_room;
int			top_vir_room = 0;
int			top_shop;
int                     top_vnum_room;
int                     top_vnum_mob;
int                     top_vnum_obj;
int                     top_mprog_index;
int                     top_oprog_index;
int                     top_rprog_index;
int                     top_trap_index;
int                     top_cabal_index = 0;
int 			mobile_count = 0;
int			newmobs = 0;
int			newobjs = 0;
bool			loadallmobs = TRUE;

/* Memory management.                                        *
 * Increase MAX_STRING if you have too.                      *
 * Tune the others only if you understand what you're doing. */
#define                 MAX_MEM_LIST             11
void *			rgFreeList	[MAX_MEM_LIST];
const int		rgSizeList	[MAX_MEM_LIST]	=
{ 16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768-64 };
extern int              nAllocString;
extern int              sAllocString;
int			nAllocPerm;
int			sAllocPerm;

/* Semi-locals. */
bool			fBootDb;
FILE *			fpArea;
char			strArea[MIL];
extern  		int MAX_STRING;
void    		init_string_space( void );
void    		boot_done( void );
struct			social_type	social_table		[MAX_SOCIALS];
int			social_count;
int			night_count;
int			mprog_count;
int			mprogf_count;

/* Local booting procedures. */
void    init_mm         args( ( void ) );
void	load_area	args( ( FILE *fp ) );
void    new_load_area   args( ( FILE *fp ) );
void	load_helps	args( ( FILE *fp ) );
void	load_helps_new	args( ( void	 ) );
void	load_old_mob	args( ( FILE *fp ) );
void 	load_mobiles	args( ( FILE *fp ) );
void 	load_army_indexes args( ( FILE *fp ) );
void 	load_cabal_indexes args( ( FILE *fp ) );
void	load_old_obj	args( ( FILE *fp ) );
void 	load_objects	args( ( FILE *fp ) );
void	load_resets	args( ( FILE *fp ) );
void	load_rooms	args( ( FILE *fp ) );
void	load_shops	args( ( FILE *fp ) );
void 	load_socials	args( ( FILE *fp ) );
void	load_specials	args( ( FILE *fp ) );
void	load_notes	args( ( void ) );
void	load_bans	args( ( void ) );
void	load_objprogs	args(( FILE *fp ));
void	load_roomprogs	args(( FILE *fp ));
void	load_traps	args(( FILE *fp ));

void	fix_exits	args( ( void ) );
void	fix_mobprogs    args( ( void ) );
void	fix_objprogs    args( ( void ) );
void	fix_roomprogs   args( ( void ) );

void	reset_area	args( ( AREA_DATA * pArea ) );
void	load_limits	args( ( void ) );
void	color_rooms	args( ( void ) );

/* extern functions */
extern void make_html_helps();

/* seeds a single object as based on entry in objseed table */
void seed_object( int i ){
  OID* pOI = get_obj_index( objseed_table[i].vnum );
  if (pOI == NULL){
    bug("seed_objects: could not load obj index.", objseed_table[i].vnum);
    return;
  }
  /* check if we need to load one of these */
  if (objseed_table[i].num > pOI->count){
    int j = pOI->count;
    for (; j < objseed_table[i].num; j++){
      OBJ_DATA* pO;		//object
      ROOM_INDEX_DATA* pR;	//room to be put in

      /* check the chance to load */
      if (number_percent() > objseed_table[i].chance)
	continue;
      /* load the object */
      pR = get_rand_room(objseed_table[i].amin,
			 objseed_table[i].amax,
			 objseed_table[i].rmin,
			 objseed_table[i].rmax,

			 objseed_table[i].i_area,
			 objseed_table[i].ia_num,
			 objseed_table[i].e_area,
			 objseed_table[i].ea_num,

			 objseed_table[i].i_sect,
			 objseed_table[i].is_num,
			 objseed_table[i].e_sect,
			 objseed_table[i].es_num,

			 objseed_table[i].i_rom1,
			 objseed_table[i].ir1_num,
			 objseed_table[i].e_rom1,
			 objseed_table[i].er1_num,

			   objseed_table[i].i_rom2,
			 objseed_table[i].ir2_num,
			 objseed_table[i].e_rom2,
			 objseed_table[i].er2_num,

			 objseed_table[i].pool,
			 objseed_table[i].fExit,
			 FALSE, NULL);
      if (pR == NULL){
	bug("seed_obj: NULL room for obj %d.", objseed_table[i].vnum);
	continue;
      }
      /* get the objects */
      if ( (pO = create_object( pOI, pOI->level)) == NULL){
	bug("seed_obj: NULL object for vnum: %d", pOI->vnum);
	continue;
      }
      /* got room and got objects, we put on in the other */
      obj_to_room(pO, pR);
      pO->homevnum = pR->vnum;
    }//END for each unloaded objects
  }//END if
}
/* following seeds random objects into the world */
/* the objseed_table contains all info. The function */
/* runs once after each reboot and loads objects into */
/* appropriate rooms IF they do not exists.	      */
void seed_objects(){
  int i = 0;
/* run through the objseed table till we hit a 0 vnum entry */
  for (i =0; objseed_table[i].vnum > 0; i++){
/* DEBUG
    nlogf("seeding entry %d of vnum %d", i, objseed_table[i].vnum);
*/
    seed_object ( i );
  }//END for each table entry
}

/* Following struct is used to record player stats. */
#define MAX_COUNT	32
struct class_info_type {
  int class_count[MAX_CLASS];	//count for each class
  int race_count[MAX_PC_RACE];	//count for each race

  /* quest stuff to keep track of quest players with counts*/
  char vamp_name[MAX_COUNT][MIL];
  int vamp;
  char crus_name[MAX_COUNT][MIL];
  int crus;
  char psi_name[MAX_COUNT][MIL];
  int psis;
  char lich_name[MAX_COUNT][MIL];
  int lich;

  char undead_name[MAX_COUNT][MIL];
  int undead;
  char demon_name[MAX_COUNT][MIL];
  int demon;
  char avatar_name[MAX_COUNT][MIL];
  int avatar;
  char tatto_name[MAX_COUNT][MIL]; //lists players with tats
  int tatto;
};

/* sends not to immortal with files purged etc. */
void send_pfile_stats(int pfiles, int purged, int wiped, int total){
  char *strtime, *namelist;
  char buf[MSL],arg[MIL];
  char* sender = "`!HAL 1000``";

  BUFFER *buffer;
  NOTE_DATA *note;

  buffer = new_buf();
  sprintf(buf,"`!Following generated at %s.``\n\r\n\r",((char *) ctime( &mud_data.current_time )));
  add_buf(buffer,buf);
  sprintf(buf, "Total pfiles checked: %-25d\n\r"\
	  "Pfiles purged: %-25d\n\r"\
	  "Pfiles deleted: %-25d\n\r"\
	  "Pfiles above level 15 (used for limit count): %d\n\r",
	  total, purged, wiped, pfiles);
  add_buf(buffer,buf);
  namelist = str_dup("immortal");
  for (;;)
    {
      namelist = one_argument(namelist,arg);
      if (arg[0] == '\0')
	break;
      note = new_note();
      note->prefix = 0;
      note->next = NULL;
      note->type = NOTE_NOTE;
      free_string(note->sender);
      note->sender = str_dup( sender );
      free_string(note->subject);
      note->subject = str_dup("Pfile Statistics");
      free_string(note->text);
      note->text = str_dup(buf_string(buffer));
      strtime                         = ctime( &mud_data.current_time );
      strtime[strlen(strtime)-1]      = '\0';
      note->date                 = str_dup( strtime );
      note->date_stamp           = mud_data.current_time + 2;
      free_string(note->to_list);
      note->to_list = str_dup(arg);
      append_note(note);
    }
  free_buf(buffer);
}

/* constructs a note based on structures passed and sends to "immortal" */
void send_stats(struct class_info_type *cl_info){
    char *strtime, *namelist;
    char buf[MSL],arg[MIL];
    char* sender = "`!HAL 1000``";

    BUFFER *buffer;
    NOTE_DATA *note;
    int i = 0;
    int j = 0;
    int count = 0;
    int race = 0;
    int class = 0;

    /* begin to generate stats */
    buffer = new_buf();
    sprintf(buf,"`!Following generated at %s.``\n\r\n\r",((char *) ctime( &mud_data.current_time )));
    add_buf(buffer,buf);
    sprintf(buf, "CLASS\t\t\tCOUNT\t\tRACE\t\t\tCOUNT\n\r");
    add_buf(buffer,buf);
    if (MAX_CLASS > MAX_PC_RACE)
      j = MAX_CLASS;
    else
      j = MAX_PC_RACE;
    for (i = 0; i< j; i++){
      bool fLine = FALSE;
      if (class < MAX_CLASS && !IS_NULLSTR(class_table[class].name) ){
	sprintf(buf,"%-25s %-10d", class_table[class].name, cl_info->class_count[class]);
	add_buf(buffer, buf);
	fLine = TRUE;
      }
      class++;
      if (race  < MAX_PC_RACE && cl_info->race_count[race] > 0){
	sprintf(buf,"%-25s %-10d", pc_race_table[race].name, cl_info->race_count[race]);
	add_buf(buffer, buf);
	fLine = TRUE;
      }
      race++;
      if (fLine){
	add_buf(buffer, "\n\r");
	if (count % 5 == 0)
	  add_buf(buffer, "\n\r");
	count++;
      }
    }
    /* show quest races/classes */
    add_buf(buffer, "\n\r\n\rVampires:");
    for (i = 0; i < cl_info->vamp; i++){
      if (i % 6 == 0 && i + 1 < cl_info->vamp)
	add_buf(buffer, "\n\r");
      sprintf(buf, "%s  ", cl_info->vamp_name[i]);
      add_buf(buffer, buf);
    }
    add_buf(buffer, "\n\r\n\rCrusaders:");
    for (i = 0; i < cl_info->crus; i++){
      if (i % 6 == 0 && i + 1 < cl_info->crus)
	add_buf(buffer, "\n\r");
      sprintf(buf, "%s  ", cl_info->crus_name[i]);
      add_buf(buffer, buf);
    }
    add_buf(buffer, "\n\r\n\rPsionicists:");
    for (i = 0; i < cl_info->psis; i++){
      if (i % 6 == 0 && i + 1 < cl_info->psis)
	add_buf(buffer, "\n\r");
      sprintf(buf, "%s  ", cl_info->psi_name[i]);
      add_buf(buffer, buf);
    }
    add_buf(buffer, "\n\r\n\rLiches:");
    for (i = 0; i < cl_info->lich; i++){
      if (i % 6 == 0 && i + 1 < cl_info->lich)
	add_buf(buffer, "\n\r");
      sprintf(buf, "%s  ", cl_info->lich_name[i]);
      add_buf(buffer, buf);
    }
    add_buf(buffer, "\n\r\n\rUndead:");
    for (i = 0; i < cl_info->undead; i++){
      if (i % 6 == 0 && i + 1 < cl_info->undead)
	add_buf(buffer, "\n\r");
      sprintf(buf, "%s  ", cl_info->undead_name[i]);
      add_buf(buffer, buf);
    }
    add_buf(buffer, "\n\r\n\rDemons:");
    for (i = 0; i < cl_info->demon; i++){
      if (i % 6 == 0 && i + 1 < cl_info->demon)
	add_buf(buffer, "\n\r");
      sprintf(buf, "%s  ", cl_info->demon_name[i]);
      add_buf(buffer, buf);
    }
    add_buf(buffer, "\n\r\n\rAvatars:");
    for (i = 0; i < cl_info->avatar; i++){
      if (i % 6 == 0 && i + 1 < cl_info->avatar)
	add_buf(buffer, "\n\r");
      sprintf(buf, "%s ", cl_info->avatar_name[i]);
      add_buf(buffer, buf);
    }
    add_buf(buffer, "\n\r\n\rTattoos:");
    for (i = 0; i < cl_info->tatto; i++){
      if (i % 6 == 0 && i + 1 < cl_info->tatto)
	add_buf(buffer, "\n\r");
      sprintf(buf, "%s ", cl_info->tatto_name[i]);
      add_buf(buffer, buf);
    }
    /* ALL DONE Send the note*/
    namelist = str_dup("immortal");
    for (;;)
    {
      namelist = one_argument(namelist,arg);
      if (arg[0] == '\0')
	break;
      note = new_note();
      note->prefix = 0;
      note->next = NULL;
      note->type = NOTE_NOTE;
      free_string(note->sender);
      note->sender = str_dup( sender );
      free_string(note->subject);
      note->subject = str_dup("Class/Race Statistics");
      free_string(note->text);
      note->text = str_dup(buf_string(buffer));
      strtime                         = ctime( &mud_data.current_time );
      strtime[strlen(strtime)-1]      = '\0';
      note->date                 = str_dup( strtime );
      note->date_stamp           = mud_data.current_time;
      free_string(note->to_list);
      note->to_list = str_dup(arg);
      append_note(note);
    }

    free_buf(buffer);
}

/* purges all playes in pflies of rares as per purge_limited in comm.c */
/* returns total pfiles to be used in the game limited item calcs */
/* booleans control which actions are done */
int purge_players(bool fPurge, bool fWipe, bool fCount){
  CABAL_DATA* pCab;
  DIR *Directory;	//Directory
  struct dirent *Dir;	//Directory entry
  struct stat Stat;	//File statistics
  char pFile[MIL];	//Current pfile name
  DESCRIPTOR_DATA *d = new_descriptor();

  /*statistical things */
  struct class_info_type cl_info;

  int tot_count = 0;	//Count of all pfiles.
  int p_count = 0;	//Count of pfiles that can carry rares.
  int tot_purged = 0;	//Count of pfiles purged.
  int tot_wiped = 0;	//Count of pfiles wiped.

  /* for speed we try to avoid lookups */
  const int vamp = class_lookup("vampire");
  const int crus = class_lookup("crusader");
  const int psi  = class_lookup("psionicist");
  //const int lich = class_lookup("lich");
  const int undead = race_lookup("undead");
  const int demon = race_lookup("demon");


  /* Zero statistical info just in case */
  memset( &cl_info, 0, sizeof( cl_info ));

  /* Open dir, get first entry */
  if ( (Directory=opendir(PLAYER_DIR)) == NULL){
    bug("purge_players: Could not access PLAYER_DIR", 0);
    return 0;
  }

  Dir= (struct dirent*) malloc (sizeof (struct dirent));
  Dir->d_name[0] = '\0';

  if ( (Dir = readdir(Directory)) == NULL){
    bug("purge_players: Could not access first directory entry.", 0);
    return 0;
  }

  while(Dir!=NULL){
    int fFound = FALSE;	//Tests if char file was loaded

    /* Get info/pointer to file entry*/
    sprintf(pFile, PLAYER_DIR"%s", (char *) Dir->d_name);
    if ( -1 == stat( pFile, &Stat)){
      char buf[MIL];
      sprintf(buf, "purge_players: stat returned error on file: %s.",
	      pFile);
      bug(buf, 0);
      Dir = readdir(Directory);
      continue;
    }
    /* check if "regular" file */
    if(S_ISREG(Stat.st_mode)){
      /* check for player file name*/
      if (Dir->d_name[0] >= 'A' && Dir->d_name[0] <= 'Z') {
	fFound = load_char (d, Dir->d_name);
	if ( fFound > 0){
	  ROOM_INDEX_DATA* room;
	  CHAR_DATA* ch = d->character;
	  /* Make sure the room is not saved */
	  d->character->pcdata->roomnum/=-1;
	  d->character->logon = 0;
	  load_obj (d->character);

	  /* put character in a room for proper extraction */
	  d->character->next = char_list;
	  char_list = d->character;
	  d->character->next_player = player_list;
	  player_list = d->character;
	  reset_char(d->character);
	  if ( (room = get_room_index(ROOM_VNUM_PORTAL)) == NULL){
	    bug("purge_players: could not get indext to ROOM_PORTAL", ROOM_VNUM_PORTAL);
	    return 0;
	  }
	  char_to_room (d->character, room);

	  /* LIMITED PURGE */
	  if (fPurge){
	    if (purge_limited(d->character))
	      tot_purged ++;
	  }

	  /* Save character at this point, ALL things affecting char should be done before this. */
	  save_char_obj(d->character);

	  /* FILE WIPE */
	  if (fWipe){
	    if (IS_SET(d->character->act, PLR_DENY)
		|| (mud_data.current_time - d->character->logoff) > (86400 * (20 + UMIN(100,2*d->character->level)))
		){
	      /* Viri: changed to be shown only for significant players, took up too much time */
	      if (d->character->level > 15){
		nlogf("%s (%d) last logged off %d days ago, and has been removed.",
		      d->character->name,
		      d->character->level,
		      (int) (mud_data.current_time - d->character->logoff) / 86400);
	      }
	      tot_wiped ++;
	      purge_limited(d->character);
	      if (ch->pCabal)
		char_from_cabal( ch );
	      unlink (pFile);
	      extract_char(d->character, TRUE);

	      Dir = readdir(Directory);
	      continue;
	    }//END if old enough
	  }//END WIPE

	  /* Cabal/Quest/Race Counting */
	  if (fCount && !IS_IMMORTAL(ch)){

	    /* count race/class/quest first */
	    cl_info.class_count[ch->class]++;
	    cl_info.race_count[ch->race]++;

	    /* quest race/class members */
	    if (ch->class == vamp){
	      sprintf(cl_info.vamp_name[cl_info.vamp++], "%s", ch->name);
	    }
	    if (ch->class == crus){
	      sprintf(cl_info.crus_name[cl_info.crus++], "%s", ch->name);
	    }
	    if (ch->class == psi){
	      sprintf(cl_info.psi_name[cl_info.psis++], "%s", ch->name);
	    }
	    /*
	    if (ch->class == lich){
	      sprintf(cl_info.lich_name[cl_info.lich++], "%s", ch->name);
	    */
	    if (ch->race == undead){
	      sprintf(cl_info.undead_name[cl_info.undead++], "%s", ch->name);
	    }
	    if (ch->race == demon){
	      sprintf(cl_info.demon_name[cl_info.demon++], "%s", ch->name);
	    }
	    if (IS_AVATAR(ch)){
	      sprintf(cl_info.avatar_name[cl_info.avatar++], "%s", ch->name);
	    }
	    if (ch->tattoo){
	      sprintf(cl_info.tatto_name[cl_info.tatto++], "%s:%s,", ch->name, deity_table[ch->tattoo].way);
	    }
	  }
	  if (d->character->level > 15)
	    p_count ++;
	  /* extract the char */
	  extract_char(d->character, TRUE);
	  tot_count++;
	}//END if found and loaded
      }//END if A to Z
    }//END If file is regular
    Dir = readdir(Directory);
  }//END while loop.

  /* Run through cabal members and check if their members exist, if not remove them */
  for(pCab = cabal_list; pCab; pCab = pCab->next){
    CMEMBER_DATA* cm, *cm_next;
    /* reset the member counts */
    pCab->cur_member = 0;
    pCab->cur_elder = 0;
    pCab->cur_leader = 0;

    for (cm = pCab->members; cm; cm = cm_next ){
      cm_next = cm->next;
      /* check if member's file exists if not, remove it */
      sprintf(pFile, PLAYER_DIR"%s", cm->name);
      if ( -1 == stat( pFile, &Stat)){
	cmember_from_cabal( cm, pCab );
	continue;
      }
      /* increae the member counts */
      pCab->cur_member ++;
      if (cm->rank == RANK_ELDER)
	pCab->cur_elder ++;
      else if (cm->rank == RANK_LEADER)
	pCab->cur_leader ++;
    }
  }

  /* cleanup */
  free_descriptor(d);
  closedir(Directory);

  /* send a note to all about bounties if any */
  if (mud_data.bounties >= 10){
    char buf[MSL];
    sprintf(buf,
	    "In a gesture of appreciation and gratitude the members of Syndicate wish to\n\r"\
	    "announce that total of %d bounties have been fulfilled, and thank you for your\n\r"\
	    "patronage.\n\r", mud_data.bounties);
    do_hal("all", "Bounty Report", buf, NOTE_NOTE);
    mud_data.bounties = 0;
  }

  /* return count of pfiles */
  if (p_count)
    mud_data.pfiles = p_count;
  if (fPurge)
    nlogf("Total files purged: %d.", tot_purged);
  if (fWipe)
    nlogf("Total files wiped: %d.", tot_wiped);
  if (fCount)
    send_stats(&cl_info);
  send_pfile_stats(mud_data.pfiles, tot_purged, tot_wiped, tot_count);
  nlogf("Total files searched: %d.", tot_count);
  nlogf("Current pfile count at: %d.", mud_data.pfiles);
  return tot_count;
}

/* initiliazes trap to index data */
void init_trap(TRAP_DATA* tr, TRAP_INDEX_DATA* pTrapIndex){
  int i  = 0;
  if (tr->name == NULL)
    tr->name		= str_dup(pTrapIndex->name);
  if (tr->echo == NULL)
    tr->echo		= str_dup(pTrapIndex->echo);
  if (tr->oEcho == NULL)
    tr->oEcho		= str_dup(pTrapIndex->oEcho);
  tr->armed		= IS_SET(pTrapIndex->flags, TRAP_DELAY) ? FALSE : TRUE;
  tr->age		= 0;

  tr->pIndexData	= pTrapIndex;
  tr->vnum		= pTrapIndex->vnum;
  tr->area		= pTrapIndex->area;
  tr->type		= pTrapIndex->type;
  tr->level		= pTrapIndex->level;
  tr->duration		= -1;
  tr->flags		= pTrapIndex->flags;

  for (i = 0; i < MAX_TRAPVAL; i++)
    tr->value[i] = pTrapIndex->value[i];
}


/* Big mama top level function. */
void boot_db( bool fHotReboot )
{
  char pFile [MSL];
  struct stat Stat;	//File statistics
  int sn;

    /* Init string stuff */
    {
	log_string("Initializing shared string manager...");
        init_string_space();
        fBootDb         = TRUE;
    }
    /* Init random number generator.  */
    {
	log_string("Initializing random number generator...");
        init_mm( );
    }
    /* Set mud data */
    reset_mdata();

    /* restore mud data if any */
    read_mud();
    /* if this is not hotreboot, reset crash time and deaths */
    if (!fHotReboot){
      mud_data.crash_time = mud_data.current_time;
      mud_data.deaths = 0;
    }

    /* check master questor */
    sprintf(pFile, PLAYER_DIR"%s", mud_data.questor);
    if ( -1 != stat( pFile, &Stat))
      mud_data.max_quest = 1;

    log_string("Assigning GRN's...");
    for ( sn = 0; race_table[sn].name; sn++){
      if (race_table[sn].grn != NULL)
	*race_table[sn].grn = sn;
    }
    /* assign gcn's for classes which have them */
    log_string("Assigning GCN's...");
    for ( sn = 0; class_table[sn].name; sn++){
      if (class_table[sn].gcn != NULL)
	*class_table[sn].gcn = sn;
    }

    /* Assign gsn's for skills which have them. */
    {
	log_string("Assigning GSN's...");
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	    if ( skill_table[sn].pgsn != NULL )
		*skill_table[sn].pgsn = sn;

	/* Assign gen's with values starting at MAX_SKILL */
	log_string("Assigning GEN's...");
	for ( sn = MAX_SKILL; sn < (MAX_SKILL + MAX_EFFECTS); sn++ )
	  {
	    if ( effect_table[gn_pos(sn)].pgen != NULL )
	      *effect_table[gn_pos(sn)].pgen = sn;
	  }
	/* Assign Songs */
	log_string("Assigning SONGS's...");
	for ( sn = 0; sn < MAX_SONG; sn++ ){
	  if ( song_table[sn].pgsn != NULL )
	    *song_table[sn].pgsn = sn;
	}
    }

    /* Read in all the area files. */
    {
	FILE *fpList;
	log_string("Reading area files...");
	if ( ( fpList = fopen( AREA_LIST, "r" ) ) == NULL )
	{
	    perror( AREA_LIST );
	    fpList = fopen( NULL_FILE, "r" );
	    fclose( fpList );
	    exit( 1 );
	}
	for ( ; ; )
	{
	    strcpy( strArea, fread_word( fpList ) );
	    if ( strArea[0] == '$' )
		break;
	    if ( strArea[0] == '-' )
	    {
		fpArea = stdin;
	    }
	    else if ( ( fpArea = fopen( strArea, "r" ) ) == NULL )
	    {
	        perror( strArea );
		fpArea = fopen( NULL_FILE, "r" );
		fclose( fpArea );
	        exit( 1 );
	    }
	    for ( ; ; )
	    {
		char *word;
		if ( fread_letter( fpArea ) != '#' )
		{
		    bug( "Boot_db: # not found.", 0 );
		    exit( 1 );
		}
		word = fread_word( fpArea );
                if ( word[0] == '$'                  )                 break;
		else if ( !str_cmp( word, "AREA"     ) ) load_area    (fpArea);
                else if ( !str_cmp( word, "AREADATA" ) ) new_load_area(fpArea);
		else if ( !str_cmp( word, "HELPS"    ) ) load_helps   (fpArea);
		else if ( !str_cmp( word, "MOBOLD"   ) ) load_old_mob (fpArea);
		else if ( !str_cmp( word, "MOBILES"  ) ) load_mobiles (fpArea);
		else if ( !str_cmp( word, "CABALS"   ) ) load_cabal_indexes(fpArea);
		else if ( !str_cmp( word, "ARMIES"   ) ) load_army_indexes(fpArea);
                else if ( !str_cmp( word, "MOBPROGS" ) ) load_mobprogs(fpArea);
		else if ( !str_cmp( word, "OBJPROGS" ) ) load_objprogs(fpArea);
		else if ( !str_cmp( word, "ROOMPROGS") ) load_roomprogs(fpArea);
		else if ( !str_cmp( word, "TRAPS"    ) ) load_traps(fpArea);
		else if ( !str_cmp( word, "OBJOLD"   ) ) bug("old obj found",0);
	  	else if ( !str_cmp( word, "OBJECTS"  ) ) load_objects (fpArea);
		else if ( !str_cmp( word, "RESETS"   ) ) load_resets  (fpArea);
		else if ( !str_cmp( word, "ROOMS"    ) ) load_rooms   (fpArea);
		else if ( !str_cmp( word, "SHOPS"    ) ) load_shops   (fpArea);
		else if ( !str_cmp( word, "SOCIALS"  ) ) load_socials (fpArea);
		else if ( !str_cmp( word, "SPECIALS" ) ) load_specials(fpArea);
		else
		{
		    bug( "Boot_db: bad section name.", 0 );
		    exit( 1 );
		}
	    }
	    if ( fpArea != stdin )
		fclose( fpArea );
	    fpArea = NULL;
	}
	fclose( fpList );
    }

    /* clans need to be loaded before cabals */
    log_string("Loading clans...");
    LoadClans( );
    /* cabals need to be loaded before their armies and rooms being restored */
    log_string("Loading cabals...");
    load_cabals( );
    log_string("Loading pacts...");
    load_pacts();
    /* area/objects are in, load virtual armies, cabal rooms and objects */
    log_string("Loading Virtual Cabal Data...");
    load_cvrooms( );

    /* Fix up exits.                    *
     * Declare db booting over.         *
     * Reset all areas once.            *
     * Load up the notes and ban files. */
    {
      log_string("Fixing cabals...");
      /* this also fixes up virtual cabal rooms */
      fix_cabals( );
      log_string("Fixing exits...");
      fix_exits( );
      log_string("Refreshing Support...");
      refresh_cabal_support();
      log_string("Refreshing Area Distances...");
      CalcCabalDistances();
      log_string("Fixing mobprogs...");
      fix_mobprogs( );
      log_string("Fixing objprogs...");
      fix_objprogs( );
      log_string("Fixing roomprogs...");
      fix_roomprogs( );

      daPrompt = str_dup( "<%hhp %mm %vmv> " );

/* CLEAR STRING ALOCATION */
      fBootDb	= FALSE;
      boot_done();

      log_string("Loading eq limits...");
      load_limits();
      log_string("Loading bounties...");
      load_bounties();
      log_string("Loading challenges...");
      load_challenges();
      if (fHotReboot)
	{
	  log_string("Loading saved items...");
	  load_obj(NULL);
	}
      log_string("Seeding objects...");
      seed_objects( );
      log_string("Refreshing area support values...");
      refresh_area_support( );
      log_string("Loading armies...");
      load_armies();
      log_string("Updating areas...");
      area_update( );
      loadallmobs = FALSE;
      log_string("Loading notes...");
      load_notes( );
      log_string("Loading helps...");
      load_helps_new( );
      log_string("Loading bans...");
      load_bans();
      log_string("Loading tomes...");
      InitTomes();

	/* MAINTANCE */
	if (IS_SET(reboot_act, REBOOT_COLOR)){
	  log_string("COLOR specified: Coloring Rooms...");
	  color_rooms();
	}
	if (IS_SET(reboot_act, REBOOT_PURGE)
	    || IS_SET(reboot_act, REBOOT_COUNT)
	    || IS_SET(reboot_act, REBOOT_WIPE)
	    || IS_SET(reboot_act, REBOOT_CABAL)){

	  if (IS_SET(reboot_act, REBOOT_PURGE))
	    log_string("PURGE specified: Purging rares...");
	  if (IS_SET(reboot_act, REBOOT_WIPE))
	    log_string("WIPE specified: Wiping old pfiles...");
	  if (IS_SET(reboot_act, REBOOT_WIPE))
	    log_string("WIPE specified: Wiping counting pfiles...");
	  if (IS_SET(reboot_act, REBOOT_CABAL))
	    log_string("CABAL specified: Compiling cabal data...");
	  /* if PURGE or WIPE is not set, only counting is done */
	  purge_players(IS_SET(reboot_act, REBOOT_PURGE),
			IS_SET(reboot_act, REBOOT_WIPE),
			IS_SET(reboot_act, REBOOT_CABAL));
	}
	if (IS_SET(reboot_act, REBOOT_HELPS)){
	  log_string("HELP specified: Generating helps...");
	  make_html_helps();
	}

        MOBtrigger = TRUE;
        if (fHotReboot)
            hotreboot_recover();
    }

}

void load_area( FILE *fp )
{

  AREA_DATA *pArea;
  pArea		= alloc_perm( sizeof(*pArea) );
  pArea->file_name	= fread_string(fp);
  pArea->area_flags   = AREA_LOADING;
  pArea->security     = 9;
  pArea->builders     = str_dup( "None" );
  pArea->vnum         = top_area;
  pArea->name		= fread_string( fp );
  pArea->prefix	= fread_string( fp );
  pArea->credits	= fread_string( fp );
  pArea->min_vnum	= fread_number(fp);
  pArea->max_vnum	= fread_number(fp);
  pArea->age		= 15;
  pArea->nplayer	= 0;
  pArea->empty	= FALSE;
  pArea->pCabal	= NULL;

  if ( !area_first )
    area_first = pArea;
  if ( area_last )
    {
      area_last->next = pArea;
      REMOVE_BIT(area_last->area_flags, AREA_LOADING);
    }
    area_last	= pArea;
    pArea->next	= NULL;
    top_area++;
    return;
}

/* OLC - Use these macros to load any new area formats that you *
 * choose to support.  See the new_load_area format below for a *
 * short example.                                               */
#if defined(KEY)
#undef KEY
#endif
#define KEY( literal, field, value )                \
                if ( !str_cmp( word, literal ) )    \
                {                                   \
                    field  = value;                 \
                    fMatch = TRUE;                  \
                    break;                          \
                }
#define SKEY( string, field )                       \
                if ( !str_cmp( word, string ) )     \
                {                                   \
                    free_string( field );           \
                    field = fread_string( fp );     \
                    fMatch = TRUE;                  \
                    break;                          \
                }

/* OLC - Snarf an 'area' header line.                           *
 * #AREAFILE                                                    *
 * Name   { All } Locke    Newbie School~                       *
 * Repop  A teacher pops in the room and says, 'Repop coming!'~ *
 * Recall 3001                                                  *
 * End                                                          */
void new_load_area( FILE *fp )
{
    AREA_DATA *pArea;
    char      *word = NULL;
    bool      fMatch = FALSE;
    int       count=0;

    /* Useless conditional */
    if( fMatch != FALSE )
        fMatch = FALSE;

    pArea               = alloc_perm( sizeof(*pArea) );
    pArea->age          = 15;
    pArea->nplayer      = 0;
    pArea->file_name    = str_dup( strArea );
    pArea->vnum         = top_area;
    pArea->name         = str_dup( "New Area" );
    pArea->builders     = str_dup( "" );
    pArea->prefix	= str_dup( "" );
    pArea->security     = 9;
    pArea->min_vnum     = 0;
    pArea->max_vnum     = 0;
    pArea->area_flags   = 0;
    pArea->pCabal	= NULL;
    pArea->bastion_max = 0;
    pArea->bastion_current = 0;

    for (count=0; count < MAX_CRIME; count++) {
      pArea->crimes[count] = 0;
    }
  /* set cabal distances to max */
    for (count  = 0; count < MAX_CABAL; count++){
      pArea->cabal_distance[count] = 999;
    }
    for ( ; ; )
    {
        word   = feof( fp ) ? "End" : fread_word( fp );
        fMatch = FALSE;
        switch ( UPPER(word[0]) )
        {
        case 'B':
            SKEY( "Builders", pArea->builders );
            break;
        case 'C':
	  SKEY( "Credits", pArea->credits );
	  if ( !str_cmp( word, "Crimes" ) ){
	    int max = fread_number( fp );
	    int i = 0;

	    for ( i = 0; i < MAX_CRIME; i++){
	      if (i < max)
		pArea->crimes[i] = fread_number( fp );
	      else
		pArea->crimes[i] = 0;
	      if (i == CRIME_OBSTRUCT)
		pArea->crimes[i] = CRIME_HEAVY;
	      else if (i == CRIME_MURDER)
		pArea->crimes[i] = UMAX(CRIME_LIGHT, pArea->crimes[i]);
	    }
	    break;
	  }
	  break;
        case 'N':
            SKEY( "Name", pArea->name );
	    log_string("Loading new area");
            break;
	case 'P':
            SKEY( "Pref", pArea->prefix );
        case 'S':
            KEY( "Security", pArea->security, fread_number( fp ) );
            KEY( "Startroom", pArea->startroom, fread_number( fp ) );
            break;
        case 'F':
	  if (!str_cmp( word, "Flags")){
	    int i = 0, flag = 0;
	    flag		= fread_number( fp );

	    for (i = 0; area_flags[i].name; i ++){
	      if (area_flags[i].settable == FALSE)
		flag &= ~area_flags[i].bit;
	    }
	    pArea->area_flags = flag;
	    break;
	  }
	  break;
        case 'V':
	  if ( !str_cmp( word, "VNUMs" ) )
            {
	      pArea->min_vnum = fread_number( fp );
	      pArea->max_vnum = fread_number( fp );
            }
	  break;
        case 'E':
	  if ( word != NULL && !str_cmp( word, "End" ) )
            {
	      fMatch = TRUE;
	      if ( area_first == NULL )
		area_first = pArea;
	      if ( area_last  != NULL )
		area_last->next = pArea;
	      area_last   = pArea;
	      pArea->next = NULL;
	      top_area++;
	      return;
            }
	  break;
	case 'M':
	  if ( !str_cmp( word, "Max_Bastions" ) ) {
	    pArea->bastion_max = fread_number( fp );
	  }
	  break;
        }
    }
}

/* Sets vnum range for area using OLC protection features. */
void assign_area_vnum( int vnum )
{
    if ( area_last->min_vnum == 0 || area_last->max_vnum == 0 )
        area_last->min_vnum = area_last->max_vnum = vnum;
    if ( vnum != URANGE( area_last->min_vnum, vnum, area_last->max_vnum ) )
    {
        if ( vnum < area_last->min_vnum )
            area_last->min_vnum = vnum;
        else
            area_last->max_vnum = vnum;
    }
}

void load_helps( FILE *fp )
{
  CABAL_INDEX_DATA* pci;
    HELP_DATA *pHelp;
    char* string;
    for ( ; ; )
    {
	pHelp		= alloc_perm( sizeof(*pHelp) );
	pHelp->level	= fread_number( fp );
	/* select type based on level and possible next word following */
	switch (pHelp->level){
	default:
	case HELP_ALL:
	  pHelp->type = 0;
	  break;
	case HELP_CLASS:
	  pHelp->type = class_lookup( fread_string (fp ));
	  break;
	case HELP_RACE:
	  string = fread_string( fp );
	  if (!str_cmp(string, "avatar"))
	    pHelp->type = -1;
	  else
	    pHelp->type = race_lookup( string );
	  free_string( string );
	  break;
	case HELP_CABAL:
	  if ( (pci = get_cabal_index_str( fread_string( fp ))) != NULL)
	    pHelp->type = pci->vnum;
	  break;
	case HELP_PSALM:
	  pHelp->type = psalm_lookup( fread_string (fp ));
	  break;
	}
	pHelp->keyword	= fread_string( fp );
	if ( pHelp->keyword[0] == '$' )
	    break;
	pHelp->text	= fread_string( fp );
	if ( !str_cmp( pHelp->keyword, "greeting" ) )
	    help_greeting = pHelp->text;
	if ( help_first == NULL )
	    help_first = pHelp;
	if ( help_last  != NULL )
	    help_last->next = pHelp;
	help_last	= pHelp;
	pHelp->next	= NULL;
	top_help++;
    }
}

void load_helps_new( ){
  HELP_DATA* pHelp;
  CABAL_INDEX_DATA* pci;
  FILE *fp;
  char* string;

  fclose( fpReserve );
  if ( ( fp = fopen( HELP_FILE, "r" ) ) == NULL ){
    fp = fopen( NULL_FILE, "r" );
    fclose (fp);
    perror( HELP_FILE );
  }
  else{
    for ( ; ; ){
      sh_int vnum;
      char letter;

      letter		= fread_letter( fp );
      if ( letter != '#' ){
	bug( "Load_help: # not found.", 0 );
	exit( 1 );
      }
      vnum	     	= fread_number( fp );
      if ( vnum == 0 )
	break;

      if ( get_help_index( vnum ) != NULL ){
	bug( "Load_helps: vnum %d duplicated.", vnum );
	exit( 1 );
      }
      pHelp		= new_help_index();
      pHelp->vnum		= vnum;
      pHelp->level	= fread_number( fp );
      /* select type based on level and possible next word following */
      switch (pHelp->level){
      default:
      case HELP_ALL:
	pHelp->type = 0;
	break;
      case HELP_CLASS:
	pHelp->type = class_lookup( fread_string (fp ));
	break;
      case HELP_RACE:
	string = fread_string( fp );
	if (!str_cmp(string, "avatar"))
	  pHelp->type = -1;
	else
	  pHelp->type = race_lookup( string );
	free_string( string );
	break;
      case HELP_CABAL:
	if ( (pci = get_cabal_index_str( fread_string( fp ))) != NULL)
	  pHelp->type = pci->vnum;
	break;
      case HELP_PSALM:
	pHelp->type = psalm_lookup( fread_string (fp ));
	break;
      }
      pHelp->keyword	= fread_string( fp );
      if ( pHelp->keyword[0] == '$' )
	break;
      pHelp->text	= fread_string( fp );
      if ( !str_cmp( pHelp->keyword, "greeting" ) )
	help_greeting = pHelp->text;
      if ( help_first == NULL )
	help_first = pHelp;
      if ( help_last  != NULL )
	help_last->next = pHelp;
      help_last	= pHelp;
      pHelp->next	= NULL;
    }
    fclose(fp);
    fpReserve = fopen( NULL_FILE, "r" );
  }
}

void load_old_mob( FILE *fp )
{
    MOB_INDEX_DATA *pMobIndex;
    int race;
    char name[MSL];
    if ( !area_last )
    {
        bug( "Load_mobiles: no #AREA seen yet.", 0 );
        exit( 1 );
    }
    for ( ; ; )
    {
	sh_int vnum;
	char letter;
	int iHash;
	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_mobiles: # not found.", 0 );
	    exit( 1 );
	}
	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;
	fBootDb = FALSE;
	if ( get_mob_index( vnum ) != NULL )
	{
	    bug( "Load_mobiles: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;
	pMobIndex			= alloc_perm( sizeof(*pMobIndex) );
	pMobIndex->vnum			= vnum;
        pMobIndex->area                 = area_last;
	pMobIndex->new_format		= FALSE;
	pMobIndex->player_name		= fread_string( fp );
	pMobIndex->short_descr		= fread_string( fp );
	pMobIndex->long_descr		= fread_string( fp );
	pMobIndex->description		= fread_string( fp );
	pMobIndex->long_descr[0]	= UPPER(pMobIndex->long_descr[0]);
	pMobIndex->description[0]	= UPPER(pMobIndex->description[0]);
	pMobIndex->act			= fread_flag( fp ) | ACT_IS_NPC;
	pMobIndex->affected_by		= fread_flag( fp );
	pMobIndex->pShop		= NULL;
	pMobIndex->alignment		= fread_number( fp );
	letter				= fread_letter( fp );
	pMobIndex->level		= fread_number( fp );
                                          fread_number( fp );
                                          fread_number( fp );
                                          fread_number( fp );
        /* 'd'          */                fread_letter( fp );
                                          fread_number( fp );
        /* '+'          */                fread_letter( fp );
                                          fread_number( fp );
                                          fread_number( fp );
        /* 'd'          */                fread_letter( fp );
                                          fread_number( fp );
        /* '+'          */                fread_letter( fp );
                                          fread_number( fp );
        pMobIndex->gold                 = fread_number( fp )/20;
                                          fread_number( fp );
        pMobIndex->start_pos            = fread_number( fp );
        pMobIndex->default_pos          = fread_number( fp );
  	if (pMobIndex->start_pos < POS_SLEEPING)
	    pMobIndex->start_pos = POS_STANDING;
	if (pMobIndex->default_pos < POS_SLEEPING)
	    pMobIndex->default_pos = POS_STANDING;
	if (pMobIndex->start_pos > POS_STANDING)
	    pMobIndex->start_pos = POS_STANDING;
	if (pMobIndex->default_pos > POS_STANDING)
	    pMobIndex->default_pos = POS_STANDING;
	pMobIndex->sex			= fread_number( fp );
	if (pMobIndex->sex > 3)
	    pMobIndex->sex = 0;
   	one_argument(pMobIndex->player_name,name);
   	if (name[0] == '\0' || (race =  race_lookup(name)) == 0)
   	{
            pMobIndex->race = race_lookup("human");
            pMobIndex->off_flags = OFF_DODGE|OFF_DISARM|OFF_TRIP|ASSIST_VNUM;
            pMobIndex->imm_flags = 0;
            pMobIndex->res_flags = 0;
            pMobIndex->vuln_flags = 0;
            pMobIndex->form = FORM_EDIBLE|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL;
            pMobIndex->parts = PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS;
    	}
    	else
    	{
            pMobIndex->race = race;
            pMobIndex->off_flags = OFF_DODGE|OFF_DISARM|OFF_TRIP|ASSIST_RACE|race_table[race].off;
            pMobIndex->imm_flags = race_table[race].imm;
            pMobIndex->res_flags = race_table[race].res;
            pMobIndex->vuln_flags = race_table[race].vuln;
            pMobIndex->form = race_table[race].form;
            pMobIndex->parts = race_table[race].parts;
    	}
	if ( letter != 'S' )
	{
	    bug( "Load_mobiles: vnum %d non-S.", vnum );
	    exit( 1 );
	}
        convert_mobile( pMobIndex );
	iHash			= vnum % MAX_KEY_HASH;
	pMobIndex->next		= mob_index_hash[iHash];
	mob_index_hash[iHash]	= pMobIndex;
	top_mob_index++;
        top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;
        assign_area_vnum( vnum );
	kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;
    }
}

void load_old_obj( FILE *fp )
{
    OBJ_INDEX_DATA *pObjIndex;
    if ( !area_last )
    {
        bug( "Load_objects: no #AREA seen yet.", 0 );
        exit( 1 );
    }
    for ( ; ; )
    {
	sh_int vnum;
	char letter;
	int iHash;
	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_objects: # not found.", 0 );
	    exit( 1 );
	}
	vnum				= fread_number( fp );
	if ( vnum == 0 )
	    break;
	fBootDb = FALSE;
	if ( get_obj_index( vnum ) != NULL )
	{
	    bug( "Load_objects: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;
	pObjIndex			= alloc_perm( sizeof(*pObjIndex) );
	pObjIndex->vnum			= vnum;
        pObjIndex->area                 = area_last;
	pObjIndex->new_format		= FALSE;
	pObjIndex->reset_num	 	= 0;
	pObjIndex->name			= fread_string( fp );
	pObjIndex->short_descr		= fread_string( fp );
	pObjIndex->description		= fread_string( fp );
                                          fread_string( fp );
	pObjIndex->short_descr[0]	= LOWER(pObjIndex->short_descr[0]);
	pObjIndex->description[0]	= UPPER(pObjIndex->description[0]);
	pObjIndex->material		= str_dup("");
	pObjIndex->class		= -1;
	pObjIndex->race			= 0;
	pObjIndex->item_type		= fread_number( fp );
	pObjIndex->extra_flags		= fread_flag( fp );
	pObjIndex->wear_flags		= fread_flag( fp );
        if (pObjIndex->item_type == ITEM_POTION && !CAN_WEAR(pObjIndex, ITEM_HOLD))
	    SET_BIT(pObjIndex->wear_flags,ITEM_HOLD);
	pObjIndex->value[0]		= fread_number( fp );
	pObjIndex->value[1]		= fread_number( fp );
	pObjIndex->value[2]		= fread_number( fp );
	pObjIndex->value[3]		= fread_number( fp );
	pObjIndex->value[4]		= 0;
	pObjIndex->level		= 0;
	pObjIndex->condition 		= 100;
	pObjIndex->weight		= fread_number( fp );
        pObjIndex->cost                 = fread_number( fp );
                                          fread_number( fp );
	if (pObjIndex->item_type == ITEM_WEAPON
	&& (is_name("two",pObjIndex->name) || is_name("two-handed",pObjIndex->name) || is_name("claymore",pObjIndex->name)))
	    SET_BIT(pObjIndex->value[4],WEAPON_TWO_HANDS);
	for ( ; ; )
	{
	    char letter;
	    letter = fread_letter( fp );
	    if ( letter == 'A' )
	    {
		AFFECT_DATA *paf;
		paf			= alloc_perm( sizeof(*paf) );
		paf->where		= TO_OBJECT;
		paf->type		= -1;
                paf->level              = 20;
		paf->duration		= -1;
		paf->location		= fread_number( fp );
		paf->modifier		= fread_number( fp );
		paf->bitvector		= 0;
		paf->next		= pObjIndex->affected;
		pObjIndex->affected	= paf;
		top_affect++;
	    }
	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;
		ed			= alloc_perm( sizeof(*ed) );
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= pObjIndex->extra_descr;
		pObjIndex->extra_descr	= ed;
		top_ed++;
	    }
            else if ( letter == 'O' )
	    {
		PROG_LIST *pOprog;
		char *word;
		int trigger = 0;

		pOprog			= alloc_perm(sizeof(*pOprog));
		word			= fread_word( fp );
		if ( !(trigger = flag_lookup( word, oprog_flags )) )
		{
		    bug( "OBJprogs: invalid trigger.",0);
		    exit(1);
		}
		SET_BIT( pObjIndex->oprog_flags, trigger );
		pOprog->trig_type	= trigger;
		pOprog->vnum	 	= fread_number( fp );
		pOprog->trig_phrase	= fread_string( fp );
		pOprog->next		= pObjIndex->oprogs;
		pObjIndex->oprogs	= pOprog;
	    }
            else if ( letter == 'T' )
            {
                OBJ_SPELL_DATA *new_spell;
                new_spell                  = alloc_perm( sizeof( *new_spell ) );
                new_spell->spell           = fread_number( fp );
                new_spell->target          = fread_number( fp );
		new_spell->percent	   = fread_number( fp );
		new_spell->message	   = fread_string( fp );
		new_spell->message2	   = fread_string( fp );
		new_spell->next		   = pObjIndex->spell;
                pObjIndex->spell           = new_spell;
            }
	    else if ( letter == 'M' )
	    {
		OBJ_MESSAGE_DATA *new_message;
		new_message		= alloc_perm( sizeof( *new_message ) );
		new_message->onself	= fread_string( fp );
		new_message->onother	= fread_string( fp );
		new_message->offself	= fread_string( fp );
		new_message->offother	= fread_string( fp );
		pObjIndex->message	= new_message;
	    }
            else
	    {
		ungetc( letter, fp );
		break;
	    }
	}
        if (pObjIndex->item_type == ITEM_ARMOR)
        {
            pObjIndex->value[1] = pObjIndex->value[0];
            pObjIndex->value[2] = pObjIndex->value[1];
        }
	switch ( pObjIndex->item_type )
	{
	case ITEM_PILL:
	case ITEM_POTION:
	case ITEM_RELIC:
        case ITEM_SOCKET:
	case ITEM_ARTIFACT:
	case ITEM_SCROLL:
	case ITEM_HERB:
	    pObjIndex->value[1] = slot_lookup( pObjIndex->value[1] );
	    pObjIndex->value[2] = slot_lookup( pObjIndex->value[2] );
	    pObjIndex->value[3] = slot_lookup( pObjIndex->value[3] );
	    pObjIndex->value[4] = slot_lookup( pObjIndex->value[4] );
	    break;
	case ITEM_STAFF:
	case ITEM_WAND:
	    pObjIndex->value[3] = slot_lookup( pObjIndex->value[3] );
	    break;
	}
        convert_object( pObjIndex );
	iHash			= vnum % MAX_KEY_HASH;
	pObjIndex->next		= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObjIndex;
	top_obj_index++;
        top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;
        assign_area_vnum( vnum );
    }
}

/* OLC - Adds a reset to a room. *
 * Similar to add_reset in olc.c */
void new_reset( ROOM_INDEX_DATA *pR, RESET_DATA *pReset )
{
    RESET_DATA *pr;
    if ( !pR )
       return;
    pr = pR->reset_last;
    if ( !pr )
    {
        pR->reset_first = pReset;
        pR->reset_last  = pReset;
    }
    else
    {
        pR->reset_last->next = pReset;
        pR->reset_last       = pReset;
        pR->reset_last->next = NULL;
    }
    top_reset++;
}

void load_reset( FILE* fp ){
  RESET_DATA *pReset = NULL;
  int iLastRoom = 0, iLastObj  = 0;

  for ( ; ; )
    {
      ROOM_INDEX_DATA *pRoomIndex;
      OBJ_INDEX_DATA *temp_index;
      char letter = '\0';

      letter = fread_letter (fp);
      /* Viri: For some reason compiler thought letter could be uninitilised at this point. */



      if (letter == 'S') {
	break;
      }
      if (letter == '*' ) {
	fread_to_eol( fp );
	continue;
      }
      pReset		= alloc_perm( sizeof(*pReset) );
      pReset->command	= letter;
      fread_number( fp );
      pReset->arg1	= fread_number( fp );
      pReset->arg2	= fread_number( fp );
      pReset->arg3    = (letter == 'G' || letter == 'R') ? 0 : fread_number( fp );
      pReset->arg4    = (letter == 'P' || letter == 'M' || letter == 'T') ? fread_number(fp) : 0;
      fread_to_eol( fp );
      switch ( letter )
	{
	default:
	  bug( "Load_resets: bad command '%c'.", letter );
	  exit( 1 );
	  break;
	case 'M':
	  pRoomIndex = get_room_index ( pReset->arg3 );
	  if ( pRoomIndex )
            {
	      new_reset( pRoomIndex, pReset );
	      iLastRoom = pReset->arg3;
            }
	  break;
	case 'O':
	  temp_index = get_obj_index ( pReset->arg1 );
	  temp_index->reset_num++;
	  if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
            {
	      new_reset( pRoomIndex, pReset );
	      iLastObj = pReset->arg3;
            }
	  break;
	case 'P':
	  temp_index = get_obj_index ( pReset->arg1 );
	  temp_index->reset_num++;
	  if ( ( pRoomIndex = get_room_index ( iLastObj ) ) )
                new_reset( pRoomIndex, pReset );
	    break;
	case 'G':
	case 'E':
            temp_index = get_obj_index  ( pReset->arg1 );
            temp_index->reset_num++;
            if ( ( pRoomIndex = get_room_index ( iLastRoom ) ) )
            {
                new_reset( pRoomIndex, pReset );
                iLastObj = iLastRoom;
            }
            break;
	case 'D':
/* this is now obsolete
   pRoomIndex = get_room_index( pReset->arg1 );
   pexit = pRoomIndex->exit[pReset->arg2];
   if ( pReset->arg2 < 0 || pReset->arg2 > (MAX_DIR -1) || !pRoomIndex || !pexit || !IS_SET( pexit->rs_flags, EX_ISDOOR ) )
   {
   bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
   exit( 1 );
   }
   switch ( pReset->arg3 )
   {
   default:
   bug( "Load_resets: 'D': bad 'locks': %d." , pReset->arg3);
   case 0: break;
   case 1: SET_BIT( pexit->rs_flags, EX_CLOSED );
   SET_BIT( pexit->exit_info, EX_CLOSED ); break;
   case 2: SET_BIT( pexit->rs_flags, EX_CLOSED | EX_LOCKED );
                    SET_BIT( pexit->exit_info, EX_CLOSED | EX_LOCKED ); break;
		    }
		    break;
*/
	case 'R':
	  pRoomIndex		= get_room_index( pReset->arg1 );
	  if ( pReset->arg2 < 0 || pReset->arg2 > MAX_DIR )
	    {
	      bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
	      exit( 1 );
	    }
	  if ( pRoomIndex )
	    new_reset( pRoomIndex, pReset );
	  break;
	case 'T':
	  if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) ){
	    new_reset( pRoomIndex, pReset );
	  }
	  break;
	}
    }
}

void load_resets( FILE *fp )
{
    if ( !area_last )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }
    load_reset( fp );
}

/* reads a single room's data */
void read_room( FILE *fp, ROOM_INDEX_DATA* pRoomIndex, AREA_DATA* area, int vnum ){
  int watch_vnum;
  Double_List * tmp_list;
  int door = 0;

  pRoomIndex->people		= NULL;
  pRoomIndex->contents		= NULL;
  pRoomIndex->extra_descr	= NULL;
  pRoomIndex->area		= area;
  pRoomIndex->vnum		= vnum;
  pRoomIndex->watch_vnums		= NULL;
  tmp_list = pRoomIndex->watch_vnums;
  pRoomIndex->name		= fread_string( fp );
  pRoomIndex->description	= fread_string( fp );
  pRoomIndex->description2	= fread_string( fp );
  if (pRoomIndex->description2[0])
    night_count++;
  fread_number( fp );
  pRoomIndex->room_flags		= fread_flag( fp );
  pRoomIndex->room_flags2		= fread_flag( fp );
  pRoomIndex->sector_type		= fread_number( fp );
  pRoomIndex->light		= 0;
  pRoomIndex->psitimer		= 0;
  pRoomIndex->temp		= 72;
  for ( door = 0; door <= 5; door++ )
    pRoomIndex->exit[door] = NULL;
  pRoomIndex->heal_rate = 100;
  pRoomIndex->mana_rate = 100;

  for ( ; ; ){
    char letter = fread_letter( fp );
    if ( letter == 'S' )
      break;
    else if ( letter == 'H')
      pRoomIndex->heal_rate = fread_number(fp);
    else if ( letter == 'M')
      pRoomIndex->mana_rate = fread_number(fp);
    else if ( letter == 'W') {
      watch_vnum = fread_number(fp);
      if (watch_vnum > 0) {
	if (pRoomIndex->watch_vnums == NULL) {
	  pRoomIndex->watch_vnums = (Double_List *) malloc (sizeof (Double_List));
	  pRoomIndex->watch_vnums->cur_entry = (void *) (long) watch_vnum;
	  pRoomIndex->watch_vnums->next_node = NULL;
	  pRoomIndex->watch_vnums->prev_node = NULL;
	  tmp_list = pRoomIndex->watch_vnums;
	}
	else {
	  tmp_list->next_node = (Double_List *) malloc (sizeof (Double_List));
	  tmp_list->next_node->cur_entry = (void *) (long) watch_vnum;
	  tmp_list->next_node->next_node = NULL;
	  tmp_list->next_node->prev_node = tmp_list;
	  tmp_list = tmp_list->next_node;
	}
      }
    }
    else if ( letter == 'C')
      {
	CABAL_INDEX_DATA* pci;
	if (pRoomIndex->pCabal)
	  {
	    bug("read_room duplicate cabal fields.",0);
	    exit(1);
	  }
	else if ( (pci = get_cabal_index_str(fread_string(fp))) != NULL)
	  pRoomIndex->cabal_vnum = pci->vnum;
      }
    else if ( letter == 'T')
      pRoomIndex->temp = fread_number(fp);
    else if ( letter == 'D' )
      {
	EXIT_DATA *pexit;
	door = fread_number( fp );
	if ( door < 0 || door >= MAX_DOOR ){
	  bug( "read_room: vnum %d has bad door number.", vnum );
	  exit( 1 );
	}
	pexit			= new_exit();
	pexit->description	= fread_string( fp );
	pexit->keyword		= fread_string( fp );
	pexit->rs_flags         = fread_flag( fp );
	pexit->exit_info	= pexit->rs_flags;
	pexit->key		= fread_number( fp );
	pexit->vnum		= fread_number( fp );
	pexit->orig_door        = door;
	pRoomIndex->exit[door]	= pexit;
	pRoomIndex->old_exit[door] = pexit;
	top_exit++;
      }
    else if ( letter == 'E' )
      {
	EXTRA_DESCR_DATA *ed;
	ed			= alloc_perm( sizeof(*ed) );
	ed->keyword		= fread_string( fp );
	ed->description		= fread_string( fp );
	ed->next		= pRoomIndex->extra_descr;
	pRoomIndex->extra_descr	= ed;
	top_ed++;
      }
    else if ( letter == 'O' )
      fread_string ( fp );
    else if ( letter == 'P' )
      {
	PROG_LIST *pRprog;
	char *word;
	int trigger = 0;

	pRprog		= alloc_perm(sizeof(*pRprog));
	word		= fread_word( fp );
	if ( !(trigger = flag_lookup( word, rprog_flags )) )
	  {
	    bug( "ROOMprogs: invalid trigger.",0);
	    exit(1);
	  }
	SET_BIT( pRoomIndex->rprog_flags, trigger );
	pRprog->trig_type	= trigger;
	pRprog->vnum		= fread_number( fp );
	pRprog->trig_phrase	= fread_string( fp );
	pRprog->next		= pRoomIndex->rprogs;
	pRoomIndex->rprogs	= pRprog;
      }
    else
      {
	bug( "read_room: vnum %d has flag not recognized.", vnum );
	exit( 1 );
      }
  }
}

/* gets the next free vnum for vir. room */
int get_vir_vnum(){
  int vnum = 0;
  /* check if there are any unused ones waiting */
  if (last_vir_vnum > 0){
    vnum = free_vir_vnums[--last_vir_vnum];
  }
  else{
    vnum = MAX_ROOM_VNUM + top_vir_room;
    top_vir_room++;
  }
  return vnum;
}
/* records  a vir vnum to be reused */
void free_vir_vnum( int vnum ){
  if (last_vir_vnum < MAX_FREE_VNUM)
    free_vir_vnums[last_vir_vnum++] = vnum;
}

/* assigns a vnum, hashes, and links a single virtual room to its exits */
void load_vir_room( ROOM_INDEX_DATA* pRoom, AREA_DATA* pArea ){
  EXIT_DATA* pexit;
  int door = 0;
  int players = 0;
  int iHash;

  /* first we give the room a new vnum which is 1 higher then the last one */
  pRoom->vnum		= get_vir_vnum();
  pRoom->area		= pArea;
  /* if this room has a cabal, we increase its total */
  if (pRoom->pCabal)
    get_parent(pRoom->pCabal)->cur_room++;


  /* we now hash and link the room to the room index table */
  iHash			= pRoom->vnum % MAX_KEY_HASH;
  pRoom->next		= room_index_hash[iHash];
  room_index_hash[iHash]= pRoom;

  /* refresh resets for new vnum */
  fix_resets( pRoom->reset_first, pRoom->vnum );

  /* deal with exits now if this is after a normal reboot */
  /* during a boot, fix_exits() deals with this */
  if (fBootDb)
    return;

  for ( door = 0; door < MAX_DOOR; door++ ){
    if ( ( pexit = pRoom->exit[door] ) == NULL )
      continue;
    else{
      ROOM_INDEX_DATA* pToRoom;
      int rev = rev_dir[door];

      if ( pexit->vnum <= 0 || (pToRoom = get_room_index(pexit->vnum)) == NULL){
	pexit->to_room = NULL;
	continue;
      }
      pexit->to_room = pToRoom;
      /* make sure the other side has a free exit */
      if (pToRoom->exit[rev] != NULL){
	char buf[MIL];
	sprintf( buf, "load_vir_room: could not link door %d for vir. room %s to room %d as exit already linked.",
		 door, pRoom->name, pexit->vnum);
	bug( buf, 0);
	continue;
      }
      else{
	pToRoom->exit[rev]		=	new_exit();
	pToRoom->exit[rev]->vnum	=	pRoom->vnum;
	pToRoom->exit[rev]->orig_door	=	rev;
	pToRoom->exit[rev]->to_room	=	pRoom;
	pToRoom->exit[rev]->rs_flags	=	pexit->rs_flags;
	pToRoom->exit[rev]->exit_info	=	pexit->exit_info;
	pToRoom->exit[rev]->key		=	pexit->key;
	if (IS_NULLSTR(pToRoom->exit[rev]->keyword))
	  pToRoom->exit[rev]->keyword	=	str_dup( pexit->keyword );
      }
    }//end if exit
  }//end for door

/* clear no reset flag if one exists, and reset room since this is after reboot */
  REMOVE_BIT( pRoom->room_flags2, ROOM_NO_RESET);
  /* we have to cheat here a bit to load objects properly */
  players = pRoom->area->nplayer;
  pRoom->area->nplayer = 0;

  reset_room( pRoom );

  pRoom->area->nplayer = players;
}

/* unlinkes and removes a single virtual room, vnum is recorded to be reused */
bool unload_vir_room( ROOM_INDEX_DATA* pRoom ){
  EXIT_DATA* pexit;
  ROOM_INDEX_DATA* prev;
  CHAR_DATA* vch;
  int iHash;
  int door;

  if (!IS_VIRROOM( pRoom))
    return FALSE;

  /* first we free up the virtual vnum */
  free_vir_vnum( pRoom->vnum);

  /* if this room has a cabal, we increase its total */
  if (pRoom->pCabal)
    get_parent(pRoom->pCabal)->cur_room--;


  /* unlink the exits, reverse too if any */
  for ( door = 0; door < MAX_DOOR; door++ ){
    if ( ( pexit = pRoom->exit[door] ) == NULL )
      continue;
    else{
      int rev = rev_dir[door];
      /* we free/unlink any reverse exit that leads here */
      if (pexit->to_room && pexit->to_room->exit[rev] && pexit->to_room->exit[rev]->to_room == pRoom){
	free_exit( pexit->to_room->exit[rev]);
	pexit->to_room->exit[rev] = NULL;
      }
      /* now we free this exit */
      free_exit( pexit );
      pRoom->exit[door] = NULL;
    }
  }

  /* we now throw any players/mobs out of the room */
  if (pRoom->people){
    act_new("With a flash of bright light the area around you disappears and you find yourself elsewhere!.",
	    pRoom->people, NULL, NULL, TO_ALL, POS_DEAD);
    while (pRoom->people){
      vch = pRoom->people;
      char_from_room( vch );

      if (!IS_NPC(vch) || (IS_NPC(vch) && IS_AFFECTED(vch, AFF_CHARM) && vch->leader)){
	ROOM_INDEX_DATA* was_in_room = IS_NPC( vch ) ? vch->leader->was_in_room : vch->was_in_room;
	if (was_in_room)
	  char_to_room( vch, was_in_room );
	else
	  char_to_room( vch, get_room_index( ROOM_VNUM_TEMPLE ) );
      }
      else
	extract_char( vch, TRUE );
    }
  }

  /* and we extract any objects in there */
  while (pRoom->contents){
    OBJ_DATA* obj = pRoom->contents;
    pRoom->contents = obj->next_content;
    obj_from_room( obj );
    extract_obj( obj );
  }

/* we now remove the room */
  iHash			= pRoom->vnum % MAX_KEY_HASH;
  prev			= room_index_hash[iHash];

  if (prev == NULL ){
    bug("unload_vir_room: error unlinking room vnum %d", pRoom->vnum );
    return FALSE;
  }
  if (prev == pRoom){
    room_index_hash[iHash] = prev->next;
  }
  else{
    while ( prev->next != NULL && prev->next != pRoom)
      prev = prev->next;
    if (prev == NULL){
      bug("unlink_vir_room: room not found vnum %d.", pRoom->vnum);
      return FALSE;
    }
    prev->next = prev->next->next;
  }
  /* free the room now */
  free_room_index( pRoom );
  return TRUE;
}


void load_rooms( FILE *fp )
{
  ROOM_INDEX_DATA *pRoomIndex;

  if ( area_last == NULL ){
    bug( "Load_resets: no #AREA seen yet.", 0 );
    exit( 1 );
  }
  for ( ; ; ){
    sh_int vnum;
    char letter;
    int iHash;
    letter				= fread_letter( fp );
    if ( letter != '#' )
      {
	bug( "Load_rooms: # not found.", 0 );
	exit( 1 );
      }
    vnum				= fread_number( fp );
    if ( vnum == 0 )
      break;
    fBootDb = FALSE;
    if ( get_room_index( vnum ) != NULL )
      {
	bug( "Load_rooms: vnum %d duplicated.", vnum );
	exit( 1 );
      }
    fBootDb = TRUE;
    pRoomIndex			= new_room_index();

    read_room( fp, pRoomIndex, area_last, vnum );

    iHash			= vnum % MAX_KEY_HASH;
    pRoomIndex->next	= room_index_hash[iHash];
    room_index_hash[iHash]	= pRoomIndex;
    top_room++;
    top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room;
  }
}

void load_shops( FILE *fp )
{
    SHOP_DATA *pShop;
    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	int iTrade;
	pShop			= alloc_perm( sizeof(*pShop) );
	pShop->keeper		= fread_number( fp );
	if ( pShop->keeper == 0 )
	    break;
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
	    pShop->buy_type[iTrade]	= fread_number( fp );
	pShop->profit_buy	= fread_number( fp );
	pShop->profit_sell	= fread_number( fp );
	pShop->open_hour	= fread_number( fp );
	pShop->close_hour	= fread_number( fp );
				  fread_to_eol( fp );
	pMobIndex		= get_mob_index( pShop->keeper );
	pMobIndex->pShop	= pShop;
	if ( shop_first == NULL )
	    shop_first = pShop;
	if ( shop_last  != NULL )
	    shop_last->next = pShop;
	shop_last	= pShop;
	pShop->next	= NULL;
	top_shop++;
    }
}

void load_specials( FILE *fp )
{
    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	char letter;
	switch ( letter = fread_letter( fp ) )
	{
	default:
	    bug( "Load_specials: letter '%c' not *MS.", letter );
	    exit( 1 );
	case 'S':
	    return;
	case '*':
	    break;
	case 'M':
	    pMobIndex		= get_mob_index	( fread_number ( fp ) );
	    pMobIndex->spec_fun	= spec_lookup	( fread_word   ( fp ) );
	    if ( pMobIndex->spec_fun == 0 )
	    {
		bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
		exit( 1 );
	    }
	    break;
	}
	fread_to_eol( fp );
    }
}

/* adds an exit to area list of exits */
void add_area_exit( AREA_DATA* pa, EXIT_DATA* pe ){

  if (pa->exits == NULL)
    pa->exits = pe;
  else{
    pe->next_in_area = pa->exits;
    pa->exits = pe;
  }
}

/* Translate all room exits from virtual to real. *
 * Has to be done after all rooms are read in.    *
 * Check for bad reverse exits.                   */
void fix_exits( void ){
  ROOM_INDEX_DATA *pRoomIndex;
  EXIT_DATA *pexit;
  int iHash, door;
  for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ ){
    for ( pRoomIndex  = room_index_hash[iHash]; pRoomIndex != NULL; pRoomIndex  = pRoomIndex->next ){
      bool fexit = FALSE;
      bool fVir = IS_VIRROOM( pRoomIndex );
      /* Useless conditional */
      if( fexit != FALSE )
        fexit = FALSE;

      if (pRoomIndex->cabal_vnum)
	pRoomIndex->pCabal = get_cabal_vnum( pRoomIndex->cabal_vnum );

      for ( door = 0; door <= 5; door++ ){
	ROOM_INDEX_DATA* pToRoom;
	/* check if this room has an exit in this direction */
	if ( ( pexit = pRoomIndex->exit[door] ) == NULL )
	  continue;
	/* if an exit exists, check if the room it leads to exists */
	if ( pexit->vnum <= 0 || (pToRoom = get_room_index(pexit->vnum)) == NULL){
	  char buf[MIL];
	  sprintf( buf, "fix_exists: could not link room %d to vnum %d", pRoomIndex->vnum, pexit->vnum);
	  bug(buf, 0);
	  pexit->to_room = NULL;
	  continue;
	}
	else{
	  fexit = TRUE;
	  pexit->to_room = pToRoom;
	  /* check for exit leading out of area */
	  if (pRoomIndex->area != pToRoom->area)
	    add_area_exit( pRoomIndex->area, pexit );

	  /* if we are working on a virtual room, and to_room is not virtual we automaticly link the other room
	     backwards as well */
	  if (fVir && !IS_VIRROOM(pToRoom)){
	    int rev = rev_dir[door];
	    /* make sure the other side has a free exit */
	    if (pToRoom->exit[rev] != NULL){
	      char buf[MIL];
	      sprintf( buf, "fix_exits: could not link door %d for vir. room %s to room %d as exit already linked.",
		       door, pRoomIndex->name, pexit->vnum);
	      bug( buf, 0);
	      continue;
	    }
	    else{
	      pToRoom->exit[rev]		=	new_exit();
	      pToRoom->exit[rev]->orig_door	=	rev;
	      pToRoom->exit[rev]->vnum		=	pRoomIndex->vnum;
	      pToRoom->exit[rev]->to_room	=	pRoomIndex;
	      pToRoom->exit[rev]->rs_flags	=	pexit->rs_flags;
	      pToRoom->exit[rev]->exit_info	=	pexit->exit_info;
	      pToRoom->exit[rev]->key		=	pexit->key;

	      if (pToRoom->exit[rev]->to_room->area != pToRoom->area)
		add_area_exit(pToRoom->area, pToRoom->exit[rev]);
	    }
	  }
	}//end if virtual room
      }//end for door
    }//end for room in current hash position
  }//end for each hash table cell
}

void area_update( void )
{
  AREA_DATA *pArea;
  int stringdif, permdif;
  char buf[MSL];
  int hash;
  ROOM_INDEX_DATA *room;

  for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
      /* cabal influence check */
      AreaInfluenceUpdate( pArea );

      //raid update (no repops during a raid)
      if (pArea->raid){
	raid_update(pArea, pArea->raid );
	continue;
      }

      if ( pArea->idle < 9999 )
	pArea->idle++;
      if ( ++pArea->age < 5 )
	continue;
      /* MUDSCHOOL resets every 5 ticks min */
      else if (IS_AREA(pArea, AREA_MUDSCHOOL))
	pArea->age = 999;
      /* Areas with flag of "NOREPOP" do not reset if player is inside */
      if (IS_SET(pArea->area_flags, AREA_NOREPOP)
	  && pArea->nplayer > 0)
	continue;

      /* EMPTY means "not-dirty".  This flag is set FALSE if a player enters area.  It is only set TRUE below. */
      if ( (!pArea->empty && (pArea->nplayer == 0 || pArea->age >= 15))
	   || pArea->age >= 30)
	{
	  int string_count = nAllocString, perm_count = nAllocPerm, string_size = sAllocString, perm_size = sAllocPerm;
	  reset_area( pArea );
	  sprintf(buf,"%s has just been reset.",pArea->name);
	  wiznet(buf,NULL,NULL,WIZ_RESETS,0,0);
	  if (string_count != nAllocString || perm_count < nAllocPerm )
	    {
	      stringdif = sAllocString - string_size;
	      permdif = sAllocPerm - perm_size;
	      sprintf(buf,"Memcheck: strings %d bytes : perms %d bytes : %s : area reset", stringdif, permdif, pArea->name );
	      wiznet(buf,NULL,NULL,WIZ_MEMORY,0,0);
	    }
	  pArea->age = number_range( 0, 5 );
	  if (pArea->nplayer == 0)
	    pArea->empty = TRUE;
	}
    }
  /* ROOMprog Triggers and tick updates */
  for ( hash = 0; hash < MAX_KEY_HASH; hash++ ){
    for ( room = room_index_hash[hash]; room; room = room->next ){
      if ( room->area->empty )
		continue;

      /* echos battle progress if people in room and armies in room */
      army_battle_echo( room );

      if ( room->rprog_delay > 0 )
	{
	  if ( --room->rprog_delay <= 0 && HAS_TRIGGER_ROOM( room, TRIG_DELAY ))
	    p_percent_trigger( NULL, NULL, room, NULL, NULL, NULL, TRIG_DELAY );
	}
      if ( HAS_TRIGGER_ROOM( room, TRIG_RANDOM ) )
	p_percent_trigger( NULL, NULL, room, NULL, NULL, NULL, TRIG_RANDOM );
    }
  }
}

/* OLC reset one room.           *
 * Called by reset_area and olc. */
void reset_room( ROOM_INDEX_DATA *pRoom )
{
    RESET_DATA  *pReset;
    CHAR_DATA   *pMob = NULL, *mob, *LastMob = NULL;
    OBJ_DATA    *pObj = NULL, *LastObj = NULL;
    TRAP_DATA* pTrap;
    int iExit, level = 0;
    bool last = FALSE;
    if ( !pRoom )
        return;
/* EXITS */
    for ( iExit = 0;  iExit < MAX_DIR;  iExit++ )
    {
        EXIT_DATA *pExit;
        if ( ( pExit = pRoom->exit[iExit] ) ){
	  pExit->exit_info = pExit->rs_flags;
	  if ( ( pExit->to_room != NULL ) && ( ( pExit = pExit->to_room->exit[rev_dir[iExit]] ) ) )
	    pExit->exit_info = pExit->rs_flags;
        }
    }
/* CABAL GUARD and ALTAR */
    if (pRoom->pCabal && pRoom->pCabal->anchor == pRoom){
      bool fAltar = FALSE;
      /* check if a galtar exists here already */
      for (mob = pRoom->pCabal->anchor->people; mob != NULL; mob = mob->next_in_room){
	if (!IS_NPC(mob))
	  continue;
	else if (mob->pIndexData->vnum == MOB_VNUM_ALTAR && is_same_cabal(mob->pCabal, pRoom->pCabal)){
	  fAltar = TRUE;
	  break;
	}
      }
      /* RESET ALTAR */
      if (!fAltar)
	reset_altar( pRoom, pRoom->pCabal );

      /* RESET GUARD */
      if (IS_CABAL(pRoom->pCabal, CABAL_GUARDMOB) && !is_captured( pRoom->pCabal)){
	reset_cabal_guard( pRoom, pRoom->pCabal );
      }
      /* we remove existing guard if there is no guardmob flag */
      else if (!IS_CABAL(pRoom->pCabal, CABAL_GUARDMOB)){
	remove_cabalguard( pRoom->pCabal );
      }
    }

/* RESETS */
    for ( pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next )
    {
        MOB_INDEX_DATA  *pMobIndex;
        OBJ_INDEX_DATA  *pObjIndex, *pObjToIndex;
        ROOM_INDEX_DATA *pRoomIndex, *pRoomIndexPrev;
	TRAP_INDEX_DATA *pTrapIndex;
	TRAP_DATA* pTrapTo;
	bool force = FALSE;
        int count = 0;
        switch ( pReset->command )
        {
        default:
            bug( "Reset_room: bad command %c.", pReset->command );
            break;
        case 'M':
            if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
            {
                bug( "Reset_room: 'M': bad vnum %d.", pReset->arg1 );
                continue;
            }
	    if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
	    {
		bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
		continue;
	    }
	    if (loadallmobs && pReset->arg4 == -1)
		force = TRUE;
            if (!force && pMobIndex->count >= pReset->arg2 )
	    {
		last = FALSE;
		break;
	    }
            for (mob = pRoomIndex->people; mob != NULL; mob = mob->next_in_room)
		if (mob->pIndexData == pMobIndex)
		{
		    count++;
		    if (count >= pReset->arg4)
		    {
		    	last = FALSE;
                        break;
		    }
                }
            if (!force && count >= pReset->arg4)
                break;
            pMob = create_mobile( pMobIndex );
	    pMob->homevnum = pRoom->vnum;
            if ( room_is_dark( pRoom ) )
                SET_BIT(pMob->affected_by, AFF_INFRARED);
            pRoomIndexPrev = get_room_index( pRoom->vnum - 1 );
            if ( pRoomIndexPrev && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
                SET_BIT( pMob->act, ACT_PET);
            char_to_room( pMob, pRoom );
            LastMob = pMob;
            level  = URANGE( 0, pMob->level - 2, LEVEL_HERO - 1 );
            last = TRUE;
            break;
        case 'O':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                bug( "Reset_room: 'O' 1 : bad vnum %d", pReset->arg1 );
                bugf ("%d %d %d %d",pReset->arg1, pReset->arg2, pReset->arg3,pReset->arg4 );
                continue;
            }
            if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
            {
                bug( "Reset_room: 'O' 2 : bad vnum %d.", pReset->arg3 );
                bugf ("%d %d %d %d",pReset->arg1, pReset->arg2, pReset->arg3,pReset->arg4 );
                continue;
            }
//changed to allow obj repop if players present if ( pRoom->area->nplayer > 0 || count_obj_list( pObjIndex, pRoom->contents ) > 0 )
	    if ( count_obj_list( pObjIndex, pRoom->contents ) > 0 )
	      {
		last = FALSE;
		break;
	      }
            if (!can_spawn(pObjIndex))
	      break;
            pObj = create_object( pObjIndex,UMIN(number_fuzzy( level ), LEVEL_HERO -1) );
	    pObj->homevnum = pRoom->vnum;
            obj_to_room( pObj, pRoom );
	    LastObj = pObj;
	    last = TRUE;
	    break;
	case 'P':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
		bug( "Reset_area: 'P': bad vnum %d.", pReset->arg1 );
		continue;
	    }
            if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
	    {
		bug( "Reset_area: 'P': bad vnum %d.", pReset->arg3 );
		continue;
	    }
//Viri: allow conainted objects to reset with players ni area            if (pRoom->area->nplayer > 0 || ( LastObj = get_obj_type( pObjToIndex ) ) == NULL
            if (( LastObj = get_obj_type( pObjToIndex, pRoom ) ) == NULL
		|| ( LastObj->in_room == NULL && !last)
		|| (count = count_obj_list(pObjIndex,LastObj->contains)) > pReset->arg4 )
	      {
		last = FALSE;
		break;
	      }
	    while (count < pReset->arg4)
	    {
	      if (!can_spawn(pObjIndex))
		break;
	      pObj = create_object( pObjIndex, number_fuzzy( LastObj->level ) );
	      pObj->homevnum = LastObj->homevnum;
	      obj_to_obj( pObj, LastObj );
	      count++;
	    }
            LastObj->value[1] = LastObj->pIndexData->value[1];
	    last = TRUE;
	    break;
        case 'T':
	  if ( (pTrapIndex = get_trap_index( pReset->arg1 )) == NULL ){
	    bug( "Reset_room: 'T' 1 : bad vnum %d", pReset->arg1 );
	    bugf ("%d %d %d %d",pReset->arg1,
		  pReset->arg2, pReset->arg3,pReset->arg4 );
	    continue;
	  }
	  if ( (pRoomIndex = get_room_index( pReset->arg3 )) == NULL ){
	    bug( "Reset_room: 'T' 2 : bad vnum %d.", pReset->arg3 );
	    bugf ("%d %d %d %d",pReset->arg1,
		  pReset->arg2, pReset->arg3,pReset->arg4 );
	    continue;
	  }
	  /* check of obj reset requierments */
	  if (pReset->arg2 == TRAP_ON_OBJ){
	    if ( !last || pObj == NULL){
	      continue;
	    }
	    else
	      pTrapTo = pObj->traps;
	  }
	  else if (pReset->arg2 == TRAP_ON_EXIT ){
	    if (pRoomIndex->exit[pReset->arg4] == NULL){
	      bug( "Reset_room 'T' 4 : no exit found for trap in room %d.",
		   pRoomIndex->vnum);
	      bugf ("%d %d %d %d",pReset->arg1,
		    pReset->arg2, pReset->arg3,pReset->arg4 );
	      continue;
	    }
	    else
	      pTrapTo = pRoomIndex->exit[pReset->arg4]->traps;
	  }
	  else{
	    bug( "Reset_room 'T' 5 : invalid trap in room %d.",
		 pRoomIndex->vnum);
	    bugf ("%d %d %d %d",pReset->arg1,
		  pReset->arg2, pReset->arg3,pReset->arg4 );
	    continue;
	  }
	  /* make sure we need to create a trap */
	  if (pTrapTo && pTrapTo->pIndexData == pTrapIndex){
	    init_trap( pTrapTo, pTrapTo->pIndexData );
	    continue;
	  }
	  else if (pTrapTo){
	    extract_trap( pRoomIndex->exit[pReset->arg4]->traps );
	  }
	  /* create the trap */
	  if ( (pTrap = create_trap( pTrapIndex, NULL)) == NULL){
	    bug( "Reset_room 'T' 5 : could not load trap in room %d.",
		 pRoomIndex->vnum);
	    bugf ("%d %d %d %d",pReset->arg1,
		  pReset->arg2, pReset->arg3,pReset->arg4 );
	    continue;
	  }
	  /* attach the trap */
	  if (pReset->arg2 == TRAP_ON_EXIT){
	    trap_to_exit( pTrap, pRoomIndex->exit[pReset->arg4] );
	  }
	  else if (pReset->arg2 == TRAP_ON_OBJ){
	    trap_to_obj( pTrap, pObj );
	  }
	  break;
	case 'G':
	case 'E':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
	    {
		bug( "Reset_area: 'E' or 'G': bad vnum %d.", pReset->arg1 );
		continue;
	    }
	    if ( !last )
		break;
            if ( !LastMob )
	    {
                bug( "Reset_area: 'E' or 'G': null mob for vnum %d.",pReset->arg1 );
		last = FALSE;
		break;
	    }
            if ( LastMob->pIndexData->pShop )
	    {
		int olevel = 0, i, j;
		if (!can_spawn(pObjIndex))
		  break;
                pObj = create_object( pObjIndex,UMIN( number_fuzzy( level ), LEVEL_HERO - 1 ) );
		if (!pObjIndex->new_format){
		  switch ( pObjIndex->item_type )
                    {
                    default: olevel = 0;
                    case ITEM_PILL:
		    case ITEM_RELIC:
		    case ITEM_SOCKET:
		    case ITEM_ARTIFACT:
                    case ITEM_POTION:
                    case ITEM_SCROLL:
                        olevel = 53;
                        for (i = 1; i < 5; i++)
                            if (pObjIndex->value[i] > 0)
			      for (j = 0; j < MAX_CLASS_NOW; j++)
				    olevel = UMIN(olevel, skill_table[pObjIndex->value[i]].skill_level[j]);
                       olevel = UMAX(0,(olevel * 3 / 4) - 2);
                       break;
                    case ITEM_WAND:         olevel = number_range( 10, 20 ); break;
                    case ITEM_STAFF:        olevel = number_range( 15, 25 ); break;
                    case ITEM_ARMOR:        olevel = number_range(  5, 15 ); break;
                    case ITEM_THROW:        olevel = number_range(  5, 15 ); break;
                    case ITEM_WEAPON:       olevel = number_range(  5, 15 ); break;
                    case ITEM_TREASURE:     olevel = number_range( 10, 20 ); break;
 		    case ITEM_RANGED:       olevel = number_range( 10, 20 ); break;
                    case ITEM_INSTRUMENT:   olevel = number_range(  5, 15 ); break;
                    case ITEM_KEY:          olevel = number_range(  1,  5 ); break;
		    case ITEM_PROJECTILE:   olevel = number_range(  5, 15 ); break;
                    }
		}
		SET_BIT( pObj->extra_flags, ITEM_INVENTORY );
            }
	    else
	    {
	      if (!can_spawn(pObjIndex))
		break;
	      pObj = create_object( pObjIndex,UMIN( number_fuzzy( level ), LEVEL_HERO - 1 ) );
	    }
	    pObj->homevnum = LastMob->homevnum;
	    /* hlh	    pObj->homevnum = LastMob->homevnum = -1; */
	    if (!can_take(LastMob, pObj)){
	      char buf[MIL];
	      sprintf(buf, "Bad reset: %s cannot hold vnum:[%d] (use noalign flag).",
		      PERS2(LastMob), pObjIndex->vnum);
	      bug(buf, pObjIndex->vnum);
	    }
            obj_to_ch( pObj, LastMob );
            if ( pReset->command == 'E' ){
	      REMOVE_BIT( pObj->extra_flags, ITEM_INVENTORY );
	      equip_char( LastMob, pObj, pReset->arg3 );
	    }
            last = TRUE;
            break;
        case 'D':
	  break;
	case 'R':
	  if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
	    {
	      bug( "Reset_area: 'R': bad vnum %d.", pReset->arg1 );
	      continue;
	    }
	    {
                EXIT_DATA *pExit;
		int d0, d1;
		for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
		{
                    d1                   = number_range( d0, pReset->arg2-1 );
                    pExit                = pRoomIndex->exit[d0];
                    pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
                    pRoomIndex->exit[d1] = pExit;
		}
	    }
	    break;
	}
    }
}

/* OLC - Reset one area. */
void reset_area( AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoom;
    int  vnum;
    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ ){
      if ( ( pRoom = get_room_index(vnum) ) != NULL && !IS_SET(pRoom->room_flags2, ROOM_NO_RESET))
	reset_room(pRoom);
    }
    /* reset virtual rooms that belong to this area */
    for ( vnum = MAX_ROOM_VNUM; vnum < MAX_ROOM_VNUM + top_vir_room; vnum++ ){
      if ( ( pRoom = get_room_index(vnum) ) != NULL && pRoom->area == pArea
	   && !IS_SET(pRoom->room_flags2, ROOM_NO_RESET))
      	reset_room(pRoom);
    }
    return;
}

CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
    CHAR_DATA *mob;
    int i;
    AFFECT_DATA af;
    mobile_count++;
    if ( pMobIndex == NULL )
    {
	bug( "Create_mobile: NULL pMobIndex.", 0 );
	exit( 1 );
    }
    mob = new_char();
    mob->pIndexData	= pMobIndex;
    mob->name           = str_dup( pMobIndex->player_name );
    mob->short_descr    = str_dup( pMobIndex->short_descr );
    mob->long_descr     = str_dup( pMobIndex->long_descr );
    mob->description    = str_dup( pMobIndex->description );
    mob->id		= get_mob_id();
    mob->spec_fun	= pMobIndex->spec_fun;
    mob->homevnum	= 0;
    mob->prompt		= NULL;
    if (pMobIndex->gold == 0)
	mob->gold   = 0;
    else
        mob->gold       = number_range(pMobIndex->gold/2, pMobIndex->gold * 3/2);
    if (pMobIndex->new_format)
    {
 	mob->group		= pMobIndex->group;
	if (pMobIndex->pCabal)
	  mob->pCabal		= get_cabal_vnum(pMobIndex->pCabal->vnum);
	mob->act 		= pMobIndex->act;
	mob->act2 		= pMobIndex->act2;
        mob->comm               = COMM_NOCHANNELS|COMM_NOYELL|COMM_NOTELL;
	mob->affected_by	= pMobIndex->affected_by;
	mob->affected2_by	= pMobIndex->affected2_by;
	mob->alignment		= pMobIndex->alignment;
	mob->level		= pMobIndex->level;
	mob->hitroll		= pMobIndex->hitroll;
	mob->damroll		= pMobIndex->damage[DICE_BONUS];
        mob->max_hit            = dice(pMobIndex->hit[DICE_NUMBER],pMobIndex->hit[DICE_TYPE])+pMobIndex->hit[DICE_BONUS];
        mob->hit                = mob->max_hit;
        mob->max_mana           = dice(pMobIndex->mana[DICE_NUMBER],pMobIndex->mana[DICE_TYPE])+pMobIndex->mana[DICE_BONUS];
	mob->mana		= mob->max_mana;
	mob->damage[DICE_NUMBER]= pMobIndex->damage[DICE_NUMBER];
	mob->damage[DICE_TYPE]	= pMobIndex->damage[DICE_TYPE];
	mob->dam_type		= pMobIndex->dam_type;
        if (mob->dam_type == 0)
    	    switch(number_range(1,3))
            {
            case (1): mob->dam_type = 3;        break;
            case (2): mob->dam_type = 7;        break;
            case (3): mob->dam_type = 11;       break;
            }
	for (i = 0; i < 4; i++)
            mob->armor[i]       = pMobIndex->ac[i];
	mob->off_flags		= pMobIndex->off_flags;
	mob->imm_flags		= pMobIndex->imm_flags;
	mob->res_flags		= pMobIndex->res_flags;
	mob->vuln_flags		= pMobIndex->vuln_flags;
	mob->start_pos		= pMobIndex->start_pos;
	mob->default_pos	= pMobIndex->default_pos;
	mob->sex		= pMobIndex->sex;
        if (mob->sex == 3)
            mob->sex = number_range(1,2);
	mob->race		= pMobIndex->race;
	mob->form		= pMobIndex->form;
	mob->parts		= pMobIndex->parts;
	mob->size		= pMobIndex->size;
	mob->extracted		= FALSE;
    	for (i = 0; i < MAX_STATS-1; i ++)
            mob->perm_stat[i] = UMIN(25,11 + mob->level/4);
        mob->perm_stat[STAT_LUCK] = 16;
        if (IS_SET(mob->act,ACT_WARRIOR))
        {
            mob->perm_stat[STAT_STR] += 3;
            mob->perm_stat[STAT_INT] -= 1;
            mob->perm_stat[STAT_CON] += 2;
        }
        if (IS_SET(mob->act,ACT_THIEF))
        {
            mob->perm_stat[STAT_DEX] += 3;
            mob->perm_stat[STAT_INT] += 1;
            mob->perm_stat[STAT_WIS] -= 1;
        }
        if (IS_SET(mob->act,ACT_CLERIC))
        {
            mob->perm_stat[STAT_WIS] += 3;
            mob->perm_stat[STAT_DEX] -= 1;
            mob->perm_stat[STAT_STR] += 1;
        }
        if (IS_SET(mob->act,ACT_MAGE))
        {
            mob->perm_stat[STAT_INT] += 3;
            mob->perm_stat[STAT_STR] -= 1;
            mob->perm_stat[STAT_DEX] += 1;
        }
        if (IS_SET(mob->off_flags,OFF_FAST))
            mob->perm_stat[STAT_DEX] += 2;
        mob->perm_stat[STAT_STR] += mob->size - SIZE_MEDIUM;
        mob->perm_stat[STAT_CON] += (mob->size - SIZE_MEDIUM) / 2;
	if (IS_AFFECTED(mob,AFF_SANCTUARY))
	{
	    af.where	 = TO_AFFECTS;
	    af.type      = skill_lookup("sanctuary");
	    af.level     = mob->level;
	    af.duration  = -1;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_SANCTUARY;
	    affect_to_char( mob, &af );
	}
	if (IS_AFFECTED(mob,AFF_HASTE))
	{
	    af.where	 = TO_AFFECTS;
	    af.type      = skill_lookup("haste");
    	    af.level     = mob->level;
      	    af.duration  = -1;
    	    af.location  = APPLY_DEX;
            af.modifier  = 1 + (mob->level >= 18) + (mob->level >= 25) + (mob->level >= 32);
    	    af.bitvector = AFF_HASTE;
    	    affect_to_char( mob, &af );
	}
	if (IS_AFFECTED(mob,AFF_PROTECT_EVIL))
	{
	    af.where	 = TO_AFFECTS;
	    af.type	 = skill_lookup("protection evil");
	    af.level	 = mob->level;
	    af.duration	 = -1;
	    af.location	 = APPLY_SAVING_SPELL;
	    af.modifier	 = -1;
	    af.bitvector = AFF_PROTECT_EVIL;
	    affect_to_char(mob,&af);
	}
        if (IS_AFFECTED(mob,AFF_PROTECT_GOOD))
        {
	    af.where	 = TO_AFFECTS;
            af.type      = skill_lookup("protection good");
            af.level     = mob->level;
            af.duration  = -1;
            af.location  = APPLY_SAVING_SPELL;
            af.modifier  = -1;
            af.bitvector = AFF_PROTECT_GOOD;
            affect_to_char(mob,&af);
        }
    }
    else
    {
	mob->act		= pMobIndex->act;
	mob->act2		= pMobIndex->act2;
	mob->affected_by	= pMobIndex->affected_by;
	mob->alignment		= pMobIndex->alignment;
	mob->level		= pMobIndex->level;
	mob->hitroll		= pMobIndex->hitroll;
	mob->damroll		= 0;
        mob->max_hit            = mob->level * 8 + number_range(mob->level * mob->level/4,mob->level * mob->level);
	mob->max_hit *= .9;
	mob->hit		= mob->max_hit;
	mob->max_mana		= 100 + dice(mob->level,10);
	mob->mana		= mob->max_mana;
	switch(number_range(1,3))
	{
            case (1): mob->dam_type = 3;        break;
            case (2): mob->dam_type = 7;        break;
            case (3): mob->dam_type = 11;       break;
	}
	for (i = 0; i < 3; i++)
	    mob->armor[i]	= interpolate(mob->level,100,-100);
	mob->armor[3]		= interpolate(mob->level,100,0);
	mob->race		= pMobIndex->race;
	mob->off_flags		= pMobIndex->off_flags;
	mob->imm_flags		= pMobIndex->imm_flags;
	mob->res_flags		= pMobIndex->res_flags;
	mob->vuln_flags		= pMobIndex->vuln_flags;
	mob->start_pos		= pMobIndex->start_pos;
	mob->default_pos	= pMobIndex->default_pos;
	mob->sex		= pMobIndex->sex;
	mob->form		= pMobIndex->form;
	mob->parts		= pMobIndex->parts;
	mob->size		= SIZE_MEDIUM;
	mob->extracted		= FALSE;
        for (i = 0; i < MAX_STATS-1; i ++)
            mob->perm_stat[i] = 11 + mob->level/4;
        mob->perm_stat[STAT_LUCK] = 16;
    }
    mob->position = mob->start_pos;
    mob->next		= char_list;
    char_list		= mob;
    pMobIndex->count++;
    return mob;
}

/* refreshes list of resets with a new room number */
void fix_resets( RESET_DATA* pReset, int room_vnum ){
  for (; pReset; pReset = pReset->next ){
    switch ( pReset->command ){
    case 'M':
    case 'O':
    case 'T':
      pReset->arg3 = room_vnum;		break;

    case 'R':
    case 'D':
      pReset->arg1 = room_vnum;		break;
    }
  }
}

void clone_room( ROOM_INDEX_DATA* src, ROOM_INDEX_DATA* dest, bool fFull ){
  EXTRA_DESCR_DATA *pExtra, *ed;
  RESET_DATA *pReset, *pr, *pr_last = NULL;
  Double_List *watch, *tmp_list;

/* cabal */
  dest->pCabal		=	src->pCabal;

/* name etc. */
  free_string( dest->name );
  free_string( dest->description );
  free_string( dest->description2 );

  dest->name		=	str_dup( src->name );
  dest->description	=	str_dup( src->description  );
  dest->description2	=	str_dup( src->description2 );

/* numeric data */
  dest->sector_type	=	src->sector_type;
  dest->room_flags	=	src->room_flags;
  dest->room_flags2	=	src->room_flags2;
  dest->heal_rate	=	src->heal_rate;
  dest->mana_rate	=	src->mana_rate;
  dest->temp		=	src->temp;


/* extra desc. data */
  while ( (pExtra = dest->extra_descr) != NULL){
    dest->extra_descr = pExtra->next;
    free_extra_descr( pExtra );
  }

  for ( pExtra = src->extra_descr; pExtra; pExtra = pExtra->next ){
    ed			=	new_extra_descr();
    ed->keyword		=	str_dup( pExtra->keyword );
    ed->description	=	str_dup( pExtra->description );
    ed->next		=	dest->extra_descr;
    dest->extra_descr	=	ed;
  }

/* rest of data is only copied on full copy */
  if (!fFull)
    return;

/* resets, we have to copy them in such way as to preserve their current order */
  while ( (pReset = dest->reset_first)){
    dest->reset_first = pReset->next;
    free_reset_data( pReset );
  }

  for ( pReset = src->reset_first; pReset; pReset = pReset->next ){
    pr			=	new_reset_data();
    pr->command		=	pReset->command;
    pr->arg1		=	pReset->arg1;
    pr->arg2		=	pReset->arg2;
    pr->arg3		=	pReset->arg3;
    pr->arg4		=	pReset->arg4;

    if (dest->reset_first == NULL){
      dest->reset_first = pr;
      pr->next = NULL;
      pr_last = pr;
    }
    else{
      pr_last->next = pr;
      pr->next = NULL;
      pr_last = pr;
    }
  }
  /* we now refresh the resets with proper room number */
  fix_resets( dest->reset_first, dest->vnum );

/* watch vnums */
  while ( dest->watch_vnums ){
    Double_List * tmp_list = dest->watch_vnums;
    dest->watch_vnums = tmp_list->next_node;
    free (tmp_list);
  }

  for (watch = src->watch_vnums; watch; watch = watch->next_node){
    tmp_list = (Double_List *) malloc (sizeof (Double_List));

    tmp_list->cur_entry =	watch->cur_entry;

    if (dest->watch_vnums == NULL){
      tmp_list->next_node	=	NULL;
      tmp_list->prev_node	=	NULL;
      dest->watch_vnums		=	tmp_list;
    }
    else{
      tmp_list->next_node	=	dest->watch_vnums;
      tmp_list->prev_node	=	NULL;
      dest->watch_vnums->prev_node =	tmp_list;
      dest->watch_vnums		=	tmp_list;
    }
  }
}


void clone_mobile(CHAR_DATA *parent, CHAR_DATA *clone)
{
    int i;
    AFFECT_DATA *paf, *paf_next;
    if ( parent == NULL || clone == NULL || !IS_NPC(parent))
	return;
    for (paf = clone->affected; paf != NULL; paf = paf_next)
    {
        paf_next = paf->next;
        affect_remove(clone, paf);
    }
    clone->homevnum	= parent->homevnum;
    clone->name 	= str_dup(parent->name);
    clone->short_descr	= str_dup(parent->short_descr);
    clone->long_descr	= str_dup(parent->long_descr);
    clone->description	= str_dup(parent->description);
    clone->group	= parent->group;
    clone->sex		= parent->sex;
    clone->class	= parent->class;
    clone->race		= parent->race;
    clone->level	= parent->level;
    clone->trust	= 0;
    clone->timer	= parent->timer;
    clone->wait		= parent->wait;
    clone->hit		= parent->hit;
    clone->max_hit	= parent->max_hit;
    clone->mana		= parent->mana;
    clone->max_mana	= parent->max_mana;
    clone->move		= parent->move;
    clone->max_move	= parent->max_move;
    clone->gold		= parent->gold;
    clone->exp		= parent->exp;
    clone->act		= parent->act;
    clone->act2		= parent->act2;
    clone->comm		= parent->comm;
    clone->imm_flags	= parent->imm_flags;
    clone->res_flags	= parent->res_flags;
    clone->vuln_flags	= parent->vuln_flags;
    clone->invis_level	= parent->invis_level;
    clone->affected_by	= parent->affected_by;
    clone->affected2_by	= parent->affected2_by;
    clone->position	= parent->position;
    clone->practice	= parent->practice;
    clone->train	= parent->train;
    clone->savingaffl	= parent->savingaffl;
    clone->savingmaled	= parent->savingmaled;
    clone->savingmental	= parent->savingmental;
    clone->savingbreath	= parent->savingbreath;
    clone->savingspell	= parent->savingspell;
    clone->alignment	= parent->alignment;
    clone->hitroll	= parent->hitroll;
    clone->damroll	= parent->damroll;
    clone->wimpy	= parent->wimpy;
    clone->form		= parent->form;
    clone->parts	= parent->parts;
    clone->size		= parent->size;
    clone->extracted	= parent->extracted;
    clone->off_flags	= parent->off_flags;
    clone->dam_type	= parent->dam_type;
    clone->start_pos	= parent->start_pos;
    clone->default_pos	= parent->default_pos;
    clone->spec_fun	= parent->spec_fun;
    for (i = 0; i < 4; i++)
    	clone->armor[i]	= parent->armor[i];
    for (i = 0; i < MAX_STATS; i++)
    {
	clone->perm_stat[i]	= parent->perm_stat[i];
	clone->mod_stat[i]	= parent->mod_stat[i];
    }
    for (i = 0; i < 3; i++)
	clone->damage[i]	= parent->damage[i];
    for (paf = parent->affected; paf != NULL; paf = paf_next)
    {
        paf_next = paf->next;
        affect_to_char(clone, paf);
    }
}


/* creates a trap, attatches to global list, and zeros the values */
TRAP_DATA* create_trap(TRAP_INDEX_DATA* pTrapIndex, CHAR_DATA* owner){
  TRAP_DATA* tr = new_trap();

  tr->pIndexData	= pTrapIndex;
  init_trap( tr, pTrapIndex);

  if (owner != NULL)
    trap_to_char( tr, owner );
  else
    tr->owner = owner;
  tr->on_obj = NULL;
  tr->on_exit = NULL;

/* attatch to global list */
  if (trap_list == NULL)
    trap_list = tr;
  else{
    tr->next = trap_list;
    trap_list = tr;
  }
  return tr;
}

OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, int level)
{
    AFFECT_DATA *paf;
    OBJ_DATA *obj;
    if ( pObjIndex == NULL )
    {
	bug( "Create_object: NULL pObjIndex.", 0 );
	exit( 1 );
    }
    obj = new_obj();
    obj->pIndexData	= pObjIndex;
    obj->in_room	= NULL;
    obj->next_bidobj	= NULL;
    obj->enchanted	= FALSE;
    obj->eldritched	= FALSE;
    if (pObjIndex->new_format)
	obj->level = pObjIndex->level;
    else
	obj->level		= UMAX(0,level);
    if (pObjIndex->pCabal)
      obj->pCabal		= get_cabal_vnum(pObjIndex->pCabal->vnum);
    obj->race		= pObjIndex->race;
    obj->class		= pObjIndex->class;
    obj->vnum		= pObjIndex->vnum;
    obj->wear_loc	= -1;
    obj->name           = str_dup( pObjIndex->name );
    obj->short_descr    = str_dup( pObjIndex->short_descr );
    obj->description    = str_dup( pObjIndex->description );
    obj->material       = str_dup(pObjIndex->material);
    obj->item_type	= pObjIndex->item_type;
    obj->extra_flags	= pObjIndex->extra_flags;
    obj->wear_flags	= pObjIndex->wear_flags;
    obj->value[0]	= pObjIndex->value[0];
    obj->value[1]	= pObjIndex->value[1];
    obj->value[2]	= pObjIndex->value[2];
    obj->value[3]	= pObjIndex->value[3];
    obj->value[4]	= pObjIndex->value[4];
    obj->weight		= pObjIndex->weight;
    obj->cost           = pObjIndex->cost;
    obj->condition       = pObjIndex->condition;
    obj->homevnum	= 0;
    obj->idle		= 0;
    obj->extracted	= FALSE;
    obj->owner          = 0;
    obj->traps		= NULL;
    switch ( obj->item_type )
    {
    default:
	bug( "Read_object: vnum %d bad type.", pObjIndex->vnum );
	break;
    case ITEM_CABAL:
        break;
    case ITEM_LIGHT:
	if (obj->value[2] == 999)
            obj->value[2] = -1;
	break;
    case ITEM_FURNITURE:
    case ITEM_TRASH:
    case ITEM_CONTAINER:
    case ITEM_DRINK_CON:
    case ITEM_KEY:
    case ITEM_BOAT:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
    case ITEM_FOUNTAIN:
    case ITEM_MAP:
    case ITEM_CLOTHING:
    case ITEM_TREASURE:
    case ITEM_INSTRUMENT:
    case ITEM_GEM:
    case ITEM_RANGED:
    case ITEM_PROJECTILE:
    case ITEM_JEWELRY:
	break;
    case ITEM_FOOD:
	if (obj->value[4] == 0)
            obj->value[4] = 168;
	obj->timer = obj->value[4];
	break;
    case ITEM_HERB:
      obj->timer = number_range( 500, 1000);
    case ITEM_RELIC:
    case ITEM_SOCKET:
    case ITEM_ARTIFACT:
    case ITEM_SCROLL:
	if (level != -1 && !pObjIndex->new_format)
            obj->value[0] = number_fuzzy( obj->value[0] );
	break;
    case ITEM_WAND:
    case ITEM_STAFF:
	if (level != -1 && !pObjIndex->new_format)
	{
	    obj->value[0]	= number_fuzzy( obj->value[0] );
	    obj->value[1]	= number_fuzzy( obj->value[1] );
	    obj->value[2]	= obj->value[1];
	}
	break;
    case ITEM_THROW:
    case ITEM_WEAPON:
	if (level != -1 && !pObjIndex->new_format)
	{
	    obj->value[1] = number_fuzzy( number_fuzzy( 1 * level / 4 + 2 ) );
	    obj->value[2] = number_fuzzy( number_fuzzy( 3 * level / 4 + 6 ) );
	}
	break;
    case ITEM_ARMOR:
	if (level != -1 && !pObjIndex->new_format)
	{
	    obj->value[0]	= number_fuzzy( level / 5 + 3 );
	    obj->value[1]	= number_fuzzy( level / 5 + 3 );
	    obj->value[2]	= number_fuzzy( level / 5 + 3 );
	}
	break;
    case ITEM_POTION:
    case ITEM_PILL:
	if (level != -1 && !pObjIndex->new_format)
	    obj->value[0] = number_fuzzy( number_fuzzy( obj->value[0] ) );
	break;
    case ITEM_MONEY:
	if (!pObjIndex->new_format)
	    obj->value[0]	= obj->cost;
	break;
    }
    for (paf = pObjIndex->affected; paf != NULL; paf = paf->next)
	if ( paf->location == APPLY_SPELL_AFFECT )
	    affect_to_obj(obj,paf);
    obj->next		= object_list;
    object_list		= obj;
    pObjIndex->count++;
    return obj;
}

void clone_object(OBJ_DATA *parent, OBJ_DATA *clone)
{
    int i;
    AFFECT_DATA *paf;
    EXTRA_DESCR_DATA *ed,*ed_new;
    if (parent == NULL || clone == NULL)
	return;
    clone->name 	= str_dup(parent->name);
    clone->short_descr 	= str_dup(parent->short_descr);
    clone->description	= str_dup(parent->description);
    clone->item_type	= parent->item_type;
    clone->extra_flags	= parent->extra_flags;
    clone->wear_flags	= parent->wear_flags;
    clone->weight	= parent->weight;
    clone->cost		= parent->cost;
    clone->pCabal	= parent->pCabal;
    clone->race		= parent->race;
    clone->class	= parent->class;
    clone->vnum		= parent->vnum;
    clone->level	= parent->level;
    clone->condition	= parent->condition;
    if (!IS_NULLSTR(clone->material))
      free_string(clone->material);
    clone->material	= str_dup(parent->material);
    clone->timer	= parent->timer;
    clone->extracted	= parent->extracted;
    for (i = 0;  i < 5; i ++)
	clone->value[i]	= parent->value[i];
    clone->enchanted	= parent->enchanted;
    for (paf = parent->affected; paf != NULL; paf = paf->next)
	affect_to_obj(clone,paf);
    for (ed = parent->extra_descr; ed != NULL; ed = ed->next)
    {
        ed_new                  = new_extra_descr();
        ed_new->keyword    	= str_dup( ed->keyword);
        ed_new->description     = str_dup( ed->description );
        ed_new->next           	= clone->extra_descr;
        clone->extra_descr  	= ed_new;
    }
}

void clear_char( CHAR_DATA *ch )
{
    static CHAR_DATA ch_zero;
    int i;
    *ch				= ch_zero;
    ch->name			= &str_empty[0];
    ch->short_descr		= &str_empty[0];
    ch->long_descr		= &str_empty[0];
    ch->description		= &str_empty[0];
    ch->prompt                  = &str_empty[0];
    ch->logon			= mud_data.current_time;
    ch->lines			= PAGELEN;
    for (i = 0; i < 4; i++)
    	ch->armor[i]		= 100;
    ch->position		= POS_STANDING;
    if (!IS_NPC(ch))
    {
	ch->hit			= pc_race_table[ch->race].hit;
	ch->mana		= pc_race_table[ch->race].mana;
    }
    else
    {
        ch->hit			= 20;
        ch->mana		= 100;
    }
    ch->max_hit			= ch->hit;
    ch->max_mana		= ch->mana;
    ch->move			= 100;
    ch->max_move		= ch->move;
    for (i = 0; i < MAX_STATS; i ++)
    {
	ch->perm_stat[i] = 13;
	ch->mod_stat[i] = 0;
    }
    ch->perm_stat[STAT_LUCK] = 16;
}

char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
    for ( ; ed != NULL; ed = ed->next )
	if ( is_name( (char *) name, ed->keyword ) )
	    return ed->description;
    return NULL;
}

char *get_auto_extra_descr( const char *name, EXTRA_DESCR_DATA *ed, bool fAuto)
{
    for ( ; ed != NULL; ed = ed->next )
      if ( (fAuto && is_auto_name( (char *) name, ed->keyword ) )
	   || is_name( (char *) name, ed->keyword ) )
	return ed->description;
    return NULL;
}

/* Translates mob virtual number to its mob index struct. *
 * Hash table lookup.                                     */
MOB_INDEX_DATA *get_mob_index( int vnum )
{
    MOB_INDEX_DATA *pMobIndex;
    for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH]; pMobIndex != NULL; pMobIndex  = pMobIndex->next )
	if ( pMobIndex->vnum == vnum )
	    return pMobIndex;
    if ( fBootDb )
    {
	bug( "Get_mob_index: bad vnum %d.", vnum );
	exit( 1 );
    }
    return NULL;
}

/* Translates mob virtual number to its obj index struct. *
 * Hash table lookup.                                     */
OBJ_INDEX_DATA *get_obj_index( int vnum )
{
    OBJ_INDEX_DATA *pObjIndex;
    for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH]; pObjIndex != NULL; pObjIndex  = pObjIndex->next )
	if ( pObjIndex->vnum == vnum )
	    return pObjIndex;
    if ( fBootDb )
    {
	bug( "Get_obj_index: bad vnum %d.", vnum );
	exit( 1 );
    }
    return NULL;
}

/* Translates mob virtual number to its room index struct. *
 * Hash table lookup.                                      */
ROOM_INDEX_DATA *get_room_index( int vnum )
{
  ROOM_INDEX_DATA *pRoomIndex;
  if (vnum <= 0)
    return NULL;
  for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH]; pRoomIndex != NULL; pRoomIndex  = pRoomIndex->next )
	if ( pRoomIndex->vnum == vnum )
	    return pRoomIndex;
    if ( fBootDb )
    {
	bug( "Get_room_index: bad vnum %d.", vnum );
	exit( 1 );
    }
    return NULL;
}

/* Read a letter from a file. */
char fread_letter( FILE *fp )
{
    char c = '\0';
    do
	c = getc( fp );
    while ( isspace(c) );
    return c;
}

/* Read a number from a file. */
int fread_number( FILE *fp )
{
    int number = 0;
    bool sign;
    char c;
    if (!fp){
      bug("fread_number: null file passed", 0);
      return 0;
    }
    do
	c = getc( fp );
    while ( isspace(c) );
    number = 0;
    sign   = FALSE;
    if ( c == '+' )
	c = getc( fp );
    else if ( c == '-' )
    {
	sign = TRUE;
	c = getc( fp );
    }
    if ( !isdigit(c) )
    {
	bug( "Fread_number: bad format.", 0 );
//	exit( 1 );
    }
    while ( isdigit(c) )
    {
	number = number * 10 + c - '0';
	c      = getc( fp );
    }
    if ( sign )
	number = 0 - number;
    if ( c == '|' )
	number += fread_number( fp );
    else if ( c != ' ' )
	ungetc( c, fp );
    return number;
}

unsigned long fread_unsigned_long( FILE *fp )
{
    unsigned long int number = 0;
    bool sign;
    char c;
    if (!fp){
      bug("fread_number: null file passed", 0);
      return 0;
    }
    do
        c = getc( fp );
    while ( isspace(c) );
    number = 0;
    sign   = FALSE;
    if ( c == '+' )
        c = getc( fp );
    else if ( c == '-' )
    {
        sign = TRUE;
        c = getc( fp );
    }
    if ( !isdigit(c) )
    {
        bug( "Fread_number: bad format.", 0 );
//      exit( 1 );
    }
    while ( isdigit(c) )
    {
        number = number * 10 + c - '0';
        c      = getc( fp );
    }
    if ( sign )
        number = 0 - number;
    if ( c == '|' )
        number += fread_number( fp );
    else if ( c != ' ' )
        ungetc( c, fp );
    return number;
}

/* read a regular long */
long fread_long( FILE *fp )
{
    long number = 0;
    bool sign;
    char c;
    if (!fp){
      bug("fread_number: null file passed", 0);
      return 0;
    }
    do
	c = getc( fp );
    while ( isspace(c) );
    number = 0;
    sign   = FALSE;
    if ( c == '+' )
	c = getc( fp );
    else if ( c == '-' )
    {
	sign = TRUE;
	c = getc( fp );
    }
    if ( !isdigit(c) )
    {
	bug( "Fread_number: bad format.", 0 );
//	exit( 1 );
    }
    while ( isdigit(c) )
    {
	number = number * 10 + c - '0';
	c      = getc( fp );
    }
    if ( sign )
	number = 0 - number;
    if ( c == '|' )
	number += fread_number( fp );
    else if ( c != ' ' )
	ungetc( c, fp );
    return number;
}

long fread_flag( FILE *fp)
{
    int number;
    char c;
    bool negative = FALSE;
    do
	c = getc(fp);
    while ( isspace(c));
    if (c == '-')
    {
	negative = TRUE;
	c = getc(fp);
    }
    number = 0;
    if (!isdigit(c))
	while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'))
	{
	    number += flag_convert(c);
	    c = getc(fp);
	}
    while (isdigit(c))
    {
	number = number * 10 + c - '0';
	c = getc(fp);
    }
    if (c == '|')
	number += fread_flag(fp);
    else if  ( c != ' ')
	ungetc(c,fp);
    if (negative)
	return -1 * number;
    return number;
}

long flag_convert(char letter )
{
    long bitsum = 0;
    char i;
    if ('A' <= letter && letter <= 'Z')
    {
	bitsum = 1;
	for (i = letter; i > 'A'; i--)
	    bitsum *= 2;
    }
    else if ('a' <= letter && letter <= 'z')
    {
        bitsum = 67108864;
	for (i = letter; i > 'a'; i --)
	    bitsum *= 2;
    }
    return bitsum;
}

/* Read to end of line (for comments). */
void fread_to_eol( FILE *fp )
{
    char c;
    do
	c = getc( fp );
    while ( c != '\n' && c != '\r' );
    do
	c = getc( fp );
    while ( c == '\n' || c == '\r' );
    ungetc( c, fp );
    return;
}

/* Read one word (into static buffer). */
char *fread_word_noquote( FILE *fp )
{
    static char word[MIL];
    char *pword;
    char cEnd;
    do
	cEnd = getc( fp );
    while ( isspace( cEnd ) && (cEnd != '\r'));
    if ( cEnd == '\'' || cEnd == '"' )
	pword   = word;
    else
    {
	word[0] = cEnd;
	pword   = word+1;
	cEnd    = ' ';
    }
    for ( ; pword < word + MIL; pword++ )
    {
	*pword = getc( fp );
	if ( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
	{
	    if ( cEnd == ' ' )
		ungetc( *pword, fp );
	    *pword = '\0';
	    return word;
	}
    }
    bug( "Fread_word: word too long", 0);
//    exit( 1 );
    return NULL;
}

/* Read one word (into static buffer). */
char *fread_word( FILE *fp )
{
    static char word[MIL];
    char *pword;
    char cEnd;
    do
	cEnd = getc( fp );
    while ( isspace( cEnd ) );
    if ( cEnd == '\'' || cEnd == '"' )
	pword   = word;
    else
    {
	word[0] = cEnd;
	pword   = word+1;
	cEnd    = ' ';
    }
    for ( ; pword < word + MIL; pword++ )
    {
	*pword = getc( fp );
	if ( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
	{
	    if ( cEnd == ' ' )
		ungetc( *pword, fp );
	    *pword = '\0';
	    return word;
	}
    }
    bug( "Fread_word: word too long", 0);
    return NULL;
}

/* Allocate some ordinary memory,              *
 * with the expectation of freeing it someday. */
void *alloc_mem( int sMem )
{
    void *pMem;
    int *magic;
    int iList;
    sMem += sizeof(*magic);
    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
        if ( sMem <= rgSizeList[iList] )
            break;
    if ( iList == MAX_MEM_LIST )
    {
        bug( "Alloc_mem: size %d too large.", sMem );
        exit( 1 );
    }
    if ( rgFreeList[iList] == NULL )
        pMem              = alloc_perm( rgSizeList[iList] );
    else
    {
        pMem              = rgFreeList[iList];
        rgFreeList[iList] = * ((void **) rgFreeList[iList]);
    }
    magic = (int *) pMem;
    *magic = MAGIC_NUM;
    pMem += sizeof(*magic);
    return pMem;
}

/* Free some memory.                                           *
 * Recycle it back onto the free list for blocks of that size. */
void free_mem( void *pMem, int sMem )
{
    int iList;
    int *magic;
    pMem -= sizeof(*magic);
    magic = (int *) pMem;
    if (*magic != MAGIC_NUM)
    {
        bug("Attempt to recyle invalid memory of size %d.",sMem);
        bug((char*) pMem + sizeof(*magic),0);
        return;
    }
    *magic = 0;
    sMem += sizeof(*magic);
    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
        if ( sMem <= rgSizeList[iList] )
            break;
    if ( iList == MAX_MEM_LIST )
    {
        bug( "Free_mem: size %d too large.", sMem );
        exit( 1 );
    }
    * ((void **) pMem) = rgFreeList[iList];
    rgFreeList[iList]  = pMem;
}


/* Allocate some permanent memory.          *
 * Permanent memory is never freed,         *
 *   pointers into it may be copied safely. */
void *alloc_perm( int sMem )
{
    static char *pMemPerm;
    static int iMemPerm;
    void *pMem = NULL;

    while ( sMem % sizeof(long) != 0 ) {
	sMem++;
    }
    if ( sMem > MAX_PERM_BLOCK ) {
      bug( "Alloc_perm: %d too large.", sMem );
      exit( 1 );
    }
    if ( pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK ) {
      iMemPerm = 0;
      init_malloc("calloc alloc_perm");
      if ( ( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) == NULL ) {
	perror( "Alloc_perm" );
	exit( 1 );
      }
      end_malloc("calloc alloc_perm");
    }
    pMem        = pMemPerm + iMemPerm;
    iMemPerm   += sMem;
    nAllocPerm += 1;
    sAllocPerm += sMem;
    return pMem;
}

void do_areas( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea1 = area_first, *pArea2 = area_first;
    int iArea, iAreaHalf = (top_area + 1) / 2;
    BUFFER *buffer;
    char buf[MSL];
    if (argument[0] != '\0')
    {
	send_to_char("No argument is used with this command.\n\r",ch);
	return;
    }
    buffer = new_buf();
    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	pArea2 = pArea2->next;
    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
    {
        sprintf( buf, "%-39s%-39s\n\r", pArea1->credits, (pArea2 != NULL) ? pArea2->credits : "" );
	add_buf(buffer,buf);
	pArea1 = pArea1->next;
	if ( pArea2 != NULL )
	    pArea2 = pArea2->next;
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}
extern int numFree;
void do_memory( CHAR_DATA *ch, char *argument )
{
    sendf( ch, "Affects:           %5d\n\r", top_affect    );
    sendf( ch, "Areas:             %5d (%d bastions)\n\r", top_area, mud_data.tot_bastion  );
    sendf( ch, "ExDes:             %5d\n\r", top_ed        );
    sendf( ch, "Exits:             %5d\n\r", top_exit      );
    sendf( ch, "Helps:             %5d\n\r", top_help      );
    sendf( ch, "Socials:           %5d\n\r", social_count  );
    sendf( ch, "Mobs:              %5d       %7d new format       %7d in use\n\r", top_mob_index,newmobs,mobile_count);
    sendf( ch, "Mob progs:         %5d       %7d files\n\r",mprog_count,mprogf_count  );
    sendf( ch, "Objs:              %5d       %7d new format\n\r", top_obj_index,newobjs );
    sendf( ch, "Traps:             %5d\n\r", top_trap_index);
    sendf( ch, "Cabals:            %5d\n\r", top_cabal_index);
    sendf( ch, "Resets:            %5d\n\r", top_reset     );
    sendf( ch, "Rooms:             %5d\n\r", top_room      );
    sendf( ch, "VirRooms:          %5d\n\r", top_vir_room  );
    sendf( ch, "Night Descrips:    %5d\n\r", night_count   );
    sendf( ch, "Shops:             %5d\n\r", top_shop      );
    sendf( ch, "Share Strings:     %5d strings of %7d bytes       %7d bytes is reboot\n\r",
	nAllocString, sAllocString, 4*MAX_STRING/5 );
    sendf( ch, "Overflow Strings:  %5d strings of %7d bytes\n\r",nOverFlowString, sOverFlowString );
    sendf( ch, "Perm Strings:      %5d blocks  of %7d bytes       %7d blocks is reboot\n\r",
	nAllocPerm, sAllocPerm, MAX_PERM_BLOCK * 4 / 5 );
sendf(ch, "Reboot counter: %d, FreeString %d/%d", reboot_tick_counter, numFree, MAX_FREE);
    if( Full )
        sendf( ch, "Shared String Heap is full, increase MAX_STRING.\n\r" );
}

void do_dump( CHAR_DATA *ch, char *argument )
{
    int count = 0, count2 = 0, num_pcs = 0, aff_count = 0, vnum, nMatch = 0;
    CHAR_DATA *fch;
    MOB_INDEX_DATA *pMobIndex;
    PC_DATA *pc;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *room;
    EXIT_DATA *exit;
    DESCRIPTOR_DATA *d;
    AFFECT_DATA *af;
    FILE *fp;
    if (argument[0] == '\0')
      {
	send_to_char("memory dump: enter anything as an argument to dump the memory.\n\r", ch);
	return;
      }
    fclose(fpReserve);
    fp = fopen("mem.dmp","w");
    fprintf(fp,"MobProt %4d (%8lu bytes)\n", top_mob_index, top_mob_index * (sizeof(*pMobIndex)));
    for (fch = char_list; fch != NULL; fch = fch->next)
    {
	count++;
	if (fch->pcdata != NULL)
	    num_pcs++;
	for (af = fch->affected; af != NULL; af = af->next)
	    aff_count++;
    }
    for (fch = char_free; fch != NULL; fch = fch->next)
	count2++;
    fprintf(fp,"Mobs    %4d (%8lu bytes), %2d free (%lu bytes)\n",
      count, count * (sizeof(*fch)), count2, count2 * (sizeof(*fch)));
    count = 0;
    for (pc = pcdata_free; pc != NULL; pc = pc->next)
	count++;
    fprintf(fp,"Pcdata  %4d (%8lu bytes), %2d free (%lu bytes)\n",
      num_pcs, num_pcs * (sizeof(*pc)), count, count * (sizeof(*pc)));
    count = 0; count2 = 0;
    for (d = descriptor_list; d != NULL; d = d->next)
	count++;
    for (d= descriptor_free; d != NULL; d = d->next)
	count2++;
    fprintf(fp, "Descs  %4d (%8lu bytes), %2d free (%lu bytes)\n",
      count, count * (sizeof(*d)), count2, count2 * (sizeof(*d)));
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
        if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
        {
	    for (af = pObjIndex->affected; af != NULL; af = af->next)
		aff_count++;
            nMatch++;
        }
    fprintf(fp,"ObjProt %4d (%8lu bytes)\n",top_obj_index, top_obj_index * (sizeof(*pObjIndex)));
    count = 0;  count2 = 0;
    for (obj = object_list; obj != NULL; obj = obj->next)
    {
	count++;
	for (af = obj->affected; af != NULL; af = af->next)
	    aff_count++;
    }
    for (obj = obj_free; obj != NULL; obj = obj->next)
	count2++;
    fprintf(fp,"Objs    %4d (%8lu bytes), %2d free (%8lu bytes)\n",
      count, count * (sizeof(*obj)), count2, count2 * (sizeof(*obj)));
    count = 0;
    for (af = affect_free; af != NULL; af = af->next)
	count++;
    fprintf(fp,"Affects %4d (%8lu bytes), %2d free (%8lu bytes)\n",
      aff_count, aff_count * (sizeof(*af)), count, count * (sizeof(*af)));
    fprintf(fp,"Rooms   %4d (%8lu bytes)\n", top_room, top_room * (sizeof(*room)));
    fprintf(fp,"Exits   %4d (%8lu bytes)\n", top_exit, top_exit * (sizeof(*exit)));
    fclose(fp);
    fp = fopen("mob.dmp","w");
    fprintf(fp,"\nMobile Analysis\n");
    fprintf(fp,  "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_mob_index; vnum++)
	if ((pMobIndex = get_mob_index(vnum)) != NULL)
	{
	    nMatch++;
	    fprintf(fp,"#%-4d %3d active %3d killed     %s\n",
              pMobIndex->vnum,pMobIndex->count, pMobIndex->killed,pMobIndex->short_descr);
	}
    fclose(fp);
    fp = fopen("obj.dmp","w");
    fprintf(fp,"\nObject Analysis\n");
    fprintf(fp,  "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_obj_index; vnum++)
	if ((pObjIndex = get_obj_index(vnum)) != NULL)
	{
	    nMatch++;
	    fprintf(fp,"#%-4d %3d active %3d reset      %s\n",
              pObjIndex->vnum,pObjIndex->count, pObjIndex->reset_num,pObjIndex->short_descr);
	}
    fclose(fp);
    fpReserve = fopen( NULL_FILE, "r" );
}

/* Stick a little fuzz on a number. */
int number_fuzzy( int number )
{
    switch ( number_bits( 2 ) )
    {
    case 0:  number -= 1; break;
    case 3:  number += 1; break;
    }
    return UMAX( 1, number );
}

int temp_adjust( int number )
{
    int hour, month, week;
    if (mud_data.time_info.month <= 8)
	month = mud_data.time_info.month - 4;
    else
	month = 4 - (mud_data.time_info.month - 8);
    week = mud_data.time_info.day / 7 - 2;
    if (mud_data.time_info.hour <= 12)
	hour = mud_data.time_info.hour - 6;
    else
	hour = 6 - (mud_data.time_info.hour - 12);
    return number + (month * 5) + week + hour;
}

/* Generate a random number. */
int number_range( int from, int to )
{
    int power, number;
    if (from == 0 && to == 0)
	return 0;
    if ( ( to = to - from + 1 ) <= 1 )
	return from;
    for ( power = 2; power < to; power <<= 1 );
    while ( ( number = number_mm() & (power -1 ) ) >= to );
    return from + number;
}

/* Generate a percentile roll. */
int number_percent( void )
{
    int percent;
    while ( (percent = number_mm() & (128-1) ) > 99 );
    return 1 + percent;
}

/* Generate a random door. */
int number_door( void )
{
    int door;
    while ( ( door = number_mm() & (8-1) ) > 5);
    return door;
}

/* Generate a random door from the room. */
/* -1 on no valid doors */
int random_door( CHAR_DATA* ch, ROOM_INDEX_DATA* room )
{
    int doors[MAX_DOOR];
    int i = 0;
    int max_door = 0;

    for (i = 0; i < MAX_DOOR; i++){
      if (room->exit[i] != NULL
	  && room->exit[i]->to_room
	  && can_see_room(ch, room->exit[i]->to_room))
	doors[max_door++] = i;
    }

    return max_door == 0 ? -1 : doors[number_range(0, max_door - 1)];
}

int number_bits( int width )
{
    return number_mm( ) & ( ( 1 << width ) - 1 );
}

/* I've gotten too many bad reports on OS-supplied random number generators. *
 * This is the Mitchell-Moore algorithm from Knuth Volume II.                *
 * Best to leave the constants alone unless you've read Knuth.               */

/* I noticed streaking with this random number generator, so I switched *
 * back to the system srandom call.  If this doesn't work for you,      *
 * define OLD_RAND to use the old system.                               */
#if defined (OLD_RAND)
static  int     rgiState[2+55];
#endif

void init_mm( )
{
#if defined (OLD_RAND)
    int *piState;
    int iState;
    piState     = &rgiState[2];
    piState[-2] = 55 - 55;
    piState[-1] = 55 - 24;
    piState[0]  = ((int) mud_data.current_time) & ((1 << 30) - 1);
    piState[1]  = 1;
    for ( iState = 2; iState < 55; iState++ )
    {
        piState[iState] = (piState[iState-1] + piState[iState-2]) & ((1 << 30) - 1);
    }
#else
    srandom(time(NULL)^getpid());
#endif
}

long number_mm( void )
{
#if defined (OLD_RAND)
    int *piState;
    int iState1, iState2, iRand;
    piState             = &rgiState[2];
    iState1             = piState[-2];
    iState2             = piState[-1];
    iRand               = (piState[iState1] + piState[iState2]) & ((1 << 30) - 1);
    piState[iState1]    = iRand;
    if ( ++iState1 == 55 )
        iState1 = 0;
    if ( ++iState2 == 55 )
        iState2 = 0;
    piState[-2]         = iState1;
    piState[-1]         = iState2;
    return iRand >> 6;
#else
    return random() >> 6;
#endif
}

/* Roll some dice. */
int dice( int number, int size )
{
    int idice, sum;
    switch ( size )
    {
    case 0: return 0;
    case 1: return number;
    }
    for ( idice = 0, sum = 0; idice < number; idice++ )
	sum += number_range( 1, size );
    return sum;
}

/* Simple linear interpolation. */
int interpolate( int level, int value_00, int value_32 )
{
    return value_00 + level * (value_32 - value_00) / 32;
}

/* Removes the tildes from a string.                        *
 * Used for player-entered strings that go into disk files. */
void smash_tilde( char *str )
{
    for ( ; *str != '\0'; str++ )
	if ( *str == '~' )
	    *str = '-';
}

void smash_tilde2( char *str )
{
    for ( ; *str != '\0'; str++ )
	if ( *str == '~' )
	    *str = '\0';
}

/* Finds a string and replaces it with another */
void str_replace(char *buf, const char *s, const char *repl)
{
    char out_buf[MSL];
    char *pc, *out;
    int len = strlen(s);
    bool found = FALSE;
    for (pc = buf, out = out_buf; *pc && (out-out_buf) < (MSL-len-4); )
    {
	if (!strncasecmp(pc, s, len))
	{
	    out += sprintf(out, "%s", repl);
            pc += len;
            found = TRUE;
	}
	else
	    *out++ = *pc++;
    }
    if (found) /* don't bother copying if we did not change anything */
    {
        *out = '\0';
        strcpy(buf, out_buf);
    }
}

/* Compare strings, case insensitive.           *
 * Return TRUE if different                     *
 *   (compatibility with historical functions). */
bool str_cmp( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
	if (bstr == NULL)
	    bug( "Str_cmp %d: null astr.", 1 );
	else
	    bugf( "Str_cmp: null astr, bstr = %s.", bstr );
	return TRUE;
    }
    if ( bstr == NULL )
    {
	bug( "Str_cmp: null bstr.", 0 );
	return TRUE;
    }
    for ( ; *astr || *bstr; astr++, bstr++ )
	if ( LOWER(*astr) != LOWER(*bstr) )
	    return TRUE;
    return FALSE;
}

/* Compare strings, case sensitive.           *
 * Return TRUE if different                     *
 *   (compatibility with historical functions). */
bool str_cmp_2( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
	bug( "Str_cmp 2: null astr.", 0 );
	return TRUE;
    }
    if ( bstr == NULL )
    {
	bug( "Str_cmp: null bstr.", 0 );
	return TRUE;
    }
    for ( ; *astr || *bstr; astr++, bstr++ )
	if ((*astr) != (*bstr) )
	    return TRUE;
    return FALSE;
}

/* Compare strings, case insensitive, for prefix matching. *
 * Return TRUE if astr not a prefix of bstr                *
 *   (compatibility with historical functions).            */
bool str_prefix( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
	bug( "Strn_cmp: null astr.", 0 );
	return TRUE;
    }
    if ( bstr == NULL )
    {
	bug( "Strn_cmp: null bstr.", 0 );
	return TRUE;
    }
    for ( ; *astr; astr++, bstr++ )
	if ( LOWER(*astr) != LOWER(*bstr) )
	    return TRUE;
    return FALSE;
}

/* Compare strings, case insensitive, for match anywhere. *
 * Returns TRUE is astr not part of bstr.                 *
 *   (compatibility with historical functions).           */
bool str_infix( const char *astr, const char *bstr )
{
    int sstr1, sstr2, ichar;
    char c0;
    if ( ( c0 = LOWER(astr[0]) ) == '\0' )
	return FALSE;
    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);
    for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
	if ( c0 == LOWER(bstr[ichar]) && !str_prefix( astr, bstr + ichar ) )
	    return FALSE;
    return TRUE;
}

/* Compare strings, case insensitive, for suffix matching. *
 * Return TRUE if astr not a suffix of bstr                *
 *   (compatibility with historical functions).            */
bool str_suffix( const char *astr, const char *bstr )
{
    int sstr1 = strlen(astr), sstr2 = strlen(bstr);
    if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
	return FALSE;
    else
	return TRUE;
}

/* Returns an initial-capped string. */
char *capitalize( const char *str )
{
    static char strcap[MSL];
    int i;
    for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = LOWER(str[i]);
    strcap[i] = '\0';
    strcap[0] = UPPER(strcap[0]);
    return strcap;
}

/* Append a string to a file. */
void append_file( CHAR_DATA *ch, char *file, char *str )
{
    FILE *fp;
// change back for IS_NPC(ch)
    if (  str[0] == '\0' )
	return;
    fclose( fpReserve );
    if ( ( fp = fopen( file, "a" ) ) == NULL )
    {
	perror( file );
	send_to_char( "Could not open the file!\n\r", ch );
    }
    else
        fprintf( fp, "[%5d] %s: %s\n", ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/* Append a string to a file. */
void append_string( char *file, char *str )
{
    FILE *fp;
    if ( str[0] == '\0' )
	return;
    fclose( fpReserve );
    if ( ( fp = fopen( file, "a" ) ) == NULL )
	perror( file );
    else
        fprintf( fp, "%s", str );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/* Reports a bug. */
void bug( const char *str, int param )
{
    char buf[MSL];
    FILE *fp;
    if ( fpArea != NULL )
    {
	int iLine, iChar;
	if ( fpArea == stdin )
	    iLine = 0;
	else
	{
	    iChar = ftell( fpArea );
	    fseek( fpArea, 0, 0 );
	    for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
                while ( getc( fpArea ) != '\n' );
	    fseek( fpArea, iChar, 0 );
	}
	nlogf( "[*****] FILE: %s LINE: %d", strArea, iLine );
	if ( ( fp = fopen( "shutdown.txt", "a" ) ) != NULL )
   	    fprintf( fp, "[*****] %s\n", buf );
	else
	    fp = fopen( NULL_FILE, "r" );
        fclose( fp );
    }
    strcpy( buf, "[*****] BUG: " );
    sprintf( buf + strlen(buf), str, param );
    log_string( buf );
}


/* Writes a string into Event log */
void log_event(CHAR_DATA* ch, char* str)
{append_file(ch, EVENT_FILE, str );}



/* Writes a string to the log. */
void log_string( const char *str )
{
    char *strtime = ctime( &mud_data.current_time );
    strtime[strlen(strtime)-1] = '\0';
    fprintf( stderr, "%s :: %s\n", strtime, str );
}

/* This function is here to aid in debugging.                      *
 * If the last expression in a function is another function call,  *
 *   gcc likes to generate a JMP instead of a CALL.                *
 * This is called "tail chaining."                                 *
 * It hoses the debugger call stack for that call.                 *
 * So I make this the last call in certain critical functions,     *
 *   where I really need the call stack to be right for debugging! *
 * If you don't understand this, then LEAVE IT ALONE.              *
 * Don't remove any calls to tail_chain anywhere.                  */
void tail_chain( void )
{
    return;
}

/* returns help index */
HELP_DATA *get_help_index( int vnum ){
  HELP_DATA* pHelp;
  for(pHelp = help_first ; pHelp; pHelp = pHelp->next ){
    if ( pHelp->vnum == vnum )
      return( pHelp );
  }
  return NULL;
}

/* returns cabal index */
CABAL_INDEX_DATA *get_cabal_index( int vnum ){
  CABAL_INDEX_DATA* pCab;
  for(pCab = cabal_index_list ; pCab; pCab = pCab->next ){
    if ( pCab->vnum == vnum )
      return( pCab );
  }
  return NULL;
}

/* returns cabal index */
CABAL_INDEX_DATA *get_cabal_index_str( char* name ){
  CABAL_INDEX_DATA* pCab;
  for(pCab = cabal_index_list ; pCab; pCab = pCab->next ){
    if (LOWER(name[0]) == LOWER(pCab->name[0]) && !str_cmp(name, pCab->name))
      return( pCab );
  }
  return NULL;
}

/* returns trap index */
TRAP_INDEX_DATA *get_trap_index( int vnum ){
  TRAP_INDEX_DATA* pTrap;
  for(pTrap = trap_index_list ; pTrap; pTrap = pTrap->next ){
    if ( pTrap->vnum == vnum )
      return( pTrap );
  }
  return NULL;
}


/* MOB Programs */
/* Returns the index of the prog list */
PROG_CODE *get_prog_index( int vnum, int type )
{
    PROG_CODE *prg;

    switch ( type )
    {
	case PRG_MPROG:
	    prg = mprog_list;
	    break;
	case PRG_OPROG:
	    prg = oprog_list;
	    break;
	case PRG_RPROG:
	    prg = rprog_list;
	    break;
	default:
	    return NULL;
    }

    for( ; prg; prg = prg->next )
    {
	if ( prg->vnum == vnum )
            return( prg );
    }
    return NULL;
}

/* This routine transfers between alpha and numeric forms of the      *
 *  mob_prog bitvector types. This allows the use of the words in the *
 *  mob/script files.                                                 */
int mprog_name_to_type ( char *name )
{
    if ( !str_cmp( name, "in_file_prog"   ) )    return IN_FILE_PROG;
    if ( !str_cmp( name, "act_prog"       ) )    return ACT_PROG;
    if ( !str_cmp( name, "speech_prog"    ) )    return SPEECH_PROG;
    if ( !str_cmp( name, "rand_prog"      ) )    return RAND_PROG;
    if ( !str_cmp( name, "fight_prog"     ) )    return FIGHT_PROG;
    if ( !str_cmp( name, "hitprcnt_prog"  ) )    return HITPRCNT_PROG;
    if ( !str_cmp( name, "death_prog"     ) )    return DEATH_PROG;
    if ( !str_cmp( name, "entry_prog"     ) )    return ENTRY_PROG;
    if ( !str_cmp( name, "greet_prog"     ) )    return GREET_PROG;
    if ( !str_cmp( name, "all_greet_prog" ) )    return ALL_GREET_PROG;
    if ( !str_cmp( name, "give_prog"      ) )    return GIVE_PROG;
    if ( !str_cmp( name, "bribe_prog"     ) )    return BRIBE_PROG;
    if ( !str_cmp( name, "time_prog"	  ) )	 return TIME_PROG;
    if ( !str_cmp( name, "get_prog"       ) )    return GET_PROG;
    if ( !str_cmp( name, "drop_prog"      ) )    return DROP_PROG;
    if ( !str_cmp( name, "guild_prog"     ) )    return GUILD_PROG;
    if ( !str_cmp( name, "bloody_prog"    ) )    return BLOODY_PROG;
    if ( !str_cmp( name, "justice_prog"    ) )   return JUSTICE_PROG;
    return( ERROR_PROG );
}

/* This routine reads in scripts of MOBprograms from a file */
MPROG_DATA* mprog_file_read( char *f, MPROG_DATA *mprg, MOB_INDEX_DATA *pMobIndex )
{
    char        MOBProgfile[MIL];
    char        letter;
    MPROG_DATA *mprg2;
    FILE       *progfile;
    bool        done = FALSE;
    sprintf( MOBProgfile, "%s%s", MOB_DIR, f);
    progfile = fopen( MOBProgfile, "r" );
    if ( !progfile )
    {
        bug( "Mob: %d couldnt open mobprog file", pMobIndex->vnum );
	printf ("File: %s\n", MOBProgfile);
	fclose (progfile);
        exit( 1 );
    }
    mprg2 = mprg;
    switch ( letter = fread_letter( progfile ) )
    {
    case '>':
	mprog_count++;
        break;
    case '|':
        bug( "empty mobprog file.", 0 );
        exit( 1 );
        break;
    default:
        bug( "in mobprog file syntax error.", 0 );
	printf ("File: %s\n", MOBProgfile);
        exit( 1 );
        break;
    }
    while ( !done )
    {
        mprg2->type = mprog_name_to_type( fread_word( progfile ) );
        switch ( mprg2->type )
        {
        case ERROR_PROG:
            bug( "mobprog file type error", 0 );
            exit( 1 );
            break;
        case IN_FILE_PROG:
            bug( "mprog file contains a call to file.", 0 );
            exit( 1 );
            break;
        default:
            pMobIndex->progtypes = pMobIndex->progtypes | mprg2->type;
            mprg2->arglist       = fread_string( progfile );
            mprg2->comlist       = fread_string( progfile );
            switch ( letter = fread_letter( progfile ) )
            {
            case '>':
		mprog_count++;
                mprg2->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
                mprg2       = mprg2->next;
                mprg2->next = NULL;
                break;
            case '|':
                done = TRUE;
                break;
            default:
                bug( "in mobprog file syntax error.", 0 );
                exit( 1 );
                break;
            }
            break;
        }
    }
    fclose( progfile );
    return mprg2;
}

/* Snarf a MOBprogram section from the area file. */
/* This handles both the old and new format of mobprogs */
void load_mobprogs( FILE *fp )
{
    MOB_INDEX_DATA *iMob;
    MPROG_DATA     *original, *working;
    PROG_CODE *pMprog;
    TRIGFILE_DATA  *originalt, *workingt;
    char            letter;
    char	   *file;
    int             value;

    if ( area_last == NULL )
    {
	bug( "Load_new_mprogs: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    /* This function distinguishes between oldmobs and new progs */
    for ( ; ; ){
      sh_int vnum = 0;
      switch ( letter = fread_letter( fp ) )
	{
	default:
	  bug( "Load_mobprogs: bad command '%c'.",letter);
	  exit(1);
	  break;
	  /* New progs are always written before S */
	case 'S':
	case 's':
	  fread_to_eol( fp );
	  return;
	case '*':
	  fread_to_eol( fp );
	  break;
	  /* OLD PROG */
        case 'M':
        case 'm':
	  value = fread_number( fp );
	  mprogf_count++;
	  if ( ( iMob = get_mob_index( value ) ) == NULL )
            {
	      bug( "Load_mobprogs: vnum %d doesnt exist", value );
	      exit( 1 );
            }
	  if ( ( original = iMob->mobprogs ) )
	    for ( ; original->next != NULL; original = original->next );
	  working = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
	  if ( original )
	    original->next = working;
	  else
	    iMob->mobprogs = working;
	  working->dowhen = fread_number(fp);
	  file = fread_word( fp );
	  if ( ( originalt = iMob->trigfile ) )
	    for ( ; originalt->next != NULL; originalt = originalt->next );
	  workingt = (TRIGFILE_DATA *)alloc_perm( sizeof( TRIGFILE_DATA ) );
	  if ( originalt )
	    originalt->next = workingt;
	  else
	    iMob->trigfile = workingt;
	  workingt->name = str_dup( file );
	  workingt->dowhen = working->dowhen;
	  working = mprog_file_read( file , working, iMob );
	  workingt->next= NULL;
	  working->next = NULL;
	  fread_to_eol( fp );
	  break;

	  /* NEW PROG */
	case '#':
	  vnum		 = fread_number( fp );

	  if ( vnum == 0 )
	    continue; 	  /* skip read, old progs follow */

	  fBootDb = FALSE;
	  if ( get_prog_index( vnum, PRG_MPROG ) != NULL )
	    {
	      bug( "Load_new_mprogs: vnum %d duplicated.", vnum );
	      exit( 1 );
	    }
	  fBootDb = TRUE;
	  pMprog		= alloc_perm( sizeof(*pMprog) );
	  pMprog->vnum  	= vnum;
	  pMprog->code  	= fread_string( fp );
	  if ( mprog_list == NULL )
	    mprog_list = pMprog;
	  else
	    {
	      pMprog->next = mprog_list;
	      mprog_list 	= pMprog;
	    }
	  top_mprog_index++;
	}
    }
}

/* This procedure is responsible for reading any in_file MOBprograms. */
void mprog_read_programs( FILE *fp, MOB_INDEX_DATA *pMobIndex)
{
    MPROG_DATA *mprg;
    char letter;
    bool done = FALSE;
    if ( ( letter = fread_letter( fp ) ) != '>' )
    {
        bug( "Load_mobiles: vnum %d MOBPROG char", pMobIndex->vnum );
        exit( 1 );
    }
    pMobIndex->mobprogs = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
    mprg = pMobIndex->mobprogs;
    while ( !done )
    {
        mprg->type = mprog_name_to_type( fread_word( fp ) );
        switch ( mprg->type )
        {
        case ERROR_PROG:
            bug( "Load_mobiles: vnum %d MOBPROG type.", pMobIndex->vnum );
            exit( 1 );
        break;
        case IN_FILE_PROG:
            mprg = mprog_file_read( fread_string( fp ), mprg,pMobIndex );
            fread_to_eol( fp );
            switch ( letter = fread_letter( fp ) )
            {
            case '>':
                mprg->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
                mprg       = mprg->next;
                mprg->next = NULL;
                break;
            case '|':
                mprg->next = NULL;
                fread_to_eol( fp );
                done = TRUE;
                break;
            default:
                bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
                exit( 1 );
                break;
            }
            break;
        default:
            pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
            mprg->arglist        = fread_string( fp );
            fread_to_eol( fp );
            mprg->comlist        = fread_string( fp );
            fread_to_eol( fp );
            switch ( letter = fread_letter( fp ) )
            {
            case '>':
                mprg->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
                mprg       = mprg->next;
                mprg->next = NULL;
                break;
            case '|':
                mprg->next = NULL;
                fread_to_eol( fp );
                done = TRUE;
                break;
            default:
                bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
                exit( 1 );
                break;
            }
            break;
        }
    }
}

void do_repop( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;
    if ( argument[0] == 0)
    {
        reset_area(ch->in_room->area);
        send_to_char("Area repop!\n\r",ch);
    }
    if (!strcmp(argument, "room"))
    {
        reset_room(ch->in_room);
        send_to_char("room repop!\n\r",ch);
    }
    if (!strcmp(argument, "all"))
    {
        for (pArea = area_first; pArea; pArea = pArea->next)
            reset_area(pArea);
        send_to_char("World has been repopped!\n\r",ch);
    }
}

/* To have VLIST show more than vnum 0 - 9900, change the number below: */
#define MAX_SHOW_VNUM   327
#define NUL '\0'
#define COLUMNS                 5
#define MAX_ROW                 ((MAX_SHOW_VNUM / COLUMNS)+1)

extern ROOM_INDEX_DATA *       room_index_hash         [MAX_KEY_HASH];
const sh_int opposite_dir [6] = { DIR_SOUTH, DIR_WEST, DIR_NORTH, DIR_EAST, DIR_DOWN, DIR_UP };
typedef enum {exit_from, exit_to, exit_both} exit_status;

/* get the 'short' name of an area (e.g. MIDGAARD, MIRROR etc. *
 * assumes that the filename saved in the AREA_DATA struct     *
 * is something like midgaard.are                              */
char * area_name (AREA_DATA *pArea)
{
    static char buffer[64];
    char  *period;
    assert (pArea != NULL);
    strncpy (buffer, pArea->file_name, 64);
    period = strchr (buffer, '.');
    if (period)
        *period = '\0';
    return buffer;
}

void room_pair (ROOM_INDEX_DATA* left, ROOM_INDEX_DATA* right, exit_status ex, char *buffer)
{
    char *sExit;
    switch (ex)
    {
    default:
        sExit = "??"; break;
    case exit_from:
        sExit = "< "; break;
    case exit_to:
        sExit = " >"; break;
    case exit_both:
        sExit = "<>"; break;
    }
    sprintf (buffer, "%5d %-26.26s %s%5d %-26.26s(%-8.8s)\n\r",
      left->vnum, left->name, sExit, right->vnum, right->name, area_name(right->area));
}

void checkexits (ROOM_INDEX_DATA *room, AREA_DATA *pArea, char* buffer)
{
    char buf[MSL];
    int i;
    EXIT_DATA *exit;
    ROOM_INDEX_DATA *to_room;
    strcpy (buffer, "");
    for (i = 0; i < 6; i++)
    {
        exit = room->exit[i];
        if (!exit)
            continue;
        else
            to_room = exit->to_room;
        if (to_room)
        {
            if ( (room->area == pArea) && (to_room->area != pArea) )
            {
                if ( to_room->exit[opposite_dir[i]] && to_room->exit[opposite_dir[i]]->to_room == room )
                    room_pair (room,to_room,exit_both,buf);
                else
                    room_pair (room,to_room,exit_to,buf);
                strcat (buffer, buf);
            }
            else if ( (room->area != pArea) && (exit->to_room->area == pArea) )
                if (!(to_room->exit[opposite_dir[i]] && to_room->exit[opposite_dir[i]]->to_room == room ))
                {
                    room_pair (to_room,room,exit_from,buf);
                    strcat (buffer, buf);
                }
        }
    }
}

void do_exlist (CHAR_DATA *ch, char * argument)
{
    AREA_DATA* pArea;
    ROOM_INDEX_DATA* room;
    int i;
    char buffer[MSL];
    pArea = ch->in_room->area;
    for (i = 0; i < MAX_KEY_HASH; i++)
        for (room = room_index_hash[i]; room != NULL; room = room->next)
        {
            checkexits (room, pArea, buffer);
            send_to_char (buffer, ch);
        }
}

void do_vlist (CHAR_DATA *ch, char *argument)
{
    int i, j, vnum;
    ROOM_INDEX_DATA *room;
    char buf2 [100];
    BUFFER *buffer;
    buffer = new_buf();
    for (i = 0; i < MAX_ROW; i++)
    {
	add_buf(buffer,"\n\r");
        for (j = 0; j < COLUMNS; j++)
        {
            vnum = (i*COLUMNS + j);
            if (vnum < MAX_SHOW_VNUM)
            {
                room = get_room_index (vnum * 100 + 1);
		if (!room)
		    room = get_room_index (vnum * 100 + 5);
		if (!room)
		    room = get_room_index (vnum * 100 + 11);
                sprintf (buf2, "%3d %-8.8s  ", vnum, room ? area_name(room->area) : "-" );
                add_buf (buffer,buf2);
            }
        }
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void load_socials( FILE *fp)
{
    for ( ; ; )
    {
    	struct social_type social;
    	char *temp;
	social.char_no_arg = NULL;
	social.others_no_arg = NULL;
	social.char_found = NULL;
	social.others_found = NULL;
	social.vict_found = NULL;
	social.char_not_found = NULL;
	social.char_auto = NULL;
	social.others_auto = NULL;
    	temp = fread_word(fp);
    	if (!strcmp(temp,"#0"))
            return;
#if defined(social_debug)
	else
	    fprintf(stderr,"%s\n\r",temp);
#endif
        strcpy(social.name,temp);
    	fread_to_eol(fp);
	temp = fread_string_eol(fp);
	if (!strcmp(temp,"$"))
	     social.char_no_arg = NULL;
	else if (!strcmp(temp,"#"))
	{
	     social_table[social_count] = social;
	     social_count++;
	     continue;
	}
        else
	    social.char_no_arg = temp;
        temp = fread_string_eol(fp);
        if (!strcmp(temp,"$"))
             social.others_no_arg = NULL;
        else if (!strcmp(temp,"#"))
        {
	     social_table[social_count] = social;
             social_count++;
             continue;
        }
        else
	    social.others_no_arg = temp;
        temp = fread_string_eol(fp);
        if (!strcmp(temp,"$"))
             social.char_found = NULL;
        else if (!strcmp(temp,"#"))
        {
	     social_table[social_count] = social;
             social_count++;
             continue;
        }
       	else
	    social.char_found = temp;
        temp = fread_string_eol(fp);
        if (!strcmp(temp,"$"))
             social.others_found = NULL;
        else if (!strcmp(temp,"#"))
        {
	     social_table[social_count] = social;
             social_count++;
             continue;
        }
        else
	    social.others_found = temp;
        temp = fread_string_eol(fp);
        if (!strcmp(temp,"$"))
             social.vict_found = NULL;
        else if (!strcmp(temp,"#"))
        {
	     social_table[social_count] = social;
             social_count++;
             continue;
        }
        else
	    social.vict_found = temp;
        temp = fread_string_eol(fp);
        if (!strcmp(temp,"$"))
             social.char_not_found = NULL;
        else if (!strcmp(temp,"#"))
        {
	     social_table[social_count] = social;
             social_count++;
             continue;
        }
        else
	    social.char_not_found = temp;
        temp = fread_string_eol(fp);
        if (!strcmp(temp,"$"))
             social.char_auto = NULL;
        else if (!strcmp(temp,"#"))
        {
	     social_table[social_count] = social;
             social_count++;
             continue;
        }
        else
	    social.char_auto = temp;
        temp = fread_string_eol(fp);
        if (!strcmp(temp,"$"))
             social.others_auto = NULL;
        else if (!strcmp(temp,"#"))
        {
             social_table[social_count] = social;
             social_count++;
             continue;
        }
        else
	    social.others_auto = temp;
	social_table[social_count] = social;
    	social_count++;
   }
}

void load_army_indexes( FILE *fp ){

  if ( !area_last ){
    bug( "Load_army_indexes: no #AREA seen yet.", 0 );
    exit( 1 );
  }

  fread_army_indexes( fp, area_last );
}

void load_cabal_indexes( FILE *fp ){

  if ( !area_last ){
    bug( "Load_cabal_indexes: no #AREA seen yet.", 0 );
    exit( 1 );
  }
  fread_cabal_indexes( fp, area_last );
}

void load_mobiles( FILE *fp )
{
    MOB_INDEX_DATA *pMobIndex;
    if ( !area_last )
    {
        bug( "Load_mobiles: no #AREA seen yet.", 0 );
        exit( 1 );
    }
    for ( ; ; )
    {
        sh_int vnum;
        char letter;
        int iHash;
        letter                          = fread_letter( fp );
        if ( letter != '#' )
        {
            bug( "Load_mobiles: # not found.", 0 );
            exit( 1 );
        }
        vnum                            = fread_number( fp );
        if ( vnum == 0 )
            break;
        fBootDb = FALSE;
        if ( get_mob_index( vnum ) != NULL )
        {
            bug( "Load_mobiles: vnum %d duplicated.", vnum );
            exit( 1 );
        }
        fBootDb = TRUE;
        pMobIndex                       = alloc_perm( sizeof(*pMobIndex) );
        pMobIndex->vnum                 = vnum;
        pMobIndex->area                 = area_last;
	pMobIndex->new_format		= TRUE;
	newmobs++;
        pMobIndex->player_name          = fread_string( fp );
        pMobIndex->short_descr          = fread_string( fp );
        pMobIndex->long_descr           = fread_string( fp );
        pMobIndex->description          = fread_string( fp );
	pMobIndex->race		 	= race_lookup(fread_string( fp ));
        pMobIndex->long_descr[0]        = UPPER(pMobIndex->long_descr[0]);
        pMobIndex->description[0]       = UPPER(pMobIndex->description[0]);
        pMobIndex->act                  = fread_flag( fp ) | ACT_IS_NPC | race_table[pMobIndex->race].act;
        pMobIndex->act2                 = fread_flag( fp ) | ACT_IS_NPC | race_table[pMobIndex->race].act2;
        pMobIndex->affected_by          = fread_flag( fp ) | race_table[pMobIndex->race].aff;
        pMobIndex->affected2_by         = fread_flag( fp );
        pMobIndex->pShop                = NULL;
        pMobIndex->alignment            = fread_number( fp );
        pMobIndex->group                = fread_number( fp );
        pMobIndex->level                = fread_number( fp );
        pMobIndex->hitroll              = fread_number( fp );
        pMobIndex->hit[DICE_NUMBER]     = fread_number( fp );
        /* 'd'          */                fread_letter( fp );
        pMobIndex->hit[DICE_TYPE]   	= fread_number( fp );
        /* '+'          */                fread_letter( fp );
        pMobIndex->hit[DICE_BONUS]      = fread_number( fp );
	pMobIndex->mana[DICE_NUMBER]	= fread_number( fp );
					  fread_letter( fp );
	pMobIndex->mana[DICE_TYPE]	= fread_number( fp );
					  fread_letter( fp );
	pMobIndex->mana[DICE_BONUS]	= fread_number( fp );
	pMobIndex->damage[DICE_NUMBER]	= fread_number( fp );
					  fread_letter( fp );
	pMobIndex->damage[DICE_TYPE]	= fread_number( fp );
					  fread_letter( fp );
	pMobIndex->damage[DICE_BONUS]	= fread_number( fp );
	pMobIndex->dam_type		= attack_lookup(fread_word(fp));
	pMobIndex->ac[AC_PIERCE]	= fread_number( fp ) * 10;
	pMobIndex->ac[AC_BASH]		= fread_number( fp ) * 10;
	pMobIndex->ac[AC_SLASH]		= fread_number( fp ) * 10;
	pMobIndex->ac[AC_EXOTIC]	= fread_number( fp ) * 10;
        pMobIndex->off_flags            = fread_flag( fp ) | race_table[pMobIndex->race].off;
        pMobIndex->imm_flags            = fread_flag( fp ) | race_table[pMobIndex->race].imm;
        pMobIndex->res_flags            = fread_flag( fp ) | race_table[pMobIndex->race].res;
        pMobIndex->vuln_flags           = fread_flag( fp ) | race_table[pMobIndex->race].vuln;
	pMobIndex->start_pos		= position_lookup(fread_word(fp));
	pMobIndex->default_pos		= position_lookup(fread_word(fp));
	pMobIndex->sex			= sex_lookup(fread_word(fp));
	pMobIndex->pCabal		= get_cabal_index_str( fread_string( fp ) );

  	if (pMobIndex->start_pos < POS_SLEEPING)
	    pMobIndex->start_pos = POS_STANDING;
	if (pMobIndex->default_pos < POS_SLEEPING)
	    pMobIndex->default_pos = POS_STANDING;
	if (pMobIndex->start_pos > POS_STANDING)
	    pMobIndex->start_pos = POS_STANDING;
	if (pMobIndex->default_pos > POS_STANDING)
	    pMobIndex->default_pos = POS_STANDING;
	if (pMobIndex->sex > 3)
	    pMobIndex->sex = 0;
        pMobIndex->gold                 = fread_number( fp );
	if (!str_cmp(race_table[pMobIndex->race].name, "unique"))
	{
            pMobIndex->form             = fread_flag( fp ) | race_table[pMobIndex->race].form;
            pMobIndex->parts            = fread_flag( fp ) | race_table[pMobIndex->race].parts;
	}
	else
	{
	    fread_flag( fp );
	    fread_flag( fp );
            pMobIndex->form             = race_table[pMobIndex->race].form;
            pMobIndex->parts            = race_table[pMobIndex->race].parts;
	}
	if (IS_SET(pMobIndex->form, FORM_SNAKE))
	    SET_BIT(pMobIndex->form, FORM_REPTILE);
	if (IS_SET(pMobIndex->form, FORM_REPTILE))
	    SET_BIT(pMobIndex->form, FORM_COLD_BLOOD);
	if (IS_SET(pMobIndex->form, FORM_AMPHIBIAN))
	    SET_BIT(pMobIndex->form, FORM_COLD_BLOOD);
	if (IS_SET(pMobIndex->form, FORM_FISH))
	    SET_BIT(pMobIndex->form, FORM_COLD_BLOOD);
	pMobIndex->size			= size_lookup(fread_word(fp));
	fread_word(fp);
	for ( ; ; )
        {
            letter = fread_letter( fp );
            if (letter == 'F')
            {
		char *word;
		long vector;
                word                    = fread_word(fp);
		vector			= fread_flag(fp);
		if (!str_prefix(word,"act"))
		    REMOVE_BIT(pMobIndex->act,vector);
                else if (!str_prefix(word,"aff"))
		    REMOVE_BIT(pMobIndex->affected_by,vector);
		else if (!str_prefix(word,"off"))
		    REMOVE_BIT(pMobIndex->off_flags,vector);
		else if (!str_prefix(word,"imm"))
		    REMOVE_BIT(pMobIndex->imm_flags,vector);
		else if (!str_prefix(word,"res"))
		    REMOVE_BIT(pMobIndex->res_flags,vector);
		else if (!str_prefix(word,"vul"))
		    REMOVE_BIT(pMobIndex->vuln_flags,vector);
		else if (!str_prefix(word,"for"))
		    REMOVE_BIT(pMobIndex->form,vector);
		else if (!str_prefix(word,"par"))
		    REMOVE_BIT(pMobIndex->parts,vector);
		else
		{
		    bug("Flag remove: flag not found.",0);
		    exit(1);
		}
	     }
             else if ( letter == '>' )
	     {
		ungetc(letter,fp);
                mprog_read_programs( fp, pMobIndex );
                break;
             }
	    else if ( letter == 'P' )
	      {
		PROG_LIST *pMprog;
		char *word;
		int trigger = 0;

		pMprog		= alloc_perm(sizeof(*pMprog));
		word		= fread_word( fp );
		if ( !(trigger = flag_lookup( word, mprog_flags )) )
		  {
		    bug( "MOBprogs: invalid trigger.",0);
		    exit(1);
		  }
		SET_BIT( pMobIndex->mprog_flags, trigger );
		pMprog->trig_type	= trigger;
		pMprog->vnum		= fread_number( fp );
		pMprog->trig_phrase	= fread_string( fp );
		pMprog->next		= pMobIndex->mprogs;
		pMobIndex->mprogs	= pMprog;
	      }
	     else
	     {
		ungetc(letter,fp);
		break;
	     }
	}
        iHash                   = vnum % MAX_KEY_HASH;
        pMobIndex->next         = mob_index_hash[iHash];
        mob_index_hash[iHash]   = pMobIndex;
        top_mob_index++;
        top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;
        assign_area_vnum( vnum );
        kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;
    }
}

void load_objects( FILE *fp )
{
  sh_int vnum = 0;
    OBJ_INDEX_DATA *pObjIndex;
    if ( !area_last )
    {
        bug( "Load_objects: no #AREA seen yet.", 0 );
        exit( 1 );
    }
    for ( ; ; )
    {
      int iHash;
      char letter = '\0';
      vnum = 0;
      letter = fread_letter( fp );
      if ( letter != '#' )
        {
            bug( "Load_objects: # not found.", 0 );
            exit( 1 );
        }
      vnum = fread_number( fp );
      if ( vnum == 0 ) {
	break;
      }
      fBootDb = FALSE;
      if ( get_obj_index( vnum ) != NULL )
        {
	  bug( "Load_objects: vnum %d duplicated.", vnum );
	  exit( 1 );
        }
      fBootDb = TRUE;
      pObjIndex                       = alloc_perm( sizeof(*pObjIndex) );
      pObjIndex->vnum                 = vnum;
      pObjIndex->area                 = area_last;
      pObjIndex->new_format           = TRUE;
      pObjIndex->pCabal		      = NULL;
      pObjIndex->race		      = 0;
      pObjIndex->class		      = -1;
      pObjIndex->reset_num	      = 0;
      newobjs++;
      pObjIndex->name                 = fread_string( fp );
      pObjIndex->short_descr          = fread_string( fp );
      pObjIndex->description          = fread_string( fp );
      pObjIndex->material	      = fread_string( fp );
      pObjIndex->item_type            = item_lookup(fread_word( fp ));
      pObjIndex->extra_flags          = fread_flag( fp );
      pObjIndex->wear_flags           = fread_flag( fp );

      if (pObjIndex->item_type == ITEM_POTION && !CAN_WEAR(pObjIndex, ITEM_HOLD))
	SET_BIT(pObjIndex->wear_flags,ITEM_HOLD);
      switch(pObjIndex->item_type)
	{
	case ITEM_WEAPON:
	  pObjIndex->value[0]		= weapon_type(fread_word(fp));
	  pObjIndex->value[1]		= fread_number(fp);
	  pObjIndex->value[2]		= fread_number(fp);
	  pObjIndex->value[3]		= attack_lookup(fread_word(fp));
	  pObjIndex->value[4]		= fread_flag(fp);
	  break;
	case ITEM_THROW:
	  pObjIndex->value[0]		= fread_number(fp);
	  pObjIndex->value[1]		= fread_number(fp);
	  pObjIndex->value[2]		= fread_number(fp);
	  pObjIndex->value[3]		= attack_lookup(fread_word(fp));
	  pObjIndex->value[4]		= fread_flag(fp);
	  break;
	case ITEM_CONTAINER:
	  pObjIndex->value[0]		= fread_number(fp);
	  pObjIndex->value[1]		= fread_flag(fp);
	  pObjIndex->value[2]		= fread_number(fp);
	  pObjIndex->value[3]		= fread_number(fp);
	  pObjIndex->value[4]		= fread_number(fp);
	  break;
        case ITEM_DRINK_CON:
	case ITEM_FOUNTAIN:
	  pObjIndex->value[0]         = fread_number(fp);
	  pObjIndex->value[1]         = fread_number(fp);
	  pObjIndex->value[2]         = liq_lookup(fread_word(fp));
	  if (pObjIndex->value[2] == -1)
	    {
	      pObjIndex->value[2] = 0;
	      bug("Unknown liquid type",0);
	    }
	  pObjIndex->value[3]         = fread_number(fp);
	  pObjIndex->value[4]         = fread_number(fp);
	  break;
	case ITEM_WAND:
	case ITEM_STAFF:
	  pObjIndex->value[0]		= fread_number(fp);
	  pObjIndex->value[1]		= fread_number(fp);
	  pObjIndex->value[2]		= fread_number(fp);
	  pObjIndex->value[3]		= skill_lookup(fread_word(fp));
	  pObjIndex->value[4]		= fread_number(fp);
	  break;
	case ITEM_POTION:
	case ITEM_RELIC:
	case ITEM_ARTIFACT:
	case ITEM_PILL:
	case ITEM_SCROLL:
	case ITEM_HERB:
	  pObjIndex->value[0]		= fread_number(fp);
	  pObjIndex->value[1]		= skill_lookup(fread_word(fp));
	  pObjIndex->value[2]		= skill_lookup(fread_word(fp));
	  pObjIndex->value[3]		= skill_lookup(fread_word(fp));
	  pObjIndex->value[4]		= skill_lookup(fread_word(fp));
	  break;
        case ITEM_SOCKET:
	  pObjIndex->value[0]		= fread_flag(fp);
	  pObjIndex->value[1]		= fread_flag(fp);
	  pObjIndex->value[2]		= fread_flag(fp);
	  pObjIndex->value[3]		= fread_flag(fp);
	  pObjIndex->value[4]		= fread_flag(fp);
	  break;
	case ITEM_RANGED:
	  pObjIndex->value[0]		= fread_flag(fp);
	  pObjIndex->value[1]		= fread_number(fp);
	  pObjIndex->value[2]		= fread_number(fp);
	  pObjIndex->value[3]		= fread_number(fp);
	  pObjIndex->value[4]		= fread_flag(fp);
	  break;
        case ITEM_PROJECTILE:
          pObjIndex->value[0]           = fread_flag(fp);
          pObjIndex->value[1]           = fread_number(fp);
          pObjIndex->value[2]           = fread_number(fp);
          pObjIndex->value[3]           = attack_lookup(fread_word(fp));
          pObjIndex->value[4]           = fread_flag(fp);
          break;
	default:
	  pObjIndex->value[0]             = fread_flag( fp );
	  pObjIndex->value[1]             = fread_flag( fp );
	  pObjIndex->value[2]             = fread_flag( fp );
	  pObjIndex->value[3]             = fread_flag( fp );
	  pObjIndex->value[4]		  = fread_flag( fp );
	  break;
	}
      pObjIndex->level		= fread_number( fp );
      pObjIndex->weight               = fread_number( fp );
      pObjIndex->cost                 = fread_number( fp );
      letter 				= fread_letter( fp );
      switch (letter)
 	{
	case ('P') :		pObjIndex->condition = 100; break;
	case ('G') :		pObjIndex->condition =  90; break;
	case ('A') :		pObjIndex->condition =  75; break;
	case ('W') :		pObjIndex->condition =  50; break;
	case ('D') :		pObjIndex->condition =  25; break;
	case ('B') :		pObjIndex->condition =  10; break;
	case ('R') :		pObjIndex->condition =   0; break;
	default:			pObjIndex->condition = 100; break;
	}
      for ( ; ; )
        {
	  char letter;
	  letter = fread_letter( fp );
	  if ( letter == 'E' )
            {
	      EXTRA_DESCR_DATA *ed;
	      ed                      = alloc_perm( sizeof(*ed) );
	      ed->keyword             = fread_string( fp );
	      ed->description         = fread_string( fp );
	      ed->next                = pObjIndex->extra_descr;
	      pObjIndex->extra_descr  = ed;
	      top_ed++;
            }
	  else if ( letter == 'A' )
            {
	      AFFECT_DATA *paf;
	      paf                     = alloc_perm( sizeof(*paf) );
	      paf->where		= TO_OBJECT;
	      paf->type               = -1;
	      paf->level              = pObjIndex->level;
	      paf->duration           = -1;
	      paf->location           = fread_number( fp );
	      paf->modifier           = fread_number( fp );
	      paf->bitvector          = 0;
	      paf->next               = pObjIndex->affected;
	      pObjIndex->affected     = paf;
	      top_affect++;
            }
	  else if ( letter == 'C')
	    pObjIndex->pCabal		= get_cabal_index_str( fread_string( fp ) );
	  else if (letter == 'F')
            {
	      AFFECT_DATA *paf;
	      paf                     = alloc_perm( sizeof(*paf) );
	      letter 			= fread_letter(fp);
	      switch (letter)
	 	{
                case 'A': paf->where = TO_AFFECTS;  break;
                case 'I': paf->where = TO_IMMUNE;   break;
                case 'R': paf->where = TO_RESIST;   break;
                case 'S': paf->where = TO_SKILL;   break;
                case 'V': paf->where = TO_VULN;     break;
                default:  bug( "Load_objects: Bad where on flag set.", 0 ); exit( 1 );
		}
                paf->type               = -1;
                paf->level              = pObjIndex->level;
                paf->duration           = -1;
                paf->location           = fread_number(fp);
                paf->modifier           = fread_number(fp);
                paf->bitvector          = fread_flag(fp);
                paf->next               = pObjIndex->affected;
                pObjIndex->affected     = paf;
                top_affect++;
            }
	  else if ( letter == 'L')
	    pObjIndex->class = fread_number (fp );
	  else if ( letter == 'M' )
	    {
		OBJ_MESSAGE_DATA *new_message;
		new_message		= alloc_perm( sizeof( *new_message ) );
		new_message->onself	= fread_string( fp );
		new_message->onother	= fread_string( fp );
		new_message->offself	= fread_string( fp );
		new_message->offother	= fread_string( fp );
		pObjIndex->message	= new_message;
	    }
	  else if ( letter == 'P' )
	      {
		PROG_LIST *pOprog;
		char *word;
		int trigger = 0;

		pOprog		= alloc_perm(sizeof(*pOprog));
		word		= fread_word( fp );
		if ( !(trigger = flag_lookup( word, oprog_flags )) )
		  {
		    bug( "OBJprogs: invalid trigger.",0);
		    exit(1);
		  }
		SET_BIT( pObjIndex->oprog_flags, trigger );
		pOprog->trig_type	= trigger;
		pOprog->vnum		= fread_number( fp );
		pOprog->trig_phrase	= fread_string( fp );
		pOprog->next		= pObjIndex->oprogs;
		pObjIndex->oprogs	= pOprog;
	      }
	  else if ( letter == 'R')
	    pObjIndex->race = fread_number (fp );
	  else if( letter == 'T' )
            {
                OBJ_SPELL_DATA *new_spell;
                new_spell                  = alloc_perm( sizeof( *new_spell ) );
                new_spell->spell           = fread_number( fp );
                new_spell->target          = fread_number( fp );
		new_spell->percent	   = fread_number( fp );
		new_spell->message	   = fread_string( fp );
		new_spell->message2	   = fread_string( fp );
		new_spell->next		   = pObjIndex->spell;
                pObjIndex->spell           = new_spell;
            }

	  else
            {
	      ungetc( letter, fp );
	      break;
            }
        }
        iHash                   = vnum % MAX_KEY_HASH;
        pObjIndex->next         = obj_index_hash[iHash];
        obj_index_hash[iHash]   = pObjIndex;
        top_obj_index++;
        top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;
        assign_area_vnum( vnum );
    }
}

/* Converts all old format objects to new format.     *
 * Called by boot_db (db.c).                          *
 * Loops over all resets to find the level of the mob *
 * loaded before the object to determine the level of *
 * the object.                                        */
void convert_objects( void )
{
    int vnum;
    AREA_DATA  *pArea;
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pMob = NULL;
    OBJ_INDEX_DATA *pObj;
    ROOM_INDEX_DATA *pRoom;
    if ( newobjs == top_obj_index )
	return;
    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
        for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
        {
            if ( !( pRoom = get_room_index( vnum ) ) )
                continue;
            for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
            {
                switch ( pReset->command )
                {
                case 'M':
                    if ( !( pMob = get_mob_index( pReset->arg1 ) ) )
                        bug( "Convert_objects: 'M': bad vnum %d.", pReset->arg1 );
                    break;
                case 'O':
                    if ( !( pObj = get_obj_index( pReset->arg1 ) ) )
                    {
                        bug( "Convert_objects: 'O': bad vnum %d.", pReset->arg1 );
                        break;
                    }
                    if ( pObj->new_format )
                        continue;
                    if ( !pMob )
                    {
                        bug( "Convert_objects: 'O': No mob reset yet.", 0 );
                        break;
                    }
                    pObj->level = pObj->level < 1 ? pMob->level - 2 : UMIN(pObj->level, pMob->level - 2);
                    break;
                case 'P':
                    {
                        OBJ_INDEX_DATA *pObj, *pObjTo;
                        if ( !( pObj = get_obj_index( pReset->arg1 ) ) )
                        {
                            bug( "Convert_objects: 'P': bad vnum %d.", pReset->arg1 );
                            break;
                        }
                        if ( pObj->new_format )
                            continue;
                        if ( !( pObjTo = get_obj_index( pReset->arg3 ) ) )
                        {
                            bug( "Convert_objects: 'P': bad vnum %d.", pReset->arg3 );
                            break;
                        }
                        pObj->level = pObj->level < 1 ? pObjTo->level : UMIN(pObj->level, pObjTo->level);
                    }
                    break;
                case 'G':
                case 'E':
                    if ( !( pObj = get_obj_index( pReset->arg1 ) ) )
                    {
                        bug( "Convert_objects: 'E' or 'G': bad vnum %d.", pReset->arg1 );
                        break;
                    }
                    if ( !pMob )
                    {
                        bug( "Convert_objects: 'E' or 'G': null mob for vnum %d.",pReset->arg1 );
                        break;
                    }
                    if ( pObj->new_format )
                        continue;
                    if ( pMob->pShop )
                    {
                        switch ( pObj->item_type )
                        {
                        default:            pObj->level = UMAX(0, pObj->level); break;
                        case ITEM_PILL:
			case ITEM_RELIC:
			case ITEM_SOCKET:
			case ITEM_ARTIFACT:
                        case ITEM_POTION:   pObj->level = UMAX(5, pObj->level); break;
                        case ITEM_SCROLL:
                        case ITEM_ARMOR:
                        case ITEM_THROW:
                        case ITEM_WEAPON:   pObj->level = UMAX(10, pObj->level); break;
                        case ITEM_WAND:
                        case ITEM_TREASURE: pObj->level = UMAX(15, pObj->level); break;
			case ITEM_RANGED: pObj->level = UMAX(15, pObj->level); break;
                        case ITEM_PROJECTILE: pObj->level = UMAX(15,pObj->level); break;
                        case ITEM_INSTRUMENT:pObj->level= UMAX(10, pObj->level); break;
                        case ITEM_STAFF:    pObj->level = UMAX(20, pObj->level); break;
                        }
                    }
                    else
                        pObj->level = pObj->level < 1 ? pMob->level : UMIN( pObj->level, pMob->level );
                    break;
                }
            }
        }
    }
    for ( pArea = area_first; pArea ; pArea = pArea->next )
        for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
            if ( (pObj = get_obj_index( vnum )) )
                if ( !pObj->new_format )
                    convert_object( pObj );
}

/* Converts an old_format obj to new_format. *
 * Called by convert_objects (db2.c).        *
 * Dug out of create_obj (db.c)              */
void convert_object( OBJ_INDEX_DATA *pObjIndex )
{
    int level, number, type;
    if ( !pObjIndex || pObjIndex->new_format )
	return;
    level = pObjIndex->level;
    pObjIndex->level    = UMAX( 0, pObjIndex->level );
    pObjIndex->cost     = 10*level;
    switch ( pObjIndex->item_type )
    {
        default:
            bug( "Obj_convert: vnum %d bad type.", pObjIndex->item_type );
            break;
        case ITEM_LIGHT:
        case ITEM_TREASURE:
	case ITEM_RANGED:
        case ITEM_INSTRUMENT:
        case ITEM_FURNITURE:
        case ITEM_TRASH:
        case ITEM_CONTAINER:
        case ITEM_DRINK_CON:
        case ITEM_KEY:
        case ITEM_FOOD:
	case ITEM_HERB:
        case ITEM_BOAT:
        case ITEM_CORPSE_NPC:
        case ITEM_CORPSE_PC:
        case ITEM_FOUNTAIN:
        case ITEM_MAP:
        case ITEM_CLOTHING:
        case ITEM_SCROLL:
            break;
        case ITEM_WAND:
        case ITEM_STAFF:
            pObjIndex->value[2] = pObjIndex->value[1];
            break;
        case ITEM_THROW:
        case ITEM_WEAPON:
           /* The conversion below is based on the values generated *
            * in one_hit() (fight.c).  Since I don't want a lvl 50  *
            * weapon to do 15d3 damage, the min value will be below *
            * the one in one_hit, and to make up for it, I've made  *
            * the max value higher.                                 *
            * (I don't want 15d2 because this will hardly ever roll *
            * 15 or 30, it will only roll damage close to 23.       *
            * I can't do 4d8+11, because one_hit there is no dice-  *
            * bounus value to set...)                               *
            * The conversion below gives:                           *
            * level:   dice      min      max      mean             *
            *   1:     1d8      1( 2)    8( 7)     5( 5)            *
            *   2:     2d5      2( 3)   10( 8)     6( 6)            8
            *   3:     2d5      2( 3)   10( 8)     6( 6)            8
            *   5:     2d6      2( 3)   12(10)     7( 7)            *
            *  10:     4d5      4( 5)   20(14)    12(10)            *
            *  20:     5d5      5( 7)   25(21)    15(14)            *
            *  30:     5d7      5(10)   35(29)    20(20)            *
            *  50:     5d11     5(15)   55(44)    30(30)            */
            number = UMIN(level/4 + 1, 5);
            type   = (level + 7)/number;
            pObjIndex->value[1] = number;
            pObjIndex->value[2] = type;
            break;
        case ITEM_ARMOR:
            pObjIndex->value[0] = level / 5 + 3;
            pObjIndex->value[1] = pObjIndex->value[0];
            pObjIndex->value[2] = pObjIndex->value[0];
            break;
        case ITEM_POTION:
	case ITEM_RELIC:
	case ITEM_SOCKET:
	case ITEM_ARTIFACT:
        case ITEM_PILL:
            break;
        case ITEM_MONEY:
           pObjIndex->value[0] = pObjIndex->cost;
           break;
    }
    pObjIndex->new_format = TRUE;
    ++newobjs;
}

/* Converts an old_format mob into new_format. *
 * Called by load_old_mob (db.c).              *
 * Dug out of create_mobile (db.c)             */
void convert_mobile( MOB_INDEX_DATA *pMobIndex )
{
    int i, type, number, bonus, level;
    if ( !pMobIndex || pMobIndex->new_format )
	return;
    level = pMobIndex->level;
    pMobIndex->act              |= ACT_WARRIOR;
    pMobIndex->act2		= 0;
   /* Calculate hit dice.  Gives close to the hitpoints                     *
    * of old format mobs created with create_mobile()  (db.c)               *
    * A high number of dice makes for less variance in mobiles              *
    * hitpoints.                                                            *
    * (might be a good idea to reduce the max number of dice)               *
    * The conversion below gives:                                           *
    *                                                                       *
    *  level:     dice         min         max        diff       mean       *
    *    1:       1d2+6       7(  7)     8(   8)     1(   1)     8(   8)    *
    *    2:       1d3+15     16( 15)    18(  18)     2(   3)    17(  17)    *
    *    3:       1d6+24     25( 24)    30(  30)     5(   6)    27(  27)    *
    *    5:      1d17+42     43( 42)    59(  59)    16(  17)    51(  51)    *
    *   10:      3d22+96     99( 95)   162( 162)    63(  67)   131(    )    *
    *   15:     5d30+161    166(159)   311( 311)   145( 150)   239(    )    *
    *   30:    10d61+416    426(419)  1026(1026)   600( 607)   726(    )    *
    *   50:    10d169+920   930(923)  2610(2610)  1680(1688)  1770(    )    *
    *                                                                       *
    * The values in parenthesis give the values generated in create_mobile. *
    * Diff = max - min.  Mean is the arithmetic mean.                       *
    * (hmm.. must be some roundoff error in my calculations.. smurfette got *
    * 1d6+23 hp at level 3 ? -- anyway.. the values above should be         *
    * approximately right..)                                                */
    type   = level*level*27/40;
    number = UMIN(type/40 + 1, 10);
    type   = UMAX(2, type/number);
    bonus  = UMAX(0, level*(8 + level)*.9 - number*type);
    pMobIndex->hit[DICE_NUMBER]    = number;
    pMobIndex->hit[DICE_TYPE]      = type;
    pMobIndex->hit[DICE_BONUS]     = bonus;
    pMobIndex->mana[DICE_NUMBER]   = level;
    pMobIndex->mana[DICE_TYPE]     = 10;
    pMobIndex->mana[DICE_BONUS]    = 100;
   /* Calculate dam dice.  Gives close to the damage *
    * of old format mobs in damage()  (fight.c)      */
    type   = level*7/4;
    number = UMIN(type/8 + 1, 5);
    type   = UMAX(2, type/number);
    bonus  = UMAX(0, level*9/4 - number*type);
    pMobIndex->damage[DICE_NUMBER] = number;
    pMobIndex->damage[DICE_TYPE]   = type;
    pMobIndex->damage[DICE_BONUS]  = bonus;
    switch ( number_range( 1, 3 ) )
    {
        case (1): pMobIndex->dam_type =  3;       break;
        case (2): pMobIndex->dam_type =  7;       break;
        case (3): pMobIndex->dam_type = 11;       break;
    }
    for (i = 0; i < 3; i++)
        pMobIndex->ac[i]         = interpolate( level, 100, -100);
    pMobIndex->ac[3]             = interpolate( level, 100, 0);
    pMobIndex->gold              /= 100;
    pMobIndex->size              = SIZE_MEDIUM;
    pMobIndex->new_format        = TRUE;
    ++newmobs;
}

void do_hotreboot (CHAR_DATA *ch, char * argument)
{
    FILE *fp;
    DESCRIPTOR_DATA *d, *d_next;
    OBJ_DATA *obj, *obj_next;
    char buf [100], buf2[100];
    extern int control;
    char arg[MIL];
    bool fSave = FALSE; //if arg == 0 then safe, otherwise its due to a signal

    argument = one_argument(argument,arg);

    fp = fopen (HOTREBOOT_FILE, "w");
    if (!fp)
    {
        sprintf(buf,"`&Mud Control`8: ``Hot Reboot file not writeable, aborted.\n\r");
        do_echo(NULL,buf);
	log_string ("Could not write to hot reboot file.");
	perror ("do_hotreboot:fopen");
	fclose (fp);
	return;
    }
    /* Consider changing all saved areas here, if you use OLC */
    if (atoi(arg)==1)
	sprintf (buf, "\n\r *** SIGBUS detected: Mud Crash is imminent - Trying to Hot Reboot ***\n\r");
    else if (atoi(arg)==2)
	sprintf (buf, "\n\r *** SIGTERM detected: Mud Crash is imminent - Trying to Hot Reboot ***\n\r");
    else if (atoi(arg)==3)
	sprintf (buf, "\n\r *** SIGABRT detected: Mud Crash is imminent - Trying to Hot Reboot ***\n\r");
    else if (atoi(arg)==4)
	sprintf (buf, "\n\r *** Planetary anomaly detected! World destruction is imminent! Trying to world shift...  ***\n\r");
    else{
      fSave = TRUE;
      if (ch != NULL){
	sprintf (buf, "\n\r *** %s has initiated a world shift - please wait! *** \n\r", PERS2(ch));
      }
      else{
	sprintf (buf, "\n\r *** The world is shifting - please wait! *** \n\r");
      }
    }

    if (fSave){
      /* SAVE MUD STATUS */
      save_mud();
      /* SAVE CABAL DATA */
      save_cabals(TRUE, NULL );
      /* SAVE ARMIES */
      save_armies();
      /* SAVE CLANS */
      SaveClans();
    }
    /* Check for Maintance */
    if (reboot_buffer[0] != '\0')
      strcat(buf," *** Extended world shift, this may take some time. *** \n\r");

    /* For each playing descriptor, save its state */
    for (d = descriptor_list; d ; d = d_next)
    {
	CHAR_DATA * och = CH (d);
	d_next = d->next; /* We delete from the list , so need to save this */
	if (!d->character || d->connected != CON_PLAYING ) /* drop those logging on */
	{
	    write_to_descriptor (d->descriptor, "\n\rSorry, we are rebooting.  Come back in a few seconds.\n\r", 0);
	    close_socket (d); /* throw'em out */
	}
	else
	{
	    /* clean off blanket of darkness between reboots */
	    affect_strip(och, gsn_blanket);

	    /* force revert */
	    do_revert(och, "" );

	    /* same for charm person */
	    if (!IS_AFFECTED(och, AFF_CHARM))
	      affect_strip(och, gsn_charm_person);

	    fprintf (fp, "%d %s %s %s %ld\n", d->descriptor, och->name, d->ident, d->host, och->logon );
	    if (fSave)
	      save_char_obj (och);

	    write_to_descriptor (d->descriptor, buf, 0);
	    if (d->character->level > 51 && atoi(arg)>=1 && atoi(arg)<=4)
	    {
		write_to_descriptor (d->descriptor, last_malloc, 0);
		write_to_descriptor (d->descriptor, "\n\r", 0);
		write_to_descriptor (d->descriptor, last_loop, 0);
		write_to_descriptor (d->descriptor, "\n\r", 0);
		write_to_descriptor (d->descriptor, last_command, 0);
		write_to_descriptor (d->descriptor, "\n\r", 0);
		write_to_descriptor (d->descriptor, last_mprog, 0);
		write_to_descriptor (d->descriptor, "\n\r", 0);
	    }
	    extract_char(och, TRUE);
	}
    }
    fprintf (fp, "-1\n");
    fprintf (fp, "%d\n", mud_data.max_on);
    fprintf (fp, "%d\n", mud_data.deaths);
    fprintf (fp, "%ld\n", mud_data.crash_time);
    fclose (fp);
    fp = fopen (OBJSAVE_FILE, "w");
    if (fSave){
      for (obj = object_list; obj != NULL; obj = obj_next )
	{
	  obj_next = obj->next;
	  if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
	    continue;
	  if (obj->in_room != NULL
	      && obj->in_room->vnum != ROOM_VNUM_BOUNTY	//objects in bounty room are saved/loaded with bounties
	      && obj->carried_by == NULL
	      && !IS_VIRROOM(obj->in_room)
	      && (IS_LIMITED(obj)
		  || obj->homevnum != obj->in_room->vnum
		  || obj->contains != NULL
		|| CAN_WEAR(obj, ITEM_HAS_OWNER)) )
	    fwrite_obj(NULL, obj, fp, 0, 0, FALSE);
	  else if (obj->item_type == ITEM_CABAL && obj->carried_by && obj->carried_by->in_room)
	    fwrite_obj(NULL, obj, fp, 9999, 0, TRUE);
	}
    }
    fprintf (fp, "#END\n");
    fclose (fp);
    /* Close reserve and other always-open files and release other resources */
    fclose (fpReserve);
    /* exec - descriptors are inherited */
    sprintf (buf, "%d", mud_data.mudport);
    sprintf (buf2, "%d", control);
    execl ("../src/clands", "MUD", buf, "Forsaken_Lands", buf2, reboot_buffer, (char *) NULL);

    /* Failed - sucessful exec will not return */
    perror ("do_hotreboot: execl");
    sprintf(buf,"[`&The Voice of God`7]`8 - World shift FAILED!``");
    do_echo(NULL,buf);
    /* Here you might want to reopen fpReserve */
    fpReserve = fopen (NULL_FILE, "r");
}

/* Recover from a Hot Reboot - load players */
void hotreboot_recover ()
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch, *ch_next;
    FILE *fp;
    char name [100], ident[MSL], host[MSL];
    time_t logont, temp;
    int tempmax, tempdeaths, desc;
    bool fOld;
    int i = 0;
    log_string ("World shift recovery initiated");
    fp = fopen (HOTREBOOT_FILE, "r");
    if (!fp) /* there are some descriptors open which will hang forever then ? */
    {
	perror ("hotreboot_recover:fopen");
	log_string ("World shift file not found. Exiting.");
	fclose (fp);
	exit (1);
    }
    unlink (HOTREBOOT_FILE); /* In case something crashes - doesn't prevent reading	*/
    for (i = 0; i < 1000; i++)
    {
	if( fscanf (fp, "%d ", &desc) < 0 ){
	    desc = 0;
    }
	if (desc == -1)
	{
	    if( fscanf (fp, "\n%d\n", &tempmax) < 0 ){
	        tempmax = 0;
        }
	    if( fscanf (fp, "\n%d\n", &tempdeaths) < 0 ){
            tempdeaths = 0;
        }
	    if( fscanf (fp, "%ld\n", &temp) < 0 ){
            temp = 0;
        }
	    mud_data.max_on = tempmax;
	    mud_data.deaths = tempdeaths;
	    mud_data.crash_time = temp;
	    break;
	}
	if( fscanf (fp, "%s %s %s %ld\n", name, ident, host, &logont) < 0 ){
        name[0] = '\0';
	}
	/* Write something, and check if it goes error-free */
	if (!write_to_descriptor (desc, "\n\rRestoring from world shift...\n\r",0))
	{
	    close (desc); /* nope */
	    continue;
	}
	d = new_descriptor();
	d->descriptor = desc;
	d->ident = str_dup (ident);
	d->host = str_dup (host);
	d->next = descriptor_list;
	descriptor_list = d;
	d->connected = CON_HOTREBOOT_RECOVER; /* -15, so close_socket frees the char */
	/* Now, find the pfile */
	fOld = load_char (d, name);
	d->character->logon = logont;
	if (!fOld) /* Player file not found?! */
	{
	    write_to_descriptor (desc, "\n\rSomehow, your character was lost in the Hot Reboot. Sorry.\n\r", 0);
	    close_socket (d);
	}
	else /* ok! */
	{
	    write_to_descriptor (desc, "\n\rWorld shifting is complete.\n\r",0);
	    /* Just In Case */
	    if (!d->character->in_room)
	        d->character->in_room = get_room_index (ROOM_VNUM_TEMPLE);
	    /* Insert in the char_list */
	    load_obj (d->character);
	    d->character->next = char_list;
	    char_list = d->character;
	    char_to_room (d->character, d->character->in_room);
	    d->character->next_player = player_list;
	    player_list = d->character;
	    reset_char(d->character);
	    do_look (d->character, "");
	    act ("$n materializes!", d->character, NULL, NULL, TO_ROOM);
	    d->connected = CON_PLAYING;
	}
    }
    fclose (fp);
    /* This is to get the player's stallions and familiars and other stuff back after reboot */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
	ch_next = ch->next;
	if (!IS_NPC(ch) || ch->in_room == NULL)
	    continue;
	if ((ch->pIndexData->vnum >= MOB_VNUM_SERVANT_FIRE
	     && ch->pIndexData->vnum <= MOB_VNUM_SERVANT_EARTH)
	    && ch->summoner != NULL && !IS_NPC(ch->summoner)){
	  ch->summoner->pcdata->familiar = ch;
	}
	else if (ch->pIndexData->vnum == MOB_VNUM_MONSTER
		 && ch->summoner != NULL
		 && !IS_NPC(ch->summoner)){
	  ch->summoner->pcdata->familiar = ch;
	}
	/* reset zombies after rebot to originals (Viri: not needed, save in pfiles now
	else if (ch->pIndexData->vnum == MOB_VNUM_ZOMBIE){
	  AFFECT_DATA* paf;
	  MOB_INDEX_DATA* mob;
	  if ( (paf = affect_find(ch->affected, skill_lookup("animate corpse"))) != NULL
	       && (mob = get_mob_index(paf->modifier)) != NULL){
	    ch->off_flags = paf->bitvector;
	    ch->dam_type = mob->dam_type;
	    ch->act = mob->act;
	    REMOVE_BIT(mob->act,ACT_SCAVENGER);
	    REMOVE_BIT(mob->act,ACT_IS_HEALER);
	    REMOVE_BIT(mob->act,ACT_TRAIN);
	    REMOVE_BIT(mob->act,ACT_PRACTICE);
	    REMOVE_BIT(mob->act,ACT_TOO_BIG);
	  }
	}
	*/
    }
}

/* format_room_name */
/* links in db.c cabal.c */
/* written by viri */
bool format_room_name(ROOM_INDEX_DATA* room, char* room_name)
{
  //The following function attaches appropriate
  //ascii color string to the room name based on the
  //room_color_table in tables.c

  //buffer for the name.
  char buf[MIL];

  //pointers for the string
  char* strptr;
  char* bufptr;


/*
  1)prefix buf with appropriate ascii code.
  2)Run through the room_name copying non-ascii char's only.
  3)When terminator is hit or MIL is reached stop.
  4)attatch end of ascii sequence, termianate string.
*/
  if (room == NULL || room_name == NULL)
    return FALSE;

  strptr = room_name;
  bufptr = buf;

//we prefix the buffer.
  *bufptr++ = '`';
  *bufptr++ = room_color_table[UMIN(MAX_SECT, room->sector_type)].ch;

//First we cleant the name length is MSL - 2(prefix) - 2 (postifx).
  while( *strptr != '\0')
    {
      //we copy the string normaly unless there is asci code.
      if ( *strptr != '`')
	{
	  *bufptr++ = *strptr++;
	  continue;
	}
      //got a ascii code, we skip  the next 2 char.
      strptr++;
      strptr++;
    }//end copy loop;
  //we terminate ascii
  *bufptr++ = '`';
  *bufptr++ = '`';
  //terminate string
  *bufptr++ = '\0';
  //free string
  free_string(room->name);
  room->name = str_dup(buf);
  return TRUE;
}//end room_name_formant



/* color_rooms*/
/* only in db.c */
/* Viri */
void color_rooms()
{
  /* We run through ALL the rooms, coloring the names. */

  ROOM_INDEX_DATA* room;
  int i = 0;

  for (i = 0; i < MAX_KEY_HASH; i++)
    for (room = room_index_hash[i]; room != NULL; room = room->next)
      {
	if (room == NULL)
	  continue;

	if (!format_room_name(room, room->name))
	  {
	    log_string("ERROR IN ROOM NAME FORMAT");
	    nlogf("format_room_name returned error on room %d", room->vnum);
	  }
      }
}

void load_objprogs( FILE *fp )
{
    PROG_CODE *pOprog;

    if ( area_last == NULL )
    {
	bug( "Load_objprogs: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	sh_int vnum;
	char letter;

	letter		  = fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_objprogs: # not found.", 0 );
	    exit( 1 );
	}

	vnum		 = fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_prog_index( vnum, PRG_OPROG ) != NULL )
	{
	    bug( "Load_objprogs: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pOprog		= alloc_perm( sizeof(*pOprog) );
	pOprog->vnum  	= vnum;
	pOprog->code  	= fread_string( fp );
	if ( oprog_list == NULL )
	    oprog_list = pOprog;
	else
	{
	    pOprog->next = oprog_list;
	    oprog_list 	= pOprog;
	}
	top_oprog_index++;
    }
    return;
}

void load_roomprogs( FILE *fp )
{
    PROG_CODE *pRprog;

    if ( area_last == NULL )
    {
	bug( "Load_roomprogs: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	sh_int vnum;
	char letter;

	letter		  = fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_roomprogs: # not found.", 0 );
	    exit( 1 );
	}

	vnum		 = fread_number( fp );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_prog_index( vnum, PRG_RPROG ) != NULL )
	{
	    bug( "Load_roomprogs: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pRprog		= alloc_perm( sizeof(*pRprog) );
	pRprog->vnum  	= vnum;
	pRprog->code  	= fread_string( fp );
	if ( rprog_list == NULL )
	    rprog_list = pRprog;
	else
	{
	    pRprog->next = rprog_list;
	    rprog_list 	= pRprog;
	}
	top_rprog_index++;
    }
    return;
}

void load_traps( FILE *fp ){
  TRAP_INDEX_DATA *pTrap;
  char* temp;

  if ( area_last == NULL ){
    bug( "Load_traps: no #AREA seen yet.", 0 );
    exit( 1 );
  }

  for ( ; ; ){
    sh_int vnum;
    char letter;

    letter		  = fread_letter( fp );
    if ( letter != '#' ){
      bug( "Load_traps: # not found.", 0 );
      exit( 1 );
    }

    vnum		 = fread_number( fp );
    if ( vnum <= 0 )
      break;

    fBootDb = FALSE;
    if ( get_trap_index( vnum ) != NULL ){
      bug( "Load_traps: vnum %d duplicated.", vnum );
      exit( 1 );
    }
    fBootDb = TRUE;

    pTrap		= new_trap_index();
    pTrap->area		= area_last;
    pTrap->vnum  	= vnum;
    /* read in commons */
    pTrap->name	      = fread_string( fp );
    pTrap->echo	      = fread_string( fp );
    pTrap->oEcho      = fread_string( fp );
    if ( (pTrap->type = trap_lookup( fread_word( fp ) )) < 1)
      bug("load_traps: Unknown trap type for vnum %d.", vnum);
    pTrap->level = fread_number( fp );
    pTrap->flags = fread_flag( fp );

    /* read in type based stuff */
    switch (pTrap->type){
    default:
    case TTYPE_DUMMY:
      pTrap->value[0] = fread_number( fp );
      pTrap->value[1] = fread_number( fp );
      pTrap->value[2] = fread_number( fp );
      pTrap->value[3] = fread_number( fp );
      pTrap->value[4] = fread_number( fp );
      break;
    case TTYPE_DAMAGE:
      pTrap->value[0] = attack_lookup( fread_word( fp ));
      if (pTrap->value[0] < 0){
	bug("Load_traps: value[0] of TTYPE_DAMAGE vnum %d was incorrect.", vnum);
	pTrap->value[0] = 1;
      }
      pTrap->value[1] = fread_number( fp );
      pTrap->value[2] = fread_number( fp );
      pTrap->value[3] = fread_number( fp );
      pTrap->value[4] = fread_number( fp );
      break;
    case TTYPE_XDAMAGE:
      temp = fread_string( fp );
      pTrap->value[0] = skill_lookup(temp);
      free_string( temp );
      if (pTrap->value[0] < 0){
	bug("Load_traps: value[0] of TTYPE_XDAMAGE vnum %d was incorrect.", vnum);
	pTrap->value[0] = 1;
      }
      pTrap->value[1] = fread_number( fp );
      pTrap->value[2] = fread_number( fp );
      pTrap->value[3] = fread_number( fp );
      pTrap->value[4] = fread_number( fp );
      break;
    case TTYPE_SPELL:
      temp = fread_string( fp );
      pTrap->value[0] = skill_lookup(temp);
      if (pTrap->value[0] < 0){
	bug("Load_traps: value[0] of TTYPE_SPELL vnum %d was incorrect.", vnum);
	pTrap->value[0] = 1;
      }
      pTrap->value[1] = fread_number( fp );
      temp = fread_string( fp );
      pTrap->value[2] = skill_lookup(temp);
      free_string( temp );
      if (pTrap->value[2] < 0){
	bug("Load_traps: value[2] of TTYPE_SPELL vnum %d was incorrect.", vnum);
	pTrap->value[2] = 1;
      }
      pTrap->value[3] = fread_number( fp );
      pTrap->value[4] = fread_number( fp );
      break;
    case TTYPE_MOB:
      pTrap->value[0] = fread_number( fp );
      pTrap->value[1] = fread_number( fp );
      pTrap->value[2] = fread_number( fp );
      pTrap->value[3] = fread_number( fp );
      pTrap->value[4] = fread_number( fp );
      break;
    }
    if ( trap_index_list == NULL )
      trap_index_list = pTrap;
    else{
      pTrap->next = trap_index_list;
      trap_index_list = pTrap;
    }
  }
  return;
}

void fix_mobprogs( void )
{
    MOB_INDEX_DATA *pMobIndex;
    PROG_LIST        *list;
    PROG_CODE        *prog;
    int iHash;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pMobIndex   = mob_index_hash[iHash];
	      pMobIndex   != NULL;
	      pMobIndex   = pMobIndex->next )
	{
	    for( list = pMobIndex->mprogs; list != NULL; list = list->next )
	    {
		if ( ( prog = get_prog_index( list->vnum, PRG_MPROG ) ) != NULL )
		    list->code = prog->code;
		else
		{
		    bug( "Fix_mobprogs: code vnum %d not found.", list->vnum );
		    exit( 1 );
		}
	    }
	}
    }
}

void fix_objprogs( void )
{
    OBJ_INDEX_DATA *pObjIndex;
    PROG_LIST        *list;
    PROG_CODE        *prog;
    int iHash;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pObjIndex   = obj_index_hash[iHash];
	      pObjIndex   != NULL;
	      pObjIndex   = pObjIndex->next )
	{
	    for( list = pObjIndex->oprogs; list != NULL; list = list->next )
	    {
		if ( ( prog = get_prog_index( list->vnum, PRG_OPROG ) ) != NULL )
		    list->code = prog->code;
		else
		{
		    bug( "Fix_objprogs: code vnum %d not found.", list->vnum );
		    exit( 1 );
		}
	    }
	}
    }
}

void fix_roomprogs( void )
{
    ROOM_INDEX_DATA *pRoomIndex;
    PROG_LIST        *list;
    PROG_CODE        *prog;
    int iHash;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
	for ( pRoomIndex   = room_index_hash[iHash];
	      pRoomIndex   != NULL;
	      pRoomIndex   = pRoomIndex->next )
	{
	    for( list = pRoomIndex->rprogs; list != NULL; list = list->next )
	    {
		if ( ( prog = get_prog_index( list->vnum, PRG_RPROG ) ) != NULL )
		    list->code = prog->code;
		else
		{
		    bug( "Fix_roomprogs: code vnum %d not found.", list->vnum );
		    exit( 1 );
		}
	    }
	}
    }
}


//prints a text entry for an object
void dump_object( FILE* fp, OBJ_INDEX_DATA* obj ){
  AFFECT_DATA* paf;
  int i;
  char buf[MIL];

//vnum rare unique type level material extra_flags weight keywords area  name
  fprintf(fp, "%d \"%s\" \"%s\" %d %d \"%s\" \"%s\" \"%s\" \"%s\"",
	  obj->vnum,
	  IS_SET(obj->wear_flags, ITEM_RARE) ? "R" : IS_SET(obj->wear_flags, ITEM_UNIQUE) ? "U" : "",
	  item_name(obj->item_type),
	  obj->level,
	  obj->weight,
	  obj->material,
	  extra_bit_name(obj->extra_flags),
	  obj->area->name,
	  obj->short_descr);

  //print item dependent info
  fprintf(fp, "\"" );

  switch ( obj->item_type ){
  case ITEM_SOCKET:
    if (IS_SOC_STAT(obj, SOCKET_ARMOR)
	&& IS_SOC_STAT(obj, SOCKET_WEAPON))
      fprintf(fp, "Ar/Wep");
    else if (IS_SOC_STAT(obj, SOCKET_ARMOR))
      fprintf(fp, "Ar    ");
    else if (IS_SOC_STAT(obj, SOCKET_WEAPON))
      fprintf(fp, "   Wep");
    else
      fprintf(fp, "   Wep");
    if (obj->value[1]){
      sprintf(buf, " Extra: %s",
	      flag_string( extra_flags,  obj->value[1] ) );
      fprintf(fp, "%s", buf);
    }
    if (obj->value[4]){
      sprintf(buf, " Flags: %s\n",
	      flag_string( weapon_type2,  obj->value[4] ) );
      fprintf(fp, "%s", buf);
    }
    break;
  case ITEM_SCROLL:
  case ITEM_ARTIFACT:
  case ITEM_POTION:
  case ITEM_PILL:
    sprintf( buf, "Level %d: ", obj->value[0] );
    for ( i = 1; i <= 4; i++ ){
      if ( obj->value[i] >= 0
	   && obj->value[i] < MAX_SKILL
	   && skill_table[obj->value[i]].name != NULL){
	strcat( buf, " '" );
	strcat( buf, skill_table[obj->value[i]].name );
	strcat( buf, "' " );
      }
    }
    fprintf(fp, "%s", buf);
    break;
  case ITEM_WAND:
  case ITEM_STAFF:
    if (skill_table[obj->value[3]].name == NULL)
      break;
    sprintf( buf, "%dx%dLvl ", obj->value[2], obj->value[0] );
    if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL ){
      strcat( buf, " '" );
      strcat( buf, skill_table[obj->value[3]].name );
      strcat( buf, "' " );
    }
    fprintf(fp, "%s", buf);
    break;
  case ITEM_DRINK_CON:
    sprintf(buf,"%s %s ",
	    liq_table[obj->value[2]].liq_color,
	    liq_table[obj->value[2]].liq_name);
    fprintf(fp, "%s", buf);
    break;
  case ITEM_CONTAINER:
    sprintf(buf,"%d at %d# ",
	    obj->value[3], obj->value[0]);
    fprintf(fp, "%s", buf);
    if (obj->value[4] != 100){
      sprintf(buf," %d%%", obj->value[4]);
      fprintf(fp, "%s", buf);
    }
    break;
  case ITEM_WEAPON:
    buf[0] = 0;
    if (IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS) )
      strcat(buf, "2h");
    switch (obj->value[0]){
    case(WEAPON_EXOTIC) : strcat(buf,"Ex");       break;
    case(WEAPON_SWORD)  : strcat(buf,"Sw");        break;
    case(WEAPON_DAGGER) : strcat(buf,"Da");       break;
    case(WEAPON_SPEAR)  : strcat(buf,"Sp");        break;
    case(WEAPON_MACE)   : strcat(buf,"Ma");    break;
    case(WEAPON_AXE)    : strcat(buf,"Ax");          break;
    case(WEAPON_FLAIL)  : strcat(buf,"Fl");        break;
    case(WEAPON_WHIP)   : strcat(buf,"Wh");         break;
    case(WEAPON_POLEARM): strcat(buf,"Po");      break;
    case(WEAPON_STAFF)  : strcat(buf,"St");        break;
    default             : strcat(buf,"??");      break;
    }
    fprintf(fp, "%s ", buf);
    if (obj->new_format){
      sprintf(buf,"%dd%d(%d)", obj->value[1],obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2);
    }
    else{
      sprintf( buf, "%d-%d(%d)", obj->value[1], obj->value[2], ( obj->value[1] + obj->value[2] ) / 2 );
    }
    fprintf(fp, "%s", buf );

    fprintf(fp, " [%s] ",
	    (obj->value[3] > 0 && obj->value[3] < MAX_DAMAGE_MESSAGE) ? attack_table[obj->value[3]].noun : "undefined");
    if (obj->value[4])
      fprintf(fp, "Flags: %s", weapon_bit_name(obj->value[4]));
    break;
  case ITEM_ARMOR:
    sprintf( buf, "AC %d %d %d %d",
	     obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
    break;
  case ITEM_THROW:
    if (obj->new_format)
      sprintf(buf,"%dd%d (%d)",
	      obj->value[1],obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2);
    else
      sprintf( buf, "%d-%d (%d)",
	       obj->value[1], obj->value[2], ( obj->value[1] + obj->value[2] ) / 2 );
    fprintf(fp,  "%s", buf );
    break;
  case ITEM_RANGED:
    if (obj->value[3] == 0)
      sprintf(buf, "%s ",
	      flag_string(projectile_type, obj->value[0]));
    else{
      OBJ_INDEX_DATA* ammo = get_obj_index( obj->value[3] );
      sprintf(buf, "%s ",
	      ammo == NULL ? "NOT FOUND" : ammo->short_descr);
    }
    fprintf(fp, "%s", buf);
    sprintf(buf, "Ammo %d Rate %d ",
	      obj->value[1], obj->value[2]);
    fprintf(fp, "%s", buf);
    sprintf(buf, "Flags: %s ",flag_string(ranged_type, obj->value[4]));
    fprintf(fp, "%s", buf);
    break;
  case ITEM_PROJECTILE:
    sprintf(buf, "%s",
	    flag_string(projectile_type, obj->value[0]));
    fprintf(fp, "%s", buf);
    if (obj->new_format)
      sprintf(buf, "%dd%d (%d) ",
	      obj->value[1],obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2);
    else
      sprintf( buf, "%d-%d (%d) ",
	       obj->value[1], obj->value[2], ( obj->value[1] + obj->value[2] ) / 2 );
    fprintf(fp, "%s", buf);
  }
  fprintf(fp, "\" " );
  fprintf(fp, "\"" );
  //now affects
  for ( paf = obj->affected; paf != NULL; paf = paf->next ){
    if ( paf->location != APPLY_NONE
	 && (paf->location < APPLY_O_COND || paf->where == TO_SKILL)
	 && paf->modifier != 0 ){
      sprintf( buf, "%s by %d.",
	       (paf->where == TO_SKILL ? skill_table[paf->location].name : affect_loc_name( paf->location )),
	       paf->modifier );
      fprintf( fp, "%-70.70s", buf );
    }
    if (paf->bitvector){
      switch(paf->where){
      case TO_AFFECTS: sprintf(buf,"Adds %s affect.", affect_bit_name(paf->bitvector)); break;
      case TO_OBJECT:  sprintf(buf,"Adds %s object flag.", extra_bit_name(paf->bitvector)); break;
      case TO_WEAPON:  sprintf(buf,"Adds %s weapon flags.", weapon_bit_name(paf->bitvector)); break;
      case TO_IMMUNE:  sprintf(buf,"Adds immunity to %s.", imm_bit_name(paf->bitvector)); break;
      case TO_RESIST:  sprintf(buf,"Adds resistance to %s.", imm_bit_name(paf->bitvector)); break;
      case TO_VULN:    sprintf(buf,"Adds vulnerability to %s.", imm_bit_name(paf->bitvector)); break;
      case TO_SKILL:   /* handled above */ break;
      default:         sprintf(buf,"Unknown bit %d: %d", paf->where,paf->bitvector); break;
      }
      fprintf( fp, "%-70.70s", buf );
    }
  }
  fprintf(fp, "\"" );
  fprintf(fp, "\n" );
}

