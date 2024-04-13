static void update_node_properties(struct device_node *np,
					struct device_node *dup)
{
	struct property *prop;
	struct property *save_next;
	struct device_node *child;
	int ret;

	for_each_child_of_node(np, child)
		child->parent = dup;

	/*
	 * "unittest internal error: unable to add testdata property"
	 *
	 *    If this message reports a property in node '/__symbols__' then
	 *    the respective unittest overlay contains a label that has the
	 *    same name as a label in the live devicetree.  The label will
	 *    be in the live devicetree only if the devicetree source was
	 *    compiled with the '-@' option.  If you encounter this error,
	 *    please consider renaming __all__ of the labels in the unittest
	 *    overlay dts files with an odd prefix that is unlikely to be
	 *    used in a real devicetree.
	 */

	/*
	 * open code for_each_property_of_node() because of_add_property()
	 * sets prop->next to NULL
	 */
	for (prop = np->properties; prop != NULL; prop = save_next) {
		save_next = prop->next;
		ret = of_add_property(dup, prop);
		if (ret) {
			if (ret == -EEXIST && !strcmp(prop->name, "name"))
				continue;
			pr_err("unittest internal error: unable to add testdata property %pOF/%s",
			       np, prop->name);
		}
	}
}