IsLogicalVolumeDescriptorSupported (
  UDF_LOGICAL_VOLUME_DESCRIPTOR *LogicalVolDesc
  )
{
  //
  // Check for a valid UDF revision range
  //
  switch (LogicalVolDesc->DomainIdentifier.Suffix.Domain.UdfRevision) {
  case 0x0102:
  case 0x0150:
  case 0x0200:
  case 0x0201:
  case 0x0250:
  case 0x0260:
    break;
  default:
    return FALSE;
  }

  //
  // Check for a single Partition Map
  //
  if (LogicalVolDesc->NumberOfPartitionMaps > 1) {
    return FALSE;
  }
  //
  // UDF 1.02 revision supports only Type 1 (Physical) partitions, but
  // let's check it any way.
  //
  // PartitionMap[0] -> type
  // PartitionMap[1] -> length (in bytes)
  //
  if (LogicalVolDesc->PartitionMaps[0] != 1 ||
      LogicalVolDesc->PartitionMaps[1] != 6) {
    return FALSE;
  }

  return TRUE;
}