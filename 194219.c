long APE::Properties::findID3v2()
{
  if(!d->file->isValid())
    return -1;

  d->file->seek(0);

  if(d->file->readBlock(3) == ID3v2::Header::fileIdentifier())
    return 0;

  return -1;
}