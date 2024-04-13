int bwa_fa2pac(int argc, char *argv[])
{
	int c, for_only = 0;
	gzFile fp;
	while ((c = getopt(argc, argv, "f")) >= 0) {
		switch (c) {
			case 'f': for_only = 1; break;
		}
	}
	if (argc == optind) {
		fprintf(stderr, "Usage: bwa fa2pac [-f] <in.fasta> [<out.prefix>]\n");
		return 1;
	}
	fp = xzopen(argv[optind], "r");
	bns_fasta2bntseq(fp, (optind+1 < argc)? argv[optind+1] : argv[optind], for_only);
	err_gzclose(fp);
	return 0;
}