static inline int acm_set_control(struct acm *acm, int control)
{
	if (acm->quirks & QUIRK_CONTROL_LINE_STATE)
		return -EOPNOTSUPP;

	return acm_ctrl_msg(acm, USB_CDC_REQ_SET_CONTROL_LINE_STATE,
			control, NULL, 0);
}