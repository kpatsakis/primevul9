void ImportEPUB::ReadIdentifierElement(QXmlStreamReader *opf_reader)
{
    QString id     = opf_reader->attributes().value("", "id").toString();
    QString scheme = opf_reader->attributes().value("", "scheme").toString();
    QString value  = opf_reader->readElementText();

    if (id == m_UniqueIdentifierId) {
        m_UniqueIdentifierValue = value;
    }

    if (m_UuidIdentifierValue.isEmpty() &&
        (value.contains("urn:uuid:") || scheme.toLower() == "uuid")) {
        m_UuidIdentifierValue = value;
    }
}