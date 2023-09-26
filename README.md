# Excella
Excella is a fast and lightweight GPU-enabled database application for keeping track of my games collection. It is designed only with my needs in mind, but you may find some use for it as a starting point for your own applications.

## Features
![](/screenshot/screenshot.png)
- Fuzzy searching entries by name
- Extensive filtering options
- Conditional entry highlighting
- Sorting columns by value
- Adding notes to entries

## Building
``` shell
git clone https://github.com/sodamouse/excella.git && cd excella
premake gmake
cd build
make config=release
```

# TODO
1. Disable spawning multiple instances
3. Overhaul statistics page with new features and graphs
