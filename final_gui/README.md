# Graphical User Interface

## Table of Contents

* [Introduction](#introduction)
* [How It Works](#how-it-works)
* [How to Install](#how-to-install)
* [How to Use](#how-to-use)
* [References Used](#references-used)
* [Previous Repos](#previous-repos)

## Introduction

The Graphical User Interface (GUI) is first written in html / css / javascript. Then the scripts are ran which strip the comments from the pages, minify the pages, and then GZIP encodes the pages. The GZIP encoding results in a binary array that is populated in to out.ino. The binary array is copied into the ESP32 (wifi module) sketch and then uploaded to the device. The GUI uses bootstrap, websocket, and chartjs APIs. Operation of the GUI is discussed on the root directory README.md.

## How It Works

The compression software utilize shell scripts and python scripts. The embed.sh is ran from a unix terminal. We used the windows subsystem for linux (WSL), Ubuntu 18.04. The embed.sh script calls minify.sh and within these 2 shell scripts byteConvert.py and replaceInFile.py are called. The scripts look into the src folder and find all files within each subfolder. Each file is stripped of its comments and minified. The html page index.html is combined with its javascript counterpart, index.js before minification. The results of the minification are placed in the min folder. These minified counterparts are then GZIP encoded and combined with embedded.ino to produce out.ino. Note that the encoded page is placed into its placeholder. For example, style.css is placed into {STYLE.CSS} and index.html is placed into {INDEX.HTML}, etc.

## How to Install

According to the previous year repo, there are some dependencies required. You can refer to the previous year repo ([Repo](https://github.com/PSoysauce/TI-Innovator-Hub-Data-Acquisition-Tool)). However, to add onto their information, you also require python3.8. There is syntax used in the python script that is not python3 compatible. Also, you need nodejs. The nodejs is called node and also contains the npm package. These packages, npm and node, need to be the most recent versions. These links are usefule for installing the dependencies:

- https://www.digitalocean.com/community/tutorials/how-to-install-node-js-on-ubuntu-18-04
- https://phoenixnap.com/kb/update-node-js-version

Once the dependencies are installed you only have to go to the final_gui directory and run embed.sh.

## How to Use

We are refering to the directory in this repository. This was performed in Ubuntu 18.04.

1. All dependencies installed and embed.sh can be ran from unix terminal.
2. Within the src folder, place your html page, there can be multiple pages.
3. Within the html page, e.g. index.html, include its javascript counterpart. Example: "<script src="js/index.js"></script>". This will cause the index.html and index.js pages to be combined.
4. Ensure the javascript counter part is in the src/js folder.
5. Ensure that all dependencies are included in the src/dependencies folder and css files are in the css folder. Mixing languages in one file (e.g. html and javascript) will cause the minify script to not completely work. Note that the html page and javascript files in src/js are combined but the rest of the dependencies are called when the webpage is loaded in the web browser. This is handled by the wifi module.
6. Go to embedded folder and open embedded.ino. Add a {FILENAME.EXTENSION} for each file you have in the src folder. Example, for dependencies we have chart.min.js so we need to have a {CHART.MIN.JS} in our embedded.ino.
7. Once all this is setup, run embed.sh from the final_gui folder. It will populate the min folder and out.ino in embedded/out folder.
8. Copy GZIP array into sketch and use.

# References Used
- [Chart.js](https://nagix.github.io/chartjs-plugin-streaming/)
- [Bootstrap CSS](https://getbootstrap.com/docs/3.4/css/#grid)
- [Websocket with MCU](https://randomnerdtutorials.com/esp8266-nodemcu-websocket-server-arduino/)
- [Javascript W3 Reference](https://www.w3schools.com/js/)
- [HTML W3 Reference](https://www.w3schools.com/html/)
- [Chart.js Reload Data](https://stackoverflow.com/questions/24785713/chart-js-load-totally-new-data)
- [Multiple Websocket](https://stackoverflow.com/questions/12498429/multiple-websockets)
- [Websocket Binary Data](https://blog.mgechev.com/2015/02/06/parsing-binary-protocol-data-javascript-typedarrays-blobs/)

# Previous Repos
- https://github.com/PSoysauce/TI-Innovator-Hub-Data-Acquisition-Tool
- https://github.com/RyanKGeorgeDev/TI-Innovator-Hub-Data-Acquisition-Tool
