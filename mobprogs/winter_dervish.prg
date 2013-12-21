>fight_prog 20~
if ispc($n)
if rand(50)
pmote gains a dangerous glint in his eyes and begins to twirl!
cast 'lightning' 
cast 'curse' 
endif
endif
~
>fight_prog 10~
if rand(50)
  c acid
else
  c 'flame arrow'
endif
~
>greet_prog 100~
if isnghost ($n)
if ispc($n)
murder $n
murder $n
endif
endif
~
>entry_prog 100~
if isnghost ($n)
if ispc($n)
murder $n
murder $n
endif
endif
~

>rand_prog 2~
if rand(50)
	mpasound You feel a slight breeze on your cheek.
else
	mpasound Lightning crackles through the air above you!
endif
~
| 
