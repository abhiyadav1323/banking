## Banking System using Client-Server socket programming

A simple client-server application for banking system which can be used for checking balance, mini statement, crediting and debiting amount.

## Installing / Getting started

* Clone the repository by typing the following command in the shell.
```shell
git clone https://github.com/abhiyadav1323/banking.git
```
* Compile the server and client source files by following commands.
```shell
gcc client.c client_customer.c client_police.c client_admin.c -o client
gcc server.c server_customer.c server_police.c server_admin.c -o server
```
* First start the server. To start the server you need to pass the port nummber as the argument. You can choose any number between 2000 and 65535. If this port is already in use, just use another port and you are good to go. The following command starts the server:
```shell
./server <port_number>  # ./server 6345
```
* Now start the client program by typing the following command:
```shell
./client <server_ip_address> <port_number>  # ./client 127.0.0.1 6345
```

## Features

This application supports 3 types of users with different requirements:
#### Bank_Customer
The customer can see AVAILABLE BALANCE in his/her account and MINI STATEMENT of his/her account.
#### Bank_Admin
The admin can CREDIT/DEBIT the certain amount of money from any Bank_Customer ACCOUNT (as we do it in a SBI single window counter). The admin also updates the respective “Customer_Account_file” by appending the new information. 
#### Police
The police can see the available balance of all customers. He is allowed to view any Customers MINI STATEMENT by quoting the Customer_ID (i.e. User_ID with user_type as ‘C’).

## How to use?

#### Customer
```shell
Enter the credentials.
Username: 101
Password: prpcpf
Welcome Bank_Customer.
Do u want to continue (y/n): y
Operation to perform: balance
BALANCE: 3886.687012

Do u want to continue (y/n): y
Operation to perform: mini_statement
MINI STATEMENT:
02-12-2016 credit 5000
02-12-2016 debit 4000
10-03-2017 credit 4055.500000
10-03-2017 debit 4010.000000
10-03-2017 credit 4020.000000
10-03-2017 debit 3886.687012

Do u want to continue (y/n): n
```

#### Bank Admin
```shell
Enter the credentials.
Username: 111
Password: snavpa
Welcome Bank_Admin.
Do u want to continue (y/n): y
User ID of Customer: 101
Transaction Type: credit
Amount: 50
Transaction successful.

Do u want to continue (y/n): y
User ID of Customer: 102
Transaction Type: debit
Amount: 10
Transaction successful.

Do u want to continue (y/n): n
```

#### Police
```shell
Enter the credentials.
Username: 115
Password: xvdxbd
Welcome Police.
Do u want to continue (y/n): y
Operation to perform: balance
User ID of Customer: 101
BALANCE: 3936.687012

Do u want to continue (y/n): y
Operation to perform: mini_statement
User ID of Customer: 103
MINI STATEMENT:
02-12-2016 credit 5000
14-03-2017 credit 5545.454102
14-03-2017 credit 5645.579102

Do u want to continue (y/n): n
```

## Licensing

The MIT License (MIT)

Copyright (c) 2017 Abhishek Yadav

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
