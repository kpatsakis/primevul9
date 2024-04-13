QString AbstractWebApplication::saveTmpFile(const QByteArray &data)
{
    QTemporaryFile tmpfile(Utils::Fs::tempPath() + "XXXXXX.torrent");
    tmpfile.setAutoRemove(false);
    if (tmpfile.open()) {
        tmpfile.write(data);
        tmpfile.close();
        return tmpfile.fileName();
    }

    qWarning() << "I/O Error: Could not create temporary file";
    return QString();
}