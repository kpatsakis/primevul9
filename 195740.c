int32_t cli_bcapi_inflate_init(struct cli_bc_ctx *ctx, int32_t from, int32_t to, int32_t windowBits)
{
    int ret;
    z_stream stream;
    struct bc_inflate *b;
    unsigned n = ctx->ninflates + 1;
    if (!get_buffer(ctx, from) || !get_buffer(ctx, to)) {
	cli_dbgmsg("bytecode api: inflate_init: invalid buffers!\n");
	return -1;
    }
    memset(&stream, 0, sizeof(stream));
    ret = inflateInit2(&stream, windowBits);
    switch (ret) {
	case Z_MEM_ERROR:
	    cli_dbgmsg("bytecode api: inflateInit2: out of memory!\n");
	    return -1;
	case Z_VERSION_ERROR:
	    cli_dbgmsg("bytecode api: inflateinit2: zlib version error!\n");
	    return -1;
	case Z_STREAM_ERROR:
	    cli_dbgmsg("bytecode api: inflateinit2: zlib stream error!\n");
	    return -1;
	case Z_OK:
	    break;
	default:
	    cli_dbgmsg("bytecode api: inflateInit2: unknown error %d\n", ret);
	    return -1;
    }

    b = cli_realloc(ctx->inflates, sizeof(*ctx->inflates)*n);
    if (!b) {
	inflateEnd(&stream);
	return -1;
    }
    ctx->inflates = b;
    ctx->ninflates = n;
    b = &b[n-1];

    b->from = from;
    b->to = to;
    b->needSync = 0;
    memcpy(&b->stream, &stream, sizeof(stream));
    return n-1;
}