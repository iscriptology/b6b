/*
 * This file is part of b6b.
 *
 * Copyright 2017 Dima Krasner
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>

struct b6b_stdio_strm {
	FILE *fp;
	struct b6b_interp *interp;
	void *buf;
	int fd;
};

void b6b_stdio_do_fclose(void *arg);

ssize_t b6b_stdio_peeksz(struct b6b_interp *interp, void *priv);
ssize_t b6b_stdio_read(struct b6b_interp *interp,
                       void *priv,
                       unsigned char *buf,
                       const size_t len,
                       int *eof,
                       int *again);
ssize_t b6b_stdio_write(struct b6b_interp *interp,
                        void *priv,
                        const unsigned char *buf,
                        const size_t len);
int b6b_stdio_fd(void *priv);
void b6b_stdio_close(void *priv);
