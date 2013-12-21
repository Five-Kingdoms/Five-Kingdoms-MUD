>greet_prog 33~
if rand(25)
	say Hey there, $N! Care to try some of my drinks?
else
	if rand(33)
		smile $N
		say Welcome to The Drunken Beggar, $N!
	else
		if rand(50)
			emote welcomes you to The Drunken Beggar.
		else
			say You must be tired, dear adventurer!
			say Would you care for something to drink?
		endif
	endif
endif
~
>rand_prog 5~
if rand(25)
	emote polishes $l bottles of drinks.
else
	if rand(33)
		emote looks around the room, making sure all $l patrons are served.
	else
		if rand(50)
			emote takes out a towel and starts wiping the tables.
		else
			emote stands cheerfully behind the bar.
			say It sure is a nice day, isn't it?
		endif
	endif
endif
~
|
