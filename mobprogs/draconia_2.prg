>death_prog 100~
if quest draconia ($n) == 1
	echo You have defeated me, but can you defeat the Great Red Dragon?
	setquest $n draconia 2
endif
~
|
