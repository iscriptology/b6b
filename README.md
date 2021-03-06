```
 _      __   _
| |__  / /_ | |__
| '_ \| '_ \| '_ \
| |_) | (_) | |_) |
|_.__/ \___/|_.__/
```

[![Build Status](https://travis-ci.org/dimkr/b6b.svg?branch=master)](https://travis-ci.org/dimkr/b6b) [![codecov](https://codecov.io/gh/dimkr/b6b/branch/master/graph/badge.svg)](https://codecov.io/gh/dimkr/b6b) [![Coverity](https://scan.coverity.com/projects/13321/badge.svg)](https://scan.coverity.com/projects/dimkr-b6b) [![Documentation Status](https://readthedocs.org/projects/b6b/badge/?version=latest)](http://b6b.readthedocs.io/en/latest/?badge=latest)

## Overview

b6b is a simple, dynamic, procedural and reflective scripting language inspired by [Tcl](http://www.tcl.tk/), [Python](http://www.python.org/), Lisp and shell scripting.

b6b wraps operating system mechanisms and interfaces such as sockets, files, shell commands and even signals with *a unified interface: the stream*. I/O events generated by streams of any kind can be managed through a **single** event loop. This way, b6b enables rapid development of lightweight, asynchronous and scalable, event-driven applications.

b6b is **easy** and safe to embed into other projects: it provides a simple API which allows the creation of multiple, *independent* interpreter instances. In addition, b6b's garbage collection is **deterministic and predictable**, which contributes to easy debugging of memory issues and high reliability in use cases such as long-running daemons. Moreover, b6b uses only *one* OS thread: b6b threads are implemented on top of userspace fibers. Therefore, b6b is safe for inclusion in existing, complex, multi-threaded applications.

b6b is named so in admiration of the *excellent* [Vincent Bach](http://www.bachbrass.com/) 6B trumpet mouthpiece, which produces compact, yet lively and expressive tone with a **strong, solid core**, that makes it a good choice for many playing situations. The name was chosen with hope that these qualities will also be attributed to the language and its implementation.

## Building

b6b is built using [Meson](http://mesonbuild.com/).

```bash
meson build
ninja -C build install
```

## Credits and Legal Information

b6b is free and unencumbered software released under the terms of the [Apache License Version 2.0](https://www.apache.org/licenses/LICENSE-2.0); see COPYING for the license text. For a list of its authors and contributors, see AUTHORS.

The ASCII art logo at the top was made using [FIGlet](http://www.figlet.org/).
