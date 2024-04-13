static int packet_do_bind(struct sock *sk, struct net_device *dev, __be16 protocol)
{
	struct packet_sock *po = pkt_sk(sk);

	if (po->fanout)
		return -EINVAL;

	lock_sock(sk);

	spin_lock(&po->bind_lock);
	unregister_prot_hook(sk, true);
	po->num = protocol;
	po->prot_hook.type = protocol;
	if (po->prot_hook.dev)
		dev_put(po->prot_hook.dev);
	po->prot_hook.dev = dev;

	po->ifindex = dev ? dev->ifindex : 0;

	if (protocol == 0)
		goto out_unlock;

	if (!dev || (dev->flags & IFF_UP)) {
		register_prot_hook(sk);
	} else {
		sk->sk_err = ENETDOWN;
		if (!sock_flag(sk, SOCK_DEAD))
			sk->sk_error_report(sk);
	}

out_unlock:
	spin_unlock(&po->bind_lock);
	release_sock(sk);
	return 0;
}