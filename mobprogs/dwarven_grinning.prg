>time_prog 75~
if istime (6) == 18  
        wake
else
        if inroom ($i) == 6523
                sleep
        endif
endif
~
>rand_prog 3~
if rand (50)
	grin
else
	grin $r
endif
~
|

