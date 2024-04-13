int af_alg_unregister_type(const struct af_alg_type *type)
{
	struct alg_type_list *node;
	int err = -ENOENT;

	down_write(&alg_types_sem);
	list_for_each_entry(node, &alg_types, list) {
		if (strcmp(node->type->name, type->name))
			continue;

		list_del(&node->list);
		kfree(node);
		err = 0;
		break;
	}
	up_write(&alg_types_sem);

	return err;
}