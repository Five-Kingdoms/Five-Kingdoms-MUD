>greet_prog 100~
if isnghost ($n)
if ispc($n)
murder $n
endif
endif
~
>fight_prog 10~
if ispc($n)
if rand(50)
  pmote wreaks havok on $n's armor.
  cas 'faerie fire'
else
  c slow
endif
endif
~
>entry_prog 100~
if isnghost ($r)
if ispc($r)
murder $r
endif
endif
~

>rand_prog 4~
if rand(50)
	mpasound The heat of an intense fire is felt nearby.
else
	mpasound Someone utters some strange words in a spidery language.
endif
~
>death_prog 80~
pmote explodes into a ball of fire!
cas 'fireball'
~

>act_prog p arrives through a gate.~
if isnghost ($n)
c 'dispel magic' $n
murder $n
endif
~

>act_prog p pops into existence.~
if isnghost ($n)
c 'dispel magic' $n
murder $n
endif
~

| 
