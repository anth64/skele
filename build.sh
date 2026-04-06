#!/bin/sh

OS=$(uname -s | tr '[:upper:]' '[:lower:]')

if [ "$OS" = "freebsd" ]; then
	MK_FILE="bmake.mk"
else
	MK_FILE="gmake.mk"
fi

if grep -q "Raspberry Pi 5" /proc/device-tree/model 2>/dev/null; then
	GL_MAJOR=3
	GL_MINOR=1
else
	GL_MAJOR=3
	GL_MINOR=3
fi

HAS_INSTALL=0
HAS_UNINSTALL=0
for arg in "$@"; do
	case "$arg" in
		install)   HAS_INSTALL=1 ;;
		uninstall) HAS_UNINSTALL=1 ;;
	esac
done

if [ "$HAS_INSTALL" = "1" ] || [ "$HAS_UNINSTALL" = "1" ]; then
	if [ "$HAS_INSTALL" = "1" ]; then
		make -f "$MK_FILE" release GL_MAJOR=$GL_MAJOR GL_MINOR=$GL_MINOR
	fi

	if [ "$(id -u)" = "0" ]; then
		PRIV=""
	elif command -v doas > /dev/null 2>&1; then
		PRIV="doas"
	elif command -v sudo > /dev/null 2>&1; then
		PRIV="sudo"
	else
		echo "error: install requires root. neither doas nor sudo found." >&2
		exit 1
	fi

	$PRIV make -f "$MK_FILE" "$@" GL_MAJOR=$GL_MAJOR GL_MINOR=$GL_MINOR
elif [ "$1" = "run" ]; then
	make -f "$MK_FILE" run GL_MAJOR=$GL_MAJOR GL_MINOR=$GL_MINOR
elif [ "$1" = "run_gl" ]; then
	make -f "$MK_FILE" run_gl GL_MAJOR=$GL_MAJOR GL_MINOR=$GL_MINOR
else
	make -f "$MK_FILE" "$@" GL_MAJOR=$GL_MAJOR GL_MINOR=$GL_MINOR
fi
