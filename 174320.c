convert_to_unix_filename(char *filename)
{
	char *p;

	while ((p = strchr(filename, '\\')))
	{
		*p = '/';
	}
}