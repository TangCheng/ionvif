#include "soapH.h"
#include <assert.h>
#include <netinet/in.h>
#include <json-glib/json-glib.h>
#include "onvif_impl.h"
#include "onvif-auth.h"
#include "request_message.h"
#include "ionvif.h"


/** Auto-test server operation __tds__GetDPAddresses */
int __tds__GetDPAddresses(struct soap *soap, struct _tds__GetDPAddresses *tds__GetDPAddresses, struct _tds__GetDPAddressesResponse *tds__GetDPAddressesResponse)
{	
	DBG_LINE;
	/* Return incomplete response with default data values */
	return SOAP_OK;
}


/** Auto-test server operation __tds__SetDPAddresses */
int __tds__SetDPAddresses(struct soap *soap, struct _tds__SetDPAddresses *tds__SetDPAddresses, struct _tds__SetDPAddressesResponse *tds__SetDPAddressesResponse)
{	
	DBG_LINE;
	return SOAP_OK;
}


int __tds__GetHostname(struct soap *soap, struct _tds__GetHostname *tds__GetHostname, struct _tds__GetHostnameResponse *tds__GetHostnameResponse)
{
	struct _tds__GetHostnameResponse *Response = tds__GetHostnameResponse;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;

    gint dhcp = ipcam_ionvif_get_int_property(ionvif, "network:autoconf");
    const gchar *hostname = ipcam_ionvif_get_string_property(ionvif, "network:hostname");

    SOAP_CALLOC_1(soap, Response->HostnameInformation);
    Response->HostnameInformation->FromDHCP = dhcp;
    SOAP_SET_STRING_FIELD(soap, Response->HostnameInformation->Name, hostname);

	return SOAP_OK;
}


int __tds__SetHostname(struct soap *soap, struct _tds__SetHostname *tds__SetHostname, struct _tds__SetHostnameResponse *tds__SetHostnameResponse)
{	
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonBuilder *builder;
	JsonNode *response = NULL;
	const char *hostname = tds__SetHostname->Name;

	ACCESS_CONTROL;

	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "hostname");
	json_builder_add_string_value(builder, hostname);
	json_builder_end_object(builder);
	json_builder_end_object(builder);

	if (onvif_invocate_action(ionvif, "set_network", json_builder_get_root(builder), &response)) {
		json_node_free(response);
	}

	g_object_unref(builder);

	return SOAP_OK;
}


/** Auto-test server operation __tds__SetHostnameFromDHCP */
int __tds__SetHostnameFromDHCP(struct soap *soap, struct _tds__SetHostnameFromDHCP *tds__SetHostnameFromDHCP, struct _tds__SetHostnameFromDHCPResponse *tds__SetHostnameFromDHCPResponse)
{	
	DBG_LINE;
	/* Return incomplete response with default data values */
	return SOAP_OK;
}


/** Auto-test server operation __tds__GetDNS */
int __tds__GetDNS(struct soap *soap, struct _tds__GetDNS *tds__GetDNS, struct _tds__GetDNSResponse *tds__GetDNSResponse)
{
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	struct _tds__GetDNSResponse *Response = tds__GetDNSResponse;
	int dhcp = 0;
	const char *dns1 = NULL, *dns2 = NULL;
    int nr_dns  = 0;

	ACCESS_CONTROL;

    dhcp = ipcam_ionvif_get_int_property(ionvif, "network:autoconf");
    dns1 = ipcam_ionvif_get_string_property(ionvif, "network:address:dns1");
    dns2 = ipcam_ionvif_get_string_property(ionvif, "network:address:dns2");

    SOAP_CALLOC_1(soap, Response->DNSInformation);
    Response->DNSInformation->FromDHCP = dhcp;

    Response->DNSInformation->__sizeSearchDomain = 1;
    SOAP_CALLOC(soap, Response->DNSInformation->SearchDomain, 1);
    SOAP_SET_STRING_FIELD(soap, Response->DNSInformation->SearchDomain[0], "ipnc");

    nr_dns = (int)!!dns1 + (int)!!dns2;
    Response->DNSInformation->__sizeDNSManual = nr_dns;
    SOAP_CALLOC(soap, Response->DNSInformation->DNSManual, nr_dns);

    int idx = 0;
    if (dns1) {
        Response->DNSInformation->DNSManual[idx].Type = tt__IPType__IPv4;
        SOAP_SET_STRING_FIELD(soap, Response->DNSInformation->DNSManual[idx].IPv4Address, dns1);
        idx++;
    }
    if (dns2) {
        Response->DNSInformation->DNSManual[idx].Type = tt__IPType__IPv4;
        SOAP_SET_STRING_FIELD(soap, Response->DNSInformation->DNSManual[idx].IPv4Address, dns2);
        idx++;
    }

    return SOAP_OK;
}


/** Auto-test server operation __tds__SetDNS */
int __tds__SetDNS(struct soap *soap, struct _tds__SetDNS *tds__SetDNS, struct _tds__SetDNSResponse *tds__SetDNSResponse)
{	
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonBuilder *builder;

	ACCESS_CONTROL;

	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "address");
	json_builder_begin_object(builder);

	json_builder_set_member_name(builder, "dns1");
	if (tds__SetDNS->__sizeDNSManual > 0)
		json_builder_add_string_value(builder, tds__SetDNS->DNSManual[0].IPv4Address);
	else
		json_builder_add_string_value(builder, "");

	json_builder_set_member_name(builder, "dns2");
	if (tds__SetDNS->__sizeDNSManual > 1)
		json_builder_add_string_value(builder, tds__SetDNS->DNSManual[1].IPv4Address);
	else
		json_builder_add_string_value(builder, "");

	json_builder_end_object(builder);
	json_builder_end_object(builder);
	json_builder_end_object(builder);

	onvif_invocate_action(ionvif, "set_network", json_builder_get_root(builder), NULL);

	g_object_unref(builder);

	return SOAP_OK;
}


int __tds__GetNTP(struct soap *soap, struct _tds__GetNTP *tds__GetNTP, struct _tds__GetNTPResponse *tds__GetNTPResponse)
{
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	struct _tds__GetNTPResponse *Response = tds__GetNTPResponse;
	const char *ntp = NULL;

	ACCESS_CONTROL;

    ntp = ipcam_ionvif_get_string_property(ionvif, "datetime:ntp_server");

    SOAP_CALLOC_1(soap, Response->NTPInformation);
    Response->NTPInformation->FromDHCP = xsd__boolean__false_;
    Response->NTPInformation->__sizeNTPManual = 1;
    SOAP_CALLOC(soap, Response->NTPInformation->NTPManual, 1);
    Response->NTPInformation->NTPManual[0].Type = tt__NetworkHostType__IPv4;
    SOAP_SET_STRING_FIELD(soap, Response->NTPInformation->NTPManual[0].IPv4Address, ntp);

	return SOAP_OK;
}


int __tds__SetNTP(struct soap *soap, struct _tds__SetNTP *tds__SetNTP, struct _tds__SetNTPResponse *tds__SetNTPResponse)
{	
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonBuilder *builder;

	ACCESS_CONTROL;

	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "ntp_server");
	json_builder_begin_object(builder);
	if (tds__SetNTP->__sizeNTPManual > 0) {
		json_builder_set_member_name(builder, "int_value");
		json_builder_add_int_value(builder, 0);
		json_builder_set_member_name(builder, "str_value");
		json_builder_add_string_value(builder, tds__SetNTP->NTPManual[0].IPv4Address);
	}
	json_builder_end_object(builder);
	json_builder_end_object(builder);
	json_builder_end_object(builder);

	onvif_invocate_action(ionvif, "set_datetime", json_builder_get_root(builder), NULL);

	g_object_unref(builder);

	return SOAP_OK;
}


/** Auto-test server operation __tds__GetDynamicDNS */
int __tds__GetDynamicDNS(struct soap *soap, struct _tds__GetDynamicDNS *tds__GetDynamicDNS, struct _tds__GetDynamicDNSResponse *tds__GetDynamicDNSResponse)
{	
	DBG_LINE;
	/* Return incomplete response with default data values */
	return SOAP_OK;
}


/** Auto-test server operation __tds__SetDynamicDNS */
int __tds__SetDynamicDNS(struct soap *soap, struct _tds__SetDynamicDNS *tds__SetDynamicDNS, struct _tds__SetDynamicDNSResponse *tds__SetDynamicDNSResponse)
{	
	DBG_LINE;
	return SOAP_OK;
}


static unsigned int Netmask2PrefixLength(const char *netmask)
{
	struct in_addr addr;
	int i = 0;

	if (netmask == NULL)
		return 0;

	if (inet_aton(netmask, &addr)) {
		for (i = 0; i < 32; i++) {
			if (addr.s_addr & (1 << i))
				break;
		}
	}

	return 32 - i;
}


int __tds__GetNetworkInterfaces(struct soap *soap, struct _tds__GetNetworkInterfaces *tds__GetNetworkInterfaces, struct _tds__GetNetworkInterfacesResponse *tds__GetNetworkInterfacesResponse)
{
	struct _tds__GetNetworkInterfacesResponse *Response = tds__GetNetworkInterfacesResponse;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	int dhcp = 0;
    const char *hwaddr = NULL;
	const char *ipaddr = NULL;
	const char *netmask = NULL;

	ACCESS_CONTROL;

    hwaddr = ipcam_ionvif_get_string_property(ionvif, "network:address:hwaddr");
    dhcp = ipcam_ionvif_get_int_property(ionvif, "network:autoconf");
    ipaddr = ipcam_ionvif_get_string_property(ionvif, "network:address:ipaddr");
    netmask = ipcam_ionvif_get_string_property(ionvif, "network:address:netmask");

    Response->__sizeNetworkInterfaces = 1;
    SOAP_CALLOC(soap, Response->NetworkInterfaces, 1);

    struct tt__NetworkInterface *netif = &Response->NetworkInterfaces[0];

    SOAP_SET_STRING_FIELD(soap, netif->token, "eth0");
    netif->Enabled = xsd__boolean__true_;

    if (hwaddr) {
        SOAP_CALLOC_1(soap, netif->Info);
        SOAP_SET_STRING_FIELD(soap, netif->Info->Name, "eth0");
        SOAP_SET_STRING_FIELD(soap, netif->Info->HwAddress, hwaddr);
    }

    SOAP_CALLOC_1(soap, netif->IPv4);
    netif->IPv4->Enabled = xsd__boolean__true_;
    SOAP_CALLOC_1(soap, netif->IPv4->Config);
    if (dhcp) {
        netif->IPv4->Config->DHCP = xsd__boolean__true_;
        SOAP_CALLOC_1(soap, netif->IPv4->Config->FromDHCP);
        SOAP_SET_STRING_FIELD(soap, netif->IPv4->Config->FromDHCP->Address, ipaddr);
        netif->IPv4->Config->FromDHCP->PrefixLength = Netmask2PrefixLength(netmask);
    }
    else {
        netif->IPv4->Config->DHCP = xsd__boolean__false_;
        netif->IPv4->Config->__sizeManual = 1;
        SOAP_CALLOC_1(soap, netif->IPv4->Config->Manual);
        SOAP_SET_STRING_FIELD(soap, netif->IPv4->Config->Manual->Address,
                              ipaddr);
        netif->IPv4->Config->Manual->PrefixLength = Netmask2PrefixLength(netmask);
    }

	return SOAP_OK;
}


static char *PrefixLength2Netmask(unsigned int prefix_len)
{
	struct in_addr addr = { 0 };
	int i;

    prefix_len = prefix_len > 32 ? 32 : prefix_len;

	for (i = 31; i >= (32 - (int)prefix_len); i--)
		addr.s_addr |= (1 << i);

	return inet_ntoa(addr);
}


int __tds__SetNetworkInterfaces(struct soap *soap, struct _tds__SetNetworkInterfaces *tds__SetNetworkInterfaces, struct _tds__SetNetworkInterfacesResponse *tds__SetNetworkInterfacesResponse)
{
	struct _tds__SetNetworkInterfaces *Request = tds__SetNetworkInterfaces;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonBuilder *builder;
	JsonNode *response = NULL;
	int dhcp = Request->NetworkInterface->IPv4->DHCP[0];

	ACCESS_CONTROL;

	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "autoconf");
	json_builder_add_int_value(builder, dhcp);
	if (!dhcp) {
		char *ipaddr;
		char *netmask;
		json_builder_set_member_name(builder, "address");
		json_builder_begin_object(builder);
		json_builder_set_member_name(builder, "ipaddr");
		ipaddr = Request->NetworkInterface->IPv4->Manual->Address;
		json_builder_add_string_value(builder, ipaddr);
		json_builder_set_member_name(builder, "netmask");
		netmask = PrefixLength2Netmask(Request->NetworkInterface->IPv4->Manual->PrefixLength);
		json_builder_add_string_value(builder, netmask);
		json_builder_end_object(builder);
	}
	json_builder_end_object(builder);
	json_builder_end_object(builder);

	if (onvif_invocate_action(ionvif, "set_network", json_builder_get_root(builder), &response)) {
		json_node_free(response);
	}

	g_object_unref(builder);

	return SOAP_OK;
}


int __tds__GetNetworkProtocols(struct soap *soap, struct _tds__GetNetworkProtocols *tds__GetNetworkProtocols, struct _tds__GetNetworkProtocolsResponse *tds__GetNetworkProtocolsResponse)
{
	struct _tds__GetNetworkProtocolsResponse *Response = tds__GetNetworkProtocolsResponse;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
    guint http, rtsp;

	ACCESS_CONTROL;

    http = ipcam_ionvif_get_int_property(ionvif, "network:port:http");
    rtsp = ipcam_ionvif_get_int_property(ionvif, "network:port:rtsp");

    Response->__sizeNetworkProtocols = 2;
    SOAP_CALLOC(soap, Response->NetworkProtocols, 2);
    Response->NetworkProtocols[0].Name = tt__NetworkProtocolType__HTTP;
    Response->NetworkProtocols[0].Enabled = xsd__boolean__true_;
    Response->NetworkProtocols[0].__sizePort = 1;
    SOAP_CALLOC(soap, Response->NetworkProtocols[0].Port, 1);
    Response->NetworkProtocols[0].Port[0] = http;

    Response->NetworkProtocols[1].Name = tt__NetworkProtocolType__RTSP;
    Response->NetworkProtocols[1].Enabled = xsd__boolean__true_;
    Response->NetworkProtocols[1].__sizePort = 1;
    SOAP_CALLOC(soap, Response->NetworkProtocols[1].Port, 1);
    Response->NetworkProtocols[1].Port[0] = rtsp;

	return SOAP_OK;
}


/** Auto-test server operation __tds__SetNetworkProtocols */
int __tds__SetNetworkProtocols(struct soap *soap, struct _tds__SetNetworkProtocols *tds__SetNetworkProtocols, struct _tds__SetNetworkProtocolsResponse *tds__SetNetworkProtocolsResponse)
{
	struct _tds__SetNetworkProtocols *Request = tds__SetNetworkProtocols;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonBuilder *builder;
	JsonNode *response = NULL;
	int i;

	ACCESS_CONTROL;

	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "server_port");
	json_builder_begin_object(builder);
	for (i = 0; i < Request->__sizeNetworkProtocols; i++)
	{
		if (Request->NetworkProtocols[i].Name == tt__NetworkProtocolType__HTTP) {
			json_builder_set_member_name(builder, "http");
			json_builder_add_int_value(builder, Request->NetworkProtocols[i].Port[0]);
		}
		else if (Request->NetworkProtocols[i].Name == tt__NetworkProtocolType__RTSP) {
			json_builder_set_member_name(builder, "rtsp");
			json_builder_add_int_value(builder, Request->NetworkProtocols[i].Port[0]);
		}
	}
	json_builder_end_object(builder);
	json_builder_end_object(builder);
	json_builder_end_object(builder);

	if (onvif_invocate_action(ionvif, "set_network", json_builder_get_root(builder), &response)) {
		json_node_free(response);
	}

	g_object_unref(builder);

	return SOAP_OK;
}


int __tds__GetNetworkDefaultGateway(struct soap *soap, struct _tds__GetNetworkDefaultGateway *tds__GetNetworkDefaultGateway, struct _tds__GetNetworkDefaultGatewayResponse *tds__GetNetworkDefaultGatewayResponse)
{
	struct _tds__GetNetworkDefaultGatewayResponse *Response = tds__GetNetworkDefaultGatewayResponse;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;

	ACCESS_CONTROL;


    const gchar *gwaddr = ipcam_ionvif_get_string_property(ionvif, "network:address:gateway");

    if (gwaddr) {
        SOAP_CALLOC_1(soap, Response->NetworkGateway);
        Response->NetworkGateway->__sizeIPv4Address = 1;
        SOAP_CALLOC(soap, Response->NetworkGateway->IPv4Address, 1);
        SOAP_SET_STRING_FIELD(soap, Response->NetworkGateway->IPv4Address[0], gwaddr);
    }

	return SOAP_OK;
}


int __tds__SetNetworkDefaultGateway(struct soap *soap, struct _tds__SetNetworkDefaultGateway *tds__SetNetworkDefaultGateway, struct _tds__SetNetworkDefaultGatewayResponse *tds__SetNetworkDefaultGatewayResponse)
{	
	struct _tds__SetNetworkDefaultGateway *Request = tds__SetNetworkDefaultGateway;
	IpcamIOnvif *ionvif = (IpcamIOnvif *)soap->user;
	JsonBuilder *builder;
	JsonNode *response = NULL;

	ACCESS_CONTROL;

	builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "items");
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "address");
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "gateway");
	json_builder_add_string_value(builder, Request->IPv4Address[0]);
	json_builder_end_object(builder);
	json_builder_end_object(builder);
	json_builder_end_object(builder);

	if (onvif_invocate_action(ionvif, "set_network", json_builder_get_root(builder), &response)) {
		json_node_free(response);
	}

	g_object_unref(builder);

	DBG_LINE;
	return SOAP_OK;
}


/** Auto-test server operation __tds__GetZeroConfiguration */
int __tds__GetZeroConfiguration(struct soap *soap, struct _tds__GetZeroConfiguration *tds__GetZeroConfiguration, struct _tds__GetZeroConfigurationResponse *tds__GetZeroConfigurationResponse)
{	
	DBG_LINE;
	/* Return incomplete response with default data values */
	return SOAP_OK;
}


/** Auto-test server operation __tds__SetZeroConfiguration */
int __tds__SetZeroConfiguration(struct soap *soap, struct _tds__SetZeroConfiguration *tds__SetZeroConfiguration, struct _tds__SetZeroConfigurationResponse *tds__SetZeroConfigurationResponse)
{	
	DBG_LINE;
	return SOAP_OK;
}


/** Auto-test server operation __tds__GetIPAddressFilter */
int __tds__GetIPAddressFilter(struct soap *soap, struct _tds__GetIPAddressFilter *tds__GetIPAddressFilter, struct _tds__GetIPAddressFilterResponse *tds__GetIPAddressFilterResponse)
{	
	DBG_LINE;
	/* Return incomplete response with default data values */
	return SOAP_OK;
}


/** Auto-test server operation __tds__SetIPAddressFilter */
int __tds__SetIPAddressFilter(struct soap *soap, struct _tds__SetIPAddressFilter *tds__SetIPAddressFilter, struct _tds__SetIPAddressFilterResponse *tds__SetIPAddressFilterResponse)
{	
	DBG_LINE;
	return SOAP_OK;
}


/** Auto-test server operation __tds__AddIPAddressFilter */
int __tds__AddIPAddressFilter(struct soap *soap, struct _tds__AddIPAddressFilter *tds__AddIPAddressFilter, struct _tds__AddIPAddressFilterResponse *tds__AddIPAddressFilterResponse)
{	
	DBG_LINE;
	return SOAP_OK;
}


/** Auto-test server operation __tds__RemoveIPAddressFilter */
int __tds__RemoveIPAddressFilter(struct soap *soap, struct _tds__RemoveIPAddressFilter *tds__RemoveIPAddressFilter, struct _tds__RemoveIPAddressFilterResponse *tds__RemoveIPAddressFilterResponse)
{	
	DBG_LINE;
	return SOAP_OK;
}
