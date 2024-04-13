get_external_key_maxtries(struct sc_card *card, unsigned char *maxtries)
{
	unsigned char maxcounter[2] = { 0 };
	static const sc_path_t file_path = {
		{0x3f, 0x00, 0x50, 0x15, 0x9f, 0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 6,
		0,
		0,
		SC_PATH_TYPE_PATH,
		{{0}, 0}
	};
	int ret;

	ret = sc_select_file(card, &file_path, NULL);
	LOG_TEST_RET(card->ctx, ret, "select max counter file failed");

	ret = sc_read_binary(card, 0, maxcounter, 2, 0);
	LOG_TEST_RET(card->ctx, ret, "read max counter file failed");

	*maxtries = maxcounter[0];
	return SC_SUCCESS;
}