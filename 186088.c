static void attach_node_and_children(struct device_node *np)
{
	struct device_node *next, *dup, *child;
	unsigned long flags;
	const char *full_name;

	full_name = kasprintf(GFP_KERNEL, "%pOF", np);

	if (!strcmp(full_name, "/__local_fixups__") ||
	    !strcmp(full_name, "/__fixups__"))
		return;

	dup = of_find_node_by_path(full_name);
	kfree(full_name);
	if (dup) {
		update_node_properties(np, dup);
		return;
	}

	child = np->child;
	np->child = NULL;

	mutex_lock(&of_mutex);
	raw_spin_lock_irqsave(&devtree_lock, flags);
	np->sibling = np->parent->child;
	np->parent->child = np;
	of_node_clear_flag(np, OF_DETACHED);
	raw_spin_unlock_irqrestore(&devtree_lock, flags);

	__of_attach_node_sysfs(np);
	mutex_unlock(&of_mutex);

	while (child) {
		next = child->sibling;
		attach_node_and_children(child);
		child = next;
	}
}