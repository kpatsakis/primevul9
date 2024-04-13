static int parseMD(struct cli_bc *bc, unsigned char *buffer)
{
    unsigned offset = 1, len = strlen((const char*)buffer);
    unsigned numMD, i, b;
    char ok = 1;
    if (buffer[0] != 'D')
	return CL_EMALFDB;
    numMD = readNumber(buffer, &offset, len, &ok);
    if (!ok) {
	cli_errmsg("Unable to parse number of MD nodes\n");
	return CL_EMALFDB;
    }
    b = bc->dbgnode_cnt;
    bc->dbgnode_cnt += numMD;
    bc->dbgnodes = cli_realloc(bc->dbgnodes, bc->dbgnode_cnt * sizeof(*bc->dbgnodes));
    if (!bc->dbgnodes)
	return CL_EMEM;
    for (i=0;i<numMD;i++) {
	unsigned j;
	struct cli_bc_dbgnode_element* elts;
	unsigned el = readNumber(buffer, &offset, len, &ok);
	if (!ok) {
	    cli_errmsg("Unable to parse number of elements\n");
	    return CL_EMALFDB;
	}
	bc->dbgnodes[b+i].numelements = el;
	bc->dbgnodes[b+i].elements = elts = cli_calloc(el, sizeof(*elts));
	if (!elts)
	    return CL_EMEM;
	for (j=0;j<el;j++) {
	    if (buffer[offset] == '|') {
		elts[j].string = readData(buffer, &offset, len, &ok, &elts[j].len);
		if (!ok)
		    return CL_EMALFDB;
	    } else {
		elts[j].len = readNumber(buffer, &offset, len, &ok);
		if (!ok)
		    return CL_EMALFDB;
		if (elts[j].len) {
		    elts[j].constant = readNumber(buffer, &offset, len, &ok);
		}
		else
		    elts[j].nodeid = readNumber(buffer, &offset, len, &ok);
		if (!ok)
		    return CL_EMALFDB;
	    }
	}
    }
    cli_dbgmsg("bytecode: Parsed %u nodes total\n", bc->dbgnode_cnt);
    return CL_SUCCESS;
}