>speech_prog p Have you heard the latest gossip?~
if rand(50)
	shake
	say No, what happened?
else
	if rand(50)
		nod
		say It's all good.
	else
		sigh
		say It's too bad...
	endif
endif
~
>speech_prog p No, what?~
if rand(50)
	say The mayor has a new baby boy.
else
	say Thaellor is going out of business.
endif
~
>speech_prog p The mayor's wife just gave birth to a baby son.~
if rand(50)
	say Wow!  I must congratulate the mayor.
else
	say Man, he's keeping his wife busy...
endif
~
>speech_prog p It's gorgeous outside!~
if rand(50)
	say Yes, we should have a festival.
endif
~
>speech_prog p I wish the sky would clear up.~
if rand(50)
	say I don't.  The crops need some rain soon.
endif
~
>speech_prog p Rain, rain, go away...~
if rand(50)
	say Come again another day...
endif
~
>rand_prog 3~
if rand(50)
	say Have you heard the latest news?
else
	if isweather (0)
		say Nice weather we're having.
	else
		if isweather (1)
			say I hope it rains soon.  The crops need it.
		else
			say It's raining, it's pouring...
		endif
	endif
endif
~
>greet_prog 5~
if ispc($n)
	shake $n
	say Hi!  How are you doing?
endif
~
|

