.globl MailboxWrite
MailboxWrite: 
	tst r0,#0b1111
	movne pc,lr
	cmp r1,#15
	movhi pc,lr

# Comparación de parámetros, asegura una resolución razonable y que los últimos 4 bits del mensaje sean 0 para asegurar el channel 1.

	channel .req r1
	value .req r2
	mov value,r0
	push {lr}
	ldr r0,=0x2000B880
	mailbox .req r0

# Nos aseguramos de no sobreescrivir los registros de link o valor.
		
	wait1$:
		status .req r3
		ldr status,[mailbox,#0x18]

		tst status,#0x80000000
		.unreq status
		bne wait1$

# Nos aseguramos de que el estado está a 0 el bit más significativo (el buzon puede recivir mensajes)

	add value,channel
	.unreq channel

# Combina channel y value
	
	str value,[mailbox,#0x20]
	.unreq value
	.unreq mailbox
	pop {pc}

# Guarda el mensaje en el campo Write del buzón (@mailbox + 20)

.globl MailboxRead
MailboxRead: 
	cmp r0,#15
	movhi pc,lr

	channel .req r1
	mov channel,r0
	push {lr}
	ldr r0,=0x2000B880
	mailbox .req r0
	
	rightmail$:
		wait2$:
			status .req r2
			ldr status,[mailbox,#0x18]
			
			tst status,#0x40000000
			.unreq status
			bne wait2$

# Comprueba que el buzón no está vacio (hay algo para leer), bit 30 de status a 0.
		
		mail .req r2
		ldr mail,[mailbox,#0]

# Lee el mensaje (MAIL)

		inchan .req r3
		and inchan,mail,#0b1111
		teq inchan,channel
		.unreq inchan
		bne rightmail$
	.unreq mailbox
	.unreq channel

# Comprueba que el canal es el mismo al cual enviamos datos

	and r0,mail,#0xfffffff0
	.unreq mail
	pop {pc}

# Mueve el bit 28 del mensaje (MAIL) a r0, si hay un 0 bien, si hay un 1 no le ha gustado el descritpor de buffer

.globl InitialiseFrameBuffer
InitialiseFrameBuffer:
	width .req r0
	height .req r1
	bitDepth .req r2
	cmp width,#4096
	cmpls height,#4096
	cmpls bitDepth,#32
	result .req r0
	movhi result,#0
	movhi pc,lr

# Comprobación de parámetros

	push {r4,lr}	
	fbInfoAddr .req r4		
	ldr fbInfoAddr,=FrameBufferInfo
	str width,[r4,#0]
	str height,[r4,#4]
	str width,[r4,#8]
	str height,[r4,#12]
	str bitDepth,[r4,#20]
	.unreq width
	.unreq height
	.unreq bitDepth

# Prepara el descriptor de Buffer 

	mov r0,fbInfoAddr
	add r0,#0x40000000
	mov r1,#1
	bl MailboxWrite

# manda el mensaje.
	
	mov r0,#1
	bl MailboxRead

# Recive la respuesta (almanecana en r0)
		
	teq result,#0
	movne result,#0
	popne {r4,pc}

# Comprueba que es 0.

	mov result,fbInfoAddr
	pop {r4,pc}
	.unreq result
	.unreq fbInfoAddr


