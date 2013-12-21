>greet_prog 5~
pmote whispers 'Be Quiet.  You're in a library.'
~
>rand_prog 3~
if rand(50)
	pmote stacks some books on the sleves.
else
	pmote pulls out a book and thumbs through it.
endif
~
>speech_prog p ~
pmote whispers 'No talking in a library.'
pmote drags $N out the door.
teleport $n 9555
~
|

