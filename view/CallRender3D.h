/*
 * CallRender3D.h
 *
 * Copyright (C) 2008 by Universitaet Stuttgart (VIS). 
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOLCORE_CALLRENDER3D_H_INCLUDED
#define MEGAMOLCORE_CALLRENDER3D_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */

#include "api/MegaMolCore.std.h"
#include "CallAutoDescription.h"
#include "view/AbstractCallRender3D.h"
#include "view/RenderOutput.h"


namespace megamol {
namespace core {
namespace view {


#ifdef _WIN32
#pragma warning(disable: 4250)  // I know what I am doing ...
#endif /* _WIN32 */
    /**
     * Base class of rendering graph calls
     *
     * Function "Render" tells the callee to render itself into the currently
     * active opengl context (TODO: Late on it could also be a FBO).
     *
     * Function "GetExtents" asks the callee to fill the extents member of the
     * call (bounding boxes, temporal extents).
     *
     * Function "GetCapabilities" asks the callee to set the capabilities
     * flags of the call.
     */
    class MEGAMOLCORE_API CallRender3D : public AbstractCallRender3D, public RenderOutput {
    public:

        /**
         * Answer the name of the objects of this description.
         *
         * @return The name of the objects of this description.
         */
        static const char *ClassName(void) {
            return "CallRender3D";
        }

        /**
         * Gets a human readable description of the module.
         *
         * @return A human readable description of the module.
         */
        static const char *Description(void) {
            return "Call for rendering a frame";
        }

        /**
         * Answer the number of functions used for this call.
         *
         * @return The number of functions used for this call.
         */
        static unsigned int FunctionCount(void) {
            return 3;
        }

        /**
         * Answer the name of the function used for this call.
         *
         * @param idx The index of the function to return it's name.
         *
         * @return The name of the requested function.
         */
        static const char * FunctionName(unsigned int idx) {
            switch (idx) {
                case 0: return "Render";
                case 1: return "GetExtents";
                case 2: return "GetCapabilities";
                default: return NULL;
            }
        }

        /** Ctor. */
        CallRender3D(void);

        /** Dtor. */
        virtual ~CallRender3D(void);

        /**
         * Assignment operator
         *
         * @param rhs The right hand side operand
         *
         * @return A reference to this
         */
        CallRender3D& operator=(const CallRender3D& rhs);

    };
#ifdef _WIN32
#pragma warning(default: 4250)
#endif /* _WIN32 */


    /** Description class typedef */
    typedef CallAutoDescription<CallRender3D> CallRender3DDescription;


} /* end namespace view */
} /* end namespace core */
} /* end namespace megamol */

#endif /* MEGAMOLCORE_CALLRENDER3D_H_INCLUDED */
