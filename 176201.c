bool Utility::use_filename_warning(const QString &filename)
{
    if (has_non_ascii_chars(filename)) {
        return QMessageBox::Apply == QMessageBox::warning(QApplication::activeWindow(),
                tr("Sigil"),
                tr("The requested file name contains non-ASCII characters. "
                   "You should only use ASCII characters in filenames. "
                   "Using non-ASCII characters can prevent the EPUB from working "
                   "with some readers.\n\n"
                   "Continue using the requested filename?"),
                QMessageBox::Cancel|QMessageBox::Apply);
    }
    return true;
}