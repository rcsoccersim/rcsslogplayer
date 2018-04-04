# Introduction

The RoboCup Soccer Simulator LogPlayer (**rcsslogplayer**) is a game log
replay tool for the RoboCup Soccer Simulator Server (**rcssserver**).


# Quick Start

From the **rcsslogplayer** directory execute:

	./configure
	make

This will built the necessary binaries to get you up and running. `rcsslogplayer/qt4/rcsslogplayer` and `rcsslogplayer/classic/rcsslogplayer_classic` are the binary for the log player. To be able to run, the binaries need to find shared libraries which created when you build rcsslogplayer. This means you must either intall the log player (make install) or run it from `rcsslogplayer/qt4` or `rcsslogplayer/classic`.

A sample game log converter command can be found at `rcsslogplayer/src/rcg3to4`, `rcsslogplayer/src/rcg4to3`,  `rcsslogplayer/src/rcg2xml` and `rcsslogplayer/src/rcgsplit`.

To see a game log with the **classic log player**, you will need to start
a **simulator monitor**, which needs to be installed separately (**rcssmonitor or rcssmonitor_classic**).


# Configuring

Before you can build **The RoboCup Soccer Simulator LogPlayer** you will need to run the **configure** script located in the root of the distribution directory.

The default configuration will set up to install the log player components in the following location:

	/usr/local/bin  #for the executables

You may need administrator privileges to install the **log player** into the default location.  This locations can be modified by using configure's `--prefix=DIR` and related options.  See `configure --help` for more details.

The **log player** has several features that can be enabled or disabled at configure time by using the `--enable-FEATURE[=ARG]` or `--disable-FEATURE` parameters to `configure` .
`--disable-FEATURE` is equivalent to `--enable-FEATURE=no` and `--enable-FEATURE` is equivalent to `--enable-FEATURE=yes`.  The only valid values for `ARG` are `yes` and `no`.


# Building

Once you have successfully configured the **rcsslogplayer**, simply run `make` to build the sources.

# Installing

When you have completed building the **rcsslogplayer**, it's components can be installed into their default locations or the locations specified during configuring by running `make install`.  Depending on where you are installing the **rcsslogplayer**, you may need special permissions.

# Uninstalling

The monitor can also be easily removed by entering the distribution directory and running `make uninstall`.  This will remove all the files that where installed, but not any directories that were created during the installation process.

# Using the Log Player

To start the log player either type:

`./rcsslogplayer <GameLogFile>`

From the directory containing the executable or:

`rcsslogplayer <GameLogFile>`

To start the classic log player either type:

`./rcsslogplay_classic <GameLogFile>`

From the directory containing the executable or:

`rcsslogplay_classic <GameLogFile>`

## Run  Rcsslogplayer w/ Docker Container (Ubuntu 12.04)

First, create a network to connect all containers (We use this version of Ubuntu, because its default for simulation 2d competition):

```
docker network create mynet
```
  
Inside this **GitHub** repository, build the **Dockerfile** in terminal:

	docker build . -t "rcsslogplayer"

You need to make the Display exportable   
 
	xhost +local:root

Run the **rcsslogplayer** client:

	docker run -it --rm -e DISPLAY=${DISPLAY} -e QT_X11_NO_MITSHM=1 -v /tmp/.X11-unix:/tmp/.X11-unix  --net mynet --name rcsslogplayer rcsslogplayer

Inside **docker image**, type to run client:
	
	rcsslogplayer

You need to run the [server docker](https://github.com/rcsoccersim/rcssserver) first, and after [rcssmonitor](https://github.com/heitorrapela/rcssmonitor) and **create each team container** to run separately.

To connect to the server docker, you will need to put **server** in connect to ip.

# Making Contact

For bug reports, feature requests and latest updates, please go to:

http://sourceforge.net/projects/sserver/

Or email bugs to:

sserver-bugs@lists.sourceforge.net

The RoboCup Soccer Server Maintainance Group:

sserver-admin@lists.sourceforge.net
