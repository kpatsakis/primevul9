static void llsec_key_release(struct kref *ref)
{
	struct mac802154_llsec_key *key;
	int i;

	key = container_of(ref, struct mac802154_llsec_key, ref);

	for (i = 0; i < ARRAY_SIZE(key->tfm); i++)
		crypto_free_aead(key->tfm[i]);

	crypto_free_sync_skcipher(key->tfm0);
	kfree_sensitive(key);
}