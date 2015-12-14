# La familia de procesadores ARM no tiene una instrucción
# de división nativa por lo que delega su implementación al 
# sistema operativo.
# Por este motivo el compilador al detectar un operador de
# módulo (%) o de cociente (/) del tipo (integer)(%)(integer)
# o (integer)(/)(integer) crea una referencia (símbolo)
# y espera que le sea facilitada la rutina pertinente en el linkado.


# 	división entera:  	
#	N = D × Q + R where 0 ≤ |R| < |D| 


# símbolos divisiones y módulos:

#	`__aeabi_idivmod'
#	`__aeabi_uidivmod'
#	`__aeabi_idiv'
#	`__aeabi_uidiv'

#.globl __aeabi_idivmod
.globl __aeabi_uidivmod
#.globl __aeabi_idiv
.globl __aeabi_uidiv

#__aeabi_idivmod:
#__aeabi_idiv:

__aeabi_uidiv:

   /* r0 is N and Ni */
    /* r1 is D */
    /* r2 is Q */
    /* r3 is Di */

    Ni .req r0
    D  .req r1
    Q  .req r2
    Di .req r3
 
    mov Di, D                   /* r3 ← r1 */
    cmp Di, Ni, LSR #1           /* update cpsr with r3 - r0/2 */
    .Lloop2A$:
      movls Di, Di, LSL #1       /* if r3 <= 2*r0 (C=0 or Z=1) then r3 ← r3*2 */
      cmp Di, Ni, LSR #1         /* update cpsr with r3 - (r0/2) */
      bls .Lloop2A$                /* branch to .Lloop2 if r3 <= 2*r0 (C=0 or Z=1) */
 
    mov Q, #0                   /* r2 ← 0 */
 
    .Lloop2B$:
      cmp Ni, Di                 /* update cpsr with r0 - r3 */
      subhs Ni, Ni, Di           /* if r0 >= r3 (C=1) then r0 ← r0 - r3 */
      adc Q, Q, Q             /* r2 ← r2 + r2 + C.
                                    Note that if r0 >= r3 then C=1, C=0 otherwise */
 
      mov Di, Di, LSR #1         /* r3 ← r3/2 */
      cmp Di, D                 	/* update cpsr with r3 - r1 */
      bhs .Lloop2B$              	/* if r3 >= r1 branch to .Lloop3 */

    .unreq Ni
    .unreq D
    .unreq Q
    .unreq Di

    mov r1,r0
    mov r0,r2

    bx lr

__aeabi_uidivmod:

    /* r0 is N and Ni */
    /* r1 is D */
    /* r2 is Q */
    /* r3 is Di */

    Ni .req r0
    D  .req r1
    Q  .req r2
    Di .req r3
 
    mov Di, D                   /* r3 ← r1 */
    cmp Di, Ni, LSR #1           /* update cpsr with r3 - r0/2 */
    .Lloop2A$:
      movls Di, Di, LSL #1       /* if r3 <= 2*r0 (C=0 or Z=1) then r3 ← r3*2 */
      cmp Di, Ni, LSR #1         /* update cpsr with r3 - (r0/2) */
      bls .Lloop2A$                /* branch to .Lloop2 if r3 <= 2*r0 (C=0 or Z=1) */
 
    mov Q, #0                   /* r2 ← 0 */
 
    .Lloop2B$:
      cmp Ni, Di                 /* update cpsr with r0 - r3 */
      subhs Ni, Ni, Di           /* if r0 >= r3 (C=1) then r0 ← r0 - r3 */
      adc Q, Q, Q             /* r2 ← r2 + r2 + C.
                                    Note that if r0 >= r3 then C=1, C=0 otherwise */
 
      mov Di, Di, LSR #1         /* r3 ← r3/2 */
      cmp Di, D                 	/* update cpsr with r3 - r1 */
      bhs .Lloop2B$              	/* if r3 >= r1 branch to .Lloop3 */

    .unreq Ni
    .unreq D
    .unreq Q
    .unreq Di

    mov r1,r0
    mov r0,r2

    bx lr

