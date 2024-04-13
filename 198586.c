main (int argc, char **argv)
{
	struct gengetopt_args_info cmdline;
	struct sc_path path;
	struct sc_context *ctx;
	struct sc_reader *reader = NULL;
	struct sc_card *card;
	unsigned char *data = NULL;
	size_t data_len = 0;
	int r;

	if (cmdline_parser(argc, argv, &cmdline) != 0)
		exit(1);

	r = initialize(cmdline.reader_arg, cmdline.verbose_given, &ctx, &reader);
	if (r < 0) {
		fprintf(stderr, "Can't initialize reader\n");
		exit(1);
	}

	if (sc_connect_card(reader, &card) < 0) {
		fprintf(stderr, "Could not connect to card\n");
		sc_release_context(ctx);
		exit(1);
	}

	sc_path_set(&path, SC_PATH_TYPE_DF_NAME, aid_hca, sizeof aid_hca, 0, 0);
	if (SC_SUCCESS != sc_select_file(card, &path, NULL))
		goto err;

	if (cmdline.pd_flag
			&& read_file(card, "D001", &data, &data_len)
			&& data_len >= 2) {
		size_t len_pd = (data[0] << 8) | data[1];

		if (len_pd + 2 <= data_len) {
			unsigned char uncompressed[1024];
			size_t uncompressed_len = sizeof uncompressed;

			if (uncompress_gzip(uncompressed, &uncompressed_len,
						data + 2, len_pd) == SC_SUCCESS) {
				dump_binary(uncompressed, uncompressed_len);
			} else {
				dump_binary(data + 2, len_pd);
			}
		}
	}

	if ((cmdline.vd_flag || cmdline.gvd_flag)
			&& read_file(card, "D001", &data, &data_len)
			&& data_len >= 8) {
		size_t off_vd  = (data[0] << 8) | data[1];
		size_t end_vd  = (data[2] << 8) | data[3];
		size_t off_gvd = (data[4] << 8) | data[5];
		size_t end_gvd = (data[6] << 8) | data[7];
		size_t len_vd = end_vd - off_vd + 1;
		size_t len_gvd = end_gvd - off_gvd + 1;

		if (off_vd <= end_vd && end_vd < data_len
				&& off_gvd <= end_gvd && end_gvd < data_len) {
			unsigned char uncompressed[1024];
			size_t uncompressed_len = sizeof uncompressed;

			if (cmdline.vd_flag) {
				if (uncompress_gzip(uncompressed, &uncompressed_len,
							data + off_vd, len_vd) == SC_SUCCESS) {
					dump_binary(uncompressed, uncompressed_len);
				} else {
					dump_binary(data + off_vd, len_vd);
				}
			}

			if (cmdline.gvd_flag) {
				if (uncompress_gzip(uncompressed, &uncompressed_len,
							data + off_gvd, len_gvd) == SC_SUCCESS) {
					dump_binary(uncompressed, uncompressed_len);
				} else {
					dump_binary(data + off_gvd, len_gvd);
				}
			}
		}
	}

	if (cmdline.vsd_status_flag
			&& read_file(card, "D00C", &data, &data_len)
			&& data_len >= 25) {
		char *status;
		unsigned int major, minor, fix;

		switch (data[0]) {
			case '0':
				status = "Transactions pending";
				break;
			case '1':
				status = "No transactions pending";
				break;
			default:
				status = "Unknown";
				break;
		}

		decode_version(data+15, &major, &minor, &fix);

		printf(
				"Status      %s\n"
				"Timestamp   %c%c.%c%c.%c%c%c%c at %c%c:%c%c:%c%c\n"
				"Version     %u.%u.%u\n",
				status,
				PRINT(data[7]), PRINT(data[8]),
				PRINT(data[5]), PRINT(data[6]),
				PRINT(data[1]), PRINT(data[2]), PRINT(data[3]), PRINT(data[4]),
				PRINT(data[9]), PRINT(data[10]),
				PRINT(data[11]), PRINT(data[12]),
				PRINT(data[13]), PRINT(data[14]),
				major, minor, fix);
	}

err:
	sc_disconnect_card(card);
	sc_release_context(ctx);
	cmdline_parser_free (&cmdline);

	return 0;
}