netlink_unlock_table(void)
{
	if (atomic_dec_and_test(&nl_table_users))
		wake_up(&nl_table_wait);
}