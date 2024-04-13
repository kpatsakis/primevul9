static int parseApis(struct cli_bc *bc, unsigned char *buffer)
{
    unsigned i, offset = 1, len = strlen((const char*)buffer), maxapi, calls;
    char ok =1;
    uint16_t *apity2ty;/*map of api type to current bytecode type ID */

    if (buffer[0] != 'E') {
	cli_errmsg("bytecode: Invalid api header: %c\n", buffer[0]);
	return CL_EMALFDB;
    }

    maxapi = readNumber(buffer, &offset, len, &ok);
    if (!ok)
	return CL_EMALFDB;
    if (maxapi > cli_apicall_maxapi) {
	cli_dbgmsg("bytecode using API %u, but highest API known to libclamav is %u, skipping\n", maxapi, cli_apicall_maxapi);
	return CL_BREAK;
    }
    calls = readNumber(buffer, &offset, len, &ok);
    if (!ok)
	return CL_EMALFDB;
    if (calls > maxapi) {
	cli_errmsg("bytecode: attempting to describe more APIs than max: %u > %u\n", calls, maxapi);
	return CL_EMALFDB;
    }
    bc->uses_apis = cli_bitset_init();
    if (!bc->uses_apis) {
	cli_errmsg("Out of memory allocating apis bitset\n");
	return CL_EMEM;
    }
    apity2ty = cli_calloc(cli_apicall_maxtypes, sizeof(*cli_apicall_types));
    if (!apity2ty) {
	cli_errmsg("Out of memory allocating apity2ty\n");
	return CL_EMEM;
    }
    for (i=0;i < calls; i++) {
	unsigned id = readNumber(buffer, &offset, len, &ok);
	uint16_t tid = readTypeID(bc, buffer, &offset, len, &ok);
	char *name = readString(buffer, &offset, len, &ok);

	/* validate APIcall prototype */
	if (id > maxapi) {
	    cli_errmsg("bytecode: API id %u out of range, max %u\n", id, maxapi);
	    ok = 0;
	}
	/* API ids start from 1 */
	id--;
	if (ok && name && strcmp(cli_apicalls[id].name, name)) {
	    cli_errmsg("bytecode: API %u name mismatch: %s expected %s\n", id, name, cli_apicalls[id].name);
	    ok = 0;
	}
	if (ok && !types_equal(bc, apity2ty, tid, cli_apicalls[id].type)) {
	    cli_errmsg("bytecode: API %u prototype doesn't match\n", id);
	    ok = 0;
	}
	/* don't need the name anymore */
	free(name);
	if (!ok)
	    return CL_EMALFDB;

	/* APIcall is valid */
	cli_bitset_set(bc->uses_apis, id);
    }
    free(apity2ty); /* free temporary map */
    cli_dbgmsg("bytecode: Parsed %u APIcalls, maxapi %u\n", calls, maxapi);
    return CL_SUCCESS;
}