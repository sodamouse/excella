# Excella
Excella is a fast and lightweight GPU-enabled database application for keeping track of my games collection. It is designed only with my needs in mind, but you may find some use for it as a starting point for your own applications.

## Features
- Fast and light weight
- Extensive searching, filtering, and sorting options
- Adding notes to entries
- URL manager

## Building on Windows
``` bash
git clone https://git.sr.ht/~sodamouse/excella && cd excella
premake5 vs2022
cd build
vcvarsall x64
MSBuild /property:Configuration=Release
```
