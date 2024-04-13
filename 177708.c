print_help(void) {
	printf("\
Usage: %s [OPTIONS] TABLE[,TABLE,...]\n",
			program_name);

	fputs("\
Translate whatever is on standard input and print it on standard\n\
output. It is intended for large-scale testing of the accuracy of\n\
Braille translation and back-translation.\n\n",
			stdout);

	fputs("\
Options:\n\
  -h, --help          display this help and exit\n\
  -v, --version       display version information and exit\n\
  -f, --forward       forward translation using the given table\n\
  -b, --backward      backward translation using the given table\n\
                      If neither -f nor -b are specified forward translation\n\
                      is assumed\n",
			stdout);
	fputs("\
Examples:\n\
  lou_translate --forward en-us-g2.ctb < input.txt\n\
  \n\
  Do a forward translation with table en-us-g2.ctb. The resulting braille is\n\
  ASCII encoded.\n\
  \n\
  lou_translate unicode.dis,en-us-g2.ctb < input.txt\n\
  \n\
  Do a forward translation with table en-us-g2.ctb. The resulting braille is\n\
  encoded as Unicode dot patterns.\n\
  \n\
  echo \",! qk br{n fox\" | lou_translate --backward en-us-g2.ctb\n\
  \n\
  Do a backward translation with table en-us-g2.ctb.\n",
			stdout);
	printf("\n");
	printf("Report bugs to %s.\n", PACKAGE_BUGREPORT);

#ifdef PACKAGE_PACKAGER_BUG_REPORTS
	printf("Report %s bugs to: %s\n", PACKAGE_PACKAGER, PACKAGE_PACKAGER_BUG_REPORTS);
#endif
#ifdef PACKAGE_URL
	printf("%s home page: <%s>\n", PACKAGE_NAME, PACKAGE_URL);
#endif
}