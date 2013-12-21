>greet_prog 50~
if hitprcnt($n) <= 25
	say You look hurt, $N.  Rest here, and I will heal you.
endif
~
>rand_prog 3~
if rand(50)
	pmote contemplates the forces of Evil in Thera.
else
	pmote contemplates the supreme power of chaos.
endif
if rand(10)
	say Would you like to make an offering to Shadowspawn?
endif
~
>give_prog 100~
if quest school ($n) != 0
	tell self shit
else
	if number ($o) == 10
		setquest $n school 1
		say Shadowspawn appreciates your offer, $N.
		say Go in Chaos, my child.
		c 'bless' $n
	endif
endif
~
|

