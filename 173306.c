static int bnx2x_init_mcast_macs_list(struct bnx2x *bp,
				      struct bnx2x_mcast_ramrod_params *p,
				      struct list_head *mcast_group_list)
{
	struct bnx2x_mcast_list_elem *mc_mac;
	struct netdev_hw_addr *ha;
	struct bnx2x_mcast_list_elem_group *current_mcast_group = NULL;
	int mc_count = netdev_mc_count(bp->dev);
	int offset = 0;

	INIT_LIST_HEAD(&p->mcast_list);
	netdev_for_each_mc_addr(ha, bp->dev) {
		if (!offset) {
			current_mcast_group =
				(struct bnx2x_mcast_list_elem_group *)
				__get_free_page(GFP_ATOMIC);
			if (!current_mcast_group) {
				bnx2x_free_mcast_macs_list(mcast_group_list);
				BNX2X_ERR("Failed to allocate mc MAC list\n");
				return -ENOMEM;
			}
			list_add(&current_mcast_group->mcast_group_link,
				 mcast_group_list);
		}
		mc_mac = &current_mcast_group->mcast_elems[offset];
		mc_mac->mac = bnx2x_mc_addr(ha);
		list_add_tail(&mc_mac->link, &p->mcast_list);
		offset++;
		if (offset == MCAST_ELEMS_PER_PG)
			offset = 0;
	}
	p->mcast_list_len = mc_count;
	return 0;
}