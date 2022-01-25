# bento-time

#### Project submodules structure:

- (root) [Current game project](https://github.com/efreyu/bento-time)
- (src/common) [Common modules for all games](https://github.com/gloomyzen/cocos2d-common)
- (cocos2d) [Modified core](https://github.com/gloomyzen/cocos2d-x)
- (lib/imgui) [ImGui for cocos2d v4](https://github.com/gloomyzen/cocos2d-x-imgui)

#### Install:
1. Install cocos2d v4
2. Clone repo with all submodules
```bash
git clone --recurse-submodules git@github.com:gloomyzen/bento-time.git bento-time 
cd bento-time
```

#### Build from sources:
```bash
cd bento-time
cmake .. -DDEBUG=1
make -j$(nproc)
```

#### Generate project file (VS, Xcode, Android studio) :

For XCode - mac project
```bash
cd bento-time
cmake -S . -B build-win32 -GXcode -DDEBUG=1
```

For XCode - ios project
```bash
cd bento-time
cmake -S . -B build-ios -GXcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphoneos
```

For Visual Studio Community 2019
```bash
cd bento-time
cmake -S . -B build-win32 -G"Visual Studio 16 2019" -Tv142 -A Win32 -DDEBUG=1
and open .sln file from `build-win32` folder
```
