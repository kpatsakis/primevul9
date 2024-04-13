EmbFile *FileSpec::getEmbeddedFile()
{
  if(!ok)
    return nullptr;

  if (embFile)
    return embFile;

  XRef *xref = fileSpec.getDict()->getXRef();
  embFile = new EmbFile(fileStream.fetch(xref));

  return embFile;
}