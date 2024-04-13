static void __init of_unittest_find_node_by_name(void)
{
	struct device_node *np;
	const char *options, *name;

	np = of_find_node_by_path("/testcase-data");
	name = kasprintf(GFP_KERNEL, "%pOF", np);
	unittest(np && !strcmp("/testcase-data", name),
		"find /testcase-data failed\n");
	of_node_put(np);
	kfree(name);

	/* Test if trailing '/' works */
	np = of_find_node_by_path("/testcase-data/");
	unittest(!np, "trailing '/' on /testcase-data/ should fail\n");

	np = of_find_node_by_path("/testcase-data/phandle-tests/consumer-a");
	name = kasprintf(GFP_KERNEL, "%pOF", np);
	unittest(np && !strcmp("/testcase-data/phandle-tests/consumer-a", name),
		"find /testcase-data/phandle-tests/consumer-a failed\n");
	of_node_put(np);
	kfree(name);

	np = of_find_node_by_path("testcase-alias");
	name = kasprintf(GFP_KERNEL, "%pOF", np);
	unittest(np && !strcmp("/testcase-data", name),
		"find testcase-alias failed\n");
	of_node_put(np);
	kfree(name);

	/* Test if trailing '/' works on aliases */
	np = of_find_node_by_path("testcase-alias/");
	unittest(!np, "trailing '/' on testcase-alias/ should fail\n");

	np = of_find_node_by_path("testcase-alias/phandle-tests/consumer-a");
	name = kasprintf(GFP_KERNEL, "%pOF", np);
	unittest(np && !strcmp("/testcase-data/phandle-tests/consumer-a", name),
		"find testcase-alias/phandle-tests/consumer-a failed\n");
	of_node_put(np);
	kfree(name);

	np = of_find_node_by_path("/testcase-data/missing-path");
	unittest(!np, "non-existent path returned node %pOF\n", np);
	of_node_put(np);

	np = of_find_node_by_path("missing-alias");
	unittest(!np, "non-existent alias returned node %pOF\n", np);
	of_node_put(np);

	np = of_find_node_by_path("testcase-alias/missing-path");
	unittest(!np, "non-existent alias with relative path returned node %pOF\n", np);
	of_node_put(np);

	np = of_find_node_opts_by_path("/testcase-data:testoption", &options);
	unittest(np && !strcmp("testoption", options),
		 "option path test failed\n");
	of_node_put(np);

	np = of_find_node_opts_by_path("/testcase-data:test/option", &options);
	unittest(np && !strcmp("test/option", options),
		 "option path test, subcase #1 failed\n");
	of_node_put(np);

	np = of_find_node_opts_by_path("/testcase-data/testcase-device1:test/option", &options);
	unittest(np && !strcmp("test/option", options),
		 "option path test, subcase #2 failed\n");
	of_node_put(np);

	np = of_find_node_opts_by_path("/testcase-data:testoption", NULL);
	unittest(np, "NULL option path test failed\n");
	of_node_put(np);

	np = of_find_node_opts_by_path("testcase-alias:testaliasoption",
				       &options);
	unittest(np && !strcmp("testaliasoption", options),
		 "option alias path test failed\n");
	of_node_put(np);

	np = of_find_node_opts_by_path("testcase-alias:test/alias/option",
				       &options);
	unittest(np && !strcmp("test/alias/option", options),
		 "option alias path test, subcase #1 failed\n");
	of_node_put(np);

	np = of_find_node_opts_by_path("testcase-alias:testaliasoption", NULL);
	unittest(np, "NULL option alias path test failed\n");
	of_node_put(np);

	options = "testoption";
	np = of_find_node_opts_by_path("testcase-alias", &options);
	unittest(np && !options, "option clearing test failed\n");
	of_node_put(np);

	options = "testoption";
	np = of_find_node_opts_by_path("/", &options);
	unittest(np && !options, "option clearing root node test failed\n");
	of_node_put(np);
}