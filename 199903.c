static unsigned int bad_file_poll(struct file *filp, poll_table *wait)
{
	return POLLERR;
}