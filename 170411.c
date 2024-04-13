static int check_max_stack_depth(struct bpf_verifier_env *env)
{
	int depth = 0, frame = 0, idx = 0, i = 0, subprog_end;
	struct bpf_subprog_info *subprog = env->subprog_info;
	struct bpf_insn *insn = env->prog->insnsi;
	bool tail_call_reachable = false;
	int ret_insn[MAX_CALL_FRAMES];
	int ret_prog[MAX_CALL_FRAMES];
	int j;

process_func:
	/* protect against potential stack overflow that might happen when
	 * bpf2bpf calls get combined with tailcalls. Limit the caller's stack
	 * depth for such case down to 256 so that the worst case scenario
	 * would result in 8k stack size (32 which is tailcall limit * 256 =
	 * 8k).
	 *
	 * To get the idea what might happen, see an example:
	 * func1 -> sub rsp, 128
	 *  subfunc1 -> sub rsp, 256
	 *  tailcall1 -> add rsp, 256
	 *   func2 -> sub rsp, 192 (total stack size = 128 + 192 = 320)
	 *   subfunc2 -> sub rsp, 64
	 *   subfunc22 -> sub rsp, 128
	 *   tailcall2 -> add rsp, 128
	 *    func3 -> sub rsp, 32 (total stack size 128 + 192 + 64 + 32 = 416)
	 *
	 * tailcall will unwind the current stack frame but it will not get rid
	 * of caller's stack as shown on the example above.
	 */
	if (idx && subprog[idx].has_tail_call && depth >= 256) {
		verbose(env,
			"tail_calls are not allowed when call stack of previous frames is %d bytes. Too large\n",
			depth);
		return -EACCES;
	}
	/* round up to 32-bytes, since this is granularity
	 * of interpreter stack size
	 */
	depth += round_up(max_t(u32, subprog[idx].stack_depth, 1), 32);
	if (depth > MAX_BPF_STACK) {
		verbose(env, "combined stack size of %d calls is %d. Too large\n",
			frame + 1, depth);
		return -EACCES;
	}
continue_func:
	subprog_end = subprog[idx + 1].start;
	for (; i < subprog_end; i++) {
		if (!bpf_pseudo_call(insn + i))
			continue;
		/* remember insn and function to return to */
		ret_insn[frame] = i + 1;
		ret_prog[frame] = idx;

		/* find the callee */
		i = i + insn[i].imm + 1;
		idx = find_subprog(env, i);
		if (idx < 0) {
			WARN_ONCE(1, "verifier bug. No program starts at insn %d\n",
				  i);
			return -EFAULT;
		}

		if (subprog[idx].has_tail_call)
			tail_call_reachable = true;

		frame++;
		if (frame >= MAX_CALL_FRAMES) {
			verbose(env, "the call stack of %d frames is too deep !\n",
				frame);
			return -E2BIG;
		}
		goto process_func;
	}
	/* if tail call got detected across bpf2bpf calls then mark each of the
	 * currently present subprog frames as tail call reachable subprogs;
	 * this info will be utilized by JIT so that we will be preserving the
	 * tail call counter throughout bpf2bpf calls combined with tailcalls
	 */
	if (tail_call_reachable)
		for (j = 0; j < frame; j++)
			subprog[ret_prog[j]].tail_call_reachable = true;

	/* end of for() loop means the last insn of the 'subprog'
	 * was reached. Doesn't matter whether it was JA or EXIT
	 */
	if (frame == 0)
		return 0;
	depth -= round_up(max_t(u32, subprog[idx].stack_depth, 1), 32);
	frame--;
	i = ret_insn[frame];
	idx = ret_prog[frame];
	goto continue_func;
}