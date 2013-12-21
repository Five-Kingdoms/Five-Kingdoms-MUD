>time_prog 100~
if istime (20) = 20
	pmote closes down $l shop.
	up
	sleep
else
	if istime (21) = 4
		say I need to get some sleep.
		sleep
	else
		if isasleep ($i)
			wake
			down
			say Open for business!
		endif
	endif
endif
~
|

