#!/bin/bash
echo "Generating build information using aclocal, autoheader, automake and autoconf"
echo "This may take a while ..."
# Regenerate configuration files.
aclocal
autoheader
automake --include-deps --add-missing --copy
autoconf
echo ""
echo "Now you are ready to run './configure'"
echo "  followed by 'make'."
echo ""
echo "You can also run  ./configure --help for extra features to enable/disable."
echo ""
echo "Typical PC Suggestion:"
echo "./configure CFLAGS=\"-O2\" CXXFLAGS=\"-O2\""
echo ""
echo "Raspberry Pi 4 64-Bit Suggestion:"
echo "./configure CFLAGS=\"-Ofast -march=armv8-a+crc -mtune=cortex-a72\" CXXFLAGS=\"-Ofast -march=armv8-a+crc -mtune=cortex-a72\""
echo ""
echo "Raspberry Pi 4 32-Bit Suggestion:"
echo "./configure CFLAGS=\"-Ofast -march=armv8-a+crc -mtune=cortex-a72 -mfpu=neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations\" CXXFLAGS=\"-Ofast -march=armv8-a+crc -mtune=cortex-a72 -mfpu=neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations\""
echo ""
echo "Raspberry Pi 3 32-Bit Suggestion:"
echo "./configure CFLAGS=\"-Ofast -march=armv8-a+crc -mtune=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations\" CXXFLAGS=\"-Ofast -march=armv8-a+crc -mtune=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations\""
echo ""
echo "Raspberry Pi 2 Suggestion:"
echo "./configure CFLAGS=\"-Ofast -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations\" CXXFLAGS=\"-Ofast -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -ftree-vectorize -funsafe-math-optimizations\""
