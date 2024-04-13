bool Utility::has_non_ascii_chars(const QString &str)
{
    QRegularExpression not_ascii("[^\\x00-\\x7F]");
    QRegularExpressionMatch mo = not_ascii.match(str);
    return mo.hasMatch();
}