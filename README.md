# SMTP Server
MTA SMTP server implementation made for a networking course.

## Build
To build we use make, so you can simply put:
* `make` - to build both server and client in debug mode by default
* `make build_type=[release|debug]` - to build both server and client in preferred mode
* `make [build_type=[release|debug]] [client|server]` - to build either client or server (default build in debug mode)

## Features
* Receive mails from any other SMTP agents;
* Save local mails;
* Send mails to any further SMTP agents.

Commands: HELO, EHLO, MAIL FROM, RCPT TO, DATA, RSET, QUIT.

## Authors
[@Mexican_Man](https://github.com/MexicanMan) - Server part \
[@kolmosckvich](https://github.com/kolmosckvich) - Client part
