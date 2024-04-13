QHash<QString, QString> ImportEPUB::LoadFolderStructure()
{
    QList<QString> keys = m_Files.keys();
    int num_files = keys.count();
    QFutureSynchronizer<std::tuple<QString, QString>> sync;

    for (int i = 0; i < num_files; ++i) {
        QString id = keys.at(i);
        sync.addFuture(QtConcurrent::run(
                           this,
                           &ImportEPUB::LoadOneFile,
                           m_Files.value(id),
                           m_FileMimetypes.value(id)));
    }

    sync.waitForFinished();
    QList<QFuture<std::tuple<QString, QString>>> futures = sync.futures();
    int num_futures = futures.count();
    QHash<QString, QString> updates;

    for (int i = 0; i < num_futures; ++i) {
        std::tuple<QString, QString> result = futures.at(i).result();
        updates[std::get<0>(result)] = std::get<1>(result);
    }

    updates.remove(UPDATE_ERROR_STRING);
    return updates;
}