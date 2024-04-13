static void yam_dotimer(struct timer_list *unused)
{
	int i;

	for (i = 0; i < NR_PORTS; i++) {
		struct net_device *dev = yam_devs[i];
		if (dev && netif_running(dev))
			yam_arbitrate(dev);
	}
	yam_timer.expires = jiffies + HZ / 100;
	add_timer(&yam_timer);
}