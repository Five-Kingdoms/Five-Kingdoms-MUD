>greet_prog 100~
if isnpc($n)
  break
endif
if charobj(30010)
 pmote glances at the tablet and loses all interest in $n.
else
  stand
  murder $n
endif
~

>act_prog p pops into existence.~
if isnpc($n)
  break
endif
if charobj(30010)
 pmote glances at the tablet and loses all interest in $n.
else
  stand
  murder $n
endif
~

>act_prog p has arrived through a gate.~
if isnpc($n)
  break
endif
if charobj(30010)
 pmote glances at the tablet and loses all interest in $n.
else
  stand
  murder $n
endif
~
|