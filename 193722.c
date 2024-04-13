int mac802154_llsec_encrypt(struct mac802154_llsec *sec, struct sk_buff *skb)
{
	struct ieee802154_hdr hdr;
	int rc, authlen, hlen;
	struct mac802154_llsec_key *key;
	u32 frame_ctr;

	hlen = ieee802154_hdr_pull(skb, &hdr);

	if (hlen < 0 || hdr.fc.type != IEEE802154_FC_TYPE_DATA)
		return -EINVAL;

	if (!hdr.fc.security_enabled ||
	    (hdr.sec.level == IEEE802154_SCF_SECLEVEL_NONE)) {
		skb_push(skb, hlen);
		return 0;
	}

	authlen = ieee802154_sechdr_authtag_len(&hdr.sec);

	if (skb->len + hlen + authlen + IEEE802154_MFR_SIZE > IEEE802154_MTU)
		return -EMSGSIZE;

	rcu_read_lock();

	read_lock_bh(&sec->lock);

	if (!sec->params.enabled) {
		rc = -EINVAL;
		goto fail_read;
	}

	key = llsec_lookup_key(sec, &hdr, &hdr.dest, NULL);
	if (!key) {
		rc = -ENOKEY;
		goto fail_read;
	}

	read_unlock_bh(&sec->lock);

	write_lock_bh(&sec->lock);

	frame_ctr = be32_to_cpu(sec->params.frame_counter);
	hdr.sec.frame_counter = cpu_to_le32(frame_ctr);
	if (frame_ctr == 0xFFFFFFFF) {
		write_unlock_bh(&sec->lock);
		llsec_key_put(key);
		rc = -EOVERFLOW;
		goto fail;
	}

	sec->params.frame_counter = cpu_to_be32(frame_ctr + 1);

	write_unlock_bh(&sec->lock);

	rcu_read_unlock();

	skb->mac_len = ieee802154_hdr_push(skb, &hdr);
	skb_reset_mac_header(skb);

	rc = llsec_do_encrypt(skb, sec, &hdr, key);
	llsec_key_put(key);

	return rc;

fail_read:
	read_unlock_bh(&sec->lock);
fail:
	rcu_read_unlock();
	return rc;
}