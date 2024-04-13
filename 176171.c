QStringRef Utility::SubstringRef(int start_index, int end_index, const QString &string)
{
    return string.midRef(start_index, end_index - start_index);
}