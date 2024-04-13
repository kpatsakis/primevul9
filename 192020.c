static void rtl8xxxu_disconnect(struct usb_interface *interface)
{
	struct rtl8xxxu_priv *priv;
	struct ieee80211_hw *hw;

	hw = usb_get_intfdata(interface);
	priv = hw->priv;

	ieee80211_unregister_hw(hw);

	priv->fops->power_off(priv);

	usb_set_intfdata(interface, NULL);

	dev_info(&priv->udev->dev, "disconnecting\n");

	kfree(priv->fw_data);
	mutex_destroy(&priv->usb_buf_mutex);
	mutex_destroy(&priv->h2c_mutex);

	if (priv->udev->state != USB_STATE_NOTATTACHED) {
		dev_info(&priv->udev->dev,
			 "Device still attached, trying to reset\n");
		usb_reset_device(priv->udev);
	}
	usb_put_dev(priv->udev);
	ieee80211_free_hw(hw);
}