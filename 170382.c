struct bpf_prog *bpf_prog_get_curr_or_next(u32 *id)
{
	struct bpf_prog *prog;

	spin_lock_bh(&prog_idr_lock);
again:
	prog = idr_get_next(&prog_idr, id);
	if (prog) {
		prog = bpf_prog_inc_not_zero(prog);
		if (IS_ERR(prog)) {
			(*id)++;
			goto again;
		}
	}
	spin_unlock_bh(&prog_idr_lock);

	return prog;
}