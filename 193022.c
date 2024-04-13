static int hash(struct signal_struct *sig, unsigned int nr)
{
	return hash_32(hash32_ptr(sig) ^ nr, HASH_BITS(posix_timers_hashtable));
}