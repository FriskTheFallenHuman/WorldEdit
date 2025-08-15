# Maintainer: codereader <greebo[AT]angua[DOT]at>
pkgname=worldedit
pkgver=3.8.0
pkgrel=1
pkgdesc="Level Editor for Doom 3 (idTech4) and similar games"
arch=("x86_64")
url="https://github.com/FriskTheFallenHuman/WorldEdit"
license=("GPL")
depends=(wxgtk2 ftgl glew freealut libvorbis python libsigc++ eigen)
makedepends=(cmake git)
source=("$pkgname::git+https://github.com/FriskTheFallenHuman/WorldEdit#tag=4.0.0")
md5sums=("SKIP")

build() {
	cd "$pkgname"
	cmake .
	make
}

package() {
	cd "$pkgname"
	make DESTDIR="$pkgdir/" install
}
