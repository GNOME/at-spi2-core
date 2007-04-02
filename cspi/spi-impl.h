/*
 * AT-SPI - Assistive Technology Service Provider Interface
 * (Gnome Accessibility Project; http://developer.gnome.org/projects/gap)
 *
 * Copyright 2001, 2002 Sun Microsystems Inc.,
 * Copyright 2001, 2002 Ximian, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

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
typedef Accessible AccessibleCollection;
typedef Accessible AccessibleComponent;
typedef Accessible AccessibleDocument;
typedef Accessible AccessibleEditableText;
typedef Accessible AccessibleHyperlink;
typedef Accessible AccessibleHypertext;
typedef Accessible AccessibleImage;
typedef Accessible AccessibleMatchRule;
typedef Accessible AccessibleRelation;
typedef Accessible AccessibleSelection;
typedef Accessible AccessibleStreamableContent;
typedef Accessible AccessibleTable;
typedef Accessible AccessibleText;
typedef Accessible AccessibleValue;
typedef Accessible AccessibilityRegistry;

/** 
 * AccessibleStateSet:
 * @ref_count: private
 * @states: private
 *
 * An opaque structure representing an accessible object's state,
 * which can then be queried via AccessibleStateSet APIs.
 **/
typedef void AccessibleStateSet;
typedef void AccessibleEventListener;
typedef void AccessibleKeystrokeListener;
typedef void AccessibleDeviceListener;

typedef unsigned int SPIBoolean;
#define SPI_FALSE (0)
#define SPI_TRUE (!SPI_FALSE)

#ifdef  __cplusplus
}
#endif

#endif
