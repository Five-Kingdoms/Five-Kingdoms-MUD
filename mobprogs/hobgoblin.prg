>time_prog 75~
if istime (6) = 18  
        wake
else
        if inroom ($i) == 1583
                sleep
	else
		if inroom ($i) == 1584
			sleep
		endif
        endif
endif
~
>rand_prog 3~
if rand (50)
	cackle
else
	grin $r
endif
~
>greet_prog 5~
if rand (50)
	say Time to have fun.
else
	say I'm not as ugly as you, $N.
endif
~
|

