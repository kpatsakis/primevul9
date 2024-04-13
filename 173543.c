GetFileEntryData (
  IN   VOID    *FileEntryData,
  IN   UINTN   FileEntrySize,
  OUT  VOID    **Data,
  OUT  UINT64  *Length
  )
{
  UDF_DESCRIPTOR_TAG       *DescriptorTag;
  UDF_EXTENDED_FILE_ENTRY  *ExtendedFileEntry;
  UDF_FILE_ENTRY           *FileEntry;

  DescriptorTag = FileEntryData;

  if (DescriptorTag->TagIdentifier == UdfExtendedFileEntry) {
    ExtendedFileEntry = (UDF_EXTENDED_FILE_ENTRY *)FileEntryData;

    *Length  = ExtendedFileEntry->InformationLength;
    *Data    = (VOID *)((UINT8 *)ExtendedFileEntry->Data +
                        ExtendedFileEntry->LengthOfExtendedAttributes);
  } else if (DescriptorTag->TagIdentifier == UdfFileEntry) {
    FileEntry = (UDF_FILE_ENTRY *)FileEntryData;

    *Length  = FileEntry->InformationLength;
    *Data    = (VOID *)((UINT8 *)FileEntry->Data +
                        FileEntry->LengthOfExtendedAttributes);
  }

  if ((*Length > FileEntrySize) ||
      ((UINTN)FileEntryData > (UINTN)(*Data)) ||
      ((UINTN)(*Data) - (UINTN)FileEntryData > FileEntrySize - *Length)) {
    return EFI_VOLUME_CORRUPTED;
  }
  return EFI_SUCCESS;
}