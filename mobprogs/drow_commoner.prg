>greet_prog 5~
if israce($n) == 3
	bow $n
else
	pmote looks at $N suspiciously.
endif
~
>rand_prog 3~
if rand(50)
	pmote marches back and forth.
else
	if mobobj (5100)
		pmote holds a commoner's longsword in a ready position.
	endif
endif
~
|

