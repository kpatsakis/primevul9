static int crypto_report_shash(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_stat_hash rhash;

	memset(&rhash, 0, sizeof(rhash));

	strscpy(rhash.type, "shash", sizeof(rhash.type));

	rhash.stat_hash_cnt =  atomic64_read(&alg->stats.hash.hash_cnt);
	rhash.stat_hash_tlen = atomic64_read(&alg->stats.hash.hash_tlen);
	rhash.stat_err_cnt = atomic64_read(&alg->stats.hash.err_cnt);

	return nla_put(skb, CRYPTOCFGA_STAT_HASH, sizeof(rhash), &rhash);
}