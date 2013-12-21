>rand_prog 5~
mpasound You hear someone crying nearby.
if rand(50)
	sob
else
	cry
endif
~
>greet_prog 25~
say They wouldn't give me one of theirs...
pout
~
>give_prog 100~
if number ($o) == 6601
	setquest $n daycare 1
	smile
	cheer
	say Thanks!
	n
	disappear
else
	say That's not what I want.
	give $o $n
endif
~
|

