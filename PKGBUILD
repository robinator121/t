# Maintainer: Robi
pkgname=t
pkgver=0.0.1
pkgrel=1
pkgdesc="t — multi-language template generator and quick-compiler"
arch=('x86_64')
url="https://github.com/robinator121/t"
license=('GPL3')
depends=('gcc' 'java-runtime' 'python' 'r' 'ldc')
makedepends=('git')
source=("git+https://github.com/robinator121/t.git")
sha256sums=('SKIP') # skip for git sources

build() {
    cd "$srcdir/t" || return 1

    # compile bin/main.cpp into build/t
    if [ ! -f bin/main.cpp ]; then
        echo "ERROR: bin/main.cpp not found in source ($srcdir/t/bin/main.cpp)"
        return 1
        fi

        mkdir -p build
        echo "Compiling bin/main.cpp -> build/t"
        g++ -std=c++17 bin/main.cpp -o build/t
}

package() {
    cd "$srcdir/t" || return 1

    # install binary
    install -Dm755 build/t "$pkgdir/usr/bin/t"

    # install docs/license if present
    if [ -f README.md ]; then
        install -Dm644 README.md "$pkgdir/usr/share/doc/$pkgname/README.md"
        fi
        if [ -f LICENSE ]; then
            install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
            elif [ -f COPYING ]; then
            install -Dm644 COPYING "$pkgdir/usr/share/licenses/$pkgname/COPYING"
            else
                mkdir -p "$pkgdir/usr/share/licenses/$pkgname"
                printf "Licensed as declared in PKGBUILD (GPL3)\n" > "$pkgdir/usr/share/licenses/$pkgname/README"
                fi

                # copy templates folder into /usr/share/t/templates if present
                if [ -d templates ]; then
                    mkdir -p "$pkgdir/usr/share/t/templates"
                    cp -a templates/* "$pkgdir/usr/share/t/templates/" || true
                    fi

                    # optional: copy learn resources if present
                    if [ -d learn ]; then
                        mkdir -p "$pkgdir/usr/share/t/learn"
                        cp -a learn/* "$pkgdir/usr/share/t/learn/" || true
                        fi
}
