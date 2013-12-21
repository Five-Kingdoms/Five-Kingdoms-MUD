>greet_prog 20~
if israce($n) == 3
	bow $n
else
	yell How dare you intrude upon my house $N!
	c "burning hands" $n
endif
~
>rand_prog 3~
if rand(50)
	meditate
else
	if mobobj (5102)
		pmote snaps a snake headed whip in the air.
	endif
endif
~
|

