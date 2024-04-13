QString Utility::CreateUUID()
{
    return QUuid::createUuid().toString().remove("{").remove("}");
}