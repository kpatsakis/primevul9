static int rtl8xxxu_set_rts_threshold(struct ieee80211_hw *hw, u32 rts)
{
	if (rts > 2347)
		return -EINVAL;

	return 0;
}