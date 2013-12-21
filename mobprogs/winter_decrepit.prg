>all_greet_prog 100~
if ispc($n)
' What are you doing here?  Chaos and Order are abound!
endif
if isnpc($n)
	if number($n) == 10337
		say Thank you for keeping my daughter safe!
		load obj 10321
		drop cloak
		purge girl
	endif
endif
~
>rand_prog 5~
if rand(50)
mutter $r
else
run
endif
~

|
