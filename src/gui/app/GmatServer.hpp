//$Header$
//------------------------------------------------------------------------------
//                               GmatServer
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// ** Legal **
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/08/27
//
/**
 * Declares GmatServer which creates new connection to client.
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "wx/ipc.h"

class GmatServer: public wxServer
{
public:
   wxConnectionBase *OnAcceptConnection(const wxString& topic);
};
