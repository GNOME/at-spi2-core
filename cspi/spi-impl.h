/*
 * A load of opaque handles that people can't poke at.
 */
#ifndef _SPI_IMPL_H_
#define _SPI_IMPL_H_

typedef struct _Accessible Accessible;
typedef Accessible AccessibleAction;
typedef Accessible AccessibleApplication;
typedef Accessible AccessibleComponent;
typedef Accessible AccessibleEditableText;
typedef Accessible AccessibleHyperlink;
typedef Accessible AccessibleHypertext;
typedef Accessible AccessibleImage;
typedef Accessible AccessibleRelation;
typedef Accessible AccessibleSelection;
typedef Accessible AccessibleStateSet;
typedef Accessible AccessibleTable;
typedef Accessible AccessibleText;
typedef Accessible AccessibleValue;
typedef Accessible AccessibilityRegistry;
typedef Accessible GenericInterface;

typedef struct _CSpiEventListener     AccessibleEventListener;
typedef struct _CSpiKeystrokeListener AccessibleKeystrokeListener;

typedef unsigned int boolean;

#endif
