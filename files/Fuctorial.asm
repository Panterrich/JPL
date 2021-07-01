

push 0
pop rfx
call :main
hlt


main:

push 0
pop [rfx + 0]
in
pop [rfx + 0]

If_begin0:
push [rfx + 0]
push 1
jae :If_end0

push -1
out
push 0
ret

If_end0:



push [rfx + 0]


push 2
push rfx
add
pop rfx

call :Fuctorial

push rfx
push 2
sub
pop rfx


pop [rfx + 1]
push [rfx + 1]
out
ret


Fuctorial:
pop [rfx + 0]


If_begin1:
push [rfx + 0]
push 1
ja :If_end1

push 1
ret

If_end1:

push [rfx + 0]


push [rfx + 0]
push 1
SUB


push 1
push rfx
add
pop rfx

call :Fuctorial

push rfx
push 1
sub
pop rfx


MUL
ret
ret


