bool bnx2x_port_after_undi(struct bnx2x *bp)
{
	struct bnx2x_prev_path_list *entry;
	bool val;

	down(&bnx2x_prev_sem);

	entry = bnx2x_prev_path_get_entry(bp);
	val = !!(entry && (entry->undi & (1 << BP_PORT(bp))));

	up(&bnx2x_prev_sem);

	return val;
}