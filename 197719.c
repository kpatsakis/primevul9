static int cgw_notifier(struct notifier_block *nb,
			unsigned long msg, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct net *net = dev_net(dev);

	if (dev->type != ARPHRD_CAN)
		return NOTIFY_DONE;

	if (msg == NETDEV_UNREGISTER) {

		struct cgw_job *gwj = NULL;
		struct hlist_node *nx;

		ASSERT_RTNL();

		hlist_for_each_entry_safe(gwj, nx, &net->can.cgw_list, list) {

			if (gwj->src.dev == dev || gwj->dst.dev == dev) {
				hlist_del(&gwj->list);
				cgw_unregister_filter(net, gwj);
				kmem_cache_free(cgw_cache, gwj);
			}
		}
	}

	return NOTIFY_DONE;
}