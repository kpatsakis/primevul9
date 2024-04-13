llsec_lookup_key(struct mac802154_llsec *sec,
		 const struct ieee802154_hdr *hdr,
		 const struct ieee802154_addr *addr,
		 struct ieee802154_llsec_key_id *key_id)
{
	struct ieee802154_addr devaddr = *addr;
	u8 key_id_mode = hdr->sec.key_id_mode;
	struct ieee802154_llsec_key_entry *key_entry;
	struct mac802154_llsec_key *key;

	if (key_id_mode == IEEE802154_SCF_KEY_IMPLICIT &&
	    devaddr.mode == IEEE802154_ADDR_NONE) {
		if (hdr->fc.type == IEEE802154_FC_TYPE_BEACON) {
			devaddr.extended_addr = sec->params.coord_hwaddr;
			devaddr.mode = IEEE802154_ADDR_LONG;
		} else if (llsec_recover_addr(sec, &devaddr) < 0) {
			return NULL;
		}
	}

	list_for_each_entry_rcu(key_entry, &sec->table.keys, list) {
		const struct ieee802154_llsec_key_id *id = &key_entry->id;

		if (!(key_entry->key->frame_types & BIT(hdr->fc.type)))
			continue;

		if (id->mode != key_id_mode)
			continue;

		if (key_id_mode == IEEE802154_SCF_KEY_IMPLICIT) {
			if (ieee802154_addr_equal(&devaddr, &id->device_addr))
				goto found;
		} else {
			if (id->id != hdr->sec.key_id)
				continue;

			if ((key_id_mode == IEEE802154_SCF_KEY_INDEX) ||
			    (key_id_mode == IEEE802154_SCF_KEY_SHORT_INDEX &&
			     id->short_source == hdr->sec.short_src) ||
			    (key_id_mode == IEEE802154_SCF_KEY_HW_INDEX &&
			     id->extended_source == hdr->sec.extended_src))
				goto found;
		}
	}

	return NULL;

found:
	key = container_of(key_entry->key, struct mac802154_llsec_key, key);
	if (key_id)
		*key_id = key_entry->id;
	return llsec_key_get(key);
}