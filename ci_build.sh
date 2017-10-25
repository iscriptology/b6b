#!/bin/sh -xe

# This file is part of b6b.
#
# Copyright 2017 Dima Krasner
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

rm -rf build build-clang build-no-threads build-small

export CFLAGS=-g
meson build
CC=clang meson -Dwith_doc=false build-clang
meson -Dwith_doc=false -Dwith_threads=false build-no-threads
meson -Dwith_doc=false -Dwith_threads=false -Dwith_miniz=false -Dwith_linenoise=false build-small

# build with GCC, clang, with GCC while thread support is disabled and a small build with all optional features off
for i in build build-clang build-no-threads
do
	ninja -C $i
	meson test -C $i --print-errorlogs
	meson test -C $i --print-errorlogs --wrapper "taskset -c 0"

	meson configure -Dbuildtype=release $i
	ninja -C $i
	meson test -C $i --print-errorlogs
	# run the tests 5 times on a single CPU, to increase the chance of finding racy tests
	meson test -C $i --print-errorlogs --repeat 5 --wrapper "taskset -c 0"

	DESTDIR=dest ninja -C $i install

	meson test -C $i --print-errorlogs --num-processes 1 --wrapper "valgrind --leak-check=full --malloc-fill=1 --free-fill=1 --track-fds=yes"
done

ninja -C build-small
DESTDIR=dest ninja -C build-small install
meson test -C build-small --print-errorlogs

# rebuild the small build with a static library
meson configure -Ddefault_library=static build-small
ninja -C build-small
DESTDIR=dest-static ninja -C build-small install

for i in build build-clang
do
	meson test -C $i --print-errorlogs --num-processes 1 --repeat 5 --wrapper "valgrind --tool=helgrind"
	meson test -C $i --print-errorlogs --num-processes 1 --wrapper "valgrind --tool=helgrind --fair-sched=yes"
done
