/*
 * Note: this header is a private implementation header, clients of
 *       the at-spi interfaces or the C bindings in libcspi should
 *       not depend on any of the information in this file directly,
 *       as it it not intended for use as a public interface.
 *
 *    You have been warned !
 */

#ifndef _SPI_IMPL_H_
#define _SPI_IMPL_H_

#include <orbit/orbit.h>
#include "Accessibility.h"

typedef unsigned int boolean;

typedef CORBA_Object Accessible;
typedef CORBA_Object AccessibleAction;
typedef CORBA_Object AccessibleApplication;
typedef CORBA_Object AccessibleComponent;
typedef CORBA_Object AccessibleEditableText;
typedef CORBA_Object AccessibleHyperlink;
typedef CORBA_Object AccessibleHypertext;
typedef CORBA_Object AccessibleImage;
typedef CORBA_Object AccessibleRelation;
typedef CORBA_Object AccessibleSelection;
typedef CORBA_Object AccessibleStateSet;
typedef CORBA_Object AccessibleTable;
typedef CORBA_Object AccessibleText;
typedef CORBA_Object AccessibleValue;
typedef CORBA_Object AccessibilityRegistry;
typedef CORBA_Object GenericInterface;

#endif
