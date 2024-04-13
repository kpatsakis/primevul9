void Utility::CopyFiles(const QString &fullfolderpath_source, const QString &fullfolderpath_destination)
{
    QDir folder_source(fullfolderpath_source);
    QDir folder_destination(fullfolderpath_destination);
    // Erase all the files in this folder
    foreach(QFileInfo file, folder_source.entryInfoList()) {
        if ((file.fileName() != ".") && (file.fileName() != "..")) {
            // If it's a file, copy it
            if (file.isFile()) {
                QString destination = fullfolderpath_destination + "/" + file.fileName();
                bool success = QFile::copy(file.absoluteFilePath(), destination);

                if (!success) {
                    std::string msg = file.absoluteFilePath().toStdString() + ": " + destination.toStdString();
                    throw(CannotCopyFile(msg));
                }
            }
            // Else it's a directory, copy everything in it
            // to a new folder of the same name in the destination folder
            else {
                folder_destination.mkpath(file.fileName());
                CopyFiles(file.absoluteFilePath(), fullfolderpath_destination + "/" + file.fileName());
            }
        }
    }
}