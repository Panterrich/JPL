# JoJo Programming Language

It is a Turing complete programming language. 

### In this language, it is implemented:

* Variables (global or locale)
* Functions (recursion is supported)
* Input and output of variable values
* Nested conditional statements but without else (it is proposed to write then a conditional operator but with the negation of the condition)
* Loops of the while type
* The differentiation function
* Standard Boolean operations
* Standard mathematical functions (the full list can be found in libr/key_words_rm.h)


### The language also supports the following optimizations:

* Convolution of constants
* Removing neutral elements
* Computation of the derivative at the translation stage

### The cherry on the cake - support for two languages (select the appropriate mode in makefile): hiragana and romaji (Latin). The resulting trees (the transition stage) are fully compatible, so a program written in one language can be easily translated into another language in the following way:

Example, we want to translate a program written in hiragana into romaji. To do this, compile the program front-end in ***hg*** mode. Then we compile the resulting file with the reverse front-end in rm mode. The resulting file is the program already rewritten in Romanji.

There is also a feature that allows you to understand how our program was translated (the new debag mode), or you can also just get a funny video. Each keyword has its own identifier (the name of the video), which is picked up at the end of the list, and then this list connects all the views in the desired order. 

Examples: 


[![Watch the video](images/mq3.jpg)](https://www.youtube.com/watch?v=pDLMoiX-FzM)


There is a special middle-end mode in which each expression will be differentiated. It's so much fun to integrate with your hands. 

[![Watch the video](images/mq2.jpg)](https://www.youtube.com/watch?v=3fvgipe_swY)


## Example of a program in romaji

This is a factorial calculation program. The syntax is ***С-like***, so to understand the program, it is enough to open the file ["libr/key_words_rm.h"](libr/key_words_rm.h). 

```
Kono, Giorno Giovanna, niwa yume ga aru ()
Za Warudo! Toki wo Tomare!
    Dio Awaken 0 Yare Yare Daze
    Kono Dio Yare Yare Daze
    Mousukoshite ora ora desuka (Dio URYAAAAA 1)
    Za Warudo! Toki wo Tomare!
        Arrivederci -1 Yare Yare Daze
        0 Kira Kuin baitsa dasuto Yare Yare Daze
    ROAD ROLLER DA!

    Diavolo Awaken Fuctorial(Dio) Yare Yare Daze

    Arrivederci Diavolo Yare Yare Daze

ROAD ROLLER DA!

Fuctorial(Jotaro)
Za Warudo! Toki wo Tomare!
    Mousukoshite ora ora desuka (Jotaro UOSHAAAA 1)
    Za Warudo! Toki wo Tomare!
        1 Kira Kuin baitsa dasuto Yare Yare Daze
    ROAD ROLLER DA!
    
    Jotaro MUDAMUDAMUDA Fuctorial(Jotaro ARIARIARIARI 1) Kira Kuin baitsa dasuto Yare Yare Daze

ROAD ROLLER DA!
```

And this is an example of the same program, but already written in hiragana (automatically obtained from a program written in romaji). There is also a file with key words for it - ["libr/key_words_hg.h"](libr/key_words_hg.h)

```
この, Giorno Giovanno, にわ ゆめ が ある ()
Za Warudo! Toki wo Tomare!
	Dio アウェイクン 0 やれやれだぜ
	この Dio やれやれだぜ

	もしかしてオラオラですかあ (Dio URYAAAAA 1)
	Za Warudo! Toki wo Tomare!
		アリーヴェデルチ -1 やれやれだぜ
		0 キラー・クイーン バイツァ・ダスト やれやれだぜ
	ROAD ROLLER DA!

	Diavolo アウェイクン Fuctorial(Dio) やれやれだぜ
	アリーヴェデルチ Diavolo やれやれだぜ
ROAD ROLLER DA!


Fuctorial (Jotaro)
Za Warudo! Toki wo Tomare!

	もしかしてオラオラですかあ (Jotaro UOSHAAAA 1)
	1 キラー・クイーン バイツァ・ダスト やれやれだぜ

	Jotaro MUDAMUDAMUDA Fuctorial(Jotaro ARIARIARIARI 1) キラー・クイーン バイツァ・ダスト やれやれだぜ
ROAD ROLLER DA!
```

The rest of the examples can be found in the folder ["files/"](files/).





