>give_prog 100~
if ispc($n)
	if number ($o) == 10827
		unlock east
		open east
		if mobobj (10828)
		say What am I to do with this???!!!
		say I have already a piece.
		drop plaque
		endif
	endif
	if number ($o) == 10828
		unlock east
		open east
		if mobobj (10827)
		say What am I to do with this extra piece???!!!
		drop blood
		endif
	endif
endif
~
>death_prog 100~
mpjunk all
~
|
