>greet_prog 100~
if iscabal($n) == 10
	yell To death with the Unbelievers!
	say Your soul will burn under my blade, $n!
	murder $n
	else
		if iscabal($n) == 12
		yell Death to any who aid in Chaos!
		say You will pay for your dark allegiance, $n!
		murder $n
		else
			if iscabal($n) == 1
			bow $n
			say The Templars have secured Revelsport, $n.
			say The city is safe again under our army's protection.
			salute $n
			say Walk in the Light.
			else
				if iscabal($n) == 3
				bow $n
				say We aid you in protection of Revelsport, $n.
				say We cannot have such madness in the City of the Light.
				salute $n
				say Walk in the Light.
			endif
		endif
	endif
endif
~

>rand_prog 15~
if rand(10)
	pmote adjusts his armor, and looks about for heathens to slay.
else
	pmote nods at you, and continues his watch over the city.
endif
~
|
	

