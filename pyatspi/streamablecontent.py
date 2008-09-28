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

import interfaces
from base import BaseProxy, Enum
from factory import add_accessible_class

__all__ = [
	   "ContentStream",
	   "StreamableContent",
	  ]

#------------------------------------------------------------------------------

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

#------------------------------------------------------------------------------

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
    
# ATTENTION - Register the Application class with the accessible factory.
add_accessible_class(interfaces.ATSPI_STREAMABLE_CONTENT, StreamableContent)

#END----------------------------------------------------------------------------
