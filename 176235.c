static void ax88179_unbind(struct usbnet *dev, struct usb_interface *intf)
{
	u16 tmp16;

	/* Configure RX control register => stop operation */
	tmp16 = AX_RX_CTL_STOP;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_RX_CTL, 2, 2, &tmp16);

	tmp16 = 0;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_CLK_SELECT, 1, 1, &tmp16);

	/* Power down ethernet PHY */
	tmp16 = 0;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_PHYPWR_RSTCTL, 2, 2, &tmp16);
}