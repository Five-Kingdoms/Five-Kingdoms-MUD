>speech_prog p passage~
emote looks you over with his clouded gaze.
    if isclass($n) == 8
      say Arr, I once had a monk friend.. For you the trip to the Atoll will be but 5000 gold.
    else
      say The trip to the Atoll will cost you 50000 gold, drylander.
    endif
~
>bribe_prog 50000~
  emote looks you over with his clouded gaze.
  if charobj(20798)
    'heh, good thing you have that canoe there, else you might find yourself marooned.
    pmote gets the boat ready and in seconds $n is off.
    qtrans $n 25149
    at 25149 say This is as far as the toll takes me. Enjoy the Atoll.
    mpjunk gold
  else
    'A drylander without a canoe. Figures!
    spit
    pmote gets the boat ready and in seconds $n is off.
    qtrans $n 25149
    at 25149 say My fare ends here. Enjoy the atoll. Methinks ye'll be here a while!
    mpjunk gold
  endif
~

>bribe_prog 5000~
if isclass($n) == 8
  emote looks you over with his clouded gaze.
  if charobj(20798)
    'heh, good thing you have that little canoe there, else your trip might have been rough.
    pmote gets the boat ready and in seconds $n is off.
    qtrans $n 25149
    at 25149 say Arr, you are on your own her. Enjoy the Atoll, drylander.
    mpjunk gold
  else
    'Arr, hope you are stronger then you look, without a canoe...
    spit
    pmote gets the boat ready and in seconds $n is off.
    qtrans $n 25149
    at 25149 say Arr, you are on your own here. Enjoy the Atoll, drylander!
    mpjunk gold
  endif
else
    say Heh, thanks there mate.
    mpjunk gold
endif
~

>bribe_prog 1~
say Heh, thanks there mate.
    mpjunk gold
~
|
