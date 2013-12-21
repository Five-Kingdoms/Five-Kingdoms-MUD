>speech_prog p How dare you intrude upon me?~
if isnghost ($r)
if ispc ($r)
	murder $r
endif
endif
~
>rand_prog 10~
if isnghost ($r)
if ispc ($r)
	yell Protect the King!
	murder $r
endif
endif
~
|

