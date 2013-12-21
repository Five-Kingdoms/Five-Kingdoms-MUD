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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "alias.h"

char    last_command[MSL];

extern		int              nAllocString;
extern		int              sAllocString;
extern		int		 top_oprog_index;
extern		int		 top_rprog_index;
extern		bool		sec_check(CHAR_DATA* ch);
extern void dreamprobe_cast( CHAR_DATA* victim, char* msg );
int                     nAllocPerm;
int                     sAllocPerm;

#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2

bool				fLogAll		= FALSE;
bool				fLogLoop	= FALSE;

const	struct	cmd_type	cmd_table	[] =
{
/* (log) = when/how the command is logged
   show = is this shown on "commands"
   mob = what mobs can do
   charm = what players can do while charmed
   order = what players can order charmed people to do
   hide = what strips hide
   sneak = what strips sneak
   camo = what strips camoflauge
   shadow = what strips shadowform
   quiet = what strips quiet movement
   switch = things you can do switched
   panther = things mind linked mobs can do
   morph = things a morphed vampire can do
   meditate = things that you can do while meditating
   0 is false (you can't do this..  this doesn't strip affects)
   1 is true (you can do this..  this does strip affects)
   for numbers: (log), (show)      mob, charm, order, hide, sneak, camo, shadow, quiet, switch, panther, morph, meditate*/
    /* Common movement commands. */
    { "north",          do_north,          POS_STANDING,    0,  LOG_NEVER, 0,  1,1,1,0,0,0,0,0,1,1,1 },
    { "east",           do_east,           POS_STANDING,    0,  LOG_NEVER, 0,  1,1,1,0,0,0,0,0,1,1,1 },
    { "south",          do_south,          POS_STANDING,    0,  LOG_NEVER, 0,  1,1,1,0,0,0,0,0,1,1,1 },
    { "west",           do_west,           POS_STANDING,    0,  LOG_NEVER, 0,  1,1,1,0,0,0,0,0,1,1,1 },
    { "up",             do_up,             POS_STANDING,    0,  LOG_NEVER, 0,  1,1,1,0,0,0,0,0,1,1,1 },
    { "down",           do_down,           POS_STANDING,    0,  LOG_NEVER, 0,  1,1,1,0,0,0,0,0,1,1,1 },
    /* Other common commands placed here so abbreviations work. */
    { "at",             do_at,             POS_DEAD,       L6,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,0,0 },
    { "cast",           do_cast,           POS_FIGHTING,    0,  LOG_NORMAL, 1, 1,0,0,1,0,1,0,0,0,0,1 },
    { "commune",        do_commune,        POS_FIGHTING,    0,  LOG_NORMAL, 1, 1,0,0,1,0,1,0,0,0,0,1 },
    { "cancel",         do_cancel,         POS_FIGHTING,    0,  LOG_NORMAL, 1, 1,0,0,1,0,1,0,0,0,0,1 },

    { "sing",           do_sing,           POS_FIGHTING,    0,  LOG_NORMAL, 1, 1,0,0,1,0,1,0,0,0,0,1 },
    
    { "buy",            do_buy,            POS_RESTING,     0,  LOG_NORMAL, 1, 0,0,0,1,0,1,0,0,0,0,1 },
    { "channels",       do_channels,       POS_DEAD,        0,  LOG_NORMAL, 1, 0,0,0,0,0,0,0,0,0,0,0 },
    { "exits",          do_exits,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "get",            do_get,            POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "goto",           do_goto,           POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "beckon",         do_beckon,         POS_MEDITATE,    0,  LOG_NORMAL, 1, 1,0,0,1,0,1,0,0,0,0,1 },
    { "group",          do_group,          POS_MEDITATE,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,0 },
    { "look",           do_look,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "cabal",          do_cabal,          POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "hit",            do_kill,           POS_FIGHTING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "inventory",      do_inventory,      POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,0 },
    { "sheathe",        do_sheath,	   POS_RESTING ,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,0 },
    { "draw",		do_draw,	   POS_FIGHTING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,0 },
    { "clan",           do_clan,	   POS_DEAD,        0,  LOG_ALWAYS, 1, 1,0,0,0,0,0,0,0,0,0,1 },
    { "kill",           do_kill,           POS_FIGHTING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "cb",             do_cabaltalk,      POS_MEDITATE,    0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,1 },
    { "coup",           do_coup,           POS_SLEEPING,    0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "cabal",          do_cabaltalk,      POS_MEDITATE,    0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,1 },
    { "cc",		do_clantalk,       POS_MEDITATE,    0,  LOG_NORMAL, 1, 0,1,1,1,0,1,0,0,0,1,1 },
    { "newbiechat",     do_newbiechat,     POS_DEAD,        0,  LOG_NORMAL, 1, 0,0,1,1,0,1,0,0,0,1,0 },
    { "order",          do_order,          POS_RESTING,     0,  LOG_NORMAL, 1, 0,0,0,1,0,1,1,0,0,1,1 },
    { "practice",       do_practice,       POS_SLEEPING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,1 },
    { "psalms",		do_psalm,       POS_SLEEPING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,1 },
    { "rest",           do_rest,           POS_MEDITATE,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,0 },
    { "rehearse",       do_rehearse,       POS_SLEEPING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,1 },
    { "sit",            do_sit,            POS_MEDITATE,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,0 },
    { "sockets",        do_sockets,        POS_DEAD,       L5,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "matchip",        do_matchip,        POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "stand",          do_stand,          POS_MEDITATE,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "tell",           do_tell,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,1 },
    { "telepathy",      do_telepathy,      POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,1 },
    { "unlock",         do_unlock,         POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,1 },
    { "wield",          do_wear,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,0,1,0,1,0,0,0,0,1 },
    { "wizhelp",        do_wizhelp,        POS_DEAD,       IM,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "for",            do_for,            POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    /* Informational commands. */
    { "affects",        do_affects,        POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,0 },
    { "areas",          do_areas,          POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "bug",            do_bug,            POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,0,0,0,0,0,0,0,1,0 },
    { "changes",        do_changes,        POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "commands",       do_commands,       POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "compare",        do_compare,        POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,1 },
    { "consider",       do_consider,       POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,1,0,1,1,1 },
    { "conceal",        do_conceal,        POS_STANDING,    0,  LOG_NORMAL, 1, 0, 0,0,0,0,0,0,0,0,0,0 },
    { "abduct",         do_abduction,      POS_STANDING,    0,  LOG_NORMAL, 1, 0, 0,0,0,0,0,0,0,0,0,0 },
    { "unseen",         do_unseen,         POS_STANDING,    0,  LOG_NORMAL, 1, 0, 0,0,0,0,0,0,0,1,1,1 },
    { "analyze",        do_analyze,        POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,1,0,1,1,1 },
    { "count",          do_count,          POS_DEAD,	      0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "equipment",      do_equipment,      POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,0 },
    { "examine",        do_examine,        POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,0,0,0,1,1,1 },
    { "help",           do_help,           POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,0 },
    { "imotd",          do_imotd,          POS_DEAD,        IM, LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "motd",           do_motd,           POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "mob",            do_mob,           POS_DEAD,        ML,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "obj",            do_obj,           POS_DEAD,        ML,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "room",           do_room,           POS_DEAD,        ML,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "news",           do_news,           POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "read",           do_look,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "report",         do_report,         POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "score",          do_score,          POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "skills",         do_skills,         POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "socials",        do_socials,        POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,0 },
    { "show",           do_show,           POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "spells",         do_spells,         POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "memorize",       do_memorize,       POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "forget",         do_forget,       POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "songs",          do_songs,          POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "time",           do_time,           POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,0 },
    { "typo",           do_typo,           POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "weather",        do_weather,        POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "who",            do_who,            POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "whois",          do_whois,          POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "whios",          do_whois,          POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "bio",            do_bio,            POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "info",           do_bio,            POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "wizlist",        do_wizlist,        POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "worth",          do_worth,          POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "reputation",     do_reputation,     POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "religion",       do_deity,          POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "accept",         do_accept,          POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    /* Configuration commands. */
    { "autolist",       do_autolist,       POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autoshow",       do_show,           POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "autoscore",      do_autoscore,      POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "autoassist",     do_autoassist,     POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autoaim",        do_autoaim,        POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autocabal",      do_autocabal,      POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autoequip",      do_autoequip,      POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autoexp",        do_autoexp,        POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autoalias",      do_autoalias,        POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autosex", 	do_autosex,	   POS_DEAD,	    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autosheath", 	do_autosheath,	   POS_DEAD,	    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autoexit",       do_autoexit,       POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autogold",       do_autogold,       POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autoloot",       do_autoloot,       POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autosac",        do_autosac,        POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autosplit",      do_autosplit,      POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "autofire",       do_autofire,       POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "mercy",		do_mercy,	   POS_DEAD,	    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "brief",          do_brief,          POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,0,0,0,0,0,0,0,1,0 },
    { "compact",        do_compact,        POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,0,0,0,0,0,0,0,1,0 },
    { "description",    do_description,    POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,0,0 },
    { "delet",          do_delet,          POS_DEAD,        0,  LOG_ALWAYS, 0, 0,1,0,0,0,0,0,0,0,0,0 },
    { "delete",         do_delete,         POS_STANDING,    0,  LOG_ALWAYS, 1, 0,1,0,0,0,0,0,0,0,0,1 },
    { "nofollow",       do_nofollow,       POS_DEAD,        0,  LOG_NORMAL, 1, 1,0,0,0,0,0,0,0,0,1,0 },
    { "nosummon",       do_nosummon,       POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "outfit",         do_outfit,         POS_RESTING,     0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,0,1 },
    { "password",       do_password,       POS_DEAD,        0,  LOG_NEVER,  1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "delpassword",    do_delpass,    POS_DEAD,        0,  LOG_NEVER,  1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "prompt",         do_prompt,         POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,0,0,0,0,0,0,0,1,0 },
    { "scroll",         do_scroll,         POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,0,0,0,0,0,0,1,1,0 },
    { "wimpy",          do_wimpy,          POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,0,0,0,0,0,0,0,1,0 },
    /* Communication commands. */
    { "afk",            do_afk,            POS_SLEEPING,   L2,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "deaf",           do_deaf,           POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "act",            do_act,            POS_SLEEPING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,0,0,0,0,1,1 },
    { "emote",          do_emote,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,0,0,0,0,1,1 },
    { "pmote",          do_pmote,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,0,0,0,0,1,1 },
    { "gtell",          do_gtell,          POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,0 },
    { ";",              do_gtell,          POS_DEAD,        0,  LOG_NORMAL, 0, 1,1,1,1,0,1,0,0,0,1,0 },
    { "idea",           do_idea,           POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "application",    do_application,    POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "note",           do_note,           POS_SLEEPING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "print",         do_print,     POS_SLEEPING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "scribe",         do_write_tome,     POS_SLEEPING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "tome",           do_read_tome,      POS_SLEEPING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "approve",        do_approve,        POS_SLEEPING,    L8,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "reject",          do_reject,        POS_SLEEPING,    L8,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "quest",          do_quest,	   POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "forfeit",        do_forfeit,	   POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,1,1,0,1,0,0,0,1,0 },
    { "quiet",          do_quiet,          POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,0,0,0,0,0,0,0,1,0 },
    { "ignore",         do_ignore,         POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "reply",          do_reply,          POS_SLEEPING,    0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,1 },
    { "replay",         do_replay,         POS_SLEEPING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,1 },
    { "say",            do_say,            POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,1 },
    { "'",              do_say,            POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,1,1,0,1,0,0,0,1,1 },
    { "unread",         do_unread,         POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "yell",           do_yell,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,1 },
    /* Object manipulation commands. */
    { "brandish",       do_brandish,       POS_FIGHTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,0,1 },
    { "brandweapon",   do_brand_weapon,    POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,0,1 },
    { "close",          do_close,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "drink",          do_drink,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "drop",           do_drop,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "eat",            do_eat,            POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "smoke",          do_smoke,          POS_RESTING,     0,  LOG_NORMAL, 1, 0,1,1,1,0,1,0,0,1,1,1 },
    { "envenom",        do_envenom,        POS_STANDING,     0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "fill",           do_fill,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,0,1 },
    { "preserve",       do_preserve,	   POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,0,1 },
    { "give",           do_give,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "heal",           do_heal,           POS_RESTING,     0,  LOG_NORMAL, 1, 0,1,0,1,0,1,0,0,0,1,1 },
    { "service",        do_heal,           POS_RESTING,     0,  LOG_NORMAL, 1, 0,1,0,1,0,1,0,0,0,1,1 },
    { "hold",           do_wear,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,0,1,0,1,0,0,0,0,1 },
    { "list",           do_list,           POS_RESTING,     0,  LOG_NORMAL, 1, 0,1,1,1,0,1,0,0,0,0,1 },
    { "browse",         do_browse,         POS_RESTING,     0,  LOG_NORMAL, 1, 0,1,1,1,0,1,0,0,0,0,1 },
    { "lock",           do_lock,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,1 },
    { "open",           do_open,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "pick",           do_pick,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,0,1,0,1,0,0,0,0,1 },
    { "defuse",         do_defuse,         POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,0,1,0,1,0,0,0,0,1 },
    { "pour",           do_pour,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,0,1 },
    { "put",            do_put,            POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "quaff",          do_quaff,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,0,1,0,1,0,0,0,1,1 },
    { "recite",         do_recite,         POS_RESTING,     0,  LOG_NORMAL, 1, 1,0,0,1,0,1,0,0,0,0,1 },
    { "remove",         do_remove,         POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,0,1 },
    { "sell",           do_sell,           POS_RESTING,     0,  LOG_NORMAL, 1, 0,1,1,1,0,1,0,0,0,0,1 },
    { "take",           do_get,            POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "sacrifice",      do_sacrifice,      POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "value",          do_value,          POS_RESTING,     0,  LOG_NORMAL, 1, 0,1,1,1,0,1,0,0,0,0,1 },
    { "wear",           do_wear,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,0,1,0,1,0,0,0,0,1 },
    { "zap",            do_zap,            POS_FIGHTING,     0,  LOG_NORMAL, 1, 1,0,1,1,0,1,0,0,0,0,1 },
    { "use",            do_use,            POS_FIGHTING,     0,  LOG_NORMAL, 1, 1,0,1,1,0,1,0,0,0,0,1 },
    /* Combat commands. */
    { "backstab",       do_backstab,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "bash",           do_bash,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "critical",       do_critical,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "boulder",        do_boulder_throw,  POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "batter",         do_batter,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "rush",           do_armored_rush,   POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "execute",        do_forms,	  POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "bs",             do_backstab,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "berserk",        do_berserk,        POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "dirt",           do_dirt,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "corrupt",        do_corrupt,        POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "dreamwalk",      do_dreamwalk,      POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "lifedrain",      do_lifedrain,      POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "mold",		do_mold,	   POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "feign",          do_feign,          POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "focusedbash",    do_focused_bash,   POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "maul",           do_mantis_maul,    POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "fury",           do_fury,	   POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "disarm",         do_disarm,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "weaponcleave",   do_weapon_cleave,  POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "weaponlock",     do_weapon_lock,    POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "shieldcleave",   do_shield_cleave,  POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "shieldbash",     do_shield_bash,  POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "flee",           do_flee,           POS_FIGHTING,    0,  LOG_NORMAL, 1, 1,0,1,1,0,1,0,0,1,1,1 },
    { "kick",           do_kick,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "onslaught",      do_onslaught,      POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "shoulder",       do_shoulder_smash, POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "armorpierce",    do_armorpierce,	   POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "pommelsmash",    do_pommel_smash,   POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "murde",          do_murde,          POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,1,1,0,1,0,0,0,1,1 },
    { "murder",         do_murder,         POS_FIGHTING,    5,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,1 },
    { "rescue",         do_rescue,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,1,1,0,1,0,0,1,1,1 },
    { "trip",           do_trip,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "cutter",         do_cutter,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "pilfer",         do_pilfer,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "push",           do_push,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    /* Miscellaneous commands. */
    { "bounty",         do_bounty,         POS_DEAD,        0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "bounties",       do_top10,          POS_DEAD,        0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "top10",		do_top10,          POS_DEAD,        0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "hunt",           do_hunt,           POS_STANDING,    0,  LOG_NORMAL, 1, 1,0,0,0,0,0,0,0,0,1,1 },
    { "track",          do_track,          POS_STANDING,    0,  LOG_NORMAL, 1, 1,0,0,0,0,0,0,0,0,1,1 },
    { "pack",           do_pack_call,      POS_STANDING,    0,  LOG_NORMAL, 1, 1,0,0,0,0,0,0,0,0,1,1 },
    { "rapid",          do_rapid_fire,     POS_STANDING,    0,  LOG_NORMAL, 1, 1,0,0,0,0,0,0,0,0,1,1 },
    { "follow",         do_follow,         POS_RESTING,     0,  LOG_NORMAL, 1, 1,0,0,0,0,0,0,0,0,1,1 },
    { "gain",           do_gain,           POS_STANDING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,1 },
    { "hide",           do_hide,           POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,0,0,0,1,0,0,0,1,1 },
    { "qui",            do_qui,            POS_DEAD,        0,  LOG_NORMAL, 0, 0,0,0,0,0,0,0,0,0,1,1 },
    { "quit",           do_quit,           POS_DEAD,        0,  LOG_NORMAL, 0, 0,0,0,0,0,0,0,0,0,1,1 },
    { "recall",         do_recall,         POS_FIGHTING,    0,  LOG_NORMAL, 1, 0,1,0,1,0,1,0,0,0,1,1 },
    { "/",              do_recall,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 0,1,0,1,0,1,0,0,0,1,1 },
    { "save",           do_save,           POS_DEAD,        0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,0 },
    { "meditate",       do_meditate,       POS_RESTING,     0,  LOG_NORMAL, 1, 0,0,0,0,0,0,0,0,0,0,0 },
    { "sleep",          do_sleep,          POS_MEDITATE,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,1,0,1,1,1 },
    { "sneak",          do_sneak,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,1,1 },
    { "disguise",       do_disguise,       POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,1,1 },
    { "study",          do_study,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,1,1 },
    { "split",          do_split,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,1 },
    { "approach",       do_approach,       POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,0,1,0,1,0,0,0,0,1 },
    { "traps",          do_traps,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,0,0,0,1,0,0,0,0,1 },
    { "steal",          do_steal,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,0,0,0,1,0,0,0,0,1 },
    { "plant",          do_plant,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,0,1,1,1,0,0,0,0,1 },
    { "pry",            do_pry,            POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,0,1,1,1,0,0,0,0,1 },
    { "train",          do_train,          POS_RESTING,     0,  LOG_NORMAL, 1, 0,1,0,1,0,1,1,0,0,1,1 },
    { "visible",        do_visible,        POS_SLEEPING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "revert",         do_revert,         POS_SLEEPING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,1 },
    { "wake",           do_wake,           POS_SLEEPING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "where",          do_where,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "law",		do_law,		   POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "crimes",		do_crimes,	   POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "surrender",	do_surrender,	   POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "dice",		do_dice,	   POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    { "witchcompass",	do_w_compass,	   POS_FIGHTING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,1,1,1 },
    /* Immortal commands. */
    { "advance",        do_advance,        POS_DEAD,       ML,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },

    /*  ADVANCE SELF */
/*  { "advself",        do_advself,        POS_DEAD,       0,   LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },*/

    { "dump",           do_dump,           POS_DEAD,       ML,  LOG_ALWAYS, 0, 1,1,1,0,0,0,0,0,0,1,0 },
    { "trust",          do_trust,          POS_DEAD,       ML,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "allow",          do_allow,          POS_DEAD,       L2,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "ban",            do_ban,            POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "forsake",        do_doof,           POS_DEAD,       L2,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "coding",         do_coding,         POS_DEAD,       L2,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "royal",          do_royal,          POS_DEAD,       0,   LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,1,0 },
    { "escort",         do_escort,         POS_DEAD,       0,   LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,1,0 },
    //    { "plag",           do_plag,           POS_DEAD,       L2,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "deny",           do_deny,           POS_DEAD,       L1,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "disconnect",     do_disconnect,     POS_DEAD,       L3,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "flag",           do_flag,           POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "freeze",         do_freeze,         POS_DEAD,       L4,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "brown",          do_brownie,        POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "grant",          do_grant,          POS_DEAD,       ML,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "permban",        do_permban,        POS_DEAD,       L2,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "protect",        do_protect,        POS_DEAD,       L1,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "reboo",          do_reboo,          POS_DEAD,       L1,  LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,1,0 },
    { "reboot",         do_reboot,         POS_DEAD,       L1,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "set",            do_set,            POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "addpsalm",       do_addpsalm,       POS_DEAD,       ML,  LOG_ALWAYS, 0, 1,1,0,0,0,0,0,0,0,0,0 },
    { "rempsalm",       do_rempsalm,       POS_DEAD,       ML,  LOG_ALWAYS, 0, 1,1,0,0,0,0,0,0,0,0,0 },
    { "setquest",       do_setquest,       POS_DEAD,       ML,  LOG_ALWAYS, 0, 1,1,0,0,0,0,0,0,0,0,0 },
    { "tempquest",      do_tempquest,      POS_DEAD,       L8,  LOG_ALWAYS, 0, 1,1,0,0,0,0,0,0,0,0,0 },
    { "permquest",      do_permquest,      POS_DEAD,       L8,  LOG_ALWAYS, 0, 1,1,0,0,0,0,0,0,0,0,0 },
    { "inviquest",      do_inviquest,      POS_DEAD,       L8,  LOG_ALWAYS, 0, 1,1,0,0,0,0,0,0,0,0,0 },
    { "remquest",       do_remquest,      POS_DEAD,        L8,  LOG_ALWAYS, 0, 1,1,0,0,0,0,0,0,0,0,0 },
    { "setquest2",      do_setquest2,      POS_DEAD,     ML+1,  LOG_ALWAYS, 0, 1,1,0,0,0,0,0,0,0,0,0 },
    { "ospell",         do_ospell,         POS_DEAD,       ML,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "shutdow",        do_shutdow,        POS_DEAD,       L1,  LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,1,0 },
    { "shutdown",       do_shutdown,       POS_DEAD,       L1,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "wizlock",        do_wizlock,        POS_DEAD,       L2,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "force",          do_force,          POS_DEAD,       L7,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "load",           do_load,           POS_DEAD,       L4,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "newlock",        do_newlock,        POS_DEAD,       L4,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "nochannels",     do_nochannels,     POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "noemote",        do_noemote,        POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "noyel",          do_noyel,          POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "noyell",         do_noyell,         POS_DEAD,        0,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "notell",         do_notell,         POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "pardon",         do_pardon,         POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "purge",          do_purge,          POS_DEAD,       L4,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "restore",        do_restore,        POS_DEAD,       L4,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "sla",            do_sla,            POS_DEAD,       L3,  LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,1,0 },
    { "slay",           do_slay,           POS_DEAD,       L3,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "rip",            do_sever,          POS_DEAD,       L3,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "teleport",       do_transfer,       POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "transfer",       do_transfer,       POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "gteleport",      do_gtransfer,      POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "gtransfer",      do_gtransfer,      POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "untransfer",     do_untransfer,     POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "gecho",          do_echo,           POS_DEAD,       L5,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "holylight",      do_holylight,      POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "losereply",      do_losereply,      POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },    
    { "hearall",        do_hearall,        POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "incognito",      do_incognito,      POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "invis",          do_invis,          POS_DEAD,       L8,  LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,1,0 },
    { "log",            do_log,            POS_DEAD,       L1,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "memory",         do_memory,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "rwhere",         do_rwhere,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "mwhere",         do_mwhere,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "twhere",         do_twhere,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "owhere",         do_owhere,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "rfind",          do_rfind,          POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "profind",        do_profind,        POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "peace",          do_peace,          POS_DEAD,       L5,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "penalty",        do_penalty,        POS_DEAD,       L8,  LOG_NORMAL, 1, 0,1,1,0,0,0,0,0,0,1,0 },
    { "echo",           do_recho,          POS_DEAD,       L6,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "zecho",          do_zecho,          POS_DEAD,       L6,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "return",         do_return,         POS_DEAD,       L6,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "snoop",          do_snoop,          POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "stat",           do_stat,           POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "raid",           do_raid,           POS_DEAD,        0,  LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,1,0 },
    //    { "build",          do_build,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "switch",         do_switch,         POS_DEAD,       L6,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "wizinvis",       do_invis,          POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "vnum",           do_vnum,           POS_DEAD,       L4,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "clone",          do_clone,          POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "wiznet",         do_wiznet,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { ":",              do_immtalk,        POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,0,0 },
    { "immtalk",        do_immtalk,        POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "smote",          do_smote,          POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "prefix",         do_prefix,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "noquit",         do_noaffquit,      POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "noaffquit",      do_noaffquit,      POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "setalarm",       do_set_alarm,      POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "broadcast",      do_broadcast,      POS_DEAD,       IM,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "auction",	do_auction,	   POS_DEAD,        0,  LOG_NORMAL, 1, 0,0,1,1,0,1,0,0,0,1,0 },
    /* Mob programs. */
    { "mpasound",       do_mpasound,       POS_DEAD,     ML+1,  LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,0,0 },
    { "mpjunk",         do_mpjunk,         POS_DEAD,     ML+1,  LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,0,0 },
    { "disappear",      do_disappear,      POS_DEAD,     ML+1,  LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,0,0 },
    /* New stuff */
    { "color",          do_color,          POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,0,0,0,0,0,0,0,1,0 },
    { "scan",           do_scan,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "scout",          do_scout,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,0,1,1 },
    { "mlevel",         do_mlevel,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "olevel",         do_olevel,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "[",              do_imptalk,        POS_DEAD,       L1,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "imptalk",        do_imptalk,        POS_DEAD,       L1,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "pray",           do_pray,           POS_SLEEPING,    0,  LOG_NORMAL, 1, 1,1,0,0,0,0,0,0,0,1,1 },
    { "mpstat",         do_mpstat,         POS_DEAD,       L1,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "bringon",        do_bringon,        POS_DEAD,       L4,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "cwhere",         do_cwhere,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "chown",          do_chown,          POS_DEAD,       L4,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "rename",         do_rename,         POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "surname",        do_surname,        POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "last",           do_last,           POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,0,0,0,0,0,0,0,1,0 },
    { "lastname",       do_lastname,       POS_DEAD,        0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,1,0 },
    { "dual",           do_second,         POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "repop",          do_repop,          POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "exlist",         do_exlist,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "noxlist",       do_noxlist,         POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "vlist",          do_vlist,          POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "tattoo",         do_tattoo,         POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "edit",           do_olc,            POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "opedit",         do_opedit,      POS_DEAD,	   L6,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "rpedit",         do_rpedit,      POS_DEAD,	   L6,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "opdump",		do_opdump,	POS_DEAD,	   L1,  LOG_NEVER,  1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "opstat",		do_opstat,	POS_DEAD,	   L1,  LOG_NEVER,  1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "rpdump",		do_rpdump,	POS_DEAD,	   L1,  LOG_NEVER,  1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "rpstat",		do_rpstat,	POS_DEAD,	   L1,  LOG_NEVER,  1, 1,1,1,0,0,0,0,0,0,1,0 },

    { "asave",          do_asave,          POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "alist",          do_alist,          POS_DEAD,       L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "resets",         do_resets,         POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "status",         do_status,         POS_DEAD,        0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "smite",          do_smite,          POS_DEAD,       L3,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "tick",		do_tick,           POS_DEAD,    	  L1,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "anum",           do_anum,           POS_DEAD,       L4,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "stash",          do_stash,          POS_STANDING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,1 },
    { "retrieve",       do_retrieve,       POS_STANDING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,1 },
    { "withdraw",       do_withdraw,       POS_STANDING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,1 },
    { "deposit",        do_deposit,        POS_STANDING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,1 },
    { "balance",        do_balance,        POS_STANDING,    0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,1 },
    { "request",        do_request,        POS_STANDING,    0,  LOG_NORMAL, 1, 1,1,0,1,0,1,1,0,0,0,1 },
    { "conjure",        do_conjure,        POS_STANDING,    0,  LOG_NORMAL, 1, 1,1,0,1,0,1,1,0,0,0,1 },
    { "chant",		do_chant,	   POS_STANDING,    0,  LOG_NORMAL, 1, 1,1,0,1,0,1,1,0,0,0,1 },
    { "interrupt",	do_interrupt,	   POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,0,1,0,1,1,0,0,0,1 },
    { "crashus",        do_crashus,        POS_DEAD,       ML+1,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,0,0 },
    { "interest",       do_interest,       POS_DEAD,       ML,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "affstrip",       do_affstrip,       POS_DEAD,       ML,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "songstrip",      do_songstrip,      POS_DEAD,       ML,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "inrange",        do_in_range,       POS_DEAD,       ML,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "identify",       do_identify,       POS_RESTING,    L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "star_seer",      do_star_seer,      POS_RESTING,    ML+1,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "obelisk_seer",   do_obelisk_seer,   POS_RESTING,    ML+1,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "random",         do_random,         POS_RESTING,    L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "home",           do_home,           POS_RESTING,    L8,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "permit",         do_permit,         POS_DEAD,       L1,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
//    { "pipe",           do_pipe,           POS_DEAD,       ML,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,0,0 },
    { "hometown",       do_hometown,       POS_RESTING,     0,  LOG_ALWAYS, 0, 0,0,0,1,1,1,1,1,0,1,1 },
    { "doas",           do_doas,           POS_RESTING,    ML,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "immpass",        do_immpass,        POS_DEAD,       IM,  LOG_NEVER,  0, 1,0,0,0,0,0,0,0,0,1,0 },
    { "release",        do_release,        POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,1 },
    /* New skills */
    { "lore",           do_lore,           POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,1,1,0,1,0,0,1,1,1 },
    { "mana charge",    do_mana_charge,    POS_STANDING,    0,  LOG_NORMAL, 0, 0,0,1,1,1,1,1,0,0,0,0 },
    { "voodoo",         do_voodoo,         POS_STANDING,    0,  LOG_NORMAL, 0, 0,0,1,1,1,1,1,0,0,0,0 },
    { "shielddisarm",   do_shield_disarm,  POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "deflect",        do_rake,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "layonhands",     do_lay_on_hands,   POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "detect_hidden",  do_detect_hidden,  POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "cleave",         do_cleave,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "behead",         do_behead,         POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "assassinate",    do_assassinate,    POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "blackjack",      do_blackjack,      POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "gag",            do_gag,            POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "strangle",       do_strangle,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,1,0,0,0,0,1 },
    { "acupuncture",    do_acupuncture,    POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "field dressing", do_fdress,	   POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "sequencer",	do_sequencer,      POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "contingecy",	do_contingency,    POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "herb",           do_herb,           POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "tame",           do_tame,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "beastcall",      do_beast_call,     POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "rage",           do_rage,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "cyclone",	do_cyclone,        POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "maelstrom",	do_maelstrom,        POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "bodyslam",       do_bodyslam,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "grapple",        do_grapple,        POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "charge",         do_charge,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "shoot",          do_shoot,          POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 }, 
    { "impale",         do_impale,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "ambush",         do_ambush,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "endure",         do_endure,         POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "barkskin",       do_barkskin,       POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "warcry",         do_warcry,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "circle",         do_circle,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "swing",          do_swing,          POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "windmill",       do_windmill,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "headbutt",       do_headbutt,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "camouflage",     do_camouflage,     POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "chameleon",      do_chameleon,      POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "keen_sight",     do_keen_sight,     POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "vanish",         do_vanish,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "chii",         	do_chii,           POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "blindness_dust", do_blindness_dust, POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "poison_smoke",   do_poison_smoke,   POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "butcher",   	do_butcher,        POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "caltraps",       do_caltraps,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "bolo",		do_bolo,	   POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "entangle",	do_entangle,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "call",		do_call,           POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "nerve",          do_nerve,          POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "truesight",      do_truesight,      POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "trophy",         do_trophy,         POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
/*  { "spellbane",      do_spellbane,      POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "cognizance",     do_cognizance,     POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 }, */
    { "stance",         do_stance,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "melee",          do_melee,          POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "wanted",         do_wanted,         POS_DEAD,        0,  LOG_ALWAYS, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "judge",          do_judge,          POS_STANDING,    0,  LOG_NORMAL, 0, 0,1,0,1,0,1,0,0,0,0,1 },
    { "watchtower",     do_watchtower,     POS_STANDING,    0,  LOG_NORMAL, 0, 0,1,1,1,1,1,0,0,0,0,1 },
    { "exile",		do_exile,	   POS_STANDING,    0,  LOG_NORMAL, 0, 0,1,1,1,1,1,0,0,0,0,1 },
    { "nemesis",	do_nemesis,	   POS_STANDING,    0,  LOG_NORMAL, 0, 0,1,1,1,1,1,0,0,0,0,1 },
    { "spawn",		do_spawn_blade,	   POS_STANDING,    0,  LOG_NORMAL, 0, 0,1,1,1,1,1,0,0,0,0,1 },
    { "bribe",		do_bribe,          POS_STANDING,    0,  LOG_NORMAL, 1, 0, 0,0,0,0,0,0,0,1,1,1 },
    { "secretarts",     do_secret_arts,    POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "throatcut",      do_throatcut,      POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "rally",		do_rally_justice,  POS_STANDING,    0,  LOG_NORMAL, 0, 0,1,1,1,1,1,0,0,0,0,1 },
    { "diplomacy",	do_diplomacy,	   POS_STANDING,    0,  LOG_NORMAL, 0, 0,1,1,1,1,1,0,0,0,0,1 },
    { "jail",           do_jail,           POS_STANDING,    0,  LOG_ALWAYS, 0, 0,1,0,1,0,1,0,0,0,0,1 },
    { "bail",           do_bail,           POS_STANDING,    0,  LOG_ALWAYS, 0, 0,1,0,1,0,1,0,0,0,0,1 },

    /*Not used (Viri)
    { "squire",         do_squire,         POS_STANDING,    0,  LOG_NORMAL, 0, 0,1,0,1,0,1,0,0,0,0,1 },
    { "knight",         do_knight,         POS_STANDING,    0,  LOG_ALWAYS, 0, 0,1,0,1,0,1,0,0,0,0,1 },
    */

    { "guard",          do_guard,          POS_STANDING,    0,  LOG_NORMAL, 0, 0,0,0,1,0,1,0,0,0,0,1 },
    { "mount",          do_mount,          POS_STANDING,    0,  LOG_NORMAL, 0, 0,1,0,1,1,1,1,1,0,0,1 },
    { "dismount",       do_dismount,       POS_RESTING,     0,  LOG_NORMAL, 0, 0,1,0,1,1,1,1,1,0,0,1 },
    { "spy",            do_spy,            POS_STANDING,    0,  LOG_ALWAYS, 0, 0,1,0,1,0,1,0,0,0,0,1 },
    { "relax",          do_relax,          POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "brew",           do_brew,           POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "modify",         do_modify,         POS_SLEEPING,    0,  LOG_NORMAL, 0, 0,1,0,1,0,1,0,0,0,0,1 },
    { "challenge",      do_challenge,      POS_STANDING,    0,  LOG_ALWAYS, 0, 0,1,0,1,0,1,0,0,0,0,1 },
    { "dmark",		do_deathmark,      POS_STANDING,    0,  LOG_ALWAYS, 0, 0,1,0,1,0,1,0,0,0,0,1 },
    { "record",         do_record,         POS_RESTING,     0,  LOG_NORMAL, 0, 0,1,0,1,0,1,0,0,0,1,1 },
    { "contract",       do_contract,       POS_STANDING,    0,  LOG_ALWAYS, 0, 0,1,0,1,0,1,0,0,0,1,1 },
    { "camp",           do_camp,           POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "collect",        do_bounty_collect, POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,1,1 },
    { "fletchery",	do_fletchery,	   POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,1,0,0,0,1 },
    { "stringbow",	do_bowyer,	   POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,1,0,0,0,1 },
    { "ranger staff",   do_ranger_staff,   POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,1,0,0,0,1 },
    { "healer staff",   do_healer_staff,   POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,1,0,0,0,1 },
    { "druid staff",    do_druid_staff,    POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,1,0,0,0,1 },
    { "invoke",         do_invoke,         POS_RESTING,     0,  LOG_NORMAL, 1, 1,0,0,1,0,1,0,0,0,0,1 },
    { "wish",           do_wish,           POS_RESTING,     0,  LOG_NORMAL, 1, 1,0,0,1,0,1,0,0,0,0,1 },
    { "throw",          do_throw,          POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "roar",           do_roar,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "edge",           do_edge_craft,     POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "counterfeit",    do_counterfeit,    POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "servant",        do_servant,        POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "army",           do_army,           POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "hound",          do_hound_obey,     POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "defecate",       do_defecate,       POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,1,1 },
    { "vomit",          do_vomit,          POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,1,1 },
    { "stalk",          do_stalk,          POS_STANDING,    0,  LOG_NORMAL, 0, 0,1,0,1,0,1,0,0,0,0,1 },
    { "control",        do_control,        POS_STANDING,    0,  LOG_NORMAL, 0, 0,1,0,1,0,1,0,0,0,0,1 },
    { "break",          do_break,          POS_DEAD,        0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,1,0,1 },
    { "ensnare",        do_ensnare,        POS_FIGHTING,    0,  LOG_NORMAL, 0, 0,1,0,1,0,1,0,0,0,1,1 },
    { "apprehend",      do_apprehend,      POS_FIGHTING,    0,  LOG_NORMAL, 0, 0,1,0,1,0,1,0,0,0,1,1 },
    { "qslay",          do_qslay,          POS_DEAD,       L3,  LOG_ALWAYS, 1, 1,1,0,0,0,0,0,0,0,1,0 },
    { "qtrans",         do_qtrans,         POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "drain",        	do_drain,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,1,1 },
    { "coffin",         do_coffin,         POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "tear",           do_tear,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,1,1 },
    /* VOTE COMMANDS */
    { "vote",		do_vote,         POS_STANDING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,1 },
    /* CLASS QUESTS */
    { "select",         do_select,         POS_STANDING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,1 },
    { "convert",        do_convert,        POS_STANDING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,1 },
    { "guild",          do_guild,	   POS_STANDING,    0,  LOG_NORMAL, 1, 0,1,0,0,0,0,0,0,0,1,1 },
    { "qrace",         do_quest_race,	   POS_DEAD,       L5,  LOG_ALWAYS, 1, 0,0,0,0,0,0,0,0,0,1,0 },
    { "qshaman",        do_quest_shaman,   POS_DEAD,       L5,  LOG_ALWAYS, 1, 0,0,0,0,0,0,0,0,0,1,0 },
    { "qhealer",        do_quest_healer,   POS_DEAD,       L5,  LOG_ALWAYS, 1, 0,0,0,0,0,0,0,0,0,1,0 },
    { "qdruid",         do_quest_druid,    POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,0,0,0,0,0,0,0,0,1,0 },
    { "qdemon",         do_quest_demon,    POS_DEAD,       L5,  LOG_ALWAYS, 1, 0,0,0,0,0,0,0,0,0,1,0 },
    { "qavatar",        do_quest_avatar,   POS_DEAD,       L5,  LOG_ALWAYS, 1, 0,0,0,0,0,0,0,0,0,1,0 },
    { "qvamp",          do_quest_vamp,     POS_DEAD,       L5,  LOG_ALWAYS, 1, 0,0,0,0,0,0,0,0,0,1,0 },
    { "qcrus",          do_quest_crus,     POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,0,0,0,0,0,0,0,0,1,0 },
    { "qelder",         do_quest_elder,    POS_DEAD,       L5,  LOG_ALWAYS, 1, 0,0,0,0,0,0,0,0,0,1,0 },
    { "qundead",        do_quest_undead,   POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,0,0,0,0,0,0,0,0,1,0 },
    { "qpsi",           do_quest_psi,      POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,0,0,0,0,0,0,0,0,1,0 },
    { "qking",          do_quest_king,     POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,0,0,0,0,0,0,0,0,1,0 },
/*  { "qdruid",         do_quest_druid,    POS_DEAD,       L5, LOG_ALWAYS, 1, 0,0,0,0,0,0,0,0,0,1,0 },*/
    { "masquerade",     do_masquerade,     POS_DEAD,        0,  LOG_ALWAYS, 1, 0,0,0,0,0,0,0,0,0,1,0 },
    { "badname",        do_badname,        POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "goodname",       do_allowname,      POS_DEAD,       L5,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "appdesc",        do_appdesc,        POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "oowner",         do_oowner,          POS_DEAD,       L8,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "stake",          do_stake,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "proficiency",    do_proficiency,    POS_RESTING,     0,  LOG_NORMAL, 1,0,1,1,0,0,0,0,0,0,1 },
    { "toxify",         do_toxify,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,1,1 },
    { "destroy-undead", do_destroy_undead, POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,1,1 },
    { "seep",           do_seep,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,1,1 },
    { "tackle",         do_tackle,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,1,1 },
    { "thrust",         do_thrust,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,1,1 },
    { "pixie_dust",     do_pixie_dust,     POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "beads",          do_beads,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "monkey",         do_monkey,         POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "tiger",          do_tiger,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "crane",          do_crane,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "mantis",         do_mantis,         POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "drunken",        do_drunken,        POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "horse",          do_horse,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "dragon",         do_dragon,         POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "buddha",         do_buddha,         POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "catalepsy",      do_catalepsy,      POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "revive",         do_revive,         POS_DEAD,        0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,0 },
    { "aura",           do_aura,           POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "healing",        do_healing,        POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "sense",          do_sense_motion,   POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "kinetics",       do_kinetics,       POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "outcast",        do_outcast,        POS_DEAD,       L6,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "empower",        do_empower,	       POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "store_chii",     do_store_chii,	    POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "thrash",         do_thrash,         POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "bolt",           do_chii_bolt,      POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "deathblow",      do_deathblow,      POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,0,0,1,0,1,0,0,0,0,1 },
    { "silvanus",	do_trap_silvanus,  POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "entomb",      	do_entomb,         POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "untomb",       	do_untomb,         POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "test",           do_test,           POS_DEAD,       L6,  LOG_NORMAL, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "double",         do_double_grip,    POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "high",           do_high,	   POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "low",            do_low,		   POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "left",           do_left,	   POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "right",          do_right,	   POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "middle",         do_middle,	   POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "damage",         do_damage,	   POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "normal",         do_normal,	   POS_RESTING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "coneofforce",    do_cone,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "transform",      do_transform,      POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "werepower",      do_werepower,      POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "burrow",         do_burrow,         POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,1,1 },
    { "unburrow",       do_unburrow,       POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,1,0 },
    { "cutpurse",       do_cutpurse,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "insectswarm",    do_insect_swarm,   POS_FIGHTING,    0,  LOG_NORMAL, 0, 0,0,1,1,1,1,1,1,0,0,1 },
    { "darkritual",     do_dark_ritual,    POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,0,1,0,1,0,0,0,0,1 },
    { "offhanddisarm",  do_offhand_disarm, POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "shed",           do_shed,           POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,0,1,0,0,0,0,0,1,1,1 },
    { "fasting",        do_fasting,        POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "swallow",   	do_swallow,        POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "probe",          do_probe,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,1,1,1,0,1,0,0,1,1,1 },
    { "decoy",          do_decoy,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "crusade",        do_crusade,        POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "avenge",         do_avenger,        POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "heroism",        do_heroism,        POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,0,0,0,0,0,0,0,1 },
    { "refrain",        do_refrain,        POS_FIGHTING,    0,  LOG_NORMAL, 1, 1,0,0,0,0,0,0,0,0,0,1 },
    { "tarot",          do_tarot,          POS_RESTING,     0,  LOG_NORMAL, 1, 1,0,1,1,0,1,0,0,0,0,1 },
    { "leech",          do_leech,          POS_STANDING,    0,  LOG_NORMAL, 0, 0,0,0,1,0,1,0,0,0,0,1 },
    { "knockout",       do_tko,   	   POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,0,1,0,1,0,0,0,0,1 },
    { "create_torch",   do_create_torch,   POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "credits",        do_credits,        POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "panhandle",      do_panhandle,      POS_RESTING,     0,  LOG_NORMAL, 0, 1,0,0,1,0,1,0,0,0,0,1 },
    { "lure",           do_attract,        POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,0,1,0,1,0,0,0,0,1 },
    { "taunt",          do_taunt,          POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,0,0,0,0,0,0,0,0,0,1 },
    { "uppercut",       do_uppercut,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,0,0,0,0,0,0,0,0,0,1 },
    { "haymaker",       do_haymaker,       POS_FIGHTING,    0,  LOG_NORMAL, 0, 1,0,1,1,1,1,1,1,0,0,1 },
    { "ventriloquate",  do_ventriloquate,  POS_FIGHTING,    0,  LOG_NORMAL, 1, 1,0,0,1,0,1,0,0,0,0,1 },
    { "duet",           do_duet,           POS_STANDING,    0,  LOG_NORMAL, 1, 0,0,0,0,0,0,0,0,0,0,1 },
    { "tune",           do_tune,           POS_STANDING,    0,  LOG_NORMAL, 0, 0,0,0,1,0,1,0,0,0,0,1 },
    { "feed",           do_feed,           POS_RESTING,     0,  LOG_NORMAL, 1, 0,0,0,0,0,0,0,0,0,0,1 },
    { "ignite",         do_ignite,         POS_STANDING,    0,  LOG_NORMAL, 1, 1,1,1,1,1,1,1,1,1,1,1 },
    { "alias",          do_alias,          POS_RESTING,     0,  LOG_ALWAYS, 1, 1,1,1,0,0,0,0,0,0,1,0 },
    { "history",        do_history,	   POS_RESTING,     0,  LOG_NORMAL, 1, 1,0,0,1,0,1,0,0,0,0,1 },
    { "ram",            do_ram,            POS_STANDING,    0,  LOG_NORMAL, 1, 1,1,0,1,0,1,0,0,0,0,1 },
    { "anger",          do_anger,          POS_STANDING,    0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "sharpen",        do_sharpen,       POS_STANDING,     0,  LOG_NORMAL, 0, 1,1,0,1,0,1,0,0,0,0,1 },
    { "roccall",        do_call_roc,       POS_FIGHTING,     0,  LOG_NORMAL, 1, 0,1,0,1,0,1,0,0,0,1,1 },  
    { "embrace",        do_embrace ,       POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,1,0,1,1,0,1,1,1,1 },
    { "predict",        do_predict ,       POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,1,0,1,1,0,1,1,1,1 },
    { "spellkill",      do_spellkiller ,   POS_STANDING,    0,  LOG_NORMAL, 0, 1,0,1,0,1,1,0,1,1,1,1 },
    /* End of list */
    { "if",             do_if,             POS_DEAD,       ML,  LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,0,0 },
    { "endif",          do_endif,          POS_DEAD,       ML,  LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,0,0 },
    { "",               0,                 POS_DEAD,        0,  LOG_NORMAL, 0, 1,1,1,0,0,0,0,0,0,0,0 }
};



int PRECOMMAND_CHECK(CHAR_DATA* ch, int cmd, char* argument)
{
  /* If function retrunts FALSE the command is NOT run. */
  //To run before any skill function is called set F1_CMDBLOCK

  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int fSuccess = TRUE;
  int result = 0;

  //---FUNCTIONS THAT BLOCK COMMANDS---..
  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }
    
    if (!IS_SET(effect_table[GN_POS(paf->type)].flags, EFF_F1_CMDBLOCK)) {
      continue;
    }

    //BLOCKING OF COMMANDS DONE HERE
    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(ch, paf, argument, paf->type, 
				      &cmd, NULL, TRUE, EFF_UPDATE_PRECOMMAND)) == TRUE) {
	return FALSE;
      }
    }
  }


//OTHER
  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }

    if (IS_SET(effect_table[GN_POS(paf->type)].flags, EFF_F1_CMDBLOCK)) {
      continue;
    }

//SPECIAL GENS HERE

    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(ch, paf, argument, paf->type, 
				      &cmd, NULL, TRUE, EFF_UPDATE_PRECOMMAND)) != TRUE) {
	if (result == -1) {
	  return -1;
	}
      }
    }
  }

  return fSuccess;
}


int POSTCOMMAND_CHECK(CHAR_DATA* ch, int cmd)
{
  //RUn after the skill function exectures.
  CHAR_DATA* vch = ch;
  AFFECT_DATA* paf;
  AFFECT_DATA* paf_next;

  int fSuccess = TRUE;
  int result = 0;

  for (paf = vch->affected; paf != NULL; paf = paf_next) {
    paf_next = paf->next;
    //we only run the update once per effect.
    if (paf_next != NULL) {
      if (paf->type == paf_next->type) {
	continue;
      }
    }
    
//SPECIAL GENS HERE

    if (IS_GEN(paf->type)) {
      if ((result = run_effect_update(ch, NULL, NULL, paf->type, 
				      &cmd, NULL, TRUE, EFF_UPDATE_POSTCOMMAND)) != TRUE) {
	if (result == -1) {
	  return -1;
	}
	else {
	  fSuccess = result;
	}
      }
    }
  }

  return fSuccess;

}



void interpret( CHAR_DATA *ch, char *argument )
{
  int string_count = nAllocString, perm_count = nAllocPerm, string_size = sAllocString, perm_size = sAllocPerm;
  int stringdif, permdif;
  int cmd, trust = get_trust(ch);
  char buf[MSL];
  char cmd_copy[MIL], prefix[MIL], command[MIL], logline[MIL];
  char *strtime;
  char *new_cmd;
  int Pre_Ret = 1;//return value out of PRECOMMAND_CHECK
  bool found = FALSE;
  Double_List * tmp_list;
  
  if (sec_check(ch))
    return;

  if (ch->in_room == NULL)
    return;
  if (ch->prefix[0] != '\0' && str_prefix("prefix",argument)) {
    if (strlen(ch->prefix) + strlen(argument) > MIL) {
      send_to_char("Line to long, prefix not processed.\r\n",ch);
    }
    else {
      sprintf(prefix,"%s %s",ch->prefix,argument);
      argument = prefix;
    }
  }
  
  while ( isspace(*argument) ) {
    argument++;
  }
   
  if ( argument[0] == '\0' ) {
    return;
  }

  if (!IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE)) {
    send_to_char( "You're totally frozen!\n\r", ch );
    return;
  }
  else if (IS_STONED(ch)){
    send_to_char("You're too busy collecting bird droppings!\n\r", ch);
    return;
  }
  if ( !IS_NPC(ch) && is_affected(ch,gsn_mortally_wounded) 
       && !IS_IMMORTAL(ch)) {
    send_to_char( "You are too hurt to do anything.\n\r",ch);
    return;
  }
  if ( is_affected(ch,gsn_ecstacy) && !IS_IMMORTAL(ch)) {
    send_to_char( "Why?!? You're so happy right now.\n\r",ch);
    return;
  }
  if ( is_song_affected(ch,gsn_dance_song) && number_percent() < 25) {
    if (!IS_AWAKE(ch) && !IS_AFFECTED(ch,AFF_SLEEP)) {
      do_wake(ch,"");
    }
    send_to_char( "You decide to dance instead!\n\r",ch);
    act("$n dances wildly in front of you!",ch,NULL,NULL,TO_ROOM);
    return;
  }
  
  strcpy( logline, argument );
  strcpy(cmd_copy,argument);
  if ( !isalpha(argument[0]) && !isdigit(argument[0]) ) {
    command[0] = argument[0];
    command[1] = '\0';
    argument++;
    while ( isspace(*argument) ) {
      argument++;
    }
  }
  else {
    argument = one_argument( argument, command );
  }

  /* alias check, if succesfull the input is stuffed with the alias */
  if (!IS_GAME(ch, GAME_NOALIAS) 
      && ( new_cmd = check_alias( ch, command, argument)) != NULL){
    char cmd[MIL];
    new_cmd[MIL] = 0;
    sprintf( cmd, "%s", new_cmd);
    interpret(ch, cmd);
    return;
  }
  AliasClearSafety();


  for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ ) {
    if ( command[0] == cmd_table[cmd].name[0] 
	 && !str_prefix( command, cmd_table[cmd].name ) 
	 && cmd_table[cmd].level <= trust ) {
      found = TRUE;
      break;
    }
  }

  //remove quit confirm
  if (!IS_NPC(ch) && ch->pcdata->confirm_quit 
      && str_cmp(cmd_table[cmd].name, "quit")){
    ch->pcdata->confirm_quit = FALSE;
  }

  //this prevents commands when burrowed except revert
  if (found
      && (is_affected(ch,gsn_burrow) 
	  && cmd_table[cmd].meditate > 0) 
      && ((strcmp("revert", cmd_table[cmd].name)) 
	  && is_affected(ch, gsn_mist_form))) {
    send_to_char( "You can't do that while underground!\n\r", ch );
    send_to_char( "Type unburrow to emerge from the gound.\n\r", ch );
    return;
  }
  
  if (found
      && IS_AFFECTED2(ch, AFF_CATALEPSY) && cmd_table[cmd].meditate > 0) {
    send_to_char( "You're in a state of self-induced catalepsy.  You can't do that!\n\r", ch );
    send_to_char( "Type revive to revive yourself if you want.\n\r", ch );
    return;
  }

  if (ch->position == POS_MEDITATE && cmd_table[cmd].meditate > 0) {
    act_new("You stop meditating and stand up.",ch,NULL,NULL,TO_CHAR,POS_DEAD);
    act("$n stops meditating and stands up.",ch,NULL,NULL,TO_ROOM);
    ch->position = POS_STANDING;
    dreamprobe_cast(ch, NULL );
  }
  
  if (found && cmd_table[cmd].hide > 0 && IS_AFFECTED(ch, AFF_HIDE)) {
    affect_strip ( ch, gsn_hide);
    REMOVE_BIT (ch->affected_by, AFF_HIDE);
    act("You step out of the shadows.",ch,NULL,NULL,TO_CHAR);
    act("$n steps out of the shadows.",ch,NULL,NULL,TO_ROOM);
  }
  
  if (found && cmd_table[cmd].sneak > 0 && IS_AFFECTED(ch, AFF_SNEAK) 
      && ch->race != race_lookup("faerie") 
      && ch->race != race_lookup("elf") 
      && ch->race != race_lookup("drow")) {
    affect_strip (ch, gsn_sneak);
    REMOVE_BIT   (ch->affected_by, AFF_SNEAK);
    send_to_char("You trample around loudly again.\n\r",ch);
  }
  
  if (found && cmd_table[cmd].camo > 0 && IS_AFFECTED2(ch, AFF_CAMOUFLAGE)) {
    affect_strip ( ch, gsn_camouflage);
    REMOVE_BIT   ( ch->affected2_by, AFF_CAMOUFLAGE);
    act("You step out from your cover.",ch,NULL,NULL,TO_CHAR);
    act("$n steps out from $s cover.",ch,NULL,NULL,TO_ROOM);
  }
  
  if (found && cmd_table[cmd].shadow > 0 && IS_AFFECTED2(ch, AFF_SHADOWFORM)) {
    affect_strip (ch, gsn_shadowform);
    REMOVE_BIT (ch->affected2_by, AFF_SHADOWFORM);
    act("Your body regains its substance, and you materialize into existence.",ch,NULL,NULL,TO_CHAR);
    act("$n's body regains its substance, and $e materializes into existence.",ch,NULL,NULL,TO_ROOM);
    WAIT_STATE( ch, 24);
  }
  
  if (found && cmd_table[cmd].shadow > 0 && IS_AFFECTED(ch, AFF_TREEFORM)) {
    affect_strip (ch, gsn_treeform);
    REMOVE_BIT (ch->affected_by, AFF_TREEFORM);
    act("Your body returns to its normal shape.",ch,NULL,NULL,TO_CHAR);
    act("$n's body returns to its normal shape.",ch,NULL,NULL,TO_ROOM);
    WAIT_STATE( ch, 24);
  }

  if ( !found ) {
    if ( !check_social( ch, command, argument ) ) {
      send_to_char( "Huh?\n\r", ch );
      if ( IS_NPC(ch) && ch->trust && ch->trust == 6969 && str_cmp(logline,"endif")) {
	nlogf ("Log %d: MProg mistake: %s", ch->pIndexData->vnum, logline );
      }
    }
    return;
  }
  
  if ( IS_NPC(ch) && cmd_table[cmd].mob == 0 ) {
    return;
  }
   
  if ( IS_NPC(ch) && is_affected(ch,gsn_linked_mind) && cmd_table[cmd].swtch == 0 ) {
    return;
  }
  
  if ( ch->race == grn_vampire && cmd_table[cmd].morph == 0 ){
    if (is_affected(ch,gsn_bat_form) 
	|| is_affected(ch,gsn_wolf_form) 
	|| is_affected(ch,gsn_mist_form)){
      send_to_char("You can't do that while morphed.\n\r", ch);
      return;
    }
  }
  
  if ( IS_AFFECTED(ch, AFF_CHARM) && cmd_table[cmd].charm == 0 ) {
    send_to_char("You can't do that while charmed.\n\r",ch);
    return;
  }
  
  if ( cmd_table[cmd].log == LOG_NEVER ) {
    strcpy( logline, "" );
  }

  if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) ) 
       || ( !IS_NPC(ch) && fLogAll ) 
       || (!IS_NPC(ch) && cmd_table[cmd].log == LOG_ALWAYS ) ) { 
    sprintf( log_buf, "[%d] Log %s: %s", ch->in_room->vnum, ch->name, logline );
    wiznet(log_buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
    log_string( log_buf );
  }
  
  if ( ch->desc != NULL && ch->desc->snoop_by != NULL ) {
    tmp_list = ch->desc->snoop_by;
    while (tmp_list != NULL) {
      write_to_buffer((DESCRIPTOR_DATA *)tmp_list->cur_entry, ch->name,0);
      write_to_buffer((DESCRIPTOR_DATA *)tmp_list->cur_entry, "% ",    2 );
      write_to_buffer((DESCRIPTOR_DATA *)tmp_list->cur_entry, logline, 0 );
      write_to_buffer((DESCRIPTOR_DATA *)tmp_list->cur_entry, "\n\r",  2 );
      tmp_list = tmp_list->next_node;
    }
  }
  
  if ( !found ) {
    if ( !check_social( ch, command, argument ) ) {
      send_to_char( "Huh?\n\r", ch );
      if ( IS_NPC(ch) && ch->trust && ch->trust == 6969 && str_cmp(logline,"endif")) {
	nlogf ("Log %d: MProg mistake: %s", ch->pIndexData->vnum, logline );
      }
    }
    return;
  }
  
  if ( ch->position < cmd_table[cmd].position) {
//Viri: This should have no effect, its taken care of above       && (ch->position != POS_MEDITATE) ) {
    switch( ch->position ) {
    case POS_DEAD:     send_to_char( "Lie still; you are DEAD.\n\r", ch ); break;
    case POS_MORTAL:
    case POS_INCAP:    send_to_char( "You are hurt far too bad for that.\n\r", ch ); break;
    case POS_STUNNED:  send_to_char( "You are too stunned to do that.\n\r", ch ); break;
    case POS_SLEEPING: send_to_char( "In your dreams, or what?\n\r", ch ); break;
    case POS_RESTING:  send_to_char( "Nah... You feel too relaxed...\n\r", ch); break;
    case POS_MEDITATE:  send_to_char( "You are still meditating.\n\r", ch); break;
    case POS_SITTING:  send_to_char( "Better stand up first.\n\r",ch); break;
    case POS_FIGHTING: send_to_char( "No way!  You are still fighting!\n\r", ch); break;
    }
    return;
  }

  strtime = ctime( &mud_data.current_time );
  strtime[strlen(strtime)-1] = '\0';
  sprintf (last_command, "Command: [%5d] %s in [%5d] %s: %s (%s)",
	   IS_NPC(ch) ? ch->pIndexData->vnum : 0, IS_NPC(ch) ? ch->short_descr : ch->name,
	   ch->in_room ? ch->in_room->vnum : 0, 
	   ch->in_room ? ch->in_room->name : "(not in a room)", logline, strtime);

    
//PRECOMMAND CHECK
  if ( (Pre_Ret = PRECOMMAND_CHECK(ch, cmd, argument)) == -1) {
    bug("Error at: PRECOMMAND_CHECK\n\r", 0);
  }


//RUN COMMAND FUNCTION HERE  IF precommand returned TRUE
  if (Pre_Ret) {
    init_loop("command");

    /*     nlogf("Char: %s , Room: %d, Cmd: %s.", 
	   IS_NPC(ch)? ch->short_descr : ch->name, 
	   ch->in_room->vnum,
	   cmd_table[cmd].name); */

    (*cmd_table[cmd].do_fun) ( ch, argument );
    end_loop("command");
  }

    /*
//POSTCOMMAND CHECK
  if (POSTCOMMAND_CHECK(ch, cmd) == -1)
  bug("Error at: POSTCOMMAND_CHECK\n\r", 0);
    */

  sprintf (last_command, "Command: (Finished) [%5d] %s in [%5d] %s: %s (%s)",
	   IS_NPC(ch) ? ch->pIndexData->vnum : 0, IS_NPC(ch) ? ch->short_descr : ch->name,
	   ch->in_room ? ch->in_room->vnum : 0, 
	   ch->in_room ? ch->in_room->name : "(not in a room)", logline, strtime);

  if (string_count != nAllocString || perm_count < nAllocPerm ) {
    stringdif = sAllocString - string_size;
    permdif = sAllocPerm - perm_size;
    sprintf(buf,"Memcheck: strings %d bytes : perms %d bytes : %s : %s", 
	    stringdif, permdif, ch->name, cmd_copy);
    wiznet(buf,NULL,NULL,WIZ_MEMORY,0,0);
  }
  tail_chain( );
}

bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    char arg[MIL];
    CHAR_DATA *victim;
    int cmd;
    bool found = FALSE;

    if (is_affected(ch, gen_request)){
      send_to_char("You are far too taken with prayer for that.\n\r", ch);
      return TRUE;
    }

    if (is_affected(ch, gen_chant)){
      send_to_char("Ever tried to do that while chanting?.\n\r", ch);
      return TRUE;
    }

    if (is_affected(ch, gen_acraft)
	|| is_affected(ch, gen_wcraft)){
      send_to_char("Ever tried to do that while working on an item?.\n\r", ch);
      return TRUE;
    }

    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
	if ( command[0] == social_table[cmd].name[0] && !str_prefix( command, social_table[cmd].name ) )
	{
	    found = TRUE;
	    break;
	}
    if ( !found )
	return FALSE;
    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
	send_to_char( "You are anti-social!\n\r", ch );
	return TRUE;
    }
    switch ( ch->position )
    {
    case POS_DEAD:     send_to_char( "Lie still; you are DEAD.\n\r", ch ); return TRUE;
    case POS_INCAP:
    case POS_MORTAL:   send_to_char( "You are hurt far too bad for that.\n\r", ch ); return TRUE;
    case POS_STUNNED:  send_to_char( "You are too stunned to do that.\n\r", ch ); return TRUE;
    case POS_SLEEPING:
	if ( !str_cmp( social_table[cmd].name, "snore" ) )
	    break;
	send_to_char( "In your dreams, or what?\n\r", ch );
	return TRUE;
    }
    one_argument( argument, arg );
    victim = NULL;
    if (IS_AFFECTED(ch, AFF_HIDE))
    {
	affect_strip( ch, gsn_hide );
	REMOVE_BIT   ( ch->affected_by, AFF_HIDE		);
	act("You step out of the shadows.",ch,NULL,NULL,TO_CHAR);
	act("$n steps out of the shadows.",ch,NULL,NULL,TO_ROOM);
    }
    if (IS_AFFECTED2(ch, AFF_CAMOUFLAGE))
    {
	affect_strip ( ch, gsn_camouflage			);
	REMOVE_BIT   ( ch->affected2_by, AFF_CAMOUFLAGE		);
	act("You step out from your cover.",ch,NULL,NULL,TO_CHAR);
	act("$n steps out from $s cover.",ch,NULL,NULL,TO_ROOM);
    }
    if ( arg[0] == '\0' )
    {
	act_new( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM, POS_RESTING + 100    );
	act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
    }
    else if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	send_to_char( "They aren't here.\n\r", ch );
    else if ( victim == ch )
    {
	act_new( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM, POS_RESTING + 100    );
	act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR    );
    }
    else
    {
	act_new( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT, POS_RESTING + 100 );
	act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
	act_new( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT, POS_RESTING + 100    );
    }
    return TRUE;
}

bool is_number ( char *arg )
{
    if ( *arg == '\0' )
        return FALSE;
    if ( *arg == '+' || *arg == '-' )
        arg++;
    for ( ; *arg != '\0'; arg++ )
        if ( !isdigit( *arg ) )
            return FALSE;
    return TRUE;
}

int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;
    for ( pdot = argument; *pdot != '\0'; pdot++ )
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    strcpy( arg, argument );
    return 1;
}

int mult_argument(char *argument, char *arg)
{
    char *pdot;
    int number;
    for ( pdot = argument; *pdot != '\0'; pdot++ )
        if ( *pdot == '*' )
        {
            *pdot = '\0';
            number = atoi( argument );
            *pdot = '*';
            strcpy( arg, pdot+1 );
            return number;
        }
    strcpy( arg, argument );
    return 1;
}

char *one_argument( char *argument, char *arg_first )
{
    char cEnd;
    while ( isspace(*argument) )
	argument++;
    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;
    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER(*argument);
	arg_first++;
	argument++;
    }
    *arg_first = '\0';
    while ( isspace(*argument) )
	argument++;
    return argument;
}

char *one_argument_2( char *argument, char *arg_first )
{
    char cEnd;
    while ( isspace(*argument) )
	argument++;
    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;
    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = (*argument);
	arg_first++;
	argument++;
    }
    *arg_first = '\0';
    while ( isspace(*argument) )
	argument++;
    return argument;
}

void do_commands( CHAR_DATA *ch, char *argument )
{
    int cmd, col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level <= LEVEL_HERO && cmd_table[cmd].level <= get_trust( ch ) && cmd_table[cmd].show)
	{
	    sendf( ch, "%-12s", cmd_table[cmd].name );
	    if ( ++col % 6 == 0 )
		send_to_char( "\n\r", ch );
	}
    }
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
}

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    int cmd, col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
        if ( cmd_table[cmd].level > LEVEL_HERO && cmd_table[cmd].level <= get_trust( ch ) && cmd_table[cmd].show)
	{
	    sendf( ch, "%-12s", cmd_table[cmd].name );
	    if ( ++col % 6 == 0 )
		send_to_char( "\n\r", ch );
	}
    if ( col % 6 != 0 )
	send_to_char( "\n\r", ch );
}

/* do_deity */
/* linked into act_info.c */
/* written by viri */
void do_deity( CHAR_DATA *ch, char *argument )
{
  //this is the command that allwos the pc to choose his religion.

  char arg1[MIL];
  int god;

  int ethos = ch->pcdata->ethos;
  int align = 0;

  //args
  argument = one_argument( argument, arg1 );

  //quick safety checks
  if (IS_NPC(ch))
    {
      send_to_char("Mobiles follow no god.\n\r",ch);
      return;
    }
  //get align.
  if (ch->alignment <= EVIL_THRESH)
    align = DIETY_ALIGN_EVIL;
  else if (ch->alignment >= GOOD_THRESH)
    align = DIETY_ALIGN_GOOD;
  else
    align = DIETY_ALIGN_NEUTRAL;

  //get ethos.
  switch (ch->pcdata->ethos)
    {
    case 0: ethos = DIETY_ETHOS_LAWFUL;break;
    case 1: ethos = DIETY_ETHOS_NEUTRAL;break;
    case 2: ethos = DIETY_ETHOS_CHAOTIC;break;
    }

  if (ch->pcdata->way > 0)
    {
      send_to_char("Religion is not something to be taken lightly,\n\ryou have made your choice already.\n\r", ch);
      return;
    }
  
  if (IS_NULLSTR(arg1))
    {
      char buf[MIL];
      char out[MSL];
      send_to_char("Deity:\n\rSyntax: Religion <choice>\n\r\n\r"\
		   "This command allows you to select the primary principle\n\r"\
		   "which shall guide you in your life.\n\rYou are required to select "\
		   "one before proceding past rank 10.\n\r", ch);
      strcpy(out, "Your choices are: \n\r" );
      for ( god = 0; (deity_table[god].way != NULL && god < MAX_DIETY); god++ )
	{
	  //print only dieties that are right for the char.
	  if (!IS_SET(deity_table[god].align, align) || !IS_SET(deity_table[god].ethos, ethos) )
	    continue;  
/* viri: changed thsi to be easier for newbies
	  sprintf( buf, "%-12s: %-15s (%s)\n\r", 
		   path_table[deity_table[god].path].name, 
		   deity_table[god].way,
		   deity_table[god].god);
*/
	  sprintf( buf, "%-15s (%s)\n\r", 
		   deity_table[god].way,
		   deity_table[god].god);
	  strcat(out, buf);
	}
      strcat( out, "\n\r" );
      send_to_char(out,ch);
      send_to_char("\n\rConsult HELP RELIGION for more information.\n\r", ch);
      return;
    }//end show dieties.
  
  //No we check for proper args.
  if (str_cmp("confirm", argument) || IS_NULLSTR(argument))
    {
      send_to_char("Deity:\n\rSyntax: Religion <choice> confirm\n\rType: 'religion' alone with no arguments for more info.\n\r", ch);
      return;
    }

  //now we can do the changing.
  if ((god = way_lookup(arg1)) == -1
      || !IS_SET(deity_table[god].align, align) 
      || !IS_SET(deity_table[god].ethos, ethos) )
    {
      send_to_char("That religion is not avaliable as a choice.\n\rType: \"religion\" with no arguments for more info.\n\r", ch);
      return;
    }    
  
//we set the deity.
  ch->pcdata->deity = deity_table[god].god;
  ch->pcdata->way = god;
  sendf(ch, "You have decided to follow the way of %s under %s's guidance.\n\r", deity_table[ch->pcdata->way].way, 
	ch->pcdata->deity);
}

// 04-10 20:00 Ath: Added "coding" flag to title in who and whois.
// 04-23 10:00 Viri: Added PRECOMMAND_CHECK and POSTCOMMAND_CHECK
// 04-28-00 Viri: Added the new advself command.
// 05-09-00 Viri: Added Deity command
//01-02-01 Ath: Added "application" type notes
