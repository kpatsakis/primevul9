static int crypto_report_rng(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_stat_rng rrng;

	memset(&rrng, 0, sizeof(rrng));

	strscpy(rrng.type, "rng", sizeof(rrng.type));

	rrng.stat_generate_cnt = atomic64_read(&alg->stats.rng.generate_cnt);
	rrng.stat_generate_tlen = atomic64_read(&alg->stats.rng.generate_tlen);
	rrng.stat_seed_cnt = atomic64_read(&alg->stats.rng.seed_cnt);
	rrng.stat_err_cnt = atomic64_read(&alg->stats.rng.err_cnt);

	return nla_put(skb, CRYPTOCFGA_STAT_RNG, sizeof(rrng), &rrng);
}