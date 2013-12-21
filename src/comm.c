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

/* This file contains all of the OS-dependent stuff:                  *
 *   OBstartup, signals, BSD sockets for tcp/ip, i/o, timing.           *
 * The data flow for input is:                                        *
 *    Game_loop ---> Read_from_descriptor ---> Read                   *
 *    Game_loop ---> Read_from_buffer                                 *
 * The data flow for output is:                                       *
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write *
 * The OS-dependent functions are Read_from_descriptor                *
 *    and Write_to_descriptor.                                        */

/* This turns hot_reboot for crashes on and off -DY */
#define signal_catching

/* This turns core dumping for crashes on and off -DY */
#define core_dumping

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include "merc.h"
#include "recycle.h"
#include "interp.h"
#include "ansi.h"
#include "vote.h"
#include "cabal.h"
#include "comm.h"

extern char *color_table[];
static char * const his_her [] = { "its", "his", "her" };

/* Malloc debugging stuff. */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern  int     malloc_debug    args( ( int  ) );
extern  int     malloc_verify   args( ( void ) );
#endif

/* Signal handling. Apollo has a problem here */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(apollo)
#undef __attribute
#endif

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "telnet.h"
#define replace_string( pstr, nstr ) { free_string( (pstr) ); pstr=str_dup( (nstr) ); }
#if !defined( STDOUT_FILENO )
#define STDOUT_FILENO 1
#endif
const   char    echo_off_str    [] = { IAC, WILL, TELOPT_ECHO, '\0' };
const   char    echo_on_str     [] = { IAC, WONT, TELOPT_ECHO, '\0' };
const   char    go_ahead_str    [] = { IAC, GA, '\0' };

/* OS-dependent declarations. */
#if     defined(_AIX)
#include <sys/select.h>
int     accept          args( ( int s, struct sockaddr *addr, int *addrlen ) );
int     bind            args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero           args( ( char *b, int length ) );
int     getpeername     args( ( int s, struct sockaddr *name, int *namelen ) );
int     getsockname     args( ( int s, struct sockaddr *name, int *namelen ) );
int     gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
int     listen          args( ( int s, int backlog ) );
int     setsockopt      args( ( int s, int level, int optname, void *optval,int optlen ) );
int     socket          args( ( int domain, int type, int protocol ) );
#endif

#if     defined(apollo)
#include <unistd.h>
void    bzero           args( ( char *b, int length ) );
#endif

#if     defined(__hpux)
int     accept          args( ( int s, void *addr, int *addrlen ) );
int     bind            args( ( int s, const void *addr, int addrlen ) );
void    bzero           args( ( char *b, int length ) );
int     getpeername     args( ( int s, void *addr, int *addrlen ) );
int     getsockname     args( ( int s, void *name, int *addrlen ) );
int     gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
int     listen          args( ( int s, int backlog ) );
int     setsockopt      args( ( int s, int level, int optname, const void *optval, int optlen ) );
int     socket          args( ( int domain, int type, int protocol ) );
#endif

#if     defined(interactive)
#include <net/errno.h>
#include <sys/fnctl.h>
#endif

#if     defined(linux)
/*  Linux shouldn't need these.  But if so, uncomment these *
int     accept          args( ( int s, struct sockaddr *addr, int *addrlen ) );
int     bind            args( ( int s, struct sockaddr *name, int namelen ) );
pid_t   fork            args( ( void ) );
*/
/*begin of porting stuff -DY*/
// asdfg pid_t   wait            args( ( int *status ) );
int     pipe            args( ( int filedes[2] ) );
pid_t   fork            args( ( void ) );
int     dup2            args( ( int oldfd, int newfd ) );
int     execl           args( ( const char *path, const char *arg, ... ) );
/*end of porting stuff -DY*/
int     close           args( ( int fd ) );
// asdfg int     getpeername     args( ( int s, struct sockaddr *name, int *namelen ) );
// asdfg int     getsockname     args( ( int s, struct sockaddr *name, int *namelen ) );
int     gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ));
// asdfg int     listen          args( ( int s, int backlog ) );
int     read            args( ( int fd, char *buf, int nbyte ) );
int     select          args( ( int width, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout ) );
int     socket          args( ( int domain, int type, int protocol ) );
int     write           args( ( int fd, char *buf, int nbyte ) );
#endif

#if     defined(MIPS_OS)
extern  int             errno;
#endif

#if     defined(NeXT)
int     close           args( ( int fd ) );
int     fcntl           args( ( int fd, int cmd, int arg ) );
#if     !defined(htons)
u_short htons           args( ( u_short hostshort ) );
#endif
#if     !defined(ntohl)
u_long  ntohl           args( ( u_long hostlong ) );
#endif
int     read            args( ( int fd, char *buf, int nbyte ) );
int     select          args( ( int width, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout ) );
int     write           args( ( int fd, char *buf, int nbyte ) );
#endif

#if     defined(sequent)
int     accept          args( ( int s, struct sockaddr *addr, int *addrlen ) );
int     bind            args( ( int s, struct sockaddr *name, int namelen ) );
int     close           args( ( int fd ) );
int     fcntl           args( ( int fd, int cmd, int arg ) );
int     getpeername     args( ( int s, struct sockaddr *name, int *namelen ) );
int     getsockname     args( ( int s, struct sockaddr *name, int *namelen ) );
int     gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
#if     !defined(htons)
u_short htons           args( ( u_short hostshort ) );
#endif
int     listen          args( ( int s, int backlog ) );
#if     !defined(ntohl)
u_long  ntohl           args( ( u_long hostlong ) );
#endif
int     read            args( ( int fd, char *buf, int nbyte ) );
int     select          args( ( int width, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout ) );
int     setsockopt      args( ( int s, int level, int optname, caddr_t optval, int optlen ) );
int     socket          args( ( int domain, int type, int protocol ) );
int     write           args( ( int fd, char *buf, int nbyte ) );
#endif

/* This includes Solaris Sys V as well */
#if defined(sun)
pid_t   wait            args( ( int *status ) );
pid_t   fork            args( ( void ) );
int     kill            args( ( pid_t pid, int sig ) );
int     pipe            args( ( int filedes[2] ) );
int     dup2            args( ( int oldfd, int newfd ) );
int     execl           args( ( const char *path, const char *arg, ... ) );
int     accept          args( ( int s, struct sockaddr *addr, int *addrlen ) );
int     bind            args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero           args( ( char *b, int length ) );
int     close           args( ( int fd ) );
int     getpeername     args( ( int s, struct sockaddr *name, int *namelen ) );
int     getsockname     args( ( int s, struct sockaddr *name, int *namelen ) );
int     gettimeofday    args( ( struct timeval *tp, void * ) );
int     listen          args( ( int s, int backlog ) );
int     read            args( ( int fd, char *buf, int nbyte ) );
int     select          args( ( int width, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout ) );
/*#if defined(SYSV)
int setsockopt          args( ( int s, int level, int optname, const char *optval, int optlen ) );
#else
int     setsockopt      args( ( int s, int level, int optname, void *optval, int optlen ) );
#endif*/
int     socket          args( ( int domain, int type, int protocol ) );
int     write           args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int     accept          args( ( int s, struct sockaddr *addr, int *addrlen ) );
int     bind            args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero           args( ( char *b, int length ) );
int     close           args( ( int fd ) );
int     getpeername     args( ( int s, struct sockaddr *name, int *namelen ) );
int     getsockname     args( ( int s, struct sockaddr *name, int *namelen ) );
int     gettimeofday    args( ( struct timeval *tp, struct timezone *tzp ) );
int     listen          args( ( int s, int backlog ) );
int     read            args( ( int fd, char *buf, int nbyte ) );
int     select          args( ( int width, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout ) );
int     setsockopt      args( ( int s, int level, int optname, void *optval, int optlen ) );
int     socket          args( ( int domain, int type, int protocol ) );
int     write           args( ( int fd, char *buf, int nbyte ) );
#endif

/* Global variables. */
DESCRIPTOR_DATA *   descriptor_list;    /* All open descriptors         */
DESCRIPTOR_DATA *   d_next;             /* Next descriptor in loop      */
FILE *              fpReserve;          /* Reserved file handle         */
bool                god;                /* All new chars are gods!      */
bool                merc_down;          /* Shutdown                     */
char                last_malloc[MSL];
char                last_loop[MSL];
char                glob_str_buf[MIL];
pid_t		    pid;
int		    control;
int		    reboot_act = 0;

/* prototypes */
void set_race_stats( CHAR_DATA* ch, int race );
void set_start_skills( CHAR_DATA* ch, int class );
void set_race_skills(CHAR_DATA* ch, int value);

void my_sig_handler( int signum)
{
    int status;
    pid_t pid;
    if ( ( pid = wait(&status) ) <0)
        perror("Ident error\n");
}

int main( int argc, char **argv )
{
    struct timeval now_time;
    bool fHotReboot = FALSE;
    /* Memory debugging if needed. */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif
    /*char buf[100];
    if( fgets( buf, 100, stdin ) != buf )
        return 0;*/


    /* Init time. */
    gettimeofday( &now_time, NULL );
    mud_data.current_time = (time_t) now_time.tv_sec;
    strcpy( mud_data.str_boot_time, ctime( &mud_data.current_time ) );
    /* Reserve one channel for our use. */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }
    /* Get the port number. */
    mud_data.mudport = 4000;
    if ( argc > 1 )
    {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( mud_data.mudport = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    exit( 1 );
	}
        if (argv[2] && argv[2][0])
        {
	    fHotReboot = TRUE;
	    control = atoi(argv[3]);
        }
        else
	{
	    fHotReboot = FALSE;
	    mud_data.crash_time = mud_data.current_time;
	    mud_data.max_on = 0;
	}
	if (argc > 3){
	  char arg[MIL];
	  argv[4] = one_argument(argv[4], arg);
	  while (arg[0]){
	    /* ROOM COLOR */
	    if (!str_cmp(arg, "color"))
	      SET_BIT(reboot_act, REBOOT_COLOR);
	    /* RARE PURGE */
	    else if (!str_cmp(arg, "purge"))
	      SET_BIT(reboot_act, REBOOT_PURGE);
	    /* OLD PFILE WIPE */
	    else if (!str_cmp(arg, "wipe"))
	      SET_BIT(reboot_act, REBOOT_WIPE)
		/* COUNT PFILES */;
	    else if (!str_cmp(arg, "count"))
	      SET_BIT(reboot_act, REBOOT_COUNT);
	    /* generate helps */
	    else if (!str_cmp(arg, "helps"))
	      SET_BIT(reboot_act, REBOOT_HELPS);
	    /* generate stats on race/class/cabal */
	    else if (!str_cmp(arg, "stats"))
	      SET_BIT(reboot_act, REBOOT_CABAL);
	    else if (!str_cmp(arg, "all")){
	      SET_BIT(reboot_act, REBOOT_CABAL);
	      SET_BIT(reboot_act, REBOOT_HELPS);
	      SET_BIT(reboot_act, REBOOT_COUNT);
	      SET_BIT(reboot_act, REBOOT_WIPE);
	      SET_BIT(reboot_act, REBOOT_PURGE);
	      SET_BIT(reboot_act, REBOOT_COLOR);
	      break;
	    }
	    argv[4] = one_argument(argv[4], arg);
	  }
	}
    }

    /* Run ROM. */
    pid = getpid();
    if (!fHotReboot) /* We have already the port if Hot Rebooted */
	control = init_socket (mud_data.mudport);
    boot_db (fHotReboot);
    init_signals( );
    install_other_handlers( );
    nlogf( "The Forsaken Lands is ready to rock on port %d.", mud_data.mudport );
    game_loop_unix( control );
    close (control);
    /* The end. */
    log_string( "Normal termination of The Forsaken Lands." );
    exit( 0 );
    return 0;
}

void sig_handler(int sig)
{
    switch(sig)
    {
    case SIGBUS:
        bug("CRASH: Sig handler SIGBUS.",0);
#if defined(core_dumping)
	dump_core("SIGBUS");
#endif
#if defined(signal_catching)
	do_hotreboot(NULL, "1");
#endif
        break;
    case SIGTERM:
        bug("CRASH: Sig handler SIGTERM.",0);
#if defined(core_dumping)
        dump_core("SIGTERM");
#endif
#if defined(signal_catching)
	do_hotreboot(NULL, "2");
#endif
        break;
    case SIGABRT:
        bug("CRASH: Sig handler SIGABRT",0);
#if defined(core_dumping)
        dump_core("SIGABRT");
#endif
#if defined(signal_catching)
	do_hotreboot(NULL, "3");
#endif
        break;
    }
}

void init_signals()
{
    log_string( "Initializing catchable signal handlers..." );
    signal(SIGBUS,sig_handler);
    signal(SIGTERM,sig_handler);
    signal(SIGABRT,sig_handler);
}

int init_socket( int port )
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;
    int fd;
    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	exit( 1 );
    }
    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof(x) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
	close(fd);
	exit( 1 );
    }
/*#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct  linger  ld;
	ld.l_onoff  = 1;
	ld.l_linger = 1000;
	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    close(fd);
	    exit( 1 );
	}
    }
#endif*/
    sa              = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port     = htons( port );
    //sa.sin_addr.s_addr = inet_addr("204.209.44.31");
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
    {
	perror("Init socket: bind" );
	close(fd);
	exit(1);
    }
    if ( listen( fd, 10 ) < 0 )
    {
	perror("Init socket: listen");
	close(fd);
	exit(1);
    }
    return fd;
}

void init_malloc ( char *argument )
{
    char *strtime = ctime( &mud_data.current_time );
  if (!LOG_MALLOC)
    return;
    strtime[strlen(strtime)-1] = '\0';
    sprintf (last_malloc, "Malloc: %s (%s)", argument, strtime );
}

void end_malloc ( char *argument )
{
  char *strtime;

  if (!LOG_MALLOC)
    return;
    strtime                    = ctime( &mud_data.current_time );
    strtime[strlen(strtime)-1] = '\0';
    sprintf (last_malloc, "Malloc: (Finished) %s (%s)", argument, strtime );
}

void init_loop ( char *argument )
{
    char *strtime;
    strtime                    = ctime( &mud_data.current_time );
    strtime[strlen(strtime)-1] = '\0';
    sprintf (last_loop, "Loop: %s (%s)", argument, strtime );
    if (fLogLoop)
      nlogf(last_loop, "Loop: %s (%s)", argument, strtime );
}

void end_loop ( char *argument )
{
    char *strtime;
    strtime                    = ctime( &mud_data.current_time );
    strtime[strlen(strtime)-1] = '\0';
    sprintf (last_loop, "Loop: (Finished) %s (%s)", argument, strtime );
    if (fLogLoop)
      nlogf(last_loop, "Loop: (Finished) %s (%s)", argument, strtime );
}

/* Write last command */
void write_last_command ()
{
    FILE *fd;
    if (!last_command[0] && !last_mprog[0] && !last_malloc[0] && !last_loop[0])
        return;
    fd = fopen (CRASH_FILE, "a");
    if (fd == NULL)
      return;
    fprintf (fd, "\n=========================================================================\n");
    fprintf (fd, "%s", last_malloc);
    fprintf (fd, "\n");
    fprintf (fd, "%s", last_loop);
    fprintf (fd, "\n");
    fprintf (fd, "%s", last_command);
    fprintf (fd, "\n");
    fprintf (fd, "%s", last_mprog);
    fclose (fd);
}

void nasty_signal_handler (int no)
{
    write_last_command();
    bug("CRASH: Sig handler SIGSEGV",0);
    log_string(last_malloc);
    log_string(last_loop);
    log_string(last_command);
    log_string(last_mprog);
#if defined(core_dumping)
    dump_core("SIGSEGV");
#endif
#if defined(signal_catching)
    do_hotreboot(NULL, "4");
#endif
    return;
}

/* Call this before starting the game_loop */
void install_other_handlers ()
{
    log_string( "Initializing other signal handlers..." );
    last_malloc [0] = '\0';
    last_loop [0] = '\0';
    last_command [0] = '\0';
    last_mprog [0] = '\0';
    if (atexit (write_last_command) != 0)
    {
        perror ("install_other_handlers:atexit");
        exit (1);
    }
    /* should probably check return code here */
    signal (SIGSEGV, nasty_signal_handler);
}

bool read_from_ident( int fd, char *buffer )
{
    static char inbuf[MSL*2];
    int iStart = strlen( inbuf ), i, j, k;
    if ( iStart >= sizeof( inbuf ) - 10 )
    {
	log_string( "Ident input overflow!!!" );
	return FALSE;
    }
    for ( ; ; )
    {
	int nRead = read( fd, inbuf + iStart, sizeof( inbuf ) - 10 - iStart );
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( inbuf[iStart-2] == '\n' || inbuf[iStart-2] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    return FALSE;
	}
	else if ( errno == EWOULDBLOCK )
	    break;
	else
	{
	    perror( "Read_from_ident" );
	    return FALSE;
	}
    }
    inbuf[iStart] = '\0';
    for ( i = 0; inbuf[i] != '\n' && inbuf[i] != '\r'; i++ )
	if ( inbuf[i] == '\0' )
	    return FALSE;
    for ( i = 0, k = 0; inbuf[i] != '\n' && inbuf[i] != '\r'; i++ )
    {
	if ( inbuf[i] == '\b' && k > 0 )
	    --k;
	else if ( isascii( inbuf[i] ) && isprint( inbuf[i] ) )
	    buffer[k++] = inbuf[i];
    }
    if ( k == 0 )
	buffer[k++] = ' ';
    buffer[k] = '\0';
    while ( inbuf[i] == '\n' || inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( inbuf[j] = inbuf[i+j] ) != '\0'; j++ );
    return TRUE;
}

void process_ident( DESCRIPTOR_DATA *d )
{
    char buffer[MIL], address[MIL];
    CHAR_DATA *ch=CH( d );
    char *user = NULL;
    int status;
    /* Useless conditional */
    if( user != NULL )
        user = NULL;
    sh_int results = 0;
    buffer[0]='\0';
    if ( !read_from_ident( d->ifd, buffer ) || IS_NULLSTR( buffer ) )
    	return;
    user = first_arg( buffer, address, FALSE );
    replace_string( d->ident, d->host );
    if ( !IS_NULLSTR( address ) )
    {
        replace_string( d->host, address );
        SET_BIT( results, 1 );
    }
    if ( results < 1 || results > 3 )
        nlogf( "%s could not be identified.", ch->name );
    close( d->ifd );
    d->ifd=-1;
    waitpid( d->ipid, &status, WNOHANG );
    d->ipid=-1;
}

void create_ident( DESCRIPTOR_DATA *d, long ip, int port )
{
    int fds[2];
    pid_t pid;
    if ( pipe( fds )!=0 )
    {
        perror( "Create_ident: pipe: " );
        return;
    }
    if ( (pid=fork( ))>0 )
    {
    	d->ifd=fds[0];
    	close( fds[1] );
    	d->ipid=pid;
    }
    else if ( pid==0 )
    {
	char str_ip[64], str_local[64], str_remote[64];
    	if ( dup2( fds[1], STDOUT_FILENO )!=STDOUT_FILENO )
    	{
            perror( "Create_ident: dup2(stdout): " );
            return;
        }
	sprintf( str_local, "%d", mud_data.mudport );
	sprintf( str_remote, "%d", port );
	sprintf( str_ip, "%ld", ip );
    	execl( "../src/resolve", "resolve", str_local, str_ip, str_remote, NULL );
    	log_string( "Exec failed; Closing child." );
    	exit( 0 );
    }
    else
    {
    	perror( "Create_ident: fork" );
    	close( fds[0] );
    	close( fds[1] );
    }
}

void game_loop_unix( int control )
{
  int iSafe = 0; //loop breaker
    static struct timeval null_time;
    struct timeval last_time;
    struct sigaction sa;

    signal( SIGPIPE, SIG_IGN );

    sa.sa_handler = my_sig_handler;
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCLD, &sa, 0) <0)
    {
        perror("Failed to install SIGCLD handler\n");
        exit(0);
    }

    gettimeofday( &last_time, NULL );
    mud_data.current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {

	fd_set in_set;
	fd_set out_set;
	fd_set exc_set;
	DESCRIPTOR_DATA *d;
	int maxdesc;

#if defined(MALLOC_DEBUG)
	if ( malloc_verify( ) != 1 )
	  abort( );
#endif
        /* Poll all active descriptors. */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( control, &in_set );
	maxdesc = control;

	for ( d = descriptor_list; d; d = d->next )
	{
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );

            if ( d->ifd!=-1 && d->ipid!=-1 )
            {
                maxdesc = UMAX( maxdesc, d->ifd );
                FD_SET( d->ifd, &in_set );
            }
	}

	if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
            if( errno == EINTR )
                continue;
	    perror( "Game_loop: select: poll" );
	    exit( 1 );
	}
        /* New connection? */
	if ( FD_ISSET( control, &in_set ) )
	    init_descriptor( control );

        /* Kick out the messed up connections. */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character && d->connected == CON_PLAYING)
		    save_char_obj( d->character );
		d->outtop       = 0;
		close_socket( d );
	    }
            else if ( check_ban(d->host,BAN_ALL) || check_ban(d->ident,BAN_ALL))
            {
	        do_traceban(d->ident);
            	write_to_buffer( d, "Your site has been banned from The Forsaken Lands.\n\r", 0 );
    	    	close_socket(d);
            }
    	    else if (check_spam(d->ident))
	    {
		write_to_buffer(d, "Your site has been permanently banned for spamming connections.\n\r", 0);
	        close_socket(d);
	    }
	}


        /* Process input. */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	  d_next      = d->next;
	  d->fcommand = FALSE;
	  if ( FD_ISSET( d->descriptor, &in_set ) )
	    {
	      if ( d->character != NULL )
		{
		  d->character->timer = 0;
		  d->character->idle = 0;
		}
	      if ( !read_from_descriptor( d ) )
		{
		  FD_CLR( d->descriptor, &out_set );
		  if ( d->character != NULL && d->connected == CON_PLAYING)
		    save_char_obj( d->character );
		  d->outtop   = 0;
		  close_socket( d );
		  continue;
		}
	    }
	  if ( ( d->connected == CON_PLAYING || CH(d)!=NULL ) && d->ifd!=-1 && FD_ISSET( d->ifd, &in_set ) )
	    process_ident( d );
	  if ( d->character != NULL && d->character->wait > 0 )
	    {
	      --d->character->wait;
	      continue;
	    }

	  read_from_buffer( d );
	  //run a forced update for the nag screen
	  if (d->connected == CGEN_NAGLAG){
	    char_gen( d, "" );
	    //clear buffer of spammed commands
	    d->incomm[0]    = '\0';
	  }

	  if ( d->incomm[0] != '\0' )
	    {
	      d->fcommand     = TRUE;
	      stop_idling( d->character );
	      if ( d->showstr_point )
		show_string( d, d->incomm );
	      else if ( d->pString )
		string_add( d->character, d->incomm );
	      else
                switch ( d->connected )
		  {
		  case CON_PLAYING:
		    init_loop("interpret");
		    if (!army_interpret( d->character, d->incomm )
			&& !run_olc_editor( d ) )
		      interpret( d->character, d->incomm );
		    end_loop("interpret");
		    break;
		  case CON_SELECT:
		    init_loop("select");
		    interpret_select( d->character, d->incomm );
		    end_loop("select");
		    break;
		  case CON_VOTE_BOOTH:
		  case CON_VOTE:
		    init_loop("vote");
		    interpret_vote( d->character, d->incomm );
		    end_loop("vote");
		    break;
		  default:
		    char_gen( d, d->incomm );
		    break;
		  }
	      d->incomm[0]    = '\0';
	    }
	}

        /* Autonomous game motion. */
	update_handler( );

	/* Out of control loop safety. */
	iSafe = 0;
        /* Output. */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	  if (++iSafe > OUTPUT_SAFETY)
	    {
	      log_string( "game_loop_unix: output loop out of control, breaking..." );
	      break;
	    }

	    d_next = d->next;
            if ( ( d->fcommand || d->outtop > 0 ) && FD_ISSET(d->descriptor, &out_set) )
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character != NULL && d->connected == CON_PLAYING)
			save_char_obj( d->character );
		    d->outtop   = 0;
		    close_socket( d );
		}
	}


        /* Synchronize to a clock.                            *
         * Sleep( last_time + 1/PULSE_PER_SECOND - now ).     *
         * Careful here of signed versus unsigned arithmetic. */
	{
	    struct timeval now_time;
	    long secDelta, usecDelta;
	    gettimeofday( &now_time, NULL );
            usecDelta   = ((int) last_time.tv_usec) - ((int) now_time.tv_usec) + 1000000 / PULSE_PER_SECOND;
	    secDelta    = ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }
	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }
	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;
		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
		{
                    if( errno == EINTR )
                        continue;
		    perror( "Game_loop: select: stall" );
		    exit( 1 );
		}
	    }
	}
	gettimeofday( &last_time, NULL );
	mud_data.current_time = (time_t) last_time.tv_sec;
    }
}

void init_descriptor( int control )
{
    char buf[MSL];
    DESCRIPTOR_DATA *dnew = NULL;
    struct sockaddr_in sock;
    int desc;
    socklen_t size = sizeof(sock);
    getsockname( control, (struct sockaddr *) &sock, &size );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
    {
	perror( "New_descriptor: accept" );
	return;
    }
#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif
    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
	perror( "New_descriptor: fcntl: FNDELAY" );
	return;
    }
    dnew = new_descriptor();
    dnew->descriptor = desc;
    dnew->ifd = -1;
    dnew->ipid = -1;
    size = sizeof(sock);
    if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
    {
	perror( "New_descriptor: getpeername" );
	dnew->host = str_dup( "(unknown)" );
	dnew->ident = str_dup( "(unknown)" );
    }
    else
    {
        /* Would be nice to use inet_ntoa here but it takes a struct arg, *
         * which isn't very compatible between gcc and system libraries.  */
	int addr;
        create_ident( dnew, sock.sin_addr.s_addr, ntohs( sock.sin_port ) );
	addr = ntohl( sock.sin_addr.s_addr );
	sprintf( buf, "%d.%d.%d.%d",
		 ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
		 ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
		 );
        dnew->host=str_dup( buf );
	dnew->ident=str_dup( buf );
        dnew->port=ntohs( sock.sin_port );
    }
    /* Init descriptor data. */
    dnew->next                  = descriptor_list;
    descriptor_list             = dnew;

    dnew->connected		= CGEN_GREET;
    char_gen(dnew, "" );
}

void close_socket( DESCRIPTOR_DATA *dclose )
{
    CHAR_DATA *ch, *rch;
    Double_List * tmp_list;
    Double_List * tmp_list2;

    if ( dclose->ipid>-1 )
    {
        int status;
        kill( dclose->ipid, SIGKILL );
        waitpid( dclose->ipid, &status, WNOHANG );
    }
    if ( dclose->ifd>-1 )
        close( dclose->ifd );
    if ( dclose->outtop > 0 )
	process_output( dclose, FALSE );
    if ( dclose->snoop_by != NULL ) {
      tmp_list = dclose->snoop_by;
      while (tmp_list != NULL) {
	write_to_buffer(((DESCRIPTOR_DATA *)tmp_list->cur_entry),
			"Your victim has left the realm.\n\r", 0 );
	tmp_list = tmp_list->next_node;
      }
    }
    {
	DESCRIPTOR_DATA *d;

/* loop over all the descriptors in the game */
      for ( d = descriptor_list; d != NULL; d = d->next ) {
	tmp_list = d->snoop_by;

/* - loop over all the people snooping this descriptor */
	while (tmp_list != NULL) {

/* -- if one of those people is us, remove us from the list of snoopers */
	  if (((DESCRIPTOR_DATA *) tmp_list->cur_entry) == dclose) {

/* --- if we are first on the list, handle the remove */
	    if (tmp_list == d->snoop_by) {
	      d->snoop_by = tmp_list->next_node;
	      if (tmp_list->next_node != NULL) {
		tmp_list->next_node->prev_node = NULL;
	      }
	      tmp_list2 = tmp_list;
	      tmp_list = tmp_list->next_node;
	      free (tmp_list2);
	    }

/* --- otherwise we are somewhere else in the list, handle the remove */
	    else {
	      tmp_list->prev_node->next_node = tmp_list->next_node;
	      if (tmp_list->next_node != NULL) {
		tmp_list->next_node->prev_node = tmp_list->prev_node;
	      }
	      tmp_list2 = tmp_list;
	      tmp_list = tmp_list->next_node;
	      free (tmp_list2);
	    }
	  }
	  else {
	    tmp_list = tmp_list->next_node;
	  }
	}
      }
    }
    if ( ( ch = dclose->character ) != NULL )
    {
	nlogf( "Closing link to %s.", ch->name );
	if ( dclose->connected == CON_PLAYING && !merc_down)
	{
            for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
                if (get_trust(rch) >= ch->invis_level && ch->desc)
		    act_new( "$n has wandered from $s mind.", ch, NULL, rch, TO_VICT, POS_RESTING + 100 );
	    sprintf( log_buf, "%s@%s has lost link in room %d.", ch->name, ch->desc->host, ch->in_room->vnum );
            wiznet(log_buf, NULL,NULL,WIZ_LINKS,0,get_trust(ch));
	    if (ch->logon != 0){
	      ch->played += (int) (mud_data.current_time - ch->logon);
	      ch->pcdata->mplayed += (int) ((mud_data.current_time - ch->logon)/60);
	    }
	    ch->logon = 0;
	    ch->desc = NULL;
	}
	else
	{
	    free_char(dclose->original ? dclose->original : dclose->character );
	}
    }
    else
      nlogf("Closing link to unknown");

    if ( d_next == dclose )
	d_next = d_next->next;
    if ( dclose == descriptor_list )
	descriptor_list = descriptor_list->next;
    else
    {
	DESCRIPTOR_DATA *d;
        for ( d = descriptor_list; d && d->next != dclose; d = d->next );
	if ( d != NULL )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }
    close( dclose->descriptor );
    free_descriptor(dclose);
}

bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;
    /* Wait if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;
    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
	nlogf( "%s input overflow!", d->host );
        write_to_descriptor( d->descriptor, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
	return FALSE;
    }
    /* Snarf input. */
    for ( ; ; )
    {
	int nRead;
	nRead = read( d->descriptor, d->inbuf + iStart,
	    sizeof(d->inbuf) - 10 - iStart );
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	    return FALSE;
	else if ( errno == EWOULDBLOCK )
	    break;
	else
	{
	    perror( "Read_from_descriptor" );
	    return FALSE;
	}
    }
    d->inbuf[iStart] = '\0';
    return TRUE;
}

/* Transfer one line from input buffer to input line. */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;
    /* Wait if pending command already. */
    if ( d->incomm[0] != '\0' )
	return;
    /* Look for at least one new line. */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
	if ( d->inbuf[i] == '\0' )
	    return;
    /* Canonical input processing. */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( k >= MIL - 3 )
	{
	    write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );
	    /* skip the rest of the line */
	    for ( ; d->inbuf[i] != '\0'; i++ )
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}
	if ( d->inbuf[i] == '\b' && k > 0 )
	    --k;
        if ( (d->inbuf[i] == '`') && (k > 0) && (d->inbuf[i+1] != '\0') && (get_trust(d->character) < 59))
	    i++;
	else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
	    d->incomm[k++] = d->inbuf[i];
    }
    /* Finish off the line. */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';
    /* Deal with bozos with #repeat 1000 ... */
    if ( k > 1 || d->incomm[0] == '!' )
    {
	if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
	    d->repeat = 0;
	else
	{
	    ++d->repeat;
            if (d->repeat == 25 && d->character && d->connected == CON_PLAYING)
	    {
		nlogf( "%s@%s input spamming!", d->character->name, d->host );
                wiznet("Spam spam spam $N spam spam spam spam spam!", d->character,NULL,WIZ_SPAM,0,get_trust(d->character));
		if (d->incomm[0] == '!')
                    wiznet(d->inlast,d->character,NULL,WIZ_SPAM,0, get_trust(d->character));
		else
                    wiznet(d->incomm,d->character,NULL,WIZ_SPAM,0, get_trust(d->character));
		send_to_char("Stop spamming.\n\r",d->character);
		WAIT_STATE(d->character,48);
	    }
            else if (d->repeat == 50 && d->character && d->connected == CON_PLAYING)
	    {
		nlogf( "%s@%s input spamming!", d->character->name, d->host );
                wiznet("Still spam spam $N spam spam spam spam spam!", d->character,NULL,WIZ_SPAM,0,get_trust(d->character));
		if (d->incomm[0] == '!')
                    wiznet(d->inlast,d->character,NULL,WIZ_SPAM,0, get_trust(d->character));
		else
                    wiznet(d->incomm,d->character,NULL,WIZ_SPAM,0, get_trust(d->character));
		send_to_char("Last warning to stop spamming.\n\r",d->character);
		WAIT_STATE(d->character,48);
	    }
            else if (d->repeat >= 75 && d->character && d->connected == CON_PLAYING && !IS_IMMORTAL(d->character))
	    {
		AFFECT_DATA maf;
		nlogf( "%s@%s input spamming, frozen!", d->character->name, d->host );
                wiznet("Frozen spam spam $N spam spam spam spam frozen!", d->character,NULL,WIZ_SPAM,0,get_trust(d->character));
		if (d->incomm[0] == '!')
                    wiznet(d->inlast,d->character,NULL,WIZ_SPAM,0, get_trust(d->character));
		else
                    wiznet(d->incomm,d->character,NULL,WIZ_SPAM,0, get_trust(d->character));
    		maf.type             = gsn_mortally_wounded;
    		maf.level            = d->character->level;
    		maf.duration         = 3;
    		maf.location         = APPLY_NONE;
    		maf.modifier         = 0;
    		maf.bitvector        = 0;
    		affect_to_char(d->character,&maf);
        	send_to_char( "You have been mortally wounded for excessive spamming!\n\r", d->character);
                d->repeat = 0;
	    }
	}
    }
    /* Do '!' substitution. */
    if ( d->incomm[0] == '!' )
	strcpy( d->incomm, d->inlast );
    else
	strcpy( d->inlast, d->incomm );
    /* Shift the input buffer. */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;
}

/* Low level output function. */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    extern bool merc_down;
    Double_List * tmp_list;

    /* Bust a prompt. */
    if ( !merc_down )
    {
        if ( d->showstr_point )
            write_to_buffer( d, "\r[Hit Return to continue]\r", 0 );
        else if ( fPrompt && d->pString && d->connected == CON_PLAYING )
            write_to_buffer( d, "> ", 2 );
        else if ( fPrompt && d->connected == CON_PLAYING )
        {
            CHAR_DATA *ch;
            CHAR_DATA *victim;
            ch = d->character;
            /* battle prompt */
            if ((victim = ch->fighting) != NULL
		&& (can_see(ch,victim) || (ch->class == gcn_blademaster && is_affected(ch, gsn_battletrance)))
		&& ch->in_room != NULL
		&& victim->in_room != NULL
		&& ch->in_room == victim->in_room)
	      {
                int percent;
                char wound[100], buf[MSL];
                if (victim->max_hit > 0)
		  percent = victim->hit * 100 / victim->max_hit;
                else
                    percent = -1;
                if (percent >= 100)     sprintf(wound,"is in excellent condition.");
                else if (percent >= 90) sprintf(wound,"has a few scratches.");
                else if (percent >= 75) sprintf(wound,"has some small wounds and bruises.");
                else if (percent >= 50) sprintf(wound,"has quite a few wounds.");
                else if (percent >= 30) sprintf(wound,"has some big nasty wounds and scratches.");
                else if (percent >= 15) sprintf(wound,"looks pretty hurt.");
                else if (percent >= 0)  sprintf(wound,"is in awful condition.");
                else if (victim->hit < -10 && IS_AFFECTED2(victim, AFF_RAGE))
		    			sprintf(wound,"growls and refuses to die!");
		else			sprintf(wound,"is bleeding to death.");
                sprintf(buf,"%s %s \n\r", PERS(victim,ch),wound);
                buf[0] = UPPER(buf[0]);
		if (!is_affected(victim,gsn_mirror_image))
                    write_to_buffer( d, buf, 0);
                }
            ch = d->original ? d->original : d->character;
            if (!IS_SET(ch->comm, COMM_COMPACT) )
                write_to_buffer( d, "\n\r", 2 );
            if ( IS_SET(ch->comm, COMM_PROMPT) )
                bust_a_prompt( d->character );
            if (IS_SET(ch->comm,COMM_TELNET_GA))
                write_to_buffer(d,go_ahead_str,0);
        }
    }
    /* Short-circuit if nothing to write.  */
    if ( d->outtop == 0 )
	return TRUE;
    /* Snoop-o-rama. */
    if ( d->snoop_by != NULL ) {
      tmp_list = d->snoop_by;
      while (tmp_list != NULL) {
	write_to_buffer((DESCRIPTOR_DATA *)tmp_list->cur_entry, "\n\r", 0 );
	if (d->character != NULL)
	    write_to_buffer( (DESCRIPTOR_DATA *)tmp_list->cur_entry, d->character->name,0);
	write_to_buffer((DESCRIPTOR_DATA *)tmp_list->cur_entry, "> ", 2 );
	write_to_buffer((DESCRIPTOR_DATA *)tmp_list->cur_entry, d->outbuf, d->outtop );
	write_to_buffer((DESCRIPTOR_DATA *)tmp_list->cur_entry, "\n\r", 0 );
	tmp_list = tmp_list->next_node;
      }
    }
    /* OS-dependent output. */
    if ( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}

/* Bust a prompt (player settable prompt) */
void bust_a_prompt( CHAR_DATA *ch )
{
    char buf[MSL], buf2[MSL];
    const char *str;
    const char *i;
    char *point;

    point = buf;
    str = ch->prompt;
    if ( ch->invis_level > 1 )
	sendf(ch,"(Wizi %d) ",ch->invis_level);
    if ( ch->incog_level > 1 )
	sendf(ch,"(Incog %d) ",ch->incog_level);
    if (IS_SET(ch->comm,COMM_AFK))
    {
	 send_to_char("<AFK> ",ch);
	 return;
    }
    if (str == NULL || str[0] == '\0')
    {
        sendf( ch, "<%dhp %dm %dmv>%s ", ch->hit,ch->mana,ch->move,ch->prefix);
        return;
    }

    while( *str != '\0' )
    {
        if( *str != '%' )
        {
	    *point++ = *str++;
            if (!(*str))
            {
                *point = 0;
                break;
            }
            else
	        continue;
        }
        ++str;
        switch( *str )
        {
        default :
            i = " "; break;
	case 'a' : sprintf( buf2,"%d", GET_CP( ch ));   i = buf2; break;
        case 'c' : sprintf( buf2,"%s","\n\r");          i = buf2; break;
	case 'C'  :
	  if (ch->fighting && ch->fighting->fighting
	      && is_same_group(ch, ch->fighting->fighting))
	    sprintf( buf2,"%s","\n\r");
	  else
	    buf2[0] = '\0';
	  i = buf2; break;
        case 'h' :
	  if (ch->max_hit != 0 && ch->hit*100/ch->max_hit <= 20)
	    sprintf( buf2, "`1%d``", ch->hit );
	  else if (ch->max_hit != 0 && ch->hit*100/ch->max_hit <= 40)
	    sprintf( buf2, "`#%d``", ch->hit );
	  else
	    sprintf( buf2, "%d", ch->hit );
	  i = buf2; break;
        case 'e' : do_promptexit(ch, buf2);		i = buf2; break;
        case 'g' : sprintf( buf2, "%ld", ch->gold);     i = buf2; break;
        case 'H' : sprintf( buf2, "%d", ch->max_hit );  i = buf2; break;
        case 'i' : if (!IS_NPC(ch) && ch->pcdata->pStallion) i = "M"; else i = "D"; break;
	case 'l' : sprintf( buf2, "`%c", *++str	    );  i = buf2; break;
	case 'L' : i = "``";					  break;
        case 'm' : sprintf( buf2, "%d", ch->mana );     i = buf2; break;
        case 'M' : sprintf( buf2, "%d", ch->max_mana ); i = buf2; break;
        case 'n' :
	  if (ch->fighting && ch->fighting->fighting
	      && is_same_group(ch, ch->fighting->fighting))
	    sprintf( buf2, "%s: ", IS_NPC(ch->fighting->fighting)?
		     ch->fighting->fighting->short_descr : ch->fighting->fighting->name);
	  else
	    buf2[0] = '\0';
	  i = buf2; break;
	case 'p' :
	  if (ch->fighting && ch->fighting->fighting
	      && is_same_group(ch, ch->fighting->fighting))
	    health_prompt(buf2, ch->fighting->fighting->hit, ch->fighting->fighting->max_hit, TRUE);
	  else
	    buf2[0] = '\0';
	  i = buf2; break;
	case 'P':
	  if (ch->fighting && ch->fighting->fighting
	      && is_same_group(ch, ch->fighting->fighting))// && ch != ch->fighting->fighting)
	    health_prompt(buf2, ch->fighting->fighting->hit, ch->fighting->fighting->max_hit, FALSE);
	  else
	    buf2[0] = '\0';
	  i = buf2; break;
        case 'r' : if( IS_IMMORTAL( ch ) && ch->in_room != NULL ) sprintf( buf2, "%s", ch->in_room->name );
	else sprintf( buf2, " " );           i = buf2; break;
        case 'R' : if( IS_IMMORTAL( ch ) && ch->in_room != NULL ) sprintf( buf2, "%d", ch->in_room->vnum );
	else sprintf( buf2, " " );           i = buf2; break;
        case 't' : sprintf( buf2, "%d", mud_data.time_info.hour ); i = buf2; break;
        case 'v' : sprintf( buf2, "%d", ch->move );     i = buf2; break;
        case 'V' : sprintf( buf2, "%d", ch->max_move ); i = buf2; break;
        case 'x' : sprintf( buf2, "%ld", ch->exp );      i = buf2; break;
        case 'X' : sprintf(buf2, "%ld", IS_NPC(ch) ? 0 : total_exp(ch) - ch->exp); i = buf2; break;
        case 'z' : if( IS_IMMORTAL( ch ) && ch->in_room != NULL ) sprintf( buf2, "%s", ch->in_room->area->name );
	else sprintf( buf2, " " );           i = buf2; break;
        case '%' : sprintf( buf2, "%%" );               i = buf2; break;
        case 'o' : sprintf( buf2, "%s", olc_ed_name(ch) ); i = buf2; break;
        case 'O' : sprintf( buf2, "%s", olc_ed_vnum(ch) ); i = buf2; break;
        }
        ++str;
        while( (*point = *i) != '\0' )
	  ++point, ++i;
    }
    send_to_char(buf,ch);
    if (ch->prefix[0] != '\0')
    {
        write_to_buffer(ch->desc,ch->prefix,0);
	write_to_buffer(ch->desc," ",0);
    }
}

#define CNUM(x) ((ch->pcdata) ? ch->pcdata->x : 0xf)
void process_color(CHAR_DATA *ch, char a)
{
    int c = 0;
    char send_it_anyway[2];
    switch(a)
    {
	case '-':
	    write_to_buffer(ch->desc, "~", 0);
	    return;
	case '=':
	    write_to_buffer(ch->desc, "`", 0);
	    return;
	case '`': c=0; break;
        case '1': c=1; break;
        case '2': c=2; break;
        case '3': c=3; break;
        case '4': c=4; break;
        case '5': c=5; break;
        case '6': c=6; break;
        case '7': c=7; break;
        case '8': c=16; break;
        case '9': c=13; break;
        case '0': c=12; break;
	case '!': c=9; break;
	case '@': c=10; break;
	case '#': c=11; break;
	case '$': c=12; break;
	case ')': c=12; break;
	case '%': c=13; break;
	case '^': c=14; break;
	case '&': c=15; break;
	case '*': c=16; break;
        case 'q': c=17; break;
        case 'w': c=18; break;
	case 'e': c=19; break;
        case 'r': c=20; break;
        case 't': c=21; break;
        case 'y': c=22; break;
        case 'u': c=23; break;
        case 'i': c=32; break;
        case 'Q': c=25; break;
        case 'W': c=26; break;
        case 'E': c=27; break;
        case 'R': c=28; break;
	case 'T': c=29; break;
        case 'Y': c=30; break;
        case 'U': c=31; break;
        case 'I': c=32; break;
        case 'a': c=33; break;
        case 's': c=34; break;
        case 'd': c=35; break;
        case 'f': c=36; break;
        case 'g': c=37; break;
        case 'h': c=38; break;
	case 'j': c=39; break;
        case 'k': c=48; break;
        case 'A': c=41; break;
        case 'S': c=42; break;
        case 'D': c=43; break;
        case 'F': c=44; break;
        case 'G': c=45; break;
        case 'H': c=46; break;
        case 'J': c=47; break;
        case 'K': c=48; break;
	case 'z': c=49; break;
        case 'x': c=50; break;
        case 'c': c=51; break;
        case 'v': c=52; break;
        case 'b': c=53; break;
        case 'n': c=54; break;
        case 'm': c=55; break;
        case ',': c=64; break;
        case 'Z': c=57; break;
        case 'X': c=58; break;
	case 'C': c=59; break;
        case 'V': c=60; break;
        case 'B': c=61; break;
        case 'N': c=62; break;
        case 'M': c=63; break;
        case '<': c=64; break;
        default:
            sprintf (send_it_anyway, "%c", a);
            write_to_buffer (ch->desc, send_it_anyway, 0);
            return;
    }
    if (IS_COLOR(ch, COLOR_ON) && !IS_NPC(ch))
    {
	char buf[16];
	sprintf (buf, "%s%sm", ANSI_ESCAPE, color_table[c]);
	if (ch->desc && buf != NULL)
    	    write_to_buffer(ch->desc, buf, strlen(buf));
    }
}

/* Append onto an output buffer. */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    /* Find length in case caller didn't. */
    if ( length <= 0 )
	length = strlen(txt);
    /* Initial \n\r if needed. */
    if ( d->outtop == 0 && !d->fcommand )
    {
	d->outbuf[0]    = '\n';
	d->outbuf[1]    = '\r';
	d->outtop       = 2;
    }
    /* Expand the buffer as needed. */
    while ( d->outtop + length >= d->outsize )
    {
	char *outbuf;
	if (d->outsize >= 32000)
	{
	    bugf("Buffer overflow. Closing %s.\n\r", d->character->name);
	    close_socket(d);
	    return;
	}
	outbuf      = alloc_mem( 2 * d->outsize );
	strncpy( outbuf, d->outbuf, d->outtop );
	free_mem( d->outbuf, d->outsize );
	d->outbuf   = outbuf;
	d->outsize *= 2;
    }
    /* Copy. */
    strncpy( d->outbuf + d->outtop, txt, length);
    d->outtop += length;
    d->outbuf[d->outtop]='\0';
}

/* Lowest level output function.                                *
 * Write a block of text to the file descriptor.                *
 * If this gives errors on very long blocks (like 'ofind all'), *
 *   try lowering the max block size.                           */
bool write_to_descriptor( int desc, char *txt, int length )
{
    int iStart, nWrite, nBlock;
    if ( length <= 0 )
	length = strlen(txt);
    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 4096 );
	if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
        {
            perror( "Write_to_descriptor" );
            return FALSE;
        }
    }
    return TRUE;
}

/* checks for login messags and prints them */
void login_messages( CHAR_DATA* ch ){
  int msg = 0;
  if (IS_NPC(ch) || ch->pcdata->messages == MSG_NONE)
    return;
  send_to_char("\n\r", ch);
  for (msg = 0; message_flags[msg].bit > 0; msg++){
    if (IS_SET(ch->pcdata->messages, message_flags[msg].bit)){
      send_to_char(message_flags[msg].name, ch);
      send_to_char("\n\r", ch);
    }
  }
  ch->pcdata->messages = MSG_NONE;
}

/* portion of nanny that handles quickstart */
void quick_start(DESCRIPTOR_DATA* d, CHAR_DATA* ch, char* argument){
  char buf[MIL], arg[MIL];
  const int start_stat = 8;
  const int max_add = 25;

  if (IS_NULLSTR(argument)){
    if (ch->train < 1){
      write_to_buffer(d, "You have no picks remaining, use \"end\" to continue.\n\r", 0);
    }
    sprintf( buf, "\n\rRolling stats for %s %s.:\n\r"\
	     "Max: Str: %-2d  Int: %-2d  Wis: %-2d  Dex: %-2d  Con: %-2d\n\r",
	     pc_race_table[ch->race].name,
	     class_table[ch->class].name,
	     get_max_train( ch, STAT_STR),
	     get_max_train( ch, STAT_INT),
	     get_max_train( ch, STAT_WIS),
	     get_max_train( ch, STAT_DEX),
	     get_max_train( ch, STAT_CON));

    write_to_buffer( d, buf, 0 );
    sprintf( buf, "Cur: Str: %-2d  Int: %-2d  Wis: %-2d  Dex: %-2d  Con: %-2d\n\r",
	     ch->perm_stat[STAT_STR],
	     ch->perm_stat[STAT_INT],
	     ch->perm_stat[STAT_WIS],
	     ch->perm_stat[STAT_DEX],
	     ch->perm_stat[STAT_CON]);
    write_to_buffer( d, buf, 0 );
    sprintf(buf, "[%d] Cmds: help, end, roll, add, rem>  \n\r", ch->train);
    write_to_buffer(d, buf, 0);
    return;
  }
/* check commands */
  argument = one_argument(argument, arg);
  /* END */
  if (!str_cmp(arg, "end")){
    /* IDIOT CHECK Viri:(removed)
    if (ch->train && str_cmp(argument, "confirm")){
      write_to_buffer(d, "You have unused picks. Are you sure? (\"end confirm\" to override.)\n\r", 0);
      return;
    }
    */
    write_to_buffer(d, "Exiting the quickstart... Saved.\n\r", 0);
    ch->perm_stat[STAT_LUCK] = _roll_stat(ch,STAT_LUCK);
    d->connected = CGEN_NEW_ALIG;
    return;
  }
/* HELP */
  else if (!str_prefix(arg, "help")){
    do_help(ch,"quickstart_cmds");
    quick_start(d, ch, "");
    return;
  }
/* ADD */
  else if (!str_prefix(arg, "add")){
    char arg2[MIL];
    int value = 0;
    int stat = -1;

    if (IS_NULLSTR(argument)){
      quick_start(d, ch, "");
      return;
    }
    if (ch->train < 1){
      write_to_buffer(d, "You have no picks remaining, use \"end\" to continue.\n\r", 0);
      quick_start(d, ch, "");
      return;
    }
    /* get stat and amount */
    argument = one_argument(argument, arg2);
    if (!str_cmp("str", arg2))
      stat = STAT_STR;
    else if (!str_cmp("int", arg2))
      stat = STAT_INT;
    else if (!str_cmp("wis", arg2))
      stat = STAT_WIS;
    else if (!str_cmp("dex", arg2))
      stat = STAT_DEX;
    else if (!str_cmp("con", arg2))
      stat = STAT_CON;
    else{
      write_to_buffer(d, "Add to which: str/int/wis/dex/con?\n\r", 0);
      quick_start(d, ch, "");
      return;
    }
    /* get value of raise */
    if (IS_NULLSTR(argument))
      value = 1;
    else if ( (value = atoi(argument)) < 1){
      write_to_buffer(d, "The amount to raise must be positive.\n\r", 0);
      quick_start(d, ch, "");
      return;
    }
    /* check if they can afford it */
    if (value > ch->train){
      sprintf(buf, "You have only %d picks left.\n\r", ch->train);
      write_to_buffer(d, buf, 0 );
      quick_start(d, ch, "");
      return;
    }
    else if (ch->perm_stat[stat] + value > max_add){
      sprintf(buf, "You may not raise this stat beyond %d.\n\r", max_add);
      write_to_buffer(d, buf, 0);
      quick_start(d, ch, "");
      return;
    }
    if (ch->perm_stat[stat] + value > get_max_train(ch, stat)){
      sprintf(buf, "You may not raise this stat beyond %d.\n\r", get_max_train(ch, stat));
      write_to_buffer(d, buf, 0);
      quick_start(d, ch, "");
      return;
    }

    ch->perm_stat[stat] += value;
    ch->train -= value;
  }
/* REM */
  else if (!str_prefix(arg, "rem")){
    char arg2[MIL];
    int value = 0;
    int stat = -1;
    if (IS_NULLSTR(argument)){
      quick_start(d, ch, "");
      return;
    }
    /* get stat and amount */
    argument = one_argument(argument, arg2);
    if (!str_cmp("str", arg2))
      stat = STAT_STR;
    else if (!str_cmp("int", arg2))
      stat = STAT_INT;
    else if (!str_cmp("wis", arg2))
      stat = STAT_WIS;
    else if (!str_cmp("dex", arg2))
      stat = STAT_DEX;
    else if (!str_cmp("con", arg2))
      stat = STAT_CON;
    else{
      write_to_buffer(d, "Remove from which: str/int/wis/dex/con?\n\r", 0);
      quick_start(d, ch, "");
      return;
    }
    /* get value of raise */
    if (IS_NULLSTR(argument))
      value = 1;
    else if ( (value = atoi(argument)) < 1){
      write_to_buffer(d, "The amount to remove must be positive.\n\r", 0);
      quick_start(d, ch, "");
      return;
    }
    /* check if they can afford it */
    /* check for low */
    if (ch->perm_stat[stat] - value < start_stat){
      sprintf(buf, "You may only lower the stats down to %d.\n\r", start_stat);
      write_to_buffer(d, buf, 0);
      quick_start(d, ch, "");
      return;
    }
    ch->perm_stat[stat] -= value;
    ch->train += value;
  }
/* ROLL */
  else if (!str_prefix(arg, "roll") || !str_prefix(arg, "reset")){
    /* reset picks */
    ch->train = roll_stat( ch ) - (5 * start_stat);
    /* reset starting stats */
    ch->perm_stat[STAT_STR] = start_stat;
    ch->perm_stat[STAT_INT] = start_stat;
    ch->perm_stat[STAT_WIS] = start_stat;
    ch->perm_stat[STAT_DEX] = start_stat;
    ch->perm_stat[STAT_CON] = start_stat;

    if (strcmp(arg, "reset"))
      quick_start(d, ch, "");
    return;
  }
  quick_start(d, ch, "");
  return;
}

/* puts the character in the game, used in nanny and vote.c */
void connect_char( DESCRIPTOR_DATA* d ){
  CHAR_DATA* ch = d->character;
  CHAR_DATA* rch;

  load_obj (d->character);
  ch->next        = char_list;
  char_list       = ch;
  d->connected    = CON_PLAYING;
  ch->next_player = player_list;
  player_list	= ch;
  reset_char(ch);


  if ( ch->in_room != NULL )
    char_to_room( ch, ch->in_room );
  else
    char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );

  for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    if (get_trust(rch) >= ch->invis_level && ch->desc)
      act_new( "$n has entered the realms.", ch, NULL, rch, TO_VICT, POS_RESTING + 100 );

  if (!str_cmp(d->host,d->ident))
    sprintf(log_buf, "%s@%s has connected in room %d.", ch->name, d->host, ch->in_room->vnum );
  else
    sprintf(log_buf, "%s@%s (%s) has connected in room %d.", ch->name, d->host, d->ident, ch->in_room->vnum );
  log_string( log_buf );
  wiznet(log_buf,NULL,NULL,WIZ_LOGINS,0,get_trust(ch));

  /* cabal echo */
  if (ch->pCabal && !IS_IMMORTAL(ch)){
    if (IS_CABAL(ch->pCabal, CABAL_AREA)){
      sprintf( log_buf, "%s %s has joined the fray.",
	       get_crank_str( ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ),
	       PERS2(ch));
    }
    else if (IS_CABAL(ch->pCabal, CABAL_ROYAL)){
      sprintf( log_buf, "%s %s has graced us with %s presence.",
	       get_crank_str( ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ),
	       PERS2(ch),
	       his_her[URANGE(0,ch->sex,2)]);
    }
    else if (IS_CABAL(ch->pCabal, CABAL_JUSTICE)){
      sprintf( log_buf, "%s %s now serves the Law with us.",
	       get_crank_str( ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ),
	       PERS2(ch));
    }
    else if (IS_CABAL(ch->pCabal, CABAL_ROUGE)){
      sprintf( log_buf, "%s %s has been noticed in the lands.",
	       get_crank_str( ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ),
	       PERS2(ch));
    }
    else{
      sprintf( log_buf, "%s %s has joined our forces.",
	       get_crank_str( ch->pCabal, ch->pcdata->true_sex, ch->pcdata->rank ),
	       PERS2(ch));
    }
    cabal_echo(ch->pCabal, log_buf );
  }

  /* bank update */
  bankUpdate(ch);

  /* check of eq purge */
  purge_limited(ch);
  do_look( ch, "auto" );
  if (ch->in_room->vnum == ROOM_VNUM_PORTAL){
    char strsave[MIL];
    save_char_obj(ch);
    sprintf( strsave, "%s%s", PURGE_DIR, capitalize( ch->name ) );
    unlink(strsave);
    if (ch->carrying == NULL)
      do_outfit(ch,"");
  }

  if (IS_IMMORTAL(ch)){
    do_status(ch,"");
    send_to_char("\n\r",ch);
  }

  do_unread(ch,"");

  // show login messages
  login_messages( ch );
  if (ch->pCabal)
    update_cabal_skills(ch, ch->pCabal, FALSE, TRUE);
  if (IS_IMMORTAL(ch))
    return;
  if (!IS_NPC(ch) && ch->level > 10){
    AFFECT_DATA qaf;
    if (is_affected(ch,gsn_noquit))
      affect_strip(ch,gsn_noquit);
    qaf.where     = TO_AFFECTS;
    qaf.type      = gsn_noquit;
    qaf.level     = ch->level;
    qaf.duration  = 10;
    qaf.modifier  = 0;
    qaf.location  = 0;
    qaf.bitvector = 0;
    affect_to_char( ch, &qaf );
  }

  //Update any new spells.
  update_skills(ch);
}

//returns string showing the chargen_table's string choices
char* show_chargen_table( struct chargen_choice_s* table, int str_width, int col ){
  static char out[MSL];
  char frmt[MIL], buf[MIL];
  int c = 0, i;

  sprintf( frmt, "%%2d. %%-%d.%ds", str_width, str_width );
  out[0] = 0;

  for (i = 0; table[i].name; i++){
    sprintf( buf, frmt, c + 1, table[i].name );
    strcat( out, buf );
    if (c++ % col == col - 1){
      strcat( out, "\n\r");
    }
  }
  if (c % col == col - 1)
    strcat( out, "\n\r");
  strcat( out, "\n\r" );
  return out;
}


CHARGEN_FUN( chargen_null ){}

CHARGEN_FUN( chargen_greet ){
  extern char * help_greeting;
  write_to_buffer( d, help_greeting  , 0 );
  write_to_buffer( d, "\n\r", 2 );
  write_to_buffer( d, echo_on_str, 0 );
  d->connected = CGEN_MAIN;
}

CHARGEN_FUN( chargen_main ){
  char buf[MSL];
  char* game_status, *site_status, *cur_time;

  if (IS_NULLSTR(argument)){
    /* get lock status */
    if (mud_data.wizlock)	game_status = "Game Status: Wizlocked";
    else if (mud_data.newlock)	game_status = "Game Status: Newlocked";
    else			game_status = "Game Status: Open for Play";

    /* get site status */
    if ( check_ban(d->host,BAN_ALL) || check_ban(d->ident,BAN_ALL))
      site_status = "Site Status: Banned.";
    else if ( check_ban(d->host,BAN_NEWBIES) || check_ban(d->ident,BAN_NEWBIES))
      site_status = "Site Status: No New Characters.";
    else if ( check_ban(d->host,BAN_PERMIT) || check_ban(d->ident,BAN_PERMIT))
      site_status = "Site Status: By Approval Only.";
    else
      site_status = "Site Status: Welcome.";

    /* get time */
    cur_time = ctime( &mud_data.current_time );

    sprintf( buf,
	     "%-30.30s %-35.35s\n\r"\
	     "%-30.30s %-35.35s\n\r"\
	     "%-30.30s %-35.35s\n\r"\
	     "%-30.30s %-35.35s\r",
	     "[E]nter  Aabahran",	game_status,
	     "[C]reate Character",	site_status,
	     "[H]elp   Files",		"     System Time",
	     "[D]isconnect",		cur_time);
    write_to_buffer( d, buf, 0 );
    write_to_buffer( d, chargen_table[CGEN_MAIN].prompt, 0);
  }
  else{
    switch( UPPER(argument[0]) ){
    case 'E':
      if ( check_ban(d->host,BAN_ALL) || check_ban(d->ident,BAN_ALL)){
	write_to_buffer( d, "Your site is not allowed to play.\n\r"\
		      "Contact the Implementors (\"help imp\")"\
		      " if you belive you deserve access.\n\r",
		      0);
	chargen_main( d, "");
	return;
      }
      d->connected = CGEN_ENTER;
      break;
    case 'H':	d->connected = CGEN_HELP;	break;
    case 'C':
      if (mud_data.wizlock){
	write_to_buffer( d, "\n\rThe Forsaken Lands are Wizlocked.\n\r", 0 );
	chargen_main( d, "");
	return;
      }
      if (mud_data.newlock){
	write_to_buffer( d, "\n\rThe Forsaken Lands are newlocked.\n\r", 0 );
	chargen_main( d, "");
	return;
      }
      if ( check_ban(d->host,BAN_ALL) || check_ban(d->ident,BAN_ALL)){
	write_to_buffer( d, "Your site is not allowed to play.\n\r"\
			 "Contact the Implementors (\"help imp\")"\
			 " if you belive you deserve access.\n\r",
			 0);
	chargen_main( d, "");
	return;
      }
      if ( (check_ban(d->host,BAN_PERMIT) || check_ban(d->ident,BAN_PERMIT))){
	write_to_buffer( d, "Your site is not allowed to play.\n\r"\
			 "Contact the Implementors (\"help imp\")"\
			 " if you belive you deserve access.\n\r",
			 0);
	chargen_main( d, "");
	return;
      }
      if ( check_ban(d->host,BAN_NEWBIES) || check_ban(d->ident,BAN_NEWBIES)){
	write_to_buffer( d, "Your site is not allowed to play.\n\r"\
			 "Contact the Implementors (\"help imp\")"\
			 " if you belive you deserve access.\n\r",
			 0);
	chargen_main( d, "");
	return;
      }
      d->connected = CGEN_CREATE;
      break;
    case 'D':
      write_to_buffer( d, "Goodbye.", 0);
      close_socket( d );
      return;
    default:
      chargen_main( d, "");
    }
  }
}

CHARGEN_FUN( chargen_help ){
  if (IS_NULLSTR(argument)){
    write_to_buffer( d, chargen_table[CGEN_HELP].prompt, 0);
  }
  else if (UPPER(argument[0]) == 'Q' && argument[1] == 0){
    d->connected = CGEN_MAIN;
    return;
  }
  else{
    HELP_DATA *pHelp;
    char* arg;
    bool found = FALSE;
    int level;

    if ( argument[0] == '?')
	arg = "summary";
    else
      arg = argument;

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next ){
      level = pHelp->level;

      if (level < HELP_ALL || level > 50)
	continue;
      else if ( !is_name( arg, pHelp->keyword ) )
	continue;

      if (found){
	write_to_buffer(d, "\n\r============================================================\n\r\n\r", 0);
      }
      write_to_buffer(d, pHelp->text, 0 );
      found = TRUE;
    }
    if (!found)
      write_to_buffer(d, "No help on that word.\n\r", 0);
    write_to_buffer( d, chargen_table[CGEN_HELP].prompt, 0);
  }
}

CHARGEN_FUN( chargen_enter ){
  int fExists = FALSE;
  CHAR_DATA* ch;
  char name[MIL];

  if (IS_NULLSTR( argument )){
    write_to_buffer( d, chargen_table[CGEN_ENTER].prompt, 0);
    return;
  }
  else if (UPPER(argument[0]) == 'Q' && argument[1] == 0){
    d->connected = CGEN_MAIN;
    return;
  }

  strcpy( name, argument );
  name[0] = UPPER(name[0]);

  /* check for proper name */
  if ( !check_parse_name( argument ) ){
    write_to_buffer( d, "Illegal name, try another.\n\r", 0 );
    free_char(d->character);
    d->character = NULL;

    chargen_enter( d, "" );
    return;
  }

  /* try to load the pfile */
  nlogf( "Attempting to load %s.", argument );
  fExists = load_char( d, argument );

  /* check for invalid pfile */
  if (fExists == -1 ){
    char buf [MIL];
    write_to_buffer( d, "Corrupted Pfile detected, contact Implementors.\n\r", 0 );
    sprintf(buf, "`!Corrupted Pfile detected: %s``", argument);
    wiznet(buf, NULL,NULL,WIZ_LOGINS,0,0);
    d->character = NULL;
    chargen_enter( d, "" );
    return;
  }

  /* got character data now */
  ch   = d->character;
  if (IS_SET(ch->act, PLR_DENY)){
    nlogf( "Denying access to %s@%s.", argument, d->host );
    write_to_buffer( d, "This character has been denied access.\n\r", 0 );
    chargen_enter( d, "" );
    return;
  }
  /* Check bans */
  else if ((check_ban(d->host,BAN_NEWBIES) || check_ban(d->ident,BAN_NEWBIES))
	   && ch->level < 10
	   && !check_approved_name(d->character->name)){

    write_to_buffer(d, "Characters below 10th level are not allowed from your site.\n\rContact Implementors if you belive you should have access.\n\r", 0 );
    chargen_enter( d, "" );
    return;
  }
  else if ((check_ban(d->host,BAN_PERMIT) || check_ban(d->ident,BAN_PERMIT))
	   && !IS_SET(ch->act,PLR_PERMIT)
	   && !check_approved_name(d->character->name)){
    write_to_buffer(d,"This character lacks permission to enter through the ban.\n\r",0);
    chargen_enter( d, "" );
    return;
  }
  else if ( mud_data.wizlock && !IS_IMMORTAL(ch)) {
    write_to_buffer( d, "\n\rThe Forsaken Lands are Wizlocked.\n\r", 0 );
    close_socket( d );
    return;
  }
  else if ( check_reconnect( d, argument, FALSE ) ){
    fExists = TRUE;
  }

  /* existing pfile */
  if ( fExists ){
    /* No Pasword check */
    if (ch->pcdata->pwd[0] == '\0' || !str_cmp("", ch->pcdata->pwd)){
      char* buf;
      write_to_buffer( d, "NULL password, resetting to player name...\n\r", 0 );
      free_string( ch->pcdata->pwd );
      buf = crypt( argument, ch->name );
      ch->pcdata->pwd = str_dup(buf);
    }//end nopass check
    write_to_buffer( d, echo_off_str, 0 );
    d->connected = CGEN_PASS;
  }
  else{
    write_to_buffer(d, "This character doesn't seem to exist.\n\r", 0);
    chargen_enter( d, "" );
    return;
  }
}

/* get existing password */
CHARGEN_FUN( chargen_pass ){
  CHAR_DATA* ch = d->character;

  if (IS_NULLSTR( argument )){
    write_to_buffer( d, chargen_table[CGEN_PASS].prompt, 0);
    return;
  }
  if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd )){
    write_to_buffer( d, "Wrong password.\n\r", 0 );
    close_socket( d );
    return;
  }
  if ( IS_IMMORTAL(ch) && ch->pcdata->pwdimm[0] != '\0'){
    d->connected = CGEN_IMMPASS;
  }
  else{
    write_to_buffer( d, echo_on_str, 0 );
    if (check_playing(d,ch->name))
      return;
    if ( check_reconnect( d, ch->name, TRUE ) ){
      return;
    }
    /* check for throwback to nagscreen if played more then 10h this month*/
    if (ch->pcdata->mplayed > 600
	&& !hasVoted(d)){
      d->connected = CGEN_NAGSCREEN;
    }
    else
      d->connected = CGEN_MOTD;
    do_help( ch, "motd" );
  }
}

CHARGEN_FUN( chargen_immpass ){
  CHAR_DATA* ch = d->character;

  if (IS_NULLSTR( argument )){
    write_to_buffer( d, chargen_table[CGEN_IMMPASS].prompt, 0);
    return;
  }
  write_to_buffer( d, echo_on_str, 0 );
  if (strcmp( crypt( argument, ch->pcdata->pwdimm ), ch->pcdata->pwdimm )){
    char log_buf[MIL];
    sprintf( log_buf, "%s@%s has failed login in room %d.", ch->name, d->host, ch->in_room->vnum );
    log_string( log_buf );
    wiznet(log_buf, NULL,NULL,WIZ_LOGINS,0,0);
    write_to_buffer( d, "Login failed.\n\r", 0 );
    close_socket( d );
    return;
  }
  if (check_playing(d,ch->name))
    return;
  if ( check_reconnect( d, ch->name, TRUE ) )
    return;
  do_help( ch, "imotd" );

  /* check for throwback to nagscreen if played more then 10h this month*/
  if (ch->pcdata->mplayed > 600
      && !hasVoted(d)){
    d->connected = CGEN_NAGSCREEN;
  }
  else
    d->connected = CGEN_MOTD;
}

CHARGEN_FUN( chargen_nag_screen ){
  CHAR_DATA* ch = d->character;

  //show nag screen
  do_help( ch, "nagscreen");
  //reset timer
  ch->pcdata->nagtimer = 4 * PULSE_VIOLENCE;
  //move to the lag part
  d->connected = CGEN_NAGLAG;
}

CHARGEN_FUN( chargen_nag_lag ){
  CHAR_DATA* ch = d->character;

  ch->pcdata->nagtimer --;

  //if no nagtimer, we continue
  if (ch->pcdata->nagtimer < 1){
    d->connected = CGEN_MOTD;
    write_to_buffer( d, "\n\r", 0);
  }
  /*
  else if (hasVoted(d)){
    write_to_buffer(d, "Thanks for voting!", 0);
    ch->pcdata->nagtimer = 1;
  }
  */
}


CHARGEN_FUN( chargen_motd ){
  CHAR_DATA* ch = d->character;
  int days = (mud_data.current_time -  ch->logoff) / (30 * MAX_HOURS);

  //print how long ago this person logged in
  if (days){
    if (days < MAX_DAYS){
      sendf( ch, "It has been %d days since your last visit.\n\r",
	     days );
    }
    else if (days < (MAX_DAYS * MAX_WEEKS)){
      sendf( ch, "It has been %d weeks since your last visit.\n\r",
	     days / MAX_DAYS );
    }
    else if (days < (MAX_DAYS * MAX_WEEKS * MAX_MONTHS)){
      sendf( ch, "It has been %d months since your last visit.\n\r",
	     days / (MAX_DAYS * MAX_WEEKS));
    }
    else{
      sendf( ch, "It has been %d years since your last visit.\n\r",
	     days / (MAX_DAYS * MAX_WEEKS * MAX_MONTHS));
    }
    sendf( ch, "It is %s\n\r", mud_date());
  }

  write_to_buffer( d, chargen_table[CGEN_MOTD].prompt, 0);

  d->connected = CGEN_READY;
}

CHARGEN_FUN( chargen_ready ){
  CHAR_DATA* ch = d->character;

  /* CHECK FOR VOTES HERE */
  if (has_votes( ch ) > 0){
    char log_buf[MIL];
    sprintf(log_buf, "%s has entered the vote booth.", ch->name);
    wiznet(log_buf,NULL,NULL,WIZ_LOGINS,0,get_trust(ch));
    d->connected = CON_VOTE_BOOTH;
    interpret_vote(ch, "" );
    return;
  }
  connect_char( d );
}

/* New player */
CHARGEN_FUN( chargen_create ){
  DESCRIPTOR_DATA* dt;
  char name[MIL];

  if (IS_NULLSTR( argument )){
    write_to_buffer( d, chargen_table[CGEN_CREATE].prompt, 0);
    return;
  }
  else if (UPPER(argument[0]) == 'Q' && argument[1] == 0){
    d->connected = CGEN_MAIN;
    return;
  }

  strcpy( name, argument );
  name[0] = UPPER(name[0]);

  /* check for proper name */
  if ( !check_parse_name( name ) ){
    write_to_buffer( d, "Illegal name, try another.\n\r", 0 );
    free_char(d->character);
    d->character = NULL;

    chargen_create( d, "" );
    return;
  }
  else if (load_char( d, name ) != 0){
    write_to_buffer(d, "This character already exists.\n\r", 0);
    free_char(d->character);
    d->character = NULL;

    chargen_create( d, "" );
    return;
  }
  nlogf( "%s@%s is a new player.", d->character->name, d->host );
  for ( dt = descriptor_list; dt != NULL; dt = dt->next ){
    if ( d != dt
	 && dt->character != NULL
	 && !str_cmp( dt->character->name, name ) ){
      write_to_buffer( d, "That name is already in the character creation process.\n\r", 0 );
      free_char( d->character );
      d->character = NULL;

      chargen_create( d, "" );
      return;
    }
  }
  d->connected = CGEN_CON_NAME;
  return;
}

/* confirms a new char name */
CHARGEN_FUN( chargen_con_name ){
  CHAR_DATA* ch = d->character;
  char buf[MIL];

  if (IS_NULLSTR(argument)){
    sprintf( buf, chargen_table[CGEN_CON_NAME].prompt, ch->name );
    write_to_buffer( d, buf, 0 );
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    d->connected = CGEN_NEW_PASS;
  }
  else{
    free_char(d->character);
    d->character = NULL;
    d->connected = CGEN_CREATE;
  }
}

CHARGEN_FUN( chargen_new_pass ){
  CHAR_DATA* ch = d->character;
  char* pwdnew, *p;
  char buf[MIL];

  if (IS_NULLSTR(argument)){
    sprintf( buf, chargen_table[CGEN_NEW_PASS].prompt, ch->name );
    write_to_buffer( d, buf, 0 );
    return;
  }
  if ( strlen(argument) < 5 ){
    write_to_buffer( d,"Password must be at least five characters.\n\r", 0);
    chargen_new_pass( d, "" );
    return;
  }

  pwdnew = crypt( argument, ch->name );

  for ( p = pwdnew; *p != '\0'; p++ ){
    if ( *p == '~' ){
      write_to_buffer( d, "New password not acceptable, try again.\n\r", 0 );
      chargen_new_pass( d, "" );
      return;
    }
  }
  free_string( ch->pcdata->pwd );
  ch->pcdata->pwd = str_dup( pwdnew );

  d->connected = CGEN_CON_PASS;
}

CHARGEN_FUN( chargen_con_pass ){
  CHAR_DATA* ch = d->character;
  char* pwdnew;
  char buf[MIL];

  if (IS_NULLSTR(argument)){
    sprintf( buf, "%s", chargen_table[CGEN_CON_PASS].prompt );
    write_to_buffer( d, buf, 0 );
    return;
  }
  else if ( str_cmp(pwdnew = crypt( argument, ch->name ), ch->pcdata->pwd)){
    write_to_buffer( d, "Passwords do not match.\n\r", 0);
    d->connected = CGEN_NEW_PASS;
    return;
  }
  else
    d->connected = CGEN_NEW_LAST;
}

CHARGEN_FUN( chargen_new_last ){
  CHAR_DATA* ch = d->character;
  char buf[MIL];

  if (IS_NULLSTR(argument)){
    sprintf( buf, "%s", chargen_table[CGEN_NEW_LAST].prompt );
    write_to_buffer( d, buf, 0 );
    return;
  }
  sprintf( buf, "%s", argument );

  if (!str_cmp( buf, "n")){
    d->connected = CGEN_CON_LAST;
    return;
  }
  else if (strlen( buf ) < 3 || strlen( buf ) > 15){
    write_to_buffer(d, "Last name must be between 3 and 15 characters.\n\r",0);
    chargen_new_last( d, "");
    return;
  }
  else if ( !check_parse_name( argument ) || check_exist_name(argument) ){
    write_to_buffer( d,"Illegal last name, try another.\n\r\n\r", 0 );
    chargen_new_last( d, "");
    return;
  }

  buf[0] = UPPER(buf[0]);
  free_string( ch->pcdata->family );
  ch->pcdata->family = strdup( buf );

  d->connected = CGEN_CON_LAST;
}

CHARGEN_FUN( chargen_con_last ){
  CHAR_DATA* ch = d->character;
  char buf[MIL];

  if (IS_NULLSTR(argument)){
    sprintf( buf, chargen_table[CGEN_CON_LAST].prompt, ch->name, ch->pcdata->family );
    write_to_buffer( d, buf, 0 );
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    d->connected = CGEN_NEW_SEX;
  }
  else{
    free_string( ch->pcdata->family );
    ch->pcdata->family = str_dup("");
    d->connected = CGEN_NEW_LAST;
  }
}

CHARGEN_FUN( chargen_new_sex ){
  CHAR_DATA* ch = d->character;
  char buf[MIL];

  switch ( argument[0] ){
  case 'm': case 'M': ch->sex = SEX_MALE;
    ch->pcdata->true_sex = SEX_MALE;
    break;
  case 'f': case 'F': ch->sex = SEX_FEMALE;
    ch->pcdata->true_sex = SEX_FEMALE;
    break;
  default:
    sprintf( buf, "%s", chargen_table[CGEN_NEW_SEX].prompt );
    write_to_buffer( d, buf, 0 );
    return;
  }
  d->connected = CGEN_NEW_RACE;
}

CHARGEN_FUN( chargen_new_adve ){
  CHAR_DATA* ch = d->character;
  char buf[MIL];

  switch ( argument[0] ){
  case 'y':
    ch->class = gcn_adventurer;
    d->connected = CGEN_CON_CLAS;
    break;
  case 'n':
    d->connected = CGEN_NEW_CLAS;
    break;
  default:
    sprintf( buf, "%s", chargen_table[CGEN_NEW_ADVE].prompt );
    write_to_buffer( d, buf, 0 );
    return;
  }

}

CHARGEN_FUN( chargen_new_tele ){
  CHAR_DATA* ch = d->character;
  char buf[MIL];

  switch ( argument[0] ){
  case 'y': case 'Y':
    SET_BIT(ch->comm, COMM_TELEPATH);
    break;
  case 'n': case 'N':
    break;
  default:
    do_help( ch, "telepathy" );
    sprintf( buf, "%s", chargen_table[CGEN_NEW_TELE].prompt );
    write_to_buffer( d, buf, 0 );
    return;
  }
  d->connected = CGEN_NEW_ADVE;
}

CHARGEN_FUN( chargen_new_enemy ){
  CHAR_DATA* ch = d->character;
  int i = 0, c = 0, choice;

  if (IS_NULLSTR(argument)){
    char buf[MIL], out[MSL];
    out[0] = '\0';

    do_help( ch, "racial enemy" );
    sprintf( out, "\n\rYou may choose  the following races as your natural enemy:\n\r\n\r");
    for (i = 0; pc_race_table[i].name; i++){
      if (pc_race_table[i].show != TRUE)
	continue;
      sprintf( buf, "%2d. %-15.15s    ",
	       c + 1,
	       pc_race_table[i].name );
      strcat( out, buf );
      if (c++ % 2 == 1){
	strcat( out, "\n\r");
      }
    }
    if (c % 2 == 1)
      strcat( out, "\n\r");
    strcat( out, "\n\r" );
    write_to_buffer( d, out, 0 );
    sprintf( buf, chargen_table[CGEN_NEW_ENEM].prompt, c );
    write_to_buffer( d, buf, 0 );
    return;
  }
  else if ( (choice = atoi( argument)) < 0){
    write_to_buffer( d, "Invalid choice.\n\r", 0);
    chargen_new_race( d, "" );
    return;
  }
  /* set the race */
  for (i = 0; pc_race_table[i].name; i++){
    if (pc_race_table[i].show != TRUE)
      continue;
    c++;
    if (c == choice){
      ch->pcdata->enemy_race = i;
      ch->pcdata->race_kills[i] = 255;

      d->connected = CGEN_ROLL;
      wiznet("Newbie alert!  $N sighted.",ch,NULL,WIZ_NEWBIE,0,0);
      quick_start(d, ch, "reset");
      return;
    }
  }
  write_to_buffer( d, "Invalid choice.\n\r", 0);
  chargen_new_enemy( d, "" );
  return;
}



CHARGEN_FUN( chargen_new_race ){
  CHAR_DATA* ch = d->character;
  int i = 0, c = 0, choice;

  if (IS_NULLSTR(argument)){
    char buf[MIL], out[MSL];
    out[0] = '\0';
    sprintf( out, "The Forsaken Lands has the following starting races:\n\r\n\r");

    for (i = 0; pc_race_table[i].name; i++){
      if (!pc_race_table[i].can_pick)
	continue;
      sprintf( buf, "%2d. %-15.15s %3d Exp    ",
	       c + 1,
	       pc_race_table[i].name,
	       pc_race_table[i].class_mult - 1500);
      strcat( out, buf );
      if (c++ % 2 == 1){
	strcat( out, "\n\r");
      }
    }
    if (c % 2 == 1)
      strcat( out, "\n\r");
    strcat( out, "\n\r" );
    write_to_buffer( d, out, 0 );
    write_to_buffer( d, "You may also modify your race to a custom race. See \"HELP QRACE\"\n\r", 0);
    sprintf( buf, chargen_table[CGEN_NEW_RACE].prompt, c );
    write_to_buffer( d, buf, 0 );
    return;
  }
  else if ( (choice = atoi( argument)) < 1){
    write_to_buffer( d, "Invalid choice.\n\r", 0);
    chargen_new_race( d, "" );
    return;
  }
  /* set the race */
  for (i = 0; pc_race_table[i].name; i++){
    if (!pc_race_table[i].can_pick)
      continue;
    else
      c++;
    if (c == choice){
      ch->race = i;
      d->connected = CGEN_CON_RACE;
      return;
    }
  }
  write_to_buffer( d, "Invalid choice.\n\r", 0);
  chargen_new_race( d, "" );
  return;
}

CHARGEN_FUN( chargen_con_race ){
  CHAR_DATA* ch = d->character;
  char buf[MIL];

  if (IS_NULLSTR(argument)){
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;

    output = new_buf();

    /* get help text */
    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next ){
      if (pHelp->level != HELP_ALL)
	continue;
      else if ( !is_name( pc_race_table[ch->race].name, pHelp->keyword))
	continue;

      add_buf(output, pHelp->text);
      found = TRUE;
      break;
    }

    if (!found){
      add_buf(output, "No help on that race.\n\r" );
    }

    sprintf( buf, chargen_table[CGEN_CON_RACE].prompt,
	     capitalize(pc_race_table[ch->race].name) );
    add_buf(output, buf);

    page_to_char(buf_string(output),ch);
    free_buf(output);
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    if (ch->race == grn_illithid)
      d->connected = CGEN_NEW_TELE;
    else
      d->connected = CGEN_NEW_ADVE;
  }
  else{
    d->connected = CGEN_NEW_RACE;
  }
}

CHARGEN_FUN( chargen_new_clas ){
  CHAR_DATA* ch = d->character;
  int i = 0, c = 0, choice;

  if (IS_NULLSTR(argument)){
    char buf[MIL], out[MSL];
    out[0] = '\0';
    sprintf( out, "\n\rYou may choose from following professions:\n\r\n\r");

    for (i = 0; i < MAX_CLASS; i++){
      if (IS_NULLSTR(pc_race_table[ch->race].class_choice[i]))
	continue;
      sprintf( buf, "%2d. %-15.15s %3d Exp    ",
	       c + 1,
	       pc_race_table[ch->race].class_choice[i],
	       class_table[i].extra_exp + pc_race_table[ch->race].class_mult - 1500);
      strcat( out, buf );
      if (c++ % 2 == 1){
	strcat( out, "\n\r");
      }
    }
    if (c % 2 == 1)
      strcat( out, "\n\r");
    strcat( out, "\n\r" );

    write_to_buffer( d, out, 0 );
    sprintf( buf, chargen_table[CGEN_NEW_CLAS].prompt, c );
    write_to_buffer( d, buf, 0 );
    return;
  }
  else if ( (choice = atoi( argument)) < 1){
    write_to_buffer( d, "Invalid choice.\n\r", 0);
    chargen_new_clas( d, "" );
    return;
  }
  /* set the class */
  for (i = 0; i < MAX_CLASS; i++){
    if (IS_NULLSTR(pc_race_table[ch->race].class_choice[i]))
      continue;
    else
      c++;
    if (c == choice){
      ch->class = class_lookup( pc_race_table[ch->race].class_choice[i] );
      d->connected = CGEN_CON_CLAS;
      return;
    }
  }
  write_to_buffer( d, "Invalid choice.\n\r", 0);
  chargen_new_clas( d, "" );
  return;
}

CHARGEN_FUN( chargen_con_clas ){
  CHAR_DATA* ch = d->character;
  char buf[MIL];

  if (IS_NULLSTR(argument)){
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;

    output = new_buf();

    /* get help text */
    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next ){
      if (pHelp->level != HELP_ALL)
	continue;
      else if ( !is_name( class_table[ch->class].name, pHelp->keyword))
	continue;


      add_buf(output, pHelp->text);
      found = TRUE;
      break;
    }

    if (!found){
      add_buf(output, "No help on that class.\n\r" );
    }
    sprintf( buf, chargen_table[CGEN_CON_RACE].prompt,
	     capitalize(class_table[ch->class].name) );
    add_buf(output, buf);

    page_to_char(buf_string(output),ch);
    free_buf(output);
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    if (ch->class == gcn_ranger)
	d->connected = CGEN_NEW_ENEM;
    else{
      d->connected = CGEN_ROLL;
      wiznet("Newbie alert!  $N sighted.",ch,NULL,WIZ_NEWBIE,0,0);
      quick_start(d, ch, "reset");
    }
  }
  else{
    d->connected = CGEN_NEW_CLAS;
  }
}

CHARGEN_FUN( chargen_new_alig ){
  CHAR_DATA* ch = d->character;
  int align;
  char out[MIL];

  /* get the possible aligns */
  align = pc_race_table[ch->race].align;
  align &= class_table[ch->class].align;

  if (IS_NULLSTR(argument)){
    char buf[MIL];

    buf[0] = 0;

    if (IS_SET( align, ALIGN_GOOD))
      strcat( buf, "[G]ood " );
    if (IS_SET( align, ALIGN_NEUTRAL))
      strcat( buf, "[N]eutral " );
    if (IS_SET( align, ALIGN_EVIL))
      strcat( buf, "[E]vil" );

    do_help( ch, "chargen align note" );
    sprintf( out, chargen_table[CGEN_NEW_ALIG].prompt,
	     buf );
    write_to_buffer( d, out, 0 );
    return;
  }
  switch ( argument[0] ){
  case 'g': case 'G':
    if (!IS_SET(align, ALIGN_GOOD)){
      write_to_buffer( d, "Illegal choice.\n\r", 0);
      chargen_new_alig( d, "" );
      return;
    }
    ch->alignment = GOOD_THRESH;	break;
  case 'n': case 'N':
    if (!IS_SET(align, ALIGN_NEUTRAL)){
      write_to_buffer( d, "Illegal choice.\n\r", 0);
      chargen_new_alig( d, "" );
      return;
    }
    ch->alignment = 0;		break;
  case 'e': case 'E':
    if (!IS_SET(align, ALIGN_EVIL)){
      write_to_buffer( d, "Illegal choice.\n\r", 0);
      chargen_new_alig( d, "" );
      return;
    }
    ch->alignment = EVIL_THRESH;	break;
  default:
    chargen_new_alig( d, "" );
    return;
  }
  d->connected = CGEN_CON_ALIG;
}
CHARGEN_FUN( chargen_con_alig ){
  CHAR_DATA* ch = d->character;

  if (IS_NULLSTR(argument)){
    char buf[MIL], out[MIL];

    if (IS_GOOD(ch))
      sprintf( buf, "Good" );
    else if (IS_EVIL(ch))
      sprintf( buf, "Evil" );
    else
      sprintf( buf, "Neutral" );

    do_help( ch, buf );
    sprintf( out, chargen_table[CGEN_CON_ALIG].prompt,
	     buf );
    write_to_buffer(d, out, 0);
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    d->connected = CGEN_NEW_ETHO;
  }
  else{
    d->connected = CGEN_NEW_ALIG;
  }
}

CHARGEN_FUN( chargen_new_etho ){
  CHAR_DATA* ch = d->character;
  int etho;
  char out[MIL];

  /* get the possible aligns */
  etho = pc_race_table[ch->race].ethos;

  if (IS_NULLSTR(argument)){
    char buf[MIL];

    buf[0] = 0;

    if (IS_SET( etho, ETHOS_LAWFUL))
      strcat( buf, "[L]awful " );
    if (IS_SET( etho, ETHOS_NEUTRAL))
      strcat( buf, "[N]eutral " );
    if (IS_SET( etho, ETHOS_CHAOTIC))
      strcat( buf, "[C]haotic" );

    do_help( ch, "chargen ethos note" );

    sprintf( out, chargen_table[CGEN_NEW_ETHO].prompt,
	     buf );
    write_to_buffer(d, out, 0);
    return;
  }
  switch ( argument[0] ){
  case 'l': case 'L':
    if (!IS_SET(etho, ETHOS_LAWFUL)){
      write_to_buffer( d, "Illegal choice.\n\r", 0);
      chargen_new_etho( d, "" );
      return;
    }
    ch->pcdata->ethos =	CH_ETHOS_LAWFUL;	break;
  case 'n': case 'N':
    if (!IS_SET(etho, ETHOS_NEUTRAL)){
      write_to_buffer( d, "Illegal choice.\n\r", 0);
      chargen_new_etho( d, "" );
      return;
    }
    ch->pcdata->ethos =	CH_ETHOS_NEUTRAL;	break;
  case 'c': case 'C':
    if (!IS_SET(etho, ETHOS_CHAOTIC)){
      write_to_buffer( d, "Illegal choice.\n\r", 0);
      chargen_new_etho( d, "" );
      return;
    }
    ch->pcdata->ethos =	CH_ETHOS_CHAOTIC;	break;
  default:
    chargen_new_etho( d, "" );
    return;
  }
  d->connected = CGEN_CON_ETHO;
}
CHARGEN_FUN( chargen_con_etho ){
  CHAR_DATA* ch = d->character;

  if (IS_NULLSTR(argument)){
    int ethos = ch->pcdata->ethos;
    char buf[MIL], buf1[MIL], out[MIL];

    if (ethos == CH_ETHOS_LAWFUL)
      sprintf( buf, "Lawful" );
    else if (ethos == CH_ETHOS_NEUTRAL)
      sprintf( buf, "Neutral" );
    else
      sprintf( buf, "Chaotic" );

    if (IS_GOOD(ch))
      sprintf( buf1, "Good" );
    else if (IS_EVIL(ch))
      sprintf( buf1, "Evil" );
    else
      sprintf( buf1, "Neutral" );

    sprintf( out, "%s %s", buf, buf1 );
    do_help( ch, out );

    sprintf( buf, chargen_table[CGEN_CON_ALIG].prompt,
	     out );
    write_to_buffer(d, buf, 0);
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    d->connected = CGEN_NEW_RELI;
  }
  else{
    d->connected = CGEN_NEW_ETHO;
  }
}

CHARGEN_FUN( chargen_roll ){
  quick_start(d, d->character, argument);
}

CHARGEN_FUN( chargen_new_reli ){
  CHAR_DATA* ch = d->character;
  char buf[MIL], out[MSL];
  int god, align, ethos, c = 0, choice;

  //get align.
  if (IS_EVIL(ch))
    align = DIETY_ALIGN_EVIL;
  else if (IS_GOOD(ch))
    align = DIETY_ALIGN_GOOD;
  else
    align = DIETY_ALIGN_NEUTRAL;

  //get ethos.
  switch (ch->pcdata->ethos){
  case CH_ETHOS_LAWFUL	: ethos = DIETY_ETHOS_LAWFUL ;break;
  case CH_ETHOS_NEUTRAL	: ethos = DIETY_ETHOS_NEUTRAL;break;
  default		: ethos = DIETY_ETHOS_CHAOTIC;break;
  }

  if (IS_NULLSTR(argument)){
    char buf1[MIL];

    sprintf( out, "You may choose from following religious paths:\n\r");

    for ( god = 0; (deity_table[god].way != NULL && god < MAX_DIETY); god++ ){
      if (!IS_SET(deity_table[god].align, align)
	  || !IS_SET(deity_table[god].ethos, ethos) )
	continue;

      sprintf( buf1, "%s",
//	       path_table[deity_table[god].path].name,
	       deity_table[god].way);
      sprintf( buf, "%2d. %-15.15s",
	       c + 1,
	       buf1 );
      strcat(out, buf);

      if (c++ % 3 == 2){
	strcat( out, "\n\r");
      }
    }
    if (c % 3 == 2)
      strcat( out, "\n\r");
    strcat( out, "\n\r" );

    write_to_buffer( d, out, 0 );
    sprintf( buf, chargen_table[CGEN_NEW_RELI].prompt, c );
    write_to_buffer( d, buf, 0 );
    return;
  }
  else if ( (choice = atoi( argument)) < 1){
    write_to_buffer( d, "Invalid choice.\n\r", 0);
    chargen_new_reli( d, "" );
    return;
  }

  for ( god = 0; (deity_table[god].way != NULL && god < MAX_DIETY); god++ ){
    if (!IS_SET(deity_table[god].align, align)
	|| !IS_SET(deity_table[god].ethos, ethos) )
      continue;
    if (++c == choice){
      ch->pcdata->way = god;
      d->connected = CGEN_CON_RELI;
      return;
    }
  }
  write_to_buffer( d, "Invalid choice.\n\r", 0);
  chargen_new_reli( d, "" );
}
CHARGEN_FUN( chargen_con_reli ){
  CHAR_DATA* ch = d->character;
  char buf[MIL];

  if (IS_NULLSTR(argument)){
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;

    output = new_buf();

    /* get help text */
    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next ){
      if (pHelp->level != HELP_ALL)
	continue;
      else if ( !is_name( deity_table[ch->pcdata->way].way, pHelp->keyword))
	continue;

      add_buf(output, pHelp->text);
      found = TRUE;
      break;
    }

    if (!found){
      add_buf(output, "No help on that religious path.\n\r" );
    }
    sprintf( buf, chargen_table[CGEN_CON_RELI].prompt,
	     path_table[deity_table[ch->pcdata->way].path].name,
	     capitalize(deity_table[ch->pcdata->way].way) );
    add_buf(output, buf);

    page_to_char(buf_string(output),ch);
    free_buf(output);
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    ch->pcdata->deity = deity_table[ch->pcdata->way].god;
    d->connected = CGEN_NEW_HOME;
  }
  else{
    d->connected = CGEN_NEW_RELI;
  }
}

/* hometown selection */

CHARGEN_FUN( chargen_new_home ){
  CHAR_DATA* ch = d->character;
  char buf[MIL], out[MSL];
  int home, c = 0, choice;

  if (IS_NULLSTR(argument)){

    sprintf( out, "You may choose from following hometowns:\n\r");

    for ( home = 0; hometown_table[home].name && home < MAX_HOMETOWN; home++ ){
      if (!IS_NULLSTR(hometown_table[home].race)
	  && ch->race != race_lookup( hometown_table[home].race )){
	continue;
      }
      else if (IS_EVIL(ch)){
	if (hometown_table[home].e_vnum < 1)
	  continue;
      }
      else if (IS_GOOD(ch)){
	if (hometown_table[home].g_vnum < 1)
	  continue;
      }
      else if (hometown_table[home].n_vnum < 1)
	continue;

      sprintf( buf, "%2d. %-15.15s",
	       c + 1,
	       hometown_table[home].name );
      strcat(out, buf);

      if (c++ % 3 == 2){
	strcat( out, "\n\r");
      }
    }
    if (c % 3 == 2)
      strcat( out, "\n\r");
    strcat( out, "\n\r" );

    write_to_buffer( d, out, 0 );
    sprintf( buf, chargen_table[CGEN_NEW_HOME].prompt, c );
    write_to_buffer( d, buf, 0 );
    return;
  }
  else if ( (choice = atoi( argument)) < 1){
    write_to_buffer( d, "Invalid choice.\n\r", 0);
    chargen_new_home( d, "" );
    return;
  }
  for ( home = 0; hometown_table[home].name && home < MAX_HOMETOWN; home++ ){
    if (!IS_NULLSTR(hometown_table[home].race)
	&& ch->race != race_lookup( hometown_table[home].race )){
      continue;
    }
    else if (IS_EVIL(ch)){
      if (hometown_table[home].e_vnum < 1)
	continue;
    }
    else if (IS_GOOD(ch)){
      if (hometown_table[home].g_vnum < 1)
	continue;
    }
    else if (hometown_table[home].n_vnum < 1)
      continue;

    if (++c == choice){
      ch->hometown = home;
      d->connected = CGEN_CON_HOME;
      return;
    }
  }
  write_to_buffer( d, "Invalid choice.\n\r", 0);
  chargen_new_home( d, "" );
}
CHARGEN_FUN( chargen_con_home ){
  CHAR_DATA* ch = d->character;
  char buf[MIL];

  if (IS_NULLSTR(argument)){
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;

    output = new_buf();
    sprintf( buf, "'%s city'", hometown_table[ch->hometown].name);

    /* get help text */
    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next ){
      if ( !is_name( buf, pHelp->keyword))
	continue;

      add_buf(output, pHelp->text);
      found = TRUE;
      break;
    }

    if (!found){
      add_buf(output, "No help on that hometown.\n\r" );
    }
    sprintf( buf, chargen_table[CGEN_CON_HOME].prompt,
	     hometown_table[ch->hometown].name);
    add_buf(output, buf);

    page_to_char(buf_string(output),ch);
    free_buf(output);
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    d->connected = CGEN_ASK_DETA;
  }
  else{
    d->connected = CGEN_NEW_HOME;
  }
}

CHARGEN_FUN( chargen_con_break ){
  CHAR_DATA* ch = d->character;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, chargen_table[CGEN_CON_BREAK].prompt, 0);
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    DESCRIPTOR_DATA* d_old, *d_next;

    for ( d_old = descriptor_list; d_old != NULL; d_old = d_next ){
      d_next = d_old->next;
      if (d_old == d || d_old->character == NULL)
	continue;
      if (str_cmp(ch->name,d_old->original ? d_old->original->name : d_old->character->name))
	continue;
      close_socket(d_old);
    }
    if (check_reconnect(d,ch->name,TRUE))
      return;
    write_to_buffer(d,"Reconnect attempt failed.\n\r",0);
    if ( d->character != NULL ){
      free_char( d->character );
      d->character = NULL;
    }
    d->connected = CGEN_ENTER;
    return;
  }
  else{
    if ( d->character != NULL ){
      free_char( d->character );
      d->character = NULL;
    }
    d->connected = CGEN_ENTER;
    return;
  }
}

CHARGEN_FUN( chargen_ask_deta ){
  CHAR_DATA* ch = d->character;

  if (IS_NULLSTR(argument)){
    do_help( ch, "chargen detail note" );
    write_to_buffer( d, chargen_table[CGEN_ASK_DETA].prompt, 0);
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    ch->pcdata->fDetail = TRUE;
    d->connected = CGEN_DET_BODY;
  }
  else{
    d->connected = CGEN_NEW_DONE;
  }
}

/* detailed creation */
CHARGEN_FUN( chargen_det_body){
  CHAR_DATA* ch = d->character;
  char buf[MIL];
  int max = CGEN_MAX_BODY;
  int choice;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, "\n\rAvailable body types:\n\r", 0 );
    write_to_buffer( d, show_chargen_table( chargen_body_table, 20, 3 ), 0);
    sprintf( buf, chargen_table[CGEN_DET_BODY].prompt, CGEN_MAX_BODY - 1 );
    write_to_buffer( d, buf, 0);
    return;
  }
  else if ( (choice = atoi( argument )) < 1 || choice >= max){
    write_to_buffer(d, "Invalid choice.\n\r", 0);
    chargen_det_body( d, "" );
    return;
  }
  ch->pcdata->body_type = choice - 1;
  d->connected = CGEN_DET_FACE;
}

CHARGEN_FUN( chargen_det_face){
  CHAR_DATA* ch = d->character;
  char buf[MIL];
  int max = CGEN_MAX_FACE;
  int choice;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, "\n\rAvailable face types:\n\r", 0 );
    write_to_buffer( d, show_chargen_table( chargen_face_table, 20, 3 ), 0);
    sprintf( buf, chargen_table[CGEN_DET_FACE].prompt, max - 1 );
    write_to_buffer( d, buf, 0);
    return;
  }
  else if ( (choice = atoi( argument )) < 1 || choice >= max){
    write_to_buffer(d, "Invalid choice.\n\r", 0);
    chargen_det_face( d, "" );
    return;
  }
  ch->pcdata->face_type = choice - 1;
  d->connected = CGEN_DET_SKIN;
}
CHARGEN_FUN( chargen_det_skin){
  CHAR_DATA* ch = d->character;
  char buf[MIL];
  int max = CGEN_MAX_SKIN;
  int choice;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, "\n\rAvailable skin colors:\n\r", 0 );
    write_to_buffer( d, show_chargen_table( chargen_skin_table, 20, 3 ), 0);
    sprintf( buf, chargen_table[CGEN_DET_SKIN].prompt, max - 1 );
    write_to_buffer( d, buf, 0);
    return;
  }
  else if ( (choice = atoi( argument )) < 1 || choice >= max){
    write_to_buffer(d, "Invalid choice.\n\r", 0);
    chargen_det_skin( d, "" );
    return;
  }
  ch->pcdata->skin_color = choice - 1;
  d->connected = CGEN_DET_EYES;
}
CHARGEN_FUN( chargen_det_eyes){
  CHAR_DATA* ch = d->character;
  char buf[MIL];
  int max = CGEN_MAX_EYES;
  int choice;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, "\n\rAvailable eye colors:\n\r", 0 );
    write_to_buffer( d, show_chargen_table( chargen_eyes_table, 20, 3 ), 0);
    sprintf( buf, chargen_table[CGEN_DET_EYES].prompt, max - 1 );
    write_to_buffer( d, buf, 0);
    return;
  }
  else if ( (choice = atoi( argument )) < 1 || choice >= max){
    write_to_buffer(d, "Invalid choice.\n\r", 0);
    chargen_det_eyes( d, "" );
    return;
  }
  ch->pcdata->eye_color = choice - 1;
  d->connected = CGEN_DET_HAIR;
}
CHARGEN_FUN( chargen_det_hair){
  CHAR_DATA* ch = d->character;
  char buf[MIL];
  int max = CGEN_MAX_HAIR;
  int choice;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, "\n\rAvailable hair colors:\n\r", 0 );
    write_to_buffer( d, show_chargen_table( chargen_hair_table, 20, 3 ), 0);
    sprintf( buf, chargen_table[CGEN_DET_HAIR].prompt, max - 1 );
    write_to_buffer( d, buf, 0);
    return;
  }
  else if ( (choice = atoi( argument )) < 1 || choice >= max){
    write_to_buffer(d, "Invalid choice.\n\r", 0);
    chargen_det_hair( d, "" );
    return;
  }
  ch->pcdata->hair_color = choice - 1;

  //skip hair length and texture for bald people
  if (ch->pcdata->hair_color == 0)
    d->connected = CGEN_DET_HAIRF;
  else
    d->connected = CGEN_DET_HAIRL;
}
CHARGEN_FUN( chargen_det_haiL){
  CHAR_DATA* ch = d->character;
  char buf[MIL];
  int max = CGEN_MAX_HAIRL;
  int choice;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, "\n\rAvailable hair lengths:\n\r", 0 );
    write_to_buffer( d, show_chargen_table( chargen_hairl_table, 20, 3 ), 0);
    sprintf( buf, chargen_table[CGEN_DET_HAIRL].prompt, max - 1 );
    write_to_buffer( d, buf, 0);
    return;
  }
  else if ( (choice = atoi( argument )) < 1 || choice >= max){
    write_to_buffer(d, "Invalid choice.\n\r", 0);
    chargen_det_haiL( d, "" );
    return;
  }
  ch->pcdata->hair_length = choice - 1;
  /* if selected bald, set hair color to bald too */
  if (ch->pcdata->hair_length == 0){
    write_to_buffer(d, "Hair color set to \"bald\"\n\r", 0);
    ch->pcdata->hair_color = 0;
    d->connected = CGEN_DET_HAIRF;
  }
  else
    d->connected = CGEN_DET_HAIRT;
}
CHARGEN_FUN( chargen_det_haiT){
  CHAR_DATA* ch = d->character;
  char buf[MIL];
  int max = CGEN_MAX_HAIRT;
  int choice;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, "\n\rAvailable hair textures:\n\r", 0 );
    write_to_buffer( d, show_chargen_table( chargen_hairt_table, 20, 3 ), 0);
    sprintf( buf, chargen_table[CGEN_DET_HAIRT].prompt, max - 1 );
    write_to_buffer( d, buf, 0);
    return;
  }
  else if ( (choice = atoi( argument )) < 1 || choice >= max){
    write_to_buffer(d, "Invalid choice.\n\r", 0);
    chargen_det_haiT(d, "" );
    return;
  }
  ch->pcdata->hair_texture = choice - 1;
  /* if selected bald, set hair color to bald too */
  if (ch->pcdata->hair_texture == 0){
    write_to_buffer(d, "Hair color and length set to \"bald\"\n\r", 0);
    ch->pcdata->hair_color = ch->pcdata->hair_length = 0;
    d->connected = CGEN_DET_HAIRF;
  }
  else
    d->connected = CGEN_DET_HAIRF;
}
CHARGEN_FUN( chargen_det_haiF){
  CHAR_DATA* ch = d->character;
  char buf[MIL];
  int max = CGEN_MAX_HAIRF;
  int choice;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, "\n\rAvailable facial hair types:\n\r", 0 );
    write_to_buffer( d, show_chargen_table( chargen_hairf_table, 20, 3 ), 0);
    sprintf( buf, chargen_table[CGEN_DET_HAIRF].prompt, max - 1 );
    write_to_buffer( d, buf, 0);
    return;
  }
  else if ( (choice = atoi( argument )) < 1 || choice >= max){
    write_to_buffer(d, "Invalid choice.\n\r", 0);
    chargen_det_haiF( d, "" );
    return;
  }
  ch->pcdata->facial_hair = choice - 1;
  d->connected = CGEN_DET_PERK;
}

CHARGEN_FUN( chargen_det_perk){
  CHAR_DATA* ch = d->character;
  char buf[MIL];
  int max = CGEN_MAX_PERK;
  int choice;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, "\n\rAvailable perks:\n\r", 0 );
    write_to_buffer( d, show_chargen_table( chargen_perk_table, 20, 3 ), 0);
    sprintf( buf, chargen_table[CGEN_DET_PERK].prompt, max - 1 );
    write_to_buffer( d, buf, 0);
    return;
  }
  else if ( (choice = atoi( argument )) < 1 || choice >= max){
    write_to_buffer(d, "Invalid choice.\n\r", 0);
    chargen_det_perk( d, "" );
    return;
  }
  else if (chargen_perk_table[choice - 1].int1 == PERK_MERCY
	   && ch->class == gcn_dk){
    write_to_buffer(d, "Dark-knights may not be merciful.\n\r", 0);
    chargen_det_perk( d, "" );
    return;
  }
  ch->pcdata->perk_bits = choice - 1;
  d->connected = CGEN_CON_PERK;
}

CHARGEN_FUN( chargen_con_perk){
  CHAR_DATA* ch = d->character;
  char buf[MSL];

  if (IS_NULLSTR(argument)){
    sprintf( buf, chargen_table[CGEN_CON_PERK].prompt,
	     chargen_perk_table[ch->pcdata->perk_bits].name,
	     chargen_perk_table[ch->pcdata->perk_bits].string,
	     chargen_perk_table[ch->pcdata->perk_bits].name);
    write_to_buffer( d, buf, 0 );
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    d->connected = CGEN_DET_HAND;
    ch->pcdata->perk_bits = chargen_perk_table[ch->pcdata->perk_bits].int1;
    write_to_buffer( d, "You may view a person's details with \"bio\" or \"info\".\n\r\n\r", 0);
  }
  else{
    d->connected = CGEN_DET_PERK;
  }
}

CHARGEN_FUN( chargen_det_hand ){
  CHAR_DATA* ch = d->character;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, chargen_table[CGEN_DET_HAND].prompt, 0);
    return;
  }
  else if (UPPER(argument[0]) == 'L'){
    ch->pcdata->lefthanded = TRUE;
  }
  else if (UPPER(argument[0]) == 'R'){
    ch->pcdata->lefthanded = FALSE;
  }
  else{
    chargen_det_hand( d, "" );
    return;
  }
  d->connected = CGEN_DET_AGE;
}

CHARGEN_FUN( chargen_det_age ){
  CHAR_DATA* ch = d->character;
  int value;
  int age;

  if (IS_NULLSTR(argument)){
    write_to_buffer( d, chargen_table[CGEN_DET_AGE].prompt, 0);
    return;
  }
  if ( (value = atoi( argument )) < 15 || value > 50){
    write_to_buffer( d, "You must choose a range between 15 to 50%% of your natural lifespan.\n\r", 0);
    chargen_det_age( d, "" );
    return;
  }
  age = value * pc_race_table[ch->race].lifespan / 100;
  ch->pcdata->birth_date = age;
  d->connected = CGEN_CON_AGE;
}

CHARGEN_FUN( chargen_con_age){
  CHAR_DATA* ch = d->character;
  char buf[MSL];

  if (IS_NULLSTR(argument)){
    sprintf( buf, chargen_table[CGEN_CON_AGE].prompt,
	     ch->pcdata->birth_date );
    write_to_buffer( d, buf, 0 );
    return;
  }
  else if (UPPER(argument[0]) == 'Y'){
    if (ch->race == grn_illithid || ch->class == gcn_monk)
      d->connected = CGEN_NEW_DONE;
    else
      d->connected = CGEN_DET_WEAP;
    /* set the birth date */
    ch->pcdata->birth_date = mud_data.current_time - (ch->pcdata->birth_date * MAX_MONTHS * MAX_WEEKS * MAX_DAYS * MAX_HOURS * 30);
  }
  else{
    d->connected = CGEN_DET_AGE;
  }
}

CHARGEN_FUN( chargen_det_weap ){
  CHAR_DATA* ch = d->character;
  int i = 0, c = 0, choice;

  if (IS_NULLSTR(argument)){
    char buf[MIL], out[MSL];
    out[0] = '\0';
    sprintf( out, "Which weapon is your favorite?:\n\r\n\r");

    for (i = 0; weapon_table[i].name; i++){
      if (skill_table[*weapon_table[i].gsn].rating[ch->class] >= 10 )
	continue;
      else if (skill_table[*weapon_table[i].gsn].rating[ch->class] < 1 )
	continue;
      else if (skill_table[*weapon_table[i].gsn].skill_level[ch->class] > 1 )
	continue;
      sprintf( buf, "%2d. %-15.15s    ",
	       c + 1,
	       weapon_table[i].name);
      strcat( out, buf );
      if (c++ % 2 == 1){
	strcat( out, "\n\r");
      }
    }
    if (c % 2 == 1)
      strcat( out, "\n\r");
    strcat( out, "\n\r" );
    write_to_buffer( d, out, 0 );

    sprintf( buf, chargen_table[CGEN_DET_WEAP].prompt, c );
    write_to_buffer( d, buf, 0 );
    return;
  }
  else if ( (choice = atoi( argument)) < 1){
    write_to_buffer( d, "Invalid choice.\n\r", 0);
    chargen_det_weap( d, "" );
    return;
  }
  /* set the weapon */
  for (i = 0; weapon_table[i].name; i++){
    if (skill_table[*weapon_table[i].gsn].rating[ch->class] >= 10 )
      continue;
    else if (skill_table[*weapon_table[i].gsn].rating[ch->class] < 1 )
      continue;
    else if (skill_table[*weapon_table[i].gsn].skill_level[ch->class] > 1 )
      continue;
    else
      c++;
    if (c == choice){
      ch->pcdata->start_wep = i;
      d->connected = CGEN_NEW_DONE;
      return;
    }
  }
  write_to_buffer( d, "Invalid choice.\n\r", 0);
  chargen_det_weap( d, "" );
  return;
}

/* completes chargen */
CHARGEN_FUN( chargen_new_done){
  CHAR_DATA* ch = d->character;

  write_to_buffer( d, chargen_table[CGEN_NEW_DONE].prompt, 0);

  /* set race and skill stats here */
  set_race_stats( ch, ch->race );
  /* this has to be after setting race stats so perk mods work right */
  set_start_skills( ch, ch->class );

  save_char_obj( ch );

  d->connected = CGEN_MOTD;
}


/* deals with sockets that do not have chars yet, and the chargen */
void char_gen( DESCRIPTOR_DATA *d, char *argument ){
  int state = d->connected;

  if (d->connected >= CGEN_MAX)
    return;

  while ( isspace(*argument) )
    argument++;

  (*chargen_table[d->connected].func) (d, argument );

  if (d->connected != state && d->connected < CGEN_MAX)
    (*chargen_table[d->connected].func) (d, "" );
}

/* Deal with sockets that haven't logged in yet. */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    DESCRIPTOR_DATA *dt, *d_old;
    char buf[MSL], arg[MIL];
    CHAR_DATA *ch;
    char *pwdnew, *p, *password = NULL;
    int class,race,hometown,align,sn;
    int fOld = 0;

    /* quick start constants */
    const int start_stat = 8;

    while ( isspace(*argument) )
	argument++;
    ch = d->character;

    switch ( d->connected )
    {
    default:
	bug( "Nanny: bad d->connected %d.", d->connected );
	close_socket( d );
	return;
    case CON_GET_NAME:
	if ( argument[0] == '\0' )
	{
	    close_socket( d );
	    return;
	}
	argument[0] = UPPER(argument[0]);
	if ( !check_parse_name( argument ) )
	{
            write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
            free_char(d->character);
            d->character = NULL;
	    return;
	}
	nlogf( "Attempting to load %s.", argument );
	fOld = load_char( d, argument );
	/* check for invalid pfile */
	if (fOld == -1 ){
	  char buf [MIL];
	  write_to_buffer( d, "Corrupted Pfile detected, contact Implementors.\n\r", 0 );
	  sprintf(buf, "`!Corrupted Pfile detected: %s``", argument);
	  wiznet(buf, NULL,NULL,WIZ_LOGINS,0,0);
	  d->character = NULL;
	  close_socket( d );
	  return;
	}
	ch   = d->character;
	if (IS_SET(ch->act, PLR_DENY))
	{
	    nlogf( "Denying access to %s@%s.", argument, d->host );
	    write_to_buffer( d, "You are denied access.\n\r", 0 );
	    close_socket( d );
	    return;
	}
        /* Check bans */
        if ( check_ban(d->host,BAN_ALL) || check_ban(d->ident,BAN_ALL))
        {
            write_to_buffer( d, "Your site has been banned from The Forsaken Lands.\n\rContact Implementors if you belive you should have access.\n\r", 0 );
    	    close_socket(d);
	    return;
        }
        if ((check_ban(d->host,BAN_NEWBIES)
	     || check_ban(d->ident,BAN_NEWBIES))
	    && ch->level < 10
	    && !check_approved_name(d->character->name))
        {
            write_to_buffer(d, "New players are not allowed from your site.\n\rContact Implementors if you belive you should have access.\n\r", 0 );
	    close_socket(d);
	    return;
	}
	if ((check_ban(d->host,BAN_PERMIT) || check_ban(d->ident,BAN_PERMIT))&& !IS_SET(ch->act,PLR_PERMIT)
	    && !check_approved_name(d->character->name))
	{
	    write_to_buffer(d,"Your site has been banned from this mud.\n\r",0);
	    close_socket(d);
	    return;
	}
	if ( check_reconnect( d, argument, FALSE ) )
	    fOld = TRUE;
	else if ( mud_data.wizlock && !IS_IMMORTAL(ch))
	{
	    write_to_buffer( d, "The realms is wizlocked.\n\r", 0 );
	    close_socket( d );
	    return;
	}
	if ( fOld )
	{

	  /* Old player */
	  /* No Pasword */
	  if (ch->pcdata->pwd[0] == '\0' || !str_cmp("", ch->pcdata->pwd)){
	    char* buf;
	    write_to_buffer( d, "NULL password, resetting to player name...\n\r", 0 );
	    free_string( ch->pcdata->pwd );
	    buf = crypt( argument, ch->name );
	    ch->pcdata->pwd = str_dup(buf);

	    write_to_buffer( d, "Password: ", 0 );
	    write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	  }
	  /*regular */
	  write_to_buffer( d, "Password: ", 0 );
	  write_to_buffer( d, echo_off_str, 0 );
	  d->connected = CON_GET_OLD_PASSWORD;
	  return;
	}
	else
	{
	    /* New player */
	    if (mud_data.newlock)
	    {
                write_to_buffer( d, "The Forsaken Lands is newlocked.\n\r",
0 );
		close_socket( d );
		return;
	    }
	    nlogf( "%s@%s is a new player.", d->character->name, d->host );
	    for ( dt = descriptor_list; dt != NULL; dt = dt->next )
                if ( d != dt && dt->character != NULL && !str_cmp( dt->character->name, ch->name ) )
		{
        	    write_to_buffer( d, "That name is already in the character creation process.\n\r", 0 );
                    write_to_buffer( d, "Choose another name. ", 0 );
		    free_char( d->character );
		    d->character = NULL;
		    d->connected = CON_GET_NAME;
	    	    return;
		}
	    sprintf( buf, "Please pick a suitable roleplaying name.\n\rDon't use two word combinations, or words found in dictionaries\n\rDid I get that right, %s (Y/N)? ", argument );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
	break;
    case CON_GET_OLD_PASSWORD:
	write_to_buffer( d, "\n\r", 2 );
	if (strcmp( crypt( argument, ch->name ), ch->pcdata->pwd ))
	{
	    write_to_buffer( d, "Wrong password.\n\r", 0 );
	    close_socket( d );
	    return;
	}
	if ( IS_IMMORTAL(ch) && ch->pcdata->pwdimm[0] != '\0')
	{
	    write_to_buffer( d, "Please enter your Imm password: ", 0 );
	    d->connected = CON_CONFIRM_IMMORTAL;
	}
	else
	{
	    write_to_buffer( d, echo_on_str, 0 );
	    if (check_playing(d,ch->name))
	    	return;
	    if ( check_ban(d->host,BAN_ALL) || check_ban(d->ident,BAN_ALL))
	    {
	        write_to_buffer( d, "Your site has been banned from The Forsaken Lands.\n\r", 0 );
		close_socket(d);
		return;
	    }
    	    if ((check_ban(d->host,BAN_NEWBIES)
		 || check_ban(d->ident,BAN_NEWBIES))
		&& ch->level < 10
		&& !check_approved_name(d->character->name))
    	    {
	      write_to_buffer(d, "New players are not allowed from your site.\n\rContact Implementors if you belive you should have access.\n\r", 0 );
		close_socket(d);
		return;
	    }
	    if ((check_ban(d->host,BAN_PERMIT) || check_ban(d->ident,BAN_PERMIT))&& !IS_SET(ch->act,PLR_PERMIT)
	    && !check_approved_name(d->character->name))
	    {
	    	write_to_buffer(d,"Your site has been banned from this mud.\n\r",0);
	    	close_socket(d);
	    	return;
	    }
	    if ( check_reconnect( d, ch->name, TRUE ) ){
	      return;
	    }
	    else{
	      do_help( ch, "motd" );
	      d->connected = CON_READ_MOTD;
	    }
	}
	break;
    case CON_CONFIRM_IMMORTAL:
	    write_to_buffer( d, "\n\r", 2 );
	    if ( strcmp( crypt( argument, ch->pcdata->pwdimm ), ch->pcdata->pwdimm ))
	    {
		sprintf( log_buf, "%s@%s has failed login in room %d.", ch->name, d->host, ch->in_room->vnum );
		log_string( log_buf );
                wiznet(log_buf, NULL,NULL,WIZ_LOGINS,0,0);
	    	write_to_buffer( d, "Login failed.\n\r", 0 );
	    	close_socket( d );
	    	return;
	    }
	    write_to_buffer( d, echo_on_str, 0 );
	    if (check_playing(d,ch->name))
	    	return;
	    if ( check_reconnect( d, ch->name, TRUE ) )
	    	return;
	    do_help( ch, "imotd" );
	    d->connected = CON_READ_IMOTD;
	break;
    case CON_BREAK_CONNECT:
	switch( *argument )
	{
	case 'y' : case 'Y':
	    for ( d_old = descriptor_list; d_old != NULL; d_old = d_next )
	    {
		d_next = d_old->next;
		if (d_old == d || d_old->character == NULL)
		    continue;
		if (str_cmp(ch->name,d_old->original ? d_old->original->name : d_old->character->name))
		    continue;
		close_socket(d_old);
	    }
	    if ( check_ban(d->host,BAN_ALL) || check_ban(d->ident,BAN_ALL))
	    {
	        write_to_buffer( d, "Your site has been banned from The Forsaken Lands.\n\r", 0 );
		close_socket(d);
		return;
	    }
    	    if ((check_ban(d->host,BAN_NEWBIES)
		 || check_ban(d->ident,BAN_NEWBIES))
		&& ch->level < 10
		&& !check_approved_name(d->character->name))
    	    {
	      write_to_buffer(d, "New players are not allowed from your site.\n\rContact Implementors if you belive you should have access.\n\r", 0 );
		close_socket(d);
		return;
	    }
	    if ((check_ban(d->host,BAN_PERMIT) || check_ban(d->ident,BAN_PERMIT))&& !IS_SET(ch->act,PLR_PERMIT)
	    && !check_approved_name(d->character->name))
	    {
	    	write_to_buffer(d,"Your site has been banned from this mud.\n\r",0);
	    	close_socket(d);
	    	return;
	    }
	    if (check_reconnect(d,ch->name,TRUE))
		return;
	    write_to_buffer(d,"Reconnect attempt failed.\n\rName: ",0);
	    if ( d->character != NULL )
	    {
		free_char( d->character );
		d->character = NULL;
	    }
	    d->connected = CON_GET_NAME;
	    break;
	case 'n' : case 'N':
	    write_to_buffer(d,"Name: ",0);
	    if ( d->character != NULL )
	    {
		free_char( d->character );
		d->character = NULL;
	    }
	    d->connected = CON_GET_NAME;
	    break;
 	default:
	    write_to_buffer(d,"Please type Y or N? ",0);
	    break;
	}
	break;
    case CON_CONFIRM_NEW_NAME:
	switch ( *argument )
	{
	case 'y': case 'Y':
	    password = str_dup(load_purge(ch->name));
	    if (password[0] != '\0')
	    {
                sprintf( buf, "\n\r\n\rA player with that name existed before the purge.\n\rPlease enter the old password in order to use the name %s: %s", ch->name, echo_off_str );
	        write_to_buffer( d, buf, 0 );
	        free_string( ch->pcdata->pwd );
        	ch->pcdata->pwd = str_dup( password );
	        d->connected = CON_GET_PURGE_PASSWORD;
	        break;
	    }
            sprintf( buf, "\n\rPlease choose a password for %s: %s", ch->name, echo_off_str );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    break;
	case 'n': case 'N':
            write_to_buffer( d, "Ok, what is it, then? ", 0 );
	    free_char( d->character );
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;
	default:
	    write_to_buffer( d, "Please type Yes or No? ", 0 );
	    break;
	}
	break;
    case CON_GET_PURGE_PASSWORD:
	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "\n\rWrong password.\n\r", 0 );
	    close_socket( d );
	    return;
	}
	write_to_buffer( d, "\n\rThat is correct.\n\r", 0 );
        sprintf( buf, "\n\rPlease choose a password for %s: %s", ch->name, echo_off_str );
	write_to_buffer( d, buf, 0 );
        free_string( ch->pcdata->pwd );
	d->connected = CON_GET_NEW_PASSWORD;
	break;
    case CON_GET_NEW_PASSWORD:
	write_to_buffer( d, "\n\r", 2 );
	if ( strlen(argument) < 5 )
	{
            write_to_buffer( d,"Password must be at least five characters long.\n\rPassword: ",0 );
	    return;
	}
	pwdnew = crypt( argument, ch->name );
	for ( p = pwdnew; *p != '\0'; p++ )
	    if ( *p == '~' )
	    {
                write_to_buffer( d, "New password not acceptable, try again.\n\rPassword: ", 0 );
		return;
	    }
	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd = str_dup( pwdnew );
	write_to_buffer( d, "Please retype password: ", 0 );
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
	write_to_buffer( d, "\n\r", 2 );
	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
            write_to_buffer( d, "Passwords don't match.\n\rRetype password: ", 0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    return;
	}
	write_to_buffer( d, echo_on_str, 0 );
        write_to_buffer(d,"\n\rWhat is your last name (case is important) (you may leave this blank)? ",0);
	d->connected = CON_GET_FAMILY;
	break;
    case CON_GET_FAMILY:
	write_to_buffer( d, "\n\r", 2 );
/* Viri: lets give last names option to be more then one word
   one_argument_2(argument,arg);
*/
	if ( strlen(argument) > 15 )
	{
            write_to_buffer( d,"Last name must be less than fifteen characters long.\n\rFamily name: ",0 );
	    return;
	}
    	if ( !IS_NULLSTR(argument) && (!check_parse_name( argument ) || check_exist_name(argument) ))
    	{
            write_to_buffer( d,"Illegal last name, try another.\n\rFamily name: ",0 );
    	    return;
    	}
	arg[0] = UPPER(arg[0]);
	free_string( ch->pcdata->family );

	ch->pcdata->family = str_dup( capitalize(argument) );
        sprintf( buf, "Did I get that right, %s %s (Y/N) (make sure you have the proper capitalization)? ", ch->name, ch->pcdata->family );
        write_to_buffer( d, buf, 0 );
	d->connected = CON_CONFIRM_FAMILY;
	break;
    case CON_CONFIRM_FAMILY:
	switch ( *argument )
	{
	case 'y': case 'Y':
	    do_help(ch,"race help");
	    write_to_buffer( d, echo_on_str, 0 );
            write_to_buffer(d,"\n\rThe following races are available:\n\r  ",0);
	    for ( race = 1; pc_race_table[race].name != NULL; race++ )
	    {
	        if (!pc_race_table[race].can_pick)
		    continue;
	        write_to_buffer(d,pc_race_table[race].name,0);
                write_to_buffer(d," ",1);
	    }
            write_to_buffer(d,"\n\rWhat is your race (help for more information)? ",0);
	    d->connected = CON_GET_NEW_RACE;
	    break;
	case 'n': case 'N':
            write_to_buffer( d, "Ok, what is it, then (case is important) (you may leave this blank)? ", 0 );
	    d->connected = CON_GET_FAMILY;
	    break;
	default:
	    write_to_buffer( d, "Please type Yes or No? ", 0 );
	    break;
	}
	break;
    case CON_GET_NEW_RACE:
	one_argument(argument,arg);
	if (!str_cmp(arg,"help"))
	{
	    argument = one_argument(argument,arg);
	    if (argument[0] == '\0')
		do_help(ch,"race help");
	    else
		do_help(ch,argument);
            write_to_buffer(d,"What is your race (help for more information)? ",0);
	    break;
	}
	race = race_lookup(argument);
        if (race == 0 || !race_table[race].pc_race || !pc_race_table[race].can_pick)
	{
	    write_to_buffer(d,"That is not a valid race.\n\r",0);
	    write_to_buffer(d,"The following races are available:\n\r  ",0);
	    for ( race = 1; pc_race_table[race].name != NULL; race++ )
	    {
	        if (!pc_race_table[race].can_pick)
		    continue;
		write_to_buffer(d,race_table[race].name,0);
                write_to_buffer(d," ",1);
	    }
            write_to_buffer(d,"\n\rWhat is your race? (help for more information) ",0);
	    break;
	}
	ch->race = race;
	/* initialize stats */
	ch->affected_by = ch->affected_by|race_table[race].aff;
	ch->imm_flags   = ch->imm_flags|race_table[race].imm;
	ch->res_flags   = ch->res_flags|race_table[race].res;
	ch->vuln_flags  = ch->vuln_flags|race_table[race].vuln;
	ch->form        = race_table[race].form;
	ch->parts       = race_table[race].parts;
        ch->size = pc_race_table[ch->race].size;
        write_to_buffer( d, "\n\rWhat is your sex (M/F)? ", 0 );
	d->connected = CON_GET_NEW_SEX;
	break;
    case CON_GET_NEW_SEX:
	switch ( argument[0] )
	{
	case 'm': case 'M': ch->sex = SEX_MALE;
			    ch->pcdata->true_sex = SEX_MALE;
			    break;
	case 'f': case 'F': ch->sex = SEX_FEMALE;
			    ch->pcdata->true_sex = SEX_FEMALE;
			    break;
	default:
            write_to_buffer( d, "That's not a sex.\n\rWhat is your sex (M/F)? ", 0 );
	    return;
	}
        do_help(ch,"class help");
	write_to_buffer( d, echo_on_str, 0 );
        write_to_buffer(d,"\n\rThe following classes are available:\n\r  ",0);
        for ( class = 0; class < MAX_CLASS_NOW; class++ )
            if (pc_race_table[ch->race].class_choice[class] != NULL)

            {
                write_to_buffer(d,pc_race_table[ch->race].class_choice[class],0);
                write_to_buffer(d," ",1);
            }
        write_to_buffer(d,"\n\rWhat is your class (help for more information)? ",0);
        d->connected = CON_GET_NEW_CLASS;
	break;
    case CON_GET_NEW_CLASS:
	one_argument(argument,arg);
	if (!str_cmp(arg,"help"))
	{
	    argument = one_argument(argument,arg);
	    if (argument[0] == '\0')
                do_help(ch,"class help");
	    else
		do_help(ch,argument);
            write_to_buffer(d,"What is your class (help for more information)? ",0);
	    break;
	}
	class = class_lookup(argument);
        if (class > -1 && pc_race_table[ch->race].class_choice[class] == NULL)
            class = -1;
        if (class == -1)
        {
            write_to_buffer(d,"That is not a valid class.\n\r",0);
            write_to_buffer(d,"The following classes are available:\n\r  ",0);
            for ( class = 0; class < MAX_CLASS_NOW; class++ )
                if (pc_race_table[ch->race].class_choice[class] != NULL)
		  {
		    write_to_buffer(d,pc_race_table[ch->race].class_choice[class],0);
		    write_to_buffer(d," ",1);
		  }
            write_to_buffer(d,"\n\rWhat is your class (help for more information)? ",0);
            break;
	}
        ch->class = class;
	/* add skills */
	for ( sn = 0; sn < MAX_SKILL; sn++ )
            if ( skill_table[sn].name != NULL && skill_table[sn].rating[ch->class] < 10 && skill_table[sn].skill_level[0] != 69)
                ch->pcdata->learned[sn] = 1;
	if (ch->class == class_lookup("bard"))
	{
	    for ( sn = 0; sn < MAX_SONG; sn++ )
            	if ( song_table[sn].name != NULL && song_table[sn].rating < 10 && song_table[sn].skill_level != 69)
                    ch->pcdata->songlearned[sn] = 1;
	}
	wiznet("Newbie alert!  $N sighted.",ch,NULL,WIZ_NEWBIE,0,0);
	write_to_buffer( d, "\n\r\n\r", 2 );
        write_to_buffer( d, "\n\rRolling stats: Choose carefully, you do not get many trains later on.\n\r\n\r",0);
	d->connected = CON_QUICK_STATS;
	/* reset picks */
	ch->train = UMAX(75, roll_stat( ch )) - (5 * start_stat);
	/* reset starting stats */
	ch->perm_stat[STAT_STR] = start_stat;
	ch->perm_stat[STAT_INT] = start_stat;
	ch->perm_stat[STAT_WIS] = start_stat;
	ch->perm_stat[STAT_DEX] = start_stat;
	ch->perm_stat[STAT_CON] = start_stat;
	quick_start(d, ch, "");
	break;
    case CON_QUICK_STATS:
      quick_start(d, ch, argument);
      break;
    case CON_GET_ALIGNMENT:
	one_argument(argument,arg);
	if (arg[0] == '\0')
	{
            write_to_buffer( d, "Please type in your alignment: ", 0 );
	    return;
	}
	align = 0;
        if (ch->class == class_lookup("paladin"))
        {   if (!str_prefix(arg,"good"))  align=1; }
        else if (ch->class == class_lookup("dark-knight"))
        {   if (!str_prefix(arg,"evil"))  align=1; }
        else if (ch->class == class_lookup("necromancer"))
        {   if (!str_prefix(arg,"evil"))  align=1; }
        else if (ch->class == class_lookup("healer"))
        {   if (!str_prefix(arg,"good"))  align=1; }
        else if (ch->class == class_lookup("shaman"))
        {   if (!str_prefix(arg,"evil"))  align=1; }
        else if (ch->class == class_lookup("bard"))
        {   if (!str_prefix(arg,"neutral"))  align=1; }


        if (align==0)
        {
            write_to_buffer(d,"That's not a valid alignment.\n\r",0);
            write_to_buffer( d, "You may be: ",0);
            if (ch->class == class_lookup("paladin"))
                write_to_buffer( d, "good ",0);
            else if (ch->class == class_lookup("dark-knight"))
                write_to_buffer( d, "evil ",0);
            else if (ch->class == class_lookup("necromancer"))
                write_to_buffer( d, "evil ",0);
            else if (ch->class == class_lookup("healer"))
                write_to_buffer( d, "good ",0);
            else if (ch->class == class_lookup("shaman"))
                write_to_buffer( d, "evil ",0);
            else if (ch->class == class_lookup("bard"))
                write_to_buffer( d, "neutral ",0);
            write_to_buffer( d, "\n\r",0);
            write_to_buffer( d, "Which alignment? ",0);
            return;
        }
        if (!str_prefix(arg,"good"))   ch->alignment=750;
        else if (!str_prefix(arg,"neutral"))  ch->alignment=0;
        else if (!str_prefix(arg,"evil"))  ch->alignment=-750;

        write_to_buffer( d, "\n\r* Lawful means that you respect the laws of the towns\n\r",0);
        write_to_buffer( d, "     adhering to them at all times.\n\r",0);
        write_to_buffer( d, "\n\r* Neutral means that you are ambivalent to the laws of society.\n\r",0);
        write_to_buffer( d, "     You would obey them if there are enforcement officers on, otherwise\n\r",0);
        write_to_buffer( d, "     they do not pose a problem for you.\n\r",0);
        write_to_buffer( d, "\n\r* Chaotic means you care nothing at all for the laws of the towns.\n\r",0);
        write_to_buffer( d, "     This means that you would steal in front of an officer without thinking\n\r",0);
        write_to_buffer( d, "     twice about it.\n\r\n\r",0);
        if (ch->class == class_lookup("monk"))
            write_to_buffer( d, "You may be: lawful neutral.\n\r",0);
	else if (ch->race == race_lookup("minotaur"))
            write_to_buffer( d, "You may be: chaotic.\n\r",0);
	else if (ch->race == race_lookup("werebeast"))
            write_to_buffer( d, "You may be: neutral chaotic.\n\r",0);
	else
            write_to_buffer( d, "You may be: lawful neutral chaotic.\n\r",0);
        write_to_buffer( d, "What is your ethos? ",0);
        d->connected = CON_GET_ETHOS;
        break;
    case CON_GET_ETHOS:
        one_argument(argument,arg);
	if (arg[0] == '\0')		      ch->pcdata->ethos= -1;
        else if (ch->class == class_lookup("monk") && !str_prefix(arg,"chaotic"))   ch->pcdata->ethos= - 1;
        else if (ch->race == race_lookup("minotaur") && str_prefix(arg,"chaotic"))   ch->pcdata->ethos= - 1;
        else if (ch->race == race_lookup("werebeast") && !str_prefix(arg,"lawful"))   ch->pcdata->ethos= - 1;
        else if (!str_prefix(arg,"lawful"))   ch->pcdata->ethos = 0;
        else if (!str_prefix(arg,"neutral"))  ch->pcdata->ethos = 1;
        else if (!str_prefix(arg,"chaotic"))  ch->pcdata->ethos = 2;
	else ch->pcdata->ethos= -1;
	if (ch->pcdata->ethos == -1)
        {
            write_to_buffer(d,"That is not a valid ethos.\n\r",0);
            if (ch->class == class_lookup("monk"))
                write_to_buffer( d, "You may be: lawful neutral.\n\r",0);
	    else if (ch->race == race_lookup("minotaur"))
                write_to_buffer( d, "You may be: chaotic.\n\r",0);
	    else if (ch->race == race_lookup("werebeast"))
                write_to_buffer( d, "You may be: neutral chaotic.\n\r",0);
	    else
                write_to_buffer( d, "You may be: lawful neutral chaotic.\n\r",0);
            write_to_buffer(d,"What is your ethos? ",0);
	    break;
        }
        write_to_buffer(d,"\n\rThe following hometowns are available:\n\rIt is suggested that right now you choose Rheydin, since it is the largest city.\n\r  ",0);
        for ( hometown = 0; hometown < MAX_HOMETOWN; hometown++ )
        {
            if (hometown_table[hometown].name == NULL)
		break;
	    /* RACE ONLY CONDITIONS HERE */
	    if (!str_cmp(hometown_table[hometown].name, "Marak")
		&& ch->race != race_lookup("ogre"))
	      continue;
            if (!str_cmp(hometown_table[hometown].name, "Gal'Ranidon")
                && ch->race != race_lookup("duergar"))
              continue;
            if (!str_cmp(hometown_table[hometown].name, "Shasarazade")
		&& ch->race != race_lookup("elf"))
	      continue;
            if (!str_cmp(hometown_table[hometown].name, "Shire")
                && ch->race != race_lookup("halfling"))
              continue;
            if (!str_cmp(hometown_table[hometown].name, "Xymerria")
		&& ch->race != race_lookup("drow"))
	      continue;
            if (!str_cmp(hometown_table[hometown].name, "Falen_Dara")
		&& ch->race != race_lookup("slith"))
	      continue;
            if (!str_cmp(hometown_table[hometown].name, "Illithid-Hive")
		&& ch->race != race_lookup("illithid"))
	      continue;
            if (!str_cmp(hometown_table[hometown].name, "Tarandue")
		&& ch->race != race_lookup("gnome"))
	      continue;
 	    if ((ch->alignment > 0 && hometown_table[hometown].g_vnum != 0)
		|| (ch->alignment < 0 && hometown_table[hometown].e_vnum != 0)
		|| (ch->alignment == 0 && hometown_table[hometown].n_vnum != 0)){
	      write_to_buffer(d,hometown_table[hometown].name,0);
	      write_to_buffer(d," ",1);
            }
	}
        write_to_buffer(d,"\n\rWhat is your hometown (you may change your hometown later on)? ",0);
        d->connected = CON_GET_HOMETOWN;
	break;
    case CON_GET_HOMETOWN:
	one_argument(argument,arg);
        hometown = hometown_lookup(argument);
        if (hometown > -1) {
	  if ( (ch->alignment > 0 && hometown_table[hometown].g_vnum == 0)
	       || (ch->alignment < 0 && hometown_table[hometown].e_vnum == 0)
	       || (ch->alignment == 0 &&
		   hometown_table[hometown].n_vnum == 0) ) {
	    hometown = -1;
	  }
	  else if (hometown >= 0){
	    /* RACE ONLY CONDITIONS HERE */
	    if (LOWER(argument[0]) == LOWER('M')
		&& hometown > -1
		&& !str_cmp(hometown_table[hometown].name, "Marak")
		&& ch->race != race_lookup("ogre"))
	      hometown = -1;
	    if (LOWER(argument[0]) == LOWER('G')
		&& hometown > -1
		&& !str_cmp(hometown_table[hometown].name, "Gal'Ranidon")
		&& ch->race != race_lookup("duergar"))
	      hometown = -1;
	    if (LOWER(argument[0]) == LOWER('S')
		&& hometown > -1
		&& !str_cmp(hometown_table[hometown].name, "Shasarazade")
		&& ch->race != race_lookup("elf"))
	      hometown = -1;
	    if (LOWER(argument[0]) == LOWER('S')
		&& hometown > -1
		&& !str_cmp(hometown_table[hometown].name, "Shire")
		&& ch->race != race_lookup("halfling"))
	      hometown = -1;
	    else if (LOWER(argument[0]) == LOWER('X')
		     && hometown > -1
		&& !str_cmp(hometown_table[hometown].name, "Xymerria")
		&& ch->race != race_lookup("drow"))
	      hometown = -1;
	    if (LOWER(argument[0]) == LOWER('F')
		&& hometown > -1
		&& !str_cmp(hometown_table[hometown].name, "Falen_Dara")
		&& ch->race != race_lookup("slith"))
	      hometown = -1;
	    if (LOWER(argument[0]) == LOWER('I')
		&& hometown > -1
		&& !str_cmp(hometown_table[hometown].name, "Illithid-Hive")
		&& ch->race != race_lookup("illithid"))
	      hometown = -1;
	    if (LOWER(argument[0]) == LOWER('T')
		&& hometown > -1
		&& !str_cmp(hometown_table[hometown].name, "Tarandue")
		&& ch->race != race_lookup("gnome"))
	    hometown = -1;
	  }
	}
        if (hometown == -1){
	  int temp = hometown_lookup( argument );
	  char buf[MIL];
	  sprintf( buf, "%s is not a valid hometown.\n\r", temp > -1 ? hometown_table[temp].name : "That");
	  write_to_buffer(d,buf,0);
	  write_to_buffer(d,"The following hometowns are available:\n\r  ",0);
	  for ( hometown = 0; hometown < MAX_HOMETOWN; hometown++ ){
	    if (hometown_table[hometown].name == NULL)
	      break;
	    /* RACE ONLY CONDITIONS HERE */
	    if (!str_cmp(hometown_table[hometown].name, "Marak")
		&& ch->race != race_lookup("ogre"))
	      continue;
            if (!str_cmp(hometown_table[hometown].name, "Gal'Ranidon")
                && ch->race != race_lookup("duergar"))
              continue;
            if (!str_cmp(hometown_table[hometown].name, "Shasarazade")
		&& ch->race != race_lookup("elf"))
	      continue;
            if (!str_cmp(hometown_table[hometown].name, "Shire")
                && ch->race != race_lookup("halfling"))
              continue;
            if (!str_cmp(hometown_table[hometown].name, "Xymerria")
		&& ch->race != race_lookup("drow"))
	      continue;
            if (!str_cmp(hometown_table[hometown].name, "Falen_Dara")
		&& ch->race != race_lookup("slith"))
	      continue;
            if (!str_cmp(hometown_table[hometown].name, "Illithid-Hive")
		&& ch->race != race_lookup("illithid"))
	      continue;
            if (!str_cmp(hometown_table[hometown].name, "Tarandue")
		&& ch->race != race_lookup("gnome"))
	      continue;
	    if ((ch->alignment > 0 && hometown_table[hometown].g_vnum != 0)
                || (ch->alignment < 0 && hometown_table[hometown].e_vnum != 0)
                || (ch->alignment == 0 && hometown_table[hometown].n_vnum != 0)){
	      write_to_buffer(d,hometown_table[hometown].name,0);
	      write_to_buffer(d," ",1);
	    }
	  }
	  write_to_buffer(d,"\n\rWhat is your hometown (you may change your hometown later on)? ",0);
	  break;
	}
        ch->hometown = hometown;
	write_to_buffer(d,"\n\r",0);
	do_help(ch,"motd");
	d->connected = CON_READ_MOTD;
	break;
    case CON_READ_IMOTD:
	write_to_buffer(d,"\n\r",2);
	do_help( ch, "motd" );
	d->connected = CON_READ_MOTD;
	break;
    case CON_READ_MOTD:
        /* Check bans */
        if ( check_ban(d->host,BAN_ALL) || check_ban(d->ident,BAN_ALL))
        {
            write_to_buffer( d, "Your site has been banned from The Forsaken Lands.\n\r", 0 );
    	    close_socket(d);
	    return;
        }
        if ((check_ban(d->host,BAN_NEWBIES)
	     || check_ban(d->ident,BAN_NEWBIES))
	    && ch->level< 10
	    && !check_approved_name(d->character->name))
        {
	  write_to_buffer(d, "New players are not allowed from your site.\n\rContact Implementors if you belive you should have access.\n\r", 0 );
	    close_socket(d);
	    return;
	}
	if ((check_ban(d->host,BAN_PERMIT) || check_ban(d->ident,BAN_PERMIT)) && !IS_SET(ch->act,PLR_PERMIT)
	    && !check_approved_name(d->character->name))
	{
	    write_to_buffer(d,"Your site has been banned from this mud.\n\r",0);
	    close_socket(d);
	    return;
	}
	if ( ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0')
	{
	    write_to_buffer( d, "Warning! Null password!\n\r",0 );
	    write_to_buffer( d, "Please report old password with bug.\n\r",0);
            write_to_buffer( d, "Type 'password null <new password>' to fix.\n\r",0);
	}
        write_to_buffer( d, "\n\rWelcome to Aabahran: The Forsaken Lands.\n\r",0 );
	/* CHECK FOR VOTES HERE */
	if (has_votes( ch ) > 0){
	  char log_buf[MIL];
	  sprintf(log_buf, "%s has entered the vote booth.", ch->name);
	  wiznet(log_buf,NULL,NULL,WIZ_LOGINS,0,get_trust(ch));
	  d->connected = CON_VOTE_BOOTH;
	  interpret_vote(ch, "" );
	  break;
	}

	connect_char( d );
    }
}

bool check_delete_name( char *name )
{
    char strsave[MIL], nameread[MSL];
    FILE *fp;
    fclose( fpReserve );
    sprintf( strsave, "%s", ILLEGAL_NAME_FILE );
    if ( (fp = fopen( strsave, "r" ) ) != NULL )
    {
        for ( ; ; )
	{
            if( fscanf (fp, " %s", nameread) < 0 ){
                nameread[0] = '\0';
            }
            if ( !str_cmp( nameread, "END" ) )
                break;
            else if (is_name(name,nameread))
		return TRUE;
        }
    }
    else
	fp = fopen( NULL_FILE, "r" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return FALSE;
}

bool check_approved_name( char *name )
{
    char strsave[MIL], nameread[MSL];
    FILE *fp;
    fclose( fpReserve );
    sprintf( strsave, "%s", APPROVED_NAME_FILE );
    if ( (fp = fopen( strsave, "r" ) ) != NULL )
    {
        for ( ; ; )
	{
            if( fscanf (fp, " %s", nameread) < 0 ){
                nameread[0] = '\n';
            }
            if ( !str_cmp( nameread, "END" ) )
                break;
            else if (is_name(name,nameread))
		return TRUE;
        }
    }
    else
	fp = fopen( NULL_FILE, "r" );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return FALSE;
}

/* Parse a name for acceptability. */
bool check_parse_name( char *name )
{
    /* Reserved words. */
    if ( is_auto_name("all auto self someone something the you none fucker shit fuck shitter shiter noone everyone everybody nobody nothing cunt beaver justice knight nexus savant syndicate royal warmaster leader god negro nigga bitch duh suck sucker sex cum smut horny hairy chink spick nigger gook swear doh piss pissed fucking finger pussy boner hardon ass asshole fuckyou",  name) )
	return FALSE;
    if (!str_prefix("imm", name))
	return FALSE;
    if (!str_prefix("imp", name))
	return FALSE;
    if (check_delete_name( name ))
	return FALSE;
    /* Length restrictions. */
    if ( strlen(name) <  2 )
	return FALSE;
    if ( strlen(name) > 12 )
	return FALSE;
    /* Alphanumerics only. Lock out IllIll people. */
    {
	char *pc;
	int first = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha(*pc) )
		return FALSE;
	    if ( isupper(*pc) && !first)
		*pc = LOWER(*pc);
	    first = FALSE;
	}
    }
    /* Prevent players from naming themselves after mobs. */
    {
	extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
	MOB_INDEX_DATA *pMobIndex;
	int iHash;
	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
            for (pMobIndex = mob_index_hash[iHash]; pMobIndex != NULL; pMobIndex = pMobIndex->next)
		if ( is_name( name, pMobIndex->player_name ) )
		    return FALSE;
    }
    return TRUE;
}

/* Look for link-dead player to reconnect. */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch, *rch;
    for ( ch = player_list; ch != NULL; ch = ch->next_player )
        if ((!fConn || ch->desc == NULL) && !str_cmp(d->character->name,ch->name))
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
		if (IS_IMMORTAL(d->character))
		{
		    free_string( d->character->pcdata->pwdimm );
		    d->character->pcdata->pwdimm = str_dup( ch->pcdata->pwdimm );
		}
	    }
	    else
	    {
	      /* temporarly set extract status for gens */
	      d->character->extracted = TRUE;
	      free_char( d->character );
	      d->character->extracted = FALSE;
	      d->character = ch;
	      ch->desc     = d;
	      ch->timer    = 0;
	      ch->idle    = 0;
	      ch->logon    = mud_data.current_time;
		if (ch->pcdata->member)
		  ch->pcdata->member->last_update = mud_data.current_time;
		if (buf_string(ch->pcdata->buffer)[0] != '\0')
                    send_to_char("Reconnecting. Type replay to see missed tells.\n\r", ch );
                for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
                    if (get_trust(rch) >= ch->invis_level && ch->desc)
			act_new( "$n has snapped out of $s trance.", ch, NULL, rch, TO_VICT, POS_RESTING + 100 );
		sprintf( log_buf, "%s@%s has reconnected in room %d.", ch->name, d->host, ch->in_room->vnum );
		log_string( log_buf );
                wiznet(log_buf, NULL,NULL,WIZ_LINKS,0,get_trust(ch));
		d->connected = CON_PLAYING;
	    }
	    return TRUE;
	}
    return FALSE;
}

/* Check if already playing. */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
    DESCRIPTOR_DATA *dold;
    for ( dold = descriptor_list; dold; dold = dold->next )
      if ( dold != d && dold->character != NULL
	   && dold->connected != CGEN_ENTER
	   && dold->connected != CGEN_PASS
	   && !str_cmp( name, dold->original ? dold->original->name : dold->character->name ) )
	{
	  write_to_buffer( d, "That character is already playing.\n\r",0);
	  //	  write_to_buffer( d, "Do you wish to connect anyway (Y/N)?",0);
	  d->connected = CGEN_CON_BREAK;
	  return TRUE;
	}
    return FALSE;
}

void stop_idling( CHAR_DATA *ch )
{
    if (ch == NULL || ch->desc == NULL || ch->desc->connected != CON_PLAYING || ch->was_in_room == NULL
	|| ch->in_room == NULL || ch->in_room->vnum != ROOM_VNUM_LIMBO)
      return;
    else{
      ROOM_INDEX_DATA* to_room = ch->was_in_room;
      ch->timer = 0;
      ch->idle = 0;
      char_from_room( ch );
      char_to_room( ch, to_room );
      ch->was_in_room     = to_room;
      act_new( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM, POS_RESTING );
    }
}

/* Write to one char. */
void send_to_char( const char *txt, CHAR_DATA * ch )
{
    int length, l, curlen = 0;
    char *a = (char *)txt, *b;
    length = strlen(txt);
    if ( txt != NULL)
      while (curlen < length && ch->desc != NULL)
        {
	  b = a;
	  l = 0;
	  while (curlen < length && *a != '`')
            {
	      l++;
	      curlen++;
	      a++;
            }
	  if (l)
	    write_to_buffer(ch->desc, b, l);
	  if (*a)
            {
	      a++;
	      curlen++;
                if (curlen < length)
		  {
                    process_color(ch, *a++);
                    curlen++;
		  }
                else
		  {
                    a++;
                    curlen++;
		  }
            }
        }
}

/* Send a page to one char. */
void page_to_char( const char *txt, CHAR_DATA *ch )
{
    if ( txt == NULL || ch->desc == NULL)
	return;
    if (ch->lines == 0 )
    {
	send_to_char(txt,ch);
	return;
    }
    ch->desc->showstr_head = alloc_mem(strlen(txt) + 1);
    strcpy(ch->desc->showstr_head,txt);
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string(ch->desc,"");
}

/* string pager */
void show_string(struct descriptor_data *d, char *input)
{
    char buffer[4*MSL], buf[MIL];
    register char *scan, *chk;
    int lines = 0, toggle = 1, show_lines;
    one_argument(input,buf);
    if (buf[0] != '\0')
    {
	if (d->showstr_head)
	{
            free_mem(d->showstr_head,strlen(d->showstr_head)+1);
	    d->showstr_head = 0;
	}
	d->showstr_point  = 0;
	return;
    }
    if (d->character)
	show_lines = d->character->lines;
    else
	show_lines = 0;
    for (scan = buffer; ; scan++, d->showstr_point++)
    {
        if (((*scan = *d->showstr_point) == '\n' || *scan == '\r') && (toggle = -toggle) < 0)
	    lines++;
	else if (!*scan || (show_lines > 0 && lines >= show_lines))
	{
            *scan = '\0';
            if (d->character)
                send_to_char(buffer,d->character);
            else
                write_to_buffer(d,buffer,strlen(buffer));
                for (chk = d->showstr_point; isspace(*chk); chk++);
                    if (!*chk)
                    {
                        if (d->showstr_head)
                        {
                            free_mem(d->showstr_head,strlen(d->showstr_head));
                            d->showstr_head = 0;
                        }
                        d->showstr_point  = 0;
                    }
	    return;
	}
    }
}

/* quick sex fixer */
void fix_sex(CHAR_DATA *ch)
{
    if (ch->sex < 0 || ch->sex > 2)
	ch->sex = IS_NPC(ch) ? 0 : ch->pcdata->true_sex;
}

void act_new( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type, int min_pos){

  static char * const he_she  [] = { "it",  "he",  "she" };
  static char * const him_her [] = { "it",  "him", "her" };
  static char * const his_her [] = { "its", "his", "her" };
  char buf[MSL], message[MSL], fname[MIL];
  CHAR_DATA *to, *vch = (CHAR_DATA *) arg2;
  OBJ_DATA *obj1 = (OBJ_DATA  *) arg1, *obj2 = (OBJ_DATA  *) arg2;
  bool fwatch = FALSE;
  const char *str;
  const char *i;
  char *point;
  Double_List * tmp_list;

  /* Discard null and zero-length messages. */
  if ( format == NULL || format[0] == '\0' || sizeof(format)== 0 )
    return;

  // discard null rooms and chars unless this is a format string./*
  if ((ch == NULL && type != TO_FORMAT)  || ch->in_room == NULL)
    return;

  to = ch->in_room->people;

//CASE TO_VICT
  if ( type == TO_VICT ) {
    if ( vch == NULL ) {
      char buf[MIL];
      sprintf(buf, "Act: null vch with TO_VICT. String: %s", format);
      bug( buf, 0);
      return;
    }//END IF vch==NULL

    if (vch->in_room == NULL) {
      return;
    }

    to = vch->in_room->people;
  }//END CASE TO_VIC

  if (type == TO_WATCHROOM) {
    to = NULL;
  }

  for ( ; to != NULL; to = to->next_in_room ) {

//Make sure person is connected.
    if ( !IS_NPC(to) && to->desc == NULL ) {
      continue;
    }

//Make sure then can hear it.
    if ( min_pos > 100 ) {
      min_pos -= 100;
      fwatch = TRUE;
    }
    if ( to->position < min_pos ) {
      continue;
    }

//EASY CASES.
    if ( (type == TO_CHAR) && to != ch )
      continue;
    if ( type == TO_VICT && ( to != vch || to == ch ) )
      continue;
    if ( type == TO_CHARVICT && to != vch )
      continue;
    if ( type == TO_ROOM )
      if ( to == ch)
	continue;

    if ( type == TO_IMMROOM ){
      if ( to == ch)
	continue;
      if (!IS_IMMORTAL(to))
	continue;
    }//END IF IMM_ROOM

    if ( type == TO_NOTVICT && (to == ch || to == vch) )
      continue;
//END OF E-Z

    point   = buf;
    str     = format;

//FORMAT STRING.
    while ( *str != '\0' ){

//LOOK FOR SPECIAL CHAR
      if ( *str != '$'){
	*point++ = *str++;
	continue;
      }//END IF FORMAT CHAER.
      ++str;

      if ( arg2 == NULL && *str >= 'A' && *str <= 'Z'){
	bug( "Act: missing arg2 for code %d.", *str );
	bugf( "string = %s", format);
	i = " <@@@> ";
      }
      else{
//THIS IS THE ACTUAL FORMATTING.
	switch ( *str ){
	default:  bug( "Act: bad code %d.", *str ); i = " <@@@> "; break;
	case 't': i = (char *) arg1;                            break;
	case 'T': i = (char *) arg2;                            break;
	case 'n': i = PERS( ch,  to  );                         break;
	case 'N': i = PERS( vch, to  );                         break;
	case 'e': i = (ch->doppel != NULL && !IS_IMMORTAL(to)
		       && is_affected(ch,gsn_doppelganger)) ?
		    he_she[URANGE(0,ch->doppel->sex,2)] :
		    he_she[URANGE(0,ch->sex,2)];
	break;
	case 'E': i = (vch->doppel != NULL && !IS_IMMORTAL(to)
		       && is_affected(vch,gsn_doppelganger)) ?
		    he_she[URANGE(0,vch->doppel->sex,2)] :
		    he_she[URANGE(0,vch->sex,2)];
	break;

	case 'm': i = (ch->doppel != NULL && !IS_IMMORTAL(to)
		       && is_affected(ch,gsn_doppelganger)) ?
		    him_her[URANGE(0,ch->doppel->sex,2)] :
		    him_her[URANGE(0,ch->sex,2)];
	break;
	case 'M':  i = (vch->doppel != NULL && !IS_IMMORTAL(to)
		       && is_affected(vch,gsn_doppelganger)) ?
		    him_her[URANGE(0,vch->doppel->sex,2)] :
		      him_her[URANGE(0,vch->sex,2)];
	break;
	case 's':  i = (ch->doppel != NULL && !IS_IMMORTAL(to)
		       && is_affected(ch,gsn_doppelganger)) ?
		     his_her[URANGE(0,ch->doppel->sex,2)] :
		     his_her[URANGE(0,ch->sex,2)];
	break;
	case 'S': i = (vch->doppel != NULL && !IS_IMMORTAL(to)
		       && is_affected(vch,gsn_doppelganger)) ?
		     his_her[URANGE(0,vch->doppel->sex,2)] :
		     his_her[URANGE(0,vch->sex,2)];
	break;
	case 'p': i = obj1 ? can_see_obj(to,obj1) ? obj1->short_descr : "something" : "error" ; break;
	case 'P': i = obj2 ? can_see_obj(to,obj2) ? obj2->short_descr : "something" : "error" ; break;
	case 'd': if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )  i = "door";
	else { i = arg2; } break;
	case 'g': i = (IS_NPC(ch)? "The One God": (ch->pcdata->deity[0] == '\0'? "The One God" : ch->pcdata->deity));break;
	case 'G': i = (IS_NPC(vch)? "The One God": (vch->pcdata->deity[0] == '\0'? "The One God" : vch->pcdata->deity));break;
	case 'c': i = (IS_NPC(ch)? "": (ch->pcdata->clan[0] == '\0'? "" : ch->pcdata->clan));break;
	case 'C': i = (IS_NPC(vch)? "": (vch->pcdata->clan[0] == '\0'? "" : vch->pcdata->clan));
	}//END SWITCH (FORMATTING)
      }//END BUG CCHECK.
      ++str;
//check for EOL
      while ( ( *point = *i ) != '\0' )
	++point, ++i;
    }//END THE FORMATTING LOOP

//ALL FORMATING DONE AT THIS POINT
//START SENDING TO CURRENT "to"

    *point++ = '\n';
    *point++ = '\r';
    *point++ = '\0';
    buf[0]   = UPPER(buf[0]);


    if (MOBtrigger){
      mprog_act_trigger(buf,to, ch, obj1, vch);
      if (IS_NPC(to) && HAS_TRIGGER_MOB(to, TRIG_ACT))
	p_act_trigger(buf, to, NULL, NULL, ch, obj1, vch, TRIG_ACT);
    }
    if (ch->in_room == NULL)
      return;

    if (to->desc)
      send_to_char(buf,to);
    if (fwatch && IS_NPC(to)
	&& IS_SET(to->special, SPECIAL_WATCH)
	&& to->summoner != NULL ){
      sprintf(message, "[%s] `2%s``", PERS2(to), buf);
      send_to_char(message, to->summoner);
      fwatch = FALSE;
    }
    else if (fwatch
	     && IS_NPC(to)
	     && IS_SET(to->special, SPECIAL_WATCH)
	     && to->summoner != NULL ){
      sprintf(message, "%s at %s reports: `#%s``", capitalize(PERS(to, to->summoner)), to->in_room->name ,buf);
      send_to_char(message, to->summoner);
      fwatch = FALSE;
    }
  }//END the for characters loop.

  // this loop handles a spectating room
  if ((type == TO_ROOM) || (type == TO_NOTVICT) || (type == TO_WATCHROOM)){
    to = NULL;
    tmp_list = ch->in_room->watch_vnums;
    while (tmp_list != NULL) {
      ROOM_INDEX_DATA* to_room = get_room_index ((long long)tmp_list->cur_entry);
      if (to_room == NULL){
	bug("non existant watchroom in room %d.", ch->in_room->vnum);
	break;
      }
      to = to_room->people;
      while (to != NULL) {

//Make sure person is connected.
	if ( !IS_NPC(to) && to->desc == NULL ) {
	  to = to->next_in_room;
	  continue;
	}

//Make sure then can hear it.
	if ( min_pos > 100 ) {
	  min_pos -= 100;
	  fwatch = TRUE;
	}

	if ( to->position < min_pos ) {
	  to = to->next_in_room;
	  continue;
	}

	if ( type == TO_ROOM ) {
	  if ( to == ch) {
	    to = to->next_in_room;
	    continue;
	  }
	}

	point   = buf;
	str     = format;

//FORMAT STRING.
	while ( *str != '\0' ) {

//LOOK FOR SPECIAL CHAR
	  if ( *str != '$') {
	    *point++ = *str++;
	    continue;
	  }//END IF FORMAT CHAER.
	  ++str;

	  if ( arg2 == NULL && *str >= 'A' && *str <= 'Z') {
	    bug( "Act: missing arg2 for code %d.", *str );
	    bugf( "string = %s", format);
	    i = " <@@@> ";
	  }
	  else {
//THIS IS THE ACTUAL FORMATTING.
	    switch ( *str ) {
	    default:  bug( "Act: bad code %d.", *str ); i = " <@@@> "; break;
	    case 't': i = (char *) arg1;                            break;
	    case 'T': i = (char *) arg2;                            break;
	    case 'n': i = PERS( ch,  to  );                         break;
	    case 'N': i = PERS( vch, to  );                         break;
	    case 'e': i = (ch->doppel != NULL && !IS_IMMORTAL(to)
			   && is_affected(ch,gsn_doppelganger)) ?
			he_she[URANGE(0,ch->doppel->sex,2)] :
			  he_she[URANGE(0,ch->sex,2)];
	    break;
	    case 'E': i = (vch->doppel != NULL && !IS_IMMORTAL(to)
			   && is_affected(vch,gsn_doppelganger)) ?
			he_she[URANGE(0,vch->doppel->sex,2)] :
			  he_she[URANGE(0,vch->sex,2)];
	    break;

	    case 'm': i = (ch->doppel != NULL && !IS_IMMORTAL(to)
			   && is_affected(ch,gsn_doppelganger)) ?
			him_her[URANGE(0,ch->doppel->sex,2)] :
			  him_her[URANGE(0,ch->sex,2)];
	    break;
	    case 'M':  i = (vch->doppel != NULL && !IS_IMMORTAL(to)
			    && is_affected(vch,gsn_doppelganger)) ?
			 him_her[URANGE(0,vch->doppel->sex,2)] :
			   him_her[URANGE(0,vch->sex,2)];
	    break;
	    case 's':  i = (ch->doppel != NULL && !IS_IMMORTAL(to)
			    && is_affected(ch,gsn_doppelganger)) ?
			 his_her[URANGE(0,ch->doppel->sex,2)] :
			   his_her[URANGE(0,ch->sex,2)];
	    break;
	    case 'S': i = (vch->doppel != NULL && !IS_IMMORTAL(to)
			   && is_affected(vch,gsn_doppelganger)) ?
			his_her[URANGE(0,vch->doppel->sex,2)] :
			  his_her[URANGE(0,vch->sex,2)];
	    break;
	    case 'p': i = can_see_obj(to,obj1) ? obj1->short_descr : "something"; break;
	    case 'P': i = can_see_obj(to,obj2) ? obj2->short_descr : "something"; break;
	    case 'd': if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )  i = "door";
	    else { one_argument( (char *) arg2, fname ); i = fname; } break;
	    case 'g': i = (IS_NPC(ch)? "The One God": (ch->pcdata->deity[0] == '\0'? "The One God" : ch->pcdata->deity));break;
	    case 'G': i = (IS_NPC(vch)? "The One God": (vch->pcdata->deity[0] == '\0'? "The One God" : vch->pcdata->deity));
	    case 'c': i = (IS_NPC(ch)? "": (ch->pcdata->clan[0] == '\0'? "" : ch->pcdata->clan));break;
	    case 'C': i = (IS_NPC(vch)? "": (vch->pcdata->clan[0] == '\0'? "" : vch->pcdata->clan));
	    }//END SWITCH (FORMATTING)
	  }//END BUG CCHECK.
	  ++str;

//check for EOL
	  while ( ( *point = *i ) != '\0' ){
	    ++point, ++i;
	  }
	}//END THE FORMATTING LOOP

//ALL FORMATING DONE AT THIS POINT
//START SENDING TO CURRENT "to"

	*point++ = '\n';
	*point++ = '\r';
	*point++ = '\0';
	buf[0]   = UPPER(buf[0]);

	if (MOBtrigger) {
	  mprog_act_trigger(buf,to, ch, obj1, vch);
	  if (IS_NPC(to) && HAS_TRIGGER_MOB(to, TRIG_ACT)) {
	    p_act_trigger(buf, to, NULL, NULL, ch, obj1, vch, TRIG_ACT);
	  }
	}
	if (ch->in_room == NULL)
	  return;

	if (to->desc) {
	  sendf (to, "[%s] %s", ch->in_room->name, buf);
	}
	if (fwatch && IS_NPC(to) && IS_SET(to->special, SPECIAL_WATCH) && to->summoner != NULL) {
	  sprintf(message, "%s tells you '`2%s``'", PERS(to, to->summoner), buf);
	  send_to_char(message, to->summoner);
	  fwatch = FALSE;
	}
	to = to->next_in_room;
      }
      tmp_list = tmp_list->next_node;
    }
  }// END to chars loop

  MOBtrigger = TRUE;

  /* ROOM/OBJ prog */
  if ( type == TO_ROOM || type == TO_NOTVICT)
    {
      OBJ_DATA *obj, *obj_next;
      CHAR_DATA *tch, *tch_next;

      point   = buf;
      str     = format;
      while( *str != '\0' )
	{
	  *point++ = *str++;
	}
      *point   = '\0';

      for( obj = ch->in_room->contents; obj; obj = obj_next )
	{
	  obj_next = obj->next_content;
	  if ( HAS_TRIGGER_OBJ( obj, TRIG_ACT ) )
	    p_act_trigger( buf, NULL, obj, NULL, ch, NULL, NULL, TRIG_ACT );
	}
      if (ch->in_room == NULL)
	return;

      for( tch = ch; tch; tch = tch_next )
	{
	  tch_next = tch->next_in_room;

	  for ( obj = tch->carrying; obj; obj = obj_next )
	    {
	      obj_next = obj->next_content;
	      if ( HAS_TRIGGER_OBJ( obj, TRIG_ACT ) )
		p_act_trigger( buf, NULL, obj, NULL, ch, NULL, NULL, TRIG_ACT );
	    }
	  if (ch->in_room == NULL)
	    return;
	}

      if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_ACT ) )
	p_act_trigger( buf, NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_ACT );
      if (ch->in_room == NULL)
	return;
    }
  return;
}

void sendf (CHAR_DATA *ch, char *fmt, ...)
{
    char buf [MSL];
    va_list args;
    va_start (args, fmt);
    vsnprintf (buf, MSL, fmt, args);
    va_end (args);
    send_to_char (buf, ch);
}

void bugf (char * fmt, ...)
{
    char buf [MSL];
    va_list args;
    va_start (args, fmt);
    vsnprintf (buf, MSL, fmt, args);
    va_end (args);
    bug (buf, 0);
}

void nlogf (char * fmt, ...)
{
    char buf [2*MSL];
    va_list args;
    va_start (args, fmt);
    vsnprintf (buf, 2*MSL, fmt, args);
    va_end (args);
    log_string (buf);
}

void dump_core(const char *why)
{
    pid_t pid;
    fprintf(stderr, "DEBUG: fork_and_dump_core: %s", why);
    fflush(NULL); /* flush all streams so fork doesn't mess things up */
    pid = fork();
    if (pid == -1)
        perror("fork_and_dump_core: fork failed");
    else if (pid == 0)
    {
        volatile int n = 0;
        n = 1 / n; /* die horribly */
        exit(0);
    }
    else
      waitpid(pid, 0, 0); /* wait for the core dump.. usefulness questioned */
    do_pipe(NULL, "echo bt|gdb ../src/clands core");
    //do_pipe(NULL, "rm core");
}



//////////////////
//Broadcast///////
//////////////////
//Sends a message to all characters in surrounding rooms
//using the standard act(..) formating string.

void broadcast(CHAR_DATA* ch, const char *string)
{
//counter
  int i;
  //Buffer pointer for exit we are working with.
  EXIT_DATA* cur_exit;

  //Buffer pointer for room we are looing in.
  ROOM_INDEX_DATA* cur_room;
  ROOM_INDEX_DATA* cent_room = ch->in_room;

  //And one more buffer for character.
  CHAR_DATA* cur_ch=NULL ;



//Check for user error :)
  if (cent_room == NULL || ch == NULL)
    return;

//We start cycling throughthe rooms.
  for (i=0; i < 6; i++)
    {
      //we make sure there is a valid exit here.
      if ( ((cur_exit = cent_room->exit[i]) == NULL) || ((cur_room = cur_exit->to_room) == NULL) )
	continue;
      //The we start running through characters in this room.
      //IF there are any to run thourgh.
      if( cur_room->people != NULL)
	{
	  for (cur_ch = cur_room->people; cur_ch != NULL; cur_ch = cur_ch->next_in_room)
	    {
	      if(cur_ch->position > POS_SLEEPING)
		send_to_char(string, cur_ch);
	    }
	}//END for if cur_room
    }//end for exits.
}//END BROADCAST

void set_cleric_skillset(CHAR_DATA* ch, int value)
{
  /*given that this will be used in few places
we make this a bit more redundent.  */
  int set_e = value;
  int set_g = 0;

  if (ch->class != class_lookup("cleric"))
    return;

  if (IS_GOOD(ch)){
      set_e = 0;
      set_g = value;
      ch->pcdata->learned[skill_lookup("stake")] = value;
      ch->pcdata->learned[skill_lookup("turn undead")] = value;
      ch->pcdata->learned[skill_lookup("earthquake")] = 0;
      ch->pcdata->learned[skill_lookup("detect good")] = 0;
    }
  else
    {
      ch->pcdata->learned[skill_lookup("earthquake")] = value;
      ch->pcdata->learned[skill_lookup("stake")] = 0;
      ch->pcdata->learned[skill_lookup("turn undead")] = 0;
      ch->pcdata->learned[skill_lookup("detect evil")] = 0;
    }
  //common
      ch->pcdata->learned[skill_lookup("detect good")] = set_e;
      ch->pcdata->learned[skill_lookup("detect evil")] = set_g;

      ch->pcdata->learned[skill_lookup("dispel good")] = set_e;
      ch->pcdata->learned[skill_lookup("dispel evil")] = set_g;

      ch->pcdata->learned[skill_lookup("path of deceit")] = set_e;
      ch->pcdata->learned[skill_lookup("ray of truth")] = set_g;

      ch->pcdata->learned[skill_lookup("portal")] = set_g;
      ch->pcdata->learned[skill_lookup("minister")] = set_e;

}

void set_race_stats( CHAR_DATA* ch, int race ){
  char buf[MIL];

  ch->race = race;

  if (ch->pcdata->birth_date == 0){
    int age;
    /* if age was not set yet, set it now */
    age = 15 * pc_race_table[ch->race].lifespan / 100;
    /* set the birth date */
    ch->pcdata->birth_date = mud_data.current_time - (age * MAX_MONTHS * MAX_WEEKS * MAX_DAYS * MAX_HOURS * 30);
  }
  /* initialize stats */
  ch->affected_by = race_table[race].aff;
  ch->imm_flags   = race_table[race].imm;
  ch->res_flags   = race_table[race].res;
  ch->vuln_flags  = race_table[race].vuln;
  ch->form        = race_table[race].form;
  ch->parts       = race_table[race].parts;
  ch->size	  = pc_race_table[race].size;

  /* were beasts animals */
  if (ch->race == grn_werebeast){
    ch->pcdata->beast_type = number_range(BEAST_RACE_TIGER, BEAST_RACE_FALCON);
  }

  /* set start things */
  if (ch->race == grn_illithid){
    ch->dam_type = 28;
    ch->sex = SEX_NEUTRAL;
    ch->pcdata->true_sex = SEX_NEUTRAL;
  }
  else
    ch->dam_type = 17;
  ch->pcdata->perm_hit = pc_race_table[ch->race].hit;
  ch->pcdata->perm_mana= pc_race_table[ch->race].mana;
  ch->max_hit = ch->pcdata->perm_hit;
  ch->max_mana= ch->pcdata->perm_mana;
  ch->level   = 1;
  ch->exp     = exp_per_level(ch,ch->level);
  ch->max_exp = ch->exp;
  ch->hit     = ch->max_hit;
  ch->mana    = ch->max_mana;
  ch->move    = ch->max_move;
  ch->train   = 3;
  ch->act	= PLR_AUTOEXIT|PLR_AUTOSAC|PLR_AUTOGOLD|PLR_AUTOSPLIT|PLR_AUTOASSIST|PLR_AUTOLOOT|PLR_NOSUMMON;
  ch->practice= pc_race_table[ch->race].practice;
  ch->gold	= 1000;
  if (ch->class == gcn_monk || ch->class == gcn_blademaster)
    ch->pcdata->anatomy[anatomy_lookup(pc_race_table[ch->race].anatomy)]  = 25;
  sprintf( buf, "the %s", title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
  set_title( ch, buf );
  ch->in_room =  get_room_index( ROOM_VNUM_PORTAL );
  ch->was_in_room = ch->in_room;
}


void set_start_perks( CHAR_DATA* ch ){

  if (IS_NPC(ch))
    return;

  /* +20% carry weight, -1 dex */
  if (IS_PERK(ch, PERK_OVERWEIGHT))
    ch->perm_stat[STAT_DEX] -= 1;
  /* +20% carried items, -1str */
  else if (IS_PERK(ch, PERK_NIMBLE))
    ch->perm_stat[STAT_STR] -= 1;

  /* +50% start hp, -25% start mana */
  else if (IS_PERK(ch, PERK_STOUT)){
    ch->pcdata->perm_hit = 3 * ch->pcdata->perm_hit / 2;
    ch->pcdata->perm_mana = 3 * ch->pcdata->perm_mana / 4;

    ch->max_hit = ch->pcdata->perm_hit;
    ch->max_mana = ch->pcdata->perm_mana;
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
  }
  /* +30% start mana, -20% start hp */
  else if (IS_PERK(ch, PERK_MAGICAL)){
    ch->pcdata->perm_hit = 4 * ch->pcdata->perm_hit / 5;
    ch->pcdata->perm_mana = 13 * ch->pcdata->perm_mana / 10;

    ch->max_hit = ch->pcdata->perm_hit;
    ch->max_mana = ch->pcdata->perm_mana;
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
  }

  /* 100% scrolls */
  else if (IS_PERK(ch, PERK_BOOKWORM)){
    if (ch->pcdata->learned[gsn_scrolls] > 0)
      ch->pcdata->learned[gsn_scrolls] = 100;
  }
  /* 100% haggle */
  else if (IS_PERK(ch, PERK_CHARISMATIC)){
    if (ch->pcdata->learned[gsn_haggle] > 0)
      ch->pcdata->learned[gsn_haggle] = 100;
  }
  /* 100% wands */
  else if (IS_PERK(ch, PERK_TINKERER)){
    if (ch->pcdata->learned[gsn_wands] > 0)
      ch->pcdata->learned[gsn_wands] = 100;
  }
  /* 100% staves */
  else if (IS_PERK(ch, PERK_ARTIFICER)){
    if (ch->pcdata->learned[gsn_staves] > 0)
      ch->pcdata->learned[gsn_staves] = 100;
  }
  /* 100% rescue */
  else if (IS_PERK(ch, PERK_HEROIC)){
    if (ch->pcdata->learned[gsn_rescue] > 0)
      ch->pcdata->learned[gsn_rescue] = 100;
  }


}

void set_start_skills( CHAR_DATA* ch, int class ){
  int sn = 0;

  ch->class = class;

  /* add skills */
  for ( sn = 0; sn < MAX_SKILL; sn++ ){
    if ( skill_table[sn].name != NULL
	 && skill_table[sn].rating[ch->class] < 10
	 && skill_table[sn].rating[ch->class] > 0
	 && skill_table[sn].skill_level[0] != 69)
      ch->pcdata->learned[sn] = 1;
  }

  if (ch->class == class_lookup("bard")){
    for ( sn = 0; sn < MAX_SONG; sn++ ){
      if ( song_table[sn].name != NULL
	   && song_table[sn].rating < 10
	   && song_table[sn].rating > 0
	   && song_table[sn].skill_level != 69)
	ch->pcdata->songlearned[sn] = 1;
    }
  }

  /* everyone gets recall */
  ch->pcdata->learned[gsn_recall] = 100;

  /* starting weapon skill */
  set_start_wep(ch);

/* set cleric skills */
  set_cleric_skillset(ch, 1);

/* race skills */
  set_race_skills(ch, 1);

/* perk related abilities */
  set_start_perks( ch );

/* page length */
  ch->lines = PAGELEN;
/* Set the Descripton NOT approved Bit */
  SET_BIT(ch->act2, PLR_DESC);

/* Set GAME BITS */
  SET_BIT(ch->game, GAME_AMOB);
  SET_BIT(ch->game, GAME_AOBJ);
  SET_BIT(ch->game, GAME_APC);
  SET_BIT(ch->game, GAME_ASHEATH);
  SET_BIT(ch->game, GAME_SEX);
  SET_BIT(ch->color, COLOR_ON);
  SET_BIT(ch->color, COLOR_DAMAGE);
  SET_BIT(ch->game, GAME_NEWSCORE);

  /* update any changed skills etc */
  update_skills( ch );
}

void set_start_wep(CHAR_DATA* ch)
{
  //sets the bonus start weapon based on the
  //practice weapon in class_table
  int weapon = 0;
  int i = 0;

  /* illithids dont get any weapon skills */
  if (ch->race  == race_lookup("illithid")){
    ch->pcdata->learned[*weapon_table[weapon_lookup("mace")].gsn] = 0;
    ch->pcdata->learned[*weapon_table[weapon_lookup("sword")].gsn] = 0;
    ch->pcdata->learned[*weapon_table[weapon_lookup("spear")].gsn] = 0;
    ch->pcdata->learned[*weapon_table[weapon_lookup("staff")].gsn] = 0;
    ch->pcdata->learned[*weapon_table[weapon_lookup("axe")].gsn] = 0;
    ch->pcdata->learned[*weapon_table[weapon_lookup("flail")].gsn] = 0;
    ch->pcdata->learned[*weapon_table[weapon_lookup("whip")].gsn] = 0;
    ch->pcdata->learned[*weapon_table[weapon_lookup("polearm")].gsn] = 0;
    ch->pcdata->learned[*weapon_table[weapon_lookup("dagger")].gsn] = 0;
    ch->pcdata->learned[gsn_hand_to_hand] = 40;
    ch->pcdata->start_wep = -1;
    return;
  }
  /* weapon chosen in creation */
  if (ch->pcdata->start_wep >= 0){
    ch->pcdata->learned[*weapon_table[ch->pcdata->start_wep].gsn] = 40;
    return;
  }
  /* defaults */
  if (ch->class == class_lookup("monk")){
    ch->pcdata->start_wep = -1;
    ch->pcdata->learned[gsn_hand_to_hand] = 40;
    return;
  }
  if (class_table[ch->class].weapon == OBJ_VNUM_SCHOOL_MACE)
    weapon = weapon_lookup("mace");
  else if (class_table[ch->class].weapon == OBJ_VNUM_SCHOOL_SWORD)
    weapon = weapon_lookup("sword");
  else if (class_table[ch->class].weapon == OBJ_VNUM_SCHOOL_SPEAR)
    weapon = weapon_lookup("spear");
  else if (class_table[ch->class].weapon == OBJ_VNUM_SCHOOL_STAFF)
    weapon = weapon_lookup("staff");
  else if (class_table[ch->class].weapon == OBJ_VNUM_SCHOOL_AXE)
    weapon = weapon_lookup("axe");
  else if (class_table[ch->class].weapon == OBJ_VNUM_SCHOOL_FLAIL)
    weapon = weapon_lookup("flail");
  else if (class_table[ch->class].weapon == OBJ_VNUM_SCHOOL_WHIP)
    weapon = weapon_lookup("whip");
  else if (class_table[ch->class].weapon == OBJ_VNUM_SCHOOL_POLEARM)
    weapon = weapon_lookup("polearm");
  else
    weapon = weapon_lookup("dagger");
  ch->pcdata->start_wep = weapon;

  /* check for race mastery */
  for (i = 0; i < 5; i++){
    if (pc_race_table[ch->race].skills[i] == NULL)
      break;
    if (skill_lookup(pc_race_table[ch->race].skills[i]) == *weapon_table[weapon].gsn)
      return;
  }
  ch->pcdata->learned[*weapon_table[weapon].gsn] = 40;
}


void set_race_skills(CHAR_DATA* ch, int value)
{
  int i, sn;
  bool fKeep = (value == -1);

  //transform for werebeasts
  if (ch->race == grn_werebeast){
    if (!fKeep){
      ch->pcdata->learned[gsn_transform] = value;
    }
    else if(ch->pcdata->learned[gsn_transform] < 1)
      ch->pcdata->learned[gsn_transform] = 1;
  }
  else
    ch->pcdata->learned[gsn_transform] = 0;

  //probe for werebeasts
  if (ch->race == grn_werebeast){
    if (!fKeep){
      ch->pcdata->learned[gsn_probe] = value;
    }
    else if(ch->pcdata->learned[gsn_probe] < 1)
      ch->pcdata->learned[gsn_probe] = 1;
  }

  //werepower for werebeast
  sn = skill_lookup("werepower");
  if (ch->race == grn_werebeast){
    if (!fKeep){
      ch->pcdata->learned[sn] = value;
    }
    else if(ch->pcdata->learned[sn] < 1)
      ch->pcdata->learned[sn] = 1;
  }
  else
    ch->pcdata->learned[sn] = 0;

  if (ch->race == race_lookup("slith") && IS_GOOD(ch)){
    if (!fKeep)
      ch->pcdata->learned[gsn_chameleon] = value;
    else if(ch->pcdata->learned[gsn_chameleon] < 1)
      ch->pcdata->learned[gsn_chameleon] = 1;
  }
  else
    ch->pcdata->learned[gsn_chameleon] = 0;

  if (ch->race == race_lookup("demon")){
    int gsn_demonfire = skill_lookup("demonfire");
    if (!fKeep)
      ch->pcdata->learned[gsn_corrupt] = value;
    else
      if (ch->pcdata->learned[gsn_corrupt] < 1)
	ch->pcdata->learned[gsn_corrupt] = 1;

    if (!fKeep)
      ch->pcdata->learned[gsn_demonfire] = value;
    else
      if (ch->pcdata->learned[gsn_demonfire] < 1)
	ch->pcdata->learned[gsn_demonfire] = 1;

    /* get sacred runes only if had sanced */
    if (ch->pcdata->learned[gsn_sanctuary]){
      if (!fKeep)
	ch->pcdata->learned[gsn_sacred_runes] = value;
      else{
	if (ch->pcdata->learned[gsn_sacred_runes] < 1)
	  ch->pcdata->learned[gsn_sacred_runes] = 1;
      }
      ch->pcdata->learned[gsn_sanctuary] = 0;
    }
  }
  else{
    int gsn_demonfire = skill_lookup("demonfire");
    ch->pcdata->learned[gsn_corrupt] = 0;
    ch->pcdata->learned[gsn_demonfire] = 0;
    ch->pcdata->learned[gsn_sacred_runes] = 0;
  }

  if (ch->race == race_lookup("undead")){
    if (!fKeep)
      ch->pcdata->learned[gsn_lifedrain] = value;
    else
      if(ch->pcdata->learned[gsn_lifedrain] < 1)
	ch->pcdata->learned[gsn_lifedrain] = 1;
  }
  else
    ch->pcdata->learned[gsn_lifedrain] = 0;

  if (ch->race == race_lookup("gnome")){
    int gsn_defuse = skill_lookup("defuse");
    if (!fKeep)
      ch->pcdata->learned[gsn_defuse] = value;
    else
	if(ch->pcdata->learned[gsn_defuse] < 1)
	  ch->pcdata->learned[gsn_defuse] = 1;
  }

  sn = skill_lookup("boulder throw");
  if (ch->race == grn_stone){
    int gsn_boulder = skill_lookup("boulder throw");
    if (!fKeep)
      ch->pcdata->learned[gsn_boulder] = value;
    else if(ch->pcdata->learned[gsn_boulder] < 1)
      ch->pcdata->learned[gsn_boulder] = 1;
  }
  else
    ch->pcdata->learned[sn] = 0;

  if (ch->race == race_lookup("faerie")){
    int gsn_ffire = skill_lookup("faerie fire");
    int gsn_ffog = skill_lookup("faerie fog");

    ch->pcdata->learned[gsn_ffire] = 0;
    ch->pcdata->learned[gsn_ffog] = 0;
  }

  if (ch->race == grn_feral){
    if (!fKeep)
      ch->pcdata->learned[gsn_fury] = value;
    else
      if(ch->pcdata->learned[gsn_fury] < 1)
	ch->pcdata->learned[gsn_fury] = 1;
  }
  else
    ch->pcdata->learned[gsn_fury] = 0;

  if (ch->race == grn_illithid){
    if (!fKeep)
      ch->pcdata->learned[gsn_cone] = value;
    else
      if(ch->pcdata->learned[gsn_cone] < 1)
	ch->pcdata->learned[gsn_cone] = 1;
  }
  else
    ch->pcdata->learned[gsn_cone] = 0;

  /* natural proficciencies */
  for (i = 0; i < 5; i++){
    if (pc_race_table[ch->race].skills[i] == NULL)
      break;
    if ( (sn = skill_lookup(pc_race_table[ch->race].skills[i])) < 1)
      continue;
    ch->pcdata->learned[sn] = 100;
  }
}


void update_skills( CHAR_DATA* ch)
{
  int sn = 0;


  sn = gsn_soul_tap;
  if (ch->class == gcn_necro && ch->pcdata->learned[sn] < 1){
    ch->pcdata->learned[sn] = 75;
  }

  if (ch->class == gcn_blademaster){
    sn = gsn_acupuncture;
    if (ch->pcdata->learned[sn] < 1){
      ch->pcdata->learned[sn] = 75;
    }
  }

  sn = gsn_dual_wield;
  if (ch->class == gcn_bard && ch->pcdata->learned[sn] < 1){
    ch->pcdata->learned[sn] = 75;
  }

  sn = gsn_fired;
  if (skill_table[sn].rating[ch->class] > 0 && skill_table[sn].rating[ch->class] < 10
      && skill_table[sn].skill_level[ch->class] > 0 && skill_table[sn].skill_level[ch->class] < 51
      && ch->pcdata->learned[sn] < 1){
    ch->pcdata->learned[sn] = 75;
  }

  sn = gsn_probe;
  if (ch->race == grn_werebeast && ch->pcdata->learned[sn] < 1){
    ch->pcdata->learned[sn] = 75;
  }

  sn = gsn_peek;
  if (ch->class == gcn_bard && ch->pcdata->learned[sn] < 1){
    ch->pcdata->learned[sn] = 75;
  }

  /* new brad update, only bards with trance are affected */
  if (ch->class == gcn_bard && ch->pcdata->learned[gsn_trance] != 0){

    //skills lost
    sn = gsn_trance;
    ch->pcdata->learned[sn] = 0;

    sn = skill_lookup("lure");
    ch->pcdata->learned[sn] = 0;

    //skills gained
    sn = gsn_mace;
    ch->pcdata->learned[sn] = 75;

    sn = gsn_staff;
    ch->pcdata->learned[sn] = 75;

    sn = gsn_pugil;
    ch->pcdata->learned[sn] = 75;

    sn = gsn_trip;
    ch->pcdata->learned[sn] = 75;

    sn = gsn_disarm;
    ch->pcdata->learned[sn] = 75;

    sn = gsn_dual_parry;
    ch->pcdata->learned[sn] = 75;

    sn = gsn_third_attack;
    ch->pcdata->learned[sn] = 75;

    sn = gsn_pick_lock;
    ch->pcdata->learned[sn] = 75;

    sn = gsn_steal;
    ch->pcdata->learned[sn] = 75;

    sn = gsn_sneak;
    ch->pcdata->learned[sn] = 75;

    //spells gained
    sn = gsn_identify;
    ch->pcdata->learned[sn] = 75;

    sn = skill_lookup("locate object");
    ch->pcdata->learned[sn] = 75;

    sn = skill_lookup("armor");
    ch->pcdata->learned[sn] = 75;

    sn = skill_lookup("magic missile");
    ch->pcdata->learned[sn] = 75;

    sn = skill_lookup("detect invis");
    ch->pcdata->learned[sn] = 75;

    sn = skill_lookup("cure serious");
    ch->pcdata->learned[sn] = 75;

    sn = skill_lookup("invisibility");
    ch->pcdata->learned[sn] = 75;

    sn = skill_lookup("word of recall");
    ch->pcdata->learned[sn] = 75;
  }



  /* new ranger udpate, only rangers with throw affected */
  if (ch->class == gcn_ranger && ch->pcdata->learned[gsn_throw] != 0){
    SKILL_DATA* nsk;

    /* remove all skills previously in ranger lore */
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("fletchery"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("bowyer"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("archery"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("beastmaster"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("doublesheath"))) != NULL)
      nskill_remove(ch, nsk );

    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("exotic expert"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("dagger expert"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("sword expert"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("mace expert"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("flail expert"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("axe expert"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("spear expert"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("staff expert"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("polearm expert"))) != NULL)
      nskill_remove(ch, nsk );
    if ( (nsk = nskill_find(ch->pcdata->newskills, skill_lookup("whip expert"))) != NULL)
      nskill_remove(ch, nsk );


    //remove throw
    ch->pcdata->learned[gsn_throw] = 0;

    //reset groups
    for (sn = 0; sn < MAX_GROUPS; sn++){
      ch->pcdata->ss_picked[sn] = 0;
    }

    //give practices if they cannot afford them
    if (ch->level > 45)
      ch->practice += 10;
  }


  /* werebeasts get transform and beastpower */
  sn = gsn_transform;
  if (ch->race == grn_werebeast){
    // fix beast race
    if (ch->pcdata->beast_type > BEAST_RACE_MAX){
      ch->pcdata->beast_type = ch->pcdata->beast_type % BEAST_RACE_MAX;
    }
    //fix transform
    if (ch->pcdata->learned[sn] < 1){
      ch->pcdata->learned[sn] = 75;
    }
  }
  sn = skill_lookup("werepower");
  if (ch->race == grn_werebeast){
    if (ch->pcdata->learned[sn] < 1){
      ch->pcdata->learned[sn] = 75;
    }
  }

  /* clerics get giant steel wall */
  if (ch->class == class_lookup("cleric")){
    sn = gsn_steel_wall;
    if (ch->pcdata->learned[sn] < 1){
      if (ch->level == 50)
	ch->pcdata->learned[sn] = 75;
      else
	ch->pcdata->learned[sn] = 1;
    }
  }
  /* healers get dispel evil */
  if (ch->class == class_lookup("healer")){
    sn = skill_lookup("dispel evil");
    if (ch->pcdata->learned[sn] < 1){
      if (ch->level == 50)
	ch->pcdata->learned[sn] = 75;
      else
	ch->pcdata->learned[sn] = 1;
    }
  }

  /* healers get stake */
  if (ch->class == class_lookup("healer")){
    sn = gsn_stake;
    if (ch->pcdata->learned[sn] < 1){
      if (ch->level == 50)
	ch->pcdata->learned[sn] = 75;
      else
	ch->pcdata->learned[sn] = 1;
    }
  }
}

bool purge_limited(CHAR_DATA* ch){
  struct tm *that_time;
  int month = 0;
  bool fPurged = FALSE;

  // Checks for hours played per month, if not enough, purge eq.
  that_time = gmtime(&mud_data.current_time);
  month = that_time->tm_mon;

  if (ch->pcdata->mplayed < 0)
    ch->pcdata->mplayed = 0;

  if(ch->pcdata->month != month){
    if (!IS_IMMORTAL(ch)
	&& !IS_SET(ch->act2,PLR_EQLIMIT)
	&& ch->pcdata->mplayed < 1000
	&& ch->level >= 15
	&& ch->played >= 60000){
      OBJ_DATA *obj, *obj_next;

/* check for no purge */
      if (IS_SET(ch->act2,PLR_NOPURGE)){
	ch->pcdata->mplayed = 0;
	ch->pcdata->month = month;
	return fPurged;
      }

      for ( obj = ch->carrying; obj != NULL; obj = obj_next ){
	obj_next = obj->next_content;

	/* Skip things that should not be purged */
	if (is_affected_obj(obj, gen_malform)
	    || IS_SET(obj->extra_flags, ITEM_HAS_OWNER))
	  continue;

	if (CAN_WEAR(obj, ITEM_UNIQUE)
	    || CAN_WEAR(obj, ITEM_RARE)
	    ||  IS_OBJ_STAT (obj, ITEM_HOLDS_RARE)){
	  obj_from_char( obj );
	  extract_obj( obj );
	  fPurged = TRUE;
	}
      }
      if (fPurged){
	sprintf(log_buf, "%s has been purged of eq.", ch->name );
	SET_BIT(ch->pcdata->messages, MSG_PURGE);
	log_string( log_buf );
	wiznet(log_buf,NULL,NULL,WIZ_LOGINS,0,get_trust(ch));
      }
    }
    ch->pcdata->mplayed = 0;
    ch->pcdata->month = month;
  }
  return fPurged;
}


//  05-18-00 Viri: Added set_cleric_skillset(ch);
//06-22-00 Viri: added update_skill
//01-08-01 Ath: added "holdsrare" items to eq purge
//01-22-02 Viri: Added purge_limited(..)
//05-21-01 Ath: added spectating rooms
