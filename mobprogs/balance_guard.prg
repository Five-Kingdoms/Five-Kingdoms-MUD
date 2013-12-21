>greet_prog 100~
if rand (10)
	say Greetings, $N. Welcome to House Aldur.
endif
if rand (33)
	say Welcome to the Balance House of Revelsport, $N.
endif
if charobj (8)
	bow $n
	say Welcome, $N. We Keep the Balance.
	unlock up
	open up
	pmote opens the gates and ushers $n into the mansion.
	qteleport $N 20753
	close up
	lock up
endif
~
>rand_prog 5~
if rand (50)
	pmote walks around the wall, his eyes scanning for trouble.
else
	pmote adjusts his sword.
endif
~
>fight_prog 10~
if rand(25)
	yell Help! The House of Aldur is being attacked by $N!
	say Die, $N!
else
	yell Help! The House of Aldur is being attacked by $N!
	say The Balance must be kept!
endif
~
|


