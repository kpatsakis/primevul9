void ImportEPUB::LocateOrCreateNCX(const QString &ncx_id_on_spine)
{
    QString load_warning;
    QString ncx_href = "not_found";
    m_NCXId = ncx_id_on_spine;

    // Handle various failure conditions, such as:
    // - ncx not specified in the spine (search for a matching manifest item by extension)
    // - ncx specified in spine, but no matching manifest item entry (create a new one)
    // - ncx file not physically present (create a new one)
    // - ncx not in spine or manifest item (create a new one)
    if (!m_NCXId.isEmpty()) {
        ncx_href = m_NcxCandidates[ m_NCXId ];
    } else {
        // Search for the ncx in the manifest by looking for files with
        // a .ncx extension.
        QHashIterator<QString, QString> ncxSearch(m_NcxCandidates);

        while (ncxSearch.hasNext()) {
            ncxSearch.next();

            if (QFileInfo(ncxSearch.value()).suffix().toLower() == NCX_EXTENSION) {
                m_NCXId = ncxSearch.key();
                load_warning = QObject::tr("The OPF file did not identify the NCX file correctly.") + "\n" + 
                               " - "  +  QObject::tr("Sigil has used the following file as the NCX:") + 
                               QString(" %1").arg(m_NcxCandidates[ m_NCXId ]);
                ncx_href = m_NcxCandidates[ m_NCXId ];
                break;
            }
        }
    }

    m_NCXFilePath = QFileInfo(m_OPFFilePath).absolutePath() % "/" % ncx_href;

    if (ncx_href.isEmpty() || !QFile::exists(m_NCXFilePath)) {
        m_NCXNotInManifest = m_NCXId.isEmpty() || ncx_href.isEmpty();
        m_NCXId.clear();
        // Things are really bad and no .ncx file was found in the manifest or
        // the file does not physically exist.  We need to create a new one.
        m_NCXFilePath = QFileInfo(m_OPFFilePath).absolutePath() % "/" % NCX_FILE_NAME;
        // Create a new file for the NCX.
        NCXResource ncx_resource(m_ExtractedFolderPath, m_NCXFilePath, m_Book->GetFolderKeeper());

        // We are relying on an identifier being set from the metadata.
        // It might not have one if the book does not have the urn:uuid: format.
        if (!m_UuidIdentifierValue.isEmpty()) {
            ncx_resource.SetMainID(m_UuidIdentifierValue);
        }

        ncx_resource.SaveToDisk();

        if (m_PackageVersion.startsWith('3')) { 
            load_warning = QObject::tr("Sigil has created a template NCX") + "\n" + 
              QObject::tr("to support epub2 backwards compatibility.");
        } else {
            if (ncx_href.isEmpty()) {
                load_warning = QObject::tr("The OPF file does not contain an NCX file.") + "\n" + 
                               " - " +  QObject::tr("Sigil has created a new one for you.");
            } else {
                load_warning = QObject::tr("The NCX file is not present in this EPUB.") + "\n" + 
                               " - " + QObject::tr("Sigil has created a new one for you.");
            }
        }
    }

    if (!load_warning.isEmpty()) {
        AddLoadWarning(load_warning);
    }
}