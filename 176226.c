static int ax88179_stop(struct usbnet *dev)
{
	u16 tmp16;

	ax88179_read_cmd(dev, AX_ACCESS_MAC, AX_MEDIUM_STATUS_MODE,
			 2, 2, &tmp16);
	tmp16 &= ~AX_MEDIUM_RECEIVE_EN;
	ax88179_write_cmd(dev, AX_ACCESS_MAC, AX_MEDIUM_STATUS_MODE,
			  2, 2, &tmp16);

	return 0;
}