static int pptp_create(struct net *net, struct socket *sock, int kern)
{
	int error = -ENOMEM;
	struct sock *sk;
	struct pppox_sock *po;
	struct pptp_opt *opt;

	sk = sk_alloc(net, PF_PPPOX, GFP_KERNEL, &pptp_sk_proto, kern);
	if (!sk)
		goto out;

	sock_init_data(sock, sk);

	sock->state = SS_UNCONNECTED;
	sock->ops   = &pptp_ops;

	sk->sk_backlog_rcv = pptp_rcv_core;
	sk->sk_state       = PPPOX_NONE;
	sk->sk_type        = SOCK_STREAM;
	sk->sk_family      = PF_PPPOX;
	sk->sk_protocol    = PX_PROTO_PPTP;
	sk->sk_destruct    = pptp_sock_destruct;

	po = pppox_sk(sk);
	opt = &po->proto.pptp;

	opt->seq_sent = 0; opt->seq_recv = 0xffffffff;
	opt->ack_recv = 0; opt->ack_sent = 0xffffffff;

	error = 0;
out:
	return error;
}