static int bond_get_lowest_level_rcu(struct net_device *dev)
{
	struct net_device *ldev, *next, *now, *dev_stack[MAX_NEST_DEV + 1];
	struct list_head *niter, *iter, *iter_stack[MAX_NEST_DEV + 1];
	int cur = 0, max = 0;

	now = dev;
	iter = &dev->adj_list.lower;

	while (1) {
		next = NULL;
		while (1) {
			ldev = netdev_next_lower_dev_rcu(now, &iter);
			if (!ldev)
				break;

			next = ldev;
			niter = &ldev->adj_list.lower;
			dev_stack[cur] = now;
			iter_stack[cur++] = iter;
			if (max <= cur)
				max = cur;
			break;
		}

		if (!next) {
			if (!cur)
				return max;
			next = dev_stack[--cur];
			niter = iter_stack[cur];
		}

		now = next;
		iter = niter;
	}

	return max;
}