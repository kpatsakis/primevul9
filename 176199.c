std::wstring Utility::QStringToStdWString(const QString &str)
{
    return std::wstring((const wchar_t *)str.utf16());
}