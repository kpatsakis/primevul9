void ImportEPUB::LoadInfrastructureFiles()
{
    // always SetEpubVersion before SetText in OPF as SetText will validate with it
    m_Book->GetOPF()->SetEpubVersion(m_PackageVersion);
    m_Book->GetOPF()->SetText(CleanSource::ProcessXML(PrepareOPFForReading(Utility::ReadUnicodeTextFile(m_OPFFilePath)),OEBPS_MIMETYPE));
    QString OPFBookRelPath = m_OPFFilePath;
    OPFBookRelPath = OPFBookRelPath.remove(0,m_ExtractedFolderPath.length()+1);
    m_Book->GetOPF()->SetCurrentBookRelPath(OPFBookRelPath);

    m_Book->GetNCX()->SetText(CleanSource::ProcessXML(Utility::ReadUnicodeTextFile(m_NCXFilePath),"application/x-dtbncx+xml"));
    m_Book->GetNCX()->SetEpubVersion(m_PackageVersion);
    QString NCXBookRelPath = m_NCXFilePath;
    NCXBookRelPath = NCXBookRelPath.remove(0,m_ExtractedFolderPath.length()+1);
    m_Book->GetNCX()->SetCurrentBookRelPath(NCXBookRelPath);
}