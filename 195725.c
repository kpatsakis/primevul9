uint32_t cli_bcapi_debug_print_str_start(struct cli_bc_ctx *ctx , const uint8_t* s, uint32_t len)
{
    if (!s || len <= 0)
	return -1;
    cli_event_fastdata(EV, BCEV_DBG_STR, s, len);
    cli_dbgmsg("bytecode debug: %.*s", len, s);
    return 0;
}