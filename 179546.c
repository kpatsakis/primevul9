static int af9005_frontend_attach(struct dvb_usb_adapter *adap)
{
	u8 buf[8];
	int i;

	/* without these calls the first commands after downloading
	   the firmware fail. I put these calls here to simulate
	   what it is done in dvb-usb-init.c.
	 */
	struct usb_device *udev = adap->dev->udev;
	usb_clear_halt(udev, usb_sndbulkpipe(udev, 2));
	usb_clear_halt(udev, usb_rcvbulkpipe(udev, 1));
	if (dvb_usb_af9005_dump_eeprom) {
		printk("EEPROM DUMP\n");
		for (i = 0; i < 255; i += 8) {
			af9005_read_eeprom(adap->dev, i, buf, 8);
			debug_dump(buf, 8, printk);
		}
	}
	adap->fe_adap[0].fe = af9005_fe_attach(adap->dev);
	return 0;
}