QString Utility::stdWStringToQString(const std::wstring &str)
{
    return QString::fromUtf16((const ushort *)str.c_str());
}