llsec_key_get(struct mac802154_llsec_key *key)
{
	kref_get(&key->ref);
	return key;
}