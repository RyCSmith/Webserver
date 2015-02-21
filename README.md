# Webserver
Overview: Simple Webserver to handle text/HTML requests.

Design: This project was assigned by Dr. Chris Murphy at the University of Pennsylvania. Fundamental server operations were borrowed from the following 2 sources: http://www.prasannatech.net/2008/07/socket-programming-tutorial.html, http://www.binarii.com/files/papers/c_sockets.txt. While modified at points, these lines generally follow immediately after the numbered comments.

Use: The server is launched from the command line and takes two arguments; the port it would like to be assigned to and the full path of the root directory you would like to host. Ex.: ./httpserver 1126 /Users/some_user/my_folder/my_public_html_folder. The server reads files by word to manage requests and as such is limited to text and HTML file.

-When the address is requested without a filename specified, ex. http://localhost:1126, the server will fetch the default index.html file.

-File not found and corrupted files will result in a 404 response.

-Memory allocation errors will result in a 500 response.

-The server maintains stats regarding its operation. This can be accessed by adding /stats to the webaddress. Ex.: http://localhost:1126/stats. The stats will accumulate for the entire time the server is in operation and records the number of requests successfully handled, the number of failures, and the total bytes sent.

-Favicon requests are acknowledged but ignored.

-To close the server, enter q or Q from the command line. 

Other Info: Makefile included.
