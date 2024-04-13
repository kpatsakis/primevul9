void Utility::DisplayStdErrorDialog(const QString &error_message, const QString &detailed_text)
{
    QMessageBox message_box(QApplication::activeWindow());
    message_box.setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    message_box.setModal(true);
    message_box.setIcon(QMessageBox::Critical);
    message_box.setWindowTitle("Sigil");
    message_box.setText(error_message);

    if (!detailed_text.isEmpty()) {
        message_box.setDetailedText(detailed_text);
    }

    message_box.setStandardButtons(QMessageBox::Close);
    message_box.exec();
}