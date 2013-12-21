>rand_prog 3~
if rand (50)
	pmote drools at the sight of food.
else
	if rand (50)
		growl
	else
		growl $r
	endif
endif
~
>greet_prog 5~
if rand (50)
	say Mmm..  Food!
else
	say Come closer, $N.
endif
~
|

