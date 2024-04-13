static bool i40e_is_any_channel(struct i40e_vsi *vsi)
{
	struct i40e_channel *ch, *ch_tmp;

	list_for_each_entry_safe(ch, ch_tmp, &vsi->ch_list, list) {
		if (ch->initialized)
			return true;
	}

	return false;
}