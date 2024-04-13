QString Utility::URLDecodePath(const QString &path)
{
    return QUrl::fromPercentEncoding(path.toUtf8());
}