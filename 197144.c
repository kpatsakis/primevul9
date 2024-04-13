static inline void slave_disable_netpoll(struct slave *slave)
{
	struct netpoll *np = slave->np;

	if (!np)
		return;

	slave->np = NULL;

	__netpoll_free(np);
}