int komeda_kms_add_wb_connectors(struct komeda_kms_dev *kms,
				 struct komeda_dev *mdev)
{
	int i, err;

	for (i = 0; i < kms->n_crtcs; i++) {
		err = komeda_wb_connector_add(kms, &kms->crtcs[i]);
		if (err)
			return err;
	}

	return 0;
}