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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"


/* PRIVATE FUNCTIONS */
//resets the pool used for keeping track of which spells are memorized
void reset_dndmemory( DNDS_MEMORY* dndmemory ){
  memset(dndmemory, 0, sizeof(DNDS_MEMORY) * MAX_SKILL);    
}

//uses up all dndspells in given header
void forget_dndspells( DNDS_HEADER* dndsh){
  int level = 0;
  int spell = 0;
  
  for (level = 0; level < DNDS_MAXLEVEL; level++){
    for (spell = 0; spell < DNDS_MAXREMEM; spell++){
      SET_DNDMEM_H(&dndsh[level], spell, 0);
    }
    dndsh[level].cur_total = 0;      
    dndsh[level].mem_ticks = 0;      
  }
}

//updates level total in all templates
void update_level_total( PC_DATA* pc, int level ){
  int template;
  int spell;
  for (template = 0; template < DNDS_TEMPLATES; template++){
    DNDS_HEADER* dndsh = &pc->dndtemplates[template][level];
    int cur_total = 0;
    for (spell = 0; spell < DNDS_MAXREMEM; spell++){
      cur_total += GET_DNDMEM_H(dndsh, spell);
    }
    dndsh->cur_total = cur_total;
  }
}

//forgets spells wich are not in current template.
void forget_dndtemplates( PC_DATA* pc ){
  int level;
  int spell;
  int template;

  /* there are four steps
     1) Run through selected template storing memorized amount in each spell
     2) clear spell memory
     3) Run trhough selected template restoring memorized amount in each spell
     4) Run through all templates and set sn = 0 (unused) on spells with set=0
  */

  for (level = 0; level < DNDS_MAXLEVEL; level++){
    DNDS_HEADER* dndsh = &pc->dndspells[level];
    for (spell = 0; spell < DNDS_MAXREMEM; spell++){
      dndsh->spells[spell].mem = GET_DNDMEM_H(dndsh, spell );
      //at this point we floor any values of set maximums
      if (dndsh->spells[spell].mem > dndsh->spells[spell].set)
	dndsh->spells[spell].mem = dndsh->spells[spell].set;
    }
  }
  //now reset memory
  reset_dndmemory( pc->dndmemory );

  //run through template again restoring the memorized amounts
  for (level = 0; level < DNDS_MAXLEVEL; level++){
    DNDS_HEADER* dndsh = &pc->dndspells[level];
    for (spell = 0; spell < DNDS_MAXREMEM; spell++){
      SET_DNDMEM_H(dndsh, spell, dndsh->spells[spell].mem);
    }
  }

  //now run through all spell groups and look for set <= 0 
  for (template = 0; template < DNDS_TEMPLATES; template++){
    for (level = 0; level < DNDS_MAXLEVEL; level++){
      DNDS_HEADER* dndsh = &pc->dndtemplates[template][level];
      int cur_total = 0;
      for (spell = 0; spell < DNDS_MAXREMEM; spell++){
	//check for clearing
	if (dndsh->spells[spell].set < 1){
	  dndsh->spells[spell].sn = 0;
	  continue;
	}
	cur_total += GET_DNDMEM_H(dndsh, spell);
      }
      dndsh->cur_total = cur_total;
    }
  }
}

//resets all the dndspell data to 0 for a character
void reset_dndspells(PC_DATA* pc ){
  int i = 0;
  int j = 0;
  
  memset(pc->dndtemplates, 0, sizeof( DNDS_HEADER) * DNDS_MAXLEVEL * DNDS_TEMPLATES);

  /* set pointers in the dnd spell headers to the spell memory  */
  for (i = 0; i < DNDS_TEMPLATES; i++){
    for (j = 0; j < DNDS_MAXLEVEL; j++){
      pc->dndtemplates[i][j].memory = pc->dndmemory;
    }
  }
  pc->dndspells = pc->dndtemplates[0];
  reset_dndmemory( pc->dndmemory );
}

//returns max dnds level by class
int get_dnds_maxlevel( int class ){
  int max = 0;
  if (class == class_lookup("psionicist"))	max =  6;
  else						max =  6;

  return (UMIN(max, DNDS_MAXLEVEL));
}

//return maximum spells per category
int get_dnds_maxset( int class, int ch_level, int level ){
  int max = 0;
  
  if (class == class_lookup("psionicist")){
    switch ( level ){
    case 0:	max = DNDS_MAXREMEM;	break;
    case 1:	max = 9;		break;
    case 2:	max = 7;		break;
    case 3:	max = 5;		break;
    case 4:	max = 3;		break;
    case 5:	max = 1;		break;
    default: return 0;
    }
    if (ch_level < 25)
      max = UMAX(1, max / 2);
  }
  else{
    max = get_dnds_maxlevel( class ) - level;
    if (ch_level < 25)
      max = UMAX(1, max / 2);
  }
  
  return UMIN(max, DNDS_MAXREMEM);
}  
    
    
//looks up an address of a given spell on a list
//set level < 0 for global search
DNDS_DATA* dnds_lookup( PC_DATA* pc, int sn, int level ){
  int spell;

  //if level is < 0 we search all categories
  if (level < 0){
    DNDS_DATA* dndsd;
    for (level = 0; level < DNDS_MAXLEVEL; level++){
      if ( (dndsd = dnds_lookup(pc, sn, level)) != NULL)
	return dndsd;
    }
    return NULL;
  }
//Selective search
  for (spell = 0; spell < DNDS_MAXREMEM; spell++){
    if (pc->dndspells[level].spells[spell].sn == sn)
      return (&pc->dndspells[level].spells[spell]);
  }
  return NULL;
}

//adds an amount of memorized and set to a single spell to the list
DNDS_DATA* add_dndspell(PC_DATA* pc, int sn, int level, int mem, int set){
  DNDS_DATA* dndsd = dnds_lookup( pc, sn, level );

  if (level >= DNDS_MAXLEVEL || level < 0)
    return NULL;
  else if (dndsd == NULL){
    int spell;
    //find first empty spot
    for (spell = 0; spell < DNDS_MAXREMEM; spell++){
      if (pc->dndspells[level].spells[spell].sn < 1){
	dndsd = &pc->dndspells[level].spells[spell];
	dndsd->set = 0;
	dndsd->mem = 0;
	dndsd->level = 0;
	break;
      }
    }
  }

  if (dndsd == NULL){
    bug("dndspell.c: add_spell could not find an empty spot. Level %d", level);
    return NULL;
  }

  dndsd->sn = sn;
  dndsd->level = level;
  dndsd->set = UMAX(0, dndsd->set + set);
  pc->dndspells[dndsd->level].set_total = UMAX(0, pc->dndspells[dndsd->level].set_total + set);

  SET_DNDMEM(pc, sn, UMAX(0, GET_DNDMEM(pc, sn) + mem));
  pc->dndspells[dndsd->level].cur_total = UMAX(0, pc->dndspells[dndsd->level].cur_total + mem);
  
  update_level_total( pc, level );
  
  return dndsd;
}

//shows list of levels and spells
char* show_dnds( CHAR_DATA* ch, PC_DATA* pc, int min_lvl, int max_lvl ){
  DNDS_DATA* dndsd;
  static char list[MSL];
  char buf[MSL];
  int level;
  int spell;
  int template = (ch->pcdata->dndspells - ch->pcdata->dndtemplates[0]) / DNDS_MAXLEVEL;

  min_lvl = UMAX(0, min_lvl );
  max_lvl = UMIN(DNDS_MAXLEVEL, max_lvl);

  sprintf(list, "%sSet %d: %s\n\r", "\n\r", template + 1, 
	  IS_NULLSTR(ch->pcdata->dndtemplate_name[template]) ? "" : ch->pcdata->dndtemplate_name[template]);

  for (level = min_lvl; level < max_lvl; level++){
    bool fFirst = TRUE;
    int tot_spell = 0;
    sprintf(buf, "%2d%s [%2d/%-2d]:",
	    level + 1,
	    level == 0 ? "st" : (level == 1 ? "nd" : (level == 2 ? "rd" : "th")),
	    pc->dndspells[level].cur_total,
	    pc->dndspells[level].set_total);
//	    get_dnds_maxset(ch->class, ch->level, level));
    strcat(list, buf );
    for (spell = 0; spell < DNDS_MAXREMEM; spell++){
      if (pc->dndspells[level].spells[spell].sn < 1)
	continue;
      else
	dndsd = &pc->dndspells[level].spells[spell];
      if (fFirst){
	sprintf(buf, " %-18s [%2d/%-2d]",
		skill_table[dndsd->sn].name,
		GET_DNDMEM(pc, dndsd->sn),
		dndsd->set);
	fFirst = FALSE;
      }
      else if (tot_spell % 2 == 1){
	sprintf(buf, "%-18s [%2d/%-2d]",
		skill_table[dndsd->sn].name,
		GET_DNDMEM(pc, dndsd->sn),
		dndsd->set);
      }
      else{
	sprintf(buf, "%13s %-18s [%2d/%-2d]",
		 "",
		skill_table[dndsd->sn].name,
		GET_DNDMEM(pc, dndsd->sn),
		dndsd->set);
      }
      if (tot_spell++ % 2 == 0)
	strcat(buf, " " );
      else
	strcat(buf, "\n\r" );
      //add onto the list
      strcat(list, buf );
    }
    if (fFirst || tot_spell % 2 != 0)
      strcat(list, "\n\r");
  }

  if (IS_NULLSTR(list)){
    sprintf(list, "None.\n\r");
  }
  else
    strcat(list, "\n\r");
  return (list);
}     

//writes memorized spells in teh pool
void fwrite_dndmemory( FILE* fp, DNDS_MEMORY* memory){
  int i = 0;
  for (i = 0; i < MAX_SKILL; i++){
    if (memory[i] > 0)
      fprintf(fp, "DndMem %s~ %d\n", skill_table[i].name, memory[i]);
  }
}

//writes a single spell data
void fwrite_dnds( FILE* fp, DNDS_DATA* dndsd, int template ){
  fprintf(fp, "DndSpell %s~ %d %d %d\n",
	  skill_table[dndsd->sn].name,
	  template,
	  dndsd->level,
	  dndsd->set);
}

//writes a single template
void fwrite_dnds_template(FILE* fp, DNDS_HEADER* dndsh, int template){
  int spell;
  int level;

  for (level = 0; level < DNDS_MAXLEVEL; level++){
    if (dndsh[level].set_total < 1)
      continue;
    for (spell = 0; spell < DNDS_MAXREMEM; spell++){
      if (dndsh[level].spells[spell].sn < 1)
	continue;
      else
	fwrite_dnds( fp, &dndsh[level].spells[spell], template);
    }
  }
}  
//reads a single spell entry, returns template number
int fread_dnds( FILE* fp, DNDS_DATA* dndsd, int char_ver ){
  char* name;
  int sn;
  int template = 0;

  name	= fread_string( fp );
  sn = skill_lookup( name );
  free_string( name );

  if (sn < 1){
    bug("dndspell.c:fread_dnds> Unknown spell.", 0);
    fread_number( fp );
    fread_number( fp );
    fread_number( fp );
    return -1;
  }
  dndsd->sn	= sn;
  if (char_ver >= 2017)
    template	= fread_number( fp );
  else
    template = 0;
  dndsd->level  = fread_number( fp );
  dndsd->mem	= 0;
  dndsd->set	= fread_number( fp );

  if (template > DNDS_TEMPLATES || template < 0){
    bug("dndspell.c:fread_dnds> Bad template.", template);
    return -1;
  }
  return template;
}

//resets the mem ticks for each spell group
void reset_memticks( PC_DATA* pc ){
  int level;

  for (level = 0; level < DNDS_MAXLEVEL; level++){
    pc->dndspells[level].mem_ticks = 0;
  }
}

//evenly distributed memorized spells
bool memorize_spells( DNDS_HEADER* dndsh, int gain ){
  int rem = 0;
  int spell;
  int low = 99;
  int low_spell = -1;

  gain = URANGE(0, dndsh->set_total - dndsh->cur_total, gain);

  if (dndsh->set_total < 1)
    return FALSE;
  else if (gain < 1)
    return FALSE;

  /* find the spell with least spells memorized */
  for (spell = 0; spell < DNDS_MAXREMEM; spell++){
    if (dndsh->spells[spell].sn < 1)
      continue;
    else if (GET_DNDMEM_H(dndsh, spell) >= dndsh->spells[spell].set){
      continue;
    }
    else if (GET_DNDMEM_H(dndsh, spell) < low){
      low = GET_DNDMEM_H(dndsh, spell);
      low_spell = spell;
    }
  }
  if (low_spell < 0)
    return FALSE;
  
  SET_DNDMEM_H(dndsh, low_spell, GET_DNDMEM_H(dndsh, low_spell) + gain);

  /*  check for too much gain */
  if (GET_DNDMEM_H(dndsh, low_spell) > dndsh->spells[low_spell].set){
    rem = GET_DNDMEM_H(dndsh, low_spell) - dndsh->spells[low_spell].set;
    SET_DNDMEM_H(dndsh, low_spell, dndsh->spells[low_spell].set);
    gain -= rem;
  }    
  dndsh->cur_total += gain;

  if (rem){
    memorize_spells( dndsh, rem );
  }
  return TRUE;
}

//evaluate if a person memorized spells
bool check_memticks( DNDS_HEADER* dndsh, int max_spells ){
  int gain = 0;
  int test = DNDS_STARTTICKS;
  //ez rejections
  if (dndsh->cur_total >= dndsh->set_total){
    //reset memticks, and move on
    dndsh->mem_ticks = 0;
    return FALSE;
  }
  else if (dndsh->mem_ticks < DNDS_STARTTICKS)
    return FALSE;
  gain = (dndsh->mem_ticks - test) * max_spells / DNDS_FULLTICKS;
  
  if (gain < 1)
    return FALSE;
  //decrease memticks by gain
  dndsh->mem_ticks -= gain * DNDS_FULLTICKS / max_spells;

  //raise the spells in this group by gain
  return (memorize_spells( dndsh, UMIN(gain, max_spells)) );

}

//add number of numticks
//0 none memorized, 1 memorized, 2 memorized and full
int add_memticks( PC_DATA* pc, int val, int class, int ch_level ){
  int level;
  bool fAdded = FALSE;
  bool fFull = TRUE;

  for (level = 0; level < DNDS_MAXLEVEL; level++){
    int max_spells = get_dnds_maxset(class, ch_level, level);
    /* make sure that the max is never higher then allowed */
    pc->dndspells[level].set_total = UMIN(pc->dndspells[level].set_total, max_spells);

    if (pc->dndspells[level].cur_total >= pc->dndspells[level].set_total )
      continue;
    pc->dndspells[level].mem_ticks = UMAX(0, pc->dndspells[level].mem_ticks + val);
    if (check_memticks(&pc->dndspells[level], max_spells)){
      fAdded = TRUE;
    }
  }

  //All spells ready check
  if (fAdded){
    for (level = 0; level < DNDS_MAXLEVEL; level++){
      if (pc->dndspells[level].cur_total >= pc->dndspells[level].set_total )
	continue;
      fFull = FALSE;
      break;
    }
  }

  if (fFull && fAdded)
    return ADD_MEMTICK_FULL;
  else if (fAdded)
    return ADD_MEMTICK_ADD;
  else
    return ADD_MEMTICK_FALSE;
}

/* INTERFACE */
//allows to set/show which spells to memorize
void do_memorize( CHAR_DATA* ch, char* argument ){
  int sn = 0, template = 0;
  char arg1[MIL];
  one_argument( argument, arg1);

  if (IS_NPC(ch))
    return;
  else if (!IS_DNDS(ch)){
    send_to_char("You are not required to memorize spells.\n\r", ch);
    return;
  }

  if (IS_NULLSTR(argument)){
    BUFFER* output;
    output = new_buf();
    add_buf(output, show_dnds(ch, ch->pcdata, 0, get_dnds_maxlevel(ch->class)));
    page_to_char(buf_string(output), ch);
    free_buf( output );
    return;
  }
  else if ( (template = atoi( argument)) > 0){
    if (template < 1 || template > DNDS_TEMPLATES){
      sendf(ch, "You may select Spell Sets 1 to %d.\n\r", DNDS_TEMPLATES);
      return;
    }
    else
      ch->pcdata->dndspells = ch->pcdata->dndtemplates[template - 1];
    do_memorize( ch, "");
    return;
  }
  else if (!str_cmp(arg1, "name")){
    int temp;
    argument = one_argument( argument, arg1);

    if (IS_NULLSTR(argument)){
      send_to_char("Syntax: memorize name <name>\n\r", ch);
      return;
    }
    temp = (ch->pcdata->dndspells - ch->pcdata->dndtemplates[0]) / DNDS_MAXLEVEL;
    free_string(ch->pcdata->dndtemplate_name[temp]);
    ch->pcdata->dndtemplate_name[temp] = str_dup( argument );
    sendf( ch, "Template now: %s\n\r", ch->pcdata->dndtemplate_name[temp]);
    return;
  }
  else if ( (sn = skill_lookup( argument)) > 0){
    DNDS_DATA* dndsd;
    int level;
    int max_lvl = get_dnds_maxlevel( ch->class );
    //check if this is a known spell
    if (get_skill(ch, sn) < 2){
      send_to_char("You are not proficient in that.\n\r", ch);
      return;
    }
    else if (skill_table[sn].spell_fun == spell_null){
      send_to_char("That's not a spell.\n\r", ch);
      return;
    }
    //check that it is a memorizable spell
    else if (IS_GNBIT(sn, GN_NO_DND)){
      send_to_char("That spell cannot be memorized.\n\r", ch);
      return;
    }
    //get level for this skill
    else if ( (level = sklevel(ch, sn) / 10) > max_lvl || level < 0){
      send_to_char("You are not capable of memorizing that spell.\n\r", ch);
      return;
    }
    //check that we have a spot open
    else if (ch->pcdata->dndspells[level].set_total >= get_dnds_maxset( ch->class, ch->level, level)){
      sendf(ch, "You cannot memorize more spells in %d%s Circle.\n\r",
	    level + 1,
	    level == 0 ? "st" : (level == 1 ? "nd" : (level == 2 ? "rd" : "th")));
      return;
    }

    //seems all good, add it
    if ( (dndsd = add_dndspell(ch->pcdata, sn, level, 0, 1)) != NULL){
      sendf(ch, "You will memorize %d spells of %s.\n\r",
	    dndsd->set,
	    skill_table[dndsd->sn].name);
    }
    else{
      send_to_char("Error.\n\r", ch);
    }
  }
  else{
    send_to_char("You do not posses such spell.\n\r", ch);
    return;
  }
}


void do_forget( CHAR_DATA* ch, char* argument ){
  int sn = 0;
  if (IS_NPC(ch))
    return;
  else if (!IS_DNDS(ch)){
    send_to_char("You are not required to memorize spells.\n\r", ch);
    return;
  }
  
  if (IS_NULLSTR(argument)){
    BUFFER* output;
    output = new_buf();
    add_buf(output, show_dnds(ch, ch->pcdata, 0, get_dnds_maxlevel(ch->class)));
    page_to_char(buf_string(output), ch);
    free_buf( output );
    return;
  }
  else if (!str_cmp("all", argument)){
    int level = 0;
    int spell = 0;
    int max_level = get_dnds_maxlevel(ch->class);
    
    for (level = 0; level < max_level; level++){
      for (spell = 0; spell < DNDS_MAXREMEM; spell++){
	memset(&ch->pcdata->dndspells[level].spells[spell], 0, sizeof(DNDS_DATA));
      }
      ch->pcdata->dndspells[level].set_total = 0;      
      ch->pcdata->dndspells[level].cur_total = 0;      
      ch->pcdata->dndspells[level].mem_ticks = 0;      
    }
    send_to_char("Done.\n\r", ch);
    return;
  }
  else if ( (sn = skill_lookup( argument)) > 0){
    DNDS_DATA* dndsd;

    if ( (dndsd = dnds_lookup(ch->pcdata, sn, -1)) == NULL){
      send_to_char("You have not set such spell to be memorized.\n\r", ch);
      return;
    }
    //seems all good, rem it
    if ( (dndsd = add_dndspell(ch->pcdata, sn, dndsd->level, 0, -1)) != NULL){
      sendf(ch, "You will memorize %d spells of %s.\n\r",
	    dndsd->set,
	    skill_table[sn].name);
    }
    else{
      send_to_char("Error.\n\r", ch);
    }
  }
  else{
    send_to_char("You do not posses such spell.\n\r", ch);
    return;
  }
}

void dnds_memorize_update( CHAR_DATA* ch ){
  int gain = DNDS_MEMTICK;
  int res = 0;

/* PSI SPELL LOSS DUE TO POSITION */
  if (ch->position < POS_RESTING
      && !IS_NPC(ch) 
      &&  number_percent() < 15 && dnds_psicheck( ch )){
    send_to_char("You lose control of your spells.\n\r", ch);
  }
  if (ch->position != POS_MEDITATE)
    return;
  else if (!IS_DNDS(ch))
    return;

  //reset memorized spells to what is in current template
  forget_dndtemplates( ch->pcdata );

  //gain bonus for wis/int
  if (get_curr_stat(ch, STAT_INT) >= 23)
    gain ++;
  if (get_curr_stat(ch, STAT_WIS) >= 23)
    gain ++;

  res = add_memticks( ch->pcdata, gain, ch->class, ch->level);
  if ( res == ADD_MEMTICK_ADD)
    send_to_char("You've memorized a spell.\n\r", ch);
  else if (res == ADD_MEMTICK_FULL)
    send_to_char("You've memorized all your spells.\n\r", ch);
}

//sets beneficial spells with AFLAG_PSI to 0 duration
bool dnds_psicheck( CHAR_DATA* ch){
  AFFECT_DATA* paf;
  bool fFound = FALSE;

  if (IS_NPC(ch)){
    return FALSE;
  }
  else if (ch->position != POS_MEDITATE)
    return FALSE;

  for (paf = ch->affected; paf; paf = paf->next){
    if (paf->duration == 0)
      continue;
    if (IS_GNBIT(paf->type, GN_BEN) && IS_GNBIT(paf->type, GN_PSI)){
      paf->duration = 0;
      fFound = TRUE;
    }
  }
  return fFound;
}

//uses up a spell that was cast
bool useup_dnds( PC_DATA* pc, int sn ){
  DNDS_DATA* dndsd = dnds_lookup( pc, sn, -1 );
  
  if (dndsd == NULL){
    return FALSE;
  }
  else{
    add_dndspell(pc, sn, dndsd->level, -1, 0);
    return TRUE;
  }
}
