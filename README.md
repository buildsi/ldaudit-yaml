# LD_AUDIT Yaml

![https://avatars.githubusercontent.com/u/85255731?s=96&v=4](https://avatars.githubusercontent.com/u/85255731?s=96&v=4)

This is a small library to make it easy to run [LD_AUDIT](https://man7.org/linux/man-pages/man7/rtld-audit.7.html) to
get a "trace" of execution in terms of libraries sniffed and loaded. Why?
I want to eventually be able to trace everything that gets loaded at a particular call
for a container, so I wanted to test this out. After [build](#build) There are a few use cases:

 - [Terminal Output](#terminal-output): prints YAML to the terminal for easy inspection
 - [File Output](#file-output): the same YAML, but to an output file defined by an environment variable.
 - [Container](#docker): Build or run a container that provides the same functionality.

## Build

```bash
$ make
```

## Test

### Terminal Output

```bash
$ make run
```

or more directly:

```bash
$ LD_AUDIT=./auditlib.so whoami
```

Since the shared library cannot have a destructor (I think it requires a main to
be called) I opted to print YAML output instead of json, since we cannot easily
mark the end and close a yaml data structure.

```bash
LD_AUDIT=./auditlib.so whoami
auditlib:
  la_version: 1
  audits:
  - event: handshake
    function: la_version
    value: 1
  - event: object_loaded
    name: ""
    function: la_objopen
    identifier: 0x7f2bc9bd6610
    flag: LM_ID_BASE
    description: Link map is part of the initial namespace
  - event: object_loaded
    name: "/lib64/ld-linux-x86-64.so.2"
    function: la_objopen
    identifier: 0x7f2bc9bd5e68
    flag: LM_ID_BASE
    description: Link map is part of the initial namespace
  - event: activity_occurring
    function: la_activity
    initiated_by: 0x7f2bc9bd6610
    flag: LA_ACT_ADD
    description: New objects are being added to the link map.
  - event: searching_for
    function: la_objsearch
    name: "libc.so.6"
    initiated_by: 0x7f2bc9bd6610
    flag: "LA_SER_ORIG"
  - event: searching_for
    function: la_objsearch
    name: "/lib/x86_64-linux-gnu/libc.so.6"
    initiated_by: 0x7f2bc9bd6610
    flag: "LA_SER_CONFIG"
  - event: object_loaded
    name: "/lib/x86_64-linux-gnu/libc.so.6"
    function: la_objopen
    identifier: 0x7f2bc96425b0
    flag: LM_ID_BASE
    description: Link map is part of the initial namespace
  - event: activity_occurring
    function: la_activity
    initiated_by: 0x7f2bc9bd6610
    flag: LA_ACT_CONSISTENT
    description: Link-map activity has been completed (map is consistent)
  - event: searching_for
    function: la_objsearch
    name: "libnss_compat.so.2"
    initiated_by: 0x7f2bc96425b0
    flag: "LA_SER_ORIG"
  - event: searching_for
    function: la_objsearch
    name: "/lib/x86_64-linux-gnu/libnss_compat.so.2"
    initiated_by: 0x7f2bc96425b0
    flag: "LA_SER_CONFIG"
  - event: activity_occurring
    function: la_activity
    initiated_by: 0x7f2bc9bd6610
    flag: LA_ACT_ADD
    description: New objects are being added to the link map.
  - event: object_loaded
    name: "/lib/x86_64-linux-gnu/libnss_compat.so.2"
    function: la_objopen
    identifier: 0x55e72fa570e0
    flag: LM_ID_BASE
    description: Link map is part of the initial namespace
  - event: activity_occurring
    function: la_activity
    initiated_by: 0x7f2bc9bd6610
    flag: LA_ACT_CONSISTENT
    description: Link-map activity has been completed (map is consistent)
  - event: searching_for
    function: la_objsearch
    name: "libnss_nis.so.2"
    initiated_by: 0x7f2bc96425b0
    flag: "LA_SER_ORIG"
  - event: searching_for
    function: la_objsearch
    name: "/lib/x86_64-linux-gnu/libnss_nis.so.2"
    initiated_by: 0x7f2bc96425b0
    flag: "LA_SER_CONFIG"
  - event: activity_occurring
    function: la_activity
    initiated_by: 0x7f2bc9bd6610
    flag: LA_ACT_ADD
    description: New objects are being added to the link map.
  - event: object_loaded
    name: "/lib/x86_64-linux-gnu/libnss_nis.so.2"
    function: la_objopen
    identifier: 0x55e72fa586c0
    flag: LM_ID_BASE
    description: Link map is part of the initial namespace
  - event: searching_for
    function: la_objsearch
    name: "libnsl.so.1"
    initiated_by: 0x55e72fa586c0
    flag: "LA_SER_ORIG"
  - event: searching_for
    function: la_objsearch
    name: "/lib/x86_64-linux-gnu/libnsl.so.1"
    initiated_by: 0x55e72fa586c0
    flag: "LA_SER_CONFIG"
  - event: object_loaded
    name: "/lib/x86_64-linux-gnu/libnsl.so.1"
    function: la_objopen
    identifier: 0x55e72fa58b90
    flag: LM_ID_BASE
    description: Link map is part of the initial namespace
  - event: searching_for
    function: la_objsearch
    name: "libnss_files.so.2"
    initiated_by: 0x55e72fa586c0
    flag: "LA_SER_ORIG"
  - event: searching_for
    function: la_objsearch
    name: "/lib/x86_64-linux-gnu/libnss_files.so.2"
    initiated_by: 0x55e72fa586c0
    flag: "LA_SER_CONFIG"
  - event: object_loaded
    name: "/lib/x86_64-linux-gnu/libnss_files.so.2"
    function: la_objopen
    identifier: 0x55e72fa59060
    flag: LM_ID_BASE
    description: Link map is part of the initial namespace
  - event: activity_occurring
    function: la_activity
    initiated_by: 0x7f2bc9bd6610
    flag: LA_ACT_CONSISTENT
    description: Link-map activity has been completed (map is consistent)
vanessasaur
```

### File Output

Since printing yaml to the terminal isn't always ideal, we can prepare an output file instead.

```bash
$ touch ldaudit.yaml
$ export LDAUDIT_OUTFILE=ldaudit.yaml
$ LD_AUDIT=./auditlib.so whoami
```

And then you won't see terminal output, but it will be in [ldaudit.yaml](ldaudit.yaml).

### Docker

To build a container to handle the build:

```bash
$ docker build -t auditlib .
```

And then run the same!

```bash
$ docker run -it  auditlib whoam
```

You can use this as a base container, and then have your application export `LDAUDIT_OUTFILE`
before running anything to get the contents to file, or more interactively:

```bash
$ docker run --env LDAUDIT_OUTFILE=/data/test.yaml -v $PWD/:/data -it auditlib whoami
root
$ cat test.yaml 
```

### License

This project is part of Spack. Spack is distributed under the terms of both the MIT license and the Apache License (Version 2.0). Users may choose either license, at their option.

All new contributions must be made under both the MIT and Apache-2.0 licenses.

See LICENSE-MIT, LICENSE-APACHE, COPYRIGHT, and NOTICE for details.

SPDX-License-Identifier: (Apache-2.0 OR MIT)

LLNL-CODE-811652
