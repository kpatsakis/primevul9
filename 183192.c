static int pn533_acr122_poweron_rdr(struct pn533_usb_phy *phy)
{
	/* Power on th reader (CCID cmd) */
	u8 cmd[10] = {PN533_ACR122_PC_TO_RDR_ICCPOWERON,
		      0, 0, 0, 0, 0, 0, 3, 0, 0};
	char *buffer;
	int transferred;
	int rc;
	void *cntx;
	struct pn533_acr122_poweron_rdr_arg arg;

	dev_dbg(&phy->udev->dev, "%s\n", __func__);

	buffer = kmemdup(cmd, sizeof(cmd), GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	init_completion(&arg.done);
	cntx = phy->in_urb->context;  /* backup context */

	phy->in_urb->complete = pn533_acr122_poweron_rdr_resp;
	phy->in_urb->context = &arg;

	print_hex_dump_debug("ACR122 TX: ", DUMP_PREFIX_NONE, 16, 1,
		       cmd, sizeof(cmd), false);

	rc = usb_bulk_msg(phy->udev, phy->out_urb->pipe, buffer, sizeof(cmd),
			  &transferred, 0);
	kfree(buffer);
	if (rc || (transferred != sizeof(cmd))) {
		nfc_err(&phy->udev->dev,
			"Reader power on cmd error %d\n", rc);
		return rc;
	}

	rc =  usb_submit_urb(phy->in_urb, GFP_KERNEL);
	if (rc) {
		nfc_err(&phy->udev->dev,
			"Can't submit reader poweron cmd response %d\n", rc);
		return rc;
	}

	wait_for_completion(&arg.done);
	phy->in_urb->context = cntx; /* restore context */

	return arg.rc;
}