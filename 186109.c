static void __init of_unittest_property_string(void)
{
	const char *strings[4];
	struct device_node *np;
	int rc;

	np = of_find_node_by_path("/testcase-data/phandle-tests/consumer-a");
	if (!np) {
		pr_err("No testcase data in device tree\n");
		return;
	}

	rc = of_property_match_string(np, "phandle-list-names", "first");
	unittest(rc == 0, "first expected:0 got:%i\n", rc);
	rc = of_property_match_string(np, "phandle-list-names", "second");
	unittest(rc == 1, "second expected:1 got:%i\n", rc);
	rc = of_property_match_string(np, "phandle-list-names", "third");
	unittest(rc == 2, "third expected:2 got:%i\n", rc);
	rc = of_property_match_string(np, "phandle-list-names", "fourth");
	unittest(rc == -ENODATA, "unmatched string; rc=%i\n", rc);
	rc = of_property_match_string(np, "missing-property", "blah");
	unittest(rc == -EINVAL, "missing property; rc=%i\n", rc);
	rc = of_property_match_string(np, "empty-property", "blah");
	unittest(rc == -ENODATA, "empty property; rc=%i\n", rc);
	rc = of_property_match_string(np, "unterminated-string", "blah");
	unittest(rc == -EILSEQ, "unterminated string; rc=%i\n", rc);

	/* of_property_count_strings() tests */
	rc = of_property_count_strings(np, "string-property");
	unittest(rc == 1, "Incorrect string count; rc=%i\n", rc);
	rc = of_property_count_strings(np, "phandle-list-names");
	unittest(rc == 3, "Incorrect string count; rc=%i\n", rc);
	rc = of_property_count_strings(np, "unterminated-string");
	unittest(rc == -EILSEQ, "unterminated string; rc=%i\n", rc);
	rc = of_property_count_strings(np, "unterminated-string-list");
	unittest(rc == -EILSEQ, "unterminated string array; rc=%i\n", rc);

	/* of_property_read_string_index() tests */
	rc = of_property_read_string_index(np, "string-property", 0, strings);
	unittest(rc == 0 && !strcmp(strings[0], "foobar"), "of_property_read_string_index() failure; rc=%i\n", rc);
	strings[0] = NULL;
	rc = of_property_read_string_index(np, "string-property", 1, strings);
	unittest(rc == -ENODATA && strings[0] == NULL, "of_property_read_string_index() failure; rc=%i\n", rc);
	rc = of_property_read_string_index(np, "phandle-list-names", 0, strings);
	unittest(rc == 0 && !strcmp(strings[0], "first"), "of_property_read_string_index() failure; rc=%i\n", rc);
	rc = of_property_read_string_index(np, "phandle-list-names", 1, strings);
	unittest(rc == 0 && !strcmp(strings[0], "second"), "of_property_read_string_index() failure; rc=%i\n", rc);
	rc = of_property_read_string_index(np, "phandle-list-names", 2, strings);
	unittest(rc == 0 && !strcmp(strings[0], "third"), "of_property_read_string_index() failure; rc=%i\n", rc);
	strings[0] = NULL;
	rc = of_property_read_string_index(np, "phandle-list-names", 3, strings);
	unittest(rc == -ENODATA && strings[0] == NULL, "of_property_read_string_index() failure; rc=%i\n", rc);
	strings[0] = NULL;
	rc = of_property_read_string_index(np, "unterminated-string", 0, strings);
	unittest(rc == -EILSEQ && strings[0] == NULL, "of_property_read_string_index() failure; rc=%i\n", rc);
	rc = of_property_read_string_index(np, "unterminated-string-list", 0, strings);
	unittest(rc == 0 && !strcmp(strings[0], "first"), "of_property_read_string_index() failure; rc=%i\n", rc);
	strings[0] = NULL;
	rc = of_property_read_string_index(np, "unterminated-string-list", 2, strings); /* should fail */
	unittest(rc == -EILSEQ && strings[0] == NULL, "of_property_read_string_index() failure; rc=%i\n", rc);
	strings[1] = NULL;

	/* of_property_read_string_array() tests */
	rc = of_property_read_string_array(np, "string-property", strings, 4);
	unittest(rc == 1, "Incorrect string count; rc=%i\n", rc);
	rc = of_property_read_string_array(np, "phandle-list-names", strings, 4);
	unittest(rc == 3, "Incorrect string count; rc=%i\n", rc);
	rc = of_property_read_string_array(np, "unterminated-string", strings, 4);
	unittest(rc == -EILSEQ, "unterminated string; rc=%i\n", rc);
	/* -- An incorrectly formed string should cause a failure */
	rc = of_property_read_string_array(np, "unterminated-string-list", strings, 4);
	unittest(rc == -EILSEQ, "unterminated string array; rc=%i\n", rc);
	/* -- parsing the correctly formed strings should still work: */
	strings[2] = NULL;
	rc = of_property_read_string_array(np, "unterminated-string-list", strings, 2);
	unittest(rc == 2 && strings[2] == NULL, "of_property_read_string_array() failure; rc=%i\n", rc);
	strings[1] = NULL;
	rc = of_property_read_string_array(np, "phandle-list-names", strings, 1);
	unittest(rc == 1 && strings[1] == NULL, "Overwrote end of string array; rc=%i, str='%s'\n", rc, strings[1]);
}