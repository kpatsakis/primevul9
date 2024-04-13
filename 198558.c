int main(int argc, char *argv[])
{
	int err = 0, r, c, long_optind = 0;
	int action_count = 0;
	int do_read_key = 0;
	int do_generate_key = 0;
	int do_create_key_files = 0;
	int do_list_keys = 0;
	int do_store_key = 0;
	int do_create_pin_file = 0;
	sc_context_param_t ctx_param;

	while (1) {
		c = getopt_long(argc, argv, "P:Vslgc:Rk:r:p:u:e:m:vwa:", options, &long_optind);
		if (c == -1)
			break;
		if (c == '?')
			util_print_usage_and_die(app_name, options, option_help, NULL);
		switch (c) {
		case 'l':
			do_list_keys = 1;
			action_count++;
			break;
		case 'P':
			do_create_pin_file = 1;
			opt_pin_num = atoi(optarg);
			action_count++;
			break;
		case 'R':
			do_read_key = 1;
			action_count++;
			break;
		case 'g':
			do_generate_key = 1;
			action_count++;
			break;
		case 'c':
			do_create_key_files = 1;
			opt_key_count = atoi(optarg);
			action_count++;
			break;
		case 's':
			do_store_key = 1;
			action_count++;
			break;
		case 'k':
			opt_key_num = atoi(optarg);
			if (opt_key_num < 1 || opt_key_num > 15) {
				fprintf(stderr, "Key number invalid.\n");
				exit(2);
			}
			break;
		case 'V':
			opt_pin_num = 1;
			break;
		case 'e':
			opt_exponent = atoi(optarg);
			break;
		case 'm':
			opt_mod_length = atoi(optarg);
			break;
		case 'p':
			opt_prkeyf = optarg;
			break;
		case 'u':
			opt_pubkeyf = optarg;
			break;
		case 'r':
			opt_reader = optarg;
			break;
		case 'v':
			verbose++;
			break;
		case 'w':
			opt_wait = 1;
			break;
		case 'a':
			opt_appdf = optarg;
			break;
		}
	}
	if (action_count == 0)
		util_print_usage_and_die(app_name, options, option_help, NULL);

	memset(&ctx_param, 0, sizeof(ctx_param));
	ctx_param.ver      = 0;
	ctx_param.app_name = app_name;

	r = sc_context_create(&ctx, &ctx_param);
	if (r) {
		fprintf(stderr, "Failed to establish context: %s\n", sc_strerror(r));
		return 1;
	}

	if (verbose > 1) {
		ctx->debug = verbose;
		sc_ctx_log_to_file(ctx, "stderr");
	}

	err = util_connect_card(ctx, &card, opt_reader, opt_wait, verbose);
	printf("Using card driver: %s\n", card->driver->name);

	if (do_create_pin_file) {
		if ((err = create_pin()) != 0)
			goto end;
		action_count--;
	}
	if (do_create_key_files) {
		if ((err = create_key_files()) != 0)
			goto end;
		action_count--;
	}
	if (do_generate_key) {
		if ((err = generate_key()) != 0)
			goto end;
		action_count--;
	}
	if (do_store_key) {
		if ((err = store_key()) != 0)
			goto end;
		action_count--;
	}
	if (do_list_keys) {
		if ((err = list_keys()) != 0)
			goto end;
		action_count--;
	}
	if (do_read_key) {
		if ((err = read_key()) != 0)
			goto end;
		action_count--;
	}
	if (pincode != NULL) {
		memset(pincode, 0, 8);
		free(pincode);
	}
end:
	if (card) {
		sc_unlock(card);
		sc_disconnect_card(card);
	}
	if (ctx)
		sc_release_context(ctx);
	return err;
}