virNodeDeviceCapMdevTypesFormat(virBufferPtr buf,
                                virMediatedDeviceTypePtr *mdev_types,
                                const size_t nmdev_types)
{
    size_t i;

    if (nmdev_types > 0) {
        virBufferAddLit(buf, "<capability type='mdev_types'>\n");
        virBufferAdjustIndent(buf, 2);
        for (i = 0; i < nmdev_types; i++) {
            virMediatedDeviceTypePtr type = mdev_types[i];
            virBufferEscapeString(buf, "<type id='%s'>\n", type->id);
            virBufferAdjustIndent(buf, 2);
            if (type->name)
                virBufferEscapeString(buf, "<name>%s</name>\n",
                                      type->name);
            virBufferEscapeString(buf, "<deviceAPI>%s</deviceAPI>\n",
                                  type->device_api);
            virBufferAsprintf(buf,
                              "<availableInstances>%u</availableInstances>\n",
                              type->available_instances);
            virBufferAdjustIndent(buf, -2);
            virBufferAddLit(buf, "</type>\n");
        }
        virBufferAdjustIndent(buf, -2);
        virBufferAddLit(buf, "</capability>\n");
    }
}