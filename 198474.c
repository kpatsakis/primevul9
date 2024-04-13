static int update_private_key(const u8 *key, size_t keysize)
{
	int r, idx = 0;
	sc_path_t path;

	r = select_app_df();
	if (r)
		return 1;
	sc_format_path("I0012", &path);
	r = sc_select_file(card, &path, NULL);
	if (r) {
		fprintf(stderr, "Unable to select private key file: %s\n", sc_strerror(r));
		return 2;
	}
	idx = keysize * (opt_key_num-1);
	r = sc_update_binary(card, idx, key, keysize, 0);
	if (r < 0) {
		fprintf(stderr, "Unable to write private key: %s\n", sc_strerror(r));
		return 2;
	}
	return 0;
}