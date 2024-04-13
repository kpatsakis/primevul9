bool Utility::ForceCopyFile(const QString &fullinpath, const QString &fulloutpath)
{
    if (!QFileInfo(fullinpath).exists()) {
        return false;
    }
    if (QFileInfo::exists(fulloutpath)) {
        Utility::SDeleteFile(fulloutpath);
    }
    return QFile::copy(fullinpath, fulloutpath);
}