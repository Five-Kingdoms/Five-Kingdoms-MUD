>rand_prog 3~
if rand(10)
	pmote spins a web.
else
	pmote stares at $R with all eight of $l eyes.
endif
~
>greet_prog 50~
if isfollow($i)
	tell self shit
else
	if charwear(6310)
		setquest $n arachnos 1
		follow $n
	endif
endif
~
|

