>greet_prog 60~
if rand(50)
	say Welcome, $N.  Do you wish my help in training you?
else
	say I offer my services to you in training you.
endif
~
>rand_prog 15~
if rand(33)
	say Look at the sign to see more information on what I offer.
else
	if rand(50)
		say I am willing to convert your practice sessions into training sessions.
	else
		say I can convert ten practice sessions into one training session.
	endif
endif
~
|
