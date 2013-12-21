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
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"

const struct monk_type monk_table[8] =
{
    {	"arms",		WEAR_ARMS,	10	},
    {	"legs",		WEAR_LEGS,	10	},
    {	"body",		WEAR_ABOUT,	5	},
    {	"hands",	WEAR_HANDS,	5	},
    {	"head",		WEAR_HEAD,	5	},
    {	"feet",		WEAR_FEET,	10	},
    {	"torso",	WEAR_BODY,	15	},
    {	"waist",	WEAR_WAIST,	10	}
};

/* NOTE: NEVER DELETE A LINE FROM A TABLE. ONLY APPEND TO THE END. *
 * THIS IS BECAUSE THE PFILE DEFINES THESE BY THEIR POSITION.      *
 * REPLACING A LINE WITH A NEW LINE IS OK, AS LONG AS YOU KNOW     *
 * WHAT YOU'RE DOING						   */

const   struct  pc_race_type    pc_race_table   []      =
{
/*
    {
        "race name",    short name,     anatomy, exp per level,
	{ bonus skills }, selectable during char creation,
        { min stats     },   { max stats },    size,       
	align, 
	ethos,
	hp, mana, pract,
	lifespan,
        { class choices},
    },
        { "warrior", "berserker", "ranger", "paladin", "dark-knight", "thief", "ninja", "cleric", "monk", "invoker","battlemage", "necromancer", NULL, NULL, NULL, NULL, "bard", "blademaster"}

*/
    {
        "none",		"null",          "none", 1500,
	{ "" },  FALSE, TRUE,
        { 6, 6, 6, 6, 6 },   { 18, 18, 18, 18, 18 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	20, 100, 5,
	100,
        { "" },
    },
// 1
    {
        "human",        "Human",        "human",	1500,
	{ "", NULL }, TRUE, TRUE,
        { 0, 0, 0, 0, 0, 0 },   { 20, 20, 20, 20, 20, 25 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	30, 100, 5,
	100,
        { "warrior", "berserker", "ranger", "paladin", "dark-knight", "thief", "ninja", "cleric", "monk", "invoker","battlemage", "necromancer", NULL, NULL, NULL, NULL, "bard", "blademaster"}
    },
// 2
    {
        "elf",          " Elf ",        "elf", 2000,
	{ "sneak", "infravision", "scout", "quiet movement" }, TRUE, TRUE,
        { 0, 3, 0, 2, -2, 0 }, { 18, 25, 20, 23, 16, 25 }, SIZE_MEDIUM, 
	ALIGN_GOOD,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	20, 150, 5,
	2000,
        { "warrior", NULL, "ranger", "paladin", NULL, "thief","ninja", "cleric", NULL, "invoker","battlemage", NULL, NULL, NULL, NULL, NULL, "bard", "blademaster",}
    },
// 3
    {
        "drow",         "Drow ",        "elf", 2000,
	{ "sneak", "infravision", "faerie fire", NULL}, TRUE,TRUE,
        { 0, 1, 0, 4, -3,  0 }, { 18, 24, 20, 24, 16, 25 }, SIZE_MEDIUM, 
	ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	20, 150, 5,
	2000,
        { "warrior", NULL, NULL, NULL, "dark-knight", "thief", "ninja", "cleric", NULL, "invoker", "battlemage", "necromancer", NULL, NULL, NULL, NULL, "blademaster"}
    },
// 4
    {
        "dwarf",        "Dwarf",        "dwarf", 1825,
	{ "berserk", "infravision", NULL }, TRUE, TRUE,
        { 1, 0, 0, -2, 4, 0 },  { 21, 18, 21, 18, 25, 25 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	60, 100, 5,
	800,
        { "warrior", "berserker", NULL, NULL, NULL, NULL, NULL, "cleric", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "bard", "blademaster",}
    },
// 5
    {
        "duergar",      "Duerg",        "dwarf", 1825,
	{ "berserk", "infravision", NULL }, TRUE, TRUE,
        { 1, -1, 0, 1, 1, -1 }, { 21, 17, 20, 20, 22, 25 }, SIZE_MEDIUM, 
	ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	55, 100, 5,
	800,
        { "warrior", "berserker", NULL, NULL, NULL, "thief", NULL, "cleric", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "blademaster"}
    },
// 6
    {
        "halfling",     "Half ",        "demihuman", 1850,
	{ "pry", NULL }, TRUE, TRUE,
        { -1, -2, 1, 3, 0, 2 }, { 17, 17, 21, 25, 20, 25 }, SIZE_SMALL, 
	ALIGN_GOOD | ALIGN_NEUTRAL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	30, 100, 5,
	200,
        { "warrior", NULL, "ranger", NULL, NULL, "thief", NULL, "cleric", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "bard", "blademaster"}
    },
// 7
    {
        "gnome",        "Gnome",        "demihuman", 1650,
	{ "identify", "defuse", "infravision", "detect invis" }, TRUE,TRUE,
        { -2, 2, 4, 0, 0, 0 }, { 18, 23, 25, 18, 18, 25 }, SIZE_SMALL, 
	ALIGN_NEUTRAL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	30, 120, 10,
	200,
        { "warrior", NULL, "ranger", NULL, NULL, "thief", NULL, NULL, NULL, "invoker","battlemage", NULL, NULL, NULL, NULL, NULL, "bard", "blademaster"}
    },
// 8
    {
        "avian",        "Avian",        "flying", 1800,
	{ "fly", NULL }, TRUE,TRUE,
        { 0, 1, 0, 2, 0, 0 },  { 19, 21, 21, 23, 19, 25 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	30, 110, 5,
	100,
        { "warrior", NULL, NULL, NULL, "dark-knight", "thief", "ninja", NULL, NULL, "invoker", "battlemage", "necromancer", NULL, NULL, NULL ,"bard", "blademaster"}
    },
// 9
    {
        "fire",         "Fire ",        "giant", 1700,
        { "bash", "enhanced damage", "bodyslam", "two handed" }, TRUE,TRUE,
        { 5, -3, -3, -2, 2, 0 }, { 25, 15, 15, 16, 23, 25 }, SIZE_LARGE, 
	ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	50, 100, 5,
	80,
        { "warrior", "berserker", NULL, NULL, NULL, NULL, NULL, "cleric", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 10
    {
        "stone",        "Stone",        "giant", 1675,
        { "bash", "enhanced damage", "bodyslam", "two handed" }, TRUE, TRUE,
        { 4, -3, -2, -1, 1, 0 }, { 24, 16, 16, 16, 24, 25 }, SIZE_LARGE, 
	ALIGN_NEUTRAL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	50, 100, 5,
	80,
        { "warrior", "berserker", "ranger", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
    },
// 11
    {
        "storm",        "Storm",        "giant", 1650,
        { "bash", "enhanced damage", "bodyslam", "two handed" }, TRUE, TRUE,
        { 3, -1, -1, -2, 1, 0 },  { 23, 17, 17, 16, 22, 25 }, SIZE_LARGE, 
	ALIGN_GOOD,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	50, 100, 5,
	80,
        { "warrior", "berserker", NULL, NULL, NULL, NULL, NULL, "cleric", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 12
    {
        "undead",        "Undea",       "unique", 1500,
	{ "infravision", NULL }, FALSE, TRUE,
        { 1, -1, 0, 1, 0, 0 },   { 22, 20, 20, 21, 21, 25 }, SIZE_MEDIUM, 
	ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	30, 100, 5,
	9999,
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 13
    {
        "illithid",        "Illit",        "unique", 2000,
        { "trance","cone of force","detect magic","meditation" }, TRUE,TRUE,
        { -1, 3, 2, 0, -1, 0 },  { 17, 25, 23, 18, 17, 25 }, SIZE_MEDIUM, 
	ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	20, 200, 5,
	9999,
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "necromancer", NULL, NULL, NULL, NULL}
    },
// 14
    {
        "minotaur",      "Minot",        "beast", 2000,
        { "rage", "charge", "axe", NULL }, TRUE, TRUE,
        { 2, -1, 0, -1, 1, 0 },  { 23, 17, 18, 17, 22, 25 }, SIZE_LARGE, 
	ALIGN_NEUTRAL,
	ETHOS_CHAOTIC,
	55, 100, 5,
	60,
        { "warrior", "berserker", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 15
    {
        "ethro",    "Ethro",          "ethro", 1800,
	{ "" }, FALSE, FALSE,
        { 1, 4, -1, 1, 0 },   { 21, 24, 14, 22, 19, 25 }, SIZE_SMALL, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	25, 90, 5,
	100,
        { NULL, NULL, NULL, NULL, NULL, "thief", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 16
    {
        "slith",    "Slith",          "beast", 2000,
	{ "spear", NULL}, TRUE,TRUE,
        { 0, -1, -1, 2, 1 },   { 20, 17, 17, 22, 21, 25 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	35, 100, 5,
	100,
        { "warrior", NULL, NULL, NULL, NULL, "thief", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "blademaster"}
    },
// 17
    {
        "troll",    "Troll",         "beast",  2000,
	{ "" }, TRUE, TRUE,
        { 0, 0, 0, 0, 0 },   { 20, 20, 20, 22, 20, 25 }, SIZE_LARGE, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	60, 100, 5,
	60,
        { "warrior", "berserker", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "bard", NULL}
    },
// 18
    {
        "feral",    "Feral",         "beast",  1600,
	{ "dodge", "parry", "dual parry" }, TRUE, TRUE,
        { -1, -2, +1, +4, 0 },   { 19, 18, 21, 24, 20, 25 }, SIZE_MEDIUM, 
	ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	35, 80, 5,
	70,
        { "warrior", "ranger", NULL, NULL, NULL, "thief", "ninja", "cleric", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "blademaster"}
    },
// 19
    {
        "werebeast",    "Beast",          "beast", 1675,
	{ "mind link", "beast call", "probe" }, TRUE, TRUE,
        { 1, -1, -1, 1, 0 },   { 21, 19, 19, 21, 20, 25 }, SIZE_MEDIUM, 
	ALIGN_NEUTRAL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	30, 100, 5,
	100,
        { NULL, "berserker", "ranger", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 20
    {
        "tiger",    "Beast",          "werebeast", 1800,
	{ "" }, FALSE, FALSE,
        { 0, 0, 0, 0, 0 },   { 22, 20, 20, 21, 20, 25 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	20, 100, 5,
	100,
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 21
    {
        "wolf",    "Beast",          "werebeast", 1800,
	{ "" }, FALSE, FALSE,
        { 0, 0, 0, 0, 0 },   { 20, 20, 20, 23, 20,25 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	20, 100, 5,
	100,
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 22
    {
        "boar",    "Beast",          "werebeast", 1800,
	{ "" }, FALSE, FALSE,
        { 0, 0, 0, 0, 0 },   { 21, 20, 20, 20, 22, 25 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	20, 100, 5,
	100,
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 23
    {
        "ocelot",    "Beast",          "werebeast", 1800,
	{ "" }, FALSE, FALSE,
        { 0, 0, 0, 0, 0 },   { 20, 20, 20, 22, 20, 25 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	20, 100, 5,
	100,
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 24
    {
        "fox",    "Beast",          "werebeast", 1800,
	{ "" }, FALSE, FALSE,
        { 0, 0, 0, 0, 0 },   { 20, 20, 20, 22, 20, 25 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	20, 100, 5,
	100,
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 25
    {
        "falcon",    "Beast",          "werebeast", 1800,
	{ "" }, FALSE, FALSE,
        { 0, 0, 0, 0, 0 },   { 20, 20, 20, 22, 20, 25 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	20, 100, 5,
	100,
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 26
    {
        "half-elf",          "H-Elf",        "human", 1750,
	{ "infravision", NULL }, TRUE, TRUE,
        { 0, 1, 0, 1, 0, 0}, { 19, 21, 20, 22, 18, 25 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	25, 130, 5,
	150,
        { "warrior", NULL, "ranger", "paladin", NULL, "thief", "ninja", NULL, "monk", NULL, NULL, NULL, NULL, NULL, NULL, NULL, "bard", "blademaster"}
    },
// 27
    {
        "faerie",          "Faeri",        "flying", 2000,
	{ "sneak", "fly", "steal", NULL}, TRUE, TRUE,
        { -4, 4, 0, 1, -4, 0 }, { 14, 25, 20, 23, 14, 25 }, SIZE_TINY, 
	ALIGN_GOOD,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	15, 200, 5,
	300,
        { NULL, NULL, NULL, NULL, NULL, "thief", NULL, NULL, NULL, "invoker", "battlemage", NULL, NULL, NULL, NULL, NULL, "bard", NULL, }
    },
// 28
    {
        "demon",         "Demon",        "unique", 2000,
	{ "charge", "enhanced damage", "infravision", NULL}, FALSE, TRUE,
        { 0, 1, 0, 2, -1, -1 }, { 23, 23, 18, 18, 22, 25 }, SIZE_LARGE, 
	ALIGN_EVIL,
	ETHOS_CHAOTIC,
	25, 170, 5,
	500,
        { "warrior", NULL, NULL, NULL, "dark-knight", NULL, NULL, NULL, NULL, NULL, "battlemage", "necromancer", "shaman", NULL, NULL, NULL}
    },
// 29
    {
        "ogre",      "Ogre ",        "giant", 1750,
	{"fast healing", "regeneration", NULL}, TRUE, TRUE,
        { 3,  0, -3, 0, 3, 0 }, { 24, 19, 15, 18, 24, 25 }, SIZE_LARGE,
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	55, 100, 5,
	60,
        { "warrior", "berserker", "ranger", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 30
    {
        "vampire",        "Undea",       "unique", 1500,
	{ "infravision", "vampiric touch", NULL }, FALSE, TRUE,
        { 1, -1, 0, 1, 0, 0 },   { 23, 20, 20, 22, 22, 25 }, SIZE_MEDIUM, 
	ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	30, 100, 5,
	9999,
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 31
    {
        "lich",        "Undea",       "unique", 1500,
	{ "infravision", NULL }, FALSE, TRUE,
        { 1, -1, 0, 1, 0, 0 },   { 23, 20, 20, 22, 20, 25 }, SIZE_MEDIUM, 
	ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	30, 100, 5,
	9999,
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    },
// 32
    {
        "goblin",          " Gob ",        "beast", 2000,
	{ "sneak", "steal", "plant", "pry", "plant", "pilfer", NULL}, TRUE, TRUE,
        { -4, 4, 0, 1, -4, 0 }, { 14, 25, 20, 23, 14, 25 }, SIZE_TINY, 
	ALIGN_GOOD,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	15, 200, 5,
	300,
        { NULL, NULL, NULL, NULL, NULL, "thief", NULL, NULL, NULL, "invoker", "battlemage", NULL, NULL, NULL, NULL, NULL, "bard", NULL, }
    },
// 33
    {
        "quickling",          "Qling",        "elf", 2000,
	{ "sneak", "steal", "plant", "pry", "plant", "pilfer", NULL}, TRUE, TRUE,
        { -4, 4, 0, 1, -4, 0 }, { 14, 25, 20, 23, 14, 25 }, SIZE_TINY, 
	ALIGN_GOOD,
	ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	15, 200, 5,
	300,
        { NULL, NULL, NULL, NULL, NULL, "thief", NULL, NULL, NULL, "invoker", "battlemage", NULL, NULL, NULL, NULL, NULL, "bard", NULL, }
    },
// last
    {
        NULL,    "null",          "none", 1500,
	{ "" }, FALSE, FALSE,
        { 6, 6, 6, 6, 6 },   { 18, 18, 18, 18, 18 }, SIZE_MEDIUM, 
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	ETHOS_LAWFUL | ETHOS_NEUTRAL | ETHOS_CHAOTIC,
	20, 100, 5,
	100,
        { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}
    }
};

const   struct  class_type      class_table     [MAX_CLASS]     =
{
// 0
    {
        "warrior", "War",  STAT_STR,  OBJ_VNUM_SCHOOL_SWORD,
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
        { 21059, 1193, 4130, 11696, 1272, 20694, 9633, 7970, 7640, 8215, 21522, 4788, 10918},  20,  -10,   9, 15, FALSE, FALSE, 0,
	&gcn_warrior,
	"warrior basics", "warrior default", {1,0,0,0,1,0}, "warrior"
    },
// 1
    {
        "berserker", "Ber",  STAT_STR,  OBJ_VNUM_SCHOOL_AXE,
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
        { 21079, 11695, 20697, 7972, 7634, 21525, 11906 },  20,  -10,  10, 16, FALSE, FALSE, 200,
	NULL,
	"warrior basics", "warrior default", {1,0,0,0,1,0}, "berserker"
    },
// 2
    {
        "ranger", "Ran",  STAT_STR,  OBJ_VNUM_SCHOOL_SPEAR,
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
        { 1199, 4129, 11417, 234, 20730, 7976, 8278, 4785, 11919},   20,  -8,  9, 13, FALSE, TRUE, 200,
	&gcn_ranger,
	"warrior basics", "warrior default", {0,0,0,1,1,0}, "ranger"
    },
// 3
    {
        "paladin", "Pal", STAT_STR,  OBJ_VNUM_SCHOOL_MACE,
	ALIGN_GOOD,
        { 11689, 20700, 8276, 4790 },   20,  -8,  8, 13, FALSE, TRUE, 350,
	&gcn_paladin,
	"warrior basics", "warrior default", {1,0,1,0,0,0}, NULL
    },
// 4
    {
        "dark-knight", "Dkn",  STAT_STR,  OBJ_VNUM_SCHOOL_POLEARM,
	ALIGN_EVIL,
        { 11697, 1274, 20706, 7974, 10937 },   20,  -8,  8, 13, FALSE, TRUE, 300,
	&gcn_dk,
	"warrior basics", "warrior default", {1,1,0,0,0,0}, NULL
    },
// 5
    {
        "thief", "Thi",  STAT_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
        { 21038, 1195, 4128, 11693, 1268, 20718, 4564, 9639,8280, 7998, 4786, 10912},  20,  -4,  8, 12, FALSE, FALSE, 0,
	NULL,
	"thief basics", "thief default", {0,0,0,1,1,0}, "thief"
    },
// 6
    {
        "ninja", "Nin",  STAT_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
        { 11694, 1276, 20722, 8280, 9710, 7980, 10924},  20,  -4,  8, 12, FALSE, TRUE, 300,
	&gcn_ninja,
	"thief basics", "thief default", {1,0,0,1,0,0}, "ninja"
    },
//7
    {
        "cleric", "Cle",  STAT_WIS,  OBJ_VNUM_SCHOOL_FLAIL,
	ALIGN_GOOD | ALIGN_EVIL,
        { 21056, 1197, 11687, 1260, 20751, 8299, 7894, 21528,4792 },  20, 2,  7, 10, TRUE, FALSE, 0,
	&gcn_cleric,
	"cleric basics", "cleric default", {0,0,1,1,0,0}, NULL
    },
// 8
    {
        "monk", "Mon",  STAT_DEX,  0,
	ALIGN_GOOD | ALIGN_EVIL,
        { 11686, 20733, 7978 },  20, -6,  8, 12, FALSE, TRUE, 300,
	&gcn_monk,
	"cleric basics", "cleric default", {1,0,1,0,0,0}, NULL
    
},
// 9
    {
        "invoker", "Inv",  STAT_INT,  OBJ_VNUM_SCHOOL_STAFF,
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
        { 4121, 11691, 1266, 20679, 9618, 7965, 8277, 4789, 10955},  20, 6,  8,  10, TRUE, FALSE, 0,
	NULL,
	"mage basics", "mage default", {0,1,1,0,0,0}, NULL
    },
// 10
    {
        "battlemage", "Bmg",  STAT_INT,  OBJ_VNUM_SCHOOL_WHIP,
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
	//4495,
        { 4118, 11690, 1262, 20682,7963, 8281,4791, 10949 },  20, -4,  8,  10, TRUE, FALSE, 0,
	NULL,
	"mage basics", "mage default", {0,1,0,1,0,0}, NULL
    },
// 11
    {
        "necromancer", "Nec",  STAT_INT,  OBJ_VNUM_SCHOOL_STAFF,
	ALIGN_EVIL,
        { 8024, 11699, 1270, 20703, 4576, 9708, 7898, 10943},  20, 6,  6,  8, TRUE, FALSE, 0,
	&gcn_necro,
	"mage basics", "mage default", {0,1,0,0,1,0}, NULL
    },
// 12
    {
        "psionicist", "Psi",  STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,
	ALIGN_GOOD | ALIGN_NEUTRAL | ALIGN_EVIL,
        {12602},  20, 6,  8,  10, TRUE, FALSE, 0,
	&gcn_psi,
	"mage basics", "mage default", {0,1,1,0,0,0}, "psionicist"
    },
// 13
    {
        "healer", "Hea",  STAT_WIS,  OBJ_VNUM_SCHOOL_MACE,
	ALIGN_GOOD,
        { 11688, 20685, 8279, 21532},  20, 5,  8, 10, TRUE, FALSE, 0,
	&gcn_healer,
	"cleric basics", "cleric default", {0,0,1,1,0,1}, NULL
    },
// 14
    {
        "shaman", "Sha",  STAT_WIS,  OBJ_VNUM_SCHOOL_FLAIL,
	ALIGN_EVIL,
        { 21054, 11400, 1264, 20767, 9599,7893},  20, 2,  8, 10, TRUE, FALSE, 0,
	&gcn_shaman,
	"cleric basics", "cleric default", {0,0,1,0,1,0}, NULL
    },
// 15
    {
	"vampire", "Vam",  STAT_DEX,  OBJ_VNUM_SCHOOL_SWORD,
	ALIGN_EVIL,
	{ 8390 }, 20, -11, 10, 14, FALSE, TRUE, 350,
	&gcn_vampire,
	"warrior basics", "warrior default", {1,1,0,0,0,0}, NULL
    },

// 16
    {
        "bard", "Bar", STAT_DEX,  OBJ_VNUM_SCHOOL_DAGGER,
	ALIGN_GOOD | ALIGN_NEUTRAL,
        { 1182, 11685, 9811, 20738, 21607, 4787 },   20,  0,  8, 12, FALSE, TRUE,  0,
	&gcn_bard,
	"warrior basics", "warrior default", {1,0,0,0,1,0}, NULL
    },
// 17
    {
	"druid", "Dru", STAT_WIS, OBJ_VNUM_SCHOOL_MACE,
	ALIGN_NEUTRAL,
	{ 6399 }, 20, -8, 9, 13, TRUE, FALSE, 300,
	&gcn_druid,
	"ranger basics", "ranger default", {0,0,0,1,1,0}, NULL
    },
// 18
    {
        "crusader", "Cru",  STAT_STR,  OBJ_VNUM_SCHOOL_POLEARM,
	ALIGN_GOOD,
        {0 },  20,  -10,   10, 15, FALSE, TRUE, 350,
	&gcn_crusader,
	"warrior basics", "warrior default", {1,0,0,0,1,0}, "crusader"
    },
//19
    {
        "adventurer", "Adv",  STAT_STR,  OBJ_VNUM_SCHOOL_SWORD,
	ALIGN_GOOD | ALIGN_EVIL | ALIGN_NEUTRAL,
        {0 },  20,  -10,   9, 15, FALSE, TRUE, 0,
	&gcn_adventurer,
	"warrior basics", "warrior default", {1,0,0,0,1,0}, NULL
    },
//20
    {
        "blademaster", "Bla",  STAT_STR,  OBJ_VNUM_SCHOOL_SWORD,
	ALIGN_GOOD | ALIGN_EVIL | ALIGN_NEUTRAL,
        {11235, 7944, 20815 },  20,  -15,   8, 14, FALSE, FALSE, 350,
	&gcn_blademaster,
	"warrior basics", "warrior default", {1,0,0,0,1,0}, "blademaster"
    },
//21
    {
        "lich", "Lic",  STAT_INT,  OBJ_VNUM_SCHOOL_STAFF,
	ALIGN_EVIL,
        {0},  20, 6,  8,  10, TRUE, FALSE, 0,
	NULL,
	"mage basics", "mage default", {0,1,1,0,0,0}, NULL
    },
//22
    {
        "conjurer", "Con",  STAT_INT,  OBJ_VNUM_SCHOOL_STAFF,
	ALIGN_GOOD | ALIGN_NEUTRAL,
        {0},  20, 6,  8,  10, TRUE, FALSE, 0,
	NULL,
	"mage basics", "mage default", {0,1,1,0,0,0}, NULL
    },
//23
    {
        "heretic", "Her",  STAT_INT,  OBJ_VNUM_SCHOOL_STAFF,
	ALIGN_EVIL,
        {0},  20, 6,  8,  10, TRUE, FALSE, 0,
	NULL,
	"mage basics", "mage default", {0,1,1,0,0,0}, NULL
    },
//24
    {
	NULL, "", 0, 0,
	0,
	{ 0 }, 0, 0, 0, 0, FALSE, FALSE, 0,
	NULL,
	"", "", {0,0,0,0,0,0}, NULL
    },
//25
    {
	NULL, "", 0, 0,
	0,
	{ 0 }, 0, 0, 0, 0, FALSE, FALSE, 0,
	NULL,
	"", "", {0,0,0,0,0,0}, NULL
    },
//26
    {
	NULL, "", 0, 0,
	0,
	{ 0 }, 0, 0, 0, 0, FALSE, FALSE, 0,
	NULL,
	"", "", {0,0,0,0,0,0}, NULL
    },
//27
    {
	NULL, "", 0, 0,
	0,
	{ 0 }, 0, 0, 0, 0, FALSE, FALSE, 0,
	NULL,
	"", "", {0,0,0,0,0,0}, NULL
    },
//28,
    {
	NULL, "", 0, 0,
	0,
	{ 0 }, 0, 0, 0, 0, FALSE, FALSE, 0,
	NULL,
	"", "", {0,0,0,0,0,0}, NULL
    },
//29
    {
	NULL, "", 0, 0,
	0,
	{ 0 }, 0, 0, 0, 0, FALSE, FALSE, 0,
	NULL,
	"", "", {0,0,0,0,0,0}, NULL
    },
//30
    {
	NULL, "", 0, 0,
	0,
	{ 0 }, 0, 0, 0, 0, FALSE, FALSE, 0,
	NULL,
	"", "", {0,0,0,0,0,0}, NULL
    },
//31
    {
	NULL, "", 0, 0,
	0,
	{ 0 }, 0, 0, 0, 0, FALSE, FALSE, 0,
	NULL,
	"", "", {0,0,0,0,0,0}, NULL
    },
};

/* The skill and spell table. Slot numbers must never be changed. */
#define SLOT(n) n
/* war, ber, ran, pal, dkn, thi, nin, cle, mon, inv, bmg, nec, psi, hea, sha, vam, bar, dru, crus, adv, con, her, blad, lich, 8xNULL*/

const   struct  skill_type      skill_table     [MAX_SKILL]     =
{
    /* Magic spells. */
    {
        "reserved",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 ,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 },
	0,                      TAR_IGNORE,             POS_STANDING,
	NULL,                   SPELL_OTHER,        0,      0,
	"",                     "",             "",
	GN_STATIC
    },
    {
        "acid blast",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 28, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10 ,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 },
	spell_acid_blast,       TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       20,     12,
        "acid blast",           "!Acid Blast!"                "",
	GN_STATIC
    },
    {
        "armor",
        {52, 52, 52,  2, 52, 52, 52,  1, 52,  5,  4,  5, 52,  1, 14, 52, 20, 52, 52, 19, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10,  1, 10,  1,  1,  1, 10,  1,  1, 10,  2, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 },
	spell_armor,            TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_PROTECTIVE,        5,     12,
	"",                     "You feel less armored.",       "",
	GN_BEN
    },
    {
        "bless",
        {52, 52, 52,  6, 52, 52, 52,  7, 52, 52, 52, 52, 52,  7, 21, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10, 10},
        spell_bless,            TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_PROTECTIVE,        5,     12,
        "",                     "You feel less righteous.",     "",
	GN_BEN
    },
    {
        "blindness",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 12, 52, 52, 24,  4, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10,  1,  1, 10, 10, 10, 10},
	spell_blindness,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_blindness,         SPELL_MALEDICTION,       25,     12,
	"",                     "You can see again.",   "$n can see once again.",
	GN_HAR
    },
    {
        "burning hands",
        {52, 52, 52, 52, 52, 52, 52, 52, 52,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10 , 10, 10, 10, 10, 10},
	spell_burning_hands,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       15,     12,
	"burning hands",        "!Burning Hands!",      "",
	GN_HAR
    },
    {
        "call lightning",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 21, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_call_lightning,   TAR_IGNORE,             POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       15,     12,
	"lightning bolt",       "!Call Lightning!",     "",
	GN_HAR
    },
    {   
      "calm",
        {52, 52, 52, 52, 52, 52, 52, 16, 52, 52, 52, 52, 52, 16, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_calm,             TAR_IGNORE,             POS_FIGHTING,
	NULL,                   SPELL_MENTAL,      30,     12,
	"",                     "You have lost your peace of mind.",    "",
	GN_HAR
    },
    {
        "cancellation",
        {52, 52, 52, 52, 52, 52, 52, 26, 52, 18, 20, 18, 52, 26, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1,  1,  1, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_cancellation,     TAR_IGNORE,     POS_STANDING,
	NULL,                   SPELL_RESTORATIVE,      20,     12,
        "",                     "!Cancellation!",       "",
	GN_HAR
    },
    {
        "cause critical",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  8, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_cause_critical,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       20,     12,
	"spell",                "!Cause Critical!",     "",
	GN_HAR
    },
    {
        "cause light",     
        {52, 52, 52, 52, 52, 52, 52,  1, 52, 52, 52, 52, 52, 52,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_cause_light,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       15,     12,
	"spell",                "!Cause Light!",        "",
	GN_HAR
    },
    {
        "cause serious",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  3, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_cause_serious,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       17,     12,
	"spell",                "!Cause Serious!",      "",
	GN_HAR
    },
    {   
        "chain lightning", 
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 33, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10 ,10, 10, 10, 10, 10},
	spell_chain_lightning,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,      25,     12,
	"lightning",            "!Chain Lightning!",    "",
	GN_HAR
    }, 
    {
        "change sex",      
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 , 10, 10, 10, 10, 10},
	spell_change_sex,       TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_MALEDICTION,       15,     12,
	"",                     "Your body feels familiar again.",      "$n's body returns to normal",
	GN_HAR
    },
    {
        "charm person",       
        {52, 52, 52, 52, 38, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10,  1, 10, 10, 10, 10},
	spell_charm_person,     TAR_CHAR_OFFENSIVE,     POS_STANDING,
	&gsn_charm_person,      SPELL_MENTAL,        5,     12,
	"",                     "You feel more self-confident.",        "",
	GN_HAR
    },
    {
        "chill touch",        
        {52, 52, 52, 52,  3, 52, 52, 52, 52, 52,  2,  1, 52, 52, 52,  3, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10,  1, 10, 10, 10, 10},
	spell_chill_touch,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       15,     12,
	"chilling touch",       "You feel less cold.",  "",
	GN_HAR
    },
    {
        "colour spray",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 16, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_colour_spray,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       15,     12,
	"colour spray",         "!Colour Spray!",       "",
	GN_HAR
    },
    {
        "continual light", 
        {52, 52, 52, 52, 52, 52, 52,  4, 52, 52,  52, 52, 52,  4, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10,  10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_continual_light,  TAR_IGNORE,             POS_STANDING,
	NULL,                   SPELL_OTHER,        7,     12,
	"",                     "!Continual Light!",    "",
	GN_HAR
    },
    {
        "control weather", 
        {52, 52, 19, 52, 52, 52, 52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52, 19, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10,  1, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
	spell_control_weather,  TAR_IGNORE,             POS_STANDING,
	NULL,                   SPELL_OTHER,       25,     12,
	"",                     "!Control Weather!",    "",
	GN_STATIC
    },
    {
        "create food",     
        {52, 52, 52,  3, 52, 52, 52,  5, 52, 10, 52, 52, 52,  5, 52, 52, 52, 23, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10,  1, 10,  1, 10, 10, 10,  1, 10, 10, 10,  1, 10, 10},
	spell_create_food,      TAR_IGNORE,             POS_STANDING,
	NULL,                   SPELL_OTHER,        25,     24,
	"",                     "!Create Food!",        "",
	GN_STATIC
    },
    {
        "create spring",   
        {52, 52, 52, 52, 52, 52, 52, 17, 52, 14, 52, 52, 52, 17, 52, 52, 52, 23, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1, 10, 10, 10,  1, 10, 10, 10,  1, 10, 10},
	spell_create_spring,    TAR_IGNORE,             POS_FIGHTING,
	NULL,                   SPELL_OTHER,       20,     12,
	"",                     "!Create Spring!",      "",
	GN_STATIC
    },
    {
        "create water",    
        {52, 52, 52,  4, 52, 52, 52,  3, 52,  8, 52, 52, 52,  3, 52, 52, 52, 52, 52, 11, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10,  1, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1},
	spell_create_water,     TAR_OBJ_INV,            POS_STANDING, 
	NULL,                   SPELL_OTHER,        5,     12,
	"",                     "!Create Water!",       "",
	GN_STATIC
    },
    {
        "cure blindness",  
        {52, 52, 52, 16, 52, 52, 52,  3, 52, 52, 52, 52, 52,  3, 21, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10, 10},
	spell_cure_blindness,   TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_RESTORATIVE,        5,     12,
	"",                     "!Cure Blindness!",     "",
	GN_STATIC
    },
    {
        "cure critical",   
        {52, 52, 52, 19, 52, 52, 52,  9, 52, 52, 52, 52, 52,  9, 21, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10, 10},
	spell_cure_critical,    TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_CURATIVE,       20,     12,
	"",                     "!Cure Critical!",      "",
	GN_STATIC
    },
    {
        "cure disease",    
        {52, 52, 52, 23, 52, 52, 52, 13, 52, 52, 52, 52, 52, 13, 26, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10, 10},
	spell_cure_disease,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_RESTORATIVE,      20,     12,
	"",                     "!Cure Disease!",       "",
	GN_STATIC
    },
    {
        "cure light",      
        {52, 52, 52,  1,  6, 52,  8, 52, 52, 52, 52, 52, 52, 52,  3, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1,  1, 10,  1, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_cure_light,       TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
	&gsn_cure_light,                   SPELL_CURATIVE,       10,     12,
	"",                     "!Cure Light!",         "",
	GN_STATIC
    },
    {
        "cure poison",     
        {52, 52, 52, 26, 52, 52, 52, 14, 52, 52, 52, 52, 52, 14, 29, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10, 10},
	spell_cure_poison,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_RESTORATIVE,        5,     12,
	"",                     "!Cure Poison!",        "",
	GN_STATIC
    },
    {
        "cure serious",    
        {52, 52, 52, 10, 52, 52, 52,  2, 52, 52, 52, 52, 52,  2, 11, 52, 35, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1,  1, 10,  2, 10, 10, 10},
	spell_cure_serious,     TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_CURATIVE,       15,     12,
	"",                     "!Cure Serious!",       "",
	GN_STATIC
    },
    {
        "curse",           
        {52, 52, 52, 52,  8, 52, 52, 30, 52, 52, 52, 10, 52, 30, 15,  8, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10,  1, 10, 10,  1, 10, 10, 10,  1, 10,  1,  1,  1, 10, 10, 10, 10},
        spell_curse,            TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_curse,             SPELL_MALEDICTION,       20,     12,
        "curse",                "The curse wears off.", "",
	GN_HAR
    },
    {
        "demonfire",       
        {38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_demonfire,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,      15,     PULSE_VIOLENCE,
	"torments",             "The black flames about you die off.",          "The black flames around $n die off.",
	GN_BEN
    },  
    {
        "detect evil",        
        {52, 52, 52,  1, 52, 52, 52,  4, 52, 52, 52, 52, 52,  4, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_detect_evil,      TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SPELL_OTHER,        5,     12,
	"",                     "The red in your vision disappears.",   "",
	GN_BEN
    },
    {
        "detect good",       
        {52, 52, 52, 52,  5, 52, 52,  4, 52, 52, 52,  5, 52, 52,  4,  5, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10,  1, 10, 10,  10, 10, 10, 10,  1, 10, 10,  1,  1, 10, 10, 10, 10},
	spell_detect_good,      TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SPELL_OTHER,        5,     12,
	"",                     "The gold in your vision disappears.",  "",
	GN_BEN
    },
    {
        "detect invis",       
        {52, 52, 52,  8, 10, 52,  7,  8, 52,  3,  6,  3, 52,  8,  6, 10, 30, 22, 52, 15, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1,  1, 10,  1,  1, 10,  1,  1,  1, 10,  1,  1,  1,  2,  1, 10,  1},
	spell_detect_invis,     TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SPELL_OTHER,        5,     12,
        "",        "You no longer see invisible objects.",       "",
	GN_BEN
    },
    {
        "detect magic",      
        {52, 52, 52, 52, 52, 52, 52,  6, 52,  2,  1,  2, 52,  6,  4, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1,  1,  1, 10,  1,  1, 10, 10, 10, 10, 10},
	spell_detect_magic,     TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SPELL_OTHER,        5,     12,
        "",        "You can no longer sense magical auras.",     "",
	GN_BEN
    },
    {
        "detect traps",     
        {52, 52, 52, 11, 52, 52, 52, 11, 52, 52, 52, 52, 52, 11, 52, 52, 52, 37, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1, 10, 10, 10,  1, 10, 10},
	spell_detect_traps,    TAR_CHAR_SELF,            POS_STANDING,
	&gsn_detect_traps,                   SPELL_OTHER,       55,     2 * PULSE_VIOLENCE,
	"",                     "You can no longer detect traps.",      "",
	GN_BEN
    },
    {
        "dispel evil",       
        {52, 52, 52, 15, 52, 52, 52, 31, 52, 52, 52, 52, 52, 31, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_dispel_evil,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       15,     12,
	"dispel evil",          "!Dispel Evil!",        "",
	GN_HAR
    },
    {
        "dispel good",       
        {52, 52, 52, 52, 25, 52, 52, 31, 52, 52, 52, 52, 52, 52, 15, 25, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10},
	spell_dispel_good,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,      15,     12,
	"dispel good",          "!Dispel Good!",        "",
	GN_HAR
    },
    {
        "dispel magic",      
        {52, 52, 52, 52, 52, 52, 52, 24, 52, 19, 20, 14, 52, 24, 19, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1,  1,  1, 10,  1,  1, 10, 10, 10, 10, 10},
	spell_dispel_magic,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_dispel_magic,                   SPELL_MENTAL,       15,     12,
	"",                     "!Dispel Magic!",       "",
	GN_HAR | GN_PSI_WEE
    },
    {
        "earthquake",       
        {52, 52, 52, 52, 52, 52, 52, 15, 52, 52, 15, 52, 52, 52, 52, 52, 52, 41, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10,  1, 10, 10},
	spell_earthquake,       TAR_IGNORE,             POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       15,     12,
	"earthquake",           "!Earthquake!",         "",
	GN_STATIC
    },
    {
        "enchant armor",      
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 16, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_enchant_armor,    TAR_OBJ_INV,            POS_STANDING,
	NULL,                   SPELL_OTHER,      125,    24,
	"",                     "!Enchant Armor!",      "",
	GN_STATIC
    },
    {
        "enchant weapon",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 17, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_enchant_weapon,   TAR_OBJ_INV,            POS_STANDING,
	NULL,                   SPELL_OTHER,       125,    24,
	"",                     "!Enchant Weapon!",     "",
	GN_STATIC
    },
    {
        "energy drain",     
        {52, 52, 52, 52, 16, 52, 52, 52, 52, 52, 52, 19, 52, 52, 52, 16, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10},
	spell_energy_drain,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_energy_drain,                   SPELL_MALEDICTION,       35,     12,
	"energy drain",         "!Energy Drain!",       "",
	GN_STATIC
    },
    {
        "faerie fire",       
        {52, 52, 52, 52, 52, 52, 52,  7, 52,  6,  12,  6, 3,  7,  2, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1,  1,  1,  1,  1,  1, 10, 10, 10, 10, 10},
	spell_faerie_fire,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_MALEDICTION,        5,     12,
        "faerie fire",   "The pink aura around you fades away.",    "The pink aura around $n fades away.",
	GN_HAR
    },
    {
        "faerie fog",        
        {52, 52, 52, 52, 52, 52, 52, 21, 52, 14, 52, 17,  9, 21, 17, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1, 10,  1,  1,  1,  1, 10, 10, 10, 10, 10},
	spell_faerie_fog,       TAR_IGNORE,             POS_STANDING,
        NULL,                   SPELL_MALEDICTION,       5,     18,
        "faerie fog",           "You are no longer glowing.",         "$n is no longer glowing.",
	GN_HAR
    },
    {
        "fireball",     
        {52, 52, 52, 52, 27, 52, 52, 52, 52, 22, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10,  1, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
	spell_fireball,         TAR_IGNORE,     	POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       15,     12,
	"fireball",             "!Fireball!",           "",
	GN_STATIC
    },
    {
        "flamestrike",     
        {52, 52, 52, 10, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_flamestrike,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       20,     12,
	"flamestrike",          "",                "",
	GN_HAR
    },
    {
        "fly",                 
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 18, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_fly,              TAR_CHAR_DEFENSIVE,     POS_STANDING,
	&gsn_fly,                   SPELL_PROTECTIVE,       10,     18,
	"",                     "You slowly float to the ground.",      "$n slowly floats to the ground.",
	GN_BEN
    },
    {
        "frenzy",           
        {52, 52, 52, 52, 52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_frenzy,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_OTHER,      30,     2 * PULSE_VIOLENCE,
	"",                     "Your rage ebbs.",      "",
	GN_BEN
    },
    {
        "gate",              
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 25, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_gate,             TAR_IGNORE,             POS_STANDING,
	NULL,                   SPELL_MENTAL,       80,     12,
	"",                     "You can once again abduct someone.",               "",
	GN_STATIC
    },
    {
        "giant strength",    
        {52, 52, 52, 52, 52, 52, 52,  6, 52, 52, 52, 52,  7, 52, 18, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10,  10, 10,  1, 10,  1, 10, 10, 10, 10, 10},
	spell_giant_strength,   TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_CURATIVE,       20,     12,
	"",                     "You feel weaker.",     "",
	GN_BEN | GN_PSI
    },
    {
        "harm",               
        {52, 52, 52, 52, 29, 52, 52, 52, 52, 52, 52, 52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10},
	spell_harm,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_harm,                   SPELL_AFFLICTIVE,       35,     12,
	"harm spell",           "",               "",
	GN_STATIC
    },
    {
        "haste",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 34, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_haste,            TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_CURATIVE,      30,     12,
	"",                     "You feel yourself slow down.", "$n slows down.",
	GN_BEN
    },
    {
        "heal",              
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 21, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10, 10},
	spell_heal,             TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
	&gsn_heal,                   SPELL_CURATIVE,       35,     12,
	"",                     "!Heal!",               "",
	GN_STATIC
    },
    {
        "holy word",           
        {52, 52, 52, 40, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_holy_word,        TAR_CHAR_SELF,     POS_FIGHTING,
	NULL,                   SPELL_OTHER,      25,    2 * PULSE_VIOLENCE,
	"divine wrath",         "The wrath of the Divine Word leaves your mind.",          "",
	GN_BEN
    },
    {
        "identify",           
        {52, 52, 52, 52, 52, 52, 52, 16, 52, 15, 25, 15,  7, 16, 22, 52, 10, 46, 52, 13},
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1,  1,  1,  1,  1,  1, 10,  2,  1, 10,  1},
	spell_identify,         TAR_IGNORE,            POS_STANDING,
	&gsn_identify,          SPELL_OTHER,       12,     24,
	"",                     "",           "",
	GN_STATIC
    },
    {
        "infravision",        
        {52, 52, 52, 52, 52, 52, 52, 23, 52,  9,  18,  9, 52, 23,  9, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1,  1,  1, 10,  1,  1, 10, 10, 10, 10, 10},
	spell_infravision,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_OTHER,        5,     18,
	"",                     "You no longer see in the dark.",       "",
	GN_BEN
    },
    {
        "invisibility",      
        {52, 52, 52, 52,  9, 52,  7, 52, 52,  4,  7,  4, 52, 52, 52,  9, 40, 52, 52, 21},
        {10, 10, 10, 10,  1, 10,  1, 10, 10,  1,  1,  1, 10, 10, 10,  1,  2, 10, 10,  1},
        spell_invis,            TAR_CHAR_DEFENSIVE,     POS_STANDING,
	&gsn_invis,             SPELL_PROTECTIVE,        5,     12,
        "",                     "You are no longer invisible.",        "$n fades into existence.",
	GN_BEN
    },
    {
        "know alignment",     
        {52, 52, 52,  4, 52, 52, 52,  9, 52, 12, 52, 12, 52,  9, 52, 52, 52, 52, 52, 52},
        {10, 10, 10,  1, 10, 10, 10,  1, 10,  1, 10,  1, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_know_alignment,   TAR_IGNORE,     POS_FIGHTING,
	NULL,                   SPELL_OTHER,        9,     PULSE_VIOLENCE,
	"",                     "!Know Alignment!",     "",
	GN_BEN
    },
    {
        "lightning bolt",      
        {52, 52, 52, 52, 13, 52, 52, 52, 52, 10, 52, 52, 52, 52, 52, 52, 52, 39, 52, 24},
        {10, 10, 10, 10,  1, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10,  1, 10,  1},
	spell_lightning_bolt,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       15,     12,
	"lightning bolt",       "!Lightning Bolt!",     "",
	GN_STATIC
    },
    {
        "locate object",     
        {52, 52, 52, 52, 52, 52, 52, 15, 52, 15, 24, 15, 11, 15, 22, 52, 15, 46, 52, 52},
        {10, 10, 10,  1, 10, 10, 10,  1, 10,  1,  1,  1,  1,  1,  1, 10,  2,  1, 10, 10},
	spell_locate_object,    TAR_IGNORE,             POS_STANDING,
	NULL,                   SPELL_OTHER,       20,     18,
	"",                     "!Locate Object!",      "",
	GN_STATIC
    },
    {
        "magic missile",       
        {52, 52, 52, 52,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  1, 25, 52, 52, 17},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1,  2, 10, 10,  1},
	spell_magic_missile,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       15,     12,
	"magic missile",        "!Magic Missile!",      "",
	GN_STATIC
    },
    {
        "mass healing",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 38, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_mass_healing,     TAR_IGNORE,             POS_FIGHTING,
	NULL,                   SPELL_CURATIVE,      100,    36,
	"",                     "!Mass Healing!",       "",
	GN_STATIC
    },
    {
        "mass invis",        
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 22, 52, 52, 36, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_mass_invis,       TAR_IGNORE,             POS_STANDING,
	&gsn_mass_invis,        SPELL_PROTECTIVE,       20,     24,
	"",                     "You are no longer invisible.",         "",
	GN_BEN
    },
    {
        "pass door",      
        {52, 52, 52, 52, 52, 52, 52, 29, 52, 24, 36, 24, 52, 29, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1,  1,  1, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_pass_door,        TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SPELL_OTHER,       20,     12,
	"",                     "You feel solid again.",        "$n phases back into existence.",
	GN_BEN
    },
    {
        "plague",               
        {52, 52, 52, 52, 25, 52, 52, 52, 52, 52, 52, 20, 52, 52, 16, 52, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10,  1, 10, 10,  1,  1, 10, 10, 10, 10},
	spell_plague,           TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_plague,            SPELL_MALEDICTION,      20,     12,
	"sickness",             "Your sores vanish.",   "$n's sores vanish.",
	GN_HAR
    },
    {
        "poison",           
        {52, 52, 52, 52,  6, 52, 52, 52, 52, 52, 52, 12, 52, 52,  7,  52, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10,  1, 10, 10,  1,  1, 10, 10, 10, 10},
        spell_poison,           TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_poison,            SPELL_MALEDICTION,       15,     12,
        "poison",               "You feel less sick.",   "$n looks less sickly.",
	GN_HAR
    },
    {
        "protection",   
        {52, 52, 52, 11,  9, 52, 52, 11, 52, 52, 52, 10, 52, 11, 10,  9, 52, 52, 52, 52},
        {10, 10, 10,  1,  1, 10, 10,  1, 10, 10, 10,  1, 10,  1,  1,  1, 10, 10, 10, 10},
	spell_protection,	TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SPELL_PROTECTIVE,       35,      12,
	"",                     "You feel less protected.",     "",
	GN_BEN
    },
    {
        "protection evil",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_protection_evil,  TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SPELL_PROTECTIVE,       5,      12,
	"",                     "You feel less protected.",     "",
	GN_BEN
    },
    {
        "protection good",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_protection_good,  TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SPELL_PROTECTIVE,       5,     12,
	"",                     "You feel less protected.",     "",
	GN_BEN
    },
    {
        "ray of truth",        
        {52, 52, 52, 52, 52, 52, 52, 40, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_ray_of_truth,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_MENTAL,      40,     12,
	"ray of truth",         "!Ray of Truth!",       "",
	GN_STATIC
    },
    {
        "refresh",            
        {52, 52, 52, 52, 52, 52, 52,  5, 52, 52, 52, 52, 52,  5, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_refresh,          TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_CURATIVE,       12,     18,
	"refresh",              "!Refresh!",            "",
	GN_STATIC
    },
    {
        "remove curse",      
        {52, 52, 52, 18, 52, 52, 52, 14, 52, 52, 52, 52, 52, 14, 30, 52, 52, 52, 52, 27},
        {10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10,  1},
	spell_remove_curse,     TAR_IGNORE,       POS_STANDING,
	NULL,                   SPELL_RESTORATIVE,       95,     2 * PULSE_VIOLENCE,
	"",                     "!Remove Curse!",       "",
	GN_STATIC
    },
    {
        "sanctuary",           
        {52, 52, 52, 27, 52, 52, 52, 20, 52, 35, 52, 52, 52, 20, 26, 52, 52, 52, 52, 30},
        {10, 10, 10,  1, 10, 10, 10,  1, 10,  1, 10, 10, 10,  1,  2, 10, 10, 10, 10,  1},
	spell_sanctuary,        TAR_CHAR_DEFENSIVE,     POS_STANDING,
	&gsn_sanctuary,         SPELL_PROTECTIVE,       75,     12,
        "",             "The white aura around your body fades.",   "The white aura around $n fades.",
	GN_BEN
    },
    {
        "shield",              
        {52, 52, 52, 52, 52, 52, 52, 25, 52, 20, 22, 52, 52, 25, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1,  1, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_shield,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_PROTECTIVE,       12,     18,
        "",         "Your force shield shimmers then fades away.",   "$n's force shield shimmers then fades away.",
	GN_BEN
    },
    {
        "shocking grasp",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  10, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_shocking_grasp,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,       15,     12,
	"shocking grasp",       "!Shocking Grasp!",     "",
	GN_STATIC
    },
    {
        "sleep",               
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 10, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_sleep,            TAR_CHAR_OFFENSIVE,     POS_STANDING,
	&gsn_sleep,             SPELL_MENTAL,       15,     12,
	"",                     "You can wake up now.", "",
	GN_HAR
    },
    {
        "slow",           
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 34, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_slow,             TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_MALEDICTION,      30,     18,
	"",                     "You feel yourself speed up.",  "$n is moving faster.",
	GN_HAR
    },
    {
        "stone skin",       
        {52, 52, 52, 52, 52, 52, 52, 35, 52, 52, 25, 52, 52, 34, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10,  1, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_stone_skin,       TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SPELL_PROTECTION,       12,     18,
	"",                     "Your skin feels soft again.",  "$n's skin returns to normal.",
	GN_HAR
    },
    {
        "summon",            
        {52, 52, 52, 21, 21, 52, 52, 12, 52, 52, 52, 27, 52, 12, 11, 52, 52, 52, 52, 52},
        {10, 10, 10,  1,  1, 10, 10,  1, 10, 10, 10,  1, 10,  1,  1, 10, 10, 10, 10, 10},
	spell_summon,           TAR_IGNORE,             POS_STANDING,
	NULL,                   SPELL_MENTAL,       50,     12,
	"",                     "!Summon!",             "",
	GN_STATIC
    },
    {
        "teleport",         
        {52, 52, 52, 52, 23, 52, 52, 52, 52, 13, 52, 13, 52, 52, 24,  7, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10,  1, 10,  1, 10, 10,  1,  1, 10, 10, 10, 10},
	spell_teleport,         TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SPELL_OTHER,       35,     12,
	"",                     "!Teleport!",           "",
	GN_STA
    },
    {
        "ventriloquate",      
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  4, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10},
	spell_null,    TAR_IGNORE,             POS_STANDING,
	NULL,                   SPELL_MENTAL,        5,     12,
	"",                     "!Ventriloquate!",      "",
	GN_STA
    },
    {
        "weaken",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 11, 52, 52,  9, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_weaken,           TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_weaken,                   SPELL_MALEDICTION,       20,     12,
	"spell",                "You feel stronger.",   "",
	GN_HAR
    },
    {
        "word of recall",     
        {52, 52, 52, 52, 52, 52, 52, 27, 52, 25, 35, 31, 27, 27, 29, 52, 45, 52, 52, 52},
        {10, 10, 10,  1, 10, 10, 10,  1, 10,  1,  1,  1,  1,  1,  1, 10,  2, 10, 10, 10},
	spell_word_of_recall,   TAR_CHAR_DEFENSIVE,     POS_RESTING,
	NULL,                   SPELL_PROTECTIVE,        35,     12,
	"",                     "You feel the effects of Word of Recall recede",     "",
	GN_STA
    },
    {
        "acid breath",      
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_acid_breath,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,      0,     4,
	"blast of acid",        "!Acid Breath!",        "",
	GN_STA
    },
    {
        "fire breath",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_fire_breath,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,      0,     4,
	"blast of flame",       "The smoke leaves your eyes.",  "$n is no longer blinded.",
	GN_HAR
    },
    {
        "frost breath",      
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_frost_breath,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,      0,     4,
	"blast of frost",       "!Frost Breath!",       "",
	GN_HAR
    },
    {
        "gas breath",         
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_gas_breath,       TAR_IGNORE,             POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,      0,     4,
	"blast of gas",         "!Gas Breath!",         "",
	GN_HAR
    },
    {
        "lightning breath",    
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_lightning_breath, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,      0,    4,
	"blast of lightning",   "!Lightning Breath!",   "",
	GN_HAR
    },
    {
        "retreat",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,      0,      12,
	"!retreat!", "!retreat!",       "",
	GN_STA
    },
    {
        "melee",      
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_melee,                   SPELL_OTHER,    30,      2 * PULSE_VIOLENCE,
	"groin kick",  "You will no longer melee.",        "",
	GN_STA
    },
    {
        "hurricane",      
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 52, 52, 52, 52, 52, 52, 52, 47, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
	spell_hurricane,        TAR_IGNORE,     POS_FIGHTING,
	&gsn_thrash,            SPELL_AFFLICTIVE,      35,      2 * PULSE_VIOLENCE,
	"hurricane",            "You can fly again.",      "$n can fly again.",
	GN_HAR
    },
    {
        "arms of gaia",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 36, 52, 52},
        {10, 10, 10,  10, 10, 10, 10,  10, 10, 10, 10, 10, 10,  10,  10, 10,  10,  1, 10, 10},
        spell_arms_of_gaia,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,      SPELL_PROTECTIVE,       75,     12,
        "",             "The Arms of Gaia no longer protect you.",   "$n looks less at peace with the Earth Mother.",
	GN_BEN
    },    
    /* combat and weapons skills */
    {
        "axe",                
        { 1,  1,  1, 52,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  1, 52, 52,  1, 52, 52},
        { 3,  2,  3, 10,  4, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  4, 10, 10,  1, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_axe,               SPELL_OTHER,       0,      0,
	"",                     "!Axe!",                "",
	GN_STA
    },
    {
        "dagger",           
        { 1,  1,  1, 52,  1,  1,  1, 52,  1,  1,  1,  1,  1, 52, 52,  1,  1,  1,  6,  1,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 2,  3,  2, 10,  2,  2,  2, 10,  3,  4,  2,  4,  4, 10, 10,  2,  4,  2,  4,  3,  2, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_dagger,            SPELL_OTHER,       0,      0,
	"",                     "!Dagger!",             "",
	GN_STA
    },
    {
        "flail",               
        { 1,  1,  1, 52,  1, 52, 52,  1, 52, 52, 52, 52, 52,  1,  1,  1, 52, 52, 6, 52},
        { 2,  3,  2, 10,  3, 10, 10,  3, 10, 10, 10, 10, 10,  4,  3,  2, 10, 10, 1, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_flail,             SPELL_OTHER,       0,      0,
	"",                     "!Flail!",              "",
	GN_STA
    },
    {
        "mace",            
        { 1,  1, 52,  1,  1,  1, 52,  1, 52,  1, 7, 52, 52, 1,  1,  1,  1,  1, 6,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 2,  3, 10,  2,  3,  5, 10,  2, 10,  5, 4, 10, 10, 4,  2,  3,  4,  2, 2,  3, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_mace,              SPELL_OTHER,       0,      0,
	"",                     "!Mace!",               "",
	GN_STA
    },
    {
        "polearm",         
        { 1,  1, 52,  1,  1, 52, 52, 52,  1, 52, 52, 52, 52, 52, 52,  1, 52, 52, 1, 52,  2},
        { 2,  3, 10,  2,  2, 10, 10, 10,  7, 10, 10, 10, 10, 10, 10,  2, 10, 10, 1, 10,  3},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_polearm,           SPELL_OTHER,       0,      0,
	"",                     "!Polearm!",            "",
	GN_STA
    },
    {
        "shield block",    
        { 1,  1,  9,  1,  1, 52, 52, 21, 52, 52, 52, 52, 52, 21, 15,  1, 52, 18, 52, 10},
        { 2,  3,  2,  4,  4, 10, 10,  5, 10, 10, 10, 10, 10,  5,  5,  4, 10,  4, 10,  4},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_shield_block,      SPELL_OTHER,        0,      0,
	"",                     "!Shield!",             "",
	GN_STA
    },
    {
        "spear",              
        { 1,  1,  1, 52,  1,  1,  1, 52,  1, 52, 52, 52, 52, 52, 52,  1, 52,  1, 1, 52,  3},
        { 3,  3,  2, 10,  3,  4,  3, 10,  5, 10, 10, 10, 10, 10, 10,  3, 10,  3, 3, 10,  3},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_spear,             SPELL_OTHER,       0,      0,
	"",                     "!Spear!",              "",
	GN_STA
    },
    {
        "sword",            
        { 1,  1,  1,  1,  1,  1,  1, 52,  1, 52,  5, 52, 52, 52, 52,  1, 1,  1,  1,  1,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 2,  3,  2,  3,  3,  3,  3, 10,  4, 10,  4, 10, 10, 10, 10,  3, 4,  3,  3,  3,  2, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_sword,             SPELL_OTHER,       0,      0,
        "",                     "!Sword!",              "",
	GN_STA
    },
    {
        "whip",              
        { 1, 52,  1, 52,  1, 52, 52,  1,  1, 52,  1,  1,  1,  1,  1,  1, 52, 52, 6, 52},
        { 2, 10,  2, 10,  3, 10, 10,  3,  3, 10,  4,  4,  4,  4,  3,  3, 10, 10,11, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_whip,              SPELL_OTHER,       0,      0,
	"",                     "!Whip!",       "",
	GN_STA
    },
    {
        "backstab",            
        {52, 52, 52, 52, 52,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_backstab,          SPELL_OTHER,        0,     24,
	"backstab",             "!Backstab!",           "",
	GN_STA
    },
    {
        "bash",             
        {15, 52, 52, 52, 15, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 1, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_bash,              SPELL_OTHER,       0,      24,
	"bash",                 "!Bash!",               "",
	GN_STA
    },
    {
        "berserk",         
        {18, 16, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 16, 52},
        { 1,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_berserk,           SPELL_OTHER,       0,      24,
	"",            "You feel your pulse slow down.",       "",
	GN_STA
    },
    {
        "dirt kicking",        
        { 3,  8,  5, 52,  3,  3,  3, 52,  5, 52, 52, 52, 52, 52, 52, 52, 1,  8, 52,  3, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 1,  1,  1, 10,  1,  1,  1, 10,  1, 10, 10, 10, 10, 10, 10, 10, 3,  1, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_dirt,              SPELL_OTHER,       0,      24,
	"kicked dirt",     "You rub the dirt out of your eyes.",   "$n rubs the dirt out of $s eyes.",
	GN_STA
    },
    {
        "disarm",            
        {10, 52, 11, 10, 11, 12, 13, 52, 11, 52, 52, 52, 52, 52, 52, 11, 14, 52, 52, 29,  11},
        { 1, 10,  1,  1,  1,  1,  1, 10,  1, 10, 10, 10, 10, 10, 10,  1,  1, 10, 10,  1,   1},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_disarm,            SPELL_OTHER,        0,     24,
	"",                     "!Disarm!",             "",
	GN_STA
    },
    {
        "dodge",             
        {17, 21, 17, 24, 23,  1,  17, 52,  1, 52, 52, 52, 52, 52, 52, 14, 7, 17, 24, 20, 20},
        { 2,  4,  2,  3,  3,  1,  2, 10,  2, 10, 10, 10, 10, 10, 10,  4,  4,  4, 4,   4,  1},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_dodge,             SPELL_OTHER,        0,     0,
	"",                     "!Dodge!",              "",
	GN_STA
    },
    {
        "enhanced damage",     
        { 1,  1, 13,  4,  5, 23, 11, 30, 52, 52, 52, 52, 52, 52, 52,  5, 35, 52,  1, 25},
        { 2,  1,  3,  3,  3,  4,  4,  5, 10, 10, 10, 10, 10, 10, 10,  3,  4, 10, 1,   4},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_enhanced_damage,   SPELL_OTHER,        0,     0,
	"",                     "You are no longer ignoring combat resistances.",    "",
	GN_STA
    },
    {
        "envenom",            
        {52, 52, 52, 52, 52, 35, 33, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10,  2,  4, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_RESTING,
	&gsn_envenom,           SPELL_OTHER,        0,      2 * PULSE_VIOLENCE,
	"",                     "!Envenom!",            "",
	GN_STA
    },
    {
        "hand to hand",       
        { 6,  1, 14,  7, 14, 15,  1, 25,  1, 23, 25, 20,  1, 25, 10, 14, 9,  1, 9,  6,  8},
        { 1,  1,  1,  1,  1,  1,  1,  2,  1,  2,  2,  2,  2,  2,  2,  1, 1,  1, 6,  2,  1},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_hand_to_hand,      SPELL_OTHER,       0,      0,
	"",                     "!Hand to Hand!",       "",
	GN_STA
    },
    {
        "kick",              
        { 8, 52,  8, 52,  9, 14,  1, 12,  6, 52, 15, 52, 52, 52,  7,  9, 7,  7, 52,  8},
        { 1, 10,  1, 10,  1,  1,  1,  2,  1, 10,  3, 10, 10, 10,  2,  1,  1, 1, 10,  1},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_kick,              SPELL_OTHER,        0,     24,
	"kick",                 "!Kick!",               "",
	GN_STA
    },
    {
        "parry",              
        { 1,  5,  1,  6,  6, 13, 1, 27, 52, 19,   9, 17, 19, 27, 17,  6,  2, 6, 5,  1,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 2,  3,  1,  3,  3,  3, 3,  4, 10,  4,   4,  4,  4,  4,  4,  3,  3, 3, 4,  4,  2, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_parry,             SPELL_OTHER,        0,     0,
	"",                     "!Parry!",              "",
	GN_STA
    },
    {
        "rescue",              
        { 1, 52, 52,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 1, 52},
        { 1, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_rescue,            SPELL_OTHER,        0,     12,
	"",                     "!Rescue!",             "",
	GN_STA
    },
    {
        "trip",               
        {15, 15, 52, 52, 15, 15, 15, 52, 15, 52, 52, 52, 52, 52, 52, 15, 15, 52, 52, 52, 16},
        { 1,  1, 10, 10,  1,  1,  1, 10,  1, 10, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10,  1},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_trip,              SPELL_OTHER,       0,      24,
	"trip",                 "!Trip!",               "",
	GN_STA
    },
    {
        "second attack",      
        { 5,  7,  8,  6,  7, 12,  9, 26, 52, 28, 52, 26, 28, 26, 23,  7, 11, 22,  8,  5},
        { 3,  4,  3,  3,  3,  4,  3,  5, 10,  6, 10,  6,  5,  5,  5,  3, 6,  1,   3,  4},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_second_attack,     SPELL_OTHER,        0,     0,
	"",                     "!Second Attack!",      "",
	GN_STA
    },
    {
        "third attack",        
        {12, 19, 25, 17, 18, 25, 18, 52, 52, 52, 52, 52, 52, 52, 52, 13, 25, 52, 22, 18},
        { 3,  4,  3,  4,  4,  5,  3, 10, 10, 10, 10, 10, 10, 10, 10,  4,  4, 10, 3,   4},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_third_attack,      SPELL_OTHER,        0,     0,
	"",                     "!Third Attack!",       "",
	GN_STA
    },
    /* non-combat skills */
    { 
        "fast healing",        
        { 6,  6,  6,  7,  7, 16,  8, 12,  8, 15, 12, 15, 15, 12,  6,  7, 8, 9,  3,  9,  13},
        { 4,  3,  5,  5,  5,  6,  6,  6,  5,  8,  8,  8,  5,  5,  5,  5, 5, 5, 1,   6,   4},
	spell_null,             TAR_IGNORE,             POS_SLEEPING,
	&gsn_fast_healing,      SPELL_OTHER,       0,      0,
	"",                     "!Fast Healing!",       "",
	GN_STA
    },
    {
        "haggle",             
        {14, 16, 15, 52,  8,  1, 17, 18, 52,  8,  8,  8,  8, 18, 18,  8, 6, 8, 14, 14,  13},
        { 1,  1,  1, 10,  1,  1,  1,  1, 10,  1,  1,  1,  1,  1,  1,  1,  1, 1, 1,  1,   1},
	spell_null,             TAR_IGNORE,             POS_RESTING,
	&gsn_haggle,            SPELL_OTHER,       0,      0,
	"",                     "!Haggle!",             "",
	GN_STA
    },
    {
        "hide",            
        {52, 52, 52, 52, 52,  1,  4, 52, 52, 52, 52, 52, 52, 52, 52, 52, 10, 52, 52, 52},
        {10, 10, 10, 10, 10,  1,  1, 10,  1, 10, 10, 10, 10, 10, 10, 10,  2, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_RESTING,
	&gsn_hide,              SPELL_OTHER,        0,     12,
	"",                     "You step out of the shadows.",  "$n steps out of the shadows.",
	GN_STA
    },
    {
        "lore",            
        {21, 25, 18, 19, 20, 18, 19, 52, 13, 52, 52, 52, 52, 52, 52, 20,  5, 52, 21, 12, 16},
        { 2,  2,  2,  2,  2,  2,  2, 10,  2, 10, 10, 10, 10, 10, 10,  1,  1, 10,  1,  1,  1},
	spell_null,             TAR_IGNORE,             POS_RESTING,
	&gsn_lore,              SPELL_OTHER,       30,      24,
	"",                     "!Lore!",               "",
	GN_STA
    },
    {
        "meditation",         
        {18, 18, 16, 14, 15, 15, 15,  6, 10,  5,  2,  5,  1,  6,  9, 15, 10, 6, 23, 16, 14},
        { 6,  7,  6,  4,  4,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4, 4, 4,   5,  5,  5},
	spell_null,             TAR_IGNORE,             POS_SLEEPING,
	&gsn_meditation,        SPELL_OTHER,       0,      0,
        "",                     "!Meditation!",         "",
	GN_STA
    },
    {
        "peek",              
        {52, 52, 52, 52, 52,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 25, 52, 52, 23},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  2, 10, 10,  1},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_peek,              SPELL_OTHER,        0,      0,
	"",                     "!Peek!",               "",
	GN_STA
    },
    {
        "pick lock",           
        {52, 52, 52, 52, 52,  7, 14, 52, 52, 52, 52, 52, 52, 52, 52, 52, 12, 52, 52, 52},
        {10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10,  2, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_pick_lock,         SPELL_OTHER,        0,     12,
	"",                     "!Pick!",               "",
	GN_STA
    },
    {
        "sneak",              
        {52, 52, 52, 52, 52,  4,  5, 52, 14, 52, 52, 52, 52, 52, 52, 52, 16, 52, 52, 52},
        {10, 10, 10, 10, 10,  1,  1, 10,  1, 10, 10, 10, 10, 10, 10, 10,  2, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_sneak,             SPELL_OTHER,        0,     12,
	"",                     "You stop sneaking.", "",
	GN_STA
    },
    {
        "steal",             
        {52, 52, 52, 52, 52,  5, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 28, 52, 52, 52},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  2, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_steal,             SPELL_OTHER,        0,     PULSE_VIOLENCE,
	"",                     "!Steal!",              "",
	GN_STA
    },
    {
        "pry",             
        {52, 52, 52, 52, 52, 21, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_pry,               SPELL_OTHER,        0,     PULSE_VIOLENCE,
	"pry attempt",       "!Pry!",              "",
	GN_STA
    },
    {
        "scrolls",          
        {52, 52, 52,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  4,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10,  1,  2,  2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1, 2,  1, 11,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_scrolls,           SPELL_OTHER,       0,      24,
	"",                     "!Scrolls!",            "",
	GN_STA
    },
    {
        "staves",              
        {52, 52,  1, 1, 52, 52,  1,  1,  1,  1,  1,  1,  1,  1,  1, 52, 52, 1, 52, 52},
        {10, 10,  2, 1, 10, 10,  2,  1,  1,  1,  1,  1,  1,  1,  1, 10, 10, 2, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_staves,            SPELL_OTHER,       0,      24,
	"",                     "!Staves!",             "",
	GN_STA
    },
    {
        "wands",            
        {52, 52, 52, 52,  1, 52, 52,  1, 52,  1,  1,  1,  1,  1,  1,  1,  1,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10,  2, 10, 10,  1, 10,  1,  1,  1,  1,  1,  1,  1,  1,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_wands,             SPELL_OTHER,       0,      24,
	"",                     "!Wands!",              "",
	GN_STA
    },
    {
        "recall",            
        { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
        { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_recall,            SPELL_OTHER,       0,      12,
	"",                     "!Recall!",             "",
	GN_STA
    },
    /* new spells */
    {
        "ice storm",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 29, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_ice_storm,        TAR_IGNORE,     	POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       25,    24,
        "ice storm",            "!Ice Storm!",          "",
	GN_STA
    },
    {
        "protective shield",       
        {52, 52, 52, 52, 52, 52, 52, 18, 52, 16, 52, 16, 52, 18, 16, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1, 10,  1, 10,  1,  1, 10, 10, 10, 10, 10},
        spell_protective_shield, TAR_CHAR_SELF,         POS_STANDING,
        &gsn_protective_shield,                    SPELL_PROTECTIVE,       70,     12,
        "",   "Your protective shield dissipates.",  "$n's protective shield dissipates.",
	GN_BEN
    },
    {
        "jet steam",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 30, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_jet_steam,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       30,    12,
        "jet of steam",         "!Jet Steam!",          "",
	GN_STA
    },
    {
        "mystic tendrils",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 27, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mystic_tendrils,  TAR_CHAR_SELF,     POS_STANDING,
        NULL,                   SPELL_OTHER,       15,    12,
        "mystic tendrils",      "!Mystic Tendrils!",    "",
	GN_STA
    },
    {
        "ice shield",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 36, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_ice_shield,       TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   SPELL_AFFLICTIVE,       100,    12,
        "ice shield",   "The ice around you melts and becomes liquid.",  "The ice around $n melts and becomes liquid.",
	GN_BEN
    },
    {
        "fire shield",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 34, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_fire_shield,      TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   SPELL_AFFLICTIVE,       100,    12,
        "fire shield",  "The ring of flames die at your feet.",    "The ring of flames around $n dies out.",
	GN_BEN
    },
    {
        "mana shield",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 29, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mana_shield,      TAR_CHAR_SELF,          POS_STANDING,
        NULL, 		         SPELL_PROTECTIVE,       120,    12,
        "",  "Your mana shield flares brightly then dissipates.",    "$n's mana shield flares brightly then dissipates.",
	GN_BEN
    },
    {
        "hellstream",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 38, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_hellstream,       TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       45,    24,
        "hellstream",           "!Hellstream!",         "",
	GN_STA
    },
    {
        "flesh golem",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_flesh_golem,      TAR_IGNORE,             POS_STANDING,
        NULL,                   SPELL_OTHER,       80,    24,
        "",  "The power to fashion another flesh golem returns.",  "",
	GN_STA
    },
    {
        "soul capture",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 23, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_soul_capture,     TAR_OBJ_ROOM,           POS_STANDING,
        NULL,                   SPELL_OTHER,       30,    24,
        "",  "The power to capture another soul returns.",  "",
	GN_STA
    },
    {
        "vampiric touch",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 25, 52, 52, 52, 20, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_vampiric_touch,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       35,    12,
        "vampiric touch",       "",     "",
	GN_STA
    },
    {
        "hold undead",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 25, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_hold_undead,     TAR_CHAR_OFFENSIVE,      POS_FIGHTING,
        NULL,                   SPELL_MENTAL,       25,    12,
        "",       "You regain control of yourself.",     "",
	GN_HAR
    },
    {
        "animate corpse",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 28, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_animate_corpse,   TAR_OBJ_ROOM,           POS_STANDING,
        NULL,                   SPELL_OTHER,       125,   24,
        "",       "You can raise the dead again.",     "",
	GN_STA
    },
    {
        "shatter",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 30, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_shatter,          TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       35,    12,
        "bone shatter",     "You feel your bones healing.",     "",
	GN_HAR
    },
    {
        "chorus of anguish",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 31, 52, 52, 10, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10,  1, 10, 10, 10, 10, 10},
        spell_chorus_of_anguish, TAR_IGNORE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       25,    12,
        "chorus of anguish",    "!Chorus of Anguish!",  "",
	GN_STA
    },
    {
        "stone golem",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 33, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_stone_golem,      TAR_IGNORE,             POS_STANDING,
        NULL,                   SPELL_OTHER,       100,    24,
        "",  "The power to fashion another stone golem returns.",  "",
	GN_STA
    },
    {
        "strength damned",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 35, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_strength_damned,  TAR_CHAR_DEFENSIVE,      POS_STANDING,
        NULL,                   SPELL_OTHER,       60,     18,
        "",  "You are once again able to give your zombies the strength of the damned.",  "",
	GN_STA
    },
    {
        "death grasp",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 33, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_death_grasp,      TAR_CHAR_OFFENSIVE,      POS_FIGHTING,
        &gsn_death_grasp,       SPELL_MALEDICTION,       15,     12,
        "death grasp",  "You regain some of your vitality.",  "",
	GN_STA
    },
    {
        "powerword kill",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_powerword_kill,   TAR_CHAR_OFFENSIVE,    POS_STANDING,
        NULL,                   SPELL_MENTAL,       100,    24,
        "powerword kill",       "!Powerword Kill!",     "",
	GN_STA
    },
    {
        "knock",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 23, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_knock,            TAR_DOOR,        POS_STANDING,
        NULL,                   SPELL_OTHER,       15,    18,
        "",                     "!Knock!",       "",
	GN_STA
    },
    {
        "blur",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 26, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_blur,             TAR_CHAR_SELF,   POS_STANDING,
        NULL,                   SPELL_PROTECTIVE,       60,    24,
        "", "Your definition returns to your physical self.", "$n's image becomes more defined.",
	GN_BEN
    },
    {
        "phantasmal griffon",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_phantasmal_griffon, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
        &gsn_phantasmal_griffon,  SPELL_MENTAL,     15,     12,
        "phantasmal griffon", "Your phantasmal griffon disappears.", "The phantasmal griffon disappears.",
	GN_STA
    },
    {
        "spectre illusion",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_illusionary_spectre, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
        &gsn_illusionary_spectre,  SPELL_MENTAL,     35,     12,
        "illusionary spectre", "Your illusionary spectre disappears.", "The illusionary spectre disappears.",
	GN_STA
    },
    {
        "phantasmal dragon",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_phantom_dragon,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
        &gsn_phantom_dragon,    SPELL_MENTAL,       45,     12,
        "phantom dragon", "Your phantom dragon disappears.", "The phantom dragon disappears.",
	GN_STA
    },
    {
        "shrink",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 29, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_shrink,           TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_MALEDICTION,       30,    24,
        "", "The world seems to grow smaller as you regain normal size.", "$n grows bigger.",
	GN_HAR
    },
    {
        "enlarge",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 37, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_enlarge,          TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   SPELL_CURATIVE,       30,    24,
        "", "The world seems to grow larger as you regain normal size.", "$n shrinks in size.",
	GN_BEN
    },
    {
        "veil of darkness",   
        {52, 52, 52, 52, 15, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_veil,          TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_MALEDICTION,       13,    12,
        "",              "The shadowy walls surrounding you dissapate.",    "",
	GN_HAR
    },
    {
        "prismatic spray",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_prismatic_spray,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       35,    24,
        "prismatic spray",             "!Prismatic Spray!",    "",
	GN_STA
    },
    {
        "thunderclap",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 33, 52, 52, 52, 52, 52, 52, 25, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_misdirection,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_misdirection,      SPELL_MALEDICTION,       35,    12,
        "thunderclap",              "Your dazed mind finaly returns to normal.",    "",
	GN_HAR
    },
    {
        "meteor swarm",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 42, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_meteor_swarm,     TAR_IGNORE,      POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       42,    24,
        "meteor",         "!Meteor Swarm!",       "",
	GN_STA
    },
    {
        "deluge water",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 36, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_deluge_water,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       35,    12,
        "deluge of water",      "!Deluge Water!",       "",
	GN_STA
    },
    {
        "shadowform",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_shadowform,       TAR_CHAR_SELF,   POS_STANDING,
        &gsn_shadowform,        SPELL_OTHER,       100,   24,
        "","Your body regains its substance, and you materialize into existence.","$n's body regains its substance, and $e materializes into existence.",
	GN_BEN
    },
    {
        "feeblemind",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_feeblemind,       TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_MENTAL,       35,    12,
        "",     "You feel your mental capabilities return to normal.",   "",
	GN_HAR
    },
    {
        "flame arrow",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 18, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_flame_arrow,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       15,    12,
        "flame arrow",          "!Flame Arrow!",         "",
	GN_STA
    },
    {
        "iceball",
        {52, 52, 52, 52, 29, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_iceball,          TAR_IGNORE,             POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       25,    12,
        "iceball",              "!Iceball!",            "",
	GN_STA
    },
    {
        "icicle",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 17, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_icicle,           TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       15,    12,
        "icicle",               "!Icicle!",       "",
	GN_STA
    },
    {
        "ranger staff",
        {52, 52, 30, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,       TAR_IGNORE, POS_FIGHTING,
        &gsn_ranger_staff,       SPELL_OTHER,       100,   18,
        "",     "You feel up to creating another ranger's staff.",    "",
	GN_STA
    },
    {
        "wrath",
        {52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_wrath,            TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       30,    20,
        "heavenly wrath",       "!Wrath!",        "",
	GN_HAR
    },
    {
        "turn undead",
        {52, 52, 52, 25, 52, 52, 52, 28, 52, 52, 52, 52, 52, 28, 52, 52, 52, 52, 52, 52},
        {10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
        spell_turn_undead,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_turn_undead,                   SPELL_MENTAL,       35,    24,
        "faith",       "Your fears subside.",        "",
	GN_HAR
    },
    {
        "ikuzachi",
        {52, 52, 52, 52, 52, 52, 32, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_ikuzachi,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       35,    24,  
        "spirits",              "!Ikuzachi!",     ""   ,
	GN_STA
    },
    {
        "weapon cleave",            
        {52, 22, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_weapon_cleave,            SPELL_OTHER,        0,     24,
	"",                     "!Weapon Cleave!",             "",
	GN_STA
    },
    {
        "shield cleave",            
        {52, 23, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_shield_cleave,            SPELL_OTHER,        0,     24,
	"",                     "!Shield Cleave!",             "",
	GN_STA
    },
/* CABAL */
    {
        "holy armor",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_holy_armor,       TAR_IGNORE,     	POS_STANDING,
        NULL,                   SPELL_OTHER,       100,    24,
        "!Holy Armor!",                "You can once again infuse $g's power into armor.",  "",
	GN_STA
    },
    {
/*CABAL*/
        "chaosbane",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_holy_weapon,      TAR_IGNORE,     	POS_STANDING,
        NULL,                   SPELL_OTHER,       80,    24,
        "",                     "You once again feel up to creating Chaosbane.", "",
	GN_STA
    },
    {
/*CABAL*/
        "sense evil",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_sense_evil,      TAR_CHAR_SELF,     	POS_STANDING,
        NULL,                   SPELL_OTHER,      10,    12,
        "",                     "You feel less aware of the evil around you.", "",
	GN_BEN
    },
    {
/*CABAL*/
        "ward of the crown",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},

        spell_ward_of_light,      TAR_CHAR_SELF,     	POS_STANDING,
        NULL,                   SPELL_PROTECTIVE,      15,    12,
        "",                     "You feel less divine.", "",
	GN_BEN
    },
    {
/*CABAL*/
        "pixie dust",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     	POS_STANDING,
        &gsn_pixie_dust,                 SPELL_MALEDICTION,       0,    12,
        "",                     "You stop glowing as the pixie dust rubs off.", "$n stops glowing.",
	GN_HAR
    }, 
    {
/*CABAL*/
        "shadowdemon",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_shadowdemon,      TAR_IGNORE,     	POS_FIGHTING,
        NULL,                   SPELL_OTHER,      100,    18,
        "",     "You regain the strength to summon another shadowdemon", "",
	GN_STA
    },
    {
        "silence",   
        {52, 52, 52, 52, 22, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 22, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_silence,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_silence,      SPELL_OTHER,       35,    12,
        "",              "Your world is filled with sound again.",    "",
	GN_HAR
    },
    {
/*CABAL*/
        "catalyst",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_catalyst,     TAR_CHAR_SELF,     POS_STANDING,
        NULL,      SPELL_OTHER,       100,    18,
        "",              "",    "",
	GN_BEN
    },
    {
/*CABAL*/
        "mana transfer",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mana_transfer,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,      SPELL_OTHER,       0,    12,
        "mana transfer",         "!Mana Transfer!",    "",
	GN_STA
    },
    {
        "mental knife",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mental_knife,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_mental_knife,      SPELL_MENTAL,       26,    1 * PULSE_VIOLENCE,
        "mental knife",              "The searing pain in your mind slowly abates.",    "",
	GN_HAR | GN_PSI_MED
    },
    {
/*CABAL*/
        "tesseract",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_tesseract,     TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_MENTAL,       160,    24,
        "",              "!Tesseract!",    ""
,
	GN_STA
    },
    {
      /* TATTOO ONLY SPELL (also used in divine_discord)*/
        "demon summon",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_demon_summon,     TAR_CHAR_SELF,     POS_FIGHTING,
        NULL,      SPELL_OTHER,       0,    0,
        "",              "Your skin returns to normal.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "scourge",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_scourge,     TAR_IGNORE,     POS_FIGHTING,
        &gsn_scourge,      SPELL_AFFLICTIVE,       40,    12,
        "Scourge of the Violet Spider",              "You've managed to mend your armor.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "golden aura",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_golden_aura,     TAR_IGNORE,     POS_DEAD,
        NULL,      SPELL_PROTECTIVE,       25,    24,
        "",              "The golden aura fades from around you.",    "The golden aura fades from around $n.",
	GN_BEN
    },
    {
/*CABAL*/
        "guard",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_STANDING,
        &gsn_guard,      SPELL_OTHER,       35,    2 * PULSE_VIOLENCE,
        "",              "!Guard!",    "",
	GN_STA
    },
    {
/*CABAL*/
        "inquisitor",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_squire,     TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_OTHER,       50,    24,
        "",              "You feel up to summoning a new servant.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "stallion",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_stallion,     TAR_IGNORE,     POS_STANDING,
        &gsn_stallion,      SPELL_OTHER,       50,    12,
        "",              "You can summon another stallion.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "wanted",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_DEAD,
        &gsn_wanted,      SPELL_OTHER,       0,    0,
        "",              "Some of your crimes have been forgotten.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "judge",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_STANDING,
        &gsn_judge,      SPELL_OTHER,       0,    12,
        "",              "!judge!",    "",
	GN_STA
    },
    {
/*CABAL*/
        "ensnare",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_STANDING,
        &gsn_ensnare,      SPELL_OTHER,       10,    PULSE_VIOLENCE,
        "",              "You are finally free to leave the area.",    "$n is once again free to leave the area.",
	GN_STA
    },
    {
/*CABAL*/
        "third eye",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_third_eye,     TAR_CHAR_SELF,     POS_STANDING,
        NULL,      SPELL_OTHER,       25,    1 * PULSE_VIOLENCE,
        "",              "Your sensitivity to the invisible fades.",    "",
	GN_BEN
    },
    {
/*CABAL*/
        "death shroud",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_death_shroud,     TAR_CHAR_SELF,     POS_STANDING,
        NULL,      SPELL_OTHER,       15,    12,
        "",              "The deathly chill leaves you as you return to the living.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "unseen",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_OTHER,       48,    12,
        "",              "!unseen!",    "",
	GN_STA
    },
    {
/*CABAL*/
        "sharpen",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_OTHER,       75,    3 * PULSE_VIOLENCE,
        "!sharpen!",              "",    "",
	GN_STA
    },
    {
/*CABAL*/
        "ocularis arcanum",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_ocularis,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_ocularis,      SPELL_MENTAL,       20,    PULSE_VIOLENCE,
        "magic",              "You sense the effect of the strage rune fade.",    "The rune of the Eye of Magic fades from $n"
,
	GN_HAR
    },
    {
/*CABAL*/
        "nightwalker",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_nightwalker,     TAR_IGNORE,     POS_FIGHTING,
        NULL,      SPELL_OTHER,       75,    12,
        "",              "You feel your summoning powers return.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "guard captain",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_guard_call,     TAR_IGNORE,     POS_FIGHTING,
        NULL,      SPELL_OTHER,       100,    24,
        "",              "A new shift of guards arrive.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "exile",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_FIGHTING,
        &gsn_exile,      SPELL_OTHER,       150,    2 * PULSE_VIOLENCE,
        "",              "",    "",
	GN_NODEATH
    },
/*CABAL*/
    {
        "watchtower",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,      SPELL_OTHER,       100,    12,
        "",              "!watch tower!",    "",
	GN_STA
    },
/*CABAL*/
    {
        "rally call",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,      SPELL_OTHER,        80,    24,
        "",              "You feel once again capable of rallying troops to your cause.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "assassin call",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_stalker,     TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_OTHER,       100,    PULSE_VIOLENCE,
        "",              "You feel up to summoning another assassin.",    "",
	GN_STA
    }, 
    {
/*CABAL*/
        "disperse",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_disperse,     TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_OTHER,      100,    24,
        "",              "You feel up to doing more dispersing.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "randomizer",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_randomizer,     TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_OTHER,       200,    24,
        "",              "You feel your randomness regenerating.",    "",
	GN_STA
    },
    {
/*CABAL*/
      "chaos blade",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_chaos_blade,     TAR_IGNORE,     POS_FIGHTING,
        NULL,      SPELL_OTHER,       60,    12,
        "",              "You feel capable of creating another chaos blade.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "simulacra",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_doppelganger,     TAR_IGNORE,     POS_STANDING,
        &gsn_doppelganger,      SPELL_OTHER,       355,    PULSE_VIOLENCE,
        "",              "The illusion about you shatters.",    "",
	GN_BEN
    },
    {
/*CABAL*/
        "nightfall",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_nightfall,     TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_OTHER,       50,    12,
        "",              "!nightfall!",    "",
	GN_STA
    },
    {
        "mirror image",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 28, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mirror_image,     TAR_CHAR_SELF,     POS_STANDING,
        &gsn_mirror_image,      SPELL_OTHER,       80,    12,
        "",              "You may once again create a mirror image of yourself.",    "",
	GN_STA
    },
    {
        "garble",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_garble,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
        &gsn_garble,      SPELL_MENTAL,       40,    12,
        "",              "Your tongue untwists.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "minor banishment",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mbanish,      TAR_CHAR_OFFENSIVE,     	POS_FIGHTING,
        NULL,                   SPELL_MENTAL,      40,    12,
        "banishment",     "!Minor Banishment!", "",
	GN_HAR
    },
    {
/*CABAL*/
        "secret arts",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_secarts,         SPELL_OTHER,       42,     PULSE_VIOLENCE,
	"strike",                     "You are no longer using the Secret Arts.", "",
	GN_STA
    }, 
    /* new skills */
    {
        "dual wield",               
        {12, 17, 18, 22, 17, 22, 16, 52, 52, 52, 52, 52, 52, 52, 52, 15,  2, 18, 52, 52},
        { 3,  4,  1,  4,  3,  3,  3, 10, 10, 10, 10, 10, 10, 10, 10,  4,  4,  4, 10, 10},
        spell_null,             TAR_IGNORE,             POS_RESTING,
        &gsn_dual_wield,        SPELL_OTHER,        0,     12,
        "",                     "!Dual Wield!",         "",
	GN_STA
    },
    {
        "trance",         
        {52, 52, 52, 38, 52, 52, 52, 15, 52, 13, 10, 14, 13, 15, 20, 52, 52, 14, 38, 52},
        {10, 10, 10,  7, 10, 10, 10,  5, 10,  6,  6,  6,  7,  7,  7, 10, 10, 6, 10, 10},
	spell_null,             TAR_IGNORE,             POS_SLEEPING,
        &gsn_trance,            SPELL_OTHER,        0,     0,
        "",                     "!Trance!",             "",
	GN_STA
    },
    {
        "blind fighting",
        {10, 52, 12, 52, 52, 52, 52, 52, 18, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  8},
        { 3, 10,  4, 10, 10, 10, 10, 10,  3, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,             TAR_IGNORE,             POS_RESTING,
        &gsn_blind_fighting,    SPELL_OTHER,        0,     0,
        "",                     "!Blind Fighting!",     "",
	GN_STA
    },
    {
        "shield disarm",
        {15, 52, 52, 12, 16, 52, 52, 52, 19, 52, 52, 52, 52, 52, 52, 16, 52, 52, 52, 52},
        { 1, 10, 10,  1,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_shield_disarm,     SPELL_OTHER,        0,     24,
        "",                     "!Shield Disarm!",      "",
	GN_STA
    },
    {
        "deflect",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 37, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_rake,              SPELL_OTHER,        15,     2 * PULSE_VIOLENCE,
        "",                     "!Rake!",               "",
	GN_STA
    },
    {
        "fourth attack",        
        {35, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_fourth_attack,     SPELL_OTHER,        0,     0,
	"",                     "!Fourth Attack!",      "",
	GN_STA
    },
    {
/*CABAL*/
        "deathblow",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_deathblow,         SPELL_OTHER,        10,     2 * PULSE_VIOLENCE,
	"power strike",                     "You recover from the wounds caused by the ground splitter.",          "$n recovers from the wounds caused by ground splitter.",
	GN_STA
    }, 
    {
/*CABAL*/
        "forms",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_forms,         SPELL_OTHER,        12,     2 * PULSE_VIOLENCE,
	"deathblow",                     "You stop trying to execute forms.",          "",
	GN_STA
    }, 
    {
        "counter",
        {25, 52, 52, 42, 52, 52, 52, 52, 34, 52, 52, 52, 52, 52, 52, 52, 52, 52, 30, 52, 18},
        { 1, 10, 10,  3, 10, 10, 10, 10,  2, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10,  1},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_counter,           SPELL_OTHER,        0,     0,
        "counter",                     "",            "",
	GN_STA
    },
    {
        "lay on hands",
        {52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  5, 52},
        {10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10}, 
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_lay_on_hands,      SPELL_CURATIVE,        50,     36,
        "",                     "Your power to lay on hands returns.",     "",
	GN_STA
    },
    {
        "detect hidden",     
        {52, 52, 52, 52, 52, 10, 10, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_detect_hidden,     SPELL_OTHER,        10,     12,
        "",        "You feel less aware of your surroundings.",    "",
	GN_STA
    },
    {
	"cleave",            
        {52, 52, 52, 52, 25, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_cleave,            SPELL_OTHER,        0,     24,
	"cleave",               "You are once again vulnerable to being cleaved.",           "",
	GN_STA
    },
    {
	"assassinate",            
        {52, 52, 52, 52, 52, 52, 42, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_assassinate,       SPELL_OTHER,        0,     3*PULSE_VIOLENCE,
	"assassination attempt","!Assassinate!",           "",
	GN_STA
    },
    {
	"blackjack",            
        {52, 52, 52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_blackjack,      	SPELL_OTHER,        0,     2*PULSE_VIOLENCE,
	"blackjack", 	"You come to with a pounding headache.",	"",
	GN_STA
    },
    {
	"gag",            
        {52, 52, 52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_gag,      	SPELL_OTHER,        0,     3 * PULSE_VIOLENCE,
	"gag attempt", 	"You work the gag out of your mouth.",	"",
	GN_STA
    },
    {
	"strangle",            
        {52, 52, 52, 52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_strangle,      	SPELL_OTHER,        0,     24,
	"strangulation", 	"Your neck feels better.",	"",
	GN_STA
    },
    {
	"acupuncture",            
        {52, 52, 52, 52, 52, 52, 25, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 15},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
	spell_null,             TAR_IGNORE,             POS_RESTING,
	&gsn_acupuncture,      	SPELL_OTHER,        70,     24,
	"", 		"You feel your senses return to normal.",	"",
	GN_STA
    },
    {
	"herb",            
        {52, 52, 10, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 10, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_herb,      	SPELL_OTHER,        50,     24,
	"", 	"The herbs look more plentiful again.",	"",
	GN_STA
    },
    {
	"tame",            
        {52, 52, 17, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 17, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_tame,      	SPELL_MENTAL,        0,     12,
	"", 	"You are no longer tamed.",	"",
	GN_HAR
    },
    {
	"beast call",            
        {52, 52, 23, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_beast_call,      	SPELL_OTHER,        80,     24,
	"", 		"You feel you can handle more beasts again.",	"",
	GN_STA
    },
    {
	"rage",
        {52,  6, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10,  4, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_rage,      	SPELL_OTHER,        0,     24,
	"", 	"You relax your muscles as the insanity stops.","$n gasps for air as $s insanity ends.",
	GN_INTERRUPT
    },
    {
	"relax",
        {52, 10, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_relax,      	SPELL_OTHER,        0,     24,
	"", 			"!Relax!",	"",
	GN_STA
    },
    { 
        "regeneration",        
        {52, 27, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10,  5, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_SLEEPING,
	&gsn_regeneration,      SPELL_OTHER,       0,      0,
	"regeneration",                     "!Regeneration!",       "",
	GN_STA
    },
    {
        "bodyslam",             
        {52, 15, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_bodyslam,          SPELL_OTHER,       0,      24,
	"bodyslam",                 "!Bodyslam!",               "",
	GN_STA
    },
    {
        "dual backstab",            
        {52, 52, 52, 52, 52, 30, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_dual_backstab,     SPELL_OTHER,        0,     0,
	"second backstab",      "!Dual backstab!",           "",
	GN_STA
    },
    {
        "grapple",             
        {52, 52, 52, 52, 52, 52, 15, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_grapple,           SPELL_OTHER,       0,      24,
	"grapple",    "!Grapple!",               "",
	GN_STA
    },
    {
        "charge",            
        {16, 13, 52, 20, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 17},
        { 1,  1, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_charge,	        SPELL_OTHER,        0,     24,
	"charge attack", 	"You are once again vulnerable to being charged.",           "",
	GN_STA
    },
    {
        "ambush",             
        {52, 52, 10, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_ambush,            SPELL_OTHER,       0,      24,
	"surprise attack",      "!Ambush!",               "",
	GN_STA
    },
    {
        "endure",             
        {52, 52, 52, 52, 52, 52, 10, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_endure,          SPELL_OTHER,       30,      12,
	"",       "You feel susceptible to magic again.",  "",
	GN_INTERRUPT
    },
    {
        "barkskin",             
        {52, 52, 15, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_barkskin,          SPELL_OTHER,      50,      12,
	"",        "The bark covering your skin flakes off.", "The bark covering $n's skin flakes off.",
	GN_INTERRUPT
    },
    {
        "warcry",             
        {20, 20, 20, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 20, 52, 52},
        { 1,  1,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_warcry,            SPELL_OTHER,       30,      12,
	"",              "Your warcry wears off.",         "",
	GN_INTERRUPT
    },
    {
        "circle",             
        {52, 52, 52, 52, 52, 10, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_circle,            SPELL_OTHER,       0,      24,
	"circle stab",          "!Circle!",               "",
	GN_STA
    },
    {
        "swing",             
        {52, 10, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 2, 52},
        {10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_swing,             SPELL_OTHER,       0,      24,
	"wild swing",           "!Swing!",               "",
	GN_STA
    },
    {
        "headbutt",             
        {52, 33, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_headbutt,          SPELL_OTHER,       0,      24,
	"headbutt","The pain in your head stops, and the dizziness clears up.","$n shakes $s head, clearing up $s dizziness.",
	GN_STA
    },
    {
        "camouflage",             
        {52, 52,  4, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 4, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_camouflage,        SPELL_OTHER,       0,      12,
	"","You step out from your cover.","$n steps out from $s cover.",
	GN_STA
    },
    {
        "quiet movement",             
        {52, 52, 12, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 12, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_quiet_movement,        SPELL_OTHER,       0,      0,
	"",			"",		"",
	GN_STA
    },
    {
        "butcher",             
        {52, 52,  9, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  7},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_butcher,           SPELL_OTHER,       0,      12,
	"",                     "!Butcher!",            "",
	GN_STA
    },
    {
        "keen sight",             
        {52, 52,  5, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  5, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_keen_sight,        SPELL_OTHER,       10,     12,
	"",	"Your vision is no longer as sharp.",	"",
	GN_STA
    },
    {
        "vanish",             
        {52, 52, 52, 52, 52, 52, 16, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_vanish,            SPELL_OTHER,       30,     12,
	"",			"!Vanish!",		"",
	GN_STA
    },
    {
        "chii",             
        {52, 52, 52, 52, 52, 52, 25, 52, 12, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_chii,               SPELL_OTHER,       20,     12,
	"",			"You feel your equilibrium shift.",	"",
	GN_INTERRUPT
    },
    {
        "blindness dust",             
        {52, 52, 52, 52, 52, 52, 18, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_blindness_dust,    SPELL_MALEDICTION,       15,     12,
	"",			"",		"",
	GN_STA
    },
    {
        "poison smoke",             
        {52, 52, 52, 52, 52, 52, 19, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_poison_smoke,      SPELL_MALEDICTION,       15,     12,
	"",			"",		"",
	GN_STA
    },
    {
        "dual parry",             
        {23, 52, 28, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 18, 52, 52, 52, 30},
        { 4, 10,  3, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  6, 10, 10, 10,  1},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_dual_parry,        SPELL_OTHER,       0,     0,
	"hilt smash",			"!Dual Parry!",		"",
	GN_STA
    },
    {
        "riposte",             
        {37, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 3, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_riposte,        SPELL_OTHER,       0,     0,
	"riposte",			"!Riposte!",		"",
	GN_STA
    },
    {
        "caltraps",             
        {52, 52, 52, 52, 52, 52, 14, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,      	POS_FIGHTING,
	&gsn_caltraps,          SPELL_OTHER,       0,     24,
	"caltraps",		"You stop limping.",		"$n stops limping.",
	GN_STA
    },
    {
        "nerve",             
        {52, 52, 52, 52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,      	POS_FIGHTING,
	&gsn_nerve,        SPELL_OTHER,       0,     24,
	"",		   "You regain your strength.",		"",
	GN_STA
    },
    {
/*CABAL*/
        "truesight",             
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_CHAR_SELF,      	POS_FIGHTING,
	&gsn_truesight,        SPELL_OTHER,       35,     2 * PULSE_VIOLENCE,
	"drop kick",		   "Your eyes see less truly.",		"",
	GN_STA
    },
    {
/*CABAL*/
        "stance",             
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_CHAR_SELF,      	POS_FIGHTING,
	&gsn_stance,        SPELL_OTHER,       25,     1 * PULSE_VIOLENCE,
	"chest kick",		   "You step out from your combat stance.",		"$n steps out of $s combat stance.",
	GN_INTERRUPT
    },
    {
/*CABAL*/
        "cognizance",             
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_CHAR_SELF,      	POS_FIGHTING,
	&gsn_cognizance,        SPELL_OTHER,       50,     24,
	"",		   "You feel your extra awareness drain away.",		"",
	GN_INTERRUPT
    },
    {
/*CABAL*/
        "spellbane",             
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_CHAR_SELF,      	POS_FIGHTING,
	&gsn_spellbane,        SPELL_OTHER,       50,     24,
	"spellbane",		   "You feel the effects of spellbane recede.",		"",
	GN_STA
    },
    {
/*CABAL*/
        "trophy",             
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_OBJ_INV,      	POS_STANDING,
	&gsn_trophy,        SPELL_OTHER,       150,     24,
	"ground splitter",		   "You feel up to making another trophy.",		"",
	GN_STA
    },
    {
        "brew",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 42, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_null,         TAR_OBJ_INV,     POS_STANDING,
        &gsn_brew,          SPELL_OTHER,       25,    12,
        "",              "You feel you can start brewing again.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "form mastery", 
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,         TAR_OBJ_INV,     POS_STANDING,
        NULL,          SPELL_OTHER,      0,    0,
        "",            "",    "",
	GN_STA
    },
    {
/*CABAL*/
        "challenge",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,          TAR_IGNORE,     POS_DEAD,
        &gsn_challenge,      SPELL_OTHER,       0,    0,
        "",              "`1You may initiate combat!``",    "",
	GN_STA
    },
    {
/*CABAL*/
        "pathfinding",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,          TAR_IGNORE,     POS_DEAD,
        &gsn_climb,          SPELL_OTHER,       0,    6,
        "",                  "!climb!",    "",
	GN_STA
    },
    /* non-combat skills */
    {
        "staff",              
        { 1, 1, 1, 1, 52, 52,  1,  1,  1,  1,  8,  1,  1,  1,  1, 52,  1, 1, 1, 52},
        { 3, 4, 2,  4, 10, 10,  4,  4,  1,  1,  1,  1,  1,  3,  3, 10, 4, 5, 2, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_staff,             SPELL_OTHER,       0,      0,
	"",                     "!Staff!",              "",
	GN_STA
    },
    {
/*CABAL*/
        "hunt",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_RESTING,
        &gsn_hunt,              SPELL_OTHER,        12,     10,
        "",                     "!Hunt!",               "",
	GN_STA
    },
    {
        "damnation",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_damnation,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_damnation,         SPELL_OTHER,       30,     12,
        "damnation",            "You are no longer damned by the Gods.", ""   ,
	GN_STA
    },
    {
        "flashfire",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 32, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_flashfire,        TAR_IGNORE,     POS_FIGHTING,
        &gsn_flashfire,         SPELL_MALEDICTION,       30,     PULSE_VIOLENCE,
        "flashfire",            "You can see again.", "$n is no longer blinded."   ,
	GN_HAR
    },
    {
        "lash",               
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_lash,              SPELL_OTHER,       0,      24,
	"lash",                 "!Trip!",               "",
	GN_STA
    },
    {
        "drained",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_drained,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_drained,         SPELL_OTHER,       30,     12,
        "drained",            "You don't feel as drained.", "",
	GN_STA
    },
    {
        "mortally wounded",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,        TAR_IGNORE,     POS_FIGHTING,
        &gsn_mortally_wounded, SPELL_OTHER,       0,     0,
        "",            "You feel like you can move again.", "$n's body looks like in much better shape.",
	GN_STA
    },
    {
/*CABAL*/
        "vortex tap",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,          TAR_IGNORE,     POS_DEAD,
        &gsn_ancient_lore,      SPELL_OTHER,       0,    0,
        "light sword",              "The Timestream returns to its normal flow.",    "",
	GN_STA

    },
    {
/*CABAL*/
        "invoke artifact",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,          TAR_IGNORE,     POS_STANDING,
        &gsn_invoke,         SPELL_OTHER,       0,    0,
        "",               "!invoke artifact!",    "",
	GN_STA
    },
    {
/*CABAL*/
        "create artifact",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_create_artifact,   TAR_OBJ_INV,            POS_STANDING,
	NULL,                    SPELL_OTHER,       80,    24,
	"",                     "!Create Artifact!",     "",
	GN_STA
    },	
    {
        "spell turning",   
        {52, 52, 52, 52, 52, 52, 52, 37, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_mimic,            TAR_CHAR_SELF,          POS_STANDING,
	&gsn_mimic,             SPELL_PROTECTIVE,       35,    2 * PULSE_VIOLENCE,
	"",                     "Your shield of spell turning collapses.",     "The shimmering, reflecting shield around $n fades away.",
	GN_BEN
    },	
    {
/*CABAL*/
        "servant",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_servant,         TAR_IGNORE,            POS_FIGHTING,
	NULL,                   SPELL_OTHER,       100,    24,
	"",    "You are able to onto a new servant if needed.", "",
	GN_STA
    },	
    {
        "camp",
        {52, 52, 15, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 15, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_camp,              SPELL_OTHER,       20,      18,
        "",			"You feel fully rested.","",
	GN_STA
    },
    {
        "scout",
        {52, 52,  7, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 7, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_scout,              SPELL_OTHER,        0,      8,
        "",			"!Scout!","",
	GN_STA
    },
    {
        "throw",
        {52, 52, 52, 52, 52, 17, 22, 52, 52, 52, 52, 52, 52, 52, 52, 52, 21, 52, 52, 52},
        {10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1,  10, 10, 10},
        spell_null,             TAR_IGNORE,          POS_FIGHTING,
        &gsn_throw,              SPELL_OTHER,       0,      2 * PULSE_VIOLENCE,
        "throw",		"You see an opportunity to throw again.",	  "",
	GN_STA
    },
    {
        "roar",
        {52, 17, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 25, 52},
        {10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 10},
        spell_null,             TAR_IGNORE,          POS_FIGHTING,
        &gsn_roar,              SPELL_OTHER,       0,      24,
        "roar",	  	"You regain your confidence.",	  "$n looks more confident.",
	GN_INTERRUPT
    },
    {
        "edge craft",
        {52, 52, 52, 52, 52, 17, 22, 52, 52, 52, 52, 52, 52, 52, 52, 52, 20, 52, 52, 52},
        {10, 10,  1, 10, 10,  1,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10},
        spell_null,             TAR_IGNORE,          POS_STANDING,
        &gsn_edge_craft,           SPELL_OTHER,       0,      24,
        "edge",		"!Edge!",	  "",
	GN_STA
    },
    {
/*CABAL*/
        "nexus", 
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_nexus,      	TAR_CHAR_SELF,     	POS_STANDING,
        NULL,                   SPELL_OTHER,      80,    12,
        "",     		"!NEXUS!", 		"",
	GN_STA
    },
    {
        "counterfeit",
        {52, 52, 52, 52, 52, 32, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,          POS_STANDING,
        &gsn_counterfeit,         SPELL_OTHER,       0,      2 * PULSE_VIOLENCE,
        "",		"!Counterfeit!",	  "",
	GN_STA
    },
    {
        "familiar link",        
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_familiar_link,              SPELL_OTHER,       0,      0,
	"",     "",   "",
	GN_STA
    },
    {
        "thunder storm",   
        {52, 52, 15, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 29, 52, 52},
        {10, 10, 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_thunder_storm,        TAR_IGNORE,     	POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       45,    12,
        "thunder storm",            "!Thunder Storm!",          "",
	GN_STA
    },
    {
        "extort",
        {52, 52, 52, 52, 30, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,          POS_DEAD,
        &gsn_extort,         SPELL_OTHER,       0,      0,
        "",		"!Extort!",	  "",
	GN_STA
    },
    {
        "curse weapon",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 19, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_curse_weapon,        TAR_OBJ_INV,     	POS_STANDING,
    	&gsn_curse_weapon,                   SPELL_OTHER,       100, 12,
        "",            "!Curse Weapon!",          "",
	GN_STA
    },
    {
        "divine retribution",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 36, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_divine_retribution,        TAR_CHAR_SELF, POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,      75,     12,
	"divine retribution",             "The wrath of the gods no longer protects you.",          "",
	GN_BEN
    },
    {
        "esuna",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 48, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_esuna,        TAR_CHAR_DEFENSIVE, POS_STANDING,
	NULL,                   SPELL_RESTORATIVE,      60,     12,
	"",             "!Esuna!",          "",
	GN_BEN
    },
    {
        "holy water",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 45, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_holy_water,        TAR_OBJ_INV, POS_STANDING,
	NULL,                   SPELL_OTHER,      150,     24,
	"",             "Your faith allows you to bless more water.", "",
	GN_STA
    },
    {
        "barrier",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_barrier,        TAR_CHAR_SELF,   POS_STANDING,
	NULL,                   SPELL_PROTECTIVE,      70,     12,
	"",             "Your physical barrier dissipates.", "The barrier around $n dissipates.",
	GN_BEN
    },
    {
        "vitalize",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 35, 52, 52, 52, 40, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10,  1, 10, 10},
	spell_vitalize,        TAR_CHAR_DEFENSIVE,   POS_STANDING,
	NULL,                   SPELL_RESTORATIVE,      20,     12,
	"",             "Your metabolism returns to normal.", "",
	GN_BEN
    },
    {
        "sacrifice",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 39, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_sacrifice,        TAR_CHAR_DEFENSIVE,   POS_STANDING,
	NULL,                   SPELL_CURATIVE,      20,     24,
	"",             "", "",
	GN_STA
    },
    {
        "portal",       
        {52, 52, 52, 52, 52, 52, 52, 42, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_heavens_gate,        TAR_IGNORE,   POS_STANDING,
	NULL,                   SPELL_OTHER,      60,     12,
	"",             "!Heavens Gate!", "",
	GN_STA
    },
    {
        "rejuvenate",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_rejuvenate,        TAR_CHAR_DEFENSIVE,   POS_FIGHTING,
	NULL,                   SPELL_CURATIVE,      60,     18,
	"",             "!Rejuvenate!", "",
	GN_BEN
    },
    {
        "prayer",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 31, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_prayer,        TAR_CHAR_DEFENSIVE,   POS_STANDING,
	&gsn_prayer,                   SPELL_OTHER,      30,     12,
	"",             "You have left your state of serenity.", "",
	GN_BEN
    },
    {
        "spirit shield",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 50, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_spirit_shield,        TAR_CHAR_SELF,   POS_STANDING,
	NULL,                   SPELL_PROTECTIVE,      100,     24,
	"",            "Your spirit shield disappears.", "$n's spirit shield disappears",
	GN_BEN
    },
    {
        "invigorate",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 42, 52, 52, 52, 32, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10,  1, 10, 10},
	spell_invigorate,        TAR_CHAR_DEFENSIVE,   POS_STANDING,
	&gsn_invigorate,                   SPELL_RESTORATIVE,      60,     24,
	"",             "!Invigorate!", "",
	GN_BEN
    },
    {
        "group recall",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 37, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_group_recall,        TAR_IGNORE,   POS_STANDING,
	NULL,                   SPELL_PROTECTION,      50,     12,
	"",             "!Group Recall!", "",
	GN_STA
    },
    {
        "healer staff",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
        spell_null,       TAR_IGNORE, POS_FIGHTING,
        &gsn_healer_staff,       SPELL_OTHER,       80,   24,
        "",     "You feel up to creating another healer staff.",    "",
	GN_STA
    },
    {
        "accession",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 33, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_accession,        TAR_CHAR_DEFENSIVE,   POS_STANDING,
	NULL,                   SPELL_PROTECTION,      100,     24,
	"",             "You are no longer tapping into your hidden potential.", "",
	GN_BEN
    },
    {
        "blasphemy",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 38, 40, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1,  1, 10, 10, 10, 10},
	spell_blasphemy,        TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
	&gsn_blasphemy,                   SPELL_MALEDICTION,      30,     12,
	"",             "Your faith has been restored.", "",
	GN_HAR
    },
    {
        "demonic visage",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_demonic_visage,        TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
	NULL,                   SPELL_MENTAL,      20,     PULSE_VIOLENCE * 1,
	"",             "The images of demons have left your mind.", "",
	GN_HAR
    },
    {
        "blanket of darkness",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 45, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_blanket_darkness,        TAR_IGNORE,   POS_STANDING,
	&gsn_blanket,           SPELL_OTHER,      150,     24,
	"",             "You are ready cast another blanket of darkness.", "",
	GN_STA
    },
    {
        "enfeeblement",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 23, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_enfeeblement,        TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
	&gsn_enfeeblement,                   SPELL_MALEDICTION,      20,     12,
	"",             "Your legs regain their strength.", "",
	GN_HAR
    },
    {
        "mass hysteria",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_mass_hysteria,        TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
	&gsn_hysteria,                   SPELL_MALEDICTION,      30,     12,	
	"",             "You gather your wits and recollect your thoughts.", "",
	GN_HAR
    },
    {
        "insomnia",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 33, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_insomnia,        TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
	&gsn_insomnia,                   SPELL_MALEDICTION,      20,     12,	
	"",             "You are suddenly overcome with exhaustion.", "",
	GN_HAR
    },
    {
        "dysentery",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 35, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_dysentery,        TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
	&gsn_dysentery,          SPELL_MALEDICTION,      20,     12,	
	"bowel movements",             "Your bowel movements returns to normal.", "",
	GN_HAR
    },
    {
        "phantom grasp",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 25, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_phantom_grasp,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_MALEDICTION,       20,     12,
	"phantom grasp",          "You have escaped from the grasp of the phantoms.",  "$n has escaped from the grasp of the phantoms.",
	GN_HAR
    },
    {
        "dark shroud",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 37, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_dark_shroud,      TAR_CHAR_SELF,     POS_STANDING,
	NULL,                   SPELL_OTHER,       25,     12,
	"",          "The dark shroud around you vanishes.",                "The dark shroud around $n vanishes.",
	GN_BEN
    },
    {
        "deteriorate",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 31, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_deteriorate,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_deteriorate,                 SPELL_MALEDICTION,       20,     12,
	"",          "You are no longer feel so worn.",                "",
	GN_HAR
    },
    {
        "cure dysentery",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 33, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_cure_dysentery,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_RESTORATIVE,       20,     12,
	"",          "!Cure Dysentery!",                "",
	GN_STA
    },
    {
        "cure insomnia",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 32, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_cure_insomnia,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_RESTORATIVE,       20,     12,
	"",          "!Cure Insomnia!",                "",
	GN_STA
    },
    {
        "remove hysteria",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 37, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_remove_hysteria,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,                   SPELL_RESTORATIVE,       20,     12,
	"",          "!Remove Hysteria!",                "",
	GN_STA
    },
    {
        "ethereal passage",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 18, 52, 32, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10,  1, 10, 10, 10, 10, 10},
	spell_ethereal_passage,        TAR_IGNORE,   POS_STANDING,
	NULL,                   SPELL_OTHER,      30,     12,	
	"",             "!Ethereal Passage!", "",
	GN_STA
    },
    {
        "mystic healing",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,        TAR_IGNORE,   POS_STANDING,
	&gsn_mystic_healing,            SPELL_CURATIVE,      30,     12,	
	"",             "Power has been restored to the relics.", "",
	GN_STA
    },
    {
        "shield of words",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,        TAR_IGNORE,   POS_STANDING,
	&gsn_shield_of_words,                   SPELL_PROTECTIVE,      30,     12,	
	"",             "Your shield of words dissipates.", "$n's shield of words dissipates.",
	GN_BEN
    },
    {
        "children of the sky",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,        TAR_IGNORE,   POS_STANDING,
	&gsn_children_sky,                   SPELL_OTHER,      30,     12,	
	"",             "", "",
	GN_BEN
    },
    {
        "defecate",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,        TAR_IGNORE,   POS_RESTING,
	&gsn_defecate,                   SPELL_OTHER,      0,     0,	
	"",             "", "",
	GN_STA
    },
    {
/*CABAL*/
        "deathmark",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,          TAR_IGNORE,     POS_DEAD,
        &gsn_challenge,      SPELL_OTHER,       0,    0,
        "",              "`1YOU MAY NOW FIGHT!``",    "",
	GN_STA
    },
    {
        "mind link",        
        {52, 52, 35, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,               TAR_IGNORE,             POS_STANDING,
	&gsn_mind_link,              SPELL_OTHER,       0,      0,
	"",     "You are once again able to communicate with your beasts.",   "",
	GN_STA
    },
    {
        "linked mind",        
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,               TAR_IGNORE,             POS_STANDING,
	&gsn_linked_mind,              SPELL_OTHER,       0,      0,
	"",     "",   "",
	GN_STA
    },
    {
        "timer",        
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_timer,             TAR_IGNORE,             POS_STANDING,
	&gsn_timer,              SPELL_OTHER,       0,      0,
	"",     "",   "",
	GN_STA
    },
    {
/*CABAL*/
        "abduct",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     	POS_FIGHTING,
        &gsn_abduct,                   SPELL_OTHER,      23,    12,
        "",     "You've lost the sense of future abduction location.", "",
	GN_STA
    },
    {
      "pilfer",
      {52, 52, 52, 52, 52, 23, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,        TAR_IGNORE,    		POS_FIGHTING,
      &gsn_pilfer,       SPELL_OTHER,       	0,   24,
      "pilfer",     "!Pilfer!",    "",
	GN_STA
    },
    {
        "morph",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 30, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_morph,        TAR_IGNORE,    		POS_STANDING,
        NULL,       SPELL_OTHER,       	10,   PULSE_VIOLENCE,
        "morph",     "!Morph!",    "",
	GN_STA
    },
    {
        "bat form",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 30, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_null,        TAR_IGNORE,    		POS_FIGHTING,
        &gsn_bat_form,       SPELL_OTHER,       	30,   PULSE_VIOLENCE,
        "shriek",     "You revert back to your natural form.",    "$n reverts back to $s natural form.",
	GN_STA
    },
    {
        "wolf form",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 35, 52, 52, 52, 52},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_null,        TAR_IGNORE,    		POS_FIGHTING,
        &gsn_wolf_form,       SPELL_OTHER,       	60,   2 *PULSE_VIOLENCE,
        "tackle",     "You revert back to your natural form.",    "$n reverts back to $s natural form.",
	GN_STA
    },
    {
        "mist form",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_null,        TAR_IGNORE,    		POS_FIGHTING,
        &gsn_mist_form,       SPELL_OTHER,       	0,   24,
        "toxic mist",     "You revert back to your natural form.",    "$n reverts back to $s natural form.",
	GN_STA
    },
    {
        "double kick",
        {52, 52, 52, 52, 52, 52, 30, 52, 21, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,        TAR_IGNORE,    		POS_FIGHTING,
        &gsn_double_kick,       SPELL_OTHER,       	0,   0,
        "double kick",     "!double kick!",    "",
	GN_STA
    },
    {
        "dark metamorphosis",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 36, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_dark_meta,        TAR_OBJ_WORN,    		POS_STANDING,
        NULL,       SPELL_OTHER,       	30,   6,
        "morph",     "Your dark metamorphosis has worn off.",    "",
	GN_STA
    },
    {
        "soul steal",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 31, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_soul_steal,        TAR_OBJ_ROOM,    		POS_STANDING,
        NULL,       SPELL_OTHER,       	15,   12,
        "soul steal",     "!Soul Steal!",    "",
	GN_STA
    },
    {
        "bloodlust",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 41, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_bloodlust,        TAR_CHAR_SELF,    		POS_STANDING,
        NULL,       SPELL_OTHER,       	30,   12,
        "bloodlust",     "Your senses return to normal.",    "The redness disappears from $n's eyes.",
	GN_BEN
    },
    {
        "ecstacy",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 25, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_ecstacy,        TAR_CHAR_OFFENSIVE,    		POS_STANDING,
        &gsn_ecstacy,       SPELL_OTHER,       	15,   12,
        "ecstacy",     "You snap out from your daydreaming.",    "$n seems to be in contact with reality.",
	GN_HAR
    },
    {
        "twilight shift",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 43, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_twilight_shift,        TAR_IGNORE,    		POS_STANDING,
        NULL,       SPELL_OTHER,       	30,   12,
        "Twilight Shift",     "!Twilight Shift!",    "",
	GN_STA
    },
    {
        "shadowstalk",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 45, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_shadowstalk,        TAR_IGNORE,    		POS_STANDING,
        NULL,       SPELL_OTHER,       	100,   12,
        "shadowstalk",     "You can give life to your shadow once more.",    "",
	GN_STA
    },
    {
        "coffin",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,        TAR_IGNORE,    		POS_STANDING,
        &gsn_coffin,       SPELL_OTHER,       	0,   24,
        "coffin",     "You rise from your coffin.",    "$n rises from $s coffin.",
	GN_STA
    },
    {
        "thrust",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 25, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_null,        TAR_IGNORE,    		POS_FIGHTING,
        &gsn_thrust,       SPELL_OTHER,       	0,   24,
        "thrust",     "!Thrust!",    "",
	GN_STA
    },
    {
        "stake",
        {52, 52, 52, 35, 52, 52, 52, 35, 52, 52, 52, 52, 52, 35, 52, 52, 52, 52, 29, 52},
        {10, 10, 10,  1, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10,  1, 10},
        spell_null,        TAR_IGNORE,    		POS_STANDING,
        &gsn_stake,       SPELL_OTHER,       	0,   12,
        "stake",     "!Stake!",    "",
	GN_STA
    },
/* Monk stuff */
    {
        "anatomy",     
        {52, 52, 52, 52, 52, 52, 52, 52,  1, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  1},
        {10, 10, 10, 10, 10, 10, 10, 10,  7, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  9},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_anatomy,  SPELL_OTHER,        0,     0,
	"",                     "!Anatomy!",    "",
	GN_STA
    },
    {
        "first parry",     
        {52, 52, 52, 52, 52, 52, 52, 52,  9, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  3, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_first_parry,  SPELL_OTHER,        0,     0,
	"",                     "!First Parry!",    "",
	GN_STA
    },
    {
        "second parry",     
        {52, 52, 52, 52, 52, 52, 52, 52, 16, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  4, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_second_parry,  SPELL_OTHER,        0,     0,
	"",                     "!Second Parry!",    "",
	GN_STA
    },
    {
        "beads",     
        {52, 52, 52, 52, 52, 52, 52, 52,  2, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_RESTING,
	&gsn_beads,  SPELL_OTHER,        0,      24,
	"",                     "!Beads!",    "",
	GN_STA
    },
    {
        "martial arts b",      
        {52, 52, 52, 52, 52, 52, 52, 52,  4, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  3, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_martial_arts_b,    SPELL_OTHER,        0,     0,
	"wrist blow",                     "!Martial Arts B!",      "",
	GN_STA
    },
    {
        "martial arts a",      
        {52, 52, 52, 52, 52, 52, 52, 52, 17, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  3, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_martial_arts_a,    SPELL_OTHER,        0,     0,
	"chakera strike",                     "!Martial Arts A!",      "",
	GN_STA
    },
    {
        "kickboxing b",      
        {52, 52, 52, 52, 52, 52, 52, 52, 11, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  3, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_kickboxing_b,      SPELL_OTHER,        0,     0,
	"throat kick",                     "!Kickboxing B!",      "",
	GN_STA
    },
    {
        "kickboxing a",      
        {52, 52, 52, 52, 52, 52, 52, 52, 24, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  3, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_kickboxing_a,      SPELL_OTHER,        0,     0,
	"kidney punch",                     "!Kickboxing A!",      "",
	GN_STA
    },
    {
        "double grip",      
        {40, 38, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 10},
        { 1,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
	spell_null,             TAR_IGNORE,             POS_FIGHTING,
	&gsn_double_grip,      SPELL_OTHER,        0,     0,
	"",                     "You loosen your grip on your weapon.",      "",
	GN_INTERRUPT
    },
    {
        "monkey stance",      
        {52, 52, 52, 52, 52, 52, 52, 52, 37, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_monkey,              SPELL_OTHER,        0,     24,
	"",             "You no longer have the speed and agility of a monkey.",      "$n slows down.",
	GN_INTERRUPT
    },
    {
        "tiger stance",      
        {52, 52, 52, 52, 52, 52, 52, 52, 23, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_tiger,              SPELL_OTHER,        0,     24,
	"tiger claw",                     "You no longer have the ferocity of a tiger.",      "$n doesn't seem as powerful.",
	GN_INTERRUPT
    },
    {
        "crane stance",      
        {52, 52, 52, 52, 52, 52, 52, 52, 14, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  2, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_crane,              SPELL_OTHER,        0,     24,
	"",                     "You no longer have the swiftness and agility of the crane.",      "$n's movement is no longer as accurate.",
	GN_INTERRUPT
    },
    {
        "preying mantis",
        {52, 52, 52, 52, 52, 52, 52, 52, 26, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_mantis,                   SPELL_OTHER,       25,     12,
        "reversal",          "You stop preparing for a counter move.",   "$n drops his guard.",
	GN_INTERRUPT
    },
    {
        "drunken stance",             
        {52, 52, 52, 52, 52, 52, 52, 52, 30, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,      	POS_STANDING,
	&gsn_drunken,        SPELL_OTHER,       20,     12,
	"",		       "Your head sobers up.",		"$n stops staggering around.",
	GN_INTERRUPT
    },
    {
        "horse stance",             
        {52, 52, 52, 52, 52, 52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  4, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,      	POS_STANDING,
	&gsn_horse,        SPELL_OTHER,       0,     24,
	"",		       "You lose the stability and endurance of a horse.", "$n doesn't look as stable.",
	GN_INTERRUPT
    },
    {
        "dragon stance",             
        {52, 52, 52, 52, 52, 52, 52, 52, 45, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,      	POS_STANDING,
	&gsn_dragon,        SPELL_OTHER,       0,     24,
	"",		       "You no longer possess the spirit of the dragon.", "$n returns to $s normal size.",
	GN_INTERRUPT
    },
    {
        "buddha stance",             
        {52, 52, 52, 52, 52, 52, 52, 52, 42, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,      	POS_STANDING,
	&gsn_buddha,        SPELL_OTHER,       0,     24,
	"",		       "You no longer have full control of your mind.", "$n doesn't look as concentrated.",
	GN_INTERRUPT
    },
    {
        "catalepsy",      
        {52, 52, 52, 52, 52, 52, 52, 52, 32, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_RESTING,
	&gsn_catalepsy,         SPELL_OTHER,        0,     24,
	"",                     "You return from the dead.",      "$n starts moving again.",
	GN_STA
    },
    {
        "sense motion",
        {52, 52, 52, 52, 52, 52, 52, 52, 25, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,              TAR_IGNORE,     	POS_STANDING,
        &gsn_sense_motion,       SPELL_OTHER,         0,    24,
        "",                     "Your awareness diminishes.", "",
	GN_INTERRUPT
    },
    {
        "balance",
        {52, 52, 52, 52, 52, 52, 52, 52, 23, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 24},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,              TAR_IGNORE,     	POS_FIGHTING,
        &gsn_balance,            SPELL_OTHER,         0,     0,
        "",                     "!balance!", "",
	GN_STA
    },
    {
        "aura",
        {52, 52, 52, 52, 52, 52, 52, 52,  7, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,              TAR_IGNORE,     	POS_STANDING,
        &gsn_aura,               SPELL_OTHER,         0,    24,
        "",                     "The aura around you fades.", "The blindingly bright aura around $n fades.",
	GN_INTERRUPT
    },
    {
        "healing",
        {52, 52, 52, 52, 52, 52, 52, 52, 22, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,              TAR_IGNORE,     	POS_STANDING,
        &gsn_healing,            SPELL_CURATIVE,         0,    48,
        "",                      "You are ready for another healing session.", "",
	GN_STA
    },
/* Psi stuff */
    {
        "kinesis",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,              TAR_IGNORE,     	POS_STANDING,
        &gsn_kinesis,            SPELL_OTHER,         0,    0,
        "ground impact",                      "!kinesis!",           "",
	GN_STA
    },
    {
        "mind thrust",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  9, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
	spell_mind_thrust,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_MENTAL,       10,     1 * PULSE_VIOLENCE,
	"mind thrust",                "!mind thrust!",        "",
	GN_STA | GN_PSI_WEE
    },
    {
        "mind disruption",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 14, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
	spell_mind_disruption,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_mind_disrupt,                   SPELL_MENTAL,       12,     PULSE_VIOLENCE,
	"mind disruption",                "You can concentrate on spells better.",        "",
	GN_STA | GN_PSI_WEE
    },
    {
        "mind blast",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 21, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
	spell_mind_blast,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_MENTAL,       32,   1 * PULSE_VIOLENCE,
	"mind blast",                "Your mind heals from its injuries.",        "",
	GN_STA | GN_PSI_MED
    },
    {
        "psychic crush",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_psychic_crush,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_MENTAL,       30,     12,
	"psychic crush",                "!psychic crush!",        "",
	GN_STA
    },
    {
        "psionic blast",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_psionic_blast,      TAR_IGNORE,     POS_FIGHTING,
	NULL,                   SPELL_MENTAL,       38,     2 * PULSE_VIOLENCE,
	"psionic blast",                "!psionic blast!",        "",
	GN_STA | GN_PSI_MED
    },
    {
        "psychic purge",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_psychic_purge,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_psychic_purge,       SPELL_MENTAL,       18,     12,
        "",                "Your mind is more resilient.",        "",
	GN_HAR | GN_PSI_STR
    },
    {
        "photon",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  4, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_photon,      TAR_CHAR_SELF,     POS_STANDING,
        NULL,                   SPELL_PROTECTIVE,       50,     18,
        "",                "The photon walls slowly fade away.",        "",
	GN_BEN  | GN_PSI
    },
    {
        "flesh armor",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  5, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_flesh_armor,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_flesh_armor,       SPELL_PROTECTIVE,       5,     12,
        "",                "Your flesh returns to normal.",        "$n's flesh returns to normal.",
	GN_BEN | GN_PSI
    },
    {
        "detect aura",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  5, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_detect_aura,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_detect_aura,                   SPELL_OTHER,       10,     12,
        "",                "You are no longer aware of auras.",        "",
	GN_BEN | GN_PSI
    },
    {
        "displacement",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_displacement,     TAR_CHAR_SELF,     POS_STANDING,
        &gsn_displacement,      SPELL_PROTECTIVE,       5,     12,
        "",                "Your image returns to you.",        "$n's image returns to $m.",
	GN_BEN
    },
    {
        "magic eye",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 25, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_magic_eye,      TAR_IGNORE,     POS_STANDING,
        NULL,                   SPELL_OTHER,       75,     2 * PULSE_VIOLENCE,
        "",                "", "",
	GN_STA
    },
    {
        "mental drain",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  8, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_mental_drain,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_MENTAL,       20,     12,
        "mental drain",                "!mental drain!",        "",
	GN_STA | GN_PSI_MED
    },
    {
        "iron will",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 15, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_iron_will,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_iron_will,       SPELL_PROTECTIVE,      75,     12,
        "",                "Your will is no longer as strong.",        "",
	GN_BEN  | GN_PSI
    },
    {
        "cell adjustment",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_cell_adjustment,      TAR_CHAR_SELF,     POS_STANDING,
        NULL,                   SPELL_OTHER,       25,     24,
        "",                "!cell adjustment!",        "",
	GN_STA
    },
    {
        "bio manipulation",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_bio_manipulation,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_bio_manipulation,                   SPELL_MENTAL,       20,     12,
        "",                "Your body functions return to normal.",        "",
	GN_HAR
    },
    {
        "ego whip",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  3, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_ego_whip,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_ego_whip,                   SPELL_MENTAL,       5,     12,
        "ego whip",                "Your ego recovers.",        "",
	GN_HAR
    },
    {
        "tele lock",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 13, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_tele_lock,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_tele_lock,                   SPELL_MENTAL,       35,     18,
        "",                "You are no longer locked on.",        "",
	GN_HAR | GN_PSI_WEE
    },
    {
        "enhance armor",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 28, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_enhance_armor,      TAR_OBJ_INV,     POS_STANDING,
        NULL,                   SPELL_OTHER,       130,     2 * PULSE_VIOLENCE,
        "",                "!enhance armor!",        "",
	GN_STA
    },
    {
        "there not there",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_there_not_there,      TAR_OBJ_INV,     POS_STANDING,
        &gsn_there_not_there,                   SPELL_OTHER,       150,     PULSE_TICK / 2,
        "",                "You once agian feel capable of phasing things out of existance",        "",
	GN_STA
    },
    {
        "fury",
        {35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_CHAR_SELF,     POS_FIGHTING,
        &gsn_fury,       SPELL_OTHER,      15,     2 * PULSE_VIOLENCE,
        "fury",             "You recover from effects of feral fury.",        "$n recovers from effects of feral fury.",
	GN_STA
    },
    {
        "destruction",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_OBJ_ROOM,     POS_FIGHTING,
        NULL,                   SPELL_OTHER,       5,     18,
        "",                "",        "",
	GN_STA
    },
    {
        "phase shift",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_phase_shift,      TAR_IGNORE,     POS_STANDING,
        NULL,                   SPELL_OTHER,       60,     24,
        "",                "!phase shift!",        "",
	GN_STA
    },
    {
        "mind store",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mind_store,      TAR_IGNORE,     POS_STANDING,
        NULL,                   SPELL_OTHER,       5,     12,
        "",                "!mind store!",        "",
	GN_STA
    },
    {
        "levitate",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 20, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_levitate,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_levitate,       SPELL_PROTECTIVE,       70,    2 *  PULSE_VIOLENCE,
        "",                "You fall to the ground.",        "$n falls to the ground.",
	GN_BEN
    },
    {
        "gravitate",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_gravitate,      TAR_OBJ_ROOM,     POS_STANDING,
        &gsn_gravitate,                  SPELL_OTHER,       10,     18,
        "",                "!gravitate!",        "",
	GN_HAR
    },
    {
        "bio feedback",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_bio_feedback,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_bio_feedback,       SPELL_OTHER,       12,     12,
        "",                "You no longer control your blood flow.",        "",
	GN_BEN
    },
    {
        "psychic healing",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_psychic_healing,      TAR_CHAR_SELF,     POS_FIGHTING,
        NULL,                   SPELL_CURATIVE,       50,     12,
        "",                "!psychic healing!",        "",
	GN_STA
    },
    {
        "pyramid of force",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 26, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_pyramid_of_force,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_pyramid_of_force,                   SPELL_PROTECTIVE,       75,     PULSE_VIOLENCE,
        "", "The pyramid around you shatters into little pieces.", "The pyramid around $n shatters into little pieces.",
	GN_BEN | GN_PSI
    },
    {
        "terror",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_terror,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_MENTAL,       5,     12,
        "",                "!terror!",        "",
	GN_HAR
    },
    {
        "body weaponry",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  2, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_body_weaponry,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_body_weaponry,                   SPELL_AFFLICTIVE,       25,     18,
        "body weaponry",                "Your hands returns to normal.",        "$n's hands return to normal.",
	GN_BEN | GN_PSI
    },
    {
        "ectoplasm",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 34, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_ectoplasm,      TAR_IGNORE,     POS_STANDING,
        &gsn_ectoplasm,       SPELL_OTHER,    150,     2 * PULSE_VIOLENCE,
        "",                "The power to summon more ectoplasm returns.",        "",
	GN_STA
    },
    {
        "hypnosis",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 10, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_hypnosis,      TAR_CHAR_OFFENSIVE,     POS_STANDING,
        &gsn_hypnosis,                   SPELL_MENTAL,       18,     12,
        "",                "You break out of the hypnosis.",        "",
	GN_HAR | GN_PSI_STR
    },
    {
        "field of fear",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_field_of_fear,      TAR_IGNORE,     POS_STANDING,
        &gsn_field_fear,                   SPELL_MENTAL,       50,     12,
        "",                "You feel you can now instill fear again.",        "",
	GN_HAR
    },
    {
        "control flames",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_control_flames,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_MENTAL,       10,     12,
        "flames",                "!control flames!",        "",
	GN_HAR
    },
    {
        "forget",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_forget,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_forget,                   SPELL_MENTAL,       10,     12,
        "",                "You remember your spells.",        "",
	GN_HAR
    },
    {
        "graft weapon",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 37, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_graft_weapon,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_graft_weapon,       SPELL_PROTECTIVE,       45,     PULSE_VIOLENCE,
        "",       "Your weapon separates itself from your hand.",  "$n's weapon separates itself from $s hand.",
	GN_BEN | GN_PSI
    },
    {
        "ballistic attack",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_ballistic_attack,      TAR_OBJ_INV,     POS_FIGHTING,
        NULL,                   SPELL_MENTAL,       25,     12,
        "ballistic attack",                "!ballistic attack!",        "",
	GN_STA
    },
    {
        "mental barrier",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 43, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_mental_barrier,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_mental_barrier,                   SPELL_PROTECTIVE,       25,     12,
        "",                "Your mind is less resilient.",        "",
	GN_BEN | GN_PSI
    },
    {
        "psychic shout",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_psychic_shout,      TAR_IGNORE,     POS_FIGHTING,
        NULL,                   SPELL_MENTAL,       25,     12,
        "psychic shout",                "!psychic shout!",        "",
	GN_STA
    },
    {
        "clairvoyance",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_clairvoyance,      TAR_IGNORE,     POS_STANDING,
        NULL,                   SPELL_MENTAL,       60,     48,
        "anguish",                "!clairvoyance!",        "",
	GN_STA
    },
    {
/*CABAL*/
        "spy",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_STANDING,
        &gsn_eavesdrop,      SPELL_OTHER,       100,    2 * PULSE_VIOLENCE,
        "",              "The spies stop reporting to you.",    "",
	GN_STA
    },
    {
        "kinetics",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_kinetics,                   SPELL_OTHER,       25,     12,
        "!kinetics!",                "Kinetics have worn out.",        "",
	GN_STA
    },
    {
        "empower",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_empower,                   SPELL_OTHER,       25,     12,
        "!empower!",            "You have empowered your abilities.",        "",
	GN_STA
    },
    {
        "offensive",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_offensive,                   SPELL_OTHER,       25,     12,
        "!offensive!",               "Your offensive energy has been depleted.",        "",
	GN_STA
    },
    {
        "defensive",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_defensive,                   SPELL_OTHER,       25,     12,
        "!defensive!",                "Your defensive energy has been depleted.",        "",
	GN_STA
    },
    {
        "air thrash",
        {52, 52, 52, 52, 52, 52, 52, 52, 28, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_thrash,                   SPELL_OTHER,       25,     24,
        "air thrash",                "You can fly again.",        "$n can fly again.",
	GN_STA
    },
    {
        "chii bolt",
        {52, 52, 52, 52, 52, 52, 52, 52, 27, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_chii_bolt,                   SPELL_OTHER,       25,     12,
        "chii bolt",                "You can no longer store your chii as it implodes within you.",        "",
	GN_INTERRUPT
    },
/* CABAL */
    {
        "justice sigil",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},

        spell_justice_sigil,    TAR_IGNORE,     POS_FIGHTING,
        NULL,                   SPELL_OTHER,       35,     12,
        "!error!",                "",        "",
	GN_STA
    },
    {
        "wire delay",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_wire_delay,                   SPELL_OTHER,       25,     12,
        "trip wire",                "You have restrung another trap ready to use.",        "",
	GN_STA
    },
    {
        "entomb",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_entomb,                   SPELL_OTHER,       25,     12,
        "entomb!",           "You step out from the tomb.",        "$n steps out from the tomb.",
	GN_STA
    },
    {
        "insect swarm",
        {52, 52, 38, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_CHAR_OFFENSIVE,     POS_STANDING,
        &gsn_insect_swarm,                   SPELL_MALEDICTION,       60,   24,
        "pollen",           "You sense you could once again summon an insect swarm.",        "",
	GN_STA
    },
    {
        "duplicate",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_duplicate,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
        &gsn_duplicate,              SPELL_OTHER,        150,     2 * PULSE_VIOLENCE,
        "!duplicate!",                "You are able to create a new duplicate.",        "",
	GN_DISPEL | GN_CANCEL
    },
    {
        "eternal day",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_eternal_day,      TAR_IGNORE,     POS_STANDING,
        NULL,                   SPELL_OTHER,        150,     36,
        "!eternal day!",                "You are able to cast another sun.",        "",
	GN_STA
    },
    {
        "everlasting night",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_everlasting_night,      TAR_IGNORE,     POS_STANDING,
        NULL,                   SPELL_OTHER,        150,     36,
        "!everlasting night!",                "You are able to cast another moon.",        "",
	GN_STA
    },
    {
        "ghoul touch",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 45, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_ghoul_touch,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_ghoul_touch,                   SPELL_MALEDICTION,        15,     12,
        "ghoul touch",     "You no longer feel sluggish.",     "$n doesn't look as sluggish.",
	GN_HAR
    },
/* CABAL */
    {
        "apathey",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_apathey,      TAR_CHAR_OFFENSIVE,     POS_STANDING,
        NULL,                   SPELL_MENTAL,        15,     12,
        "apathey",     "You no longer feel apathetic.",     "$n doesn't look as apathetic.",
	GN_HAR
    },
    {
        "feedback",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_feedback,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_feedback,                   SPELL_MENTAL,        15,     12,
        "feedback",     "You no longer relay your pain.",     "$n no longer feeds back.",
	GN_HAR
    },
    {
        "dark ritual",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 50, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10},
        spell_null,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_dark_ritual,                   SPELL_OTHER,        100,     24,
        "dark ritual",     "You are ready to perform another ritual.",     "",
	GN_STA
    },
    {
        "covenant of blood",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_covenant,                   SPELL_OTHER,        100,     24,
        "",     "You are no longer blessed by the Covenant of Blood.",     "",
	GN_STA
    },
    {
        "dispel undead",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 31, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
        spell_dispel_undead,      TAR_CHAR_OFFENSIVE,     POS_STANDING,
        NULL,                   SPELL_AFFLICTIVE,        35,     24,
        "dispel undead",     "", "",
	GN_STA
    },
    {
        "restore faith",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 35, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
        spell_restore_faith,      TAR_CHAR_DEFENSIVE,     POS_STANDING,
        NULL,                   SPELL_RESTORATIVE,        15,     12,
        "",     "", "",
	GN_STA
    },
    {
        "cone of force",
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_cone,                   SPELL_MENTAL,       50,     PULSE_VIOLENCE,
        "cone of force",           "Your mental energy has charged up.",        "",
	GN_STA
    },
    {
        "burrow",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 30, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_burrow,                   SPELL_OTHER,       50,     48,
        "burrow",           "",      "",
	GN_STA
    },
    {
        "werepower",
        {42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_werepower,                  SPELL_OTHER,       0,		0,
        "werepower",          "You can once again call on your werebeast's powers.",        "",
	GN_STA
    },
    {
        "cutpurse",
        {52, 52, 52, 52, 52, 38, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_cutpurse,                   SPELL_OTHER,       25,     24,
        "cutpurse",           "You realize there is a hole in your purse and close it up.",        "",
	GN_STA
    },
    {
        "offhand disarm",
        {33, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_offhand_disarm,                   SPELL_OTHER,       25,     24,
        "Offhand Disarm",           "",        "" ,
	GN_STA
    },
    {
        "were-tiger",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_weretiger,                   SPELL_OTHER,       15,     2 * PULSE_VIOLENCE,
        "tackle",           "You return to your natural form.",        "$n transforms back to $s natural form.",
	GN_STA
    },
    {
        "were-wolf",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_werewolf,                   SPELL_OTHER,       25,     1 * PULSE_VIOLENCE,
        "",           "You return to your natural form.",        "$n transforms back to $s natural form.",
	GN_STA
    },
    {
        "were-bear",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_werebear,                   SPELL_OTHER,       15,     2 * PULSE_VIOLENCE,
        "bearhug",          "You return to your natural form.",        "$n transforms back to $s natural form.",
	GN_STA
    },
    {
        "probe",
	{ 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  4},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_probe,                   SPELL_OTHER,       0,     24,
        "",           "",        "",
	GN_STA
    },
    {
        "transform",
        {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_transform,                   SPELL_OTHER,       50,     2 * PULSE_VIOLENCE,
        "",           "You can once again tranform into your hidden form.",  "",
	GN_STA
    },
    {
        "were-falcon",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_werefalcon,                   SPELL_OTHER,      10,     PULSE_VIOLENCE / 3,
        "",           "You return to your natural form.",        "$n transforms back to $s natural form.",
	GN_STA
    },
    {
        "fasting",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_null,     TAR_IGNORE,     POS_STANDING,
        &gsn_fasting,      SPELL_OTHER,      55,    12,
        "",              "You are no longer resistant to thirst and hunger.",    "",
	GN_INTERRUPT
    },
    {
        "shed skin",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_shed_skin,                   SPELL_OTHER,       25,     24,
        "tail",           "You can cast off another skin.",       "",
	GN_STA
    },
    {
        "crusade",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 32, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10}, 
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_crusade,      SPELL_PROTECTIVE,        50,     36,
        "",                     "Your crusade ends.",     "",
	GN_STA
    },
    {
        "heroism",
        {52, 52, 52, 40, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}, 
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_heroism,           SPELL_OTHER,        50,     36,
        "",                     "You no longer feel so virtuous.",     "",
	GN_BEN
    },
    {
        "decoy",
        {52, 52, 52, 52, 52, 52, 33, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_decoy,                   SPELL_OTHER,       25,     24,
        "",           "",       "",
	GN_STA
    },
    {
        "refrain",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 23, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_refrain,                   SPELL_OTHER,       0,     24,
        "",           "Your mind is cleared of the song's melody.",       "",
	GN_STA
    },
    {
        "tarot cards",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 30, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_tarot,                   SPELL_OTHER,       50,     24,
        "tarot cards",           "The affects of the tarot cards wear off.",       "",
	GN_STA
    },
    {
        "knockout",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 17, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_tko,                   SPELL_OTHER,       50,      24,
        "knock out",           "Your head stops spinning.",       "",
	GN_STA
    },
    {
        "create torch",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 1, 22, 52,  2, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1,  1, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        NULL,                SPELL_OTHER,       50,      24,
        "",           "",       "",
	GN_STA
    },
    {/* used for counting ignored bounties in syndicate */
        "analyze",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 13, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        NULL,                   SPELL_OTHER,       50,      24,
        "",           "",       "",
	GN_STA | GN_NODEATH
    },
    {
        "panhandle",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 16, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        NULL,                   SPELL_OTHER,       50,      24,
        "",           "",       "",
	GN_STA
    },
    {
        "lure",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_attract,                   SPELL_OTHER,       50,      12,
        "",           "You break free from your attracion.",       "$n gains control of $s will.",
	GN_HAR
    },
    {
        "uppercut",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 24, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_uppercut,                   SPELL_OTHER,       50,      24,
        "uppercut",           "Your jaws feel better.",       "",
	GN_STA
    },
    {
        "taunt",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 27, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_taunt,                   SPELL_OTHER,       50,      24,
        "taunt",           "You are no longer affected by the taunt.",       "",
	GN_STA
    },
    {
        "tune",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 34, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        NULL,                   SPELL_OTHER,       50,      24,
        "",           "",       "",
	GN_STA
    },
    {
        "duet",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 22, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        NULL,                   SPELL_OTHER,       0,      12,
        "",           "",       "",
	GN_STA
    },
    {
        "leech",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_leech,                   SPELL_OTHER,       50,     24,
        "leech",           "The effects of the leech has worn off.",       "",
	GN_STA
    },
    {
        "no quit",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_noquit,                   SPELL_OTHER,       50,     24,
        "",           "You are allowed to leave the realms.",       "",
	GN_STA
    },
    {
        "heal mana",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 32, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_cure_mana,      TAR_CHAR_SELF,     POS_FIGHTING,
        NULL,                   SPELL_OTHER,       0,     12,
        "",           "",       "",
	GN_STA
    },
    {
        "brawl",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 33, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  6, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_brawl,                   SPELL_OTHER,       50,      24,
        "",           "",       "",
	GN_STA
    },
    {
        "virtuous light",
        {52, 52, 52, 29, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_virtuous_light,	TAR_CHAR_SELF,          POS_STANDING,
	NULL,                   SPELL_OTHER,       25,      12,
	"",                     "The light of virtue fades from your body.",     "",
	GN_BEN
    },
    {
        "unholy strength",
        {52, 52, 52, 52, 25, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_unholy_strength,     TAR_IGNORE,     POS_STANDING,
	&gsn_unholy_strength,      SPELL_OTHER,       15,     12,
	"",                     "",        "",
	GN_BEN
    },
    {
        "hex",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 17, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_hex,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_hex,          SPELL_OTHER,       20,     12,
	"",                     "The hex wears off.",        "",
	GN_HAR
    },
    {
/*CABAL*/
        "ram",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,         TAR_IGNORE,     POS_STANDING,
        &gsn_ram,          SPELL_OTHER,      20,    24,
        "",              "",    "",
	GN_STA
    },
    {
      "blood gout",   
      {52, 52, 52,52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 27, 52, 52, 52, 52},
      {10, 10, 10, 10,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10, 10, 10, 10},
       spell_blood_gout,         TAR_IGNORE,     POS_FIGHTING,
       NULL,          SPELL_AFFLICTIVE,      20,    12,
       "blood gout",              "!Blood Gout!",    "",
	GN_STA
    },
    {
      "embrace",
     {52, 52, 52,52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 38, 52, 52, 52, 52},
     {10, 10, 10, 10,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10, 10, 10, 10},
      spell_null,         TAR_IGNORE,     POS_FIGHTING,
      &gsn_embrace,          SPELL_AFFLICTIVE,      20,    12,
      "tainted bite",              "",    "",
	GN_STA
    },
    //This is for the poison effect of embrace, I chose a gsn flag over a bitvector as
    //bit vectors are finite in supply:) Viri.
    {
      "vampire bite",
     {62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62},
     {10, 10, 10, 10,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,10, 10, 10, 10, 10},
      spell_null,         TAR_CHAR_OFFENSIVE,     POS_STANDING,
      &gsn_embrace_poison,          SPELL_MALEDICTIVE,      0,    0,
      "blood of undead",              "", "",
	GN_STA
    },
    
    {
      "holy hands",
      {62, 62, 62, 62, 62, 62, 62, 50, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62},
      {10, 10, 10, 10,  10, 10,10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_holy_hands,         TAR_CHAR_SELF,     POS_STANDING,
      &gsn_holy_hands,      SPELL_MALEDICTIVE,      15,    PULSE_VIOLENCE,
      "corruption",              "", "",
      GN_STA
    },
/* CABAL */
    {
        "elemental call",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_elemental_call,     TAR_CHAR_SELF,   POS_STANDING,
	NULL,              SPELL_PROTECTIVE,      65,     2 * PULSE_VIOLENCE,
	"",    "You are no longer protected by the elements of nature.", "",
	GN_BENEFICIAL | GN_NOCANCEL | GN_EXTEND,
    },
    {
      "!behead!",
     {62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62},
     {10, 10, 10, 10,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_behead,         TAR_CHAR_OFFENSIVE,     POS_STANDING,
      NULL ,                 SPELL_MALEDICTIVE,      0,    0,
      "",              "", "",
      GN_STA
    },
    {
//This is for the Continuum which allows vamp to sense each other's death.
      "Continuum",
     {62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62},
     {10, 10, 10, 10,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,         TAR_CHAR_OFFENSIVE,     POS_STANDING,
      &gsn_continuum,          SPELL_MALEDICTIVE,      0,    0,
      "",              "", "",
      GN_STA
    },
    {
      "tear",
     {62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62},
     {10, 10, 10, 10,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,         TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
      &gsn_tear,          SPELL_MALEDICTIVE,      0,    0,
      "tear",              "", "",
      GN_STA
    },
    {
      "unholy gift",
     {62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 50, 62, 62, 62, 62},
     {10, 10, 10, 10,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_unholy_gift,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
      &gsn_unholy_gift,          SPELL_MALEDICTIVE,      0,    0,
      "Unholy Gift",              "", "",
      GN_STA
    },
    {
        "firestorm",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 35, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_firestorm,        TAR_IGNORE,     	POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       60,    24,
        "firestorm",            "!Firestorm!",          "",
      GN_STA
    },
    {
        "mana charge",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 36, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,        TAR_IGNORE,     	POS_STANDING,
        &gsn_mana_charge,       SPELL_AFFLICTIVE,       0,    4,
        "mana flare",            "You once again feel capable of infusing mana into items. ",          "",
	GN_STA
    },
    {
        "disguise",   
        {52, 52, 52, 52, 52, 25, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 31, 52, 52, 52},
        {10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  2, 10, 10, 10},
        spell_null,        TAR_IGNORE,     	POS_STANDING,
        &gsn_disguise,       SPELL_AFFLICTIVE,       0,    36,
        "disguise",            "You are confident you can attempt to disguise yourself once again.",          "",
	GN_STA
    },
    {
        "push",   
        {52, 52, 52, 52, 52, 52, 35, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,        TAR_IGNORE,     	POS_STANDING,
        &gsn_push,         SPELL_AFFLICTIVE,       30,    24,
        "palm strike",            "",          "",
	GN_STA
    },
    {
        "study",   
        {52, 52, 52, 52, 52, 52, 38, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,        TAR_IGNORE,     	POS_STANDING,
        &gsn_study,         SPELL_AFFLICTIVE,       80,     12,
        "",            "You stop studying your target and prepare to meditate.",          "",
	GN_STA
    },
/*CABAL*/
    {   "warmaster code",             
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_CHAR_SELF,      POS_STANDING,
	&gsn_warmaster_code,        SPELL_OTHER,       100,     2,
	"",   "","",
	GN_STA
    },
    {
        "haymaker",   
        {52, 36, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,        TAR_IGNORE,     	POS_STANDING,
        &gsn_haymaker,         SPELL_AFFLICTIVE,       0,     24,
        "punch",            "Your hurt hand has healed sufficiently to be of use again.",          "",
	GN_STA
    },

    {
        "malform weapon",     
        {52, 52, 52, 52, 43, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_malform_weapon,   TAR_OBJ_INV,            POS_STANDING,
	NULL,                   SPELL_OTHER,       500,    1 * PULSE_TICK,
	"evil",                 "You once again feel capable of attempting to malform a weapon.",     "",
	GN_STA
    },
    {
/*CABAL*/
        "gift",  
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_gift,          TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_OTHER,       25,    2*PULSE_VIOLENCE,
        "",              "!gift!",    "",
	GN_STA
    },
    {
      "corruption",     
      {42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,        TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
      &gsn_corrupt,                   SPELL_MALEDICTION,       10,    PULSE_VIOLENCE,
      "corruption",                 "You feel the influence of Chaos receding.",     "The slight flaws in reality around $n slowly fade away.",
      GN_STA
    },

    {
        "isolate",      
        {52, 52, 52, 52, 52, 52, 52, 45, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_isolate,     TAR_CHAR_OFFENSIVE,     POS_STANDING,
	NULL,                   SPELL_MENTAL,       40,     12,
	"",                     "!Isolate!",        "",
      GN_STA
    },

    {
        "path of deceit",      
        {52, 52, 52, 52, 52, 52, 52, 40, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_path_of_deceit,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_MENTAL,       30,     12,
	"evil aura",                 "!path of deceit!",        "",
      GN_STA
    },

    {
        "minister",      
        {52, 52, 52, 52, 52, 52, 52, 42, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_minister,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_MENTAL,       20,     12,
	"sermons",           "!sermon!",        "",
	GN_STA
    },

    {
        "bless arms",     
        {52, 52, 52, 52, 52, 52, 52, 32, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_bless_arms,   TAR_OBJ_INV,            POS_STANDING,
	NULL,                   SPELL_OTHER,       100,    1 * PULSE_TICK,
	"evil",                 "",     "",
	GN_STA
    },
    {
        "divine intervention",     
        {52, 52, 52, 52, 52, 52, 52, 50, 52, 52, 52, 52, 52, 50, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_divine_int,   TAR_CHAR_SELF,            POS_FIGHTING,
	NULL,                   SPELL_OTHER,       65,    1 * PULSE_VIOLENCE,
	"wrath",                 "You feel that once again you might ask for $g's favor.",     "",
	GN_STA
    },
    {
        "voodoo",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 50, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
	spell_null,      TAR_CHAR_DEFENSIVE,            POS_STANDING,
	&gsn_voodoo_spell,                   SPELL_OTHER,       300,    2 * PULSE_VIOLENCE,
	"voodoo",                 "The powers of voodoo are with you once again.",     "",
	GN_STA
    },

/* BATTLE MAGE */
    {
        "force field",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 11, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_forcefield,      TAR_CHAR_SELF,            POS_STANDING,
	&gsn_forcefield,           SPELL_PROTECTIVE,       100,     12,
	"!forcefield!", "The force field shielding you collapses.", "The force field shielding $n collapses.",
	GN_BEN
    },

    {
        "terra shield",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 15, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_terra_shield,      TAR_CHAR_SELF,            POS_STANDING,
	&gsn_terra_shield,     SPELL_PROTECTIVE,       35,     12,
	"!forcefield!", "You lose your control over the ground around you.", "The ground at $n's feet grows still.",
	GN_BEN
    },

    {
        "localize gravity ",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 21, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_loc_grav,      TAR_IGNORE,            POS_FIGHTING,
	NULL,            SPELL_AFFLICTIVE,       23,     12,
	"!localize!", "", "",
	GN_BEN
    },

    {
        "rust",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 23, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_rust,      TAR_CHAR_OFFENSIVE,            POS_FIGHTING,
	&gsn_rust,            SPELL_MALEDICTIVE,       5,     12,
	"!rust!",        "The thick layer of rust slowly flakes off from your possesions.", "$n's possesions shed a thick coat of reddish powder.",
	GN_HAR
    },

    {
        "air shield",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 28, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_air_shield,      TAR_CHAR_SELF,            POS_STANDING,
	&gsn_air_shield,       SPELL_PROTECTIVE,       33,     12,
	"!air_shield!",        "The dense cusion of air around you dissapates.", "The air around $n blurs and shimmers momentarly.",
	GN_BEN
    },

    {
        "blades",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 31, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  6, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_blades,      TAR_CHAR_SELF,            POS_STANDING,
	&gsn_blades,       SPELL_PROTECTIVE,       20,     12,
	"scythe",        "You sense the deadly scythes phase away from this reality.", "A translucent outline of something wavers and disappears.",
	GN_BEN
    },

    {
	"field dressing",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 35, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_RESTING,
	&gsn_field_dress,      	SPELL_OTHER,        50,     24,
	"", 		"You are once again ready to tend to your wounds.",	"",
	GN_STA
    },

    {
        "sharpmetal",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 38, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_sharpmetal,      TAR_CHAR_OFFENSIVE,            POS_FIGHTING,
	NULL,            SPELL_AFFLICTIVE,       8,     12,
	"sharpmetal",        "", "",
	GN_STA
    },

    {
        "plumbum manus",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_plumbum,      TAR_CHAR_SELF,            POS_STANDING,
	&gsn_plumbum,       SPELL_AFFLICTIVE,       25,     12,
	"plumbum",        "", "",
	GN_BEN
    },


    {
        "sear",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 44, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_sear,      TAR_CHAR_OFFENSIVE,            POS_FIGHTING,
	NULL,            SPELL_AFFLICTIVE,       45,     24,
	"searing beam",        "", "",
	GN_STA
    },

    {
      "dancing blade",     
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 46, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  6, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_dan_blade,      TAR_OBJ_INV,            POS_STANDING,
      &gsn_dan_blade,     SPELL_OTHER,       45,     12,
      "dancing blade",        "You can once again enchant a weapon to do your bidding.", "",
      GN_BEN
    },
    {
        "manalock",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 48, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_manalock,      TAR_CHAR_SELF,            POS_STANDING,
	&gsn_mana_lock,     SPELL_PROTECTIVE,       50,     24,
	"manalock",        "Your mana lock disintagrates sending waves of energy through the area.", "The area around $n shimmers and blurs.",
	GN_BEN
    },
    {
        "reflective shield",     
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 50, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_ref_shield,          TAR_CHAR_SELF,            POS_STANDING,
	&gsn_ref_shield,     SPELL_PROTECTIVE,       55,     24,
	"refshield",        "Your reflective shield collapses rapidly expanding.", "You are suddenly thrown back with huge force.",
	GN_BEN
    },
    {
/*CABAL*/
        "anger", 
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,         TAR_IGNORE,     POS_STANDING,
        NULL,               SPELL_OTHER,      50,    2 * PULSE_VIOLENCE,
        "",              "",    "",
	GN_INTERRUPT
    },
    {
        "chameleon",             
        {42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_chameleon,        SPELL_OTHER,       25,      12,
	"","You can once again attempt to chameleon.","",
	GN_STA
    },
    {
        "two handed",             
        {1, 1, 14, 1, 1, 52, 52, 32, 52, 52, 52, 52, 52, 52, 52, 5, 52, 52, 1,   15,  10},
        {6, 7,  8,  8,  8, 10, 10, 1, 10, 10, 10, 10, 10, 10, 10,  8, 10, 10, 1,  7,   6},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	&gsn_2hands,        SPELL_OTHER,       0,      0,
	"!2hands!","", "",
	GN_STA
    },
    {
        "comprehend languages",             
        {52, 52, 52, 52, 52, 52, 52,  2, 52,  9, 13,  5,  1, 12, 14, 52, 52, 12, 52, 52},
        {10, 10, 10, 10, 10, 10, 10,  1, 10,  1,  1,  1,  1,  1,  1, 10, 10,  1, 10, 10},
	spell_comprehend_lan,             TAR_CHAR_DEFENSIVE,             POS_STANDING,
	&gsn_com_lan,        SPELL_OTHER,       25,      12,
	"!langague!", "You no longer understand all tongues.", "",
	GN_BEN
    },
    {
      "darklife", /* Tatto spell */
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_darklife,        TAR_IGNORE,		POS_FIGHTING,
      NULL,       SPELL_OTHER,       	30,   12,
      "!DarkLife!",     "!Darklife!",    "",
	GN_STA
    },
    {
/*CABAL*/
      "shroud of discord",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_discord,        TAR_CHAR_SELF,   POS_STANDING,
      NULL,              SPELL_PROTECTIVE,      15,     12,	
	"!Discord!",    "!Discord!", "",
	GN_BEN
    },

    {
/*CABAL*/
      "reverse time",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_rev_time,TAR_CHAR_SELF,   POS_STANDING,
      & gsn_rev_time,    SPELL_MENTAL,      60,     2 * PULSE_VIOLENCE,	
      "time stream",    "You may once again reverse the flow of time.", "",
      GN_STA
    },
    {
/*CABAL*/
      "call void",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_void,        TAR_IGNORE,   POS_FIGHTING,
      NULL,              SPELL_OTHER,      15,     PULSE_VIOLENCE,	
	"call void",    "", "",
      GN_STA
    },
    {
/*CABAL*/
      "eye of moloch",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_orb_chaos,        TAR_CHAR_SELF,   POS_FIGHTING,
      NULL,              SPELL_OTHER,      30,     12,	
	"orb of chaos",    "", "",
      GN_BEN
    },
    /* Item spell Only */
    {
        "purge unlife",             
        {52, 52, 52, 52, 52, 52, 52,  52, 52,  52, 52,  52,  52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_purge_unlife,             TAR_CHAR_DEFENSIVE,             POS_STANDING,
	NULL,        SPELL_RESTORATIVE,       25,      12,
	"taint", ".", "",
	GN_STA
    },
    {
/*CABAL*/
        "mold chaos",             
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	NULL,        SPELL_OTHER,       5,      24,
	"!CALL!", "You feel you can once again attempt to control Chaos.", "",
	GN_STA
    },
    /* New Conclave spells */
    {
/*CABAL*/
      "fade",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,        TAR_CHAR_SELF,   POS_FIGHTING,
      &gsn_fade,              SPELL_MENTAL,      0,     0,	
      "fade",    "", "",
      GN_STA
    },
    {
/*CABAL*/
      "conceal",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,        TAR_CHAR_SELF,   POS_FIGHTING,
      NULL,              SPELL_OTHER,      5,     12,	
	"",    "", ""	,
      GN_STA
    },
    {
      "sacred runes",
      {36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_sacred_runes,        TAR_CHAR_SELF,   POS_STANDING,
      &gsn_sacred_runes,         SPELL_PROTECTIVE,      40,     24,	
	"ritual",   "You feel $g's power recede from your flesh.", "The bleeding runes fade away from $n's flesh.",
      GN_BEN
    },
    {
      "visitation",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 42, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10},
      spell_visitation,       TAR_CHAR_OFFENSIVE,   POS_FIGHTING,
      NULL,              SPELL_MALEDICTION,      28,     12,	
	"!Visitation!",   "", "",
      GN_HAR
    },
    /* SAVANT Damage Shield */
    {
/*CABAL*/
        "efuzan warding",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_sav_ward,          TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_PROTECTIVE,       250,    2*PULSE_VIOLENCE,
        "",              "The powerful runes fade from around you.",    "The powerful runes fade from around $n.",
      GN_BEN
    },
    /* JUSTICE ELDER SENTINELS */
    {
/*CABAL*/
        "observer call",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_sentinel_call,     TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_OTHER,       40,    PULSE_VIOLENCE,
        "",              "A new observer has just become avaliable.",    "",
	GN_STA
    },
    {
      /* TATTOO ONLY SPELL */
        "control fauna",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 20, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_verdigar_tat,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,      SPELL_OTHER,       40,    2*PULSE_VIOLENCE,
        "",              "",    "",
	GN_STA
    },
    /* physical (non-magical) healin' baby :) */
    {
        "endorphin rush",    
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_endorphin_rush,     TAR_CHAR_DEFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_CURATIVE,       15,     12,
	"",                     "!Endorphin Rush!",       "",
	GN_STA
    },
    {
/* CABAL */
        "serpent call",           
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_serpent,     TAR_IGNORE,     POS_STANDING,
	&gsn_serpent,                   SPELL_OTHER,    100,     2 * PULSE_VIOLENCE,
	"",                     "You may once again call for a steed.",      "",
	GN_STA
    },
//Druid circle of protection 12-02-00 -Shadow
    {
        "circle of protection",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 40, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_circle_of_protection,          TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_PROTECTIVE,       250,    2*PULSE_VIOLENCE,
        "",              "The circle of protection around you fades.", "The circle of protection fades from around $n.",
	GN_BEN
    },
    {
        "shield of thorns",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 35, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_thorn_shield,        TAR_CHAR_SELF, POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,      75,     12,
        "thorns",             "The shield of thorns falls away from your body.", "The shield of thorns falls away from $n's body.",
	GN_BEN
    },
    {
        "lifeforce",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 35, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_lifeforce,        TAR_CHAR_DEFENSIVE,     POS_STANDING,
        &gsn_lifeforce,         SPELL_PROTECTIVE,       75,     12,
        "",             "The green aura around your body fades.",   "The green aura around $n fades.",
	GN_BEN
    },
    {
        "forest mist",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 15, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_forest_mist,          TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_forest_mist,                   SPELL_MALEDICTION,       50,    12,
        "",              "The thick mist surrounding you dissapates.", "The thick mist surrounding $n dissipates.",
	GN_HAR
    },
    {
        "restoration",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 44, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_restoration,        TAR_CHAR_DEFENSIVE, POS_STANDING,
        NULL,                   SPELL_RESTORATIVE,      60,     12,
        "",             "!Restoration!",          "",
	GN_STA
    },
    {
        "treeform",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 42, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_treeform,       TAR_CHAR_SELF,   POS_STANDING,
        &gsn_treeform,        SPELL_OTHER,       100,   24,
        "","Your body regains its normal form.", "$n's body regains its normal form.",
	GN_BEN
    },
    {
        "druid staff",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 34, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_null,       TAR_IGNORE, POS_FIGHTING,
        &gsn_druid_staff,       SPELL_OTHER,       80,   24,
        "",     "You feel up to creating another druid staff.",    "",
	GN_STA
    },
    {
        "trap of silvanus",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 26, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_trap_silvanus,                   SPELL_OTHER,       25, 12,
        "trap of silvanus",                "Your trap collapses.", "$n's trap collapses.",
	GN_STA
    },
    {
        "druid gate",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 30, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_druid_gate,         TAR_CHAR_SELF,          POS_STANDING,
        NULL,                   SPELL_OTHER,       35,     12,
        "",                     "!Druid_Gate!",           "",
	GN_STA
    },
    {
        "entangle",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 28, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
        &gsn_entangle,          SPELL_OTHER,       0,     24,
        "entangle",             "The thornbriars disappear around you.", "The thornbriars disappear around $n.",
	GN_HAR
    },
    {
/*CABAL*/
        "fatality",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,          TAR_IGNORE,     POS_DEAD,
        &gsn_fatality,      SPELL_OTHER,       0,    0,
        "",              "`!Finish him!``",    "",
	GN_STA
    },
    {
/*CABAL*/
        "hound call",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_hound_call,     TAR_IGNORE,     POS_STANDING,
        NULL,      SPELL_OTHER,       10,    12,
        "",              "You can call another hound.",    "",
	GN_STA
    },
    {
/*CABAL*/
        "collect bounty",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,          TAR_IGNORE,     POS_DEAD,
        &gsn_bounty_collect,      SPELL_OTHER,       0,    0,
        "",              "`!Finish him!``",    "",
	GN_STA
    },
    {
/*CABAL*/
        "deadfall",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_deadfall,                   SPELL_OTHER,       25,     12,
        "deadfall",                "You rub your aching temples.", "$n rubs $s pounding temples.",
	GN_STA
    },
    {
        "sequencer",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 43, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        NULL,            SPELL_OTHER,       35,     1 * PULSE_VIOLENCE,
        "!SEQUNCER!",                "", "",
	GN_STA
    },
    {
        "songbird",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_songbird,         TAR_IGNORE,     POS_FIGHTING,
	NULL,                   SPELL_PROTECTIVE,      30,     12,
	"",                     "You feel yourself slow down.", "$n slows down.",
	GN_BEN
    },
    {
      /* SYSTEM USE ONLY */
        "pardon",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,         TAR_IGNORE,     POS_FIGHTING,
	NULL,                   SPELL_PROTECTIVE,      30,     PULSE_VIOLENCE,
	"",                     "", "",
	GN_STA
    },

    {
      /* Raght's Hourglass spell */
        "reveal hidden",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_reveal_hidden,         TAR_IGNORE,     POS_FIGHTING,
	NULL,                   SPELL_OTHER,      0,     PULSE_VIOLENCE,
	"",                     "", "",
	GN_STA
    },
    {
        "resurrection",       
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 30, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10},
	spell_resurrection,        TAR_OBJ_ROOM, POS_STANDING,
	&gsn_resurrection,    SPELL_AFFLICTIVE,      200,     2*PULSE_VIOLENCE,
	"resurrection",             "You sense you lost your chance to be brough back to life.",          "",
	GN_STA
    },
    {
        "life-insurance",    
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_life_insurance,   TAR_CHAR_DEFENSIVE,     POS_STANDING,
	&gsn_life_insurance,    SPELL_PROTECTIVE,       15,     12,
	"",                     "You feel less secure about death!",       "",
	GN_STA
    },
     {
        "lifedrain",    
	{ 1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,		TAR_CHAR_OFFENSIVE,     POS_STANDING,
	&gsn_lifedrain,    SPELL_MALEDICTION,        15,     12,
	"touch",         "You regain your vitality.",       "",
	GN_STA
     },
    {
      /* Eirik's tattoo spell */
        "mark of prey",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_mark_prey,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_mark_prey,                   SPELL_MALEDICTION,      0,    PULSE_VIOLENCE,
	"",                     "You feel safer.", "",
	GN_STA
    },
    {
      /* Savant item spell */
        "light sword",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_light_sword,        TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,                   SPELL_AFFLICTIVE,   0,    PULSE_VIOLENCE,
	"light sword",                    "", "",
	GN_STA
    },
    /* Crusader Skills */
    {
        "shoulder smash",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 11, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10},
	spell_null,        TAR_CHAR_OFFENSIVE,     POS_STANDING,
	NULL,              SPELL_AFFLICTIVE,    5,    2 * PULSE_VIOLENCE,
	"shoulder smash",                    "The bruises left by the shoulder smash heal up.", "",
	GN_STA
    },
    {
        "armored rush",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 15, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10},
	spell_null,        TAR_CHAR_OFFENSIVE,     POS_STANDING,
	&gsn_armored_rush, SPELL_AFFLICTIVE,   25, 2 * PULSE_VIOLENCE,
	"thwack",                    "", "",
	GN_STA
    },
    {
        "batter",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 15, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10},
	spell_null,        TAR_CHAR_OFFENSIVE,     POS_STANDING,
	&gsn_batter,              SPELL_AFFLICTIVE,   15,    2 * PULSE_VIOLENCE / 3,
	"!batter!",                    "You recover from the battering you've recived.", "",
	GN_STA
    },
    {
        "impale",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 18, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10},
	spell_null,        TAR_CHAR_OFFENSIVE,     POS_STANDING,
	NULL,              SPELL_AFFLICTIVE,   25,    PULSE_VIOLENCE,
	"impale",                    "You are once more vulnerable to impaling.", "",
	GN_STA
    },
    {
        "destroy undead",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 29, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10},
	spell_null,        TAR_CHAR_OFFENSIVE,     POS_STANDING,
	NULL,              SPELL_AFFLICTIVE,   80,    2 * PULSE_VIOLENCE,
	"destroy undead",                    "You once again feel up to the task of eradicating the unliving.", "",
	GN_STA
    },
    {
        "avenger",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 31, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10},
	spell_null,        TAR_CHAR_OFFENSIVE,     POS_STANDING,
	&gsn_avenger,              SPELL_AFFLICTIVE,   25,    PULSE_VIOLENCE,
	"!avenger!",                    "", "",
	GN_STA
    },
    {
        "windmill",            
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 35, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10},
	spell_null,        TAR_CHAR_OFFENSIVE,     POS_STANDING,
	&gsn_windmill,              SPELL_AFFLICTIVE,   15,    2 * PULSE_VIOLENCE,
	"!windmill!",                    "You stop swinging your weapon", "$n stops swinging $s weapon",
	GN_INTERRUPT
    },
    {
        "psionic control",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_psi_control,      TAR_IGNORE,     POS_STANDING,
        NULL,                   SPELL_OTHER,       55,     24,
        "",                "You have some control over your psionic blast.",        "",
	GN_BEN
    },
    {
        "vomit",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_RESTING,
        &gsn_vomit,                   SPELL_OTHER,       0,     0,
        "",                "",   "",
	GN_STA
    },  
    {
/* CABAL */
        "bolo",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_bolo,                   SPELL_MALEDICTION,       25,     24,
        "bolo",                "Your legs have healed.",   "",
	GN_STA
    },  
    /* USED FOR SIRANT TATTOO ONLY */
    {
        "swords to plows",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 },
        spell_swords_to_plows,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,        0,     12,
        "!sword to plow!",                "", "",
	GN_STA
    },
    {
        "death breath",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 },
        spell_death_breath,  TAR_IGNORE,     POS_FIGHTING,
        NULL,                SPELL_AFFLICTIVE,        0,     12,
        "black breath",                "", "",
	GN_STA
    },
    {
        "socketed object",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52 },
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 },
        spell_null,		    TAR_IGNORE,     POS_STANDING,
        NULL,                SPELL_OTHER,        0,     0,
        "",                "", "",
	GN_STA
    },
    {
        "fear",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_STANDING,
        &gsn_fear,                SPELL_MENTAL,       50,     12,
        "",                "You shake off the fear that has gripped you.",        "",
	GN_HAR
    },
    {
        "drug use",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_drug_use,      TAR_CHAR_OFFENSIVE,     POS_STANDING,
        &gsn_drug_use,            SPELL_MENTAL,       50,     12,
        "",                "The affects of the drugs wear off.",        "",
	GN_STA
    },
    {
      "doomsday",
     {62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62},
     {10, 10, 10, 10,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_doomsday,         TAR_CHAR_OFFENSIVE,     POS_STANDING,
      NULL ,                 SPELL_MALEDICTIVE,      0,    0,
      "",              "", "",
      GN_STA
    },
    {
        "marshall",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_marshall,        TAR_IGNORE,     POS_FIGHTING,
        NULL,      SPELL_OTHER,       100,    12,
        "",              "You can once again marshal your troops.",    "",
	GN_STA
    },                             
    /* CABAL */
    {
        "royal escort",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_FIGHTING,
        NULL,      SPELL_OTHER,       50,    12,
        "",              "You will no longer summon  new royal guards.",    "",
	GN_STA
    },
    {
/*CABAL*/
      "research",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,        TAR_CHAR_SELF,   POS_FIGHTING,
      &gsn_research,              SPELL_OTHER,      30,     12,
        "fortification research",    "", "",
      GN_STA
    },
    {
/*CABAL*/
      "roc call",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},

      spell_null,         TAR_IGNORE,     POS_STANDING,
      &gsn_call_roc,               SPELL_OTHER,      50,    2 * PULSE_VIOLENCE,
      "",              "You feel up to calling another roc.",    "",
      GN_STA
    },   
    {
        "decipher",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10,  1,  1,1,  1,  1,  1,  1},
        spell_null,         TAR_IGNORE,     POS_STANDING,
        &gsn_decipher,               SPELL_OTHER,      50,    PULSE_VIOLENCE,
        "",              "You feel up to deciphering again.",    "",
      GN_STA
    },
    {
        "torment bind",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_torment_bind,     TAR_IGNORE,     POS_STANDING,
        &gsn_torment_bind,               SPELL_OTHER,       25,     PULSE_VIOLENCE,
        "!torment bind!",                "",        "",
      GN_STA
    },
    {
        "fired projectile",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_fired_projectile,      TAR_OBJ_INV,     POS_FIGHTING,
        NULL,                   SPELL_AFFLICTIVE,       15,    12,
        "fired projectile",          "!Fired Projectile!",         "",
      GN_STA
    },  
    {
        "paralyze",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_paralyze, TAR_CHAR_OFFENSIVE,     POS_STANDING,
        &gsn_paralyze,  SPELL_MENTAL,   160,    2 * PULSE_VIOLENCE,
        "!paralyze!",              "The effects of paralysis wear off.",    "$n is no longer paralyzed.",
      GN_HAR
    },   
    {
        "fired weapons",
        { 43, 52, 16, 52, 52, 28, 40, 52, 52, 52, 52, 52, 52, 52, 52, 52, 26, 52, 52, 15},
        {  6, 10,  4, 10, 10, 10,  6, 10, 10, 10, 10, 10, 10, 10, 10, 10,  6, 10, 10,  5},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_fired,             SPELL_OTHER,       5,   2 * PULSE_VIOLENCE,
        "projectile",                     "!Fired!",              "",
      GN_STA
    },        
/* CRUSADER MINOR SELECTIONS */
    {
        "2h handling",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_2h_handling,			SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
      GN_STA
    }, 
    {
        "2h tactics",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_2h_tactics,			SPELL_OTHER,        0,     PULSE_VIOLENCE,
        "!error!",               "",           "",
      GN_STA
    }, 
    {
        "history of armaments",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        NULL,			SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
      GN_STA
    }, 
    {
        "arms maintenance",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_arms_main,			SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
      GN_STA
    }, 
    {
        "basic armor",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_basic_armor,		SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
      GN_STA
    }, 
    {
        "orealts 1st",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_orealts_1st,		SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
      GN_STA
    },
    /* CRUSADER MAJOR SELECTIONS */
    {
        "adv. handling",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_adv_handling,		SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
      GN_STA
    },
    {
        "2h mastery",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_2h_mastery,		SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
      GN_STA
    },
    {
        "orealts 2nd",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_orealts_2nd,		SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
	GN_STA
    },
    {
        "damage mastery B",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_dam_masteryB,		SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
	GN_STA
    },
    {
        "damage mastery P",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_dam_masteryP,		SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
	GN_STA
    },
    {
        "damage mastery S",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_dam_masteryS,		SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
	GN_STA
    },
    {
        "adv. armor",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_adv_armor,		SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
	GN_STA
    },
    {
        "armor enhancement",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_armor_enhance,	SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
	GN_STA
    },
    {
        "backcutter",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        NULL,		SPELL_OTHER,        25,     0,
        "backcutter",               "",           "",
	GN_STA
    },
    {
        "cutter",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        NULL,		SPELL_OTHER,        25,     2 * PULSE_VIOLENCE,
        "cutter",               "",           "",
	GN_STA
    },
    /* CRUSADER SPECIALTY SELECTIONS */
    {
        "control damage",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_con_damage,	SPELL_OTHER,        0,     PULSE_VIOLENCE,
        "!error!",               "",           "",
	GN_STA
    },
    {
        "cusinart",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_cusinart,		SPELL_OTHER,        50,     2 * PULSE_VIOLENCE,
        "!error!",               "",           "",
	GN_STA
    },
    {
        "psalm master",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_psalm_master,		SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
	GN_STA
    },
    {
        "witch compass",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        NULL,		SPELL_OTHER,        35,     PULSE_VIOLENCE,
        "slash",               "The witch compass has run out of blood.",           "",
	GN_STA
    },
    {
        "weaponcraft",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_weaponcraft,		SPELL_OTHER,        0,     24,
        "!error!",               "",           "",
	GN_STA
    },
    {
        "armorcraft",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_armorcraft,       SPELL_OTHER,       100,     2*PULSE_VIOLENCE,
        "!error!",               "",           "",
	GN_STA
    },
    /* CRUSADER SIGNATURE SELECTIONS */
    {
        "pommel smash",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        NULL,			SPELL_OTHER,        15,  2 * PULSE_VIOLENCE,
        "pommel smash",          "Your head stops spinning.",           "$n recovers from the pommel smash.",
	GN_STA
    },
    {
        "behead",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_behead,		SPELL_OTHER,        25, 2 * PULSE_VIOLENCE,
        "behead",               "Your neck feels vulnerable once again.",           "",
	GN_STA
    },
    {
      "armor pierce",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_FIGHTING,
      &gsn_apierce,		SPELL_OTHER,    20,     2 * PULSE_VIOLENCE,
      "armor pierce",               "You manage to mend your damaged armor.",         "",
	GN_STA
    },
    {
        "pious",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        NULL,		SPELL_OTHER,        35,     PULSE_VIOLENCE,
        "!error!",               "",           "",
	GN_STA
    },
    {
        "high sight",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,             TAR_IGNORE,             POS_STANDING,
        &gsn_high_sight,	SPELL_OTHER,        0,     PULSE_VIOLENCE,
        "!error!",               "",           "",
	GN_STA
    },
/* CHANT SKILL for psalms*/
    {
      "chant",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 4, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_chant,	      SPELL_OTHER,        100,    2 * PULSE_VIOLENCE,
      "chant wait",           "You may once again attempt to chant a psalm.",           "",
      GN_STA
    },
    /* PSALMS */
    {
      "righteousness",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_righteous,             TAR_IGNORE,             POS_STANDING,
      &gsn_righteous,		SPELL_OTHER,     0,     120,
      "!error!",               "The psalm of Righteousness leaves your mind.",          "",
      GN_STA
    },
    {
      "dvoid",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_dvoid,             TAR_IGNORE,             POS_STANDING,
      &gsn_dvoid,		SPELL_OTHER,     0,     120,
      "divine void",               "The psalm of Divine Void leaves your mind.",           "",
      GN_STA
    },
    {
      "insight",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_insight,             TAR_IGNORE,             POS_STANDING,
      &gsn_insight,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Insight leaves your mind.",           "",
      GN_STA
    },
    {
      "divine wrath",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_dwrath,             TAR_IGNORE,             POS_STANDING,
      &gsn_dwrath,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Divine Wrath leaves your mind.",           "",
      GN_STA
    },
    {
      "shadow bane",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_sbane,             TAR_IGNORE,             POS_STANDING,
      &gsn_sbane,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Shadow Bane leaves your mind.",           "",
      GN_STA
    },
    {
      "pure water",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_pwater,             TAR_IGNORE,             POS_STANDING,
      &gsn_pwater,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Pure Water leaves your mind.",           "",
      GN_STA
    },
    {
      "icefire",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_icefire,             TAR_IGNORE,             POS_STANDING,
      &gsn_icefire,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Icefire leaves your mind.",           "",
      GN_STA
    },
    {
      "searing might",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_smight,             TAR_IGNORE,             POS_STANDING,
      &gsn_smight,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Searing Might leaves your mind.",           "",
      GN_STA
    },
    {
      "divine might",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_dmight,             TAR_IGNORE,             POS_STANDING,
      &gsn_dmight,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Divine Might leaves your mind.",           "",
      GN_STA
    },
    {
      "vitality",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_vitality,             TAR_IGNORE,             POS_STANDING,
      &gsn_vitality,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Vitality leaves your mind.",           "",
      GN_STA
    },
    {
      "swift retribution",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_sretrib,             TAR_IGNORE,             POS_STANDING,
      &gsn_sretrib,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Swift Retribution leaves your mind.",           "",
      GN_STA
    },
    {
      "virtues",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_virtues,             TAR_IGNORE,             POS_STANDING,
      &gsn_virtues,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Virtues leaves your mind.",           "",
      GN_STA
    },
    {
      "preservation",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_preserve,             TAR_IGNORE,             POS_STANDING,
      &gsn_preserve,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Preservation leaves your mind.",           "",
      GN_STA
    },
    {
      "final parting",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_fpart,             TAR_IGNORE,             POS_STANDING,
      &gsn_fpart,		SPELL_OTHER,     0,     120,
      "final parting",        "The psalm of Final Parting leaves your mind.",           "",
      GN_STA
    },
    {
      "vicarious redemption",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_vredem,             TAR_IGNORE,             POS_STANDING,
      &gsn_vredem,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Vicarious Redemption leaves your mind.",           "",
      GN_STA
    },
    {
      "the Triumph of One God",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_ogtrium,             TAR_IGNORE,             POS_STANDING,
      &gsn_ogtrium,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of the One God's Triumph leaves your mind.",           "",
      GN_STA
    },
    {
      "the Triumph of Sirant",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_strium,             TAR_IGNORE,             POS_STANDING,
      &gsn_strium,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Sirant's Triumph leaves your mind.",           "",
      GN_STA
    },
    {
      "the Triumph of Palison",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_ptrium,             TAR_IGNORE,             POS_STANDING,
      &gsn_ptrium,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Palison's Triumph leaves your mind.",           "",
      GN_STA
    },
    {
      "undeniable order",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_uorder,             TAR_IGNORE,             POS_STANDING,
      &gsn_uorder,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Undeniable Order leaves your mind.",           "",
      GN_STA
    },
    {
      "inner strength",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_istrength,             TAR_IGNORE,             POS_STANDING,
      &gsn_istrength,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Inner Strength leaves your mind.",           "",
      GN_STA
    },
    {
      "purity",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_purity,             TAR_IGNORE,             POS_STANDING,
      &gsn_purity,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Purity leaves your mind.",           "",
      GN_STA
    },
    {
      "constitution",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_const,             TAR_IGNORE,             POS_STANDING,
      &gsn_const,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Constitution leaves your mind.",           "",
      GN_STA
    },
    {
      "the Eye of Palison",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_epal,             TAR_IGNORE,             POS_STANDING,
      &gsn_epal,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of the Eye of Palison leaves your mind.",           "",
      GN_STA
    },
    {
      "divine health",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_dheal,             TAR_IGNORE,             POS_STANDING,
      &gsn_dheal,		SPELL_OTHER,     0,     120,
      "!error!",        "The psalm of Divine Health leaves your mind.",           "",
      GN_STA
    },
    {
      "Baptism",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_baptize,             TAR_IGNORE,             POS_STANDING,
      &gsn_baptize,		SPELL_OTHER,     0,     120,
      "!error!",        "The effect of the Ritual of Baptism wear off.",           "",
      GN_STA
    },
    {
      "inner calm",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_icalm,             TAR_IGNORE,             POS_STANDING,
      &gsn_icalm,		SPELL_OTHER,     0,     120,
      "!error!",        "The effect of the Psalm of Inner Calm wear off.",           "",
      GN_STA
    },
    /*ninjitsu skills for ninjas and other classes*/
    {
      "pugil",
      {28, 52, 27, 52, 52, 52, 28, 52, 52, 52, 52, 52, 52, 52, 52, 52, 42, 52, 52, 52},
      { 1, 10, 1, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10,   2, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_pugil,	      SPELL_OTHER,        0, 0,
      "!pugil!",               "",           "",
      GN_STA
    },
    {
      "shadow arts",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_shadow_strike,      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "shadow strike",       "You recover from your injuries.",           "",
      GN_STA
    },
    {
      "lotus scourge",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_lotus_sc,		      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "!lotus scourge!",       "",           "",
      GN_STA
    },
    /* RANGER ARCHERY SKILLS */
    {
      "bowyer",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        75, 2 * PULSE_VIOLENCE,
      "!bowyer!",       "",           "",
      GN_STA
    },
    {
      "fletchery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        35, 2 * PULSE_VIOLENCE,
      "!fletchery!",       "",           "",
      GN_STA
    },
    {
      "archery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_archery,		      SPELL_OTHER,        5, 2 * PULSE_VIOLENCE,
      "!archery!",       "",           "",
      GN_STA
    },
    {
      "beastmaster",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        5, 2 * PULSE_VIOLENCE,
      "!beastmaster!",       "",           "",
      GN_STA
    },
    /* weapon masteries */
    {
      "exotic mastery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_exotic_mastery,      SPELL_OTHER,        0, 0,
      "!exotic mastery!",       "",           "",
      GN_STA
    },
    {
      "sword mastery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_sword_mastery,      SPELL_OTHER,        0, 0,
      "!sword mastery!",       "",           "",
      GN_STA
    },
    {
      "dagger mastery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_dagger_mastery,      SPELL_OTHER,        0, 0,
      "!dagger mastery!",       "",           "",
      GN_STA
    },
    {
      "spear mastery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_spear_mastery,      SPELL_OTHER,        0, 0,
      "!spear mastery!",       "",           "",
      GN_STA
    },
    {
      "mace mastery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_mace_mastery,      SPELL_OTHER,        0, 0,
      "!mace mastery!",       "",           "",
      GN_STA
    },
    {
      "axe mastery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_axe_mastery,      SPELL_OTHER,        0, 0,
      "!axe mastery!",       "",           "",
      GN_STA
    },
    {
      "flail mastery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_flail_mastery,      SPELL_OTHER,        0, 0,
      "!flail mastery!",       "",           "",
      GN_STA
    },
    {
      "whip mastery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_whip_mastery,      SPELL_OTHER,        0, 0,
      "!whip mastery!",       "",           "",
      GN_STA
    },
    {
      "staff mastery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_staff_mastery,      SPELL_OTHER,        0, 0,
      "!staff mastery!",       "",           "",
      GN_STA
    },
    {
      "polearm mastery",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_polearm_mastery,      SPELL_OTHER,        0, 0,
      "!polearm mastery!",       "",           "",
      GN_STA
    },
/* weapon expertise */
    {
      "exotic expert",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_exotic_expert,      SPELL_OTHER,        0, 0,
      "!exotic expert!",       "",           "",
      GN_STA
    },

    {
      "sword expert",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_sword_expert,      SPELL_OTHER,        0, 0,
      "!sword expert!",       "",           "",
      GN_STA
    },
    {
      "dagger expert",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_dagger_expert,      SPELL_OTHER,        0, 0,
      "!dagger expert!",       "",           "",
      GN_STA
    },
    {
      "spear expert",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_spear_expert,      SPELL_OTHER,        0, 0,
      "!spear expert!",       "",           "",
      GN_STA
    },
    {
      "mace expert",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_mace_expert,      SPELL_OTHER,        0, 0,
      "!mace expert!",       "",           "",
      GN_STA
    },
    {
      "axe expert",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_axe_expert,      SPELL_OTHER,        0, 0,
      "!axe expert!",       "",           "",
      GN_STA
    },
    {
      "flail expert",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_flail_expert,      SPELL_OTHER,        0, 0,
      "!flail expert!",       "",           "",
      GN_STA
    },
    {
      "whip expert",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_whip_expert,      SPELL_OTHER,        0, 0,
      "!whip expert!",       "",           "",
      GN_STA
    },
    {
      "polearm expert",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_polearm_expert,      SPELL_OTHER,        0, 0,
      "!polearm expert!",       "",           "",
      GN_STA
    },
    {
      "staff expert",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_staff_expert,      SPELL_OTHER,        0, 0,
      "!staff expert!",       "",           "",
      GN_STA
    },
    /* new thief skills */
    {
      "plant",
      {52, 52, 52, 52, 52, 36, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_plant,	      SPELL_OTHER,        0, 2 * PULSE_VIOLENCE,
      "!plant!",       "",           "",
      GN_STA
    },

    /* new thief skills */
    {
      "traps",
      {52, 52, 52, 52, 52, 40, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,	      SPELL_OTHER,        20,	1 * PULSE_VIOLENCE,
      "!traps!",       "You've managed to gather materials for another trap.",           "",
      GN_STA
    },
    {
      "defuse",
      {22, 22, 22, 22, 22,  2, 17, 22, 22, 22, 22, 22, 22, 22, 22, 22, 19, 22, 22, 22},
      { 10, 10, 10, 10, 10, 1,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10,  2, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,	      SPELL_OTHER,        20,	2 * PULSE_VIOLENCE,
      "!defuse!",       "",           "",
      GN_STA
    },
/* skills for trap select groups */
    {
      "poison needle",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        60, 2 * PULSE_VIOLENCE,
      "poison needle",       "",           "",
      GN_STA
    },
    {
      "amnesia charm",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,			SPELL_OTHER,      50, 2 * PULSE_VIOLENCE,
      "amnesi",       "",           "",
      GN_STA
    },
    {
      "snares",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        45, 2 * PULSE_VIOLENCE,
      "jagged edge",       "",           "",
      GN_STA
    },
    {
      "runes",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,       50, 2 * PULSE_VIOLENCE,
      "magic",       "",           "",
      GN_STA
    },
    {
      "eyedust",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        80, 2 * PULSE_VIOLENCE,
      "eyedust",       "",           "",
      GN_STA
    },
    {
      "antimagic",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        80, 2 * PULSE_VIOLENCE,
      "antimagic",       "",           "",
      GN_STA
    },
    {
      "enfeebler",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        80, 2 * PULSE_VIOLENCE,
      "enfeebler",       "",           "",
      GN_STA
    },
    {
      "blackspore",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,       80, 2 * PULSE_VIOLENCE,
      "blackspore",       "",           "",
      GN_STA
    },
    {
      "chestbuster",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        100, 3 * PULSE_VIOLENCE,
      "shrapnel",       "",           "",
      GN_STA
    },
    {
      "brambus needler",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        100, 3 * PULSE_VIOLENCE,
      "sliver salvo",       "",           "",
      GN_STA
    },
    {
      "devil wheel",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        100, 3 * PULSE_VIOLENCE,
      "poison needle",       "",           "",
      GN_STA
    },
    {
      "webcaster",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        100, 3 * PULSE_VIOLENCE,
      "web trap",       "",           "",
      GN_STA
    },
    {
      "fireseed",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        20, 1 * PULSE_VIOLENCE,
      "inferno",       "",           "",
      GN_STA
    },
    {
      "doublesheath",
      {52, 52, 52, 52, 52, 37, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  5},
      { 10, 10, 10, 10, 10, 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_sheath,	      SPELL_OTHER,        20, 1 * PULSE_VIOLENCE,
      "circle sidestep",       "",           "",
      GN_STA
    },
    {
      "magic web",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_webcaster,             TAR_CHAR_OFFENSIVE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        100, 3 * PULSE_VIOLENCE,
      "web trap",       "",           "",
      GN_STA
    },
/* warriors advanced skills */
    {
      "feign",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_feign,	      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "feign",       "",           "",
      GN_STA
    },

    {
      "power grip",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,	      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "power grip",       "",           "",
      GN_STA
    },
    {
      "mantis maul",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,	      SPELL_OTHER,        10, 2 * PULSE_VIOLENCE,
      "mantis maul",       "",           "",
      GN_STA
    },
    {
      "focused bash",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,	      SPELL_OTHER,        10, 2 * PULSE_VIOLENCE,
      "focused bash",       "",           "",
      GN_STA
    },
    {
      "weapon lock",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_wlock,	      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "weapon lock",       "You untangle the whip.",           "",
      GN_STA
    },
    {
      "piercing strike",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,	      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "piercing strike",       "",           "",
      GN_STA
    },
    {
      "shield expertise",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_shield_expert,      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "!shield expertise!",       "",           "",
      GN_STA
    },
    {
      "shield bash",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,	      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "shield bash",       "",           "",
      GN_STA
    },
    {
      "weapon seize",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,	      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "weapon seize",       "",           "",
      GN_STA
    },
    {
      "soul transfer",
      {52, 52, 52, 52, 45, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_soul_transfer,             TAR_OBJ_INV,             POS_STANDING,
      NULL,	      SPELL_OTHER,       450, 2 * PULSE_VIOLENCE,
      "soul transfer",       "You may once again attempt a soul transfer.",           "",
      GN_STA
    },
    {
      "path of anger",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_path_anger,	      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "!anger!",       "",           "",
      GN_STA
    },
    {
      "path of fury",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_path_fury,	      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "!fury!",       "",           "",
      GN_STA
    },
    {
     "path of devastation",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_path_dev,	      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "!devastation!",       "",           "",
      GN_STA
    },
    {
     "skull crusher",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,	      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "crush",       "",           "",
      GN_STA
    },
    {
     "blood haze",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_bloodhaze,	      SPELL_OTHER,        15, 2 * PULSE_VIOLENCE,
      "crush",       "",           "",
      GN_STA
    },
    {
     "spirit bind",
     {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 28, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10,10, 10, 10, 10, 10, 10, 10, 10, 10, 1, 10, 10, 10, 10, 10},
      spell_bind_spirit,             TAR_OBJ_ROOM,             POS_STANDING,
      &gsn_spirit_bind,	      SPELL_OTHER,        15, 1 * PULSE_VIOLENCE,
      "spirits",       "",           "",
      GN_STA
    },

    {
     "mounted",
     {52, 52, 52, 45, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
     {10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_OBJ_ROOM,             POS_STANDING,
      &gsn_mounted,	      SPELL_OTHER,        15, 1 * PULSE_VIOLENCE,
     "!mounted!",       "Your mount departs leaving you on foot.",           "",
      GN_STA
    },
    {
     "goliath",
     {52, 52, 52, 32, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
     {10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_stallion,             TAR_CHAR_SELF,             POS_STANDING,
      NULL,	      SPELL_OTHER,        55, 2 * PULSE_VIOLENCE,
      "!mounted!",       "",           "",
      GN_STA
    },
    {
/* CABAL */
        "bribe",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_bribe,                   SPELL_MALEDICTION,       0,     2 * PULSE_VIOLENCE,
        "!bribe!",                "You are no longer protected by your bribe.",   "",
	GN_STA
    },  
    {
/* CABAL */
        "throat cut",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,            SPELL_MALEDICTION,       25,     2 * PULSE_VIOLENCE,
        "throat cut",                "",   "",
	GN_STA
    },  
    {
/* CABAL */
        "asphyxiate",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_asphyxiate,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,            SPELL_MALEDICTION,       28,     1 * PULSE_VIOLENCE,
        "!asphxiation!",                "",   "",
	GN_STA
    },  
    {
/* CABAL */
        "shadowmaster",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_shadowmaster,            SPELL_MALEDICTION,       28,     1 * PULSE_VIOLENCE,
        "!shadowmaster!",                "",   "",
	GN_STA
    },  
    {
/* CABAL */
        "bloodmark",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mark_fear,      TAR_IGNORE,     POS_FIGHTING,
        &gsn_mark_fear,            SPELL_MALEDICTION,       28,     1 * PULSE_VIOLENCE,
        "!mark of fear!",                "You are no longer marked by Syndicate.",   "",
	GN_STA
    },  
/* CABAL */
    {
        "diplomacy",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_IGNORE,     POS_FIGHTING,
        NULL,            SPELL_MALEDICTION,       50,     2 * PULSE_VIOLENCE,
        "!diplomacy!",                "!diplomacy!",   "",
	GN_STA
    },  
/* CABAL */
    {
        "vengeance",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_vengeance,     TAR_CHAR_SELF,     POS_FIGHTING,
        NULL,            SPELL_OTHER,       50,     2 * PULSE_VIOLENCE,
        "vengeance",                "The Vengeance of Light no longer protects you.",   "The angry white aura around $n fades.",
	GN_BEN
    },  
/* CABAL */
    {
        "spawn blade",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_STANDING,
        NULL,            SPELL_OTHER,      35,     1 * PULSE_VIOLENCE,
        "vengeance",                "",   "",
	GN_STA
    },  

/* CABAL */
    {
      "conjuration",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_STANDING,
        NULL,            SPELL_OTHER,      125,     2 * PULSE_VIOLENCE,
        "!conjuration!",                "",   "",
	GN_STA
    },  
/* CABAL */
    {
      "maze",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_maze,     TAR_CHAR_OFFENSIVE,     POS_STANDING,
        NULL,            SPELL_OTHER,      20,     1 * PULSE_VIOLENCE,
        "maze",                "You can once more imprison someone within a maze.",   "",
	GN_STA
    },  
/* CABAL */
    {
      "nemesis",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,     TAR_IGNORE,     POS_STANDING,
        NULL,            SPELL_OTHER,      350,     3 * PULSE_VIOLENCE,
        "nemesis",                "",   "",
	GN_STA
    },  
/* CABAL */
    {
      "stand ground",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_stand_ground,     TAR_CHAR_SELF,     POS_STANDING,
        &gsn_stand_ground,            SPELL_OTHER,      80,     2 * PULSE_VIOLENCE,
        "",                "",   "",
	GN_NOCANCEL
    },  
/* CABAL */
    {
      "chrono shield",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_chrono_shield,     TAR_CHAR_SELF,     POS_STANDING,
      NULL,            SPELL_OTHER,      32,     1 * PULSE_VIOLENCE,
        "past",               "",   "",
	GN_BEN
    },  
/* CABAL */
    {
      "temporal storm",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_temporal_storm,     TAR_CHAR_SELF,     POS_FIGHTING,
      &gsn_temp_storm,            SPELL_OTHER,      55,     1 * PULSE_VIOLENCE,
      "temporal storm",               "The raging temporal storm subsides.",   "The wild temporal energies about $n subside.",
	GN_HAR
    },  
/* CABAL */
    {
      "faith ward",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_ward_faith,     TAR_CHAR_SELF,     POS_STANDING,
      NULL,            SPELL_OTHER,      120,     3 * PULSE_VIOLENCE,
      "",               "",   "",
      GN_STA
    },  
/* CABAL */
    {
      "vocalize",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_vocalize,     TAR_CHAR_SELF,     POS_STANDING,
      NULL,            SPELL_OTHER,      120,     3 * PULSE_VIOLENCE,
      "",               "",   "",
      GN_STA
    },  
/* CABAL */
    {
      "curse ward",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_ward_curse,     TAR_CHAR_SELF,     POS_STANDING,
      NULL,            SPELL_OTHER,      120,     3 * PULSE_VIOLENCE,
      "",               "",   "",
      GN_STA
    },  
/* CABAL */
    {
      "paradox",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_paradox,   TAR_CHAR_SELF,     POS_STANDING,
      NULL,            SPELL_OTHER,      72,     2 * PULSE_VIOLENCE,
      "",               "",   "",
      GN_STA
    },  
/* CABAL */
    {
      "age",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_age,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
      NULL,            SPELL_OTHER,      33,     2 * PULSE_VIOLENCE,
      "aging",               "You feel much younger and nimbler.",   "$n looks much younger.",
      GN_HAR
    },  
/* CABAL */
    {
      "temporal avenger",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_temporal_avenger,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
      NULL,            SPELL_OTHER,     16,     PULSE_VIOLENCE,
      "attacks",               "You are once again capable of creating a temporal avenger.",   "",
      GN_BEN
    },  
/* CABAL */
    {
      "town gate",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
     spell_town_gate,   TAR_IGNORE,     POS_STANDING,
      NULL,            SPELL_OTHER,     85, PULSE_VIOLENCE * 3,
      "!town gate!",               "",   "",
      GN_STA
    },  

/* CABAL */
    {
      "contingency",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,   TAR_IGNORE,     POS_STANDING,
      &gsn_cont,            SPELL_OTHER,     105, PULSE_VIOLENCE * 2,
      "!town gate!",             "Your Contingency has expired.",   "",
      GN_STA
    },  
/* CABAL */
    {
      "ripple",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_ripple,    TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
      &gsn_ripple,           SPELL_OTHER,     15, PULSE_VIOLENCE,
      "time blast",             "The strange ripples in your vision subside.",   "The ripples around $n subside.",
      GN_HAR
    },  
/* CABAL */
    {
      "raise morale",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_raise_morale,    TAR_CHAR_SELF,     POS_FIGHTING,
      &gsn_raise_morale,           SPELL_OTHER,     35, PULSE_VIOLENCE * 2,
      "!raise morale!",             "You are no longer raising morale of your followers.",   "",
      GN_BEN
    },  
/* CABAL */
    {
      "close combat",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,    TAR_CHAR_SELF,     POS_FIGHTING,
      NULL,           SPELL_OTHER,     1, 0,
      "stunning blow",             "",   "",
      GN_STA
    },  
    /* CABAL */
    {
      "trample",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,    TAR_CHAR_SELF,     POS_FIGHTING,
      &gsn_trample,           SPELL_OTHER,     0, 0,
      "trample",             "",   "",
      GN_STA
    },  
    /* CABAL */
    {
      "cyclone",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,    TAR_CHAR_SELF,     POS_FIGHTING,
      &gsn_cyclone,           SPELL_OTHER,     12,  2 * PULSE_VIOLENCE,
      "cyclone",             "",   "",
      GN_STA
    },  
    {
      /* CABAL */
      "maelstrom",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,    TAR_CHAR_SELF,     POS_FIGHTING,
      &gsn_maelstrom,           SPELL_OTHER,     15, 2 * PULSE_VIOLENCE,
      "maelstrom",             "",   "",
      GN_STA
    },  
    /* CABAL */
    {
      "adrenaline rush",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,		TAR_CHAR_SELF,     POS_FIGHTING,
      &gsn_adrenaline_rush,          SPELL_OTHER,    0, 0,
      "!adrenaline!",             "The after effects of adrenaline rush subside.",   "",
      GN_STA
    },  
    /* CABAL */
    {
      "recharge",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_recharge,		TAR_OBJ_INV,     POS_STANDING,
      NULL,          SPELL_OTHER,    100, 2 * PULSE_VIOLENCE,
      "!adrenaline!",             "",   "",
      GN_STA
    },  
    /* CABAL */
    {
      "overload",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_overload,		TAR_OBJ_INV,     POS_STANDING,
      NULL,          SPELL_OTHER,    100, 2 * PULSE_VIOLENCE,
      "!adrenaline!",             "",   "",
      GN_STA
    },  

    {
      "death kiss",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,		TAR_OBJ_INV,     POS_STANDING,
      &gsn_death_kiss,          SPELL_OTHER,    100, 2 * PULSE_VIOLENCE,
      "!adrenaline!",             "",   "",
      GN_STA
    },  
    {
      "falcon eye",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,		TAR_OBJ_INV,     POS_STANDING,
      NULL,          SPELL_OTHER,    100, 2 * PULSE_VIOLENCE,
      "!adrenaline!",             "",   "",
      GN_STA
    },
    {
      "defusion",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_defusion,		TAR_IGNORE,     POS_STANDING,
      NULL,          SPELL_OTHER,    0, PULSE_VIOLENCE,
      "!adrenaline!",             "",   "",
      GN_STA
    },
    {
      "omnipotence",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_omnipotence,		TAR_CHAR_SELF,     POS_STANDING,
      &gsn_omnipotence,          SPELL_OTHER,    0, PULSE_VIOLENCE,
      "!adrenaline!",             "",   "",
      GN_STA
    },
    /* CABAL */
    {
      "act",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},

      spell_null,		TAR_CHAR_SELF,     POS_STANDING,
      NULL,          SPELL_OTHER,    0, 0,
      "!adrenaline!",             "",   "",
      GN_STA
    },
    /* CABAL */
    {
      "scribe",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,		TAR_CHAR_SELF,     POS_STANDING,
      NULL,          SPELL_OTHER,    0, 0,
      "!adrenaline!",             "",   "",
      GN_STA
    },
    {
      "no-learn",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,		TAR_CHAR_SELF,     POS_STANDING,
      &gsn_nolearn,          SPELL_OTHER,    0, 0,
      "!adrenaline!",             "",   "",
      GN_STA
    },

    {
        "metabolic brake",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 12, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_metabolic_brake,      TAR_CHAR_SELF,     POS_STANDING,
        NULL,              SPELL_MENTAL,       25,     18,
        "",                "Your metabolism speeds up.",        "$n begins to move faster.",
	GN_BEN | GN_PSI
    },    

    {
        "metabolic boost",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 33, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_metabolic_boost,      TAR_CHAR_SELF,     POS_STANDING,
        NULL,              SPELL_MENTAL,       35,     18,
        "",                "Your metabolism slows down.",        "$n seems to slow down.",
	GN_BEN | GN_PSI
    },    

    {
        "remove magic",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  16, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_remove_magic,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,      SPELL_MENTAL,       26,    1 * PULSE_VIOLENCE,
        "",              "",    "",
	GN_HAR
    },

    {
        "psi amp",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 38, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_null,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_psi_amp,       SPELL_OTHER,       50,     18,
        "",                "Your psionic amplification wears off.",        "",
	GN_BEN | GN_PSI
    },    

    {
        "subvocalize",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 42, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_subvocalize,      TAR_CHAR_SELF,     POS_STANDING,
        &gsn_subvocal,          SPELL_OTHER,      38,     2 * PULSE_VIOLENCE,
        "",                "You stop subvocalizing your spells.",        "",
	GN_BEN | GN_PSI
    },    

    {
        "wish",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 30, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10},
        spell_wish,      TAR_CHAR_SELF,     POS_STANDING,
        NULL,              SPELL_MENTAL,      90,     2 * PULSE_VIOLENCE,
        "wish",          "You can once again make a wish.",        "",
	GN_STA
    },    
    /* PSI KINESIS AND DESTRUCTION SPELLS */
    {
        "mindsurge",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mindsurge,   TAR_CHAR_SELF,     POS_STANDING,
        &gsn_mindsurge,               SPELL_OTHER,       60,     2 * PULSE_VIOLENCE,
        "",       "Your mindsurge has come to an end.",        "",
	GN_BEN | GN_PSI
    },    

    {
        "pyro kinesis",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_pyro_kinesis,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                 SPELL_AFFLICTIVE,       20,     1 * PULSE_VIOLENCE,
        "pyrokinetic blast",       "",        "",
	GN_HAR | GN_PSI_WEE
    },    

    {
        "cryo kinesis",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_cryo_kinesis,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                 SPELL_AFFLICTIVE,       20,     1 * PULSE_VIOLENCE,
        "cryokinetic blast",       "",        "",
	GN_HAR | GN_PSI_WEE
    },    

    {
        "hydro kinesis",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_hydro_kinesis,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                 SPELL_AFFLICTIVE,       20,     1 * PULSE_VIOLENCE,
        "hydrokinetic blast",       "",        "",
	GN_HAR | GN_PSI_WEE
    },    

    {
        "electro kinesis",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_electro_kinesis,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                 SPELL_AFFLICTIVE,       20,     1 * PULSE_VIOLENCE,
        "electrokinetic blast",       "",        "",
	GN_HAR | GN_PSI_WEE
    },    

    {
        "tele kinesis",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_tele_kinesis,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                 SPELL_AFFLICTIVE,       20,     1 * PULSE_VIOLENCE,
        "telekinetic blast",       "",        "",
	GN_HAR | GN_PSI_WEE
    },    

    {
        "photon blast",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_photon_blast,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                 SPELL_AFFLICTIVE,       42,     1 * PULSE_VIOLENCE,
        "photon blast",       "",        "",
	GN_HAR | GN_PSI_WEE
    },    

    {
        "singularity",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_singularity,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                SPELL_OTHER,       55,     2 * PULSE_VIOLENCE,
        "singularity",       "",        "",
	GN_HAR | GN_PSI_STR
    },    

    {
        "entropic touch",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_entropic_touch,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                  SPELL_OTHER,       48,  1 * PULSE_VIOLENCE,
        "entropic touch",       "Your armors return to normal.",        "$n's armors returns to normal.",
	GN_HAR  | GN_PSI_STR
    },    

    {
        "amorphous infection",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_amorphous_infection,  TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,                       SPELL_OTHER,       60,  1 * PULSE_VIOLENCE,
        "amorphous parasite",       "",        "",
	GN_STA | GN_PSI_WEE
    },    

    {
        "disintegrate",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_disintegrate,  TAR_CHAR_OFFENSIVE,     POS_STANDING,
        NULL,                SPELL_OTHER,     100,  2 * PULSE_VIOLENCE,
        "disintegration",       "Your molecules regain their old position.",        "$n looks much healthier.",
	GN_STA
    },    

    {
        "breach",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_breach,               TAR_CHAR_OFFENSIVE,     POS_STANDING,
       &gsn_breach,                  SPELL_OTHER,     100,  2 * PULSE_VIOLENCE,
        "breach",       "",        "",
	GN_HAR
    },    

    /* nightmares */
    {
        "omen",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_omen,               TAR_IGNORE,     POS_STANDING,
        NULL,                  SPELL_OTHER,      50,  2 * PULSE_VIOLENCE,
        "",       "",        "",
	GN_STA | GN_NOCAST
    },    
    {
        "deathmare",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_deathmare,               TAR_IGNORE,     POS_STANDING,
        NULL,                  SPELL_OTHER,      50,  2 * PULSE_VIOLENCE,
        "",       "",        "",
	GN_STA | GN_NOCAST
    },    
    {
        "dreamprobe",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_dreamprobe,               TAR_IGNORE,     POS_STANDING,
        &gsn_dreamprobe,                  SPELL_OTHER,      50,  2 * PULSE_VIOLENCE,
        "",       "Your dreamlink has been severed.",        "",
	GN_STA | GN_NOCAST
    },    
    {
        "mindmelt",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mindmelt,               TAR_IGNORE,     POS_STANDING,
        NULL,                  SPELL_OTHER,      50,  2 * PULSE_VIOLENCE,
        "",       "",        "",
	GN_STA | GN_NOCAST
    },    

    {
        "telepathy",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,              TAR_IGNORE,     	POS_STANDING,
        &gsn_telepathy,           SPELL_OTHER,         0,    0,
        "",                      "",           "",
	GN_STA
    },
    {
        "domination",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,              TAR_IGNORE,     	POS_STANDING,
        NULL,           SPELL_OTHER,         0,    0,
        "",                      "",           "",
	GN_STA
    },

    {
        "spell vise",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_spell_vise,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        NULL,			   SPELL_MENTAL,       38,    2 * PULSE_VIOLENCE,
        "spell vise",              "The pressure around your temples eases off.",    "The blue ring around $n's mind fades away.",
	GN_HAR | GN_PSI_MED
    },

    {
        "mana trap",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mana_trap,     TAR_CHAR_SELF,     POS_FIGHTING,
        &gsn_mana_trap,			   SPELL_PROTECTIVE,       10,    2 * PULSE_VIOLENCE,
        "mana trap",              "Your mana trap has deactivated.", "",
	GN_BEN | GN_PSI
    },

    {
        "spell trap",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_spell_trap,     TAR_CHAR_SELF,     POS_FIGHTING,
        &gsn_spell_trap,			   SPELL_PROTECTIVE,       80,    2 * PULSE_VIOLENCE,
        "mana trap",              "Your spell trap has deactivated.", "",
	GN_BEN | GN_PSI
    },

    {
        "spell blast",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_spell_blast,     TAR_IGNORE,     POS_FIGHTING,
        &gsn_spell_blast,      SPELL_PROTECTIVE,       0,    2 * PULSE_VIOLENCE,
        "spell blast",          "Your mana supply for spell blast has dissipated.", "$n's spell blast has stopped functioning.",
	GN_BEN | GN_PSI_WEE | GN_PSI
    },

    {
        "soul pump",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_soul_pump,     TAR_CHAR_OFFENSIVE,     POS_STANDING,
        NULL,		     SPELL_MENTAL,       115,    2 * PULSE_VIOLENCE,
        "soul pump",        "You recover from the effects of soul pump.", "",
	GN_HAR | GN_PSI_STR
    },

    {
        "masochism",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_masochism,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_masochism,	     SPELL_MENTAL,       65,    2 * PULSE_VIOLENCE,
        "masochistic control",        "", "",
	GN_HAR | GN_PSI_STR
    },

    {
        "total recall",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_total_recall,     TAR_CHAR_SELF,     POS_FIGHTING,
        NULL,			   SPELL_PROTECTIVE,      180,    2 * PULSE_VIOLENCE,
        "", "", "",
	GN_BEN | GN_PSI
    },

    {
        "brain death",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_brain_death,   TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
        &gsn_brain_death,	     SPELL_MENTAL,       150,    2 * PULSE_VIOLENCE,
        "brain death",        "You regain your normal faculties.", "",
	GN_HAR
    },

    {
        "unminding",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_unminding,   TAR_CHAR_OFFENSIVE,     POS_STANDING,
        &gsn_unminding,		   SPELL_MENTAL,       150,    2 * PULSE_VIOLENCE,
        "unminding",        "You regain your normal faculties.", "",
	GN_HAR
    },

    {
        "illusion",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,              TAR_IGNORE,     	POS_STANDING,
        NULL,           SPELL_OTHER,         0,    0,
        "",                      "",           "",
	GN_STA
    },

    {
        "alteration",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,              TAR_IGNORE,     	POS_STANDING,
        NULL,           SPELL_OTHER,         0,    0,
        "",                      "",           "",
	GN_STA
    },

    {
        "false weapon",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_false_weapon,     TAR_IGNORE,     POS_STANDING,
	&gsn_false_weapon,	SPELL_PROTECTIVE,       20,    PULSE_VIOLENCE,
        "!false weapon!",       "The illusions around your weapon collapse.", "",
	GN_BEN | GN_PSI
    },

    {
        "numbness",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_numbness,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_numbness,	SPELL_MENTAL,       25,    PULSE_VIOLENCE,
        "!numbness!",   "Your nerves return to normal.", "",
	GN_HAR | GN_PSI_STR
    },

    {
        "shadow door",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_shadow_door,     TAR_CHAR_SELF,     POS_STANDING,
	&gsn_shadow_door, SPELL_PROTECTIVE,       32,    2 * PULSE_VIOLENCE,
        "!shadow door!",   "Your shadow door spell has collapsed.", "",
	GN_BEN | GN_PSI
    },

    {
        "nerve amp",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_nerve_amp,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_nerve_amp,      SPELL_MENTAL,       25,    PULSE_VIOLENCE,
        "!nerve amp!",   "Your nerves return to normal.", "",
	GN_HAR | GN_PSI_STR
    },

    {
        "mirror cloak",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mirror_cloak,     TAR_CHAR_DEFENSIVE,     POS_STANDING,
	&gsn_mirror_cloak,	SPELL_PROTECTIVE,       38,    2 * PULSE_VIOLENCE,
        "!mirror cloak!",   "Your mirror cloak shatters.", "$n's cloak of mirrors shatters.",
	GN_BEN | GN_PSI
    },

    {
        "mirage",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_mirage,     TAR_CHAR_SELF,     POS_STANDING,
	&gsn_mirage,	SPELL_PROTECTIVE,       40,    2 * PULSE_VIOLENCE,
        "!mirage!",   "Your mirage spell has collapsed.", "$n's mirage spell collapses.",
	GN_BEN | GN_PSI_WEE | GN_PSI
    },

    {
        "blink",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_blink,     TAR_CHAR_SELF,     POS_STANDING,
	&gsn_blink,	SPELL_PROTECTIVE,       33,    2 * PULSE_VIOLENCE,
        "!blink!",   "You stop blinking out into other planes.", "$n looks more solid.",
	GN_BEN | GN_PSI
    },

    {
        "spook",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_phantasm,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	NULL,		SPELL_MENTAL,       52,    PULSE_VIOLENCE,
        "!blink!",   "You've managed to disbelieve the phantasm.", "$n has managed to disbelieve the phantasm.",
	GN_STA | GN_PSI_MED
    },

    {
        "petrify",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_petrify,     TAR_CHAR_OFFENSIVE,     POS_STANDING,
	NULL,		SPELL_OTHER,       200,    2 * PULSE_VIOLENCE,
        "!petrify!",   "Your flesh returns to normal.", "$n's flesh loses the stoney look.",
	GN_HAR
    },

    {
        "time compression",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_time_compression,     TAR_CHAR_SELF,     POS_FIGHTING,
	&gsn_time_comp,		SPELL_OTHER,       100,    2 * PULSE_VIOLENCE,
        "!time compression!",   "You are no longer compressing time.", "$n stops leaving an after image.",
	GN_BEN | GN_PSI
    },

    {
        "planar rift",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_planar_rift,     TAR_CHAR_SELF,     POS_STANDING,
	NULL,		SPELL_OTHER,       115,    2 * PULSE_VIOLENCE,
        "planar rift",   "You can fashion a new rift blade.", "",
	GN_STA
    },

    {
        "molecular leash",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_molecular_leash,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_leash,		SPELL_MENTAL,     75,    PULSE_VIOLENCE,
        "molecular leash",   "You break the molecular leash apart.", "$n breaks through the molecular leash.",
	GN_STA | GN_PSI_MED
    },
    /* WATCHER */
    /* CABAL */
    {
        "webbing",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_webbing,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
	&gsn_webbing,	   SPELL_MALEDICTIVE,     15,    PULSE_VIOLENCE,
        "webbing",   "You manage to break the sticky webs.", "$n breaks through the sticky webs.",
	GN_HAR
    },

    /* CABAL */
    {
        "awaken life",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_awaken_life, TAR_IGNORE,     POS_STANDING,
	NULL,		   SPELL_OTHER,    250,    3 * PULSE_VIOLENCE,
        "!awaken life!",   "You can once again awaken forces of nature.", "",
	GN_STA
    },
    /* CABAL */
    {
        "greenheart",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_greenheart,  TAR_CHAR_SELF,     POS_STANDING,
	&gsn_greenheart,		   SPELL_OTHER,    33,    2 * PULSE_VIOLENCE,
        "!awaken life!",   "Your skin loses its green tint.", "$n's skin loses its green tint.",
	GN_BEN
    },
    /* CABAL */
    {
        "planar seal",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_planar_seal,  TAR_CHAR_OFFENSIVE,  POS_STANDING,
	&gsn_planar_seal,     SPELL_MALEDICTIVE,    65,    2 * PULSE_VIOLENCE,
        "!awaken life!",   "You can once again cast Planar Seal.", "",
	GN_STA
    },
    /* CABAL */
    {
        "dragon blood",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_dragon_blood,  TAR_CHAR_SELF,     POS_STANDING,
	NULL,		   SPELL_PROTECTIVE,    28,  2 * PULSE_VIOLENCE,
        "!awaken life!",   "You shed the scales covering your flesh.", "$n sheds the scales covering $s flesh.",
	GN_BEN
    },
    /* CABAL */
    {
        "whirlwind",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_whirlwind,  TAR_CHAR_OFFENSIVE,     POS_STANDING,
	NULL,		   SPELL_AFFLICTIVE,    12,  1 * PULSE_VIOLENCE,
        "whirlwind",   "", "",
	GN_HAR
    },

    {
        "boulder throw",             
	{42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42},
	{10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
	spell_null,             TAR_IGNORE,             POS_STANDING,
	NULL,			SPELL_OTHER,       12,      2 * PULSE_VIOLENCE,
	"boulder throw","You regain your sense of direction.", "$n's eyes lose their dazed, confused look.",
	GN_STA
    },
    /* CABAL */
    {
        "leadership",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,  TAR_IGNORE,     POS_STANDING,
	&gsn_leadership,		   SPELL_OTHER,    0,  0,
        "",   "", "",
	GN_STA
    },
    {
        "steel wall",   
        {52, 52, 52, 52, 52, 52, 52, 22, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_steel_wall,  TAR_IGNORE,     POS_STANDING,
	&gsn_steel_wall,		   SPELL_PROTECTIVE,   28,  2 * PULSE_VIOLENCE,
        "steel shards",   "The shards of metal protecting you fall to the ground.", "The whirling steel around $n falls to the ground.",
	GN_BEN
    },
    /* new ranger skilss */
    {
      "tracker",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_tracker,	      SPELL_OTHER,        55, 2 * PULSE_VIOLENCE,
      "!tracker!",       "You stop looking for tracks.",           "",
      GN_INTERRUPT
    },
    {
      "horn of nature",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        0, 0,
      "!horn of nature!",       "",           "",
      GN_STA
    },
    {
      "forestwalk",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_forest_walk,	      SPELL_OTHER,        0, 0,
      "!forest walk!",       "",           "",
      GN_STA
    },
    {
      "pack call",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        25, 2 * PULSE_VIOLENCE,
      "!tracker!",       "You can once more call on your pack.",           "",
      GN_STA
    },
    {
      "group herb",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        0, 0,
      "!group herb!",       "",           "",
      GN_STA
    },
    {
      "concealment",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        0, 0,
      "!tracker!",       "",           "",
      GN_STA
    },
    {
      "marksman",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_marksman,	      SPELL_OTHER,        0, 0,
      "marksman",       "",           "",
      GN_STA
    },
    {
      "horde",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      NULL,		      SPELL_OTHER,        5, 2 * PULSE_VIOLENCE,
      "!horde!",       "",           "",
      GN_STA
    },
    {
      "rapid fire",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_IGNORE,             POS_STANDING,
      &gsn_rapid_fire,	      SPELL_OTHER,        28, 2 * PULSE_VIOLENCE,
      "!rapid fire!",       "You return to firing at normal speed.",           "",
      GN_INTERRUPT
    },

    {
      "missile shield",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      spell_null,             TAR_CHAR_DEFENSIVE,      POS_STANDING,
      &gsn_missile_shield,      SPELL_PROTECTIVE,        35, 1 * PULSE_VIOLENCE,
      "!rapid fire!",       "Your missile shield collapses.",           "$n's missile shield has collapsed.",
      GN_BEN
    },
    /* CABAL */
    {
        "regenerate",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_regenerate,  TAR_CHAR_DEFENSIVE,     POS_STANDING,
	NULL,		   SPELL_OTHER,    100,  2 * PULSE_VIOLENCE,
        "!regenerate!",   "You can once again regenerate your wounds.", "",
	GN_STA
    },
    /* CABAL */
    {
        "forestmeld",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_forestmeld,  TAR_CHAR_DEFENSIVE,     POS_STANDING,
	&gsn_forestmeld,		   SPELL_OTHER,    15,  2 * PULSE_VIOLENCE,
        "whirlwind",   "You are no longer blending with the flora.", "$n's skin loses its green glow.",
	GN_BEN
    },
    /* CABAL */
    {
        "eye of nature",   
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_eye_of_nature,  TAR_IGNORE,     POS_STANDING,
	NULL,		   SPELL_OTHER,    5,  PULSE_VIOLENCE / 3,
        "whirlwind",   "You can once again focus the eye of nature on an area.", "",
	GN_STA
    },

    /* BLADEMASTER skill */
    {
        "onslaught",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  4},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	&gsn_onslaught,		SPELL_OTHER,		3,	PULSE_VIOLENCE,
        "thrust",   "", "",
	GN_STA
    },
    {
        "chop",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 12},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	NULL,			SPELL_OTHER,		5,	PULSE_VIOLENCE,
        "chop",   "", "",
	GN_STA
    },
    {
        "bladerush",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 25},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	NULL,			SPELL_OTHER,		5,	PULSE_VIOLENCE,
        "bladerush",   "", "",
	GN_STA
    },
    {
        "backpin",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 39},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	NULL,			SPELL_OTHER,		5,	PULSE_VIOLENCE,
        "backpin",   "", "",
	GN_STA
    },
    {
        "pinwheel",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 44},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	NULL,			SPELL_OTHER,		5,	PULSE_VIOLENCE,
        "pinwheel",   "", "",
	GN_STA
    },

    {
        "critical strike",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  5},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	NULL,			SPELL_OTHER,		10,	2 * PULSE_VIOLENCE,
        "critical strike",   "You recover from your injuries.", "",
	GN_STA
    },

    {
        "bladework",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 22},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  4},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	&gsn_bladework,			SPELL_OTHER,		5,	2 * PULSE_VIOLENCE,
        "",   "", "",
	GN_STA
    },

    {
        "footwork",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,  6},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  5},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	&gsn_footwork,			SPELL_OTHER,		5,	2 * PULSE_VIOLENCE,
        "",   "", "",
	GN_STA
    },

    {
        "battle stance",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 32},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  6},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	&gsn_battlestance,	SPELL_OTHER,		5,	2 * PULSE_VIOLENCE,
        "",   "", "",
	GN_STA
    },

    {
        "avatar of steel",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 46},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  7},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	&gsn_avatar_of_steel,	SPELL_OTHER,		5,	2 * PULSE_VIOLENCE,
        "",   "", "",
	GN_STA
    },

    {
        "powerstrike",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 22},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  6},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	&gsn_powerstrike,	SPELL_OTHER,		5,	2 * PULSE_VIOLENCE,
        "",   "", "",
	GN_STA
    },

    {
        "deathweaver",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 24},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_deathweaver,	TAR_CHAR_SELF,			POS_STANDING,
	&gsn_deathweaver,	SPELL_OTHER,		25,	2 * PULSE_VIOLENCE,
        "",   "Your focus wavers and you step out of the deathweaver stance.", "$n steps out of $s stance.",
	GN_BEN
    },

    {
        "bladestorm",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_bladestorm,	TAR_CHAR_SELF,			POS_STANDING,
	&gsn_bladestorm,	SPELL_OTHER,				25,	2 * PULSE_VIOLENCE,
        "",   "Your focus wavers and you step out of the bladestorm stance.", "$n steps out of $s stance.",
	GN_BEN
    },

    {
        "ironarm",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_ironarm,	TAR_CHAR_SELF,			POS_STANDING,
	&gsn_ironarm,		SPELL_OTHER,			25,	2 * PULSE_VIOLENCE,
        "ironarm",   "Your focus wavers and you step out of the ironarm stance.", "$n steps out of $s stance.",
	GN_BEN
    },

    {
        "Kyousanken",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_iron_curtain,	TAR_CHAR_SELF,			POS_STANDING,
	&gsn_iron_curtain,	SPELL_OTHER,		25,	2 * PULSE_VIOLENCE,
        "",   "Your focus wavers and you step out of the kyousanken stance.", "$n steps out of $s stance.",
	GN_BEN
    },

    {
        "shadowdancer",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_shadowdancer,	TAR_CHAR_SELF,			POS_STANDING,
	&gsn_shadowdancer,	SPELL_OTHER,		25,	2 * PULSE_VIOLENCE,
        "",   "Your focus wavers and you step out of the shadowdancer stance.", "$n steps out of $s stance.",
	GN_BEN
    },

    {
        "doomsinger",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_doomsinger,	TAR_CHAR_SELF,			POS_STANDING,
	&gsn_doomsinger,	SPELL_OTHER,		25,	2 * PULSE_VIOLENCE,
        "",   "Your focus wavers and you step out of the doomsinger stance.", "$n steps out of $s stance.",
	GN_BEN
    },

    {
        "kairishi",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_puppet_master,	TAR_IGNORE,			POS_STANDING,
	&gsn_puppet_master,	SPELL_OTHER,		25,	2 * PULSE_VIOLENCE,
        "",   "Your focus wavers and you step out of the kairishi stance.", "$n steps out of $s stance.",
	GN_BEN
    },

    {
        "vigil",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 27},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	NULL,			SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "suprise counter",   "", "",
	GN_STA
    },

    {
        "twin counter",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 28},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_FIGHTING,
	NULL,			SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "second counter",   "", "",
	GN_STA
    },

    {
        "predict",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 29},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_predict,		SPELL_OTHER,	       15,	2 * PULSE_VIOLENCE,
        "counter",   "You stop looking for the attack you predicted.", "$n stops analyzing your every move.",
	GN_INTERRUPT 
    },

    {
        "snakespeed",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 41},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	NULL,			SPELL_OTHER,	       0,	PULSE_VIOLENCE,
        "snakestrike",   "", "",
	GN_STA
    },

    {
        "spellkiller",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 48},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_spellkiller,	SPELL_OTHER,	     35,	2 * PULSE_VIOLENCE,
        "mana flare",   "You stop looking for the spell you predicted.", "$n stops analyzing your every move.",
	GN_INTERRUPT 
    },

    {
        "deathstrike",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 42},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	NULL,			SPELL_OTHER,	     0,		PULSE_VIOLENCE,
        "deathstrike",		"", "",
	GN_STA 
    },

    {
        "mob expert",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_mob_expert,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "human expert",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_human_expert,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "elf expert",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_elf_expert,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "dwarf expert",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_dwarf_expert,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "demihuman expert",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_demihuman_expert,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "giant expert",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_giant_expert,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "beast expert",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_beast_expert,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "flying expert",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_flying_expert,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "unique expert",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_unique_expert,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "mob master",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_mob_master,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "human master",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_human_master,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "dwarf master",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_dwarf_master,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "elf master",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_elf_master,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "beast master",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_beast_master,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "giant master",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_giant_master,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "demihuman master",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_demihuman_master,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "flying master",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_flying_master,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "unique master",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_IGNORE,			POS_STANDING,
	&gsn_unique_master,	SPELL_OTHER,		0,	PULSE_VIOLENCE,
        "",   "","",
	GN_STA 
    },

    {
        "blood vow",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 21},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_blood_vow,		TAR_OBJ_INV,			POS_STANDING,
	&gsn_bloodvow,			SPELL_OTHER,	       150,	10 * PULSE_VIOLENCE,
        "ritual",   "Your blood vow has run its course.","",
	GN_STA 
    },

    {
        "battlesphere",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 27},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_battlesphere,		TAR_CHAR_DEFENSIVE,			POS_STANDING,
	&gsn_battlesphere,		SPELL_OTHER,	       15,	2 * PULSE_VIOLENCE,
        "",				"You lose your battlesphere.", "$n's movements seem less accurate.",
	GN_INTERRUPT 
    },

    {
        "battlefocus",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 31},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_battlefocus,		TAR_CHAR_DEFENSIVE,			POS_STANDING,
	&gsn_battlefocus,		SPELL_OTHER,	       20,	2 * PULSE_VIOLENCE,
        "",				"You lose your battlefocus.", "$n seems less focused.",
	GN_INTERRUPT 
    },

    {
        "battletrance",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 37},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1},
        spell_battletrance,		TAR_CHAR_DEFENSIVE,			POS_STANDING,
	&gsn_battletrance,		SPELL_OTHER,	       25,	2 * PULSE_VIOLENCE,
        "",				"You snap out of your battletrance.", "$n seems less aware.",
	GN_INTERRUPT 
    },

    {
        "mercy wait",   
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_null,		TAR_CHAR_DEFENSIVE,			POS_STANDING,
	&gsn_mercy_wait,	SPELL_OTHER,	       25,	2 * PULSE_VIOLENCE,
        "",			"You can once again be given mercy in battle.", "", 
	GN_STA
    },

    {
        "soul tap",
        {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 50, 52, 52, 52, 52, 52, 52, 52, 52},
        {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  1, 10, 10, 10, 10, 10, 10, 10, 10},
        spell_soul_tap,      TAR_CHAR_SELF,     POS_FIGHTING,
        &gsn_soul_tap,       SPELL_OTHER,        180,     2 * PULSE_VIOLENCE,
        "soul tap",     "You've used up all the energy from the tapped soul.",     "",
	GN_STA
    },

/* SKILLS END HERE */
};


const   struct  group_type      group_table     [MAX_GROUP]     =
{
    {
        "rom basics", {0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ "recall", NULL, NULL, NULL, NULL }
    },
    {
        "warrior basics",     {0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        { "" }
    },
    {
        "berserker basics",   {-1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        { "" }
    },
    {
        "ranger basics", {-1,-1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0},
        { "" }
    },
    {
        "paladin basics",     {-1,-1,-1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1},
        { "" }
    },
    {
        "dark-knight basics", {-1,-1,-1,-1,0,-1,-1,-1,-1,-1,-1,-1,-1},
        { "" }
    },
    {
        "thief basics",       {-1,-1,-1,-1,-1,0,-1,-1,-1,-1,-1,-1,-1},
        { "" }
    },
    {
        "ninja basics",    {-1,-1,-1,-1,-1,-1,0,-1,-1,-1,-1,-1,-1},
        { "" }
    },
    {
        "cleric basics",      {-1,-1,-1,-1,-1,-1,-1,0,-1,-1,-1,-1,-1},
        { "" }
    },
    {
        "monk basics",        {-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1,-1,-1},
        { "" }
    },
    {
        "invoker basics",     {-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1,-1},
        { "" }
    },
    {
        "battlemage basics", {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1},
        { "" }
    },
    {
        "necromancer basics", {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1},
        { "" }
    },
    {
        "psiconicist basics", {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0},
        { "" }
    }
}; 


