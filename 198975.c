static ssize_t wCountryCodes_show
(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct usb_interface *intf = to_usb_interface(dev);
	struct acm *acm = usb_get_intfdata(intf);

	memcpy(buf, acm->country_codes, acm->country_code_size);
	return acm->country_code_size;
}