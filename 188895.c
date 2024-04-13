int i40e_free_virt_mem_d(struct i40e_hw *hw, struct i40e_virt_mem *mem)
{
	/* it's ok to kfree a NULL pointer */
	kfree(mem->va);
	mem->va = NULL;
	mem->size = 0;

	return 0;
}