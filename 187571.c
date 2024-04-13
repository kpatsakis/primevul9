static bool dev_type_flutter(enum sas_device_type new, enum sas_device_type old)
{
	if (old == new)
		return true;

	/* treat device directed resets as flutter, if we went
	 * SAS_END_DEVICE to SAS_SATA_PENDING the link needs recovery
	 */
	if ((old == SAS_SATA_PENDING && new == SAS_END_DEVICE) ||
	    (old == SAS_END_DEVICE && new == SAS_SATA_PENDING))
		return true;

	return false;
}