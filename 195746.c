static int parseGlobals(struct cli_bc *bc, unsigned char *buffer)
{
    unsigned i, offset = 1, len = strlen((const char*)buffer), numglobals;
    unsigned maxglobal;
    char ok=1;

    if (buffer[0] != 'G') {
	cli_errmsg("bytecode: Invalid globals header: %c\n", buffer[0]);
	return CL_EMALFDB;
    }
    maxglobal = readNumber(buffer, &offset, len, &ok);
    if (maxglobal > cli_apicall_maxglobal) {
	cli_dbgmsg("bytecode using global %u, but highest global known to libclamav is %u, skipping\n", maxglobal, cli_apicall_maxglobal);
	return CL_BREAK;
    }
    numglobals = readNumber(buffer, &offset, len, &ok);
    bc->globals = cli_calloc(numglobals, sizeof(*bc->globals));
    if (!bc->globals) {
	cli_errmsg("bytecode: OOM allocating memory for %u globals\n", numglobals);
	return CL_EMEM;
    }
    bc->globaltys = cli_calloc(numglobals, sizeof(*bc->globaltys));
    if (!bc->globaltys) {
	cli_errmsg("bytecode: OOM allocating memory for %u global types\n", numglobals);
	return CL_EMEM;
    }
    bc->num_globals = numglobals;
    if (!ok)
	return CL_EMALFDB;
    for (i=0;i<numglobals;i++) {
	unsigned comp;
	bc->globaltys[i] = readTypeID(bc, buffer, &offset, len, &ok);
	comp = type_components(bc, bc->globaltys[i], &ok);
	if (!ok)
	    return CL_EMALFDB;
	bc->globals[i] = cli_malloc(sizeof(*bc->globals[0])*comp);
	if (!bc->globals[i])
	    return CL_EMEM;
	readConstant(bc, i, comp, buffer, &offset, len, &ok);
    }
    if (!ok)
	return CL_EMALFDB;
    if (offset != len) {
	cli_errmsg("Trailing garbage in globals: %d extra bytes\n",
		   len-offset);
	return CL_EMALFDB;
    }
    return CL_SUCCESS;
}