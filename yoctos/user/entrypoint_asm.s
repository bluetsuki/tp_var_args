extern main

global exit

section .entrypoint
    call  main
    jmp   exit

section .text

exit:
    iret
