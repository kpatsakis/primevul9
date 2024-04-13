QHash<QString, QString> ImportEPUB::ParseEncryptionXml()
{
    QString encrpytion_xml_path = m_ExtractedFolderPath + "/META-INF/encryption.xml";

    if (!QFileInfo(encrpytion_xml_path).exists()) {
        return QHash<QString, QString>();
    }

    QXmlStreamReader encryption(Utility::ReadUnicodeTextFile(encrpytion_xml_path));
    QHash<QString, QString> encrypted_files;
    QString encryption_algo;
    QString uri;

    while (!encryption.atEnd()) {
        encryption.readNext();

        if (encryption.isStartElement()) {
            if (encryption.name() == "EncryptionMethod") {
                encryption_algo = encryption.attributes().value("", "Algorithm").toString();
            } else if (encryption.name() == "CipherReference") {
                uri = Utility::URLDecodePath(encryption.attributes().value("", "URI").toString());
                encrypted_files[ uri ] = encryption_algo;
            }
        }
    }

    if (encryption.hasError()) {
        const QString error = QString(QObject::tr("Error parsing encryption xml.\nLine: %1 Column %2 - %3"))
                              .arg(encryption.lineNumber())
                              .arg(encryption.columnNumber())
                              .arg(encryption.errorString());
        throw (EPUBLoadParseError(error.toStdString()));
    }

    return encrypted_files;
}