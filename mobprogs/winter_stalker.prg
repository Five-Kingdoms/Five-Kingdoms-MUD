>all_greet_prog 100~
if isnghost ($n)
if ispc($n)
	murder $n
	murder $n
endif
endif
~

>entry_prog 100~
if isnghost ($n)
if ispc($n)
	
	trip $n
	murder $n
endif
endif
~

>rand_prog 3~
if rand(50)
	mpasound A raunchy smell reaches your nostrils.
else
	mpasound A gurgling sound is heard as a dagger is plunged into
someones back!
endif
~

>fight_prog 20~
pmote chuckles to himself as $n finds a dagger into his stomach!
circle
~
|
