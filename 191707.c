static int network_flush (__attribute__((unused)) cdtime_t timeout,
		__attribute__((unused)) const char *identifier,
		__attribute__((unused)) user_data_t *user_data)
{
	pthread_mutex_lock (&send_buffer_lock);

	if (send_buffer_fill > 0)
	  flush_buffer ();

	pthread_mutex_unlock (&send_buffer_lock);

	return (0);
} /* int network_flush */