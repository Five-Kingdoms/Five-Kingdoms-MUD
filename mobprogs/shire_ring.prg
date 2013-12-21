>greet_prog 10~
if mobobj(1105)
	pmote stares lovingly at his ring.
	say My pretty, prety ring.
else
	say Where's my ring!
	say I need my ring!
endif
~
>rand_prog 5~
if rand(75)
	wear ring
else
	remove ring
endif
~
|

