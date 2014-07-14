/*
 * onvif-authentication.c
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

#ifdef WITH_OPENSSL
#include "plugin/wsseapi.h"
#endif
#include "soapH.h"

int onvif_access_control(struct soap* soap)
{
#ifdef WITH_OPENSSL
	const char *username = soap_wsse_get_Username(soap);
	const char *password;

	if (!username)
		return soap->error; // no username: return FailedAuthentication

	password = "123456"; // lookup password of username
	if (soap_wsse_verify_Password(soap, password))
		return soap->error; // password verification failed: return FailedAuthentic
#endif

	return SOAP_OK;
}
