static bool llsec_key_id_equal(const struct ieee802154_llsec_key_id *a,
			       const struct ieee802154_llsec_key_id *b)
{
	if (a->mode != b->mode)
		return false;

	if (a->mode == IEEE802154_SCF_KEY_IMPLICIT)
		return ieee802154_addr_equal(&a->device_addr, &b->device_addr);

	if (a->id != b->id)
		return false;

	switch (a->mode) {
	case IEEE802154_SCF_KEY_INDEX:
		return true;
	case IEEE802154_SCF_KEY_SHORT_INDEX:
		return a->short_source == b->short_source;
	case IEEE802154_SCF_KEY_HW_INDEX:
		return a->extended_source == b->extended_source;
	}

	return false;
}