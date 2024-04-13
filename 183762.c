static int crypto_report_cipher(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_stat_cipher rcipher;

	memset(&rcipher, 0, sizeof(rcipher));

	strscpy(rcipher.type, "cipher", sizeof(rcipher.type));

	rcipher.stat_encrypt_cnt = atomic64_read(&alg->stats.cipher.encrypt_cnt);
	rcipher.stat_encrypt_tlen = atomic64_read(&alg->stats.cipher.encrypt_tlen);
	rcipher.stat_decrypt_cnt =  atomic64_read(&alg->stats.cipher.decrypt_cnt);
	rcipher.stat_decrypt_tlen = atomic64_read(&alg->stats.cipher.decrypt_tlen);
	rcipher.stat_err_cnt =  atomic64_read(&alg->stats.cipher.err_cnt);

	return nla_put(skb, CRYPTOCFGA_STAT_CIPHER, sizeof(rcipher), &rcipher);
}