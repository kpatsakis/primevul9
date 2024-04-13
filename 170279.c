static int check_subprogs(struct bpf_verifier_env *env)
{
	int i, ret, subprog_start, subprog_end, off, cur_subprog = 0;
	struct bpf_subprog_info *subprog = env->subprog_info;
	struct bpf_insn *insn = env->prog->insnsi;
	int insn_cnt = env->prog->len;

	/* Add entry function. */
	ret = add_subprog(env, 0);
	if (ret < 0)
		return ret;

	/* determine subprog starts. The end is one before the next starts */
	for (i = 0; i < insn_cnt; i++) {
		if (!bpf_pseudo_call(insn + i))
			continue;
		if (!env->bpf_capable) {
			verbose(env,
				"function calls to other bpf functions are allowed for CAP_BPF and CAP_SYS_ADMIN\n");
			return -EPERM;
		}
		ret = add_subprog(env, i + insn[i].imm + 1);
		if (ret < 0)
			return ret;
	}

	/* Add a fake 'exit' subprog which could simplify subprog iteration
	 * logic. 'subprog_cnt' should not be increased.
	 */
	subprog[env->subprog_cnt].start = insn_cnt;

	if (env->log.level & BPF_LOG_LEVEL2)
		for (i = 0; i < env->subprog_cnt; i++)
			verbose(env, "func#%d @%d\n", i, subprog[i].start);

	/* now check that all jumps are within the same subprog */
	subprog_start = subprog[cur_subprog].start;
	subprog_end = subprog[cur_subprog + 1].start;
	for (i = 0; i < insn_cnt; i++) {
		u8 code = insn[i].code;

		if (code == (BPF_JMP | BPF_CALL) &&
		    insn[i].imm == BPF_FUNC_tail_call &&
		    insn[i].src_reg != BPF_PSEUDO_CALL)
			subprog[cur_subprog].has_tail_call = true;
		if (BPF_CLASS(code) == BPF_LD &&
		    (BPF_MODE(code) == BPF_ABS || BPF_MODE(code) == BPF_IND))
			subprog[cur_subprog].has_ld_abs = true;
		if (BPF_CLASS(code) != BPF_JMP && BPF_CLASS(code) != BPF_JMP32)
			goto next;
		if (BPF_OP(code) == BPF_EXIT || BPF_OP(code) == BPF_CALL)
			goto next;
		off = i + insn[i].off + 1;
		if (off < subprog_start || off >= subprog_end) {
			verbose(env, "jump out of range from insn %d to %d\n", i, off);
			return -EINVAL;
		}
next:
		if (i == subprog_end - 1) {
			/* to avoid fall-through from one subprog into another
			 * the last insn of the subprog should be either exit
			 * or unconditional jump back
			 */
			if (code != (BPF_JMP | BPF_EXIT) &&
			    code != (BPF_JMP | BPF_JA)) {
				verbose(env, "last insn is not an exit or jmp\n");
				return -EINVAL;
			}
			subprog_start = subprog_end;
			cur_subprog++;
			if (cur_subprog < env->subprog_cnt)
				subprog_end = subprog[cur_subprog + 1].start;
		}
	}
	return 0;
}