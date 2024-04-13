static struct pppox_sock *lookup_chan(u16 call_id, __be32 s_addr)
{
	struct pppox_sock *sock;
	struct pptp_opt *opt;

	rcu_read_lock();
	sock = rcu_dereference(callid_sock[call_id]);
	if (sock) {
		opt = &sock->proto.pptp;
		if (opt->dst_addr.sin_addr.s_addr != s_addr)
			sock = NULL;
		else
			sock_hold(sk_pppox(sock));
	}
	rcu_read_unlock();

	return sock;
}