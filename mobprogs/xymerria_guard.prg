>rand_prog 1~
if rand (50)
	pmote watches $R closely.
else
	say You better not cause any trouble in my town, $N
endif
~
>greet_prog 1~
if isevil($n)
	tip $n
else
	if isgood($n)
		say What the hell is a Lightwalker doing in this town!?
		murder $n
	else
		exa $n
	endif
endif
~
|

