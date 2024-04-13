bool Utility::RenameFile(const QString &oldfilepath, const QString &newfilepath)
{
    // Make sure the path exists, otherwise very
    // bad things could happen
    if (!QFileInfo(oldfilepath).exists()) {
        return false;
    }

    // Ensure that the newfilepath doesn't already exist but due to case insenstive file systems
    // check if we are actually renaming to an identical path with a different case.
    if (QFileInfo(newfilepath).exists() && QFileInfo(oldfilepath) != QFileInfo(newfilepath)) {
        return false;
    }

    // On case insensitive file systems, QFile::rename fails when the new name is the
    // same (case insensitive) to the old one. This is workaround for that issue.
    int ret = -1;
#if defined(Q_OS_WIN32)
    ret = _wrename(Utility::QStringToStdWString(oldfilepath).data(), Utility::QStringToStdWString(newfilepath).data());
#else
    ret = rename(oldfilepath.toUtf8().data(), newfilepath.toUtf8().data());
#endif

    if (ret == 0) {
        return true;
    }

    return false;
}