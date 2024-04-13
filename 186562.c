_dbus_error_from_gai (int gai_res,
                      int saved_errno)
{
  switch (gai_res)
    {
#ifdef EAI_FAMILY
      case EAI_FAMILY:
        /* ai_family not supported (at all) */
        return DBUS_ERROR_NOT_SUPPORTED;
#endif

#ifdef EAI_SOCKTYPE
      case EAI_SOCKTYPE:
        /* ai_socktype not supported (at all) */
        return DBUS_ERROR_NOT_SUPPORTED;
#endif

#ifdef EAI_MEMORY
      case EAI_MEMORY:
        /* Out of memory */
        return DBUS_ERROR_NO_MEMORY;
#endif

#ifdef EAI_SYSTEM
      case EAI_SYSTEM:
        /* Unspecified system error, details in errno */
        return _dbus_error_from_errno (saved_errno);
#endif

      case 0:
        /* It succeeded, but we didn't get any addresses? */
        return DBUS_ERROR_FAILED;

      /* EAI_AGAIN: Transient failure */
      /* EAI_BADFLAGS: invalid ai_flags (programming error) */
      /* EAI_FAIL: Non-recoverable failure */
      /* EAI_NODATA: host exists but has no addresses */
      /* EAI_NONAME: host does not exist */
      /* EAI_OVERFLOW: argument buffer overflow */
      /* EAI_SERVICE: service not available for specified socket
       * type (we should never see this because we use numeric
       * ports) */
      default:
        return DBUS_ERROR_FAILED;
    }
}