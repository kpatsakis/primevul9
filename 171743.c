static void alg_do_release(const struct af_alg_type *type, void *private)
{
	if (!type)
		return;

	type->release(private);
	module_put(type->owner);
}