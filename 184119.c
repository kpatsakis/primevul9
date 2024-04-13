void legacyOptsHook(void)
{
	legacyOptsLL_t *pThis = pLegacyOptsLL;

	while(pThis != NULL) {
		if(pThis->line != NULL) {
			errno = 0;
			errmsg.LogError(0, NO_ERRCODE, "Warning: backward compatibility layer added to following "
				        "directive to rsyslog.conf: %s", pThis->line);
			conf.cfsysline(pThis->line);
		}
		pThis = pThis->next;
	}
}