>give_prog 100~
if number ($o) == 5376
	mpjunk scroll stw
	say Ahhh..  This is perfect!  Thank you, $N.
	say Here, take this.  The wizard owes me a favor.
	load obj 5377
	give note $n
	setquest $n mirror3 1
else
	shake
	say Thanks, but that won't help.
endif
~
>rand_prog 5~
if rand (25)
	mutter
else
	say If Only I had an ancient scroll to study.
endif
~
|
