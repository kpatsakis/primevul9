static void bnx2x_free_mcast_macs_list(struct list_head *mcast_group_list)
{
	struct bnx2x_mcast_list_elem_group *current_mcast_group;

	while (!list_empty(mcast_group_list)) {
		current_mcast_group = list_first_entry(mcast_group_list,
				      struct bnx2x_mcast_list_elem_group,
				      mcast_group_link);
		list_del(&current_mcast_group->mcast_group_link);
		free_page((unsigned long)current_mcast_group);
	}
}