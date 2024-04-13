static bool need_encode(unsigned char cval)
{
	if (cval < 0x20 || cval > 0x7E || strchr(" *()\\&|!\"", cval)) {
		return true;
	}
	return false;
}