>speech_prog p Get me a scalpel.~
if ispc($n)
	tell self shit
else
	if rand(10)
		pmote hands a scalpel to the dwarven doctor.
	endif
endif
~
>rand_prog 3~
if rand(50)
	pmote scrubs the blood off of the walls.
else
	pmote hurries around assisting the dwarven doctor.
endif
~
|
