getPRI(msg_t *pM)
{
	/* PRI is a number in the range 0..191. Thus, we use a simple lookup table to obtain the
	 * string value. It looks a bit clumpsy here in code ;)
	 */
	int iPRI;

	if(pM == NULL)
		return "";

	iPRI = getPRIi(pM);
	return (iPRI > 191) ? "invld" : (char*)syslog_pri_names[iPRI].pszName;
}