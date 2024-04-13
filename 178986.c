static int atalk_seq_socket_show(struct seq_file *seq, void *v)
{
	struct sock *s;
	struct atalk_sock *at;

	if (v == SEQ_START_TOKEN) {
		seq_printf(seq, "Type Local_addr  Remote_addr Tx_queue "
				"Rx_queue St UID\n");
		goto out;
	}

	s = sk_entry(v);
	at = at_sk(s);

	seq_printf(seq, "%02X   %04X:%02X:%02X  %04X:%02X:%02X  %08X:%08X "
			"%02X %u\n",
		   s->sk_type, ntohs(at->src_net), at->src_node, at->src_port,
		   ntohs(at->dest_net), at->dest_node, at->dest_port,
		   sk_wmem_alloc_get(s),
		   sk_rmem_alloc_get(s),
		   s->sk_state,
		   from_kuid_munged(seq_user_ns(seq), sock_i_uid(s)));
out:
	return 0;
}