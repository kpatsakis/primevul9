static void register_prot_hook(struct sock *sk)
{
	struct packet_sock *po = pkt_sk(sk);
	if (!po->running) {
		if (po->fanout)
			__fanout_link(sk, po);
		else
			dev_add_pack(&po->prot_hook);
		sock_hold(sk);
		po->running = 1;
	}
}