Object getFileSpecName (const Object *fileSpec)
{
  if (fileSpec->isString()) {
    return fileSpec->copy();
  }
  
  if (fileSpec->isDict()) {
    Object fileName = fileSpec->dictLookup("UF");
    if (fileName.isString()) {
      return fileName;
    }
    fileName = fileSpec->dictLookup("F");
    if (fileName.isString()) {
      return fileName;
    }
    fileName = fileSpec->dictLookup("DOS");
    if (fileName.isString()) {
      return fileName;
    }
    fileName = fileSpec->dictLookup("Mac");
    if (fileName.isString()) {
      return fileName;
    }
    fileName = fileSpec->dictLookup("Unix");
    if (fileName.isString()) {
      return fileName;
    }
  }
  return Object();
}