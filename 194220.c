void APE::Properties::read()
{
  // First we are searching the descriptor
  long offset = findDescriptor();
  if(offset < 0)
    return;

  // Then we read the header common for all versions of APE
  d->file->seek(offset);
  ByteVector commonHeader=d->file->readBlock(6);
  if(!commonHeader.startsWith("MAC "))
    return;
  d->version = commonHeader.mid(4).toUInt(false);

  if(d->version >= 3980) {
    analyzeCurrent();
  }
  else {
    analyzeOld();
  }
}