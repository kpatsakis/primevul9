static int packet_create(struct net *net, struct socket *sock, int protocol,
			 int kern)
{
	struct sock *sk;
	struct packet_sock *po;
	__be16 proto = (__force __be16)protocol; /* weird, but documented */
	int err;

	if (!capable(CAP_NET_RAW))
		return -EPERM;
	if (sock->type != SOCK_DGRAM && sock->type != SOCK_RAW &&
	    sock->type != SOCK_PACKET)
		return -ESOCKTNOSUPPORT;

	sock->state = SS_UNCONNECTED;

	err = -ENOBUFS;
	sk = sk_alloc(net, PF_PACKET, GFP_KERNEL, &packet_proto);
	if (sk == NULL)
		goto out;

	sock->ops = &packet_ops;
	if (sock->type == SOCK_PACKET)
		sock->ops = &packet_ops_spkt;

	sock_init_data(sock, sk);

	po = pkt_sk(sk);
	sk->sk_family = PF_PACKET;
	po->num = proto;

	sk->sk_destruct = packet_sock_destruct;
	sk_refcnt_debug_inc(sk);

	/*
	 *	Attach a protocol block
	 */

	spin_lock_init(&po->bind_lock);
	mutex_init(&po->pg_vec_lock);
	po->prot_hook.func = packet_rcv;

	if (sock->type == SOCK_PACKET)
		po->prot_hook.func = packet_rcv_spkt;

	po->prot_hook.af_packet_priv = sk;

	if (proto) {
		po->prot_hook.type = proto;
		register_prot_hook(sk);
	}

	spin_lock_bh(&net->packet.sklist_lock);
	sk_add_node_rcu(sk, &net->packet.sklist);
	sock_prot_inuse_add(net, &packet_proto, 1);
	spin_unlock_bh(&net->packet.sklist_lock);

	return 0;
out:
	return err;
}