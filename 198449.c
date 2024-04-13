set_string (char **strp, const char *value)
{
	if (*strp)
		free (*strp);
	*strp = value ? strdup (value) : NULL;
}