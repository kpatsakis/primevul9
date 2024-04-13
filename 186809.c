QStringMap AbstractWebApplication::initializeContentTypeByExtMap()
{
    QStringMap map;

    map["htm"] = Http::CONTENT_TYPE_HTML;
    map["html"] = Http::CONTENT_TYPE_HTML;
    map["css"] = Http::CONTENT_TYPE_CSS;
    map["gif"] = Http::CONTENT_TYPE_GIF;
    map["png"] = Http::CONTENT_TYPE_PNG;
    map["js"] = Http::CONTENT_TYPE_JS;

    return map;
}