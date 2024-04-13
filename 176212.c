QString Utility::ReadUnicodeTextFile(const QString &fullfilepath)
{
    // TODO: throw an exception instead of
    // returning an empty string
    QFile file(fullfilepath);

    // Check if we can open the file
    if (!file.open(QFile::ReadOnly)) {
        std::string msg = fullfilepath.toStdString() + ": " + file.errorString().toStdString();
        throw(CannotOpenFile(msg));
    }

    QTextStream in(&file);
    // Input should be UTF-8
    in.setCodec("UTF-8");
    // This will automatically switch reading from
    // UTF-8 to UTF-16 if a BOM is detected
    in.setAutoDetectUnicode(true);
    return ConvertLineEndings(in.readAll());
}