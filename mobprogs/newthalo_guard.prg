>rand_prog 1~
if rand (50)
	pmote watches $R closely.
else
	say You better not cause any trouble in my town, $N
endif
~
>greet_prog 1~
if isgood($n)
	tip $n
else
	if isevil($n)
		say I'm keeping my eye on you, $N.
	else
		exa $n
	endif
endif
~
|

