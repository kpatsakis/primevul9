read_resolution(const char *arg)
{
	char *sep = strchr(arg, ',');

	if (sep == NULL)
		sep = strchr(arg, 'x');
	if (sep == NULL)
		sep = strchr(arg, ':');
	if (sep == NULL)
		sep = strchr(arg, ';');

	x_resolution = fz_atoi(arg);
	if (sep && sep[1])
		y_resolution = fz_atoi(arg);
	else
		y_resolution = x_resolution;
}