 Weatherio

**CLI weather application written in C99**

Features:
- 

## Table of Contents

- [Installation](#installation)  
- [Usage](#usage)  
- [Contribution](#contribution)
- [Dev Notes](#dev-notes)  

## Installation

**Make instructions:**
Each target can be individually built from root directory, adding a suffix to the target name.

Targets (project modules):
- server
- client
- client_cpp

Make suffixes:
- /run
- /clean
- /valgrind
- /addlinks
- /rmlinks

For example:
`make server/run`
will build and run only the server target/module.

These general commands can also be run for all targets combined: 
- make (same as make all, builds all targets)
- make clean

## Usage

## Contribution

1. Push only to main branch after tested fully in development branch

2. Code formatting:
    - Tabs = 2 spaces
    - Snake case only
    - Passed arguments begin with underscore
    - Our Structs Are Capitally Named

3. ...

## Dev Notes

