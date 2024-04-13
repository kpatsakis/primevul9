static inline funcid_t readFuncID(struct cli_bc *bc, unsigned char *p,
				  unsigned *off, unsigned len, char *ok)
{
    funcid_t id = readNumber(p, off, len, ok)-1;
    if (*ok && id >= bc->num_func) {
	cli_errmsg("Called function out of range: %u >= %u\n", id, bc->num_func);
	*ok = 0;
	return ~0;
    }
    return id;
}