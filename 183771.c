static int crypto_report_acomp(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_stat_compress racomp;

	memset(&racomp, 0, sizeof(racomp));

	strscpy(racomp.type, "acomp", sizeof(racomp.type));
	racomp.stat_compress_cnt = atomic64_read(&alg->stats.compress.compress_cnt);
	racomp.stat_compress_tlen = atomic64_read(&alg->stats.compress.compress_tlen);
	racomp.stat_decompress_cnt =  atomic64_read(&alg->stats.compress.decompress_cnt);
	racomp.stat_decompress_tlen = atomic64_read(&alg->stats.compress.decompress_tlen);
	racomp.stat_err_cnt = atomic64_read(&alg->stats.compress.err_cnt);

	return nla_put(skb, CRYPTOCFGA_STAT_ACOMP, sizeof(racomp), &racomp);
}