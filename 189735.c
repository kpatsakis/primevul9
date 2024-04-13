void ImportEPUB::AddObfuscatedButUndeclaredFonts(const QHash<QString, QString> &encrypted_files)
{
    if (encrypted_files.empty()) {
        return;
    }

    QDir opf_dir = QFileInfo(m_OPFFilePath).dir();
    foreach(QString filepath, encrypted_files.keys()) {
        if (!FONT_EXTENSIONS.contains(QFileInfo(filepath).suffix().toLower())) {
            continue;
        }

        // Only add the path to the manifest if it is not already included.
        QMapIterator<QString, QString> valueSearch(m_Files);

        if (!valueSearch.findNext(opf_dir.relativeFilePath(filepath))) {
            m_Files[ Utility::CreateUUID() ] = opf_dir.relativeFilePath(filepath);
        }
    }
}