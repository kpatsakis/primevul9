static int do_jit(struct bpf_prog *bpf_prog, int *addrs, u8 *image,
		  int oldproglen, struct jit_context *ctx)
{
	struct bpf_insn *insn = bpf_prog->insnsi;
	int insn_cnt = bpf_prog->len;
	bool seen_ld_abs = ctx->seen_ld_abs | (oldproglen == 0);
	bool seen_exit = false;
	u8 temp[BPF_MAX_INSN_SIZE + BPF_INSN_SAFETY];
	int i;
	int proglen = 0;
	u8 *prog = temp;
	int stacksize = MAX_BPF_STACK +
		32 /* space for rbx, r13, r14, r15 */ +
		8 /* space for skb_copy_bits() buffer */;

	EMIT1(0x55); /* push rbp */
	EMIT3(0x48, 0x89, 0xE5); /* mov rbp,rsp */

	/* sub rsp, stacksize */
	EMIT3_off32(0x48, 0x81, 0xEC, stacksize);

	/* all classic BPF filters use R6(rbx) save it */

	/* mov qword ptr [rbp-X],rbx */
	EMIT3_off32(0x48, 0x89, 0x9D, -stacksize);

	/* bpf_convert_filter() maps classic BPF register X to R7 and uses R8
	 * as temporary, so all tcpdump filters need to spill/fill R7(r13) and
	 * R8(r14). R9(r15) spill could be made conditional, but there is only
	 * one 'bpf_error' return path out of helper functions inside bpf_jit.S
	 * The overhead of extra spill is negligible for any filter other
	 * than synthetic ones. Therefore not worth adding complexity.
	 */

	/* mov qword ptr [rbp-X],r13 */
	EMIT3_off32(0x4C, 0x89, 0xAD, -stacksize + 8);
	/* mov qword ptr [rbp-X],r14 */
	EMIT3_off32(0x4C, 0x89, 0xB5, -stacksize + 16);
	/* mov qword ptr [rbp-X],r15 */
	EMIT3_off32(0x4C, 0x89, 0xBD, -stacksize + 24);

	/* clear A and X registers */
	EMIT2(0x31, 0xc0); /* xor eax, eax */
	EMIT3(0x4D, 0x31, 0xED); /* xor r13, r13 */

	if (seen_ld_abs) {
		/* r9d : skb->len - skb->data_len (headlen)
		 * r10 : skb->data
		 */
		if (is_imm8(offsetof(struct sk_buff, len)))
			/* mov %r9d, off8(%rdi) */
			EMIT4(0x44, 0x8b, 0x4f,
			      offsetof(struct sk_buff, len));
		else
			/* mov %r9d, off32(%rdi) */
			EMIT3_off32(0x44, 0x8b, 0x8f,
				    offsetof(struct sk_buff, len));

		if (is_imm8(offsetof(struct sk_buff, data_len)))
			/* sub %r9d, off8(%rdi) */
			EMIT4(0x44, 0x2b, 0x4f,
			      offsetof(struct sk_buff, data_len));
		else
			EMIT3_off32(0x44, 0x2b, 0x8f,
				    offsetof(struct sk_buff, data_len));

		if (is_imm8(offsetof(struct sk_buff, data)))
			/* mov %r10, off8(%rdi) */
			EMIT4(0x4c, 0x8b, 0x57,
			      offsetof(struct sk_buff, data));
		else
			/* mov %r10, off32(%rdi) */
			EMIT3_off32(0x4c, 0x8b, 0x97,
				    offsetof(struct sk_buff, data));
	}

	for (i = 0; i < insn_cnt; i++, insn++) {
		const s32 imm32 = insn->imm;
		u32 dst_reg = insn->dst_reg;
		u32 src_reg = insn->src_reg;
		u8 b1 = 0, b2 = 0, b3 = 0;
		s64 jmp_offset;
		u8 jmp_cond;
		int ilen;
		u8 *func;

		switch (insn->code) {
			/* ALU */
		case BPF_ALU | BPF_ADD | BPF_X:
		case BPF_ALU | BPF_SUB | BPF_X:
		case BPF_ALU | BPF_AND | BPF_X:
		case BPF_ALU | BPF_OR | BPF_X:
		case BPF_ALU | BPF_XOR | BPF_X:
		case BPF_ALU64 | BPF_ADD | BPF_X:
		case BPF_ALU64 | BPF_SUB | BPF_X:
		case BPF_ALU64 | BPF_AND | BPF_X:
		case BPF_ALU64 | BPF_OR | BPF_X:
		case BPF_ALU64 | BPF_XOR | BPF_X:
			switch (BPF_OP(insn->code)) {
			case BPF_ADD: b2 = 0x01; break;
			case BPF_SUB: b2 = 0x29; break;
			case BPF_AND: b2 = 0x21; break;
			case BPF_OR: b2 = 0x09; break;
			case BPF_XOR: b2 = 0x31; break;
			}
			if (BPF_CLASS(insn->code) == BPF_ALU64)
				EMIT1(add_2mod(0x48, dst_reg, src_reg));
			else if (is_ereg(dst_reg) || is_ereg(src_reg))
				EMIT1(add_2mod(0x40, dst_reg, src_reg));
			EMIT2(b2, add_2reg(0xC0, dst_reg, src_reg));
			break;

			/* mov dst, src */
		case BPF_ALU64 | BPF_MOV | BPF_X:
			EMIT_mov(dst_reg, src_reg);
			break;

			/* mov32 dst, src */
		case BPF_ALU | BPF_MOV | BPF_X:
			if (is_ereg(dst_reg) || is_ereg(src_reg))
				EMIT1(add_2mod(0x40, dst_reg, src_reg));
			EMIT2(0x89, add_2reg(0xC0, dst_reg, src_reg));
			break;

			/* neg dst */
		case BPF_ALU | BPF_NEG:
		case BPF_ALU64 | BPF_NEG:
			if (BPF_CLASS(insn->code) == BPF_ALU64)
				EMIT1(add_1mod(0x48, dst_reg));
			else if (is_ereg(dst_reg))
				EMIT1(add_1mod(0x40, dst_reg));
			EMIT2(0xF7, add_1reg(0xD8, dst_reg));
			break;

		case BPF_ALU | BPF_ADD | BPF_K:
		case BPF_ALU | BPF_SUB | BPF_K:
		case BPF_ALU | BPF_AND | BPF_K:
		case BPF_ALU | BPF_OR | BPF_K:
		case BPF_ALU | BPF_XOR | BPF_K:
		case BPF_ALU64 | BPF_ADD | BPF_K:
		case BPF_ALU64 | BPF_SUB | BPF_K:
		case BPF_ALU64 | BPF_AND | BPF_K:
		case BPF_ALU64 | BPF_OR | BPF_K:
		case BPF_ALU64 | BPF_XOR | BPF_K:
			if (BPF_CLASS(insn->code) == BPF_ALU64)
				EMIT1(add_1mod(0x48, dst_reg));
			else if (is_ereg(dst_reg))
				EMIT1(add_1mod(0x40, dst_reg));

			switch (BPF_OP(insn->code)) {
			case BPF_ADD: b3 = 0xC0; break;
			case BPF_SUB: b3 = 0xE8; break;
			case BPF_AND: b3 = 0xE0; break;
			case BPF_OR: b3 = 0xC8; break;
			case BPF_XOR: b3 = 0xF0; break;
			}

			if (is_imm8(imm32))
				EMIT3(0x83, add_1reg(b3, dst_reg), imm32);
			else
				EMIT2_off32(0x81, add_1reg(b3, dst_reg), imm32);
			break;

		case BPF_ALU64 | BPF_MOV | BPF_K:
			/* optimization: if imm32 is positive,
			 * use 'mov eax, imm32' (which zero-extends imm32)
			 * to save 2 bytes
			 */
			if (imm32 < 0) {
				/* 'mov rax, imm32' sign extends imm32 */
				b1 = add_1mod(0x48, dst_reg);
				b2 = 0xC7;
				b3 = 0xC0;
				EMIT3_off32(b1, b2, add_1reg(b3, dst_reg), imm32);
				break;
			}

		case BPF_ALU | BPF_MOV | BPF_K:
			/* mov %eax, imm32 */
			if (is_ereg(dst_reg))
				EMIT1(add_1mod(0x40, dst_reg));
			EMIT1_off32(add_1reg(0xB8, dst_reg), imm32);
			break;

		case BPF_LD | BPF_IMM | BPF_DW:
			if (insn[1].code != 0 || insn[1].src_reg != 0 ||
			    insn[1].dst_reg != 0 || insn[1].off != 0) {
				/* verifier must catch invalid insns */
				pr_err("invalid BPF_LD_IMM64 insn\n");
				return -EINVAL;
			}

			/* movabsq %rax, imm64 */
			EMIT2(add_1mod(0x48, dst_reg), add_1reg(0xB8, dst_reg));
			EMIT(insn[0].imm, 4);
			EMIT(insn[1].imm, 4);

			insn++;
			i++;
			break;

			/* dst %= src, dst /= src, dst %= imm32, dst /= imm32 */
		case BPF_ALU | BPF_MOD | BPF_X:
		case BPF_ALU | BPF_DIV | BPF_X:
		case BPF_ALU | BPF_MOD | BPF_K:
		case BPF_ALU | BPF_DIV | BPF_K:
		case BPF_ALU64 | BPF_MOD | BPF_X:
		case BPF_ALU64 | BPF_DIV | BPF_X:
		case BPF_ALU64 | BPF_MOD | BPF_K:
		case BPF_ALU64 | BPF_DIV | BPF_K:
			EMIT1(0x50); /* push rax */
			EMIT1(0x52); /* push rdx */

			if (BPF_SRC(insn->code) == BPF_X)
				/* mov r11, src_reg */
				EMIT_mov(AUX_REG, src_reg);
			else
				/* mov r11, imm32 */
				EMIT3_off32(0x49, 0xC7, 0xC3, imm32);

			/* mov rax, dst_reg */
			EMIT_mov(BPF_REG_0, dst_reg);

			/* xor edx, edx
			 * equivalent to 'xor rdx, rdx', but one byte less
			 */
			EMIT2(0x31, 0xd2);

			if (BPF_SRC(insn->code) == BPF_X) {
				/* if (src_reg == 0) return 0 */

				/* cmp r11, 0 */
				EMIT4(0x49, 0x83, 0xFB, 0x00);

				/* jne .+9 (skip over pop, pop, xor and jmp) */
				EMIT2(X86_JNE, 1 + 1 + 2 + 5);
				EMIT1(0x5A); /* pop rdx */
				EMIT1(0x58); /* pop rax */
				EMIT2(0x31, 0xc0); /* xor eax, eax */

				/* jmp cleanup_addr
				 * addrs[i] - 11, because there are 11 bytes
				 * after this insn: div, mov, pop, pop, mov
				 */
				jmp_offset = ctx->cleanup_addr - (addrs[i] - 11);
				EMIT1_off32(0xE9, jmp_offset);
			}

			if (BPF_CLASS(insn->code) == BPF_ALU64)
				/* div r11 */
				EMIT3(0x49, 0xF7, 0xF3);
			else
				/* div r11d */
				EMIT3(0x41, 0xF7, 0xF3);

			if (BPF_OP(insn->code) == BPF_MOD)
				/* mov r11, rdx */
				EMIT3(0x49, 0x89, 0xD3);
			else
				/* mov r11, rax */
				EMIT3(0x49, 0x89, 0xC3);

			EMIT1(0x5A); /* pop rdx */
			EMIT1(0x58); /* pop rax */

			/* mov dst_reg, r11 */
			EMIT_mov(dst_reg, AUX_REG);
			break;

		case BPF_ALU | BPF_MUL | BPF_K:
		case BPF_ALU | BPF_MUL | BPF_X:
		case BPF_ALU64 | BPF_MUL | BPF_K:
		case BPF_ALU64 | BPF_MUL | BPF_X:
			EMIT1(0x50); /* push rax */
			EMIT1(0x52); /* push rdx */

			/* mov r11, dst_reg */
			EMIT_mov(AUX_REG, dst_reg);

			if (BPF_SRC(insn->code) == BPF_X)
				/* mov rax, src_reg */
				EMIT_mov(BPF_REG_0, src_reg);
			else
				/* mov rax, imm32 */
				EMIT3_off32(0x48, 0xC7, 0xC0, imm32);

			if (BPF_CLASS(insn->code) == BPF_ALU64)
				EMIT1(add_1mod(0x48, AUX_REG));
			else if (is_ereg(AUX_REG))
				EMIT1(add_1mod(0x40, AUX_REG));
			/* mul(q) r11 */
			EMIT2(0xF7, add_1reg(0xE0, AUX_REG));

			/* mov r11, rax */
			EMIT_mov(AUX_REG, BPF_REG_0);

			EMIT1(0x5A); /* pop rdx */
			EMIT1(0x58); /* pop rax */

			/* mov dst_reg, r11 */
			EMIT_mov(dst_reg, AUX_REG);
			break;

			/* shifts */
		case BPF_ALU | BPF_LSH | BPF_K:
		case BPF_ALU | BPF_RSH | BPF_K:
		case BPF_ALU | BPF_ARSH | BPF_K:
		case BPF_ALU64 | BPF_LSH | BPF_K:
		case BPF_ALU64 | BPF_RSH | BPF_K:
		case BPF_ALU64 | BPF_ARSH | BPF_K:
			if (BPF_CLASS(insn->code) == BPF_ALU64)
				EMIT1(add_1mod(0x48, dst_reg));
			else if (is_ereg(dst_reg))
				EMIT1(add_1mod(0x40, dst_reg));

			switch (BPF_OP(insn->code)) {
			case BPF_LSH: b3 = 0xE0; break;
			case BPF_RSH: b3 = 0xE8; break;
			case BPF_ARSH: b3 = 0xF8; break;
			}
			EMIT3(0xC1, add_1reg(b3, dst_reg), imm32);
			break;

		case BPF_ALU | BPF_LSH | BPF_X:
		case BPF_ALU | BPF_RSH | BPF_X:
		case BPF_ALU | BPF_ARSH | BPF_X:
		case BPF_ALU64 | BPF_LSH | BPF_X:
		case BPF_ALU64 | BPF_RSH | BPF_X:
		case BPF_ALU64 | BPF_ARSH | BPF_X:

			/* check for bad case when dst_reg == rcx */
			if (dst_reg == BPF_REG_4) {
				/* mov r11, dst_reg */
				EMIT_mov(AUX_REG, dst_reg);
				dst_reg = AUX_REG;
			}

			if (src_reg != BPF_REG_4) { /* common case */
				EMIT1(0x51); /* push rcx */

				/* mov rcx, src_reg */
				EMIT_mov(BPF_REG_4, src_reg);
			}

			/* shl %rax, %cl | shr %rax, %cl | sar %rax, %cl */
			if (BPF_CLASS(insn->code) == BPF_ALU64)
				EMIT1(add_1mod(0x48, dst_reg));
			else if (is_ereg(dst_reg))
				EMIT1(add_1mod(0x40, dst_reg));

			switch (BPF_OP(insn->code)) {
			case BPF_LSH: b3 = 0xE0; break;
			case BPF_RSH: b3 = 0xE8; break;
			case BPF_ARSH: b3 = 0xF8; break;
			}
			EMIT2(0xD3, add_1reg(b3, dst_reg));

			if (src_reg != BPF_REG_4)
				EMIT1(0x59); /* pop rcx */

			if (insn->dst_reg == BPF_REG_4)
				/* mov dst_reg, r11 */
				EMIT_mov(insn->dst_reg, AUX_REG);
			break;

		case BPF_ALU | BPF_END | BPF_FROM_BE:
			switch (imm32) {
			case 16:
				/* emit 'ror %ax, 8' to swap lower 2 bytes */
				EMIT1(0x66);
				if (is_ereg(dst_reg))
					EMIT1(0x41);
				EMIT3(0xC1, add_1reg(0xC8, dst_reg), 8);

				/* emit 'movzwl eax, ax' */
				if (is_ereg(dst_reg))
					EMIT3(0x45, 0x0F, 0xB7);
				else
					EMIT2(0x0F, 0xB7);
				EMIT1(add_2reg(0xC0, dst_reg, dst_reg));
				break;
			case 32:
				/* emit 'bswap eax' to swap lower 4 bytes */
				if (is_ereg(dst_reg))
					EMIT2(0x41, 0x0F);
				else
					EMIT1(0x0F);
				EMIT1(add_1reg(0xC8, dst_reg));
				break;
			case 64:
				/* emit 'bswap rax' to swap 8 bytes */
				EMIT3(add_1mod(0x48, dst_reg), 0x0F,
				      add_1reg(0xC8, dst_reg));
				break;
			}
			break;

		case BPF_ALU | BPF_END | BPF_FROM_LE:
			switch (imm32) {
			case 16:
				/* emit 'movzwl eax, ax' to zero extend 16-bit
				 * into 64 bit
				 */
				if (is_ereg(dst_reg))
					EMIT3(0x45, 0x0F, 0xB7);
				else
					EMIT2(0x0F, 0xB7);
				EMIT1(add_2reg(0xC0, dst_reg, dst_reg));
				break;
			case 32:
				/* emit 'mov eax, eax' to clear upper 32-bits */
				if (is_ereg(dst_reg))
					EMIT1(0x45);
				EMIT2(0x89, add_2reg(0xC0, dst_reg, dst_reg));
				break;
			case 64:
				/* nop */
				break;
			}
			break;

			/* ST: *(u8*)(dst_reg + off) = imm */
		case BPF_ST | BPF_MEM | BPF_B:
			if (is_ereg(dst_reg))
				EMIT2(0x41, 0xC6);
			else
				EMIT1(0xC6);
			goto st;
		case BPF_ST | BPF_MEM | BPF_H:
			if (is_ereg(dst_reg))
				EMIT3(0x66, 0x41, 0xC7);
			else
				EMIT2(0x66, 0xC7);
			goto st;
		case BPF_ST | BPF_MEM | BPF_W:
			if (is_ereg(dst_reg))
				EMIT2(0x41, 0xC7);
			else
				EMIT1(0xC7);
			goto st;
		case BPF_ST | BPF_MEM | BPF_DW:
			EMIT2(add_1mod(0x48, dst_reg), 0xC7);

st:			if (is_imm8(insn->off))
				EMIT2(add_1reg(0x40, dst_reg), insn->off);
			else
				EMIT1_off32(add_1reg(0x80, dst_reg), insn->off);

			EMIT(imm32, bpf_size_to_x86_bytes(BPF_SIZE(insn->code)));
			break;

			/* STX: *(u8*)(dst_reg + off) = src_reg */
		case BPF_STX | BPF_MEM | BPF_B:
			/* emit 'mov byte ptr [rax + off], al' */
			if (is_ereg(dst_reg) || is_ereg(src_reg) ||
			    /* have to add extra byte for x86 SIL, DIL regs */
			    src_reg == BPF_REG_1 || src_reg == BPF_REG_2)
				EMIT2(add_2mod(0x40, dst_reg, src_reg), 0x88);
			else
				EMIT1(0x88);
			goto stx;
		case BPF_STX | BPF_MEM | BPF_H:
			if (is_ereg(dst_reg) || is_ereg(src_reg))
				EMIT3(0x66, add_2mod(0x40, dst_reg, src_reg), 0x89);
			else
				EMIT2(0x66, 0x89);
			goto stx;
		case BPF_STX | BPF_MEM | BPF_W:
			if (is_ereg(dst_reg) || is_ereg(src_reg))
				EMIT2(add_2mod(0x40, dst_reg, src_reg), 0x89);
			else
				EMIT1(0x89);
			goto stx;
		case BPF_STX | BPF_MEM | BPF_DW:
			EMIT2(add_2mod(0x48, dst_reg, src_reg), 0x89);
stx:			if (is_imm8(insn->off))
				EMIT2(add_2reg(0x40, dst_reg, src_reg), insn->off);
			else
				EMIT1_off32(add_2reg(0x80, dst_reg, src_reg),
					    insn->off);
			break;

			/* LDX: dst_reg = *(u8*)(src_reg + off) */
		case BPF_LDX | BPF_MEM | BPF_B:
			/* emit 'movzx rax, byte ptr [rax + off]' */
			EMIT3(add_2mod(0x48, src_reg, dst_reg), 0x0F, 0xB6);
			goto ldx;
		case BPF_LDX | BPF_MEM | BPF_H:
			/* emit 'movzx rax, word ptr [rax + off]' */
			EMIT3(add_2mod(0x48, src_reg, dst_reg), 0x0F, 0xB7);
			goto ldx;
		case BPF_LDX | BPF_MEM | BPF_W:
			/* emit 'mov eax, dword ptr [rax+0x14]' */
			if (is_ereg(dst_reg) || is_ereg(src_reg))
				EMIT2(add_2mod(0x40, src_reg, dst_reg), 0x8B);
			else
				EMIT1(0x8B);
			goto ldx;
		case BPF_LDX | BPF_MEM | BPF_DW:
			/* emit 'mov rax, qword ptr [rax+0x14]' */
			EMIT2(add_2mod(0x48, src_reg, dst_reg), 0x8B);
ldx:			/* if insn->off == 0 we can save one extra byte, but
			 * special case of x86 r13 which always needs an offset
			 * is not worth the hassle
			 */
			if (is_imm8(insn->off))
				EMIT2(add_2reg(0x40, src_reg, dst_reg), insn->off);
			else
				EMIT1_off32(add_2reg(0x80, src_reg, dst_reg),
					    insn->off);
			break;

			/* STX XADD: lock *(u32*)(dst_reg + off) += src_reg */
		case BPF_STX | BPF_XADD | BPF_W:
			/* emit 'lock add dword ptr [rax + off], eax' */
			if (is_ereg(dst_reg) || is_ereg(src_reg))
				EMIT3(0xF0, add_2mod(0x40, dst_reg, src_reg), 0x01);
			else
				EMIT2(0xF0, 0x01);
			goto xadd;
		case BPF_STX | BPF_XADD | BPF_DW:
			EMIT3(0xF0, add_2mod(0x48, dst_reg, src_reg), 0x01);
xadd:			if (is_imm8(insn->off))
				EMIT2(add_2reg(0x40, dst_reg, src_reg), insn->off);
			else
				EMIT1_off32(add_2reg(0x80, dst_reg, src_reg),
					    insn->off);
			break;

			/* call */
		case BPF_JMP | BPF_CALL:
			func = (u8 *) __bpf_call_base + imm32;
			jmp_offset = func - (image + addrs[i]);
			if (seen_ld_abs) {
				EMIT2(0x41, 0x52); /* push %r10 */
				EMIT2(0x41, 0x51); /* push %r9 */
				/* need to adjust jmp offset, since
				 * pop %r9, pop %r10 take 4 bytes after call insn
				 */
				jmp_offset += 4;
			}
			if (!imm32 || !is_simm32(jmp_offset)) {
				pr_err("unsupported bpf func %d addr %p image %p\n",
				       imm32, func, image);
				return -EINVAL;
			}
			EMIT1_off32(0xE8, jmp_offset);
			if (seen_ld_abs) {
				EMIT2(0x41, 0x59); /* pop %r9 */
				EMIT2(0x41, 0x5A); /* pop %r10 */
			}
			break;

			/* cond jump */
		case BPF_JMP | BPF_JEQ | BPF_X:
		case BPF_JMP | BPF_JNE | BPF_X:
		case BPF_JMP | BPF_JGT | BPF_X:
		case BPF_JMP | BPF_JGE | BPF_X:
		case BPF_JMP | BPF_JSGT | BPF_X:
		case BPF_JMP | BPF_JSGE | BPF_X:
			/* cmp dst_reg, src_reg */
			EMIT3(add_2mod(0x48, dst_reg, src_reg), 0x39,
			      add_2reg(0xC0, dst_reg, src_reg));
			goto emit_cond_jmp;

		case BPF_JMP | BPF_JSET | BPF_X:
			/* test dst_reg, src_reg */
			EMIT3(add_2mod(0x48, dst_reg, src_reg), 0x85,
			      add_2reg(0xC0, dst_reg, src_reg));
			goto emit_cond_jmp;

		case BPF_JMP | BPF_JSET | BPF_K:
			/* test dst_reg, imm32 */
			EMIT1(add_1mod(0x48, dst_reg));
			EMIT2_off32(0xF7, add_1reg(0xC0, dst_reg), imm32);
			goto emit_cond_jmp;

		case BPF_JMP | BPF_JEQ | BPF_K:
		case BPF_JMP | BPF_JNE | BPF_K:
		case BPF_JMP | BPF_JGT | BPF_K:
		case BPF_JMP | BPF_JGE | BPF_K:
		case BPF_JMP | BPF_JSGT | BPF_K:
		case BPF_JMP | BPF_JSGE | BPF_K:
			/* cmp dst_reg, imm8/32 */
			EMIT1(add_1mod(0x48, dst_reg));

			if (is_imm8(imm32))
				EMIT3(0x83, add_1reg(0xF8, dst_reg), imm32);
			else
				EMIT2_off32(0x81, add_1reg(0xF8, dst_reg), imm32);

emit_cond_jmp:		/* convert BPF opcode to x86 */
			switch (BPF_OP(insn->code)) {
			case BPF_JEQ:
				jmp_cond = X86_JE;
				break;
			case BPF_JSET:
			case BPF_JNE:
				jmp_cond = X86_JNE;
				break;
			case BPF_JGT:
				/* GT is unsigned '>', JA in x86 */
				jmp_cond = X86_JA;
				break;
			case BPF_JGE:
				/* GE is unsigned '>=', JAE in x86 */
				jmp_cond = X86_JAE;
				break;
			case BPF_JSGT:
				/* signed '>', GT in x86 */
				jmp_cond = X86_JG;
				break;
			case BPF_JSGE:
				/* signed '>=', GE in x86 */
				jmp_cond = X86_JGE;
				break;
			default: /* to silence gcc warning */
				return -EFAULT;
			}
			jmp_offset = addrs[i + insn->off] - addrs[i];
			if (is_imm8(jmp_offset)) {
				EMIT2(jmp_cond, jmp_offset);
			} else if (is_simm32(jmp_offset)) {
				EMIT2_off32(0x0F, jmp_cond + 0x10, jmp_offset);
			} else {
				pr_err("cond_jmp gen bug %llx\n", jmp_offset);
				return -EFAULT;
			}

			break;

		case BPF_JMP | BPF_JA:
			jmp_offset = addrs[i + insn->off] - addrs[i];
			if (!jmp_offset)
				/* optimize out nop jumps */
				break;
emit_jmp:
			if (is_imm8(jmp_offset)) {
				EMIT2(0xEB, jmp_offset);
			} else if (is_simm32(jmp_offset)) {
				EMIT1_off32(0xE9, jmp_offset);
			} else {
				pr_err("jmp gen bug %llx\n", jmp_offset);
				return -EFAULT;
			}
			break;

		case BPF_LD | BPF_IND | BPF_W:
			func = sk_load_word;
			goto common_load;
		case BPF_LD | BPF_ABS | BPF_W:
			func = CHOOSE_LOAD_FUNC(imm32, sk_load_word);
common_load:
			ctx->seen_ld_abs = seen_ld_abs = true;
			jmp_offset = func - (image + addrs[i]);
			if (!func || !is_simm32(jmp_offset)) {
				pr_err("unsupported bpf func %d addr %p image %p\n",
				       imm32, func, image);
				return -EINVAL;
			}
			if (BPF_MODE(insn->code) == BPF_ABS) {
				/* mov %esi, imm32 */
				EMIT1_off32(0xBE, imm32);
			} else {
				/* mov %rsi, src_reg */
				EMIT_mov(BPF_REG_2, src_reg);
				if (imm32) {
					if (is_imm8(imm32))
						/* add %esi, imm8 */
						EMIT3(0x83, 0xC6, imm32);
					else
						/* add %esi, imm32 */
						EMIT2_off32(0x81, 0xC6, imm32);
				}
			}
			/* skb pointer is in R6 (%rbx), it will be copied into
			 * %rdi if skb_copy_bits() call is necessary.
			 * sk_load_* helpers also use %r10 and %r9d.
			 * See bpf_jit.S
			 */
			EMIT1_off32(0xE8, jmp_offset); /* call */
			break;

		case BPF_LD | BPF_IND | BPF_H:
			func = sk_load_half;
			goto common_load;
		case BPF_LD | BPF_ABS | BPF_H:
			func = CHOOSE_LOAD_FUNC(imm32, sk_load_half);
			goto common_load;
		case BPF_LD | BPF_IND | BPF_B:
			func = sk_load_byte;
			goto common_load;
		case BPF_LD | BPF_ABS | BPF_B:
			func = CHOOSE_LOAD_FUNC(imm32, sk_load_byte);
			goto common_load;

		case BPF_JMP | BPF_EXIT:
			if (seen_exit) {
				jmp_offset = ctx->cleanup_addr - addrs[i];
				goto emit_jmp;
			}
			seen_exit = true;
			/* update cleanup_addr */
			ctx->cleanup_addr = proglen;
			/* mov rbx, qword ptr [rbp-X] */
			EMIT3_off32(0x48, 0x8B, 0x9D, -stacksize);
			/* mov r13, qword ptr [rbp-X] */
			EMIT3_off32(0x4C, 0x8B, 0xAD, -stacksize + 8);
			/* mov r14, qword ptr [rbp-X] */
			EMIT3_off32(0x4C, 0x8B, 0xB5, -stacksize + 16);
			/* mov r15, qword ptr [rbp-X] */
			EMIT3_off32(0x4C, 0x8B, 0xBD, -stacksize + 24);

			EMIT1(0xC9); /* leave */
			EMIT1(0xC3); /* ret */
			break;

		default:
			/* By design x64 JIT should support all BPF instructions
			 * This error will be seen if new instruction was added
			 * to interpreter, but not to JIT
			 * or if there is junk in bpf_prog
			 */
			pr_err("bpf_jit: unknown opcode %02x\n", insn->code);
			return -EINVAL;
		}

		ilen = prog - temp;
		if (ilen > BPF_MAX_INSN_SIZE) {
			pr_err("bpf_jit_compile fatal insn size error\n");
			return -EFAULT;
		}

		if (image) {
			if (unlikely(proglen + ilen > oldproglen)) {
				pr_err("bpf_jit_compile fatal error\n");
				return -EFAULT;
			}
			memcpy(image + proglen, temp, ilen);
		}
		proglen += ilen;
		addrs[i] = proglen;
		prog = temp;
	}
	return proglen;
}