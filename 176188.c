void Utility::WriteUnicodeTextFile(const QString &text, const QString &fullfilepath)
{
    QFile file(fullfilepath);

    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Truncate  |
                   QIODevice::Text
                  )
       ) {
        std::string msg = file.fileName().toStdString() + ": " + file.errorString().toStdString();
        throw(CannotOpenFile(msg));
    }

    QTextStream out(&file);
    // We ALWAYS output in UTF-8
    out.setCodec("UTF-8");
    out << text;
}