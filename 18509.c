get_password_passthrough(PK11SlotInfo *slot UNUSED,
			 PRBool retry, void *arg)
{
	if (retry || !arg)
		return NULL;

	char *ret = strdup(arg);
	if (!ret)
		err(1, "Could not allocate memory");

	return ret;
}