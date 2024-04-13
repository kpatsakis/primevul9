static __always_inline void __flow_hash_secret_init(void)
{
	net_get_random_once(&hashrnd, sizeof(hashrnd));
}