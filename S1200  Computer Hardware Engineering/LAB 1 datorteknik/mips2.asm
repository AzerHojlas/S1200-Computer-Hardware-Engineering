  # hexmain.asm
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

	.text
main:
	li	$a0,22		# change this to test different values. INput is a decimal number. Output is lsb hexa number 

	jal	hexasc		# call hexasc
	nop			# delay slot filler (just in case)	

	move	$a0,$v0		# copy return value to argument register

	li	$v0,11		# syscall with v0 = 11 will print out
	syscall			# one byte from a0 to the Run I/O window 
	
stop:	j	stop		# stop after one run
	nop			# delay slot filler (just in case)

  # You can write your own code for hexasc here
  #
hexasc:
	andi	$t0,$a0,0xf	# Only 4 lsb matter, because only numbers 0-15 are converted
	
	slti	$t1,$t0,10	# If t0 is less than 10, then t1 is 1. If t0 is equal or bigger than 10, then 0.
	beq	$t1,$zero,else	# If the number is instead a letter, then jump to letter converter
	nop
	
	addi	$v0,$t0,48	# 48 is added to get the ASCII character
	jr	$ra		# return to main
	
else:	
	addi	$v0,$t0,55	# If number is 10 or above, then we add 55 to get to the ASCII letters
	jr	$ra		# return to main
	nop