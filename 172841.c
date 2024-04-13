static char *get_frame_label(int type) {
	static char label[128];
	int nf = egg->lang.nfunctions;
	int nb = egg->lang.nbrackets;
	int ct = context;
	/* TODO: this type hack to substruct nb and ctx looks weird */
#if 1
	if (type == 1) {
		nb--;
	} else if (type == 2) {
		ct--;
	}
#endif
	/* THIS IS GAS_ONLY */
	snprintf (label, sizeof (label), FRAME_FMT, nf, nb, ct);
	return label;
}