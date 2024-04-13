static bbid_t readBBID(struct cli_bc_func *func, const unsigned char *buffer, unsigned *off, unsigned len, char *ok) {
    unsigned id = readNumber(buffer, off, len, ok);
    if (!id || id >= func->numBB) {
	cli_errmsg("Basic block ID out of range: %u\n", id);
	*ok = 0;
    }
    if (!*ok)
	return ~0;
    return id;
}