static int sony_suspend(struct hid_device *hdev, pm_message_t message)
{
#ifdef CONFIG_SONY_FF

	/* On suspend stop any running force-feedback events */
	if (SONY_FF_SUPPORT) {
		struct sony_sc *sc = hid_get_drvdata(hdev);

		sc->left = sc->right = 0;
		sony_send_output_report(sc);
	}

#endif
	return 0;
}