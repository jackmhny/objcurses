#!/bin/bash
set -e

BIN_NAME="objcurses"
BIN_PATH="cmake-build-release/$BIN_NAME"
OUT_DIR="package"
DEB_DIR="$OUT_DIR/${BIN_NAME}-deb"

# check binary exists
if [ ! -f "$BIN_PATH" ]; then
    echo "error: binary not found at $BIN_PATH"
    exit 1
fi

# extract version from --version output
VERSION=$("$BIN_PATH" --version | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
if [ -z "$VERSION" ]; then
    echo "error: could not extract version"
    exit 1
fi

echo "packaging $BIN_NAME version $VERSION..."

# prepare output directory
mkdir -p "$OUT_DIR"

# prepare folder for tar.gz: package/objcurses-<version>-linux/objcurses
TAR_DIR_NAME="${BIN_NAME}-${VERSION}-linux"
TAR_DIR_PATH="$OUT_DIR/$TAR_DIR_NAME"
mkdir -p "$TAR_DIR_PATH"
cp "$BIN_PATH" "$TAR_DIR_PATH/$BIN_NAME"
strip "$TAR_DIR_PATH/$BIN_NAME"

# create tar.gz archive with versioned folder and plain binary inside
tar -czf "$OUT_DIR/${TAR_DIR_NAME}.tar.gz" -C "$OUT_DIR" "$TAR_DIR_NAME"
rm -rf "$TAR_DIR_PATH"

# create .deb directory structure
mkdir -p "$DEB_DIR/usr/bin" "$DEB_DIR/DEBIAN"
cp "$BIN_PATH" "$DEB_DIR/usr/bin/$BIN_NAME"

cat > "$DEB_DIR/DEBIAN/control" <<EOF
Package: $BIN_NAME
Version: $VERSION
Section: utils
Priority: optional
Architecture: amd64
Depends: libc6 (>= 2.27), libncurses6 (>= 6), libtinfo6 (>= 6), libstdc++6, libgcc1
Maintainer: Anton Dmitriev <contact.admtrv@gmail.com>
Description: ncurses 3d object viewer
 It renders .obj models in real time using ASCII characters and a simple rendering pipeline.
EOF

# build .deb package
dpkg-deb --build "$DEB_DIR"
mv "${DEB_DIR}.deb" "$OUT_DIR/${BIN_NAME}-${VERSION}-linux.deb"
rm -rf "$DEB_DIR"

# final output
echo "done, files created:"
echo " - $OUT_DIR/${BIN_NAME}-${VERSION}-linux.tar.gz"
echo " - $OUT_DIR/${BIN_NAME}-${VERSION}-linux.deb"
