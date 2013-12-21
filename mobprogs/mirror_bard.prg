>rand_prog 3~
mpasound You hear some singing nearby.
if rand (50)
	pmote starts to sing...
else
	if mobobj(5310)
		pmote strums on a golden harp.
	else
		if mobobj(5309)
			pmote plays a mandolin.
		endif
	endif
endif
~
|

