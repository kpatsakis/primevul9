static GF_Err mpgviddmx_initialize(GF_Filter *filter)
{
	GF_MPGVidDmxCtx *ctx = gf_filter_get_udta(filter);
	ctx->hdr_store_size = 0;
	ctx->hdr_store_alloc = 8;
	ctx->hdr_store = gf_malloc(sizeof(char)*8);
	return GF_OK;
}