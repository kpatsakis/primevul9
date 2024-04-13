llsec_lookup_seclevel(const struct mac802154_llsec *sec,
		      u8 frame_type, u8 cmd_frame_id,
		      struct ieee802154_llsec_seclevel *rlevel)
{
	struct ieee802154_llsec_seclevel *level;

	list_for_each_entry_rcu(level, &sec->table.security_levels, list) {
		if (level->frame_type == frame_type &&
		    (frame_type != IEEE802154_FC_TYPE_MAC_CMD ||
		     level->cmd_frame_id == cmd_frame_id)) {
			*rlevel = *level;
			return 0;
		}
	}

	return -EINVAL;
}