## AnyLink Secure Client

This is an SSL VPN client that supports OpenConnect or Cisco's AnyConnect [SSL VPN Protocol](https://datatracker.ietf.org/doc/html/draft-mavrogiannopoulos-openconnect-03).  It use [vpnagent](https://github.com/tlslink/vpnagent) as the core to communicate with the server.

Currently the following servers are supported,

- [ocserv](https://gitlab.com/openconnect/ocserv)
- [anylink](https://github.com/bjdgyc/anylink)

## Install

### Ubuntu

tested with Ubuntu 20.04 and Ubuntu 22.04

```bash
tar -zxvf anylink-linux-amd64.tar.gz
./install.sh
```

