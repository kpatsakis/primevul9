static inline char *getSeverity(msg_t *pM)
{
	char *name = NULL;

	if(pM == NULL)
		return "";

	if(pM->iSeverity < 0 || pM->iSeverity > 7) {
		name = "invld";
	} else {
		name = syslog_number_names[pM->iSeverity];
	}

	return name;
}