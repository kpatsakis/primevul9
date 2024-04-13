QStringList Utility::ZipInspect(const QString &zippath)
{
    QStringList filelist;
    int res = 0;

    if (!cp437) {
        cp437 = new QCodePage437Codec();
    }
#ifdef Q_OS_WIN32
    zlib_filefunc64_def ffunc;
    fill_win32_filefunc64W(&ffunc);
    unzFile zfile = unzOpen2_64(Utility::QStringToStdWString(QDir::toNativeSeparators(zippath)).c_str(), &ffunc);
#else
    unzFile zfile = unzOpen64(QDir::toNativeSeparators(zippath).toUtf8().constData());
#endif

    if ((zfile == NULL) || (!IsFileReadable(zippath))) {
        return filelist;
    }
    res = unzGoToFirstFile(zfile);
    if (res == UNZ_OK) {
        do {
            // Get the name of the file in the archive.
            char file_name[MAX_PATH] = {0};
            unz_file_info64 file_info;
            unzGetCurrentFileInfo64(zfile, &file_info, file_name, MAX_PATH, NULL, 0, NULL, 0);
            QString qfile_name;
            QString cp437_file_name;
            qfile_name = QString::fromUtf8(file_name);
            if (!(file_info.flag & (1<<11))) {
                cp437_file_name = cp437->toUnicode(file_name);
            }

            // If there is no file name then we can't do anything with it.
            if (!qfile_name.isEmpty()) {
                if (!cp437_file_name.isEmpty() && cp437_file_name != qfile_name) {
                    filelist.append(cp437_file_name);
                } else {
                    filelist.append(qfile_name);
                }
            }
        } while ((res = unzGoToNextFile(zfile)) == UNZ_OK);
    }
    unzClose(zfile);
    return filelist;
}