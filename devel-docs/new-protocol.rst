Next-generation accessibility architecture
==========================================

This is a high-level overview of a proposed next-generation accessibility
architecture for free software platforms.

Goals
-----

- Sandboxing support.

- Window system independence in the core, with Wayland integration layered on
  top.

- Minimal impact on performance and resource usage when assistive technologies
  are not active.

- Low latency, especially when assistive technologies are responding to user
  commands.

- Efficient support for assistive technologies that need to traverse the whole
  accessibility tree; we must not assume that we know what specific access
  patterns ATs will need.

- Network transparency with reasonable performance, enabling accessible remote
  desktops, something that even proprietary platforms generally don't support.

- Support for one-to-many use cases, both live and pre-recorded, such as
  accessible screencasts and screenshots; no platform does this yet.

Terminology
-----------

This document uses the terms "client" and "provider" as defined by Microsoft's
UI Automation (UIA) API. Briefly, the applications that are being made
accessible are providers; the assistive technologies are clients.

Core concepts
-------------

The central difference between this accessibility architecture and most others
being used today, including AT-SPI, is that this architecture is
push-based. Providers push full accessibility trees and incremental tree updates
to interested clients. Clients only make requests to individual providers to
perform actions that change the state of the UI, such as programmatically
setting focus or activating a UI element (the equivalent of a mouse
click). Clients don't send requests to query the tree; each client maintains a
local copy of the tree based on pushes from the provider, and queries that
copy. This is true even for features that have traditionally relied heavily on
IPC, such as rich text support. The current plan is for clients to infer events
from changes to the tree, by comparing the old and new versions of updated
nodes.

The key to making all of this work is to define the optimal set of node roles,
properties, and relationships. The primary inspiration for the proposed
architecture is the internal accessibility architecture of Chromium, which the
Firefox accessibility team has recently imitated (with some differences) in its
Cache The World project. Because Chromium and Firefox are multi-process, while
current pull-based platform accessibility APIs are synchronous, and considering
that having a high number of IPC round trips is inefficient, both browsers
arrived at an architecture where the sandboxed content/renderer process pushes
accessibility trees and tree updates to the privileged main process. What we
propose here is to extend this push-based approach all the way to the assistive
technologies.

One guiding principle for the design of accessibility properties in this
architecture is that they should be independent of global context. For example,
all coordinates are relative to the containing surface, in contrast with
AT-SPI's absolute screen coordinates. For the same reason, and also for
security, providers must not be required, or even able, to tell clients which
provider has the global focus; clients will get that information from the
windowing system (e.g. Wayland compositor).

We propose a separate accessibility tree for each surface, as opposed to
AT-SPI's tree per application. In our experience, assistive technologies rarely,
if ever, need to traverse across surface boundaries. If needed, client code can
combine the separate trees into a single tree; with the push approach, all trees
will already be immediately available in the client's address space. In our
opinion, there's no need to have application nodes. However, we understand the
importance of clients knowing which application or toolkit a particular tree is
coming from, so they can implement application-specific workarounds.

Tree snapshots and updates are analogous to frames of visual output, in that all
information is made available at once, without requiring further
request/response round trips. Our proposed architecture shares `Wayland's goal
of atomicity
<https://wayland-book.com/protocol-design/design-patterns.html#atomicity>`_,
where "every frame is perfect." Ideally, each accessibility tree update should
be pushed at the same time as its corresponding visual frame.

We will make sure that providers know at all times whether any clients are
currently active, to minimize the performance usage and resource footprint of
the accessibility stack when no ATs are active. Users must be able to load and
unload ATs at will, without having to restart applications.

Benefits and risks
------------------

This push-based architecture meets all of the goals listed above, including the
later goals which, as noted above, advance the state of the art for
accessibility even compared to proprietary platforms.

Notably, the push approach means that assistive technologies can be fully
responsive even when an application's UI thread is busy or hung. Just as sighted
users can still see the latest visual frame in this case, AT users will be able
to query the latest tree snapshot, even if the application itself doesn't
respond to their actions.

Because two of the three major browser engines already use a push approach
internally, the core concept has been well validated. With careful design of the
core serialized data formats, the browsers' content/renderer processes should be
able to push tree updates in our format to assistive technologies with minimal
involvement from the privileged main browser process, thus reducing the scope of
that security-sensitive process.

The push approach does have risks. In particular, because it requires all
details of an accessibility tree to be pushed up front, there is a risk that,
particularly for large, complex trees, such as long text documents and large
lists or tables, performance may be degraded compared to a pull-based approach
where clients send queries on demand via IPC. However, for lists or tables, in
both native and web applications, it's common to implement a virtualized view,
where view-level data structures are created lazily for only a portion of the
full dataset. We believe that it's similarly feasible to virtualize large text
documents, though this needs to be done carefully to avoid degrading the
functionality that AT users have come to expect. Some legacy providers may have
been implemented with the assumption that the accessibility tree is queried
lazily, rather than being traversed in its entirety up-front. If we find that
this is a problem in practice, it may be necessary for such providers to stay on
AT-SPI rather than the new architecture. It will likely not be possible to
completely discontinue AT-SPI support anyway.

Another challenge for the push approach is hit-testing. ATs need to be able to
determine which UI element is at a particular point, so they can respond to
mouse movement and touch input. In current accessibility APIs, the client sends
a hit-test request to the provider, which typically passes the request onto an
existing hit-testing implementation in the GUI toolkit, browser engine,
etc. These existing hit-testing implementations are often complex, particularly
for browser engines, and it may be challenging to correctly match their behavior
in client implementations using information that can be practically conveyed in
a provider-independent accessibility tree. But because a hit-test is a query
rather than a UI-updating action, in keeping with the goals set out above, this
query should be done locally in the client, not through IPC. Chromium worked
around the problem by responding to the first hit-test request using information
in the accessibility tree, but then asynchronously sending a hit-test request to
the renderer and using the response in future accessibility hit-tests. The
Firefox Cache The World project implemented an alternative solution, where the
information pushed by the content process includes a list of on-screen nodes
sorted in z-order. Our current plan is to follow the Firefox approach.

Overall, we believe the benefits of this proposed architecture, particularly the
potential ways of advancing the state of the art in accessibility as outlined
above, outweigh the risks.

Open questions
--------------

Host protocol: Wayland or D-Bus
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We'll need to work with key decision-makers, including toolkit and compositor
developers, to determine whether our proposed architecture will be integrated
with the broader free desktop stack via Wayland or D-Bus. We would prefer to
extend Wayland rather than running in parallel with Wayland via D-Bus as AT-SPI
does. We believe it's important that clients get some key global information,
such as the set of all surfaces with accessibility trees and the currently
focused surface, directly from the compositor. Mediating communication between
clients and providers through the Wayland compositor reduces security
considerations specific to the accessibility stack. Finally, as noted above, it
would be ideal to synchronize accessibility tree updates with corresponding
visual frames, and a Wayland protocol extension seems like the best way to do
this. However, we understand that the decision will ultimately need to be made
based on whatever is most politically feasible, not what's technically ideal.

Serialization format
~~~~~~~~~~~~~~~~~~~~

To minimize the performance risks of the push approach, we'd prefer to use a
zero-copy serialization format such as `Cap'n Proto
<https://capnproto.org/>`_. Note that we would only use the serialization layer
of Cap'n Proto, not the RPC system. The reference implementation in C++ is under
the permissive MIT license; implementations for other languages are under
similarly permissive licenses. A downside of choosing Cap'n Proto is that there
isn't an actively maintained C implementation. `FlatBuffers
<https://flatbuffers.dev/>`_ is conceptually similar, and it has an active C
implementation, but the existing implementations are under the Apache 2.0
license, which is incompatible with GPLv2. In any case, we realize that it may
be better to choose a serialization format that's already being used by the free
desktop stack, such as the D-Bus serialization format. In addition, addressing
performance issues at an architectural level, e.g. by virtualizing large
documents or lists as discussed above, is likely to be more effective than
trying to optimize serialization.

Project plan
------------

This proposed architecture will build on the work that has already been done by
the `AccessKit <https://github.com/AccessKit/accesskit>`_ project. The AccessKit
schema, which defines all node properties, as well as roles and other enums, is
heavily based on Chromium's accessibility architecture, and has gone through
several iterations of its own. However, the schema isn't yet frozen; we'll need
to get it to that point as part of this project. We're already planning to make
some modest changes to the AccessKit schema to meet the specific goals of this
architecture.

Once we've agreed on a host protocol (Wayland or D-Bus), we'll prototype a
provider and a client. The first provider will be an AccessKit platform adapter,
written in Rust. The first client for this architecture will be a bridge to
AT-SPI, building on AccessKit's current AT-SPI provider implementation, which is
also written in Rust.

A key use case which needs to be validated as part of this project is web
browsing. We believe it would make the most sense to implement a provider for
the new architecture in Chromium first, because Chromium and AccessKit are most
conceptually similar, and because all relevant code in Chromium is under a
permissive BSD license. This use case will certainly require us to work through
several limitations of AccessKit, in both the schema and the AT-SPI
provider. Thus, implementing the new architecture in a browser engine is likely
to be the most time-consuming part of the whole project.

An AT-SPI provider that acts as a proxy for the new architecture, as described
above, won't meet all of the goals of the project, particularly around latency
and efficiency, due to the pull-based, IPC-heavy nature of AT-SPI. To meet these
goals completely, ATs such as Orca will need to directly implement the client
side of the new architecture, while continuing to support AT-SPI for backward
compatibility. Our current plan is to implement our client library in Rust and
provide Python bindings for it using PyO3. If it's deemed necessary to implement
client support for the new architecture in pyatspi and the underlying C library,
to avoid major refactoring of the ATs themselves, that will increase the
difficulty of this part of the project.
