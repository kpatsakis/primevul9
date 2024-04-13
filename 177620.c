isdn_getrev(const char *revision)
{
	char *rev;
	char *p;

	if ((p = strchr(revision, ':'))) {
		rev = p + 2;
		p = strchr(rev, '$');
		*--p = 0;
	} else
		rev = "???";
	return rev;
}