QWidget *Utility::GetMainWindow()
{
    QWidget *parent_window = QApplication::activeWindow();

    while (parent_window && !(dynamic_cast<QMainWindow *>(parent_window))) {
        parent_window = parent_window->parentWidget();
    }

    return parent_window;
}