>greet_prog 5~
if israce($n) == 3
	tell self shit
else
	say Go away before I decide to kill you.
endif
~
>rand_prog 3~
if rand(50)
	pmote punches an imaginary foe.
else
	if mobobj (5105)
		pmote points a black longsword at $R.
	endif
endif
~
|

