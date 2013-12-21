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
 * and -Mirlan-: The Age of Mirlan Mud by Virigoth                        *
 *                                                                         *
 * Continued Production of this code is available at www.flcodebase.com    *
 ***************************************************************************/


#include <sys/types.h>
#include <time.h>
#if !defined( ultrix )
#include <memory.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "vote.h"
#include "cabal.h"
#include "bounty.h"
#include "olc.h"
#include "recycle.h"
#include "alias.h"
#include "clan.h"

/* stuff for recycling mobprograms */
PROG_LIST *mprog_free;
PROG_LIST *oprog_free;
PROG_LIST *rprog_free;

CHALLENGE_DATA* new_challenge(){
  CHALLENGE_DATA* chal = alloc_perm( sizeof( *chal) );

  chal->next	= NULL;
  chal->name	= NULL;
  chal->record	= NULL;

  chal->win	= 0;
  chal->loss	= 0;
  chal->tie	= 0;
  chal->refused	= 0;

  return chal;
}

void free_challenge( CHALLENGE_DATA* chal ){
  if (chal->name )
    free_string( chal->name );
  if (chal->record )
    free_string( chal->record );
  free_mem( chal, sizeof(*chal) );
}

QUEST_DATA *quest_free;
QUEST_DATA *new_quest()
{
  QUEST_DATA *quest;
  if (quest_free == NULL)
    quest = alloc_perm(sizeof(*quest));
  else
    {
      quest = quest_free;
      quest_free = quest_free->next;
    }
  VALIDATE(quest);
  return quest;
}

void free_quest(QUEST_DATA *quest)
{
  if (!IS_VALID(quest))
    return;
  if (!IS_NULLSTR(quest->quest))
    free_string( quest->quest    );
  quest->next = NULL;

  INVALIDATE(quest);
  quest->next	= quest_free;
  quest_free	= quest;
}


NOTE_DATA *note_free;
NOTE_DATA *new_note()
{
    NOTE_DATA *note;
    if (note_free == NULL)
	note = alloc_perm(sizeof(*note));
    else
    {
	note = note_free;
	note_free = note_free->next;
    }
    VALIDATE(note);
    return note;
}


void free_note(NOTE_DATA *note)
{
    if (!IS_VALID(note))
	return;
    free_string( note->text    );
    free_string( note->subject );
    free_string( note->to_list );
    free_string( note->date    );
    free_string( note->sender  );
    INVALIDATE(note);
    note->next = note_free;
    note_free   = note;
}

BAN_DATA *ban_free;
BAN_DATA *new_ban(void)
{
    static BAN_DATA ban_zero;
    BAN_DATA *ban;
    if (ban_free == NULL)
	ban = alloc_perm(sizeof(*ban));
    else
    {
	ban = ban_free;
	ban_free = ban_free->next;
    }
    *ban = ban_zero;
    VALIDATE(ban);
    ban->name = &str_empty[0];
    return ban;
}

void free_ban(BAN_DATA *ban)
{
    if (!IS_VALID(ban))
	return;
    free_string(ban->name);
    INVALIDATE(ban);
    ban->next = ban_free;
    ban_free = ban;
}

DESCRIPTOR_DATA *descriptor_free;
DESCRIPTOR_DATA *new_descriptor(void)
{
    static DESCRIPTOR_DATA d_zero;
    DESCRIPTOR_DATA *d;
    if (descriptor_free == NULL)
	d = alloc_perm(sizeof(*d));
    else
    {
	d = descriptor_free;
	descriptor_free = descriptor_free->next;
    }
    *d = d_zero;
    VALIDATE(d);
    d->connected        = CON_GET_NAME;
    d->showstr_head     = NULL;
    d->showstr_point = NULL;
    d->outsize  = 2000;
    d->outbuf   = alloc_mem( d->outsize );
    d->pEdit         = NULL;
    d->pString       = NULL;
    d->editor        = 0;
    d->ifd           = -1;
    d->ipid          = -1;
    d->timer	     = 0;
    return d;
}

void free_descriptor(DESCRIPTOR_DATA *d)
{
    if (!IS_VALID(d))
	return;
    free_string( d->host );
    free_string( d->ident );
    free_mem( d->outbuf, d->outsize );
    INVALIDATE(d);
    d->next = descriptor_free;
    descriptor_free = d;
}

EXTRA_DESCR_DATA *extra_descr_free;
EXTRA_DESCR_DATA *new_extra_descr(void)
{
    EXTRA_DESCR_DATA *ed;
    if (extra_descr_free == NULL)
	ed = alloc_perm(sizeof(*ed));
    else
    {
	ed = extra_descr_free;
	extra_descr_free = extra_descr_free->next;
    }
    ed->keyword = &str_empty[0];
    ed->description = &str_empty[0];
    VALIDATE(ed);
    return ed;
}

void free_extra_descr(EXTRA_DESCR_DATA *ed)
{
    if (!IS_VALID(ed))
	return;
    free_string(ed->keyword);
    free_string(ed->description);
    INVALIDATE(ed);
    ed->next = extra_descr_free;
    extra_descr_free = ed;
}

TRAP_DATA *trap_free;
TRAP_DATA *new_trap(void)
{
    static TRAP_DATA tr_zero;
    TRAP_DATA *tr;
    if (trap_free == NULL){
      tr = alloc_perm(sizeof(*tr));
    }
    else{
      tr = trap_free;
      trap_free = trap_free->next;
      tr->next = NULL;
    }
    *tr = tr_zero;

    tr->name = NULL;
    tr->echo = NULL;
    tr->oEcho = NULL;
    tr->owner =	NULL;
    tr->on_obj=	NULL;
    tr->on_exit=NULL;

    VALIDATE(tr);
    return tr;
}
void free_trap(TRAP_DATA *tr){
  if (!IS_VALID(tr))
    return;
  INVALIDATE(tr);

  tr->owner = NULL;
  tr->on_obj = NULL;
  tr->on_exit = NULL;
  if (!IS_NULLSTR( tr->name ))
    free_string( tr->name );
  if (!IS_NULLSTR( tr->echo ))
    free_string( tr->echo );
  if (!IS_NULLSTR( tr->oEcho ))
    free_string( tr->oEcho );

  tr->next = trap_free;
  trap_free = tr;
}

TRAP_INDEX_DATA *trap_index_free;
TRAP_INDEX_DATA *new_trap_index(void)
{
    static TRAP_INDEX_DATA tr_zero;
    int i = 0;
    TRAP_INDEX_DATA *tr;
    if (trap_index_free == NULL){
      tr = alloc_perm(sizeof(*tr));
    }
    else{
      tr = trap_index_free;
      trap_index_free = trap_index_free->next;
      tr->next = NULL;
    }
    *tr = tr_zero;
    top_trap_index++;
    VALIDATE(tr);

/* reset initial things */
    tr->name		= str_dup("a trap");
    tr->echo		= str_dup("");
    tr->oEcho		= str_dup("");
    tr->type		= TTYPE_DAMAGE;
    tr->level		= 1;
    tr->duration	= -1;
    tr->flags		= TRAP_NONE;
    for (i =0; i< MAX_TRAPVAL; i++){
      tr->value[i] = 1;
    }
/* return pointer */
    return tr;
}
void free_trap_index(TRAP_INDEX_DATA *tr){
  if (!IS_VALID(tr))
    return;
  INVALIDATE(tr);

/* free things needing freeing */
  free_string( tr->name );
  free_string( tr->echo );
  free_string( tr->oEcho );

/* done */

  tr->next = trap_index_free;
  trap_index_free = tr;
  top_trap_index--;
}

CMEMBER_DATA *new_cmember(void){

  CMEMBER_DATA *cm = malloc( sizeof( *cm ));

/* reset initial things */
  memset( cm, 0, sizeof( CMEMBER_DATA ));

  cm->name		= str_dup("");
  cm->sponsor		= str_dup("");
  cm->sponsored		= str_dup("");

  cm->last_update	= mud_data.current_time;


  return cm;
};

void free_cmember( CMEMBER_DATA* cm ){

  free_string( cm->name );
  free_string( cm->sponsor );
  free_string( cm->sponsored );

  free( cm );
}


CABAL_INDEX_DATA *cabal_index_free;
CABAL_INDEX_DATA *new_cabal_index(void)
{
    static CABAL_INDEX_DATA ca_zero;
    CABAL_INDEX_DATA *ca;

    if (cabal_index_free == NULL){
      ca = alloc_perm(sizeof(*ca));
    }
    else{
      ca = cabal_index_free;
      cabal_index_free = cabal_index_free->next;
      ca->next = NULL;
    }
    *ca = ca_zero;
    memset( ca, 0, sizeof ( CABAL_INDEX_DATA ));

/* reset initial things */
    ca->name		= str_dup("cabal");
    ca->immortal        = str_dup("");
    ca->who_name        = str_dup("");
    ca->currency        = str_dup("");
    ca->gate_on         = str_dup("");
    ca->gate_off        = str_dup("");
    ca->gate_msg        = str_dup("");
    ca->gate_on		= str_dup("");
    ca->gate_off	= str_dup("");
    ca->gate_msg	= str_dup("");
    ca->city_name	= str_dup("");

    ca->skills		= NULL;
    ca->votes		= NULL;

    ca->next		= NULL;
    ca->parent_vnum	= 0;

    ca->anchor_vnum	= 0;
    ca->guard_vnum	= 0;

    ca->army		= 0;
    ca->army_upgrade    = 0;
    ca->tower		= 0;
    ca->tower_upgrade	= 0;

    ca->mprefix		= NULL;
    ca->fprefix		= NULL;

/* return pointer */
    return ca;
}
void free_cabal_index(CABAL_INDEX_DATA *ca){
  CSKILL_DATA* cSkill = ca->skills;
  CVOTE_DATA* cVote = ca->votes;
  int i = 0;

/* free things needing freeing */
  free_string( ca->name );
  free_string( ca->who_name );
  free_string( ca->immortal );
  free_string( ca->currency );

  while ( ca->skills ){
    cSkill = ca->skills;
    ca->skills = ca->skills->next;

    free_skill( cSkill->pSkill );
    free( cSkill );
  }

  while ( ca->votes ){
    cVote = ca->votes;
    ca->votes = ca->votes->next;

    free( cVote );
  }

  for (i = 0; i < CROOM_LEVELS; i ++){
    if (ca->crooms[i] == NULL)
      continue;
    free_croom( ca->crooms[i] );
    ca->crooms[i] = NULL;
  }

  for (i = 0; i < RANK_MAX; i++){
    if (!IS_NULLSTR(ca->mranks[i]))
      free_string( ca->mranks[i] );
    if (!IS_NULLSTR(ca->franks[i]))
      free_string( ca->franks[i] );
  }
/* done */

  ca->next = cabal_index_free;
  cabal_index_free = ca;
}

HELP_DATA *help_index_free;
HELP_DATA *new_help_index(void)
{
    static HELP_DATA hp_zero;
    HELP_DATA *hp;
    if (help_index_free == NULL){
      hp = alloc_perm(sizeof(*hp));
    }
    else{
      hp = help_index_free;
      help_index_free = help_index_free->next;
      hp->next = NULL;
    }
    *hp = hp_zero;
    top_help ++;

/* return pointer */
    return hp;
}
void free_help_index(HELP_DATA *hp){
/* free things needing freeing */
  free_string( hp->keyword );
  free_string( hp->text );

/* done */
  hp->next = help_index_free;
  help_index_free = hp;
  top_help --;
}

AFFECT_DATA *affect_free;
AFFECT_DATA *new_affect(void)
{
    static AFFECT_DATA af_zero;
    AFFECT_DATA *af;
    if (affect_free == NULL)
      {
	af = alloc_perm(sizeof(*af));
	af->has_string = FALSE;
	af->string = &str_empty[0];
      }
    else
    {
	af = affect_free;
	af->has_string = FALSE;
	af->string = &str_empty[0];
	affect_free = affect_free->next;
        af->next = NULL;
    }
    *af = af_zero;
//reset pointers and safety for the pointer.
    af->has_string = FALSE;
    af->string = &str_empty[0];
    VALIDATE(af);
    return af;
}

OBJ_SPELL_DATA *objspell_free;
OBJ_SPELL_DATA *new_obj_spell(void)
{
    static OBJ_SPELL_DATA sp_zero;
    OBJ_SPELL_DATA *sp;

    if (objspell_free == NULL)
      {
	sp = alloc_perm(sizeof(*sp));
	//set up the strings.
	sp->message = NULL;
	sp->message2 = NULL;
      }
    else
      {
	sp = objspell_free;
	objspell_free = objspell_free->next;
        sp->next = NULL;
    }
    *sp = sp_zero;
    return sp;
}

void free_obj_spell(OBJ_SPELL_DATA *sp)
{
    /* we free the strings. */

   if (sp->message != NULL)
     {
       free_string(sp->message);
       sp->message = NULL;
     }
   if (sp->message2 != NULL)
     {
       free_string(sp->message2);
       sp->message = NULL;
     }

  //save the spell.
    sp->next = objspell_free;
    objspell_free = sp;
}

void free_affect(AFFECT_DATA *af)
{
    if (!IS_VALID(af))
	return;
    INVALIDATE(af);

    /* we free the string. */
    string_from_affect(af);
    af->next = affect_free;
    affect_free = af;
}

SKILL_DATA *skill_free;
SKILL_DATA *new_skill(void)
{
    static SKILL_DATA sk_zero;
    SKILL_DATA *sk;
    if (skill_free == NULL)
	sk = alloc_perm(sizeof(*sk));
    else
    {
	sk = skill_free;
	skill_free = skill_free->next;
        sk->next = NULL;
    }
    *sk = sk_zero;
    VALIDATE(sk);
    return sk;
}

void free_skill(SKILL_DATA *sk)
{
    if (!IS_VALID(sk))
	return;
    INVALIDATE(sk);
    sk->next = skill_free;
    skill_free = sk->next;
}

OBJ_DATA *obj_free;
OBJ_DATA *new_obj(void)
{
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;
    if (obj_free == NULL)
	obj = alloc_perm(sizeof(*obj));
    else
    {
	obj = obj_free;
	obj_free = obj_free->next;
    }
    *obj = obj_zero;
    VALIDATE(obj);
    return obj;
}

void free_obj(OBJ_DATA *obj)
{
    AFFECT_DATA *paf, *paf_next;
    OBJ_SPELL_DATA* sp, *sp_next;
    EXTRA_DESCR_DATA *ed, *ed_next;
    if (!IS_VALID(obj))
	return;
    for (paf = obj->affected; paf != NULL; paf = paf_next)
    {
	paf_next = paf->next;
	free_affect(paf);
    }
    obj->affected = NULL;

    //free spells
    for (sp = obj->spell; sp != NULL; sp = sp_next){
      sp_next = sp->next;
      free_obj_spell(sp);
    }
    obj->spell = NULL;

    for (ed = obj->extra_descr; ed != NULL; ed = ed_next )
    {
	ed_next = ed->next;
	free_extra_descr(ed);
    }
    obj->extra_descr = NULL;
    free_string( obj->name        );
    free_string( obj->description );
    free_string( obj->short_descr );
    free_string( obj->material	  );
    INVALIDATE(obj);
    obj->next   = obj_free;
    obj_free    = obj;
}

CHAR_DATA *char_free;
CHAR_DATA *new_char (void)
{
    static CHAR_DATA ch_zero;
    CHAR_DATA *ch;
    int i;
    if (char_free == NULL)
	ch = alloc_perm(sizeof(*ch));
    else
    {
	ch = char_free;
	char_free = char_free->next;
    }
    *ch				= ch_zero;
    VALIDATE(ch);
    ch->in_room			= NULL;
    ch->was_in_room		= NULL;
    ch->leader			= NULL;
    ch->master			= NULL;
    ch->summoner		= NULL;
    ch->name                    = &str_empty[0];
    ch->short_descr             = &str_empty[0];
    ch->long_descr              = &str_empty[0];
    ch->description             = &str_empty[0];
    ch->prompt                  = &str_empty[0];
    ch->prefix			= &str_empty[0];
    ch->logon                   = mud_data.current_time;
    ch->lines                   = 0;
    for (i = 0; i < 4; i++)
        ch->armor[i]            = 100;
    ch->position                = POS_STANDING;
    if (!IS_NPC(ch))
    {
        ch->hit                 = pc_race_table[ch->race].hit;
        ch->mana                = pc_race_table[ch->race].mana;
    }
    else
    {
        ch->hit                 = 20;
        ch->mana                = 100;
    }
    ch->max_hit                 = ch->hit;
    ch->max_mana                = ch->mana;
    ch->move                    = 100;
    ch->max_move                = ch->move;
    for (i = 0; i < MAX_STATS; i ++)
    {
        ch->perm_stat[i] = 13;
        ch->mod_stat[i] =   0;
    }
    for (i = 0; i < MAX_TRAPS; i++)
        ch->traps[i]            = NULL;
    return ch;
}

void free_char (CHAR_DATA *ch){

    OBJ_DATA *obj, *obj_next;
    AFFECT_DATA *paf, *paf_next;

    if (!IS_VALID(ch))
	return;

    if (IS_NPC(ch))
	mobile_count--;
    //log_string("Freeing objects");
    for (obj = ch->carrying; obj != NULL; obj = obj_next)
    {
	obj_next = obj->next_content;
	extract_obj(obj);
    }
    //log_string("Freeing Affects");
    for (paf = ch->affected; paf != NULL; paf = paf_next)
    {
	paf_next = paf->next;
	affect_remove(ch,paf);
    }
    //log_string("Freeing Strings");
    free_string(ch->name);
    free_string(ch->short_descr);
    free_string(ch->long_descr);
    free_string(ch->description);
    free_string(ch->prompt);
    free_string(ch->prefix);

    if (ch->spec_path )
      clean_path_queue( ch->spec_path );

    //log_string("Freeing PC DAtA");
    if (ch->pcdata != NULL)
    	free_pcdata(ch->pcdata);

    ///log_string("Re Pointing");
    ch->next = char_free;
    char_free  = ch;
    ///log_string("Invalidating");
    INVALIDATE(ch);
    //log_string("Done");
}

PC_DATA *pcdata_free;
PC_DATA *new_pcdata(void)
{
    static PC_DATA pcdata_zero;
    PC_DATA *pcdata;
    if (pcdata_free == NULL)
	pcdata = alloc_perm(sizeof(*pcdata));
    else
    {
	pcdata = pcdata_free;
	pcdata_free = pcdata_free->next;
    }
    *pcdata = pcdata_zero;
    pcdata->buffer = new_buf();
    VALIDATE(pcdata);
    return pcdata;
}

void free_pcdata(PC_DATA *pcdata)
{
    int i;
    if (!IS_VALID(pcdata))
	return;
    free_string(pcdata->pwd);
    free_string(pcdata->pwddel);
    free_string(pcdata->pwdimm);
    free_string(pcdata->bamfin);
    free_string(pcdata->bamfout);
    free_string(pcdata->title);
    free_string(pcdata->ignore);
    free_string(pcdata->family);
    free_string(pcdata->alias);
    free_string(pcdata->race);
    free_string(pcdata->clan);
    free_string(pcdata->royal_guards);
    while (pcdata->quests){
      QUEST_DATA* q = pcdata->quests->next;
      free_quest(pcdata->quests);
      pcdata->quests = q;
    }
    while(pcdata->aliases){
      ALIAS_DATA* al = pcdata->aliases;
      rem_alias( pcdata, al );
      free_alias( al );
    }
    free_buf(pcdata->buffer);
    free_note(pcdata->pnote);
    free_vote(pcdata->pvote);
    free_bbid(pcdata->pbbid);
    free_cvroom(pcdata->pcvroom);
    for (i = 0; i < DNDS_TEMPLATES; i++){
      free_string(pcdata->dndtemplate_name[i]);
    }
    INVALIDATE(pcdata);
    pcdata->next = pcdata_free;
    pcdata_free = pcdata;
}

long    last_pc_id;
long	last_mob_id;

long get_pc_id(void)
{
    int val = (mud_data.current_time <= last_pc_id) ? last_pc_id + 1 : mud_data.current_time;
    last_pc_id = val;
    return val;
}

long get_mob_id(void)
{
    last_mob_id++;
    return last_mob_id;
}

BUFFER *buf_free;

const int buf_size[MAX_BUF_LIST] =
{ 16,32,64,128,256,1024,2048,4096,8192,16384,24567 };

/* local procedure for finding the next acceptable size *
 * -1 indicates out-of-boundary error                   */
int get_size (int val)
{
    int i;
    for (i = 0; i < MAX_BUF_LIST; i++)
      if (buf_size[i] >= val)
	return buf_size[i];
    return -1;
}

BUFFER *new_buf()
{
    BUFFER *buffer;
    if (buf_free == NULL)
	buffer = alloc_perm(sizeof(*buffer));
    else
    {
	buffer = buf_free;
	buf_free = buf_free->next;
    }
    buffer->next	= NULL;
    buffer->state	= BUFFER_SAFE;
    buffer->size	= get_size(BASE_BUF);
    buffer->string	= alloc_mem(buffer->size);
    buffer->string[0]	= '\0';
    VALIDATE(buffer);
    return buffer;
}

BUFFER *new_buf_size(int size)
{
    BUFFER *buffer;
    if (buf_free == NULL)
        buffer = alloc_perm(sizeof(*buffer));
    else
    {
        buffer = buf_free;
        buf_free = buf_free->next;
    }
    buffer->next        = NULL;
    buffer->state       = BUFFER_SAFE;
    buffer->size        = get_size(size);
    if (buffer->size == -1)
    {
        bug("new_buf: buffer size %d too large.",size);
        exit(1);
    }
    buffer->string      = alloc_mem(buffer->size);
    buffer->string[0]   = '\0';
    VALIDATE(buffer);
    return buffer;
}

void free_buf(BUFFER *buffer)
{
  if (!IS_VALID(buffer))
    return;
  free_mem(buffer->string,buffer->size);
  buffer->string = NULL;
  buffer->size   = 0;
  buffer->state  = BUFFER_FREED;
  INVALIDATE(buffer);
  buffer->next  = buf_free;
  buf_free      = buffer;
}

bool add_buf(BUFFER *buffer, char *string)
{
    int len, oldsize = buffer->size;
    char *oldstr = buffer->string;
    if (buffer->state == BUFFER_OVERFLOW)
	return FALSE;
    len = strlen(buffer->string) + strlen(string) + 1;
    while (len >= buffer->size)
    {
	buffer->size 	= get_size(buffer->size + 1);
	{
            if (buffer->size == -1)
	    {
		buffer->size = oldsize;
		buffer->state = BUFFER_OVERFLOW;
		bug("buffer overflow past size %d",buffer->size);
		return FALSE;
	    }
  	}
    }
    if (buffer->size != oldsize)
    {
	buffer->string	= alloc_mem(buffer->size);
	strcpy(buffer->string,oldstr);
	free_mem(oldstr,oldsize);
    }
    strcat(buffer->string,string);
    return TRUE;
}

void clear_buf(BUFFER *buffer)
{
    buffer->string[0] = '\0';
    buffer->state     = BUFFER_SAFE;
}

char *buf_string(BUFFER *buffer)
{
    return buffer->string;
}

BAN_DATA *ban_list;
void save_bans(void)
{
    BAN_DATA *pban;
    FILE *fp;
    bool found = FALSE;
    fclose( fpReserve );
    if ( ( fp = fopen( BAN_FILE, "w" ) ) == NULL )
    {
        perror( BAN_FILE );
	fp = fopen( NULL_FILE, "r" );
    }
    for (pban = ban_list; pban != NULL; pban = pban->next)
	if (IS_SET(pban->ban_flags,BAN_PERMANENT))
	{
	    found = TRUE;
            fprintf(fp,"%-20s %-2d %s\n",pban->name,pban->level,print_flags(pban->ban_flags));
	}
    fclose(fp);
    fpReserve = fopen( NULL_FILE, "r" );
    if (!found)
	unlink(BAN_FILE);
}

void load_bans(void)
{
    FILE *fp;
    BAN_DATA *ban_last;
    if ( ( fp = fopen( BAN_FILE, "r" ) ) == NULL )
    {
	fp = fopen( NULL_FILE, "r" );
	fclose (fp);
        return;
    }
    ban_last = NULL;
    for ( ; ; )
    {
        BAN_DATA *pban;
        if ( feof(fp) )
        {
            fclose( fp );
            return;
        }
        pban = new_ban();
        pban->name = str_dup(fread_word(fp));
	pban->level = fread_number(fp);
	pban->ban_flags = fread_flag(fp);
	fread_to_eol(fp);
        if (ban_list == NULL)
	    ban_list = pban;
	else
	    ban_last->next = pban;
	ban_last = pban;
    }
}

bool check_ban(char *site,int type)
{
    BAN_DATA *pban;
    char host[MSL];
    strcpy(host,capitalize(site));
    host[0] = LOWER(host[0]);
    for ( pban = ban_list; pban != NULL; pban = pban->next )
    {
	if(!IS_SET(pban->ban_flags,type))
	    continue;
	if (IS_SET(pban->ban_flags,BAN_PREFIX) && IS_SET(pban->ban_flags,BAN_SUFFIX) && strstr(host,pban->name) != NULL)
	    return TRUE;
	if (IS_SET(pban->ban_flags,BAN_PREFIX) && !str_suffix(pban->name,host))
	    return TRUE;
	if (IS_SET(pban->ban_flags,BAN_SUFFIX) && !str_prefix(pban->name,host))
	    return TRUE;
	if (!IS_SET(pban->ban_flags,BAN_PREFIX) && !IS_SET(pban->ban_flags,BAN_SUFFIX) && !str_cmp(pban->name,host))
	    return TRUE;
    }
    return FALSE;
}

void ban_site(CHAR_DATA *ch, char *argument, bool fPerm)
{
    char buf[MSL], buf2[MSL], arg1[MIL], arg2[MIL];
    char *name;
    BUFFER *buffer;
    BAN_DATA *pban, *prev = NULL;
    bool prefix = FALSE,suffix = FALSE;
    int type;
    argument = one_argument(argument,arg1);
    one_argument(argument,arg2);
    if ( arg1[0] == '\0' )
    {
	if (ban_list == NULL)
	{
	    send_to_char("No sites banned at this time.\n\r",ch);
	    return;
  	}
	buffer = new_buf();
        add_buf(buffer,"Banned sites  level  type     status\n\r");
        for (pban = ban_list;pban != NULL;pban = pban->next)
        {
            sprintf(buf2,"%s%s%s :%s", IS_SET(pban->ban_flags,BAN_PREFIX) ? "*" : "",
              pban->name, IS_SET(pban->ban_flags,BAN_SUFFIX) ? "*" : "", pban->text);
            sprintf(buf,"%-12s    %-3d  %-7s  %s\n\r", buf2, pban->level,
              IS_SET(pban->ban_flags,BAN_NEWBIES) ? "newbies" :
              IS_SET(pban->ban_flags,BAN_PERMIT)  ? "permit"  :
              IS_SET(pban->ban_flags,BAN_ALL)     ? "all"     : "",
              IS_SET(pban->ban_flags,BAN_PERMANENT) ? "perm" : "temp");
	    add_buf(buffer,buf);
        }
        page_to_char( buf_string(buffer), ch );
	free_buf(buffer);
        return;
    }
    if (arg2[0] == '\0' || !str_prefix(arg2,"all")){
	type = BAN_PERMIT;
	argument = one_argument(argument,arg2);
    }
    else if (!str_prefix(arg2,"newbies")){
	type = BAN_NEWBIES;
	argument = one_argument(argument,arg2);
    }
    else if (!str_prefix(arg2,"permit")){
	type = BAN_PERMIT;
	argument = one_argument(argument,arg2);
    }
    else if (!str_prefix(arg2,"all")){
	type = BAN_ALL;
	argument = one_argument(argument,arg2);
    }
    else
      type = BAN_PERMIT;
    name = arg1;
    if (name[0] == '*')
    {
	prefix = TRUE;
	name++;
    }
    if (name[strlen(name) - 1] == '*')
    {
	suffix = TRUE;
	name[strlen(name) - 1] = '\0';
    }
    if (strlen(name) == 0)
    {
        send_to_char("You have to ban something.\n\r",ch);
	return;
    }
    for ( pban = ban_list; pban != NULL; prev = pban, pban = pban->next )
        if (!str_cmp(name,pban->name))
        {
	    if (pban->level > get_trust(ch))
	    {
            	send_to_char( "That ban was set by a higher power.\n\r", ch );
            	return;
	    }
	    else
	    {
		if (prev == NULL)
		    ban_list = pban->next;
		else
		    prev->next = pban->next;
		free_ban(pban);
	    }
        }
    pban = new_ban();
    pban->name = str_dup(name);
    pban->text = str_dup(argument);
    pban->level = get_trust(ch);
    pban->ban_flags = type;
    if (prefix)
	SET_BIT(pban->ban_flags,BAN_PREFIX);
    if (suffix)
	SET_BIT(pban->ban_flags,BAN_SUFFIX);
    if (fPerm)
	SET_BIT(pban->ban_flags,BAN_PERMANENT);
    pban->next  = ban_list;
    ban_list    = pban;
    save_bans();
    sendf(ch,"%s has been banned.\n\r",pban->name);
}

void do_ban(CHAR_DATA *ch, char *argument)
{
    ban_site(ch,argument,FALSE);
}

void do_permban(CHAR_DATA *ch, char *argument)
{
    ban_site(ch,argument,TRUE);
}

void do_permit(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MIL];
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Permit or unpermit who to log in through ban?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    if (IS_SET(victim->act,PLR_PERMIT))
    {
        send_to_char("That person may not log in through ban anymore.\n\r",ch);
        REMOVE_BIT(victim->act,PLR_PERMIT);
    }
    else
    {
        send_to_char("That peson may now log in through ban.\n\r",ch);
        SET_BIT(victim->act,PLR_PERMIT);
    }
}


void do_allow( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    BAN_DATA *prev = NULL, *curr;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Remove which site from the ban list?\n\r", ch );
        return;
    }
    for ( curr = ban_list; curr != NULL; prev = curr, curr = curr->next )
        if ( !str_cmp( arg, curr->name ) )
        {
	    if (curr->level > get_trust(ch))
	    {
                send_to_char("You are not powerful enough to lift that ban.\n\r",ch);
		return;
	    }
            if ( prev == NULL )
                ban_list   = ban_list->next;
            else
                prev->next = curr->next;
            free_ban(curr);
	    sendf(ch,"Ban on %s lifted.\n\r",arg);
	    save_bans();
            return;
        }
    send_to_char( "Site is not banned.\n\r", ch );
}

typedef struct BE BufEntry;

#define intType int
#define intTypeSize (sizeof( intType ))
#define addrType void *
#define addrTypeSize (sizeof( addrType ))
#define addrSizeMask (sizeof( addrType ) - 1)

struct BE
{
    BufEntry	*	next;
    intType		size;	/* size of the chunk (regardless of NULL CHAR) */
    intType		usage;	/* how many pointers to the string */
    addrType		buf[1];	/* chunk starts here */
};

/* This is for the temporary hashing of strings at bootup to speedup   *
 * comparison/crunching of the string space. The temp_string_hash will *
 * be freed after boot_done() is called.                               */
typedef struct TH TempHash;

struct TH
{
    TempHash	*	next;
    intType			len;
    char	 	*	str;
};

TempHash **temp_string_hash;
char	str_empty[1];
char *	string_space;
char *	top_string;
long	nAllocString;
long	sAllocString;
long	nOverFlowString;
long	sOverFlowString;
int     numFree = 0;
bool	Full;
extern bool	fBootDb;

char *str_dup           ( const char * );
void free_string        ( char * );
char *fread_string      ( FILE * );
char *fread_word_dup    ( FILE * );


/* ssm_buf_head points to start of shared space, *
 * ssm_buf_free points to next free block        */
BufEntry *ssm_buf_head, *ssm_buf_free;

long	MAX_STRING = 1024*1024*12;
int		HEADER_SIZE;


static inline unsigned get_string_hash( register const char *key, int len)
{
    register int i = UMIN(len,32);
    register unsigned hash = 0;
    while(i--)
        hash = hash * 33U + *key++;
    return hash % MAX_KEY_HASH;
}

void init_string_space()
{
    init_malloc("init_string_space");
    if (mud_data.mudport == TEST_PORT)
      MAX_STRING = MAX_STRING;
    string_space = (char *)malloc( MAX_STRING );
    end_malloc("init_string_space");
    if( !string_space )
    {
        bug( "[SSM] Cant allocate shared string space.", 0 );
        exit(1);
    }
    numFree = 0;
    top_string = string_space + MAX_STRING-1;
    ssm_buf_head = (BufEntry *)string_space;
    HEADER_SIZE = (int)((char*)&ssm_buf_head->buf[0] - (char*)ssm_buf_head);
    ssm_buf_head->usage = 0;
    ssm_buf_head->size = MAX_STRING - HEADER_SIZE;
    ssm_buf_head->next = 0;
    ssm_buf_free = ssm_buf_head;
    init_malloc("calloc init_string_space");
    temp_string_hash = (TempHash **)calloc( sizeof(TempHash *), MAX_KEY_HASH );
    end_malloc("calloc init_string_space");
}

int defrag_heap()
{
   /* Walk through the shared heap and merge adjacent free blocks.            *
    * Free blocks are merged in str_free if free->next is free but            *
    * if the block preceding free is free, it stays unmerged. I would         *
    * rather not have the heap as a DOUBLE linked list for 2 reasons...       *
    *  (1) Extra 4 bytes per struct uses more mem                             *
    *  (2) Speed - don't want to bog down str_ functions with heap management *
    * The "orphaned" blocks will eventually be either merged or reused.       *
    * The str_dup function will call defrag if it cant allocate a buf.        */
    BufEntry *walk, *last_free, *next;
    int merges = 0;
    ssm_buf_free = 0;
    for( walk=ssm_buf_head,last_free=0; walk; walk = next )
    {
        next = walk->next;
        if( walk->usage > 0 )
        {
            last_free = 0;
            continue;
        }
        else if( !last_free )
        {
            last_free = walk;
            if( walk > ssm_buf_free )
                ssm_buf_free = walk;
            continue;
        }
        else
        {
	  merges++;
	  last_free->size += walk->size + HEADER_SIZE;
	  last_free->next = walk->next;
        }
    }
    if( merges )
        nlogf( "[SSM] defrag_heap : made %d block merges, Good.", merges );
    else
      nlogf( "[SSM] defrag_heap : resulted in 0 merges. Oh well.");
    numFree = 0;
    return merges;
}

/* Dup a string into shared space. If string exists, the usage count       *
 * gets incremented and the reference is returned. If the string does      *
 * not exist in heap, space is allocated and usage is 1.                   *
 * This is a linked list first fit algorithm, so strings can be            *
 * freed. Upon bootup, there is a seperate hash table constructed in order *
 * to do crunching, then the table is destroyed.                           */
char *str_dup( const char *str )
{
    BufEntry *ptr;
    int len, rlen;
    char *str_new;
    if( !str || !*str )
        return &str_empty[0];

    if( str > string_space && str < top_string ){
      ptr = (BufEntry *)(str - HEADER_SIZE);
      if( ptr->usage <= 0 ){
	bug( "str_dup : invalid str", 0 );
	bug( str, 0 );
      }
      ptr->usage++;
      return (char *)str;
    }
    rlen = len = (int)strlen( str ) + 1;
    /* Round up to machine dependant address size.                     *
    * Don't remove this, because when the BufEntry struct is overlaid *
    * the struct must be aligned correctly.                           */
    if( len & addrSizeMask )
      len += addrTypeSize - ( len & addrSizeMask );

    if( ssm_buf_free ){
    RETRY:
      /* look for first free string that is bigger or equal to what we have */
      for( ptr = ssm_buf_free; ptr; ptr = ptr->next ){
	if( ptr->usage <= 0 && ptr->size >= len )
	  break;
      }
      /* No free strings, or all the strings are smaller then what we need. (defragmentation) */
      if( !ptr ){
	if( numFree >= MAX_FREE ){
	  int merges;
	  log_string( "[SSM] Attempting to optimize shared string heap.");
	  merges = defrag_heap();
	  if( merges )
	    goto RETRY;
	}
	init_malloc("str_dup part 1");
	str_new = (char *)malloc( rlen );
	end_malloc("str_dup part 1");
	strcpy( str_new, str );
	sOverFlowString += rlen;
	nOverFlowString++;
	return str_new;
      }
      /* New string is smaller then free chunk of memeory, we split the chunk into two */
      else if( ptr->size-len >= HEADER_SIZE ){
	BufEntry *temp;
	temp = (BufEntry*)((char *)ptr + HEADER_SIZE + len);
	temp->size = ptr->size - (len + HEADER_SIZE);
	temp->next = ptr->next;
	temp->usage = 0;
	ptr->size = len;
	ptr->next = temp;
	if( ptr == ssm_buf_free )
	  ssm_buf_free = temp;
      }
      /* new chunk is bigger then the first free chunk */
      ptr->usage = 1;
      str_new = (char *)&ptr->buf[0];
      /* find the next "Free" string to be used */
      if( ptr == ssm_buf_free ){
	for( ssm_buf_free = ssm_buf_head; ssm_buf_free; ssm_buf_free = ssm_buf_free->next ){
	  if( ssm_buf_free->usage <= 0 )
	    break;
	}
      }
      strcpy( str_new, str );
      nAllocString++;
      sAllocString += len + HEADER_SIZE;
    }
    else{
      if( !Full ){
	bug( "[SSM] The shared string heap is full!", 0 );
	Full = 1;
      }
      init_malloc("str_dup part 2");
      str_new = (char *)malloc( rlen );
      end_malloc("str_dup part 2");
      strcpy( str_new, str );
      sOverFlowString += rlen;
      nOverFlowString++;
    }
    return str_new;
}

/* If string is in shared space, decrement usage, if usage then is 0, *
 * free the chunk and attempt to merge with next node. Other          *
 * strings are freed with standard free.                              *
 * Never call free/delete externally on a shared string.              */
void free_string( char *str )
{
    BufEntry *ptr;
    if( !str || str == &str_empty[0] )
        return;

    /* check if string is in the string heap */
   if( str > string_space && str < top_string )
    {
        ptr = (BufEntry *)(str - HEADER_SIZE);
        if( --ptr->usage > 0 )
            return;
        else if( ptr->usage < 0 )
        {
            bug( "SSM:free_string: multiple free or invalid string.", 0 );
            bug( (char*)&ptr->buf[0], 0 );
            return;
        }
	/* 0 count, we free, and check for merge */
        sAllocString -= (ptr->size + HEADER_SIZE);
	nAllocString--;
	/* try to merge the string if possible */
        if( ptr->next && ptr->next->usage <= 0 )
        {
	  ptr->size += ptr->next->size + HEADER_SIZE;
	  ptr->next = ptr->next->next;
        }
	/* lone free string, we increase the count */
	else
	  numFree++;
        if( ssm_buf_free > ptr )
	  ssm_buf_free = ptr;
	/* check for fragmentation */
	if (numFree >= MAX_FREE){
	  log_string( "[SSM] Attempting to optimize shared string heap.");
	  defrag_heap();
	  numFree = 0;
	}
	return;
    }
    sOverFlowString -= strlen( str ) + 1;
    nOverFlowString--;
    init_malloc("free free_string");
    free( str );
    end_malloc("free free_string");
}

/* Read and allocate space for a string from a file.          *
 * This replaces db.c fread_string                            */
char *fread_string( FILE *fp )
{
    char buf[MSL*4];
    char *ptr = buf;
    int c;
    do
    {
        c = getc( fp );
        *ptr = c;
    }
    while( isspace( c ) );
        if( ( *ptr++ = c ) == '~' )
            return &str_empty[0];
    for ( ;; )
        switch ( *ptr = getc( fp ) )
        {
        default:   ptr++; break;
        case EOF:  bug( "Fread_string: EOF", 0 ); exit( 1 ); break;
        case '\n': ptr++; *ptr++ = '\r'; break;
        case '\r': break;
        case '~':  *ptr = '\0';
	  if( fBootDb )
	    {
	      int len = ptr-buf;
	      ptr = temp_hash_find( buf, len );
	      if( ptr )
		return str_dup (ptr);
	      ptr = str_dup( buf );
	      temp_hash_add( ptr, len );
	      return ptr;
	    }
	  return str_dup( buf );
        }
}

/* This is a modified version of fread_string:                          *
 * It reads till a '\n' or a '\r' instead of a '~' (like fread_string). *
 * ROM uses this function to read in the socials.                       */
char *fread_string_eol( FILE *fp )
{
    char buf[MSL*4];
    char *ptr = buf;
    int c;
    do
    {
        c = getc( fp );
        *ptr = c;
    }
    while ( isspace( c ) );
        if ( ( *ptr++ = c ) == '\n' )
            return &str_empty[0];
    for ( ;; )
        switch ( *ptr = getc( fp ) )
        {
        default:   ptr++; break;
        case EOF:  bug( "Fread_string: EOF", 0 ); exit( 1 ); break;
        case '\n':
        case '\r': *ptr = '\0';
                   if( fBootDb )
                   {
		       int len = ptr-buf;
		       ptr = temp_hash_find( buf, len );
		       if( ptr )
		           return str_dup (ptr);
		       ptr = str_dup( buf );
	               temp_hash_add( ptr, len );
		       return ptr;
                   }
                   return str_dup( buf );
        }
}

/* Read string into user supplied buffer. *
 * Modified version of fread_string       */
void temp_fread_string( FILE *fp, char *buf )
{
    char *ptr = buf;
    int c;
    do
    {
        c = getc( fp );
        *ptr = c;
    }
    while ( isspace( c ) );
        if ( ( *ptr++ = c ) == '~' )
	{
            *buf = '\0';
            return;
	}
    for ( ;; )
        switch ( *ptr = getc( fp ) )
        {
        default:   ptr++; break;
        case EOF:  bug( "Fread_string: EOF", 0 ); exit( 1 ); break;
        case '\n': ptr++; *ptr++ = '\r'; break;
        case '\r': break;
        case '~':  *ptr = '\0'; return;
        }
}

/* Lookup the string in the boot-time hash table. */
char *temp_hash_find( const char *str, int len )
{
    TempHash *ptr;
    int ihash;
    if( !fBootDb || !*str )
        return 0;
    ihash = get_string_hash(str,len);
    for( ptr = temp_string_hash[ ihash ]; ptr; ptr = ptr->next )
    {
        if( *ptr->str != *str )
            continue;
        else if( strcmp( ptr->str, str ) )
            continue;
        else return ptr->str;
    }
    return 0;
}


/* Add a reference in the temporary hash table.          *
 * String is still in the linked list structure but      *
 * reference is kept here for quick lookup at boot time; */
void temp_hash_add( char *str, int len )
{
    int ihash;
    TempHash *add;
    if( !fBootDb || !*str || ( str <= string_space && str >= top_string ))
        return;
    ihash = get_string_hash(str,len);
    init_malloc("temp_hash_add");
    add = (TempHash *)malloc( sizeof( TempHash ) );
    end_malloc("temp_hash_add");
    add->next = temp_string_hash[ ihash ];
    temp_string_hash[ ihash ] = add;
    add->str = str;
}

/* Free the temp boot string hash table */
void boot_done( void )
{
    TempHash *ptr, *next;
    int ihash;
    for( ihash = 0; ihash < MAX_KEY_HASH; ihash++ )
        for( ptr = temp_string_hash[ ihash ]; ptr; ptr = next )
        {
            next = ptr->next;
	    init_malloc("free boot_done part 1");
            free( ptr );
	    end_malloc("free boot_done part 1");
        }
    init_malloc("free boot_done part 2");
    free( temp_string_hash );
    end_malloc("free boot_done part 2");
    temp_string_hash = 0;
    numFree = 0;
}

extern FILE *                  fpArea;
extern char                    strArea[MIL];

void load_thread(char *name, NOTE_DATA **list, int type, time_t free_time);
void parse_note(CHAR_DATA *ch, char *argument, int type);
bool hide_note(CHAR_DATA *ch, NOTE_DATA *pnote);

NOTE_DATA *note_list;
NOTE_DATA *idea_list;
NOTE_DATA *application_list;
NOTE_DATA *penalty_list;
NOTE_DATA *news_list;
NOTE_DATA *changes_list;

int count_spool(CHAR_DATA *ch, NOTE_DATA *spool)
{
    int count = 0;
    NOTE_DATA *pnote;
    for (pnote = spool; pnote != NULL; pnote = pnote->next)
	if (!hide_note(ch,pnote))
	    count++;
    return count;
}

void do_unread(CHAR_DATA *ch)
{
    int count;
    bool found = FALSE;
    if (IS_NPC(ch))
	return;
    if ((count = count_spool(ch,news_list)) > 0)
    {
	found = TRUE;
        sendf(ch,"There %s %d new news article%s waiting.\n\r", count > 1 ? "are" : "is",count, count > 1 ? "s" : "");
    }
    if ((count = count_spool(ch,changes_list)) > 0)
    {
	found = TRUE;
        sendf(ch,"There %s %d change%s waiting to be read.\n\r", count > 1 ? "are" : "is", count, count > 1 ? "s" : "");
    }
    if ((count = count_spool(ch,note_list)) > 0)
    {
	found = TRUE;
        sendf(ch,"You have %d new note%s waiting.\n\r", count, count > 1 ? "s" : "");
    }
    if ((count = count_spool(ch,application_list)) > 0)
    {
	found = TRUE;
        sendf(ch,"You have %d new application%s waiting.\n\r", count, count > 1 ? "s" : "");
    }
    if (IS_TRUSTED(ch,ANGEL) && (count = count_spool(ch,idea_list)) > 0)
    {
	found = TRUE;
        sendf(ch,"You have %d unread idea%s to peruse.\n\r", count, count > 1 ? "s" : "");
    }
    if (IS_TRUSTED(ch,ANGEL) && (count = count_spool(ch,penalty_list)) > 0)
    {
	found = TRUE;
        sendf(ch,"%d %s been added.\n\r", count, count > 1 ? "penalties have" : "penalty has");
    }
    if (!found)
	send_to_char("You have no unread notes.\n\r",ch);
}
void do_note(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_NOTE);
}

void do_idea(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_IDEA);
}

void do_application(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_APPLICATION);
}

void do_penalty(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_PENALTY);
}

void do_news(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_NEWS);
}

void do_changes(CHAR_DATA *ch,char *argument)
{
    parse_note(ch,argument,NOTE_CHANGES);
}

void save_notes(int type)
{
    FILE *fp;
    char *name;
    NOTE_DATA *pnote;
    switch (type)
    {
    default:           return;
    case NOTE_NOTE:    name = NOTE_FILE; pnote = note_list; break;
    case NOTE_IDEA:    name = IDEA_FILE; pnote = idea_list; break;
    case NOTE_APPLICATION:  name = APPLICATION_FILE;
      pnote = application_list; break;
    case NOTE_PENALTY: name = PENALTY_FILE; pnote = penalty_list; break;
    case NOTE_NEWS:    name = NEWS_FILE; pnote = news_list; break;
    case NOTE_CHANGES: name = CHANGES_FILE; pnote = changes_list; break;
    }
    fclose( fpReserve );
    if ( ( fp = fopen( name, "w" ) ) == NULL )
    {
	fp = fopen( NULL_FILE, "r" );
	fclose (fp);
	perror( name );
    }
    else
    {
	for ( ; pnote != NULL; pnote = pnote->next )
	{
	  fprintf( fp, "Sender  %s~\n", pnote->sender);
	  fprintf( fp, "Prefix  %d\n",  pnote->prefix);
	  fprintf( fp, "Date    %s~\n", pnote->date);
	  fprintf( fp, "Stamp   %ld\n", pnote->date_stamp);
	  fprintf( fp, "To      %s~\n", pnote->to_list);
	  fprintf( fp, "Subject %s~\n", pnote->subject);
	  fprintf( fp, "Text\n%s~\n",   pnote->text);
	}
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
    }
}

void load_notes(void)
{
    load_thread(NOTE_FILE,&note_list, NOTE_NOTE, 14 * VOTE_DAY );
    load_thread(IDEA_FILE,&idea_list, NOTE_IDEA, 28 * VOTE_DAY);
    load_thread(APPLICATION_FILE,&application_list, NOTE_APPLICATION, 14 * VOTE_DAY);
    load_thread(PENALTY_FILE,&penalty_list, NOTE_PENALTY, 14 * VOTE_DAY);
    load_thread(NEWS_FILE,&news_list, NOTE_NEWS, 14 * VOTE_DAY);
    load_thread(CHANGES_FILE,&changes_list,NOTE_CHANGES, 28 * VOTE_DAY);
}

void load_thread(char *name, NOTE_DATA **list, int type, time_t free_time)
{
    FILE *fp;
    NOTE_DATA *pnotelast;
    if ( ( fp = fopen( name, "r" ) ) == NULL )
    {
	fp = fopen( NULL_FILE, "r" );
	fclose (fp);
	return;
    }
    pnotelast = NULL;
    for ( ; ; )
    {
	NOTE_DATA *pnote;
	char letter;
	bool skip = FALSE;
	do
	{
	    letter = getc( fp );
            if ( feof(fp) )
            {
                fclose( fp );
                return;
            }
        }
        while ( isspace(letter) );
        ungetc( letter, fp );
        pnote           = alloc_perm( sizeof(*pnote) );
        if ( str_cmp( fread_word( fp ), "sender" ) )
            break;
        pnote->sender   = fread_string( fp );
	skip = FALSE;
        if ( str_cmp( fread_word( fp ), "prefix" ) )
	{
	    skip = TRUE;
            pnote->date     = fread_string( fp );
	}
	else
            pnote->prefix   = fread_number( fp );
	if (!skip)
	{
            if ( str_cmp( fread_word( fp ), "date" ) )
                break;
            pnote->date     = fread_string( fp );
	}
        if ( str_cmp( fread_word( fp ), "stamp" ) )
            break;
        pnote->date_stamp = fread_number(fp);
        if ( str_cmp( fread_word( fp ), "to" ) )
            break;
        pnote->to_list  = fread_string( fp );
        if ( str_cmp( fread_word( fp ), "subject" ) )
            break;
        pnote->subject  = fread_string( fp );
        if ( str_cmp( fread_word( fp ), "text" ) )
            break;
        pnote->text     = fread_string( fp );
        if (free_time && pnote->date_stamp < mud_data.current_time - free_time)
        {
	    free_note(pnote);
            continue;
        }
	pnote->type = type;
        if (*list == NULL)
            *list = pnote;
        else
            pnotelast->next = pnote;
        pnotelast = pnote;
    }
    strcpy( strArea, NOTE_FILE );
    fpArea = fp;
    bug( "Load_notes: bad key word.", 0 );
    exit( 1 );
}

void append_note(NOTE_DATA *pnote)
{
//    FILE *fp;
    char *name = NULL;
    NOTE_DATA **list;
    NOTE_DATA *last;

    /* Useless conditional */
    if( name != NULL )
        name = NULL;

    switch(pnote->type)
    {
    default:           return;
    case NOTE_NOTE:    name = NOTE_FILE; list = &note_list; break;
    case NOTE_IDEA:    name = IDEA_FILE; list = &idea_list; break;
    case NOTE_APPLICATION:  name = APPLICATION_FILE;
      list = &application_list; break;
    case NOTE_PENALTY: name = PENALTY_FILE; list = &penalty_list; break;
    case NOTE_NEWS:    name = NEWS_FILE; list = &news_list; break;
    case NOTE_CHANGES: name = CHANGES_FILE; list = &changes_list; break;
    }
    if (*list == NULL)
	*list = pnote;
    else
    {
      /* we get the very last note */
	for ( last = *list; last->next != NULL; last = last->next);
      /* we make sure that the date_stamp is at least 1 greater then last */
	pnote->date_stamp = UMAX(pnote->date_stamp, last->date_stamp + 1 );
	last->next = pnote;
    }
    save_notes( pnote->type );

/* Unused, use save_notes instead to prevent accumulation of old notes
    fclose(fpReserve);
    if ( ( fp = fopen(name, "a" ) ) == NULL )
    {
	fp = fopen( NULL_FILE, "r" );
	fclose (fp);
        perror(name);
    }
    else
    {
        fprintf( fp, "Sender  %s~\n", pnote->sender);
        fprintf( fp, "Prefix  %d\n",  pnote->prefix);
        fprintf( fp, "Date    %s~\n", pnote->date);
        fprintf( fp, "Stamp   %ld\n", pnote->date_stamp);
        fprintf( fp, "To      %s~\n", pnote->to_list);
        fprintf( fp, "Subject %s~\n", pnote->subject);
        fprintf( fp, "Text\n%s~\n", pnote->text);
        fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
*/
}

bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
  if (pnote->type == NOTE_APPLICATION && !IS_IMMORTAL(ch))
    return FALSE;
  if (pnote->type == NOTE_APPLICATION && get_trust(ch) == IMPLEMENTOR)
    return TRUE;
  if ( is_name( "all", pnote->to_list ) )
    return TRUE;
  if ( !str_cmp( ch->name, pnote->sender ) )
    return TRUE;
  if ( IS_IMMORTAL(ch) && (is_name( "immortal", pnote->to_list )
			   || is_name( "immortals", pnote->to_list )
			   || is_name( "imms", pnote->to_list ) ))
    return TRUE;
  if (get_trust(ch) >= 60 && (is_name("implementor", pnote->to_list)
			      || is_name("implementors", pnote->to_list)
			      || is_name("imp", pnote->to_list)
			      || is_name("imps", pnote->to_list)) )
    return TRUE;
  if (is_name(deity_table[ch->pcdata->way].way, pnote->to_list))
    return TRUE;
  if (is_name(path_table[deity_table[ch->pcdata->way].path].name, pnote->to_list))
    return TRUE;
  if ( ch->pCabal ){
    CABAL_DATA* pPar = get_parent( ch->pCabal );
    if (is_name(ch->pCabal->name, pnote->to_list)
	|| is_name(pPar->name, pnote->to_list)
	|| is_name("cabals", pnote->to_list))
      return TRUE;
  }
  if (is_name(hometown_table[ch->hometown].name, pnote->to_list)
      && is_name("city", pnote->to_list))
    return TRUE;
  if (HAS_CLAN(ch) && is_name(GetClanName(GET_CLAN(ch)), pnote->to_list))
    return TRUE;
  if ( is_name(class_table[ch->class].name,pnote->to_list))
    return TRUE;
  if ( is_name(race_table[ch->race].name,pnote->to_list))
    return TRUE;
  if ( is_name( ch->name, pnote->to_list ) )
    return TRUE;
/*
  if ( get_trust(ch) > 59 )
    return TRUE;
*/
  return FALSE;
}

void note_attach( CHAR_DATA *ch, int type )
{
    NOTE_DATA *pnote;
    int result = 0;
    if ( ch->pcdata->pnote != NULL )
	return;
    if (IS_IMMORTAL(ch))
	result = ch->level * 100;
    if (ch->pCabal && IS_GAME(ch, GAME_SHOW_CABAL))
      result += ch->pCabal->vnum;
    pnote = new_note();
    pnote->next		= NULL;
    pnote->sender	= str_dup( ch->name );
    pnote->prefix	= result;
    pnote->date		= str_dup( "" );
    pnote->to_list	= str_dup( "" );
    pnote->subject	= str_dup( "" );
    pnote->text		= str_dup( "" );
    pnote->type		= type;
    ch->pcdata->pnote		= pnote;
    return;
}

void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote, bool delete)
{
    char to_new[MIL], to_one[MIL];
    NOTE_DATA *prev;
    NOTE_DATA **list;
    char *to_list;
    if (!delete)
    {
        to_new[0]	= '\0';
        to_list	= pnote->to_list;
        while ( *to_list != '\0' )
        {
    	    to_list	= one_argument( to_list, to_one );
    	    if ( to_one[0] != '\0' && str_cmp( ch->name, to_one ) )
	    {
	        strcat( to_new, " " );
	        strcat( to_new, to_one );
	    }
        }
        if ( str_cmp( ch->name, pnote->sender ) && to_new[0] != '\0' )
        {
            free_string( pnote->to_list );
            pnote->to_list = str_dup( to_new + 1 );
            return;
        }
    }
    switch(pnote->type)
    {
    default:           return;
    case NOTE_NOTE:    list = &note_list; break;
    case NOTE_IDEA:    list = &idea_list; break;
    case NOTE_APPLICATION:    list = &application_list; break;
    case NOTE_PENALTY: list = &penalty_list; break;
    case NOTE_NEWS:    list = &news_list; break;
    case NOTE_CHANGES: list = &changes_list; break;
    }
    if ( pnote == *list )
	*list = pnote->next;
    else
    {
	for ( prev = *list; prev != NULL; prev = prev->next )
	    if ( prev->next == pnote )
		break;
	if ( prev == NULL )
	{
	    bug( "Note_remove: pnote not found.", 0 );
	    return;
	}
	prev->next = pnote->next;
    }
    save_notes(pnote->type);
    free_note(pnote);
}

bool hide_note (CHAR_DATA *ch, NOTE_DATA *pnote)
{
    time_t last_read;
    if (IS_NPC(ch))
	return TRUE;
    switch (pnote->type)
    {
    default:           return TRUE;
    case NOTE_NOTE:    last_read = ch->pcdata->last_note; break;
    case NOTE_IDEA:    last_read = ch->pcdata->last_idea; break;
    case NOTE_APPLICATION:  last_read = ch->pcdata->last_application; break;
    case NOTE_PENALTY: last_read = ch->pcdata->last_penalty; break;
    case NOTE_NEWS:    last_read = ch->pcdata->last_news; break;
    case NOTE_CHANGES: last_read = ch->pcdata->last_changes; break;
    }
    if (pnote->date_stamp <= last_read)
	return TRUE;
    if (!str_cmp(ch->name,pnote->sender))
	return TRUE;
    if (!is_note_to(ch,pnote))
	return TRUE;
    return FALSE;
}

void update_read(CHAR_DATA *ch, NOTE_DATA *pnote)
{
    time_t stamp = pnote->date_stamp;
    if (IS_NPC(ch))
	return;
    switch (pnote->type)
    {
    default:           return;
    case NOTE_NOTE:    ch->pcdata->last_note = UMAX(ch->pcdata->last_note,stamp); break;
    case NOTE_IDEA:    ch->pcdata->last_idea = UMAX(ch->pcdata->last_idea,stamp); break;
    case NOTE_APPLICATION:   ch->pcdata->last_application = UMAX(ch->pcdata->last_application,stamp); break;
    case NOTE_PENALTY: ch->pcdata->last_penalty = UMAX(ch->pcdata->last_penalty,stamp); break;
    case NOTE_NEWS:    ch->pcdata->last_news = UMAX(ch->pcdata->last_news,stamp); break;
    case NOTE_CHANGES: ch->pcdata->last_changes = UMAX(ch->pcdata->last_changes,stamp); break;
    }
}

/* checks if an app meets crusader requirements */
bool create_crus_app( CHAR_DATA* ch, char* to, char* subject, char* text ){
  const int minl = 50;
  const int maxl = 50;
  const int age = 144000;	//40 h

  /* check if this is a crus app */
  if (!is_auto_name("crusader", subject) || !is_auto_name("app", subject)){
    return VOTE_NONE;
  }
/* check if we already crusader */
  else if (ch->class == class_lookup("crusader")){
    send_to_char("You've already been inducted into the Order of Crusades!\n\r", ch );
    return VOTE_ERROR;
  }
/* check faith */
  else if (str_cmp(deity_table[ch->pcdata->way].way, "Church")){
    send_to_char("You do not belive in the One God.\n\r", ch);
    return VOTE_ERROR;
  }
/* check cabal */
  if (ch->pCabal && ch->pCabal->class[0] >= 0
      && ch->pCabal->class[class_lookup("crusader")] == 0){
    send_to_char("Your cabal does not allow Crusaders!\n\r",ch );
    return VOTE_ERROR;
  }
/*  check class */
  else if (ch->class != class_lookup("warrior")
      && ch->class != class_lookup("berserk")
      && ch->class != class_lookup("paladin")){
    send_to_char("Only warriors, berserkers, and paladins may apply.\n\r", ch);
    return VOTE_ERROR;
  }
  else if (ch->race != race_lookup("human")
	   && ch->race != race_lookup("elf")
	   && ch->race != race_lookup("half-elf")
	   && ch->race != race_lookup("storm")){
    send_to_char("Your race prevents your application from being accepted.\n\r", ch );
    return VOTE_ERROR;
  }
  else if (IS_AVATAR(ch)){
    send_to_char("Avatars may not be accepted.\n\r", ch);
    return VOTE_ERROR;
  }
/* check align */
  else if (!IS_GOOD(ch)){
    send_to_char("You must be of good align.\n\r", ch);
    return VOTE_ERROR;
  }
/* check levels */
  else if (ch->level < minl || ch->level > maxl){
    sendf(ch, "You may only make this application between ranks of %d and %d.\n\r", minl, maxl );
    return VOTE_ERROR;
  }
/* check age */
  else if ((ch->played + (int) (mud_data.current_time - ch->logon)) < age){
    sendf( ch, "You have not been present in the lands enough to be considered.\n\r");
    return VOTE_ERROR;
  }
  return VOTE_ACCEPTED;
}

/* checks if an app meets psi requirements */
bool create_psi_app( CHAR_DATA* ch, char* to, char* subject, char* text ){
  const int minl = 50;
  const int maxl = 50;
  const int age = 144000;	//40 h

  /* check if this is a crus app */
  if (!is_auto_name("psi", subject) || !is_auto_name("app", subject)){
    return VOTE_NONE;
  }
/* check if we already crusader */
  else if (ch->class == gcn_psi){
    send_to_char("You've already been inducted into the Tower of the Mind!\n\r", ch );
    return VOTE_ERROR;
  }
/* check cabal */
  if (ch->pCabal && ch->pCabal->class[0] >= 0
      && ch->pCabal->class[gcn_psi] == 0){
    send_to_char("Your cabal does not allow Psionicists!\n\r",ch );
    return VOTE_ERROR;
  }
/*  check class */
  else if (ch->class != class_lookup("invoker")
      && ch->class != class_lookup("necromancer")
      && ch->class != class_lookup("monk")
      && ch->class != class_lookup("battlemage")){
    send_to_char("Only warriors, berserkers, and paladins may apply.\n\r", ch);
    return VOTE_ERROR;
  }
  else if (ch->race != race_lookup("human")
	   && ch->race != race_lookup("elf")
	   && ch->race != race_lookup("half-elf")
	   && ch->race != race_lookup("gnome")
	   && ch->race != race_lookup("avian")
	   && ch->race != race_lookup("faerie")
	   && ch->race != race_lookup("illithid")){
    send_to_char("Your race prevents your application from being accepted.\n\r", ch );
    return VOTE_ERROR;
  }
/* check align */
  else if (IS_EVIL(ch) && ch->race != grn_illithid){
    send_to_char("Only Illithids may be evil Psionicists.\n\r", ch);
    return VOTE_ERROR;
  }
/* check levels */
  else if (ch->level < minl || ch->level > maxl){
    sendf(ch, "You may only make this application between ranks of %d and %d.\n\r", minl, maxl );
    return VOTE_ERROR;
  }
/* check age */
  else if ((ch->played + (int) (mud_data.current_time - ch->logon)) < age){
    sendf( ch, "You have not been present in the lands enough to be considered.\n\r");
    return VOTE_ERROR;
  }
  return VOTE_ACCEPTED;
}

/* checks if an app meets avatar requirements */
bool create_avatar_app( CHAR_DATA* ch, char* to, char* subject, char* text ){
  const int minl = 15;
  const int maxl = 50;
  const int age = 90000;	//25 h

/* check if this is a crus app */
  if (!is_auto_name("avatar", subject) || !is_auto_name("app", subject)){
    return VOTE_NONE;
  }
/* check if we already avatar */
  else if (IS_AVATAR(ch)){
    send_to_char("You've already been granted your powers!\n\r", ch );
    return VOTE_ERROR;
  }
/* check faith */
  else if (str_cmp(deity_table[ch->pcdata->way].way, "Purity")){
    send_to_char("You do not belive in Purity.\n\r", ch);
    return VOTE_ERROR;
  }
/* check cabal */
  if (ch->pCabal && !ch->pCabal->fAvatar){
    send_to_char("Your cabal does not allow Avatars!\n\r",ch );
    return VOTE_ERROR;
  }
/* check align */
  else if (!IS_GOOD(ch)){
    send_to_char("You must be of good align.\n\r", ch);
    return VOTE_ERROR;
  }
/* check levels */
  else if (ch->level < minl || ch->level > maxl){
    sendf(ch, "You may only make this application between ranks of %d and %d.\n\r", minl, maxl );
    return VOTE_ERROR;
  }
/* check age */
  else if ((ch->played + (int) (mud_data.current_time - ch->logon)) < age){
    sendf( ch, "You have not been present in the lands enough to be considered.\n\r");
    return VOTE_ERROR;
  }
  return VOTE_ACCEPTED;
}


/* checks if an app meets demon requirements */
bool create_demon_app( CHAR_DATA* ch, char* to, char* subject, char* text ){
  const int minl = 15;
  const int maxl = 30;
  const int age = 72000;	//20 h

  /* check if this is a crus app */
  if (!is_auto_name("demon", subject) || !is_auto_name("app", subject)){
    return VOTE_NONE;
  }
/* check if we already demon */
  else if (ch->race == race_lookup("demon")){
    send_to_char("You've already been turned into a demon!\n\r", ch );
    return VOTE_ERROR;
  }
/*  check class */
  else if (ch->class != class_lookup("warrior")
      && ch->class != class_lookup("berserk")
      && ch->class != class_lookup("cleric")
      && ch->class != class_lookup("shaman")
      && ch->class != class_lookup("necromancer")
      && ch->class != class_lookup("battlemage")
      && ch->class != class_lookup("dark-knight")){
    send_to_char("Your profession prevents your application from being accepted.\n\r", ch);
    return VOTE_ERROR;
  }
/* check faith */
  else if (str_cmp(deity_table[ch->pcdata->way].way, "Discord")){
    send_to_char("You do not belive in Discord.\n\r", ch);
    return VOTE_ERROR;
  }
/* check cabal */
  if (ch->pCabal && ch->pCabal->race[0] >= 0
      && ch->pCabal->race[race_lookup("demon")] == 0){
    send_to_char("Your cabal does not allow Demons!\n\r",ch );
    return VOTE_ERROR;
  }
/* check align */
  else if (!IS_EVIL(ch)){
    send_to_char("You must be of evil align.\n\r", ch);
    return VOTE_ERROR;
  }
  else if (ch->race != race_lookup("human")){
    send_to_char("Your race prevents your application from being accepted.\n\r", ch );
    return VOTE_ERROR;
  }
/* check levels */
  else if (ch->level < minl || ch->level > maxl){
    sendf(ch, "You may only make this application between ranks of %d and %d.\n\r", minl, maxl );
    return VOTE_ERROR;
  }
/* check age */
  else if ((ch->played + (int) (mud_data.current_time - ch->logon)) < age){
    sendf( ch, "You have not been present in the lands enough to be considered.\n\r");
    return VOTE_ERROR;
  }
  return VOTE_ACCEPTED;
}


/* checks if an app meets demon requirements */
bool create_vamp_app( CHAR_DATA* ch, char* to, char* subject, char* text ){
  const int minl = 15;
  const int maxl = 30;
  const int age = 72000;	//20 h

  /* check if this is a crus app */
  if (!is_auto_name("vampire", subject) || !is_auto_name("app", subject)){
    return VOTE_NONE;
  }
/* check if we already crusader */
  else if (ch->race == race_lookup("vampire")){
    send_to_char("You've already joined the kindred!\n\r", ch );
    return VOTE_ERROR;
  }
/*  check class */
  else if (ch->class != class_lookup("dark-knight")){
    send_to_char("Your profession prevents your application from being accepted.\n\r", ch);
    return VOTE_ERROR;
  }
/* check align */
  else if (!IS_EVIL(ch)){
    send_to_char("You must be of evil align.\n\r", ch);
    return VOTE_ERROR;
  }
  else if (ch->race != race_lookup("human")){
    send_to_char("Your race prevents your application from being accepted.\n\r", ch );
    return VOTE_ERROR;
  }
/* check cabal */
  if (ch->pCabal && ch->pCabal->race[0] >= 0
      && ch->pCabal->race[race_lookup("vampire")] == 0){
    send_to_char("Your cabal does not allow Vampires!\n\r",ch );
    return VOTE_ERROR;
  }
/* check faith */
  else if (deity_table[ch->pcdata->way].path != PATH_DEATH){
    send_to_char("You do not belive in powers of Death.\n\r", ch);
    return VOTE_ERROR;
  }
/* check levels */
  else if (ch->level < minl || ch->level > maxl){
    sendf(ch, "You may only make this application between ranks of %d and %d.\n\r", minl, maxl );
    return VOTE_ERROR;
  }
/* check age */
  else if ((ch->played + (int) (mud_data.current_time - ch->logon)) < age){
    sendf( ch, "You have not been present in the lands enough to be considered.\n\r");
    return VOTE_ERROR;
  }
  return VOTE_ACCEPTED;
}


/* checks if an app meets demon requirements */
bool create_undead_app( CHAR_DATA* ch, char* to, char* subject, char* text ){
  const int minl = 15;
  const int maxl = 30;
  const int age = 72000;	//20 h

  /* check if this is a crus app */
  if (!is_auto_name("undead", subject) || !is_auto_name("app", subject)){
    return VOTE_NONE;
  }
/* check if we already crusader */
  if (ch->race == race_lookup("undead")){
    send_to_char("You've already joined the undead!\n\r", ch );
    return VOTE_ERROR;
  }
/* check align */
  if (!IS_EVIL(ch)){
    send_to_char("You must be of evil align.\n\r", ch);
    return VOTE_ERROR;
  }
/* check levels */
  if (ch->level < minl || ch->level > maxl){
    sendf(ch, "You may only make this application between ranks of %d and %d.\n\r", minl, maxl );
    return VOTE_ERROR;
  }
  if (ch->pCabal && ch->pCabal->race[0] >= 0
      && ch->pCabal->race[race_lookup("undead")] == 0){
    send_to_char("Your cabal does not allow Undead!\n\r",ch );
    return VOTE_ERROR;
  }
/* check faith */
  else if (deity_table[ch->pcdata->way].path != PATH_DEATH){
    send_to_char("You do not belive in powers of Death.\n\r", ch);
    return VOTE_ERROR;
  }
/* check age */
  if ((ch->played + (int) (mud_data.current_time - ch->logon)) < age){
    sendf( ch, "You have not been present in the lands enough to be considered.\n\r");
    return VOTE_ERROR;
  }
  return VOTE_ACCEPTED;
}

void parse_note( CHAR_DATA *ch, char *argument, int type )
{
    BUFFER *buffer;
    char buf[MSL], arg[MIL], arg2[MIL], arg3[MIL];
    NOTE_DATA *pnote;
    NOTE_DATA **list;
    char *list_name;
    int vnum, anum;
    char prefix[MIL];
    if ( IS_NPC(ch) )
	return;
    switch(type)
    {
    default:           return;
    case NOTE_NOTE:    list = &note_list; list_name = "notes"; break;
    case NOTE_IDEA:    list = &idea_list; list_name = "ideas"; break;
    case NOTE_APPLICATION:    list = &application_list; list_name = "applications"; break;
    case NOTE_PENALTY: list = &penalty_list; list_name = "penalties"; break;
    case NOTE_NEWS:    list = &news_list; list_name = "news"; break;
    case NOTE_CHANGES: list = &changes_list; list_name = "changes"; break;
    }
    argument = one_argument( argument, arg );
    smash_tilde( argument );
    if ( arg[0] == '\0' || !str_prefix( arg, "read" ) )
    {
        bool fAll;
        if ( (strcmp(list_name, "ideas") == 0) && (!IS_TRUSTED(ch, AVATAR)) )
        {
            send_to_char("Only immortals may read ideas.\n\r",ch);
            return;
        }
        if ( !str_cmp( argument, "all" ) )
        {
            fAll = TRUE;
            anum = 0;
        }
        else if ( argument[0] == '\0' || !str_prefix(argument, "next"))
        {
            vnum = 0;
            for ( pnote = *list; pnote != NULL; pnote = pnote->next)
            {
                if (!hide_note(ch,pnote))
                {
		    prefix[0] = '\0';
		    if (pnote->prefix > 100)
		    {
	                switch(pnote->prefix / 100)
        	        {
                	    case MAX_LEVEL - 0 : sprintf(prefix, "IMP ");     break;
	                    case MAX_LEVEL - 1 : sprintf(prefix, "CRE ");     break;
        	            case MAX_LEVEL - 2 : sprintf(prefix, "SUP ");     break;
                	    case MAX_LEVEL - 3 : sprintf(prefix, "DEI ");     break;
	                    case MAX_LEVEL - 4 : sprintf(prefix, "GOD ");     break;
        	            case MAX_LEVEL - 5 : sprintf(prefix, "IMM ");     break;
                	    case MAX_LEVEL - 6 : sprintf(prefix, "DEM ");     break;
	                    case MAX_LEVEL - 7 : sprintf(prefix, "ANG ");     break;
        	            case MAX_LEVEL - 8 : sprintf(prefix, "AVA ");     break;
                	    case MAX_LEVEL - 9 : sprintf(prefix, "HER ");     break;
	                }
		    }
		    if (pnote->prefix % 100){
		      CABAL_DATA* pCab = get_cabal_vnum( pnote->prefix % 100);
		      if (pCab){
			strcat(prefix, pCab->who_name);
			strcat(prefix, " ");
		      }
		    }
		    sendf( ch, "[%3d] %s%s: %s\n\r%s\n\rTo: %s\n\r",
			   vnum, prefix, pnote->sender, pnote->subject, pnote->date, pnote->to_list);
		    page_to_char( pnote->text, ch );
		    update_read(ch,pnote);
		    return;
		}
                else if (is_note_to(ch,pnote))
		  vnum++;
            }
	    sendf(ch,"You have no unread %s.\n\r",list_name);
            return;
        }
        else if ( is_number( argument ) )
        {
            fAll = FALSE;
            anum = atoi( argument );
        }
        else
        {
            send_to_char( "Read which number?\n\r", ch );
            return;
        }
        vnum = 0;
        for ( pnote = *list; pnote != NULL; pnote = pnote->next )
        {
            if ( is_note_to( ch, pnote ) && ( vnum++ == anum || fAll ) )
            {
		prefix[0] = '\0';
		if (pnote->prefix > 100)
		{
	            switch(pnote->prefix / 100)
        	    {
                	case MAX_LEVEL - 0 : sprintf(prefix, "IMP ");     break;
	                case MAX_LEVEL - 1 : sprintf(prefix, "CRE ");     break;
        	        case MAX_LEVEL - 2 : sprintf(prefix, "SUP ");     break;
                	case MAX_LEVEL - 3 : sprintf(prefix, "DEI ");     break;
	                case MAX_LEVEL - 4 : sprintf(prefix, "GOD ");     break;
        	        case MAX_LEVEL - 5 : sprintf(prefix, "IMM ");     break;
                	case MAX_LEVEL - 6 : sprintf(prefix, "DEM ");     break;
	                case MAX_LEVEL - 7 : sprintf(prefix, "ANG ");     break;
        	        case MAX_LEVEL - 8 : sprintf(prefix, "AVA ");     break;
                	case MAX_LEVEL - 9 : sprintf(prefix, "HER ");     break;
	            }
		}
		if (pnote->prefix % 100){
		  CABAL_DATA* pCab = get_cabal_vnum( pnote->prefix % 100);
		  if (pCab){
		    strcat(prefix, pCab->who_name);
		    strcat(prefix, " ");
		  }
		}
		sendf( ch, "[%3d] %s%s: %s\n\r%s\n\rTo: %s\n\r",
		       vnum - 1, prefix, pnote->sender, pnote->subject, pnote->date, pnote->to_list);
		page_to_char( pnote->text, ch );
		update_read(ch,pnote);
		return;
            }
        }
	sendf(ch,"There aren't that many %s.\n\r",list_name);
        return;
    }
    if ( !str_prefix( arg, "format" ) ){
      if ( ch->pcdata->pnote == NULL ){
	send_to_char( "You have no note in progress.\n\r", ch );
	return;
      }
      if (ch->pcdata->pnote->type != type){
	send_to_char("You aren't working on that kind of note.\n\r",ch);
	return;
      }
      ch->pcdata->pnote->text = format_string(ch->pcdata->pnote->text);
      send_to_char("Text formatted.\n\r", ch);
      return;
    }
    if ( !str_prefix( arg, "list" ) )
    {
	vnum = 0;
        if ( (strcmp(list_name, "ideas") == 0) && (!IS_TRUSTED(ch,ANGEL)) )
        {
            send_to_char("Only immortals may read ideas.\n\r",ch);
            return;
        }
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	for ( pnote = *list; pnote != NULL; pnote = pnote->next )
	    if ( is_note_to( ch, pnote ) )
	    {
	      if (!str_prefix (arg2, "to")) {
		if (!is_name (arg3, pnote->to_list)) {
		  vnum++;
		  continue;
		}
	      }
	      if (!str_prefix (arg2, "from")) {
		if (!is_name (arg3, pnote->sender)) {
		  vnum++;
		  continue;
		}
	      }
	      prefix[0] = '\0';
	      if (pnote->prefix > 100)
		{
	            switch(pnote->prefix / 100)
        	    {
                	case MAX_LEVEL - 0 : sprintf(prefix, "IMP ");     break;
	                case MAX_LEVEL - 1 : sprintf(prefix, "CRE ");     break;
        	        case MAX_LEVEL - 2 : sprintf(prefix, "SUP ");     break;
                	case MAX_LEVEL - 3 : sprintf(prefix, "DEI ");     break;
	                case MAX_LEVEL - 4 : sprintf(prefix, "GOD ");     break;
        	        case MAX_LEVEL - 5 : sprintf(prefix, "IMM ");     break;
                	case MAX_LEVEL - 6 : sprintf(prefix, "DEM ");     break;
	                case MAX_LEVEL - 7 : sprintf(prefix, "ANG ");     break;
        	        case MAX_LEVEL - 8 : sprintf(prefix, "AVA ");     break;
                	case MAX_LEVEL - 9 : sprintf(prefix, "HER ");     break;
	            }
		}
	      if (pnote->prefix % 100){
		CABAL_DATA* pCab = get_cabal_vnum( pnote->prefix % 100);
		if (pCab){
		  strcat(prefix, pCab->who_name);
		  strcat(prefix, " ");
		}
	      }
	      sendf( ch, "[%3d%s] %s%s: %s\n\r", vnum, hide_note(ch,pnote) ? " " : "N", prefix, pnote->sender, pnote->subject );
	      vnum++;
	    }
	if (!vnum)
	    switch(type)
	    {
            case NOTE_NOTE:    send_to_char("There are no notes for you.\n\r",ch); break;
            case NOTE_IDEA:    send_to_char("There are no ideas for you.\n\r",ch); break;
            case NOTE_APPLICATION:    send_to_char("There are no applications for you.\n\r",ch); break;
            case NOTE_PENALTY: send_to_char("There are no penalties for you.\n\r",ch); break;
            case NOTE_NEWS:    send_to_char("There is no news for you.\n\r",ch); break;
            case NOTE_CHANGES: send_to_char("There are no changes for you.\n\r",ch); break;
	    }
	return;
    }
    if ( !str_prefix( arg, "remove" ) )
    {
        if ( !is_number( argument ) )
        {
            send_to_char( "Note remove which number?\n\r", ch );
            return;
        }
        anum = atoi( argument );
        vnum = 0;
        for ( pnote = *list; pnote != NULL; pnote = pnote->next )
            if ( is_note_to( ch, pnote ) && vnum++ == anum )
            {
                note_remove( ch, pnote, FALSE );
                send_to_char( "Ok.\n\r", ch );
                return;
            }
	sendf(ch,"There aren't that many %s.",list_name);
        return;
    }
    if ( !str_prefix( arg, "delete" ) && get_trust(ch) >= MAX_LEVEL - 1)
    {
        if ( !is_number( argument ) )
        {
            send_to_char( "Note delete which number?\n\r", ch );
            return;
        }
        anum = atoi( argument );
        vnum = 0;
        for ( pnote = *list; pnote != NULL; pnote = pnote->next )
            if ( is_note_to( ch, pnote ) && vnum++ == anum )
            {
                note_remove( ch, pnote,TRUE );
                send_to_char( "Ok.\n\r", ch );
                return;
            }
 	sendf(ch,"There aren't that many %s.\n\r",list_name);
        return;
    }
    if (!str_prefix(arg,"catchup"))
    {
	switch(type)
	{
        case NOTE_NOTE:    ch->pcdata->last_note = mud_data.current_time; break;
        case NOTE_IDEA:    ch->pcdata->last_idea = mud_data.current_time; break;
        case NOTE_APPLICATION:   ch->pcdata->last_application = mud_data.current_time;
	  break;
        case NOTE_PENALTY: ch->pcdata->last_penalty = mud_data.current_time; break;
        case NOTE_NEWS:    ch->pcdata->last_news = mud_data.current_time; break;
        case NOTE_CHANGES: ch->pcdata->last_changes = mud_data.current_time; break;
        }
        send_to_char( "Ok.\n\r", ch );
	return;
    }
    if ((type == NOTE_NEWS && !IS_TRUSTED(ch,ANGEL)) || (type == NOTE_CHANGES && !IS_TRUSTED(ch,CREATOR)))
    {
	sendf(ch,"You aren't high enough level to write %s.",list_name);
	return;
    }
    if ( !str_cmp( arg, "+" ) )
    {
	note_attach( ch,type );
	if (ch->pcdata->pnote->type != type)
        {
            send_to_char( "You already have a different note in progress.\n\r",ch);
	    return;
        }
	if (strlen(ch->pcdata->pnote->text)+strlen(argument) >= 4096)
	{
	    send_to_char( "Note too long.\n\r", ch );
	    return;
	}
 	buffer = new_buf();
	add_buf(buffer,ch->pcdata->pnote->text);
	add_buf(buffer,argument);
	add_buf(buffer,"\n\r");
	free_string( ch->pcdata->pnote->text );
	ch->pcdata->pnote->text = str_dup( buf_string(buffer) );
	free_buf(buffer);
	send_to_char( "Ok.\n\r", ch );
	return;
    }
    if (!str_cmp(arg,"-"))
    {
 	int len;
	bool found = FALSE;
	note_attach(ch,type);
        if (ch->pcdata->pnote->type != type)
        {
            send_to_char( "You already have a different note in progress.\n\r",ch);
            return;
        }
	if (ch->pcdata->pnote->text == NULL || ch->pcdata->pnote->text[0] == '\0')
	{
	    send_to_char("No lines left to remove.\n\r",ch);
	    return;
	}
	strcpy(buf,ch->pcdata->pnote->text);
	for (len = strlen(buf); len > 0; len--)
	    if (buf[len] == '\r')
	    {
                if (!found)
		{
		    if (len > 0)
			len--;
		    found = TRUE;
		}
                else
		{
		    buf[len + 1] = '\0';
		    free_string(ch->pcdata->pnote->text);
		    ch->pcdata->pnote->text = str_dup(buf);
		    return;
		}
	    }
	buf[0] = '\0';
	free_string(ch->pcdata->pnote->text);
	ch->pcdata->pnote->text = str_dup(buf);
        send_to_char( "Ok.\n\r", ch );
	return;
    }
    if ( !str_prefix( arg, "subject" ) )
    {
	note_attach( ch,type );
        if (ch->pcdata->pnote->type != type)
        {
            send_to_char( "You already have a different note in progress.\n\r",ch);
            return;
        }
	free_string( ch->pcdata->pnote->subject );
	ch->pcdata->pnote->subject = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }
    if ( !str_prefix( arg, "to" ) )
    {
	int i;
        if ( (!str_infix("all ",argument) || !str_infix(" all",argument)
	      || !str_cmp("all",argument) || is_name( "city", argument))
	     && ch->level <= LEVEL_HERO
	     && !IS_ELDER(ch))

	  {
	    if (ch->class != class_lookup("bard")
		&& (ch->pCabal == NULL || !IS_CABAL2(ch->pCabal, CABAL2_NOTEALL)))
	      {
            	send_to_char("Notes to cities or all aren't allowed from mortals.\n\r",ch);
            	return;
	      }
	    else if (ch->level < 30)
	      {
	    	send_to_char("You are not ready to display your penmanship to the world yet.",ch);
		return;
	      }
	  }
	for (i = 0;class_table[i].name != NULL && i < MAX_CLASS;i++)
	{
            if ( !str_infix(class_table[i].name,argument) && ch->class != i && ch->level <= LEVEL_HERO)
	    {
		send_to_char("You can only post notes to your own class.\n\r",ch);
		return;
	    }
	}
	for (i = 1;pc_race_table[i].name != NULL && i < MAX_PC_RACE;i++)
	{
            if ( !str_infix(pc_race_table[i].name,argument) && ch->race != i && ch->level <= LEVEL_HERO)
	    {
		send_to_char("You can only post notes to your own race.\n\r",ch);
		return;
	    }
	}
	note_attach( ch,type );
        if (ch->pcdata->pnote->type != type)
        {
            send_to_char( "You already have a different note in progress.\n\r",ch);
            return;
        }
	free_string( ch->pcdata->pnote->to_list );
	ch->pcdata->pnote->to_list = str_dup( argument );
	send_to_char( "Ok.\n\r", ch );
	return;
    }

    if ( !str_prefix( arg, "clear" ) )
    {
	if ( ch->pcdata->pnote != NULL )
	{
	    free_note(ch->pcdata->pnote);
	    ch->pcdata->pnote = NULL;
	}
	send_to_char( "Ok.\n\r", ch );
	return;
    }
    if ( !str_prefix( arg, "show" ) )
    {
	if ( ch->pcdata->pnote == NULL )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}
	if (ch->pcdata->pnote->type != type)
	{
	    send_to_char("You aren't working on that kind of note.\n\r",ch);
	    return;
	}
	sendf( ch, "%s: %s\n\rTo: %s\n\r", ch->pcdata->pnote->sender, ch->pcdata->pnote->subject, ch->pcdata->pnote->to_list);
	send_to_char( ch->pcdata->pnote->text, ch );
	return;
    }
    if ( !str_prefix( arg, "post" ) || !str_prefix(arg, "send"))
    {
	char *strtime;
	CHAR_DATA* vch;
	if ( ch->pcdata->pnote == NULL )
	{
	    send_to_char( "You have no note in progress.\n\r", ch );
	    return;
	}
        if (ch->pcdata->pnote->type != type)
        {
            send_to_char("You aren't working on that kind of note.\n\r",ch);
            return;
        }
	if (!str_cmp(ch->pcdata->pnote->to_list,""))
	{
            send_to_char("You need to provide a recipient (name, cabal, or immortal).\n\r",
		ch);
	    return;
	}
	if (!str_cmp(ch->pcdata->pnote->subject,""))
	{
	    send_to_char("You need to provide a subject.\n\r",ch);
	    return;
	}
/* VOTES DONE HERE */
	/* do a check for an application */
	if (ch->pcdata->pnote->type == NOTE_APPLICATION){
	  /* CABAL */
	  int result = create_app_vote(ch, ch->pcdata->pnote->to_list, ch->pcdata->pnote->subject, ch->pcdata->pnote->text);
	  if ( result == VOTE_ACCEPTED){
	    free_note(ch->pcdata->pnote);
	    ch->pcdata->pnote = NULL;
	    return;
	  }
	  else if ( result == VOTE_ERROR ){
	    return;
	  }
	  /* CRUSADER */
	  result = create_crus_app(ch, ch->pcdata->pnote->to_list, ch->pcdata->pnote->subject, ch->pcdata->pnote->text);
	  if ( result == VOTE_ERROR ){
	    return;
	  }
	  /* PSI */
	  result = create_psi_app(ch, ch->pcdata->pnote->to_list, ch->pcdata->pnote->subject, ch->pcdata->pnote->text);
	  if ( result == VOTE_ERROR ){
	    return;
	  }
	  /* AVATAR */
	  result = create_avatar_app(ch, ch->pcdata->pnote->to_list, ch->pcdata->pnote->subject, ch->pcdata->pnote->text);
	  if ( result == VOTE_ERROR ){
	    return;
	  }
	  /* DEMON */
	  result = create_demon_app(ch, ch->pcdata->pnote->to_list, ch->pcdata->pnote->subject, ch->pcdata->pnote->text);
	  if ( result == VOTE_ERROR ){
	    return;
	  }
	  /* VAMPIRE */
	  result = create_vamp_app(ch, ch->pcdata->pnote->to_list, ch->pcdata->pnote->subject, ch->pcdata->pnote->text);
	  if ( result == VOTE_ERROR ){
	    return;
	  }
	  /* UNDEAD */
	  result = create_undead_app(ch, ch->pcdata->pnote->to_list, ch->pcdata->pnote->subject, ch->pcdata->pnote->text);
	  if ( result == VOTE_ERROR ){
	    return;
	  }
	}

	ch->pcdata->pnote->next			= NULL;
	strtime				= ctime( &mud_data.current_time );
	strtime[strlen(strtime)-1]	= '\0';
	ch->pcdata->pnote->date			= str_dup( strtime );
	ch->pcdata->pnote->date_stamp		= mud_data.current_time;
	append_note(ch->pcdata->pnote);
	wiznet("$N has posted a note.",ch,NULL,WIZ_NOTES,0,get_trust(ch));
	/*  Let char know he/they recived a note */
	for ( vch = player_list; vch != NULL; vch = vch->next_player ){
	  if (is_ignore( vch, ch))
	    continue;
	  else if (is_note_to(vch, ch->pcdata->pnote)
	      && ch != vch)
	    act_new("`8A messenger hands you a scroll marked $t``\n\r", vch,
		  ch->pcdata->pnote->type == NOTE_IDEA ? "\"idea\""  :
		  ch->pcdata->pnote->type == NOTE_NOTE ? "\"note\""  :
		  ch->pcdata->pnote->type == NOTE_APPLICATION ? "\"application\""  :
		  ch->pcdata->pnote->type == NOTE_PENALTY ? "\"penalty\""  :
		  ch->pcdata->pnote->type == NOTE_NEWS ? "\"news\""  :
		  ch->pcdata->pnote->type == NOTE_CHANGES ? "\"changes\""  : "\"note\"", NULL, TO_CHAR, IS_IMMORTAL(ch) ? POS_DEAD : POS_RESTING);
	}
	ch->pcdata->pnote = NULL;
        send_to_char( "Ok.\n\r", ch );
	return;
    }
    send_to_char( "You can't do that.\n\r", ch );
}

bool check_spam(char *site)
{
    int num = 0;
    char host[MSL], host1[MSL], host2[MSL], buf[MSL];
    DESCRIPTOR_DATA *d, *d_next;
    BAN_DATA *pban;
    strcpy(host,capitalize(site));
    host[0] = LOWER(host[0]);
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
	d_next = d->next;
    	strcpy(host1,capitalize(d->host));
    	strcpy(host2,capitalize(d->ident));
    	host1[0] = LOWER(host1[0]);
    	host2[0] = LOWER(host2[0]);
        if (!str_cmp(host,host1) || !str_cmp(host,host2))
	    num++;
    }
    if (num < 15)
	return FALSE;
    pban = new_ban();
    pban->name = str_dup(site);
    pban->level = 60;
    pban->ban_flags = BAN_ALL;
    SET_BIT(pban->ban_flags,BAN_PERMANENT);
    pban->next  = ban_list;
    ban_list    = pban;
    save_bans();
    sprintf(buf,"%s has been caught hammering, site autobanned.",site);
    nlogf("%s", buf);
    wiznet(buf, NULL,NULL,WIZ_LINKS,0,51);
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
	d_next = d->next;
    	strcpy(host1,capitalize(d->host));
    	strcpy(host2,capitalize(d->ident));
    	host1[0] = LOWER(host1[0]);
    	host2[0] = LOWER(host2[0]);
        if (!str_cmp(host,host1) || !str_cmp(host,host2))
	    close_socket(d);
    }
    return TRUE;
}

void do_traceban( char *argument )
{
    char strsave[MIL], namelist[MSL], nameread[MSL], buf[MSL];
    int times = 0;
    bool exist = FALSE;
    FILE *fp;
    fclose( fpReserve );
    sprintf( strsave, "%s", TRACKIP_FILE );
    sprintf(namelist,"%s","");
    if ( (fp = fopen( strsave, "r" ) ) != NULL )
    {
        for ( ; ; )
        {
            if( fscanf (fp, "%s", nameread) < 0 ){
                nameread[0] = '\0';
            }
            if ( !str_cmp( nameread, "END" ) )
                break;
            else
            {
	    	times = fread_number(fp);
		if (!str_cmp(argument,nameread))
		{
                   exist = TRUE;
		   times++;
		}
		sprintf(buf,"%s %d\n",nameread,times);
		strcat(namelist, buf);
            }
        }
    }
    else
        fp = fopen( NULL_FILE, "r" );
    fclose( fp );
    fp = fopen( strsave, "w" );
    if (!exist)
    {
	sprintf(buf,"%s %d\n",argument,1);
	strcat(namelist, buf);
    }
    fprintf( fp, "%s", namelist );
    fprintf( fp, "END" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
}

PROG_LIST *new_mprog(void)
{
   static PROG_LIST mp_zero;
   PROG_LIST *mp;

   if (mprog_free == NULL)
       mp = alloc_perm(sizeof(*mp));
   else
   {
       mp = mprog_free;
       mprog_free=mprog_free->next;
   }

   *mp = mp_zero;
   mp->vnum             = 0;
   mp->trig_type        = 0;
   mp->code             = str_dup("");
   VALIDATE(mp);
   return mp;
}

void free_mprog(PROG_LIST *mp)
{
   if (!IS_VALID(mp))
      return;

   INVALIDATE(mp);
   mp->next = mprog_free;
   mprog_free = mp;
}


PROG_LIST *new_oprog(void)
{
   static PROG_LIST op_zero;
   PROG_LIST *op;

   if (oprog_free == NULL)
       op = alloc_perm(sizeof(*op));
   else
   {
       op = oprog_free;
       oprog_free=oprog_free->next;
   }

   *op = op_zero;
   op->vnum             = 0;
   op->trig_type        = 0;
   op->code             = str_dup("");
   VALIDATE(op);
   return op;
}

void free_oprog(PROG_LIST *op)
{
   if (!IS_VALID(op))
      return;

   INVALIDATE(op);
   op->next = oprog_free;
   oprog_free = op;
}

PROG_LIST *new_rprog(void)
{
   static PROG_LIST rp_zero;
   PROG_LIST *rp;

   if (rprog_free == NULL)
       rp = alloc_perm(sizeof(*rp));
   else
   {
       rp = rprog_free;
       rprog_free=rprog_free->next;
   }

   *rp = rp_zero;
   rp->vnum             = 0;
   rp->trig_type        = 0;
   rp->code             = str_dup("");
   VALIDATE(rp);
   return rp;
}

void free_rprog(PROG_LIST *rp)
{
   if (!IS_VALID(rp))
      return;

   INVALIDATE(rp);
   rp->next = rprog_free;
   rprog_free = rp;
}

//05-09-00 Viri: Modifierd free char to free deity string
//05-11-00 Viri: Added new_obj_spell, free_obj_spell
//01-02-01 Ath: Added "application" type notes
//02-05-01 Ath: Added note list "to"/"from" feature
//02-05-01 Ath: Added imms can read notes to ALL cabals
//03-06-01: Viri: Added prog stuff.
