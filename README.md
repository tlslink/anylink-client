## AnyLink Secure Client

This is an SSL VPN client that supports OpenConnect or Cisco's AnyConnect [SSL VPN Protocol](https://datatracker.ietf.org/doc/html/draft-mavrogiannopoulos-openconnect-03).  It use [sslcon](https://github.com/tlslink/sslcon) as the core to communicate with the server.

Currently the following servers are supported,

- [AnyLink](https://github.com/bjdgyc/anylink)
- [OpenConnect VPN server](https://gitlab.com/openconnect/ocserv)

> This software can save passwords(optional) in the Keychain of the operating system, but you should evaluate whether your usage scenarios allow saving passwords and avoid potential security risks.
>
> This project serves as a demonstration of how to use [sslcon](https://github.com/tlslink/sslcon) and will remain open source for Windows and Linux. For other platforms, please refer to the download instructions below.

## Screenshot

<img src="./images/screenshot.png" alt="screenshot" width=70% />

## Download

- Windows and Linux:  [Releases](https://github.com/tlslink/anylink-client/releases) or at https://anylink.pro

## Install

### Windows

Please use Windows 10 or newer.

### Linux

You should use `anylink-linux_openssl1-amd64.tar.gz` with Ubuntu 20.04 or newer.

```bash
tar -zxvf anylink-*.tar.gz
./install.sh
```

The `.run` package is supposed to support other Linux distributions like RedHat, but you should check the `install.sh` file and install some dependencies.

## Help

| You may want to join in the QQ Group(66899589) to get some help. |
| :----------------------------------------------------------: |
|                  ![](./images/qqgroup.png)                   |
