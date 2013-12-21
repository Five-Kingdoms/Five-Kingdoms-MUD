>greet_prog 100~
if isnpc($n)
  Kill $n
else
if isevil($n)
     Say Go to the Onyx Gate to the west $n.
else
if isgood($n)
       Say Go to the Ivory Arch $n which is to the east.
  else
if neutral($n)
       Say You are welcome to pass through this Gate $n by going up.  Welcome home $n!
endif
endif
endif
endif
~
|
