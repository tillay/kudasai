# Maintainer: Tilley
pkgname='kudasai'
_pkgname='kudasai-git'
pkgver=r13.5d2629c
pkgrel=1
epoch=1
pkgdesc="Daemon I made to make your computer moan upon USB drives being inserted or removed."
arch=('any')
url="https://github.com/tillay/kudasai.git"
license=('MIT')
depends=()
makedepends=() 
source=('git+https://github.com/AngelJumbo/lavat.git')
md5sums=('SKIP')

pkgver() {
	cd "$srcdir/${_pkgname}"
	printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
	cd ${_pkgname}
	make
}

package() {
	cd ${_pkgname}
	install -Dm755 ./lavat "$pkgdir/usr/bin/$_pkgname"
}
