The Fira Math Font
==================

Fira Math is a sans-serif font with Unicode math support. This font is a fork of

- [FiraSans](https://github.com/bBoxType/FiraSans)
- [FiraGO](https://github.com/bBoxType/FiraGO)

Usage
-----

Fira Math can be used via XeLaTeX or LuaLaTeX, with [`unicode-math`](https://ctan.org/pkg/unicode-math) package.

    % Compiled with XeLaTeX or LuaLaTeX
    \documentclass{article}
    \usepackage{amsmath}
    \usepackage[mathrm=sym]{unicode-math}
    \setmathfont{Fira Math}

    \begin{document}
    \[
      \int_0^{\mathrm{\pi}} \sin x \, \mathrm{d}x = 2
    \]
    \end{document}

You may try the [`firamath-otf`](https://ctan.org/pkg/firamath-otf) package as well.

Contributing
------------

[Issues](https://github.com/firamath/firamath/issues) and
[pull requests](https://github.com/firamath/firamath/pulls)
are always welcome.

License
-------

This Font Software is licensed under the [SIL Open Font License](http://scripts.sil.org/OFL), Version 1.1.

-----

Copyright (C) 2018&ndash;2020 by Xiangdong Zeng <xdzeng96@gmail.com>.
