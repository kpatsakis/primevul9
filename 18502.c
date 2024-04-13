get_env(const char *name)
{
	char *value;

	value = secure_getenv(name);
	if (value)
		value = strdup(value);
	return value;
}