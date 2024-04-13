static void llsec_key_put(struct mac802154_llsec_key *key)
{
	kref_put(&key->ref, llsec_key_release);
}