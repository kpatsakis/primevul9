void AbstractWebApplication::translateDocument(QString& data)
{
    const QRegExp regex("QBT_TR\\((([^\\)]|\\)(?!QBT_TR))+)\\)QBT_TR(\\[CONTEXT=([a-zA-Z_][a-zA-Z0-9_]*)\\])?");
    const QRegExp mnemonic("\\(?&([a-zA-Z]?\\))?");
    const std::string contexts[] = {
        "TransferListFiltersWidget", "TransferListWidget", "PropertiesWidget",
        "HttpServer", "confirmDeletionDlg", "TrackerList", "TorrentFilesModel",
        "options_imp", "Preferences", "TrackersAdditionDlg", "ScanFoldersModel",
        "PropTabBar", "TorrentModel", "downloadFromURL", "MainWindow", "misc",
        "StatusBar", "AboutDlg", "about", "PeerListWidget", "StatusFiltersWidget",
        "CategoryFiltersList", "TransferListDelegate"
    };
    const size_t context_count = sizeof(contexts) / sizeof(contexts[0]);
    int i = 0;
    bool found = true;

    const QString locale = Preferences::instance()->getLocale();
    bool isTranslationNeeded = !locale.startsWith("en") || locale.startsWith("en_AU") || locale.startsWith("en_GB");

    while(i < data.size() && found) {
        i = regex.indexIn(data, i);
        if (i >= 0) {
            //qDebug("Found translatable string: %s", regex.cap(1).toUtf8().data());
            QByteArray word = regex.cap(1).toUtf8();

            QString translation = word;
            if (isTranslationNeeded) {
                QString context = regex.cap(4);
                if (context.length() > 0) {
#ifndef QBT_USES_QT5
                    translation = qApp->translate(context.toUtf8().constData(), word.constData(), 0, QCoreApplication::UnicodeUTF8, 1);
#else
                    translation = qApp->translate(context.toUtf8().constData(), word.constData(), 0, 1);
#endif
                }
                else {
                    size_t context_index = 0;
                    while ((context_index < context_count) && (translation == word)) {
#ifndef QBT_USES_QT5
                        translation = qApp->translate(contexts[context_index].c_str(), word.constData(), 0, QCoreApplication::UnicodeUTF8, 1);
#else
                        translation = qApp->translate(contexts[context_index].c_str(), word.constData(), 0, 1);
#endif
                        ++context_index;
                    }
                }
            }
            // Remove keyboard shortcuts
            translation.replace(mnemonic, "");

            // Use HTML code for quotes to prevent issues with JS
            translation.replace("'", "&#39;");
            translation.replace("\"", "&#34;");

            data.replace(i, regex.matchedLength(), translation);
            i += translation.length();
        }
        else {
            found = false; // no more translatable strings
        }
    }
}