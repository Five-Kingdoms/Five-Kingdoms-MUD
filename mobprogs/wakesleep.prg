>time_prog 100~
if isindoor ($i)
	if rand (10)
		if istime (20) = 4
			sleep
		endif
	endif
endif
if rand (90)
	if istime (20) = 4
		tell self shit
	else
		wake
	endif
endif
~
|

