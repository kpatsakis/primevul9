FindUdfVolumeIdentifiers (
  IN EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  IN EFI_DISK_IO_PROTOCOL   *DiskIo
  )
{
  EFI_STATUS                            Status;
  UINT64                                Offset;
  UINT64                                EndDiskOffset;
  CDROM_VOLUME_DESCRIPTOR               VolDescriptor;
  CDROM_VOLUME_DESCRIPTOR               TerminatingVolDescriptor;

  ZeroMem ((VOID *)&TerminatingVolDescriptor, sizeof (CDROM_VOLUME_DESCRIPTOR));

  //
  // Start Volume Recognition Sequence
  //
  EndDiskOffset = MultU64x32 (BlockIo->Media->LastBlock,
                              BlockIo->Media->BlockSize);

  for (Offset = UDF_VRS_START_OFFSET; Offset < EndDiskOffset;
       Offset += UDF_LOGICAL_SECTOR_SIZE) {
    //
    // Check if block device has a Volume Structure Descriptor and an Extended
    // Area.
    //
    Status = DiskIo->ReadDisk (
      DiskIo,
      BlockIo->Media->MediaId,
      Offset,
      sizeof (CDROM_VOLUME_DESCRIPTOR),
      (VOID *)&VolDescriptor
      );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (CompareMem ((VOID *)VolDescriptor.Unknown.Id,
                    (VOID *)UDF_BEA_IDENTIFIER,
                    sizeof (VolDescriptor.Unknown.Id)) == 0) {
      break;
    }

    if ((CompareMem ((VOID *)VolDescriptor.Unknown.Id,
                     (VOID *)CDVOL_ID,
                     sizeof (VolDescriptor.Unknown.Id)) != 0) ||
        (CompareMem ((VOID *)&VolDescriptor,
                     (VOID *)&TerminatingVolDescriptor,
                     sizeof (CDROM_VOLUME_DESCRIPTOR)) == 0)) {
      return EFI_NOT_FOUND;
    }
  }

  //
  // Look for "NSR0{2,3}" identifiers in the Extended Area.
  //
  Offset += UDF_LOGICAL_SECTOR_SIZE;
  if (Offset >= EndDiskOffset) {
    return EFI_NOT_FOUND;
  }

  Status = DiskIo->ReadDisk (
    DiskIo,
    BlockIo->Media->MediaId,
    Offset,
    sizeof (CDROM_VOLUME_DESCRIPTOR),
    (VOID *)&VolDescriptor
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((CompareMem ((VOID *)VolDescriptor.Unknown.Id,
                   (VOID *)UDF_NSR2_IDENTIFIER,
                   sizeof (VolDescriptor.Unknown.Id)) != 0) &&
      (CompareMem ((VOID *)VolDescriptor.Unknown.Id,
                   (VOID *)UDF_NSR3_IDENTIFIER,
                   sizeof (VolDescriptor.Unknown.Id)) != 0)) {
    return EFI_NOT_FOUND;
  }

  //
  // Look for "TEA01" identifier in the Extended Area
  //
  Offset += UDF_LOGICAL_SECTOR_SIZE;
  if (Offset >= EndDiskOffset) {
    return EFI_NOT_FOUND;
  }

  Status = DiskIo->ReadDisk (
    DiskIo,
    BlockIo->Media->MediaId,
    Offset,
    sizeof (CDROM_VOLUME_DESCRIPTOR),
    (VOID *)&VolDescriptor
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (CompareMem ((VOID *)VolDescriptor.Unknown.Id,
                  (VOID *)UDF_TEA_IDENTIFIER,
                  sizeof (VolDescriptor.Unknown.Id)) != 0) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}