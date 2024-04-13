bool ImportEPUB::BookContentEncrypted(const QHash<QString, QString> &encrypted_files)
{
    foreach(QString algorithm, encrypted_files.values()) {
        if (algorithm != ADOBE_FONT_ALGO_ID &&
            algorithm != IDPF_FONT_ALGO_ID) {
            return true;
        }
    }
    return false;
}