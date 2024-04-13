static bool is_reencryption_helper(const char *name)
{
	size_t len;

	if (!name)
		return false;

	len = strlen(name);
	return (len >= 9 && (!strncmp(name + len - 8, "-hotzone-", 9) ||
			     !strcmp(name + len - 8, "-overlay")));

}