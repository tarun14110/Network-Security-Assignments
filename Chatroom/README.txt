# ChatApp
When Client registers, it creates a private key and Certificate signing request (CSR). And sends CSR to CA server. CA signs the certificate for the client and sends its to the client. (Instead od sending CSR and receiving certificate, I am using a common directory for this purpose). Whenever User A wants to communicate to B. It sends a request to server, server than decides common available port for both, and notifies both of them. A will act as a client, and asks for B certificate. B also asks for A's certificate for mutual authentication. Then they both verifies the certificate. If succesfull, connection setups and message transfer using TLS connection.

#### How to run
Dillinger is a cloud-enabled, mobile-ready, offline-storage, AngularJS powered HTML5 Markdown editor.

  - #### start servers
        ./register_server = server for registeration
        ./ca_server = CA server to provide certificates
        ./server <port> = starts chatting server on provided port.
  - #### To register new client (optional)
        sudo ./client <IP of server> <port> 
    Provide IP address of server and port. If server is on localhost enter IP = 127.0.0.1 . port of register server is 5555.
  - #### To login client
        sudo ./client <IP of server> <port> 
    Provide IP address of server and port. If server is on localhost enter IP = 127.0.0.1 .For test purpose, I had already registered 2 clients. a) username=amb password=amb b) username = tarun password= pass.
  - #### To send message from one client to another
        /msg <client username> <message> 

