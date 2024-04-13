ptp_free_DI (PTPDeviceInfo *di) {
	free (di->SerialNumber);
	free (di->DeviceVersion);
	free (di->Model);
	free (di->Manufacturer);
	free (di->ImageFormats);
	free (di->CaptureFormats);
	free (di->VendorExtensionDesc);
	free (di->OperationsSupported);
	free (di->EventsSupported);
	free (di->DevicePropertiesSupported);
	memset(di, 0, sizeof(*di));
}