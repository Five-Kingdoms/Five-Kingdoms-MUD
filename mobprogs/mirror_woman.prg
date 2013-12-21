>speech_prog p Have you heard the latest news?~
if rand(50)
	shake
	say No, what?
else
	if rand(50)
		nod
		say Isn't it wonderful?
	else
		sigh
		say It's a shame...
	endif
endif
~
>speech_prog p No, what happened?~
if rand(50)
	say The mayor's wife just gave birth to a baby son.
else
	say Thaellor is closing down his shop.
endif
~
>speech_prog p The mayor has a new baby boy.~
if rand(50)
	say That's wonderful!
else
	say Another child?
	shake
endif
~
>speech_prog p Nice weather we're having.~
if rand(50)
	say Yes, it's wonderful.
endif
~
>speech_prog p I hope it rains soon.  The crops need it.~
if rand(50)
	say I don't.  I hate the rain!
endif
~
>speech_prog p It's raining, it's pouring...~
if rand(50)
	say The old man is snoring...
endif
~
>rand_prog 3~
if rand(50)
	say Have you heard the latest gossip?
else
	if isweather (0)
		say It's gorgeous outside!
	else
		if isweather (1)
			say I wish the sky would clear up.
		else
			say Rain, rain, go away...
		endif
	endif
endif
~
>greet_prog 5~
if ispc($n)
	curtsey $n
	say Hi!  How are you doing?
endif
~
|

