u16 capi20_get_serial(u32 contr, u8 serial[CAPI_SERIAL_LEN])
{
	struct capi_ctr *ctr;
	u16 ret;

	if (contr == 0) {
		strlcpy(serial, driver_serial, CAPI_SERIAL_LEN);
		return CAPI_NOERROR;
	}

	mutex_lock(&capi_controller_lock);

	ctr = get_capi_ctr_by_nr(contr);
	if (ctr && ctr->state == CAPI_CTR_RUNNING) {
		strlcpy(serial, ctr->serial, CAPI_SERIAL_LEN);
		ret = CAPI_NOERROR;
	} else
		ret = CAPI_REGNOTINSTALLED;

	mutex_unlock(&capi_controller_lock);
	return ret;
}