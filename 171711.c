int af_alg_register_type(const struct af_alg_type *type)
{
	struct alg_type_list *node;
	int err = -EEXIST;

	down_write(&alg_types_sem);
	list_for_each_entry(node, &alg_types, list) {
		if (!strcmp(node->type->name, type->name))
			goto unlock;
	}

	node = kmalloc(sizeof(*node), GFP_KERNEL);
	err = -ENOMEM;
	if (!node)
		goto unlock;

	type->ops->owner = THIS_MODULE;
	if (type->ops_nokey)
		type->ops_nokey->owner = THIS_MODULE;
	node->type = type;
	list_add(&node->list, &alg_types);
	err = 0;

unlock:
	up_write(&alg_types_sem);

	return err;
}