void Utility::DisplayExceptionErrorDialog(const QString &error_info)
{
    QMessageBox message_box(QApplication::activeWindow());
    message_box.setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    message_box.setModal(true);
    message_box.setIcon(QMessageBox::Critical);
    message_box.setWindowTitle("Sigil");
    // Spaces are added to the end because otherwise the dialog is too small.
    message_box.setText(QObject::tr("Sigil has encountered a problem.") % "                                                                                                       ");
    message_box.setInformativeText(QObject::tr("Sigil may need to close."));
    message_box.setStandardButtons(QMessageBox::Close);
    QStringList detailed_text;
    detailed_text << "Error info: "    + error_info
                  << "Sigil version: " + QString(SIGIL_FULL_VERSION)
                  << "Runtime Qt: "    + QString(qVersion())
                  << "Compiled Qt: "   + QString(QT_VERSION_STR);
#if defined Q_OS_WIN32
    detailed_text << "Platform: Windows SysInfo ID " + QString::number(QSysInfo::WindowsVersion);
#elif defined Q_OS_MAC
    detailed_text << "Platform: Mac SysInfo ID " + QString::number(QSysInfo::MacintoshVersion);
#else
    detailed_text << "Platform: Linux";
#endif
    message_box.setDetailedText(detailed_text.join("\n"));
    message_box.exec();
}