int32_t cli_bcapi_inflate_process(struct cli_bc_ctx *ctx , int32_t id)
{
    int ret;
    unsigned avail_in_orig, avail_out_orig;
    struct bc_inflate *b = get_inflate(ctx, id);
    if (!b || b->from == -1 || b->to == -1)
	return -1;

    b->stream.avail_in = avail_in_orig =
	cli_bcapi_buffer_pipe_read_avail(ctx, b->from);

    b->stream.next_in = cli_bcapi_buffer_pipe_read_get(ctx, b->from,
						       b->stream.avail_in);

    b->stream.avail_out = avail_out_orig =
	cli_bcapi_buffer_pipe_write_avail(ctx, b->to);

    b->stream.next_out = cli_bcapi_buffer_pipe_write_get(ctx, b->to,
							 b->stream.avail_out);

    if (!b->stream.avail_in || !b->stream.avail_out || !b->stream.next_in || !b->stream.next_out)
	return -1;
    /* try hard to extract data, skipping over corrupted data */
    do {
	if (!b->needSync) {
	    ret = inflate(&b->stream, Z_NO_FLUSH);
	    if (ret == Z_DATA_ERROR) {
		cli_dbgmsg("bytecode api: inflate at %lu: %s, trying to recover\n", b->stream.total_in,
			   b->stream.msg);
		b->needSync = 1;
	    }
	}
	if (b->needSync) {
	    ret = inflateSync(&b->stream);
	    if (ret == Z_OK) {
		cli_dbgmsg("bytecode api: successfully recovered inflate stream\n");
		b->needSync = 0;
		continue;
	    }
	}
	break;
    } while (1);
    cli_bcapi_buffer_pipe_read_stopped(ctx, b->from, avail_in_orig - b->stream.avail_in);
    cli_bcapi_buffer_pipe_write_stopped(ctx, b->to, avail_out_orig - b->stream.avail_out);

    if (ret == Z_MEM_ERROR) {
	cli_dbgmsg("bytecode api: out of memory!\n");
	cli_bcapi_inflate_done(ctx, id);
	return ret;
    }
    if (ret == Z_STREAM_END) {
	cli_bcapi_inflate_done(ctx, id);
    }
    if (ret == Z_BUF_ERROR) {
	cli_dbgmsg("bytecode api: buffer error!\n");
    }

    return ret;
}