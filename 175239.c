static inline u_int8_t instance_hashfn(u_int16_t queue_num)
{
	return ((queue_num >> 8) ^ queue_num) % INSTANCE_BUCKETS;
}