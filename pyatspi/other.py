#Copyright (C) 2008 Codethink Ltd

#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License version 2 as published by the Free Software Foundation.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#You should have received a copy of the GNU Lesser General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

from base import *
from accessible import *

class Action(BaseProxy):
    
    
    """
    An interface through which a user-actionable user interface component
    can be manipulated. Components which react to mouse or keyboard
    input from the user, (with the exception of pure text entry fields
    with no other function), should implement this interface. Typical
    actions include "click", "press", "release" (for instance for
    buttons), "menu" (for objects which have context menus invokable
    from mouse or keyboard), "open" for icons representing files
    folders, and others.
    """
    
    
    def doAction(self, *args, **kwargs):
        """
        doAction: 
        @param : index
        the 0-based index of the action to perform.
        Causes the object to perform the specified action.
        @return : a boolean indicating success or failure.
        """
        func = self.get_dbus_method("doAction")
        return func(*args, **kwargs)
    
    def getDescription(self, *args, **kwargs):
        """
        getDescription: 
        @param : index
        the index of the action for which a description is desired.
        Get the description of the specified action. The description
        of an action may provide information about the result of action
        invocation, unlike the action name. 
        @return : a string containing the description of the specified
        action.
        """
        func = self.get_dbus_method("getDescription")
        return func(*args, **kwargs)
    
    def getKeyBinding(self, *args, **kwargs):
        """
        getKeyBinding: 
        @param : index
        the 0-based index of the action for which a key binding is requested.
        Get the key binding associated with a specific action.
        @return : a string containing the key binding for the specified
        action, or an empty string ("") if none exists.
        """
        func = self.get_dbus_method("getKeyBinding")
        return func(*args, **kwargs)
    
    def getName(self, *args, **kwargs):
        """
        getName: 
        @param : index
        the index of the action whose name is requested.
        Get the name of the specified action. Action names generally
        describe the user action, i.e. "click" or "press", rather then
        the result of invoking the action.
        @return : a string containing the name of the specified action.
        """
        func = self.get_dbus_method("getName")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)
    
    def get_nActions(self):
        self._pgetter(self._dbus_interface, "nActions")
    def set_nActions(self, value):
        self._psetter(self._dbus_interface, "nActions", value)
    _nActionsDoc = \
        """
        nActions: a long containing the number of actions this object
        supports.
        """
    nActions = property(fget=get_nActions, fset=set_nActions, doc=_nActionsDoc)


class Application(Accessible):
    
    
    """
    An interface identifying an object which is the root of the user
    interface Accessible hierarchy associated with a running application.
    Children of Application are typically, but not exclusively, top-level
    windows.
    """
    
    
    def getLocale(self, *args, **kwargs):
        """
        Gets the locale in which the application is currently operating.
        For the current message locale, use lctype LOCALE_TYPE_MESSAGES.
        @param : lctype
        The LocaleType for which the locale is queried. 
        @return a string compliant with the POSIX standard for locale
        description.
        """
        func = self.get_dbus_method("getLocale")
        return func(*args, **kwargs)
    
    def pause(self, *args, **kwargs):
        """
        Request that the application temporarily stop sending events.
        In most cases this should pause the application's main event
        loop.
        @return : true if the request succeeded, false otherwise.
        """
        func = self.get_dbus_method("pause")
        return func(*args, **kwargs)
    
    def registerObjectEventListener(self, *args, **kwargs):
        """
        registerObjectEventListener: 
        @param : listener
        an EventListener object which will receive the requested events
        @param : eventName
        a UTF-8 string indicating the type of (toolkit-specific) event
        being requested. Register with this application toolkit for "Accessibility::Accessible"
        event notifications.
        """
        func = self.get_dbus_method("registerObjectEventListener")
        return func(*args, **kwargs)
    
    def registerToolkitEventListener(self, *args, **kwargs):
        """
        @param : listener
        an EventListener object which will receive the requested events
        from the application's toolkits via toolit 'bridges' 
        @param : eventName
        a UTF-8 string indicating the type of (toolkit-specific) event
        being requested. Not all applications can generate toolkit events
        of a given type.
        Register with this application's toolkit for "toolkit-specific"
        event notifications.
        """
        func = self.get_dbus_method("registerToolkitEventListener")
        return func(*args, **kwargs)
    
    def resume(self, *args, **kwargs):
        """
        Request that the application resume sending events.
        @return : True if the request succeeded, False otherwise.
        """
        func = self.get_dbus_method("resume")
        return func(*args, **kwargs)
    
    def unImplemented2_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2_")
        return func(*args, **kwargs)
    
    def unImplemented3_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3_")
        return func(*args, **kwargs)
    
    def unImplemented_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented_")
        return func(*args, **kwargs)
    
    def get_id(self):
        self._pgetter(self._dbus_interface, "id")
    def set_id(self, value):
        self._psetter(self._dbus_interface, "id", value)
    _idDoc = \
        """
        The application instance's unique ID as assigned by the registry.
        """
    id = property(fget=get_id, fset=set_id, doc=_idDoc)
    
    def get_toolkitName(self):
        self._pgetter(self._dbus_interface, "toolkitName")
    def set_toolkitName(self, value):
        self._psetter(self._dbus_interface, "toolkitName", value)
    _toolkitNameDoc = \
        """
        A string indicating the type of user interface toolkit which
        is used by the application.
        """
    toolkitName = property(fget=get_toolkitName, fset=set_toolkitName, doc=_toolkitNameDoc)
    
    def get_version(self):
        self._pgetter(self._dbus_interface, "version")
    def set_version(self, value):
        self._psetter(self._dbus_interface, "version", value)
    _versionDoc = \
        """
        A string indicating the version number of the application's accessibility
        bridge implementation.
        """
    version = property(fget=get_version, fset=set_version, doc=_versionDoc)


class BoundingBox(list):
    def __new__(cls, x, y, width, height):
        list.__new__(cls, (x, y, width, height))
    def __init__(self, x, y, width, height):
        list.__init__(self, (x, y, width, height))
    
    def _get_x(self):
        return self[0]
    def _set_x(self, val):
        self[0] = val
    x = property(fget=_get_x, fset=_set_x)
    def _get_y(self):
        return self[1]
    def _set_y(self, val):
        self[1] = val
    y = property(fget=_get_y, fset=_set_y)
    def _get_width(self):
        return self[2]
    def _set_width(self, val):
        self[2] = val
    width = property(fget=_get_width, fset=_set_width)
    def _get_height(self):
        return self[3]
    def _set_height(self, val):
        self[3] = val
    height = property(fget=_get_height, fset=_set_height)


class Collection(BaseProxy):
    
    def createMatchRule(self, *args, **kwargs):
        func = self.get_dbus_method("createMatchRule")
        return func(*args, **kwargs)
    
    def freeMatchRule(self, *args, **kwargs):
        func = self.get_dbus_method("freeMatchRule")
        return func(*args, **kwargs)
    
    def getActiveDescendant(self, *args, **kwargs):
        func = self.get_dbus_method("getActiveDescendant")
        return func(*args, **kwargs)
    
    def getMatches(self, *args, **kwargs):
        func = self.get_dbus_method("getMatches")
        return func(*args, **kwargs)
    
    def getMatchesFrom(self, *args, **kwargs):
        func = self.get_dbus_method("getMatchesFrom")
        return func(*args, **kwargs)
    
    def getMatchesTo(self, *args, **kwargs):
        func = self.get_dbus_method("getMatchesTo")
        return func(*args, **kwargs)
    
    def isAncestorOf(self, *args, **kwargs):
        func = self.get_dbus_method("isAncestorOf")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)

    class MatchType(Enum):
        _enum_lookup = {
            0:'MATCH_INVALID',
            1:'MATCH_ALL',
            2:'MATCH_ANY',
            3:'MATCH_NONE',
            4:'MATCH_EMPTY',
            5:'MATCH_LAST_DEFINED',
        }
    
    MATCH_ALL = MatchType(1)
    
    MATCH_ANY = MatchType(2)
    
    MATCH_EMPTY = MatchType(4)
    
    MATCH_INVALID = MatchType(0)
    
    MATCH_LAST_DEFINED = MatchType(5)
    
    MATCH_NONE = MatchType(3)

    class SortOrder(Enum):
        _enum_lookup = {
            0:'SORT_ORDER_INVALID',
            1:'SORT_ORDER_CANONICAL',
            2:'SORT_ORDER_FLOW',
            3:'SORT_ORDER_TAB',
            4:'SORT_ORDER_REVERSE_CANONICAL',
            5:'SORT_ORDER_REVERSE_FLOW',
            6:'SORT_ORDER_REVERSE_TAB',
            7:'SORT_ORDER_LAST_DEFINED',
        }
    
    SORT_ORDER_CANONICAL = SortOrder(1)
    
    SORT_ORDER_FLOW = SortOrder(2)
    
    SORT_ORDER_INVALID = SortOrder(0)
    
    SORT_ORDER_LAST_DEFINED = SortOrder(7)
    
    SORT_ORDER_REVERSE_CANONICAL = SortOrder(4)
    
    SORT_ORDER_REVERSE_FLOW = SortOrder(5)
    
    SORT_ORDER_REVERSE_TAB = SortOrder(6)
    
    SORT_ORDER_TAB = SortOrder(3)

    class TreeTraversalType(Enum):
        _enum_lookup = {
            0:'TREE_RESTRICT_CHILDREN',
            1:'TREE_RESTRICT_SIBLING',
            2:'TREE_INORDER',
            3:'TREE_LAST_DEFINED',
        }
    
    TREE_INORDER = TreeTraversalType(2)
    
    TREE_LAST_DEFINED = TreeTraversalType(3)
    
    TREE_RESTRICT_CHILDREN = TreeTraversalType(0)
    
    TREE_RESTRICT_SIBLING = TreeTraversalType(1)

class Command(list):
    def __new__(cls, name, id):
        list.__new__(cls, (name, id))
    def __init__(self, name, id):
        list.__init__(self, (name, id))
    
    def _get_name(self):
        return self[0]
    def _set_name(self, val):
        self[0] = val
    name = property(fget=_get_name, fset=_set_name)
    def _get_id(self):
        return self[1]
    def _set_id(self, val):
        self[1] = val
    id = property(fget=_get_id, fset=_set_id)

class CommandListener(BaseProxy):
    """
    An interface which should be implemented by assistive technologies
    or other clients of the Selector interface, over which notifications
    to the list of available commands is made. The notifyCommands()
    method of the client is then called by the Selector instance.
    """
    
    
    def notifyCommands(self, *args, **kwargs):
        """
        Notify the CommandListener instance of changes to the currently
        available commands, by sending the current CommandList.
        @param : commands
        The newly-available list of Command objects which may be invoked
        by the listener.
        """
        func = self.get_dbus_method("notifyCommands")
        return func(*args, **kwargs)


class Component(BaseProxy):
    
    
    """
    The Component interface is implemented by objects which occupy
    on-screen space, e.g. objects which have onscreen visual representations.
    The methods in Component allow clients to identify where the
    objects lie in the onscreen coordinate system, their relative
    size, stacking order, and position. It also provides a mechanism
    whereby keyboard focus may be transferred to specific user interface
    elements programmatically. This is a 2D API, coordinates of 3D
    objects are projected into the 2-dimensional screen view for
    purposes of this interface.
    """
    
    
    def contains(self, *args, **kwargs):
        """
        @return True if the specified point lies within the Component's
        bounding box, False otherwise.
        """
        func = self.get_dbus_method("contains")
        return func(*args, **kwargs)
    
    def deregisterFocusHandler(self, *args, **kwargs):
        """
        Request that an EventListener registered via registerFocusHandler
        no longer be notified when this object receives keyboard focus.
        """
        func = self.get_dbus_method("deregisterFocusHandler")
        return func(*args, **kwargs)
    
    def getAccessibleAtPoint(self, *args, **kwargs):
        """
        @return the Accessible child whose bounding box contains the
        specified point.
        """
        func = self.get_dbus_method("getAccessibleAtPoint")
        return func(*args, **kwargs)
    
    def getAlpha(self, *args, **kwargs):
        """
        Obtain the alpha value of the component. An alpha value of 1.0
        or greater indicates that the object is fully opaque, and an
        alpha value of 0.0 indicates that the object is fully transparent.
        Negative alpha values have no defined meaning at this time.
        """
        func = self.get_dbus_method("getAlpha")
        return func(*args, **kwargs)
    
    def getExtents(self, *args, **kwargs):
        """
        Obtain the Component's bounding box, in pixels, relative to the
        specified coordinate system. 
        @return a BoundingBox which entirely contains the object's onscreen
        visual representation.
        """
        func = self.get_dbus_method("getExtents")
        return func(*args, **kwargs)
    
    def getLayer(self, *args, **kwargs):
        """
        @return the ComponentLayer in which this object resides.
        """
        func = self.get_dbus_method("getLayer")
        return func(*args, **kwargs)
    
    def getMDIZOrder(self, *args, **kwargs):
        """
        Obtain the relative stacking order (i.e. 'Z' order) of an object.
        Larger values indicate that an object is on "top" of the stack,
        therefore objects with smaller MDIZOrder may be obscured by objects
        with a larger MDIZOrder, but not vice-versa. 
        @return an integer indicating the object's place in the stacking
        order.
        """
        func = self.get_dbus_method("getMDIZOrder")
        return func(*args, **kwargs)
    
    def getPosition(self, *args, **kwargs):
        """
        Obtain the position of the current component in the coordinate
        system specified by coord_type. 
        @param : coord_type
        @param : x
        an out parameter which will be back-filled with the returned
        x coordinate. 
        @param : y
        an out parameter which will be back-filled with the returned
        y coordinate.
        """
        func = self.get_dbus_method("getPosition")
        return func(*args, **kwargs)
    
    def getSize(self, *args, **kwargs):
        """
        Obtain the size, in the coordinate system specified by coord_type,
        of the rectangular area which fully contains the object's visual
        representation, without accounting for viewport clipping. 
        @param : width
        the object's horizontal extents in the specified coordinate system.
        @param : height
        the object's vertical extents in the specified coordinate system.
        """
        func = self.get_dbus_method("getSize")
        return func(*args, **kwargs)
    
    def grabFocus(self, *args, **kwargs):
        """
        Request that the object obtain keyboard focus.
        @return True if keyboard focus was successfully transferred to
        the Component.
        """
        func = self.get_dbus_method("grabFocus")
        return func(*args, **kwargs)
    
    def registerFocusHandler(self, *args, **kwargs):
        """
        Register an EventListener for notification when this object receives
        keyboard focus.
        """
        func = self.get_dbus_method("registerFocusHandler")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)


class ComponentLayer(Enum):
    _enum_lookup = {
        0:'LAYER_INVALID',
        1:'LAYER_BACKGROUND',
        2:'LAYER_CANVAS',
        3:'LAYER_WIDGET',
        4:'LAYER_MDI',
        5:'LAYER_POPUP',
        6:'LAYER_OVERLAY',
        7:'LAYER_WINDOW',
        8:'LAYER_LAST_DEFINED',
    }


class ContentStream(BaseProxy):
    """
    An interface by which the requested data from a StreamableContent
    object may be read by the client.
    """
    
    def close(self, *args, **kwargs):
        """
        close the stream and release associated resources. A client should
        not perform further operations on a StreamableContent::Stream
        object after closing it.
        """
        func = self.get_dbus_method("close")
        return func(*args, **kwargs)
    
    def read(self, *args, **kwargs):
        """
        Request/read a specified amount of data from a Stream. 
        @return the number of bytes actually read into the client buffer.
        """
        func = self.get_dbus_method("read")
        return func(*args, **kwargs)
    
    def seek(self, *args, **kwargs):
        """
        Seek to a specified position in the Stream. 
        @param : offset
        an offset specifying the requested position in the stream, relative
        to the SeekType specified in whence. 
        @param : whence
        a SeekType specifying the reference point from which the seek
        offset is calculated. Some forms of seek are not supported by
        certain implementations of Stream, in which case a NotSupported
        exception will be raised. 
        @return the actual resulting offset, if no exception was raised.
        """
        func = self.get_dbus_method("seek")
        return func(*args, **kwargs)
    
    def unimplemented(self, *args, **kwargs):
        """
        /cond
        """
        func = self.get_dbus_method("unimplemented")
        return func(*args, **kwargs)
    
    def unimplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unimplemented2")
        return func(*args, **kwargs)
    
    class IOError(Exception):
        pass
    
    class NoPermission(Exception):
        pass
    
    class NotSupported(Exception):
        pass

    class SeekType(Enum):
        """
        Specifies the meaning of a seek 'offset'. Not all SeekTypes are
        supported by all StreamableContent data sources, for instance
        some streams may not support seeking from the beginning or other
        types of 'backwards' seeks.
        """
        _enum_lookup = {
            0:'SEEK_SET',
            1:'SEEK_CURRENT',
            2:'SEEK_END',
        }
    
    SEEK_CURRENT = SeekType(1)
    
    SEEK_END = SeekType(2)
    
    SEEK_SET = SeekType(0)

class DeviceEvent(list):
    def __new__(cls, type, id, hw_code, modifiers, timestamp, event_string, is_text):
        list.__new__(cls, (type, id, hw_code, modifiers, timestamp, event_string, is_text))
    def __init__(self, type, id, hw_code, modifiers, timestamp, event_string, is_text):
        list.__init__(self, (type, id, hw_code, modifiers, timestamp, event_string, is_text))
    
    def _get_type(self):
        return self[0]
    def _set_type(self, val):
        self[0] = val
    type = property(fget=_get_type, fset=_set_type)
    def _get_id(self):
        return self[1]
    def _set_id(self, val):
        self[1] = val
    id = property(fget=_get_id, fset=_set_id)
    def _get_hw_code(self):
        return self[2]
    def _set_hw_code(self, val):
        self[2] = val
    hw_code = property(fget=_get_hw_code, fset=_set_hw_code)
    def _get_modifiers(self):
        return self[3]
    def _set_modifiers(self, val):
        self[3] = val
    modifiers = property(fget=_get_modifiers, fset=_set_modifiers)
    def _get_timestamp(self):
        return self[4]
    def _set_timestamp(self, val):
        self[4] = val
    timestamp = property(fget=_get_timestamp, fset=_set_timestamp)
    def _get_event_string(self):
        return self[5]
    def _set_event_string(self, val):
        self[5] = val
    event_string = property(fget=_get_event_string, fset=_set_event_string)
    def _get_is_text(self):
        return self[6]
    def _set_is_text(self, val):
        self[6] = val
    is_text = property(fget=_get_is_text, fset=_set_is_text)

class DeviceEventController(BaseProxy):
    """
    The interface via which clients request notification of device
    events, and through which device events may be simulated.
    """

    def deregisterDeviceEventListener(self, *args, **kwargs):
        """
        De-register a previously registered keyboard eventlistener. 
        @param : listener
        a DeviceEventListener which will intercept events. 
        @param : typeseq
        an EventTypeSeq indicating which event types to stop listening
        for.
        """
        func = self.get_dbus_method("deregisterDeviceEventListener")
        return func(*args, **kwargs)
    
    def deregisterKeystrokeListener(self, *args, **kwargs):
        """
        De-register a previously registered keyboard eventlistener. 
        @param : listener
        a DeviceEventListener which will intercept key events. 
        @param : keys
        a KeySet indicating which keys to intercept, or KEYSET_ALL_KEYS.
        @param : mask
        a ControllerEventMask filtering the intercepted key events. 
        @param : type
        an EventType mask that may created by ORing event types together.
        """
        func = self.get_dbus_method("deregisterKeystrokeListener")
        return func(*args, **kwargs)
    
    def generateKeyboardEvent(self, *args, **kwargs):
        """
        Synthesize a keyboard event. 
        @param : keycode
        a long integer indicating the keycode of the keypress to be synthesized.
        @param : keystring
        an optional UTF-8 string indicating a complex keyboard input
        event. 
        @param : type
        a KeySynthType indicating the type of event(s) to be synthesized:
        a key press, release, press-release pair, or a complex input
        string (for instance from an internationalized or complex text
        input method, or a composed character).
        """
        func = self.get_dbus_method("generateKeyboardEvent")
        return func(*args, **kwargs)
    
    def generateMouseEvent(self, *args, **kwargs):
        """
        Synthesize a mouse event. 
        @param : x
        a long integer indicating the screen x coord for the mouse event.
        @param : y
        a long integer indicating the screen y coord for the mouse event.
        @param : eventName
        a string indicating the type of mouse event, e.g. "button1up"
        """
        func = self.get_dbus_method("generateMouseEvent")
        return func(*args, **kwargs)
    
    def notifyListenersAsync(self, *args, **kwargs):
        """
        Notify the Registry instance that a device event has taken place
        in an asynchronous manner. This is the method used by accessibility
        bridges to forward "toolkit dependent" device events to the Registry
        from the application's process space. If the event in question
        is potentially pre-emptible. notifyListenersSync should be used
        instead.
        """
        func = self.get_dbus_method("notifyListenersAsync")
        return func(*args, **kwargs)
    
    def notifyListenersSync(self, *args, **kwargs):
        """
        Notify the Registry instance that a device event has taken place,
        and allow pre-emptive listeners the opportunity to 'consume'
        the event and thus prevent its further issuance/forwarding. This
        is the method used by accessibility bridges to forward "toolkit
        dependent" device events to the Registry from the application's
        process space.
        @return True if the event was consumed by a (pre-emptive) listener,
        False if not (in which case the device event will be forwarded
        as normal to any application which would normally receive it,
        e.g. the currently active application in the case of mouse or
        keyboard events).
        """
        func = self.get_dbus_method("notifyListenersSync")
        return func(*args, **kwargs)
    
    def registerDeviceEventListener(self, *args, **kwargs):
        """
        Register to intercept events, and either pass them on or consume
        them. To listen to keyboard events use registerKeystrokeListener
        instead. 
        @param : listener
        a DeviceEventListener which will intercept events. 
        @param : typeseq
        an EventTypeSeq indicating which event types to listen for. 
        @return True if successful, False if not
        """
        func = self.get_dbus_method("registerDeviceEventListener")
        return func(*args, **kwargs)
    
    def registerKeystrokeListener(self, *args, **kwargs):
        """
        Register to intercept keyboard events, and either pass them on
        or consume them.
        @param : listener
        a DeviceEventListener which will intercept key events. 
        @param : keys
        a KeySet indicating which keys to intercept, or KEYSET_ALL_KEYS.
        @param : mask
        a ControllerEventMask filtering the intercepted key events. 
        @param : type
        a KeyEventTypeSeq that may created by ORing event types together.
        @param : mode
        an EventListenerMode indicating whether the listener should receive
        the events synchronously, potentially consuming them, or just
        be notified asynchronously of those events that have been generated.
        @return True if the DeviceEventListener was successfully registered
        for the requested KeySet, ControllerEventMask, event types, and
        EventListenerMode; otherwise returns False.
        """
        func = self.get_dbus_method("registerKeystrokeListener")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)


class DeviceEventListener(BaseProxy):
    """
    This interface should be implemented by AT-SPI clients who wish
    to make use of the DeviceEventController to receive device event
    notifications. DeviceEvents include keyboard events and mouse
    button/motion events.
    """
    
    def notifyEvent(self, *args, **kwargs):
        """
        Notify an interested DeviceEventListener that a DeviceEvent has
        occurred. 
        @return True if the recipient/consumer wishes to consume the
        event, i.e. prevent it from being delivered to the desktop, False
        if the event should continue to be delivered as normal.
        """
        func = self.get_dbus_method("notifyEvent")
        return func(*args, **kwargs)
    
    def unImplemented_2_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented_2_")
        return func(*args, **kwargs)
    
    def unImplemented_3_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented_3_")
        return func(*args, **kwargs)
    
    def unImplemented_4_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented_4_")
        return func(*args, **kwargs)
    
    def unImplemented_5_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented_5_")
        return func(*args, **kwargs)
    
    def unImplemented_6_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented_6_")
        return func(*args, **kwargs)
    
    def unImplemented__(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented__")
        return func(*args, **kwargs)


class Document(BaseProxy):
    """
    Primarily a 'tagging' interface which indicates the start of
    document content in the Accessibility hierarchy. Accessible objects
    below the node implementing Document are normally assumed to
    be part of the document content. Attributes of Document are those
    attributes associated with the document as a whole. Objects that
    implement Document are normally expected to implement Collection
    as well.
    """
    
    def getAttributeValue(self, *args, **kwargs):
        """
        Gets the value of a single attribute, if specified for the document
        as a whole.
        @param : attributename
        a string indicating the name of a specific attribute (name-value
        pair) being queried.
        @return a string corresponding to the value of the specified
        attribute, or an empty string if the attribute is unspecified
        for the object.
        """
        func = self.get_dbus_method("getAttributeValue")
        return func(*args, **kwargs)
    
    def getAttributes(self, *args, **kwargs):
        """
        Gets all attributes specified for a document as a whole. For
        attributes which change within the document content, see Accessibility::Text::getAttributes
        instead.
        @return an AttributeSet containing the attributes of the document,
        as name-value pairs.
        """
        func = self.get_dbus_method("getAttributes")
        return func(*args, **kwargs)
    
    def getLocale(self, *args, **kwargs):
        """
        Gets the locale associated with the document's content. e.g.
        the locale for LOCALE_TYPE_MESSAGES.
        @return a string compliant with the POSIX standard for locale
        description.
        """
        func = self.get_dbus_method("getLocale")
        return func(*args, **kwargs)
    
    def unImplemented2_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2_")
        return func(*args, **kwargs)
    
    def unImplemented3_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3_")
        return func(*args, **kwargs)
    
    def unImplemented4_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4_")
        return func(*args, **kwargs)
    
    def unImplemented_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented_")
        return func(*args, **kwargs)

class Text(BaseProxy):
    """
    The text interface should be implemented by objects which place
    textual information onscreen as character strings or glyphs.
    The text interface allows access to textual content, including
    display attributes and semantic hints associated with runs of
    text, and access to bounding box information for glyphs and substrings.
    It also allows portions of textual content to be selected, if
    the object's StateSet includes STATE_SELECTABLE_TEXT.
    In some cases a Text object may have, as its content, an empty
    string. In particular this can occur in the case of Hypertext
    objects which do not display explicitly textual information onscreen,
    as Hypertext is derived from the Text interface. 
    Typographic and semantic attributes of onscreen textual content,
    for instance typeface, weight, language, and such qualities as
    'emphasis' or 'blockquote', are represented as text attributes.
    Contiguous sequences of characters over which these attributes
    are unchanged are referred to as "attribute runs", and are available
    via Text::getAttributeRun. Where possible, implementing clients
    will report textual attributes which are the same over the entire
    text object, for instance those inherited from a default or document-scope
    style, via getDefaultAttributes instead of reporting them explicitly
    for each character. Therefore, for any span of text, the attributes
    in effect are the union of the set returned by Text::getDefaultAttributes,
    and the set returned at a particular character offset via Text::getAttributeRun.
    """
    
    
    def addSelection(self, *args, **kwargs):
        """
        The result of calling addSelection on objects which already have
        one selection present, and which do not include STATE_MULTISELECTABLE,
        is undefined, other than the return value. 
        @return True of the selection was successfully added, False otherwise.
        Selection may fail if the object does not support selection of
        text (see STATE_SELECTABLE_TEXT), if the object does not support
        multiple selections and a selection is already defined, or for
        other reasons (for instance if the user does not have permission
        to copy the text into the relevant selection buffer).
        """
        func = self.get_dbus_method("addSelection")
        return func(*args, **kwargs)
    
    def getAttributeRun(self, *args, **kwargs):
        """
        Query a particular text object for the text attributes defined
        at a given offset, obtaining the start and end of the "attribute
        run" over which these attributes are currently invariant. Text
        attributes are those presentational, typographic, or semantic
        attributes or qualitites which apply to a range of text specifyable
        by starting and ending offsets. Attributes relevant to localization
        should be provided in accordance with the w3c "Internationalization
        and Localization Markup Requirements", http://www.w3.org/TR/2005/WD-itsreq-20051122/
        Other text attributes should choose their names and value semantics
        in accordance with relevant standards such as CSS level 2 (http://www.w3.org/TR/1998/REC-CSS2-19980512),
        XHTML 1.0 (http://www.w3.org/TR/2002/REC-xhtml1-20020801), and
        WICD (http://www.w3.org/TR/2005/WD-WICD-20051121/). Those attributes
        from the aforementioned specifications and recommendations which
        do not concern typographic, presentational, or semantic aspects
        of text should be exposed via the more general Accessible::getAttributes()
        API (if at all).
        For example, CSS attributes which should be exposed on text (either
        as default attributes, or as explicitly-set attributes when non-default
        values are specified in the content view) include the Font attributes
        (i.e. "css2:font-weight", "css2:font-style"), the "css2:color"
        and "css2:background-color" attributes, and "css2:text-decoration"
        attribute.
        If includeDefaults is TRUE, then this AttributeSet should include
        the default attributes as well as those which are explicitly
        assigned to the attribute run in question. startOffset and endOffset
        will be back-filled to indicate the start and end of the attribute
        run which contains 'offset' - an attribute run is a contiguous
        section of text whose attributes are homogeneous. 
        @param : offset
        the offset of the character whose attributes will be reported.
        @param : startOffset
        backfilled with the starting offset of the character range over
        which all text attributes match those of offset, i.e. the start
        of the homogeneous attribute run including offset. 
        @param : endOffset
        backfilled with the offset of the first character past the character
        range over which all text attributes match those of offset, i.e.
        the character immediately after the homogeneous attribute run
        including offset. 
        @param : includeDefaults
        if False, the call should only return those attributes which
        are explicitly set on the current attribute run, omitting any
        attributes which are inherited from the default values. See also
        Text::getDefaultAttributes.
        @return the AttributeSet defined at offset, optionally including
        the 'default' attributes.
        """
        func = self.get_dbus_method("getAttributeRun")
        return func(*args, **kwargs)
    
    def getAttributeValue(self, *args, **kwargs):
        """
        Get the string value of a named attribute at a given offset,
        if defined. 
        @param : offset
        the offset of the character for which the attribute run is to
        be obtained. 
        @param : attributeName
        the name of the attribute for which the value is to be returned,
        if defined. 
        @param : startOffset
        back-filled with the offset of the first character in the attribute
        run containing the character at offset. 
        @param : endOffset
        back-filled with the offset of the first character past the end
        of the attribute run containing the character at offset. 
        @param : defined
        back-filled with True if the attributeName has a defined value
        at offset, False otherwise. 
        @return the value of attribute (name-value pair) corresponding
        to "name", if defined.
        """
        func = self.get_dbus_method("getAttributeValue")
        return func(*args, **kwargs)
    
    def getAttributes(self, *args, **kwargs):
        """
        getAttributes is deprecated in favor of getAttributeRun. 
        @return the attributes at offset, as a semicolon-delimited set
        of colon-delimited name-value pairs.
        """
        func = self.get_dbus_method("getAttributes")
        return func(*args, **kwargs)
    
    def getBoundedRanges(self, *args, **kwargs):
        """
        Return the text content within a bounding box, as a list of Range
        structures. Depending on the TEXT_CLIP_TYPE parameters, glyphs
        which are clipped by the bounding box (i.e. which lie partially
        inside and partially outside it) may or may not be included in
        the ranges returned. 
        @param : x
        the minimum x ( i.e. leftmost) coordinate of the bounding box.
        @param : y
        the minimum y coordinate of the bounding box. 
        @param : width
        the horizontal size of the bounding box. The rightmost bound
        of the bounding box is (x + width); 
        @param : height
        the vertical size of the bounding box. The maximum y value of
        the bounding box is (y + height); 
        @param : coordType
        If 0, the above coordinates are interpreted as pixels relative
        to corner of the screen; if 1, the coordinates are interpreted
        as pixels relative to the corner of the containing toplevel window.
        @param : xClipType
        determines whether text which intersects the bounding box in
        the x direction is included. 
        @param : yClipType
        determines whether text which intersects the bounding box in
        the y direction is included.
        """
        func = self.get_dbus_method("getBoundedRanges")
        return func(*args, **kwargs)
    
    def getCharacterAtOffset(self, *args, **kwargs):
        """
        @return an unsigned long integer whose value corresponds to the
        UCS-4 representation of the character at the specified text offset,
        or 0 if offset is out of range.
        """
        func = self.get_dbus_method("getCharacterAtOffset")
        return func(*args, **kwargs)
    
    def getCharacterExtents(self, *args, **kwargs):
        """
        Obtain a the bounding box, as x, y, width, and height, of the
        character or glyph at a particular character offset in this object's
        text content. The coordinate system in which the results are
        reported is specified by coordType. If an onscreen glyph corresponds
        to multiple character offsets, for instance if the glyph is a
        ligature, the bounding box reported will include the entire glyph
        and therefore may apply to more than one character offset. 
        @param : offset
        the character offset of the character or glyph being queried.
        @param : x
        the minimum horizontal coordinate of the bounding box of the
        glyph representing the character at offset. 
        @param : y
        the minimum vertical coordinate of the bounding box of the glyph
        representing the character at offset. 
        @param : width
        the horizontal extent of the bounding box of the glyph representing
        the character at offset. 
        @param : height
        the vertical extent of the bounding box of the glyph representing
        the character at offset. 
        @param : coordType
        If 0, the results will be reported in screen coordinates, i.e.
        in pixels relative to the upper-left corner of the screen, with
        the x axis pointing right and the y axis pointing down. If 1,
        the results will be reported relative to the containing toplevel
        window, with the x axis pointing right and the y axis pointing
        down.
        """
        func = self.get_dbus_method("getCharacterExtents")
        return func(*args, **kwargs)
    
    def getDefaultAttributeSet(self, *args, **kwargs):
        """
        Return an AttributeSet containing the text attributes which apply
        to all text in the object by virtue of the default settings of
        the document, view, or user agent; e.g. those attributes which
        are implied rather than explicitly applied to the text object.
        For instance, an object whose entire text content has been explicitly
        marked as 'bold' will report the 'bold' attribute via getAttributeRun(),
        whereas an object whose text weight is inspecified may report
        the default or implied text weight in the default AttributeSet.
        """
        func = self.get_dbus_method("getDefaultAttributeSet")
        return func(*args, **kwargs)
    
    def getDefaultAttributes(self, *args, **kwargs):
        """
        Deprecated in favor of getDefaultAttributeSet. 
        @return the attributes which apply to the entire text content,
        but which were not explicitly specified by the content creator.
        """
        func = self.get_dbus_method("getDefaultAttributes")
        return func(*args, **kwargs)
    
    def getNSelections(self, *args, **kwargs):
        """
        Obtain the number of separate, contiguous selections in the current
        Text object. Text objects which do not implement selection of
        discontiguous text regions will always return '0' or '1'. Note
        that "contiguous" is defined by continuity of the offsets, i.e.
        a text 'selection' is defined by a start/end offset pair. In
        the case of bidirectional text, this means that a continguous
        selection may appear visually discontiguous, and vice-versa.
        @return the number of contiguous selections in the current Text
        object.
        """
        func = self.get_dbus_method("getNSelections")
        return func(*args, **kwargs)
    
    def getOffsetAtPoint(self, *args, **kwargs):
        """
        Get the offset of the character at a given onscreen coordinate.
        The coordinate system used to interpret x and y is determined
        by parameter coordType. 
        @param : x
        @param : y
        @param : coordType
        if 0, the input coordinates are interpreted relative to the entire
        screen, if 1, they are relative to the toplevel window containing
        this Text object. 
        @return the text offset (as an offset into the character array)
        of the glyph whose onscreen bounds contain the point x,y, or
        -1 if the point is outside the bounds of any glyph.
        """
        func = self.get_dbus_method("getOffsetAtPoint")
        return func(*args, **kwargs)
    
    def getRangeExtents(self, *args, **kwargs):
        """
        Obtain the bounding box which entirely contains a given text
        range. Negative values may be returned for the bounding box parameters
        in the event that all or part of the text range is offscreen
        or not mapped to the screen. 
        @param : startOffset
        the offset of the first character in the specified range. 
        @param : endOffset
        the offset of the character immediately after the last character
        in the specified range. 
        @param : x
        an integer parameter which is back-filled with the minimum horizontal
        coordinate of the resulting bounding box. 
        @param : y
        an integer parameter which is back-filled with the minimum vertical
        coordinate of the resulting bounding box. 
        @param : width
        an integer parameter which is back-filled with the horizontal
        extent of the bounding box. 
        @param : height
        an integer parameter which is back-filled with the vertical extent
        of the bounding box. 
        @param : coordType
        If 0, the above coordinates are reported in pixels relative to
        corner of the screen; if 1, the coordinates are reported relative
        to the corner of the containing toplevel window.
        """
        func = self.get_dbus_method("getRangeExtents")
        return func(*args, **kwargs)
    
    def getSelection(self, *args, **kwargs):
        """
        The result of calling getSelection with an out-of-range selectionNum
        (i.e. for a selection which does not exist) is not strictly defined,
        but should set endOffset equal to startOffset.
        """
        func = self.get_dbus_method("getSelection")
        return func(*args, **kwargs)
    
    def getText(self, *args, **kwargs):
        """
        Obtain all or part of the onscreen textual content of a Text
        object. If endOffset is specified as "-1", then this method will
        return the entire onscreen textual contents of the Text object.
        @return the textual content of the current Text object beginning
        startOffset (inclusive) up to but not including the character
        at endOffset.
        """
        func = self.get_dbus_method("getText")
        return func(*args, **kwargs)
    
    def getTextAfterOffset(self, *args, **kwargs):
        """
        Obtain a subset of the text content of an object which entirely
        follows offset, delimited by character, word, line, or sentence
        boundaries as specified by type. The starting and ending offsets
        of the resulting substring are returned in startOffset and endOffset.
        By definition, if such a substring exists, startOffset must be
        greater than offset. 
        @param : offset
        the offset from which the substring search begins, and which
        must lie before the returned substring. 
        @param : type
        the text-boundary delimiter which determines whether the returned
        text constitures a character, word, line, or sentence (and possibly
        attendant whitespace), and whether the start or ending of such
        a substring forms the boundary condition. 
        @param : startOffset
        back-filled with the starting offset of the resulting substring,
        if one exists. 
        @param : endOffset
        back-filled with the offset of the character immediately following
        the resulting substring, if one exists. 
        @return a string which is a substring of the text content of
        the object, delimited by the specified boundary condition.
        """
        func = self.get_dbus_method("getTextAfterOffset")
        return func(*args, **kwargs)
    
    def getTextAtOffset(self, *args, **kwargs):
        """
        Obtain a subset of the text content of an object which includes
        the specified offset, delimited by character, word, line, or
        sentence boundaries as specified by type. The starting and ending
        offsets of the resulting substring are returned in startOffset
        and endOffset. 
        @param : offset
        the offset from which the substring search begins, and which
        must lie within the returned substring. 
        @param : type
        the text-boundary delimiter which determines whether the returned
        text constitures a character, word, line, or sentence (and possibly
        attendant whitespace), and whether the start or ending of such
        a substring forms the boundary condition. 
        @param : startOffset
        back-filled with the starting offset of the resulting substring,
        if one exists. 
        @param : endOffset
        back-filled with the offset of the character immediately following
        the resulting substring, if one exists. 
        @return a string which is a substring of the text content of
        the object, delimited by the specified boundary condition.
        """
        func = self.get_dbus_method("getTextAtOffset")
        return func(*args, **kwargs)
    
    def getTextBeforeOffset(self, *args, **kwargs):
        """
        Obtain a subset of the text content of an object which entirely
        precedes offset, delimited by character, word, line, or sentence
        boundaries as specified by type. The starting and ending offsets
        of the resulting substring are returned in startOffset and endOffset.
        By definition, if such a substring exists, endOffset is less
        than or equal to offset. 
        @param : offset
        the offset from which the substring search begins. 
        @param : type
        the text-boundary delimiter which determines whether the returned
        text constitures a character, word, line, or sentence (and possibly
        attendant whitespace), and whether the start or ending of such
        a substring forms the boundary condition. 
        @param : startOffset
        back-filled with the starting offset of the resulting substring,
        if one exists. 
        @param : endOffset
        back-filled with the offset of the character immediately following
        the resulting substring, if one exists. 
        @return a string which is a substring of the text content of
        the object, delimited by the specified boundary condition.
        """
        func = self.get_dbus_method("getTextBeforeOffset")
        return func(*args, **kwargs)
    
    def removeSelection(self, *args, **kwargs):
        """
        Deselect the text contained in the specified selectionNum, if
        such a selection exists, otherwise do nothing. Removal of a non-existant
        selectionNum has no effect. 
        @return True if the selection was successfully removed, False
        otherwise.
        """
        func = self.get_dbus_method("removeSelection")
        return func(*args, **kwargs)
    
    def setCaretOffset(self, *args, **kwargs):
        """
        Programmatically move the text caret (visible or virtual, as
        above) to a given position. 
        @param : offset
        a long int indicating the desired character offset. Not all implementations
        of Text will honor setCaretOffset requests, so the return value
        below should be checked by the client. 
        @return TRUE if the request was carried out, or FALSE if the
        caret could not be moved to the requested position.
        """
        func = self.get_dbus_method("setCaretOffset")
        return func(*args, **kwargs)
    
    def setSelection(self, *args, **kwargs):
        """
        Modify an existing selection's start or ending offset.
        Calling setSelection for a selectionNum that is not already defined
        has no effect. The result of calling setSelection with a selectionNum
        greater than 0 for objects that do not include STATE_MULTISELECTABLE
        is undefined. 
        @param : selectionNum
        indicates which of a set of non-contiguous selections to modify.
        @param : startOffset
        the new starting offset for the selection 
        @param : endOffset
        the new ending offset for the selection 
        @return True if the selection corresponding to selectionNum is
        successfully modified, False otherwise.
        """
        func = self.get_dbus_method("setSelection")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def get_caretOffset(self):
        self._pgetter(self._dbus_interface, "caretOffset")
    def set_caretOffset(self, value):
        self._psetter(self._dbus_interface, "caretOffset", value)
    _caretOffsetDoc = \
        """
        The current offset of the text caret in the Text object. This
        caret may be virtual, e.g. non-visual and notional-only, but
        if an onscreen representation of the caret position is visible,
        it will correspond to this offset. The caret offset is given
        as a character offset, as opposed to a byte offset into a text
        buffer or a column offset.
        """
    caretOffset = property(fget=get_caretOffset, fset=set_caretOffset, doc=_caretOffsetDoc)
    
    def get_characterCount(self):
        self._pgetter(self._dbus_interface, "characterCount")
    def set_characterCount(self, value):
        self._psetter(self._dbus_interface, "characterCount", value)
    _characterCountDoc = \
        """
        The total current number of characters in the Text object, including
        whitespace and non-spacing characters.
        """
    characterCount = property(fget=get_characterCount, fset=set_characterCount, doc=_characterCountDoc)
    
    class Range(list):
        def __new__(cls, startOffset, endOffset, content, data):
            list.__new__(cls, (startOffset, endOffset, content, data))
        def __init__(self, startOffset, endOffset, content, data):
            list.__init__(self, (startOffset, endOffset, content, data))
        
        def _get_startOffset(self):
            return self[0]
        def _set_startOffset(self, val):
            self[0] = val
        startOffset = property(fget=_get_startOffset, fset=_set_startOffset)
        def _get_endOffset(self):
            return self[1]
        def _set_endOffset(self, val):
            self[1] = val
        endOffset = property(fget=_get_endOffset, fset=_set_endOffset)
        def _get_content(self):
            return self[2]
        def _set_content(self, val):
            self[2] = val
        content = property(fget=_get_content, fset=_set_content)
        def _get_data(self):
            return self[3]
        def _set_data(self, val):
            self[3] = val
        data = property(fget=_get_data, fset=_set_data)


class EditableText(Text):
    """
    Derived from interface Text, EditableText provides methods for
    modifying textual content of components which support editing.
    EditableText also interacts with the system clipboard via copyText,
    cutText, and pasteText.
    """
    
    def copyText(self, *args, **kwargs):
        """
        Copy a range of text into the system clipboard. 
        @param : startPos
        the character offset of the first character in the range of text
        being copied. 
        @param : endPos
        the offset of the first character past the end of the range of
        text being copied.
        """
        func = self.get_dbus_method("copyText")
        return func(*args, **kwargs)
    
    def cutText(self, *args, **kwargs):
        """
        Excise a range of text from a Text object, copying it into the
        system clipboard. 
        @param : startPos
        the character offset of the first character in the range of text
        being cut. 
        @param : endPos
        the offset of the first character past the end of the range of
        text being cut. 
        @return True if the text was successfully cut, False otherwise.
        """
        func = self.get_dbus_method("cutText")
        return func(*args, **kwargs)
    
    def deleteText(self, *args, **kwargs):
        """
        Excise a range of text from a Text object without copying it
        into the system clipboard. 
        @param : startPos
        the character offset of the first character in the range of text
        being deleted. 
        @param : endPos
        the offset of the first character past the end of the range of
        text being deleted. 
        @return True if the text was successfully deleted, False otherwise.
        """
        func = self.get_dbus_method("deleteText")
        return func(*args, **kwargs)
    
    def insertText(self, *args, **kwargs):
        """
        Insert new text contents into an existing text object at a given
        location, while retaining the old contents. 
        @param : position
        the character offset into the Text implementor's content at which
        the new content will be inserted. 
        @param : text
        a UTF-8 string of which length characters will be inserted into
        the text object's text buffer. 
        @param : length
        the number of characters of text to insert. If the character
        count of text is less than or equal to length, the entire contents
        of text will be inserted.
        @return True if the text content was successfully inserted, False
        otherwise.
        """
        func = self.get_dbus_method("insertText")
        return func(*args, **kwargs)
    
    def pasteText(self, *args, **kwargs):
        """
        Copy the text contents of the system clipboard, if any, into
        a Text object, inserting it at a particular character offset.
        @param : position
        the character offset before which the text will be inserted.
        @return True if the text was successfully pasted into the Text
        object, False otherwise.
        """
        func = self.get_dbus_method("pasteText")
        return func(*args, **kwargs)
    
    def setAttributes(self, *args, **kwargs):
        """
        Apply a particular set of attributes to a range of text.
        @return True if the text attributes were successfully modified,
        False otherwise.
        """
        func = self.get_dbus_method("setAttributes")
        return func(*args, **kwargs)
    
    def setTextContents(self, *args, **kwargs):
        """
        Replace the text contents with a new string, discarding the old
        contents.
        @param : newContents
        a UTF-8 string with which the text object's contents will be
        replaced. 
        @return True if the text content was successfully changed, False
        otherwise.
        """
        func = self.get_dbus_method("setTextContents")
        return func(*args, **kwargs)
    
    def unImplemented10(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented10")
        return func(*args, **kwargs)
    
    def unImplemented11(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented11")
        return func(*args, **kwargs)
    
    def unImplemented12(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented12")
        return func(*args, **kwargs)
    
    def unImplemented5(self, *args, **kwargs):
        """
        unImplemented:
        placeholders for future expansion. Note that these are named
        'unimplemented5 and unimplemented6' to avoid conflict with placeholders
        from Accessibility::Text.
        """
        func = self.get_dbus_method("unImplemented5")
        return func(*args, **kwargs)
    
    def unImplemented6(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented6")
        return func(*args, **kwargs)
    
    def unImplemented9(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented9")
        return func(*args, **kwargs)

class Event(list):
    def __new__(cls, type, source, detail1, detail2, any_data):
        list.__new__(cls, (type, source, detail1, detail2, any_data))
    def __init__(self, type, source, detail1, detail2, any_data):
        list.__init__(self, (type, source, detail1, detail2, any_data))
    
    def _get_type(self):
        return self[0]
    def _set_type(self, val):
        self[0] = val
    type = property(fget=_get_type, fset=_set_type)
    def _get_source(self):
        return self[1]
    def _set_source(self, val):
        self[1] = val
    source = property(fget=_get_source, fset=_set_source)
    def _get_detail1(self):
        return self[2]
    def _set_detail1(self, val):
        self[2] = val
    detail1 = property(fget=_get_detail1, fset=_set_detail1)
    def _get_detail2(self):
        return self[3]
    def _set_detail2(self, val):
        self[3] = val
    detail2 = property(fget=_get_detail2, fset=_set_detail2)
    def _get_any_data(self):
        return self[4]
    def _set_any_data(self, val):
        self[4] = val
    any_data = property(fget=_get_any_data, fset=_set_any_data)


class EventDetails(list):
    def __new__(cls, host_application, source_role, source_name, any_data):
        list.__new__(cls, (host_application, source_role, source_name, any_data))
    def __init__(self, host_application, source_role, source_name, any_data):
        list.__init__(self, (host_application, source_role, source_name, any_data))
    
    def _get_host_application(self):
        return self[0]
    def _set_host_application(self, val):
        self[0] = val
    host_application = property(fget=_get_host_application, fset=_set_host_application)
    def _get_source_role(self):
        return self[1]
    def _set_source_role(self, val):
        self[1] = val
    source_role = property(fget=_get_source_role, fset=_set_source_role)
    def _get_source_name(self):
        return self[2]
    def _set_source_name(self, val):
        self[2] = val
    source_name = property(fget=_get_source_name, fset=_set_source_name)
    def _get_any_data(self):
        return self[3]
    def _set_any_data(self, val):
        self[3] = val
    any_data = property(fget=_get_any_data, fset=_set_any_data)

class EventListener(BaseProxy):
    """
    A generic interface implemented by objects for the receipt of
    event notifications. EventListener is the interface from which
    Accessibility::Registry is derived, and via which clients of
    the Registry receive notification of changes to an application's
    user interface and content.
    """
    
    def notifyEvent(self, *args, **kwargs):
        """
        Synchronously notify an EventListener that an event has occurred,
        by passing it an Event struct. 
        @param : e
        The Event about which the listener is being notified.
        """
        func = self.get_dbus_method("notifyEvent")
        return func(*args, **kwargs)
    
    def unImplemented2_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2_")
        return func(*args, **kwargs)
    
    def unImplemented3_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3_")
        return func(*args, **kwargs)
    
    def unImplemented4_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4_")
        return func(*args, **kwargs)
    
    def unImplemented_(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented_")
        return func(*args, **kwargs)


class EventListenerMode(list):
    def __new__(cls, synchronous, preemptive, global_):
        list.__new__(cls, (synchronous, preemptive, global_))
    def __init__(self, synchronous, preemptive, global_):
        list.__init__(self, (synchronous, preemptive, global_))
    
    def _get_synchronous(self):
        return self[0]
    def _set_synchronous(self, val):
        self[0] = val
    synchronous = property(fget=_get_synchronous, fset=_set_synchronous)
    def _get_preemptive(self):
        return self[1]
    def _set_preemptive(self, val):
        self[1] = val
    preemptive = property(fget=_get_preemptive, fset=_set_preemptive)
    def _get_global_(self):
        return self[2]
    def _set_global_(self, val):
        self[2] = val
    global_ = property(fget=_get_global_, fset=_set_global_)


class EventType(Enum):
    _enum_lookup = {
        0:'KEY_PRESSED_EVENT',
        1:'KEY_RELEASED_EVENT',
        2:'BUTTON_PRESSED_EVENT',
        3:'BUTTON_RELEASED_EVENT',
    }

class Hyperlink(BaseProxy):
    """
    Instances of Hyperlink are returned by Hypertext objects, and
    are the means by which end users and clients interact with linked,
    and in some cases embedded, content. Hyperlinks may have multiple
    "anchors", where an anchor corresponds to a reference to a particular
    resource with a corresponding resource identified (URI). Hyperlinks
    may be queried for their URIs, or queried for the objects corresponding
    to their anchors. The objects thus obtained are instances of
    Accessible, and may be queried, and manipulated via the Action
    interface.
    """
    
    def getObject(self, *args, **kwargs):
        """
        Gets the i'th object, (where i is an integer between 0 and Hyperlink::numAnchors
        - 1, inclusive) associated with a Hyperlink. The objects returned
        are usually actionable (i.e. they should implement Accessibility::Action),
        and the available actions often include "open", "bookmark", "save
        link as", etc. They may also implement Accessibility::StreamableContent,
        although clients can normally use getURI to obtain a resource
        locator via which the object's data may be accessed.
        @return an Accessible object instance representing the Hyperlink's
        ith anchor, or through which the content associated with the
        ith anchor can be accessed.
        """
        func = self.get_dbus_method("getObject")
        return func(*args, **kwargs)
    
    def getURI(self, *args, **kwargs):
        """
        Obtain a resource locator ('URI') which can be used to access
        the content to which this link "points" or is connected. 
        @return a string corresponding to the URI of the Hyperlink's
        'ith' anchor, if one exists, or a NIL string otherwise.
        """
        func = self.get_dbus_method("getURI")
        return func(*args, **kwargs)
    
    def isValid(self, *args, **kwargs):
        """
        Check the hyperlink to see if a connection to its backing content
        can be established, or if its URI is valid. 
        @return True if the object's content is available, or False if
        the hyperlink's URI is invalid, or a connection to the resource
        can not be established.
        """
        func = self.get_dbus_method("isValid")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)
    
    def get_endIndex(self):
        self._pgetter(self._dbus_interface, "endIndex")
    def set_endIndex(self, value):
        self._psetter(self._dbus_interface, "endIndex", value)
    _endIndexDoc = \
        """
        the ending offset within the containing Hypertext content with
        which this Hyperlink is associated; that is, the offset of the
        first element past the range within the Hypertext associated
        with this Hyperlink.
        """
    endIndex = property(fget=get_endIndex, fset=set_endIndex, doc=_endIndexDoc)
    
    def get_nAnchors(self):
        self._pgetter(self._dbus_interface, "nAnchors")
    def set_nAnchors(self, value):
        self._psetter(self._dbus_interface, "nAnchors", value)
    _nAnchorsDoc = \
        """
        the number of separate anchors associated with this Hyperlink
        """
    nAnchors = property(fget=get_nAnchors, fset=set_nAnchors, doc=_nAnchorsDoc)
    
    def get_startIndex(self):
        self._pgetter(self._dbus_interface, "startIndex")
    def set_startIndex(self, value):
        self._psetter(self._dbus_interface, "startIndex", value)
    _startIndexDoc = \
        """
        the starting offset within the containing Hypertext content with
        which this Hyperlink is associated
        """
    startIndex = property(fget=get_startIndex, fset=set_startIndex, doc=_startIndexDoc)


class Hypertext(BaseProxy):
    """
    An interface used for objects which implement linking between
    multiple resource or content locations, or multiple 'markers'
    within a single document. A Hypertext instance is associated
    with one or more Hyperlinks, which are associated with particular
    offsets within the Hypertext's included content.
    """
    
    def getLink(self, *args, **kwargs):
        """
        Get one of the Hyperlinks associated with this Hypertext object,
        by index.
        @param : linkIndex
        an integer from 0 to getNLinks() - 1. 
        @return the Hyperlink in this Hypertext object.
        """
        func = self.get_dbus_method("getLink")
        return func(*args, **kwargs)
    
    def getLinkIndex(self, *args, **kwargs):
        """
        Get the hyperlink index, if any, associated with a particular
        character offset in the Hypertext object. For Hypertext implementors
        without textual content, all hyperlinks are associated with character
        offset '0'.
        @return the index of the Hyperlink associated with character
        offset characterIndex, or -1 if no Hyperlink is associated with
        that character offset.
        """
        func = self.get_dbus_method("getLinkIndex")
        return func(*args, **kwargs)
    
    def getNLinks(self, *args, **kwargs):
        """
        Query the hypertext object for the number of Hyperlinks it contains.
        @return the number of Hyperlinks associated with this Hypertext
        object, as a long integer.
        """
        func = self.get_dbus_method("getNLinks")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)


class Image(BaseProxy):
    """
    An interface implemented by objects which render image data or
    pictorial information to the screen. When onscreen components
    include graphical information that is not purely intended to
    enhance "3d effect" or visual layout, but which conveys some
    semantic or informational content to the sighted user, they should
    implement Image, and that semantic content should be conveyed
    textually to the extent possible via the image description, as
    well as the Accessible::name and Accessible::description properties.
    """
    
    def getImageExtents(self, *args, **kwargs):
        """
        Obtain a bounding box which entirely contains the image contents,
        as displayed on screen. The bounds returned do not account for
        any viewport clipping or the fact that the image may be partially
        or wholly obscured by other onscreen content. 
        @param : coordType
        If 0, the returned bounding box position is returned relative
        to the screen; if 1, the bounding box position is returned relative
        to the containing window. 
        @return a BoundingBox enclosing the image's onscreen representation.
        """
        func = self.get_dbus_method("getImageExtents")
        return func(*args, **kwargs)
    
    def getImagePosition(self, *args, **kwargs):
        """
        Get the coordinates of the current image position on screen.
        @param : x
        Back-filled with the x coordinate of the onscreen image (i.e.
        the minimum x coordinate) 
        @param : y
        Back-filled with the y coordinate of the onscreen image (i.e.
        the minimum y coordinate) 
        @param : coordType
        If 0, the returned x and y coordinates are returned relative
        to the screen; if 1, they are returned relative to the containing
        window.
        """
        func = self.get_dbus_method("getImagePosition")
        return func(*args, **kwargs)
    
    def getImageSize(self, *args, **kwargs):
        """
        Obtain the width and height of the current onscreen view of the
        image. The extents returned do not account for any viewport clipping
        or the fact that the image may be partially or wholly obscured
        by other onscreen content. 
        @param : width
        Back-filled with the x extents of the onscreen image (i.e. the
        image width in pixels) 
        @param : height
        Back-filled with the y extents of the onscreen image (i.e. the
        image height in pixels)
        """
        func = self.get_dbus_method("getImageSize")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def get_imageDescription(self):
        self._pgetter(self._dbus_interface, "imageDescription")
    def set_imageDescription(self, value):
        self._psetter(self._dbus_interface, "imageDescription", value)
    _imageDescriptionDoc = \
        """
        A UTF-8 string providing a textual description of what is visually
        depicted in the image.
        """
    imageDescription = property(fget=get_imageDescription, fset=set_imageDescription, doc=_imageDescriptionDoc)
    
    def get_imageLocale(self):
        self._pgetter(self._dbus_interface, "imageLocale")
    def set_imageLocale(self, value):
        self._psetter(self._dbus_interface, "imageLocale", value)
    _imageLocaleDoc = \
        """
        A string corresponding to the POSIX LC_MESSAGES locale used by
        the imageDescription.
        """
    imageLocale = property(fget=get_imageLocale, fset=set_imageLocale, doc=_imageLocaleDoc)


class KeyDefinition(list):
    def __new__(cls, keycode, keysym, keystring, unused):
        list.__new__(cls, (keycode, keysym, keystring, unused))
    def __init__(self, keycode, keysym, keystring, unused):
        list.__init__(self, (keycode, keysym, keystring, unused))
    
    def _get_keycode(self):
        return self[0]
    def _set_keycode(self, val):
        self[0] = val
    keycode = property(fget=_get_keycode, fset=_set_keycode)
    def _get_keysym(self):
        return self[1]
    def _set_keysym(self, val):
        self[1] = val
    keysym = property(fget=_get_keysym, fset=_set_keysym)
    def _get_keystring(self):
        return self[2]
    def _set_keystring(self, val):
        self[2] = val
    keystring = property(fget=_get_keystring, fset=_set_keystring)
    def _get_unused(self):
        return self[3]
    def _set_unused(self, val):
        self[3] = val
    unused = property(fget=_get_unused, fset=_set_unused)

class KeyEventType(Enum):
    _enum_lookup = {
        0:'KEY_PRESSED',
        1:'KEY_RELEASED',
    }

class KeySynthType(Enum):
    _enum_lookup = {
        0:'KEY_PRESS',
        1:'KEY_RELEASE',
        2:'KEY_PRESSRELEASE',
        3:'KEY_SYM',
        4:'KEY_STRING',
    }

class LOCALE_TYPE(Enum):
    _enum_lookup = {
        0:'LOCALE_TYPE_MESSAGES',
        1:'LOCALE_TYPE_COLLATE',
        2:'LOCALE_TYPE_CTYPE',
        3:'LOCALE_TYPE_MONETARY',
        4:'LOCALE_TYPE_NUMERIC',
        5:'LOCALE_TYPE_TIME',
    }

class LoginHelper(BaseProxy):
    """
    An interface for use by assistive technologies by which they
    can access system information and services on a 'need to know'
    basis while the screen is locked, during user authentication,
    or during other sensitive operations.     
    This interface is intended for use by assistive technologies
    and related user-enabling services, and by applications and utilities
    which may wish to restrict access to certain system devices and
    services during security-sensitive states, e.g. when the screen
    is locked or during authentication into some secure service.
    Such 'applications' (for instance, screen lock dialogs and security-enabled
    web browsers) use the LoginHelper client interfaces, and the
    bonobo-activation query service, to query for assistive technologies
    which advertise the LoginHelper service. The client then queries
    these assistive technologies for their device I/O requirements,
    via the getDeviceReqs call. The client may then issue the advisory
    request setSafe (TRUE), which requests that the LoginHelper -implementing
    service make a best-effort attempt to make itself more secure
    (for instance, an onscreen keyboard might turn off word prediction,
    and a screenreader may turn off keyboard echo via speech). The
    return value of setSafe is an advisory indication of whether
    this attempt was successful (no specific guarantees are implied).
    Once the 'security sensitive' state is exited, the client should
    call setSafe (FALSE).
    The return values from getDeviceReqs inform the client of which
    services the LoginHelper service (e. g. assistive technology)
    needs in order to do its job. The client may use this information
    to loosen any restrictions on access which it may currently have
    in place (for instance, keyboard grabs, etc.). If it does not
    do so, the likely outcome is that the end-user will experience
    loss of access to the system.
    """
    
    def getDeviceReqs(self, *args, **kwargs):
        """
        getDeviceReqs:
        Query a LoginHelper for the types of device I/O it requires,
        in order to do its job. For instance, a LoginHelper which needs
        to receive keyboard events will include Accessibility_LoginHelper_CORE_KEYBOARD
        in this list.
        @return : A sequence of LoginHelper_DeviceReq indicating the
        device I/O required in order to facilitate end-user access to
        the system.
        """
        func = self.get_dbus_method("getDeviceReqs")
        return func(*args, **kwargs)
    
    def getRaiseWindows(self, *args, **kwargs):
        """
        getRaiseWindows:
        Get a list of window IDs that need raising on login.
        @return : a sequence containing window IDS for toplevels which
        need to be raised/made visible during user authentication, in
        order for the LoginHelper to facilitate end-user access to the
        system.
        """
        func = self.get_dbus_method("getRaiseWindows")
        return func(*args, **kwargs)
    
    def setSafe(self, *args, **kwargs):
        """
        setSafe: 
        @param : safe_mode
        TRUE if the client is requesting that 'safe mode' be initiated,
        FALSE if the client is advising that 'safe mode' may be exited,
        i.e. normal operation may be resumed.
        Request a LoginHelper to enter "safe" mode, or inform LoginHelper
        that "safe" mode may be exited. If safe_mode is TRUE, but the
        return value is FALSE, the requesting client may wish to deny
        services to the LoginHelper, for instance avoid raising its toplevels.
        The return value is purely advisory, and no guarantees are intended
        about what the implementing LoginHelper will do to improve security
        when in "safe" mode.
        @return : whether the LoginHelper is now "safe" or not.
        """
        func = self.get_dbus_method("setSafe")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)

    class DeviceReq(Enum):
        _enum_lookup = {
            0:'GUI_EVENTS',
            1:'CORE_KEYBOARD',
            2:'CORE_POINTER',
            3:'EXT_INPUT',
            4:'POST_WINDOWS',
            5:'AUDIO_OUT',
            6:'AUDIO_IN',
            7:'NETWORK',
            8:'LOCALHOST',
            9:'SERIAL_OUT',
            10:'SERIAL_IN',
        }
    
    AUDIO_IN = DeviceReq(6)
    
    AUDIO_OUT = DeviceReq(5)
    
    CORE_KEYBOARD = DeviceReq(1)
    
    CORE_POINTER = DeviceReq(2)
    
    EXT_INPUT = DeviceReq(3)
    
    GUI_EVENTS = DeviceReq(0)
    
    LOCALHOST = DeviceReq(8)
    
    NETWORK = DeviceReq(7)
    
    POST_WINDOWS = DeviceReq(4)
    
    SERIAL_IN = DeviceReq(10)
    
    SERIAL_OUT = DeviceReq(9)
    
    class WindowInfo(list):
        def __new__(cls, winID):
            list.__new__(cls, (winID))
        def __init__(self, winID):
            list.__init__(self, (winID))
        
        def _get_winID(self):
            return self[0]
        def _set_winID(self, val):
            self[0] = val
        winID = property(fget=_get_winID, fset=_set_winID)

class ModifierType(Enum):
    _enum_lookup = {
        0:'MODIFIER_SHIFT',
        1:'MODIFIER_SHIFTLOCK',
        2:'MODIFIER_CONTROL',
        3:'MODIFIER_ALT',
        4:'MODIFIER_META',
        5:'MODIFIER_META2',
        6:'MODIFIER_META3',
        7:'MODIFIER_NUMLOCK',
    }

class Registry(EventListener):
    """
    The Registry is a service through which applications providing
    accessibility services (servers) can rendezvous with consumers
    of those services (Assistive Technologies). The Registry is the
    first "port of call" for accessible applications and for assistive
    technologies wishing to query and interact with those applications.
    The Registry service provides four basic functions to Assistive
    Technology (AT) clients: 
    it provides a list of the applications who have registered with
    the AT-SPI framework, thereby announcing their participation
    in the AT-SPI framework; 
    it allows AT clients to register for notification of changes
    in application state (at-spi Events); 
    it dispatches/relays said events from participating applications
    to the registered listeners; 
    it gives access to system device events via the associated DeviceEventController
    interface.
    From the point of view of accessible applications (i.e. AT-SPI
    service producers), the Registry is primarily a registration
    and event delivery service. Applications normally only call the
    registerApplication and deregisterApplication Registry methods,
    and its inherited EventListener::notifyEvent method.
    The Registry normally lives in its own process space; communication
    via Registry and both application services and AT clients takes
    place via IPC. A process space diagram illustrating the relationship
    between applications, Registry, and AT is shown below.
    """
    
    def deregisterApplication(self, *args, **kwargs):
        """
        De-register an application previously registered with the broker.
        deregisterApplication: 
        @param : app
        a reference to the Application to be deregistered.
        """
        func = self.get_dbus_method("deregisterApplication")
        return func(*args, **kwargs)
    
    def deregisterGlobalEventListener(self, *args, **kwargs):
        """
        deregisterGlobalEventListener: 
        @param : listener
        the requesting EventListener 
        @param : eventName
        a string indicating the type of events
        Request that a previously registered client stop receiving global
        notifications for events of a certain type.
        """
        func = self.get_dbus_method("deregisterGlobalEventListener")
        return func(*args, **kwargs)
    
    def deregisterGlobalEventListenerAll(self, *args, **kwargs):
        """
        deregisterGlobalEventListenerAll: 
        @param : listener
        the requesting EventListener
        Request that a previously registered client stop receiving global
        notifications for all events for which it was registered.
        """
        func = self.get_dbus_method("deregisterGlobalEventListenerAll")
        return func(*args, **kwargs)
    
    def getDesktop(self, *args, **kwargs):
        """
        getDesktop: 
        : the index of the requested Desktop.
        Get the nth accessible desktop.
        @return a reference to the requested Desktop.
        """
        func = self.get_dbus_method("getDesktop")
        return func(*args, **kwargs)
    
    def getDesktopCount(self, *args, **kwargs):
        """
        event types: "Window" "Desktop" "Window:Create" "Window:Destroy"
        "Window:Iconify" "Window:Restore" "Window:Fullscreen" "Window:Resize"
        "Desktop:Create" "Desktop:Destroy" "Desktop:Focus" "Desktop:Defocus"
        "Desktop:Reorder" "Focus" "GtkWidget:show" "GObject:notify:<propertyname>"
        ( not sure we should allow these last 2 forms, since they are
        toolkit-specific, but they're powerful ) getDesktopCount:
        Get the current number of desktops. 
        @return a short integer indicating the current number of Desktops.
        """
        func = self.get_dbus_method("getDesktopCount")
        return func(*args, **kwargs)
    
    def getDesktopList(self, *args, **kwargs):
        """
        Get a list of accessible desktops.
        @return : a sequence containing references to the Desktops.
        """
        func = self.get_dbus_method("getDesktopList")
        return func(*args, **kwargs)
    
    def getDeviceEventController(self, *args, **kwargs):
        """
        Obtain an object which can be used to request device event notifications.
        @return : an object implementing DeviceEventController
        """
        func = self.get_dbus_method("getDeviceEventController")
        return func(*args, **kwargs)
    
    def registerApplication(self, *args, **kwargs):
        """
        Register a new application with the accessibility broker. 
        @param : app
        a reference to the requesting Application
        """
        func = self.get_dbus_method("registerApplication")
        return func(*args, **kwargs)
    
    def registerGlobalEventListener(self, *args, **kwargs):
        """
        Register a client's interest in (all) application events of a
        certain type. 
        @param : listener
        a reference to the requesting EventListener. 
        @param : eventName
        a string which indicates the type of events about which the client
        desires notification.
        """
        func = self.get_dbus_method("registerGlobalEventListener")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)
    
    def unImplemented5(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented5")
        return func(*args, **kwargs)
    
    def unImplemented6(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented6")
        return func(*args, **kwargs)


class Relation(BaseProxy):
    """
    An interface via which objects' non-hierarchical relationships
    to one another are indicated. An instance of Relations represents
    a "one-to-many" correspondance.
    """
    
    def getNTargets(self, *args, **kwargs):
        """
        @return the number of objects to which this relationship applies.
        """
        func = self.get_dbus_method("getNTargets")
        return func(*args, **kwargs)
    
    def getRelationType(self, *args, **kwargs):
        """
        @return the RelationType of this Relation.
        """
        func = self.get_dbus_method("getRelationType")
        return func(*args, **kwargs)
    
    def getRelationTypeName(self, *args, **kwargs):
        """
        @return an unlocalized string representing the relation type.
        """
        func = self.get_dbus_method("getRelationTypeName")
        return func(*args, **kwargs)
    
    def getTarget(self, *args, **kwargs):
        """
        @return an Object which is the 'nth'target of this Relation,
        e.g. the Object at index i in the list of Objects having the
        specified relationship to this Accessible.
        """
        func = self.get_dbus_method("getTarget")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)


class RelationType(Enum):
    _enum_lookup = {
        0:'RELATION_NULL',
        1:'RELATION_LABEL_FOR',
        2:'RELATION_LABELLED_BY',
        3:'RELATION_CONTROLLER_FOR',
        4:'RELATION_CONTROLLED_BY',
        5:'RELATION_MEMBER_OF',
        6:'RELATION_TOOLTIP_FOR',
        7:'RELATION_NODE_CHILD_OF',
        8:'RELATION_EXTENDED',
        9:'RELATION_FLOWS_TO',
        10:'RELATION_FLOWS_FROM',
        11:'RELATION_SUBWINDOW_OF',
        12:'RELATION_EMBEDS',
        13:'RELATION_EMBEDDED_BY',
        14:'RELATION_POPUP_FOR',
        15:'RELATION_PARENT_WINDOW_OF',
        16:'RELATION_DESCRIPTION_FOR',
        17:'RELATION_DESCRIBED_BY',
        18:'RELATION_LAST_DEFINED',
    }

class Role(Enum):
    _enum_lookup = {
        0:'ROLE_INVALID',
        1:'ROLE_ACCELERATOR_LABEL',
        2:'ROLE_ALERT',
        3:'ROLE_ANIMATION',
        4:'ROLE_ARROW',
        5:'ROLE_CALENDAR',
        6:'ROLE_CANVAS',
        7:'ROLE_CHECK_BOX',
        8:'ROLE_CHECK_MENU_ITEM',
        9:'ROLE_COLOR_CHOOSER',
        10:'ROLE_COLUMN_HEADER',
        11:'ROLE_COMBO_BOX',
        12:'ROLE_DATE_EDITOR',
        13:'ROLE_DESKTOP_ICON',
        14:'ROLE_DESKTOP_FRAME',
        15:'ROLE_DIAL',
        16:'ROLE_DIALOG',
        17:'ROLE_DIRECTORY_PANE',
        18:'ROLE_DRAWING_AREA',
        19:'ROLE_FILE_CHOOSER',
        20:'ROLE_FILLER',
        21:'ROLE_FOCUS_TRAVERSABLE',
        22:'ROLE_FONT_CHOOSER',
        23:'ROLE_FRAME',
        24:'ROLE_GLASS_PANE',
        25:'ROLE_HTML_CONTAINER',
        26:'ROLE_ICON',
        27:'ROLE_IMAGE',
        28:'ROLE_INTERNAL_FRAME',
        29:'ROLE_LABEL',
        30:'ROLE_LAYERED_PANE',
        31:'ROLE_LIST',
        32:'ROLE_LIST_ITEM',
        33:'ROLE_MENU',
        34:'ROLE_MENU_BAR',
        35:'ROLE_MENU_ITEM',
        36:'ROLE_OPTION_PANE',
        37:'ROLE_PAGE_TAB',
        38:'ROLE_PAGE_TAB_LIST',
        39:'ROLE_PANEL',
        40:'ROLE_PASSWORD_TEXT',
        41:'ROLE_POPUP_MENU',
        42:'ROLE_PROGRESS_BAR',
        43:'ROLE_PUSH_BUTTON',
        44:'ROLE_RADIO_BUTTON',
        45:'ROLE_RADIO_MENU_ITEM',
        46:'ROLE_ROOT_PANE',
        47:'ROLE_ROW_HEADER',
        48:'ROLE_SCROLL_BAR',
        49:'ROLE_SCROLL_PANE',
        50:'ROLE_SEPARATOR',
        51:'ROLE_SLIDER',
        52:'ROLE_SPIN_BUTTON',
        53:'ROLE_SPLIT_PANE',
        54:'ROLE_STATUS_BAR',
        55:'ROLE_TABLE',
        56:'ROLE_TABLE_CELL',
        57:'ROLE_TABLE_COLUMN_HEADER',
        58:'ROLE_TABLE_ROW_HEADER',
        59:'ROLE_TEAROFF_MENU_ITEM',
        60:'ROLE_TERMINAL',
        61:'ROLE_TEXT',
        62:'ROLE_TOGGLE_BUTTON',
        63:'ROLE_TOOL_BAR',
        64:'ROLE_TOOL_TIP',
        65:'ROLE_TREE',
        66:'ROLE_TREE_TABLE',
        67:'ROLE_UNKNOWN',
        68:'ROLE_VIEWPORT',
        69:'ROLE_WINDOW',
        70:'ROLE_EXTENDED',
        71:'ROLE_HEADER',
        72:'ROLE_FOOTER',
        73:'ROLE_PARAGRAPH',
        74:'ROLE_RULER',
        75:'ROLE_APPLICATION',
        76:'ROLE_AUTOCOMPLETE',
        77:'ROLE_EDITBAR',
        78:'ROLE_EMBEDDED',
        79:'ROLE_ENTRY',
        80:'ROLE_CHART',
        81:'ROLE_CAPTION',
        82:'ROLE_DOCUMENT_FRAME',
        83:'ROLE_HEADING',
        84:'ROLE_PAGE',
        85:'ROLE_SECTION',
        86:'ROLE_REDUNDANT_OBJECT',
        87:'ROLE_FORM',
        88:'ROLE_LINK',
        89:'ROLE_INPUT_METHOD_WINDOW',
        90:'ROLE_LAST_DEFINED',
    }

class Selection(BaseProxy):
    """
    An interface which indicates that an object exposes a 'selection'
    model, allowing the selection of one or more of its children.
    Read-only Selection instances are possible, in which case the
    interface is used to programmatically determine the selected-ness
    of its children. A selected child has State::STATE_SELECTED,
    and a child which may hypothetically be selected (though possibly
    not programmatically selectable) has State::STATE_SELECTABLE.
    """
    
    def clearSelection(self, *args, **kwargs):
        """
        Attempt to clear all selections (i.e. deselect all children)
        of a Selection. Not all Selection implementations allow the removal
        of all selections.
        @return True if the selections were successfully cleared, False
        otherwise.
        """
        func = self.get_dbus_method("clearSelection")
        return func(*args, **kwargs)
    
    def deselectChild(self, *args, **kwargs):
        """
        Remove a child from the selected children list of a Selection,
        if the child is currently selected.
        @param : childIndex
        a long integer (the zero offset index into the Accessible object's
        list of children) indicating which child of the Selection is
        to be selected.
        @return True if the child was successfully selected, False otherwise.
        """
        func = self.get_dbus_method("deselectChild")
        return func(*args, **kwargs)
    
    def deselectSelectedChild(self, *args, **kwargs):
        """
        Remove a child to the selected children list of a Selection.
        @param : selectedChildIndex
        a long integer indicating which of the selected children of the
        Selection is to be deselected. The index is a zero-offset index
        into the 'selected child list', not a zero-offset index into
        the list of all children of the Selection.
        @return True if the child was successfully deselected, False
        otherwise.
        """
        func = self.get_dbus_method("deselectSelectedChild")
        return func(*args, **kwargs)
    
    def getSelectedChild(self, *args, **kwargs):
        """
        Get the i-th selected Accessible child of a Selection. 
        @param : selectedChildIndex
        a long integer indicating which of the selected children of an
        object is being requested. 
        @return a pointer to a selected Accessible child object, specified
        by selectedChildIndex.
        """
        func = self.get_dbus_method("getSelectedChild")
        return func(*args, **kwargs)
    
    def isChildSelected(self, *args, **kwargs):
        """
        Determine whether a particular child of an Selection implementor
        is currently selected. Note that childIndex is the zero-offset
        index into the standard Accessible container's list of children.
        @param : childIndex
        an index into the Selection's list of children.
        @return True if the specified child is currently selected, False
        otherwise.
        """
        func = self.get_dbus_method("isChildSelected")
        return func(*args, **kwargs)
    
    def selectAll(self, *args, **kwargs):
        """
        Attempt to select all of the children of a Selection implementor.
        Not all Selection implementors support this operation (for instance,
        implementations which support only "single selection" do not
        support this operation).
        @return True if successful, False otherwise.
        """
        func = self.get_dbus_method("selectAll")
        return func(*args, **kwargs)
    
    def selectChild(self, *args, **kwargs):
        """
        Add a child to the selected children list of a Selection. 
        @param : childIndex
        a long integer indicating which child of the Selection is to
        be selected.
        @return True if the child was successfully selected, False otherwise.
        """
        func = self.get_dbus_method("selectChild")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        """
        unImplemented:
        placeholders for future expansion.
        """
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def get_nSelectedChildren(self):
        self._pgetter(self._dbus_interface, "nSelectedChildren")
    def set_nSelectedChildren(self, value):
        self._psetter(self._dbus_interface, "nSelectedChildren", value)
    _nSelectedChildrenDoc = \
        """
        The number of children of a Selection implementor which are currently
        selected.
        """
    nSelectedChildren = property(fget=get_nSelectedChildren, fset=set_nSelectedChildren, doc=_nSelectedChildrenDoc)


class Selector(BaseProxy):
    """
    This interface is intended for use by assistive technologies
    and related user-agents. Via this interface, an assistive technology
    or user agent may expose a series of choices or selections in
    textual form, which can be activated on demand by a client of
    the Selector interface.
    Examples of the use of this interface include voice-command and
    remote-control applications, in which the user interaction is
    wholly or partly delegated by the implementor to an external
    agent.
    """
    
    def activateCommand(self, *args, **kwargs):
        """
        Request that the Selector invoke the specified Command. 
        @param : cmd
        the Command to activate/invoke. 
        @return a CommandResult indicating whether the request was honored,
        and the reason for failure if the Command could not be activated
        or invoked.
        """
        func = self.get_dbus_method("activateCommand")
        return func(*args, **kwargs)
    
    def deregisterChangeListener(self, *args, **kwargs):
        """
        Tell the Selector instance to cease notifying the specified CommandListener
        of changes to the command list. 
        @param : listener
        the CommandListener to remove from the notification list.
        """
        func = self.get_dbus_method("deregisterChangeListener")
        return func(*args, **kwargs)
    
    def getCommands(self, *args, **kwargs):
        """
        Query the Selector for the current CommandList.
        @return the currently available CommandList
        """
        func = self.get_dbus_method("getCommands")
        return func(*args, **kwargs)
    
    def refreshCommands(self, *args, **kwargs):
        """
        Ask the Selector to re-calculate its CommandList. 
        @return TRUE if the CommandList changed.
        """
        func = self.get_dbus_method("refreshCommands")
        return func(*args, **kwargs)
    
    def registerChangeListener(self, *args, **kwargs):
        """
        Register a :CommandListener instance for notification of changes
        to the command set. 
        @param : listener
        the CommandListener to be notified of changes.
        """
        func = self.get_dbus_method("registerChangeListener")
        return func(*args, **kwargs)
    
    def replaceCommands(self, *args, **kwargs):
        """
        @return TRUE if the replacement request was successful, FALSE
        if the request could not be honored.
        """
        func = self.get_dbus_method("replaceCommands")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)
    
    def get_supportsReplace(self):
        self._pgetter(self._dbus_interface, "supportsReplace")
    def set_supportsReplace(self, value):
        self._psetter(self._dbus_interface, "supportsReplace", value)
    _supportsReplaceDoc = \
        """
        This attribute is TRUE if this Selector allows its CommandList
        to be specified by the client
        """
    supportsReplace = property(fget=get_supportsReplace, fset=set_supportsReplace, doc=_supportsReplaceDoc)

    class CommandResult(Enum):
        """
        A code returned by a call to activateCommand, indicating the
        result of the activation request.
        """
        _enum_lookup = {
            0:'COMMAND_RESULT_INVALID',
            1:'COMMAND_RESULT_SUCCESS',
            2:'COMMAND_RESULT_FAILED',
            3:'COMMAND_RESULT_OBSOLETE',
            4:'COMMAND_RESULT_LAST_DEFINED',
        }
    
    COMMAND_RESULT_FAILED = CommandResult(2)
    
    COMMAND_RESULT_INVALID = CommandResult(0)
    
    COMMAND_RESULT_LAST_DEFINED = CommandResult(4)
    
    COMMAND_RESULT_OBSOLETE = CommandResult(3)
    
    COMMAND_RESULT_SUCCESS = CommandResult(1)

class StateSet(BaseProxy):
    """
    The StateSet interface encapsulates a collection of state information.
    It allows comparison of state information between object instances,
    and comparisons of an object's state with some hypothetical collection
    of states.
    """
    
    def add(self, *args, **kwargs):
        """
        Add a StateType to an existing StateSet, if not already present.
        """
        func = self.get_dbus_method("add")
        return func(*args, **kwargs)
    
    def compare(self, *args, **kwargs):
        """
        Compare two StateSet instances and obtain their differences.
        @return a 'difference set', i.e. a StateSet consisting of those
        states not shared by the two sets being compared.
        """
        func = self.get_dbus_method("compare")
        return func(*args, **kwargs)
    
    def contains(self, *args, **kwargs):
        """
        Query a StateSet for a specific StateType. 
        @param : state
        the StateType being queried for. 
        @return TRUE if the StateSet contains StateType state.
        """
        func = self.get_dbus_method("contains")
        return func(*args, **kwargs)
    
    def equals(self, *args, **kwargs):
        """
        Compare two statesets for equivalence. 
        @param : tarStateSet
        the StateSet to be compared with this one. 
        @return TRUE if the two StateSet objects are composed of the
        same StateTypes.
        """
        func = self.get_dbus_method("equals")
        return func(*args, **kwargs)
    
    def getStates(self, *args, **kwargs):
        func = self.get_dbus_method("getStates")
        return func(*args, **kwargs)
    
    def isEmpty(self, *args, **kwargs):
        """
        @return TRUE if the StateSet contains no states.
        """
        func = self.get_dbus_method("isEmpty")
        return func(*args, **kwargs)
    
    def remove(self, *args, **kwargs):
        """
        Remove a StateType to an existing StateSet, if it is present.
        """
        func = self.get_dbus_method("remove")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)


class StateType(Enum):
    _enum_lookup = {
        0:'STATE_INVALID',
        1:'STATE_ACTIVE',
        2:'STATE_ARMED',
        3:'STATE_BUSY',
        4:'STATE_CHECKED',
        5:'STATE_COLLAPSED',
        6:'STATE_DEFUNCT',
        7:'STATE_EDITABLE',
        8:'STATE_ENABLED',
        9:'STATE_EXPANDABLE',
        10:'STATE_EXPANDED',
        11:'STATE_FOCUSABLE',
        12:'STATE_FOCUSED',
        13:'STATE_HAS_TOOLTIP',
        14:'STATE_HORIZONTAL',
        15:'STATE_ICONIFIED',
        16:'STATE_MODAL',
        17:'STATE_MULTI_LINE',
        18:'STATE_MULTISELECTABLE',
        19:'STATE_OPAQUE',
        20:'STATE_PRESSED',
        21:'STATE_RESIZABLE',
        22:'STATE_SELECTABLE',
        23:'STATE_SELECTED',
        24:'STATE_SENSITIVE',
        25:'STATE_SHOWING',
        26:'STATE_SINGLE_LINE',
        27:'STATE_STALE',
        28:'STATE_TRANSIENT',
        29:'STATE_VERTICAL',
        30:'STATE_VISIBLE',
        31:'STATE_MANAGES_DESCENDANTS',
        32:'STATE_INDETERMINATE',
        33:'STATE_REQUIRED',
        34:'STATE_TRUNCATED',
        35:'STATE_ANIMATED',
        36:'STATE_INVALID_ENTRY',
        37:'STATE_SUPPORTS_AUTOCOMPLETION',
        38:'STATE_SELECTABLE_TEXT',
        39:'STATE_IS_DEFAULT',
        40:'STATE_VISITED',
        41:'STATE_LAST_DEFINED',
    }

class StreamableContent(BaseProxy):
    """
    An interface whereby an object allows its backing content to
    be streamed to clients. Negotiation of content type is allowed.
    Clients may examine the backing data and transform, convert,
    or parse the content in order to present it in an alternate form
    to end-users.
    """
    
    def getContent(self, *args, **kwargs):
        """
        DEPRECATED, use getStream instead. getContent: Retrieve this
        object's content, in a format appropriate to a requested mimetype.
                long Bonobo::Stream:seek (in long offset, in SeekType
        whence)
                        raises (NoPermission, IOError)
                void Bonobo::Stream:read (in long count, out iobuf buffer)
                        raises (NoPermission, IOError)
               
        @return a Bonobo::Stream whose mimetype matches contentType,
        if available, or NIL.
        """
        func = self.get_dbus_method("getContent")
        return func(*args, **kwargs)
    
    def getContentTypes(self, *args, **kwargs):
        """
        getContentTypes: 
        @return the list of available mimetypes for this object's content.
        """
        func = self.get_dbus_method("getContentTypes")
        return func(*args, **kwargs)
    
    def getStream(self, *args, **kwargs):
        """
        Retrieve this object's content, in a format appropriate to a
        requested mimetype, as a ContentStream instance.
        @param : contentType
        a string specifying the desired mimetype for the content stream.
        @return a Stream whose mimetype matches contentType, if available,
        or NIL.
        """
        func = self.get_dbus_method("getStream")
        return func(*args, **kwargs)
    
    def getURI(self, *args, **kwargs):
        """
        Get a URI pointing to the content of the specified type, if such
        a URI can be obtained. Not all streamable content providers have
        URI representations.
        @param : contentType
        a string specifying the desired mimetype for the content stream.
        If NULL, then a URI for the default content type will be returned,
        if available.
        @return a string which constitutes a URI for a stream of the
        specified content type, or NULL if no such URI can be obtained.
        """
        func = self.get_dbus_method("getURI")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)


class TEXT_BOUNDARY_TYPE(Enum):
    _enum_lookup = {
        0:'TEXT_BOUNDARY_CHAR',
        1:'TEXT_BOUNDARY_WORD_START',
        2:'TEXT_BOUNDARY_WORD_END',
        3:'TEXT_BOUNDARY_SENTENCE_START',
        4:'TEXT_BOUNDARY_SENTENCE_END',
        5:'TEXT_BOUNDARY_LINE_START',
        6:'TEXT_BOUNDARY_LINE_END',
    }

class TEXT_CLIP_TYPE(Enum):
    _enum_lookup = {
        0:'TEXT_CLIP_NONE',
        1:'TEXT_CLIP_MIN',
        2:'TEXT_CLIP_MAX',
        3:'TEXT_CLIP_BOTH',
    }

class Table(BaseProxy):
    """
    An interface used by containers whose contained data is arranged
    in a "tabular" (i.e. row-column) fashion. Tables may resemble
    a two-dimensional grid, as in a spreadsheet, or may feature objects
    which span multiple rows and/or columns, but whose bounds are
    aligned on a row/column matrix. Thus, the Table interface may
    be used to represent "spreadsheets" as well as "frames".
    Objects within tables are children of the Table instance, and
    they may be referenced either via a child index or via a row/column
    pair. Their role may be ROLE_TABLE_CELL, but table 'cells' may
    have other roles as well. These 'cells' may implement other interfaces,
    such as Text, Action, Image, and Component, and should do so
    as appropriate to their onscreen representation and/or behavior.
    """
    
    def addColumnSelection(self, *args, **kwargs):
        """
        Select the specified column, adding it to the current column
        selection, if the table's selection model permits it.
        @param : column
        @return True if the specified column was successfully selected,
        False if not.
        """
        func = self.get_dbus_method("addColumnSelection")
        return func(*args, **kwargs)
    
    def addRowSelection(self, *args, **kwargs):
        """
        Select the specified row, adding it to the current row selection,
        if the table's selection model permits it.
        @param : row
        @return True if the specified row was successfully selected,
        False if not.
        """
        func = self.get_dbus_method("addRowSelection")
        return func(*args, **kwargs)
    
    def getAccessibleAt(self, *args, **kwargs):
        """
        Get the table cell at the specified row and column indices. 
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return an Accessible object representing the specified table
        cell.
        """
        func = self.get_dbus_method("getAccessibleAt")
        return func(*args, **kwargs)
    
    def getColumnAtIndex(self, *args, **kwargs):
        """
        Get the table column index occupied by the child at a particular
        1-D child index.
        @param : index
        the specified child index, zero-indexed.
        @return a long integer indicating the first column spanned by
        the child of a table, at the specified 1-D (zero-offset) index.
        """
        func = self.get_dbus_method("getColumnAtIndex")
        return func(*args, **kwargs)
    
    def getColumnDescription(self, *args, **kwargs):
        """
        Get a text description of a particular table column. This differs
        from AccessibleTable_getColumnHeader, which returns an Accessible.
        @param : column
        the specified table column, zero-indexed.
        @return a UTF-8 string describing the specified table column,
        if available.
        """
        func = self.get_dbus_method("getColumnDescription")
        return func(*args, **kwargs)
    
    def getColumnExtentAt(self, *args, **kwargs):
        """
        Get the number of columns spanned by the table cell at the specific
        row and column. (some tables can have cells which span multiple
        rows and/or columns).
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return a long integer indicating the number of columns spanned
        by the specified cell.
        """
        func = self.get_dbus_method("getColumnExtentAt")
        return func(*args, **kwargs)
    
    def getColumnHeader(self, *args, **kwargs):
        """
        Get the header associated with a table column, if available,
        as an instance of Accessible. This differs from getColumnDescription,
        which returns a string.
        @param : column
        the specified table column, zero-indexed.
        @return an Accessible representatin of the specified table column,
        if available.
        """
        func = self.get_dbus_method("getColumnHeader")
        return func(*args, **kwargs)
    
    def getIndexAt(self, *args, **kwargs):
        """
        Get the 1-D child index corresponding to the specified 2-D row
        and column indices. 
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return a long integer which serves as the index of a specified
        cell in the table, in a form usable by Accessible::getChildAtIndex.
        """
        func = self.get_dbus_method("getIndexAt")
        return func(*args, **kwargs)
    
    def getRowAtIndex(self, *args, **kwargs):
        """
        Get the table row index occupied by the child at a particular
        1-D child index.
        @param : index
        the specified child index, zero-indexed.
        @return a long integer indicating the first row spanned by the
        child of a table, at the specified 1-D (zero-offset) index.
        """
        func = self.get_dbus_method("getRowAtIndex")
        return func(*args, **kwargs)
    
    def getRowColumnExtentsAtIndex(self, *args, **kwargs):
        """
        Given a child index, determine the row and column indices and
        extents, and whether the cell is currently selected. If the child
        at index is not a cell (for instance, if it is a summary, caption,
        etc.), False is returned.
        @param : index
        the index of the Table child whose row/column extents are requested.
        @param : row
        back-filled with the first table row associated with the cell
        with child index index. 
        @param : col
        back-filled with the first table column associated with the cell
        with child index index. 
        @param : row_extents
        back-filled with the number of table rows across which child
        i extends. 
        @param : col_extents
        back-filled with the number of table columns across which child
        i extends. 
        @param : is_selected
        a boolean which is back-filled with True if the child at index
        i corresponds to a selected table cell, False otherwise.
        Example: If the Table child at index '6' extends across columns
        5 and 6 of row 2 of a Table instance, and is currently selected,
        then retval=table::getRowColumnExtentsAtIndex(6,row,col,
        row_extents,
        col_extents,
        is_selected);
         will return True, and after the call row, col, row_extents,
        col_extents, and is_selected will contain 2, 5, 1, 2, and True,
        respectively.
        @return True if the index is associated with a valid table cell,
        False if the index does not correspond to a cell. If False is
        returned, the values of the out parameters are undefined.
        """
        func = self.get_dbus_method("getRowColumnExtentsAtIndex")
        return func(*args, **kwargs)
    
    def getRowDescription(self, *args, **kwargs):
        """
        Get a text description of a particular table row. This differs
        from AccessibleTable_getRowHeader, which returns an Accessible.
        @param : row
        the specified table row, zero-indexed.
        @return a UTF-8 string describing the specified table row, if
        available.
        """
        func = self.get_dbus_method("getRowDescription")
        return func(*args, **kwargs)
    
    def getRowExtentAt(self, *args, **kwargs):
        """
        Get the number of rows spanned by the table cell at the specific
        row and column. (some tables can have cells which span multiple
        rows and/or columns).
        @param : row
        the specified table row, zero-indexed. 
        @param : column
        the specified table column, zero-indexed.
        @return a long integer indicating the number of rows spanned
        by the specified cell.
        """
        func = self.get_dbus_method("getRowExtentAt")
        return func(*args, **kwargs)
    
    def getRowHeader(self, *args, **kwargs):
        """
        Get the header associated with a table row, if available. This
        differs from getRowDescription, which returns a string.
        @param : row
        the specified table row, zero-indexed.
        @return an Accessible representatin of the specified table row,
        if available.
        """
        func = self.get_dbus_method("getRowHeader")
        return func(*args, **kwargs)
    
    def getSelectedColumns(self, *args, **kwargs):
        """
        Obtain the indices of all columns which are currently selected.
        @return a sequence of integers comprising the indices of columns
        currently selected.
        """
        func = self.get_dbus_method("getSelectedColumns")
        return func(*args, **kwargs)
    
    def getSelectedRows(self, *args, **kwargs):
        """
        Obtain the indices of all rows which are currently selected.
        @return a sequence of integers comprising the indices of rows
        currently selected.
        """
        func = self.get_dbus_method("getSelectedRows")
        return func(*args, **kwargs)
    
    def isColumnSelected(self, *args, **kwargs):
        """
        Determine whether a table column is selected. 
        @param : column
        the column being queried.
        @return True if the specified column is currently selected, False
        if not.
        """
        func = self.get_dbus_method("isColumnSelected")
        return func(*args, **kwargs)
    
    def isRowSelected(self, *args, **kwargs):
        """
        Determine whether a table row is selected. 
        @param : row
        the row being queried.
        @return True if the specified row is currently selected, False
        if not.
        """
        func = self.get_dbus_method("isRowSelected")
        return func(*args, **kwargs)
    
    def isSelected(self, *args, **kwargs):
        """
        Determine whether the cell at a specific row and column is selected.
        @param : row
        a row occupied by the cell whose state is being queried. 
        @param : column
        a column occupied by the cell whose state is being queried.
        @return True if the specified cell is currently selected, False
        if not.
        """
        func = self.get_dbus_method("isSelected")
        return func(*args, **kwargs)
    
    def removeColumnSelection(self, *args, **kwargs):
        """
        Remove the specified column from current column selection, if
        the table's selection model permits it.
        @param : column
        @return True if the specified column was successfully de-selected,
        False if not.
        """
        func = self.get_dbus_method("removeColumnSelection")
        return func(*args, **kwargs)
    
    def removeRowSelection(self, *args, **kwargs):
        """
        Remove the specified row from current row selection, if the table's
        selection model permits it.
        @param : row
        @return True if the specified row was successfully de-selected,
        False if not.
        """
        func = self.get_dbus_method("removeRowSelection")
        return func(*args, **kwargs)
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)
    
    def unImplemented5(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented5")
        return func(*args, **kwargs)
    
    def unImplemented6(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented6")
        return func(*args, **kwargs)
    
    def unImplemented7(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented7")
        return func(*args, **kwargs)
    
    def get_caption(self):
        self._pgetter(self._dbus_interface, "caption")
    def set_caption(self, value):
        self._psetter(self._dbus_interface, "caption", value)
    _captionDoc = \
        """
        An Accessible which represents of a caption for a Table.
        """
    caption = property(fget=get_caption, fset=set_caption, doc=_captionDoc)
    
    def get_nColumns(self):
        self._pgetter(self._dbus_interface, "nColumns")
    def set_nColumns(self, value):
        self._psetter(self._dbus_interface, "nColumns", value)
    _nColumnsDoc = \
        """
        The total number of columns in this table (including empty columns),
        exclusive of columns which are programmatically hidden. Columns
        which are scrolled out of view or clipped by the current viewport
        are included.
        """
    nColumns = property(fget=get_nColumns, fset=set_nColumns, doc=_nColumnsDoc)
    
    def get_nRows(self):
        self._pgetter(self._dbus_interface, "nRows")
    def set_nRows(self, value):
        self._psetter(self._dbus_interface, "nRows", value)
    _nRowsDoc = \
        """
        The total number of rows in this table (including empty rows),
        exclusive of any rows which are programmatically hidden. Rows
        which are merely scrolled out of view are included.
        """
    nRows = property(fget=get_nRows, fset=set_nRows, doc=_nRowsDoc)
    
    def get_nSelectedColumns(self):
        self._pgetter(self._dbus_interface, "nSelectedColumns")
    def set_nSelectedColumns(self, value):
        self._psetter(self._dbus_interface, "nSelectedColumns", value)
    _nSelectedColumnsDoc = \
        """
        The number of columns currently selected. A selected column is
        one in which all included cells are selected.
        """
    nSelectedColumns = property(fget=get_nSelectedColumns, fset=set_nSelectedColumns, doc=_nSelectedColumnsDoc)
    
    def get_nSelectedRows(self):
        self._pgetter(self._dbus_interface, "nSelectedRows")
    def set_nSelectedRows(self, value):
        self._psetter(self._dbus_interface, "nSelectedRows", value)
    _nSelectedRowsDoc = \
        """
        The number of rows currently selected. A selected row is one
        in which all included cells are selected.
        """
    nSelectedRows = property(fget=get_nSelectedRows, fset=set_nSelectedRows, doc=_nSelectedRowsDoc)
    
    def get_summary(self):
        self._pgetter(self._dbus_interface, "summary")
    def set_summary(self, value):
        self._psetter(self._dbus_interface, "summary", value)
    _summaryDoc = \
        """
        An accessible object which summarizes the contents of a Table.
        This object is frequently itself a Table instance, albeit a simplified
        one.
        """
    summary = property(fget=get_summary, fset=set_summary, doc=_summaryDoc)



class Value(BaseProxy):
    """
    An interface supporting controls which allow a one-dimensional,
    scalar quantity to be modified or which reflect a scalar quantity.
    (If STATE_EDITABLE is not present, the valuator is treated as
    "read only".
    """
    
    def unImplemented(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented")
        return func(*args, **kwargs)
    
    def unImplemented2(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented2")
        return func(*args, **kwargs)
    
    def unImplemented3(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented3")
        return func(*args, **kwargs)
    
    def unImplemented4(self, *args, **kwargs):
        func = self.get_dbus_method("unImplemented4")
        return func(*args, **kwargs)
    
    def get_currentValue(self):
        self._pgetter(self._dbus_interface, "currentValue")
    def set_currentValue(self, value):
        self._psetter(self._dbus_interface, "currentValue", value)
    _currentValueDoc = \
        """
        The current value of the valuator.
        """
    currentValue = property(fget=get_currentValue, fset=set_currentValue, doc=_currentValueDoc)
    
    def get_maximumValue(self):
        self._pgetter(self._dbus_interface, "maximumValue")
    def set_maximumValue(self, value):
        self._psetter(self._dbus_interface, "maximumValue", value)
    _maximumValueDoc = \
        """
        The maximum value allowed by this valuator.
        """
    maximumValue = property(fget=get_maximumValue, fset=set_maximumValue, doc=_maximumValueDoc)
    
    def get_minimumIncrement(self):
        self._pgetter(self._dbus_interface, "minimumIncrement")
    def set_minimumIncrement(self, value):
        self._psetter(self._dbus_interface, "minimumIncrement", value)
    _minimumIncrementDoc = \
        """
        The smallest incremental change which this valuator allows. If
        0, the incremental changes to the valuator are limited only by
        the precision of a double precision value on the platform.
        """
    minimumIncrement = property(fget=get_minimumIncrement, fset=set_minimumIncrement, doc=_minimumIncrementDoc)
    
    def get_minimumValue(self):
        self._pgetter(self._dbus_interface, "minimumValue")
    def set_minimumValue(self, value):
        self._psetter(self._dbus_interface, "minimumValue", value)
    _minimumValueDoc = \
        """
        The minimum value allowed by this valuator.
        """
    minimumValue = property(fget=get_minimumValue, fset=set_minimumValue, doc=_minimumValueDoc)

BUTTON_PRESSED_EVENT = EventType(2)

BUTTON_RELEASED_EVENT = EventType(3)

KEY_PRESS = KeySynthType(0)

KEY_PRESSED = KeyEventType(0)

KEY_PRESSED_EVENT = EventType(0)

KEY_PRESSRELEASE = KeySynthType(2)

KEY_RELEASE = KeySynthType(1)

KEY_RELEASED = KeyEventType(1)

KEY_RELEASED_EVENT = EventType(1)

KEY_STRING = KeySynthType(4)

KEY_SYM = KeySynthType(3)

LAYER_BACKGROUND = ComponentLayer(1)

LAYER_CANVAS = ComponentLayer(2)

LAYER_INVALID = ComponentLayer(0)

LAYER_LAST_DEFINED = ComponentLayer(8)

LAYER_MDI = ComponentLayer(4)

LAYER_OVERLAY = ComponentLayer(6)

LAYER_POPUP = ComponentLayer(5)

LAYER_WIDGET = ComponentLayer(3)

LAYER_WINDOW = ComponentLayer(7)

LOCALE_TYPE_COLLATE = LOCALE_TYPE(1)

LOCALE_TYPE_CTYPE = LOCALE_TYPE(2)

LOCALE_TYPE_MESSAGES = LOCALE_TYPE(0)

LOCALE_TYPE_MONETARY = LOCALE_TYPE(3)

LOCALE_TYPE_NUMERIC = LOCALE_TYPE(4)

LOCALE_TYPE_TIME = LOCALE_TYPE(5)

MODIFIER_ALT = ModifierType(3)

MODIFIER_CONTROL = ModifierType(2)

MODIFIER_META = ModifierType(4)

MODIFIER_META2 = ModifierType(5)

MODIFIER_META3 = ModifierType(6)

MODIFIER_NUMLOCK = ModifierType(7)

MODIFIER_SHIFT = ModifierType(0)

MODIFIER_SHIFTLOCK = ModifierType(1)

RELATION_CONTROLLED_BY = RelationType(4)

RELATION_CONTROLLER_FOR = RelationType(3)

RELATION_DESCRIBED_BY = RelationType(17)

RELATION_DESCRIPTION_FOR = RelationType(16)

RELATION_EMBEDDED_BY = RelationType(13)

RELATION_EMBEDS = RelationType(12)

RELATION_EXTENDED = RelationType(8)

RELATION_FLOWS_FROM = RelationType(10)

RELATION_FLOWS_TO = RelationType(9)

RELATION_LABELLED_BY = RelationType(2)

RELATION_LABEL_FOR = RelationType(1)

RELATION_LAST_DEFINED = RelationType(18)

RELATION_MEMBER_OF = RelationType(5)

RELATION_NODE_CHILD_OF = RelationType(7)

RELATION_NULL = RelationType(0)

RELATION_PARENT_WINDOW_OF = RelationType(15)

RELATION_POPUP_FOR = RelationType(14)

RELATION_SUBWINDOW_OF = RelationType(11)

RELATION_TOOLTIP_FOR = RelationType(6)

ROLE_ACCELERATOR_LABEL = Role(1)

ROLE_ALERT = Role(2)

ROLE_ANIMATION = Role(3)

ROLE_APPLICATION = Role(75)

ROLE_ARROW = Role(4)

ROLE_AUTOCOMPLETE = Role(76)

ROLE_CALENDAR = Role(5)

ROLE_CANVAS = Role(6)

ROLE_CAPTION = Role(81)

ROLE_CHART = Role(80)

ROLE_CHECK_BOX = Role(7)

ROLE_CHECK_MENU_ITEM = Role(8)

ROLE_COLOR_CHOOSER = Role(9)

ROLE_COLUMN_HEADER = Role(10)

ROLE_COMBO_BOX = Role(11)

ROLE_DATE_EDITOR = Role(12)

ROLE_DESKTOP_FRAME = Role(14)

ROLE_DESKTOP_ICON = Role(13)

ROLE_DIAL = Role(15)

ROLE_DIALOG = Role(16)

ROLE_DIRECTORY_PANE = Role(17)

ROLE_DOCUMENT_FRAME = Role(82)

ROLE_DRAWING_AREA = Role(18)

ROLE_EDITBAR = Role(77)

ROLE_EMBEDDED = Role(78)

ROLE_ENTRY = Role(79)

ROLE_EXTENDED = Role(70)

ROLE_FILE_CHOOSER = Role(19)

ROLE_FILLER = Role(20)

ROLE_FOCUS_TRAVERSABLE = Role(21)

ROLE_FONT_CHOOSER = Role(22)

ROLE_FOOTER = Role(72)

ROLE_FORM = Role(87)

ROLE_FRAME = Role(23)

ROLE_GLASS_PANE = Role(24)

ROLE_HEADER = Role(71)

ROLE_HEADING = Role(83)

ROLE_HTML_CONTAINER = Role(25)

ROLE_ICON = Role(26)

ROLE_IMAGE = Role(27)

ROLE_INPUT_METHOD_WINDOW = Role(89)

ROLE_INTERNAL_FRAME = Role(28)

ROLE_INVALID = Role(0)

ROLE_LABEL = Role(29)

ROLE_LAST_DEFINED = Role(90)

ROLE_LAYERED_PANE = Role(30)

ROLE_LINK = Role(88)

ROLE_LIST = Role(31)

ROLE_LIST_ITEM = Role(32)

ROLE_MENU = Role(33)

ROLE_MENU_BAR = Role(34)

ROLE_MENU_ITEM = Role(35)

ROLE_OPTION_PANE = Role(36)

ROLE_PAGE = Role(84)

ROLE_PAGE_TAB = Role(37)

ROLE_PAGE_TAB_LIST = Role(38)

ROLE_PANEL = Role(39)

ROLE_PARAGRAPH = Role(73)

ROLE_PASSWORD_TEXT = Role(40)

ROLE_POPUP_MENU = Role(41)

ROLE_PROGRESS_BAR = Role(42)

ROLE_PUSH_BUTTON = Role(43)

ROLE_RADIO_BUTTON = Role(44)

ROLE_RADIO_MENU_ITEM = Role(45)

ROLE_REDUNDANT_OBJECT = Role(86)

ROLE_ROOT_PANE = Role(46)

ROLE_ROW_HEADER = Role(47)

ROLE_RULER = Role(74)

ROLE_SCROLL_BAR = Role(48)

ROLE_SCROLL_PANE = Role(49)

ROLE_SECTION = Role(85)

ROLE_SEPARATOR = Role(50)

ROLE_SLIDER = Role(51)

ROLE_SPIN_BUTTON = Role(52)

ROLE_SPLIT_PANE = Role(53)

ROLE_STATUS_BAR = Role(54)

ROLE_TABLE = Role(55)

ROLE_TABLE_CELL = Role(56)

ROLE_TABLE_COLUMN_HEADER = Role(57)

ROLE_TABLE_ROW_HEADER = Role(58)

ROLE_TEAROFF_MENU_ITEM = Role(59)

ROLE_TERMINAL = Role(60)

ROLE_TEXT = Role(61)

ROLE_TOGGLE_BUTTON = Role(62)

ROLE_TOOL_BAR = Role(63)

ROLE_TOOL_TIP = Role(64)

ROLE_TREE = Role(65)

ROLE_TREE_TABLE = Role(66)

ROLE_UNKNOWN = Role(67)

ROLE_VIEWPORT = Role(68)

ROLE_WINDOW = Role(69)

STATE_ACTIVE = StateType(1)

STATE_ANIMATED = StateType(35)

STATE_ARMED = StateType(2)

STATE_BUSY = StateType(3)

STATE_CHECKED = StateType(4)

STATE_COLLAPSED = StateType(5)

STATE_DEFUNCT = StateType(6)

STATE_EDITABLE = StateType(7)

STATE_ENABLED = StateType(8)

STATE_EXPANDABLE = StateType(9)

STATE_EXPANDED = StateType(10)

STATE_FOCUSABLE = StateType(11)

STATE_FOCUSED = StateType(12)

STATE_HAS_TOOLTIP = StateType(13)

STATE_HORIZONTAL = StateType(14)

STATE_ICONIFIED = StateType(15)

STATE_INDETERMINATE = StateType(32)

STATE_INVALID = StateType(0)

STATE_INVALID_ENTRY = StateType(36)

STATE_IS_DEFAULT = StateType(39)

STATE_LAST_DEFINED = StateType(41)

STATE_MANAGES_DESCENDANTS = StateType(31)

STATE_MODAL = StateType(16)

STATE_MULTISELECTABLE = StateType(18)

STATE_MULTI_LINE = StateType(17)

STATE_OPAQUE = StateType(19)

STATE_PRESSED = StateType(20)

STATE_REQUIRED = StateType(33)

STATE_RESIZABLE = StateType(21)

STATE_SELECTABLE = StateType(22)

STATE_SELECTABLE_TEXT = StateType(38)

STATE_SELECTED = StateType(23)

STATE_SENSITIVE = StateType(24)

STATE_SHOWING = StateType(25)

STATE_SINGLE_LINE = StateType(26)

STATE_STALE = StateType(27)

STATE_SUPPORTS_AUTOCOMPLETION = StateType(37)

STATE_TRANSIENT = StateType(28)

STATE_TRUNCATED = StateType(34)

STATE_VERTICAL = StateType(29)

STATE_VISIBLE = StateType(30)

STATE_VISITED = StateType(40)

TEXT_BOUNDARY_CHAR = TEXT_BOUNDARY_TYPE(0)

TEXT_BOUNDARY_LINE_END = TEXT_BOUNDARY_TYPE(6)

TEXT_BOUNDARY_LINE_START = TEXT_BOUNDARY_TYPE(5)

TEXT_BOUNDARY_SENTENCE_END = TEXT_BOUNDARY_TYPE(4)

TEXT_BOUNDARY_SENTENCE_START = TEXT_BOUNDARY_TYPE(3)

TEXT_BOUNDARY_WORD_END = TEXT_BOUNDARY_TYPE(2)

TEXT_BOUNDARY_WORD_START = TEXT_BOUNDARY_TYPE(1)

TEXT_CLIP_BOTH = TEXT_CLIP_TYPE(3)

TEXT_CLIP_MAX = TEXT_CLIP_TYPE(2)

TEXT_CLIP_MIN = TEXT_CLIP_TYPE(1)

TEXT_CLIP_NONE = TEXT_CLIP_TYPE(0)


