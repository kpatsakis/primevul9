const sc_path_t *sc_get_mf_path(void)
{
	static const sc_path_t mf_path = {
		{0x3f, 0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 2,
		0,
		0,
		SC_PATH_TYPE_PATH,
		{{0},0}
	};
	return &mf_path;
}