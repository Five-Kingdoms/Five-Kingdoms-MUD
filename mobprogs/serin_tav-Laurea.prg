>greet_prog 50~
if rand(25)
	say Hey there, $N!
	say Would you like to try some of my specialities?
	say Or perhaps you'd like a room in my Inn?
	smile
else
	if rand(33)
		smile $N
		say Welcome to The White Lion, $N!
		say No fighting in the bar, okay?
	else
		if rand(50)
			emote welcomes you to her tavern.
			say Care for a room, $n?
		else
			say You must be tired, dear adventurer!
			say What would you like to drink?
			say Or perhaps you need some rest?
			say Rooms here are only 10,000 gold per night.
			smile $n
		endif
	endif
endif
~
>rand_prog 10~
if rand(25)
	emote polishes $l bottles of drinks.
	emote hefts an iron club she keeps hidden behind the bar.
else
	if rand(33)
		emote looks around to make sure her customers are served.
	else
		if rand(50)
			emote takes out a towel and starts wiping the tables.
		else
			emote stands cheerfully waiting to serve her customers.
		endif
	endif
endif
~
>fight_prog 100~
peace
restore room
say No fighting in my bar! Out with you!
yell Guards, there is a fight at the White Lion!
teleport $n 20735
~
|
