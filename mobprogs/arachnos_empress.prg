>rand_prog 3~
if rand(50)
	say Obey or die, $R.
else
	say You will be mine, $R.
endif
setquest $r arachnos2 1
c charm $r
~
>greet_prog 5~
if ispc($n)
	say You will obey me, $N.
	setquest $n arachnos2 1
	c charm $n
endif
~
|

