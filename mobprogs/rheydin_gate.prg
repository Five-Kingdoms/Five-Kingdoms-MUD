>greet_prog 100~
if isnpc($n)
  break
endif
tell $n For a small price of 300 gold I will provide magical transport to Ulsym.
~

>bribe_prog 300~
pmote mutters an incanation as the runes about $n flare with power.
at 11500 echo A giant shimmering gate opens up as $n arrives.
gteleport $n 11500
mpjunk gold
~


>bribe_prog 1~
tell $n The price is 300 gold.
give 1.gold $n
~

| 
