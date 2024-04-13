PartitionInstallUdfChildHandles (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Handle,
  IN  EFI_DISK_IO_PROTOCOL         *DiskIo,
  IN  EFI_DISK_IO2_PROTOCOL        *DiskIo2,
  IN  EFI_BLOCK_IO_PROTOCOL        *BlockIo,
  IN  EFI_BLOCK_IO2_PROTOCOL       *BlockIo2,
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  UINT32                       RemainderByMediaBlockSize;
  EFI_STATUS                   Status;
  EFI_BLOCK_IO_MEDIA           *Media;
  EFI_PARTITION_INFO_PROTOCOL  PartitionInfo;
  EFI_LBA                      StartingLBA;
  EFI_LBA                      EndingLBA;
  BOOLEAN                      ChildCreated;

  Media = BlockIo->Media;
  ChildCreated = FALSE;

  //
  // Check if UDF logical block size is multiple of underlying device block size
  //
  DivU64x32Remainder (
    UDF_LOGICAL_SECTOR_SIZE,   // Dividend
    Media->BlockSize,          // Divisor
    &RemainderByMediaBlockSize // Remainder
    );
  if (RemainderByMediaBlockSize != 0) {
    return EFI_NOT_FOUND;
  }

  //
  // Detect El Torito feature first.
  // And always continue to search for UDF.
  //
  Status = PartitionInstallElToritoChildHandles (
             This,
             Handle,
             DiskIo,
             DiskIo2,
             BlockIo,
             BlockIo2,
             DevicePath
             );
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "PartitionDxe: El Torito standard found on handle 0x%p.\n", Handle));
    ChildCreated = TRUE;
  }

  //
  // Search for an UDF file system on block device
  //
  Status = FindUdfFileSystem (BlockIo, DiskIo, &StartingLBA, &EndingLBA);
  if (EFI_ERROR (Status)) {
    return (ChildCreated ? EFI_SUCCESS : EFI_NOT_FOUND);
  }

  //
  // Create Partition Info protocol for UDF file system
  //
  ZeroMem (&PartitionInfo, sizeof (EFI_PARTITION_INFO_PROTOCOL));
  PartitionInfo.Revision = EFI_PARTITION_INFO_PROTOCOL_REVISION;
  PartitionInfo.Type = PARTITION_TYPE_OTHER;

  //
  // Install partition child handle for UDF file system
  //
  Status = PartitionInstallChildHandle (
    This,
    Handle,
    DiskIo,
    DiskIo2,
    BlockIo,
    BlockIo2,
    DevicePath,
    (EFI_DEVICE_PATH_PROTOCOL *)&gUdfDevicePath,
    &PartitionInfo,
    StartingLBA,
    EndingLBA,
    Media->BlockSize,
    NULL
    );
  if (EFI_ERROR (Status)) {
    return (ChildCreated ? EFI_SUCCESS : Status);
  }

  return EFI_SUCCESS;
}