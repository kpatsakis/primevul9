_isalnum(int c)
{
	return isalnum(c) && c < 0x7f;
}