static int crypto_report_akcipher(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_stat_akcipher rakcipher;

	memset(&rakcipher, 0, sizeof(rakcipher));

	strscpy(rakcipher.type, "akcipher", sizeof(rakcipher.type));
	rakcipher.stat_encrypt_cnt = atomic64_read(&alg->stats.akcipher.encrypt_cnt);
	rakcipher.stat_encrypt_tlen = atomic64_read(&alg->stats.akcipher.encrypt_tlen);
	rakcipher.stat_decrypt_cnt = atomic64_read(&alg->stats.akcipher.decrypt_cnt);
	rakcipher.stat_decrypt_tlen = atomic64_read(&alg->stats.akcipher.decrypt_tlen);
	rakcipher.stat_sign_cnt = atomic64_read(&alg->stats.akcipher.sign_cnt);
	rakcipher.stat_verify_cnt = atomic64_read(&alg->stats.akcipher.verify_cnt);
	rakcipher.stat_err_cnt = atomic64_read(&alg->stats.akcipher.err_cnt);

	return nla_put(skb, CRYPTOCFGA_STAT_AKCIPHER,
		       sizeof(rakcipher), &rakcipher);
}