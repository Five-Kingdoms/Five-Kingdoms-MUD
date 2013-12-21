>fight_prog 10~
yell Guards!  Apprehend $N!
~
>rand_prog 3~
if rand (50)
	smile $r
else
	stretch
endif
~
>greet_prog 5~
if inroom ($i) == (9689)
	say Hi, $N!  Welcome to Falen Dara!
endif
~
|

