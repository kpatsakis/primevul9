static void wrap_nettle_hash_deinit(void *hd)
{
	gnutls_free(hd);
}