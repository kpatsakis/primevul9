get_th(char *num, int type)
{
	int			len = strlen(num),
				last,
				seclast;

	last = *(num + (len - 1));
	if (!isdigit((unsigned char) last))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("\"%s\" is not a number", num)));

	/*
	 * All "teens" (<x>1[0-9]) get 'TH/th', while <x>[02-9][123] still get
	 * 'ST/st', 'ND/nd', 'RD/rd', respectively
	 */
	if ((len > 1) && ((seclast = num[len - 2]) == '1'))
		last = 0;

	switch (last)
	{
		case '1':
			if (type == TH_UPPER)
				return numTH[0];
			return numth[0];
		case '2':
			if (type == TH_UPPER)
				return numTH[1];
			return numth[1];
		case '3':
			if (type == TH_UPPER)
				return numTH[2];
			return numth[2];
		default:
			if (type == TH_UPPER)
				return numTH[3];
			return numth[3];
	}
}