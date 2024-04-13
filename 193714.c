static int llsec_recover_addr(struct mac802154_llsec *sec,
			      struct ieee802154_addr *addr)
{
	__le16 caddr = sec->params.coord_shortaddr;

	addr->pan_id = sec->params.pan_id;

	if (caddr == cpu_to_le16(IEEE802154_ADDR_BROADCAST)) {
		return -EINVAL;
	} else if (caddr == cpu_to_le16(IEEE802154_ADDR_UNDEF)) {
		addr->extended_addr = sec->params.coord_hwaddr;
		addr->mode = IEEE802154_ADDR_LONG;
	} else {
		addr->short_addr = sec->params.coord_shortaddr;
		addr->mode = IEEE802154_ADDR_SHORT;
	}

	return 0;
}