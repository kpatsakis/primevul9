static int fixup_call_args(struct bpf_verifier_env *env)
{
#ifndef CONFIG_BPF_JIT_ALWAYS_ON
	struct bpf_prog *prog = env->prog;
	struct bpf_insn *insn = prog->insnsi;
	int i, depth;
#endif
	int err = 0;

	if (env->prog->jit_requested &&
	    !bpf_prog_is_dev_bound(env->prog->aux)) {
		err = jit_subprogs(env);
		if (err == 0)
			return 0;
		if (err == -EFAULT)
			return err;
	}
#ifndef CONFIG_BPF_JIT_ALWAYS_ON
	if (env->subprog_cnt > 1 && env->prog->aux->tail_call_reachable) {
		/* When JIT fails the progs with bpf2bpf calls and tail_calls
		 * have to be rejected, since interpreter doesn't support them yet.
		 */
		verbose(env, "tail_calls are not allowed in non-JITed programs with bpf-to-bpf calls\n");
		return -EINVAL;
	}
	for (i = 0; i < prog->len; i++, insn++) {
		if (!bpf_pseudo_call(insn))
			continue;
		depth = get_callee_stack_depth(env, insn, i);
		if (depth < 0)
			return depth;
		bpf_patch_call_args(insn, depth);
	}
	err = 0;
#endif
	return err;
}