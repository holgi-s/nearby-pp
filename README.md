# nearby-pp
C++ port of Nearby Connections

Nearby++ is a library implementing the Nearby Connections protocol.
See https://developers.google.com/nearby/connections/overview

It is targeting RaspberryPi3 and Windows.

For device discovery I use the Avahi C++ library.
On Windows I use the external Bonjour service.

Currently only the server side of the protocol is implemented


PI Dependencies:

Update PI:
  apt-get update

Install CMAKE:
  sudo apt-get install cmake

Install Avahi cient header and libraries:
  sudo apt-get install libavahi-core-dev
  sudo apt-get install libavahi-client-dev

Optional (for the led example):
Install pigpiod
  http://abyz.co.uk/rpi/pigpio/download.html

