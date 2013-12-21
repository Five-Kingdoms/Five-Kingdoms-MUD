>give_prog 100~
if objtype ($o) == 19
	mpjunk $o
	say Thank you so much!
	pmote eats $O hungrily.
	pmote wipes his mouth.
	if rand (50)
		say Here's a bit of advice.  Hide your keys in your fireplace so you won't lose them.
	else
		say I'll tell you a secret.  Shargugh told me that he thinks he's the king of brownies.
	endif
endif
~
>greet_prog 5~
say I'm starving!  I don't suppose you have any extra food, do you?
~
>rand_prog 3~
if rand (50)
	pmote wipes the sweat from his forehead
else
	echo John the Lumberjack's stomache growls loudly.
endif
~
|
