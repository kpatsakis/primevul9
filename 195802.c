static inline char *readString(const unsigned char *p, unsigned *off, unsigned len, char *ok)
{
    unsigned stringlen;
    char *str = readData(p, off, len, ok, &stringlen);
    if (*ok && stringlen && str[stringlen-1] != '\0') {
	str[stringlen-1] = '\0';
	cli_errmsg("bytecode: string missing \\0 terminator: %s\n", str);
	free(str);
	*ok = 0;
	return NULL;
    }
    return str;
}