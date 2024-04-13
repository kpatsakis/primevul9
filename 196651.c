is_valid_culture (const char *cname)
{
	int i;
	int found;

	found = *cname == 0;
	for (i = 0; i < G_N_ELEMENTS (valid_cultures); ++i) {
		if (g_ascii_strcasecmp (valid_cultures [i], cname)) {
			found = 1;
			break;
		}
	}
	return found;
}