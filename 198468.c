static int read_key(void)
{
	RSA *rsa = RSA_new();
	u8 buf[1024], *p = buf;
	u8 b64buf[2048];
	int r;

	if (rsa == NULL)
		return -1;
	r = read_public_key(rsa);
	if (r)
		return r;
	r = i2d_RSA_PUBKEY(rsa, &p);
	if (r <= 0) {
		fprintf(stderr, "Error encoding public key.\n");
		return -1;
	}
	r = sc_base64_encode(buf, r, b64buf, sizeof(b64buf), 64);
	if (r < 0) {
		fprintf(stderr, "Error in Base64 encoding: %s\n", sc_strerror(r));
		return -1;
	}
	printf("-----BEGIN PUBLIC KEY-----\n%s-----END PUBLIC KEY-----\n", b64buf);

	r = read_private_key(rsa);
	if (r == 10)
		return 0;
	else if (r)
		return r;
	p = buf;
	r = i2d_RSAPrivateKey(rsa, &p);
	if (r <= 0) {
		fprintf(stderr, "Error encoding private key.\n");
		return -1;
	}
	r = sc_base64_encode(buf, r, b64buf, sizeof(b64buf), 64);
	if (r < 0) {
		fprintf(stderr, "Error in Base64 encoding: %s\n", sc_strerror(r));
		return -1;
	}
	printf("-----BEGIN RSA PRIVATE KEY-----\n%s-----END RSA PRIVATE KEY-----\n", b64buf);

	return 0;
}