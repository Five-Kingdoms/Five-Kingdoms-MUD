>give_prog 100~
if objtype ($o) == 1
	say Nooo!!
	vanish
endif
~
>rand_prog 3~
if rand(50)
	pmote bares his fangs at $R.
else
	echo Blood drips from $I's lips.
endif
~
|

