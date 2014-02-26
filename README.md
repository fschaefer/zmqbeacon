zmqbeacon
=========
Usage speaks more or less for itself:

    zmqbeacon 0.1 - LAN service announcement and discovery

      Usage for LAN service announcement:
      zmqbeacon -a announcement [-i interval]
        -i : beaconing interval in ms (default: 1000)
        -a : announcement to send;
             Instead of -a, the announcement can be set
             via environment variable ZMQBEACON_DATA.

      Usage for LAN service discovery:
      zmqbeacon -l [-t timeout] [-s filter] [-r repeat] [-c command]
        -l : listen for beacons to arrive
        -t : wait for at most ms (default: -1 = forever)
        -s : subscribe to messages; no filter gets everything
        -r : repeat listening to beacons (default: 1)
        -c : execute shell command if a message is received;
             Received data is stored in environment variables
             ZMQBEACON_IP and ZMQBEACON_DATA.

      Additional options for both modes:
      zmqbeacon ... [-p port] [-v]
        -p : UDP port (default: 5670)
        -v : verbose output to stderr

### License ###
    Copyright 2014 Florian Schäfer <florian.schaefer@gmail.com>

    Redistribution and use in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:

       1. Redistributions of source code must retain the above copyright notice, this list of
          conditions and the following disclaimer.

       2. Redistributions in binary form must reproduce the above copyright notice, this list
          of conditions and the following disclaimer in the documentation and/or other materials
          provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
    FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    The views and conclusions contained in the software and documentation are those of the
    authors and should not be interpreted as representing official policies, either expressed
    or implied, of the copyright holders.
