int cfg80211_wext_siwmlme(struct net_device *dev,
			  struct iw_request_info *info,
			  struct iw_point *data, char *extra)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct iw_mlme *mlme = (struct iw_mlme *)extra;
	struct cfg80211_registered_device *rdev;
	int err;

	if (!wdev)
		return -EOPNOTSUPP;

	rdev = wiphy_to_rdev(wdev->wiphy);

	if (wdev->iftype != NL80211_IFTYPE_STATION)
		return -EINVAL;

	if (mlme->addr.sa_family != ARPHRD_ETHER)
		return -EINVAL;

	wdev_lock(wdev);
	switch (mlme->cmd) {
	case IW_MLME_DEAUTH:
	case IW_MLME_DISASSOC:
		err = cfg80211_disconnect(rdev, dev, mlme->reason_code, true);
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	wdev_unlock(wdev);

	return err;
}