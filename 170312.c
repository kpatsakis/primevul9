static struct bpf_insn *bpf_insn_prepare_dump(const struct bpf_prog *prog,
					      const struct cred *f_cred)
{
	const struct bpf_map *map;
	struct bpf_insn *insns;
	u32 off, type;
	u64 imm;
	u8 code;
	int i;

	insns = kmemdup(prog->insnsi, bpf_prog_insn_size(prog),
			GFP_USER);
	if (!insns)
		return insns;

	for (i = 0; i < prog->len; i++) {
		code = insns[i].code;

		if (code == (BPF_JMP | BPF_TAIL_CALL)) {
			insns[i].code = BPF_JMP | BPF_CALL;
			insns[i].imm = BPF_FUNC_tail_call;
			/* fall-through */
		}
		if (code == (BPF_JMP | BPF_CALL) ||
		    code == (BPF_JMP | BPF_CALL_ARGS)) {
			if (code == (BPF_JMP | BPF_CALL_ARGS))
				insns[i].code = BPF_JMP | BPF_CALL;
			if (!bpf_dump_raw_ok(f_cred))
				insns[i].imm = 0;
			continue;
		}
		if (BPF_CLASS(code) == BPF_LDX && BPF_MODE(code) == BPF_PROBE_MEM) {
			insns[i].code = BPF_LDX | BPF_SIZE(code) | BPF_MEM;
			continue;
		}

		if (code != (BPF_LD | BPF_IMM | BPF_DW))
			continue;

		imm = ((u64)insns[i + 1].imm << 32) | (u32)insns[i].imm;
		map = bpf_map_from_imm(prog, imm, &off, &type);
		if (map) {
			insns[i].src_reg = type;
			insns[i].imm = map->id;
			insns[i + 1].imm = off;
			continue;
		}
	}

	return insns;
}