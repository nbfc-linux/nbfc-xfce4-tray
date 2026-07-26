# nbfc-xfce4-tray

**nbfc-xfce4-tray** is a simple Gtk-based XFCE4 Panel Plugin for [nbfc-linux](https://github.com/nbfc-linux/nbfc-linux).

It is written in C.

Installation
------------

### Packages

| Operating System      | Download                       | Command                                        |
|-----------------------|--------------------------------|-------------------------------------------------
| Arch Linux            | [0.1.0](https://github.com/nbfc-linux/nbfc-xfce4-tray/releases/download/0.1.0/arch-linux-nbfc-xfce4-tray-git-0.1.0-1-x86_64.pkg.tar.zst)       | `sudo pacman -U ./arch-linux-nbfc-xfce4-tray-git-0.1.0-1-x86_64.pkg.tar.zst`         |
| Debian Trixie         | [0.1.0](https://github.com/nbfc-linux/nbfc-xfce4-tray/releases/download/0.1.0/debian-trixie-nbfc-xfce4-tray_0.1.0_amd64.deb)   | `sudo apt install ./debian-trixie-nbfc-xfce4-tray_0.1.0_amd64.deb`   |
| Debian Bookworm       | [0.1.0](https://github.com/nbfc-linux/nbfc-xfce4-tray/releases/download/0.1.0/debian-bookworm-nbfc-xfce4-tray_0.1.0_amd64.deb) | `sudo apt install ./debian-bookworm-nbfc-xfce4-tray_0.1.0_amd64.deb` |
| Ubuntu Noble          | [0.1.0](https://github.com/nbfc-linux/nbfc-xfce4-tray/releases/download/0.1.0/ubuntu-noble-nbfc-xfce4-tray_0.1.0_amd64.deb)    | `sudo apt install ./ubuntu-noble-nbfc-xfce4-tray_0.1.0_amd64.deb`    |
| Ubuntu Jammy          | [0.1.0](https://github.com/nbfc-linux/nbfc-xfce4-tray/releases/download/0.1.0/ubuntu-jammy-nbfc-xfce4-tray_0.1.0_amd64.deb)    | `sudo apt install ./ubuntu-jammy-nbfc-xfce4-tray_0.1.0_amd64.deb`    |
| Linux Mint 22         | [0.1.0](https://github.com/nbfc-linux/nbfc-xfce4-tray/releases/download/0.1.0/linux-mint-22-nbfc-xfce4-tray_0.1.0_amd64.deb)   | `sudo apt install ./linux-mint-22-nbfc-xfce4-tray_0.1.0_amd64.deb`   |
| Linux Mint 21         | [0.1.0](https://github.com/nbfc-linux/nbfc-xfce4-tray/releases/download/0.1.0/linux-mint-21-nbfc-xfce4-tray_0.1.0_amd64.deb)   | `sudo apt install ./linux-mint-21-nbfc-xfce4-tray_0.1.0_amd64.deb`   |
| Fedora 44             | [0.1.0](https://github.com/nbfc-linux/nbfc-xfce4-tray/releases/download/0.1.0/fedora-44-nbfc-xfce4-tray-0.1.0-1.x86_64.rpm)       | `sudo dnf install ./fedora-44-nbfc-xfce4-tray-0.1.0-1.x86_64.rpm`       |
| Fedora 43             | [0.1.0](https://github.com/nbfc-linux/nbfc-xfce4-tray/releases/download/0.1.0/fedora-43-nbfc-xfce4-tray-0.1.0-1.x86_64.rpm)       | `sudo dnf install ./fedora-43-nbfc-xfce4-tray-0.1.0-1.x86_64.rpm`       |
| OpenSuse (Tumbleweed) | [0.1.0](https://github.com/nbfc-linux/nbfc-xfce4-tray/releases/download/0.1.0/opensuse-nbfc-xfce4-tray-0.1.0-1.x86_64.rpm)        | `sudo zypper install ./opensuse-nbfc-xfce4-tray-0.1.0-1.x86_64.rpm`     |

Usage
-----

After installing the plugin, the panel has to be restarted:

```sh
xfce4-panel -r
```

The plugin can be added by right-clicking the panel then clicking `Panel -> Add new items`
and selecting the `NBFC` plugin.

