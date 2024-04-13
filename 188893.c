static void i40e_verify_eeprom(struct i40e_pf *pf)
{
	int err;

	err = i40e_diag_eeprom_test(&pf->hw);
	if (err) {
		/* retry in case of garbage read */
		err = i40e_diag_eeprom_test(&pf->hw);
		if (err) {
			dev_info(&pf->pdev->dev, "eeprom check failed (%d), Tx/Rx traffic disabled\n",
				 err);
			set_bit(__I40E_BAD_EEPROM, pf->state);
		}
	}

	if (!err && test_bit(__I40E_BAD_EEPROM, pf->state)) {
		dev_info(&pf->pdev->dev, "eeprom check passed, Tx/Rx traffic enabled\n");
		clear_bit(__I40E_BAD_EEPROM, pf->state);
	}
}