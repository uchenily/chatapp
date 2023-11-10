compile:
	meson compile -C build

setup:
	meson setup --reconfigure build --native-file meson-native.txt --wrap-mode nodownload

install:
	meson install -C build

ci:
	pre-commit run -a
