MenuMaker
=========

MenuMaker is an interpreter and generator for menu structures. The interpreter is written in C++ and targets the Arduino platform but omits the hardware interface. 

The generator is a Perl script that is fed by a JSON file describing the menu structure. It generates a MenuMaker_menu.h file to be included into Your build (or Sketch). It optimizes RAM and ROM usage by creating const char* pointers for unique strings easily allowing the use of PROGMEM and other Harward-Barrier braking techniques.

The menu structure is based on ID references; menus contain multiple entries. If an entry ID matches a menu ID, the entry becomes a sub-menu. Navigation history and entry highlighting is handled by the interpreter. Entries with IDs that don't match any other menu are either actions or input fields. Actions result in a call to the virtual onEntry(int id) method, input fields allow the manipulation of lists and numbers. Lists can be checklists offering single or multiple choice, numbers can be in- or decreased.

Input is fed via a few methods: onUp, onDown, onLeft, onRight, onSelect, onBack. If you skip the number input fields you only need to call the D-Pad methods, because onLeft is always onBack and onRight is onSelect for all other entries.

This design allows low CPU and RAM footprint as well as easy adoption to any lower output (serial console, TFT, speech) and input (again serial, hardware buttons, USB HID devices) layer.

