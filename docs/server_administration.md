# VPN Server

Welcome to our VPN Server! This instruction will guide you through the steps to build and run the server using the provided Makefile.



## Preparing your machine

Before running server you **need to configure TUN device**. To do this you need to adjust the parameters in the `config` file. Remember, this action must be repeated **each time after a server restart or when removing configuration via 'unconfigure.sh' script**.

#### Config Parameters:
- `TUN_INTERFACE`: Interface name. Default is `tun0`.
- `TUN_IP`: Your local private network. Default is `10.0.0.1`.
- `TUN_NETMASK`: Your local private network mask. Default is `24`.
- `MAIN_INTERFACE`: Your main interface with Internet access. Default is `enp0s3`.

#### Configuration:

Open a terminal, navigate to the server folder, and execute `configure.sh` with administrative rights to set up the device:
```sh
sudo bash configure.sh
```

To undo all the changes, simply run `unconfigure.sh` with administrative rights:
```sh
sudo bash unconfigure.sh
```



## Build Instructions

1. Ensure you have `g++` installed on your system.

2. Clone this repository to your local machine.

3. Open a terminal and navigate to the directory where you cloned the repository.

4. Run the following command to build the server: 

```sh
make
```



## Running the Server

Once the server is built, you can run it using the following command (you can run with user rights, no need for root):

```sh
./server <port> <num_threads>
```

Example: 
```sh
./server 25565 4
```

Enjoy your VPN server!

