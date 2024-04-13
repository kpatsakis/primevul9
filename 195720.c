static void parseType(struct cli_bc *bc, struct cli_bc_type *ty,
		      unsigned char *buffer, unsigned *off, unsigned len,
		      char *ok)
{
    unsigned j;

    ty->numElements = readNumber(buffer, off, len, ok);
    if (!*ok) {
	cli_errmsg("Error parsing type\n");
	*ok = 0;
	return;
    }
    ty->containedTypes = cli_malloc(sizeof(*ty->containedTypes)*ty->numElements);
    if (!ty->containedTypes) {
	cli_errmsg("Out of memory allocating %u types\n", ty->numElements);
	*ok = 0;
	return;
    }
    for (j=0;j<ty->numElements;j++) {
	ty->containedTypes[j] = readTypeID(bc, buffer, off, len, ok);
    }
}