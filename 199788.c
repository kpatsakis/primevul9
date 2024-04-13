static int if6_seq_show(struct seq_file *seq, void *v)
{
	struct inet6_ifaddr *ifp = (struct inet6_ifaddr *)v;
	seq_printf(seq, "%pi6 %02x %02x %02x %02x %8s\n",
		   &ifp->addr,
		   ifp->idev->dev->ifindex,
		   ifp->prefix_len,
		   ifp->scope,
		   ifp->flags,
		   ifp->idev->dev->name);
	return 0;
}