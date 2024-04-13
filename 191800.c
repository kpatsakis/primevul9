static int __netlink_create(struct net *net, struct socket *sock,
			    struct mutex *cb_mutex, int protocol,
			    int kern)
{
	struct sock *sk;
	struct netlink_sock *nlk;

	sock->ops = &netlink_ops;

	sk = sk_alloc(net, PF_NETLINK, GFP_KERNEL, &netlink_proto, kern);
	if (!sk)
		return -ENOMEM;

	sock_init_data(sock, sk);

	nlk = nlk_sk(sk);
	if (cb_mutex) {
		nlk->cb_mutex = cb_mutex;
	} else {
		nlk->cb_mutex = &nlk->cb_def_mutex;
		mutex_init(nlk->cb_mutex);
		lockdep_set_class_and_name(nlk->cb_mutex,
					   nlk_cb_mutex_keys + protocol,
					   nlk_cb_mutex_key_strings[protocol]);
	}
	init_waitqueue_head(&nlk->wait);

	sk->sk_destruct = netlink_sock_destruct;
	sk->sk_protocol = protocol;
	return 0;
}