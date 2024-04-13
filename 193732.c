int mac802154_llsec_get_params(struct mac802154_llsec *sec,
			       struct ieee802154_llsec_params *params)
{
	read_lock_bh(&sec->lock);
	*params = sec->params;
	read_unlock_bh(&sec->lock);

	return 0;
}