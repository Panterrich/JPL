all: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/Differentiator/Differentiator.cpp 		 		\
	 Front-end/Frontend.cpp Front-end/main.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp Middle-end/Middleend.cpp Middle-end/main.cpp 		\
	 Back-end/Backend.cpp Back-end/main.cpp                                                                                                     \
	 CPU/CPU/CPU.cpp CPU/CPU/CPU_main.cpp CPU/libr/Stack.cpp CPU/libr/Guard.cpp CPU/Assembler/Assembler.cpp CPU/Assembler/Assembler_main.cpp    \
	 CPU/libr/Str_lib.cpp CPU/Disassembler/Disassembler.cpp CPU/Disassembler/Disassembler_main.cpp
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Front-end/Frontend.cpp Front-end/main.cpp -o fe-hg -D HG
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Front-end/Frontend.cpp Front-end/main.cpp -o fe-rm
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp -o rfe-hg -D HG
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp -o rfe-rm
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/Differentiator/Differentiator.cpp Middle-end/Middleend.cpp Middle-end/main.cpp -o me
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Back-end/Backend.cpp Back-end/main.cpp -o be
	g++ CPU/CPU/CPU.cpp CPU/CPU/CPU_main.cpp CPU/libr/Stack.cpp CPU/libr/Guard.cpp -o cpu
	g++ CPU/Assembler/Assembler.cpp CPU/Assembler/Assembler_main.cpp CPU/libr/Str_lib.cpp -o asm
	g++ CPU/Disassembler/Disassembler.cpp CPU/Disassembler/Disassembler_main.cpp CPU/libr/Str_lib.cpp -o dis

clean_all: 
	rm -f "images/equation.aux"
	rm -f "images/equation.log"
	rm -f "images/equation.out"
	rm -f "images/equation.pdf"
	rm -f "images/equation.tex"
	rm -f "images/end.log"
	rm -f "images/end.tex.bak"
	rm -f "images/equation.tex.bak"
	rm -f "images/preambule_article.tex.bak"
	rm -f "images/title.tex.bak"
	rm -f "images/title.log"
	rm -f "images/tree_graph.dot"
	rm -f images/base_*
clean:
	rm -f "images/equation.aux"
	rm -f "images/equation.log"
	rm -f "images/equation.out"
	rm -f "images/equation.tex"
	rm -f "images/tree_graph.dot"
	rm -f "images/end.log"
	rm -f "images/end.tex.bak"
	rm -f "images/equation.tex.bak"
	rm -f "images/preambule_article.tex.bak"
	rm -f "images/title.tex.bak"
	rm -f "images/title.log"

lang_all: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/Differentiator/Differentiator.cpp 	  \
	 Front-end/Frontend.cpp Front-end/main.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp Middle-end/Middleend.cpp Middle-end/main.cpp  \
	 Back-end/Backend.cpp Back-end/main.cpp
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Front-end/Frontend.cpp Front-end/main.cpp -o fe-hg -D HG
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Front-end/Frontend.cpp Front-end/main.cpp -o fe-rm
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp -o rfe-hg -D HG
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp -o rfe-rm
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/Differentiator/Differentiator.cpp Middle-end/Middleend.cpp Middle-end/main.cpp -o me
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Back-end/Backend.cpp Back-end/main.cpp -o be


fe-hg: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Front-end/Frontend.cpp Front-end/main.cpp 
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Front-end/Frontend.cpp Front-end/main.cpp -o fe-hg -D HG

fe-rm: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Front-end/Frontend.cpp Front-end/main.cpp
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Front-end/Frontend.cpp Front-end/main.cpp -o fe-rm

rfe-hg: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp -o rfe-hg -D HG

rfe-rm: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp RFront-end/RFrontend.cpp RFront-end/main.cpp -o rfe-rm

me: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/Differentiator/Differentiator.cpp Middle-end/Middleend.cpp Middle-end/main.cpp 
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp libr/Differentiator/Differentiator.cpp Middle-end/Middleend.cpp Middle-end/main.cpp -o me

be: libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Back-end/Backend.cpp Back-end/main.cpp
	g++ libr/Stack/Stack.cpp libr/Stack/Guard.cpp libr/Tree/Tree.cpp libr/Onegin/Str_lib.cpp Back-end/Backend.cpp Back-end/main.cpp -o be

CPUall: CPU/CPU/CPU.cpp CPU/CPU/CPU_main.cpp CPU/libr/Stack.cpp CPU/libr/Guard.cpp CPU/Assembler/Assembler.cpp CPU/Assembler/Assembler_main.cpp \
		CPU/libr/Str_lib.cpp CPU/Disassembler/Disassembler.cpp CPU/Disassembler/Disassembler_main.cpp
	g++ CPU/CPU/CPU.cpp CPU/CPU/CPU_main.cpp CPU/libr/Stack.cpp CPU/libr/Guard.cpp -o cpu
	g++ CPU/Assembler/Assembler.cpp CPU/Assembler/Assembler_main.cpp CPU/libr/Str_lib.cpp -o asm
	g++ CPU/Disassembler/Disassembler.cpp CPU/Disassembler/Disassembler_main.cpp CPU/libr/Str_lib.cpp -o dis

CPU: CPU/CPU/CPU.cpp CPU/CPU/CPU_main.cpp CPU/libr/Stack.cpp CPU/libr/Guard.cpp
	g++ CPU/CPU/CPU.cpp CPU/CPU/CPU_main.cpp CPU/libr/Stack.cpp CPU/libr/Guard.cpp -o cpu

Assembler: CPU/Assembler/Assembler.cpp CPU/Assembler/Assembler_main.cpp CPU/libr/Str_lib.cpp
	g++ CPU/Assembler/Assembler.cpp CPU/Assembler/Assembler_main.cpp CPU/libr/Str_lib.cpp -o asm

Disassembler: CPU/Disassembler/Disassembler.cpp CPU/Disassembler/Disassembler_main.cpp CPU/libr/Str_lib.cpp
	g++ CPU/Disassembler/Disassembler.cpp CPU/Disassembler/Disassembler_main.cpp CPU/libr/Str_lib.cpp -o dis