>rand_prog 5~
if israce ($r) == 4
	tell self shit
else
	if level ($r) >= 5
		if isnghost ($r)
			yell Protect the children!
			murder $r
		endif
	endif
endif
~
|

