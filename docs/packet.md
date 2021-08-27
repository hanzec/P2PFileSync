# Protocol Definition

## Packet

### Message Header

* Format:
    |      Source      |    Destination   |  Payload Size  |  Payload |
    |      :----:      |      :----:      |     :----:     |  :----:  |
    |  IP+Port(6byte)  |  IP+Port(6byte)  |     4 Byte     |   ....   |
    |   Class IPAddr   |   Class IPAddr   |    uint32_t    |   ....   |
* note:
    1. packet size must smaller than 4294967295 byte(uint32 max) which is about 4.29gb
