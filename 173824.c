static int sony_allocate_output_report(struct sony_sc *sc)
{
	if ((sc->quirks & SIXAXIS_CONTROLLER) ||
			(sc->quirks & NAVIGATION_CONTROLLER))
		sc->output_report_dmabuf =
			devm_kmalloc(&sc->hdev->dev,
				sizeof(union sixaxis_output_report_01),
				GFP_KERNEL);
	else if (sc->quirks & DUALSHOCK4_CONTROLLER_BT)
		sc->output_report_dmabuf = devm_kmalloc(&sc->hdev->dev,
						DS4_OUTPUT_REPORT_0x11_SIZE,
						GFP_KERNEL);
	else if (sc->quirks & (DUALSHOCK4_CONTROLLER_USB | DUALSHOCK4_DONGLE))
		sc->output_report_dmabuf = devm_kmalloc(&sc->hdev->dev,
						DS4_OUTPUT_REPORT_0x05_SIZE,
						GFP_KERNEL);
	else if (sc->quirks & MOTION_CONTROLLER)
		sc->output_report_dmabuf = devm_kmalloc(&sc->hdev->dev,
						MOTION_REPORT_0x02_SIZE,
						GFP_KERNEL);
	else
		return 0;

	if (!sc->output_report_dmabuf)
		return -ENOMEM;

	return 0;
}