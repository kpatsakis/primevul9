void ImportEPUB::ProcessFontFiles(const QList<Resource *> &resources,
                                  const QHash<QString, QString> &updates,
                                  const QHash<QString, QString> &encrypted_files)
{
    if (encrypted_files.empty()) {
        return;
    }

    QList<FontResource *> font_resources = m_Book->GetFolderKeeper()->GetResourceTypeList<FontResource>();

    if (font_resources.empty()) {
        return;
    }

    QHash<QString, QString> new_font_paths_to_algorithms;
    foreach(QString old_update_path, updates.keys()) {
        if (!FONT_EXTENSIONS.contains(QFileInfo(old_update_path).suffix().toLower())) {
            continue;
        }

        QString new_update_path = updates[ old_update_path ];
        foreach(QString old_encrypted_path, encrypted_files.keys()) {
            if (old_update_path == old_encrypted_path) {
                new_font_paths_to_algorithms[ new_update_path ] = encrypted_files[ old_encrypted_path ];
            }
        }
    }
    foreach(FontResource * font_resource, font_resources) {
        QString match_path = "../" + font_resource->GetRelativePathToOEBPS();
        QString algorithm  = new_font_paths_to_algorithms.value(match_path);

        if (algorithm.isEmpty()) {
            continue;
        }

        font_resource->SetObfuscationAlgorithm(algorithm);

        // Actually we are de-obfuscating, but the inverse operations of the obfuscation methods
        // are the obfuscation methods themselves. For the math oriented, the obfuscation methods
        // are involutary [ f( f( x ) ) = x ].
        if (algorithm == ADOBE_FONT_ALGO_ID) {
            FontObfuscation::ObfuscateFile(font_resource->GetFullPath(), algorithm, m_UuidIdentifierValue);
        } else {
            FontObfuscation::ObfuscateFile(font_resource->GetFullPath(), algorithm, m_UniqueIdentifierValue);
        }
    }
}