static int read_rsa_privkey(RSA **rsa_out)
{
	RSA *rsa = NULL;
	BIO *in = NULL;
	int r;

	in = BIO_new(BIO_s_file());
	if (opt_prkeyf == NULL) {
		fprintf(stderr, "Private key file must be set.\n");
		return 2;
	}
	r = BIO_read_filename(in, opt_prkeyf);
	if (r <= 0) {
		perror(opt_prkeyf);
		return 2;
	}
	rsa = PEM_read_bio_RSAPrivateKey(in, NULL, NULL, NULL);
	if (rsa == NULL) {
		fprintf(stderr, "Unable to load private key.\n");
		return 2;
	}
	BIO_free(in);
	*rsa_out = rsa;
	return 0;
}