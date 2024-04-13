static void rtl8xxxu_set_linktype(struct rtl8xxxu_priv *priv,
				  enum nl80211_iftype linktype)
{
	u8 val8;

	val8 = rtl8xxxu_read8(priv, REG_MSR);
	val8 &= ~MSR_LINKTYPE_MASK;

	switch (linktype) {
	case NL80211_IFTYPE_UNSPECIFIED:
		val8 |= MSR_LINKTYPE_NONE;
		break;
	case NL80211_IFTYPE_ADHOC:
		val8 |= MSR_LINKTYPE_ADHOC;
		break;
	case NL80211_IFTYPE_STATION:
		val8 |= MSR_LINKTYPE_STATION;
		break;
	case NL80211_IFTYPE_AP:
		val8 |= MSR_LINKTYPE_AP;
		break;
	default:
		goto out;
	}

	rtl8xxxu_write8(priv, REG_MSR, val8);
out:
	return;
}