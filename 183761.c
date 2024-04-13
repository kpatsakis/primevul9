static int crypto_report_aead(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_stat_aead raead;

	memset(&raead, 0, sizeof(raead));

	strscpy(raead.type, "aead", sizeof(raead.type));

	raead.stat_encrypt_cnt = atomic64_read(&alg->stats.aead.encrypt_cnt);
	raead.stat_encrypt_tlen = atomic64_read(&alg->stats.aead.encrypt_tlen);
	raead.stat_decrypt_cnt = atomic64_read(&alg->stats.aead.decrypt_cnt);
	raead.stat_decrypt_tlen = atomic64_read(&alg->stats.aead.decrypt_tlen);
	raead.stat_err_cnt = atomic64_read(&alg->stats.aead.err_cnt);

	return nla_put(skb, CRYPTOCFGA_STAT_AEAD, sizeof(raead), &raead);
}