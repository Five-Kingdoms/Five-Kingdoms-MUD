>fight_prog 60~
if rand(25)
	commune 'heal'
else
	if rand(33)
		commune 'cure critical'
	else
		if rand(50)
			commune 'divine retribution'
		else
			commune 'dispel magic'
		endif
	endif
endif
~
|
