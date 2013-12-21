>rand_prog 5~
if rand(5)
        pmote breathes a heavy sigh.
	say We are taking heavy losses in Val Miran, General.
else
if rand(5)
        salute general
	say Our forces hold Val Miran, General Reyton.
else
if rand(5)
	salute general
	say Morale is good, General. Our scouts hold the Eastern Road, as well.
else
	salute general
	say The Templars await further orders, General Reyton.
endif
endif
endif 
~
>greet_prog 100~
if isgood($n)
	pmote straightens as you enter the room, and nods slightly.
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
>speech_prog p I realize this, Tineal. Mobilize the infantry, and increase the guard on the roads.~
nod general
say As you command, General Reyton. 
bow general
~

>speech_prog p Very good. We have reinforcements coming from the West. They should be here soon.~
nod general
say Excellent, General. We look forward to crushing the factions of Chaos.
salute general
~

>speech_prog p Excellent. Keep the guards posted in Val Miran. We need to keep with that objective.~
nod general
say Aye, General. Guards are holding Val Miran as we speak.
bow general
~

>speech_prog p Double the guard on the Eastern Road. We NEED to find their cabals, and rout them there.~
nod general
say Our scouts are seeing to it, General Reyton. We should have answers soon.
bow general
~

>fight_prog 50~
if rand(33)
        say You will not escape this tent alive, $n!
else  
        if rand(10)
                say Protect General Reyton!
        else
                if rand(20)
                        say Guards, kill $n!
                        rescue general
                else
                        say Guards, kill $n!
                        rescue general
                endif
        endif
endif 
~
|



