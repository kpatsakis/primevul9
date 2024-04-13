static void __init of_unittest_parse_phandle_with_args(void)
{
	struct device_node *np;
	struct of_phandle_args args;
	int i, rc;

	np = of_find_node_by_path("/testcase-data/phandle-tests/consumer-a");
	if (!np) {
		pr_err("missing testcase data\n");
		return;
	}

	rc = of_count_phandle_with_args(np, "phandle-list", "#phandle-cells");
	unittest(rc == 7, "of_count_phandle_with_args() returned %i, expected 7\n", rc);

	for (i = 0; i < 8; i++) {
		bool passed = true;

		memset(&args, 0, sizeof(args));
		rc = of_parse_phandle_with_args(np, "phandle-list",
						"#phandle-cells", i, &args);

		/* Test the values from tests-phandle.dtsi */
		switch (i) {
		case 0:
			passed &= !rc;
			passed &= (args.args_count == 1);
			passed &= (args.args[0] == (i + 1));
			break;
		case 1:
			passed &= !rc;
			passed &= (args.args_count == 2);
			passed &= (args.args[0] == (i + 1));
			passed &= (args.args[1] == 0);
			break;
		case 2:
			passed &= (rc == -ENOENT);
			break;
		case 3:
			passed &= !rc;
			passed &= (args.args_count == 3);
			passed &= (args.args[0] == (i + 1));
			passed &= (args.args[1] == 4);
			passed &= (args.args[2] == 3);
			break;
		case 4:
			passed &= !rc;
			passed &= (args.args_count == 2);
			passed &= (args.args[0] == (i + 1));
			passed &= (args.args[1] == 100);
			break;
		case 5:
			passed &= !rc;
			passed &= (args.args_count == 0);
			break;
		case 6:
			passed &= !rc;
			passed &= (args.args_count == 1);
			passed &= (args.args[0] == (i + 1));
			break;
		case 7:
			passed &= (rc == -ENOENT);
			break;
		default:
			passed = false;
		}

		unittest(passed, "index %i - data error on node %pOF rc=%i\n",
			 i, args.np, rc);
	}

	/* Check for missing list property */
	memset(&args, 0, sizeof(args));
	rc = of_parse_phandle_with_args(np, "phandle-list-missing",
					"#phandle-cells", 0, &args);
	unittest(rc == -ENOENT, "expected:%i got:%i\n", -ENOENT, rc);
	rc = of_count_phandle_with_args(np, "phandle-list-missing",
					"#phandle-cells");
	unittest(rc == -ENOENT, "expected:%i got:%i\n", -ENOENT, rc);

	/* Check for missing cells property */
	memset(&args, 0, sizeof(args));
	rc = of_parse_phandle_with_args(np, "phandle-list",
					"#phandle-cells-missing", 0, &args);
	unittest(rc == -EINVAL, "expected:%i got:%i\n", -EINVAL, rc);
	rc = of_count_phandle_with_args(np, "phandle-list",
					"#phandle-cells-missing");
	unittest(rc == -EINVAL, "expected:%i got:%i\n", -EINVAL, rc);

	/* Check for bad phandle in list */
	memset(&args, 0, sizeof(args));
	rc = of_parse_phandle_with_args(np, "phandle-list-bad-phandle",
					"#phandle-cells", 0, &args);
	unittest(rc == -EINVAL, "expected:%i got:%i\n", -EINVAL, rc);
	rc = of_count_phandle_with_args(np, "phandle-list-bad-phandle",
					"#phandle-cells");
	unittest(rc == -EINVAL, "expected:%i got:%i\n", -EINVAL, rc);

	/* Check for incorrectly formed argument list */
	memset(&args, 0, sizeof(args));
	rc = of_parse_phandle_with_args(np, "phandle-list-bad-args",
					"#phandle-cells", 1, &args);
	unittest(rc == -EINVAL, "expected:%i got:%i\n", -EINVAL, rc);
	rc = of_count_phandle_with_args(np, "phandle-list-bad-args",
					"#phandle-cells");
	unittest(rc == -EINVAL, "expected:%i got:%i\n", -EINVAL, rc);
}