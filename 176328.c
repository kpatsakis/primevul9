static void add_object(struct i915_mmu_object *mo)
{
	GEM_BUG_ON(!RB_EMPTY_NODE(&mo->it.rb));
	interval_tree_insert(&mo->it, &mo->mn->objects);
}