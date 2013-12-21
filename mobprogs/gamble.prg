>greet_prog 100~
if rand(75)
        Say $n you have lost to the house the house cards.
        mpforce $n 1 gold.
      else
if rand(15)
        Say $n You have tied the house care to play again?
      else
if rand(10)
      Give 1 gold $n   
      Say you have beaten the house, care to play again $n?
endif
endif
endif
endif
~
|
