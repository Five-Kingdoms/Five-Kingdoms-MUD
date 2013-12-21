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
if charobj(10842)
force doodoo south
force doodoo drop ankh
get heirloom
mpjunk all
echo The gonturan heirloom rises in the air and attaches itself to the
echo far end of the chest!
echo A strange ankh is found within and drops out.
trans doodoo 10885
endif
at 78 load mob 10800
~
|
