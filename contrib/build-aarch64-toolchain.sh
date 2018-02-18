export TARGET=aarch64-none-elf
export PREFIX=$HOME/gcc-aarch64-none-elf
export JN="-j4"

export BINVER=2.29.1
export GCCVER=7.3.0

if [ -e "$PREFIX" ]; then
    echo "Installation directory already exists!"
    exit
fi

rm -rf build-*/
rm -rf binutils-*/
rm -rf gcc-*/

# Get archives
[ ! -f binutils-$BINVER.tar.gz ] && wget https://ftp.gnu.org/gnu/binutils/binutils-$BINVER.tar.gz
[ ! -f gcc-$GCCVER.tar.gz ] && wget https://ftp.gnu.org/gnu/gcc/gcc-$GCCVER/gcc-$GCCVER.tar.gz

# Extract archives
tar xf binutils-$BINVER.tar.gz
tar xf gcc-$GCCVER.tar.gz

(
    # Build binutils
    mkdir build-binutils
    cd build-binutils
    ../binutils-$BINVER/configure --target=$TARGET --prefix=$PREFIX
    echo "MAKEINFO = :" >> Makefile
    make $JN all
    make install
)

(
    # Build GCC
    mkdir build-gcc
    cd build-gcc
    ../gcc-$GCCVER/configure --target=$TARGET --prefix=$PREFIX --without-headers --with-newlib  --with-gnu-as --with-gnu-ld --enable-languages=c,c++
    make $JN all-gcc
    make install-gcc

    # Build libgcc.a
    make $JN all-target-libgcc CFLAGS_FOR_TARGET="-g -O2"
    make install-target-libgcc
)
