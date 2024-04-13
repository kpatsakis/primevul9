isdn_statstr(void)
{
	static char istatbuf[2048];
	char *p;
	int i;

	sprintf(istatbuf, "idmap:\t");
	p = istatbuf + strlen(istatbuf);
	for (i = 0; i < ISDN_MAX_CHANNELS; i++) {
		sprintf(p, "%s ", (dev->drvmap[i] < 0) ? "-" : dev->drvid[dev->drvmap[i]]);
		p = istatbuf + strlen(istatbuf);
	}
	sprintf(p, "\nchmap:\t");
	p = istatbuf + strlen(istatbuf);
	for (i = 0; i < ISDN_MAX_CHANNELS; i++) {
		sprintf(p, "%d ", dev->chanmap[i]);
		p = istatbuf + strlen(istatbuf);
	}
	sprintf(p, "\ndrmap:\t");
	p = istatbuf + strlen(istatbuf);
	for (i = 0; i < ISDN_MAX_CHANNELS; i++) {
		sprintf(p, "%d ", dev->drvmap[i]);
		p = istatbuf + strlen(istatbuf);
	}
	sprintf(p, "\nusage:\t");
	p = istatbuf + strlen(istatbuf);
	for (i = 0; i < ISDN_MAX_CHANNELS; i++) {
		sprintf(p, "%d ", dev->usage[i]);
		p = istatbuf + strlen(istatbuf);
	}
	sprintf(p, "\nflags:\t");
	p = istatbuf + strlen(istatbuf);
	for (i = 0; i < ISDN_MAX_DRIVERS; i++) {
		if (dev->drv[i]) {
			sprintf(p, "%ld ", dev->drv[i]->online);
			p = istatbuf + strlen(istatbuf);
		} else {
			sprintf(p, "? ");
			p = istatbuf + strlen(istatbuf);
		}
	}
	sprintf(p, "\nphone:\t");
	p = istatbuf + strlen(istatbuf);
	for (i = 0; i < ISDN_MAX_CHANNELS; i++) {
		sprintf(p, "%s ", dev->num[i]);
		p = istatbuf + strlen(istatbuf);
	}
	sprintf(p, "\n");
	return istatbuf;
}