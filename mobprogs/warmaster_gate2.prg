>greet_prog 100~
if isnpc($n)
  break
endif
if iscabal($n) == 4
  tell $n You give shiny circle, I throw.	  
  break
endif


~

>bribe_prog 1~
if iscabal($n) != 4
  'I cannot aid you $n for you are not a Warmaster.
  give 1. $n
  break
endif
pmote grunts mightly as $e heaves.
at 9191 echo Screaming loudly $n falls from above.
gteleport $n 9191
mpjunk gold
~
| 
