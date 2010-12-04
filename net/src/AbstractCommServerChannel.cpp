/*
 * AbstractCommServerChannel.cpp
 *
 * Copyright (C) 2010 by Christoph M�ller. Alle Rechte vorbehalten.
 * Copyright (C) 2006 - 2010 by Visualisierungsinstitut Universitaet Stuttgart.
 * Alle Rechte vorbehalten.
 */

#include "vislib/AbstractCommServerChannel.h"

#include "vislib/StackTrace.h"


/*
 * vislib::net::AbstractCommServerChannel::AbstractCommServerChannel
 */
vislib::net::AbstractCommServerChannel::AbstractCommServerChannel(void) 
        : Super() {
    VLSTACKTRACE("AbstractCommServerChannel::AbstractCommServerChannel", 
        __FILE__, __LINE__);
}


/*
 * vislib::net::AbstractCommServerChannel::~AbstractCommServerChannel
 */
vislib::net::AbstractCommServerChannel::~AbstractCommServerChannel(void) {
    VLSTACKTRACE("AbstractCommServerChannel::~AbstractCommServerChannel", 
        __FILE__, __LINE__);
}
