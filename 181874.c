u16 capi20_isinstalled(void)
{
	u16 ret = CAPI_REGNOTINSTALLED;
	int i;

	mutex_lock(&capi_controller_lock);

	for (i = 0; i < CAPI_MAXCONTR; i++)
		if (capi_controller[i] &&
		    capi_controller[i]->state == CAPI_CTR_RUNNING) {
			ret = CAPI_NOERROR;
			break;
		}

	mutex_unlock(&capi_controller_lock);

	return ret;
}