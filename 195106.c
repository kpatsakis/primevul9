FindLogicalVolumeLocation (
  IN   EFI_BLOCK_IO_PROTOCOL                 *BlockIo,
  IN   EFI_DISK_IO_PROTOCOL                  *DiskIo,
  IN   UDF_ANCHOR_VOLUME_DESCRIPTOR_POINTER  *AnchorPoint,
  IN   EFI_LBA                               LastRecordedBlock,
  OUT  UINT64                                *MainVdsStartBlock,
  OUT  UINT64                                *MainVdsEndBlock
  )
{
  EFI_STATUS                     Status;
  UINT32                         BlockSize;
  UDF_EXTENT_AD                  *ExtentAd;
  UINT64                         SeqBlocksNum;
  UINT64                         SeqStartBlock;
  UINT64                         GuardMainVdsStartBlock;
  VOID                           *Buffer;
  UINT64                         SeqEndBlock;
  BOOLEAN                        StopSequence;
  UINTN                          LvdsCount;
  UDF_LOGICAL_VOLUME_DESCRIPTOR  *LogicalVolDesc;
  UDF_DESCRIPTOR_TAG             *DescriptorTag;

  BlockSize = BlockIo->Media->BlockSize;
  ExtentAd = &AnchorPoint->MainVolumeDescriptorSequenceExtent;

  //
  // UDF 2.60, 2.2.3.1 struct MainVolumeDescriptorSequenceExtent
  //
  // The Main Volume Descriptor Sequence Extent shall have a minimum length of
  // 16 logical sectors.
  //
  // Also make sure it does not exceed maximum number of blocks in the disk.
  //
  SeqBlocksNum = DivU64x32 ((UINT64)ExtentAd->ExtentLength, BlockSize);
  if (SeqBlocksNum < 16 || (EFI_LBA)SeqBlocksNum > LastRecordedBlock + 1) {
    return EFI_VOLUME_CORRUPTED;
  }

  //
  // Check for valid Volume Descriptor Sequence starting block number
  //
  SeqStartBlock = (UINT64)ExtentAd->ExtentLocation;
  if (SeqStartBlock > LastRecordedBlock ||
      SeqStartBlock + SeqBlocksNum - 1 > LastRecordedBlock) {
    return EFI_VOLUME_CORRUPTED;
  }

  GuardMainVdsStartBlock = SeqStartBlock;

  //
  // Allocate buffer for reading disk blocks
  //
  Buffer = AllocateZeroPool ((UINTN)BlockSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SeqEndBlock = SeqStartBlock + SeqBlocksNum;
  StopSequence = FALSE;
  LvdsCount = 0;
  Status = EFI_VOLUME_CORRUPTED;
  //
  // Start Main Volume Descriptor Sequence
  //
  for (; SeqStartBlock < SeqEndBlock && !StopSequence; SeqStartBlock++) {
    //
    // Read disk block
    //
    Status = BlockIo->ReadBlocks (
      BlockIo,
      BlockIo->Media->MediaId,
      SeqStartBlock,
      BlockSize,
      Buffer
      );
    if (EFI_ERROR (Status)) {
      goto Out_Free;
    }

    DescriptorTag = Buffer;

    //
    // ECMA 167, 8.4.1 Contents of a Volume Descriptor Sequence
    //
    // - A Volume Descriptor Sequence shall contain one or more Primary Volume
    //   Descriptors.
    // - A Volume Descriptor Sequence shall contain zero or more Implementation
    //   Use Volume Descriptors.
    // - A Volume Descriptor Sequence shall contain zero or more Partition
    //   Descriptors.
    // - A Volume Descriptor Sequence shall contain zero or more Logical Volume
    //   Descriptors.
    // - A Volume Descriptor Sequence shall contain zero or more Unallocated
    //   Space Descriptors.
    //
    switch (DescriptorTag->TagIdentifier) {
    case UdfPrimaryVolumeDescriptor:
    case UdfImplemenationUseVolumeDescriptor:
    case UdfPartitionDescriptor:
    case UdfUnallocatedSpaceDescriptor:
      break;

    case UdfLogicalVolumeDescriptor:
      LogicalVolDesc = Buffer;

      //
      // Check for existence of a single LVD and whether it is supported by
      // current EDK2 UDF file system implementation.
      //
      if (++LvdsCount > 1 ||
          !IsLogicalVolumeDescriptorSupported (LogicalVolDesc)) {
        Status = EFI_UNSUPPORTED;
        StopSequence = TRUE;
      }

      break;

    case UdfTerminatingDescriptor:
      //
      // Stop the sequence when we find a Terminating Descriptor
      // (aka Unallocated Sector), se we don't have to walk all the unallocated
      // area unnecessarily.
      //
      StopSequence = TRUE;
      break;

    default:
      //
      // An invalid Volume Descriptor has been found in the sequece. Volume is
      // corrupted.
      //
      Status = EFI_VOLUME_CORRUPTED;
      goto Out_Free;
    }
  }

  //
  // Check if LVD was found
  //
  if (!EFI_ERROR (Status) && LvdsCount == 1) {
    *MainVdsStartBlock = GuardMainVdsStartBlock;
    //
    // We do not need to read either LVD or PD descriptors to know the last
    // valid block in the found UDF file system. It's already
    // LastRecordedBlock.
    //
    *MainVdsEndBlock = LastRecordedBlock;

    Status = EFI_SUCCESS;
  }

Out_Free:
  //
  // Free block read buffer
  //
  FreePool (Buffer);

  return Status;
}