static inline char *getFacility(msg_t *pM)
{
	char *name = NULL;

	if(pM == NULL)
		return "";

	if(pM->iFacility < 0 || pM->iFacility > 23) {
		name = "invld";
	} else {
		name = syslog_number_names[pM->iFacility];
	}

	return name;
}