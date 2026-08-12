#!/bin/bash
set -e
cd "$(dirname "$0")"

APPNAME=$(basename "$(pwd)")

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
echo -e "MODULES :\n${MODULES[@]}\n"

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
  APPNAME="${APPNAME}_test"
  DEFINES+=(-DMG_EXPERIMENTAL_INTERFACES)
  DEFINES+=(-DUSE_SERVER_STATS)
  DEFINES+=(-DPLOUCKY_ENABLE_VEDIS_CMD)
fi

CONFORMING="-std=c99 -D_POSIX_C_SOURCE=200809L"
$COMPILER $DEBUG -pedantic $CONFORMING ${DEFINES[@]} -o $APPNAME main.c $MODULES $LIBLINK

if [ "$CASE" == "valgrind" ]; then
  valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$APPNAME "$@" && rm ./$APPNAME
  #--malloc-fill=0xAA --free-fill=0xBB --log-file=valgrind.log  --show-leak-kinds=all --track-origins=yes --num-callers=40 --verbose 
elif [ "$CASE" == "run" ]; then
  ./$APPNAME "$@"
  if [ -f "$APPNAME" ]; then
    rm ./$APPNAME
  fi
fi

