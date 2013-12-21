>fight_prog 30~
if rand(25)
	kill 1.	
  	bodyslam 1.
else
	if rand(33)
		kill 2.
		bodyslam 2.
	else
		if rand(50)
			kill 3.
			bodyslam 3.
		else
			pmote kneels down and pounds the ground with his fist.
			c earthquake
			c earthquake
		endif
	endif
endif
~

>death_prog 100~
at 78 load mob 10800
~
|
