>fight_prog 20~
if rand(25)
	kill 1.	
  	bash 1.
else
	if rand(33)
		kill 2.
		bash 2.
	else
		if rand(50)
			kill 3.
			bash 3.
		else
			pmote executes a ground shaking maneuver.
			trip 1.
			trip 2.
			trip 3.
			trip 4.
			trip 5.
		endif
	endif
endif
~

>death_prog 100~
at 78 load mob 10801
~
|
