int i40e_free_dma_mem_d(struct i40e_hw *hw, struct i40e_dma_mem *mem)
{
	struct i40e_pf *pf = (struct i40e_pf *)hw->back;

	dma_free_coherent(&pf->pdev->dev, mem->size, mem->va, mem->pa);
	mem->va = NULL;
	mem->pa = 0;
	mem->size = 0;

	return 0;
}