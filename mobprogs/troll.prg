>rand_prog 3~
if rand (33)
	say Kill, kill, kill...
else
	if rand (50)
		snarl $r
	else
		growl $r
	endif
endif
~
>greet_prog 5~
if rand (50)
	say You ready to die, $N?
else
	say Die, die, die...
endif
~
|

