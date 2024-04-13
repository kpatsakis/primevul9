void ImportEPUB::ReadMetadataLinkElement(QXmlStreamReader *opf_reader)
{
    QString relation = opf_reader->attributes().value("", "rel").toString();
    QString mtype = opf_reader->attributes().value("", "media-type").toString();
    QString props = opf_reader->attributes().value("", "properties").toString();
    QString href = opf_reader->attributes().value("", "href").toString();
    if (!href.isEmpty()) {
        QUrl url = QUrl(href);
        if (url.isRelative()) {
	    // we have a local unmanifested metadata file to handle
	    // attempt to map deprecated record types into proper media-types
	    if (relation == "marc21xml-record") {
                mtype = "application/marcxml+xml";
	    }
	    else if (relation == "mods-record") {
                mtype = "application/mods+xml";
	    }
	    else if (relation == "onix-record") {
                mtype = "application/xml;onix";
            }
	    else if (relation == "xmp-record") {
                mtype = "application/xml;xmp";
	    }
            else if (relation == "record") {
                if (props == "onix") mtype = "application/xml;onix";
                if (props == "xmp") mtype = "application/xml;xmp";
	    }
            QDir opf_dir = QFileInfo(m_OPFFilePath).dir();
	    QString path = opf_dir.absolutePath() + "/" + url.path();
	    if (QFile::exists(path)) {
	        QString id = Utility::CreateUUID();
		m_Files[ id ]  = opf_dir.relativeFilePath(path);
		m_FileMimetypes[ id ] = mtype;
	    }
	}
    }
}