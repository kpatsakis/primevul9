static int wait_for_bgprint_to_finish(void)
{
	if (!bgprint.active || !bgprint.started)
		return 0;

	mu_wait_semaphore(&bgprint.stop);
	bgprint.started = 0;
	return bgprint.status;
}