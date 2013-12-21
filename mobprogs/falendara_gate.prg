>greet_prog 100~
if isnpc($n)
  break
endif
tell $n For a small price of 300 gold I will provide magical transport to Miruvhor.
~

>bribe_prog 300~
pmote mutters an incanation as the runes about $n flare with power.
at 10063 echo Spraying sand in all directions a gate from Falen Dara opens up.
gteleport $n 10063
mpjunk gold
~


>bribe_prog 1~
tell $n The price is 300 gold.
give 1.gold $n
~

| 
