QSharedPointer<Book> ImportEPUB::GetBook(bool extract_metadata)
{
    QList<XMLResource *> non_well_formed;
    SettingsStore ss;

    if (!Utility::IsFileReadable(m_FullFilePath)) {
        throw (EPUBLoadParseError(QString(QObject::tr("Cannot read EPUB: %1")).arg(QDir::toNativeSeparators(m_FullFilePath)).toStdString()));
    }

    // These read the EPUB file
    ExtractContainer();
    QHash<QString, QString> encrypted_files = ParseEncryptionXml();

    if (BookContentEncrypted(encrypted_files)) {
        throw (FileEncryptedWithDrm(""));
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // These mutate the m_Book object
    LocateOPF();
    m_opfDir = QFileInfo(m_OPFFilePath).dir();
    // These mutate the m_Book object
    ReadOPF();
    AddObfuscatedButUndeclaredFonts(encrypted_files);
    AddNonStandardAppleXML();
    LoadInfrastructureFiles();
    // Check for files missing in the Manifest and create warning
    QStringList notInManifest;
    foreach(QString file_path, m_ZipFilePaths) {
        // skip mimetype and anything in META-INF and the opf itself
        if (file_path == "mimetype") continue;
        if (file_path.startsWith("META-INF")) continue;
	if (m_OPFFilePath.contains(file_path)) continue;
	if (!m_ManifestFilePaths.contains(file_path)) {
	    notInManifest << file_path;
	}
    }
    if (!notInManifest.isEmpty()) {
        Utility::DisplayStdWarningDialog(tr("Files exist in epub that are not listed in the manifest, they will be ignored"), notInManifest.join("\n"));
    }
    const QHash<QString, QString> updates = LoadFolderStructure();
    const QList<Resource *> resources     = m_Book->GetFolderKeeper()->GetResourceList();

    // We're going to check all html files until we find one that isn't well formed then we'll prompt
    // the user if they want to auto fix or not.
    //
    // If we have non-well formed content and they shouldn't be auto fixed we'll pass that on to
    // the universal update function so it knows to skip them. Otherwise we won't include them and
    // let it modify the file.
    for (int i=0; i<resources.count(); ++i) {
        if (resources.at(i)->Type() == Resource::HTMLResourceType) {
            HTMLResource *hresource = dynamic_cast<HTMLResource *>(resources.at(i));
            if (!hresource) {
                continue;
            }
            // Load the content into the HTMLResource so we can perform a well formed check.
            try {
                hresource->SetText(HTMLEncodingResolver::ReadHTMLFile(hresource->GetFullPath()));
            } catch (...) {
                if (ss.cleanOn() & CLEANON_OPEN) {
                    non_well_formed << hresource;
                    continue;
                }
            }
            if (ss.cleanOn() & CLEANON_OPEN) {
              if (!XhtmlDoc::IsDataWellFormed(hresource->GetText(),hresource->GetEpubVersion())) {
                    non_well_formed << hresource;
                }
            }
        }
    }
    if (!non_well_formed.isEmpty()) {
        QApplication::restoreOverrideCursor();
        if (QMessageBox::Yes == QMessageBox::warning(QApplication::activeWindow(),
                tr("Sigil"),
                tr("This EPUB has HTML files that are not well formed. "
                   "Sigil can attempt to automatically fix these files, although this "
                   "can result in data loss.\n\n"
                   "Do you want to automatically fix the files?"),
                QMessageBox::Yes|QMessageBox::No)
           ) {
            non_well_formed.clear();
        }
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }

    const QStringList load_errors = UniversalUpdates::PerformUniversalUpdates(false, resources, updates, non_well_formed);

    Q_FOREACH (QString err, load_errors) {
        AddLoadWarning(QString("%1").arg(err));
    }

    ProcessFontFiles(resources, updates, encrypted_files);

    if (m_PackageVersion.startsWith('3')) {
        HTMLResource * nav_resource = NULL;
        if (m_NavResource) {
            if (m_NavResource->Type() == Resource::HTMLResourceType) {
                nav_resource = dynamic_cast<HTMLResource*>(m_NavResource);
            }
        }
        if (!nav_resource) { 
            // we need to create a nav file here because one was not found
            // it will automatically be added to the content.opf
            nav_resource = m_Book->CreateEmptyNavFile(true);
            Resource * res = dynamic_cast<Resource *>(nav_resource);
            m_Book->GetOPF()->SetItemRefLinear(res, false);
        }
        m_Book->GetOPF()->SetNavResource(nav_resource);
    }

    if (m_NCXNotInManifest) {
        // We manually created an NCX file because there wasn't one in the manifest.
        // Need to create a new manifest id for it.
        m_NCXId = m_Book->GetOPF()->AddNCXItem(m_NCXFilePath);
    }

    // Ensure that our spine has a <spine toc="ncx"> element on it now in case it was missing.
    m_Book->GetOPF()->UpdateNCXOnSpine(m_NCXId);
    // Make sure the <item> for the NCX in the manifest reflects correct href path
    m_Book->GetOPF()->UpdateNCXLocationInManifest(m_Book->GetNCX());

    // If spine was not present or did not contain any items, recreate the OPF from scratch
    // preserving any important metadata elements and making a new reading order.
    if (!m_HasSpineItems) {
        QList<MetaEntry> originalMetadata = m_Book->GetOPF()->GetDCMetadata();
        m_Book->GetOPF()->AutoFixWellFormedErrors();
        if (extract_metadata) {
            m_Book->GetOPF()->SetDCMetadata(originalMetadata);
        }
        AddLoadWarning(QObject::tr("The OPF file does not contain a valid spine.") % "\n" %
                       QObject::tr("Sigil has created a new one for you."));
    }

    // If we have modified the book to add spine attribute, manifest item or NCX mark as changed.
    m_Book->SetModified(GetLoadWarnings().count() > 0);
    QApplication::restoreOverrideCursor();
    return m_Book;
}