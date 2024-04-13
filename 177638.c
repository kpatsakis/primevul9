isdn_map_eaz2msn(char *msn, int di)
{
	isdn_driver_t *this = dev->drv[di];
	int i;

	if (strlen(msn) == 1) {
		i = msn[0] - '0';
		if ((i >= 0) && (i <= 9))
			if (strlen(this->msn2eaz[i]))
				return (this->msn2eaz[i]);
	}
	return (msn);
}