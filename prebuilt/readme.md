# otf2clm

Convert an OTF font to `clm` data file.

Usage:

```shell
fontforge -lang=py -script otf2clm \
    <path/to/OTFFont> \
    <is_math_font: true | false> \
    <path/to/save/clm_file>
```
