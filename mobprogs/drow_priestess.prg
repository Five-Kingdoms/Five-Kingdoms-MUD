>greet_prog 20~
if israce($n) == 3
	smile $n
else
	if isnghost ($n)
		yell Kill $N!
		murder $n
	endif
endif
~
>rand_prog 3~
if rand(50)
	pmote shouts some orders to the servants.
else
	if mobobj (5103)
		pmote snaps a snake headed whip in the air.
	endif
endif
~
|

