>speech_prog p In the darkest shadows, I await.~
if ispc($n)
        break
else
        say Welcome.
	load obj 8398
	give key $n
	say The Keep is yours, $n.
endif
~

>bloody_prog 100~
say You are too bloody to enter our guild, $N!
say Take refuge elsewhere until a later time.
pmote escorts $n out of the guild.
teleport $n 8390
~
|

