/*
 * onvif-authentication.h
 *
 * Copyright (C) 2014 - Watson Xu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

extern int access_control(struct soap* soap);

#define ACCESS_CONTROL \
do {\
	DBG_LINE;\
	int ret = access_control(soap);\
	if (ret != SOAP_OK) return ret;\
} while(0)