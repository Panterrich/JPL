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


[![Watch the video](https://i9.ytimg.com/vi/pDLMoiX-FzM/mq2.jpg?sqp=CMDy94YG&rs=AOn4CLDU_M9sUvI9cKRRfxvOQZehstn-nA)](https://www.youtube.com/watch?v=pDLMoiX-FzM)


There is a special middle-end mode in which each expression will be differentiated. It's so much fun to integrate with your hands. 

[![Watch the video](https://i9.ytimg.com/vi/3fvgipe_swY/mq2.jpg?sqp=CMDy94YG&rs=AOn4CLAP1vQDytKc08Yu0JWwe_-aIdc9Yg)](https://www.youtube.com/watch?v=3fvgipe_swY)




