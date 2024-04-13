llsec_do_encrypt_unauth(struct sk_buff *skb, const struct mac802154_llsec *sec,
			const struct ieee802154_hdr *hdr,
			struct mac802154_llsec_key *key)
{
	u8 iv[16];
	struct scatterlist src;
	SYNC_SKCIPHER_REQUEST_ON_STACK(req, key->tfm0);
	int err, datalen;
	unsigned char *data;

	llsec_geniv(iv, sec->params.hwaddr, &hdr->sec);
	/* Compute data payload offset and data length */
	data = skb_mac_header(skb) + skb->mac_len;
	datalen = skb_tail_pointer(skb) - data;
	sg_init_one(&src, data, datalen);

	skcipher_request_set_sync_tfm(req, key->tfm0);
	skcipher_request_set_callback(req, 0, NULL, NULL);
	skcipher_request_set_crypt(req, &src, &src, datalen, iv);
	err = crypto_skcipher_encrypt(req);
	skcipher_request_zero(req);
	return err;
}