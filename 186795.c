QString AbstractWebApplication::generateSid()
{
    QString sid;

    do {
        const size_t size = 6;
        quint32 tmp[size];

        for (size_t i = 0; i < size; ++i)
            tmp[i] = Utils::Random::rand();

        sid = QByteArray::fromRawData(reinterpret_cast<const char *>(tmp), sizeof(quint32) * size).toBase64();
    }
    while (sessions_.contains(sid));

    return sid;
}