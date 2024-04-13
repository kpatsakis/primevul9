static inline char *getFacilityStr(msg_t *pM)
{
        char *name = NULL;

        if(pM == NULL)
                return "";

	if(pM->iFacility < 0 || pM->iFacility > 23) {
		name = "invld";
	} else {
		name = syslog_fac_names[pM->iFacility];
	}

	return name;
}