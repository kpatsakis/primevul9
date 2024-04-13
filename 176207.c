QString Utility::URLEncodePath(const QString &path)
{
    QString newpath = path;
    QUrl href = QUrl(newpath);
    QString scheme = href.scheme();
    if (!scheme.isEmpty()) {
        scheme = scheme + "://";
        newpath.remove(0, scheme.length());
    }
    QByteArray encoded_url = QUrl::toPercentEncoding(newpath, QByteArray("/#"));
    return scheme + QString::fromUtf8(encoded_url.constData(), encoded_url.count());
}