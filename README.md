# dolphin-seafile-plugin
A plugin for dolphin to show the seafile synchronization status

## Features
A simple VCS plugin for KDEs [Dolphin](https://www.kde.org/applications/system/dolphin/) file manager, that displays sync status for your [Seafile](https://www.seafile.com/en/home/) repositories. Simply install and enable under "Services". Currently, the plugin can:

- Display sync status of seafile repositories
	- Per File/Folder thats inside a seafile folder
	- Including special states, e.g. "ignored"

More features, such as sharing, etc. are planned.

### Limitations/Workaround
Due to the way those dolphin plugins work, only repositories that are inside your seafile main directory will work. Additionally, you will have to open this folder once, before the plugin recognizes them.

The reason is, that the plugin reacts on the presence of the `.seafile-data` folder. If you have custom locations outside of the folder, or don't want to always go there first, simply create an empty file called `.seafile-data` in whichever directory you need the plugin to work.
One useful location would be your home directory, as thats the default directory dolphin opens.

## Dependencies
You'll need Dolphin (and thus Qt and KDE frameworks) and Seafile. The plugin only needs the seafile daemon, so make sure you have that one with whatever seafile package you use.

## Installation
For now, only an AUR package is provided: [`dolphin-seafile-plugin`](https://aur.archlinux.org/packages/dolphin-seafile-plugin)

Until then, you can simply build it yourself. Install all the stated dependencies and build via:
```
cd plugin
qmake
make
make install
```
