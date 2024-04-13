static struct aarp_entry *__aarp_find_entry(struct aarp_entry *list,
					    struct net_device *dev,
					    struct atalk_addr *sat)
{
	while (list) {
		if (list->target_addr.s_net == sat->s_net &&
		    list->target_addr.s_node == sat->s_node &&
		    list->dev == dev)
			break;
		list = list->next;
	}

	return list;
}