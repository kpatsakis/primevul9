static GFINLINE void mpgviddmx_update_time(GF_MPGVidDmxCtx *ctx)
{
	assert(ctx->cur_fps.num);

	if (ctx->timescale) {
		u64 inc = 3000;
		if (ctx->cur_fps.den && ctx->cur_fps.num) {
			inc = ctx->cur_fps.den;
			if (ctx->cur_fps.num != ctx->timescale) {
				inc *= ctx->timescale;
				inc /= ctx->cur_fps.num;
			}
		}
		ctx->cts += inc;
		ctx->dts += inc;
	} else {
		assert(ctx->cur_fps.den);
		ctx->cts += ctx->cur_fps.den;
		ctx->dts += ctx->cur_fps.den;
	}
}