XkbComputeGetMapReplySize(XkbDescPtr xkb,xkbGetMapReply *rep)
{
int	len;

    rep->minKeyCode= xkb->min_key_code;
    rep->maxKeyCode= xkb->max_key_code;
    len= XkbSizeKeyTypes(xkb,rep);
    len+= XkbSizeKeySyms(xkb,rep);
    len+= XkbSizeKeyActions(xkb,rep);
    len+= XkbSizeKeyBehaviors(xkb,rep);
    len+= XkbSizeVirtualMods(xkb,rep);
    len+= XkbSizeExplicit(xkb,rep);
    len+= XkbSizeModifierMap(xkb,rep);
    len+= XkbSizeVirtualModMap(xkb,rep);
    rep->length+= (len/4);
    return Success;
}