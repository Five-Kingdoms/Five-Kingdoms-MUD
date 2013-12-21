>fight_prog 19~
if rand(10)
	say Feel my wrath, foolish MORTAL!
	cas 'prismatic spray'
else
if rand(30)
	echo Vlad the Impaler's fingertips frost over in ice.
	cas 'icicle'
else
if rand(50)
	pmote sends a cry for aid and summons a dark servant!
	load mob 3111
	force underling fol vlad
	force underling say Your bidding my Master!
	group underling
else
	echo Vlad the Impaler's eyes blaze red.
	cas 'flame arrow'
endif
endif
endif
~

>rand_prog 3~ 
mpasound You shiver as the smell of freshly drawn blood fills the air.
if rand(15)
yell AH! More fools to their death! I could always use MORE BLOOD!!
mpasound Wild screams of anguish suddenly break the silence.  
endif
~
|
