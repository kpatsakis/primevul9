void get_random_bytes(void *buf, int nbytes)
{
	static void *previous;

	warn_unseeded_randomness(&previous);
	_get_random_bytes(buf, nbytes);
}