static void aarp_purge(void)
{
	int ct;

	write_lock_bh(&aarp_lock);
	for (ct = 0; ct < AARP_HASH_SIZE; ct++) {
		__aarp_expire_all(&resolved[ct]);
		__aarp_expire_all(&unresolved[ct]);
		__aarp_expire_all(&proxies[ct]);
	}
	write_unlock_bh(&aarp_lock);
}