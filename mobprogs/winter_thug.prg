>entry_prog 100~
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

>all_greet_prog 100~
if ispc($n) then
murder $n
endif
~

>fight_prog 8~
pmote scoffs in your direction.
~

>fight_prog 18~
if sex($r) = 2 then
pmote looks at $r with a primitive lusty look in his eyes.
endif
~

>act_prog p spits on you~
say Yo man, what the hell?
poke thief
~
|
