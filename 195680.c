uint32_t cli_bcapi_debug_print_uint(struct cli_bc_ctx *ctx, uint32_t a)
{
    cli_event_int(EV, BCEV_DBG_INT, a);
    if (!cli_debug_flag)
	return 0;
    return fprintf(stderr, "%d", a);
}