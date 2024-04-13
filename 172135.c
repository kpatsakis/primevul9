int parse_sar_opt(char *argv[], int *opt, struct activity *act[],
		  unsigned int *flags, int caller)
{
	int i, p;

	for (i = 1; *(argv[*opt] + i); i++) {
		/*
		 * Note: argv[*opt] contains something like "-BruW"
		 *     *(argv[*opt] + i) will contain 'B', 'r', etc.
		 */

		switch (*(argv[*opt] + i)) {

		case 'A':
			select_all_activities(act);
			*flags |= S_F_OPTION_A;

			/*
			 * Force '-r ALL -u ALL -F'.
			 * Setting -F is compulsory because corresponding activity
			 * has AO_MULTIPLE_OUTPUTS flag set.
			 * -P ALL / -I ALL will be set only corresponding option has
			 * not been exlicitly entered on the command line.
			 */
			p = get_activity_position(act, A_MEMORY, EXIT_IF_NOT_FOUND);
			act[p]->opt_flags |= AO_F_MEMORY + AO_F_SWAP + AO_F_MEM_ALL;

			p = get_activity_position(act, A_CPU, EXIT_IF_NOT_FOUND);
			act[p]->opt_flags = AO_F_CPU_ALL;

			p = get_activity_position(act, A_FS, EXIT_IF_NOT_FOUND);
			act[p]->opt_flags = AO_F_FILESYSTEM;
			break;

		case 'B':
			SELECT_ACTIVITY(A_PAGE);
			break;

		case 'b':
			SELECT_ACTIVITY(A_IO);
			break;

		case 'C':
			*flags |= S_F_COMMENT;
			break;

		case 'd':
			SELECT_ACTIVITY(A_DISK);
			break;

		case 'F':
			p = get_activity_position(act, A_FS, EXIT_IF_NOT_FOUND);
			act[p]->options |= AO_SELECTED;
			if (!*(argv[*opt] + i + 1) && argv[*opt + 1] && !strcmp(argv[*opt + 1], K_MOUNT)) {
				(*opt)++;
				act[p]->opt_flags |= AO_F_MOUNT;
				return 0;
			}
			else {
				act[p]->opt_flags |= AO_F_FILESYSTEM;
			}
			break;

		case 'H':
			SELECT_ACTIVITY(A_HUGE);
			break;

		case 'h':
			/*
			 * Make output easier to read by a human.
			 * Option -h implies --human and -p (pretty-print).
			 */
			*flags |= S_F_HUMAN_READ + S_F_UNIT + S_F_DEV_PRETTY;
			break;

		case 'j':
			if (!argv[*opt + 1]) {
				return 1;
			}
			(*opt)++;
			if (strnlen(argv[*opt], MAX_FILE_LEN) >= MAX_FILE_LEN - 1)
				return 1;

			strncpy(persistent_name_type, argv[*opt], MAX_FILE_LEN - 1);
			persistent_name_type[MAX_FILE_LEN - 1] = '\0';
			strtolower(persistent_name_type);
			if (!get_persistent_type_dir(persistent_name_type)) {
				fprintf(stderr, _("Invalid type of persistent device name\n"));
				return 2;
			}
			/*
			 * If persistent device name doesn't exist for device, use
			 * its pretty name.
			 */
			*flags |= S_F_PERSIST_NAME + S_F_DEV_PRETTY;
			return 0;
			break;

		case 'p':
			*flags |= S_F_DEV_PRETTY;
			break;

		case 'q':
			SELECT_ACTIVITY(A_QUEUE);
			break;

		case 'r':
			p = get_activity_position(act, A_MEMORY, EXIT_IF_NOT_FOUND);
			act[p]->options   |= AO_SELECTED;
			act[p]->opt_flags |= AO_F_MEMORY;
			if (!*(argv[*opt] + i + 1) && argv[*opt + 1] && !strcmp(argv[*opt + 1], K_ALL)) {
				(*opt)++;
				act[p]->opt_flags |= AO_F_MEM_ALL;
				return 0;
			}
			break;

		case 'S':
			p = get_activity_position(act, A_MEMORY, EXIT_IF_NOT_FOUND);
			act[p]->options   |= AO_SELECTED;
			act[p]->opt_flags |= AO_F_SWAP;
			break;

		case 't':
			/*
			 * Check sar option -t here (as it can be combined
			 * with other ones, eg. "sar -rtu ..."
			 * But sadf option -t is checked in sadf.c as it won't
			 * be entered as a sar option after "--".
			 */
			if (caller != C_SAR) {
				return 1;
			}
			*flags |= S_F_TRUE_TIME;
			break;

		case 'u':
			p = get_activity_position(act, A_CPU, EXIT_IF_NOT_FOUND);
			act[p]->options |= AO_SELECTED;
			if (!*(argv[*opt] + i + 1) && argv[*opt + 1] && !strcmp(argv[*opt + 1], K_ALL)) {
				(*opt)++;
				act[p]->opt_flags = AO_F_CPU_ALL;
				return 0;
			}
			else {
				act[p]->opt_flags = AO_F_CPU_DEF;
			}
			break;

		case 'v':
			SELECT_ACTIVITY(A_KTABLES);
			break;

		case 'w':
			SELECT_ACTIVITY(A_PCSW);
			break;

		case 'W':
			SELECT_ACTIVITY(A_SWAP);
			break;

		case 'y':
			SELECT_ACTIVITY(A_SERIAL);
			break;

		case 'z':
			*flags |= S_F_ZERO_OMIT;
			break;

		case 'V':
			print_version();
			break;

		default:
			return 1;
		}
	}
	return 0;
}