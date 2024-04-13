int32_t cli_bcapi_inflate_done(struct cli_bc_ctx *ctx , int32_t id)
{
    int ret;
    struct bc_inflate *b = get_inflate(ctx, id);
    if (!b || b->from == -1 || b->to == -1)
	return -1;
    ret = inflateEnd(&b->stream);
    if (ret == Z_STREAM_ERROR)
	cli_dbgmsg("bytecode api: inflateEnd: %s\n", b->stream.msg);
    b->from = b->to = -1;
    return ret;
}