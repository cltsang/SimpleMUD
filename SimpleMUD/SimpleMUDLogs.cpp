// MUD Programming
// Ron Penton
// (C)2003
// SimpleMUDLogs.cpp - This file contains all of the various logs used
//                     in SimpleMUD
// 
// 

#include "SimpleMUDLogs.h"

using namespace BasicLib;

namespace SimpleMUD
{

    TextLog ERRORLOG( "logs/errors.log", "Error Log", true, true );
    TextLog USERLOG( "logs/users.log", "User Log", true, true );


}   // end namespace SimpleMUD

