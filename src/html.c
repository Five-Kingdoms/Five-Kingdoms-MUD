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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "magic.h"
#include "s_skill.h"


/* Written by: Virigoth
 *   returns: length of string						*
 *   comment: Takes single character, returns html string in buffer	*/
int char_to_html(char color, char* dest){
   char code[25];
   char* p;

   switch (color){
   case '\0': code[0] = '\0';					break;
   case ' ': sprintf( code, " " );				break;
   case '`': sprintf( code, "</font>" );			break;
   case '<': sprintf( code, "&lt" );				break;
   case '>': sprintf( code, "&gt" );				break;
   case '1': sprintf( code, "<font color=""#8B0000"">" );	break;
   case '!': sprintf( code, "<font color=""#FF0000"">" );	break;
   case '2': sprintf( code, "<font color=""#006400"">" );	break;
   case '@': sprintf( code, "<font color=""#00FF00"">" );	break;
   case '3': sprintf( code, "<font color=""#B4B400"">" );	break;
   case '#': sprintf( code, "<font color=""#FFFF00"">" );	break;
   case '4': sprintf( code, "<font color=""#00008B"">" );	break;
   case '$': sprintf( code, "<font color=""#0000FF"">" );	break;
   case '5': sprintf( code, "<font color=""#8B008B"">" );	break;
   case '%': sprintf( code, "<font color=""#FF00FF"">" );	break;
   case '6': sprintf( code, "<font color=""#008B8B"">" );	break;
   case '^': sprintf( code, "<font color=""#00FFFF"">" );	break;
   case '7': sprintf( code, "<font color=""#808080"">" );	break;
   case '&': sprintf( code, "<font color=""#FFFFFF"">" );	break;
   case '8': sprintf( code, "<font color=""#A9A9A9"">" );	break;
   default:  sprintf( code, "<font color=""#006400"">" );	break;
   }

   /* copy code to dest */
   p = code;
   while( *p != '\0' ) {
     *dest = *p++;
     *++dest = '\0';
   }
   return( strlen( code ) );
}

/* Written by: Virigoth
 *   returns: void		     	*
 *   comment: Converts a string to html if fSpace then spaces are coverted *
 * to hard coded spaces */
int string_to_html(char* source, char* Dest, bool fSpace){
   char* p;
   char *dest = Dest;
   int skip = 0;
   int iChange = 0;	//tracks how many /fonts to put

   /* parse th string, look for color codes and /n/r's */
   for( p = source ; *p ; p++ ){
     
     /* test for color code */
     if( *p == '`' ){
       p++;
       if( *p == '\0' )
	 p--;
       else{
	 if ( (*p >= '1' && *p <= '9')
	      || (*p >= '!' && *p <= '*'))
	   iChange ++;
	 else if (*p == '`')
	   iChange --;
	 skip = char_to_html( *p, dest );
	 while( skip-- > 0 )++dest;
	 continue;
       }
     }
     /* check for /n, /rest is ignored */
     /*
     if (*p == '/' && *(p + 1) != '\0'){
       if (*(p+1) == 'n'){
	 *dest++ = '<';
	 *dest++ = 'B';
	 *dest++ = 'R';
	 *dest++ = '>';
	 }
	 }*/
     if (*p == '\r'){
       *dest++ = '<';
       *dest++ = 'B';
       *dest++ = 'R';
       *dest++ = '>';
     }
     else if (*p == ' ' && fSpace){
       *dest++ = '&';
       *dest++ = 'n';
       *dest++ = 'b';
       *dest++ = 's';
       *dest++ = 'p';
       *dest++ = ';';
     }
     else if (*p == '<'){
       *dest++ = '&';
       *dest++ = 'l';
       *dest++ = 't';
       *dest++ = ';';
     }
     else if (*p == '>'){
       *dest++ = '&';
       *dest++ = 'g';
       *dest++ = 't';
       *dest++ = ';';
     }
     else
       *dest++ = *p;
     *dest = '\0';
   }
   /* make sure we are ok with colors */
   if (!iChange)
     *dest = '\0';
   else{
   for (; iChange > 0; iChange --)
     strcat(dest, "</font>");
   }
   return( strlen( dest ) );
 }
   

       

       

/* Written by: Virigoth
 *   returns: void							*
 *   comment: runs thruogh list of players, generates a html who_list   *
 *   "fShow = FALSE" skips characters with invis level above invis spell*
 *   cabal tags shown only if "autocabal" is on, imms with visi. level  *
 *  over 1 not shown							*/

void make_htmlwho(bool fShow){
  FILE *fp;   

  DESCRIPTOR_DATA* d;

  char buf[2*MSL]; 
  char buf2[2*MSL]; 

  int count = 0;

  /* housekeeping */
  buf[0] = '\0';  
  buf2[0] = '\0';    
  fclose(fpReserve);

  /* get file pointer */
  sprintf( buf, "%s%s", HTML_DIR, WHO_HTML_FILE);
  if ( (fp = fopen(buf, "w") ) == NULL){
    bug("Error creating who html file", 0);
    fclose (fp);
    return;
  }

  /* GENERATE HEADER */
  fprintf(fp, "<HTML>\n<HEAD>\n<TITLE> Aabahran: The Forsaken Lands - Who is online?"\
	  "</TITLE>\n<SCRIPT LANGUAGE=\"JavaScript\" src=\"http://www.popuptraffic.com"\
	  "/assign.php?l=shadowspawn&mode=behind\"></script>\n</HEAD>\n");
  fprintf(fp, "<BODY TEXT=\"#A9A9A9\" BGCOLOR=\"black\" LINK=\"#00FFFF\""\
	  "VLINK=\"#FFFFFF\" ALINK=\"#008080\" FONT FACE= \"Arial\">\n");  
  fprintf(fp, "<LEFT>\n");
  fprintf(fp, "<h1><center>Aabahran: The Forsaken Lands - Who is online?"\
	  "</center></h1>\n<PRE>\n");  

  /* start running through the characters */
  for ( d = descriptor_list; d != NULL ; d = d->next ){
    CHAR_DATA* wch;
    char const *class;
    if (d->connected != CON_PLAYING)
      continue;
    count ++;
    if (!d->character)
      continue;
    if (IS_NPC(d->character))
      continue;
    wch = d->character;

    if ( !fShow &&
	 (IS_AFFECTED(wch, AFF_HIDE)
	  || wch->invis_level > LEVEL_HERO 
	  || IS_AFFECTED2(wch, AFF_CAMOUFLAGE) 
	  || IS_AFFECTED2(wch,AFF_SHADOWFORM)
	  || IS_AFFECTED2(wch,AFF_TREEFORM)
	  || IS_SET(wch->act2,PLR_NOWHO)
	  || is_affected(wch, gen_d_shroud)) )
      continue;

    class = class_table[wch->class].who_name;
    switch ( wch->level )
      {
      default: break;
	{
	case MAX_LEVEL - 0 : class = "IMP";     break;
	case MAX_LEVEL - 1 : class = "CRE";     break;
	case MAX_LEVEL - 2 : class = "SUP";     break;
	case MAX_LEVEL - 3 : class = "DEI";     break;
	case MAX_LEVEL - 4 : class = "GOD";     break;
	case MAX_LEVEL - 5 : class = "IMM";     break;
	case MAX_LEVEL - 6 : class = "DEM";     break;
	case MAX_LEVEL - 7 : class = "ANG";     break;
	case MAX_LEVEL - 8 : class = "AVA";     break;
	case MAX_LEVEL - 9 : class = "HER";     break;
	}
      }

    if (IS_IMMORTAL(wch))
      sprintf( buf, "[%2d %5s %s] ",
	       wch->level, 
	       wch->race < MAX_PC_RACE ? 
	       IS_QRACE(wch) ? wch->pcdata->race : 
	       pc_race_table[is_affected(wch, gen_unlife) ? 
			    race_lookup("undead") : wch->race].who_name 
	       : "     ", class);
    else
      {
	//We do a small change here for MASQUED vamps.
	if (IS_SET(wch->act2, PLR_MASQUERADE))
	  sprintf( buf, "[   %5s    ] ", 
		   wch->race < MAX_PC_RACE ? pc_race_table[race_lookup("human")].who_name : "     ");
	else
	  sprintf( buf, "[   %5s    ] ", 
		   wch->race < MAX_PC_RACE ? 
		   IS_QRACE(wch) ? wch->pcdata->race : 
		   pc_race_table[is_affected(wch, gen_unlife) ? 
				race_lookup("undead") : wch->race].who_name :
		   "     ");
      }//end ELSE
    string_to_html(buf, buf2, FALSE);
    fprintf(fp, buf2);

    if (wch->pCabal && IS_GAME(wch, GAME_SHOW_CABAL) && !IS_IMMORTAL(wch)){
      sprintf( buf, "[%s]", wch->pCabal->who_name);
      string_to_html(buf, buf2, FALSE);
      fprintf(fp, buf2);
    }      
    if (IS_SET(wch->act2, PLR_CODING)){
      sprintf( buf, "`3{`#CODING`3}`` ");
      string_to_html(buf, buf2, FALSE);
      fprintf(fp, buf2);
    }
    //NOBLE and ROYAL added here.
    if ( IS_SET(wch->act2, PLR_ROYAL)){
      sprintf( buf, "`3<`#ROYAL`3>`` ");
      string_to_html(buf, buf2, FALSE);
      fprintf(fp, buf2);
    }
    if (IS_SET(wch->act2, PLR_NOBLE)){
      sprintf( buf, "`3<`!NOBLE`3>`` ");
      string_to_html(buf, buf2, FALSE);
      fprintf(fp, buf2);
    }
    sprintf( buf, "%s%s%s%s%s%s%s\n",
	     IS_SET(wch->comm, COMM_AFK) ? "[AFK] " : "",  
	     IS_SET(wch->act, PLR_WANTED) ? "(`1WANTED``) " : "", 
	     IS_SET(wch->act, PLR_OUTCAST) ? "(`1OUTCAST``) " : "", 
	     wch->name,
	     wch->pcdata->family[0] == '\0' ? "" : " ",  
	     wch->pcdata->family[0] == '\0' ? "" : wch->pcdata->family, 
	     IS_NPC(wch) ? "" : wch->pcdata->title );
    string_to_html(buf, buf2, FALSE);
    fprintf(fp, buf2);
  }// END for
  
  if (mud_data.max_on == count)
    fprintf(fp, "\nThere %s %d character%s on, the most so far since startup.\n",
	    count == 1 ? "is" : "are", count,count == 1 ? "" : "s" );
  else
    fprintf(fp, "\nThere %s %d character%s on, the most on since startup was %d.\n",
	    count == 1 ? "is" : "are", count,count == 1 ? "" : "s", mud_data.max_on );

/* FINISH THE LIST */
  fprintf(fp, "</PRE>\n");
  fprintf(fp, "<font face=""Times New Roman""><center>\n");
  sprintf(buf, "File last updated at %s Western Time.\n", ((char *) ctime( &mud_data.current_time )));
  fprintf(fp, buf);  fprintf(fp, "</LEFT></FONT>\n");
  fprintf(fp, "<br><br>\n");
  fprintf(fp, "<CENTER><P><A HREF=""forsakenlands.org/ogre2.html"" TARGET=""ogre2""><Font Size=+1>\n");
  fprintf(fp, "Back</A> </P></CENTER></Font>\n");
  fprintf(fp, "</body>\n");  fprintf(fp, "</html>\n");  
  fclose( fp ); 
  fpReserve = fopen( NULL_FILE, "r" );
}
    
/* Written by: Virigoth
 *   returns: pointer to last Help found, NULL on fail			*
 *   comment: copies the text of help found under keyword into dest.    */

HELP_DATA* get_help_txt( char* key, char* dest ){
    HELP_DATA *pHelp;
    HELP_DATA *found = NULL;
    dest[0] = '\0';

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
      /* do not show limited helps */
      if ( pHelp->level != HELP_ALL)
	continue;

      if ( is_name( key, pHelp->keyword ) ){
	if (found)
	  strcat(dest,"\n\r============================================================\n\r\n\r");
	if ( pHelp->text[0] == '.' )
	  strcat(dest,pHelp->text+1);
	else
	  strcat(dest,pHelp->text);
	found = pHelp;
      }
    }

    return found;

}

/* Written by: Virigoth
 *   returns: void							
 *   comment: Generates index for all skill helps, as well as resets all the help files (A to Z)
 *   with proper header.
 */
void reset_helps(){
  FILE *fp;
  int i = 0;

  /* reset all the helps first */
  for (i = 'A'; i <= 'Z'; i++){
    int j = 0;
    char file[MIL];
    char buf[MIL];
    fclose(fpReserve);

    sprintf(file, "%sskill_%c_help.html", HTML_DIR, i);
    /* get file pointer */
    if ( (fp = fopen(file, "w") ) == NULL){
      bug("reset_helps: Error opening dbase file", 0);
      return;
    }
    /* write in the header */
    fprintf( fp, "<HTML>\n<HEAD>\n"\
	     "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=windows-1252\">\n"\
	     "<META NAME=\"Generator\" CONTENT=\"Microsoft Word 97\">\n");
    fprintf( fp, "<TITLE>Skill and spell related help for the Forsaken Lands</TITLE>\n"\
	     "</HEAD>\n<BODY LINK=\"#0000ff\" VLINK=\"#800080\" BGCOLOR=\"#000000\">\n<CENTER>\n");
    fprintf(fp, "<B><FONT FACE=\"Arial\" SIZE=6 COLOR=\"#ff0000\"><P>Selected Skills and Spells for the Forsaken Lands</P>\n");
    fprintf( fp, "</B></FONT><FONT FACE=\"Arial\" SIZE = 2 COLOR=\"#c0c0c0\"><P>(Complete help file database is only avaliable while logged onto Forsaken Lands.) </P>\n");
    fprintf( fp, "</B></FONT><FONT FACE=\"Arial\" SIZE = 2 COLOR=\"#c0c0c0\"><P>All text within "\
	     "is property of the Forsaken Lands. Any unauthorized use is protected by applicable laws.</P>\n");
    sprintf(buf, "File last updated at %s Western Time.</P>\n", ((char *) ctime( &mud_data.current_time )));	     
    fprintf(fp , buf);
    fprintf( fp, "</CENTER>");
    /* create index of entries for other files */
    fprintf( fp, "</FONT><B><FONT FACE=\"Arial\" SIZE=2 COLOR=\"#c0c0c0\"><P><A NAME=\"index\"></A>Index:</P>\n");
    for (j = 'A'; j <= 'Z'; j++){
      sprintf(buf, "<A HREF=\"skill_%c_help.html\">%c</A> ", j, j);
      fprintf( fp, buf);
    }
    fprintf( fp, "\n<BR> <BR>\n");

    /* Done with header */
    fclose( fp ); 
    fpReserve = fopen( NULL_FILE, "r" );
  }
}


/* Written by: Virigoth
 *   returns: void							
 *   comment: takes a keyword, opens appropriate dbase help file, and writes the help text	*
 *   with correct formatting and coloring into it. (append)					*/

void write_help(char *key){
  HELP_DATA* pHelp;
  char buf[4*MSL]; 

  if (!key || !key[0])
    return;

  /* housekeeping */
  buf[0] = '\0';  
  fclose(fpReserve);

  /* search for help on keyword */
  if ( (pHelp = get_help_txt(key, buf)) != NULL){
    FILE *fp;   
    char file[MIL];
    char buf2[4*MSL]; 

    /* get the help file we will write to */
    sprintf(file, "%sskill_%c_help.html", HTML_DIR, isdigit(key[0]) ? 'A' : UPPER(key[0]));

    /* get file pointer */
    if ( (fp = fopen(file, "a") ) == NULL){
      bug("Error creating html dbase file", 0);
      return;
    }

    /* convert to html */
    string_to_html(buf, buf2, FALSE);
    /* Generate link for future use */
    fprintf (fp, "<A NAME=\"%s\"></A><BR>", key);
    fprintf( fp, "</FONT><FONT FACE=\"Arial\" SIZE=2 COLOR=\"#c0c0c0\">\n");
    /* put the string into the help file */
    fprintf( fp, buf2);
    fprintf(fp, "</FONT><P><A HREF=\"#index\"><FONT FACE=\"Arial\" SIZE = 1 COLOR=\"#008000\">(Back to Top)</A></P>\n");
    fclose( fp ); 
  }
  fpReserve = fopen( NULL_FILE, "r" );
}

/* Written by: Virigoth
 *   returns: void							
 *   comment: Generates an html race help file */

void race_help(int race){
  FILE *fp;   
  char* name = pc_race_table[race].name;
  char file[MIL];
  char buf[MSL];
  bool fQuest = TRUE;

  if (race == -1){
    name = "avatar";
  }
  else if (race  < 0 || race > MAX_PC_RACE)
    return;
  /* get path for file being rewritten */
  sprintf(file, "%s%s_help.html", HTML_DIR, name);

  /* try to open the file for rewritre using the reserve*/
  fclose(fpReserve);
  
  /* get file pointer */
  if ( (fp = fopen(file, "w") ) == NULL){
    bug("race_help: Error opening help file", 0);
    return;
  }
  /* Check if this is a restricted race somehow */
  if (race != race_lookup("vampire")
      && race != race_lookup("undead")
      && race != race_lookup("demon")
      && race != race_lookup("lich")
      && race != race_lookup("avatar")){
    fQuest = FALSE;
  }

  /* start writing into the file */
  if (race == -1 || fQuest){
    fprintf(fp, "<P><B><FONT SIZE=4>%s</FONT> <FONT SIZE = 3>&nbsp;Penalty: ? ? ?</B></P>\n\r", 
	    capitalize(name));
  }
  else{
    fprintf(fp, "<P><B><FONT SIZE=4>%s</FONT> <FONT SIZE = 3>&nbsp;Penalty: %d</B>\n\r", 
	    capitalize(name), pc_race_table[race].class_mult - 1500);
    /* print attributes */
    fprintf( fp, "<BR><FONT SIZE=2>" );
    fprintf( fp, "Str: %-2d  Int: %-2d  Wis: %-2d  Dex: %-2d  Con: %-2d\n\r",
	     pc_race_table[race].max_stats[STAT_STR],
	     pc_race_table[race].max_stats[STAT_INT],
	     pc_race_table[race].max_stats[STAT_WIS],
	     pc_race_table[race].max_stats[STAT_DEX],
	     pc_race_table[race].max_stats[STAT_CON]
	     );
    fprintf( fp, "</FONT></P>");
  }
  
  /* Begin actual help here */
  if (get_help_txt(name, buf)){
    char buf2[MSL];
    buf2[0] = '\0';
    fprintf( fp, "<FONT FACE=\"Arial\">\n");
    string_to_html(buf, buf2, FALSE);
    fprintf( fp, buf2);
  }


  fprintf(fp, "<center><br>\n");
  sprintf(buf, "File last updated at %s Western Time.\n", ((char *) ctime( &mud_data.current_time )));
  fprintf( fp, buf );
  fprintf(fp, "</center>\n");

  /* cleanup */
  fclose( fp ); 
  fpReserve = fopen( NULL_FILE, "r" );
}

//prints help for a religion
void religion_help(int reli){
  FILE *fp;   
  OBJ_INDEX_DATA* tat;
  char* way  = deity_table[reli].way;
  char* path  = path_table[deity_table[reli].path].name;
  char* diety = deity_table[reli].god;
  int align = deity_table[reli].align;
  int etho = deity_table[reli].ethos;
  char file[MIL];
  char buf[MSL], buf2[MSL];


  if (reli  < 0 || reli > MAX_DIETY)
    return;

  /* get path for file being rewritten */
  sprintf(file, "%sreligion_%s_help.html", HTML_DIR, way);

  /* try to open the file for rewritre using the reserve*/
  fclose(fpReserve);
  
  /* get file pointer */
  if ( (fp = fopen(file, "w") ) == NULL){
    bug("religion_help: Error opening help file", 0);
    return;
  }
  /* heading of the help */
  sprintf( buf, 
	   "%s->%s\n\r"\
	   "Diety: %s",
	   path,
	   way,
	   diety
	   );
  if ( (tat = get_obj_index( deity_table[reli].tattoo)) != NULL){
    strcat( buf, " (Tattoo of ");
    strcat( buf, tat->short_descr );
    strcat( buf, ")\n\r" );
  }
  else
    strcat( buf, "\n\r" );
  strcat( buf, "Aligns: " );
  if (IS_SET( align, ALIGN_GOOD))
    strcat( buf, "Good " );
  if (IS_SET( align, ALIGN_NEUTRAL))
    strcat( buf, "Neutral " );
  if (IS_SET( align, ALIGN_EVIL))
    strcat( buf, "Evil" );

  strcat( buf, "\n\rEthos: " );

  if (IS_SET( etho, ETHOS_LAWFUL))
    strcat( buf, "Lawful " );
  if (IS_SET( etho, ETHOS_NEUTRAL))
    strcat( buf, "Neutral " );
  if (IS_SET( etho, ETHOS_CHAOTIC))
    strcat( buf, "Chaotic" );
  strcat( buf, "\n\r" );

  buf2[0] = 0;
  string_to_html(buf, buf2, FALSE);
  fprintf(fp, "<B><FONT SIZE=3>%s</FONT></B><BR><BR>\n", buf2 );
  
  /* Begin actual help here */
  if (get_help_txt(way, buf)){
    char buf2[MSL];
    buf2[0] = '\0';
    fprintf( fp, "<FONT FACE=\"Arial\">\n");
    string_to_html(buf, buf2, FALSE);
    fprintf( fp, buf2);
  }

  fprintf(fp, "<center><br>\n");
  sprintf(buf, "File last updated at %s Western Time.\n", ((char *) ctime( &mud_data.current_time )));
  fprintf( fp, buf );
  fprintf(fp, "</center>\n");

  /* cleanup */
  fclose( fp ); 
  fpReserve = fopen( NULL_FILE, "r" );
}

/* Written by: Virigoth
 *   returns: void							
 *   comment: Generates an html cabal help file */

void cabal_help(int cabal){
  FILE *fp; 
  CABAL_INDEX_DATA* pc;
  char file[MIL];
  char buf[MSL];
  char buf2[MSL];
    

  if ( (pc = get_cabal_index( cabal )) == NULL)
    return;

  /* get path for file being rewritten */
  sprintf(file, "%s%s_help.html", HTML_DIR, capitalize(pc->name));

  /* try to open the file for rewritre using the reserve*/
  fclose(fpReserve);
  
  /* get file pointer */
  if ( (fp = fopen(file, "w") ) == NULL){
    bug("cabal_help: Error opening help file", 0);
    return;
  }

/* start the help */
  sprintf( buf, "[%s]", pc->who_name );
  buf2[0] = 0;
  string_to_html(buf, buf2, FALSE);
  fprintf(fp, "<B><FONT SIZE=4>%s</FONT></B><BR><BR>\n", buf2 );
  
  /* Begin actual help here */
  if (get_help_txt(pc->name, buf)){
    buf2[0] = '\0';
    fprintf( fp, "<FONT FACE=\"Arial\">\n");
    string_to_html(buf, buf2, FALSE);
    fprintf( fp, buf2);
  }
  fprintf(fp, "<B><FONT SIZE=2>%s</FONT></B>\n", 
	  "For current application criteria consult online help files."
	  );

  fprintf(fp, "<center><br>\n");
  sprintf(buf, "File last updated at %s Western Time.\n", ((char *) ctime( &mud_data.current_time )));
  fprintf( fp, buf );
  fprintf(fp, "</center>\n");

  /* cleanup */
  fclose( fp ); 
  fpReserve = fopen( NULL_FILE, "r" );
}

/* Written by: Virigoth
 *   returns: void							
 *   comment: Generates a table of skills and spells for a given class */
void class_help(int class){
    FILE *fp;   
    char* name = class_table[class].name;
    char file[MIL];
    char buf[MSL];
    int level = 0;
    bool fQuest = TRUE;

    if (class  < 0 || class > MAX_CLASS || IS_NULLSTR(name))
      return;
    /* get path for file being rewritten */
    sprintf(file, "%s%s_help.html", HTML_DIR, name);

    /* try to open the file for rewritre using the reserve*/
    fclose(fpReserve);

    /* get file pointer */
    if ( (fp = fopen(file, "w") ) == NULL){
      bug("class_help: Error opening help file", 0);
      return;
    }
    /* Check if this is a restricted class somehow */
    if (class != class_lookup("vampire")
	&& class != class_lookup("crusader")
	&& class != class_lookup("lich")
	&& class != class_lookup("psionicist")){
      fQuest = FALSE;
    }

    /* start writing into the file */
    if (fQuest){
      fprintf(fp, "<B><FONT SIZE=4>%s</FONT> <FONT SIZE = 3>&nbsp;Penalty: ??? <P></B>\n", 
	      capitalize(name));
    }
    else{
      fprintf(fp, "<B><FONT SIZE=4>%s</FONT> <FONT SIZE = 3>&nbsp;Penalty: %d <P></B>\n", 
	      capitalize(name), class_table[class].extra_exp);
    }

    /* Begin actual help here */
    /* Start with help file */
    if (get_help_txt(name, buf)){
      char buf2[MSL];
      buf2[0] = '\0';
      fprintf( fp, "<FONT FACE=\"Arial\">\n");
      string_to_html(buf, buf2, FALSE);
      fprintf( fp, buf2);
    }

    /* Check if this is a restricted class somehow */
    if (!fQuest){
      char buf3[MSL];
      char skill_list[LEVEL_IMMORTAL][MSL];
      char skill_columns[LEVEL_IMMORTAL];
      char spell_list[LEVEL_IMMORTAL][MSL];
      char spell_columns[LEVEL_IMMORTAL];
      bool fSpell = FALSE;
      bool fSkill = FALSE;
      int sn = 0;
      buf3[0] = '\0';
      /* cleanup */
      for (level = 0; level < LEVEL_IMMORTAL; level++){
	  skill_columns[level] = 0;
	  skill_list[level][0] = '\0';
	  spell_columns[level] = 0;
	  spell_list[level][0] = '\0';
	}
      /* Regular stuff for skill/spell tables */
      for (sn = 0; sn < MAX_SKILL; sn++){
        if (skill_table[sn].name == NULL )
	  continue;
        if (!skill_table[sn].name[0])
	  continue;
	/* skip cabal skills and such*/
	if (skill_table[sn].skill_level[0] == 69)
	  continue;
	/* skip skills not auto learned */
	if (skill_table[sn].rating[class] > 9 || skill_table[sn].rating[class] < 1)
	  continue;
	level = skill_table[sn].skill_level[class];
	if  (level > LEVEL_IMMORTAL || level < 1)
	  continue;

	/* Skill entry */
	if (skill_table[sn].spell_fun == spell_null){
	  fSkill = TRUE;
	  sprintf(buf3,"<A HREF=\"skill_%c_help.html#%s\">%s</A>\t\t", 
		  isdigit(skill_table[sn].name[0]) ? 'A' : 
		  UPPER(skill_table[sn].name[0]),
		  skill_table[sn].name, 
		  skill_table[sn].name);


	  if (skill_list[level][0] == '\0')
	    sprintf(skill_list[level],"<TH> %2d </TH> <TD> <PRE>%s", level, buf3);
	  else
	    {
	      if ( ++skill_columns[level] % 2 == 0)
		strcat(skill_list[level],"<BR>");
	      strcat(skill_list[level],buf3);
	    }
	}
	/* Spell entry */
	else{
	  fSpell = TRUE;
	  sprintf(buf3,"<A HREF=\"skill_%c_help.html#%s\">%-18s (%d) </A>", 
		  isdigit(skill_table[sn].name[0]) ? 'A' : 
		  UPPER(skill_table[sn].name[0]),
		  skill_table[sn].name, 
		  skill_table[sn].name,
		  skill_table[sn].min_mana);


	  if (spell_list[level][0] == '\0')
	    sprintf(spell_list[level],"<TH> %2d </TH> <TD><PRE>%s ",level,buf3);
	  else
	    {
	      if ( ++skill_columns[level] % 2 == 0)
		strcat(spell_list[level],"<BR>");
	      strcat(spell_list[level],buf3);
	    }
	}//END spell
      }//END spell/SKill loop.
    
      if (HAS_SS(class)){
	int ss = 0;
	/* put in each group of ss's at their level */
	if ( (ss = ss_lookup(class_table[class].ss)) != 0){
	  int i = 0;
	  int picks = 0;
	  /* check if we even have selections at this level */
	  for (i = 0; ss_table[ss].level > 0 && i < MAX_SELECT; i++){
	    int level = ss_table[ss].level[i];
	  
	    if ( (picks = ss_table[ss].picks[i]) == 0
		 || IS_NULLSTR(ss_table[ss].group[i]))
	      break;
	    fSkill = TRUE;
	    sprintf(buf3,"<A HREF=\"skill_%c_help.html#%s\">%s</A>\t\t", 
		    isdigit(ss_table[ss].group[i][0]) ? 'A' : 
		    UPPER(ss_table[ss].group[i][0]),
		    ss_table[ss].group[i], 
		    ss_table[ss].group[i]);
	    if (skill_list[level][0] == '\0')
	      sprintf(skill_list[level],"<TH> %2d </TH> <TD> <PRE>%s",level,buf3);
	    else
	      {
		if ( ++skill_columns[level] % 2 == 0)
		  strcat(skill_list[level],"<BR>");
		strcat(skill_list[level],buf3);
	      }
	  }
	}
      }
      
      /* Print skill / spells out */
      /* Table formatting and heading */
      fprintf( fp, "<BR><BR><TABLE BORDER = \"1\" WIDTH = \"50%%\">\n");
      
      fprintf( fp, "<COLGROUP>\n"\
	       "<COL ALIGN = \"top\">\n"\
	       "<COL SPAN = \"2\" ALIGN = \"left\">\n"\
	       "<COLGROUP>\n");
      fprintf( fp, "<THEAD>\n"\
	       "<TR VALIGN = \"top\">\n"\
	       "<TH> </FONT> <FONT COLOR = \"FF000\" SIZE = 3>Level</TH>\n"\
	       "<TH> </FONT> <FONT COLOR = \"FF000\" SIZE = 4>Skills </TH>\n"\
	       "</TR>\n"\
	       "</THEAD>\n"\
	       "<TBODY>\n");

      if (!fSkill){
	fprintf( fp, "</TBODY>\n</TABLE>\n\n");
	fprintf( fp, "No Skills Found.<BR>\n");
      }
      else{
	for (level = 0; level < LEVEL_IMMORTAL; level++){
	  if (skill_list[level][0] != '\0'){
	    string_to_html(skill_list[level], buf3, FALSE);
	    fprintf( fp, "<TR>\n");
	    fprintf(fp, skill_list[level]);
	    fprintf( fp, "</PRE></TD></TR>\n");
	  }
	}
	fprintf( fp, "</TBODY>\n</TABLE>\n\n");
      }

      /* Table formatting and heading */
      fprintf( fp, "<BR><BR><TABLE BORDER = \"1\" WIDTH = \"50%%\">\n");

      fprintf( fp, "<COLGROUP>\n"\
	       "<COL ALIGN = \"center\">\n"\
	       "<COL SPAN = \"2\" ALIGN = \"left\">\n"\
	       "<COLGROUP>\n");
      fprintf( fp, "<THEAD>\n"\
	       "<TR VALIGN = \"bottom\">\n"\
	       "<TH> </FONT> <FONT COLOR = \"FF000\">Level</TH>\n"\
	       "<TH> </FONT> <FONT COLOR = \"FF000\" SIZE = 4>Spells</TH>\n"\
	       "</TR>\n"\
	       "</THEAD>\n"\
	       "<TBODY>\n");

      if (!fSpell){
	fprintf( fp, "</TBODY>\n</TABLE>\n\n");
	fprintf( fp, "No Spell Found.<BR>\n");
      }
      else{
	for (level = 0; level < LEVEL_IMMORTAL; level++){
	  if (spell_list[level][0] != '\0'){
	    string_to_html(spell_list[level], buf3, FALSE);
	    fprintf( fp, "<TR>\n");
	    fprintf(fp, spell_list[level]);
	    fprintf( fp, "\n</PRE></TD></TR>\n");
	  }
	}
	fprintf( fp, "</TBODY>\n</TABLE>\n\n");
      }
    }//END normal classes

    fprintf( fp, "<BR><BR>\n");
    /* Footer */
    fprintf( fp, "</B><FONT FACE=\"Arial\" SIZE = 2><P>All text within "\
	     "is property of the Forsaken Lands. Any unauthorized use is protected by applicable laws.</P>\n");
    sprintf(buf, "File last updated at %s Western Time.</P>\n", ((char *) ctime( &mud_data.current_time )));	     
    fprintf(fp , buf);
    
    /* cleanup */
    fclose( fp ); 
    fpReserve = fopen( NULL_FILE, "r" );
}



/* Written by: Virigoth
 *   returns: void							
 *   comment: Generates complete data base of all help files for skills/spells in skill_table */
void make_html_helps(){
  int sn = 0;
  CABAL_INDEX_DATA* pCab;

  /* reset all the helps first */
  reset_helps();

  /* begin running through complete skill_table and generate helps */
  for (sn = 0; sn < MAX_SKILL; sn ++){
    if ( skill_table[sn].name == NULL ) 
      continue;         
    write_help(skill_table[sn].name);
  }

  /* generate select skill group helps */
  for (sn = 0; sn < MAX_GROUPS; sn ++){
    if ( ss_group_table[sn].name == NULL ) 
      continue;         
    write_help(ss_group_table[sn].name);
  }

  /* Now generate Class based helps */
  for (sn = 0; sn  < MAX_CLASS; sn++){
    if (class_table[sn].name && class_table[sn].name[0]){
      class_help(sn);
    }
  }

  /* Now generate race based helps */
  for (sn = 0; sn  < MAX_PC_RACE; sn++){
    if (pc_race_table[sn].name && pc_race_table[sn].name[0]){
      race_help(sn);
    }
  }
  /* do avatar race help */
  race_help(-1);

  /* Now generate cabal helps */
  for (pCab = cabal_index_list; pCab != NULL; pCab = pCab->next){
    cabal_help(pCab->vnum);
  }

  /* religions */
  for (sn = 0; sn  < MAX_DIETY; sn++){
    if (deity_table[sn].way && deity_table[sn].way[0]){
      religion_help(sn);
    }
  }
}

/* generates the database for areas asp script */
void make_area_db(){
  FILE *fp;   
  AREA_DATA* pa;
  char buf[MSL]; 
  char name[MIL];

  fclose(fpReserve);
  /* get file pointer */
  sprintf( buf, "%s%s", HTML_DIR, AREA_HTML_FILE);
  if ( (fp = fopen(buf, "w") ) == NULL){
    bug("Error creating area html file", 0);
    fclose (fp);
    return;
  }
  /* run through all areas print out fields to db */

  for (pa = area_first; pa; pa = pa->next){
    char* string;
    char* dest;
    if (IS_SET(pa->area_flags, AREA_RESTRICTED))
      continue;

    /* strip mud color from the area name */
    string = pa->name;
    dest = name;
    while (*string != '\0'){
      if (isalpha(*string)){
	*dest = *string;
	dest++;
      }
      string++;
    }
    *dest++ = '\0';
    fprintf(fp , "%s\n", name);
    fprintf(fp , "%s\n", pa->pCabal ? pa->pCabal->name : "none");
    fprintf(fp , "%d\n", pa->bastion_current);
    fprintf(fp , "%d\n", pa->bastion_max);
    /* last is for foot note or some other text */
    fprintf(fp, "%s\n", "none");
  }
  fclose( fp ); 
  fpReserve = fopen( NULL_FILE, "r" );
}
