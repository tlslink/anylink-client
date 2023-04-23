## AnyLink Secure Client

This is an SSL VPN client that supports OpenConnect or Cisco's AnyConnect [SSL VPN Protocol](https://datatracker.ietf.org/doc/html/draft-mavrogiannopoulos-openconnect-03).  It use [vpnagent](https://github.com/tlslink/vpnagent) as the core to communicate with the server.

Currently the following servers are supported,

- [ocserv](https://gitlab.com/openconnect/ocserv)
- [anylink](https://github.com/bjdgyc/anylink)

> This software can save passwords in the Keychain of the operating system to avoid plaintext passwords, but you should evaluate whether your usage scenarios allow saving passwords and avoid potential security risks.

## Screenshot

<img src="./assets/screenshot.png" alt="screenshot" width=70% />

## Download

Go to [Releases](https://github.com/tlslink/anylink-client/releases) or at https://anylink.pro

The [Continuous release](https://github.com/tlslink/anylink-client/releases/tag/continuous) may contain quick fixes for some bugs!

## Security

本软件开源，同任何其它开源软件一样，可能会被第三方修改并重新打包发布，建议采取如下措施避免安全隐患：

- 自行下载 [项目源代码](https://github.com/tlslink/anylink-client) 编译使用
- 在 [官网](https://anylink.pro) 而**不**是第三方网站（包括 CSDN、各种网盘、群共享）等渠道下载本软件的预编译版本
- 服务端开启OTP（一次性密码验证）功能

另外您可能还需要采取如下措施：

- 在企业应用中不使用包括本软件在内的任何开源客户端，因为它们都存在被第三方滥用的风险
- 在企业应用中不使用破解版、绿色版等盗版软件，因为它们都有可能被第三方植入木马病毒而造成财产损失

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
| ![](./assets/qqgroup.png) |
