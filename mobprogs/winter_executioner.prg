>time_prog 100~
if istime (8) = 6 
	goto 10694
	sleep
else
	if istime (23) = 23 
		goto 10694	
		sleep
	else
		if isasleep ($i)
			wake
			goto 10634
			echo The air feels a lot colder as the stench of death arrives.
		endif
	endif
endif
~
>death_prog 100~
at 78 load mob 10601
~
|
