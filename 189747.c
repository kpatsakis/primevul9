void ImportEPUB::ReadManifestItemElement(QXmlStreamReader *opf_reader)
{
    QString id   = opf_reader->attributes().value("", "id").toString();
    QString href = opf_reader->attributes().value("", "href").toString();
    QString type = opf_reader->attributes().value("", "media-type").toString();
    QString properties = opf_reader->attributes().value("", "properties").toString();
    // Paths are percent encoded in the OPF, we use "normal" paths internally.
    href = Utility::URLDecodePath(href);
    QString extension = QFileInfo(href).suffix().toLower();

    // find the epub root relative file path from the opf location and the item href
    QString file_path = m_opfDir.absolutePath() + "/" + href;
    file_path = file_path.remove(0, m_ExtractedFolderPath.length() + 1); 
    
    if (type != NCX_MIMETYPE && extension != NCX_EXTENSION) {
        if (!m_ManifestFilePaths.contains(file_path)) {
            if (m_Files.contains(id)) {
                // We have an error situation with a duplicate id in the epub.
                // We must warn the user, but attempt to use another id so the epub can still be loaded.
                QString base_id = QFileInfo(href).fileName();
                QString new_id(base_id);
                int duplicate_index = 0;

                while (m_Files.contains(new_id)) {
                    duplicate_index++;
                    new_id = QString("%1%2").arg(base_id).arg(duplicate_index);
                }

                const QString load_warning = QObject::tr("The OPF manifest contains duplicate ids for: %1").arg(id) +
                  " - " + QObject::tr("A temporary id has been assigned to load this EPUB. You should edit your OPF file to remove the duplication.");
                id = new_id;
                AddLoadWarning(load_warning);
            }

            m_Files[ id ] = href;
            m_FileMimetypes[ id ] = type;
            m_ManifestFilePaths << file_path;

            // store information about any nav document
            if (properties.contains("nav")) {
                m_NavId = id;
                m_NavHref = href;
            }
        }
    } else {
        m_NcxCandidates[ id ] = href;
	m_ManifestFilePaths << file_path;
    }
}