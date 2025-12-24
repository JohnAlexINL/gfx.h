We need a lightweight library that does all the following;

- Framebuffer graphics with primitives
    - Triangles
    - Squares
    - Circles
    - Precompiled character glyphs

- Support for 2D and 3D Translations
    - Rotation (X, Y, Z)
    - Translation
    - Skew

Things the library does *not* do;
- *Deciding* what does or does not get drawn,
- Load models, backface cull, et cetera

and it externally needs;

- A library that can handle Window creation and giving a buffer/texture to write,
- Wrapper functions passed back to the lib which let it signal the buffer is ready for display