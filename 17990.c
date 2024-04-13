XkbSizeGeomKeyAliases(XkbGeometryPtr geom)
{
    return geom->num_key_aliases*(2*XkbKeyNameLength);
}