>greet_prog 20~
if israce($n) == 3
	say What do you want, $N?
else
	if isnghost ($n)
		yell How dare you invade my house, $N?
		murder $n
	endif
endif
~
>rand_prog 3~
if rand(50)
	pmote gives her servants some orders.
else
	if mobobj (5104)
		pmote cracks a snake headed whip at $R.
	endif
endif
~
|

