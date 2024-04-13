static int parseHeader(struct cli_bc *bc, unsigned char *buffer, unsigned *linelength)
{
    uint64_t magic1;
    unsigned magic2;
    char ok = 1;
    unsigned offset, len, flevel;
    char *pos;

    if (strncmp((const char*)buffer, BC_HEADER, sizeof(BC_HEADER)-1)) {
	cli_errmsg("Missing file magic in bytecode");
	return CL_EMALFDB;
    }
    offset = sizeof(BC_HEADER)-1;
    len = strlen((const char*)buffer);
    bc->metadata.formatlevel = readNumber(buffer, &offset, len, &ok);
    if (!ok) {
	cli_errmsg("Unable to parse (format) functionality level in bytecode header\n");
	return CL_EMALFDB;
    }
    /* we support 2 bytecode formats */
    if (bc->metadata.formatlevel != BC_FORMAT_096 &&
	bc->metadata.formatlevel != BC_FORMAT_LEVEL) {
	cli_dbgmsg("Skipping bytecode with (format) functionality level: %u (current %u)\n", 
		   bc->metadata.formatlevel, BC_FORMAT_LEVEL);
	return CL_BREAK;
    }
    /* Optimistic parsing, check for error only at the end.*/
    bc->metadata.timestamp = readNumber(buffer, &offset, len, &ok);
    bc->metadata.sigmaker = readString(buffer, &offset, len, &ok);
    bc->metadata.targetExclude = readNumber(buffer, &offset, len, &ok);
    bc->kind = readNumber(buffer, &offset, len, &ok);
    bc->metadata.minfunc = readNumber(buffer, &offset, len, &ok);
    bc->metadata.maxfunc = readNumber(buffer, &offset, len, &ok);
    flevel = cl_retflevel();
    /* in 0.96 these 2 fields are unused / zero, in post 0.96 these mean
     * min/max flevel.
     * So 0 for min/max means no min/max
     * Note that post 0.96 bytecode/bytecode lsig needs format 7, because
     * 0.96 doesn't check lsig functionality level.
     */
    if ((bc->metadata.minfunc && bc->metadata.minfunc > flevel) ||
        (bc->metadata.maxfunc && bc->metadata.maxfunc < flevel)) {
      cli_dbgmsg("Skipping bytecode with (engine) functionality level %u-%u (current %u)\n",
                 bc->metadata.minfunc, bc->metadata.maxfunc, flevel);
      return CL_BREAK;
    }
    bc->metadata.maxresource = readNumber(buffer, &offset, len, &ok);
    bc->metadata.compiler = readString(buffer, &offset, len, &ok);
    bc->num_types = readNumber(buffer, &offset, len, &ok);
    bc->num_func = readNumber(buffer, &offset, len, &ok);
    bc->state = bc_loaded;
    bc->uses_apis = NULL;
    bc->dbgnodes = NULL;
    bc->dbgnode_cnt = 0;
    if (!ok) {
	cli_errmsg("Invalid bytecode header at %u\n", offset);
	return CL_EMALFDB;
    }
    magic1 = readNumber(buffer, &offset, len, &ok);
    magic2 = readFixedNumber(buffer, &offset, len, &ok, 2);
    if (!ok || magic1 != 0x53e5493e9f3d1c30ull || magic2 != 42) {
      unsigned long m0 = magic1 >> 32;
      unsigned long m1 = magic1;
      cli_errmsg("Magic numbers don't match: %lx%lx, %u\n", m0, m1, magic2);
      return CL_EMALFDB;
    }
    if (buffer[offset] != ':') {
	cli_errmsg("Expected : but found: %c\n", buffer[offset]);
	return CL_EMALFDB;
    }
    offset++;
    *linelength = strtol((const char*)buffer+offset, &pos, 10);
    if (*pos != '\0') {
	cli_errmsg("Invalid number: %s\n", buffer+offset);
	return CL_EMALFDB;
    }

    bc->funcs = cli_calloc(bc->num_func, sizeof(*bc->funcs));
    if (!bc->funcs) {
	cli_errmsg("Out of memory allocating %u functions\n", bc->num_func);
	return CL_EMEM;
    }
    bc->types = cli_calloc(bc->num_types, sizeof(*bc->types));
    if (!bc->types) {
	cli_errmsg("Out of memory allocating %u types\n", bc->num_types);
	return CL_EMEM;
    }
    return CL_SUCCESS;
}