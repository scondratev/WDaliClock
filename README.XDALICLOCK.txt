                                 XDaliClock

XDaliClock is a digital clock for the X Window System; when a digit changes,
it ``melts'' into its new shape.

It can display in 12 or 24 hour modes, and displays the date when a mouse
button is held down.  It has two large fonts built into it, but it can
animate most other fonts that contain all of the digits.  It can also do some
funky psychedelic colormap cycling, and can use overlay planes or the `shape'
extension so that the window is transparent.

In the default configuration, this program is very network-intensive.  If it
seems sluggish, try the "-memory high" option (see the man page).  It will
take a little longer to start up, but will then go easy on the bandwidth.

This program was inspired by the Alto/Macintosh programs of the same name,
written by Steve Capps some time in the early eighties.  This implementation
is mine, and if you're looking for another reason to hate the X Windows
``feature'' of network-transparent graphics, well here you go.

Copyright (c) 1991-1999 by Jamie Zawinski <jwz@jwz.org>.  Please let
me know if you find any bugs or make any improvements.

The latest version of xdaliclock can always be found at
http://www.jwz.org/xdaliclock/

==============================================================================

To build for the X Window System:

    cd xdaliclock/X11/
    ./configure
    make
    make install

==============================================================================

To build for PalmOS devices (using gcc as a Unix --> PalmOS cross-compiler):

    cd xdaliclock/palm/
    ./configure --host=m68k-palmos --build=`../config.guess`
    make CFLAGS="-O3"

Then use

    pilot-xfer /dev/ttya -i daliclock.prc

to download it to your device, or whatever your prefered sync method is.

If you don't already have a working PalmOS cross-compilation environment
set up, you'll need to install "m68k-palmos-gcc", "pilrc", "obj-res", and
"build-prc" first.  This can be a lot of work -- you're on your own!
Check http://www.palmos.com/dev/support/ to find the things you need.

==============================================================================


    11-Sep-02 (v2.19)   Added -window-id option, for use by xscreensaver-demo.
                        Handle exposure events in "-root -noseconds" mode.
                        Updated the PalmOS source to work with PalmOS SDK 4.0.
                        Fixed a dumb bug in the PalmOS preferences where you
                        couldn't set the FPS to exactly "10".
                        Updated make.com for VMS.
    29-Nov-99 (v2.18)   Oops, month number was off by one in the Palm version.
    28-Nov-99 (v2.17)   Now uses `configure' instead of `Imake'.
                        ANSI C is now required.
                        Ported to Palm Pilot.
    12-Nov-99 (v2.16)   Fixed some warnings, cleaned up the fonts some more.
    11-Nov-99 (v2.15)   Added two new (smaller) builtin fonts, bringing the
                        total to four.  The smallest one is 27 pixels high,
                        and is cleaner looking than any X font I've seen at
                        that size...
     4-Jan-99 (v2.14)   Fixed a bug that caused X errors on some systems with
                        PseudoColor visuals when using `-transparent'.
                        Some portability fixes for OpenVMS.
                        Added `xdaliclock.spec' so that everyone builds their
                        RPMs in the same way.
     7-Nov-98 (v2.13)   Put a brain-dead simple date parser in `-countdown'.
                        Updated my email address and home page.
     6-Nov-98 (v2.12)   Botched, please ignore.
    31-Dec-97 (v2.11)   Added cheesy `-countdown' switch.  Happy new year...
    25-Jun-97 (v2.10)   Fixed a bad interaction with xscreensaver...
     2-Jun-97 (v2.09)   Renamed -shape to -transparent, and made it use
                        SERVER_OVERLAY_VISUALS if available -- that makes the
                        option actually be *usable* on SGIs and on advanced HP,
                        DEC, or IBM systems (since overlay visuals actually
                        work, unlike the Shape extension.)
    30-Aug-96 (v2.08)   Fixed a lingering bug where sometimes clicking would
                        not cause the date to be displayed.
    23-Feb-96 (v2.07)   Added WM_COMMAND property for session management.
                        More better VMS support from Martin Zinser.
    24-Dec-95 (v2.06)   Made -cycle work on non-PseudoColor visuals.
                        Fixed a couple of shape-related bugs.
     8-Jan-95 (v2.05)   Fixed a silly bug in determining the visual and cmap
                        to use in -root mode, which caused it to not work with
                        xscreensaver when the saver wasn't using the default
                        visual and cmap.
     8-Nov-94 (v2.04)   Some fixes for SGIs and non-default visuals.
    31-May-94 (v2.03)   Minor tweaks to Imakefile; submitted for X11r6 contrib.
    21-May-94 (v2.01)   VMS and R3 support, with help from Richard L. Dyson
                        <dyson@sunfish.physics.uiowa.edu>.
    16-May-94 (v2.00)   Converted to use Xt instead of raw Xlib.
     4-Sep-93 (v1.07)   Fixed shape handling; some minor portability changes.
     3-Feb-93 (v1.06)   Fixed some memory errors.
    10-Jun-92 (v1.05)   More from Tony Kennedy: support visuals with different
                        depths from default, center digits correctly in
                        initial window, and merge geometry defaults in a more
                        sophisticated way.  Merged in a slightly reworked
                        version of more general segment-handling code from
                        Dan Wallach <c169-bg@auriga.berkeley.edu>.  Added a
                        second, even bigger builtin font.  Added the -root
                        and -fullscreen arguments.
     4-Jun-92 (v1.04)   More VMS support for resource database files, from
                        Tony Kennedy <adk@scri.fsu.edu>.
    16-Jan-92 (v1.03)   Added more intelligent visual support.
                        Made it not die on fonts without per-char info.
     3-Jan-92 (v1.02)   Merged in VMS support from Daniel C. Newman
                        <dan@innosoft.com>.
    26-Oct-91 (v1.01)   Fixed Expose handling in -noseconds mode;
                        access() called with too few args;
                        added resource for MM/DD/YY, DD/MM/YY, etc.
     7-Oct-91 (v1.00)   Submitted for X11r5 contrib tape.
