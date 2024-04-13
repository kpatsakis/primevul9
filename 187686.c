static void __exit yam_cleanup_driver(void)
{
	struct yam_mcs *p;
	int i;

	del_timer_sync(&yam_timer);
	for (i = 0; i < NR_PORTS; i++) {
		struct net_device *dev = yam_devs[i];
		if (dev) {
			unregister_netdev(dev);
			free_netdev(dev);
		}
	}

	while (yam_data) {
		p = yam_data;
		yam_data = yam_data->next;
		kfree(p);
	}

	remove_proc_entry("yam", init_net.proc_net);
}