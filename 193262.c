OFBool DcmSCP::getPresentationContextInfo(const T_ASC_Association *assoc,
                                          const Uint8 presID,
                                          DcmPresentationContextInfo &info)
{
  if (assoc != NULL)
  {
    DUL_PRESENTATIONCONTEXT *pc = findPresentationContextID(assoc->params->DULparams.acceptedPresentationContext, presID);
    if (pc != NULL)
    {
      info.abstractSyntax = pc->abstractSyntax;
      info.acceptedTransferSyntax = pc->acceptedTransferSyntax;
      info.presentationContextID = pc->presentationContextID;
      info.proposedSCRole = pc->proposedSCRole;
      info.acceptedSCRole = pc->acceptedSCRole;
      return OFTrue;
    }
  }
  return OFFalse;
}