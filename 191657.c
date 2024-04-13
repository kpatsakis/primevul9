static void *receive_thread (void __attribute__((unused)) *arg)
{
	return (network_receive () ? (void *) 1 : (void *) 0);
} /* void *receive_thread */