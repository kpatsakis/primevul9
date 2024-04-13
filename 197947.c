static void fin_muraster_locks(void)
{
	int i;

	for (i = 0; i < FZ_LOCK_MAX; i++)
		mu_destroy_mutex(&mutexes[i]);
}