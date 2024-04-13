static int i40e_setup_misc_vector_for_recovery_mode(struct i40e_pf *pf)
{
	int err;

	if (pf->flags & I40E_FLAG_MSIX_ENABLED) {
		err = i40e_setup_misc_vector(pf);

		if (err) {
			dev_info(&pf->pdev->dev,
				 "MSI-X misc vector request failed, error %d\n",
				 err);
			return err;
		}
	} else {
		u32 flags = pf->flags & I40E_FLAG_MSI_ENABLED ? 0 : IRQF_SHARED;

		err = request_irq(pf->pdev->irq, i40e_intr, flags,
				  pf->int_name, pf);

		if (err) {
			dev_info(&pf->pdev->dev,
				 "MSI/legacy misc vector request failed, error %d\n",
				 err);
			return err;
		}
		i40e_enable_misc_int_causes(pf);
		i40e_irq_dynamic_enable_icr0(pf);
	}

	return 0;
}