	__releases(nl_table_lock)
{
	write_unlock_irq(&nl_table_lock);
	wake_up(&nl_table_wait);
}