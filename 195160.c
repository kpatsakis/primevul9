GooString *FileSpec::getFileNameForPlatform()
{
  if (platformFileName)
    return platformFileName;

  Object obj1 = getFileSpecNameForPlatform(&fileSpec);
  if (obj1.isString())
    platformFileName = obj1.getString()->copy();

  return platformFileName;
}