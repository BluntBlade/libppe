# libppe
A portable programming environment containing APIs of sorts.

## Overview
Subject to the lack of a layer of abstractive, consistent and portable APIs, it's hard to write programs correctly and easily without considering the different features and capabilities prodvied by mainstream operating systems. That is why I try to create this library, which contains a set of easy-to-use types and APIs coded in C. Based on it anybody can make some useful light-weight libraries to build bigger programs or applications. Meanwhile, in the course of programming, lots of knowledge of the computer science can be written down as notes and kept in the repository.

## Layout

```
    ./
     | -- inc               # Header files.
     | -- notes             # Notes which contain information, thoughts and technique decisions.
     | -- samples           # Samples demonstrating how to use this library.
     | -- src               # Source files.
     |     | -- darwin      # Source files for Mac OS X.
     |     | -- unix        # Source files for UNIX.
     |     | -- linux       # Source files for Linux.
     |     \ -- windows     # Source files for Windows.
     \ -- tests             # Unit tests.
```
