from websocket import create_connection

import os


print(os.environ)

ws = create_connection("ws://172.16.9.49:3000")
print( "Sending 'Hello, World'...")
ws.send("Hello, World")
print( "Sent")
print( "Receiving...")
result =  ws.recv()
print( "Received '%s'" % result)
ws.close()