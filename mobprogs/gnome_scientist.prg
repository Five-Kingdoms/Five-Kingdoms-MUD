>rand_prog 3~
if rand(50)
	ponder
else
	pmote scratches his head thoughtfully.
endif
~
>give_prog 75~
if rand (25)
	mpjunk $o
else
	if objtype($o) == 10
		identify $o
		give $o $n
	endif
endif
~
|

