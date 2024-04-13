void cmdusage()
{
	const char *s;
	int i;
	cmdinfo();
	fprintf(stderr, "usage: %s [options]\n", cmdname);
	for (i = 0, s = helpinfo[i]; s; ++i, s = helpinfo[i]) {
		fprintf(stderr, "%s", s);
	}

	fprintf(stderr, "The following formats are supported:\n");
	int fmtind;
	const jas_image_fmtinfo_t *fmtinfo;
	int numfmts = jas_image_getnumfmts();
	for (fmtind = 0; fmtind < numfmts; ++fmtind) {
		if (!(fmtinfo = jas_image_getfmtbyind(fmtind))) {
			break;
		}
		fprintf(stderr, "    %s %-5s    %s\n",
		  fmtinfo->enabled ? "[enabled] " : "[disabled]",
		  fmtinfo->name,
		  fmtinfo->desc);
	}
	exit(EXIT_FAILURE);
}