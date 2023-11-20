# PCOM-HTTP-Client-REST

Dinu Dan

For the implementation of this project, I used the HTTP laboratory's code skeleton.
For parsing JSON, I used the parson.h header.
For generating GET, POST, DELETE type messages, I used the functions from the HTTP laboratory.

The implementation consists of reading the client's commands from the keyboard and interacting with the server based on each command.
Thus, we check the input received:

In the case of the 'exit' command: we close the connection with the server and exit the program.

In the case of the 'register' command:

we read the username and password from the keyboard
we create a JSON packet with the received data and pass it as a parameter to the function that generates the POST type message
This will generate the message with an IP:PORT, a route, and a JSON body
We open a connection with the server and send the message
We parse the response received from the server and display an appropriate message
In the case of the 'login' command:

We follow similar steps to 'register'
We parse the response received from the server and display an appropriate message
From the response, we retain the session cookie for the client
In the case of the 'enter_library' command:

We check if the client has the cookie set (is logged in)
We follow similar steps to 'register/login', adding the cookie in the request message
We parse the response received from the server and display an appropriate message
From the response, we retain the JWT token for the client
In the case of the 'get_books' command:

We check if the client has the token set (has access to the library)
We follow similar steps to 'register/login', adding the token in the request message
In parsing the response received, we extract the details of the books and display them, using functions from the parson.h header
We iterate through the array of books and display the ID and TITLE for each book
In the case of the 'add_book' command:

We check if the client has the token set (has access to the library)
We read the details about the book the client wants to add
We format the data for a JSON packet and send it as a parameter to the function generating the POST type message, adding the token
We check the response received from the server and display an appropriate message
In the case of the 'get_book' command:

We check if the client has the token set (has access to the library)
We read the ID of the book the client wants to display
We generate a route for the GET type message and send it as a parameter to the function generating the GET type message, adding the token
We parse the response received from the server and display the book's details, using functions from the parson.h header
In the case of the 'delete_book' command:

We check if the client has the token set (has access to the library)
We read the ID of the book the client wants to delete
We generate a route for the DELETE type message and send it as a parameter to the function generating the DELETE type message, adding the token
We check the response received from the server and display an appropriate message
In the case of the 'logout' command:

We check if the client has the cookie set (is logged in)
We follow similar steps to 'register/login', adding the cookie in the request message
We parse the response received from the server and display an appropriate message
If successful, we set the login and library access flag to 0
