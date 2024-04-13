int cli_bytecode_runlsig(cli_ctx *cctx, struct cli_target_info *tinfo,
			 const struct cli_all_bc *bcs, unsigned bc_idx,
			 const char **virname, const uint32_t* lsigcnt,
			 const uint32_t *lsigsuboff, fmap_t *map)
{
    int ret;
    struct cli_bc_ctx ctx;
    const struct cli_bc *bc = &bcs->all_bcs[bc_idx-1];
    struct cli_pe_hook_data pehookdata;

    memset(&ctx, 0, sizeof(ctx));
    cli_bytecode_context_setfuncid(&ctx, bc, 0);
    ctx.hooks.match_counts = lsigcnt;
    ctx.hooks.match_offsets = lsigsuboff;
    cli_bytecode_context_setctx(&ctx, cctx);
    cli_bytecode_context_setfile(&ctx, map);
    if (tinfo && tinfo->status == 1) {
	ctx.sections = tinfo->exeinfo.section;
	memset(&pehookdata, 0, sizeof(pehookdata));
	pehookdata.offset = tinfo->exeinfo.offset;
	pehookdata.ep = tinfo->exeinfo.ep;
	pehookdata.nsections = tinfo->exeinfo.nsections;
	pehookdata.hdr_size = tinfo->exeinfo.hdr_size;
	ctx.hooks.pedata = &pehookdata;
	ctx.resaddr = tinfo->exeinfo.res_addr;
    }
    if (bc->hook_lsig_id) {
	cli_dbgmsg("hook lsig id %d matched (bc %d)\n", bc->hook_lsig_id, bc->id);
	/* this is a bytecode for a hook, defer running it until hook is
	 * executed, so that it has all the info for the hook */
	if (cctx->hook_lsig_matches)
	    cli_bitset_set(cctx->hook_lsig_matches, bc->hook_lsig_id-1);
	/* save match counts */
	memcpy(&ctx.lsigcnt, lsigcnt, 64*4);
	memcpy(&ctx.lsigoff, lsigsuboff, 64*4);
	cli_bytecode_context_clear(&ctx);
	return CL_SUCCESS;
    }

    cli_dbgmsg("Running bytecode for logical signature match\n");
    ret = cli_bytecode_run(bcs, bc, &ctx);
    if (ret != CL_SUCCESS) {
	cli_warnmsg("Bytcode %u failed to run: %s\n", bc->id, cl_strerror(ret));
	cli_bytecode_context_clear(&ctx);
	return CL_SUCCESS;
    }
    if (ctx.virname) {
	int rc;
	cli_dbgmsg("Bytecode found virus: %s\n", ctx.virname);
	if (virname)
	    *virname = ctx.virname;
	if (!strncmp(*virname, "BC.Heuristics", 13))
	    rc = cli_found_possibly_unwanted(cctx);
	else
	    rc = CL_VIRUS;
	cli_bytecode_context_clear(&ctx);
	return rc;
    }
    ret = cli_bytecode_context_getresult_int(&ctx);
    cli_dbgmsg("Bytecode %u returned code: %u\n", bc->id, ret);
    cli_bytecode_context_clear(&ctx);
    return CL_SUCCESS;
}