# lroverlay.exe

[![Downloads](https://img.shields.io/github/downloads/retorillo/lroverlay/total)](https://img.shields.io/github/downloads/retorillo/lroverlay/total)
[![Windows](https://svgshare.com/i/ZhY.svg)](https://svgshare.com/i/ZhY.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Donate with Bitcoin](https://en.cryptobadges.io/badge/micro/bc1qqtzjl35enhe0l7a3fuhtkxjenn9sx797v2z64t)](https://en.cryptobadges.io/donate/bc1qqtzjl35enhe0l7a3fuhtkxjenn9sx797v2z64t)

lroverlay.exe simply shows L and R text overlay on your screen and can help you to modeling 2D/3D character.

By default, L shows topright and R shows topleft.
To changed this behaviour, edit batchfile by using the following options.

```
lroverlay --margin 5 --size 5 --p top -o 50 --screen 1 --invert 0 -ltext R -rtext L
```

- `--margin` or `-m` : Margin for overlay placement. This value is percentage of your screen. (range: 0 - 30, default: 5)
- `--size` or `-s` : Text (eg. L, R) size. This value is percentage of your screen that minused margin. (range: 1 - 20, default: 5)
- `--place` or `-p` : Text vertical placement. Supported value is the following: top, middle, bottom (default: top)
- `--screen` or `-S` : Screen ID. For example, specify `2` to use secondary monitor. (default: 1)
- `--left` or `-l` : Text that shows on the left (default: R)
- `--right` or `-r` : Text that shows on the right (default: L)
- `--opacity` or `-o` : Text opacity (10 - 100, default: 50)
- `--invert` or `-i` : Invert text color, specify `1` to invert color. (default: 0)

## License

Licensed under the MIT License

Copyright (C) 2021 Retorillo
