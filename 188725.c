static void i40e_free_misc_vector(struct i40e_pf *pf)
{
	/* Disable ICR 0 */
	wr32(&pf->hw, I40E_PFINT_ICR0_ENA, 0);
	i40e_flush(&pf->hw);

	if (pf->flags & I40E_FLAG_MSIX_ENABLED && pf->msix_entries) {
		synchronize_irq(pf->msix_entries[0].vector);
		free_irq(pf->msix_entries[0].vector, pf);
		clear_bit(__I40E_MISC_IRQ_REQUESTED, pf->state);
	}
}