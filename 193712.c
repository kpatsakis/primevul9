int mac802154_llsec_set_params(struct mac802154_llsec *sec,
			       const struct ieee802154_llsec_params *params,
			       int changed)
{
	write_lock_bh(&sec->lock);

	if (changed & IEEE802154_LLSEC_PARAM_ENABLED)
		sec->params.enabled = params->enabled;
	if (changed & IEEE802154_LLSEC_PARAM_FRAME_COUNTER)
		sec->params.frame_counter = params->frame_counter;
	if (changed & IEEE802154_LLSEC_PARAM_OUT_LEVEL)
		sec->params.out_level = params->out_level;
	if (changed & IEEE802154_LLSEC_PARAM_OUT_KEY)
		sec->params.out_key = params->out_key;
	if (changed & IEEE802154_LLSEC_PARAM_KEY_SOURCE)
		sec->params.default_key_source = params->default_key_source;
	if (changed & IEEE802154_LLSEC_PARAM_PAN_ID)
		sec->params.pan_id = params->pan_id;
	if (changed & IEEE802154_LLSEC_PARAM_HWADDR)
		sec->params.hwaddr = params->hwaddr;
	if (changed & IEEE802154_LLSEC_PARAM_COORD_HWADDR)
		sec->params.coord_hwaddr = params->coord_hwaddr;
	if (changed & IEEE802154_LLSEC_PARAM_COORD_SHORTADDR)
		sec->params.coord_shortaddr = params->coord_shortaddr;

	write_unlock_bh(&sec->lock);

	return 0;
}