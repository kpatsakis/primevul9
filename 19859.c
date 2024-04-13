static struct sw_flow_actions *nla_alloc_flow_actions(int size)
{
	struct sw_flow_actions *sfa;

	WARN_ON_ONCE(size > MAX_ACTIONS_BUFSIZE);

	sfa = kmalloc(sizeof(*sfa) + size, GFP_KERNEL);
	if (!sfa)
		return ERR_PTR(-ENOMEM);

	sfa->actions_len = 0;
	return sfa;
}