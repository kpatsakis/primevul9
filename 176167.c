QString Utility::GetEnvironmentVar(const QString &variable_name)
{
    // Renaming this function (and all references to it)
    // to GetEnvironmentVariable gets you a linker error
    // on MSVC 9. Funny, innit?
    QRegularExpression search_for_name("^" + QRegularExpression::escape(variable_name) + "=");
    QString variable = QProcess::systemEnvironment().filter(search_for_name).value(0);

    if (!variable.isEmpty()) {
        return variable.split("=")[ 1 ];
    } else {
        return QString();
    }
}