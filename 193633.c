static int crypto_report_cipher(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_report_cipher rcipher;

	memset(&rcipher, 0, sizeof(rcipher));

	strscpy(rcipher.type, "cipher", sizeof(rcipher.type));

	rcipher.blocksize = alg->cra_blocksize;
	rcipher.min_keysize = alg->cra_cipher.cia_min_keysize;
	rcipher.max_keysize = alg->cra_cipher.cia_max_keysize;

	return nla_put(skb, CRYPTOCFGA_REPORT_CIPHER,
		       sizeof(rcipher), &rcipher);
}