static int crypto_report_comp(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_stat_compress rcomp;

	memset(&rcomp, 0, sizeof(rcomp));

	strscpy(rcomp.type, "compression", sizeof(rcomp.type));
	rcomp.stat_compress_cnt = atomic64_read(&alg->stats.compress.compress_cnt);
	rcomp.stat_compress_tlen = atomic64_read(&alg->stats.compress.compress_tlen);
	rcomp.stat_decompress_cnt = atomic64_read(&alg->stats.compress.decompress_cnt);
	rcomp.stat_decompress_tlen = atomic64_read(&alg->stats.compress.decompress_tlen);
	rcomp.stat_err_cnt = atomic64_read(&alg->stats.compress.err_cnt);

	return nla_put(skb, CRYPTOCFGA_STAT_COMPRESS, sizeof(rcomp), &rcomp);
}