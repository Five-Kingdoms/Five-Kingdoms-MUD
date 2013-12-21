>greet_prog 100~
if isnpc($n)
  break
endif
tell $n For a small price of 300 gold I will provide magical transport to Revelsport.
~

>bribe_prog 300~
pmote mutters an incanation as the runes about $n flare with power.
at 20601 echo A giant black gate fills the area with fog as $n arrives from Zalango.
gteleport $n 20601
mpjunk gold
~


>bribe_prog 1~
tell $n The price is 300 gold.
give 1.gold $n
~

| 
