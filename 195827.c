static uint16_t readTypeID(struct cli_bc *bc, unsigned char *buffer,
			   unsigned *offset, unsigned len, char *ok)
{
    uint64_t t = readNumber(buffer, offset, len, ok);
    if (!ok)
	return ~0;
    if (t >= bc->num_types + bc->start_tid) {
	cli_errmsg("Invalid type id: %llu\n", (unsigned long long)t);
	*ok = 0;
	return ~0;
    }
    return t;
}