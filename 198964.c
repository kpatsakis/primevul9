static void acm_port_dtr_rts(struct tty_port *port, int raise)
{
	struct acm *acm = container_of(port, struct acm, port);
	int val;
	int res;

	if (raise)
		val = ACM_CTRL_DTR | ACM_CTRL_RTS;
	else
		val = 0;

	/* FIXME: add missing ctrlout locking throughout driver */
	acm->ctrlout = val;

	res = acm_set_control(acm, val);
	if (res && (acm->ctrl_caps & USB_CDC_CAP_LINE))
		dev_err(&acm->control->dev, "failed to set dtr/rts\n");
}