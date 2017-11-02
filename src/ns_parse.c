/*
 * Copyright (c) 2004 by Internet Systems Consortium, Inc. ("ISC")
 * Copyright (c) 1996,1999 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* Import. */

#include <sys/types.h>
#define __OPTIMIZE__ 1
#include <libc-symbols.h>
#include <arpa/nameser.h>

#include <glibc-errno.h>
#include <glibc-stdio.h>
#include <glibc-string.h>
#include <ucresolv_log.h>

/* Forward. */
 
static const char hex_digits[16] = "0123456789abcdef";

static void	setsection(ns_msg *msg, ns_sect sect);

/* Macros. */

#define RETERR(err) do { __set_errno (err); return (-1); } while (0)

/* Public. */

/*
 * Skip over a compressed domain name. Return the size or -1.
 */
int
dn_skipname(const u_char *ptr, const u_char *eom) {
	const u_char *saveptr = ptr;

	ucresolv_info ("UCLIBC dn_skipname\n");
	if (ns_name_skip(&ptr, eom) == -1)
		return (-1);
	return (ptr - saveptr);
}
libresolv_hidden_def (dn_skipname)

/*
 * Expand compressed domain name 'comp_dn' to full domain name.
 * 'msg' is a pointer to the beginning of the message,
 * 'eomorig' points to the first location after the message,
 * 'exp_dn' is a pointer to a buffer of size 'length' for the result.
 * Return size of compressed name or -1 if there was an error.
 */
int
dn_expand(const u_char *msg, const u_char *eom, const u_char *src,
	  char *dst, int dstsiz)
{
	int n = ns_name_uncompress(msg, eom, src, dst, (size_t)dstsiz);

	if (n > 0 && dst[0] == '.')
		dst[0] = '\0';
	return (n);
}
libresolv_hidden_def (dn_expand)

int
__ns_skiprr(const u_char *ptr, const u_char *eom, ns_sect section, int count) {
	const u_char *optr = ptr;

	ucresolv_info ("UCLIBC ns_skiprr %d\n", count);
	for ((void)NULL; count > 0; count--) {
		int b, rdlength;

		b = dn_skipname(ptr, eom);
		if (b < 0)
			RETERR(EMSGSIZE);
		ptr += b/*Name*/ + NS_INT16SZ/*Type*/ + NS_INT16SZ/*Class*/;
		if (section != ns_s_qd) {
			if (ptr + NS_INT32SZ + NS_INT16SZ > eom)
				RETERR(EMSGSIZE);
			ptr += NS_INT32SZ/*TTL*/;
			NS_GET16(rdlength, ptr);
			ptr += rdlength/*RData*/;
		}
	}
	if (ptr > eom)
		RETERR(EMSGSIZE);
	return (ptr - optr);
}
libresolv_hidden_def (__ns_skiprr)

int
ns_skiprr(const u_char *ptr, const u_char *eom, ns_sect section, int count) {
  return __ns_skiprr (ptr, eom, section, count);
}
libresolv_hidden_def (ns_skiprr)


void show_parse_buf (const u_char *msg, int msglen)
{  
  int i, o, c;
  const char prefix[] = "UCLIBC ns_initparse: ";
#define BUFLEN 72
	char buf[BUFLEN+1];
#define ENDBUF() \
  buf[o] = 0; \
  ucresolv_info ("%s\n", buf); \
  o = 0;
#define BUFPUT(c) \
  if (o >= BUFLEN) { \
    ENDBUF() \
  } \
  buf[o++] = c;

  o = 0;
  for (i=0; (c=prefix[i]) != 0; i++) {
    BUFPUT(c)
  }
  for (i=0; i<msglen; i++) {
    c = msg[i];
    if ((c>=' ') && (c<=0x7F)) {
      BUFPUT (c)
    } else {
      BUFPUT (hex_digits[c>>4])
      BUFPUT (hex_digits[c&15])
    }
  }
  ENDBUF()
#undef BUFPUT
#undef ENDBUF
#undef BUFLEN
}

int
__ns_initparse(const u_char *msg, int msglen, ns_msg *handle) {
	const u_char *eom = msg + msglen;
	int i;

  show_parse_buf (msg, msglen);
	memset(handle, 0x5e, sizeof *handle);
	handle->_msg = msg;
	handle->_eom = eom;
	if (msg + NS_INT16SZ > eom)
		RETERR(EMSGSIZE);
	NS_GET16(handle->_id, msg);
	if (msg + NS_INT16SZ > eom)
		RETERR(EMSGSIZE);
	NS_GET16(handle->_flags, msg);
	ucresolv_info ("UCLIBC ns_initparse get counts\n");
	for (i = 0; i < ns_s_max; i++) {
		if (msg + NS_INT16SZ > eom)
			RETERR(EMSGSIZE);
		NS_GET16(handle->_counts[i], msg);
	}
	for (i = 0; i < ns_s_max; i++)
		if (handle->_counts[i] == 0)
			handle->_sections[i] = NULL;
		else {
			int b = __ns_skiprr(msg, eom, (ns_sect)i,
					  handle->_counts[i]);

			if (b < 0)
				return (-1);
			handle->_sections[i] = msg;
			msg += b;
		}
	if (msg != eom)
		RETERR(EMSGSIZE);
	setsection(handle, ns_s_max);
	return (0);
}
libresolv_hidden_def (__ns_initparse)

int
ns_initparse(const u_char *msg, int msglen, ns_msg *handle) {
  return __ns_initparse (msg, msglen, handle);
}
libresolv_hidden_def (ns_initparse)

int
__ns_parserr(ns_msg *handle, ns_sect section, int rrnum, ns_rr *rr) {
	int b;
	int tmp;

	/* Make section right. */
	tmp = section;
	if (tmp < 0 || section >= ns_s_max)
		RETERR(ENODEV);
	if (section != handle->_sect)
		setsection(handle, section);

	/* Make rrnum right. */
	if (rrnum == -1)
		rrnum = handle->_rrnum;
	if (rrnum < 0 || rrnum >= handle->_counts[(int)section])
		RETERR(ENODEV);
	if (rrnum < handle->_rrnum)
		setsection(handle, section);
	if (rrnum > handle->_rrnum) {
		b = __ns_skiprr(handle->_msg_ptr, handle->_eom, section,
			      rrnum - handle->_rrnum);

		if (b < 0)
			return (-1);
		handle->_msg_ptr += b;
		handle->_rrnum = rrnum;
	}

	/* Do the parse. */
	b = dn_expand(handle->_msg, handle->_eom,
		      handle->_msg_ptr, rr->name, NS_MAXDNAME);
	if (b < 0)
		return (-1);
	handle->_msg_ptr += b;
	if (handle->_msg_ptr + NS_INT16SZ + NS_INT16SZ > handle->_eom)
		RETERR(EMSGSIZE);
	NS_GET16(rr->type, handle->_msg_ptr);
	NS_GET16(rr->rr_class, handle->_msg_ptr);
	if (section == ns_s_qd) {
		rr->ttl = 0;
		rr->rdlength = 0;
		rr->rdata = NULL;
	} else {
		if (handle->_msg_ptr + NS_INT32SZ + NS_INT16SZ > handle->_eom)
			RETERR(EMSGSIZE);
		NS_GET32(rr->ttl, handle->_msg_ptr);
		NS_GET16(rr->rdlength, handle->_msg_ptr);
		if (handle->_msg_ptr + rr->rdlength > handle->_eom)
			RETERR(EMSGSIZE);
		rr->rdata = handle->_msg_ptr;
		handle->_msg_ptr += rr->rdlength;
	}
	if (++handle->_rrnum > handle->_counts[(int)section])
		setsection(handle, (ns_sect)((int)section + 1));

	/* All done. */
	return (0);
}
libresolv_hidden_def (__ns_parserr)

int
ns_parserr(ns_msg *handle, ns_sect section, int rrnum, ns_rr *rr) {
  return __ns_parserr (handle, section, rrnum, rr);
}
libresolv_hidden_def (ns_parserr)

/* Private. */

static void
setsection(ns_msg *msg, ns_sect sect) {
  ucresolv_info ("setsection\n");
	msg->_sect = sect;
	if (sect == ns_s_max) {
		msg->_rrnum = -1;
		msg->_msg_ptr = NULL;
	} else {
		msg->_rrnum = 0;
		msg->_msg_ptr = msg->_sections[(int)sect];
	}
}

/*! \file */
