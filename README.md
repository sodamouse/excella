# Excella
Excella is a fast and lightweight GPU-enabled database application for keeping track of my games collection. It is designed only with my needs in mind, but you may find some use for it as a starting point for your own applications.

## Features
![](/screenshot/screenshot.png)
- Conditional entry highlighting
- Filtering entries by name
- Sorting columns by value
- Adding notes to entries

## Building
``` shell
git clone https://github.com/sodamouse/excella.git && cd excella
premake gmake
cd build
make config=release
```

## Usage
- Create $USER/.config/excella/excella.conf and specify a path where Excella should store its default database file.

## Design philosophy
Excella has been made with a performance-aware mindset. It is largely data-oriented, lacks templates and inheritance, and other OOP garbage. It currently uses std::string and nlohmann::json (which *is* OOP) with plans to completely get rid of them in the future.

# TODO
1. Disable spawning multiple instances
2. Change default sort to `sortingTitle`
3. Overhaul statistics page with new features and graphs
4. Replace JSON with a custom, performant data serialization strategy
