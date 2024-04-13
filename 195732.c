int cli_bytecode_load(struct cli_bc *bc, FILE *f, struct cli_dbio *dbio, int trust)
{
    unsigned row = 0, current_func = 0, bb=0;
    char *buffer;
    unsigned linelength=0;
    char firstbuf[FILEBUFF];
    enum parse_state state;
    int rc, end=0;

    memset(bc, 0, sizeof(*bc));
    cli_dbgmsg("Loading %s bytecode\n", trust ? "trusted" : "untrusted");
    bc->trusted = trust;
    if (!f && !dbio) {
	cli_errmsg("Unable to load bytecode (null file)\n");
	return CL_ENULLARG;
    }
    if (!cli_dbgets(firstbuf, FILEBUFF, f, dbio)) {
	cli_errmsg("Unable to load bytecode (empty file)\n");
	return CL_EMALFDB;
    }
    cli_chomp(firstbuf);
    rc = parseHeader(bc, (unsigned char*)firstbuf, &linelength);
    state = PARSE_BC_LSIG;
    if (rc == CL_BREAK) {
	const char *len = strchr(firstbuf, ':');
	bc->state = bc_skip;
	if (!linelength) {
	    linelength = len ? atoi(len+1) : 4096;
	}
	if (linelength < 4096)
	    linelength = 4096;
	cli_dbgmsg("line: %d\n", linelength);
	state = PARSE_SKIP;
	rc = CL_SUCCESS;
    }
    if (rc != CL_SUCCESS) {
	cli_errmsg("Error at bytecode line %u\n", row);
	return rc;
    }
    buffer = cli_malloc(linelength);
    if (!buffer) {
	cli_errmsg("Out of memory allocating line of length %u\n", linelength);
	return CL_EMEM;
    }
    while (cli_dbgets(buffer, linelength, f, dbio) && !end) {
	cli_chomp(buffer);
	row++;
	switch (state) {
	    case PARSE_BC_LSIG:
		rc = parseLSig(bc, buffer);
		if (rc == CL_BREAK) /* skip */ {
		    bc->state = bc_skip;
		    state = PARSE_SKIP;
		    continue;
		}
		if (rc != CL_SUCCESS) {
		    cli_errmsg("Error at bytecode line %u\n", row);
		    free(buffer);
		    return rc;
		}
		state = PARSE_BC_TYPES;
		break;
	    case PARSE_BC_TYPES:
		rc = parseTypes(bc, (unsigned char*)buffer);
		if (rc != CL_SUCCESS) {
		    cli_errmsg("Error at bytecode line %u\n", row);
		    free(buffer);
		    return rc;
		}
		state = PARSE_BC_APIS;
		break;
	    case PARSE_BC_APIS:
		rc = parseApis(bc, (unsigned char*)buffer);
		if (rc == CL_BREAK) /* skip */ {
		    bc->state = bc_skip;
		    state = PARSE_SKIP;
		    continue;
		}
		if (rc != CL_SUCCESS) {
		    cli_errmsg("Error at bytecode line %u\n", row);
		    free(buffer);
		    return rc;
		}
		state = PARSE_BC_GLOBALS;
		break;
	    case PARSE_BC_GLOBALS:
		rc = parseGlobals(bc, (unsigned char*)buffer);
		if (rc == CL_BREAK) /* skip */ {
		    bc->state = bc_skip;
		    state = PARSE_SKIP;
		    continue;
		}
		if (rc != CL_SUCCESS) {
		    cli_errmsg("Error at bytecode line %u\n", row);
		    free(buffer);
		    return rc;
		}
		state = PARSE_MD_OPT_HEADER;
		break;
	    case PARSE_MD_OPT_HEADER:
		if (buffer[0] == 'D') {
		    rc = parseMD(bc, (unsigned char*)buffer);
		    if (rc != CL_SUCCESS) {
			cli_errmsg("Error at bytecode line %u\n", row);
			free(buffer);
			return rc;
		    }
		    break;
		}
		/* fall-through */
	    case PARSE_FUNC_HEADER:
                if (*buffer == 'S') {
		    end = 1;
		    break;
		}
		rc = parseFunctionHeader(bc, current_func, (unsigned char*)buffer);
		if (rc != CL_SUCCESS) {
		    cli_errmsg("Error at bytecode line %u\n", row);
		    free(buffer);
		    return rc;
		}
		bb = 0;
		state = PARSE_BB;
		break;
	    case PARSE_BB:
		rc = parseBB(bc, current_func, bb++, (unsigned char*)buffer);
		if (rc != CL_SUCCESS) {
		    cli_errmsg("Error at bytecode line %u\n", row);
		    free(buffer);
		    return rc;
		}
		if (bb >= bc->funcs[current_func].numBB) {
		    if (bc->funcs[current_func].insn_idx != bc->funcs[current_func].numInsts) {
			cli_errmsg("Parsed different number of instructions than declared: %u != %u\n",
				   bc->funcs[current_func].insn_idx, bc->funcs[current_func].numInsts);
			free(buffer);
			return CL_EMALFDB;
		    }
		    cli_dbgmsg("Parsed %u BBs, %u instructions\n",
			       bb, bc->funcs[current_func].numInsts);
		    state = PARSE_FUNC_HEADER;
		    current_func++;
		}
		break;
	    case PARSE_SKIP:
		/* stop at S (source code), readdb.c knows how to skip this one
		 * */
		if (buffer[0] == 'S')
		    end = 1;
		/* noop parse, but we need to use dbgets with dynamic buffer,
		 * otherwise we get 'Line too long for provided buffer' */
		break;
	}
    }
    free(buffer);
    cli_dbgmsg("Parsed %d functions\n", current_func);
    if (current_func != bc->num_func && bc->state != bc_skip) {
	cli_errmsg("Loaded less functions than declared: %u vs. %u\n",
		   current_func, bc->num_func);
	return CL_EMALFDB;
    }
    return CL_SUCCESS;
}