static void readConstant(struct cli_bc *bc, unsigned i, unsigned comp,
			 unsigned char *buffer, unsigned *offset,
			 unsigned len, char *ok)
{
    unsigned j=0;
    if (*ok && buffer[*offset] == 0x40 &&
	buffer [*offset+1] == 0x60) {
	/* zero initializer */
	memset(bc->globals[i], 0, sizeof(*bc->globals[0])*comp);
	(*offset)+=2;
	return;
    }
    while (*ok && buffer[*offset] != 0x60) {
	if (j >= comp) {
	    cli_errmsg("bytecode: constant has too many subcomponents, expected %u\n", comp);
	    *ok = 0;
	    return;
	}
	buffer[*offset] |= 0x20;
	bc->globals[i][j++] = readNumber(buffer, offset, len, ok);
    }
    if (*ok && j != comp) {
	cli_errmsg("bytecode: constant has too few subcomponents: %u < %u\n", j, comp);
	*ok = 0;
    }
    (*offset)++;
}