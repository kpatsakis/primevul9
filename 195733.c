int32_t cli_bcapi_hex2ui(struct cli_bc_ctx *ctx, uint32_t ah, uint32_t bh)
{
    char result = 0;
    unsigned char in[2];
    in[0] = ah;
    in[1] = bh;

    if (cli_hex2str_to((const char*)in, &result, 2) == -1)
	return -1;
    return result;
}