rsum:
    main:
        andq    %rsi, %rsi
        jle .end
        pushq   %rbx
        mrmovq  (%rdi), %rbx
        irmovq  $1, %r8
        irmovq  $8, %r9
        subq    %r8, %rsi
        addq    %r9, %rdi
        call    rsum
        addq    %rbx, %rax
        popq    %rbx
    end:
        irmovq $0, %rax
        ret