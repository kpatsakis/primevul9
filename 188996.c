static int i40e_setup_misc_vector(struct i40e_pf *pf)
{
	struct i40e_hw *hw = &pf->hw;
	int err = 0;

	/* Only request the IRQ once, the first time through. */
	if (!test_and_set_bit(__I40E_MISC_IRQ_REQUESTED, pf->state)) {
		err = request_irq(pf->msix_entries[0].vector,
				  i40e_intr, 0, pf->int_name, pf);
		if (err) {
			clear_bit(__I40E_MISC_IRQ_REQUESTED, pf->state);
			dev_info(&pf->pdev->dev,
				 "request_irq for %s failed: %d\n",
				 pf->int_name, err);
			return -EFAULT;
		}
	}

	i40e_enable_misc_int_causes(pf);

	/* associate no queues to the misc vector */
	wr32(hw, I40E_PFINT_LNKLST0, I40E_QUEUE_END_OF_LIST);
	wr32(hw, I40E_PFINT_ITR0(I40E_RX_ITR), I40E_ITR_8K >> 1);

	i40e_flush(hw);

	i40e_irq_dynamic_enable_icr0(pf);

	return err;
}