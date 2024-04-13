static bool name_is_nis (const char *name)
{
	return (('+' == name[0]) || ('-' == name[0]));
}