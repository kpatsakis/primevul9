int isdn_msncmp(const char *msn1, const char *msn2)
{
	char TmpMsn1[ISDN_MSNLEN];
	char TmpMsn2[ISDN_MSNLEN];
	char *p;

	for (p = TmpMsn1; *msn1 && *msn1 != ':';)  // Strip off a SPID
		*p++ = *msn1++;
	*p = '\0';

	for (p = TmpMsn2; *msn2 && *msn2 != ':';)  // Strip off a SPID
		*p++ = *msn2++;
	*p = '\0';

	return isdn_wildmat(TmpMsn1, TmpMsn2);
}