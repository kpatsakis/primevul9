static int do_check_common(struct bpf_verifier_env *env, int subprog)
{
	bool pop_log = !(env->log.level & BPF_LOG_LEVEL2);
	struct bpf_verifier_state *state;
	struct bpf_reg_state *regs;
	int ret, i;

	env->prev_linfo = NULL;
	env->pass_cnt++;

	state = kzalloc(sizeof(struct bpf_verifier_state), GFP_KERNEL);
	if (!state)
		return -ENOMEM;
	state->curframe = 0;
	state->speculative = false;
	state->branches = 1;
	state->frame[0] = kzalloc(sizeof(struct bpf_func_state), GFP_KERNEL);
	if (!state->frame[0]) {
		kfree(state);
		return -ENOMEM;
	}
	env->cur_state = state;
	init_func_state(env, state->frame[0],
			BPF_MAIN_FUNC /* callsite */,
			0 /* frameno */,
			subprog);

	regs = state->frame[state->curframe]->regs;
	if (subprog || env->prog->type == BPF_PROG_TYPE_EXT) {
		ret = btf_prepare_func_args(env, subprog, regs);
		if (ret)
			goto out;
		for (i = BPF_REG_1; i <= BPF_REG_5; i++) {
			if (regs[i].type == PTR_TO_CTX)
				mark_reg_known_zero(env, regs, i);
			else if (regs[i].type == SCALAR_VALUE)
				mark_reg_unknown(env, regs, i);
			else if (regs[i].type == PTR_TO_MEM_OR_NULL) {
				const u32 mem_size = regs[i].mem_size;

				mark_reg_known_zero(env, regs, i);
				regs[i].mem_size = mem_size;
				regs[i].id = ++env->id_gen;
			}
		}
	} else {
		/* 1st arg to a function */
		regs[BPF_REG_1].type = PTR_TO_CTX;
		mark_reg_known_zero(env, regs, BPF_REG_1);
		ret = btf_check_func_arg_match(env, subprog, regs);
		if (ret == -EFAULT)
			/* unlikely verifier bug. abort.
			 * ret == 0 and ret < 0 are sadly acceptable for
			 * main() function due to backward compatibility.
			 * Like socket filter program may be written as:
			 * int bpf_prog(struct pt_regs *ctx)
			 * and never dereference that ctx in the program.
			 * 'struct pt_regs' is a type mismatch for socket
			 * filter that should be using 'struct __sk_buff'.
			 */
			goto out;
	}

	ret = do_check(env);
out:
	/* check for NULL is necessary, since cur_state can be freed inside
	 * do_check() under memory pressure.
	 */
	if (env->cur_state) {
		free_verifier_state(env->cur_state, true);
		env->cur_state = NULL;
	}
	while (!pop_stack(env, NULL, NULL, false));
	if (!ret && pop_log)
		bpf_vlog_reset(&env->log, 0);
	free_states(env);
	if (ret)
		/* clean aux data in case subprog was rejected */
		sanitize_insn_aux_data(env);
	return ret;
}