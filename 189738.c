ImportEPUB::ImportEPUB(const QString &fullfilepath)
    : Importer(fullfilepath),
      m_ExtractedFolderPath(m_TempFolder.GetPath()),
      m_HasSpineItems(false),
      m_NCXNotInManifest(false),
      m_NavResource(NULL)
{
}