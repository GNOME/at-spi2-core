/*
 * A load of opaque handles that people can't poke at.
 */
#ifndef _SPI_IMPL_H_
#define _SPI_IMPL_H_

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _Accessible AccessibleUnknown;
typedef AccessibleUnknown  Accessible;

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

typedef void AccessibleEventListener;
typedef void AccessibleKeystrokeListener;

typedef unsigned int SPIBoolean;


#ifdef  __cplusplus
}
#endif

#endif
