static int __init of_unittest_check_node_linkage(struct device_node *np)
{
	struct device_node *child;
	int count = 0, rc;

	for_each_child_of_node(np, child) {
		if (child->parent != np) {
			pr_err("Child node %pOFn links to wrong parent %pOFn\n",
				 child, np);
			rc = -EINVAL;
			goto put_child;
		}

		rc = of_unittest_check_node_linkage(child);
		if (rc < 0)
			goto put_child;
		count += rc;
	}

	return count + 1;
put_child:
	of_node_put(child);
	return rc;
}