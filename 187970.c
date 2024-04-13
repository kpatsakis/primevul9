static int packet_seq_show(struct seq_file *seq, void *v)
{
	if (v == SEQ_START_TOKEN)
		seq_puts(seq, "sk       RefCnt Type Proto  Iface R Rmem   User   Inode\n");
	else {
		struct sock *s = sk_entry(v);
		const struct packet_sock *po = pkt_sk(s);

		seq_printf(seq,
			   "%pK %-6d %-4d %04x   %-5d %1d %-6u %-6u %-6lu\n",
			   s,
			   atomic_read(&s->sk_refcnt),
			   s->sk_type,
			   ntohs(po->num),
			   po->ifindex,
			   po->running,
			   atomic_read(&s->sk_rmem_alloc),
			   sock_i_uid(s),
			   sock_i_ino(s));
	}

	return 0;
}