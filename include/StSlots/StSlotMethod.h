/**
 * Copyright © 2009-2011 Kirill Gavrilov <kirill@sview.ru>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file license-boost.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef __StSlotMethod_h_
#define __StSlotMethod_h_

#include "StSlotTypes.h"
#include "StSlot.h"

/**
 * This is an implementation template for class StSlot
 * designed for callback Slot is a non static method of some class.
 *
 * This is the main StSlot implementation which provides full compile-time
 * types check and thus - types safe. However it uses C++ methods types
 * which has compiler-dependent implementation.
 *
 * Class implements ::call() methods for all possible arguments list,
 * however only one of them will be callable from StSlot interface.
 */
template<typename class_t, typename slotMethod_t>
class StSlotMethod : public StSlot<slotMethod_t> {

        public:

    typedef StSlotTypes<class_t, slotMethod_t> types; //!< fast link to all useful types
    typedef typename types::method_t method_t;        //!< fast link to method type definition

        private:

    class_t* myClassPtr;  //!< pointer to the class instance
    method_t myMethodPtr; //!< pointer to the method (not static!) within the class

    bool isValidInline() const {
        return myClassPtr != NULL && myMethodPtr != NULL;
    }

        public:

    /**
     * Main constructor.
     */
    StSlotMethod(class_t* theClassPtr, method_t theMethodPtr)
    : StSlot<slotMethod_t>(),
      myClassPtr(theClassPtr),
      myMethodPtr(theMethodPtr) {}

    virtual ~StSlotMethod() {}

    /**
     * Validate the callback Slot.
     * @return true if pointer to the class instance and it's method are set.
     */
    virtual bool isValid() const {
        return isValidInline();
    }

    bool call() const {
        if(isValidInline()) {
            (myClassPtr->*myMethodPtr)();
            return true;
        }
        return false;
    }

    bool call(typename types::arg1_t arg1) const {
        if(isValidInline()) {
            (myClassPtr->*myMethodPtr)(arg1);
            return true;
        }
        return false;
    }

    bool call(typename types::arg1_t arg1,
              typename types::arg2_t arg2) const {
        if(isValidInline()) {
            (myClassPtr->*myMethodPtr)(arg1, arg2);
            return true;
        }
        return false;
    }

    bool call(typename types::arg1_t arg1,
              typename types::arg2_t arg2,
              typename types::arg3_t arg3) const {
        if(isValidInline()) {
            (myClassPtr->*myMethodPtr)(arg1, arg2, arg3);
            return true;
        }
        return false;
    }

};

#endif //__StSlotMethod_h_