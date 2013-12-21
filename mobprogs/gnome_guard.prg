>time_prog 75~
if istime (6) = 18
	wake
else
	if inroom ($i) == 1530
		sleep
	endif
endif
~
>greet_prog 5~
if israce($n) == 6
	bow $n
else
	pmote looks at $N suspiciously.
endif
~
>rand_prog 3~
if rand(50)
	pmote marches back and forth.
else
	if mobobj (1504)
		pmote practices with a gnome sword.
	endif
endif
~
|

