int mac802154_llsec_decrypt(struct mac802154_llsec *sec, struct sk_buff *skb)
{
	struct ieee802154_hdr hdr;
	struct mac802154_llsec_key *key;
	struct ieee802154_llsec_key_id key_id;
	struct mac802154_llsec_device *dev;
	struct ieee802154_llsec_seclevel seclevel;
	int err;
	__le64 dev_addr;
	u32 frame_ctr;

	if (ieee802154_hdr_peek(skb, &hdr) < 0)
		return -EINVAL;
	if (!hdr.fc.security_enabled)
		return 0;
	if (hdr.fc.version == 0)
		return -EINVAL;

	read_lock_bh(&sec->lock);
	if (!sec->params.enabled) {
		read_unlock_bh(&sec->lock);
		return -EINVAL;
	}
	read_unlock_bh(&sec->lock);

	rcu_read_lock();

	key = llsec_lookup_key(sec, &hdr, &hdr.source, &key_id);
	if (!key) {
		err = -ENOKEY;
		goto fail;
	}

	dev = llsec_lookup_dev(sec, &hdr.source);
	if (!dev) {
		err = -EINVAL;
		goto fail_dev;
	}

	if (llsec_lookup_seclevel(sec, hdr.fc.type, 0, &seclevel) < 0) {
		err = -EINVAL;
		goto fail_dev;
	}

	if (!(seclevel.sec_levels & BIT(hdr.sec.level)) &&
	    (hdr.sec.level == 0 && seclevel.device_override &&
	     !dev->dev.seclevel_exempt)) {
		err = -EINVAL;
		goto fail_dev;
	}

	frame_ctr = le32_to_cpu(hdr.sec.frame_counter);

	if (frame_ctr == 0xffffffff) {
		err = -EOVERFLOW;
		goto fail_dev;
	}

	err = llsec_update_devkey_info(dev, &key_id, frame_ctr);
	if (err)
		goto fail_dev;

	dev_addr = dev->dev.hwaddr;

	rcu_read_unlock();

	err = llsec_do_decrypt(skb, sec, &hdr, key, dev_addr);
	llsec_key_put(key);
	return err;

fail_dev:
	llsec_key_put(key);
fail:
	rcu_read_unlock();
	return err;
}