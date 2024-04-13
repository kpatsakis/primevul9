static int mptsas_scsi_device_find(MPTSASState *s, int bus, int target,
                                   uint8_t *lun, SCSIDevice **sdev)
{
    if (bus != 0) {
        return MPI_IOCSTATUS_SCSI_INVALID_BUS;
    }

    if (target >= s->max_devices) {
        return MPI_IOCSTATUS_SCSI_INVALID_TARGETID;
    }

    *sdev = scsi_device_find(&s->bus, bus, target, lun[1]);
    if (!*sdev) {
        return MPI_IOCSTATUS_SCSI_DEVICE_NOT_THERE;
    }

    return 0;
}