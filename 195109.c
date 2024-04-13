FindUdfFileSystem (
  IN EFI_BLOCK_IO_PROTOCOL  *BlockIo,
  IN EFI_DISK_IO_PROTOCOL   *DiskIo,
  OUT EFI_LBA               *StartingLBA,
  OUT EFI_LBA               *EndingLBA
  )
{
  EFI_STATUS                            Status;
  UDF_ANCHOR_VOLUME_DESCRIPTOR_POINTER  AnchorPoint;
  EFI_LBA                               LastRecordedBlock;

  //
  // Find UDF volume identifiers
  //
  Status = FindUdfVolumeIdentifiers (BlockIo, DiskIo);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Find Anchor Volume Descriptor Pointer
  //
  Status = FindAnchorVolumeDescriptorPointer (
    BlockIo,
    DiskIo,
    &AnchorPoint,
    &LastRecordedBlock
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Find Logical Volume location
  //
  Status = FindLogicalVolumeLocation (
    BlockIo,
    DiskIo,
    &AnchorPoint,
    LastRecordedBlock,
    (UINT64 *)StartingLBA,
    (UINT64 *)EndingLBA
    );

  return Status;
}