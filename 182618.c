parse_guid (gchar *string, unsigned short guid[16])
{
	// Maps bytes to positions in guidString
	static const int indexes[] = {6, 4, 2, 0, 11, 9, 16, 14, 19, 21, 24, 26, 28, 30, 32, 34};
	int i;

	if (strlen (string) <= 35) {
		return FALSE;
	}

	for (i = 0; i < 16; i++) {
		guint hex1;
		guint hex2;

		if (!hex_int (string + indexes[i], 1, &hex1) ||
		    !hex_int (string + indexes[i] + 1, 1, &hex2))
			return FALSE;

		guid[i] = hex1 * 16 + hex2;
	}

	return TRUE;
}