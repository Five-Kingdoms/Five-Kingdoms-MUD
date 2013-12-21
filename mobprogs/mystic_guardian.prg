>all_greet_prog 100~
if ispc($n)
	if iscabal($n) == 12
		say Greetings $n.  Walk in Magic.
	endif
endif
~

>fight_prog 17~
if rand(10)
	cas 'faerie fire'
else
if rand(30)
	cas 'flame arrow'
else
if rand(40)
	cas icicle
else
	cas acid
endif
endif
endif
~
|
