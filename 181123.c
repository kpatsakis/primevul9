void ath6kl_wmi_sscan_timer(struct timer_list *t)
{
	struct ath6kl_vif *vif = from_timer(vif, t, sched_scan_timer);

	cfg80211_sched_scan_results(vif->ar->wiphy, 0);
}