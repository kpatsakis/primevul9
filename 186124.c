static void __init of_unittest_match_node(void)
{
	struct device_node *np;
	const struct of_device_id *match;
	int i;

	for (i = 0; i < ARRAY_SIZE(match_node_tests); i++) {
		np = of_find_node_by_path(match_node_tests[i].path);
		if (!np) {
			unittest(0, "missing testcase node %s\n",
				match_node_tests[i].path);
			continue;
		}

		match = of_match_node(match_node_table, np);
		if (!match) {
			unittest(0, "%s didn't match anything\n",
				match_node_tests[i].path);
			continue;
		}

		if (strcmp(match->data, match_node_tests[i].data) != 0) {
			unittest(0, "%s got wrong match. expected %s, got %s\n",
				match_node_tests[i].path, match_node_tests[i].data,
				(const char *)match->data);
			continue;
		}
		unittest(1, "passed");
	}
}