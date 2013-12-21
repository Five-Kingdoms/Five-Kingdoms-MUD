>speech_prog p A Hero~
if ispc($n)
	if charobj(10841)
	say You have answered the riddle correctly.  Seek you the Ankh?
	load obj 10842
	give book $n
	endif
endif
~
|
