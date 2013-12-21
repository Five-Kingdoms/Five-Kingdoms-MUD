>rand_prog 2~
yell Help!  Somebody please help me!
mpasound You hear a fight nearby.
~
>greet_prog 5~
if rand (50)
	sob
else
	cry
endif
say Some dark raiders killed my family.
~
>give_prog 100~
if number ($o) == 3534
	say Thanks.  At least I can remember my family with this.
	emote stops crying and brightens up a little.
	setquest $n midennir 1
endif
~
|

