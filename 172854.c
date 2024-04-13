static char *get_end_frame_label(REgg *egg) {
	static char label[128];
	snprintf (label, sizeof (label) - 1, FRAME_END_FMT,
		egg->lang.nfunctions, egg->lang.nbrackets, context - 1);
// eprintf ("--> (egg->lang.endframe: %d %d %d)\n", egg->lang.nfunctions, egg->lang.nbrackets, context);
	// snprintf (label, sizeof (label)-1, "frame_end_%d_%d", egg->lang.nfunctions, egg->lang.nbrackets);
	return label;
}