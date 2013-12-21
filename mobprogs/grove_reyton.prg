>rand_prog 10~
if rand(5)
        pmote pores over some large maps.
else
        pmote looks about the room briefly.
endif 
~
>greet_prog 100~
if isgood($n)
	pmote looks up from his desk, and nods at you.
else
	if iscabal($n) == 10
		if isnghost ($n)
			yell You are not allowed in here, $n!
			pmote unsheathes his sword and attacks!
			murder $n
		endif
	else
	if iscabal($n) == 12
		if isnghost ($n)
			yell You are not allowed in here, $n!
			pmote unsheathes his sword and attacks!
			murder $n
		endif
	else
	if isevil($n)
		if isnghost ($n)
			yell Evil is not welcome at this camp, $n!
			pmote unsheathes his sword, and attacks!
			murder $n
		endif
	endif
endif
endif
endif
~
>speech_prog p We are taking heavy losses in Val Miran, General.~
emote sighs regretfully.
say I realize this, Tineal. Mobilize the infantry, and increase the guard on the roads. 
emote looks over his maps, and runs his hands through his hair.
~

>speech_prog p Our forces hold Val Miran, General Reyton.~
nod tineal
say Very good. We have reinforcements coming from the West. They should be here soon.
emote looks over a map marked in red ink.
~

>speech_prog p Morale is good, General. Our scouts hold the Eastern Road, as well.~
nod tineal
say Excellent. Keep the guards posted in Val Miran. We need to keep with
that objective.
emote sips from a steel canteen.
~

>speech_prog p The Templars await further orders, General Reyton.~
nod tinel
say Double the guard on the Eastern Road. We NEED to find their cabals, and rout them there.
salute tineal
~

>fight_prog 50~
if rand(33)
        say You will not escape this tent alive, $n!
else  
        if rand(10)
                say Guards, off with $n's head!
        else
                if rand(20)
                        say Guards, kill $n!
                        force tineal rescue general
                else
                        say Guards, kill $n!
                        force tineal rescue general
                endif
        endif
endif 
~
|



