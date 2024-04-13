static int htab_lru_map_gen_lookup(struct bpf_map *map,
				   struct bpf_insn *insn_buf)
{
	struct bpf_insn *insn = insn_buf;
	const int ret = BPF_REG_0;
	const int ref_reg = BPF_REG_1;

	BUILD_BUG_ON(!__same_type(&__htab_map_lookup_elem,
		     (void *(*)(struct bpf_map *map, void *key))NULL));
	*insn++ = BPF_EMIT_CALL(BPF_CAST_CALL(__htab_map_lookup_elem));
	*insn++ = BPF_JMP_IMM(BPF_JEQ, ret, 0, 4);
	*insn++ = BPF_LDX_MEM(BPF_B, ref_reg, ret,
			      offsetof(struct htab_elem, lru_node) +
			      offsetof(struct bpf_lru_node, ref));
	*insn++ = BPF_JMP_IMM(BPF_JNE, ref_reg, 0, 1);
	*insn++ = BPF_ST_MEM(BPF_B, ret,
			     offsetof(struct htab_elem, lru_node) +
			     offsetof(struct bpf_lru_node, ref),
			     1);
	*insn++ = BPF_ALU64_IMM(BPF_ADD, ret,
				offsetof(struct htab_elem, key) +
				round_up(map->key_size, 8));
	return insn - insn_buf;
}