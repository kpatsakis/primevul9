generate_keys(cms_context *cms, PK11SlotInfo *slot,
		SECKEYPrivateKey **privkey, SECKEYPublicKey **pubkey)
{
	PK11RSAGenParams rsaparams = {
		.keySizeInBits = 2048,
		.pe = 0x010001,
	};

	SECStatus rv;
	rv = PK11_Authenticate(slot, PR_TRUE, cms);
	if (rv != SECSuccess)
		cmsreterr(-1, cms, "could not authenticate with pk11 service");

	void *params = &rsaparams;
	*privkey = PK11_GenerateKeyPair(slot, CKM_RSA_PKCS_KEY_PAIR_GEN,
					params, pubkey, PR_TRUE, PR_TRUE,
					cms);
	if (!*privkey)
		cmsreterr(-1, cms, "could not generate RSA keypair");
	return 0;
}