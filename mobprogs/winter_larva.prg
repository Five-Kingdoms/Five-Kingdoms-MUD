>fight_prog 6~
if rand(50)
  pmote seems to absorb the shock of $n's attack!
else
  pmote opens its gaping jaws and strikes!
  cas poison
endif
~
>act_prog p pops into existence.~
if isnghost ($n)
yell $n, I don't want you here!
murder $n
c 'dispel magic'
endif
~

>all_greet_prog 100~
if isnghost ($n)
if ispc($n)
	murder $n
endif
endif
~

>entry_prog 100~
if isnghost ($n)
if ispc($n)
	murder $n
endif
endif
~

>rand_prog 4~
if rand(50)
	mpasound A strange crackling fills the air.
else
	mpasound You can hear something squirming nearby.
endif
~
|
