>entry_prog 5~
if rand(33)
	say Greetings $N, do you have the courage to explore Haon Dor?
else
	if rand(50)
		say Greetings $N, are you curious about the Emerald Forest?
	else
		say Beware of the foul goblins south of the city!
	endif
endif
~
>rand_prog 5~
if rand(25)
	say Are you brave enough to explore the forest of Haon Dor?
	say If you have the courage to explore Haon Dor, follow me.
else
	if rand(33)
		say Beware of the goblins south of the city.
		say I hear they are a mean bunch!
	else
		if rand(50)
			say Have you heard of the "Emerald Forest"?
			say It is said that magical creatures reside there!
		else
			say I hear a wise wizard lives north of this city.
			say And he is most powerful in brewing potions!
		endif
	endif
endif
~
|
