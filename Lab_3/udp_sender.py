import socket
import time

# Completati cu adresa IP a platformei ESP32
PEER_IP = "192.168.89.44"
PEER_PORT = 10001
LED_OFF = b"GPIO4=0"
LED_ON = b"GPIO4=1"

# Creeaza un socket UDP
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Functie pentru a trimite starea LED-ului
def send_led_state(state):
    sock.sendto(state, (PEER_IP, PEER_PORT))
    print("Am trimis mesajul: ", state)

try:
    while True:
        # Alterneaza starea LED-ului
        send_led_state(LED_ON)
        time.sleep(1)
        send_led_state(LED_OFF)
        time.sleep(1)
except KeyboardInterrupt:
    sock.close()



#early code
# import socket
# import time

# # Completati cu adresa IP a platformei ESP32
# PEER_IP = "192.168.89.26"
# PEER_PORT = 10001

# MESSAGE = b"Salut!"
# i = 0

# sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# while 1:
#     try:
#         TO_SEND = MESSAGE + bytes(str(i),"ascii")
#         sock.sendto(TO_SEND, (PEER_IP, PEER_PORT))
#         print("Am trimis mesajul: ", TO_SEND)
#         i = i + 1
#         time.sleep(1)
#     except KeyboardInterrupt:
#         break