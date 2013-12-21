>give_prog 100~
if ispc($n)
	if number ($o) == 10618
		if mobobj (10619)
			goto 10530
			force jademob drop jade
			get jade
			goto 10540
			give jade $n
			mpjunk all
		endif
	endif
	if number ($o) == 10619
		if mobobj (10618)
			goto 10530
			force jademob drop jade
			get jade
			goto 10540
			give jade $n
			mpjunk all
		endif
	endif
endif
~
|
