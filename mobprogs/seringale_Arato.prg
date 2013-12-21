>fight_prog 20~
if rand(25)
	shielddisarm
else
	if rand(33)
		cast 'wrath'
	else
		if rand(50)
			cast 'cure serious'
		else
			cast 'cure critical'
		endif
	endif
endif
~
|
