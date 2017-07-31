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

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <b6b.h>

static void setup(const char *buf, const ssize_t len)
{
	int fd;

	fd = open("file", O_WRONLY | O_CREAT | O_TRUNC, 0600);
	assert(fd >= 0);
	assert(write(fd, buf, (size_t)len) == len);
	close(fd);
}

static void teardown(const char *buf, const ssize_t len)
{
	static char tmp[128];
	int fd;

	fd = open("file", O_RDONLY);
	assert(fd >= 0);
	assert(read(fd, tmp, sizeof(tmp)) == len);
	close(fd);
	tmp[len] = '\0';
	assert(strcmp(tmp, buf) == 0);
}

int main()
{
	struct b6b_interp interp;

	assert((unlink("file") == 0) || (errno == ENOENT));
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file r] read}", 21) == B6B_ERR);
	b6b_interp_destroy(&interp);

	setup("", 0);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file r] read}", 21) == B6B_OK);
	assert(b6b_as_str(interp.fg->_));
	assert(!interp.fg->_->slen);
	b6b_interp_destroy(&interp);
	teardown("", 0);

	setup("abcdefgh", 8);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file r] read -1}", 24) == B6B_ERR);
	b6b_interp_destroy(&interp);
	teardown("abcdefgh", 8);

	setup("abcdefgh", 8);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file r] read 3}", 23) == B6B_OK);
	assert(b6b_as_str(interp.fg->_));
	assert(strcmp(interp.fg->_->s, "abc") == 0);
	b6b_interp_destroy(&interp);
	teardown("abcdefgh", 8);

	setup("abcdefgh", 8);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file r] read 30}", 24) == B6B_OK);
	assert(b6b_as_str(interp.fg->_));
	assert(strcmp(interp.fg->_->s, "abcdefgh") == 0);
	b6b_interp_destroy(&interp);
	teardown("abcdefgh", 8);

	setup("abcdefgh", 8);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file r] read}", 21) == B6B_OK);
	assert(b6b_as_str(interp.fg->_));
	assert(strcmp(interp.fg->_->s, "abcdefgh") == 0);
	b6b_interp_destroy(&interp);
	teardown("abcdefgh", 8);

	setup("abcdefgh", 8);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file] read}", 19) == B6B_OK);
	assert(b6b_as_str(interp.fg->_));
	assert(strcmp(interp.fg->_->s, "abcdefgh") == 0);
	b6b_interp_destroy(&interp);
	teardown("abcdefgh", 8);

	setup("abcdefgh", 8);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file] write a}", 22) == B6B_ERR);
	b6b_interp_destroy(&interp);
	teardown("abcdefgh", 8);

	setup("abcd", 4);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file w] write efgh}", 27) == B6B_OK);
	assert(b6b_as_float(interp.fg->_));
	assert(interp.fg->_->f == 4);
	b6b_interp_destroy(&interp);
	teardown("efgh", 4);

	setup("abcd", 4);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp,
	                     "{[$open file r+] write efgh}",
	                     28) == B6B_OK);
	assert(b6b_as_float(interp.fg->_));
	assert(interp.fg->_->f == 4);
	b6b_interp_destroy(&interp);
	teardown("efgh", 4);

	setup("abcd", 4);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file a] write efgh}", 27) == B6B_OK);
	assert(b6b_as_float(interp.fg->_));
	assert(interp.fg->_->f == 4);
	b6b_interp_destroy(&interp);
	teardown("abcdefgh", 8);

	setup("abcd", 4);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file a] read}", 21) == B6B_ERR);
	b6b_interp_destroy(&interp);
	teardown("abcd", 4);

	setup("abcd", 4);
	assert(b6b_interp_new_argv(&interp, 0, NULL, B6B_OPT_TRACE));
	assert(b6b_call_copy(&interp, "{[$open file w] read}", 21) == B6B_ERR);
	b6b_interp_destroy(&interp);
	teardown("", 0);

	assert(unlink("file") == 0);

	return EXIT_SUCCESS;
}
