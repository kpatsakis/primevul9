EmbFile *FileSpec::getEmbeddedFile()
{
  if(!ok)
    return nullptr;

  if (embFile)
    return embFile;

  Object obj1;
  XRef *xref = fileSpec.getDict()->getXRef();
  obj1 = fileStream.fetch(xref);
  embFile = new EmbFile(&obj1);

  return embFile;
}