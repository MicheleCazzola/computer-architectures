.data

v1:		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		
v2:		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0

v3:		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		
v4:		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		.double	1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0
		
v5:		.space 512
v6:		.space 512
v7:		.space 512	

main:
        daddui  $sp,$sp,-48
        sd      $fp,40($sp)
        move    $fp,$sp
        lui     $4,%hi(%neg(%gp_rel(main)))
        daddu   $4,$4,$25
        daddui  $4,$4,%lo(%neg(%gp_rel(main)))
        li      $2,1                        # 0x1
        sw      $2,0($fp)
        sd      $0,8($fp)
        sw      $0,24($fp)
        b       .L2
        nop

.L5:
        lw      $2,24($fp)
        andi    $2,$2,0x1
        bne     $2,$0,.L3
        nop

        ld      $3,%got_disp(v1)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        ldc1    $f1,0($2)
        lw      $3,0($fp)
        lw      $2,24($fp)
        sll     $2,$3,$2
        dmtc1   $2,$f0
        nop
        cvt.d.w $f0,$f0
        mul.d   $f0,$f1,$f0
        sdc1    $f0,16($fp)
        ldc1    $f0,16($fp)
        trunc.w.d $f0,$f0
        mfc1    $2,$f0
        nop
        sw      $2,0($fp)
        b       .L4
        nop

.L3:
        ld      $3,%got_disp(v1)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        ldc1    $f1,0($2)
        lw      $2,0($fp)
        mtc1    $2,$f0
        nop
        cvt.d.w $f2,$f0
        lw      $2,24($fp)
        mtc1    $2,$f0
        nop
        cvt.d.w $f0,$f0
        mul.d   $f0,$f2,$f0
        div.d   $f0,$f1,$f0
        sdc1    $f0,16($fp)
        ld      $3,%got_disp(v4)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        ldc1    $f0,0($2)
        trunc.w.d $f0,$f0
        mfc1    $2,$f0
        nop
        move    $3,$2
        lw      $2,24($fp)
        sra     $2,$3,$2
        dmtc1   $2,$f0
        nop
        cvt.d.w $f0,$f0
        sdc1    $f0,8($fp)
.L4:
        ld      $3,%got_disp(v2)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        ldc1    $f1,0($2)
        ldc1    $f0,16($fp)
        mul.d   $f1,$f1,$f0
        ld      $3,%got_disp(v3)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        ldc1    $f0,0($2)
        add.d   $f1,$f1,$f0
        ld      $3,%got_disp(v4)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        ldc1    $f0,0($2)
        add.d   $f0,$f1,$f0
        ld      $3,%got_disp(v5)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        sdc1    $f0,0($2)
        ld      $3,%got_disp(v5)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        ldc1    $f1,0($2)
        ld      $3,%got_disp(v1)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        ldc1    $f2,0($2)
        ldc1    $f0,8($fp)
        add.d   $f0,$f2,$f0
        div.d   $f0,$f1,$f0
        ld      $3,%got_disp(v6)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        sdc1    $f0,0($2)
        ld      $3,%got_disp(v6)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        ldc1    $f1,0($2)
        ld      $3,%got_disp(v2)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        ldc1    $f2,0($2)
        ld      $3,%got_disp(v3)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        ldc1    $f0,0($2)
        add.d   $f0,$f2,$f0
        mul.d   $f0,$f1,$f0
        ld      $3,%got_disp(v7)($4)
        lw      $2,24($fp)
        dsll    $2,$2,3
        daddu   $2,$3,$2
        sdc1    $f0,0($2)
        lw      $2,24($fp)
        addiu   $2,$2,1
        sw      $2,24($fp)
.L2:
        lw      $2,24($fp)
        slt     $2,$2,64
        bne     $2,$0,.L5
        nop

        move    $2,$0
        move    $sp,$fp
        ld      $fp,40($sp)
        daddui  $sp,$sp,48
        jr      $31
        nop