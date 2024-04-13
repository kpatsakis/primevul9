std::tuple<QString, QString> ImportEPUB::LoadOneFile(const QString &path, const QString &mimetype)
{
    QString fullfilepath = QDir::cleanPath(QFileInfo(m_OPFFilePath).absolutePath() + "/" + path);
    QString currentpath = fullfilepath;
    currentpath = currentpath.remove(0,m_ExtractedFolderPath.length()+1);
    try {
        Resource *resource = m_Book->GetFolderKeeper()->AddContentFileToFolder(fullfilepath, false, mimetype);
        if (path == m_NavHref) {
            m_NavResource = resource;
        }
        resource->SetCurrentBookRelPath(currentpath);
        QString newpath = "../" + resource->GetRelativePathToOEBPS();
        return std::make_tuple(currentpath, newpath);
    } catch (FileDoesNotExist) {
        return std::make_tuple(UPDATE_ERROR_STRING, UPDATE_ERROR_STRING);
    }
}