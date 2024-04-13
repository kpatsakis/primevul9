static int list_keys(void)
{
	int r, idx = 0;
	sc_path_t path;
	u8 buf[2048], *p = buf;
	size_t keysize, i;
	int mod_lens[] = { 512, 768, 1024, 2048 };
	size_t sizes[] = { 167, 247, 327, 647 };

	r = select_app_df();
	if (r)
		return 1;
	sc_format_path("I1012", &path);
	r = sc_select_file(card, &path, NULL);
	if (r) {
		fprintf(stderr, "Unable to select public key file: %s\n", sc_strerror(r));
		return 2;
	}
	do {
		int mod_len = -1;

		r = sc_read_binary(card, idx, buf, 3, 0);
		if (r < 0) {
			fprintf(stderr, "Unable to read public key file: %s\n", sc_strerror(r));
			return 2;
		}
		keysize = (p[0] << 8) | p[1];
		if (keysize == 0)
			break;
		idx += keysize;
		for (i = 0; i < sizeof(sizes)/sizeof(sizes[ 0]); i++)
			if (sizes[i] == keysize)
				mod_len = mod_lens[i];
		if (mod_len < 0)
			printf("Key %d -- unknown modulus length\n", p[2] & 0x0F);
		else
			printf("Key %d -- Modulus length %d\n", p[2] & 0x0F, mod_len);
	} while (1);
	return 0;
}