isdn_capi_rec_hl_msg(capi_msg *cm)
{
	switch (cm->Command) {
	case CAPI_FACILITY:
		/* in the moment only handled in tty */
		return (isdn_tty_capi_facility(cm));
	default:
		return (-1);
	}
}