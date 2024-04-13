static int crypto_report_kpp(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_stat_kpp rkpp;

	memset(&rkpp, 0, sizeof(rkpp));

	strscpy(rkpp.type, "kpp", sizeof(rkpp.type));

	rkpp.stat_setsecret_cnt = atomic64_read(&alg->stats.kpp.setsecret_cnt);
	rkpp.stat_generate_public_key_cnt = atomic64_read(&alg->stats.kpp.generate_public_key_cnt);
	rkpp.stat_compute_shared_secret_cnt = atomic64_read(&alg->stats.kpp.compute_shared_secret_cnt);
	rkpp.stat_err_cnt = atomic64_read(&alg->stats.kpp.err_cnt);

	return nla_put(skb, CRYPTOCFGA_STAT_KPP, sizeof(rkpp), &rkpp);
}