long APE::Properties::findDescriptor()
{
  long ID3v2Location = findID3v2();
  long ID3v2OriginalSize = 0;
  bool hasID3v2 = false;
  if(ID3v2Location >= 0) {
    ID3v2::Tag tag(d->file, ID3v2Location);
    ID3v2OriginalSize = tag.header()->completeTagSize();
    if(tag.header()->tagSize() > 0)
      hasID3v2 = true;
  }

  long offset = 0;
  if(hasID3v2)
    offset = d->file->find("MAC ", ID3v2Location + ID3v2OriginalSize);
  else
    offset = d->file->find("MAC ");

  if(offset < 0) {
    debug("APE::Properties::findDescriptor() -- APE descriptor not found");
    return -1;
  }

  return offset;
}