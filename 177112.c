static void dump_rs(struct seq_file *seq, const struct gfs2_blkreserv *rs)
{
	gfs2_print_dbg(seq, "  B: n:%llu s:%llu b:%u f:%u\n",
		       (unsigned long long)rs->rs_inum,
		       (unsigned long long)gfs2_rbm_to_block(&rs->rs_rbm),
		       rs->rs_rbm.offset, rs->rs_free);
}