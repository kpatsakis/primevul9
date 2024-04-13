static void __init of_unittest_parse_phandle_with_args_map(void)
{
	struct device_node *np, *p0, *p1, *p2, *p3;
	struct of_phandle_args args;
	int i, rc;

	np = of_find_node_by_path("/testcase-data/phandle-tests/consumer-b");
	if (!np) {
		pr_err("missing testcase data\n");
		return;
	}

	p0 = of_find_node_by_path("/testcase-data/phandle-tests/provider0");
	if (!p0) {
		pr_err("missing testcase data\n");
		return;
	}

	p1 = of_find_node_by_path("/testcase-data/phandle-tests/provider1");
	if (!p1) {
		pr_err("missing testcase data\n");
		return;
	}

	p2 = of_find_node_by_path("/testcase-data/phandle-tests/provider2");
	if (!p2) {
		pr_err("missing testcase data\n");
		return;
	}

	p3 = of_find_node_by_path("/testcase-data/phandle-tests/provider3");
	if (!p3) {
		pr_err("missing testcase data\n");
		return;
	}

	rc = of_count_phandle_with_args(np, "phandle-list", "#phandle-cells");
	unittest(rc == 7, "of_count_phandle_with_args() returned %i, expected 7\n", rc);

	for (i = 0; i < 8; i++) {
		bool passed = true;

		memset(&args, 0, sizeof(args));
		rc = of_parse_phandle_with_args_map(np, "phandle-list",
						    "phandle", i, &args);

		/* Test the values from tests-phandle.dtsi */
		switch (i) {
		case 0:
			passed &= !rc;
			passed &= (args.np == p1);
			passed &= (args.args_count == 1);
			passed &= (args.args[0] == 1);
			break;
		case 1:
			passed &= !rc;
			passed &= (args.np == p3);
			passed &= (args.args_count == 3);
			passed &= (args.args[0] == 2);
			passed &= (args.args[1] == 5);
			passed &= (args.args[2] == 3);
			break;
		case 2:
			passed &= (rc == -ENOENT);
			break;
		case 3:
			passed &= !rc;
			passed &= (args.np == p0);
			passed &= (args.args_count == 0);
			break;
		case 4:
			passed &= !rc;
			passed &= (args.np == p1);
			passed &= (args.args_count == 1);
			passed &= (args.args[0] == 3);
			break;
		case 5:
			passed &= !rc;
			passed &= (args.np == p0);
			passed &= (args.args_count == 0);
			break;
		case 6:
			passed &= !rc;
			passed &= (args.np == p2);
			passed &= (args.args_count == 2);
			passed &= (args.args[0] == 15);
			passed &= (args.args[1] == 0x20);
			break;
		case 7:
			passed &= (rc == -ENOENT);
			break;
		default:
			passed = false;
		}

		unittest(passed, "index %i - data error on node %s rc=%i\n",
			 i, args.np->full_name, rc);
	}

	/* Check for missing list property */
	memset(&args, 0, sizeof(args));
	rc = of_parse_phandle_with_args_map(np, "phandle-list-missing",
					    "phandle", 0, &args);
	unittest(rc == -ENOENT, "expected:%i got:%i\n", -ENOENT, rc);

	/* Check for missing cells,map,mask property */
	memset(&args, 0, sizeof(args));
	rc = of_parse_phandle_with_args_map(np, "phandle-list",
					    "phandle-missing", 0, &args);
	unittest(rc == -EINVAL, "expected:%i got:%i\n", -EINVAL, rc);

	/* Check for bad phandle in list */
	memset(&args, 0, sizeof(args));
	rc = of_parse_phandle_with_args_map(np, "phandle-list-bad-phandle",
					    "phandle", 0, &args);
	unittest(rc == -EINVAL, "expected:%i got:%i\n", -EINVAL, rc);

	/* Check for incorrectly formed argument list */
	memset(&args, 0, sizeof(args));
	rc = of_parse_phandle_with_args_map(np, "phandle-list-bad-args",
					    "phandle", 1, &args);
	unittest(rc == -EINVAL, "expected:%i got:%i\n", -EINVAL, rc);
}