static int ax88179_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct usbnet *dev = usb_get_intfdata(intf);
	u16 tmp16;
	u8 tmp8;

	usbnet_suspend(intf, message);

	/* Disable RX path */
	ax88179_read_cmd_nopm(dev, AX_ACCESS_MAC, AX_MEDIUM_STATUS_MODE,
			      2, 2, &tmp16);
	tmp16 &= ~AX_MEDIUM_RECEIVE_EN;
	ax88179_write_cmd_nopm(dev, AX_ACCESS_MAC, AX_MEDIUM_STATUS_MODE,
			       2, 2, &tmp16);

	/* Force bulk-in zero length */
	ax88179_read_cmd_nopm(dev, AX_ACCESS_MAC, AX_PHYPWR_RSTCTL,
			      2, 2, &tmp16);

	tmp16 |= AX_PHYPWR_RSTCTL_BZ | AX_PHYPWR_RSTCTL_IPRL;
	ax88179_write_cmd_nopm(dev, AX_ACCESS_MAC, AX_PHYPWR_RSTCTL,
			       2, 2, &tmp16);

	/* change clock */
	tmp8 = 0;
	ax88179_write_cmd_nopm(dev, AX_ACCESS_MAC, AX_CLK_SELECT, 1, 1, &tmp8);

	/* Configure RX control register => stop operation */
	tmp16 = AX_RX_CTL_STOP;
	ax88179_write_cmd_nopm(dev, AX_ACCESS_MAC, AX_RX_CTL, 2, 2, &tmp16);

	return 0;
}