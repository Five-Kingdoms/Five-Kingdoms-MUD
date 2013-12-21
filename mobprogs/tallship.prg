>fight_prog 20~
if rand(25)
	echo `8The tubes on the ship explode with fire! A flaming ball flies toward you!``
	com 'high explosive'
else
	if rand(33)
		pmote maneuvers into an attack position.
	else
		if rand(50)
			echo `8A woman on the ship calls upon lightning!``
			com 'lightning'
		else
			echo `8In position, the ship fires a volley of cannonballs at you!``
			com 'high explosive'
		endif
	endif
endif
~
|
