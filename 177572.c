static char *map_drvname(int di)
{
	if ((di < 0) || (di >= ISDN_MAX_DRIVERS))
		return (NULL);
	return (dev->drvid[di]); /* driver name */
} /* map_drvname */