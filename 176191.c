QString Utility::Substring(int start_index, int end_index, const QString &string)
{
    return string.mid(start_index, end_index - start_index);
}