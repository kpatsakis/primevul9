struct bpf_prog *bpf_prog_by_id(u32 id)
{
	struct bpf_prog *prog;

	if (!id)
		return ERR_PTR(-ENOENT);

	spin_lock_bh(&prog_idr_lock);
	prog = idr_find(&prog_idr, id);
	if (prog)
		prog = bpf_prog_inc_not_zero(prog);
	else
		prog = ERR_PTR(-ENOENT);
	spin_unlock_bh(&prog_idr_lock);
	return prog;
}