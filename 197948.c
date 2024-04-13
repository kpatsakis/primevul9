static int usage(void)
{
	fprintf(stderr,
		"muraster version " FZ_VERSION "\n"
		"Usage: muraster [options] file [pages]\n"
		"\t-p -\tpassword\n"
		"\n"
		"\t-o -\toutput file name\n"
		"\t-F -\toutput format (default inferred from output file name)\n"
		"\t\tpam, pbm, pgm, pkm, ppm\n"
		"\n"
		"\t-s -\tshow extra information:\n"
		"\t\tm - show memory use\n"
		"\t\tt - show timings\n"
		"\n"
		"\t-R {auto,0,90,180,270}\n"
		"\t\trotate clockwise (default: auto)\n"
		"\t-r -{,_}\tx and y resolution in dpi (default: " stringify(X_RESOLUTION) "x" stringify(Y_RESOLUTION) ")\n"
		"\t-w -\tprintable width (in inches) (default: " stringify(PAPER_WIDTH) ")\n"
		"\t-h -\tprintable height (in inches) (default: " stringify(PAPER_HEIGHT) "\n"
		"\t-f\tfit file to page if too large\n"
		"\t-B -\tminimum band height (e.g. 32)\n"
		"\t-M -\tmax bandmemory (e.g. 655360)\n"
#ifndef DISABLE_MUTHREADS
		"\t-T -\tnumber of threads to use for rendering\n"
		"\t-P\tparallel interpretation/rendering\n"
#endif
		"\n"
		"\t-W -\tpage width for EPUB layout\n"
		"\t-H -\tpage height for EPUB layout\n"
		"\t-S -\tfont size for EPUB layout\n"
		"\t-U -\tfile name of user stylesheet for EPUB layout\n"
		"\t-X\tdisable document styles for EPUB layout\n"
		"\n"
		"\t-A -\tnumber of bits of antialiasing (0 to 8)\n"
		"\t-A -/-\tnumber of bits of antialiasing (0 to 8) (graphics, text)\n"
		"\n"
		"\tpages\tcomma separated list of page numbers and ranges\n"
		);
	return 1;
}