

>all_greet_prog 100~
if ispc($n)
murder $n
endif
~

>act_prog p pops into existence.~
murder $n
~

>act_prog p has arrived through a gate.~
murder $n
~

>entry_prog 100~
if ispc($n)
trip $n
murder $n
endif
~
>rand_prog 3~
if rand(50)
  mpasound Water drips in the darkness.....
else
  mpasound Somebody screams in the distance.
endif
~

>fight_prog 20~
pmote ducks the attack and plunges a dagger into $n's back!
circle 
~
|
