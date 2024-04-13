static const struct af_alg_type *alg_get_type(const char *name)
{
	const struct af_alg_type *type = ERR_PTR(-ENOENT);
	struct alg_type_list *node;

	down_read(&alg_types_sem);
	list_for_each_entry(node, &alg_types, list) {
		if (strcmp(node->type->name, name))
			continue;

		if (try_module_get(node->type->owner))
			type = node->type;
		break;
	}
	up_read(&alg_types_sem);

	return type;
}