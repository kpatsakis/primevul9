static int i40e_vsi_request_irq(struct i40e_vsi *vsi, char *basename)
{
	struct i40e_pf *pf = vsi->back;
	int err;

	if (pf->flags & I40E_FLAG_MSIX_ENABLED)
		err = i40e_vsi_request_irq_msix(vsi, basename);
	else if (pf->flags & I40E_FLAG_MSI_ENABLED)
		err = request_irq(pf->pdev->irq, i40e_intr, 0,
				  pf->int_name, pf);
	else
		err = request_irq(pf->pdev->irq, i40e_intr, IRQF_SHARED,
				  pf->int_name, pf);

	if (err)
		dev_info(&pf->pdev->dev, "request_irq failed, Error %d\n", err);

	return err;
}