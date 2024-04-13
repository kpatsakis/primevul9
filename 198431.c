const char *sc_print_path(const sc_path_t *path)
{
	static char buffer[SC_MAX_PATH_STRING_SIZE + SC_MAX_AID_STRING_SIZE];

	if (sc_path_print(buffer, sizeof(buffer), path) != SC_SUCCESS)
		buffer[0] = '\0';

	return buffer;
}