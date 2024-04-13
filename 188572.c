lha_parse_linkname(struct archive_string *linkname,
    struct archive_string *pathname)
{
	char *	linkptr;
	size_t 	symlen;

	linkptr = strchr(pathname->s, '|');
	if (linkptr != NULL) {
		symlen = strlen(linkptr + 1);
		archive_strncpy(linkname, linkptr+1, symlen);

		*linkptr = 0;
		pathname->length = strlen(pathname->s);

		return (1);
	}
	return (0);
}