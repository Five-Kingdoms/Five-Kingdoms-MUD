>rand_prog 3~
if rand(50)
	pmote studies the plants and animals around $l.
else
	if sex ($i) == 1
		pmote picks some hollies.
	else
		pmote gathers some ivy.
	endif
endif
~
>give_prog 75~
if rand (25)
	mpjunk $o
else
	if objtype($o) == 26
		identify $o
		give $o $n
	endif
endif
~
|

