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


/* Written by Virigoth sometime circa april 2000 for FORSAKEN LANDS mud.*/
/* This is the implementation of the selectable skills code		*/
/* NOT TO BE USED OR REPLICATED WITHOUT EXPLICIT PERMISSION OF AUTHOR	*/
/* umplawny@cc.umanitoba.ca						*/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "s_skill.h"

/*
This is the main table containing data referencing each "Set" of selections
to the actual group and skill information in the ss_group_table.

The name of the set of skills is used in class_table of const.c to make
the game check the following table for need to select any skills.

NOTE* For speed's sake the values of "level" are assumed to be sorted
in ascending order.

End of table is signaled by NULL in name.
*/
/* name, level, picks, group */
const struct ss_type	ss_table [MAX_CLASS] = 
{
  {
  /* FALSE */
  	"",
	{0},
	{0},
	{NULL}
  },

  {
    /* Crusaders */
    "crusader",
    /* levels at which selections occur */
    {7,	    12,	    20,		26,	33,	39,	43,	0},
    /* number of choices per selection */
    {1,	     1,	     1,		1,	1,	1,	1,	0},
    /* groups to choose from */
    {"minor", "minor", "minor", "major", "major", "specialty", "signature", NULL}
  },
  /* Ninja */
  {
    "ninja",
    /* levels at which selections occur */
    {37,	    0,	     0,		 0,	 0,	0,	0,	0},
    /* number of choices per selection */
    {1,	     0,	     0,		0,	0,	0,	0,	0},
    /* groups to choose from */
    {"ninjitsu", NULL, NULL, NULL, NULL, NULL, NULL, NULL}
  },

  /* RANGER */
  {
    "ranger",
    /* levels at which selections occur */
    {25,	  30,	     35,		40,	43,    45,	50,	0},
    /* number of choices per selection */
    {1,	     1,	     1,		1,	1,	1,	1,	0},
    /* groups to choose from */
    {"ranger path", "forest lore", "forest lore", "forest lore", "weapon expertise", "ranger lore", "weapon expertise", NULL}
  },

  /* WARRIOR */
  {
    "warrior",
    /* levels at which selections occur */
    {28,	   39,	    45,	       50,	0,	0,	0,	0},
    /* number of choices per selection */
    { 1,	1,	1,	1,	0,	0,	0,	0},
    /* groups to choose from */
    {"weapon expertise", "weapon expertise", "weapon mastery", "warrior lore", NULL, NULL, NULL, NULL}
  },

  /* BERSERKER */
  {
    "berserker",
    /* levels at which selections occur */
    {30,	42,	   46,		 0,	 0,	0,	0,	0},
    /* number of choices per selection */
    { 1,	   1,	    1,		0,	0,	0,	0,	0},
    /* groups to choose from */
    {"path focus", "weapon expertise", "rage focus", NULL, NULL, NULL, NULL, NULL}
  },

  /* THIEF */
  {
    "thief",
    /* levels at which selections occur */
    {43,	   46,	    50,		 0,	 0,	0,	0,	0},
    /* number of choices per selection */
    { 2,	   2,	    1,		0,	0,	0,	0,	0},
    /* groups to choose from */
    {"booby traps", "magic traps", "mechanisms", NULL, NULL, NULL, NULL, NULL}
  },

  /* PSI */
  {
    "psionicist",
    /* levels at which selections occur */
    {20, 22,  24,  29, 31, 35, 39, 40, 45, 47, 49, 50,  0,  0,  0,  0},
    /* number of choices per selection */
    {1,    2,   2,  1,  1,  2,  1,  1,  1,  1,  1,  2,  0,  0,  0,  0},
    /* groups to choose from */
    {"psi focus", "3rd circle", "3rd circle", "3rd circle", "4th circle", "4th circle", "4th circle", "psi lore", "5th circle", "5th circle", "5th circle", "6th circle", NULL, NULL, NULL, NULL}, 
  },

  /* BLADEMASTER */
  {
    "blademaster",
    /* levels at which selections occur */
    {29,	   33,	    36,		40,	43,    45,     47,     49, 50},
    /* number of choices per selection */
    { 1,	   1,	    1,		1,	1,	1,	1,	1,  1},
    /* groups to choose from */
    {"bladelore", "anatomy expertise", "bladelore", "bladelore", "anatomy expertise", "bladelore", "bladelore", "bladelore", "anatomy mastery"}
  },
  {
    /* END */
    NULL,
	{0},
	{0},
	{NULL}
  }
};


/* 
This is the table containing data to each specific group of skills.
The single selection is made out of single group, where each skill is a 
single choice.  The choices themselfs are limited by their pre-requisite
skills and skills which exclude them.

Table can be terminated by NULL in group name 
*/

/* name, skill, skill info:	-> name
				-> rating (how easy to learn)
				-> starting profficiency
				-> pre
				-> ex
*/

const struct ss_group_type	ss_group_table [MAX_GROUPS] = 
{
  /* NONE: FALSE return 0 */
  {"", 0,
   {
     {NULL, 0, 0,   {NULL},    {NULL}  }
   }
  },

  /* CRUSADER: MINORS */
  {"minor", 6,
   {
     {"2h handling",  1, 100,	{NULL},		
      {"orealts 1st"} },
     {"2h tactics",   1, 100,	{NULL},		
      {NULL} },
     {"history of armaments",   1, 1,	{NULL},		
      {NULL} },
     {"arms maintenance",   1, 100,	{NULL},		
      {NULL} },
     {"basic armor",   1, 100,	{NULL},		
      {NULL} },
     {"orealts 1st",   1, 100,	{NULL},		
      {"2h handling"} },


   }
  },
  /* CRUSADER: MAJORS */
  {"major", 8,
   {
     {"adv. handling",  1, 1,	{"2h handling"},	
      {"orealts 2nd"} },
     {"2h mastery",  1, 100,	{NULL},			
      {NULL} },
     {"orealts 2nd",  1, 1,	{"orealts 1st"},	
      {"adv. handling"} },
     {"damage mastery B", 1, 1,	{NULL},			
      {"armor enhancement", "damage mastery P", "damage mastery S"}},
     {"damage mastery P", 1, 1,	{NULL},			
      {"armor enhancement", "damage mastery B", "damage mastery S"}},
     {"damage mastery S", 1, 1,	{NULL},			
      {"armor enhancement", "damage mastery P", "damage mastery B"}},
     {"adv. armor",  1, 100,	{"basic armor"},	
      {NULL} },
     {"armor enhancement",1, 100,{"history of armaments"}, 
      {"damage mastery B", "damage mastery S", "damage mastery P"} },
   }
  },
  /* CRUSADER: SPECIALTY */
  {"specialty", 7,
   {
     {"control damage", 1, 1,	
      {"damage mastery B", "damage mastery S", "damage mastery P"},
      {"armorcraft"} },
     {"blind fighting",	5, 1,	{"orealts 2nd"},	
      {"cusinart", "adv. handling", "2h handling"} },
     {"cusinart",	1, 100,	{"adv. handling"},	
      {"blind fighting", "orealts 2nd", "orealts 1st"} },
     {"psalm master",	1, 100,	{NULL},			
      {NULL} },
     {"witch compass",	1, 1,	{NULL},			
      {NULL} },
     {"weaponcraft",	1, 100,	{"arms maintenance"},	
      {NULL} },
     {"armorcraft",	1, 100,	{"armor enhancement"},	
      {"control damage", "damage mastery B", "damage mastery S", "damage mastery P"} }
   }
  },
  /* CRUSADER: SIGNATURE */
  {"signature", 6,
   {
     {"fourth attack", 9, 1,	{"adv. handling"},	
      {"behead", "orealts 2nd", "orealts 1st"} },
     {"pommel smash",   1, 1,	{"2h mastery"},			
      {NULL} },
     {"behead",	       1, 1,	{"orealts 2nd"},	
      {"fourth attack", "adv. handling", "2h handling" } },
     {"armor pierce",  1, 1,	{"adv. armor"},		
      {NULL} },
     {"pious",	       1, 1,	{NULL},			
      {NULL} },
     {"high sight",    1, 1,	
      {"damage mastery B", "damage mastery S", "damage mastery P"},		
      {NULL} }
   }
  },

  /* NINJA: NINJITSU */
  {"ninjitsu", 6,
   {
     {"falcon eye",	1,100,	{NULL},		{NULL}},
     {"death kiss",	1,100,	{NULL},		{NULL}},
     {"doublesheath",	1,100,	{NULL},		{NULL}},
     {"shadow arts",	5,  1,	{NULL},		{NULL}},
     {"lotus scourge",	1,100,	{NULL},		{NULL}}
   }
  },

  /* RANGER: RANGER LORE */
  {"ranger lore", 3,
   {
     {"marksman",	5, 1,	{"archery"},	{NULL}},
     {"concealment",	5, 1,	{"tracker"},	{NULL}},
     {"horde",		5, 1,	{"beastmaster"},{NULL}},
   }
  },

/* VARIOUS: WEAPON EXPERTISE */
  {"weapon expertise", 10,
   {
     {"exotic expert",	1, 100,	{NULL},		{NULL}},
     {"sword expert",	1, 100,	{"sword"},		{NULL}},
     {"dagger expert",	1, 100,	{"dagger"},		{NULL}},
     {"spear expert",	1, 100,	{"spear"},		{NULL}},
     {"mace expert",	1, 100,	{"mace"},		{NULL}},
     {"flail expert",	1, 100,	{"flail"},		{NULL}},
     {"axe expert",	1, 100,	{"axe"},		{NULL}},
     {"staff expert",	1, 100,	{"staff"},		{NULL}},
     {"whip expert",	1, 100,	{"whip"},		{NULL}},
     {"polearm expert",	1, 100,	{"polearm"},		{NULL}}
   }
  },

/* VARIOUS: WEAPON MASTERY */
  {"weapon mastery", 10,
   {
     {"exotic mastery",	1, 100,	{NULL},			{NULL}},
     {"sword mastery",	1, 100,	{"sword"},		{NULL}},
     {"dagger mastery",	1, 100,	{"dagger"},		{NULL}},
     {"spear mastery",	1, 100,	{"spear"},		{NULL}},
     {"mace mastery",	1, 100,	{"mace"},		{NULL}},
     {"flail mastery",	1, 100,	{"flail"},		{NULL}},
     {"axe mastery",	1, 100,	{"axe"},		{NULL}},
     {"staff mastery",	1, 100,	{"staff"},		{NULL}},
     {"whip mastery",	1, 100,	{"whip"},		{NULL}},
     {"polearm mastery",1, 100,	{"polearm"},		{NULL}}
   }
  },
  /* THIEF: TRAPS */
  {"booby traps", 5,
   {
     {"poison needle",	1,  1,	{NULL},		{NULL}},
     {"enfeebler",	1,  1,	{NULL},		{NULL}},
     {"fireseed",	1,  1,	{NULL},		{NULL}},
     {"snares",		1,  1,	{NULL},		{NULL}},
     {"runes",		1,  1,	{NULL},		{NULL}},
   }
  },
  {"magic traps", 4,
   {
     {"eyedust",	1,  1,	{NULL},		{NULL}},
     {"antimagic",	1,  1,	{NULL},		{NULL}},
     {"amnesia charm",	1,  1,	{NULL},		{NULL}},
     {"blackspore",	1,  1,	{NULL},		{NULL}},
   }
  },
  {"mechanisms", 4,
   {
     {"chestbuster",	1,  1,	{NULL},		{NULL}},
     {"brambus needler",1,  1,	{NULL},		{NULL}},
     {"devil wheel",	1,  1,	{NULL},		{NULL}},
     {"webcaster",	1,  1,	{NULL},		{NULL}},
   }
  },
/* WARRIOR LORE */
  {"warrior lore", 10,
   {
     {"shield expertise",1,100,	{NULL},				       {NULL}},
     {"shield bash",	1,  1,	{NULL},				       {NULL}},
     {"feign",		1,  1,	{"sword mastery", "dagger mastery"},   {NULL}},
     {"doublesheath",	1,100,	{"sword mastery", "dagger mastery"},   {NULL}},
     {"backcutter",	1,  1,	{"spear mastery", "staff mastery"},    {NULL}},
     {"mantis maul",	1,  1,	{"flail mastery"},		       {NULL}},
     {"focused bash",	1,  1,	{"mace mastery"},		       {NULL}},
     {"weapon lock",	1,  1,	{"whip mastery"},		       {NULL}},
     {"weapon seize",	1,100,	{"exotic mastery"},		       {NULL}},
     {"power grip",	1,100,	{"axe mastery", "polearm mastery", "staff mastery"},    {NULL}},
   }
  },
/* PATH FOCUS */
  {"path focus", 3,
   {
     {"path of anger",	1,100,	{NULL},				       {NULL}},
     {"path of fury",	1,100,	{NULL},				       {NULL}},
     {"path of devastation",1,100,{NULL},				       {NULL}}
   }
  },

/* RAGE FOCUS */
  {"rage focus", 3,
   {
     {"skull crusher",	1,100,	{"path of anger"},		       {NULL}},
     {"power grip",	1,100,	{"path of fury"},		       {NULL}},
     {"blood haze",	1,100,	{"path of devastation"},	       {NULL}}
   }
  },

/* PSI CIRCLES */
  {"psi focus", 3,
   {
     //we cheat here a bit and make cone reject kinesis and illusion
     //in order to limit Illithids to telepathy only.

     {"kinesis",	1,1,	{NULL},		{"cone of force"}},
     {"illusion",	1,1,	{NULL},		{"cone of force"}},
     {"telepathy",	1,1,	{NULL},		{NULL}},

   }
  },

  {"psi lore", 3,
   {
     {"destruction",	1,100,	{"kinesis"},			{NULL}},
     {"domination",	1,100,	{"telepathy"},			{NULL}},
     {"alteration",	1,100,	{"illusion"},			{NULL}},
   }
  },

  {"3rd circle", 15,
   {
     {"pyro kinesis",	1,1,	{"kinesis"},		       {NULL}},
     {"cryo kinesis",	1,1,	{"kinesis"},		       {NULL}},
     {"hydro kinesis",	1,1,	{"kinesis"},		       {NULL}},
     {"electro kinesis",1,1,	{"kinesis"},		       {NULL}},
     {"tele kinesis",	1,1,	{"kinesis"},		       {NULL}},

     {"omen",		1,1,	{"telepathy"},		       {NULL}},
     {"deathmare",	1,1,	{"telepathy"},		       {NULL}},
     {"dreamprobe",	1,1,	{"telepathy"},		       {NULL}},
     {"mindmelt",	1,1,	{"telepathy"},		       {NULL}},
     {"phase shift",	1,1,	{"telepathy"},		       {NULL}},

     {"false weapon",	1,1,	{"illusion"},		       {NULL}},
     {"mirror cloak",	1,1,	{"illusion"},		       {NULL}},
     {"shadow door",	1,1,	{"illusion"},		       {NULL}},
     {"numbness",	1,1,	{"illusion"},		       {NULL}},
     {"nerve amp",	1,1,	{"illusion"},		       {NULL}},

   }
  },

  {"4th circle", 12,
   {
     {"photon blast",	1,1,	{"kinesis"},		       {NULL}},
     {"psychic purge",	1,1,	{"kinesis"},		       {NULL}},
     {"mindsurge",	1,1,	{"kinesis"},		       {NULL}},
     {"psionic blast",	1,1,	{"kinesis"},		       {NULL}},

     {"mental knife",	1,1,	{"telepathy"},		       {NULL}},
     {"spell vise",	1,1,	{"telepathy"},		       {NULL}},
     {"spell blast",	1,1,	{"telepathy"},		       {NULL}},
     {"spell trap",	1,1,	{"telepathy"},		       {NULL}},

     {"mirage",		1,1,	{"illusion"},		       {NULL}},
     {"blink",		1,1,	{"illusion"},		       {NULL}},
     {"duplicate",	1,1,	{"illusion"},		       {NULL}},
     {"spook",		1,1,	{"illusion"},		       {NULL}},
   }
  },

  {"5th circle", 9,
   {
     {"singularity",	   1,1,	{"destruction"},		{NULL}},
     {"entropic touch",	   1,1,	{"destruction"},		{NULL}},
     {"amorphous infection",1,1,{"destruction"},		{NULL}},

     {"soul pump",	   1,1, {"domination"},			{NULL}},
     {"brain death",	   1,1,	{"domination"},			{NULL}},
     {"masochism",	   1,1, {"domination"},			{NULL}},

     {"time compression",  1,1, {"alteration"},			{NULL}},
     {"planar rift",	   1,1, {"alteration"},			{NULL}},
     {"molecular leash",   1,1, {"alteration"},			{NULL}},
   }
  },

  {"6th circle", 6,
   {
     {"disintegrate",	1,1,	{"destruction"},		{NULL}},
     {"breach",		1,1,	{"destruction"},		{NULL}},

     {"unminding",	1,1,	{"domination"},			{NULL}},
     {"total recall",	1,1,	{"domination"},			{NULL}},

     {"paralyze",	1,1,	{"alteration"},			{NULL}},
     {"petrify",	1,1,	{"alteration"},			{NULL}},
   }
  },

  /* RANGER: RANGER PATH */
  {"ranger path", 3,
   {
     {"archery",	3, 1,	{NULL},		{NULL}},
     {"beastmaster",	3,100,	{NULL},		{NULL}},
     {"tracker",	3, 1,	{NULL},		{NULL}},
   }
  },

  /* RANGER: FOREST LORE */
  {"forest lore", 9,
   {
     {"bowyer",		4, 1,	{"archery"},	{NULL}},
     {"fletcher",	4, 1,	{"archery"},	{NULL}},
     {"rapid fire",	4, 1,	{"archery"},	{NULL}},

     {"throw",		4, 1,	{"tracker"},	{NULL}},
     {"forestwalk",	4, 1,	{"tracker"},	{NULL}},
     {"doublesheath",	4, 1,	{"tracker"},	{NULL}},

     {"horn of nature",4, 1,	{"beastmaster"},{NULL}},
     {"pack call",	4, 1,	{"beastmaster"},{NULL}},
     {"group herb",	4, 1,	{"beastmaster"},{NULL}},
   }
  },

  /* BLADEMASTER: BLADELORE */
  {"bladelore", 6,
   {
     {"bladestorm",		1, 1,	{NULL},	{NULL}},
     {"ironarm",		1, 1,	{NULL},	{NULL}},
     {"kyousanken",		1, 1,	{NULL},	{NULL}},

     {"doomsinger",		1, 1,	{NULL},	{NULL}},
     {"shadowdancer",		1, 1,	{NULL},	{NULL}},
     {"kairishi",		1, 1,	{NULL},	{NULL}},

   }
  },

  /* BLADEMASTER: ANATOMY EXPERTISE */
  {"anatomy expertise", 9,
   {
     {"mob expert",		1, 100,	{NULL},	{NULL}},
     {"human expert",		1, 100,	{NULL},	{NULL}},
     {"elf expert",		1, 100,	{NULL},	{NULL}},
     {"dwarf expert",		1, 100,	{NULL},	{NULL}},
     {"demihuman expert",	1, 100,	{NULL},	{NULL}},
     {"giant expert",		1, 100,	{NULL},	{NULL}},
     {"beast expert",		1, 100,	{NULL},	{NULL}},
     {"flying expert",		1, 100,	{NULL},	{NULL}},
     {"unique expert",		1, 100,	{NULL},	{NULL}},
   }
  },

  /* BLADEMASTER: ANATOMY MASTER */
  {"anatomy mastery", 9,
   {
     {"mob master",		1, 100,	{NULL},	{NULL}},
     {"human master",		1, 100,	{NULL},	{NULL}},
     {"elf master",		1, 100,	{NULL},	{NULL}},
     {"dwarf master",		1, 100,	{NULL},	{NULL}},
     {"demihuman master",	1, 100,	{NULL},	{NULL}},
     {"giant master",		1, 100,	{NULL},	{NULL}},
     {"beast master",		1, 100,	{NULL},	{NULL}},
     {"flying master",		1, 100,	{NULL},	{NULL}},
     {"unique master",		1, 100,	{NULL},	{NULL}},
   }
  },

  /* NONE: END */
  {NULL, 0,
   {
     {NULL, 0, 0,   {NULL},    {NULL}  }
   }
  }
};


/* maintenance routines for lookups etc. */
/* looks up the position for given skillset (ss) */
int ss_lookup( const char *name )
{
    int type;
    for (type = 0; ss_table[type].name != NULL; type++){
      if (LOWER(name[0]) == LOWER(ss_table[type].name[0]) 
	  && !str_prefix(name,ss_table[type].name))
	return type;
    }
    return 0;
}

/* looks up the position for given group */
int ss_group_lookup( const char *name )
{
    int type;
    for (type = 0; ss_group_table[type].name != NULL; type++){
      if (LOWER(name[0]) == LOWER(ss_group_table[type].name[0]) 
	  && !str_prefix(name,ss_group_table[type].name))
	return type;
    }
    return 0;
}

/* looks up position of a given skill in a given group */
/* NOTE -1 on fail NOT 0 */
int ss_skill_lookup( const char *name, int group)
{
    int type;
    for (type = 0; ss_group_table[group].ss_info[type].name != NULL; type++){
      if (LOWER(name[0]) == LOWER(ss_group_table[group].ss_info[type].name[0]) 
	  && !str_prefix(name, ss_group_table[group].ss_info[type].name))
	return type;
    }
    return -1;
}

/* checks if given gskillset (ss) has a selection at given level */
/* returns number of selections allowed at level */
int get_selections ( char* sset, int level ){
  int ss = ss_lookup(sset);
  int i;
  if (!ss || level  < 1)
    return 0;

  /* quick check for first entry */
  if (ss_table[ss].level[0] > level)
    return 0;

  for (i = 0; i < MAX_SELECT; i++){
    if (ss_table[ss].level == 0)
      return 0;
    if (ss_table[ss].level[i] > level)
      return 0;
    else if (ss_table[ss].level[i] == level)
      return i;
  }
  return 0;
}


/* 
returns how many skills a player can select out of given group.
if level is given, returns how many skills can be slected at
that level.
*/

int can_group_select(CHAR_DATA* ch, int gr, int level){
  int ss = 0;
  int i = 0;
  int tot = 0;
  int lvl = 0;

  if (!gr){
    bug("can_group_select: 0 group passed.", 0);
    return 0;
  }
  if (!HAS_SS(ch->class))
    return 0;
  if ( (ss = ss_lookup(class_table[ch->class].ss)) == 0)
    return 0;

  /* check how many selections the char is allowed */
  for (i=0; i < MAX_SELECT; i++){
    /* stop on 0's */
    if (ss_table[ss].level[i] == 0)
      break;
    /* count only groups that matter to us */
    if (ss_group_lookup(ss_table[ss].group[i]) != gr)
      continue;
    /* count selections */
    if (level && ss_table[ss].level[i] <= level){
      tot +=  ss_table[ss].picks[i];
      if (level == ss_table[ss].level[i])
	lvl = ss_table[ss].picks[i];
    }
    else if (!level)
      tot += ss_table[ss].picks[i];
  }
  /* the number of allowed selections is tot - selections already made */
  /* the lvl serves as a check so we return proper values if there is
     not selection at given level, OR the skills before this level were not
     picked yet (causes inflated count, since code assumes you pick skill on
     each level before progressing.
  */
  if (level)
    return URANGE(0, tot - ch->pcdata->ss_picked[gr], lvl);
  else
    return UMAX(0, tot - ch->pcdata->ss_picked[gr]);
}

/*
Checks if target player has any valid selection of skills to make.
If true, the location of group for selections to be made from is returned.
Level of first selection is passed by argument
*/
int can_ss_select(CHAR_DATA* ch, int* lvl){
  int ss = ss_lookup(class_table[ch->class].ss);
  int i = 0;

  if (IS_NPC(ch))
    return 0;

  if (!ss){
    nlogf("can_ss_select: ch %s has no valid skill selections.", ch->name);
    return 0;
  }
  /* start looking if player has any selections to be made */
  for (i = 0; i < MAX_SELECT; i++){
    int level = ss_table[ss].level[i];
	  
    if (ss_table[ss].picks[i] == 0
	|| IS_NULLSTR(ss_table[ss].group[i]))
      break;
    /* check if we have any picks out of this group */
    if (can_group_select(ch, ss_group_lookup(ss_table[ss].group[i]), level) == 0)
      continue;
    /* check if we are allowed to select this based on level*/
    if (ch->level < level){
      return 0;
    }
    /* found the first group */
    *lvl = (int) level;
    return ss_group_lookup(ss_table[ss].group[i]);
  }
  return 0;
}

/* 
Decides based on pre/ex entries in the group table if the given
skill is selectable by character.
RETURNS: 0 for exclusion, -1 for failed prerequiste, 1 othrwise 
*/
int can_skill_select(CHAR_DATA* ch, int group, char* name){
  SKILL_DATA *nsk;   
  int sn = skill_lookup(name);
  int gr_sn = 0;
  int i = 0;
  bool fFound = FALSE;

  if (sn < 1){
    char buf[MIL];
    sprintf(buf, "can_select_skill: sn for \"%s\" not found.", name);
    bug(buf, sn);
    return FALSE;
  }
  /* find the location of the skill within the group first */
  if ( (gr_sn = ss_skill_lookup(name, group)) < 0){
    char buf[MIL];
    sprintf(buf, "can_select_skill: \"%s\" not found in group %d.", name, group);
    bug(buf, sn);
    return FALSE;
  }

  /* check if we have this skill */
  nsk = nskill_find(ch->pcdata->newskills, sn);
  if (ch->pcdata->learned[sn] > 0
      || nsk != NULL )
    return FALSE;


  /* start checking the group of pre requsites */
  fFound = FALSE;
  for (i = 0; i < MAX_PRE; i++){
    int pre_sn;
    char* name = ss_group_table[group].ss_info[gr_sn].pre[i];
    /* stop on empties */
    if (IS_NULLSTR(name)){
      if (i == 0)     
	fFound = TRUE;
      break;
    }
    /* get sn */
    if ( (pre_sn = skill_lookup(name)) < 0){
      char buf[MIL];
      sprintf(buf, "can_select_skill: sn for pre \"%s\" not found", name);
      bug(buf, sn);
      return FALSE;
    }
    /* check if character has the prerequisite */
    nsk = nskill_find(ch->pcdata->newskills, pre_sn);
    if (ch->pcdata->learned[pre_sn] > 0
	|| nsk != NULL )
      fFound = TRUE;
  }
  if (!fFound)
    return -1;

  /* start checking the skills that exclude this choice */
  for (i = 0; i < MAX_EX; i++){
    int ex_sn;
    char* name = ss_group_table[group].ss_info[gr_sn].ex[i];
    /* stop on empties */
    if (IS_NULLSTR(name))
      break;
    /* get sn */
    if ( (ex_sn = skill_lookup(name)) < 0){
      char buf[MIL];
      sprintf(buf, "can_select_skill: sn for ex \"%s\" not found", name);
      bug(buf, sn);
      return FALSE;
    }
    /* check if character has the skill that disallows this */
    nsk = nskill_find(ch->pcdata->newskills, ex_sn);
    if (ch->pcdata->learned[ex_sn] > 0
	|| nsk != NULL )
      return FALSE;
  }
  return TRUE;
}

/*
Displays a list of valid skill choices out of given group.
Returns the number of valid skill choices
Choices are only shown if fShow (false to simply count)
*/
int show_group_choice(CHAR_DATA* ch, int group, bool fShow){
  int i = 0;
  int tot = 0;

  if (fShow)
    sendf(ch, "You have following choices avaliable from \"%s\".\n\r\t",
	  ss_group_table[group].name);

  for (i = 0; i < ss_group_table[group].skills; i++){
    int result = 0;
    if (IS_NULLSTR(ss_group_table[group].ss_info[i].name))
      break;
    if ( (result = can_skill_select(ch, group, ss_group_table[group].ss_info[i].name)) == 1)
      tot++;
    if (!fShow)
      continue;
    if (i && i % 5 == 0)
      send_to_char("\n\r", ch);
    /* color the choice based on result */
    switch (result ){
      /* excluded or already picked*/
    case  0:
      sendf(ch, "`8%s``,  ", ss_group_table[group].ss_info[i].name);break;
      /* failed prerequisite */
    case -1:
      sendf(ch, "`1%s``,  ", ss_group_table[group].ss_info[i].name);break;
    default:
      sendf(ch, "%s,  ", ss_group_table[group].ss_info[i].name);break;
    }
  }
  if (fShow){
    if (!tot)
      send_to_char("\nNo possible choices at this time.\n\r", ch);
    else
      send_to_char("\n\r", ch);
  }
  return tot;
}

/*
adds a given skill
*/
void ss_add_skill(CHAR_DATA* ch, int sn, int level, int rating, int skill){
  SKILL_DATA *psk, nsk;   

  /* check if we have this skill */
  psk = nskill_find(ch->pcdata->newskills, sn);
  if (ch->pcdata->learned[sn] > 0
      || psk != NULL ){
    char buf[MIL];
    sprintf(buf, "select_skill: sn %d  already present on %s.", sn, ch->name);
    bug(buf, 0);
    return;
  }

  /* add the skill */
  nsk.keep 	= 1;
  nsk.sn	= sn;
  nsk.level	= level;
  nsk.rating	= 1;
  nsk.learned	= skill;
  nskill_to_char(ch, &nsk);
}

/*
finds info on given skill, passes it into add_ss_skill
*/
void ss_select_skill(CHAR_DATA* ch, int group, int skill, int level){
  char *name = ss_group_table[group].ss_info[skill].name;
  int sn = skill_lookup(name);

  if (sn < 1){
    char buf[MIL];
    sprintf(buf, "ss_select_skill: sn for \"%s\" not found.", name);
    bug(buf, sn);
    return;
  }
  /* add the skill */
  ss_add_skill(ch, sn, level, ss_group_table[group].ss_info[skill].rating,
	       ss_group_table[group].ss_info[skill].skill);
  /* incremeant increment the group count */
  ch->pcdata->ss_picked[group] ++;
}


/* 
Skill Set selection command.  Checks for first avaliable skill set to
select from and enters the proper connection state
*/
void do_select( CHAR_DATA *ch, char *argument){
  int group = 0;
  int level = 0;
  if (!HAS_SS(ch->class)){
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if ( (group = can_ss_select(ch, &level)) != 0){
    send_to_char("Entering skill set selection..\n\r\n\r", ch);
    ch->desc->connected = CON_SELECT;
    interpret_select(ch, "" );
    return;
  }
  send_to_char("You have no selections to be made at this time.\n\r", ch);
}

/* interpreter for the selection of skills */
void interpret_select( CHAR_DATA *ch, char *argument )
{
  char arg1[MIL];
  while ( isspace(*argument) ) {
    argument++;
  }
  argument = one_argument(argument, arg1);  
  if ( IS_NULLSTR(arg1)){
    int group = 0;
    int level = 0;
    send_to_char("\n\r[Skill Set Selection]\n\r", ch);
    if ( (group = can_ss_select(ch, &level)) != 0){
      show_group_choice(ch, group, TRUE);
      sendf(ch, "\n\rYou have %d choices remaining.\n\r", 
	    can_group_select(ch, group, level));
    }
  }
  else if (!str_cmp(arg1, "quit")){
    do_save(ch, "");
    ch->desc->connected = CON_PLAYING; 
    return;
  }
  else if (!str_cmp(arg1, "help")){
    if (IS_NULLSTR(argument))
      do_help(ch, "skill select");
    else
      do_help(ch, argument);
  }
  else if (!str_cmp(arg1, "select")){
    if (IS_NULLSTR(argument)){
      send_to_char("syntax: select <choice>\n\r", ch);
    }
    else{
      int skill = 0;
      int level = 0;
      int group = 0;
      int picks = 0;
      int result = 0;;
      /* get group and level info */
      if ( (group = can_ss_select(ch, &level)) == 0){
	bug("interp_select: Selection was allowed with no valid selections!", 0);
	send_to_char("You have no selections, returning to normal mode.\n\r", ch);
	interpret_select(ch, "quit");
	return;
      }
      /* get skill info */
      if ( (skill = ss_skill_lookup(argument, group))  < 0){
	sendf(ch, "Not a valid choice.\n\r");
      }
      else{
	/* check requirements */
	result = can_skill_select(ch, group, argument);
	switch (result){
	case  0: send_to_char("You already posses this skill.\n\r", ch);break;
	case -1: send_to_char("You do not meet prerequisites for this skill.\n\r", ch);break;
	case -2: send_to_char("Your previous choices exclude this skill.\n\r", ch);break;
	default:
	  ss_select_skill(ch, group, skill, level);
	  picks = can_group_select(ch, group, level);
	  sendf(ch, "You have selected %s, with %d choice%s remaining.\n\r", 
		ss_group_table[group].ss_info[skill].name, picks, picks == 1 ? "" : "s");
	  if (!picks){
	    send_to_char("You have no selections remaining, returning to normal mode.\n\r", ch);
	    interpret_select(ch, "quit");
	    return;
	  }//end if picks
	}//end switch
      }//end if has not skill
    }//end if skill passed
  }//end
  else if (!str_cmp(arg1, "forfeit")){
    if (str_cmp(argument, "confirm")){
      send_to_char("This will PERMAMENTLY skip this group of selections.\n\r"\
		   "Use: \"forfeit confirm\" to proceed.\n\r", ch);
    }
    else{
      int group = 0;
      int level = 0;
      /* get group and level info */
      if ( (group = can_ss_select(ch, &level)) == 0){
	bug("interp_select: Selection was allowed with no valid selections!", 0);
	send_to_char("You have no selections, returning to normal mode.\n\r", ch);
	interpret_select(ch, "quit");
	return;
      }
      ch->pcdata->ss_picked[group] += can_group_select(ch, group, level);
      send_to_char("Skipping category...\n\rReturning to normal mode...\n\r", ch);
      interpret_select(ch, "quit");
      ch->desc->connected = CON_PLAYING; 
      return;
    }
  }
  else{
    interpret_select(ch, "" );
    return;
  }
  send_to_char("Cmds: quit, help, select, forfeit>  ", ch);
}
