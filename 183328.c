ptp_free_EOS_DI (PTPCanonEOSDeviceInfo *di)
{
	free (di->EventsSupported);
	free (di->DevicePropertiesSupported);
	free (di->unk);
}