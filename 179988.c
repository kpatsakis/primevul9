concat_class_name (char *buf, int bufsize, MonoClass *klass)
{
	int nspacelen, cnamelen;
	nspacelen = strlen (klass->name_space);
	cnamelen = strlen (klass->name);
	if (nspacelen + cnamelen + 2 > bufsize)
		return 0;
	if (nspacelen) {
		memcpy (buf, klass->name_space, nspacelen);
		buf [nspacelen ++] = '.';
	}
	memcpy (buf + nspacelen, klass->name, cnamelen);
	buf [nspacelen + cnamelen] = 0;
	return nspacelen + cnamelen;
}