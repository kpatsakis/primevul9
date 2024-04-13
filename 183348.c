ptp_unpack_SI (PTPParams *params, unsigned char* data, PTPStorageInfo *si, unsigned int len)
{
	uint8_t storagedescriptionlen;

	if (len < 26) return 0;
	si->StorageType=dtoh16a(&data[PTP_si_StorageType]);
	si->FilesystemType=dtoh16a(&data[PTP_si_FilesystemType]);
	si->AccessCapability=dtoh16a(&data[PTP_si_AccessCapability]);
	si->MaxCapability=dtoh64a(&data[PTP_si_MaxCapability]);
	si->FreeSpaceInBytes=dtoh64a(&data[PTP_si_FreeSpaceInBytes]);
	si->FreeSpaceInImages=dtoh32a(&data[PTP_si_FreeSpaceInImages]);

	/* FIXME: check more lengths here */
	si->StorageDescription=ptp_unpack_string(params, data,
		PTP_si_StorageDescription,
		len,
		&storagedescriptionlen);
	si->VolumeLabel=ptp_unpack_string(params, data,
		PTP_si_StorageDescription+storagedescriptionlen*2+1,
		len,
		&storagedescriptionlen);
	return 1;
}