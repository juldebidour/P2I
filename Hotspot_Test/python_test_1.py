
from http.server import BaseHTTPRequestHandler, HTTPServer
import network

# Partie serveur

class MyRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/':
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(b'Welcome to the ESP Station !')
        elif self.path == '/page2':
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(b'Enjoy the data !')
        else:
            self.send_error(404, 'Not Found')

def run(server_class=HTTPServer, handler_class=MyRequestHandler, port=80):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f'Starting server on port {port}')
    httpd.serve_forever()

if __name__ == '__main__':
    run()

# Partie Wifi ESP 

ap = network.WLAN(network.AP_IF)
ap.active(True)
ap.config(essid='FaryLink_817496')
