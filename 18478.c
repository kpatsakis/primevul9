random_poll(struct file *file, poll_table * wait)
{
	__poll_t mask;

	poll_wait(file, &crng_init_wait, wait);
	poll_wait(file, &random_write_wait, wait);
	mask = 0;
	if (crng_ready())
		mask |= EPOLLIN | EPOLLRDNORM;
	if (ENTROPY_BITS(&input_pool) < random_write_wakeup_bits)
		mask |= EPOLLOUT | EPOLLWRNORM;
	return mask;
}