void ImportEPUB::LocateOPF()
{
    QString fullpath = m_ExtractedFolderPath + "/META-INF/container.xml";
    QXmlStreamReader container;
    try {
        container.addData(Utility::ReadUnicodeTextFile(fullpath));
    } catch (CannotOpenFile) {
        // Find the first OPF file.
        QString OPFfile;
        QDirIterator files(m_ExtractedFolderPath, QStringList() << "*.opf", QDir::NoFilter, QDirIterator::Subdirectories);
        while (files.hasNext()) {
            OPFfile = QDir(m_ExtractedFolderPath).relativeFilePath(files.next());
            break;
        }

        if (OPFfile.isEmpty()) {
            std::string msg = fullpath.toStdString() + ": " + tr("Epub has missing or improperly specified OPF.").toStdString();
            throw (CannotOpenFile(msg));
        }

        // Create a default container.xml.
        QDir folder(m_ExtractedFolderPath);
        folder.mkdir("META-INF");
        Utility::WriteUnicodeTextFile(CONTAINER_XML.arg(OPFfile), fullpath);
        container.addData(Utility::ReadUnicodeTextFile(fullpath));
    }

    while (!container.atEnd()) {
        container.readNext();

        if (container.isStartElement() &&
            container.name() == "rootfile"
           ) {
            if (container.attributes().hasAttribute("media-type") &&
                container.attributes().value("", "media-type") == OEBPS_MIMETYPE
               ) {
                m_OPFFilePath = m_ExtractedFolderPath + "/" + container.attributes().value("", "full-path").toString();
                // As per OCF spec, the first rootfile element
                // with the OEBPS mimetype is considered the "main" one.
                break;
            }
        }
    }

    if (container.hasError()) {
        const QString error = QString(
                                  QObject::tr("Unable to parse container.xml file.\nLine: %1 Column %2 - %3"))
                              .arg(container.lineNumber())
                              .arg(container.columnNumber())
                              .arg(container.errorString());
        throw (EPUBLoadParseError(error.toStdString()));
    }

    if (m_OPFFilePath.isEmpty() || !QFile::exists(m_OPFFilePath)) {
        throw (EPUBLoadParseError(QString(QObject::tr("No appropriate OPF file found")).toStdString()));
    }
}