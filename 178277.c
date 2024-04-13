static guint16 get_id(void)
{
	uint64_t rand;

	__connman_util_get_random(&rand);

	return rand;
}