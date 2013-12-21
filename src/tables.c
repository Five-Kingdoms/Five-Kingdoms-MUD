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
#include "jail.h"
#include "cabal.h"
#include "interp.h"
#include "comm.h"

const char*   day_name        [MAX_DAYS] =
{ 
  "Firstday", 
  "Seconday", 
  "Thirday", 
  "Fourthday", 
  "Fiftday", 
  "Sixday", 
  "Sevenday", 
  "Eightday", 
  "Nineday", 
  "Sunday" ,
};

const char*   month_name      [MAX_MONTHS] =
{
  "Life", 
  "Nature", 
  "Balance", 
  "Chaos", 
  "Death", 
  "Order",
};

/* Holds Jail info for JUSTICE jails 
name		holds name of hometown, NULL for termination
entrance	VNUM of room that the guard is in 
cell[MAX_CELL]	Array of cells, mates are dumped in lowest populated one first
exe_mob		Vnum of mob used for executions, (Also used for yells).
exe_room	Vnum of room used for execution, yells etc.

*/
const struct jail_type jail_table[MAX_HOMETOWN] =
{
  /* FIRST SPOT needed for "FALSE" return of 0 
     and for jail cells selected if characters hometown has no jails*/
  {"",		20900,	20501,	{20502, 20504, 20506}, 20901,	20605},
  {"ValMiran",  20900,	20501,	{20502, 20504, 20506}, 20901,	20605},
  {"Rheydin",   11698,  11453,  {11449, 11452, 11455}, 11455,   11500},  
  {NULL,	0,	0,	{0, 0, 0},	       0,	    0}
};

const struct execution_type execution_table[] = 
{
  /* first spot left blank for return of FALSE */
  {
    "","","","","",""
  },

  {
    "execution",
    "to execution!",
    "A crossbow bearing squad of guards enters the room led by $n.",
    "$n orders '`#Fire!``' as $N is riddled with poisoned arrows.",
    "$n orders '`#Fire!``' as the guards riddle you with poisoned arrows!",
    "A corpse of %s riddled with arrows, rests in a pool of its own blood."
  },

  {
    "behead",
    "to be beheaded!",
    "A large oaken stump is carried in, followed by $n bearing a large blunt tipped blade and an iron pike.",
    "$n says '`#Hope you won't miss your ugly head $N.``' as he brings a replica of Carnifex Pax down.\n\r"\
    "You hear a soft sound of a melon being cleaved in half as $N's head flies off and rolls to your feet.",
    "$n says '`#Hope you won't miss your ugly head $N.``' as he brings a replica of Carnifex Pax down.\n\r"\
    "Last thing you feel is a sting across your neck.",
    "%s's head is stuck on a tall metal pike here."
  },

  {
    "burnt",
    "to be burnt alive at the stake!",
    "$n and $s somber helpers quickly setup a large stake and a pire of dry wood.  All that is missing now is $N...",
    "$n applies a brand of firewood to the pire, and yells throught the roaring flames '`#Feeling hot today $N?!``'",
    "$n lights the pire.  Last thing you hear is $s voice through the roaring flames: '`#Feeling hot today $N?!``'",
    "An unrecognizable corpse, burnt to crisp and still tied to a giant stake."
  },

  {
    "whip",
    "to be whipped into bloody bits!",
    "Clad in $s hood, $n enters. A giant barbed cat'o'nine tail in $s hands.",
    "$n begins the whipping at a frantic pace, bits of $N's torn flesh and innards flying about!",
    "Soon after $n begins whipping at a frantic pace, your torn and tortured body fails you.",
    "A torn and shredded corpse barely resembling %s lies amidst mess of torn flesh and blood."
  },

  {
    "wheel",
    "to be broken on the Wheel of Justice!",
    "Grinning with pride $n rolls in a giant spiked wheel with many edged and barbed blades.",
    "With helps of $s assistants $n ties $N to the wheel and begins the procedure.\n\r"\
    "Soon the air is filled with sickly pop and crackle of bursting flesh, bones and sinew.",
    "With helps of $s assistants $n ties you to the wheel and begins the procedure.\n\r"\
    "Your bones begin to crack as your flesh is torn and ripped.  Darkness embraces your quickly.",
    "A broken heap of broken bones and mangled flesh barely resembling %s lies here."
  },

  {
    "hanged",
    "to be hanged for all to see!",
    "Chuckling and grinning to $mself, $n walks in and quickly prepares the gallows for $N.",
    "$n kicks the support out from under $N. $S's neck emits an audible crack followed by few jerks and kicks.",
    "$n kicks the support out from under you. Last sound you hear is your neck snapping.",
    "Dangling on the noose %s adds a bit of morbid charm to the area.",
  },

  {NULL, "", "", "", "",""}
};


/* psalm table (gGroup = TRUE means group allowed)*/
const struct psalm_type  psalm_table[] = 
{
  {"", NULL, 0, 0, FALSE, FALSE, FALSE},
/*	name,		gsn,   check,  
	chant,	wait,	fTwo,	fRand,	fGroup  */
  {"Righteousness", &gsn_righteous,  &gsn_righteous, 
				4, 120,	TRUE,	TRUE,	FALSE},
  {"Divine void",   &gsn_dvoid,	&gen_dvoid,
				5, 144,	TRUE,	TRUE,	FALSE},
  {"Insight",	    &gsn_insight, &gsn_insight,
				7, 396,	TRUE,	TRUE,	FALSE},
  {"Divine Wrath",  &gsn_dwrath, &gsn_dwrath,     
				1,  48,	TRUE,	FALSE,	FALSE},
  {"Shadow Bane",   &gsn_sbane, &gsn_sbane,     
				1,  48,	TRUE,	TRUE,	FALSE},

  {"Purewater",     &gsn_pwater, &gsn_pwater,     
				1,  48,	TRUE,	TRUE,	FALSE},
  {"Icefire",	    &gsn_icefire, &gsn_icefire,    
				1,  48,	TRUE,	TRUE,	FALSE},
  {"Searing Might", &gsn_smight, &gsn_smight,     
				3,  48,	TRUE,	TRUE,	FALSE},
  {"Divine Might",  &gsn_dmight, &gsn_dmight,     
				3,  48,	TRUE,	TRUE,	FALSE},
  {"Vitality",      &gsn_vitality, &gsn_vitality,
				7, 372,	TRUE,	TRUE,	FALSE},

  {"Purity",        &gsn_purity, &gsn_purity,     
				3, 120,	TRUE,	TRUE,	FALSE},
  {"Constitution",  &gsn_const, &gsn_const,
				3, 120,	TRUE,	TRUE,	FALSE},
  {"the Eye of Palison",&gsn_epal, &gsn_epal,  
				3, 120,	TRUE,	TRUE,	FALSE},
  {"Swift retribution",&gsn_sretrib, &gsn_sretrib,
				0,  48,	TRUE,	TRUE,	TRUE},
  {"Virtues",       &gsn_virtues, &gsn_virtues,  
				1,   1,	TRUE,	TRUE,	TRUE},

  {"Preservation",  &gsn_preserve,  &gsn_preserve,
				    1,   46,	TRUE,	TRUE,	TRUE},
  {"Final Parting", &gsn_fpart,     &gsn_fpart,
				    1,   96,	TRUE,	FALSE,	FALSE},
  {"Divine Health", &gsn_dheal,     &gsn_dheal,
				    2,   96,	TRUE,	FALSE,	FALSE},
  {"Vicarious Redemption",&gsn_vredem, &gsn_vredem,
				    8,360,	TRUE,	TRUE,	FALSE},
  {"the Triumph of One God",&gsn_ogtrium, &gsn_ogtrium,
				   10,  360,	TRUE,	FALSE,	TRUE},

  {"the Triumph of Sirant",&gsn_strium, &gsn_strium,
				   11,  360,	TRUE,	FALSE,	TRUE},
  {"the Triumph of Palison",&gsn_ptrium, &gsn_ptrium,
				   11,  360,	TRUE,	FALSE,	FALSE},
  {"Undeniable Order",&gsn_uorder, &gsn_uorder,     
				   11,  360,	TRUE,	FALSE,	FALSE},
  {"Baptism",      &gsn_baptize,   &gsn_baptize,
				   11,  20,	FALSE,	FALSE,	FALSE},
  {"Inner Strength",&gsn_istrength, &gsn_istrength,     
				    3,   96,	TRUE,	FALSE,	FALSE},
  {"Inner Peace",&gsn_icalm, &gsn_icalm,     
				    23,  239,	TRUE,	TRUE,	FALSE},


  {NULL, NULL, NULL, 0, 0, FALSE, FALSE, FALSE},
};

const struct hometown_type hometown_table[MAX_HOMETOWN] =
{
    /*  name, Temple:   good,     neutral,  evil        */
    /*  pit room :	good,	  neutral,  evil        */
    /*  pit obj :	good,	  neutral,  evil        */
    /*  race						*/

  { 
    "ValMiran",		
    20790,    20784,	    0,        
    20790,    20784,	    0,
    20700,    20784,        0,       
    NULL
  },	

  { 
    "Falen_Dara", 
    9609,     9609,     9609,        
    9610,     9610,     9610,	
    9609,     9609,     9609,
    "Slith"
  },

  { "Miruvhor",
    0,	 7870,	   7870,	
    0,	 7870,	   7870,	
    0,	 7870,	   7870,
    NULL,
  },

  { "Marak",
    7646,	 7646,	   7646,	
    7646,	 7646,	   7646,	
    7646,	 7646,	   7646,
    "Ogre"
  },  


  { "Shasarazade",
    8293,         0,     0,
    8293,         0,     0,
    8293,         0,     0,
    "Elf"
  },


  { "Xymerria", 
    0,         0,     1281,
    0,         0,     1281,
    0,         0,     1281,
    "Drow"
  },  

  { "Rheydin",
    11663,	  11667,    11665,
    11663,        11667,    11665,
    11663,        11667,    11666,
    NULL
  },

  { "Illithid-Hive",
    0,             0,     8049,
    0,             0,     8049,
    0,             0,     8049,
    "Illithid"
  },  

  { "Tarandue",
    0,          4109,     0,
    0,          4109,     0,
    0,          4109,     0,
    "Gnome"
  },    

  { "Shire",
    1185,       1185,     0,
    1185,       1185,     0,
    1185,       1185,     0,
  "Halfling"
  }, 

  { "Gal-Ranidon",
    0,        0,     21052,
    0,        0,     21052,
    0,        0,     21052,
    "Duergar"
  }, 

  { "Khorandain", 
    21579,        21559,     0,
    21578,        21560,     0,
    21510,        21511,     0,
    "Dwarf"
  }, 

  { "Ralardia", 
    0,        11922,     0,
    0,        11921,     0,
    0,        11904,     0,
    "werebeast"
  }, 

  /*
  { "Airia", 
    0,        10972,     0,
    0,        10972,     0,
    0,        10924,     0,
    "avian"
  }, 
  */
  { NULL,
    0,        0,        0,        
    0,        0,        0,        
    0,        0,        0,
    NULL
  }
};


/* table with bash attacks */
/* format:
to_char:  "Your %s is ineffective as %s a <shield name>."
to_vict:  "$n's %s %s <shield name>."
to_room:  "$n's %s %s $N's <shield name>",

**look in skill.c lagprot_msg for details.
*/

const struct lagattack_type lagattack_table[] = 
{
  {
    &gsn_bash, 
    "bash", 
    "you slam into",
    "collides with your",
    "collides with",

    //balance etc.
    "You slam into $N, but $E stays on $S feet!",
    "$n hits you with a powerful bash, but you stay on your feet!",
    "$n hits $N with a powerful bash, but $E stays on $S feet."
  },

  {
    &gsn_bodyslam, 
    "bodyslam", 
    "you slam into",
    "collides into your",
    "collides with",
    
//balance
    "You slam into $N, but $E stays on $S feet!",
    "$n knocks you with a full on bodyslam, but you stay on your feet!",
    "$n knocks $N a full on bodyslam, but $E stays on $S feet."
  },

  {
    &gsn_grapple, 
    "grapple", 
    "your hands slide around",
    "is useless due to your",
    "is useless due to",

    //balance
    "You maneuver to grapple $N, but $E keeps $S balance!",
    "$n maneuvers to grapple you, but you keep your balance!",
    "$n maneuvers to grapple $N, but $E keeps $S balance."
  },

  {
    &gsn_thrust,
    "thrust", 
    "it becomes absorbed by",
    "is absorbed by your",
    "collides into",

    //balance
    "Your force slams into $N, but $E keeps $S balance!",
    "$n's force slams into you, but you keep your balance!",
    "$n's force slams into $N, but $E keeps $S balance.",
  },

  {
    &gsn_wolf_form, 
    "tackle", 
    "it's stopped short by",
    "seems to land short before your",
    "collides into",

    //balance
    "You tackle $N, but $E stays $S feet!",
    "$n tackles $N, but $E stays $S feet.",
    "$n tackles you, but you stay on your feet!"
  },

  {
    &gsn_armored_rush,
    "swipe", 
    "it's stopped short by",
    "digs into your",
    "digs into",

    //balance
    "You swipe at $N, but $E stays $S feet!",
    "$n swipes at $N, but $E stays $S feet.",
    "$n swipes at you, but you stay on your feet!"
  },

  {
    NULL, 
    "",
    "",
    "",
    "",

    //balance
    "",
    "",
    "",
  },
};


/* table with the possible paths */
const struct flag_type path_table[] = {
  {"Life",		PATH_LIFE,	TRUE},
  {"Death",		PATH_DEATH,	TRUE},
  {"Chance",		PATH_CHANCE,	TRUE},
  {"Equilibrium",	PATH_EQUIL,	TRUE},
  {"Knowledge",		PATH_KNOW,	TRUE},
  {"Faith",		PATH_FAITH,	TRUE},

  {NULL, 0, FALSE}
};

/* table with the way/align/ethos/god for religions 
 * Table length set by MAX_DIETY in merc.h 
 * WAY, GOD, PATH, 
 ALIGN, 
 ETHOS , 
 SHRINE, TATTOO_VNUM

 * NOTE: IF A SHRINE IS NOT PRESENT, THE WAY WILL PREVENT COMMUNING
 */
const struct deity_type deity_table[MAX_DIETY]=
{
  {
    "Fate",		"Creator",	PATH_FAITH,
    0,
    0,
    0, 30027,
  },
  {
    "Compassion",	"Sylvantis",	PATH_LIFE,
    DIETY_ALIGN_GOOD,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    9200, 180,
  },
  {
    "Purity",		"Sylvantis",	PATH_LIFE,
    DIETY_ALIGN_GOOD,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    8900, 181,
  },
  {
    "Tranquility",	"Sylvantis",	PATH_LIFE,
    DIETY_ALIGN_GOOD,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    3598, 182,
  },


  {
    "Scourge",		"Virigoth",	PATH_DEATH,
    DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    2718, 191,
  },
  {
    "Cycle",		"Virigoth",	PATH_DEATH,
    DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    3472, 30001,
  },
  {
    "Necropsy",		"Virigoth",	PATH_DEATH,
    DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    2200, 192,
  },


  {
    "Discord",		"Avariel",	PATH_CHANCE,
    DIETY_ALIGN_NEUTRAL | DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    2300, 188,
  },

  {
    "Guile",		"Avariel",	PATH_CHANCE,
    DIETY_ALIGN_GOOD | DIETY_ALIGN_NEUTRAL | DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    1150, 190,
  },
  {
    "Greed",		"Avariel",	PATH_CHANCE,
    DIETY_ALIGN_GOOD | DIETY_ALIGN_NEUTRAL | DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    900, 189,
  },


  {
    "Order",		"Bryntryst",	PATH_EQUIL,
    DIETY_ALIGN_NEUTRAL,
    DIETY_ETHOS_LAWFUL,
    8890, 183,
  },
  {
    "Neutrality",	"Bryntryst",	PATH_EQUIL,
    DIETY_ALIGN_NEUTRAL,
    DIETY_ETHOS_NEUTRAL,
    2719, 185,
  },
  {
    "Chaos",		"Bryntryst",	PATH_EQUIL,
    DIETY_ALIGN_NEUTRAL,
    DIETY_ETHOS_CHAOTIC,
    1499, 184,
  },


  {
    "Mystic",		"Mircea",	PATH_KNOW,
    DIETY_ALIGN_GOOD | DIETY_ALIGN_NEUTRAL | DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    1484, 186,
  },
  {
    "Combat",		"Mhalador",	PATH_KNOW,
    DIETY_ALIGN_GOOD | DIETY_ALIGN_NEUTRAL | DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    990, 187,
  },
  {
    "Nature",		"Kalug",	PATH_KNOW,
    DIETY_ALIGN_GOOD | DIETY_ALIGN_NEUTRAL | DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    2530, 196,
  },
  {
    "Atheism",	"Creator",	PATH_FAITH,
    DIETY_ALIGN_GOOD | DIETY_ALIGN_NEUTRAL | DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    0, 195,
  },

  {
    "Agnostic",		"Creator",	PATH_FAITH,
    DIETY_ALIGN_GOOD | DIETY_ALIGN_NEUTRAL | DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    0, 194,
  },
  {
    "Church",	"The One God",	PATH_FAITH,
    DIETY_ALIGN_GOOD | DIETY_ALIGN_NEUTRAL | DIETY_ALIGN_EVIL,
    DIETY_ETHOS_LAWFUL | DIETY_ETHOS_NEUTRAL | DIETY_ETHOS_CHAOTIC,
    20800, 193,
  },

  {NULL, NULL, 0, 0, 0, 0}
};

const struct flag_type crime_table[] =
{
  {	"Sheath"	,	CRIME_FLAG_S,	FALSE },
  {	"Looting"	,	CRIME_FLAG_L,	TRUE },
  {	"Theft"		,	CRIME_FLAG_T,	TRUE },
  {	"Assault"	,	CRIME_FLAG_A,	TRUE },
  {	"Murder"	,	CRIME_FLAG_M,	TRUE },
  {	"Obstruction"	,	CRIME_FLAG_O,	FALSE },
  {	NULL		,		   0,	FALSE }
};

const struct flag_type punish_table[] =
{
  {	"Allowed"	,	CRIME_ALLOW,	TRUE },
  {	"Petty"		,	CRIME_LIGHT,	TRUE },
  {	"Severe"	,	CRIME_NORMAL,	TRUE },
  {	"Felony"	,	CRIME_HEAVY,	TRUE },
  {	"Attrocity"	,	CRIME_DEATH,	TRUE },
  {	NULL		,		   0,	FALSE }
};

const struct flag_type pact_flags[] = 
{
  {	"none"		,	PACT_NONE,	FALSE },
  {	"Trade"		,	PACT_TRADE,	TRUE },
  {	"NA-Pact",		PACT_NAPACT,	FALSE },
  {	"Alliance"	,	PACT_ALLIANCE,	FALSE },
  {	"Vendetta"	,	PACT_VENDETTA,	TRUE },
  {	"Peace"	,		PACT_PEACE,	TRUE },
  {	"Embargo",		PACT_BREAK,	TRUE },
  {	"Negotiate",		PACT_IMPROVE,	TRUE },
  {	NULL		,		   0,	FALSE }
};

const struct flag_type mprog_flags[] =
{
    {	"act",			TRIG_ACT,		TRUE	},
    {	"fight",		TRIG_FIGHT,		TRUE	},
    {	"death",		TRIG_DEATH,		TRUE	},
    {	"kill",			TRIG_KILL,		TRUE	},
    {	"hpcnt",		TRIG_HPCNT,		TRUE	},
    {	"give",			TRIG_GIVE,		TRUE	},
    {	"bribe",		TRIG_BRIBE,		TRUE	},
    {	"greet",		TRIG_GREET,		TRUE	},
    {   "grall",		TRIG_GRALL,		TRUE	},
    {	"random",		TRIG_RANDOM,		TRUE	},
    {	"tell",			TRIG_TELL,		TRUE	},
    {   "speech",		TRIG_SPEECH,		TRUE	},
    {	"entry",		TRIG_ENTRY,		TRUE	},
    {	"exit",			TRIG_EXIT,		TRUE	},
    {	"exall",		TRIG_EXALL,		TRUE	},
    {	"delay",		TRIG_DELAY,		TRUE	},
    {	"drop",			TRIG_DROP,		TRUE	},
    {	"get",			TRIG_GET,		TRUE	},
    {	"put",			TRIG_PUT,		TRUE	},
    {	"special",		TRIG_SPECIAL,		TRUE	},
    {	"look",			TRIG_LOOK,		TRUE	},
    {	NULL,			0,			TRUE	},
};


const struct flag_type oprog_flags[] =
{
    {	"act",			TRIG_ACT,		TRUE	},
    {	"fight",		TRIG_FIGHT,		TRUE	},
    {	"death",		TRIG_DEATH,		TRUE	},
    {	"kill",			TRIG_KILL,		TRUE	},
    {	"give",			TRIG_GIVE,		TRUE	},
    {   "grall",		TRIG_GRALL,		TRUE	},
    {	"random",		TRIG_RANDOM,		TRUE	},
    {   "speech",		TRIG_SPEECH,		TRUE	},
    {	"exall",		TRIG_EXALL,		TRUE	},
    {	"delay",		TRIG_DELAY,		TRUE	},
    {	"drop",			TRIG_DROP,		TRUE	},
    {	"get",			TRIG_GET,		TRUE	},
    {	"put",			TRIG_PUT,		TRUE	},
    {	"sit",			TRIG_SIT,		TRUE	},
    {	"use",			TRIG_USE,		TRUE	},
    {	"look",			TRIG_LOOK,		TRUE	},
    {	"shoot",		TRIG_SHOOT,		TRUE	},
    {	NULL,			0,			TRUE	},
};

const struct flag_type rprog_flags[] =
{
    {	"act",			TRIG_ACT,		TRUE	},
    {	"fight",		TRIG_FIGHT,		TRUE	},
    {	"death",		TRIG_DEATH,		TRUE	},
    {	"kill",			TRIG_KILL,		TRUE	},
    {	"drop",			TRIG_DROP,		TRUE	},
    {	"grall",		TRIG_GRALL,		TRUE	},
    {	"random",		TRIG_RANDOM,		TRUE	},
    {	"speech",		TRIG_SPEECH,		TRUE	},
    {	"exall",		TRIG_EXALL,		TRUE	},
    {	"delay",		TRIG_DELAY,		TRUE	},
    {	"get",			TRIG_GET,		TRUE	},
    {	"put",			TRIG_GET,		TRUE	},
    {	"sit",			TRIG_SIT,		TRUE	},
    {	NULL,			0,			TRUE	},
};


/* table with descriptions for study total */
/* If adding in here, %s can be replaced with the char name 
ONLY in the 6th entry, this was done in order to be
able to display character's name even when he is not online.
*/
const struct study_msg_type  study_msg_table [] = 
{
  {"You would be a fool to try to"},
  {"You should wait another day before trying to"},
  {"You have a fair chance to"},
  {"There is little doubt you could"},
  {"You are certain of your ability to"},
  {"Even blind and deaf you would still"},
  {"%s's soul bears your mark"}
};

/* room name color table referenced byt SECT_CONST */
const struct room_color_type room_color_table [MAX_SECT] =
{
  {'8'}, /* SECT_INSIDE  dark-gray*/
  {'7'}, /* SECT_CITY  bright-green/blue*/
  {'3'}, /* SECT_FIELD  dark-white*/
  {'2'}, /* SECT_FOREST dark-green*/
  {'3'}, /* SECT_HILLS dark-yellow*/
  {'3'}, /* SECT_MOUNTAIN dark-yellow*/
  {'^'}, /* SECT_WATER_SWIM  dark-blue*/
  {'4'}, /* SECT_WATER_NOSWIM dark-blue*/
  {'8'}, /* SECT_SWAMP bright-green*/
  {'6'}, /* SECT_AIR dark-cyan?*/
  {'#'}, /* SECT_DESERT  bright-yellow*/
  {'1'}, /* SECT_LAVA  bright-red */
  {'&'}, /* SECT_SNOW snow */
};


/* mana charge amount description table */
const struct mcharge_type  mcharge_table[] =
{
  {"no"},
  {"a tiny"},
  {"a small"},
  {"a medium"},
  {"a considerable"},
  {"a large"},
  {"an extensive"},
  {"a potent"},
  {"an immense"},
  {"a titanic"},
  {"a deadly"},
  {"a `@lethal``"},
  {"NULL"}
};

/* contains descriptions for capacity of mana charged item in % */
const struct maxmcharge_type  maxmcharge_table[] =
{
 {"insignificant"},
 {"trivial"},
 {"small"},
 {"ample"},
 {"medium"},
 {"half"},
 {"considerable"},
 {"substantial"},
 {"high"},
 {"almost full"},
 {"`!critical``"},
 {"NULL"},
 
};
 

const struct position_type position_table[] =
{
    {	"dead",			"dead"	},
    {	"mortally wounded",	"mort"	},
    {	"incapacitated",	"incap"	},
    {	"stunned",		"stun"	},
    {	"meditate",		"med"	},
    {	"sleeping",		"sleep"	},
    {	"resting",		"rest"	},
    {   "sitting",		"sit"   },
    {	"fighting",		"fight"	},
    {	"standing",		"stand"	},
    {	NULL,			NULL	}
};

const struct sex_type sex_table[] =
{
   {    "none"          },
   {    "male"          },
   {    "female"        },
   {    "either"        },
   {    NULL            }
};

const struct ethos_type ethos_table[] =
{
   {    "lawful"        },
   {    "neutral"       },
   {    "chaotic"       },
   {    NULL            }
};


const struct anatomy_type anatomy_table[] =
{
   {	"mob"		},
   {	"human"		},
   {	"elf"		},
   {	"dwarf"		},
   {	"demihuman"	},
   {	"giant"		},
   {	"beast"		},
   {	"flying"	},
   {	"unique"	},
   {	NULL		}
};

const struct size_type size_table[] =
{ 
    {	"tiny"		},
    {	"small" 	},
    {	"medium"	},
    {	"large"		},
    {   "huge"          },
    {	"giant" 	},
    {	NULL		}
};

const struct flag_type act_flags[] =
{
    {	"npc",			ACT_IS_NPC,	FALSE	},
    {	"sentinel",		ACT_SENTINEL,	TRUE	},
    {	"scavenger",		ACT_SCAVENGER,	TRUE	},
    {	"aggressive",		ACT_AGGRESSIVE,	TRUE	},
    {	"contracter",		ACT_CONTRACTER,	TRUE	},
    {	"mount",		ACT_MOUNT,	TRUE	},
    {	"noncombat",		ACT_NONCOMBAT,	TRUE	},
    {	"raider",		ACT_RAIDER,	FALSE	},
    {	"stay_area",		ACT_STAY_AREA,	TRUE	},
    {	"wimpy",		ACT_WIMPY,	TRUE	},
    {	"pet",			ACT_PET,	TRUE	},
    {	"train",		ACT_TRAIN,	TRUE	},
    {	"practice",		ACT_PRACTICE,	TRUE	},
    {	"undead",		ACT_UNDEAD,	TRUE	},
    {	"cleric",		ACT_CLERIC,	TRUE	},
    {	"mage",			ACT_MAGE,	TRUE	},
    {	"thief",		ACT_THIEF,	TRUE	},
    {	"warrior",		ACT_WARRIOR,	TRUE	},
    {	"noalign",		ACT_NOALIGN,	TRUE	},
    {	"nopurge",		ACT_NOPURGE,	TRUE	},
    {	"outdoors",		ACT_OUTDOORS,	TRUE	},
    {	"indoors",		ACT_INDOORS,	TRUE	},
    {   "too_big",              ACT_TOO_BIG,    TRUE    },
    {	"healer",		ACT_IS_HEALER,	TRUE	},
    {	"gain",			ACT_GAIN,	TRUE	},
    {	"update_always",	ACT_UPDATE_ALWAYS,	TRUE	},
    {   "wander",               ACT_WANDER,     TRUE    },
    {   "no_wander",            ACT_NO_WANDER,   TRUE    },
    {	NULL,			0,	FALSE	}
};
const struct flag_type act2_flags[] =
{
    {   "duplicate",            ACT_DUPLICATE,	FALSE   },
    {   "noexp",            	ACT_NOEXP,	TRUE    },
    {   "lawful",		ACT_LAWFUL,	FALSE   },	//set by spec_gateguard, and spec_executioner
    {   "history",              ACT_HISTORY,	TRUE    },
    {   "needsmaster",          ACT_NEED_MASTER,TRUE    },
    {   "morph",		ACT_MORPH,      TRUE    },
    {   "noloss",		ACT_NOLOSS,     TRUE    },
    {   "nodeath",		ACT_NODEATH,    TRUE    },
    {   "army",			(J),		TRUE	},
    {	NULL,			0,		FALSE	}
};

const struct flag_type plr_flags[] =
{
    {	"npc",			PLR_IS_NPC,	FALSE	},
    {	"autoassist",		PLR_AUTOASSIST,	FALSE	},
    {	"autoexit",		D,	FALSE	},
    {	"autoloot",		E,	FALSE	},
    {	"autosac",		F,	FALSE	},
    {	"autogold",		G,	FALSE	},
    {	"autosplit",		H,	FALSE	},
    {	"eqlimit",		PLR_EQLIMIT,	FALSE	},
    {	"holylight",		N,	FALSE	},
    {	"undead",		O,	TRUE	},
    {	"nosummon",		Q,	FALSE	},
    {	"nofollow",		R,	FALSE	},
    {	"permit",		U,	TRUE	},
    {	"log",			W,	FALSE	},
    {	"deny",			X,	FALSE	},
    {	"freeze",		Y,	FALSE	},
    {   "wanted",               Z,      FALSE   },
    {   "contracted",           aa,     FALSE   },
    {   "doof",                 bb,     FALSE   },
    {   "hearall",              ee,     FALSE   },
    {   "outcast",              dd,     FALSE   },
    {   "masquerade",           J,      FALSE   },
    {   "coding",               K,      FALSE   },
    {   "royal",                L,      FALSE   },
    {   "noble",                M,      FALSE   },
    {	"description",		N,	TRUE	},
    {	"avatar",		P,	TRUE	},
    {	NULL,			0,	0	}
};

/* terminated with non-positive */
/* 
this is a bit different from normal flag type use
it stores strings to be printed on login if that flag 
is present.
*/
const struct flag_type message_flags[] =
{
  {"While you were gone a new quest has been given to you!",
   MSG_QUEST,	FALSE},
  {"Due to lack of your presence your rare equipment has been lost (\"help rare\").",
   MSG_PURGE,	FALSE},
  {"Your cabal application has been voted on and `#accepted``! (\"help cabal\").",
   MSG_CAB_ACCEPT,	FALSE},
  {"Your cabal application has been voted on and `!rejected``. (\"help cabal\").",
   MSG_CAB_REJECT,	FALSE},
  {"You have been promoted within your organization! (\"help promote\")",
   MSG_CAB_PROM,	FALSE},
  {"You have been demoted within your organization!",
   MSG_CAB_DEMO,	FALSE},
  {"You have been `!expelled`` from your organization!",
   MSG_CAB_EXPEL,	FALSE},
  {"Due to lack of your presence you have been removed from your organization!",
   MSG_CAB_TEXPEL,	FALSE},
  {"Underground contacts of your organization report a new bounty on your head.",
   MSG_BOUNTY,		FALSE},
  {"`@You have recived some items as a result of a bounty being fulfilled.``",
   MSG_BOUNTY_REWARD,		FALSE},
  {"`@One of your bounties has been returned to you.``",
   MSG_BOUNTY_RETURN,		FALSE},
  {"`@You have been given a chance to apply to your clan's Cabal.``",
   MSG_CABAL_ALLOW,		FALSE},
  {	 NULL,				-1,	FALSE }
  
};

/* basic cabal room data flags */
const struct flag_type croom_flags[] =
{
    {	"nodup",		CROOM_NODUP,	TRUE	},
    {	"fixedname",		CROOM_FIXEDNAME,TRUE	},
    {	"pitroom",		CROOM_PITROOM,	TRUE	},
    {	 NULL,				0,	FALSE	}
};

/* cabal progression bits */
const struct flag_type cabal_progress_flags[] =
{
  {	"cp",			PROGRESS_CP,	TRUE },
  {	"skill",		PROGRESS_SKILL,	TRUE },
  {	"spell",		PROGRESS_SPELL,	TRUE },
  {	"quest",		PROGRESS_QUEST,	TRUE },
  {	"coup",			PROGRESS_COUP,	TRUE },
  {	"chall",		PROGRESS_CHALL,	TRUE },
  {	NULL,			0,	FALSE	}
};

/* basic cabal flags */
const struct flag_type cabal_flags[] =
{
    {	"changed",		CABAL_CHANGED,	FALSE	},
    {	"vrchanged",		CABAL_VRCHANGED,FALSE	},
    {	"chat",			CABAL_CHAT,	FALSE	},
    {	"coffers",		CABAL_COFFER,	FALSE	},
    {	"canhide",		CABAL_CANHIDE,	TRUE	},
    {	"canshow",		CABAL_CANSHOW,	TRUE	},
    {	"justice",		CABAL_JUSTICE,	TRUE	},
    {	"royal",		CABAL_ROYAL,	TRUE	},
    {	"nomagic",		CABAL_NOMAGIC,	TRUE	},
    {	"service",		CABAL_SERVICE,	TRUE	},
    {	"area",			CABAL_AREA,	TRUE	},
    {	"rouge",		CABAL_ROUGE,	TRUE	},
    {	"sponsor",		CABAL_SPONSOR,	TRUE	},
    {	"highgain",		CABAL_HIGHGAIN,	FALSE	},
    {	"hearguard",		CABAL_HEARGUARD,FALSE	},
    {	"gate",			CABAL_GATE,	FALSE	},
    {	"guard",		CABAL_GUARDMOB,	FALSE	},

    {	"allow2",		CABAL_ALLOW_2,	FALSE	},
    {	"allow3",		CABAL_ALLOW_3,	FALSE	},
    {	"allow4",		CABAL_ALLOW_4,	FALSE	},
    {	"allow5",		CABAL_ALLOW_5,	FALSE	},

    {	"subcabal",		CABAL_SUBCABAL,	FALSE	},
    {	"trade",		CABAL_TRADE,	FALSE	},
    {	"napact",		CABAL_NAPACT,	FALSE	},
    {	"alliance",		CABAL_ALLIANCE,	FALSE	},
    {	"vendetta",		CABAL_VENDETTA,	FALSE	},
    {	"spies",		CABAL_SPIES,	FALSE	},

    {	"elite",		CABAL_ELITE,	TRUE	},
    {	"swarms",		CABAL_SWARM,	TRUE	},
    {	"history",		CABAL_HISTORY,	TRUE	},
    {	"raid",			CABAL_RAID,	TRUE	},

    {	 NULL,				0,	FALSE }

};

const struct flag_type cabal_flags2[] =
{
    {	"newbie",		CABAL2_NEWBIE,	TRUE	},
    {	"noteall",		CABAL2_NOTEALL,	TRUE	},
    {	"areagain",		CABAL2_AREAGAIN,FALSE	},

    {	 NULL,				0,	FALSE }

};
/* Color flags */
/* terminated with non-positive */
const struct flag_type color_flags[] =
{
  {	"on",		COLOR_ON,		TRUE },
  {	"damage",	COLOR_DAMAGE,		TRUE },
  {	 NULL,				-1,	FALSE }
};

/* Gameplay flags */
/* terminated with non-positive */
const struct flag_type game_flags[] =
{
  {	"automob",	GAME_AMOB,		TRUE },
  {	"autopc",	GAME_APC,		TRUE },
  {	"autoobj",	GAME_AOBJ,		TRUE },
  {	"autocabal",	GAME_SHOW_CABAL,	TRUE },
  {	"no-requip",	GAME_NOREQ_CABAL,	TRUE },
  {	"autosex",	GAME_SEX,		TRUE },
  {	"newbie",	GAME_NEWBIE,		TRUE },
  {	"nospells",	GAME_NOSPELLS,		TRUE },
  {	"sheath",	GAME_ASHEATH,		TRUE },
  {	"mercy",	GAME_MERCY,		TRUE },
  {	"noquestor",	GAME_NOQUESTOR,		TRUE },
  {	"noalias",	GAME_NOALIAS,		TRUE },
  {	"newscore",	GAME_NEWSCORE,		TRUE },
  {	"nofire",	GAME_NOAUTOFIRE,	TRUE },
  {     "dndspells",	GAME_DNDS,		FALSE },
  {	"`!pk warn``",	GAME_PKWARN,		FALSE },
  {	"`!STAINED``",	GAME_STAIN,		FALSE },
  {	 NULL,				-1,	FALSE }

};


const struct flag_type affect_flags[] =
{
    {	"blind",		A,	TRUE	},
    {	"invisible",		B,	TRUE	},
    {	"detect_evil",		C,	TRUE	},
    {	"detect_invis",		D,	TRUE	},
    {	"detect_magic",		E,	TRUE	},
    {	"detect_hidden",	F,	TRUE	},
    {	"detect_good",		G,	TRUE	},
    {	"sanctuary",		H,	TRUE	},
    {	"faerie_fire",		I,	TRUE	},
    {	"infrared",		J,	TRUE	},
    {	"curse",		K,	TRUE	},
    {	"poison",		M,	TRUE	},
    {	"protect_evil",		N,	TRUE	},
    {	"protect_good",		O,	TRUE	},
    {	"sneak",		P,	TRUE	},
    {	"hide",			Q,	TRUE	},
    {	"sleep",		R,	TRUE	},
    {	"charm",		S,	TRUE	},
    {	"flying",		T,	TRUE	},
    {	"pass_door",		U,	TRUE	},
    {	"haste",		V,	TRUE	},
    {	"calm",			W,	TRUE	},
    {	"plague",		X,	TRUE	},
    {	"weaken",		Y,	TRUE	},
    {   "protect_shield",       Z,      TRUE    },
    {	"berserk",		aa,	TRUE	},
    {	"siege",		AFF_SIEGE,	FALSE	},
    {	"slow",			dd,	TRUE	},
    {	NULL,			0,	0	}
};

const struct flag_type affect2_flags[] =
{
    {	"protect_shield",	A,	TRUE	},
    {	"ice_shield",		B,	TRUE	},
    {	"fire_shield",		C,	TRUE	},
    {	"hold",			D,	TRUE	},
    {	"blur",			E,	TRUE	},
    {	"terrain",		F,	TRUE	},
    {	"misdirection",		G,	TRUE	},
    {	"shadowform",		H,	TRUE	},
    {	"rage",			I,	TRUE	},
    {	"camouflage",		J,	TRUE	},
    {	"keen_sight",		K,	TRUE	},
    {	"silence",		L,	TRUE	},
    {	"mana_shield",		M,	TRUE	},
    {	"barrier",		O,	TRUE	},
    {	"catalepsy",		P,	TRUE	},
    {	"severed",		AFF_SEVERED,	FALSE	},
    {	"petrify",		AFF_PETRIFY,	TRUE	},
    {	"affect noremove",	bb,	FALSE	},
    {	NULL,			0,	0	}
};

const struct flag_type off_flags[] =
{
    {	"area_attack",		A,	TRUE	},
    {	"backstab",		B,	TRUE	},
    {	"bash",			C,	TRUE	},
    {	"berserk",		D,	TRUE	},
    {	"disarm",		E,	TRUE	},
    {	"dodge",		F,	TRUE	},
    {	"fade",			G,	FALSE	},
    {	"fast",			H,	TRUE	},
    {	"kick",			I,	TRUE	},
    {	"dirt_kick",		J,	TRUE	},
    {	"parry",		K,	TRUE	},
    {	"rescue",		L,	TRUE	},
    {	"tail",			M,	FALSE	},
    {	"trip",			N,	TRUE	},
    {	"crush",		O,	FALSE	},
    {	"assist_all",		P,	TRUE	},
    {	"assist_align",		Q,	TRUE	},
    {	"assist_race",		R,	TRUE	},
    {	"assist_players",	S,	TRUE	},
    {	"assist_guard",		T,	TRUE	},
    {	"assist_vnum",		U,	TRUE	},
    {	"guild_guard",		V,	TRUE	},
    {	"cabal_guard",		W,	TRUE	},
    {	"extra_attack",		X,	TRUE	},
    {	"dual_parry",		OFF_DPARRY,	TRUE	},
    {   "bastion",		Z,	TRUE	},
    {	NULL,			0,	0	}
};

const struct flag_type imm_flags[] =
{
    {	"summon",		A,	TRUE	},
    {	"charm",		B,	TRUE	},
    {	"magic",		C,	TRUE	},
    {	"weapon",		D,	TRUE	},
    {	"bash",			E,	TRUE	},
    {	"pierce",		F,	TRUE	},
    {	"slash",		G,	TRUE	},
    {	"fire",			H,	TRUE	},
    {	"cold",			I,	TRUE	},
    /* light must be before lighting as its a prefix of it. */
    {	"light",		S,	TRUE	},
    {	"lightning",		J,	TRUE	},
    {	"acid",			K,	TRUE	},
    {	"poison",		L,	TRUE	},
    {	"negative",		M,	TRUE	},
    {	"holy",			N,	TRUE	},
    {	"energy",		O,	TRUE	},
    {	"mental",		P,	TRUE	},
    {	"disease",		Q,	TRUE	},
    {	"drowning",		R,	TRUE	},
    {	"sound",		T,	TRUE	},
    {	"wood",			X,	TRUE	},
    {	"silver",		Y,	TRUE	},
    {	"iron",			Z,	TRUE	},
    {	"mithril",		aa,	TRUE	},
    {	"air",			VULN_AIR,	TRUE	},
    {	NULL,			0,	0	}
};

const struct flag_type special_flags[] =
{
    {   "sentinel",		SPECIAL_SENTINEL,	TRUE	},
    {   "return",		SPECIAL_RETURN,		TRUE	},
    {   "watch",		SPECIAL_WATCH,		TRUE	},
    {   "coward",		SPECIAL_COWARD,		TRUE	},
    {	NULL,			0,			0	}
};


const struct flag_type form_flags[] =
{
    {	"edible",		FORM_EDIBLE,		TRUE	},
    {	"poison",		FORM_POISON,		TRUE	},
    {	"magical",		FORM_MAGICAL,		TRUE	},
    {	"instant_decay",	FORM_INSTANT_DECAY,	TRUE	},
    {	"other",		FORM_OTHER,		TRUE	},
    {	"animal",		FORM_ANIMAL,		TRUE	},
    {	"sentient",		FORM_SENTIENT,		TRUE	},
    {	"undead",		FORM_UNDEAD,		TRUE	},
    {	"construct",		FORM_CONSTRUCT,		TRUE	},
    {	"mist",			FORM_MIST,		TRUE	},
    {	"intangible",		FORM_INTANGIBLE,	TRUE	},
    {	"biped",		FORM_BIPED,		TRUE	},
    {	"centaur",		FORM_CENTAUR,		TRUE	},
    {	"insect",		FORM_INSECT,		TRUE	},
    {	"spider",		FORM_SPIDER,		TRUE	},
    {	"crustacean",		FORM_CRUSTACEAN,	TRUE	},
    {	"worm",			FORM_WORM,		TRUE	},
    {	"blob",			FORM_BLOB,		TRUE	},
    {	"mammal",		FORM_MAMMAL,		TRUE	},
    {	"bird",			FORM_BIRD,		TRUE	},
    {	"reptile",		FORM_REPTILE,		TRUE	},
    {	"snake",		FORM_SNAKE,		TRUE	},
    {	"dragon",		FORM_DRAGON,		TRUE	},
    {	"amphibian",		FORM_AMPHIBIAN,		TRUE	},
    {	"fish",			FORM_FISH ,		TRUE	},
    {	"cold_blood",		FORM_COLD_BLOOD,	TRUE	},
    {	NULL,			0,			0	}
};

const struct flag_type part_flags[] =
{
    {	"head",			PART_HEAD,		TRUE	},
    {	"arms",			PART_ARMS,		TRUE	},
    {	"legs",			PART_LEGS,		TRUE	},
    {	"heart",		PART_HEART,		TRUE	},
    {	"brains",		PART_BRAINS,		TRUE	},
    {	"guts",			PART_GUTS,		TRUE	},
    {	"hands",		PART_HANDS,		TRUE	},
    {	"feet",			PART_FEET,		TRUE	},
    {	"fingers",		PART_FINGERS,		TRUE	},
    {	"ear",			PART_EAR,		TRUE	},
    {	"eye",			PART_EYE,		TRUE	},
    {	"long_tongue",		PART_LONG_TONGUE,	TRUE	},
    {	"eyestalks",		PART_EYESTALKS,		TRUE	},
    {	"tentacles",		PART_TENTACLES,		TRUE	},
    {	"fins",			PART_FINS,		TRUE	},
    {	"wings",		PART_WINGS,		TRUE	},
    {	"tail",			PART_TAIL,		TRUE	},
    {	"claws",		PART_CLAWS,		TRUE	},
    {	"fangs",		PART_FANGS,		TRUE	},
    {	"horns",		PART_HORNS,		TRUE	},
    {	"scales",		PART_SCALES,		TRUE	},
    {	"tusks",		PART_TUSKS,		TRUE	},
    {	NULL,			0,			0	}
};

const struct flag_type comm_flags[] =
{
    {	"quiet",		COMM_QUIET,		TRUE	},
    {   "deaf",			COMM_DEAF,		TRUE	},
    {   "nowiz",		COMM_NOWIZ,		TRUE	},
    {   "noauction",            COMM_NOAUCTION,         TRUE    },
    {   "noquestion",		COMM_NOQUESTION,	TRUE	},
    {   "nopray",               COMM_NOPRAY,            TRUE    },
    {   "nocabal",              COMM_NOCABAL,           TRUE    },
    {   "noclan",               COMM_NOCLAN,            TRUE    },
    {   "nonewbie",             COMM_NONEWBIE,          TRUE    },
    {   "yellsoff",             COMM_YELLSOFF,          TRUE    },
    {   "compact",		COMM_COMPACT,		TRUE	},
    {   "brief",		COMM_BRIEF,		TRUE	},
    {   "prompt",		COMM_PROMPT,		TRUE	},
    {   "telnet_ga",		COMM_TELNET_GA,		TRUE	},
    {   "show_affects",		COMM_SHOW_AFFECTS,	TRUE	},
    {   "telepath",		COMM_TELEPATH,		TRUE	},
    {   "noemote",		COMM_NOEMOTE,		FALSE	},
    {   "noyell",               COMM_NOYELL,            FALSE   },
    {   "notell",		COMM_NOTELL,		FALSE	},
    {   "nochannels",		COMM_NOCHANNELS,	FALSE	},
    {   "snoop_proof",		COMM_SNOOP_PROOF,	FALSE	},
    {   "afk",			COMM_AFK,		TRUE	},
    {	NULL,			0,			0	}
};

const struct item_type          item_table      []      =
{
    {   ITEM_LIGHT,     "light"         },
    {   ITEM_SCROLL,    "scroll"        },
    {   ITEM_WAND,      "wand"          },
    {   ITEM_STAFF,     "staff"         },
    {   ITEM_WEAPON,    "weapon"        },
    {   ITEM_TREASURE,  "treasure"      },
    {   ITEM_ARMOR,     "armor"         },
    {   ITEM_POTION,    "potion"        },
    {   ITEM_CLOTHING,  "clothing"      },
    {   ITEM_FURNITURE, "furniture"     },
    {   ITEM_TRASH,     "trash"         },
    {   ITEM_CONTAINER, "container"     },
    {   ITEM_DRINK_CON, "drink"         },
    {   ITEM_KEY,       "key"           },
    {   ITEM_FOOD,      "food"          },
    {   ITEM_MONEY,     "money"         },
    {   ITEM_BOAT,      "boat"          },
    {   ITEM_CORPSE_NPC,"npc_corpse"    },
    {   ITEM_CORPSE_PC, "pc_corpse"     },
    {   ITEM_FOUNTAIN,  "fountain"      },
    {   ITEM_PILL,      "pill"          },
    {   ITEM_MAP,       "map"           },
    {   ITEM_GEM,       "gem"           },
    {   ITEM_JEWELRY,   "jewelry"       },
    {   ITEM_CABAL,     "cabal"         },
    {   ITEM_HERB,      "herb"          },
    {   ITEM_ARTIFACT,  "artifact"      },
    {   ITEM_RELIC,     "relic"         },
    {   ITEM_THROW,     "throw"         },
    {   ITEM_INSTRUMENT,"instrument"    }, 
    {   ITEM_SOCKET,    "socket"        },
    {   ITEM_RESEARCH,  "research"      },
    {   ITEM_RANGED,    "ranged"	},
    {   ITEM_PROJECTILE,"projectile"    },
    {   0,              NULL            }
};

const   struct  sheath_type     sheath_table    []      ={
/* class        show   both      left sheath          right sheath */
  {"none",	TRUE, FALSE,	"<sheathed>          ","<sheathed>          "},
  {"thief",	FALSE, TRUE,	"<left sleeve>       ","<right sleeve>      "},
  {"ninja",	FALSE, TRUE,	"<hidden>            ","<hidden>            "},
  {"warrior",	TRUE,  TRUE,	"<sheathed on belt>  ","<sheathed on waist> "},
  {"ranger",	TRUE,  TRUE,	"<left shoulder>     ","<right shoulder>    "},
  {"berserker",	TRUE,  TRUE,	"<sheathed on back>  ","<sheathed on back>  "},
  {"blademaster",TRUE,  TRUE,	"<sheathed on back>  ","<sheathed on waist> "},
  {NULL,	TRUE, FALSE,	"",		""			     }
};


const   struct  weapon_type     weapon_table    []      =
{
  { "sword",   OBJ_VNUM_SCHOOL_SWORD,  WEAPON_SWORD,   &gsn_sword,	&gsn_sword_mastery,	&gsn_sword_expert      },
   { "mace",    OBJ_VNUM_SCHOOL_MACE,   WEAPON_MACE,    &gsn_mace,	&gsn_mace_mastery,	&gsn_mace_expert       },
   { "dagger",  OBJ_VNUM_SCHOOL_DAGGER, WEAPON_DAGGER,  &gsn_dagger,	&gsn_dagger_mastery,	&gsn_dagger_expert     },
   { "axe",     OBJ_VNUM_SCHOOL_AXE,    WEAPON_AXE,     &gsn_axe,	&gsn_axe_mastery,	&gsn_axe_expert        },
   { "staff",   OBJ_VNUM_SCHOOL_STAFF,  WEAPON_STAFF,   &gsn_staff,	&gsn_staff_mastery,	&gsn_staff_expert      },
   { "spear",   OBJ_VNUM_SCHOOL_SPEAR,  WEAPON_SPEAR,   &gsn_spear,	&gsn_spear_mastery,	&gsn_spear_expert      },
   { "flail",   OBJ_VNUM_SCHOOL_FLAIL,  WEAPON_FLAIL,   &gsn_flail,	&gsn_flail_mastery,	&gsn_flail_expert      },
   { "whip",    OBJ_VNUM_SCHOOL_WHIP,   WEAPON_WHIP,    &gsn_whip,	&gsn_whip_mastery,	&gsn_whip_expert       },
   { "polearm", OBJ_VNUM_SCHOOL_POLEARM,WEAPON_POLEARM, &gsn_polearm,	&gsn_polearm_mastery,	&gsn_polearm_expert    },
   { NULL,      0,                              0,      NULL            }
};

const   struct wiznet_type      wiznet_table    []              =
{
   {    "on",           WIZ_ON,         IM },
   {    "notes",        WIZ_NOTES,      IM },
   {    "ticks",        WIZ_TICKS,      IM },
   {    "logins",       WIZ_LOGINS,     IM },
   {    "sites",        WIZ_SITES,      L4 },
   {    "links",        WIZ_LINKS,      L7 },
   {    "newbies",      WIZ_NEWBIE,     IM },
   {    "spam",         WIZ_SPAM,       L5 },
   {    "deaths",       WIZ_DEATHS,     IM },
   {    "resets",       WIZ_RESETS,     L4 },
   {    "mobdeaths",    WIZ_MOBDEATHS,  L4 },
   {    "flags",        WIZ_FLAGS,      L5 },
   {    "penalties",    WIZ_PENALTIES,  L5 },
   {    "fighting",     WIZ_FIGHTING,   L5 },
   {    "levels",       WIZ_LEVELS,     IM },
   {    "load",         WIZ_LOAD,       L2 },
   {    "restore",      WIZ_RESTORE,    L2 },
   {    "snoops",       WIZ_SNOOPS,     L2 },
   {    "switches",     WIZ_SWITCHES,   L2 },
   {    "secure",       WIZ_SECURE,     L1 },
   {    "memory",	WIZ_MEMORY,	L1 },
   {    "save",		WIZ_SAVE,	L1 },
   {    NULL,           0,              0  }
};

/* viri: in order to make this backwards compat. with the old code
the DAM_XX constants in the table MUST correspond to the place in table
*/
const   struct flag_type      damtype_table    []    ={
  {	"none",		DAM_NONE,	FALSE		},
  {	"bash",		DAM_BASH,	TRUE		},
  {	"pierce",	DAM_PIERCE,	TRUE		},
  {	"slash",	DAM_SLASH,	TRUE		},
  {	"fire",		DAM_FIRE,	TRUE		},
  {	"cold",		DAM_COLD,	TRUE		},
  {	"lightning",	DAM_LIGHTNING,	TRUE		},
  {	"acid",		DAM_ACID,	TRUE		},
  {	"poison",	DAM_POISON,	TRUE		},
  {	"negative",	DAM_NEGATIVE,	TRUE		},
  {	"holy",		DAM_HOLY,	TRUE		},
  {	"energy",	DAM_ENERGY,	TRUE		},
  {	"mental",	DAM_MENTAL,	TRUE		},
  {	"disease",	DAM_DISEASE,	TRUE		},
  {	"drowning",	DAM_DROWNING,	TRUE		},
  {	"light",	DAM_LIGHT,	TRUE		},
  {	"other",	DAM_OTHER,	TRUE		},
  {	"harm",		DAM_HARM,	TRUE		},
  {	"charm",	DAM_CHARM,	TRUE		},
  {	"sound",	DAM_SOUND,	TRUE		},
  {	"silver",	DAM_SILVER,	TRUE		},
  {	"wood",		DAM_WOOD,	TRUE		},
  {	"iron",		DAM_IRON,	TRUE		},
  {	"mithril",	DAM_MITHRIL,	TRUE		},
  {	"maled",	DAM_MALED,	TRUE		},
  {	"internal",	DAM_INTERNAL,	TRUE		},
  {	"air",		DAM_AIR,	TRUE		},
  {	NULL,		DAM_INTERNAL,	FALSE		}
};


const   struct attack_type      attack_table    [MAX_DAMAGE_MESSAGE]    =
{
    {   "none",         "hit",          -1              },  /*  0 */
    {   "slice",        "slice",        DAM_SLASH       },      
    {   "stab",         "stab",         DAM_PIERCE      },
    {   "slash",        "slash",        DAM_SLASH       },
    {   "whip",         "whip",         DAM_SLASH       },
    {   "claw",         "claw",         DAM_SLASH       },  /*  5 */
    {   "blast",        "blast",        DAM_BASH        },
    {   "pound",        "pound",        DAM_BASH        },
    {   "crush",        "crush",        DAM_BASH        },
    {   "grep",         "grep",         DAM_SLASH       },
    {   "bite",         "bite",         DAM_PIERCE      },  /* 10 */
    {   "pierce",       "pierce",       DAM_PIERCE      },
    {   "suction",      "suction",      DAM_BASH        },
    {   "beating",      "beating",      DAM_BASH        },
    {   "digestion",    "digestion",    DAM_ACID        },
    {   "charge",       "charge",       DAM_BASH        },  /* 15 */
    {   "slap",         "slap",         DAM_BASH        },
    {   "punch",        "punch",        DAM_BASH        },
    {   "wrath",        "wrath",        DAM_HOLY        },
    {   "magic",        "magic",        DAM_ENERGY      },
    {   "divine",       "divine power", DAM_HOLY        },  /* 20 */
    {   "cleave",       "cleave",       DAM_SLASH       },
    {   "scratch",      "scratch",      DAM_PIERCE      },
    {   "peck",         "peck",         DAM_PIERCE      },
    {   "peckb",        "peck",         DAM_BASH        },
    {   "chop",         "chop",         DAM_SLASH       },  /* 25 */
    {   "sting",        "sting",        DAM_PIERCE      },
    {   "smash",        "smash",        DAM_BASH        },
    {   "shbite",       "shocking bite",DAM_LIGHTNING   },
    {   "flbite",       "flaming bite", DAM_FIRE        },
    {   "frbite",       "freezing bite", DAM_COLD       },  /* 30 */
    {   "acbite",       "acidic bite",  DAM_ACID        },
    {   "chomp",        "chomp",        DAM_PIERCE      },
    {   "drain",        "life drain",   DAM_NEGATIVE    },
    {   "thrust",       "thrust",       DAM_PIERCE      },
    {   "slime",        "slime",        DAM_ACID        },  /* 35 */
    {   "shock",        "shock",        DAM_LIGHTNING   },
    {   "thwack",       "thwack",       DAM_BASH        },
    {   "flame",        "flame",        DAM_FIRE        },
    {   "chill",        "chill",        DAM_COLD        },
    { "illusion1",  "phantasmal griffon",    DAM_MENTAL },  /* 40 */
    { "illusion2",  "illusionary spectre",   DAM_MENTAL },
    { "illusion3",  "phantom dragon",        DAM_MENTAL },
    { "illusion3a", "phantom dragon's claw", DAM_MENTAL },
    { "illusion3b", "phantom dragon's tail", DAM_MENTAL },
    { "illusion3c", "phantom dragon's breath",DAM_MENTAL },  /* 45 */
    { "hunger",         "hunger",       DAM_OTHER       },
    { "thirst",         "thirst",       DAM_OTHER       },
    { "sidestep",       "side step",    DAM_PIERCE      },
    { "tainted",	"tainted slash",DAM_ENERGY	},
    { "kiss",		"kiss of death",DAM_NEGATIVE	},  /* 50 */
    { "pain",		"pain",		DAM_NEGATIVE	},
    { "rays",		"rays",		DAM_LIGHT	},
    { "jab",		"jab",		DAM_PIERCE	},  /* 53 to 64 are monks only */
    { "backfist",	"back fist",	DAM_BASH	},//54
    { "elbow slash",	"elbow slash",	DAM_SLASH	},//55
    { "palmstrike",	"palm strike",  DAM_BASH    	},//56
    { "tigeruppercut",	"tiger claw",	DAM_BASH	},//57
    { "dragonpunch",	"dragon claw",	DAM_BASH	},//58
    { "kneethrust",	"knee thrust",	DAM_PIERCE	},//59
    { "spinkick",	"spin kick",	DAM_SLASH	},  /* 60 */
    { "jumpkick",	"jump kick",	DAM_BASH	},//61
    { "roundhouse",	"roundhouse kick",DAM_BASH	},//62
    { "lightning",	"pierce kick",  DAM_BASH	},//63
    { "whirlwind",	"whirlwind kick",DAM_SLASH	},//64
    { "hydrokinesis",   "hydrokinesis", DAM_DROWNING	}, /* 65 to 70 are psi's only */
    { "cryokinesis",    "cryokinesis",  DAM_COLD	},//66
    { "pyrokinesis",    "pyrokinesis",  DAM_FIRE	},//67
    { "electrokinesis", "electrokinesis",DAM_LIGHTNING	},//68
    { "photokinesis",   "photokinesis", DAM_LIGHT	},//69
    { "telekinesis",    "telekinesis",  DAM_ENERGY	}, /* 70 */
    { "tail",		"tail",		DAM_PIERCE	},
    { "torrent",	"torrent",	DAM_DROWNING	},
    { "typhoon",	"typhoon",	DAM_DROWNING	},
    { "drowning",	"drowning",	DAM_DROWNING	},
    { "corruption",	"corruption",	DAM_NEGATIVE	}, /* 75 */
    { "psbite",         "poisoned bite",DAM_POISON      },
    { "spray",          "spray",        DAM_DROWNING    },
    { "disease",        "disease",      DAM_DISEASE     },
    { "thorns",		"thorns",	DAM_PIERCE	},
    { "agony",		"agony",	DAM_NEGATIVE    }, /* 80 */
    { "steam",		"steam",	DAM_FIRE        },
    { "defense",	"defense",	DAM_BASH	}, // Bastions
    { "rift",		"planar rift",	DAM_HOLY	},
    { "mcharge",	"mounted charge", DAM_BASH	},
    { "arrows",		"arrows",	DAM_PIERCE	}, /* 85 */
    { "scharge",	"spear charge", DAM_PIERCE	},
    { "catapults",	"catapults",    DAM_FIRE	},
    { "mattack",	"mass attack",  DAM_SLASH	},
    { "pdart",		"poison darts", DAM_POISON	},
    { "fury",		"fury",		DAM_BASH	}, /* 90 */
    { "skdrain",	"skeletal drain", DAM_NEGATIVE  },
    { "touch",		"touch",	DAM_NEGATIVE    },
    { "explosives",	"explosive projectiles", DAM_PIERCE },
    { "bladed",		"bladed onslaught",DAM_SLASH	},
    { "frsmash",        "freezing smash", DAM_COLD	}, /* 95 */
    { "blight",		"blight",	DAM_NEGATIVE    },
    { "icefire",	"icefire",	DAM_COLD	},
    { "frarrow",        "icy tip",      DAM_COLD        },
    { "fiarrow",        "fiery tip",    DAM_FIRE        },
    { "sharrow",        "charged tip", DAM_LIGHTNING    },/* 100 */
    { "acarrow",        "acid tip",	DAM_ACID	},
    { "blarrow",        "blunt tip",    DAM_BASH	},
    { "brarrow",        "barbed tip",   DAM_PIERCE	},
    { "airblast",       "air blast",    DAM_AIR		},
    { "tentacle",       "tentacle",     DAM_BASH	},/*105*/
    { "gaze",           "gaze",		DAM_MENTAL	},
    { "faith",          "faith",	DAM_ENERGY	},
    { "disrupt",        "disruption",	DAM_ENERGY	},
    {   NULL,           NULL,           0               }
};

const   struct  race_type       race_table      [MAX_RACE]              =
{
/*
    {
	name,           pc_race?,	act2 bits,
	act bits,       aff_by bits,    off bits,
	imm,            res,            vuln,
	form,           parts,		
	grn
    },
*/
    { 
	"unique",               FALSE,  0,
	0,              0,              0,
	0,              0,              0,
	0,        	0,		
	NULL,
    },
    { 
	"human",                TRUE, 	0,
	0,              0,              0,
	0,              0,              0,
	FORM_HUMANOID,
        PART_HUMANOID,
	
	NULL,
    },
    {
	"elf",                  TRUE,	0,
	0,              AFF_SNEAK|AFF_INFRARED,   0,
	0,              RES_CHARM,    VULN_IRON,
	FORM_HUMANOID,
        PART_HUMANOID,
	
	NULL,
    },
    {
	"drow",                 TRUE,	0,
	0,              AFF_SNEAK|AFF_INFRARED,   0,
	0,              RES_CHARM,    VULN_SILVER|VULN_MITHRIL,
	FORM_HUMANOID,
        PART_HUMANOID,
	
	NULL,
    },
    {
	"dwarf",                TRUE,	0,
	0,              AFF_INFRARED,   0,
	0,              RES_POISON|RES_MAGIC, VULN_DROWNING,
	FORM_HUMANOID,
        PART_HUMANOID,
	
	NULL,
    },
    {
	"duergar",              TRUE,	0,
	0,              AFF_INFRARED,   0,
	0,              RES_POISON|RES_MAGIC, VULN_DROWNING,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K,
	
	NULL,
    },
    {
	"halfling",             TRUE,	0,
	0,              0,   0,
	0,              RES_MAGIC,              0,
	FORM_HUMANOID,
        PART_HUMANOID,
	
	NULL,
    },
    {
	"gnome",                TRUE,	0,
	0,              AFF_INFRARED,   0,
	0,              0,              VULN_CHARM | VULN_MENTAL,
	FORM_HUMANOID,
        PART_HUMANOID,
	
	NULL,
    },
    {
	"avian",                TRUE,	0,
	0,              AFF_FLYING,        0,
        0,              0,              VULN_AIR,
	FORM_HUMANOID | FORM_BIRD,
        PART_HUMANOID | PART_WINGS,
	
	NULL,
    },
    {
	"fire",                 TRUE,	0,
	0,              0,              0,
	0,              RES_FIRE|RES_WEAPON,      VULN_MENTAL|VULN_COLD,
	FORM_HUMANOID,
        PART_HUMANOID,

	&grn_fire,
    },
    {
	"stone",                TRUE,	0,
	0,              0,              0,
	0,              RES_WEAPON,     VULN_MENTAL,
	FORM_HUMANOID,
        PART_HUMANOID,

	&grn_stone,
    },
    {
	"storm",                TRUE,	0,
	0,              0,              0,
	0,              RES_LIGHTNING|RES_WEAPON,      VULN_MENTAL|VULN_WOOD,
	FORM_HUMANOID,
        PART_HUMANOID,

	NULL,
    },
    { 
	"undead",                TRUE, 	0,
	ACT_UNDEAD,              AFF_INFRARED,              0,
	IMM_DISEASE|IMM_POISON,  RES_COLD,             VULN_HOLY,
	(FORM_HUMANOID|FORM_COLD_BLOOD| FORM_UNDEAD|FORM_INSTANT_DECAY) & !FORM_EDIBLE,
        PART_HUMANOID,

	&grn_undead,
    },
    { 
	"illithid",                TRUE, 	  0,
	0,             AFF_INFRARED,              0,
	0,	       RES_MENTAL|RES_LIGHTNING,  VULN_LIGHT,
	FORM_HUMANOID,
        PART_HUMANOID | PART_TENTACLES,

	&grn_illithid,
    }, 
    {
	"minotaur",                  TRUE,	0,
	0,              0,              0,
	0,              0,              0,
	FORM_HUMANOID,
        PART_HUMANOID | PART_HORNS | PART_TUSKS,

	&grn_minotaur,
    },
    {
	"ethro",                 FALSE,	0,
	0,              AFF_INFRARED,              0,
	0,              0,              0,

	FORM_HUMANOID,
        PART_HUMANOID,

	NULL,
    },
    {
	"slith",                  TRUE,	0,
	0,              AFF_INFRARED,              0,
	0,              0,              0,
	(FORM_HUMANOID|FORM_REPTILE) & !FORM_MAMMAL,
        PART_HUMANOID|PART_TAIL|PART_LONG_TONGUE|FORM_COLD_BLOOD|PART_SCALES,

	NULL,
    },
    {
	"troll",                  FALSE,0,
	0,              0,              0,
	0,              0,              0,
	FORM_HUMANOID | FORM_POISON,
	PART_HUMANOID,

	NULL,
    },
    {
	"feral",                  TRUE,	0,
	0,              0,              0,
	0,              0,              VULN_FIRE,
	FORM_HUMANOID | FORM_ANIMAL,
	PART_HUMANOID | PART_TAIL | PART_CLAWS,        
	&grn_feral,
    },
    {
	"werebeast",                  TRUE,	0,
	0,              0,              0,
	0,              0,              VULN_SILVER,
	FORM_HUMANOID,
        PART_HUMANOID,	

	&grn_werebeast,
    },
    {
	"tiger",                  FALSE,0,
	0,              0,              0,
	0,              0,              0,
	FORM_HUMANOID,
        PART_HUMANOID,	

	NULL,
    },
    {
	"wolf",                  FALSE,	0,
	0,              0,              0,
	0,              0,              0,
	FORM_HUMANOID,
        PART_HUMANOID,	

	NULL,
    },
    {
	"boar",                  FALSE,	0,
	0,              0,              0,
	0,              0,              0,
	FORM_HUMANOID,
        PART_HUMANOID,	

	NULL,
    },
    {
	"ocelot",                  TRUE,	0,
	0,              0,              0,
	0,              0,              0,
	FORM_HUMANOID,
        PART_HUMANOID,	

	NULL,
    },
    {
	"fox",                  TRUE,	0,
	0,              0,              0,
	0,              0,              0,
	FORM_HUMANOID,
        PART_HUMANOID,	

	NULL,
    },
    {
	"falcon",                  TRUE,	0,
	0,              AFF_FLYING,              0,
	0,              0,              0,
	FORM_HUMANOID,
        PART_HUMANOID,       

	NULL,
    },
    {
	"half-elf",                 TRUE,	 0,
	0,              AFF_INFRARED,   0,
	0,              RES_CHARM,      0,
	FORM_HUMANOID,
        PART_HUMANOID,	

	&grn_half_elf,
    },
    { 
      "faerie",               TRUE ,  0,
      0,              AFF_FLYING | AFF_SNEAK,     0,
      0,              0,              0,
      FORM_HUMANOID,
      PART_HUMANOID,      

      &grn_faerie,
    },

    { 
      "demon",               TRUE ,   0,
      0,              AFF_INFRARED | AFF_FLYING | AFF_DETECT_INVIS,   0,
      0,              0,              VULN_SUMMON,
      FORM_HUMANOID,
      PART_HUMANOID,      

      &grn_demon,
    },

    { 
      "ogre",                TRUE ,   0,
      0,              0,              0,
      0,              0,	      VULN_MAGIC,
      FORM_HUMANOID,
      PART_HUMANOID,      

      &grn_ogre,
    },
    { 
      "vampire",                TRUE, 	0,
      ACT_UNDEAD,              AFF_INFRARED,              0,
      IMM_DISEASE|IMM_POISON,  RES_COLD,             VULN_HOLY|VULN_LIGHT,
      FORM_HUMANOID,
      PART_HUMANOID,      
      
      &grn_vampire,
    },
    { 
      "lich",                TRUE, 	0,
      ACT_UNDEAD,              AFF_INFRARED,              0,
      IMM_DISEASE|IMM_POISON,  RES_COLD,             VULN_HOLY,
      FORM_HUMANOID,
      PART_HUMANOID,       
      
      NULL,
    }, 
    { 
	"goblin",                TRUE, 	0,
	0,              0,              0,
	0,              0,              0,
	FORM_HUMANOID,
        PART_HUMANOID,
	
	NULL,
    },
    { 
	"quickling",                TRUE, 	0,
	0,              0,              0,
	0,              0,              0,
	FORM_HUMANOID,
        PART_HUMANOID,
	
	NULL,
    },
    {
      "giant",                FALSE,	0,
      0,              0,              0,
      0,              RES_WEAPON,     VULN_MENTAL,
      FORM_HUMANOID,
      PART_HUMANOID,	
      
      NULL,
    },
    {
      "bat",                  FALSE,	0,
      0,              AFF_FLYING,     OFF_DODGE|OFF_FAST,
      0,              0,              VULN_LIGHT,
      FORM_EDIBLE|FORM_ANIMAL|FORM_BIRD,
      PART_ANIMAL|PART_WINGS,
      NULL,
    },
    {
	"bear",                 FALSE,	0,
	0,              0,              OFF_CRUSH|OFF_DISARM|OFF_BERSERK,
	0,              RES_BASH|RES_COLD,      0,
	FORM_EDIBLE|FORM_ANIMAL,
	PART_ANIMAL,

	NULL,
    },
    {
	"cat",                  FALSE,	0,
        0,              0,              OFF_FAST|OFF_DODGE,
	0,              0,              0,
	A|G|V,          A|C|D|E|F|H|J|K|Q|U|V,

	NULL,
    },
    {
	"centipede",            FALSE,	0,
        0,              0,              0,
	0,              RES_PIERCE|RES_COLD,    VULN_BASH,
	A|B|G|cc,                A|C|F|K   ,

	NULL,
    },
    {
	"dog",                  FALSE,	0,
	0,              0,              OFF_FAST,
	0,              0,              0,
	A|G|V,          A|C|D|E|F|H|J|K|U|V,

	NULL,
    },
    {
	"doll",                 FALSE,	0,
	0,              0,              0,
	IMM_CHARM|IMM_POISON|IMM_DISEASE,		  RES_BASH|RES_LIGHT,
	VULN_SLASH|VULN_FIRE|VULN_ACID|VULN_LIGHTNING|VULN_ENERGY,
	A|E|J|M|cc,       A|B|C|G|H|I|J|K,

	NULL,
    },
    {   "dragon",               FALSE, 0,
	0,                      AFF_INFRARED|AFF_FLYING,        0,
	0,                      RES_FIRE|RES_BASH|RES_CHARM, 
	VULN_PIERCE|VULN_COLD,
	A|H|Z,          A|C|D|E|F|G|H|I|J|K|L|P|Q|U|V|X,

	NULL,
    },
    {
	"fido",                 FALSE,	0,
	0,              0,              OFF_DODGE|ASSIST_RACE,
	0,              0,                      VULN_MAGIC,
	A|B|G|V,        A|C|D|E|F|H|J|K|Q|V,

	NULL,
    },   
    {
	"goblin",               FALSE,	0,
	0,              AFF_INFRARED,   0,
	0,              RES_DISEASE,    VULN_MAGIC,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K,

	NULL,
    },
    {
	"hobgoblin",            FALSE,	0,
	0,              AFF_INFRARED,   0,
	0,              RES_DISEASE|RES_POISON, 0,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y,

	NULL,
    },
    {
	"kobold",               FALSE,	0,
	0,              AFF_INFRARED,   0,
	0,              RES_POISON,     VULN_MAGIC,
	A|B|H|M|V,      A|B|C|D|E|F|G|H|I|J|K|Q,

	NULL,
    },
    {
	"lizard",               FALSE,	0,
	0,              0,              0,
	0,              RES_POISON,     VULN_COLD,
	A|G|X|cc,       A|C|D|E|F|H|K|L|Q,

	NULL,
    },
    {
	"modron",               FALSE,	0,
	0,              AFF_INFRARED,           ASSIST_RACE|ASSIST_ALIGN,
	IMM_CHARM|IMM_DISEASE,
			RES_FIRE|RES_COLD|RES_ACID,     0,
	H,              A|B|C|G|H|J|K,

	NULL,
    },
    {
	"orc",                  FALSE,	0,
	0,              AFF_INFRARED,   0,
	0,              RES_DISEASE,    VULN_LIGHT,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K,

	NULL,
    },
    {
	"ogre",                  FALSE,	0,
	0,              AFF_INFRARED,   0,
	0,              RES_DISEASE,    VULN_LIGHT,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K,

	NULL,
    },
    {
	"pig",                  FALSE,	0,
	0,              0,              0,
	0,              0,              0,
	A|G|V,          A|C|D|E|F|H|J|K,

	NULL,
    },  
    {
	"rabbit",               FALSE,	0,
	0,              0,              OFF_DODGE|OFF_FAST,
	0,              0,              0,
	A|G|V,          A|C|D|E|F|H|J|K,

	NULL,
    },
    {
	"school monster",       FALSE,	0,
	ACT_NOALIGN,            0,              0,
	IMM_CHARM|IMM_SUMMON,   0,              VULN_MAGIC,
	A|M|V,          A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|U|V|W|X|Y,

	NULL,
    },  
    {
	"snake",                FALSE,	0,
	0,              0,              0,
	0,              RES_POISON,     VULN_COLD,
	A|G|X|Y|cc,     A|D|E|F|K|L|Q|V|X,

	NULL,
    },
    {
	"song bird",            FALSE,	0,
	0,              AFF_FLYING,             OFF_FAST|OFF_DODGE,
	0,              0,              0,
	A|G|W,          A|C|D|E|F|H|K|P,

	NULL,
    },
    {
	"water fowl",           FALSE,	0,
        0,              AFF_FLYING,             0,
	0,              RES_DROWNING,           0,
	A|G|W,          A|C|D|E|F|H|K|P,

	NULL,
    },          
    {
	"wyvern",               FALSE,	0,
	0,              AFF_FLYING|AFF_DETECT_INVIS|AFF_DETECT_HIDDEN,
	OFF_BASH|OFF_FAST|OFF_DODGE,
	IMM_POISON,     0,      VULN_LIGHT,
	A|B|G|Z,                A|C|D|E|F|H|J|K|P|Q|V|X,

	NULL,
    },
    { 
	"horse",               FALSE, 	0,
	0,              0,              0,
	0,              0,              0,
	A|G|V,          A|B|C|D|E|F|H|J|K,

	NULL,
    },
    { 
	"horned beast",               FALSE, 	0,
	0,              0,              0,
	0,              0,              0,
	A|G|V,          A|B|C|D|E|F|H|J|K|W,

	NULL,
    },
    { 
	"insect",               FALSE, 	0,
	0,              0,              0,
	0,              0,              0,
	A|G|O|cc,          A|C|D|E|F|H|K,

	NULL,
    },
    { 
	"fly",               FALSE, 	0,
	0,              T,              0,
	0,              0,              0,
	A|G|O|cc,          A|C|D|E|F|H|P,

	NULL,
    },
    { 
	"centaur",               FALSE, 0,
	0,              0,              0,
	0,              0,              0,
	A|H|N|V,          A|C|D|E|F|H|J|K|Q,

	NULL,
    },
    { 
	"deer",               FALSE, 	0,
	0,              0,              0,
	0,              0,              0,
	A|G|V,          A|C|D|E|F|J|K|Q,

	NULL,
    },
    { 
	"golem",               FALSE, 	0,
	0,              0,              0,
	0,              0,              0,
	H|J|cc,          A|B|C|H|I|J|K,

	NULL,
    },
    {
	"treant",                FALSE,	0,
	0,              0,              0,
	0,              0,		VULN_FIRE,
	A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K,

	NULL,
    },
    { 
	"cold",               FALSE, 	0,
	0,              0,              0,
	0,              0,              0,
	C|J|cc,          0,

	NULL,
    },

    {
      NULL, 0, 0, 0, 0, 0, 0, 0
    }
};

char *  const                   title_table     [MAX_CLASS][MAX_LEVEL+1][2] =
{
    {
        { "Warrior",                    "Warrior"                       },


	{ "Swordpupil",			"Swordpupil"			},
	{ "Recruit",			"Recruit"			},
	{ "Sentry",			"Sentress"			},
	{ "Fighter",			"Fighter"			},
	{ "Soldier",			"Soldier"			},

	{ "Warrior",			"Warrior"			},
	{ "Veteran",			"Veteran"			},
	{ "Swordsman",			"Swordswoman"			},
	{ "Fencer",			"Fenceress"			},
	{ "Combatant",			"Combatess"			},

	{ "Hero",			"Heroine"			},
	{ "Myrmidon",			"Myrmidon"			},
	{ "Swashbuckler",		"Swashbuckleress"		},
	{ "Mercenary",			"Mercenaress"			},
	{ "Swordmaster",		"Swordmistress"			},

	{ "Lieutenant",			"Lieutenant"			},
	{ "Champion",			"Lady Champion"			},
	{ "Dragoon",			"Lady Dragoon"			},
	{ "Cavalier",			"Lady Cavalier"			},
	{ "Knight",			"Lady Knight"			},

	{ "Grand Knight",		"Grand Knight"			},
	{ "Master Knight",		"Master Knight"			},
	{ "Gladiator",			"Gladiator"			},
	{ "Master Gladiator",		"Master Gladiator"		},
	{ "Demon Slayer",		"Demon Slayer"			},

	{ "Greater Demon Slayer",	"Greater Demon Slayer"		},
	{ "Dragon Slayer",		"Dragon Slayer"			},
	{ "Greater Dragon Slayer",	"Greater Dragon Slayer"		},
	{ "Underlord",			"Underlord"			},
	{ "Overlord",			"Overlord"			},

	{ "Baron of Thunder",		"Baroness of Thunder"		},
	{ "Baron of Storms",		"Baroness of Storms"		},
	{ "Baron of Tornadoes",		"Baroness of Tornadoes"		},
	{ "Baron of Hurricanes",	"Baroness of Hurricanes"	},
	{ "Baron of Meteors",		"Baroness of Meteors"		},

	{ "Baron of Fire",  "Baroness of Fire"},
	{ "Baron of Ice",  "Baroness of Ice"},
	{ "Baron of Lightning",  "Baroness of Lightning"},
	{ "Master of the Elements",  "Mistress of the Elements"},
	{ "Master of Copper",  "Mistress of Copper"},
	
	{ "Master of Bronze",  "Mistress of Bronze"},
	{ "Master of Brass",  "Mistress of Brass"},
	{ "Master of Iron",  "Mistress of Iron"},
	{ "Master of Steel",  "Mistress of Steel"},
	{ "Master of Mithril",  "Mistress of Mithril"},
	
	{ "Master of Adamantite",  "Mistress of Adamantite"},
	{ "Captain",  "Captain"},
	{ "General",  "General"},
	{ "Field Marshall",  "Field Marshall"},
	{ "Master of War",  "Mistress of War"},
	
	{ "Knight Hero",		"Knight Heroine"		},
	{ "Avatar of War",		"Avatar of War"			},
	{ "Angel of War",		"Angel of War"			},
	{ "Demigod of War",		"Demigoddess of War"		},
	{ "Immortal Warlord",		"Immortal Warlord"		},
	{ "God of War",			"God of War"			},
	{ "Deity of War",		"Deity of War"			},
	{ "Supreme Master of War",	"Supreme Mistress of War"	},
        { "Creator",                    "Creator"                       },
	{ "Implementor",		"Implementress"			}
    },

    {
        { "Berserker",                  "Berserker"                     },

	{ "Trainee",                    "Trainee"                       },
	{ "Foul Tempered",              "Foul Tempered"                 },
	{ "Angry",                      "Angry"                         },
	{ "Fighter Bully",              "Fighter Bully"                 },
	{ "Lasher",                     "Lasher"                        },

	{ "Beater",                     "Beater"                        },
	{ "Crusher",                    "Crusher"                       },
	{ "Brawler",                  "Brawler"                     },
	{ "Deviant",			"Deviant"                   },
	{ "Outcast",                    "Outcast"                       },

	{ "Violent",                      "Violent"                      },
	{ "Provoked",               "Provoked"                  },
	{ "Hostile",                   "Hostile"                 },
	{ "Wrathful",                      "Wrathful"                     },
	{ "Infuriated",                 "Infuriated"                  },

	{ "Extreme",                    "Extreme"                    },
	{ "Uncontrolled",                "Uncontrolled"                 },
	{ "Dreaded",                "Dreaded"                   },
	{ "Monstrosity",                    "Monstrosity"                  },
	{ "Berserker",                  "Lady Berserker"                },

	{ "Greater Berserker",         "Greater Berserker"                  },
	{ "Grand Berserker",         "Grand Berserker"                  },
	{ "Master Berserker",              "Master Berserker"                 },
	{ "Greater Destroyer",              "Greater Destroyer"                 },
	{ "Grand Destroyer",              "Grand Destroyer"                 },

	{ "Master Destroyer",              "Master Destroyer"                 },
	{ "Destructor",               "Destructor"                  },
	{ "Greater Destructor",       "Greater Destructor"          },
	{ "Grand Destructor",              "Grand Destructor"                 },
	{ "Master Destructor",      "Master Destructor"         },

	{ "Baron of Destruction",    "Baroness of Destruction"           },
	{ "Baron of Devastation",          "Baroness of Devastation"       },
	{ "Baron of Annihilation",         "Baroness of Annihilation"      },
	{ "Baron of Wrath",        "Baroness of Wrath"        },
	{ "Baron of Power",        "Baroness of Power"        },

	{ "Baron of Rage",           "Baroness of Rage"           },
	{ "Baron of Fury",           "Baroness of Fury"           },
	{ "Master of Destruction",      "Mistress of Destruction"         },
	{ "Master of Devastation",      "Mistress of Devastation"         },
	{ "Master of Annihilation",    "Mistress of Annihilation"      },

	{ "Master of Wrath",             "Mistress of Wrath"                },
	{ "Master of Power",             "Mistress of Power"                },
	{ "Master of Rage",             "Mistress of Rage"                },
	{ "Master of Fury",             "Mistress of Fury"                },
	{ "Uncontrollable",             "Uncontrollable"                },

	{ "Indominable",             "Indominable"                },
	{ "Conqueror",             "Conqueror"                },
	{ "Titan",             "Titan"                },
	{ "Berserker Lord",             "Berserker Lord"                },
	{ "Knight of Fury",             "Knight of Fury"                },

	{ "Berserker Hero",                "Berserker Heroine"                },
	{ "Avatar of Fury",              "Avatar of Fury"                 },
	{ "Angel of Fury",               "Angel of Fury"                  },
	{ "Demigod of Fury",             "Demigoddess of Fury"            },
	{ "Immortal Berserker",           "Immortal Berserker"              },

	{ "Berserker God",                 "Berserker Goddess"              },
	{ "Berserker Deity",               "Berserker Deity"                  },
	{ "Supreme Berserker",      "Supreme Berserker"       },
	{ "Creator",                    "Creator"                       },
	{ "Implementor",                "Implementress"                 }
    },

    {
        { "Ranger",                     "Ranger"                        },

      { "Forest Pupil",  "Forest Pupil"},
      { "Forest Recruit",  "Forest Recruit"},
      { "Forester",  "Forester"},
      { "Fletcher",  "Fletcher"},
      { "Bowyer",  "Bowyer"},

      { "Trapper",  "Trapper"},
      { "Hunter",  "Huntress"},
      { "Tracker",  "Tracker"},
      { "Seeker",  "Seeker"},
      { "Scout",  "Scout"},

      { "Master Scout",  "Mistress Scout"},
      { "Green Man",  "Green Woman"},
      { "Archer",  "Archer"},
      { "Apprentice Ranger",  "Apprentice Ranger"},
      { "Woodsman",  "Woodswoman"},

      { "Master Woodsman",  "Mistress Woodswoman"},
      { "Ranger Initiate",  "Ranger Initiate"},
      { "Ranger Candidate",  "Ranger Candidate"},
      { "Ranger Squire",  "Ranger Squire"},
      { "Ranger",  "Ranger"},

      { "Ranger of the First Circle",  "Ranger of the First Circle"},
      { "Ranger of the Second Circle",  "Ranger of the Second Circle"},
      { "Ranger of the Third Circle",  "Ranger of the Third Circle"},
      { "Ranger Captain",  "Ranger Captain"},
      { "Warder",  "Warder"},

      { "Warder Captain",  "Warder Captain"},
      { "Warder General",  "Warder General"},
      { "Master of Warders",  "Mistress of Warders"},
      { "Knight of the Forest",  "Lady of the Forest"},
      { "Sword of the Forest",  "Sword of the Forest"},

      { "Guardian of the Forest",  "Guardian of the Forest"},
      { "Lord of the Forest",  "Queen of the Forest"},
      { "Overlord of the Forest",  "Overlord of the Forest"},
      { "Baron of Forests",  "Baroness of Forests"},
      { "Keeper",  "Keeper"},

      { "Huntsman",  "Huntswoman"},
      { "Caller of the Pack",  "Caller of the Pack"},
      { "Master of the Pack",  "Mistress of the Pack"},
      { "Ranger Knight Initiate",  "Ranger Lady Initiate"},
      { "Ranger Knight",  "Ranger Contessa"},

      { "Ranger Lord",  "Ranger Lady"},
      { "Ranger Baron", "Ranger Baroness"},
      { "Ranger Prince",  "Ranger Princess"},
      { "Ranger King",  "Ranger Queen"},
      { "Warder of the Glade",  "Warder of the Glade"},

      { "Keeper of the Glade",  "Keeper of the Glade"},
      { "Knight of the Glade",  "Lady of the Glade"},
      {"Master of the Glade",  "Mistress of the Glade"},
      { "King of the Glade",  "Queen of the Glade"},
        { "Vengeance of the Forest",    "Vengeance of the Forest"       },

      { "Ranger Hero",  "Ranger Heroine"},
	{ "Ranger Avatar",	"Ranger Avatar"		},
	{ "Ranger Angel",		"Ranger Angel"		},
	{ "Ranger Demigod",		"Ranger Demigoddess"		},
	{ "Immortal Ranger",	"Immortal Ranger"		},

	{ "Ranger God",		"Ranger God"			},
	{ "Ranger Deity",		"Ranger Deity" 		},
	{ "Supreme Ranger",	"Supreme Ranger"	},
        { "Creator",                    "Creator"                       },
	{ "Implementor",		"Implementress"			}

    },

    {
        { "Paladin",                    "Paladin"                       },

      { "Paladin Pupil",  "Paladin Pupil"},
      { "Scullery Man",  "Scullery Maid"},
      { "Squire Candidate",  "Squire Candidate"},
      { "Shield Bearer",  "Shield Bearer"},
      { "Sword Bearer",  "Sword Bearer"},

      { "Bow Bearer",  "Bow Bearer"},
      { "Standard Bearer",  "Standard Bearer"},
      { "Horseman",  "Horsewoman"},
      { "Squire Initiate",  "Squire Initiate"},
      { "Squire",  "Squire"},

      { "Footman",  "Footwoman"},
      { "Pikeman",  "Pikewoman"},
      { "Bowman",  "Bowwoman"},
      { "Swordsman",  "Swordswoman"},
      { "Honorable",  "Honorable"},

      { "Noble",  "Noble"},
      { "Trustworthy",  "Trustworthy"},
      { "Truthful",  "Truthful"},
      { "Chivalrous",  "Chivalrous"},
      { "Paladin",  "Paladin"},

      { "Questor",  "Questor"},
      { "Cavalier",  "Cavalier"},
      { "Champion",  "Champion"},
      { "Knight of Renown",  "Knight of Renown"},
      { "Paladin Knight",  "Paladin Lady"},

      { "Templar Initiate",  "Templar Initiate"},
      { "Priest-Knight",  "Priestess-Knight"},
      { "Knight of the Cross",  "Lady of the Cross"},
      { "Champion of the Cross",  "Champion of the Cross"},
      { "Knight Templar",  "Lady Templar"},

      { "Templar",  "Templar"},
      { "Templar Footman",  "Templar Footwoman"},
      { "Templar Lieutenant",  "Templar Lieutenant"},
      { "Templar Captain",  "Templar Captain"},
      { "Templar Colonel",  "Templar Colonel"},

      { "Templar General",  "Templar General"},
      { "Templar Field Marshall",  "Templar Field Marshall"},
      { "Healer-Knight",  "Healer-Lady"},
      { "Avenger",  "Avenger"},
      { "Crusader",  "Crusader"},

      { "Leader of the Crusades",  "Leader of the Crusades"},
      { "Champion of the Crusades",  "Champion of the Crusades"},
      { "Champion of the Templars",  "Champion of the Templars"},
      { "Priest of the Templars",  "Priestess of the Templars"},
      { "High Priest of the Templars",  "High Priestess of Templars"},

      { "Lord of the Templars",  "Lady of the Templars"},
      { "Hammer of Heretics",  "Hammer of Heretics"},
      { "Slayer of Infidels",  "Slayer of Infidels"},
      { "Pious",  "Pious"},
      { "Holy Knight",  "Holy Lady"},

      { "Paladin Hero",  "Paladin Heroine"},
      { "Avatar of Honor",		"Avatar of Honor"		},
      { "Angel of Honor",		"Angel of Honor"		},
      { "Demigod of Honor",		"Demigoddess of Honor"		},
      { "Immortal Knight",		"Immortal Knight"		},

      { "God of Honor",		"God of Honor"			},
      { "Deity of Honor",		"Deity of Honor"		},
      { "Supreme Master of Honor",	"Supreme Mistress of Honor"	},
      { "Creator",                    "Creator"                       },
      { "Implementor",		"Implementress"			}
    },

    {
        { "Dark-Knight",                "Dark-Knight"                   },

      { "Scum",  "Scum"},
      { "Bully",  "Bully"},
      { "Thug",  "Moll"},
      { "Brute",  "Bitch"},
      { "Ruffian",  "Butch"},

      { "Pillager",  "Pillager"},
      { "Destroyer",  "Destroyer"},
      { "Burner",  "Burner"},
      { "Hired Killer",  "Hired Killer"},
      { "Brigand",  "Brigand"},

      { "Mercenary",  "Mercenary"},
      { "Black Sword",  "Black Sword"},
      { "Crimson Sword",  "Crimson Sword"},
      { "Sneaky",  "Sneaky"},
      { "Cruel",  "Cruel"},

      { "Stealer",  "Stealer"},
      { "Infamous",  "Infamous"},
      { "Hated",  "Hated"},
      { "Complete Bastard",  "Complete Bitch"},
      { "Dark Knight",  "Dark Knight"},

      { "Evil Fighter",  "Evil Fighter"},
      { "Rogue Knight",  "Rogue Lady"},
      { "Evil Champion",  "Evil Champion"},
      { "Dishonorable",  "Dishonorable"},
      { "Black Knight",  "Black Lady"},

      { "Crimson Knight",  "Crimson Lady"},
      { "Knight of Brimstone",  "Lady of Brimstone"},
      { "Knight of the Inverted Cross",  "Lady of the Inverted Cross"},
      { "Knight of Pain",  "Lady of Pain"},
      { "Knight of Legion",  "Lady of Legion"},

      { "Footman of Legion",  "Footwoman of Legion"},
      { "Cavalier of Legion",  "Cavalier of Legion"},
      { "Captain of Legion",  "Captain of Legion"},
      { "General of Legion",  "General of Legion"},
      { "Field Marshall of Legion",  "Field Marshall of Legion"},

      { "Knight of the Apocalypse",  "Lady of the Apocalypse"},
      { "LightSlayer",  "LightSlayer"},
      { "Invoker of Suffering",  "Invoker of Suffering"},
      { "Archfiend",  "Archfiend"},
      { "Evil Knight",  "Evil Lady"},

      { "Evil Lord",  "Evil Baroness"},
      { "Evil King",  "Evil Queen"},
      { "Herald of War",  "Herald of War"},
      { "Spreader of Pestilence",  "Spreader of Pestilence"},
      { "Bringer of Famine",  "Bringer of Famine"},

      { "Harbinger of Death",  "Harbinger of Death"},
      { "Unholy Bishop",  "Unholy Priestess"},
      { "Slayer",  "Slayer"},
      { "Evil Incarnate",  "Evil Incarnate"},
      { "Unholy Knight",  "Unholy Lady"},

      { "Anti-Hero",  "Anti-Heroine"},
      { "Avatar of Evil",		"Avatar of Evil"		},
      { "Angel of Evil",		"Angel of Evil"		},
      { "Demigod of Evil",		"Demigoddess of Evil"		},
      { "Immortal Anti-Knight",	"Immortal Anti-Knight"		},
      { "God of Evil",		"God of Evil"			},
      { "Deity of Evil",		"Deity of Evil"		},
      { "Supreme Master of Evil",	"Supreme Mistress of Evil"	},
      { "Creator",                    "Creator"                       },
      { "Implementor",		"Implementress"			}
    },

    {
        { "Thief",                      "Thief"                         },

	{ "Pilferer",			"Pilferess"			},
	{ "Footpad",			"Footpad"			},
	{ "Filcher",			"Filcheress"			},
	{ "Pick-Pocket",		"Pick-Pocket"			},
	{ "Sneak",			"Sneak"				},

	{ "Pincher",			"Pincheress"			},
	{ "Cut-Purse",			"Cut-Purse"			},
	{ "Snatcher",			"Snatcheress"			},
	{ "Sharper",			"Sharpress"			},
	{ "Rogue",			"Rogue"				},

	{ "Robber",			"Robber"			},
	{ "Magsman",			"Magswoman"			},
	{ "Highwayman",			"Highwaywoman"			},
	{ "Burglar",			"Burglaress"			},
	{ "Thief",			"Thief"				},

	{ "Knifer",			"Knifer"			},
	{ "Quick-Blade",		"Quick-Blade"			},
	{ "Killer",			"Murderess"			},
	{ "Brigand",			"Brigand"			},
	{ "Cut-Throat",			"Cut-Throat"			},

	{ "Spy",			"Spy"				},
	{ "Grand Spy",			"Grand Spy"			},
	{ "Master Spy",			"Master Spy"			},
	{ "Assassin",			"Assassin"			},
	{ "Greater Assassin",		"Greater Assassin"		},

	{ "Master of Vision",		"Mistress of Vision"		},
	{ "Master of Hearing",		"Mistress of Hearing"		},
	{ "Master of Smell",		"Mistress of Smell"		},
	{ "Master of Taste",		"Mistress of Taste"		},
	{ "Master of Touch",		"Mistress of Touch"		},

	{ "Crime Lord",			"Crime Mistress"		},
	{ "Infamous Crime Lord",	"Infamous Crime Mistress"	},
	{ "Greater Crime Lord",		"Greater Crime Mistress"	},
	{ "Master Crime Lord",		"Master Crime Mistress"		},
	{ "Godfather",			"Godmother"			},

	{ "Crime Boss",  "Crime Boss"},
	{ "Kingpin",  "Kingpin"},
	{ "Lord of the Underworld",  "Lady of the Underworld"},
	{ "Shadow",  "Shadow"},
	{ "Master of the Night",  "Mistress of the Night"},

	{ "Master of Silence",  "Mistress of Silence"},
	{ "Master of Guile",  "Mistress of Guile"},
	{ "Master of the Blade",  "Mistress of the Blade"},
	{ "Master of Poison",  "Mistress of Poison"},
	{ "Master of Stealth",  "Mistress of Stealth"},

	{ "Master Thief",  "Master Thief"},
	{ "Master Assassin",  "Master Assassin"},
	{ "Unseen",  "Unseen"},
	{ "Grand Master of Thieves",  "Grand Mistress of Thieves"},
	{ "Grand Master of Assassins",  "Grand Mistress of Assassins"},
	
	{ "Assassin Hero",		"Assassin Heroine"		},
	{ "Avatar of Death",		"Avatar of Death",		},
	{ "Angel of Death",		"Angel of Death"		},
	{ "Demigod of Assassins",	"Demigoddess of Assassins"	},
	{ "Immortal Assasin",		"Immortal Assassin"		},
	{ "God of Assassins",		"God of Assassins",		},
	{ "Deity of Assassins",		"Deity of Assassins"		},
	{ "Supreme Master",		"Supreme Mistress"		},
        { "Creator",                    "Creator"                       },
	{ "Implementor",		"Implementress"			}
    },

    {
        { "Assassin",                   "Assassin"                      },

      { "Uke",  "Uke"},
      { "Tori",  "Tori"},
      { "White Belt",  "White Belt"},
      { "Yellow belt",  "Yellow Belt"},
      { "Green Belt",  "Green Belt"},

      { "3rd Brown",  "3rd Brown"},
      { "2nd Brown",  "2nd Brown"},
      { "1st Brown",  "1st Brown"},
      { "3rd Black",  "3rd Black"},
      { "2nd Black",  "2nd Black"},

      { "Okuiri",  "Okuiri"},
      { "Shuto",  "Shuto"},
      { "Uraken",  "Uraken"},
      { "Shotei",  "Shotei"},
      { "Tateken",  "Tateken"},

      { "Seiken",  "Seiken"},
      { "Ude",  "Ude"},
      { "Empi",  "Empi"},
      { "Josokutei",  "Josokotei"},
      { "Kasokutei",  "Kasokutei"}, 

      { "Master of Atemi Waza",  "Mistress of Atemi Waza"},
      { "Master of Kotegaeshi",  "Mistress of Kotegaeshi"},
      { "Master of Kansetsuwaza",  "Mistress of Kansetsuwaza"},
      { "Master of Taisabaki",  "Mistress of Taisabaki"},
      { "Master of Kyusho",  "Mistress of Kyusho"},

      { "Student of Kamae",  "Student of Kamae"},
      { "Master of Kamae",  "Mistress of Kamae"},
      { "Master of Ukemi",  "Master of Ukemi"},
      { "Mokuroku",  "Mokuroku"},
      { "Ogoshi",  "Ogoshi"},

      { "Ippon Seinage",  "Ippon Seinage"},
      { "Koshi Garuma",  "Koshi Garuma"},
      { "Sukuinage",  "Sukuinage"},
      { "Osotogari",  "Osotogari"},
      { "Uki Goshi",  "Uki Goshi"},	

      { "Taiotoshi",  "Taiotoshi"},
      { "Harai Goshi",  "Harai Goshi"},
      { "Yama Arashi",  "Yama Arashi"},
      { "Master of Nage Waza",  "Mistress of Nage Waza"},
      { "Entrance to Owaza",  "Entrance to Owaza"},

      { "Novice of Owaza",  "Novice of Owaza"},
      { "Student of Owaza",  "Student of Owaza"},
      { "Learned of Owaza",  "Learned of Owaza"},
      { "Master of Owaza",  "Mistress of Owaza"},
      { "Menkyo",  "Menkyo"},

      { "Sensei",  "Sensei"},
      { "Shinan",  "Shinan"},
      { "Shihan",  "Shihan"},
      { "Kaiden",  "Kaiden"},
      { "Master of the Miyama Ryu",  "Mistress of the Miyama Ryu"},

      { "Ninja Hero",  "Ninja Heroine"},
	{ "Ninja Avatar",	"Ninja Avatar"		},
	{ "Ninja Angel",		"Ninja Angel"		},
	{ "Ninja Demigod",		"Ninja Demigoddess"		},
	{ "Immortal Ninja",	"Immortal Ninja"		},

	{ "Ninja God",		"Ninja God"			},
	{ "Ninja Deity",		"Ninja Deity" 		},
	{ "Supreme Ninja",	"Supreme Ninja"	},
        { "Creator",                    "Creator"                       },
	{ "Implementor",		"Implementress"			}
    },

    {
        { "Cleric",                     "Cleric"                        },

	{ "Believer",                   "Believer"                      },
	{ "Attendant",                  "Attendant"                     },
	{ "Acolyte",                    "Acolyte"                       },
	{ "Novice",                     "Novice"                        },
	{ "Missionary",                 "Missionary"                    },

	{ "Adept",                      "Adept"                         },
	{ "Deacon",                     "Deaconess"                     },
	{ "Vicar",                      "Vicaress"                      },
	{ "Priest",                     "Priestess"                     },
	{ "Minister",                   "Lady Minister"                 },

	{ "Canon",                      "Canon"                         },
	{ "Levite",                     "Levitess"                      },
	{ "Curate",                     "Curess"                        },
	{ "Father",                     "Nun"                           },
	{ "Healer",                     "Healess"                       },

	{ "Chaplain",                   "Chaplain"                      },
	{ "Expositor",                  "Expositress"                   },
	{ "Bishop",                     "Bishop"                        },
	{ "Arch Bishop",                "Arch Lady of the Temple"       },
	{ "Patriarch",                  "Matriarch"                     },

	{ "Elder Patriarch",            "Elder Matriarch"               },
	{ "Grand Patriarch",            "Grand Matriarch"               },
	{ "Great Patriarch",            "Great Matriarch"               },
	{ "Bane of Pagans",             "Bane of Pagans"                },
	{ "Savior of Unbelievers",       "Savior of Unbelievers"        },

	{ "Cardinal of the Sea",        "Cardinal of the Sea"           },
	{ "Cardinal of the Earth",      "Cardinal of the Earth"         },
	{ "Cardinal of the Air",        "Cardinal of the Air"           },
	{ "Cardinal of the Ether",      "Cardinal of the Ether"         },
	{ "Cardinal of the Heavens",    "Cardinal of the Heavens"       },

	{ "Avatar of Believers",        "Avatar of Believers"		},
	{ "Avatar of a Deity",          "Avatar of a Deity"             },
	{ "Avatar of Supremity",        "Avatar of Supremity"           },
	{ "Avatar of Faith",		"Avatar of Faith"		},
	{ "Master of All Divinity",     "Mistress of All Divinity"      },

	{ "Entrusted",  		"Entrusted"			},
	{ "Empowered",			"Empowered"			},
	{ "Enlightened",  		"Enlightened"			},
	{ "Divine",			"Divine"			},
	{ "Cardinal of Elements",	"Cardinal of Elements"		},
	
	{ "Cardinal of Water",		"Cardinal of Water"		},
	{ "Cardinal of Fire",		"Cardinal of Fire"		},
	{ "Cardinal of Ice",		"Cardinal of Ice"		},
	{ "Cardinal of Wind",		"Cardinal of Wind"		},
	{ "Cardinal of Storms",		"Cardinal of Storms"		},

	{ "Lord of the Temple",		"Mother of the Temple"		},
	{ "Lord of Faith",		"Mother of Faith"		},
	{ "Voice of Faith",		"Voice of Faith"		},
	{ "Hand of Faith",		"Hand of Faith"			},
	{ "Will of Gods",		"Will of Gods"			},
	
	{ "Holy Hero",			"Holy Heroine"			},
	{ "Holy Avatar",		"Holy Avatar"			},
	{ "Angel",			"Angel"				},
	{ "Demigod",			"Demigoddess",			},
	{ "Immortal",			"Immortal"			},
	{ "God",			"Goddess"			},
	{ "Deity",			"Deity"				},
	{ "Supreme Master",		"Supreme Mistress"		},
        { "Creator",                    "Creator"                       },
	{ "Implementor",		"Implementress"			}
    },

    {
        { "Monk",                       "Monk"                          },

	{ "Novice",                     "Novice"                        },
	{ "Chanter",                    "Chantress"                     },
	{ "Meditator",                  "Meditator"                     },
	{ "Child of the Monastery",     "Child of the Monastery"        },
	{ "Student of the Monastery",   "Student of the Monastery"      },

	{ "Student of Control",         "Student of Control"            },
	{ "Student of the Mind",        "Student of the Mind"           },
	{ "Student of the Body",        "Student of the Body"           },
	{ "Student of the Soul",        "Student of the Soul"           },
	{ "Student of the Spirit",      "Student of the Spirit"         },

	{ "Student of Healing",         "Student of Healing"       	},
	{ "Initiate of the Rudiments",  "Initiate of the Rudiments"     },
	{ "Initiate of the Elements",   "Initiate of the Elements"      },
	{ "Initiate of the Principles", "Initiate of the Principles"    },
	{ "Brother",                    "Sister"                        },

	{ "Elder Brother",              "Elder Sister"                  },
	{ "Disciple",                   "Disciple"                      },
	{ "Disciple of Secrets",        "Disciple of Secrets"           },
	{ "Disciple of Mysteries",      "Disciple of Mysteries"         },
	{ "Disciple of the Unexplained","Disciple of the Unexplained"   },

	{ "Disciple of the Unknown",    "Disciple of the Unknown"       },
	{ "Disciple of the Supernatural","Disciple of the Supernatural" },
	{ "Attendant of the Monastery", "Attendant of the Monastery"    },
	{ "Monk",                       "Monk"                          },
	{ "Seeker of Knowledge",        "Seeker of Knowledge"           },

	{ "Elder Monk",                 "Elder Monk"                    },
	{ "Defender of the Monastery",  "Defender of the Monastery"     },
	{ "Guardian of Knowledge",      "Guardian of Knowledge"         },
	{ "Master of the Mind",         "Mistress of the Mind"          },
	{ "Master of the Body",         "Mistress of the Body"          },

	{ "Master of the Soul",         "Mistress of the Soul"          },
	{ "Master of the Spirit",       "Mistress of the Spirit"        },
	{ "Master of the Monastery",    "Mistress of the Monastery"     },
	{ "Master of the North Wind",   "Mistress of the North Wind"    },
	{ "Master of the West Wind",    "Mistress of the West Wind"     },

	{ "Master of the South Wind",   "Mistress of the South Wind"    },
	{ "Master of the East Wind",    "Mistress of the East Wind"     },
	{ "Master of the Four Winds",   "Mistress of the Four Winds"    },
	{ "Master of Dragons",          "Mistress of Dragons"           },
	{ "Master of Snow",             "Mistress of Snow"              },

	{ "Master of Leaves",           "Mistress of Leaves"            },
	{ "Master of Trees",            "Mistress of Trees"             },
	{ "Master of Flowers",          "Mistress of Flowers"           },
	{ "Master of Orbit",            "Mistress of Orbit"             },
	{ "Grand Master of Dragons",    "Grand Mistress of Dragons"     },

	{ "Grand Master of Winter",     "Grand Mistress of Winter"      },
	{ "Grand Master of Autumn",     "Grand Mistress of Autumn"      },
	{ "Grand Master of Summer",     "Grand Mistress of Summer"      },
	{ "Grand Master of Spring",     "Grand Mistress of Spring"      },
	{ "Grand Master of Seasons",    "Grand Mistress of Seasons"     },

	{ "Holy Hero",                  "Holy Heroine"                  },
	{ "Holy Avatar",                "Holy Avatar"                   },
	{ "Angel",                      "Angel"                         },
	{ "Demigod",                    "Demigoddess",                  },
	{ "Immortal",                   "Immortal"                      },
	{ "God",                        "Goddess"                       },
	{ "Deity",                      "Deity"                         },
	{ "Supreme Master",             "Supreme Mistress"              },
	{ "Creator",                    "Creator"                       },
	{ "Implementor",                "Implementress"                 }
    },

    {
        { "Invoker",                    "Invoker"                       },

	{ "Apprentice of Magic",        "Apprentice of Magic"           },
	{ "Spell Student",              "Spell Student"                 },
	{ "Scholar of Magic",           "Scholar of Magic"              },
	{ "Delver in Spells",           "Delveress in Spells"           },
/* 5 */ { "Medium of Magic",            "Medium of Magic"               },
	{ "Scribe of Magic",            "Scribess of Magic"             },
	{ "Seer",                       "Seeress"                       },
	{ "Sage",                       "Sage"                          },
        { "Magus",                      "Craftess"                      },
/* 10 */{ "Abjurer",                    "Abjuress"                      },
        { "Prestidigitator",            "Prestidigitatress"             },
	{ "Enchanter",                  "Enchantress"                   },
	{ "Conjurer",                   "Conjuress"                     },
	{ "Magician",                   "Witch"                         },
/* 15 */{ "Creator",                    "Creator"                       },
	{ "Savant",                     "Savant"                        },
        { "Wizard",                     "Wizard"                        },
        { "Sorcerer",                   "Sorceress"                     },
        { "Grand Sorceror",             "Grand Sorceress"               },
/* 20 */{ "Invoker",                    "Invoker"                       },
        { "Greater Invoker",            "Greater Invoker"               },
        { "Greater Enchanter",          "Greater Enchantress"           },
        { "Channeler",                  "Channeler"                     },
        { "Evoker",                     "Evoker"                        },
/* 25 */{ "Seeker of Cold",             "Seeker of Cold"                },
        { "Seeker of Heat",             "Seeker of Heat",               },
        { "Seeker of Electricity",      "Seeker of Electricity",        },
        { "Seeker of the Elements",     "Seeker of the Elements",       },
        { "Theurgist",                  "Theurgist",                    },
/* 30 */{ "Thaumaturgist",              "Thaumaturgist",                },
        { "Master Enchanter",           "Master Enchantress"            },
        { "Master Channeler",           "Master Channeler"              },
        { "Master Evoker",              "Master Evoker"                 },
        { "Caller of Ice",              "Caller of Ice"                 },
/* 35 */{ "Caller of Inferno",          "Caller of Inferno"             },
        { "Caller of Energy",           "Caller of Energy"              },
        { "Conjurer of Energy",         "Conjuress of Energy"           },
        { "Wizard of the Mystic",       "Wizard of the Mystic"          },
        { "Creator of the Arcane",      "Creator of the Arcane"         },
/* 40 */{ "Master of the Arcane",       "Mistress of the Arcane"        },
        { "Master of Frost",            "Mistress of Frost"             },
        { "Master of Fire",             "Mistress of Fire"              },
        { "Master of Lightning",        "Mistress of Lightning"         },
        { "Master of Divination",       "Mistress of Divination"        },
/* 45 */{ "Master of Pyrotechniques",   "Mistress of Pyrotechniques"    },
        { "Master of Invokation",       "Mistress of Invokation"        },
        { "Archmage",                   "Archmage"                      },
        { "Mystical",                   "Mystical"                      },
        { "Spellbinder",                "Spellbindress"                 },
/* 50 */{ "Angel of Arcana",            "Angel of Arcana"               },
        { "Hero of Invokation",         "Heroine of Invokation"         },
        { "Avatar of Invokation",       "Avatar of Invokation"          },
        { "Angel of Invokation",        "Angel of Invokation"           },
        { "Demigod of Invokation",      "Demigoddess of Invokation"     },
        { "Immortal of Invokation",     "Immortal of Invokation"        },
        { "God of Invokation",          "Goddess of Invokation"         },
        { "Deity of Invokation",        "Deity of Invokation"           },
        { "Supremity of Invokation",    "Supremity of Invokation"       },
        { "Creator of Invokation",      "Creator of Invokation"         },
        { "Implementor of Invokation",  "Implementress of Invokation"   }
    },

    {
        { "Battlemage",                 "Battlemage"			},
	{ "Apprentice of Magic",        "Apprentice of Magic"		},
	{ "Spell Student",              "Spell Student"			},
	{ "Scholar of Magic",           "Scholar of Magic"		},
	{ "Delver in Spells",           "Delveress in Spells"		},
/* 5*/  { "Medium of Magic",            "Medium of Magic"		},
	{ "Scribe of Magic",            "Scribess of Magic"		},
	{ "Seer",                       "Seeress"			},
	{ "Sage",                       "Sage"				},
        { "Magus",                      "Craftess"			},
/* 10 */{ "Abjurer",                    "Abjuress"			},
        { "Prestidigitator",            "Prestidigitatress"		},
	{ "Enchanter",                  "Enchantress"			},
	{ "Conjurer",                   "Conjuress"			},
	{ "Magician",                   "Witch"				},
/* 15 */{ "Creator",                    "Creator"			},
	{ "Savant",                     "Savant"			},
	{ "Wizard",                     "Wizard"			},
        { "Sorcerer",                   "Sorceress"			},
	{ "Grand Sorcerer",             "Grand Sorceress"		},
/* 20 */{ "Warmage",			"Warmage"			},
        { "Elder Warmage",		"Elder Warmage"			},
	{ "Master of Engagements",	"Mistress of Engagements",	},
	{ "Master of the Battlefield",	"Mistress of the Battlefields", },
	{ "Master of Battles",		"Mistress of Battles",		},
/* 25 */{ "Battlemage",			"Battlemage"			},
	{ "Student of Light",		"Student of Light",		},
	{ "Student of Metal",		"Student of Metal",		},
	{ "Student of Air",		"Student of Air",		},
	{ "Student of Earth",		"Student of Earth",		},
/* 30 */{ "Adept of Metal",		"Adept of Metal",		},
	{ "Adept of Earth",		"Adept of Earth",		},
	{ "Adept of Light",		"Adept of Light",		},
	{ "Caller of Blades",		"Caller of Blades",		},
	{ "Walking Scythe",		"Walking Scythe",		},
/* 35 */{ "Walking Armory",     	"Walking Armory"        	},
	{ "Walking Legion",		"Walking Legion"		},
	{ "Bane of Warriors",		"Bane of Warriors"		},
	{ "Gore Maker",			"Gore Maker"			},
	{ "Focus of Gravity",		"Focus of Gravity"           	},
/* 40 */{ "Master of Craters",		"Mistress of Craters",		},
	{ "Summoner of Meteors",	"Summoner of Meteors",		},
	{ "Caller of Asteroids",	"Caller of Asteroids",		},
	{ "Initiate of the Crimson Omen","Initiate of the Crimson Omen",},
    	{ "Eye of the Sun",	  	"Eye of the Sun",		},
/* 45 */{ "Bladed Horror",		"Bladed Horror",		},
	{ "Magnate of the Crimson Star","Savant of the Crimson Star"	},
        { "Battlemage of Annihilation", "Battlemage of Annihilation"	},
        { "Battlelord of Holocausts",	"High Lady of Holocausts"     	},
        { "Harbinger of Genocide",      "Harbinger of Genocide"       	},
/* 50 */{ "Scourge of the Battlefield", "Scourge of the Battlefield"	},
        { "Hero of Battlefields",       "Heroine of Battlefields"	},
        { "Avatar of Destruction",      "Avatar of Destruction"		},
        { "Angel of Demolishion",       "Angel of Demolishion"		},
        { "Demigod of Annihilation",    "Demigoddess of Annihilation"	},
        { "Immortal of Annihilation",   "Immortal of Annihilation"	},
        { "God of Annihilation",        "Goddess of Annihilation"	},
        { "Deity of Annihilation",      "Deity of Annihilation"		},
        { "Supremity of Annihilation",  "Supremity of Annihilation"	},
        { "Creator of Annihilation",    "Creator of Annihilation"	},
        { "Implementor of Destruction", "Implementress of Destruction"	}
    },

    {
        { "Necromancer",                 "Necromancer"                  },
	{ "Apprentice of Magic",        "Apprentice of Magic"           },
	{ "Spell Student",              "Spell Student"                 },
	{ "Scholar of Magic",           "Scholar of Magic"              },
	{ "Delver in Spells",           "Delveress in Spells"           },
/* 5 */ { "Medium of Magic",            "Medium of Magic"               },
	{ "Scribe of Magic",            "Scribess of Magic"             },
	{ "Seer",                       "Seeress"                       },
	{ "Sage",                       "Sage"                          },
	{ "Illusionist",                "Illusionist"                   },
/* 10 */{ "Abjurer",                    "Abjuress"                      },
	{ "Invoker",                    "Invoker"                       },
	{ "Enchanter",                  "Enchantress"                   },
	{ "Conjurer",                   "Conjuress"                     },
	{ "Magician",                   "Witch"                         },
/* 15 */{ "Creator",                    "Creator"                       },
        { "Savant",                     "Savant"                        },
        { "Wizard",                     "Wizard"                        },
	{ "Sorcerer",                   "Sorceress"                     },
        { "Grand Sorcerer",             "Grand Sorceress"               },
/* 20 */{ "Necromancer",                "Necromancer"                   },
        { "Greater Necromancer",        "Greater Necromancer"           },
	{ "Golem Maker",                "Golem Maker"                   },
	{ "Greater Golem Maker",        "Greater Golem Maker"           },
        { "Soul Searcher",              "Soul Searcher"                 },
/* 25 */{ "Life Stealer",               "Life Stealer"                  },
        { "Banisher",                   "Banisher"                      },
        { "Summoner",                   "Summoner"                      },
        { "Animator",                   "Animator"                      },
        { "Abomination",                "Abomination"                   },
/* 30 */{ "Master Golem Maker",         "Master Golem Maker"            },
        { "Master Banisher",            "Master Banisher"               },
        { "Master Summoner",            "Master Summoner"               },
        { "Master Animator",            "Master Animator"               },
        { "Lord of Zombies",            "Lady of Zombies"               },
/* 35 */{ "Lord of Wights",             "Lady of Wights"                },
        { "Lord of Ghouls",             "Lady of Ghouls"                },
        { "Bringer of Plague",          "Bringer of Plague"             },
        { "Bringer of Suffering",       "Bringer of Suffering"          },
        { "Bringer of Torment",         "Bringer of Torment"            },
/* 40 */{ "Master of Death",            "Mistress of Death"             },
        { "Master of Exile",            "Mistress of Exile"             },
        { "Master Occultist",           "Master Occultist"              },
        { "Master of Unholy Rites",     "Mistress of Unholy Rites"      },
        { "Master of Necromancy",       "Mistress of Necromancy"        },
/* 45 */{ "Master of Black Magic",      "Mistress of Black Magic"       },
        { "Master of the Undead",       "Mistress of the Undead"        },
        { "Soul Keeper of Wraiths",     "Soul Keeper of Wraiths"        },
        { "Soul Keeper of Spectres",    "Soul Keeper of Spectres"       },
        { "Soul Keeper of Liches",      "Soul Keeper of Liches"          },
/* 50 */{ "King of Devils",             "Queen of Devils"               },
        { "Hero of Death",              "Heroine of Death"              },
        { "Avatar of Necromancy",       "Avatar of Necromancy"          },
        { "Angel of Necromancy"         "Angel of Necromancy"           },
        { "Demigod of Necromancy"       "Demigoddess of Necromancy"     },
        { "Immortal of Necromancy"      "Immortal of Necromancy"        },
        { "God of Necromancy"           "Goddess of Necromancy"         },
        { "Deity of Necromancy"         "Deity of Necromancy"           },
        { "Supremity of Necromancy"     "Supremity of Necromancy"       },
        { "Creator of Necromancy",      "Creator of Necromancy"         },
        { "Implementor of Necromancy",  "Implementress of Necromancy"   }
    },

    {
        { "Psionicist",                "Psionicist"                   },

	{ "Apprentice of Thought",      "Apprentice of Thought"		},
	{ "Spell Student",              "Spell Student"                 },
	{ "Scholar of the Mind",        "Scholar of the Mind"		},
	{ "Delver in Spells",           "Delveress in Spells"           },
/* 5 */	{ "Medium of the Mind",         "Medium of the Mind"		},

	{ "Scribe of the Mind",         "Scribess of the Mind"		},
	{ "Seer",                       "Seeress"                       },
	{ "Sage",                       "Sage"                          },
/* 10 */{ "Psychic Medium",             "Psychic Medium"		},
	{ "Abjurer",                    "Abjuress"                      },

	{ "Gifted",                     "Gifted"                        },
	{ "Enchanter",                  "Enchantress"                   },
	{ "Conjurer",                   "Conjuress"                     },
	{ "Magician",                   "Witch"                         },
/* 15 */{ "Sideshow Freak",             "Sideshow Freak"		},

	{ "Savant",                     "Savant"                        },
	{ "Magus",                      "Craftess"                      },
	{ "Palmist",                    "Palmist"			},
	{ "Hypnotist",                  "Hypnotist"			},
/* 20 */{ "Psionicist",                 "Psionicist"			},

	{ "Elder Psionic",              "Elder Psionic"                 },
	{ "Grand Psionic",              "Grand Psionic"	                },
	{ "Apprentice Psychic",         "Apprentice Psychic"		},
	{ "Adept of the Psyche",        "Adept of the Psyche"		},
/* 25 */{ "Master Psychic",             "Lady Psychic"			},

	{ "Psychic of Earth",           "Psychic of Earth",		},
	{ "Psychic of Water",           "Psychic of Water",             },
	{ "Psychic of Air",             "Psychic of Air",             	},
	{ "Psychic of Fire",            "Psychic of Fire",		},
/* 30 */{ "Psychic of the Elements",    "Psychic of the Elements",	},

	{ "Focus of the Mind",          "Focus of the Mind"		},
	{ "Focus of the Spirit",        "Focus of the Spirit"        	},
	{ "Focus of the Body",          "Focus of the Body",		},
	{ "Focus of the Soul",          "Focus of the Soul"	        },
/* 35 */{ "Cerebral Foci",              "Cerebral Foci"			},

	{ "Mind of Fear",               "Mind of Fear"			},
	{ "Master Psychic",             "Mistress Psychic"		},
	{ "Lord of the Soul",           "Lady of the Soul"		},
	{ "Lord of the Pentacles",      "Lady of the Pentacles"		},
/* 40 */{ "Lord of the Psyche",         "Lady of the Psyche"		},

	{ "Master of the Senses",       "Mistress of the Senses"	},
	{ "Master of Perception",       "Mistress of Perception"	},
	{ "Master of the Mind",         "Mistress of the Mind"		},
	{ "Master Clairvoyant",         "Mistress of Clairvoyance"	},
/* 45 */{ "Master of the Third Eye",    "Mistress of the Third Eye"	},

	{ "Master of Kinetics",         "Mistress of Kinetics"		},
	{ "Master of Hydrokinesis",     "Mistress of Hydrokinesis"	},
	{ "Master of Pyrokinesis",      "Mistress of Pyrokinesis"	},
	{ "Master of Telekinesis",      "Mistress of Telekinesis"	},
/* 50 */{ "Master of the Sixth Sense",  "Mistress of the Sixth Sense"   },

	{ "Mage Hero",                  "Mage Heroine"                  },
	{ "Avatar of Magic",            "Avatar of Magic"               },
	{ "Angel of Magic",             "Angel of Magic"                },
	{ "Demigod of Magic",           "Demigoddess of Magic"          },
	{ "Immortal of Magic",          "Immortal of Magic"             },
	{ "God of Magic",               "Goddess of Magic"              },
	{ "Deity of Magic",             "Deity of Magic"                },
	{ "Supremity of Magic",         "Supremity of Magic"            },
	{ "Creator",                    "Creator"                       },
	{ "Implementor",                "Implementress"                 }
    },

    {
        { "Healer",                     "Healer"                        },

	{ "Believer",                   "Believer"                      },
	{ "Faithful",			"Faithful"			},
	{ "Attendant",                  "Attendant"                     },
	{ "Disciple",			"Disciple"			},
	{ "Missionary",                 "Missionary"                    },

	{ "Acolyte",                    "Acolyte"                       },
	{ "Adept",                      "Adept"                         },
	{ "Deacon",                     "Deaconess"                     },
	{ "Archdeacon",			"Archdeaconess"			},
	{ "Priest",                     "Priestess"                     },

	{ "Vicar",                      "Vicaress"                      },
	{ "Minister",                   "Lady Minister"                 },
	{ "Canon",                      "Canon"                         },
	{ "Levite",                     "Levitess"                      },
	{ "Curate",                     "Curess"                        },

	{ "Father",                     "Nun"                           },
	{ "Chaplain",			"Chaplain"			},
	{ "Expositor",                  "Expositress"                   },
	{ "Exorcist",			"Exorcist"			},
	{ "Apprentice Healer", 		"Apprentice Healer",		},

	{ "Caretaker",			"Caretaker"			},
	{ "Bishop",                     "Bishop"                        },
	{ "Arch Bishop",                "Arch Lady of the Church"       },
	{ "Miracle Worker",		"Miracle Worker"		},
	{ "Holy Physician",		"Holy Physician"		},

	{ "Cardinal of the Sea",        "Cardinal of the Sea"           },
	{ "Cardinal of the Earth",      "Cardinal of the Earth"         },
	{ "Cardinal of the Air",        "Cardinal of the Air"           },
	{ "Cardinal of the Ether",      "Cardinal of the Ether"         },
	{ "Cardinal of the Heavens",    "Cardinal of the Heavens"       },

	{ "Enlightened",  		"Enlightened"			},
	{ "Sacred Heart",  		"Sacred Heart"			},
	{ "Saint",  			"Saint"				},
	{ "Baptist", 			"Baptist"			},
	{ "Divine Healer",		"Divine Healer"			},	
	
	{ "Healer of Blindness",	"Healer of Blindness"		},
	{ "Healer of Deafness",		"Healer of Deafness"		},
	{ "Healer of Poison",		"Healer of Poison"		},
	{ "Healer of Disease", 		"Healer of Disease"		},
	{ "Cleanser of Maledictions",	"Cleanser of Maledictions"	},
	
	{ "Rejuvenator",		"Rejuvenator"			},
	{ "Priest of Life",		"Priestess of Life"		},
	{ "Shepherd of the Weak",	"Shepherd of the Weak"		},
	{ "Shepherd of the Helpless",	"Shepherd of the Helpless"	},
	{ "Shepherd of the Misguided",	"Shepherd of the Misguided"	},	
	
	{ "Defender of the Faithful",	"Defender of the Faithful"	},
	{ "Pillar of Faith",		"Pillar of Faith"		},
	{ "Living Touch",		"Living Touch"			},
	{ "Holy Father",		"Holy Mother"			},
	{ "Holy Patriarch of Healing",  "Holy Matriarch of Healing"	},
	
	{ "Holy Hero",			"Holy Heroine"			},
	{ "Holy Avatar",		"Holy Avatar"			},
	{ "Angel",			"Angel"				},
	{ "Demigod",			"Demigoddess",			},
	{ "Immortal",			"Immortal"			},
	{ "God",			"Goddess"			},
	{ "Deity",			"Deity"				},
	{ "Supreme Master",		"Supreme Mistress"		},
        { "Creator",                    "Creator"                       },
	{ "Implementor",		"Implementress"			}
    },

    {
        { "Shaman",                     "Shaman"},

	{ "Believer",                   "Believer"},
	{ "Faithful",			"Faithful"},
	{ "Pilgrim",  			"Pilgrim"},
	{ "Disciple",			"Disciple"},
	{ "Devout",			"Devout"},

	{ "Acolyte",			"Acolyte"},
	{ "Novice",			"Novice"},
	{ "Ascetic",			"Ascetic"},
	{ "Dogmatic",			"Dogmatic"},
	{ "Dark Priest",                "Dark Priestess"},

	{ "Student of Faith",		"Student of Faith"},
	{ "Preacher",			"Preacher"},
	{ "Prophet",			"Prophet"},
	{ "Seer",			"Seer"},
	{ "Oracle",			"Oracle"},

	{ "Prelate",			"Prelate"},
	{ "Archprelate",		"Archprelate"},
	{ "Expositor",			"Expositor"},
	{ "Diviner",			"Diviness"},
	{ "Apprentice Shaman",		"Apprentice Shaman"},
	
	{ "Apothecary",			"Apothecary"},
	{ "Inquisitor",			"Inquisitor"},
	{ "Witch Doctor",		"Witch Doctor"},
	{ "Demon Killer",               "Demon Killer"},
	{ "Greater Demon Killer",       "Greater Demon Killer"},

	{ "Acolyte of Conviction",	"Acolyte of Conviction"},
	{ "Order of the White Robe",	"Order of the White Robe"},
	{ "Order of the Grey Robe",	"Order of the Grey Robe"},
	{ "Order of the Black Robe",	"Order of the Black Robe"},
	{ "Order of the Red Robe",	"Order of the Red Robe"},
 
	{ "Zealot",			"Zealot"},
	{ "Chosen",			"Chosen"},
	{ "Adept of Faith",		"Adept of Faith"},
	{ "Spirit Watcher",		"Spirit Watcher"},
	{ "Spirit Slayer",		"Spirit Slayer"},

	{ "Communer of Famine",		"Communer of Famine"},
	{ "Communer of Damnation",	"Communer of Damnation"},
	{ "Communer of Decay",		"Communer of Decay"},
	{ "Communer of Diseases",	"Communer of Diseases"},
	{ "Wrath of the Blasphemers",	"Wrath of the Blasphemers"},	
	
	{ "Keeper of Faith",		"Keeper of Faith"},
	{ "Shaman of Fire",		"Shaman of Fire"},
	{ "Shaman of Ice",		"Shaman of Ice"},
	{ "Master Shaman",		"Master Shaman"},
	{ "Harbinger of Armageddon",	"Harbinger of Armageddon"},

	{ "Voice of an Immortal",	"Voice of an Immortal"},
	{ "Hand of an Immortal",	"Hand of an Immortal"},
	{ "Wrath of an Immortal",	"Wrath of an Immortal"},
	{ "Master of Faith",		"Master of Faith"},
	{ "Holy Shaman",  "Holy Shaman"},
	
	{ "Shaman Hero",		"Shaman Hero"			},
	{ "Holy Avatar",		"Holy Avatar"			},
	{ "Angel",			"Angel"				},
	{ "Demigod",			"Demigoddess"			},
	{ "Immortal",			"Immortal"			},
	{ "God",			"Goddess"			},
	{ "Deity",			"Deity"				},
	{ "Supreme Master",		"Supreme Mistress"		},
        { "Creator",                    "Creator"                       },
	{ "Implementor",		"Implementress"			}
    },

    {
        { "Vampire",                "Vampire"                   },

      { "Scum",  "Scum"},
      { "Bully",  "Bully"},
      { "Thug",  "Moll"},
      { "Brute",  "Bitch"},
      { "Ruffian",  "Butch"},

      { "Pillager",  "Pillager"},
      { "Destroyer",  "Destroyer"},
      { "Burner",  "Burner"},
      { "Hired Killer",  "Hired Killer"},
      { "Brigand",  "Brigand"},

      { "Mercenary",  "Mercenary"},
      { "Black Sword",  "Black Sword"},
      { "Crimson Sword",  "Crimson Sword"},
      { "Sneaky",  "Sneaky"},
      { "Cruel",  "Cruel"},

      { "Stealer",  "Stealer"},
      { "Infamous",  "Infamous"},
      { "Hated",  "Hated"},
      { "Complete Bastard",  "Complete Bitch"},
      { "Dark Knight",  "Dark Knight"},

      { "Evil Fighter",  "Evil Fighter"},
      { "Rogue Knight",  "Rogue Lady"},
      { "Evil Champion",  "Evil Champion"},
      { "Dishonorable",  "Dishonorable"},
      { "Black Knight",  "Black Lady"},

      { "Crimson Knight",  "Crimson Lady"},
      { "Knight of Brimstone",  "Lady of Brimstone"},
      { "Knight of the Inverted Cross",  "Lady of the Inverted Cross"},
      { "Knight of Pain",  "Lady of Pain"},
      { "Vampire Initiate",  	"Succubus Initiate"},

      { "Underling",		"Underling"},
      { "Impaler",  		"Impaler"},
      { "Bloodseeker",  	"Bloodseeker"},
      { "Lifestealer",  	"Lifestealer"},
      { "Vampire",  		"Succubus"},

      { "Master Vampire",  	"Master Succubus"},
      { "Child of Darkness",  	"Child of Darkness"},
      { "Nightwalker",  	"Nightwalker"},
      { "Harbinger of Night",  	"Harbinger of Night"},
      { "Dark Metamorphosis",   "Dark Metamorphosis"},

      { "Bloodlust",  		"Bloodlust"},
      { "Living Death",  	"Living Death"},
      { "Nocturnal Knight",  	"Nocturnal Lady"},
      { "Dhampiri",  		"Dhampiri"},
      { "Shadowstalker",  	"Shadowstalker"},

      { "Acolyte of Darkness",  "Acolyte of Darkness"},
      { "Prince of Blood",  	"Princess of Blood"},
      { "Prince of Carnage",  	"Princess of Carnage"},
      { "Prince of Darkness", 	"Princess of Darkness"},
      { "Count",  		"Countess"},

      { "Undead Hero",  	"Undead Heroine"},
      { "Avatar of Blood",	"Avatar of Blood"		},
      { "Angel of Darkness",	"Angel of Darkness"		},
      { "Demigod of Night",	"Demigoddess of Night"		},
      { "Immortal Vampire",	"Immortal Vampire"		},
      { "God of Unliving",	"God of Unliving"		},
      { "Deity of Carnage",	"Deity of Carnage"		},
      { "Supreme Master of Death","Supreme Mistress of Death"	},
      { "Creator",               "Creator"                       },
      { "Implementor",		"Implementress"			}
    },
    {
	{ "Bards",			"Bards"				},

	{ "Apprentice Minstrel",	"Apprentice Minstrel"		},
	{ "Jester",			"Jester"			},
	{ "Juggler",			"Juggler"			},
	{ "Serenader",			"Serenader"			},
	{ "Caroler",			"Caroler"			},
// 5
	{ "Drummer",			"Drummer"			},
	{ "Piper",			"Piper"				},
	{ "Fiddler",			"Fiddler"			},
	{ "Chanter",			"Chanter"			},
	{ "Troubadour",			"Troubadour"			},
// 10
	{ "Flutist",			"Flutist"			},
	{ "Harpist",			"Harpist"			},
	{ "Singer",			"Singer"			},
	{ "Soloist",			"Soloist"			},
	{ "Instrumentalist",		"Instrumentalist"		},
// 15
	{ "Rhymer",			"Rhymer"			},
	{ "Poet",			"Poet"				},
	{ "Dancer",			"Dancer"			},
	{ "Balladeer",			"Balladeer"			},
	{ "Musician",			"Musician"			},
// 20
	{ "Master Singer",		"Master Singer"			},
	{ "Master Instrumentalist",	"Master Instrumentalist"	},
	{ "Master Rhymer",		"Master Rhymer"			},
	{ "Master Poet",		"Master Poet"			},
	{ "Composer",			"Composer"			},
// 25
	{ "Lyricist",			"Lyricist"			},
	{ "Melody Maker",		"Melody Maker"			},
	{ "Hymnist",			"Hymnist"			},
	{ "Songsmith",			"Songsmith"			},
	{ "Minstrel",			"Minstrel"			},
// 30
	{ "Master Minstrel",		"Master Minstrel"		},
	{ "Master of Instrument",	"Master of Instrument"		},
	{ "Master of Words",		"Master of Words"		},
	{ "Master of Notes",		"Master of Notes"		},
	{ "Master of Melody",		"Master of Melody"		},
// 35
	{ "Master of Song",		"Master of Song"		},
	{ "Master of Verse",		"Master of Verse"		},
	{ "Master of Harmony",		"Master of Harmony"		},
	{ "Master of Euphony",		"Master of Euphony"		},
	{ "Court Muscian",		"Court Muscian"			},
// 40
	{ "Master Poet",		"Master Poet"			},
	{ "Master Melody Maker",	"Master Melody Maker"		},
	{ "Master Composer",		"Master Composer"		},
	{ "Master Songsmith",		"Master Songsmith"		},
	{ "Student of History",		"Student of History"		},
// 45
	{ "Historian",			"Historian"			},
	{ "Master Historian",		"Master Historian"		},
	{ "Chronicler",			"Chronicler"			},
	{ "Master Chronicler",		"Master Chronicler"		},
	{ "Master Bard",		"Master Bard"			},
// 50
	{ "Bard Hero",			"Bard Heroine"			},
	{ "Avatar of Bards",		"Avatar of Bards"		},
	{ "Angel of Bards",		"Angel of Bards"		},
	{ "Demigod of Bards",		"Demigodess of Bards"		},
	{ "Immortal Bard",		"Immortal Bard"			},
// 55
	{ "God of Bards",		"God of Bards"			},
	{ "Deity of Bards",		"Deity of Bards"		},
	{ "Supreme Master of Bards",	"Supreme Master of Bards"	},
	{ "Creator",			"Creator"			},
	{ "Implementor",		"Implementor"			},
    },
    {
        { "Druid",                      "Druid"                         },

        { "Forest Pupil",               "Forest Pupil"                  },
        { "Natural",                    "Natural"                       },
        { "Gatherer",                   "Gatherer"                      },
        { "Wanderer",                   "Wanderer"                      },
        { "Drifter",                    "Drifter"                       },
//5
        { "Forest Scholar",             "Forest Scholar"                },
        { "Forest Adept",               "Forest Adept"                  },
        { "Student of the Leaf",        "Student of the Leaf"           },
        { "Student of the Trees",       "Student of the Trees"          },
        { "Adept of Nature",            "Adept of Nature"               },
//10
        { "Dilettante Druid",           "Dilettante Druidess"           },
        { "Adept of the Woods",         "Adept of the Woods"            },
        { "Priest of Nature",           "Priestess of Nature"           },
        { "Apprentice Druid",           "Apprentice Druid"              },
        { "Scholar of Nature",          "Scholar of Nature"             },
//15
        { "Master Woodsman",            "Mistress Woodswoman"           },
        { "Druid Initiate",             "Druidess Initiate"             },
        { "Druid Candidate",            "Druidess Candidate"            },
        { "Druid Liege",                "Druidess Liege"                },
        { "Druid",                      "Druidess"                      },
//20
        { "Druid of the Red Circle",    "Druidess of the Red Circle"    },
        { "Druid of the Blue Circle",   "Druidess of the Blue Circle"   },
        { "Druid of the White Circle",  "Druidess of the White Circle"  },
        { "Order of the Leaf",          "Order of the Leaf"             },
        { "Order of Nature",            "Order of Nature"               },
//25
        { "Keeper of the Maple",        "Keeper of the Maple"           },
        { "Keeper of the Oak",          "Keeper of the Oak"             },
        { "Keeper of the Forest",       "Keeper of the Forest"          },
        { "Protector of the Forest",    "Protector of the Forest"       },
        { "Guardian of the Sacred Circle", "Guardian of the Sacred Circle"},
//30
        { "Druid Lord",                 "Druidess Lady"                 },
        { "Elder Druid",                "Elder Druid"                   },
        { "Force of the Trees",         "Force of the Trees"            },
        { "Eye of the Forest",          "Eye of the Forest"             },
        { "Master of the Green Circle", "Mistress of the Green Circle"  },
//35
        { "Master of the Maple",        "Mistress of the Maple"         },
        { "Master of the Oak",          "Mistress of the Oak"           },
        { "Master of Nature",           "Mistress of Nature"            },
        { "Master of the Inner Circle", "Mistress of the Inner Circle"  },
        { "Legend of the Forest",       "Legend of the Forest"          },
//40
        { "Adept of the Earth",         "Adept of the Earth"            },
        { "Lord of the Trees",          "Lady of the Trees"             },
        { "Keeper of Nature",           "Keeper of Nature"              },
        { "Lord of the Oaken Peace",    "Lady of the Oaken Peace"       },
        { "Lord of the Druids",         "Lady of the Druids"            },
//45
        { "Lord of the Green Circle",   "Lady of the Green Circle"      },
        { "Oracle of the Forest",       "Oracle of the Forest"          },
        { "High Protector of Nature",   "High Protector of Nature"      },
        { "Chosen of the Druids",       "Chosen of the Druids"          },
        { "Exalted Defender of Nature", "Exalted Defender of Nature"    },
//50
      	{ "Druid Hero", 		"Druid Heroine"			},
      	{ "Druid Avatar", 		"Druid Avatar"			},
      	{ "Druid Angel", 		"Druid Angel"			},
      	{ "Druid Demigod", 		"Druid Demigod"			},
      	{ "Immortal Druid", 		"Immortal Druid"		},
      
      	{ "Druid God", 			"Druid God"			},
      	{ "Druid Deity", 		"Druid Deity"			},
      	{ "Druid Supremity", 		"Druid Supremity"		},
      	{ "Creator", 			"Creator"			},
      	{ "Implementor", 		"Implementress"			}
    },
    {
        { "Crusader",                   "Crusader"                      },

        { "Devout",                     "Devout"                        },
        { "Chaste",                     "Chaste"                        },
        { "Ardent",                     "Ardent"                        },
        { "Venerable",                  "Venerable"                     },
        { "Reverent",                   "Reverent"                      },

        { "Ethical",                    "Ethical"                       },  
        { "Fervid",                     "Fervid"                        },
        { "Proselytor",                 "Proselytress"                  },
        { "Moralist",                   "Moralist"                      },
        { "Militant",                   "Militant"                      },

        { "Puritan",                    "Puritan"                       },
        { "Cathartic",                  "Cathartic"                     },
        { "Sanctified",                 "Sanctified"                    },
        { "Redeemer",                   "Redeemer"                      },
        { "Savior",                     "Savior"                        },

        { "Steadfast",                  "Steadfast"                     },
        { "Staunch",                    "Staunch"                       },
        { "Child of God",               "Child of God"                  },
        { "Devoted",                    "Devoted"                       },
        { "Crusader",                   "Crusader"                      },   

        { "Hand of God",                "Hand of God"                   },
        { "Will of the Holy",           "Will of the Holy"              },
        { "Advocate of the Order",      "Advocate of the Order"         },
        { "Partisan of the Order",      "Partisan of the Order"         },
        { "Initate of the Order",       "Initiate of the Order"         },

        { "Sectarian of the Order",     "Sectarian of the Order"        },
        { "Champion of the Order",      "Champion of the Order"         },
        { "Pathos of God",              "Pathos of God"                 },
        { "Dauntless Vindicator",       "Dauntless Vindicator"          },
        { "Purifier of Hedonists",      "Purifier of Hedonists"         },

        { "Bane of Idolators",          "Bane of Idolators"             },
        { "Bane of Atheists",           "Bane of Atheists"              },
        { "Bane of Heathens",           "Bane of Heathens"              },
        { "Bane of Heretics",           "Bane of Heretics"              },
        { "Bane of Sybarites",          "Bane of Sybarites"             },  

        { "Cleanser of Infidels",       "Cleanser of Infidels"		},
        { "Cleanser of Pagans",         "Cleanser of Pagans"		},
        { "Epitome of Temperance",      "Epitome of Temperance"		},
        { "Herald of the Crusades",     "Herald of the Crusades"	},
        { "Hierarch of the Crusades",   "Hierarch of the Crusades"	},

        { "Master of Truth",            "Mistress of Truth"		},
        { "Master of Virtue",           "Mistress of Virtue"		},
        { "Master of Valor",            "Mistress of Valor"		},
        { "Master of Honor",            "Mistress of Honor"		},
        { "Master of Piety",            "Mistress of Piety"             },

        { "Lieutenant of the Order",    "Lieutenant of the Order"	},
        { "Captain of the Order",       "Captain of the Order"		},
        { "Major of the Order",         "Major of the Order" 		},
        { "Commander of the Order",     "Commander of the Order"	},     
/* DEFAULT LEVEL 50 TITLE */
        { "Cardinal of the Order",	"Patron Sister of the Order"	},     
/* Level 50 titles in this order: */
/* behead, pommel bash, armor pierce, high sight, pious, fourth attack */

        { "Executioner of the Order",   "Executioner of the Order"      },
        { "High Cavalier of the Crusades", "High Cavalier of the Crusades"},
        { "Indomitable Titan of the Order","Indomitable Arm of the Order" },
        { "Righteous Eye of God",       "Righteous Eye of God"          },
        { "Patron Saint of the Order",  "Matron Saint of the Order"     },
        { "General of the Holy Order",  "General of the Holy Order"	},

/* God titles */
        { "Deity of the Crusades",      "Deity of the Crusades"         },
        { "Supreme Master of the Order","Supreme Mistress of the Order" },
        { "Creator",                    "Creator"                       },
        { "Implementor",                "Implementress"                 }
    },

    {
        { "Adventurer",                 "Adventurer"                    },

        { "Lost",			"Lost"				},
        { "Greenhorn",			"Greenhorn"			},
	{ "Rookie",			"Rookie"			},
	{ "Beginner",			"Beginner"			},
	{ "Amateur",			"Amateur"			},
//5
        { "Recruit",			"Recruit"			},
        { "Novice",			"Novice"			},
        { "Sightseer",			"Sightseer"			},
        { "Spelunker",			"Spelunker"			},
        { "Traveler",			"Traveler"			},
//10
        { "Voyager",			"Voyager"			},
        { "Pioneer",			"Pioneer"			},
        { "Trailblazer",		"Trailblazer"			},
        { "Explorer",			"Explorer"			},
        { "Adventurer",			"Adventurer"			},
//15
        { "Seeker of Ornaments",        "Seeker of Ornaments"		},
        { "Seeker of Trinkets",		"Seeker of Trinkets"		},
        { "Seeker of Antiques",		"Seeker of Antiques"		},
        { "Seeker of Artifacts",	"Seeker of Artifacts"		},
        { "Seeker of Relics",		"Seeker of Relics"		},

//20
        { "Seeker of Wealth",		"Seeker of Wealth"		},
        { "Seeker of Fortune",		"Seeker of Fortune"		},
        { "Seeker of Fame",		"Seeker of Fame"		},
        { "Seeker of Truth",		"Seeker of Truth"		},
        { "Seeker of Glory",		"Seeker of Glory"		},

//25
        { "Looter of Tombs",		"Looter of Tombs"		},
        { "Slayer of Monsters",         "Slayer of Monsters"            },
        { "Master Explorer",		"Master Explorer"		},
        { "Relic Hunter",		"Relic Hunter"			},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
//30
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
//35
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
//40
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
//45
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
//50
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
//55      
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
        { "Questor Extraordinaire",	"Questor Extraordinaire"		},
//60
    },

    {
        { "Blademaster",                "Blademaster"                   },

        { "Initiate of Body",			"Initiate of Body"	},
        { "Initiate of Mind",			"Initiate of Mind"	},
	{ "Initiate of Soul",			"Initiate of Soul"	},
        { "Initiate of Blood",			"Initiate of Blood"	},
        { "Initiate of Fear",			"Initiate of Fear"	},
//5
	{ "Student of Blades",			"Student of Blades"	},
	{ "Student of Swords",			"Student of Swords"	},
	{ "Student of Daggers",			"Student of Daggers"	},
        { "Student of Spears",			"Student of Spears"	},
        { "Student of Glaives",			"Student of Glaives"	},
//10
        { "Sheath Cleaner",			"Sheath Cleaner"	},
        { "Sheath Carrier",			"Sheath Carrier"	},
	{ "Sword Cleaner",			"Sword Cleaner"		},
	{ "Sword Carrier",			"Sword Carrier"		},
	{ "Sword Master",			"Sword Mistress"},
//15
        { "Acolyte of Copper",			"Acolyte of Copper"	},
        { "Acolyte of Bronze",			"Acolyte of Bronze"	},
	{ "Acolyte of Iron",			"Acolyte of Iron"	},
	{ "Acolyte of Steel",			"Acolyte of Steel"	},
	{ "Blademaster",			"Blademistress"		},
//20
        { "Kenin",				"Kenin"			},
        { "Ronin",				"Ronin"			},
	{ "Samurai",				"Samurai"		},
	{ "Daimyo",				"Daimyo"		},
	{ "Shogun",				"Shogun"		},
//25
        { "Battle Surveyor",			"Battle Surveyor"	},
        { "Battle Mediator",			"Battle Mediator"	},
	{ "Battle Patron",			"Battle Patron"		},
	{ "Battle Fanatic",			"Battle Fanatic"	},
	{ "Battle Lord",			"Battle Dame"		},
//30
        { "Iron Disciple",			"Iron Disciple"		},
        { "Iron Acolyte",			"Iron Acolyte"		},
	{ "Iron Priest",			"Iron Priest"		},
	{ "Iron Bishop",			"Iron Bishop"		},
	{ "Iron Angel",				"Iron Angel"		},
//35
        { "Blade of Loyalty",			"Blade of Loyalty"	},
        { "Blade of Honor",			"Blade of Honor"	},
	{ "Blade of Piety",			"Blade of Piety"	},
	{ "Blade of Accuracy",			"Blade of Accuracy"	},
	{ "Blade of Perfection",		"Blade of Perfection"	},
//40
        { "Bladed Lament",			"Bladed Lament"		},
        { "Bladed Serpent",			"Bladed Serpent"	},
	{ "Bladed Avenger",			"Bladed Avenger"	},
	{ "Bladed Terror",			"Bladed Terror"		},
	{ "Bladed Avatar",			"Bladed Avatar"		},
//45
	{ "Nemesis of Arcane",			"Nemesis of Arcane"	},
        { "Shadowdancer",			"Shadowdancer"		},
        { "Bloodweaver",			"Bloodweaver"		},
	{ "Deathsinger",			"Deathsinger"		},
	{ "Doomblade",				"Doomblade"		},
//50
	{ "Blademaster Hero",			"Blademaster Hero" },
	{ "Blademaster Hero",			"Blademaster Hero" },
	{ "Blademaster Hero",			"Blademaster Hero" },
	{ "Blademaster Hero",			"Blademaster Hero" },
	{ "Blademaster Hero",			"Blademaster Hero" },
//55
	{ "Blademaster Hero",			"Blademaster Hero" },
	{ "Blademaster Hero",			"Blademaster Hero" },
	{ "Blademaster Hero",			"Blademaster Hero" },
	{ "Blademaster Hero",			"Blademaster Hero" },
	{ "Blademaster Hero",			"Blademaster Hero" },
    }
	
};

const   struct  str_app_type    str_app         [26]            =
{
/* to hit, to dam, carry, wield */
    {  0,  0, 140, 0 },          
    {  0,  0, 140, 0 },            
    {  0,  0, 140, 0 },
    {  0,  0, 140, 0 },            
    {  0,  0, 140, 18 },
    {  0,  0, 140, 18 }, /* 5  */
    {  0,  0, 140, 18 },
    {  0,  0, 140, 18 },
    {  0,  0, 140, 18 },
    {  0,  0, 140, 18 },
    {  1,  1, 140, 18 }, /* 10  */
    {  1,  1, 140, 18 },
    {  1,  1, 140, 18 },
    {  1,  1, 140, 18 },              
    {  1,  2, 140, 18 },
    {  1,  2, 150, 18 }, /* 15  */
    {  1,  3, 165, 20 },
    {  2,  3, 180, 24 },
    {  3,  4, 200, 28 },             
    {  3,  5, 225, 32 },
    {  4,  6, 250, 38 }, /* 20  */
    {  6,  7, 300, 40 },
    {  7,  9, 350, 45 },
    {  9, 11, 400, 50 },
    { 10, 12, 450, 55 },
    { 12, 14, 500, 60 }  /* 25  */
};

const   struct  int_app_type    int_app         [26]            =
{
/* practice , learn, manap */
    { 20, 5,  0 },
    { 20, 5,  0 },                
    { 20, 5,  0 },
    { 20, 5,  0 },               
    { 20, 5,  0 },
    { 20, 5,  0 },     /*  5 */
    { 20, 5,  0 },
    { 21, 5,  0 },
    { 22, 5,  0 },
    { 23, 5,  0 },
    { 24, 6,  0 },     /* 10 */
    { 25, 6,  0 },
    { 26, 6,  0 },
    { 27, 6,  1 },
    { 28, 6,  1 },
    { 32, 7,  2 },     /* 15 */
    { 34, 7,  2 },
    { 38, 8,  2 },
    { 41, 8,  3 },   
    { 43, 9,  3 },
    { 46, 9,  3 },     /* 20 */
    { 56,10,  4 },
    { 69,11,  4 },
    { 82,12,  5 },
    { 85,13,  6 },
    {100,14,  7 }      /* 25 */
};

const   struct  wis_app_type    wis_app         [26]            =
{
/* practice, mana gain */
    { 0,  0 },                 
    { 0,  0 },                
    { 0,  0 },
    { 0,  0 },                
    { 0,  0 },
    { 1,  0 },      /*  5 */
    { 1,  0 },
    { 1,  0 },
    { 1,  0 },
    { 1,  0 },
    { 1,  0 },      /* 10 */
    { 1,  0 },
    { 1,  1 },
    { 1,  1 },
    { 1,  1 },
    { 2,  2 },      /* 15 */
    { 2,  2 },
    { 2,  2 },
    { 3,  2 },                 
    { 3,  3 },
    { 3,  3 },      /* 20 */
    { 3,  4 },
    { 4,  4 },
    { 4,  4 },
    { 4,  5 },
    { 5,  5 }       /* 25 */
};

const   struct  dex_app_type    dex_app         [26]            =
{
/* defensive */
    {    0 },           
    {    0 },            
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 5 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};

const   struct  con_app_type    con_app         [26]            =
{
/* hitpoints, shock */
    {  0,  5 },             
    {  0, 10 },           
    {  0, 15 },
    {  0, 20 },           
    {  0, 25 },
    {  0, 30 },   /*  5 */
    {  0, 35 },
    {  0, 40 },
    {  0, 45 },
    {  0, 50 },
    {  0, 55 },   /* 10 */
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },
    {  0, 75 },
    {  1, 80 },   /* 15 */
    {  1, 85 },
    {  2, 88 },
    {  2, 90 },              
    {  3, 95 },
    {  3, 97 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 }    /* 25 */
};

const   struct  liq_type        liq_table       []      =
{
/*    name                      color   proof, full, thirst, food, ssize */
    { "water",                  "clear",        {   0, 1, 10, 0, 16 }   },
    { "holy water",             "glowing",      {   0, 1, 10, 0, 16 }   },
    { "beer",                   "amber",        {  12, 1,  8, 1, 12 }   },
    { "red wine",               "burgundy",     {  30, 1,  8, 1,  5 }   },
    { "ale",                    "brown",        {  15, 1,  8, 1, 12 }   },
// 5
    { "dark ale",               "dark",         {  16, 1,  8, 1, 12 }   },
    { "whisky",                 "golden",       { 120, 1,  5, 0,  2 }   },
    { "lemonade",               "pink",         {   0, 1,  9, 2, 12 }   },
    { "firebreather",           "boiling",      { 190, 0,  4, 0,  2 }   },
    { "local specialty",        "clear",        { 151, 1,  3, 0,  2 }   },
// 10
    { "slime mold juice",       "green",        {   0, 2, -8, 1,  2 }   },
    { "milk",                   "white",        {   0, 2,  9, 3, 12 }   },
    { "tea",                    "tan",          {   0, 1,  8, 0,  6 }   },
    { "coffee",                 "black",        {   0, 1,  8, 0,  6 }   },
    { "blood",                  "red",          {   0, 2, -1, 2,  6 }   },
// 15
    { "salt water",             "clear",        {   0, 1, -2, 0,  1 }   },
    { "coke",                   "brown",        {   0, 2,  9, 2, 12 }   }, 
    { "root beer",              "brown",        {   0, 2,  9, 2, 12 }   },
    { "elvish wine",            "green",        {  35, 2,  8, 1,  5 }   },
    { "white wine",             "golden",       {  28, 1,  8, 1,  5 }   },
// 20
    { "champagne",              "golden",       {  32, 1,  8, 1,  5 }   },
    { "mead",                   "honey-colored",{  34, 2,  8, 2, 12 }   },
    { "rose wine",              "pink",         {  26, 1,  8, 1,  5 }   },
    { "benedictine wine",       "burgundy",     {  40, 1,  8, 1,  5 }   },
    { "vodka",                  "clear",        { 130, 1,  5, 0,  2 }   },
// 25
    { "cranberry juice",        "red",          {   0, 1,  9, 2, 12 }   },
    { "orange juice",           "orange",       {   0, 2,  9, 3, 12 }   }, 
    { "absinthe",               "green",        { 200, 1,  4, 0,  2 }   },
    { "brandy",                 "golden",       {  80, 1,  5, 0,  4 }   },
    { "aquavit",                "clear",        { 140, 1,  5, 0,  2 }   },
// 30
    { "schnapps",               "clear",        {  90, 1,  5, 0,  2 }   },
    { "icewine",                "purple",       {  50, 2,  6, 1,  5 }   },
    { "amontillado",            "burgundy",     {  35, 2,  8, 1,  5 }   },
    { "sherry",                 "red",          {  38, 2,  7, 1,  5 }   },      
    { "framboise",              "red",          {  50, 1,  7, 1,  5 }   },
// 35
    { "rum",                    "amber",        { 151, 1,  4, 0,  2 }   },
    { "cordial",                "clear",        { 100, 1,  5, 0,  2 }   },
    { "everclear",              "clear",        { 190, 1, 10, 0, 12 }   },
    { NULL,                     NULL,           {   0, 0,  0, 0,  0 }   }
};

const struct flag_type area_flags[] =
{
    {  "none",                 AREA_NONE,              FALSE   },
    {  "changed",              AREA_CHANGED,           TRUE    },
    {  "added",                AREA_ADDED,             TRUE    },
    {  "loading",              AREA_LOADING,           FALSE   },
    {  "city",                 AREA_CITY,              TRUE    },
    {  "lawful",	       AREA_LAWFUL,	       TRUE    },
    {  "mudschool",            AREA_MUDSCHOOL,         TRUE    },
    {  "nogate",	       AREA_NOMORTAL,          TRUE    },
    {  "norepop",	       AREA_NOREPOP,	       TRUE    },
    {  "system",	       AREA_RESTRICTED,	       TRUE    },
    {  "nopathfind",	       AREA_NOPATH,	       TRUE    },
    {  "marked",	       AREA_MARKED,	       FALSE   },
    {  "raid",		       AREA_RAID,	       FALSE   },
    {  "cabal",		       AREA_CABAL,	       FALSE   },
    {  NULL,                   0,                      0       }
};

const struct flag_type sex_flags[] =
{
    {  "male",                 SEX_MALE,               TRUE    },
    {  "female",               SEX_FEMALE,             TRUE    },
    {  "neutral",              SEX_NEUTRAL,            TRUE    },
    {  "random",               3,                      TRUE    },
    {  "none",                 SEX_NEUTRAL,            TRUE    },
    {  NULL,                   0,                      0       }
};

const struct flag_type exit_flags[] =
{
    {  "door",                 EX_ISDOOR,              TRUE    },
    {  "closed",               EX_CLOSED,              TRUE    },
    {  "locked",               EX_LOCKED,              TRUE    },
    {  "pickproof",            EX_PICKPROOF,           TRUE    },
    {  "nopass",               EX_NOPASS,              TRUE    },
    {  "easy",                 EX_EASY,                TRUE    },
    {  "hard",                 EX_HARD,                TRUE    },
    {  "infuriating",          EX_INFURIATING,         TRUE    },
    {  "noclose",              EX_NOCLOSE,             TRUE    },
    {  "nolock",               EX_NOLOCK,              TRUE    },
    {  "secret",               EX_SECRET,              TRUE    },
    {  NULL,                   0,                      0       }
};

const struct flag_type door_resets[] =
{
    {  "open and unlocked",    0,              TRUE    },
    {  "closed and unlocked",  1,              TRUE    },
    {  "closed and locked",    2,              TRUE    },
    {  NULL,                   0,              0       }
};

const struct flag_type room_flags[] =
{
    {  "dark",                 ROOM_DARK,              TRUE    },
    {  "mark",                 ROOM_MARK,              FALSE   },
    {  "path-mark",            ROOM_PATH_MARK,         FALSE   },
    {  "no_mob",               ROOM_NO_MOB,            TRUE    },
    {  "indoors",              ROOM_INDOORS,           TRUE    },
    {  "damage",               ROOM_DAMAGE,            TRUE    },
    {  "private",              ROOM_PRIVATE,           TRUE    },
    {  "safe",                 ROOM_SAFE,              TRUE    },
    {  "no_quit",              ROOM_NO_QUIT,           TRUE    },
    {  "solitary",             ROOM_SOLITARY,          TRUE    },
    {  "pet_shop",             ROOM_PET_SHOP,          TRUE    },
    {  "no_inout",             ROOM_NO_INOUT,          TRUE    },
    {  "imp_only",             ROOM_IMP_ONLY,          TRUE    },
    {  "gods_only",            ROOM_GODS_ONLY,         TRUE    },
    {  "newbies_only",         ROOM_NEWBIES_ONLY,      TRUE    },
    {  "nowhere",              ROOM_NOWHERE,           TRUE    },
    {  "registry",             ROOM_REGISTRY,          TRUE    },
    {  "no_gatein",            ROOM_NO_GATEIN,         TRUE    },
    {  "no_gateout",           ROOM_NO_GATEOUT,        TRUE    },
    {  "no_summonin",          ROOM_NO_SUMMONIN,       TRUE    },
    {  "no_summonout",         ROOM_NO_SUMMONOUT,      TRUE    },
    {  "no_recall",            ROOM_NO_RECALL,         TRUE    },
    {  "no_vortex",            ROOM_NO_VORTEX,         TRUE    },
    {  "no_scan",              ROOM_NO_SCAN,           TRUE    },
    {  "no_teleportin",        ROOM_NO_TELEPORTIN,     TRUE    },
    {  "no_teleportout",       ROOM_NO_TELEPORTOUT,    TRUE    },
    {  "dark_rift",            ROOM_DARK_RIFT,         TRUE    },
    {  "no_ghost",             ROOM_NO_GHOST,          TRUE    },
    {  NULL,                   0,                      0       }
};

const struct flag_type room_flags2[] =
{
    {  "no_magic",             	ROOM_NO_MAGIC,          TRUE    },
    {  "no_reset",             	ROOM_NO_RESET,          TRUE    },
    {  "day",                  	ROOM_DAY,               FALSE   },
    {  "night",                	ROOM_NIGHT,             FALSE   },
    {  "no_newbies",           	ROOM_NO_NEWBIES,        TRUE    },
    {  "alarm",       		ROOM_ALARM,        	TRUE    },
    {  "spec_update",           ROOM_SPEC_UPDATE,       FALSE   },
    {  "jailcell",		ROOM_JAILCELL,		TRUE   },
    {  "psi_fear",              ROOM_PSI_FEAR,          FALSE    },
    {  "mercy",			ROOM_MERCY,		TRUE    },
    {  "nohunger",		ROOM_NOHUNGER,		TRUE    },
    {  "library",		ROOM_LIBRARY,		TRUE    },
    {  "guild",			ROOM_GUILD_ENTRANCE,	TRUE    },
    {  NULL,                   	0,                      0       }
};

const struct flag_type sector_flags[] =
{
    {  "inside",       SECT_INSIDE,            TRUE    },
    {  "city",         SECT_CITY,              TRUE    },
    {  "field",        SECT_FIELD,             TRUE    },
    {  "forest",       SECT_FOREST,            TRUE    },
    {  "hills",        SECT_HILLS,             TRUE    },
    {  "mountain",     SECT_MOUNTAIN,          TRUE    },
    {  "swim",         SECT_WATER_SWIM,        TRUE    },
    {  "noswim",       SECT_WATER_NOSWIM,      TRUE    },
    {  "swamp",        SECT_SWAMP,             TRUE    },
    {  "air",          SECT_AIR,               TRUE    },
    {  "desert",       SECT_DESERT,            TRUE    },
    {  "lava",         SECT_LAVA,              TRUE    },
    {  "snow",	       SECT_SNOW,	       TRUE    },
    {  NULL,           0,                      0       }
};

const struct flag_type type_flags[] =
{
    {  "light",                ITEM_LIGHT,             TRUE    },
    {  "scroll",               ITEM_SCROLL,            TRUE    },
    {  "wand",                 ITEM_WAND,              TRUE    },
    {  "staff",                ITEM_STAFF,             TRUE    },
    {  "weapon",               ITEM_WEAPON,            TRUE    },
    {  "treasure",             ITEM_TREASURE,          TRUE    },
    {  "armor",                ITEM_ARMOR,             TRUE    },
    {  "potion",               ITEM_POTION,            TRUE    },
    {  "clothing",             ITEM_CLOTHING,          TRUE    },
    {  "furniture",            ITEM_FURNITURE,         TRUE    },
    {  "trash",                ITEM_TRASH,             TRUE    },
    {  "container",            ITEM_CONTAINER,         TRUE    },
    {  "drinkcontainer",       ITEM_DRINK_CON,         TRUE    },
    {  "key",                  ITEM_KEY,               TRUE    },
    {  "food",                 ITEM_FOOD,              TRUE    },
    {  "money",                ITEM_MONEY,             TRUE    },
    {  "boat",                 ITEM_BOAT,              TRUE    },
    {  "npccorpse",            ITEM_CORPSE_NPC,        TRUE    },
    {  "pc corpse",            ITEM_CORPSE_PC,         FALSE   },
    {  "fountain",             ITEM_FOUNTAIN,          TRUE    },
    {  "pill",                 ITEM_PILL,              TRUE    },
    {  "map",                  ITEM_MAP,               TRUE    },
    {  "gem",                  ITEM_GEM,               TRUE    },
    {  "jewelry",              ITEM_JEWELRY,           TRUE    },
    {  "cabal",                ITEM_CABAL,             TRUE    },
    {  "herb",                 ITEM_HERB,              TRUE    },
    {  "relic",                ITEM_RELIC,             TRUE    },
    {  "throw",                ITEM_THROW,             TRUE    },
    {  "instrument",           ITEM_INSTRUMENT,        TRUE    },
    {  "socket",	       ITEM_SOCKET,	       TRUE    },
    {  "research",	       ITEM_RESEARCH,	       TRUE    },
    {  "ranged",	       ITEM_RANGED,	       TRUE    },
    {  "projectile",	       ITEM_PROJECTILE,	       TRUE    },
    {  NULL,                   0,                      0       }
};

const struct flag_type extra_flags[] =
{
    {  "glow",                 ITEM_GLOW,              TRUE    },
    {  "hum",                  ITEM_HUM,               TRUE    },
    {  "dark",                 ITEM_DARK,              TRUE    },
    {  "lock",                 ITEM_LOCK,              TRUE    },
    {  "evil",                 ITEM_EVIL,              TRUE    },
    {  "invis",                ITEM_INVIS,             TRUE    },
    {  "magic",                ITEM_MAGIC,             TRUE    },
    {  "nodrop",               ITEM_NODROP,            TRUE    },
    {  "bless",                ITEM_BLESS,             TRUE    },
    {  "antigood",             ITEM_ANTI_GOOD,         TRUE    },
    {  "antievil",             ITEM_ANTI_EVIL,         TRUE    },
    {  "antineutral",          ITEM_ANTI_NEUTRAL,      TRUE    },
    {  "noremove",             ITEM_NOREMOVE,          TRUE    },
    {  "inventory",            ITEM_INVENTORY,         TRUE    },
    {  "nopurge",              ITEM_NOPURGE,           TRUE    },
    {  "rotdeath",             ITEM_ROT_DEATH,         TRUE    },
    {  "visdeath",             ITEM_VIS_DEATH,         TRUE    },
    {  "psienhance",           ITEM_PSI_ENHANCE,       TRUE    },
    {  "nonmetal",             ITEM_NONMETAL,          TRUE    },
    {  "nolocate",             ITEM_NOLOCATE,          TRUE    },
    {  "meltdrop",             ITEM_MELT_DROP,         TRUE    },
    {  "hadtimer",             ITEM_HAD_TIMER,         TRUE    },
    {  "sellextract",          ITEM_SELL_EXTRACT,      TRUE    },
    {  "USABLE",               ITEM_USE,	       TRUE    },
    {  "burnproof",            ITEM_BURN_PROOF,        TRUE    },
    {  "nouncurse",            ITEM_NOUNCURSE,         TRUE    },
    {  "cabaltake",            ITEM_CABALTAKE,         FALSE   },
    {  "holdsrare",            ITEM_HOLDS_RARE,        TRUE    },
    {  "`!STAINED``",          ITEM_STAIN,             TRUE    },
    {  "update_always",        ITEM_UPDATE_ALWAYS,     TRUE    },
    {  "socketable",	       ITEM_SOCKETABLE,	       TRUE    },
    {  NULL,                   0,                      0       }
};

const struct flag_type army_flags[] =
{
    {  "fly",			ARMY_FLAG_FLYING,	TRUE   },
    {  "elite",			ARMY_FLAG_ELITE,	FALSE  },
    {  "garrison",		ARMY_FLAG_GARRISON,	FALSE  },
    {  "fought",		ARMY_FLAG_FOUGHT,	FALSE  },
    {  NULL,                   0,			0      }
};

const struct flag_type army_types[] =
{
    {  "unit",			ARMY_TYPE_UNIT,		TRUE    },
    {  "bastion",		ARMY_TYPE_BASTION,	TRUE    },
    {  NULL,                   0,			0       }
};

const struct flag_type wear_flags[] =
{
    {  "take",                 ITEM_TAKE,              TRUE    },
    {  "finger",               ITEM_WEAR_FINGER,       TRUE    },
    {  "neck",                 ITEM_WEAR_NECK,         TRUE    },
    {  "body",                 ITEM_WEAR_BODY,         TRUE    },
    {  "head",                 ITEM_WEAR_HEAD,         TRUE    },
    {  "face",		       ITEM_WEAR_FACE,	       TRUE    },
    {  "legs",                 ITEM_WEAR_LEGS,         TRUE    },
    {  "feet",                 ITEM_WEAR_FEET,         TRUE    },
    {  "hands",                ITEM_WEAR_HANDS,        TRUE    },
    {  "arms",                 ITEM_WEAR_ARMS,         TRUE    },
    {  "shield",               ITEM_WEAR_SHIELD,       TRUE    },
    {  "about",                ITEM_WEAR_ABOUT,        TRUE    },
    {  "waist",                ITEM_WEAR_WAIST,        TRUE    },
    {  "wrist",                ITEM_WEAR_WRIST,        TRUE    },
    {  "wield",                ITEM_WIELD,             TRUE    },
    {  "hold",                 ITEM_HOLD,              TRUE    },
    {  "nosac",                ITEM_NO_SAC,            TRUE    },
    {  "float",                ITEM_WEAR_FLOAT,        TRUE    },
    {  "secondary",            ITEM_WEAR_SECONDARY,    TRUE    },
    {  "unique",               ITEM_UNIQUE,            TRUE    },
    {  "tattoo",               ITEM_WEAR_TATTOO,       TRUE    },
    {  "shroud",               ITEM_WEAR_SHROUD,       TRUE    },
    {  "replaceme2",           ITEM_REPLACEME2,        TRUE    },
    {  "rare",                 ITEM_RARE,              TRUE    },
    {  "parry",                ITEM_PARRY,             TRUE    },
    {  "earring",              ITEM_WEAR_EARRING,      TRUE    },
    {  "owner",                ITEM_HAS_OWNER,	       FALSE    },
    {  "ranged",               ITEM_WEAR_RANGED,       TRUE    },  
    {  "quiver",               ITEM_WEAR_QUIVER,       TRUE    },  
    {  NULL,                   0,                      0       }
};

const struct flag_type apply_flags[] =
{
    {  "none",                 APPLY_NONE,             TRUE    },
    {  "str",                  APPLY_STR,              TRUE    },
    {  "dex",		       APPLY_DEX,              TRUE    },
    {  "int",		       APPLY_INT,              TRUE    },
    {  "wis",		       APPLY_WIS,              TRUE    },
    {  "con",		       APPLY_CON,              TRUE    },
    {  "sex",                  APPLY_SEX,              TRUE    },
    {  "size",                 APPLY_SIZE,             TRUE    },
    {  "class",                APPLY_CLASS,            TRUE    },
    {  "level",                APPLY_LEVEL,            TRUE    },
    {  "age",                  APPLY_AGE,              TRUE    },
    {  "mana",                 APPLY_MANA,             TRUE    },
    {  "hp",                   APPLY_HIT,              TRUE    },
    {  "move",                 APPLY_MOVE,             TRUE    },
    {  "gold",                 APPLY_GOLD,             TRUE    },
    {  "experience",           APPLY_EXP,              TRUE    },
    {  "ac",                   APPLY_AC,               TRUE    },
    {  "hitroll",              APPLY_HITROLL,          TRUE    },
    {  "damroll",              APPLY_DAMROLL,          TRUE    },
    {  "savingaffl",           APPLY_SAVING_AFFL,      TRUE    },
    {  "savingmaled",          APPLY_SAVING_MALED,     TRUE    },
    {  "savingmental",         APPLY_SAVING_MENTAL,    TRUE    },
    {  "savingbreath",         APPLY_SAVING_BREATH,    TRUE    },
    {  "savingspell",          APPLY_SAVING_SPELL,     TRUE    },
    {  "spellaffect",          APPLY_SPELL_AFFECT,     FALSE   },
    {  "luck",                 APPLY_LUCK,             TRUE    },
    {  "spelllvl",             APPLY_SPELL_LEVEL,      TRUE    },
    {  "affllvl",	       APPLY_AFFL_LEVEL,       TRUE    },
    {  "maledlvl",             APPLY_MALED_LEVEL,      TRUE    },
    {  "mentallvl",            APPLY_MENTAL_LEVEL,     TRUE    },
    {  "spellcost",            APPLY_SPELL_COST,       TRUE    },
    {  "spelllag",             APPLY_WAIT_STATE,       TRUE    },
    {  "skilllearn",           APPLY_SKILL_LEARN,      TRUE    },
    {  "spelllearn",           APPLY_SPELL_LEARN,      TRUE    },
    {  "hitgain",              APPLY_HIT_GAIN,	       TRUE    },
    {  "managain",             APPLY_MANA_GAIN,	       TRUE    },
    {  "movegain",             APPLY_MOVE_GAIN,	       TRUE    },
    {  "condition",            APPLY_O_COND,	       FALSE   },
    {  "level",		       APPLY_O_LEVEL,	       FALSE   },
    {  "cabal",		       APPLY_O_CABAL,	       FALSE   },
    {  "weight",	       APPLY_O_WEIGHT,	       FALSE   },
    {  "cost",		       APPLY_O_COST,	       FALSE   },
    {  "val0",		       APPLY_O_VAL0,	       FALSE   },
    {  "val1",		       APPLY_O_VAL1,	       FALSE   },
    {  "val2",		       APPLY_O_VAL2,	       FALSE   },
    {  "val3",		       APPLY_O_VAL3,	       FALSE   },
    {  "val4",		       APPLY_O_VAL4,	       FALSE   },
    {  NULL,                 -1,             FALSE    }
/* KEEP THIS ENTRY LAST (bit needs to be -1) */
};

/* What is seen. */
const struct flag_type wear_loc_strings[] =
{
    {  "in the inventory",     WEAR_NONE,      TRUE    },
    {  "as a light",           WEAR_LIGHT,     TRUE    },
    {  "on the left finger",   WEAR_FINGER_L,  TRUE    },
    {  "on the right finger",  WEAR_FINGER_R,  TRUE    },
    {  "around the neck (1)",  WEAR_NECK_1,    TRUE    },
    {  "around the neck (2)",  WEAR_NECK_2,    TRUE    },
    {  "on the body",          WEAR_BODY,      TRUE    },
    {  "over the head",        WEAR_HEAD,      TRUE    },
    {  "on the face",          WEAR_FACE,      TRUE    },
    {  "on the legs",          WEAR_LEGS,      TRUE    },
    {  "on the feet",          WEAR_FEET,      TRUE    },
    {  "on the hands",         WEAR_HANDS,     TRUE    },
    {  "on the arms",          WEAR_ARMS,      TRUE    },
    {  "as a shield",          WEAR_SHIELD,    TRUE    },
    {  "about the shoulders",  WEAR_ABOUT,     TRUE    },
    {  "around the waist",     WEAR_WAIST,     TRUE    },
    {  "on the left wrist",    WEAR_WRIST_L,   TRUE    },
    {  "on the right wrist",   WEAR_WRIST_R,   TRUE    },
    {  "wielded",              WEAR_WIELD,     TRUE    },
    {  "held in the hands",    WEAR_HOLD,      TRUE    },
    {  "floating nearby",      WEAR_FLOAT,     TRUE    },
    {  "dual wielded",         WEAR_SECONDARY, TRUE    },
    {  "as a tattoo",          WEAR_TATTOO,    TRUE    },
    {  "as a shroud",          WEAR_SHROUD,    TRUE    },
    {  "on the ears",          WEAR_EARRING,   TRUE    },
    {  "as ranged  ",          WEAR_RANGED ,   TRUE    },
    {  "in quiver",            WEAR_QUIVER ,   TRUE    },
    {  NULL,                   0             , 0       }
};

const struct flag_type wear_loc_flags[] =
{
    {  "none",         WEAR_NONE,      TRUE    },
    {  "light",        WEAR_LIGHT,     TRUE    },
    {  "lfinger",      WEAR_FINGER_L,  TRUE    },
    {  "rfinger",      WEAR_FINGER_R,  TRUE    },
    {  "neck1",        WEAR_NECK_1,    TRUE    },
    {  "neck2",        WEAR_NECK_2,    TRUE    },
    {  "body",         WEAR_BODY,      TRUE    },
    {  "head",         WEAR_HEAD,      TRUE    },
    {  "face",	       WEAR_FACE,      TRUE    },
    {  "legs",         WEAR_LEGS,      TRUE    },
    {  "feet",         WEAR_FEET,      TRUE    },
    {  "hands",        WEAR_HANDS,     TRUE    },
    {  "arms",         WEAR_ARMS,      TRUE    },
    {  "shield",       WEAR_SHIELD,    TRUE    },
    {  "about",        WEAR_ABOUT,     TRUE    },
    {  "waist",        WEAR_WAIST,     TRUE    },
    {  "lwrist",       WEAR_WRIST_L,   TRUE    },
    {  "rwrist",       WEAR_WRIST_R,   TRUE    },
    {  "wielded",      WEAR_WIELD,     TRUE    },
    {  "hold",         WEAR_HOLD,      TRUE    },
    {  "floating",     WEAR_FLOAT,     TRUE    },
    {  "dwielded",     WEAR_SECONDARY, TRUE    },
    {  "tattoo",       WEAR_TATTOO,    TRUE    },
    {  "shroud",       WEAR_SHROUD,    TRUE    },
    {  "earring",      WEAR_EARRING,   TRUE    },
    {  "ranged",       WEAR_RANGED,    TRUE    },
    {  "sheathl",      WEAR_SHEATH_L,  TRUE    },
    {  "sheathr",      WEAR_SHEATH_R,  TRUE    },
    {  "quiver",       WEAR_QUIVER,    TRUE    },
    {  NULL,           0,              0       }
};

const struct flag_type container_flags[] =
{
    {  "closeable",            1,              TRUE    },
    {  "pickproof",            2,              TRUE    },
    {  "closed",               4,              TRUE    },
    {  "locked",               8,              TRUE    },
    {  "puton",                16,             TRUE    },
    {  NULL,                   0,              0       }
};

const struct thief_trap_type thief_traps[] ={
/*	name		vnum	dur	skill name,	small  exit  obj */
  {	"none",		0,	0,	"reserved",	FALSE, FALSE, FALSE},

/* BASIC TRAPS */
  {	"alarm",	1,	24,	"traps",	FALSE, TRUE, FALSE},
  {	"simple",	2,	 6,	"traps",	FALSE, TRUE, FALSE},

/* BOOBY TRAPS */
/*	name		vnum	dur	skill name,	small  exit  obj */
  {	"poison",	3,	12,	"poison needle",TRUE, FALSE, TRUE},
  {	"enfeebler",	4,	14,	"enfeebler",	FALSE, TRUE, TRUE},
  {	"fireseed",    15,	24,	"fireseed",	TRUE, FALSE, TRUE},
  {	"jagged",	5,	20,	"snares",	FALSE, TRUE, TRUE},
  {	"silver",	6,	20,	"snares",	FALSE, TRUE, TRUE},
  {	"bramble",	7,	20,	"snares",	FALSE, TRUE, TRUE},
  {	"blue",		8,	24,	"runes",	FALSE, TRUE, TRUE},
  {	"white",	9,	24,	"runes",	FALSE, TRUE, TRUE},
  {	"green",       10,	24,	"runes",	FALSE, TRUE, TRUE},

/* MAGICAL TRAPS */
/*	name		vnum	dur	skill name,	small  exit  obj */
  {	"eyedust",     11,     10,	"eyedust",	FALSE, TRUE, TRUE},
  {	"antimagic",   12,      8,	"antimagic",	FALSE, TRUE, TRUE},
  {	"amnesia charm",13,    12,	"amnesia charm",TRUE,  TRUE, TRUE},
  {	"blackspore",  14,     48,	"blackspore",	TRUE, TRUE, TRUE },

/* MECHANISMS */
/*	name		vnum	dur	skill name,	small  exit  obj */
  {	"chestbuster", 16,     72,	"chestbuster",	TRUE, TRUE, TRUE},
  {	"brambus needler",17,  36,	"brambus needler",FALSE, TRUE, TRUE},
  {	"devil wheel", 18,     72,	"devil wheel",	FALSE, TRUE, TRUE},
  {	"webcaster",  19,      40,	"webcaster",	FALSE, TRUE, TRUE},

  {	NULL,		0,	0,	NULL,		FALSE, FALSE, FALSE}
};

const struct flag_type trap_table[] =
{
    {  "none",			TTYPE_NONE,	FALSE   },
    {  "damage",		TTYPE_DAMAGE,	TRUE    },
    {  "xdamage",		TTYPE_XDAMAGE,	TRUE    },
    {  "spell",			TTYPE_SPELL,	TRUE    },
    {  "dummy",			TTYPE_DUMMY,	TRUE    },
    {  "mob",			TTYPE_MOB,	TRUE    },
    {  NULL,			TTYPE_NONE,	FALSE   }
};


const struct flag_type trap_flags[] =
{
    {  "nodisarm",		TRAP_NODISARM,	TRUE    },
    {  "alldisarm",		TRAP_ALLDISARM,	TRUE    },
    {  "easy",			TRAP_EASY,	TRUE    },
    {  "medium",		TRAP_MEDIUM,	TRUE    },
    {  "hard",			TRAP_HARD,	TRUE    },
    {  "nododge",		TRAP_NODODGE,	TRUE    },
    {  "magical",		TRAP_SAVES,	TRUE    },
    {  "invisible",		TRAP_INVIS,	TRUE    },
    {  "visible",		TRAP_VIS,	TRUE    },
    {  "delay",			TRAP_DELAY,	TRUE    },
    {  "notify",		TRAP_NOTIFY,	TRUE    },
    {  "rearm",			TRAP_REARM,	TRUE    },
    {  "silent",		TRAP_SILENT,	TRUE    },
    {  "permament",		TRAP_PERMAMENT,	TRUE    },
    {  "area",			TRAP_AREA,	TRUE    },
    {  NULL,			0,              0       }
};

const struct flag_type ac_type[] =
{
    {   "pierce",        AC_PIERCE,            TRUE    },
    {   "bash",          AC_BASH,              TRUE    },
    {   "slash",         AC_SLASH,             TRUE    },
    {   "exotic",        AC_EXOTIC,            TRUE    },
    {   NULL,              0,                    0      }
};

const struct flag_type size_flags[] =
{
    {   "tiny",          SIZE_TINY,            TRUE    },
    {   "small",         SIZE_SMALL,           TRUE    },
    {   "medium",        SIZE_MEDIUM,          TRUE    },
    {   "large",         SIZE_LARGE,           TRUE    },
    {   "huge",          SIZE_HUGE,            TRUE    },
    {   "giant",         SIZE_GIANT,           TRUE    },
    {   NULL,              0,                    0     },
};

const struct flag_type weapon_class[] =
{
    {   "exotic",        WEAPON_EXOTIC,                    TRUE    },
    {   "sword",         WEAPON_SWORD,                     TRUE    },
    {   "dagger",        WEAPON_DAGGER,                    TRUE    },
    {   "spear",         WEAPON_SPEAR,                     TRUE    },
    {   "staff",         WEAPON_STAFF,                     TRUE    },
    {   "mace",          WEAPON_MACE,                      TRUE    },
    {   "axe",           WEAPON_AXE,                       TRUE    },
    {   "flail",         WEAPON_FLAIL,                     TRUE    },
    {   "whip",          WEAPON_WHIP,                      TRUE    },
    {   "polearm",       WEAPON_POLEARM,                   TRUE    },
    {   NULL,            0,                    0       }
};

const struct flag_type weapon_type2[] =
{
    {   "flaming",       WEAPON_FLAMING,       TRUE    },
    {   "frost",         WEAPON_FROST,         TRUE    },
    {   "vampiric",      WEAPON_VAMPIRIC,      TRUE    },
    {   "sharp",         WEAPON_SHARP,         TRUE    },
    {   "vorpal",        WEAPON_VORPAL,        TRUE    },
    {   "twohands",      WEAPON_TWO_HANDS,     TRUE    },
    {   "shocking",      WEAPON_SHOCKING,      TRUE    },
    {   "poison",        WEAPON_POISON,        TRUE    },
    {   "paralyze",      WEAPON_PARALYZE,      TRUE    },
    {   "deadly",	 WEAPON_DEADLY,	       TRUE    },
    {   "jeweled",       WEAPON_JEWELED,       FALSE   },
    {   "wounds",	 WEAPON_WOUNDS,	       TRUE    },
    {   NULL,              0,                  0       }
};	


const struct flag_type socket_flags[] =
{
    {   "weapon",	SOCKET_WEAPON,	      TRUE    },
    {   "armor",        SOCKET_ARMOR,         TRUE    },
    {   NULL,              0,                  0       }
};	

const struct flag_type ranged_type[] =
{
  {	"fast",		RANGED_FAST,		TRUE },
  {	"longrange",	RANGED_LONGRANGE,	TRUE },
  {	"passdoor",	RANGED_PASSDOOR,	TRUE },
  {	"armorpierce",	RANGED_ARMORPIERCE,	TRUE },
  {	"slow",		RANGED_SLOW,		TRUE },
  {	"unlimited",	RANGED_UNLIMITED,	TRUE },

  {   NULL,              0,                  0       }
};

/* the last number refers to the max number of shots/round
   used to prevent cases where too powerful weapon/ammo combos
   are introduced
*/
const struct flag_type projectile_type[] =
{
    {   "arrow",       PROJECTILE_ARROW,       TRUE	},
    {   "bolt",        PROJECTILE_BOLT,        TRUE	},
    {   "stone",       PROJECTILE_STONE,       TRUE	},
    {   "quarrel",     PROJECTILE_QUARREL,     TRUE	},
    {   "dart",        PROJECTILE_DART,        TRUE	},
    {   "spear",       PROJECTILE_SPEAR,       TRUE	},
    {   "blade",       PROJECTILE_BLADE,       TRUE	},
    {   "bullet",      PROJECTILE_BULLET,       TRUE	},
    {   NULL,	       0,		       FALSE    }   
};

const struct flag_type proj_spec_type[] =
{
    {   "poison",         PROJ_SPEC_POISON,       TRUE    },  
    {   "plague",         PROJ_SPEC_PLAGUE,       TRUE    },  
    {   "sharp",          PROJ_SPEC_SHARP,        TRUE    },
    {   "blunt",	  PROJ_SPEC_BLUNT,        TRUE    },
    {   "paralyze",       PROJ_SPEC_PARALYZE,     TRUE    },
    {   "flaming",        PROJ_SPEC_FLAMING,      TRUE    },
    {   "frost",          PROJ_SPEC_FROST,        TRUE    },
    {   "sleep",          PROJ_SPEC_SLEEP,        TRUE    },
    {   "barbed",         PROJ_SPEC_BARBED,       TRUE    },
    {   "shock",          PROJ_SPEC_SHOCK,        TRUE    },
    {   "explosive",	  PROJ_SPEC_EXPLODE,      TRUE    },
    {   "vorpal",	  PROJ_SPEC_VORPAL,	  TRUE    },
    {   "ghostly",	  PROJ_SPEC_PASSDOOR,	  TRUE    },
    {   NULL,          0,                      FALSE    }
};                                                         

const struct flag_type res_flags[] =
{
    {   "summon",        RES_SUMMON,           TRUE    },
    {   "charm",         RES_CHARM,            TRUE    },
    {   "magic",         RES_MAGIC,            TRUE    },
    {   "weapon",        RES_WEAPON,           TRUE    },
    {   "bash",          RES_BASH,             TRUE    },
    {   "pierce",        RES_PIERCE,           TRUE    },
    {   "slash",         RES_SLASH,            TRUE    },
    {   "fire",          RES_FIRE,             TRUE    },
    {   "cold",          RES_COLD,             TRUE    },
    {   "lightning",     RES_LIGHTNING,        TRUE    },
    {   "acid",          RES_ACID,             TRUE    },
    {   "poison",        RES_POISON,           TRUE    },
    {   "negative",      RES_NEGATIVE,         TRUE    },
    {   "holy",          RES_HOLY,             TRUE    },
    {   "energy",        RES_ENERGY,           TRUE    },
    {   "mental",        RES_MENTAL,           TRUE    },
    {   "disease",       RES_DISEASE,          TRUE    },
    {   "drowning",      RES_DROWNING,         TRUE    },
    {   "light",         RES_LIGHT,            TRUE    },
    {   "sound",         RES_SOUND,            TRUE    },
    {   "wood",          RES_WOOD,             TRUE    },
    {   "silver",        RES_SILVER,           TRUE    },
    {   "iron",          RES_IRON,             TRUE    },
    {   "air",           RES_AIR,              TRUE    },
    {   NULL,            0,                    0       }
};

const struct flag_type vuln_flags[] =
{
    {   "summon",        VULN_SUMMON,          TRUE    },
    {   "charm",         VULN_CHARM,           TRUE    },
    {   "magic",         VULN_MAGIC,           TRUE    },
    {   "weapon",        VULN_WEAPON,          TRUE    },
    {   "bash",          VULN_BASH,            TRUE    },
    {   "pierce",        VULN_PIERCE,          TRUE    },
    {   "slash",         VULN_SLASH,           TRUE    },
    {   "fire",          VULN_FIRE,            TRUE    },
    {   "cold",          VULN_COLD,            TRUE    },
    {   "lightning",     VULN_LIGHTNING,       TRUE    },
    {   "acid",          VULN_ACID,            TRUE    },
    {   "poison",        VULN_POISON,          TRUE    },
    {   "negative",      VULN_NEGATIVE,        TRUE    },
    {   "holy",          VULN_HOLY,            TRUE    },
    {   "energy",        VULN_ENERGY,          TRUE    },
    {   "mental",        VULN_MENTAL,          TRUE    },
    {   "disease",       VULN_DISEASE,         TRUE    },
    {   "drowning",      VULN_DROWNING,        TRUE    },
    {   "light",         VULN_LIGHT,           TRUE    },
    {   "sound",         VULN_SOUND,           TRUE    },
    {   "wood",          VULN_WOOD,            TRUE    },
    {   "silver",        VULN_SILVER,          TRUE    },
    {   "iron",          VULN_IRON,            TRUE    },
    {   "mithril",       VULN_MITHRIL,            TRUE    },
    {   "air",           VULN_AIR,            TRUE    },
    {   NULL,              0,                    0     }
};

const struct flag_type position_flags[] =
{
    {   "dead",           POS_DEAD,            FALSE   },
    {   "mortal",         POS_MORTAL,          FALSE   },
    {   "incap",          POS_INCAP,           FALSE   },
    {   "stunned",        POS_STUNNED,         FALSE   },
    {   "meditate",       POS_MEDITATE,        TRUE    },
    {   "sleeping",       POS_SLEEPING,        TRUE    },
    {   "resting",        POS_RESTING,         TRUE    },
    {   "sitting",        POS_SITTING,         TRUE    },
    {   "fighting",       POS_FIGHTING,        FALSE   },
    {   "standing",       POS_STANDING,        TRUE    },
    {   NULL,             0,                   0       }
};

const struct  flag_type       apply_types     []      =
{
      {       "affects",      TO_AFFECTS,     TRUE    },
      {       "object",       TO_OBJECT,      TRUE    },
      {       "immune",       TO_IMMUNE,      TRUE    },
      {       "resist",       TO_RESIST,      TRUE    },
      {       "vuln",         TO_VULN,        TRUE    },
      {       "weapon",       TO_WEAPON,      TRUE    },
      {       NULL,           0,              TRUE    }
};

const struct  bit_type        bitvector_type  []      =
{
      {       affect_flags,   "affect"        },
      {       apply_flags,    "apply"         },
      {       imm_flags,      "imm"           },
      {       res_flags,      "res"           },
      {       vuln_flags,     "vuln"          },
      {       weapon_type2,   "weapon"        }
};


char *  const                   clanr_table     [MAX_CLANR] [2] =
{
  /* WHO and CHAT strings*/
  {  "I",		"Inductee"	},
  {  "M",		"Member"	},
  {  "V",		"Veteran"	},
  {  "T",		"Trusted"	},
  {  "E",		"Elder"		},
  {  "L",		"Leader"	}
};


char *  const                   rank_table     [MAX_CABAL][6][2] =
{
    {
	{ "",		""	},
	{ "",		""	},
	{ "",		""	},
	{ "",		""	},
	{ "",		""	},
	{ "",		""	}
    },
    // Knights
    {
	{ "",		""	},
	{"",		""	},
	{"Elder",	"Elder"	},
	{"Leader",	"Leader"},

	{"",		""	},
	{"",		""	}
    },
    // Legion
    {
	{ "",		""	},
	{ "",		""	},
	{ "Elder",	"Elder"	},
	{ "Leader",	"Leader"},
	{ "",		""	},
	{ "",		""	}
    },
    // Justice
    {
	{ "",	"" },
	{ "",	"" },
	{ "Elder",	"Elder"	},
	{ "Leader",	"Leader"},
	{ "",	"" },
	{ "",	"" }
    },
    // Warlord
    {
	{ "",	""	},
	{ "",	""	},
	{ "Elder",	"Elder"	},
	{ "Leader",	"Leader"},
	{ "",	""	},
	{ "",	""	}
    },
    // Gimp
    {
	{ "",		""	},
	{ "Grease",		"Grease"	},
	{ "Leather",		"Leather"	},
	{ "Buttplug",		"Buttplug"	},
	{ "Wasabi",		"Wasabi"	},
	{ "Yui",		"Yui"	}
    },
    // Herald
    {
	{ "",		""	},
	{ "",	""	},
	{ "Elder",		"Elder"	},
	{ "Leader",	"Leader"},
	{ "",	""	},
	{ "",	""	}
    },
    // Battle
    {
	{ "",		""	},
	{ "",		""	},
	{ "Elder",	"Elder"	},
	{ "Leader",	"Leader"},
	{ "",		""	},
	{ "",		""	}
    },
    // Master
    {
	{ "",		""	},
	{ "",		""	},
	{ "Elder",	"Elder"	},
	{ "Leader",	"Leader"},
	{ "",		""	},
	{ "",		""	}
    },
    // Entropy
    {
	{ "",		""	},
	{ "",		""	},
	{ "Elder",	"Elder"	},
	{ "Leader",	"Leader"},
	{ "",		""	},
	{ "",		""	}
    },
    // Shadow
    {
	{ "",		""	},
	{ "",		""	},
	{ "Elder",	"Elder"	},
	{ "Leader",	"Leader"},
	{ "",		""	},
	{ "",		""	}
    },
    // Assassin
    {
	{ "",		""	},
	{ "",		""	},
	{ "Elder",	"Elder"	},
	{ "Leader",	"Leader"},
	{ "",		""	},
	{ "",		""	}
    },
    // Mystic
    {
	{ "",		""	},
	{ "",		""	},
	{ "Elder",	"Elder"	},
	{ "Leader",	"Leader"},
	{ "",		""	},
	{ "",	""	}
    },
    {
	{ "",		""	},
	{ "",		""	},
	{ "Elder",	"Elder"	},
	{ "Leader",	"Leader"},
	{ "",		""	},
	{ "",		""	}
    }
};

const struct tarot_type tarot_table[] =
{
    { "[ None ]" },
    { "[ Fool ]" },
    { "[ Magician ]" },
    { "[ High Priestess ]" },
    { "[ Empress ]" },
    { "[ Emperor ]" },
    { "[ Hierophant ]" },
    { "[ Lovers ]" },
    { "[ Chariot ]" },
    { "[ Strength ]" },
    { "[ Hermit ]" },
    { "[ Wheel of Fortune ]" },
    { "[ Justice ]" },
    { "[ Hanged Man ]" },
    { "[ Death ]" },
    { "[ Temperance ]" },
    { "[ Devil ]" },
    { "[ Tower ]" },
    { "[ Star ]" },
    { "[ Moon ]" },
    { "[ Sun ]" },
    { "[ Judgement ]" },
    { "[ World ]" }
};
/* warning text for automated warnings */
/* subj, text */
/* %s in subj represents character name */
const struct warn_type warn_table[] =
{
  {
"Excessive PK warning for: %s.",
"You have been noticed to engage in excessive player killing for your\r\n level.  While Forsaken Lands is a Player Killing and Role-playing MUD, \r\n our goal and mission is for all players to better themselves in both\r\n categories and enjoy themselves.\r\n\r\n Since PK combat at low ranks is only meaningful as a learning tool, and \r\n you seem to be skilled enough to kill with ease the following now applies\r\n to your character starting as of now:\r\n\r\n 1) You are to avoid any player to player combat until past the rank of 25.\r\n 2) You are advised to rank quickly and behave according to the FL rules \r\n    (\"help rules\") and hence avoid drawing negative attention to yourself .\r\n\r\n Failure to abide by these two simple rules will result in strict penalties \r\n to your character and or your site.\r\n\r\n FL Staff.\r\n"
  },
  {
"Excessive PK warning issed to: %s.",
"The above player has been found to have 6 or more PK kills below level 20.\r\n An automatic note has been issued quoted below, and a [`!W``] flag has been\r\n attached (\"who warned\").\r\n\r\n ---------Important parts of note are as follows -------\r\n Following applies to your character starting as of now:\r\n\r\n 1) You are to avoid any player to player combat until past the rank of 25.\r\n 2) You are advised to rank quickly and behave according to the FL rules \r\n    (\"help rules\") and hence avoid drawing negative attention to yourself .\r\n\r\n Failure to abide by these two simple rules will result in strict penalties \r\n to your character and or your site.\r\n\r\n FL Staff.\r\n"
  },
  {
    "PK warning cleared for %s",
"As the player has reached the level of 25 or more, the PK warning flag [`!W``]\r\n has been removed.\r\n"
  },

  {
    "%s has PK'ed while on PK warning!",
"The above player has killed another player while being flagged for\r\n excessive lowbie pking.  The original note states:\r\n\r\n Following applies to your character starting as of now:\r\n\r\n 1) You are to avoid any player to player combat until past the rank of 25.\r\n 2) You are advised to rank quickly and behave according to the FL rules\r\n    (\"help rules\") and hence avoid drawing negative attention to yourself .\r\n\r\n Failure to abide by these two simple rules will result in strict penalties \r\n to your character and or your site.\r\n\r\n FL Staff.\r\n"
  },
  {
    "%s has come under protection of %s!",

" Let it be known to all whom reside in this fair city, that your Royal Leaders\r\n have formed a trade pact with our forces, which shall patrol your streets on\r\n the lookout for any actions decided as criminal.\r\n\r\n Become familiar with the presence of `#\"Law\"`` in your streets and be aware\r\n that the help scrolls of `#\"help law\"`` may aid you greatly.\r\n"
  },
  {
    "The bounty on %s has been fulfilled.",

" As per your request the indicated target has been relived \r\n of their mortal coil.\r\n\r\n The original creator of the bid selected by the bounty \r\n hunter has received the deceased limited items if any.  \r\n\r\n The [`8SYNDICATE``] thanks you for your continuing patronage.\r\n"
  },

  {
    "Good vs Good Pk.  %s has killed %s.",

" The specified good aligned player is over level 15 and has taken \r\nthe life of another good aligned mortal.\r\n"
  },

  {
    "%s:%s",
" A new tome has been added to the world famous Library of Aabahran.\r\nOn details of how you may glean the contents of the tomes in the collection\r\nconsult \"HELP LIBRARY\".\r\n\r\nThe Library Staff.\r\n"
  }

};

/* 
   what follows is the table that orginizes the information used to seed
   objects into random places of the game. (crusader obelisks, starstones etc.)
   
   Fields are as follows:

   vnum :  vnum obj object to seed
   chance: chance for this object to be seeded on reboot

   amin: minimum (>=) AREA vnum to choose from (0 to ignore)
   amax: max AREA (<=) vnum to choose from (0 to ignore)

   rmin: minimum ROOM vnum to that area must contain
   rmax: max ROOM vnum to that area must contain

   i_area: array of area vnum's to include	(NULL to ignore)
   ia_num: length of the includ array		

   e_area: array of area vnum's to exclude	(NULL to ignore)
   ea_num: length of the includ array

   i_sect: array of sectors requried		(NULL to ignore)
   is_num: length of the sector array

   e_sect: array of sectors excluded		(NULL to ignore)
   es_num: length of the sector array

   i_rom1: array of room1 bits required		(NULL to ignore)
   ir1_num:length of above array

   e_rom1: array of room1 bits excluded		(NULL to ignore)
   er1_num: length of above array

   i_rom2: array of room1 bits required		(NULL to ignore)
   ir2_num:length of above array

   e_rom2: array of room1 bits excluded		(NULL to ignore)
   er2_num: length of above array

   int pool: number of areas to use as a pool (top_area/10 maximum)

   fExit: TRUE to check for avaliable exit in the room as a requirement

   The function cheats a bit by selecting rooms only out of a pool
   of random areas, rather then out of the whole mud (far to costly)
   Then a list of rooms using above requierements is compiled and one room
   is seleceted.

   FOLLOWING AUTOMATICLY REJECT A ROOM/AREA
   - area: nogate
   - room: safe
   - room: private
   - room: jailcell
   - room: solitary
*/

struct objseed_type	objseed_table[] = 
{
/* RIGHTESOUNESS */
  {114, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "Order of Crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* DIVINE VOID */
  {115, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* INSIGHT */
  {116, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* DIVINE MIGHT (Olympus, Caranduin, Seryb) */
  {122, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {"The Cloud Kingdom", "Caranduin", "Temple of Sreyb"},4,//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* SHADOW BANE */
  {118, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {"Xymerria", "The Western Underdark", "The Eastern Underdark", "Duergar City", "The Illithid Hive"}, 5,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
  /* PURE WATER */
  {119, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {"Elemental Canyon", "Crystalmir Lake", "The River Wood", "Tainted Valley", "The Dead Sea" }, 5,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {SECT_WATER_SWIM, SECT_WATER_NOSWIM}, 2,	//sectors required  
   {SECT_AIR, SECT_LAVA}, 2,			//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* ICEFIRE */
  {120, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,				//amin, amax, rmin, rmax
   {"Fortress of Ice"}, 1,				//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM}, 3,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* SEARING MIGHT */
  {121, 1, 100,					//vnum, amount chance, 
   0,0, 10800,10900,				//amin, amax, rmin, rmax
   {"Fortress of Fire"}, 1,			//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM}, 3,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* HOLY WRATH */
  {117, 1, 100,					//vnum, amount chance, 
   0,0, 5484,5484,				//amin, amax, rmin, rmax
   {"Clockwork Gauntlet"}, 1,			//areas by name to select from
   {"valmiran", "rheydin", "miruvhor"},0,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* VITALITY */
  {123, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* SWIFT RETRIBUTION */
  {124, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* VIRTUE */
  {125, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* PURITY */
  {126, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* CONSTITUTION */
  {127, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* PALISONS EYE */
  {128, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* FEASTING */
  {129, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* FINAL PARTING */
  {130, 1, 100,					//vnum, amount chance, 
   0,0, 13209,13209,				//amin, amax, rmin, rmax
   {"End of World"}, 1,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* DIVINE HEALTH (GASTERIDE) */
  {131, 1, 100,					//vnum, amount chance, 
   0,0, 4858,4858,				//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* Vicarious Redemption */
  {132, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {"Temple of Sreyb", "Ofcol", "Thalos", "Elderwood", "New Ofcol", "Shire", "Winter", "Isile of Exile", "Tarandue"}, 9,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* ONE GODS TRIUMPH (OLYMPUS) */
  {133, 1, 100,					//vnum, amount chance, 
   0,0, 905,905,				//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* The TRIUMPH of Sirant (Tainted Valley, essence of evil)*/
  {134, 1, 100,					//vnum, amount chance, 
   0,0, 3683,3683,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* THE TRIUMPH OF PALISON (By vald in the mansion) */
  {135, 1, 100,					//vnum, amount chance, 
   0,0, 3273,3273,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* UNDENIABLE ORDER (Towr at the End of Time)*/
  {136, 1, 100,					//vnum, amount chance, 
   0,0, 10547,10547,				//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* Inner Strength (DISCHORD) */
  {137, 1, 100,					//vnum, amount chance, 
   0,0, 4985,4990,				//amin, amax, rmin, rmax
   {"Dischord"}, 1,				//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* Baptism (Inside the Order of Crusades) */
  {138, 1, 100,					//vnum, amount chance, 
   0,0, 6413,6413,				//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "miruvhor"},2,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },
/* INNER PEACE */

  {139, 1, 100,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {"Elderwood"}, 1,				//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_LAVA}, 4,	//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   5, TRUE					//area pool size, fExit
  },

/* STARSTONE */
  {141, 20, 80,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {"valmiran", "rheydin", "miruvhor", "order of crusades"},4,	//areas by name to exclude
   {0}, 0,					//sectors required
   {SECT_INSIDE, SECT_CITY, SECT_AIR, SECT_WATER_SWIM, SECT_WATER_NOSWIM, SECT_AIR}, 6,					//sectors excluding choice
   {0}, 0,					//room1 flags required
   {ROOM_INDOORS}, 1,				//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   10, TRUE					//area pool size, fExit
  },
/* TERMINATOR */
  {0, 0, 0,					//vnum, amount chance, 
   0,0, 0,0,					//amin, amax, rmin, rmax
   {NULL}, 0,					//areas by name to select from
   {NULL}, 0,					//areas by name to exclude
   {0}, 0,					//sectors required
   {0}, 0,					//sectors excluding choice
   {0}, 0,					//room1 flags required
   {0}, 0,					//room1 flags excluding
   {0}, 0,					//room2 flags required
   {0}, 0,					//room2 flags excluding
   0, TRUE					//area pool size, fExit
  }
};

/* used for info on modifications to armors */
/* name, level, bit, ident, dur, weight,     fPart, fSame, fRare, stones */
const struct amodify_type	amodify_table[] = {
/* 0 RETURN (LEVEL CONTAINS SIZE OF THIS TABLE!) */
  {"",		30, 0,  0,  0, 0,	TRUE, TRUE, TRUE,   0,
   ""},

/* LEVEL 0 (always) */
/*  name   level  bit id dur weight     part   same   rare  stones */
  {"repairs",	0, A,  1, 2, 10,	FALSE, FALSE, FALSE,  0,
   "Field repair of armors. Success and effect dependant on level." },
  {"rivets",	0, B,  2, 2, 15,	TRUE, FALSE, FALSE,   0,
   "Addition of studs into the armor plating to increase combat performance."},
  {"balance",	0, B,  3, 3,-15,	TRUE, FALSE, FALSE,   0,
   "Balancing of the weight distribution in the piece to increase aim."},
  {"padding",	0, C,  4, 1, 10,	FALSE, FALSE, FALSE,  0,
   "Soft leather padding for under armor, prevents chafing and bruises."},

/* LEVEL 1 (HISTORY OF ARMAMENTS) */
  {"conversion",1, D,  5, 4, 0,		FALSE, FALSE, FALSE,  0,
   "Process of converting the structure of the piece to allow for further modficiations."},
  {"plating",	1, E,  6, 3, 15,	TRUE, FALSE,  FALSE,   0,
   "Attatchment of heavy plates to greatly increase protection."},
  {"stardust",	1, F,  7, 2,  0,	FALSE, FALSE,  FALSE,   1,
   "Treatment of armor with starstone dust to better magical protection."},

/* LEVEL 2 (ARMOR ENHANCEMENT) CAN ONLY BE DONE ON CONVERTED PIECES */
/*  name   level  bit id dur weight     part   same   rare  stones */
  {"barbs",	2, G,  8, 3, 30,	TRUE, FALSE, FALSE,  2,
   "Addition of sharp barbs, studs and edges to make the piece more useful in damage dealing."},
  {"joints",	2, G,  9, 5, 15,	TRUE, FALSE, FALSE,  1,
   "Modification of the armor to allow for more flexibility and ease of aim in combat."},
  {"buckles",	2, G, 10, 3, 10,	TRUE, FALSE, FALSE,  1,
   "Addition of fitted buckles, joists and supports to make the armor more effective and balanced."},
  {"deathward",	2, G, 11, 6,-10,	TRUE, FALSE, FALSE,  2,
   "Technique of welding the starstone into the armor, creating wards protecting from various maladies."},
  {"hellward",	2, G, 12, 6,-10,	TRUE, FALSE, FALSE,  2,
   "Technique of welding the starstone into the armor, creating wards protecting from heat and stress."},
  {"mindward",	2, G, 13, 6,-10,	TRUE, FALSE, FALSE,  2,
   "Technique of welding the starstone into the armor, creating wards protecting mind and body."},
  {"enhancement",2, H, 14,9, -15,	TRUE, TRUE, TRUE,  1,
   "Various enhancements and improvements that depend on the type of armor."},

/* LEVEL 3 (ARMOR CRAFT) CAN ONLY BE DONE ON CONVERTED PIECES */
/*  name   level  bit id dur weight     part   same   rare  stones */
  {"lamination", 3, I,  15, 6, -10,	TRUE, FALSE, FALSE,    1,
   "Application of fine layers of starstone mixed with steel to achive perfection in plating."},
  {"starrunes", 3, J,  16, 7, -5,	FALSE, FALSE, FALSE, 3,
   "Addition of fine starstone runes into the armor to improve magical defense."},
  {"warplates", 3,  K,  17, 5, 50,	TRUE, FALSE, TRUE,    3,
   "Addition of massive jagged plates and dangerous wards to achive perfection in damage."},
  {"battlemail", 3, K,  18, 6, 40,	TRUE, FALSE, TRUE,    2,
   "Conversion into chain mail using fine workings of starstone to achive mobility and power."},
  {"starseam,",  3, K,  19, 7,-10,	TRUE, FALSE, FALSE,   4,
   "Wonderous techinque of sewing the piece with thread of starstone for extreme flexibility."},
  {"blessing",  3, L,  20, 3, 0,	FALSE, FALSE, FALSE, 2,
   "Religious teachings for the Order of Crusades are used to bless the piece against maladies."},
  {"heatproofing", 3,L,21, 3, 0,	TRUE, FALSE, FALSE,   0,
   "Application of heat resistant layers to further protect the wearer against heat and stress."},
  {"embossing",  3, L,  22, 4, 0,	FALSE, FALSE, FALSE,    1,
   "Historical and thematic engravings etched into the armor help with morale and mental resistance."},
  {"spellward",  3, K,  23, 9, 5,	TRUE, FALSE, FALSE,  5,
   "Attatchment of massive starstone warding that can protect against any magic."},
/*  name   level  bit id dur weight     part   same   rare  stones */
  {"armorcraft", 3, M, 24, 11,-12,	TRUE, TRUE, TRUE,    5,
   "Use of the finest of teachings of the Order to improve the armor beyond means of ordinary artisans."},
  {"hellplating",3,N, 25,   9, 30,	TRUE, FALSE, TRUE,     2,
   "A process bordering on heresey which causes the armor to become nearly indestructible."},
  {"starweight", 3,N, 26,  9,-60,	TRUE, FALSE, FALSE,     5,
   "Use of the natural property of starstone to greatly reduce the weight of armor."},
  {"starplating",3,N, 27,  9, 0,	TRUE, FALSE, TRUE,    6,
   "The strongest of armor platings, its protective abilities are stuff of legends."},
  {"starmark",   3,N, 28,  9, 0,	TRUE, FALSE, TRUE,     1,
   "Addition of a starlaced engraving to allow socketing of attachments to the armor."},
  {"signature",  3,N, 29,  19, 0,	FALSE, FALSE, FALSE,   12,
   "Using a secret wonderous solder of starstone dust, a master armorer may add his signature to the armor."},

/* TERMINATOR */
  {NULL,	0, 0,  0, 0,  0,	TRUE, TRUE, TRUE,   0, ""}
};


/* used for info on modifications to weapons */
/* name, level, hlevel, bit, ident, dur, weight,     fPart, fRare, stones */
const struct wmodify_type	wmodify_table[] = {
/* 0 RETURN (LEVEL CONTAINS SIZE OF THIS TABLE!) */
  {"",		 56, 0,  0,  0,  0, 0,		TRUE, TRUE,    0,
   ""},

/* LEVEL 0 (always) */
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"repairs",	0,	0,    A,   1,   3,   0,      FALSE,  FALSE,  0,
   "Field repair of weapons. Success and effect dependant on level." },
  {"grinding",	0,	0,    B,   2,   3,   0,      FALSE,  FALSE,  0,
   "Improvement of the blade by rough grinding of the edge and point."},
  {"adjustment",	0,	0,    B,   3,   3,  -10,      FALSE,  FALSE,  0,
   "Improvement of aim by fitting the hilt to its owner's hand."},
  {"consecration",0,	0,    C,   4,   5,   -20,      FALSE,  FALSE,  1,
   "A holy ceremony that blesses the weapon allowing it to be further improved."},
/* HOLY WEAPON EFFECTS */
  /* LEVEL 1*/
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"sharpening",	0,	1,    D,   5,   3,   0,      FALSE,  FALSE,  0,
   "A standard grinding of the edge and point to improve damage."},
  {"balance",	0,	1,    D,   6,   3,  -10,      FALSE,  FALSE,  0,
   "Balancing of the blade to improve aim and handling."},
  {"ornate hilt",0,	1,    E,   7,   4,   1,      TRUE,   FALSE,  0,
   "Addition of a longer, decorated hilt which allows for stronger grasp."},
  {"ornate pommel",0,	1,    E,   8,   4,   1,      TRUE,   FALSE,  0,
   "Addition of perfectly balanced pommel marked by the Order of the Crusades."},
  /* LEVEL 2 */
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"honing",	0,	2,    F,   9,   4,   0,      FALSE,  FALSE,  0,
   "A process of careful sharpening of the blade to allow for better damage."},
  {"folding",	0,	2,    F,  10,   4,   2,      TRUE,  FALSE,  0,
   "Technique of folding the metal on the point of the weapon to improve penetration."},
  {"leathering",0,	2,    G,  11,   4,   2,      TRUE,   FALSE,  0,
   "The hilt of the weapon is bound in blessed and cured leather to absorb shock of combat."},
  {"minor blessing",0,	2,    G,  12,   4,   0,      FALSE,   FALSE,  0,
   "A minor blessing placed on the weapon helps to alleviate wounds caused in combat."},

  /* LEVEL 3 */
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"longblade",	0,	3,    H,  13,   4,   5,     TRUE,  TRUE,   0,
   "An addition of longer blade or haft, increasing reach and usability."},
  {"weight",	0,	3,    H,  14,   4,   10,     TRUE,  FALSE,  0,
   "Addition of extra weight into the blade or haft to allow for better ability to block weapons."},
  {"blessing",  0,	3,    I,  15,   4,   0,      FALSE,   FALSE,  0,
   "A standard blessing uttered on weapons and armor by Order's Patron Saint."}, 
  {"stargrind",0,	3,    I,  16,   4,  -15,      FALSE,   FALSE,  1,
   "Use a startstone to grind the edge of the weapon to instil extremely sharp edge to it."},

  /* LEVEL 4 */
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"knife guards",0,	4,    J,  17,   5,   4,     TRUE,  FALSE,   1,
   "An addition of pointed, sharp guards to the weapon."},
  {"carbide",	0,	4,    J,  18,   4,  -10,     TRUE,  FALSE,   1,
   "Mixing and grinding of carbide into the edge and point to improve damage and penetration."},
  {"starhilt",	0,	4,    J,  19,   5,  -15,    TRUE,  FALSE,   1,
   "Reforging of the hilt out of almost pure starstone to give excelent handling to the weapon."},
  {"cuffs",     0,	4,    K,  20,   4,   0,     TRUE,  TRUE,    1,
   "Forged from starstone a fine sikly chain is attached to the weapon to prevent its loss in combat."},
  {"starforge",0,	4,    K,  21,   6,  -25,     TRUE,   FALSE,  3,
   "The damage dealing edges of the weapon are completely reforged from starstone."},

  /* LEVEL 5 */
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"blessing of accuracy",
		0,	5,    L,  22,   5,   0,    FALSE,  FALSE,  2,
   "A powerful blessing empowering the user's aim and handling in combat."},
  {"blessing of resistance",  
		0,	5,    L,  23,   5,   0,    FALSE,  FALSE,  2,
   "A powerful blessing helping to resist the shock of blocking an attack."},
  {"blessing of health",  
		0,	5,    M,  24,   5,   0,    FALSE,  FALSE,  2,
   "A powerful blessing which helps the user's constitution."},
  {"blessing of vitality",  
		0,	5,    M,  25,   5,   0,    FALSE,  FALSE,  2,
   "A powerful blessing which helps the user's vitality."},

  /* LEVEL 6 */
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"serrations",
		0,	6,    N,  26,   6,   6,    TRUE,  TRUE,  0,
   "Additions of dangerous serrations onto the blade or the haft."},
  {"spacers",
		0,	6,    N,  27,   8,  -50,    TRUE,  TRUE,  2,
   "Creation of holes and spaces reinforced with starstone to reduce weight and imporve handling."},
  {"elongation",
		0,	6,    N,  28,   6,    0,    TRUE,  TRUE,  2,
   "Using starstone for support the weapon is further elongated increasing reach."},
  {"starplating",
		0,	6,    O,  29,   10,   6,    FALSE,  FALSE,  5,
   "A wonderous blessing combined with plating of starstone makes the weapon nearly indestructible."},
  {"corrosion",
		0,	6,    O,  30,   10,   6,    FALSE,  FALSE,  8,
   "Using extremely rare particles of starstone, the weapon's edge is made to leak glowing toxins."},
  {"prayer of health",
		0,	6,    P,  31,   6,   0,    FALSE,  FALSE,  0,
   "A prayer of healing chanted by the Patron Saint of the Order."},
  {"prayer of vitality",
		0,	6,    P,  32,   6,   0,    FALSE,  FALSE,  0,
   "A prayer of vitality chanted by the Patron Saint of the Order."},
  {"psalm of Sacrifice",
		0,	6,   aa,  35,   7,    0,    FALSE,  FALSE,  8,
   "Inscription of the history of Sacrifice made by Order of Light to trap the Nameless."},

  /* LEVEL 7 */
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"prayer of accuracy",
		0,	7,    Q,  33,   7,    0,    FALSE,  FALSE,  3,
   "A prayer of accuracy inscribed with starstone in the haft or blade of the weapon."},
  {"prayer of resistance",
		0,	7,    Q,  34,   7,    0,    FALSE,  FALSE,  3,
   "A prayer of resistance inscribed with starstone in the haft or blade of the weapon."},

  {"signature",	0,      7,    R,  53,   6,   0,   FALSE, FALSE,     8,
   "Using a secret wonderous solder of starstone dust, a master smith may add his signature to the weapon."},

  {"psalm of the Order",
		0,	7,    R,  36,   7,    0,    FALSE,  FALSE,  8,
   "Inscription of a holy psalm describing the mission of the Order of Crusades."},

  /* LEVEL 8 */
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"psalm of retribution",
		0,	8,    S,  37,   8,    0,    TRUE,  TRUE,  4,
   "Addition of inscription of Psalm of Retribution."},
  {"psalm of equality",
		0,	8,    S,  38,   8,    0,    TRUE,  TRUE,  4,
   "Addition of inscription of Psalm of Equiality."},
  {"psalm of balance",
		0,	8,    S,  39,   8,    0,    TRUE,  TRUE,  4,
   "Addition of inscription of Psalm of Balance."},
  {"reliquary of health",
		0,	8,    T,  40,   8,    0,    TRUE,  TRUE,  1,
   "A powerful holy artifact of health set in the pommel of the weapon."},
  {"reliquary of vitality",
		0,	8,    T,  41,   8,    0,    TRUE,  TRUE,  1,
   "A powerful holy artifact of vitality set in the pommel of the weapon."},

  /* LEVEL 9 */
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"the Nail of Orealt",
		0,	9,    U,  42,   9,    0,    FALSE,  FALSE,  1,
   "A portion of the great Elf's nail, contained in starstone seal."},
/* END OF HOLY WEAPON EFFECTS */

/* LEVEL 1 EFFECTS */
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"wideguard",	2,	0,    V,  43,   4,   6,    TRUE,  TRUE,  1,
   "Addition of very wide guard on the weapon allowing for better defense."},
  {"twohands",	2,	0,    W,  44,   4,   25,    TRUE,  FALSE, 0,
   "Basic operation of converting a one handed weapon for use in two hands."},
  {"spiked",	2,	0,    X,  45,   2,   5,    TRUE,  FALSE, 0,
   "Basic operation of adding a large spikard into the hilt or pommel"},
  {"barbed",	2,	0,    X,  46,   2,   5,    TRUE,  FALSE, 0,
   "Basic operation of adding barbs into the guards, shaft or blade."},

/* LEVEL 2 EFFECTS */
/* name,      level, hlevel, bit, ident, dur, weight,  fPart, fRare, stones */
  {"wideblade",	3,	3,    Y,  47,   6,   25,    TRUE,  TRUE,  1,
   "Addition of very wide blade improving effects of battering an opponent."},
  {"greatblade",3,	3,    Y,  48,   6,   35,    TRUE,  TRUE,  1,
   "Addition of extremely heavy, large blade, improving effects of armored rush."},
  {"blunttip",	3,	3,    Y,  49,   6,   -5,    TRUE,  TRUE,  1,
   "Modification of the weapon to best suit it for execution purposes."},
  {"staredge",	3,	5,    Z,  50,   8,  -15,    TRUE,  TRUE,  5,
   "Conversion of the edges into starstone alloy allowing the weapon to cut through steel like butter."},
  {"starblade",	3,	5,    Z,  51,   8,  -75,    TRUE,  TRUE,  10,
   "Conversion of the blade into starstone blessing it with the properites of the stone."},
  {"starguard",	3,	5,    Z,  52,   6,   5,    TRUE,  TRUE,    7,
   "Addition of special guard-guantlet which allows the weapon to be attached to the hand."},
  {"starmark",	3,	8,   cc,  55,  10,   0,   FALSE, FALSE,   8,
   "A process of creating a starstone based socket for weapon attatchements."},
  {"stargem",	3,	9,   bb,  54,  20,   0,   FALSE, FALSE, 10,
   "A theory put forth by the Order's Master Smith about the abilities of\n\r extremely rare sort of starstone. As of now it is still untested."},
/* TERMINATOR */
  {NULL, 0,  0,  0,  0, 0, 0,		TRUE, TRUE,    0,
   ""},
};


const struct  flag_type       cont_table     []      =
{
  {       "dispel",     CONT_DISPEL,    TRUE    },
  {       "summon",     CONT_SUMMON,    TRUE    },
  {       "attack",     CONT_ATTACK,    TRUE    },
  {       "defense",    CONT_ATTACKED,  TRUE    },
  {       "health",	CONT_HP30,	TRUE	 },
  {       "mind",	CONT_MN30,	TRUE	 },  
    {   NULL,             0,                   0       }
};


const struct flag_type fletcher_type[] =		//skill
{
  {   "sharp",		PROJ_SPEC_SHARP,        TRUE    },//80
  {   "flaming",        PROJ_SPEC_FLAMING,      TRUE    },//82
  {   "frost",          PROJ_SPEC_FROST,        TRUE    },//84
  {   "shock",          PROJ_SPEC_SHOCK,        TRUE    },//86
  {   "blunt",          PROJ_SPEC_BLUNT,        TRUE    },//88
  {   "vorpal",         PROJ_SPEC_VORPAL,       TRUE    },//90
  {   "explode",	PROJ_SPEC_EXPLODE,      TRUE    },//92
  {   "poison",         PROJ_SPEC_POISON,       TRUE    },//94
  {   "sleepy",         PROJ_SPEC_SLEEP,        TRUE    },//96
  {   "diseased",       PROJ_SPEC_PLAGUE,       TRUE    },//98
  {   "magic",          PROJ_SPEC_PASSDOOR,     TRUE    },//100

  {   "",		0		,       TRUE    },
  {   NULL,          0,                      FALSE    }
};                                                         

/* detailed creation choices, all using chargen_table_s
 * name, string, int1 int2 */
struct chargen_choice_s chargen_body_table[CGEN_MAX_BODY] = 
{
  { "Normal",	NULL,		0,	0 },
  { "Slender",	NULL,		0,	0 },
  { "Wirey",	NULL,		0,	0 },
  { "Broad",	NULL,		0,	0 },
  { "Large",	NULL,		0,	0 },
  { "Muscular",	NULL,		0,	0 },
  { "Heavy",	NULL,		0,	0 },
  { "Light",	NULL,		0,	0 },
  { "Fragile",	NULL,		0,	0 },
  { "Stocky",	NULL,		0,	0 },
  { "Puny",	NULL,		0,	0 },
  { "Athletic",	NULL,		0,	0 },
  { "Voluptuous",NULL,		0,	0 },
  { NULL,	NULL,		0,	0 }
};

struct chargen_choice_s chargen_face_table[CGEN_MAX_FACE] = {
  { "Ordinary",	NULL,		0,	0 },
  { "Round",	NULL,		0,	0 },
  { "Long",	NULL,		0,	0 },
  { "Pleasant",	NULL,		0,	0 },
  { "Plain",	NULL,		0,	0 },
  { "Gentle",	NULL,		0,	0 },
  { "Rugged",	NULL,		0,	0 },
  { "Scary",	NULL,		0,	0 },
  { "Ugly",	NULL,		0,	0 },
  { "Beautiful",NULL,		0,	0 },
  { "Chubby",	NULL,		0,	0 },
  { "Chiseled",	NULL,		0,	0 },
  { "Scarred",	NULL,		0,	0 },
  { NULL,	NULL,		0,	0 }
};

struct chargen_choice_s chargen_skin_table[CGEN_MAX_SKIN] = {
  { "Pale",	NULL,		0,	0 },
  { "Tan",	NULL,		0,	0 },
  { "Brown",	NULL,		0,	0 },
  { "Dark",	NULL,		0,	0 },
  { "Exotic",	NULL,		0,	0 },
  { NULL,	NULL,		0,	0 }
};

struct chargen_choice_s chargen_hair_table[CGEN_MAX_HAIR] = {
  { "Bald",		NULL,		0,	0 },
  { "Blond",		NULL,		0,	0 },
  { "Light blonde",	NULL,		0,	0 },
  { "Dark blonde",	NULL,		0,	0 },
  { "Strawberry blonde",NULL,		0,	0 },
  { "Red",		NULL,		0,	0 },
  { "Brown",		NULL,		0,	0 },
  { "Light brown",	NULL,		0,	0 },
  { "Dark brown",	NULL,		0,	0 },
  { "Black",		NULL,		0,	0 },
  { "Gray",		NULL,		0,	0 },
  { "Auburn",		NULL,		0,	0 },
  { "White",		NULL,		0,	0 },
  { NULL,	NULL,		0,	0 }
};
struct chargen_choice_s chargen_hairl_table[CGEN_MAX_HAIRL] = {
  { "Bald",		NULL,		0,	0 },
  { "Partly bald",	NULL,		0,	0 },
  { "Crew-cut",		NULL,		0,	0 },
  { "Short",		NULL,		0,	0 },
  { "Close",		NULL,		0,	0 },
  { "Cropped",		NULL,		0,	0 },
  { "Shoulder-length",	NULL,		0,	0 },
  { "Long",		NULL,		0,	0 },
  { "Waist-long",	NULL,		0,	0 },
  { NULL,	NULL,		0,	0 }
};

struct chargen_choice_s chargen_hairt_table[CGEN_MAX_HAIRT] = {
  { "Bald",		NULL,		0,	0 },
  { "Curly",		NULL,		0,	0 },
  { "Straight",		NULL,		0,	0 },
  { "Wavy",		NULL,		0,	0 },
  { "Shaggy",		NULL,		0,	0 },
  { "Dread-locks",	NULL,		0,	0 },
  { "Thick",		NULL,		0,	0 },
  { "Thin",		NULL,		0,	0 },
  { "Messy",		NULL,		0,	0 },
  { "Exotic",		NULL,		0,	0 },
  { NULL,	NULL,		0,	0 }
};
struct chargen_choice_s chargen_hairf_table[CGEN_MAX_HAIRF] = {
  { "Clean",		NULL,		0,	0 },
  { "Shaven",		NULL,		0,	0 },
  { "Short beard",	NULL,		0,	0 },
  { "Long beard",	NULL,		0,	0 },
  { "Goatee",		NULL,		0,	0 },
  { "Mustache",		NULL,		0,	0 },
  { "Beard and mustache",NULL,		0,	0 },
  { "Stubble",		NULL,		0,	0 },
  { "Sideburns",	NULL,		0,	0 },
  { NULL,	NULL,		0,	0 }
};

struct chargen_choice_s chargen_eyes_table[CGEN_MAX_EYES] = {
  { "Blue",	NULL,		0,	0 },
  { "Brown",	NULL,		0,	0 },
  { "Green",	NULL,		0,	0 },
  { "Hazel",	NULL,		0,	0 },
  { "Black",	NULL,		0,	0 },
  { "Gray",	NULL,		0,	0 },
  { "Yellow",	NULL,		0,	0 },
  { "Red",	NULL,		0,	0 },
  { "White",	NULL,		0,	0 },
  { "Exotic",	NULL,		0,	0 },
  { NULL,	NULL,		0,	0 }
};

struct chargen_choice_s chargen_perk_table[CGEN_MAX_PERK] = {
  { "Overweight",	
    "You are used to carrying more weight, but are less agile.",
    PERK_OVERWEIGHT,	0 
  },
  { "Underweight",	
    "You can count your ribs with closed eyes and feast on a small snack.",
    PERK_UNDERWEIGHT,	0 
  },
  { "Bookworm",	
    "Your love of books has paid off.  You are a master at reading scrolls.",
    PERK_BOOKWORM,	&gsn_scrolls
  },
  { "Athletic",	
    "Your athletic build make you stronger and more accurate then others.",
    PERK_ATHLETIC,	0 
  },

  { "Healthy",	
    "You're an example of perfect health.  You heal faster then normal.",
    PERK_HEALTHY,	0 
  },

  { "Sickly",	
    "You cough, weeze, and learn faster then your peers.",
    PERK_SICKLY,	0 
  },

  { "Stout",	
    "You're healthier then normal, but your mana reserves are smaller.",
    PERK_STOUT,	0 
  },

  { "Nimble",	
    "You can carry more items then normal, but you're weaker then normal.",
    PERK_NIMBLE,	0 
  },

  { "Charismatic",	
    "You are a natural charmer and a master at haggling.",
    PERK_CHARISMATIC,	&gsn_haggle
  },

  { "Fearsome",	
    "Your fearsome appearance makes creatures less aggressive.",
    PERK_FEARSOME,	0 
  },

  { "Greedy",	
    "Avarice is your middle name.  You avoid paying bank transaction fees.",
    PERK_GREEDY,	0 
  },

  { "Tall",	
    "You've sprouted taller then most.  Your long legs allow for extra movement.",
    PERK_TALL,	0 
  },

  { "Short",	
    "Your smaller figure is easier to conceal in shadows or foliage.",
    PERK_SHORT,	0 
  },

  { "Alcoholic",	
    "You love all things spiritual and ignore negative effects of alcohol.",
    PERK_ALCOHOLIC,	0 
  },

  { "Addict",	
    "You love all things chemical and resist negative effects of drugs.",
    PERK_ADDICT,	0 
  },

  { "Lucky",	
    "Lady Luck fancies you.  Your luck tends to run higher then average.",
    PERK_LUCKY,	0 
  },

  { "Unlucky",	
    "Jinx is your middle name but you tend to grow healthier with ranks.",
    PERK_UNLUCKY,	0 
  },

  { "Pious",	
    "Your faith has no equal.  Gods give you with more gold for sacrifices.",
    PERK_PIOUS,	0 
  },

  { "Magical",	
    "Your mana resources are greater but you're less healthy then normal.",
    PERK_MAGICAL,	0 
  },

  { "Artificer",	
    "Your knowledge about magical artifacts, orbs and staves knowns no equal.",
    PERK_ARTIFICER,	&gsn_staves
  },

  { "Tinkerer",	
    "You own Brambusworks VIP scroll.  Your skill with wands is unmatched.",
    PERK_TINKERER,	&gsn_wands
  },

  { "Eagle-eye",	
    "Your perfect eyesight alows you to scan and scout over longer distance.",
    PERK_EAGLEEYE,	0
  },

  { "Adventurer",	
    "Your spirit thirsts for the unknown.  You gain more experience.",
    PERK_ADVENTURER,	0
  },

  { "Heroic",	
    "Another day, another damsel in distress.  You're a one-man rescue team.",
    PERK_HEROIC,	0
  },

  { "Merciful",	
    "You fight in calm manner and thus may grant \"mercy\" to your foes.",
    PERK_MERCY,	0
  },

  { "none",	
    "You have no special perks.",
    0,	0
  },

  { NULL,	
    NULL,
    0, 0
  },

};

/* this should correspond to CGEN_ constants */
const struct chargen_type chargen_table[] = 
{
  { 
    "Playing",		
    "",
    chargen_null				},

  { "",			
    "",
    chargen_greet				},

  { "Main",		
    "\n\rYour Choice> ",
    chargen_main				},
 
  { "Help",		
    "\n\r\"?\" for info or [Q]uit.\n\rSearch for> ",
    chargen_help				},

  { "Enter",		
    "\n\r([Q]uit) Enter Aabahran with which Name?> ",
    chargen_enter				},

  { "Create",		
    "\n\r([Q]uit) What will be your Name?> ",
    chargen_create				},

  { "Passwrd",		
    "\n\rPassword?> ",
    chargen_pass				},

  { "ImmPass",		
    "\n\rImm-Password?> ",
    chargen_immpass				},

  { "motd",		
    "\n\rPrepare yourself. For you are about to <Enter> the Forsaken Lands!\n\r",
    chargen_motd				},

  { "ready",		
    "",
    chargen_ready				},
  { "con-name",		
    "Did I get that right: %s? (Y/N)>",
    chargen_con_name				},

  { "new-pass",		
    "\n\rEnter password for %s>",
    chargen_new_pass				},
  { "con-pass",		
    "\n\rRetype Password>",
    chargen_con_pass				},

  { "new-last",		
    "\n\r([N]one) What is your last name> ",
    chargen_new_last				},
  { "con-last",		
    "Did I get that right: %s %s? (Y/N)>",
    chargen_con_last				},

  { "new-sex",		
    "\n\rSex [M/F]>",
    chargen_new_sex				},

  { "new-race",		
    "\n\rRace [1 - %d]>",
    chargen_new_race				},
  { "con-race",		
    "\n\rDid I get that right: %s? (Y/N)>",
    chargen_con_race				},

  { "new-class",		
    "\n\rClass [1 - %d]>",
    chargen_new_clas				},
  { "con-class",		
    "\n\rDid I get that right: %s? (Y/N)>",
    chargen_con_clas				},

  { "new-alig",		
    "\n\rChoose your Alignment: %s>",
    chargen_new_alig				},
  { "con-alig",		
    "\n\rDid I get that right: %s? (Y/N)>",
    chargen_con_alig				},

  { "new-etho",		
    "\n\rChoose your Ethos: %s>",
    chargen_new_etho				},
  { "con-etho",		
    "\n\rDid I get that right: %s? (Y/N)>",
    chargen_con_etho				},

  { "rolling",		
    "",
    chargen_roll				},

  { "new-reli",		
    "\n\rReligious path [1 - %d]>",
    chargen_new_reli				},
  { "con-reli",		
    "\n\rDid I get that right: %s->%s? (Y/N)>",
    chargen_con_reli				},

  { "new-home",		
    "\n\rHome city [1 - %d]>",
    chargen_new_home				},
  { "con-home",		
    "\n\rDid I get that right: %s? (Y/N)>",
    chargen_con_home				},

  { "ask-deta",		
    "\n\rProceed with detailed creation? (Y/N)>",
    chargen_ask_deta				},

  { "new-end",		
    "\n\rYou may view your personal information using \"bio\" or \"info\""\
    "\n\rCharacter Saved...  Press <ENTER> to continue.\n\r",
    chargen_new_done				},

  { "reconn",		
    "\n\rThat character is already playing.  Connect anyway? (Y/N)>",
    chargen_con_break				},

  { "det-body",		
    "\n\rPlease choose your body build [1 - %d]>",
    chargen_det_body				},  
  { "det-face",		
    "\n\rPlease choose your facial features [1 - %d]>",
    chargen_det_face				},  
  { "det-skin",		
    "\n\rPlease choose your skin color [1 - %d]>",
    chargen_det_skin				},  
  { "det-eyes",		
    "\n\rPlease choose your eye color [1 - %d]>",
    chargen_det_eyes				},  
  { "det-hair",		
    "\n\rPlease choose your hair color [1 - %d]>",
    chargen_det_hair				},  
  { "det-haiL",		
    "\n\rPlease choose your hair length [1 - %d]>",
    chargen_det_haiL				},  
  { "det-haiT",		
    "\n\rPlease choose your hair texture [1 - %d]>",
    chargen_det_haiT				},  
  { "det-haiF",		
    "\n\rPlease choose your facial hair [1 - %d]>",
    chargen_det_haiF				},  
  { "det-perk",		
    "\n\rPlease choose your perk [1 - %d]>",
    chargen_det_perk				},  
  { "con-perk",		
    "\n\r%s:\n\r%s\n\rDid I get that right: %s? (Y/N)>",
    chargen_con_perk				},
  { "det-hand",		
    "\n\rAre you [L]eft or [R]ight handed?>",
    chargen_det_hand				},
  { "det-age",		
    "\n\rHow old are you in terms of your natural lifespan? (15 - 50%)?>",
    chargen_det_age				},  
  { "con-age",		
    "\n\rDid I get that right: %d years? (Y/N)>",
    chargen_con_age				},  
  { "det-weap",		
    "Please choose your favorite weapon [1 - %d]>",
    chargen_det_weap				},  
  { "new-tele",		
    "Do you wish to be telepathic? [Y/N]>",
    chargen_new_tele				},  
  { "new-enem",		
    "Choose your racial enemy. [1 - %d]>",
    chargen_new_enemy				},  
  { "new-adve",		
    "\n\r\n\rIf you are new to Forsaken Lands and wish to initially avoid the PLAYER-KILLING.\n\r"\
    "We suggest you play with the ADVENTURER class which is exempt from PK attacks.\n\r\n\r"\
    "Do you wish to view the Adventurer class information? [Y/N]>",
    chargen_new_adve },

  { "nag-scre",		
    "",
    chargen_nag_screen				},  
  { "nag-lag",		
    "",
    chargen_nag_lag				},  
  { "???",		
    "",
    NULL					},  
};

/* used as basic for how many repetitions required to progress in a skill */
/* get_skill_reps(..) sets the actual number of reps for given % skill    */

const int progress_table[] = {
  3,	//0-04
  4,	//05-09
  5,	//10-14
  6,	//15-19
  7,	//20-24
  8,	//25-29
  9,	//30-34
  12,	//35-39
  16,	//40-44
  20,	//45-49
  24,	//50-54
  29,	//55-59
  34,	//60-64
  40,	//65-69
  46,	//70-74

  54,	//75-79
  63,	//80-84
  73,	//85-89
  84,	//90-94
  96,	//95-99
  110,	//100+
};

/* Ratings in each category from worst to best based on table below */
/* ATTACK	DEFEND		WITHDIS		TODIS	WITHSDIS	*
 * St -4	W -5		Sp -4		St  4	D -5		*
 * D  -2	A -4		M  -3		S   3	Sp-4		*
 * Sp -1	F -3		A  -2		Sp  2	S -3		*
 * S   0	Ex-2		St -2		A   1	St-2		*
 * Po  1	M -2		Po -1		D   0	M  0		*
 * Ex  2	D -1		D   0	        Po -1	Po 1		*
 * M   2	S  0		S   1		F  -2	Ax 2		*
 * F   3	Sp 2		F   2		M  -3	W  3		*
 * A   4	Po 3		Ex  3		Ex -4	Ex 4		*
 * W   5	St 5		W   5		W  -5	F  5		*
 */
   
//weapon ratings in various categories on scale of -5(bad) to 5(excellent)
//Hand to Hand is considered Exotic as far as ratings
const struct weapon_rate_type weapon_rating [WEAPON_MAX] = 
/* attack	defense		with_dis	tobe_dis	with_sdis */
{
  //exotic
  {2	,	-2	,	 3	,	-4	,	4 },

  //sword
  { 0,		 0	,	 1	, 	 3	,      -3 },

  //dagger
  {-2	,	 0	,	 0	,	 0	,      -5 },

  //spear
  {-1	,	 2	,	-4	,	 2	,      -4 },

  //mace
  {2	,	-1	,       -3	,	-3	,	0 },

  //axe
  {4	,	-4	,	-2	,	 1	,	2 },

  //flail
  {3	,	-3	,	 2	,	-2	,	5 },

  //whip
  {5	,	-5	,	 5	,	-5	,	3 },

  //pole
  {1	,	 3	,	-1	,	-1	,	1 },

  //staff
  {-4	,	 5	,	-2	,	 4	,      -2 },
};

const struct croom_type croom_table[CROOM_LEVELS] = {
  /* STANDART HALLWAY */
  {
    "Hallway",
    "A generic hallway.",
    0,
    0,
    1,
    0
  },

  /* STANDART GARDEN */
  {
    "Garden",
    "A generic garden.",
    0,
    0,
    1,
    0
  },
  
  /* CHAT AND HEARGUARD */
  {
    "Messengers",
    "Provides Cabal Chat.",
    CABAL_CHAT,
    CROOM_NODUP | CROOM_FIXEDNAME,
    3,
    5,
  },

  /* PIT AND SUPPLY SHOP */
  {
    "Supplies",
    "Supply shop and cabal pit.",
    0,
    CROOM_NODUP | CROOM_FIXEDNAME | CROOM_PITROOM,
    3,
    0,
  },
  /* HEALING AND TRAIN SERVICE */
  {
    "Healer",
    "Healing services.",
    0,
    CROOM_NODUP | CROOM_FIXEDNAME,
    6,
    0,
  },
/* TREASUREY AND CABAL2 */
  {
    "Treasury",
    "Enables basic powers and costly rooms.",
    CABAL_COFFER | CABAL_ALLOW_2,
    CROOM_NODUP | CROOM_FIXEDNAME,
    6,
    25,
  },
/* TEACH SKILL SPELL AND HIGHGAIN */
  {
    "Academy",
    "High cp gain and skill/spell service.",
    CABAL_HIGHGAIN,
    CROOM_NODUP | CROOM_FIXEDNAME,
    10,
    50,
  },
/* CABAL3 AND GUARD */
  {
    "Guards",
    "Provies Cabal Guard and advanced powers.",
    CABAL_GUARDMOB | CABAL_ALLOW_3,
    CROOM_NODUP | CROOM_FIXEDNAME,
    10,
    70,
  },

/* OBJECT SERVICE AND CABAL SKILL */
  {
    "Armory",
    "Object services and cabal training.",
    0,
    CROOM_NODUP | CROOM_FIXEDNAME,
    14,
    0,
  },

/* CABAL4 AND GATE */
  {
    "Gate",
    "Elder powers and cabal gateway.",
    CABAL_ALLOW_4 | CABAL_GATE,
    CROOM_NODUP | CROOM_FIXEDNAME,
    14,
    80,
  },

/* RELICS */
  {
    "Reliquary",
    "Provides sale of cabal relics.",
    0,
    CROOM_NODUP | CROOM_FIXEDNAME,
    17,
    0,
  },

/* CABAL5 AND LIFE INSURANCE*/
  {
    "Keep",
    "Life-Insurance service and Leader powers.",
    CABAL_ALLOW_5,
    CROOM_NODUP | CROOM_FIXEDNAME,
    17,
    95,
  },
};


const struct critical_type critical_table[] = {
  //min skill		name
  {	 0,		"chest"		},
  {	20,		"stomach"	},
  {	40,		"legs"		},
  {	60,		"head"		},
  {	75,		"wrist"		},
  {	90,		"feet"		},
  
  {	101,		"spleen"	},//expert level
  {	104,		"temple"	},
  {	107,		"diaphragm"	},
  {	110,		"eyes"		},

  {	113,		"heart"		},//master level
  {	116,		"lungs"		},
  {	119,		"kidneys"	},
  {	120,		"solar plexus"	},

  {	121,		"aorta"		},//expert+master level
  {	126,		"intestine"	},
  {	130,		"brain"		},

  {	999,		NULL		},
};


const  char*	predict_table[] = {
  "backstab",
  "bash",
  "critical",
  "boulder",
  "rush",
  "dirt",
  "corrupt",
  "lifedrain",
  "focusedbash",
  "maul",
  "fury",
  "shoulder",
  "trip",
  "cutter",
  "push",
  "cleave",
  "behead",
  "charge",
  "assassinate",
  "blackjack",
  "strangle",
  "bodyslam",
  "impale",
  "ambush",
  "blindness_dust",
  "poison_smoke",
  "tear",
  "tackle",
  "thrust",
  "thrash",
  "bolt",
  "coneofforce",
  "cutpurse",
  "insectswarm",
  "knockout",
  "embrace",
  NULL
};
