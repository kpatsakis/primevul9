static void __addrconf_sysctl_unregister(struct ipv6_devconf *p)
{
	struct addrconf_sysctl_table *t;

	if (p->sysctl == NULL)
		return;

	t = p->sysctl;
	p->sysctl = NULL;
	unregister_net_sysctl_table(t->sysctl_header);
	kfree(t);
}