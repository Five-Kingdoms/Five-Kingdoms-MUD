>give_prog 100~
if ispc($n)
	if mobobj(10346)
		say Where did you find this?!!! This was my father's!
say Can you lead me to him?  I have been looking for him for some time.
		fol $n
		mpjunk ring
	endif
endif
~
>rand_prog 3~
if rand(50)
	pmote wails in despair.
else
	sigh
endif
~
|
