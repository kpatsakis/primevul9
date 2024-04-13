void cli_bytecode_describe(const struct cli_bc *bc)
{
    char buf[128];
    int cols;
    unsigned i;
    time_t stamp;
    int had;

    if (!bc) {
	printf("(null bytecode)\n");
	return;
    }

    stamp = bc->metadata.timestamp;
    printf("Bytecode format functionality level: %u\n", bc->metadata.formatlevel);
    printf("Bytecode metadata:\n\tcompiler version: %s\n",
	   bc->metadata.compiler ? bc->metadata.compiler : "N/A");
    printf("\tcompiled on: (%d) %s",
	   (uint32_t)stamp,
	   cli_ctime(&stamp, buf, sizeof(buf)));
    printf("\tcompiled by: %s\n", bc->metadata.sigmaker ? bc->metadata.sigmaker : "N/A");
    /*TODO: parse and display arch name, also take it into account when
      JITing*/
    printf("\ttarget exclude: %d\n", bc->metadata.targetExclude);
    printf("\tbytecode type: ");
    switch (bc->kind) {
	case BC_GENERIC:
	    puts("generic, not loadable by clamscan/clamd");
	    break;
	case BC_STARTUP:
	    puts("run on startup (unique)");
	    break;
	case BC_LOGICAL:
	    puts("logical only");
	    break;
	case BC_PE_UNPACKER:
	    puts("PE hook");
	    break;
	default:
	    printf("Unknown (type %u)", bc->kind);
	    break;
    }
    /* 0 means no limit */
    printf("\tbytecode functionality level: %u - %u\n",
	   bc->metadata.minfunc, bc->metadata.maxfunc);
    printf("\tbytecode logical signature: %s\n",
	       bc->lsig ? bc->lsig : "<none>");
    printf("\tvirusname prefix: %s\n",
	   bc->vnameprefix);
    printf("\tvirusnames: %u\n", bc->vnames_cnt);
    printf("\tbytecode triggered on: ");
    switch (bc->kind) {
	case BC_GENERIC:
	    puts("N/A (loaded in clambc only)");
	    break;
	case BC_LOGICAL:
	    puts("files matching logical signature");
	    break;
	case BC_PE_UNPACKER:
	    if (bc->lsig)
		puts("PE files matching logical signature (unpacked)");
	    else
		puts("all PE files! (unpacked)");
	    break;
	case BC_PDF:
	    puts("PDF files");
	    break;
	case BC_PE_ALL:
	    if (bc->lsig)
		puts("PE files matching logical signature");
	    else
		puts("all PE files!");
	    break;
	default:
	    puts("N/A (unknown type)\n");
	    break;
    }
    printf("\tnumber of functions: %u\n\tnumber of types: %u\n",
	   bc->num_func, bc->num_types);
    printf("\tnumber of global constants: %u\n", (unsigned)bc->num_globals);
    printf("\tnumber of debug nodes: %u\n", bc->dbgnode_cnt);
    printf("\tbytecode APIs used:");
    cols = 0; /* remaining */
    had = 0;
    for (i=0;i<cli_apicall_maxapi;i++) {
	if (cli_bitset_test(bc->uses_apis, i)) {
	    unsigned len = strlen(cli_apicalls[i].name);
	    if (had)
		printf(",");
	    if (len > cols) {
		printf("\n\t");
		cols = 72;
	    }
	    printf(" %s", cli_apicalls[i].name);
	    had = 1;
	    cols -= len;
	}
    }
    printf("\n");
}