static int store_key(void)
{
	u8 prv[1024], pub[1024];
	size_t prvsize, pubsize;
	int r;
	RSA *rsa;

	r = read_rsa_privkey(&rsa);
	if (r)
		return r;
	r = encode_private_key(rsa, prv, &prvsize);
	if (r)
		return r;
	r = encode_public_key(rsa, pub, &pubsize);
	if (r)
		return r;
	if (verbose)
		printf("Storing private key...\n");
	r = select_app_df();
	if (r)
		return r;
	r = update_private_key(prv, prvsize);
	if (r)
		return r;
	if (verbose)
		printf("Storing public key...\n");
	r = select_app_df();
	if (r)
		return r;
	r = update_public_key(pub, pubsize);
	if (r)
		return r;
	return 0;
}