static fz_locks_context *init_muraster_locks(void)
{
	int i;
	int failed = 0;

	for (i = 0; i < FZ_LOCK_MAX; i++)
		failed |= mu_create_mutex(&mutexes[i]);

	if (failed)
	{
		fin_muraster_locks();
		return NULL;
	}

	return &muraster_locks;
}