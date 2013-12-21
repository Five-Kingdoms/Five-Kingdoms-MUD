>greet_prog 50~
if isnghost ($i)
if inroom ($i) != 2216
	say You do not belong here!
	murder $n
endif
endif
~
>speech_prog p Die Fool!~
if ispc ($r)
if isnghost ($i)
	murder $r
endif
endif
~
|

