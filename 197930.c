static void muraster_lock(void *user, int lock)
{
	mu_lock_mutex(&mutexes[lock]);
}