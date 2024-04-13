uint32_t cli_bcapi_debug_print_str(struct cli_bc_ctx *ctx, const uint8_t *str, uint32_t len)
{
    cli_event_fastdata(EV, BCEV_DBG_STR, str, strlen((const char*)str));
    cli_dbgmsg("bytecode debug: %s\n", str);
    return 0;
}