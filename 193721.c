llsec_find_seclevel(const struct mac802154_llsec *sec,
		    const struct ieee802154_llsec_seclevel *sl)
{
	struct ieee802154_llsec_seclevel *pos;

	list_for_each_entry(pos, &sec->table.security_levels, list) {
		if (pos->frame_type != sl->frame_type ||
		    (pos->frame_type == IEEE802154_FC_TYPE_MAC_CMD &&
		     pos->cmd_frame_id != sl->cmd_frame_id) ||
		    pos->device_override != sl->device_override ||
		    pos->sec_levels != sl->sec_levels)
			continue;

		return container_of(pos, struct mac802154_llsec_seclevel,
				    level);
	}

	return NULL;
}