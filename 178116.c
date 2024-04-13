static json_bool numbered(struct crypt_device *cd, const char *name, const char *key)
{
	int i;

	for (i = 0; key[i]; i++)
		if (!isdigit(key[i])) {
			log_dbg(cd, "%s \"%s\" is not in numbered form.", name, key);
			return 0;
		}
	return 1;
}