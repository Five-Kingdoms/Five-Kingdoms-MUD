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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <malloc.h>
#include "merc.h"
#include "cabal.h"
#include "recycle.h"
#include "bounty.h"
#include "armies.h"
#include "alias.h"
#include "dndspell.h"
#include "clan.h"


extern CABAL_DATA* get_cabal(const char *name);
extern int ClanAddMember( int handle, CHAR_DATA* ch);
/* test for complete write of a pfile */
bool test_pfile(FILE *fp ){
  rewind(fp);

  while (!feof( fp )){
    char letter = fread_letter( fp );
    char* word;

    if (letter != '#')
      continue;

    word = fread_word( fp );
    if (word != NULL && !str_cmp("END", word)){
      rewind(fp);
      return TRUE;
    }
  }
  rewind(fp);
  return FALSE;
}


char *print_flags(int flag)
{
    int count, pos = 0;
    static char buf[52];
    for (count = 0; count < 32;  count++)
        if (IS_SET(flag,1<<count))
        {
            if (count < 26)
                buf[pos] = 'A' + count;
            else
                buf[pos] = 'a' + (count - 26);
            pos++;
        }
    if (pos == 0)
    {
        buf[pos] = '0';
        pos++;
    }
    buf[pos] = '\0';
    return buf;
}

#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];

/* Save a character and inventory.                     *
 * Would be cool to save NPC's too for quest purposes, *
 *   some of the infrastructure is provided.           */
void save_char_obj( CHAR_DATA *ch)
{
    CHAR_DATA *vch;
    char strsave[MIL];
    FILE *fp;
    if ( IS_NPC(ch) )
	return;

    if ( ch->desc != NULL && ch->desc->original != NULL )
	ch = ch->desc->original;
    if (IS_IMMORTAL(ch) || ch->level >= LEVEL_IMMORTAL)
    {
	fclose(fpReserve);
	sprintf(strsave, "%s%s",GOD_DIR, capitalize(ch->name));
	if ((fp = fopen(strsave,"w")) == NULL)
	{
	    bug("Save_char_obj: fopen",0);
	    perror(strsave);
 	}
	fprintf(fp,"Lev %2d Trust %2d  %s%s\n",
	    ch->level, get_trust(ch), ch->name, ch->pcdata->title);
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
    }
    fclose( fpReserve );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
    if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
    {
	bug( "Save_char_obj: fopen", 0 );
	perror( strsave );
	fp = fopen( NULL_FILE, "r" );
    }
    else
    {
      fwrite_char( ch, fp );
      if ( ch->carrying != NULL )
	  fwrite_obj( ch, ch->carrying, fp, 0, 0, FALSE );
	if (ch->pet != NULL && IS_NPC(ch->pet))
	    fwrite_pet(ch->pet,fp, FALSE);
	/* save other mobs but do not save pet twice */
        for (vch = char_list; vch; vch = vch->next){
	  if (IS_NPC(vch)
	      && vch->summoner == ch
	      && vch->pIndexData->vnum != MOB_VNUM_MAGIC_EYE
	      && (ch->pet == NULL || vch != ch->pet))
	    fwrite_pet(vch,fp, FALSE);
	}
	fprintf( fp, "#END\n" );
    }
    fclose ( fp );
    if ( ( fp = fopen( TEMP_FILE, "r" ) ) != NULL ){
      /* test here for proper file write */
      if (test_pfile (fp) == FALSE){
	char buf[MIL];
	send_to_char("`!Corrupted Save detected.  Aborting..``\n\r", ch);
	sprintf(buf, "`!Corrupted Save detected: %s``", ch->name);
	wiznet(buf, NULL,NULL,WIZ_LOGINS,0,0);
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
      }
      else{
	rename(TEMP_FILE,strsave);
	fclose( fp );
	fpReserve = fopen( NULL_FILE, "r" );
	save_limits();
      }
    }
    else{
      bug("save_char_obj: Could not reopen temp file.", 0);
      fpReserve = fopen( NULL_FILE, "r" );
    }
}

/* Write the char. */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
    AFFECT_DATA *paf;
    SKILL_DATA *nsk;
    QUEST_DATA* q;
    ALIAS_DATA* al;
    int counter;
    int sn, gn;
    fprintf( fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYER"	);
    fprintf( fp, "SaveVer %d\n",	SAVE_VERSION    );
    ch->save_ver = SAVE_VERSION;
    fprintf( fp, "Name %s~\n",	ch->name		);
    if (!IS_NULLSTR(ch->pcdata->family))
        fprintf( fp, "Fam  %s~\n", ch->pcdata->family	);
    if (ch->pcdata->alias[0] != '\0' )
        fprintf( fp, "Alias %s~\n", ch->pcdata->alias	);
    if (ch->pcdata->race[0] != '\0' )
        fprintf( fp, "ReRace %s~\n", ch->pcdata->race	);
    fprintf( fp, "Religion %d\n", ch->pcdata->way	);
    if (HAS_CLAN(ch)){
      //synch clan data
      SynchClanData( ch );
      fprintf( fp, "ClanHandle %s~\n", GetClanName(GET_CLAN(ch))	);
    }
    fprintf( fp, "Id   %ld\n", ch->id			);
    if (ch->pcdata->fight_delay != (time_t)NULL && ch->pcdata->fight_delay >= 0
	&& (int)difftime(mud_data.current_time,ch->pcdata->fight_delay) >= 0)
	fprintf( fp, "FLag %ld\n", ch->pcdata->fight_delay );
    if (ch->pcdata->pk_delay != (time_t)NULL && ch->pcdata->pk_delay >= 0
	&& (int)difftime(mud_data.current_time,ch->pcdata->pk_delay) >= 0)
	fprintf( fp, "PLag %ld\n", ch->pcdata->pk_delay );
    if (ch->pcdata->ghost != (time_t)NULL && ch->pcdata->ghost >= 0
	&& (int)difftime(mud_data.current_time,ch->pcdata->ghost) >= 0){
      fprintf( fp, "Ghost %ld\n", (long int)difftime(mud_data.current_time, ch->pcdata->ghost));
    }
    fprintf( fp, "Ansi %d\n", ch->color);
    if (ch->short_descr[0] != '\0')
      	fprintf( fp, "ShD  %s~\n",	ch->short_descr	);
    if( ch->long_descr[0] != '\0')
	fprintf( fp, "LnD  %s~\n",	ch->long_descr	);
    if (ch->description[0] != '\0')
    	fprintf( fp, "Desc %s~\n",	ch->description	);
    if (ch->prompt != NULL || !str_cmp(ch->prompt,"%n%P%C<%hhp %mm %vmv> "))
        fprintf( fp, "Prom %s~\n",      ch->prompt  	);
    fprintf( fp, "Race %s~\n", pc_race_table[ch->race].name );
    fprintf( fp, "Etho %s~\n", ethos_table[ch->pcdata->ethos].name);
    if (ch->pcdata->clan_points)
      fprintf( fp, "ClanPts %d\n", ch->pcdata->clan_points );
    if (ch->pCabal){
      fprintf( fp, "Cabal %d %s~\n", ch->pcdata->rank, ch->pCabal->name );
      fprintf( fp, "Cpts %ld\n", ch->pcdata->cpoints);
      if (!IS_NULLSTR(ch->pcdata->royal_guards))
	fprintf( fp, "RGuard %s~\n", ch->pcdata->royal_guards);
    }
    fprintf( fp, "Sex  %d\n",	ch->sex			);
    fprintf( fp, "Cla  %d\n",	ch->class		);
    fprintf( fp, "Tatt %d\n",   ch->tattoo >= MAX_TATTOO ? 0 : ch->tattoo              );
    fprintf( fp, "Home %d\n",   ch->hometown            );
    fprintf( fp, "Levl %d\n",	ch->level		);
    if (ch->in_bank != 0)
        fprintf(fp, "Bank %d\n",     ch->in_bank        );
    fprintf( fp, "Rec  %d %d %d %d %d\n", ch->pcdata->kpc, ch->pcdata->knpc, ch->pcdata->dpc, ch->pcdata->dnpc, ch->pcdata->dall);
    if (ch->pcdata->start_wep){
      fprintf( fp, "StartWep %d\n", ch->pcdata->start_wep );
    }
    //conversion for old pfiles
    if (ch->pcdata->birth_date == 0){
      int age = 18 + ch->played / 76000;
      age = UMIN(2000, age);
      ch->pcdata->birth_date = mud_data.current_time - (age * MAX_MONTHS * MAX_WEEKS * MAX_DAYS * MAX_HOURS * 30);
    }
    fprintf( fp, "Birthdate %ld\n", ch->pcdata->birth_date );

    /* detailed info */
    if (ch->pcdata->fDetail){
      fprintf( fp, "Detail %d\n", ch->pcdata->fDetail );
      fprintf( fp, "Det_Body %d\n", ch->pcdata->body_type );
      fprintf( fp, "Det_Face %d\n", ch->pcdata->face_type );
      fprintf( fp, "Det_Skin %d\n", ch->pcdata->skin_color );
      fprintf( fp, "Det_Eyes %d\n", ch->pcdata->eye_color );
      fprintf( fp, "Det_Hair %d\n", ch->pcdata->hair_color );
      fprintf( fp, "Det_HairL %d\n", ch->pcdata->hair_length );
      fprintf( fp, "Det_HairT %d\n", ch->pcdata->hair_texture);
      fprintf( fp, "Det_HairF %d\n", ch->pcdata->facial_hair);
      fprintf( fp, "Det_Hand %d\n", ch->pcdata->lefthanded);
    }
    if (ch->pcdata->enemy_race){
      fprintf( fp, "EnemyRace %s~\n", pc_race_table[ch->pcdata->enemy_race].name);
    }
    if (ch->class == gcn_monk || ch->class == gcn_blademaster)
    {
      fprintf( fp, "Monk %d ", MAX_ANATOMY);
      for (counter = 0; counter < MAX_ANATOMY; counter++)
	fprintf( fp, " %d", ch->pcdata->anatomy[counter] );
      fprintf( fp, "\n");
    }

    /* number of kills of each race */
    fprintf(fp, "AOS %d ", MAX_PC_RACE );
    for (counter = 0; counter < MAX_PC_RACE; counter++)
      fprintf( fp, " %d", ch->pcdata->race_kills[counter] );
    fprintf( fp, "\n");


    if (ch->pcdata->divfavor)
      fprintf( fp, "DFavor %d\n",	ch->pcdata->divfavor	);
    if (ch->pcdata->wep_pos != WEPPOS_NORMAL
	&& (get_skill(ch, gsn_battlestance) || get_skill(ch, gsn_2h_tactics)))
      fprintf( fp, "WPos %d\n",	ch->pcdata->wep_pos	);
    if (ch->pcdata->bat_pos != BATPOS_MIDDLE)
      fprintf( fp, "BPos %d\n",	ch->pcdata->bat_pos	);
    if (ch->pcdata->beacon != 0)
	fprintf( fp, "Beac %d\n", ch->pcdata->beacon	);
    if (ch->imm_flags != race_table[ch->race].imm)
	fprintf( fp, "ImmF %ld\n", ch->imm_flags	);
    if (ch->res_flags != race_table[ch->race].res)
	fprintf( fp, "ResF %ld\n", ch->res_flags	);
    if (ch->vuln_flags != race_table[ch->race].vuln)
	fprintf( fp, "VulnF %ld\n", ch->vuln_flags);
    fprintf( fp, "FLAGGED %d\n", ch->pcdata->flagged	);
    fprintf( fp, "RANK %d\n", ch->pcdata->rank		);
    if (ch->race == grn_werebeast)
      fprintf( fp, "Btype %d\n", ch->pcdata->beast_type);
    if (ch->trust != 0 && mud_data.mudport == MAIN_PORT)
	fprintf( fp, "Tru  %d\n",	ch->trust	);
    fprintf( fp, "Sec  %d\n",    ch->pcdata->security   );
    fprintf( fp, "Plyd %d\n", ch->played + (ch->logon == 0 ? 0 : (int) (mud_data.current_time - ch->logon)));
    fprintf( fp, "Not  %ld %ld %ld %ld %ld %ld\n",
	     ch->pcdata->last_note,ch->pcdata->last_idea,
	     ch->pcdata->last_penalty,
	     ch->pcdata->last_news,
	     ch->pcdata->last_changes,
	     ch->pcdata->last_application);
    fprintf( fp, "Vote %ld\n", 	     ch->pcdata->last_vote);
    fprintf( fp, "BankStamp %ld\n",  ch->pcdata->bank_stamp);
    if (ch->pcdata->vote_skip > 0)
      fprintf( fp, "VSkip  %d\n",    ch->pcdata->vote_skip);
    if (ch->pcdata->last_cabal != NULL){
      fprintf( fp, "LastCabal  %s~\n", ch->pcdata->last_cabal->name);
      fprintf( fp, "LastApp %ld\n", ch->pcdata->last_app);
    }
    if (ch->pcdata->last_bounty)
      fprintf( fp, "LastBounty %ld\n", ch->pcdata->last_bounty);
    fprintf( fp, "Scro %d\n", 	ch->lines		);

    if (ch->pcdata->roomnum < 0){
      fprintf( fp, "LogO %ld\n",	ch->logoff		);
      fprintf( fp, "Room %d\n", 0-ch->pcdata->roomnum );
    }
    else{
      /* this is to set the right room to return the character too */
      int vnum = ROOM_VNUM_TEMPLE;

      /* was_in_room holds the last room entered that was not limbo or virtual room */
      if (ch->was_in_room)
	vnum = ch->was_in_room->vnum;
      /* this should not be neccessary, just here for safety */
      else if (ch->in_room && ch->in_room->vnum != ROOM_VNUM_LIMBO && !IS_VIRROOM(ch->in_room))
	vnum = ch->was_in_room->vnum;

      /* Print last logoff only if proper room was set (not brough on etc.) */
      fprintf( fp, "LogO %ld\n",	mud_data.current_time		);
      fprintf( fp, "Room %d\n", vnum );

    }
    fprintf( fp, "HMV  %d %d %d %d %d %d\n", ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );
    if (ch->gold > 0)
        fprintf( fp, "Gold %ld\n",        ch->gold      );
    else
        fprintf( fp, "Gold %d\n", 0                     );
    fprintf( fp, "Exp  %ld\n",      ch->exp              );
    fprintf( fp, "MExp %ld\n",   ch->max_exp          );
    if (ch->act != 0)
	fprintf( fp, "Act  %s\n",   print_flags(ch->act));
    if (ch->act2 != 0)
	fprintf( fp, "Act2  %s\n",  print_flags(ch->act2));
    if (ch->game != 0){
      fprintf( fp, "Game  %s\n",  print_flags(ch->game));
    }
    if (ch->pcdata->perk_bits != 0){
      fprintf( fp, "Perks  %s\n",  print_flags(ch->pcdata->perk_bits));
    }
    if (ch->affected_by != 0)
	fprintf( fp, "AfBy %s\n",   print_flags(ch->affected_by));
    if (ch->affected2_by != 0)
      fprintf( fp, "AfB2 %s\n",   print_flags(ch->affected2_by));
    fprintf( fp, "Comm %s\n",       print_flags(ch->comm));
    if (ch->wiznet)
    	fprintf( fp, "Wizn %s\n",   print_flags(ch->wiznet));
    if (ch->invis_level)
	fprintf( fp, "Invi %d\n", 	ch->invis_level	);
    if (ch->incog_level)
	fprintf(fp,"Inco %d\n",ch->incog_level);
    fprintf( fp, "Pos  %d\n", (ch->position == POS_FIGHTING) ? POS_STANDING : ch->position );
    if (ch->practice != -9999)
    	fprintf( fp, "Prac %d\n",	ch->practice	);
    if (ch->train != -9999)
	fprintf( fp, "Trai %d\n",	ch->train	);
    fprintf( fp, "Saves %d %d %d %d %d\n",	ch->savingaffl, ch->savingmaled, ch->savingmental, ch->savingbreath, ch->savingspell);
    fprintf( fp, "Alig  %d\n",	ch->alignment		);
    if (ch->hitroll != 0)
	fprintf( fp, "Hit   %d\n",	ch->hitroll	);
    if (ch->damroll != 0)
	fprintf( fp, "Dam   %d\n",	ch->damroll	);
    fprintf( fp, "ACs %d %d %d %d\n",
	ch->armor[0],ch->armor[1],ch->armor[2],ch->armor[3]);
    if (ch->wimpy !=0 )
	fprintf( fp, "Wimp  %d\n",	ch->wimpy	);
    fprintf( fp, "AttrB %d %d %d %d %d %d\n",
	     ch->perm_stat[STAT_STR],
	     ch->perm_stat[STAT_INT],
	     ch->perm_stat[STAT_WIS],
	     ch->perm_stat[STAT_DEX],
	     ch->perm_stat[STAT_CON],
	     ch->perm_stat[STAT_LUCK] );
    fprintf (fp, "AModB %d %d %d %d %d %d\n",
	     UMIN(ch->perm_stat[STAT_STR], ch->mod_stat[STAT_STR]),
	     UMIN(ch->perm_stat[STAT_INT], ch->mod_stat[STAT_INT]),
	     UMIN(ch->perm_stat[STAT_WIS], ch->mod_stat[STAT_WIS]),
	     UMIN(ch->perm_stat[STAT_DEX], ch->mod_stat[STAT_DEX]),
	     UMIN(ch->perm_stat[STAT_CON], ch->mod_stat[STAT_CON]),
	     UMIN(ch->perm_stat[STAT_LUCK], ch->mod_stat[STAT_LUCK]));


    fprintf( fp, "Mods %d", MAX_MODIFIER);
    for (counter = 0; counter < MAX_MODIFIER; counter++)
      fprintf( fp, " %d", ch->aff_mod[counter] );
    fprintf( fp, "\n");
    fprintf( fp, "SSpicked %d", MAX_GROUPS);
    for (counter = 0; counter < MAX_GROUPS; counter++)
      fprintf( fp, " %d", ch->pcdata->ss_picked[counter] );
    fprintf( fp, "\n");

    if ( IS_NPC(ch) )
	fprintf( fp, "Vnum %d\n",	ch->pIndexData->vnum	);
    else
    {
      if (ch->pcdata->messages){
    	fprintf( fp, "Msgs %s\n",   print_flags(ch->pcdata->messages));
      }
      fprintf( fp, "Pass %s~\n",	ch->pcdata->pwd		);
      if (!IS_NULLSTR(ch->pcdata->pwddel))
	fprintf( fp, "PassDel %s~\n",	ch->pcdata->pwddel		);
      if (IS_IMMORTAL(ch) && ch->pcdata->pwdimm[0] != '\0' )
	fprintf( fp, "PassImm %s~\n",	ch->pcdata->pwdimm	);
      if (ch->pcdata->bamfin[0] != '\0')
	fprintf( fp, "Bin  %s~\n",	ch->pcdata->bamfin);
      if (ch->pcdata->bamfout[0] != '\0')
	fprintf( fp, "Bout %s~\n",      ch->pcdata->bamfout);
      if (!IS_NULLSTR(ch->pcdata->title))
	fprintf( fp, "Title %s~\n",	ch->pcdata->title	);
      if (ch->pcdata->psalm_pen != 0){
	fprintf( fp, "PsaPen %d\n",	ch->pcdata->psalm_pen  );
      }
      fprintf( fp, "TSex %d\n",	ch->pcdata->true_sex	);
      fprintf( fp, "LLev %d\n",	ch->pcdata->last_level	);
      fprintf( fp, "HMVP %d %d %d\n", ch->pcdata->perm_hit, ch->pcdata->perm_mana, ch->pcdata->perm_move);
      fprintf( fp, "Cnd  %d %d %d %d\n", ch->pcdata->condition[0],
	       ch->pcdata->condition[1], ch->pcdata->condition[2], ch->pcdata->condition[3] );
      fprintf( fp, "Mplayed %d %d\n", ch->pcdata->month, ch->pcdata->mplayed + UMAX(0,(ch->logon == 0 ? 0 : (int) ((mud_data.current_time - ch->logon)/60))));

/* Quests */
      q = ch->pcdata->quests;
      while (q){
	if (q->save != QUEST_TEMP)
	  fprintf( fp, "Quest %s~ %d\n", q->quest, q->save);
	q = q->next;
      }
      for (al = ch->pcdata->aliases; al; al = al->next){
	fprintf( fp, "Aliases %s~ %s~\n", al->name, al->content);
	if (al->next == ch->pcdata->aliases)
	  break;
      }
      for ( sn = 0; sn < MAX_AFF_DISPLAY; sn++){
	if (IS_NULLSTR(ch->pcdata->custom_aff[sn].name))
	  continue;
	else
	  fprintf( fp, "CusA %d %s %s~\n", sn,
		   ch->pcdata->custom_aff[sn].name,
		   IS_GEN( ch->pcdata->custom_aff[sn].gsn) ? effect_table[GN_POS(ch->pcdata->custom_aff[sn].gsn)].name : skill_table[GN_POS(ch->pcdata->custom_aff[sn].gsn)].name );
      }
      for ( sn = 0; sn < MAX_SKILL; sn++ ){
	if ( skill_table[sn].name != NULL
	     && skill_table[sn].rating[ch->class] > 0
	     && ch->pcdata->learned[sn] > 0 )
	  fprintf( fp, "Sk %d '%s'\n",
		   UMIN(get_skillmax(ch, sn), ch->pcdata->learned[sn]),
		   skill_table[sn].name );
	if ( skill_table[sn].name != NULL && ch->pcdata->progress[sn] > 0 )
	  fprintf( fp, "Pr %d '%s'\n",
		   ch->pcdata->progress[sn],
		   skill_table[sn].name );
      }
      for ( sn = 0; sn < MAX_SONG; sn++ )
	if ( song_table[sn].name != NULL && ch->pcdata->songlearned[sn] > 0 )
	  fprintf( fp, "Sg %d '%s'\n", ch->pcdata->songlearned[sn], song_table[sn].name );
	for ( sn = 0; sn < MAX_PSALM; sn++ ){
	  if ( psalm_table[sn].name != NULL && ch->pcdata->psalms[sn] > 0)
	    fprintf( fp, "Psa %d '%s'\n", ch->pcdata->psalms[sn], psalm_table[sn].name );
	}
	for ( gn = 0; gn < MAX_GROUP; gn++ )
	  if ( group_table[gn].name != NULL && ch->pcdata->group_known[gn])
	    fprintf( fp, "Gr '%s'\n",group_table[gn].name);
    }
    for ( paf = ch->affected; paf != NULL; paf = paf->next )
      {

	if (paf->type < 0 || (paf->type >= (MAX_SKILL + MAX_EFFECTS)) || paf->type == gsn_doppelganger)
	  continue;
	if (paf->type == gsn_phantasmal_griffon
	    || paf->type == gsn_illusionary_spectre
	    || paf->type == gsn_phantom_dragon
	    || paf->type == gsn_eavesdrop)
	    continue;

 //check fo NOSAVE on gen's
	if (IS_GEN(paf->type)){
	  if (IS_SET(effect_table[GN_POS(paf->type)].flags, EFF_F1_NOSAVE))
	    continue;
	}

	//GEN's
	if (IS_GEN(paf->type))
        fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d %3d %3d %3s~\n",
	  effect_table[GN_POS(paf->type)].name,
	  paf->where,
          paf->level,
          paf->duration,
          paf->modifier,
          paf->location,
          paf->bitvector,
	  paf->flags,
          (paf->has_string && paf->string != NULL ? 1 : 0),
          (paf->has_string && paf->string != NULL ? paf->string : "NULL"));
	  else
	    //GSN's
        fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d %3d %3d %3s~\n",
	  skill_table[paf->type].name,
          paf->where,
          paf->level,
          paf->duration,
          paf->modifier,
          paf->location,
          paf->bitvector,
	  paf->flags,
          (paf->has_string && paf->string != NULL ? 1 : 0),
          (paf->has_string && paf->string != NULL ? paf->string : ""));

    }
    for ( paf = ch->affected2; paf != NULL; paf = paf->next )
    {
	if (paf->type < 0 || paf->type >= MAX_SONG)
	    continue;
        fprintf( fp, "Affsong '%s' %3d %3d %3d %3d %3d %10d\n",
	  song_table[paf->type].name, paf->where, paf->level, paf->duration, paf->modifier, paf->location, paf->bitvector);
    }
    for ( nsk = ch->pcdata->newskills; nsk != NULL; nsk = nsk->next )
    {
	if (nsk->sn < 0 || nsk->sn >= MAX_SKILL)
	    continue;
        fprintf( fp, "NewSk '%s' %3d %3d %3d %3d\n",
	  skill_table[nsk->sn].name, nsk->level, nsk->rating, nsk->learned, nsk->keep);
    }
    //TEMPORARY
    if (IS_DNDS(ch) && ch->class != gcn_psi){
      REMOVE_BIT(ch->game, GAME_DNDS);
    }

    /* DNDSpells						*
     * There are DNDS_TEMPLATES templates, and   the currently  *
     * used one is pointed to by ->dndspells			*
     * We write the index of current template, followed by all  *
     * templates.						*/
    if (IS_DNDS(ch)){
      int template;

      //write the memorized spells (must be before reading templates)
      fwrite_dndmemory( fp, ch->pcdata->dndmemory );
      //write the template names
      for (template = 0; template < DNDS_TEMPLATES; template++){
	if (!IS_NULLSTR(ch->pcdata->dndtemplate_name[template]))
	  fprintf(fp, "DnDTempName %d %s~\n", template, ch->pcdata->dndtemplate_name[template]);
      }
      for (template = 0; template < DNDS_TEMPLATES; template++){
	fwrite_dnds_template(fp, ch->pcdata->dndtemplates[template], template);
      }
      //set the start template
      fprintf( fp, "DndTemplate %ld\n", (ch->pcdata->dndspells - ch->pcdata->dndtemplates[0]) / DNDS_MAXLEVEL);
    }
    fprintf( fp, "End\n\n" );
}

/* write a pet */
void fwrite_pet( CHAR_DATA *pet, FILE *fp, bool fArmy)
{
    AFFECT_DATA *paf;
    int counter = 0;
    int vnum = 0;

    if (!IS_NPC(pet))
      return;

    if (!fArmy)
      fprintf(fp,"#PET\n");
    fprintf(fp,"Vnum %d\n",pet->pIndexData->vnum);
    fprintf(fp,"Name %s~\n", pet->name);
    fprintf(fp,"LogO %ld\n", mud_data.current_time);
    if (pet->short_descr != pet->pIndexData->short_descr)
    	fprintf(fp,"ShD  %s~\n", pet->short_descr);
    if (pet->long_descr != pet->pIndexData->long_descr)
    	fprintf(fp,"LnD  %s~\n", pet->long_descr);
    if (pet->description != pet->pIndexData->description)
    	fprintf(fp,"Desc %s~\n", pet->description);
    if (pet->race != pet->pIndexData->race)
    	fprintf(fp,"Race %s~\n", race_table[pet->race].name);
    if (pet->pCabal)
        fprintf( fp, "Cab %s~\n", pet->pCabal->name);
    fprintf(fp,"Sex  %d\n", pet->sex);
    if (pet->level != pet->pIndexData->level)
    	fprintf(fp,"Levl %d\n", pet->level);
    fprintf(fp, "HMV  %d %d %d %d %d %d\n", pet->hit, pet->max_hit, pet->mana, pet->max_mana, pet->move, pet->max_move);
    if (pet->gold > 0)
    	fprintf(fp,"Gold %ld\n",pet->gold);
//used in armies to store last direction walked in
    fprintf( fp, "Status %d\n", pet->status );
    if (pet->exp > 0)
    	fprintf(fp, "Exp  %ld\n", pet->exp);
    if (pet->act != pet->pIndexData->act)
    	fprintf(fp, "Act  %s\n", print_flags(pet->act));
    if (pet->act2 != pet->pIndexData->act2)
    	fprintf(fp, "Act2  %s\n", print_flags(pet->act2));
    if (pet->off_flags != pet->pIndexData->off_flags)
    	fprintf(fp, "Off  %s\n", print_flags(pet->off_flags));
    if (pet->affected_by != pet->pIndexData->affected_by)
    	fprintf(fp, "AfBy %s\n", print_flags(pet->affected_by));
    if (pet->comm != 0)
    	fprintf(fp, "Comm %s\n", print_flags(pet->comm));
    fprintf(fp,"Pos  %d\n", (pet->position == POS_FIGHTING) ? POS_STANDING : pet->position);

    /* this is to set the right room to return the character too */
    vnum = ROOM_VNUM_TEMPLE;

    /* was_in_room holds the last room entered that was not limbo or virtual room */
    if (pet->was_in_room)
      vnum = pet->was_in_room->vnum;
    /* this should not be neccessary, just here for safety */
    else if (pet->in_room && pet->in_room->vnum != ROOM_VNUM_LIMBO && !IS_VIRROOM(pet->in_room))
      vnum = pet->was_in_room->vnum;

    /* Print last logoff only if proper room was set (not brough on etc.) */
    fprintf( fp, "Room %d\n", vnum );

    if (pet->practice != -9999 && pet->practice != 0)
    	fprintf( fp, "Prac %d\n",	pet->practice	);
    if (pet->train != -9999 && pet->train != 0)
	fprintf( fp, "Trai %d\n",	pet->train	);
    fprintf( fp, "Saves %d %d %d %d %d\n", pet->savingaffl, pet->savingmaled, pet->savingmental, pet->savingbreath, pet->savingspell);
    if (pet->alignment != pet->pIndexData->alignment)
    	fprintf(fp, "Alig %d\n", pet->alignment);
    if (pet->hitroll != pet->pIndexData->hitroll)
    	fprintf(fp, "Hit  %d\n", pet->hitroll);
    if (pet->damroll != pet->pIndexData->damage[DICE_BONUS])
    	fprintf(fp, "Dam  %d\n", pet->damroll);
    if (pet->dam_type != pet->pIndexData->dam_type)
      fprintf(fp, "DamType  %d\n", pet->dam_type);
    fprintf(fp, "ACs  %d %d %d %d\n", pet->armor[0],pet->armor[1],pet->armor[2],pet->armor[3]);
    fprintf(fp, "Attr %d %d %d %d %d\n",
      pet->perm_stat[STAT_STR],pet->perm_stat[STAT_INT],pet->perm_stat[STAT_WIS],pet->perm_stat[STAT_DEX],pet->perm_stat[STAT_CON]);
    fprintf(fp, "AMod %d %d %d %d %d\n",
	     UMIN(pet->perm_stat[STAT_STR], pet->mod_stat[STAT_STR]),
	     UMIN(pet->perm_stat[STAT_INT], pet->mod_stat[STAT_INT]),
	     UMIN(pet->perm_stat[STAT_WIS], pet->mod_stat[STAT_WIS]),
	     UMIN(pet->perm_stat[STAT_DEX], pet->mod_stat[STAT_DEX]),
	     UMIN(pet->perm_stat[STAT_CON], pet->mod_stat[STAT_CON]));
    fprintf( fp, "Mods %d", MAX_MODIFIER);
    for (counter = 0; counter < MAX_MODIFIER; counter++)
      fprintf( fp, " %d", pet->aff_mod[counter] );
    fprintf( fp, "\n" );
    for ( paf = pet->affected; paf != NULL; paf = paf->next )
    {
    	if (paf->type < 0 || (paf->type >= (MAX_SKILL + MAX_EFFECTS) ) || paf->type == gsn_doppelganger)
    	    continue;
	if (paf->type == gsn_phantasmal_griffon
	    || paf->type == gsn_illusionary_spectre
	    || paf->type == gsn_phantom_dragon
	    || paf->type == gsn_eavesdrop)
	    continue;

   //check fo NOSAVE on gen's
	if (IS_GEN(paf->type))
	  if (IS_SET(effect_table[GN_POS(paf->type)].flags, EFF_F1_NOSAVE))
	    continue;


	//GEN's
	if (IS_GEN(paf->type))
        fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d %3d %3d %3s~\n",
	  effect_table[GN_POS(paf->type)].name,
	  paf->where,
          paf->level,
          paf->duration,
          paf->modifier,
          paf->location,
          paf->bitvector,
          paf->flags,
          (paf->has_string && paf->string != NULL ? 1 : 0),
          (paf->has_string && paf->string != NULL ? paf->string : "NULL"));
	  else
	    //GSN's
        fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d %3d %3d %3s~\n",
	  skill_table[paf->type].name,
          paf->where,
          paf->level,
          paf->duration,
          paf->modifier,
          paf->location,
          paf->bitvector,
          paf->flags,
          (paf->has_string && paf->string != NULL ? 1 : 0),
          (paf->has_string && paf->string != NULL ? paf->string : ""));
    }//END FOR
    for ( paf = pet->affected2; paf != NULL; paf = paf->next )
    {
    	if (paf->type < 0 || paf->type >= MAX_SONG)
    	    continue;
        fprintf(fp, "Affsong '%s' %3d %3d %3d %3d %3d %10d\n", song_table[paf->type].name,
            paf->where, paf->level, paf->duration, paf->modifier,paf->location, paf->bitvector);
    }
    fprintf(fp,"End\n\n");
}

/* Write an object and its contents. */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest, int hold_limit, bool CABAL )
{
    OBJ_SPELL_DATA *spell;
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *paf;
    TRAP_DATA* pTrap;
    char buf[MIL];


/* check for ch being a crusader */
    bool fCrus = (ch && ch->class == class_lookup("crusader"));
/* check if object is being counted as limited */
    bool fLim = IS_LIMITED(obj) || IS_OBJ_STAT (obj, ITEM_HOLDS_RARE)
      || (fCrus && obj->pCabal && (obj->level >= 30 || obj->cost >= 125));
/* rare litmit for any character, crusaders get special treatment to double it */
    int rare_limit = ch ? fCrus ? 2 * ((ch->level / 10) - 1) : (ch->level / 10) - 1 : 0;

    /* if bounty reward has just been done then we allow all rares to be saved */
    if (ch && !IS_NPC(ch)
	&& (IS_SET(ch->pcdata->messages, MSG_BOUNTY_REWARD)
	    || IS_SET(ch->pcdata->messages, MSG_BOUNTY_RETURN))
	)
      rare_limit = 999;

    if (fLim && obj->item_type == ITEM_SOCKET && obj->in_obj != NULL)
      fLim = FALSE;
    if (fLim && obj->item_type == ITEM_PROJECTILE && obj->in_obj != NULL)
      fLim = FALSE;

    /* further limit check for cabal rares on crusaders */
/* don't count objects inside HOLDS RARE object or sockets inside objects*/
    if (fLim && !IS_SET(obj->wear_flags, ITEM_HAS_OWNER)
	&& (obj->in_obj == NULL || !IS_OBJ_STAT (obj->in_obj, ITEM_HOLDS_RARE))
	&& !is_affected_obj(obj, gen_malform) ){
/* crusaders count worn items unless gsn_uorder */
      if (obj->wear_loc == WEAR_NONE)
	hold_limit++;
      else if (fCrus && !is_affected(ch, gsn_uorder))
	hold_limit++;
    }
/* recursion, check for this object being the cabal item */
    if (!CABAL && obj->next_content != NULL && obj->in_room == NULL )
      fwrite_obj( ch, obj->next_content, fp, iNest, hold_limit, FALSE );

/* Castrate storage characters. */
    if (obj->item_type == ITEM_KEY)
      return;
    if ((ch) && ch->level < 15 && fLim)
      return;
/* check for hold limit being surpassed */
    if ((ch) && !IS_IMMORTAL(ch) && fLim && obj->pIndexData->vnum != OBJ_VNUM_MAL_WEAPON){
      if (obj->wear_loc == WEAR_NONE && hold_limit  > rare_limit)
	return;
      else if (fCrus && hold_limit > rare_limit && !is_affected(ch, gsn_uorder))
	return;
    }
    if (ch && obj->item_type == ITEM_CABAL)
      return;
    if ( !ch && obj->in_room && obj->in_room->vnum == ROOM_VNUM_LIMIT)
      return;
/* END OF DECISIONS< START SAVING THE OBJECT */

    fprintf( fp, "#O\n" );
    fprintf( fp, "Vnum %d\n",   obj->pIndexData->vnum        );
    fprintf( fp, "SaveVer %d\n",	SAVE_VERSION    );
    if (!obj->pIndexData->new_format)
	fprintf( fp, "Oldstyle\n");
    if (obj->enchanted)
	fprintf( fp,"Enchanted\n");
    if (obj->eldritched)
	fprintf( fp,"Eldritched\n");

    if (iNest == 9999)
    {
	fprintf( fp, "Room %d\n", obj->carried_by->in_room->vnum );
	iNest=0;
    }
    else if (!ch && obj->in_room != NULL)
    {
        fprintf( fp, "Room %d\n", obj->in_room->vnum );
	if (obj->idle != 0)
	    fprintf( fp, "Idle %d\n", obj->idle );
    }
    if (!ch)
	fprintf( fp, "Home %d\n", obj->homevnum );
    fprintf( fp, "Nest %d\n",	iNest	  	     );
    if ( obj->name != obj->pIndexData->name)
    	fprintf( fp, "Name %s~\n",	obj->name		     );
    if ( obj->short_descr != obj->pIndexData->short_descr)
        fprintf( fp, "ShD  %s~\n",	obj->short_descr	     );
    if ( obj->description != obj->pIndexData->description)
        fprintf( fp, "Desc %s~\n",	obj->description	     );
    if ( obj->extra_flags != obj->pIndexData->extra_flags)
      fprintf( fp, "ExtF %d\n",	obj->extra_flags	     );
    if ( obj->wear_flags != obj->pIndexData->wear_flags)
        fprintf( fp, "WeaF %ld\n",	obj->wear_flags		     );
    if ( obj->item_type != obj->pIndexData->item_type)
        fprintf( fp, "Ityp %d\n",	obj->item_type		     );
    if ( obj->weight != obj->pIndexData->weight)
        fprintf( fp, "Wt   %d\n",	obj->weight		     );
    if ( obj->condition != obj->pIndexData->condition)
	fprintf( fp, "Cond %d\n",	obj->condition		     );
    if ( obj->pCabal
	 && (obj->pIndexData->pCabal == NULL || obj->pIndexData->pCabal->vnum != obj->pCabal->vnum))
      fprintf( fp, "Cabal %s~\n",	obj->pCabal->name	     );
    if ( obj->race != obj->pIndexData->race)
	fprintf( fp, "Race %d\n",	obj->race		     );
    if ( obj->class != obj->pIndexData->class)
	fprintf( fp, "Class %d\n",	obj->class		     );
    fprintf( fp, "Wear %d\n",   obj->wear_loc                );
    if (obj->level != obj->pIndexData->level)
        fprintf( fp, "Lev  %d\n",	obj->level		     );
    if (obj->recall != 0)
	fprintf( fp, "Rec  %d\n",	obj->recall		);
    if (obj->owner != 0)
	fprintf( fp, "Own  %ld\n",	obj->owner		);
    if (obj->timer != 0)
        fprintf( fp, "Time %d\n",	obj->timer	     );
    if (obj->vnum != obj->pIndexData->vnum)
      fprintf( fp, "VirVnum %d\n",	obj->vnum	     );
    fprintf( fp, "Cost %d\n",	obj->cost		     );
    if (obj->value[0] != obj->pIndexData->value[0] || obj->value[1] != obj->pIndexData->value[1]
    || obj->value[2] != obj->pIndexData->value[2] || obj->value[3] != obj->pIndexData->value[3]
    || obj->value[4] != obj->pIndexData->value[4])
        fprintf( fp, "Val  %d %d %d %d %d\n", obj->value[0], obj->value[1], obj->value[2], obj->value[3], obj->value[4] );
    switch ( obj->item_type )
    {
    case ITEM_POTION:
    case ITEM_SCROLL:
    case ITEM_PILL:
	if ( obj->value[1] > 0 )
            fprintf( fp, "Spell 1 '%s'\n", skill_table[obj->value[1]].name );
	if ( obj->value[2] > 0 )
            fprintf( fp, "Spell 2 '%s'\n", skill_table[obj->value[2]].name );
	if ( obj->value[3] > 0 )
            fprintf( fp, "Spell 3 '%s'\n", skill_table[obj->value[3]].name );
	break;
    case ITEM_STAFF:
    case ITEM_WAND:
	if ( obj->value[3] > 0 )
            fprintf( fp, "Spell 3 '%s'\n", skill_table[obj->value[3]].name );
	break;
    }

//Write affects
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
//Viri 04-23-00 added suport for GEN's
      if (paf->type < 0 || (paf->type >= (MAX_SKILL + MAX_EFFECTS)))
	    continue;

 //check fo NOSAVE on gen's
	if (IS_GEN(paf->type))
	  if (IS_SET(effect_table[GN_POS(paf->type)].flags, EFF_F1_NOSAVE))
	    continue;
	//GEN'S
      if (IS_GEN(paf->type))
        fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d %3d %3d %3s~\n",
		 effect_table[GN_POS(paf->type)].name,
		 paf->where,
		 paf->level,
		 paf->duration,
		 paf->modifier,
		 paf->location,
		 paf->bitvector,
		 paf->flags,
		 (paf->has_string && paf->string != NULL ? 1 : 0),
		 (paf->has_string && paf->string != NULL ? paf->string : "NULL"));
      else
	//GSN's
        fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d %3d %3d %3s~\n",
		 skill_table[paf->type].name,
		 paf->where,
		 paf->level,
		 paf->duration,
		 paf->modifier,
		 paf->location,
		 paf->bitvector,
		 paf->flags,
		 (paf->has_string && paf->string != NULL ? 1 : 0),
		 (paf->has_string && paf->string != NULL ? paf->string : ""));
    }//END FOR PAF

//Write Spells
    for ( spell = obj->spell; spell != NULL; spell = spell->next )
      {
	if (spell->spell < 0 || spell->spell > MAX_SKILL)
	  continue;
	fprintf(fp, "Affs '%s' %3d %3d\n%s~\n%s~\n",
		skill_table[spell->spell].name,
		spell->target,
		spell->percent,
		spell->message,
		spell->message2);
      }//end FOR SPELLS

/* traps only non-owner traps are saved over boot */
    for (pTrap = obj->traps; pTrap; pTrap = pTrap->next_trap){
      int i = 0;
      if (pTrap->owner)
	continue;
      fprintf( fp, "Trap %d %s~\n",
	       pTrap->pIndexData->vnum,
	       pTrap->name);
      fprintf(fp, "%d %d %d %d %d %s\n%d",
	      pTrap->armed,
	      pTrap->age,
	      pTrap->type,
	      pTrap->level,
	      pTrap->duration,
	      fwrite_flag( pTrap->flags, buf),
	      MAX_TRAPVAL);
      for (i = 0; i < MAX_TRAPVAL; i++){
	fprintf( fp, " %d", pTrap->value[i]);
      }
      fprintf( fp, "\n");
    }

    for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
        fprintf( fp, "ExDe %s~ %s~\n", ed->keyword, ed->description );
    if (IS_LIMITED(obj))
	fprintf( fp, "Limited %d\n", obj->pIndexData->vnum);
    fprintf( fp, "End\n\n" );
    if ( obj->contains != NULL )
	fwrite_obj( ch, obj->contains, fp, iNest + 1, hold_limit, FALSE );
}

char *load_purge( char *name )
{
    char *pass = NULL;
    char strsave[MIL];
    FILE *fp;
    fclose( fpReserve );
    sprintf( strsave, "%s%s", PURGE_DIR, capitalize( name ) );
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	int iNest;
        char *word;
	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;
	for ( ; ; )
	{
	    word = feof( fp ) ? "Pass" : fread_word( fp );
            if ( !str_cmp( word, "Pass" ) )
	    {
		pass = fread_word  ( fp );
		smash_tilde2( pass );
	        break;
	    }
	}
    }
    else
	fp = fopen( NULL_FILE, "r" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return pass;
}

/* Load a char and inventory into a new ch structure. */
int load_char( DESCRIPTOR_DATA *d, char *name )
{
    int stat = 0;
    char strsave[MIL];
    FILE *fp;
    bool found;
    struct tm *this_time;
    CHAR_DATA *ch = new_char();
    ch->pcdata = new_pcdata();
    this_time = gmtime(&mud_data.current_time);
    d->character			= ch;
    if (d->editor == 69)
        ch->desc			= NULL;
    else
	ch->desc			= d;
    ch->name				= str_dup( name );
    ch->pcdata->alias			= str_dup( "" );
    ch->pcdata->race			= str_dup( "" );
    ch->pcdata->family			= str_dup( "" );
    ch->pcdata->deity			= deity_table[0].god;
    ch->pcdata->way			= 0;
    ch->pcdata->clan			= str_dup( "" );
    ch->pcdata->quests			= NULL;
    ch->pcdata->max_quests		= 0;
    ch->id				= get_pc_id();
    ch->logoff				= mud_data.current_time;
    ch->color                           = 0;
    ch->pCabal				= NULL;
    ch->pcdata->member			= NULL;
    ch->pcdata->month			= this_time->tm_mon;
    ch->pcdata->mplayed			= 0;
    ch->race				= race_lookup("human");
    ch->pcdata->ethos			= ETHOS_NEUTRAL;
    ch->act				= PLR_NOSUMMON;
    ch->act2				= 0;
    ch->comm                            = COMM_PROMPT;
    ch->game				= 0;
    ch->prompt 				= str_dup("%n%P%C<%hhp %mm %vmv> ");
    ch->pcdata->messages		= MSG_NONE;
    ch->pcdata->confirm_delete		= FALSE;
    ch->pcdata->confirm_quit		= FALSE;
    ch->pcdata->pwd			= str_dup( "" );
    ch->pcdata->pwddel			= str_dup( "" );
    ch->pcdata->pwdimm			= str_dup( "" );
    ch->pcdata->bamfin			= str_dup( "" );
    ch->pcdata->bamfout			= str_dup( "" );
    ch->pcdata->title			= str_dup( "" );
    ch->pcdata->pbounty			= get_bounty( ch->name);
    ch->pcdata->psalm_pen		= 0;
    ch->pcdata->start_wep		= -1;


    for (stat = 0; stat < MAX_GROUPS; stat++){
      ch->pcdata->ss_picked[stat]	 = 0;
    }
    for (stat = 0; stat < MAX_PSALM; stat++){
      ch->pcdata->psalms[stat]	 = 0;
    }
    for (stat = 0; stat < MAX_MODIFIER; stat++){
      ch->aff_mod[stat]		 = 0;
    }
    for (stat = 0; stat < MAX_QUEST; stat++)
    {
	ch->pcdata->quest[stat] = 0;
	ch->pcdata->questorder[stat] = 0;
    }
    for (stat = 0; stat < MAX_TRAPS; stat++)
        ch->traps[stat]            = NULL;
    for (stat = 0; stat < MAX_ANATOMY; stat++)
	ch->pcdata->anatomy[stat]	= 0;
    for (stat = 0; stat < MAX_STATS-1; stat++)
	ch->perm_stat[stat]		= 13;
    for (stat = 0; stat < MAX_SKILL; stat++){
      ch->pcdata->learned[stat]		 = 0;
      ch->pcdata->to_learned[stat]	 = 0;
    }
    /* reset dndspells */
    reset_dndspells(ch->pcdata);

    ch->perm_stat[STAT_LUCK]		= 16;
    ch->pcdata->condition[COND_THIRST]	= 48;
    ch->pcdata->condition[COND_FULL]	= 48;
    ch->pcdata->condition[COND_HUNGER]	= 48;
    ch->pcdata->security                = 0;
    ch->pcdata->dpc                     = 0;
    ch->pcdata->dnpc                    = 0;
    ch->pcdata->dall                    = 0;
    ch->pcdata->flagged			= 0;
    ch->pcdata->rank			= 0;
    ch->pcdata->beast_type		= 0;
    ch->pcdata->kpc                     = 0;
    ch->pcdata->knpc                    = 0;
    ch->tattoo                          = 0;
    ch->pcdata->fight_delay		= (time_t)NULL;
    ch->pcdata->pk_delay		= (time_t)NULL;
    ch->savingaffl			= 0;
    ch->savingmaled			= 0;
    ch->savingmental			= 0;
    ch->savingbreath			= 0;
    ch->savingspell			= 0;
    ch->extracted			= FALSE;
    ch->caballeader			= FALSE;
    ch->status				= 0;
    ch->pcdata->transnum		= 0;
    ch->pcdata->pStallion		= NULL;
    ch->pcdata->ignore			= str_dup( "" );
    ch->pcdata->beacon			= 0;
    ch->pcdata->divfavor		= 0;
    ch->pcdata->wep_pos			= WEPPOS_NORMAL;
    ch->pcdata->cpoints			= 0;
    ch->pcdata->vote_skip		= 0;
    ch->pcdata->last_app		= (time_t)NULL;
    ch->pcdata->last_bounty		= (time_t)NULL;
    ch->pcdata->last_cabal		= NULL;


    found				= FALSE;
    fclose( fpReserve );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
      int iNest;
      /* test if we can read this char */
      if (test_pfile( fp ) == FALSE){
	char buf [MIL];
	sprintf (buf, "Corrupted Pfile detected: %s.", ch->name);
	bug(buf, 0);
	return -1;
      }

      for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	rgObjNest[iNest] = NULL;
      found = TRUE;
      for ( ; ; )
	{
	  char *word;
	    char letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }
	    if ( letter != '#' )
	    {
		bug( "Load_obj: # not found.", 0 );
		break;
	    }
	    word = fread_word( fp );
            if ( !str_cmp( word, "PLAYER" ) )
		fread_char  ( ch, fp );
            else
		break;
	}
    }
    else
	fp = fopen( NULL_FILE, "r" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );

    if (found)
    {
	int i;
	if (ch->race == 0)
	    ch->race = race_lookup("human");
	ch->size = pc_race_table[ch->race].size;
	if (ch->race == race_lookup("illithid"))
            ch->dam_type = 28;
	else
            ch->dam_type = 17;
	for (i = 0; i < 5; i++)
	{
	    if (pc_race_table[ch->race].skills[i] == NULL)
		break;
	    group_add(ch,pc_race_table[ch->race].skills[i],TRUE);
	}
	ch->affected_by = ch->affected_by|race_table[ch->race].aff;
	ch->imm_flags	= ch->imm_flags | race_table[ch->race].imm;
	ch->res_flags	= ch->res_flags | race_table[ch->race].res;
	ch->vuln_flags	= ch->vuln_flags | race_table[ch->race].vuln;
	ch->form	= race_table[ch->race].form;
	ch->parts	= race_table[ch->race].parts;
        if (ch->level > MAX_LEVEL)
            ch->level = 1;
        if (ch->trust > MAX_LEVEL)
            ch->trust = 1;
    }
    return found;
}

void load_obj( CHAR_DATA *ch )
{
    char strsave[MIL];
    FILE *fp;
    fclose( fpReserve );
    if (ch)
        sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
    else
	sprintf( strsave, "%s", OBJSAVE_FILE );

    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	int iNest;
	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;
	for ( ; ; )
	{
	    char *word;
	    char letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }
	    if ( letter != '#' )
	    {
		bug( "Load_obj: # not found.", 0 );
		break;
	    }
	    word = fread_word( fp );
            if      ( !str_cmp( word, "PLAYER" ) ) fread_null ( ch, fp );
            else if ( !str_cmp( word, "OBJECT" ) ) fread_obj  ( ch, fp, NULL );
            else if ( !str_cmp( word, "O"      ) ) fread_obj  ( ch, fp, NULL );
            else if ( !str_cmp( word, "PET"    ) ) fread_pet  ( ch, fp, ch->in_room );
            else if ( word != NULL && !str_cmp( word, "END"    ) ) break;
            else
            {
                bug( "Load_obj: bad section.", 0 );
                break;
            }
	}
    }
    else
	fp = fopen( NULL_FILE, "r" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
}

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                                    \
    if ( !str_cmp( word, literal ) )                                    \
    {                                                                   \
        field  = value;                                                 \
        fMatch = TRUE;                                                  \
        break;                                                          \
    }

/* provided to free strings */
#if defined(KEYS)
#undef KEYS
#endif

#define KEYS( literal, field, value )					\
    if ( !str_cmp( word, literal ) )                                    \
    {                                                                   \
        free_string(field);                                             \
        field  = value;                                                 \
        fMatch = TRUE;                                                  \
        break;                                                          \
    }

void fread_char( CHAR_DATA *ch, FILE *fp )
{
    char *word = NULL;
    bool fMatch = FALSE;
    int lastlogoff = mud_data.current_time, percent;
    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;
	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;
	case 'A':
            if (!str_cmp( word, "AC"))
	    {
		fread_to_eol(fp);
		fMatch = TRUE;
		break;
	    }
	    if (!str_cmp(word,"ACs"))
	    {
		int i;
		for (i = 0; i < 4; i++)
		    ch->armor[i] = fread_number(fp);
		fMatch = TRUE;
		break;
	    }
	    KEY( "Act",		ch->act,		fread_flag( fp) | race_table[ch->race].act);
	    KEY( "Act2",	ch->act2,		fread_flag( fp) | race_table[ch->race].act2);
	    KEY( "AfBy",	ch->affected_by,	fread_flag( fp ) );
	    KEY( "AfB2",	ch->affected2_by,	fread_flag( fp ) );
            if (!str_cmp(word, "Affc"))
            {
                AFFECT_DATA *paf = new_affect();
                int sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown skill.",0);
                else
                    paf->type = sn;
                paf->where      = fread_number(fp);
                paf->level      = fread_number( fp );
                paf->duration   = fread_number( fp );
                paf->modifier   = fread_number( fp );
                paf->location   = fread_number( fp );
                paf->bitvector  = fread_number( fp );
		if (ch->save_ver > 2005)
		  paf->flags = fread_number( fp );
		paf->has_string = fread_number( fp );
		if (paf->has_string)
		  paf->string =   fread_string( fp );
		else
		  fread_string( fp );
                paf->next       = ch->affected;
                ch->affected    = paf;
                fMatch = TRUE;
                break;
            }
	    if (!str_cmp(word, "AffD"))
	    {
                AFFECT_DATA *paf = new_affect();
                int sn = skill_lookup(fread_word(fp));
		if (sn < 0)
		    bug("Fread_char: unknown skill.",0);
		else
		    paf->type = sn;
		paf->level	= fread_number( fp );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		if (ch->save_ver > 2005)
		  paf->flags = fread_number( fp );
		paf->has_string = fread_number( fp );
		if (paf->has_string)
		  paf->string =   fread_string( fp );
		else
		  fread_string( fp );
		paf->next	= ch->affected;
		ch->affected	= paf;
		fMatch = TRUE;
		break;
	    }
            if (!str_cmp(word, "Affsong"))
            {
                AFFECT_DATA *paf = new_affect();
                int sn = song_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown song.",0);
                else
                    paf->type = sn;
                paf->where      = fread_number(fp);
                paf->level      = fread_number( fp );
                paf->duration   = fread_number( fp );
                paf->modifier   = fread_number( fp );
                paf->location   = fread_number( fp );
                paf->bitvector  = fread_number( fp );
                paf->next       = ch->affected2;
                ch->affected2    = paf;
                fMatch = TRUE;
                break;
            }
	    KEYS( "Alias",	ch->pcdata->alias,	fread_string( fp ) );
	    if ( !str_cmp( word, "Aliases" ) ){
	      char* name;
	      char* content;
	      name = fread_string( fp );
	      content = fread_string( fp );
	      alias_to_char(ch, name , content);
	      free_string( name );
	      free_string( content );
	      fMatch = TRUE;
	      break;
	    }
	    KEY( "Alig",	ch->alignment,		fread_number( fp ) );
            if ( !str_cmp( word, "Ansi"))
            {
                ch->color               = fread_number( fp );
                fMatch=TRUE;
                break;
            }
            if ( !str_cmp( word, "AOS")){
	      int max = fread_number( fp );
	      int i;

	      for (i = 0; i < max; i++){
		if (i < MAX_PC_RACE)
		  ch->pcdata->race_kills[i] = fread_number( fp );
		else
		  fread_number( fp );
	      }
	      fMatch = TRUE;
	      break;
	    }
            if (!str_cmp(word,"Attr"))
	    {
		int stat;
		for (stat = 0; stat < (MAX_STATS-1); stat++)
			ch->perm_stat[stat] = fread_number(fp);
		ch->perm_stat[STAT_LUCK] = _roll_stat(ch,STAT_LUCK);
		fMatch = TRUE;
		break;
	    }
            if (!str_cmp(word,"AttrB"))
	    {
		int stat;
		for (stat = 0; stat < MAX_STATS; stat++)
			ch->perm_stat[stat] = fread_number(fp);
		fMatch = TRUE;
		break;
	    }
            if (!str_cmp(word,"AMod"))
	    {
		int stat;
		for (stat = 0; stat < (MAX_STATS-1); stat ++)
		   ch->mod_stat[stat] = fread_number(fp);
		ch->mod_stat[STAT_LUCK] = 0;
		fMatch = TRUE;
		break;
	    }
            if (!str_cmp(word,"AModB"))
	    {
		int stat;
		for (stat = 0; stat < MAX_STATS; stat ++)
		   ch->mod_stat[stat] = fread_number(fp);
		fMatch = TRUE;
		break;
	    }
	    break;
	case 'B':
	    KEY( "Bank",        ch->in_bank,            fread_number( fp ) );
	    KEY( "BankStamp",   ch->pcdata->bank_stamp, fread_number( fp ) );
	    KEY( "Beac",        ch->pcdata->beacon,     fread_number( fp ) );
	    KEYS( "Bin",	ch->pcdata->bamfin,	fread_string( fp ) );
	    KEY( "Birthdate",   ch->pcdata->birth_date, fread_long( fp ) );
	    KEYS( "Bout",	ch->pcdata->bamfout,	fread_string( fp ) );
	    KEY( "BPos",	ch->pcdata->bat_pos,	fread_number( fp ) );
	    KEY( "Btype",       ch->pcdata->beast_type, fread_number( fp ) );
	    break;
	case 'C':
	  KEY( "ClanPts",       ch->pcdata->clan_points,fread_number( fp ) );
	  if (!str_cmp(word, "Cabal")){
	    int rank			=		fread_number( fp );
	    char *buf = fread_string( fp );

	    ch->pCabal			=		get_cabal( buf );
	    ch->pcdata->rank		=		rank;
	    free_string(buf);
	    ch->pcdata->member		=		add_cmember( ch, get_parent(ch->pCabal) );
	    if (ch->pcdata->member)
	      ch->pcdata->member->last_update =		mud_data.current_time;
	    fMatch = TRUE;
	    break;
	  }
	  if ( !str_cmp( word, "CusA" ) ){
	      char* name, *skill, buf[MIL];
	      int pos = fread_number( fp );
	      int sn;

	      name = fread_word( fp );
	      skill = fread_string( fp );

	      strcpy( buf, name);
	      buf[3] = 0;
	      sn = skill_lookup( skill );

	      free_string( name );
	      free_string( skill );

	      if (pos >= 0 && pos < MAX_AFF_DISPLAY
		  && sn > 0 && sn < (MAX_SKILL + MAX_EFFECTS)){
		strcpy(ch->pcdata->custom_aff[pos].name, buf );
		ch->pcdata->custom_aff[pos].gsn = sn;
	      }
	      fMatch = TRUE;
	      break;
	  }
	  KEY( "Cla",	      ch->class,		fread_number( fp ) );
	  if (!str_cmp(word, "ClanHandle")){
	    char* name = fread_string( fp );
	    int handle = GetClanHandle( name );

	    if (!str_cmp(name, "Sacredoak"))
	      handle = GetClanHandle( "Sacred Oak" );
	    if ( handle < 1){
	      nlogf("fread_char: Unable to load clan %s.", name );
	    }
	    else if (GetClanMember( handle, ch->name ) != NULL){
	      ch->pcdata->clan_handle = handle;
	    }

	    free_string( name );
	    fMatch = TRUE;
	    break;
	  }
	  if (!str_cmp(word,"Cnd"))
            {
                ch->pcdata->condition[0] = fread_number( fp );
                ch->pcdata->condition[1] = fread_number( fp );
                ch->pcdata->condition[2] = fread_number( fp );
		ch->pcdata->condition[3] = fread_number( fp );
                fMatch = TRUE;
                break;
            }
	    KEY("Comm",		ch->comm,		fread_flag( fp ) );
            if (!str_cmp(word,"Cond"))
	    {
		ch->pcdata->condition[0] = fread_number( fp );
		ch->pcdata->condition[1] = fread_number( fp );
		ch->pcdata->condition[2] = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    KEY( "Cpts",	ch->pcdata->cpoints,		fread_number( fp ) );
	    break;
	case 'D':
	    KEY( "Dam",		ch->damroll,		fread_number( fp ) );
	    KEYS( "Desc",	ch->description,	fread_string( fp ) );
	    KEY( "Detail",	ch->pcdata->fDetail,	fread_number( fp ) );
	    KEY( "Det_Body",	ch->pcdata->body_type,	fread_number( fp ) );
	    KEY( "Det_Face",	ch->pcdata->face_type,	fread_number( fp ) );
	    KEY( "Det_Skin",	ch->pcdata->skin_color,	fread_number( fp ) );
	    KEY( "Det_Eyes",	ch->pcdata->eye_color,	fread_number( fp ) );
	    KEY( "Det_Hair",	ch->pcdata->hair_color,	fread_number( fp ) );
	    KEY( "Det_HairL",	ch->pcdata->hair_length,fread_number( fp ) );
	    KEY( "Det_HairT",	ch->pcdata->hair_texture,fread_number( fp ) );
	    KEY( "Det_HairF",	ch->pcdata->facial_hair,fread_number( fp ) );
	    KEY( "Det_Hand",	ch->pcdata->lefthanded,	fread_number( fp ) );
	    if (!str_cmp(word, "DndMem")){
	      char* temp = fread_string( fp );
	      int sn = skill_lookup( temp );
	      free_string( temp );

	      if (sn >= 0)
		ch->pcdata->dndmemory[sn] = fread_number( fp );
	      else
		fread_number( fp );
	      fMatch = TRUE;
	      break;
	    }
	    if (!str_cmp(word, "DndTemplate")){
	      int template = fread_number( fp );
	      ch->pcdata->dndspells = ch->pcdata->dndtemplates[template];
	      fMatch = TRUE;
	      break;
	    }
	    if (!str_cmp(word, "DndTempName")){
	      int template = fread_number( fp );
	      ch->pcdata->dndtemplate_name[template] = fread_string( fp );
	      fMatch = TRUE;
	      break;
	    }
	    if (!str_cmp(word, "DndSpell")){
	      DNDS_DATA dndsd;
	      int template = fread_dnds(fp, &dndsd, ch->save_ver );
	      if (template >= 0 && IS_DNDS(ch)){
		/* point the dndspells pointer to the template we want */
		ch->pcdata->dndspells = ch->pcdata->dndtemplates[template];
		add_dndspell(ch->pcdata, dndsd.sn, dndsd.level, dndsd.mem, dndsd.set);
	      }
	      fMatch = TRUE;
	      break;
	    }
	    KEY( "DPC",         ch->pcdata->dpc,        fread_number( fp ) );
            KEY( "DNPC",        ch->pcdata->dnpc,       fread_number( fp ) );
            KEY( "DALL",        ch->pcdata->dall,       fread_number( fp ) );
            KEY( "DFavor",      ch->pcdata->divfavor,   fread_number( fp ) );
	    break;
	case 'E':
	  if (!str_cmp(word, "EnemyRace")){
	    char* race = fread_string( fp );
	    ch->pcdata->enemy_race = race_lookup( race );
	    free_string( race );

	    fMatch = TRUE;
	    break;
	  }
	  if ( word != NULL && !str_cmp( word, "End" ) )
	    {
	      lastlogoff = ch->logoff;
	      SynchClanData( ch );
	      percent = (mud_data.current_time - lastlogoff) * 25 / ( 2 * 60 * 60);
	      percent = UMIN(percent,100);
	      if (percent > 0 && !IS_AFFECTED(ch,AFF_POISON) && !IS_AFFECTED(ch,AFF_PLAGUE))
    		{
		  ch->hit	+= (ch->max_hit - ch->hit) * percent / 100;
		  ch->mana    += (ch->max_mana - ch->mana) * percent / 100;
		  ch->move    += (ch->max_move - ch->move)* percent / 100;
    		}
	      return;
	    }
	  if (!str_cmp(word, "Etho")){
	    char *buf = fread_string( fp );
	    ch->pcdata->ethos		=		ethos_lookup( buf );
	    free_string(buf);
	    fMatch = TRUE;
	    break;
	  }
	  KEY( "Exp",		ch->exp,		fread_long( fp ) );
	  break;
	case 'F':
	  KEY( "FLag",        ch->pcdata->fight_delay,fread_number( fp ) );
            KEYS( "Fam",         ch->pcdata->family,	fread_string( fp ) );
            KEY( "FLAGGED",     ch->pcdata->flagged,    fread_number( fp ) );
	    break;
	case 'G':
	    KEY( "Game",	ch->game,   fread_flag( fp ) );
	    if (!str_cmp( word, "Ghost" )){
	      long diff = fread_long( fp );
	      ch->pcdata->ghost = ((time_t) time( NULL ) - diff);
	      fMatch = TRUE;
	      break;
	    }
	    KEY( "Gold",	ch->gold,		fread_long( fp ) );
            if ( !str_cmp( word, "Group" )  || !str_cmp(word,"Gr"))
            {
                char *temp = fread_word( fp ) ;
                int gn = group_lookup(temp);
                if ( gn < 0 )
                {
                    fprintf(stderr,"%s",temp);
                    bug( "Fread_char: unknown group. ", 0 );
                }
                else
		    gn_add(ch,gn);
                fMatch = TRUE;
            }
	    break;
	case 'H':
	    KEY( "Hit",		ch->hitroll,		fread_number( fp ) );
            if (!str_cmp(word,"HMV"))
	    {
		ch->hit		= fread_number( fp );
		ch->max_hit	= fread_number( fp );
		ch->mana	= fread_number( fp );
		ch->max_mana	= fread_number( fp );
		ch->move	= fread_number( fp );
		ch->max_move	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }
            if (!str_cmp(word,"HMVP"))
            {
                ch->pcdata->perm_hit	= fread_number( fp );
                ch->pcdata->perm_mana   = fread_number( fp );
                ch->pcdata->perm_move   = fread_number( fp );
                fMatch = TRUE;
                break;
            }
            KEY( "Home",        ch->hometown,           fread_number( fp ) );
	    break;
	case 'I':
	    KEY( "Id",		ch->id,			fread_number( fp ) );
	    KEY( "ImmF",	ch->imm_flags,		fread_flag( fp) | race_table[ch->race].imm);
	    KEY( "Inco",	ch->incog_level,	fread_number( fp ) );
	    KEY( "Invi",	ch->invis_level,	fread_number( fp ) );
	    break;
	case 'K':
            KEY( "KPC",         ch->pcdata->kpc,        fread_number( fp ) );
            KEY( "KNPC",        ch->pcdata->knpc,       fread_number( fp ) );
	    break;
	case 'L':
	  if ( !str_cmp( word, "LastCabal" ) ){
	    char* name = fread_string (fp );
	    CABAL_DATA* pc = get_cabal( name );

	    if (!IS_NULLSTR( name ))
	      free_string( name );
	    if (pc != NULL)
	      ch->pcdata->last_cabal = pc;
	    else
	      ch->pcdata->last_cabal = NULL;
	    fMatch = TRUE;
	    break;
	  }
	  KEY( "LastApp",	ch->pcdata->last_app,	fread_number( fp ) );
	  KEY( "LastBounty",	ch->pcdata->last_bounty,fread_number( fp ) );
	  KEY( "LLev",	ch->pcdata->last_level, fread_number( fp ) );
	  KEY( "Levl",	ch->level,		fread_number( fp ) );
	  KEY( "LogO",	ch->logoff,		fread_long( fp ) );
	  KEYS( "LnD",	ch->long_descr,		fread_string( fp ) );
	  break;
        case 'M':
            KEY( "MExp",        ch->max_exp,            fread_number( fp ) );
	    if ( !str_cmp( word, "Monk" ) )
	    {
	      int counter;
	      if (ch->save_ver < 3000){
		for (counter = 0; counter < MAX_PC_RACE; counter++)
		if (counter < MAX_ANATOMY)
		  ch->pcdata->anatomy[counter] = fread_number(fp);
		else
		  fread_number(fp);
	      }
	      else{
		int max = fread_number( fp );
		for (counter = 0; counter < max; counter++)
		  if (counter < MAX_ANATOMY)
		    ch->pcdata->anatomy[counter] = fread_number(fp);
		  else
		    fread_number(fp);
	      }
	      fMatch = TRUE;
	      break;
	    }
	    if ( !str_cmp( word, "Mods" ) ){
	      /* read in the length of array */
	      int max_modifier		= fread_number(fp);
	      int counter = 0;
	      /* make sure we will never go out of bounds */
	      if (max_modifier > MAX_MODIFIER)
		nlogf("[BUG***]fread_char: Length of written \"Mods\" array was greater then current. (%s)",
		      ch->name);
	      for (counter = 0; counter < MAX_MODIFIER; counter++)
		ch->aff_mod[counter]	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Mplayed" ) )
	    {
		ch->pcdata->month 	= fread_number(fp);
		ch->pcdata->mplayed 	= fread_number(fp);
		fMatch = TRUE;
		break;
	    }
	    KEY( "Msgs",	ch->pcdata->messages,   fread_flag( fp ) );
            break;
	case 'N':
	    KEYS( "Name",	ch->name,		fread_string( fp ) );
	    if (!str_cmp(word,"Not"))
	    {
		ch->pcdata->last_note			= fread_number(fp);
                ch->pcdata->last_idea                = fread_number(fp);
		ch->pcdata->last_penalty		= fread_number(fp);
		ch->pcdata->last_news			= fread_number(fp);
		ch->pcdata->last_changes		= fread_number(fp);
		ch->pcdata->last_application		= fread_number(fp);
		fMatch = TRUE;
		break;
	    }
            if (!str_cmp(word, "NewSk"))
            {
                SKILL_DATA *nsk = new_skill();
		char *temp = fread_word( fp ) ;
                int sn = skill_lookup(temp);
                if (sn < 0)
                    bug("Fread_char: unknown new skill.",0);
                else
                    nsk->sn = sn;
                nsk->level      = fread_number( fp );
                nsk->rating     = fread_number( fp );
                nsk->learned    = fread_number( fp );
                nsk->keep       = fread_number( fp );
                nsk->next       = ch->pcdata->newskills;
                ch->pcdata->newskills   = nsk;
                fMatch = TRUE;
                break;
            }
	    KEY( "Note",	ch->pcdata->last_note,	fread_number( fp ) );
	    break;
	case 'P':
	  KEYS( "Pass",		ch->pcdata->pwd,	fread_string( fp ) );
	  KEYS( "PassDel",	ch->pcdata->pwddel,	fread_string( fp ) );
	  KEYS( "PassImm",	ch->pcdata->pwdimm,	fread_string( fp ) );
	  KEY( "Perks",		ch->pcdata->perk_bits,  fread_flag( fp )   );
	  KEY( "PLag",          ch->pcdata->pk_delay,	fread_number( fp ) );
	  KEY( "Plyd",		ch->played,		fread_number( fp ) );
	  if( !str_cmp(word, "Pos")){
	    ch->position = fread_number( fp );
	    if (ch->position == POS_FIGHTING || (ch->position == POS_MEDITATE && ch->class != class_lookup("monk")))
	      ch->position = POS_STANDING;
	    fMatch = TRUE;
	    break;
	  }
	  KEY( "Prac",	ch->practice,		fread_number( fp ) );
	  if ( !str_cmp( word, "Pr" ) ){
	    int value = fread_number( fp );
	    char *temp = fread_word( fp ) ;

	    int sn = skill_lookup(temp);
	    if ( sn < 0 ){
	      nlogf("%s, progress of: %s ",ch->name, temp);
	      bug( "Fread_char: unknown skill. ", 0 );
	    }
	    else{
	      ch->pcdata->progress[sn] = value;
	    }
	    fMatch = TRUE;
	    break;
	  }
	  KEYS( "Prom",	ch->prompt,		fread_string( fp ) );
	  if ( !str_cmp( word, "Psa" ) || !str_cmp(word,"Psalm")){
	    int value = fread_number( fp );
	    char *temp = fread_word( fp ) ;
	    int sn = psalm_lookup(temp);
	    if ( sn < 1 ){
	      nlogf("%s, psalm: %s ",ch->name, temp);
	      bug( "Fread_char: unknown psalm. ", 0 );
	    }
	    else
	      ch->pcdata->psalms[sn] = value;
	    fMatch = TRUE;
	    break;
	  }
	  KEY( "PsaPen",      ch->pcdata->psalm_pen, 	fread_number( fp ) );
	case 'Q':
	  if ( !str_cmp( word, "Quest" ) )
	    {
	      char* temp;
	      int type = QUEST_PERM;
	      temp = fread_string( fp );

	      /* REMOVE THIS LATER */
	      if (ch->save_ver > 2004){
		type = fread_number( fp );
	      }

	      add_quest(ch, temp , type);
	      free_string( temp );
	      fMatch = TRUE;
	      break;
	    }
	  break;
	case 'R':
	  if (!str_cmp(word, "Race")){
	    char *buf = fread_string( fp );
	    ch->race			=		race_lookup( buf );
	      free_string(buf);
	      fMatch = TRUE;
	      break;
	  }
	  if (!str_cmp(word, "Religion")){
	    ch->pcdata->way = fread_number( fp );
	    ch->pcdata->deity = deity_table[ch->pcdata->way].god;
	    fMatch = TRUE;
	    break;
	  }

	  KEY( "RANK",     ch->pcdata->rank,    fread_number( fp ) );
	    if ( !str_cmp( word, "Rec" ) )
	    {
		ch->pcdata->kpc = fread_number( fp );
		ch->pcdata->knpc = fread_number( fp );
		ch->pcdata->dpc = fread_number( fp );
		ch->pcdata->dnpc = fread_number( fp );
		ch->pcdata->dall = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
            KEYS( "ReRace",      ch->pcdata->race,	fread_string( fp ) );
	    KEYS( "RGuard",	 ch->pcdata->royal_guards,fread_string( fp ) );
	    KEY( "ResF",	ch->res_flags,		fread_flag( fp) | race_table[ch->race].res);
	    if ( !str_cmp( word, "Room" ) )
	    {
	      ROOM_INDEX_DATA* in_room;
	      sh_int temp = fread_number( fp );
	      in_room = get_room_index( temp );
	      if (in_room == NULL)
		in_room = get_room_index( ROOM_VNUM_CABAL_LIMBO );
	      ch->in_room = in_room;
	      ch->was_in_room = ch->in_room;
	      fMatch = TRUE;
	      if (!IS_NPC(ch))
		ch->pcdata->roomnum = temp;
	      break;
	    }
	    break;
	case 'S':
            if (!str_cmp(word,"Saves"))
            {
		ch->savingaffl		= fread_number( fp );
		ch->savingmaled		= fread_number( fp );
		ch->savingmental	= fread_number( fp );
		ch->savingbreath= fread_number( fp );
		ch->savingspell	= fread_number( fp );
                fMatch = TRUE;
                break;
	    }
	    KEY( "SaveVer",	ch->save_ver,		fread_number( fp ) );
	    KEY( "Scro",	ch->lines,		fread_number( fp ) );
            KEY( "Sec",         ch->pcdata->security,   fread_number( fp ) );
	    KEY( "Sex",		ch->sex,		fread_number( fp ) );
	    KEYS( "ShD",	ch->short_descr,	fread_string( fp ) );
	    if ( !str_cmp( word, "Skill" ) || !str_cmp(word,"Sk"))
	    {
		int value = fread_number( fp );
		char *temp = fread_word( fp ) ;
		int sn = skill_lookup(temp);
		if ( sn < 0 )
		{
		    nlogf("%s, skill: %s ",ch->name, temp);
		    bug( "Fread_char: unknown skill. ", 0 );
		}
		else{
		    ch->pcdata->learned[sn] = value;
		}
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Song" ) || !str_cmp(word,"Sg"))
	    {
		int value = fread_number( fp );
		char *temp = fread_word( fp ) ;
		int sn = song_lookup(temp);
		if ( sn < 0 )
		{
		    fprintf(stderr,"%s",temp);
		    bug( "Fread_char: unknown song. ", 0 );
		}
		else
		    ch->pcdata->songlearned[sn] = value;
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "SSpicked" ) ){
	      /* read in the length of array */
	      int max_groups		= fread_number(fp);
	      int counter = 0;
	      /* make sure we will never go out of bounds */
	      if (max_groups > MAX_GROUPS)
		nlogf("[BUG***]fread_char: Length of written \"SSpicked\" array was greater then current. (%s)",
		      ch->name);
	      for (counter = 0; counter < MAX_GROUPS; counter++)
		ch->pcdata->ss_picked[counter]	= fread_number( fp );
	      fMatch = TRUE;
	      break;
	    }
	    KEY( "StartWep",	ch->pcdata->start_wep,	fread_number( fp ) );
	    break;
	case 'T':
            KEY( "Tatt",        ch->tattoo,             fread_number( fp ) );
	    KEY( "Trai",	ch->train,		fread_number( fp ) );
	    KEY( "Tru",		ch->trust,		fread_number( fp ) );
	    if ( !str_cmp( word, "Title" )  || !str_cmp( word, "Titl"))
	    {
	      char buf[MIL];
		ch->pcdata->title = fread_string( fp );
    		if (ch->pcdata->title[0] != '.' && ch->pcdata->title[0] != ','
		&&  ch->pcdata->title[0] != '!' && ch->pcdata->title[0] != '?')
		{
		    sprintf( buf, " %s", ch->pcdata->title );
		    free_string( ch->pcdata->title );
		    ch->pcdata->title = str_dup( buf );
		}
		fMatch = TRUE;
		break;
	    }
	    KEY( "TSex",	ch->pcdata->true_sex,   fread_number( fp ) );
	    break;
	case 'V':
	    if ( !str_cmp( word, "Vnum" ) )
	    {
		ch->pIndexData = get_mob_index( fread_number( fp ) );
		fMatch = TRUE;
		break;
	    }
	    KEY( "Vote",	ch->pcdata->last_vote,	fread_number( fp ) );
	    KEY( "VSkip",	ch->pcdata->vote_skip,	fread_number( fp ) );
	    KEY( "VulnF",	ch->vuln_flags,		fread_flag( fp) | race_table[ch->race].vuln);
	    break;
	case 'W':
	    KEY( "WPos",	ch->pcdata->wep_pos,	fread_number( fp ) );
	    KEY( "Wimp",	ch->wimpy,		fread_number( fp ) );
	    KEY( "Wizn",	ch->wiznet,		fread_flag( fp ) );
	    break;
	}
	if ( !fMatch )
	{
	    bugf("Fread_char: [%s] no match.", word);
	    fread_to_eol( fp );

	}
    }
}

/* load a pet or army */
CHAR_DATA* fread_pet( CHAR_DATA *ch, FILE *fp, ROOM_INDEX_DATA* room )
{
    CHAR_DATA *pet;
    bool fMatch = FALSE;
    int lastlogoff = mud_data.current_time, percent;
    char *word = feof(fp) ? "END" : fread_word(fp);
    if (!str_cmp(word,"Vnum"))
    {
    	int vnum = fread_number(fp);
    	if (get_mob_index(vnum) == NULL)
	{
    	    bug("Fread_pet: bad vnum %d.",vnum);
	    pet = create_mobile(get_mob_index(MOB_VNUM_DUMMY));
	}
    	else
    	    pet = create_mobile(get_mob_index(vnum));
    }
    else
    {
        bug("Fread_pet: no vnum in file.",0);
        pet = create_mobile(get_mob_index(MOB_VNUM_DUMMY));
    }
    /* before restoring the pet, we strip all the effects from it */
    while (pet->affected){
      affect_remove( pet, pet->affected );
    }

    for ( ; ; )
    {
    	word 	= feof(fp) ? "END" : fread_word(fp);
    	fMatch = FALSE;
    	switch (UPPER(word[0]))
    	{
    	case '*':
    	    fMatch = TRUE;
    	    fread_to_eol(fp);
    	    break;
    	case 'A':
    	    if (!str_cmp(word,"ACs"))
    	    {
    	    	int i;
    	    	for (i = 0; i < 4; i++)
    	    	    pet->armor[i] = fread_number(fp);
    	    	fMatch = TRUE;
    	    	break;
    	    }
    	    KEY( "Act",		pet->act,		fread_flag(fp) | race_table[pet->race].act);
    	    KEY( "Act2",	pet->act2,		fread_flag(fp) | race_table[pet->race].act2);
    	    KEY( "AfBy",	pet->affected_by,	fread_flag(fp));
    	    KEY( "AfB2",	pet->affected2_by,	fread_flag(fp));
            if (!str_cmp(word,"Affc"))
            {
                AFFECT_DATA *paf = new_affect();
                int sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown skill.",0);
                else
                   paf->type = sn;
		paf->where	= fread_number(fp);
                paf->level      = fread_number(fp);
                paf->duration   = fread_number(fp);
                paf->modifier   = fread_number(fp);
                paf->location   = fread_number(fp);
                paf->bitvector  = fread_number(fp);
		if (ch->save_ver > 2005)
		  paf->flags = fread_number( fp );
		paf->has_string = fread_number( fp );
		if (paf->has_string)
		  paf->string =   fread_string( fp );
		else
		  fread_string( fp );

                paf->next       = pet->affected;
                pet->affected   = paf;
                fMatch          = TRUE;
                break;
            }
            if (!str_cmp(word,"AffD"))
            {
                AFFECT_DATA *paf = new_affect();
                int sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown skill.",0);
                else
                   paf->type = sn;
                paf->level      = fread_number(fp);
                paf->duration   = fread_number(fp);
                paf->modifier   = fread_number(fp);
                paf->location   = fread_number(fp);
                paf->bitvector  = fread_number(fp);
		paf->has_string = fread_number( fp );
		if (paf->has_string)
		  paf->string =   fread_string( fp );
		else
		  fread_string( fp );
                paf->next       = pet->affected;
                pet->affected   = paf;
                fMatch          = TRUE;
                break;
            }
            if (!str_cmp(word,"Affsong"))
            {
                AFFECT_DATA *paf = new_affect();
                int sn = song_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown song.",0);
                else
                   paf->type = sn;
		paf->where	= fread_number(fp);
                paf->level      = fread_number(fp);
                paf->duration   = fread_number(fp);
                paf->modifier   = fread_number(fp);
                paf->location   = fread_number(fp);
                paf->bitvector  = fread_number(fp);
                paf->next       = pet->affected2;
                pet->affected2   = paf;
                fMatch          = TRUE;
                break;
            }
            KEY( "Alig",        pet->alignment,         fread_number(fp));
            if (!str_cmp(word,"AMod"))
            {
                int stat;
                for (stat = 0; stat < (MAX_STATS-1); stat++)
                    pet->mod_stat[stat] = fread_number(fp);
                fMatch = TRUE;
                break;
            }
            if (!str_cmp(word,"Attr"))
            {
                int stat;
                for (stat = 0; stat < (MAX_STATS-1); stat++)
                    pet->perm_stat[stat] = fread_number(fp);
                fMatch = TRUE;
                break;
            }
            break;
        case 'C':
	  if (!str_cmp(word, "Cab")){
	    char *buf = fread_string( fp );
	    pet->pCabal			=		get_cabal( buf );
	    free_string(buf);
	    fMatch = TRUE;
	    break;
	  }
	  KEY( "Comm",       pet->comm,              fread_flag(fp));
	  break;
        case 'D':
            KEY( "Dam",        pet->damroll,           fread_number(fp));
            KEY( "DamType",    pet->dam_type,          fread_number(fp));
            KEYS( "Desc",      pet->description,       fread_string(fp));
            break;
        case 'E':
	  if (word != NULL && !str_cmp(word,"End"))
            {
	      if (pet->pIndexData->vnum != MOB_VNUM_MONSTER){
		pet->leader = ch;
		pet->master = ch;
	      }

	      if (IS_SET(pet->act, ACT_PET))
		ch->pet = pet;
	      pet->summoner = ch;
	      lastlogoff = pet->logoff;

	      percent = (mud_data.current_time - lastlogoff) * 25 / ( 2 * 60 * 60);
	      if (percent > 0 && !IS_AFFECTED(ch,AFF_POISON) && !IS_AFFECTED(ch,AFF_PLAGUE))
		{
		  percent = UMIN(percent,100);
		  pet->hit    += (pet->max_hit - pet->hit) * percent / 100;
		  pet->mana   += (pet->max_mana - pet->mana) * percent / 100;
		  pet->move   += (pet->max_move - pet->move)* percent / 100;
		}
	    }
	  if (room && pet->in_room != NULL){
	    if (pet->in_room)
	      char_from_room( pet );
	    char_to_room(pet, room);
	    act("$n materializes!.",pet,NULL,NULL,TO_ROOM);
	  }
	  return pet;
	  break;
	  KEY( "Exp",        pet->exp,               fread_long(fp));
	  break;
	case 'G':
	  KEY( "Gold",       pet->gold,              fread_long(fp));
	  break;
	case 'H':
	  KEY( "Hit",        pet->hitroll,           fread_number(fp));
	  if (!str_cmp(word,"HMV"))
	    {
	      pet->hit        = fread_number(fp);
	      pet->max_hit    = fread_number(fp);
	      pet->mana       = fread_number(fp);
	      pet->max_mana   = fread_number(fp);
	      pet->move       = fread_number(fp);
	      pet->max_move   = fread_number(fp);
	      fMatch = TRUE;
	      break;
	    }
	  break;
       case 'L':
            KEY( "Levl",       pet->level,             fread_number(fp));
            KEYS( "LnD",       pet->long_descr,        fread_string(fp));
            KEY( "LogO",       pet->logoff,            fread_long(fp));
            break;
	case 'M':
	    if ( !str_cmp( word, "Mods" ) ){
	      /* read in the length of array */
	      int max_modifier		= fread_number(fp);
	      int counter = 0;
	      /* make sure we will never go out of bounds */
	      if (max_modifier > MAX_MODIFIER)
		nlogf("[BUG***]fread_char: Length of written \"Mods\" array was greater then current. (%s)",
		      pet->name);
	      for (counter = 0; counter < MAX_MODIFIER; counter++)
		pet->aff_mod[counter]	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    break;
       case 'N':
            KEYS( "Name",       pet->name,              fread_string(fp));
            break;
       case 'O':
            KEY( "Off",        pet->off_flags,         fread_flag(fp));
	    break;
       case 'P':
	    if( !str_cmp(word, "Pos"))
	    {
		pet->position = fread_number( fp );
		if (pet->position == POS_FIGHTING || pet->position == POS_MEDITATE)
		    pet->position = POS_STANDING;
		fMatch = TRUE;
		break;
	    }
	    KEY( "Prac",	pet->practice,		fread_number( fp ) );
            break;
       case 'R':
	    if (!str_cmp(word, "Race")){
	      char *buf = fread_string( fp );
	      pet->race			=		race_lookup( buf );
	      free_string(buf);
	      fMatch = TRUE;
	      break;
	    }
	    if ( !str_cmp( word, "Room" ) ){
	      ROOM_INDEX_DATA* in_room = NULL;

	      sh_int temp = fread_number( fp );
	      in_room = get_room_index( temp );
	      if (in_room == NULL)
		in_room = get_room_index( ROOM_VNUM_CABAL_LIMBO );
	      char_to_room( pet, in_room);
	      pet->was_in_room = pet->in_room;
	      fMatch = TRUE;
	      if (!IS_NPC(pet))
		pet->pcdata->roomnum = temp;
	      break;
	    }
	    break;
	case 'S' :
	 KEY( "SaveVer",	pet->save_ver,	fread_number( fp ) );
	 if (!str_cmp(word,"Saves"))
	   {
	     pet->savingaffl		= fread_number( fp );
	     pet->savingmaled	= fread_number( fp );
	     pet->savingmental	= fread_number( fp );
	     pet->savingbreath	= fread_number( fp );
	     pet->savingspell	= fread_number( fp );
	     fMatch = TRUE;
	     break;
	   }
	 KEY( "Sex",         pet->sex,               fread_number(fp));
	 KEYS( "ShD",        pet->short_descr,       fread_string(fp));
	 KEY( "Status",	     pet->status,	     fread_number( fp ) );
	 break;
	case 'T' :
	   KEY( "Trai",	pet->train,		fread_number( fp ) );
           break;
       }
       if ( !fMatch )
       {
           bug("Fread_pet: no match.",0);
           fread_to_eol(fp);
       }
    }
}



void fread_null( CHAR_DATA *ch, FILE *fp )
{
    char *word;
    bool fDesc = FALSE;
    for ( ; ; )
    {
      word = feof( fp ) ? "End" : fread_word( fp );
      if (!str_cmp(word, "Desc") && !fDesc){
	char *buf;
	buf = fread_string( fp );
	free_string(buf);
	fDesc = TRUE;
      }
      if (word != NULL && !str_cmp(word,"End"))
	    return;
	else
	    fread_to_eol( fp );
    }
}

extern OBJ_DATA        *obj_free;

OBJ_DATA* fread_obj( CHAR_DATA *ch, FILE *fp, ROOM_INDEX_DATA* pRoom )
{
    OBJ_DATA *obj = NULL;
    int iNest, load_room = 0, trash = 0, idle = 0;
    bool fMatch = FALSE, fNest, fVnum = FALSE, first = TRUE, new_format = FALSE, make_new = FALSE;
    char *word = feof( fp ) ? "End" : fread_word( fp );

    /* Useless conditional */
    if( trash != 0 )
        trash = 0;

    if (!str_cmp(word,"Vnum" ))
    {
        int vnum = fread_number( fp );
        first = FALSE;
        if (  get_obj_index( vnum )  == NULL )
            bug( "Fread_obj: bad vnum %d.", vnum );
        else
	{
	    if (ch)
                remove_limit(vnum);
            obj = create_object(get_obj_index(vnum),-1);
	    new_format = TRUE;
	    obj->recall = 0;
	}
    }
    if (obj == NULL)
    {
    	obj = new_obj();
    	obj->name		= str_dup( "" );
    	obj->short_descr	= str_dup( "" );
    	obj->description	= str_dup( "" );
    }
    fNest		= FALSE;
    fVnum		= TRUE;
    iNest		= 0;
    for ( ; ; )
    {
	if (first)
	    first = FALSE;
	else
	    word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;
	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;
	case 'A':
            if (!str_cmp(word,"Affc"))
            {
                AFFECT_DATA *paf = new_affect();
                int sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_obj: unknown skill.",0);
                else
                paf->type = sn;
		paf->where	= fread_number( fp );
                paf->level      = fread_number( fp );
                paf->duration   = fread_number( fp );
                paf->modifier   = fread_number( fp );
                paf->location   = fread_number( fp );
                paf->bitvector  = fread_number( fp );
		if (obj->save_ver > 2005 || obj->save_ver < 2000)
		  paf->flags = fread_number( fp );
		paf->has_string = fread_number( fp );
		if (paf->has_string)
		  paf->string =   fread_string( fp );
		else
		fread_string( fp );
                paf->next       = obj->affected;
                obj->affected   = paf;
                fMatch          = TRUE;
                break;
            }
	    if (!str_cmp(word,"AffD"))
	    {
                AFFECT_DATA *paf = new_affect();
                int sn = skill_lookup(fread_word(fp));
		if (sn < 0)
		    bug("Fread_obj: unknown skill.",0);
		else
		    paf->type = sn;
		paf->level	= fread_number( fp );
		paf->duration	= fread_number( fp );
		paf->modifier	= fread_number( fp );
		paf->location	= fread_number( fp );
		paf->bitvector	= fread_number( fp );
		paf->has_string = fread_number( fp );
		if (paf->has_string)
		  paf->string =   fread_string( fp );
		else
		  fread_string( fp );
		paf->next	= obj->affected;
		obj->affected	= paf;
		fMatch		= TRUE;
		break;
	    }
	    if (!str_cmp(word,"Affs"))
	    {
	        OBJ_SPELL_DATA *sp = new_obj_spell();
                int sn = skill_lookup(fread_word(fp));
		if (sn < 0)
		    bug("Fread_obj: unknown skill.",0);
		else
		    sp->spell = sn;
		sp->target	= fread_number( fp );
		sp->percent	= fread_number( fp );
		sp->message	= fread_string( fp );
		sp->message2	= fread_string( fp );
		sp->next	= obj->spell;
		obj->spell	= sp;
		fMatch		= TRUE;
		break;
	    }
	    break;
	case 'C':
	  if (!str_cmp(word, "Cabal")){
	    char *buf = fread_string( fp );
	    obj->pCabal			=		get_cabal( buf );
	    free_string( buf );
	    fMatch = TRUE;
	    break;
	  }
	  KEY( "Cla",	ch->class,		fread_number( fp ) );
	  KEY( "Class",	obj->class,		fread_number( fp ) );
	  KEY( "Cond",	obj->condition,		fread_number( fp ) );
	  KEY( "Cost",	obj->cost,		fread_number( fp ) );
	  break;
	case 'D':
	    KEY( "Desc",	obj->description,	fread_string( fp ) );
	    break;
	case 'E':
	    if ( !str_cmp( word, "Enchanted"))
	    {
		obj->enchanted = TRUE;
	 	fMatch 	= TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Eldritched"))
	    {
		obj->eldritched = TRUE;
	 	fMatch 	= TRUE;
		break;
	    }
	    if ( word != NULL && (!str_cmp( word, "End" ) || !str_cmp( word, "#End" )) )
	    {
                if ( !fNest || ( fVnum && obj->pIndexData == NULL ) )
                {
		    bug( "Fread_obj: incomplete object.", 0 );
		    free_obj(obj);
		    rgObjNest[iNest] = NULL;
                    return NULL;
		}
		else
		{
                    if ( !fVnum )
                    {
			free_obj(obj);
                        obj = create_object( get_obj_index( OBJ_VNUM_DUMMY ), 0 );
                    }
		    if (!new_format)
		    {
		    	obj->next	= object_list;
		    	object_list	= obj;
		    	obj->pIndexData->count++;
		    }
		    if (!obj->pIndexData->new_format  && obj->item_type == ITEM_ARMOR  && obj->value[1] == 0)
		    {
			obj->value[1] = obj->value[0];
			obj->value[2] = obj->value[0];
		    }
		    if (make_new)
		    {
			int wear = obj->wear_loc;
			extract_obj(obj);
                        obj = create_object(obj->pIndexData,0);
			obj->wear_loc = wear;
		    }
		    if (load_room != 0){
		      ROOM_INDEX_DATA *to_room = pRoom ? pRoom : get_room_index(load_room);
		      if (to_room)
			obj_to_room( obj, to_room );
		      else
			extract_obj( obj );
		    }
                    else if ( ch && (iNest == 0 || rgObjNest[iNest-1] == NULL ) )
			obj_to_char( obj, ch );
		    else
			obj_to_obj( obj, rgObjNest[iNest-1] );
		    obj->idle = idle;
		    return obj;
		}
	    }
            if (!str_cmp(word,"ExDe"))
	    {
		EXTRA_DESCR_DATA *ed = new_extra_descr();
		ed->keyword		= fread_string( fp );
		ed->description		= fread_string( fp );
		ed->next		= obj->extra_descr;
		obj->extra_descr	= ed;
		fMatch = TRUE;
	    }
	    KEY( "ExtF",	obj->extra_flags,	fread_number( fp ) );
	    break;
	case 'H':
	    KEY( "Home",	obj->homevnum,		fread_number( fp ) );
	    break;
	case 'I':
	    KEY( "Idle",	idle,			fread_number( fp ) );
	    KEY( "Ityp",	obj->item_type,		fread_number( fp ) );
	    break;
	case 'L':
	    KEY( "Lev",		obj->level,		fread_number( fp ) );
	    KEY( "Limited",     trash,			fread_number( fp ) );
	    break;
	case 'N':
	    KEY( "Name",	obj->name,		fread_string( fp ) );
	    if ( !str_cmp( word, "Nest" ) )
	    {
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		    bug( "Fread_obj: bad nest %d.", iNest );
		else
		{
		    rgObjNest[iNest] = obj;
		    fNest = TRUE;
		}
		fMatch = TRUE;
	    }
	    break;
   	case 'O':
	    if ( !str_cmp( word,"Oldstyle" ) )
	    {
		if (obj->pIndexData != NULL && obj->pIndexData->new_format)
		    make_new = TRUE;
		fMatch = TRUE;
	    }
	    KEY( "Own",		obj->owner,		fread_number( fp ) );
	    break;
	case 'R':
	    KEY( "Race",	obj->race,		fread_number( fp ) );
	    KEY( "Rec",		obj->recall,		fread_number( fp ) );
	    KEY( "Room",	load_room,		fread_number( fp ) );
	    break;
	case 'S':
	  KEY( "SaveVer",	obj->save_ver,		fread_number( fp ) );
	    KEY( "ShD",		obj->short_descr,	fread_string( fp ) );
	    if ( !str_cmp( word, "Spell" ) )
	    {
		int iValue = fread_number( fp );
		int sn = skill_lookup( fread_word( fp ) );
		if ( iValue < 0 || iValue > 4 )
		    bug( "Fread_obj: bad iValue %d.", iValue );
		else if ( sn < 0 )
		    bug( "Fread_obj: unknown skill.", 0 );
		else
		    obj->value[iValue] = sn;
		fMatch = TRUE;
		break;
	    }
	    break;
	case 'T':
	    KEY( "Time",	obj->timer,		fread_number( fp ) );
	    if ( !str_cmp( word, "Trap" ) ){
	      int vnum	=	fread_number( fp );
	      TRAP_INDEX_DATA* pTrapIndex = get_trap_index( vnum );

	      if (pTrapIndex == NULL){
		bug("fread_obj: could not load trap index vnum: %d", vnum);
		continue;
	      }
	      else{
		TRAP_DATA* pTrap = create_trap( pTrapIndex, NULL);
		int i = 0;
		int max = 0;

		/* get the rest of info */
		if (!IS_NULLSTR( pTrap->name ))
		  free_string( pTrap->name );
		pTrap->name	=	fread_string( fp );
		pTrap->armed	=	fread_number( fp );
		pTrap->age	=	fread_number( fp );
		pTrap->type	=	fread_number( fp );
		pTrap->level	=	fread_number( fp );
		pTrap->duration	=	fread_number( fp );
		pTrap->flags	=	fread_flag( fp );

		max = fread_number( fp );

		for (i = 0; i < max && i < MAX_TRAPVAL; i++){
		  pTrap->value[i] = fread_number( fp );
		}

		/* trap is all ready, we slap it onto the object */
		trap_to_obj( pTrap, obj );

		fMatch = TRUE;
		break;
	      }
	    }
	    break;
	case 'V':
	    if ( !str_cmp( word, "Val" ) )
	    {
		obj->value[0] 	= fread_number( fp );
	 	obj->value[1]	= fread_number( fp );
	 	obj->value[2] 	= fread_number( fp );
		obj->value[3]	= fread_number( fp );
		obj->value[4]	= fread_number( fp );
		fMatch = TRUE;
		break;
	    }
            if (!str_cmp(word,"Vals"))
	    {
		obj->value[0]	= fread_number( fp );
		obj->value[1]	= fread_number( fp );
		obj->value[2]	= fread_number( fp );
		obj->value[3]	= fread_number( fp );
		if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
		   obj->value[0] = obj->pIndexData->value[0];
		fMatch		= TRUE;
		break;
	    }
	    if ( !str_cmp( word, "VirVnum" ) )
	    {
	      OBJ_INDEX_DATA* pIndex = NULL;
		int vnum = fread_number( fp );
		if ( ( pIndex = get_obj_index( vnum ) ) == NULL )
		  bug( "Fread_obj: bad virvnum %d.", vnum );
		else
		  obj->vnum = pIndex->vnum;
		fMatch = TRUE;
		break;
	    }
	    if ( !str_cmp( word, "Vnum" ) )
	    {
		int vnum = fread_number( fp );
		if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL )
		    bug( "Fread_obj: bad vnum %d.", vnum );
		else
		    fVnum = TRUE;
		fMatch = TRUE;
		break;
	    }
	    break;
	case 'W':
	    KEY( "WeaF",	obj->wear_flags,	fread_number( fp ) );
	    KEY( "Wear",	obj->wear_loc,		fread_number( fp ) );
	    KEY( "Wt",		obj->weight,		fread_number( fp ) );
	    break;
	}
	if ( !fMatch )
	{
	    bug( "Fread_obj: no match.", 0 );
	    fread_to_eol( fp );
	}
    }
    return NULL;
}

void save_limits(void)
{
    FILE *fd;
    CHAR_DATA *ch;
    OBJ_DATA *obj2;

/* close reserve file */
    fclose(fpReserve);

    fd = fopen (LIMITTEMP_FILE, "w");
    fprintf( fd, "%d\n", mud_data.pfiles);
    for ( obj2 = get_room_index(ROOM_VNUM_LIMIT)->contents; obj2 != NULL; obj2 = obj2->next_content )
	if (IS_LIMITED(obj2) && obj2->item_type!=ITEM_CABAL)
            fprintf( fd, "%d\n", obj2->pIndexData->vnum);
    for ( ch = player_list; ch != NULL; ch = ch->next_player)
    {
        OBJ_DATA *obj;
        for ( obj = ch->carrying; obj != NULL; obj = obj->next_content)
	    if (IS_LIMITED(obj) && obj->item_type!=ITEM_CABAL)
                fprintf( fd, "%d\n",obj->pIndexData->vnum);
    }
    fprintf (fd, "0\n");
    fclose (fd);
    rename(LIMITTEMP_FILE,LIMIT_FILE);
    fpReserve = fopen( NULL_FILE, "r" );
}

void load_limits(void)
{
    FILE *fd;
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    int obj_vnum;
    fd = fopen (LIMIT_FILE, "r");
    if (fd == NULL)
      return;
    mud_data.pfiles = fread_number(fd);
    for ( ; ; )
    {
        obj_vnum = fread_number(fd);
        if (obj_vnum == 0)
            break;
        if ( (pObjIndex=get_obj_index(obj_vnum)) !=NULL)
	{
            obj = create_object(pObjIndex,0);
            obj_to_room(obj,get_room_index(ROOM_VNUM_LIMIT));
	}
    }
    fclose (fd);
}

void remove_limit( int vnum )
{
    OBJ_DATA *obj2;
    for ( obj2 = get_room_index(ROOM_VNUM_LIMIT)->contents; obj2 != NULL; obj2 = obj2->next_content)
        if (obj2->pIndexData->vnum == vnum)
        {
	    if ( obj2->in_room == NULL )
	    {
	        bug( "Limit Code: Object %d not in limit room.", vnum );
	        return;
	    }
	    obj_from_room(obj2);
            extract_obj( obj2);
	}
}


//04-23-00 VIRI: Added support for GEN's in save_char_obj
//04-24-00 Viri: Changed default status of ->extracted for PC's to FALSE from true, as to make the exrtaction and release of player data more bug proof.
//05-02-00 Viri: Added save support for the "string" parts of effects.
//05-09-00 Viri: Added support for saveing and loading of the deity name.
//05-10-00 Viri: Added support for saveing and loading spells on items.
//06-25-00 Viri: Line 438 Added if to clean up lvl 50 test eq. (removed)
//01-02-01 Ath: Added "application" type notes
//01-08-01 Ath: Added any "holdsrare" item counted as rare item when saving
