void ImportEPUB::AddNonStandardAppleXML()
{
    QDir opf_dir = QFileInfo(m_OPFFilePath).dir();
    QStringList aberrant_Apple_filenames;
    aberrant_Apple_filenames.append(m_ExtractedFolderPath + "/META-INF/com.apple.ibooks.display-options.xml");

    for (int i = 0; i < aberrant_Apple_filenames.size(); ++i) {
        if (QFile::exists(aberrant_Apple_filenames.at(i))) {
            m_Files[ Utility::CreateUUID() ]  = opf_dir.relativeFilePath(aberrant_Apple_filenames.at(i));
        }
    }
}