static void del_object(struct i915_mmu_object *mo)
{
	if (RB_EMPTY_NODE(&mo->it.rb))
		return;

	interval_tree_remove(&mo->it, &mo->mn->objects);
	RB_CLEAR_NODE(&mo->it.rb);
}