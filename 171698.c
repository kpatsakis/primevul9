static int __net_init sock_inuse_init_net(struct net *net)
{
	net->core.inuse = alloc_percpu(struct prot_inuse);
	return net->core.inuse ? 0 : -ENOMEM;
}