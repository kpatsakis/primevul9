int i40e_allocate_dma_mem_d(struct i40e_hw *hw, struct i40e_dma_mem *mem,
			    u64 size, u32 alignment)
{
	struct i40e_pf *pf = (struct i40e_pf *)hw->back;

	mem->size = ALIGN(size, alignment);
	mem->va = dma_alloc_coherent(&pf->pdev->dev, mem->size, &mem->pa,
				     GFP_KERNEL);
	if (!mem->va)
		return -ENOMEM;

	return 0;
}