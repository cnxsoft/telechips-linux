
    AREA SAT_Table, DATA, READONLY

sat_table	DCB   0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
			DCB   0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
			DCB   0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15
			DCB   16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	26,	27,	28,	29,	30,	31
			DCB   31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31
			DCB   31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31,	31

   CODE32
   AREA |IMG_SHARP|, CODE, READONLY
   		
   EXPORT ImageSharp_asm

#ImageSharp_asm(unsigned short *pSrc, unsigned int Size)
# R0 : pSrc
# R1 : IMAGE_XSIZE*IMAGE_YSIZE


ImageSharp_asm
   		STMFD    		r13!,{r2-r12,r14}			#push r2-r12,r14
   		SUB				r2,  r1, #8					#r2  = counter = size -8
		LDR				r12, =sat_table + 32		#r12 = sat_table  +32			

loop_sharp1

		#**************************  1th pixel *******************************
		#r1_high = R3|G3|B3  r1_low = R2|G2|B2
		#r3,r4,r5 = pixel,  r6,r7,r8 = pixel+1,   r9,r10,r11 = pixel+2

		#caculate B pixel
		ADD				r5, r5, r11					#r5 = B0 + B2
		RSB				r5, r5, r8,LSL #2			#r5 = B0_n = B1<<2 - B0 - B2
		LDRB			r5,[r12,r5,ASR #1]			#r5 = sat[B0_n >>1]
		LDRH			r1,[r0, #4]					#r1 = R|G|B
		
		#caculate G pixel
		ADD				r4,r4, r10					#r4 = G0 + G2
		RSB				r4,r4, r7,LSL #2			#r4 = G0_n = G1<<2 - G0 - G2
		LDRB			r4,[r12,r4,ASR #7]			#r4 = sat[G0_n >>7]
		
		#caculate R pixel
		ADD				r3,r3, r9					#r3 = R0 + R2
		RSB				r3,r3, r6,LSL #2			#r3 = R0_n = R1<<2 - R0 - R2
		LDRB			r3,[r12,r3,ASR #12]			#r3 = sat[R0_n >>12]
		
		#r14_low = R0_n|G0_n|B0_n
		ORR				r14,r5, r4,LSL #6
		AND				r4, r1, #0x7e0				#r4 = r1 & 0x 7e0, G3<<5
		AND				r5, r1, #0x1f				#r5 = r1 & 0x  1f, B3
		ORR				r14,r14,r3,LSL #11			
		AND				r3, r1, #0xf800				#r3 = r1 & 0xf800, R3<<11
		STRH			r14,[r0],#2					#store r14

		#************************** 2th pixel ******************************
		#r6,r7,r8 = pixel,  r9,r10,r11 = pixel+1,   r3,r4,r5 = pixel+2
		
		#caculate B pixel
		ADD				r8, r8,r5					#r8 = B0 + B2
		RSB				r8, r8,r11,LSL #2			#r8 = B0_n = B1<<2 - B0 - B2
		LDRB			r8,[r12,r8,ASR #1]			#r8 = sat[B0_n]
		LDRH			r1,[r0, #4]					#r1 = R|G|B
		
		#caculate G pixel
		ADD				r7,r7, r4					#r7 = G0 + G2
		RSB				r7,r7, r10,LSL #2			#r7 = G0_n = G1<<2 - G0 - G2
		LDRB			r7,[r12,r7, ASR #7]			#r7 = sat[G0_n]
		
		#caculate R pixel
		ADD				r6,r6, r3					#r6 = R0 + R2
		RSB				r6,r6, r9,LSL #2			#r6 = R0_n = R1<<2 - R0 - R2
		LDRB			r6,[r12,r6,ASR #12]			#r6 = sat[R0_n]	
			
		ORR				r14,r8, r7,LSL #6
		ORR				r14,r14,r6,LSL #11			
		STRH			r14,[r0],#2					#store r14
		
		#prepare data for next loop
		MOV				r6,r3
		MOV				r7,r4
		MOV				r8,r5
		
		MOV				r3,r9
		MOV				r4,r10
		MOV				r5,r11
					
		AND				r9 , r1, #0xf800	
		AND				r10, r1, #0x7e0	
		AND				r11, r1, #0x1f
		
		SUBS			r2, r2,#1
		BNE				loop_sharp1
		
_end_loop_sharp1
		
		LDMFD    		r13!,{r2-r12,r14}			#pop r2-r12,r14
		
   		#[ THUMB
    	BX      lr                              	# Return to caller
   		#|
   		#MOV      pc,lr                           	# Return to caller
   		#]
   END
