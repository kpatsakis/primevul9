static int __addrconf_sysctl_register(struct net *net, char *dev_name,
		struct inet6_dev *idev, struct ipv6_devconf *p)
{
	int i;
	struct addrconf_sysctl_table *t;
	char path[sizeof("net/ipv6/conf/") + IFNAMSIZ];

	t = kmemdup(&addrconf_sysctl, sizeof(*t), GFP_KERNEL);
	if (t == NULL)
		goto out;

	for (i = 0; t->addrconf_vars[i].data; i++) {
		t->addrconf_vars[i].data += (char *)p - (char *)&ipv6_devconf;
		t->addrconf_vars[i].extra1 = idev; /* embedded; no ref */
		t->addrconf_vars[i].extra2 = net;
	}

	snprintf(path, sizeof(path), "net/ipv6/conf/%s", dev_name);

	t->sysctl_header = register_net_sysctl(net, path, t->addrconf_vars);
	if (t->sysctl_header == NULL)
		goto free;

	p->sysctl = t;
	return 0;

free:
	kfree(t);
out:
	return -ENOBUFS;
}