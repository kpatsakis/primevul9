QString Utility::EncodeXML(const QString &text)
{
    QString newtext(text);
    return newtext.toHtmlEscaped();
}