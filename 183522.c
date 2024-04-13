static char *iniparser_getstring_nonempty(dictionary *d, char *key, char *def)
{
	char *ret = iniparser_getstring(d, key, def);
	if (ret && strlen(ret) == 0) {
		ret = NULL;
	}
	return ret;
}