static bool validate_masked(u8 *data, int len)
{
	u8 *mask = data + len;

	while (len--)
		if (*data++ & ~*mask++)
			return false;

	return true;
}