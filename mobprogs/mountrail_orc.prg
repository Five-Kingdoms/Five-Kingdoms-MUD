>rand_prog 3~
if rand(50)
	pmote looks around for some elves to kill.
else
	say The elves will never get Greginsham back.
	cackle
endif
~
>greet_prog 5~
if israce($n) == 2
	if isnghost ($n)
		murder $n
else
	say You're lucky you're not an elf.
	snarl $n
endif
endif
~
|

