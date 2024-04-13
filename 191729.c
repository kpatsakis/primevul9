static int netlink_seq_show(struct seq_file *seq, void *v)
{
	if (v == SEQ_START_TOKEN) {
		seq_puts(seq,
			 "sk       Eth Pid    Groups   "
			 "Rmem     Wmem     Dump     Locks     Drops     Inode\n");
	} else {
		struct sock *s = v;
		struct netlink_sock *nlk = nlk_sk(s);

		seq_printf(seq, "%pK %-3d %-6u %08x %-8d %-8d %d %-8d %-8d %-8lu\n",
			   s,
			   s->sk_protocol,
			   nlk->portid,
			   nlk->groups ? (u32)nlk->groups[0] : 0,
			   sk_rmem_alloc_get(s),
			   sk_wmem_alloc_get(s),
			   nlk->cb_running,
			   refcount_read(&s->sk_refcnt),
			   atomic_read(&s->sk_drops),
			   sock_i_ino(s)
			);

	}
	return 0;
}