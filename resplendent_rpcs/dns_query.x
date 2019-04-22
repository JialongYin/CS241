/*Define the query the client wants the DNS query server to resolve
*/
struct query{
	string host<>;
};

/****** Responses from the DNS query ******
/*What is my host's IPv4 address?
*/
struct host_address{
	string host<>; /*the name of the host*/
	string host_ipv4_address<>;/*its corresponding ipv4 address*/
};

/*Response from the dns query service. Some fields can be NULL.
*/
struct response{
	struct host_address *address;
	int success; /* 0 if -1.-1.-1.-1 or -1.-1.-1.-1:-1 was retrieved at any step, then address is undefined. 1 if it was found*/
};

program DNS_QUERY_SERVICE {
	version DNS_QUERY_SERVICE_V1 {
		response DNS_QUERY(query) = 1;
	} = 1;
} = 0x2fffffff;
