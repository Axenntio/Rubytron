# Rubytron

![Rubytron Desktop](media/interface.png)

## :warning: This project is currently work in progress, so do not expect anything working / usefull for now

## Inspiration

Rubytron is based on the desktop experience of (unreleased) [Picotron](https://www.lexaloffle.com/picotron.php) (Screenshots can be found on the [zep's Twitter](https://twitter.com/lexaloffle)).  
Unlike Picotron, Rubytron use Ruby as scripting language for games/programs.

## Capabilities

Display: 192x128 / 4bits (16 colors)  
Code: mruby 3.1 (2.x ruby)

## Build

```console
$ cp rubytron.rb mruby/build_config/. && cd mruby && MRUBY_CONFIG=rubytron rake && cd ..
[...]
$ meson setup build
[...]
$ ninja -C build
```

## Run

```console
./build/src/rubytron
```
