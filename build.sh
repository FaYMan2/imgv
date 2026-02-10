#!/bin/sh

echo "Building imgv..."

gcc \
    main.c \
    file.c \
    -o imgv \
    `sdl2-config --cflags --libs`

if [ $? -eq 0 ]; then
    echo "Build successful!"
else
    echo "Build failed."
fi
