<h3 align="center">ploucky</h3>
<p align="center">fullstack webapp with C backend</p>

usage: ./exec.sh -run||-valgrind||-build

everything is included in the binary build located in the _\_build_ folder.

folder structure:
- \_build: contains the build and run(dev).
- modules: everything related to C.
- web: everything related to html, js, css (bundled with webpack) output: modules/_generated/web.h.
- exec.sh: the script to use.
- main.c: the C entry.

#### prerequisite

- **nodejs** required to build the _web.h_ which contains the UI bundle and a list of HTTP headers.
- **gcc** to _./exec.sh -run_ in development (debug) mode.
- **valgrind** to _./exec.sh -valgring_ in development mode for memory checking.
- **musl-gcc** to _./exec.sh -build_ and generate the final binary.

#### execution

you can run multiple instances of the same build. each instance automatically selects the next available port between 8000 and 65535 for HTTP.
- each instance stores its database folder at _$HOME/.ploucky/\<port>/_.

#### _libs included_

- vedis (database).
- civetweb _mongoose_ (http).
- linenoise (cli).
- cjson.
