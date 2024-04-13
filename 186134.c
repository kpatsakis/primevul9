static void __init of_unittest_changeset(void)
{
#ifdef CONFIG_OF_DYNAMIC
	struct property *ppadd, padd = { .name = "prop-add", .length = 1, .value = "" };
	struct property *ppname_n1,  pname_n1  = { .name = "name", .length = 3, .value = "n1"  };
	struct property *ppname_n2,  pname_n2  = { .name = "name", .length = 3, .value = "n2"  };
	struct property *ppname_n21, pname_n21 = { .name = "name", .length = 3, .value = "n21" };
	struct property *ppupdate, pupdate = { .name = "prop-update", .length = 5, .value = "abcd" };
	struct property *ppremove;
	struct device_node *n1, *n2, *n21, *nchangeset, *nremove, *parent, *np;
	struct of_changeset chgset;

	n1 = __of_node_dup(NULL, "n1");
	unittest(n1, "testcase setup failure\n");

	n2 = __of_node_dup(NULL, "n2");
	unittest(n2, "testcase setup failure\n");

	n21 = __of_node_dup(NULL, "n21");
	unittest(n21, "testcase setup failure %p\n", n21);

	nchangeset = of_find_node_by_path("/testcase-data/changeset");
	nremove = of_get_child_by_name(nchangeset, "node-remove");
	unittest(nremove, "testcase setup failure\n");

	ppadd = __of_prop_dup(&padd, GFP_KERNEL);
	unittest(ppadd, "testcase setup failure\n");

	ppname_n1  = __of_prop_dup(&pname_n1, GFP_KERNEL);
	unittest(ppname_n1, "testcase setup failure\n");

	ppname_n2  = __of_prop_dup(&pname_n2, GFP_KERNEL);
	unittest(ppname_n2, "testcase setup failure\n");

	ppname_n21 = __of_prop_dup(&pname_n21, GFP_KERNEL);
	unittest(ppname_n21, "testcase setup failure\n");

	ppupdate = __of_prop_dup(&pupdate, GFP_KERNEL);
	unittest(ppupdate, "testcase setup failure\n");

	parent = nchangeset;
	n1->parent = parent;
	n2->parent = parent;
	n21->parent = n2;

	ppremove = of_find_property(parent, "prop-remove", NULL);
	unittest(ppremove, "failed to find removal prop");

	of_changeset_init(&chgset);

	unittest(!of_changeset_attach_node(&chgset, n1), "fail attach n1\n");
	unittest(!of_changeset_add_property(&chgset, n1, ppname_n1), "fail add prop name\n");

	unittest(!of_changeset_attach_node(&chgset, n2), "fail attach n2\n");
	unittest(!of_changeset_add_property(&chgset, n2, ppname_n2), "fail add prop name\n");

	unittest(!of_changeset_detach_node(&chgset, nremove), "fail remove node\n");
	unittest(!of_changeset_add_property(&chgset, n21, ppname_n21), "fail add prop name\n");

	unittest(!of_changeset_attach_node(&chgset, n21), "fail attach n21\n");

	unittest(!of_changeset_add_property(&chgset, parent, ppadd), "fail add prop prop-add\n");
	unittest(!of_changeset_update_property(&chgset, parent, ppupdate), "fail update prop\n");
	unittest(!of_changeset_remove_property(&chgset, parent, ppremove), "fail remove prop\n");

	unittest(!of_changeset_apply(&chgset), "apply failed\n");

	of_node_put(nchangeset);

	/* Make sure node names are constructed correctly */
	unittest((np = of_find_node_by_path("/testcase-data/changeset/n2/n21")),
		 "'%pOF' not added\n", n21);
	of_node_put(np);

	unittest(!of_changeset_revert(&chgset), "revert failed\n");

	of_changeset_destroy(&chgset);
#endif
}