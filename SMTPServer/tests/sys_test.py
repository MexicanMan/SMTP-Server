#!/usr/bin/python3

import sys
import signal
import smtplib

from_addr = "V@arasaka.com"
to_addrs = "breathtaking_johny@arasaka.com kolmosckvich@yandex.ru".split()

msg = ("From: %s\r\nTo: %s\r\n" % (from_addr, ", ".join(to_addrs)))
msg += "Let's f*ck Arasaka!\r\n"

server = smtplib.SMTP('127.0.0.1', 2525)
server.set_debuglevel(1)
for i in range(1):
    server.sendmail(from_addr, to_addrs, msg)
server.quit()

print("All OK!")