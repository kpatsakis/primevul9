static void free_voice_opt(struct voice_opt_s *opt)
{
	struct voice_opt_s *opt2;

	while (opt) {
		opt2 = opt->next;
		free(opt);
		opt = opt2;
	}
}