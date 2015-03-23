# Introduction #

The pt\_event object (with FLTK-implementing subclass) is the interface that allows events to be handled with the future option of migrating from FLTK.

# Usage #
A viewport\_event object is kept in each [FLTKviewport](FLTKviewport.md). It is updated prior to engaging the callback (function viewport\_callback), the [viewport](viewport.md) then accesses this object and handles the event.

When a pt\_event is handled, the `grab` metod has to be called to mark the event as taken care of.