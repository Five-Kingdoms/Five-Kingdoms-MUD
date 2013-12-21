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


/* save version for mud file */
#define SAVEMUD_VER		1000

/* directory and files */
#define SAVEMUD_DIR        "../savemud/"        /* mud/cabal etc. files */
#define SAVEMUD_FILE	  "mud.sav"		/* mud info */


/* main structure for mud data, delcared in merc.h */
struct mud_type  mud_data;

/* macros */
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


/* functions */
void	save_mud	();
void	read_mud	();
void	reset_mdata	();
