static u32 rtl8xxxu_80211_to_rtl_queue(u32 queue)
{
	u32 rtlqueue;

	switch (queue) {
	case IEEE80211_AC_VO:
		rtlqueue = TXDESC_QUEUE_VO;
		break;
	case IEEE80211_AC_VI:
		rtlqueue = TXDESC_QUEUE_VI;
		break;
	case IEEE80211_AC_BE:
		rtlqueue = TXDESC_QUEUE_BE;
		break;
	case IEEE80211_AC_BK:
		rtlqueue = TXDESC_QUEUE_BK;
		break;
	default:
		rtlqueue = TXDESC_QUEUE_BE;
	}

	return rtlqueue;
}