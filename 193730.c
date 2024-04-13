llsec_do_decrypt_auth(struct sk_buff *skb, const struct mac802154_llsec *sec,
		      const struct ieee802154_hdr *hdr,
		      struct mac802154_llsec_key *key, __le64 dev_addr)
{
	u8 iv[16];
	unsigned char *data;
	int authlen, datalen, assoclen, rc;
	struct scatterlist sg;
	struct aead_request *req;

	authlen = ieee802154_sechdr_authtag_len(&hdr->sec);
	llsec_geniv(iv, dev_addr, &hdr->sec);

	req = aead_request_alloc(llsec_tfm_by_len(key, authlen), GFP_ATOMIC);
	if (!req)
		return -ENOMEM;

	assoclen = skb->mac_len;

	data = skb_mac_header(skb) + skb->mac_len;
	datalen = skb_tail_pointer(skb) - data;

	sg_init_one(&sg, skb_mac_header(skb), assoclen + datalen);

	if (!(hdr->sec.level & IEEE802154_SCF_SECLEVEL_ENC)) {
		assoclen += datalen - authlen;
		datalen = authlen;
	}

	aead_request_set_callback(req, 0, NULL, NULL);
	aead_request_set_crypt(req, &sg, &sg, datalen, iv);
	aead_request_set_ad(req, assoclen);

	rc = crypto_aead_decrypt(req);

	kfree_sensitive(req);
	skb_trim(skb, skb->len - authlen);

	return rc;
}