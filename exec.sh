#!/bin/bash
set -e
cd "$(dirname "$0")"

APPNAME=$(basename "$(pwd)")
BUILDFOLDER="_build"
if [ ! -d "./${BUILDFOLDER}" ]; then
    mkdir "./${BUILDFOLDER}"
fi

CASE=""
COMPILER="gcc"
LIBLINK="-lpthread -ldl -lm"
DEBUG="-g -O0"


while [[ $# -gt 0 ]]; do
    case $1 in
        -valgrind|--valgrind)
            CASE="valgrind"
            shift
            break
            ;;
        -run|--run)
            CASE="run"
            shift
            break
            ;;
        -build|--build)
            CASE="build"
            COMPILER="musl-gcc"
            LIBLINK="${LIBLINK} -static"
            DEBUG=""
            shift
            break
            ;;
        *)
            CASE="run"
            shift
            break
            ;;
    esac
done

if ! command -v npm &> /dev/null; then
  echo "TODO Install nodejs (web bundle)."
  exit -1
fi
if [ "$CASE" == "build" ]; then
  cd web && npm run build && cd ..
else
  cd web && npm run build-keepconsole && cd ..
fi

for file in $(find ./modules -type f); do
  if [ ! -r "$file" ]; then
    echo "ERROR : '$file' (not readable)."
    exit 1
  fi
done

MODULES=$(find ./modules -type f -name "*.c")
if [ -z "$MODULES" ]; then
  echo "ERROR: no modules found."
  exit 1
fi
echo -e "MODULES:\n${MODULES[@]}\n"
# h files printed as info only
HFILES=$(find ./modules -type f -name "*.h")
if [ -z "$HFILES" ]; then
  echo "ERROR: no h files found."
  exit 1
fi
echo -e "H FILES:\n${HFILES[@]}\n"


DEFINES=(
    -DNO_SSL
    -DNO_CGI
    -DNO_CACHING
    -DNO_FILES
    -DNO_FILESYSTEM
    -DNO_USER_GROUP
    -DNO_THREAD_NAME
    -DUSE_WEBSOCKET
    -DUSE_IPV6
)

if [ "$CASE" == "build" ]; then
  APPNAME="${APPNAME}_build"
else
  APPNAME="${APPNAME}_run"
  DEFINES+=(-DMG_EXPERIMENTAL_INTERFACES)
  DEFINES+=(-DUSE_SERVER_STATS)
  DEFINES+=(-DPLOUCKY_ENABLE_VEDIS_CMD)
fi

CONFORMING="-std=c99 -D_POSIX_C_SOURCE=200809L -Wno-overlength-strings"
$COMPILER $DEBUG -pedantic $CONFORMING ${DEFINES[@]} -o "./${BUILDFOLDER}/${APPNAME}" main.c $MODULES $LIBLINK

if [ "$CASE" == "valgrind" ]; then
  valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes "./${BUILDFOLDER}/${APPNAME}" "$@"
  #--malloc-fill=0xAA --free-fill=0xBB --log-file=valgrind.log  --show-leak-kinds=all --track-origins=yes --num-callers=40 --verbose 
elif [ "$CASE" == "run" ]; then
  "./${BUILDFOLDER}/${APPNAME}" "$@"
fi

