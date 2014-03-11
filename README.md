#smudge

## Introduction
smudge is an ncurses-based Multi-User Dungeon (MUD) client scriptable with Scheme.
It exists because I like MUDs, curses interfaces, and Lisp dialects. Since MUDs are text-based, a
curses interface seems appropriate. I was also excited about the possible features I could
implement with the power of Scheme. smudge is based on an older client known as mcl, although
it shares no code whatsoever with it. I liked the general interface of mcl, but it wasn't ideal
for me for several reasons:

* mcl doesn't support terminal resizing; the windows are a fixed size. This is a mess in a tiling window manager.
* mcl doesn't have regex scrollback searching.
* I prefer Scheme over Perl/Python for scripting.
* mcl is old, unmaintained, and won't build easily in modern Linuxes. In fact, I think it dropped off the internet recently.
* mcl doesn't have automapping.
* mcl was written in C++ and I prefer C.

smudge was created to address these issues and just generally be a fun project to me to work on. If someone else finds
it useful in any way, that's just icing on the cake.

## Features
Note that the client is currently in super-alpha stage. It works fine for basic stuff, but there are chunks of
functionality missing (notably auto-mapping). Feel free to mess with it at your leisure.

* Basic MUD server/client interaction (DONE)
* ANSI colors (DONE)
* Terminal resizing support (DONE)
* Scrollback support (DONE; one known issue)
* Telnet command support (MOSTLY DONE)
* Regex searching of the scrollback buffer (MOSTLY DONE)
* Scheme interaction
  * Hooks for user input (aliases, backtracking, speedwalk) (DONE; see example mud.scm)
  * Hooks for MUD server data (triggers, yanking, recoloring, etc.) (NOT STARTED)
  * Customizable settings (NOT STARTED)
* Key bindings
  * Set bindings in code (DONE)
  * Set bindings from Scheme (NOT STARTED)
* Auto-mapping (NOT STARTED)
* Mud Client Compression Protocol (MCCP) support (NOT STARTED)
* Mud Extension Protocol (MXP) support (NOT STARTED)
