static inline funcid_t readAPIFuncID(struct cli_bc *bc, unsigned char *p,
				     unsigned *off, unsigned len, char *ok)
{
    funcid_t id = readNumber(p, off, len, ok)-1;
    if (*ok && !cli_bitset_test(bc->uses_apis, id)) {
	cli_errmsg("Called undeclared API function: %u\n", id);
	*ok = 0;
	return ~0;
    }
    return id;
}