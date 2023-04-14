## AnyLink Secure Client

This is an SSL VPN client that supports OpenConnect or Cisco's AnyConnect [SSL VPN Protocol](https://datatracker.ietf.org/doc/html/draft-mavrogiannopoulos-openconnect-03).  It use [vpnagent](https://github.com/tlslink/vpnagent) as the core to communicate with the server.

Currently the following servers are supported,

- [ocserv](https://gitlab.com/openconnect/ocserv)
- [anylink](https://github.com/bjdgyc/anylink)

> This software can save passwords in the Keychain of the operating system to avoid plaintext passwords, but you should evaluate whether your usage scenarios allow saving passwords and avoid potential security risks.

## Download

Go to [Releases](https://github.com/tlslink/anylink-client/releases) or at https://anylink.pro

The [Continuous release](https://github.com/tlslink/anylink-client/releases/tag/continuous) may contain quick fixes for some bugs!

## Install

### Windows

Please use Windows 10 or above!

### Linux

You should use `anylink-linux_openssl1-amd64.tar.gz` with Ubuntu 20.04 or above.

For Ubuntu 22.04, you may need to manually install openssl1.1 package , as it only supports openssl3 by default.

> Currently anylink-linux_openssl3-amd64.tar.gz is still in beta phase.

```bash
tar -zxvf anylink-*.tar.gz
./install.sh
```

The `.run` package is supposed to support other Linux distributions like RedHat, but you should check the `install.sh` file and install some dependencies.

## Help

|                     You may want to join in the QQ Group(66899589) to get some help.                     |
| :----------------------------------------------------------: |
| ![](./images/qqgroup.png) |
