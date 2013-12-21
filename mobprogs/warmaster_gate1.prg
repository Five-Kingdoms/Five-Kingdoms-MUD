>greet_prog 100~
if isnpc($n)
  break
endif
if iscabal($n) == 4
  tell $n For but a single coin of gold I will carry you across the lands.
  break
endif
~

>bribe_prog 1~
if iscabal($n) != 4
  'I cannot aid you $n for you are not a Warmaster.
  give 1. $n
  break
endif
pmote spreads its shimmering wings of pure white.
at 4013 echo $n arrives carried upon a beautifull unicorn.
gteleport $n 4013
mpjunk gold
~
| 
