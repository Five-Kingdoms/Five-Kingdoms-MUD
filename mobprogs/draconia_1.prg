>death_prog 100~
if quest draconia ($n) == 0
	echo You have defeated me, but can you defeat the Great Black Dragon?
	setquest $n draconia 1
endif
~
|
