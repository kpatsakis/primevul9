bool AbstractWebApplication::readFile(const QString& path, QByteArray &data, QString &type)
{
    QString ext = "";
    int index = path.lastIndexOf('.') + 1;
    if (index > 0)
        ext = path.mid(index);

    // find translated file in cache
    if (translatedFiles_.contains(path)) {
        data = translatedFiles_[path];
    }
    else {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug("File %s was not found!", qPrintable(path));
            return false;
        }

        data = file.readAll();
        file.close();

        // Translate the file
        if ((ext == "html") || ((ext == "js") && !path.endsWith("excanvas-compressed.js"))) {
            QString dataStr = QString::fromUtf8(data.constData());
            translateDocument(dataStr);

            if (path.endsWith("about.html") || path.endsWith("index.html") || path.endsWith("client.js"))
                dataStr.replace("${VERSION}", VERSION);

            data = dataStr.toUtf8();
            translatedFiles_[path] = data; // cashing translated file
        }
    }

    type = CONTENT_TYPE_BY_EXT[ext];
    return true;
}