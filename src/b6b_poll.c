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

#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <b6b.h>

static enum b6b_res b6b_poll_proc(struct b6b_interp *interp,
                                  struct b6b_obj *args)
{
	struct epoll_event ev = {0}, *evs;
	struct b6b_obj *fds[3], *p, *op, *n, *l, *fd;
	int i, j = 0, out, err, r, w, e;

	if (!b6b_proc_get_args(interp, args, "o s n", &p, &op, &n) ||
	    (n->n > INT_MAX))
		return B6B_ERR;

	if (strcmp(op->s, "add") == 0) {
		if (n->n < 0)
			return B6B_ERR;

		ev.events = EPOLLIN | EPOLLOUT;
		ev.data.fd = (int)n->n;

		if ((epoll_ctl((int)(intptr_t)p->priv,
		               EPOLL_CTL_ADD,
		               ev.data.fd,
		               &ev) < 0) &&
		    (errno != EEXIST))
			return b6b_return_strerror(interp, errno);

		return B6B_OK;
	}
	else if (strcmp(op->s, "remove") == 0) {
		if (n->n < 0)
			return B6B_ERR;

		if ((epoll_ctl((int)(intptr_t)p->priv,
		               EPOLL_CTL_DEL,
		               (int)n->n,
		               NULL) < 0) &&
		    (errno != ENOENT))
			return b6b_return_strerror(interp, errno);

		return B6B_OK;
	}
	else if (strcmp(op->s, "wait") == 0) {
		if (n->n <= 0)
			return B6B_ERR;

		l = b6b_list_new();
		if (b6b_unlikely(!l))
			return B6B_ERR;

		for (i = 0; i < 3; ++i) {
			fds[i] = b6b_list_new();
			if (b6b_unlikely(!fds[i])) {
				for (i -= 1; i >= 0; --i)
					b6b_destroy(fds[i]);

				b6b_destroy(l);
				return B6B_ERR;
			}

			if (b6b_unlikely(!b6b_list_add(l, fds[i]))) {
				for (; i >= 0; --i)
					b6b_destroy(fds[i]);

				b6b_destroy(l);
				return B6B_ERR;
			}

			/* l still holds a reference */
			b6b_unref(fds[i]);
		}

		evs = (struct epoll_event *)malloc(sizeof(struct epoll_event) * n->n);
		if (!evs) {
			b6b_destroy(l);
			return B6B_ERR;
		}

		out = epoll_wait((int)(intptr_t)p->priv, evs, (int)n->n, -1);
		if (out < 0) {
			err = errno;
			free(evs);
			b6b_destroy(l);
			return b6b_return_strerror(interp, err);
		}

		for (i = 0; (j < out) && (i < n->n); ++i) {
			if (!evs[i].events)
				continue;

			r = w = e = 0;

			if (evs[i].events & EPOLLIN)
				r = 1;

			if (evs[i].events & EPOLLOUT)
				w = 1;

			if (evs[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
				e = 1;

			if (r || w || e) {
				fd = b6b_num_new((b6b_num)evs[i].data.fd);
				if (b6b_unlikely(!fd))
					goto err;

				if ((r && b6b_unlikely(!b6b_list_add(fds[0], fd))) ||
				    (w && b6b_unlikely(!b6b_list_add(fds[1], fd))) ||
				    (e && b6b_unlikely(!b6b_list_add(fds[2], fd)))) {
					b6b_destroy(fd);
					goto err;
				}

				b6b_unref(fd);
				++j;
			}

			continue;

err:
			free(evs);
			b6b_destroy(l);
			return B6B_ERR;
		}

		free(evs);
		return b6b_return(interp, l);
	}

	return B6B_ERR;
}

static enum b6b_res b6b_poll_proc_poll(struct b6b_interp *interp,
                                       struct b6b_obj *args)
{
	struct b6b_obj *o;
	int fd;

	if (!b6b_proc_get_args(interp, args, "o", NULL))
		return B6B_ERR;

	fd = epoll_create1(0);
	if (fd < 0)
		return b6b_return_strerror(interp, errno);

	o = b6b_str_fmt("poll:%d", fd);
	if (b6b_unlikely(!o)) {
		close(fd);
		return B6B_ERR;
	}

	o->proc = b6b_poll_proc;
	o->priv = (void *)(intptr_t)fd;

	return b6b_return(interp, o);
}

static const struct b6b_ext_obj b6b_poll[] = {
	{
		.name = "poll",
		.type = B6B_OBJ_STR,
		.val.s = "poll",
		.proc = b6b_poll_proc_poll
	}
};
__b6b_ext(b6b_poll);
