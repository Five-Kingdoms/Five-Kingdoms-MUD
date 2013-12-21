>give_prog 100~
if number ($o) == 6112
	mpjunk $o
	say Yes, now I am king!
	pmote wears $o on $l head.
	say Hey, it broke!
	setquest $n haon 1
endif
~
>greet_prog 1~
vis
~
>rand_prog 3~
if mobobj (6114)
	pmote shows $r an iron ring.
	say Do you like my ring?  I only wear iron.
endif
~
|
