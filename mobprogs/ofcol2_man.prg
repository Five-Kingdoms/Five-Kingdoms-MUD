>rand_prog 3~
if rand (50)
	pmote whistles happily.
else
	smile $n
endif
~
>greet_prog 5~
if rand (33)
	tip $n
else
	if rand (50)
		say How are you doing, $N?
	else
		say Good day to you, $N.
	endif
endif
~
|

