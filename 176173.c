QString Utility::GetTemporaryFileNameWithExtension(const QString &extension)
{
    SettingsStore ss;
    QString temp_path = ss.tempFolderHome();
    if (temp_path == "<SIGIL_DEFAULT_TEMP_HOME>") {
        temp_path = QDir::tempPath();
    }
    return temp_path +  "/sigil_" + Utility::CreateUUID() + extension;
}