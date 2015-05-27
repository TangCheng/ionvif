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
#include "ionvif.h"

static void
calc_digest(struct soap *soap, const char *created, const char *nonce, int noncelen, const char *password, char hash[SOAP_SMD_SHA1_SIZE])
{
	struct soap_smd_data context;
	/* use smdevp engine */
	soap_smd_init(soap, &context, SOAP_SMD_DGST_SHA1, NULL, 0);
	soap_smd_update(soap, &context, nonce, noncelen);
	soap_smd_update(soap, &context, created, strlen(created));
	soap_smd_update(soap, &context, password, strlen(password));
	soap_smd_final(soap, &context, hash, NULL);
}

const char*
soap_get_username(struct soap *soap)
{
	_wsse__UsernameToken *token = soap_wsse_UsernameToken(soap, NULL);
	DBGFUN("soap_wsse_get_Username");
	if (token)
		return token->Username;
	soap_wsse_fault(soap, wsse__FailedAuthentication, "Username authentication required");
	return NULL;
}

int
soap_verify_password(struct soap *soap, const char *password)
{
	_wsse__UsernameToken *token = soap_wsse_UsernameToken(soap, NULL);

	/* if we have a UsernameToken with a Password, check it */
	if (token && token->Password)
	{ /* password digest or text? */
		if (token->Password->Type
			&& !strcmp(token->Password->Type, wsse_PasswordDigestURI))
		{ /* check password digest: compute SHA1(created, nonce, password) */
			if (token->Nonce
				&& token->wsu__Created
				&& strlen(token->Password->__item) == 28)	/* digest pw len = 28 */
			{ char HA1[SOAP_SMD_SHA1_SIZE], HA2[SOAP_SMD_SHA1_SIZE];
				/* The specs are not clear: compute digest over binary nonce or base64 nonce? The formet appears to be the case: */
				int noncelen;
				const char *nonce = soap_base642s(soap, token->Nonce, NULL, 0, &noncelen);
				/* compute HA1 = SHA1(created, nonce, password) */
				calc_digest(soap, token->wsu__Created, nonce, noncelen, password, HA1);
				/*
				 calc_digest(soap, token->wsu__Created, token->Nonce, strlen(token->Nonce), password, HA1);
				 */
				/* get HA2 = supplied digest from base64 Password */
				soap_base642s(soap, token->Password->__item, HA2, SOAP_SMD_SHA1_SIZE, NULL);
				/* compare HA1 to HA2 */
				if (!memcmp(HA1, HA2, SOAP_SMD_SHA1_SIZE))
					return SOAP_OK;
			}
		}
		else
		{ /* check password text */
			if (!strcmp(token->Password->__item, password))
				return SOAP_OK;
		}
	}
	return soap_wsse_fault(soap, wsse__FailedAuthentication, NULL);
}

int onvif_access_control(struct soap* soap)
{
#ifdef WITH_OPENSSL
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	gchar *username = soap_get_username(soap);
	gchar *password = NULL;

	if (!username)
		return soap->error; // no username: return FailedAuthentication

	if (ionvif_get_user_password(ionvif, username, &password)) {
		if (soap_verify_password(soap, password))
			return soap->error; // password verification failed: return FailedAuthentic
	}
	else
		return soap_wsse_fault(soap, wsse__FailedAuthentication, NULL);
#endif

	return SOAP_OK;
}
