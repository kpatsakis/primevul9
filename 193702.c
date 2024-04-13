llsec_do_decrypt_unauth(struct sk_buff *skb, const struct mac802154_llsec *sec,
			const struct ieee802154_hdr *hdr,
			struct mac802154_llsec_key *key, __le64 dev_addr)
{
	u8 iv[16];
	unsigned char *data;
	int datalen;
	struct scatterlist src;
	SYNC_SKCIPHER_REQUEST_ON_STACK(req, key->tfm0);
	int err;

	llsec_geniv(iv, dev_addr, &hdr->sec);
	data = skb_mac_header(skb) + skb->mac_len;
	datalen = skb_tail_pointer(skb) - data;

	sg_init_one(&src, data, datalen);

	skcipher_request_set_sync_tfm(req, key->tfm0);
	skcipher_request_set_callback(req, 0, NULL, NULL);
	skcipher_request_set_crypt(req, &src, &src, datalen, iv);

	err = crypto_skcipher_decrypt(req);
	skcipher_request_zero(req);
	return err;
}