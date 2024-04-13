netlink_lock_table(void)
{
	/* read_lock() synchronizes us to netlink_table_grab */

	read_lock(&nl_table_lock);
	atomic_inc(&nl_table_users);
	read_unlock(&nl_table_lock);
}