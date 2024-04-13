llsec_tfm_by_len(struct mac802154_llsec_key *key, int authlen)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(key->tfm); i++)
		if (crypto_aead_authsize(key->tfm[i]) == authlen)
			return key->tfm[i];

	BUG();
}