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

#include <stdlib.h>
#include <orbit/orbit.h>
#include "Accessibility.h"

typedef unsigned int boolean;

typedef CORBA_Object SpiAccessible;
typedef CORBA_Object SpiAccessibleAction;
typedef CORBA_Object SpiAccessibleApplication;
typedef CORBA_Object SpiAccessibleComponent;
typedef CORBA_Object SpiAccessibleEditableText;
typedef CORBA_Object SpiAccessibleHyperlink;
typedef CORBA_Object SpiAccessibleHypertext;
typedef CORBA_Object SpiAccessibleImage;
typedef CORBA_Object SpiAccessibleRelation;
typedef CORBA_Object SpiAccessibleSelection;
typedef CORBA_Object SpiAccessibleStateSet;
typedef CORBA_Object SpiAccessibleTable;
typedef CORBA_Object SpiAccessibleText;
typedef CORBA_Object SpiAccessibleValue;
typedef CORBA_Object AccessibilityRegistry;
typedef CORBA_Object GenericInterface;

#endif
