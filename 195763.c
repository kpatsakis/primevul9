uint32_t cli_bcapi_debug_print_str_nonl(struct cli_bc_ctx *ctx , const uint8_t* s, uint32_t len)
{
    if (!s || len <= 0)
	return -1;
    if (!cli_debug_flag)
	return 0;
    return fwrite(s, 1, len, stderr);
}