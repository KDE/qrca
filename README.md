# Qrca

Qrca is a simple application for [Plasma Desktop](https://kde.org/plasma-desktop/) and [Plasma Mobile](https://plasma-mobile.org/) that lets you scan many barcode formats and create your own QR code images.

## Get it

Qrca is currently available on Linux as a nightly Flatpak:

```bash
flatpak install --user https://cdn.kde.org/flatpak/qrca-nightly/org.kde.qrca.flatpakref
```

Qrca is also available as a nightly for Android on F-Droid with the [KDE F-Droid Nightly Build Repository](https://community.kde.org/Android/F-Droid#KDE_F-Droid_Nightly_Build_Repository).

## Build it

The most well supported way to build Qrca is with [kde-builder](https://kde-builder.kde.org).

Follow the tutorial to [get started with kde-builder](https://develop.kde.org/docs/getting-started/building/kde-builder-setup/), then run:

```bash
# To build
kde-builder qrca
# To run
kde-builder --run qrca
```

To build the project manually and install it locally, run:

```bash
# To build
cmake -B build/
cmake --build build/ --parallel
cmake --install build/ --prefix ~/.local
# To run
qrca
```

See [Building KDE software manually](https://develop.kde.org/docs/getting-started/building/cmake-build/) and [Installing build dependencies](https://develop.kde.org/docs/getting-started/building/help-dependencies/) if you run into any issues during compilation.
